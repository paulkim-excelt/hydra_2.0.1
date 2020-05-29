/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/******************************************************************************
File Name  : rtp_receiver.c
Description: This file contains the implementation of RTP Listener
             component.
 ******************************************************************************/

#include <component.h>
#include <media_port.h>
#include <msg_queue.h>
#include <circ_queue.h>
#include <system.h>
#include <bcm_utils.h>
#include <cortex.h>
#include <atomic.h>
#include <ee.h>
#include <bcm_time.h>
#include <inet.h>
#include <udp.h>
#include <nif.h>
#include "rtp_config.h"

#include <rtp_rtcp.h>
#include <rtp_receiver.h>
#include <osil/bcm_osil.h>

#include "ulog.h"
#define GetModuleLogLevel() ULOG_LVL_ERROR

/** RTPL internal functions IDs for error reporting */
#define RTPR_RX_PKT_INDICATION_API_ID           0xB0U
#define RTPR_RESET_STATE_HANDLER_API_ID         0xB1U
#define RTPR_INIT_STATE_HANDLER_API_ID          0xB2U
#define RTPR_READY_STATE_HANDLER_API_ID         0xB4U
#define RTPR_RUN_STATE_HANDLER_API_ID           0xB5U
#define RTPR_FLUSH_BUFFERS_API_ID               0xB6U
#define RTPR_ERIKA_TASK_API_ID                  0xB7U
#define RTPR_GETSYSTEMTIME_API_ID               0xB8U
#define RTPR_POST_DATA_TO_PORTQ_API_ID          0xB9U
#define RTPR_SORT_BUF_DATA_PROCESS_API_ID       0xBAU
#define RTPR_SEND_COMMAND_API_ID                0xBBU
#define RTPR_GET_MEDIA_OUTPORT_API_ID           0xBCU

#ifndef RTP_MAX_NUM_OUTPUT_PORTS
/* Number of output ports defined by default */
#define RTP_MAX_NUM_OUTPUT_PORTS (1UL)
#endif

/* Buffer to receive the Rx data from UDP stack - 9KB in size */
#define MAX_ETH_RX_BUF_SIZE      (INET_MAX_REASS_BUF_SIZE)

/* Count of buffers used to receive and copy incoming Rx data */
#define NUM_RX_PKT_BUFFERS       (8UL)
#define NUM_RX_PKT_WAIT_BUFFERS  (4UL)

/* Flag to manage the Rx buffer pool */
#define RTPR_ALLOC_FLAG_COUNT    ((NUM_RX_PKT_BUFFERS + 31UL) >> 5UL)

/** Maximum number of Channels for RTPL component's output port */
#define RTPR_CHANNEL_MAX         (1UL)

#define RTPR_TS_DIFF_THRESHOLD   (500UL * 1000000UL) /* 500ns */

/***********************************************************************
 * Data specific to RTPL component
 **********************************************************************/
/**
 * RTPL Component State Machine.
 * RTPL component shall be in RTPR_STATE_RESET by default.
 * The state machine for RTPL component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * RTPR_STATE_RESET    MSYS_COMP_CMD_INIT       RTPR_STATE_INIT
 * RTPR_STATE_INIT     MSYS_COMP_CMD_CONFIG     RTPR_STATE_READY
 * RTPR_STATE_READY    MSYS_COMP_CMD_CONFIG     RTPR_STATE_READY
 * RTPR_STATE_READY    MSYS_COMP_CMD_START      RTPR_STATE_RUNNING
 * RTPR_STATE_READY    MSYS_COMP_CMD_DEINIT     RTPR_STATE_RESET
 * RTPR_STATE_RUNNING  MSYS_COMP_CMD_STOP       RTPR_STATE_READY
 * RTPR_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     RTPR_STATE_RESET
 */
typedef uint32_t RTPR_StateType;
#define RTPR_STATE_RESET   (0UL)   /**< Component Reset state */
#define RTPR_STATE_INIT    (1UL)   /**< Component Init state */
#define RTPR_STATE_READY   (2UL)   /**< Component Ready state */
#define RTPR_STATE_RUNNING (3UL)   /**< Component Running state */

/* RTPL component event masks */
/** Event mask for the component's command Queue */
#define RTPR_CMDQ_EVENT        SystemEvent0
/** Event mask used for different payloads for Rx event */
/**  notification from INET                             */
#define RTPR_RX0_EVENT          SystemEvent4
#define RTPR_RX1_EVENT          SystemEvent5
#define RTPR_RX2_EVENT          SystemEvent6
#define RTPR_RX3_EVENT          SystemEvent7
/** Event mask for all the output ports for the component */
#define RTPR_PORT_EVENTS     (SystemEvent8 | \
                              SystemEvent9 | \
                              SystemEvent10 | \
                              SystemEvent11)
/** Event mask for all UDP Rx events used by the component */
#define RTPR_UDP_RX_EVENTS     (SystemEvent4 | \
                                SystemEvent5 | \
                                SystemEvent6 | \
                                SystemEvent7)

/** @struct  RTPR_Rcvr_Stats
    @brief Statistics maintained by the RTP listener component.
    Can be logged using the RTP_PrintStatistics() API.
*/
typedef struct _RTPR_Rcvr_Stats {

    /** Number of RTP Rx packets received */
    uint32_t pktRxCount;

    /** Number of Rx packet that were dropped as EthRx CIRCQ was full */
    uint32_t dropCount;

    /* Number of invalid Rx RTP packets */
    uint32_t invalidRxCount;

    /* Port CIRCQ drop stats */
    uint32_t cQdrop[RTP_MAX_NUM_OUTPUT_PORTS];

    /* Port Wait CIRCQ drop stats */
    uint32_t waitCQdrop[RTP_MAX_NUM_OUTPUT_PORTS];

    /* Number of times packet sequence discontinuity was detected */
    uint32_t missCount[RTP_MAX_NUM_OUTPUT_PORTS];

    /** Number of Rx packets which failed header parse */
    uint32_t numRxPktHdrErrors;

    /** Number of times output port buffer was unavailable */
    uint32_t numRxOutBufNA[RTP_MAX_NUM_OUTPUT_PORTS];

    /** Number of RTCP packets received */
    uint32_t numRxRTCPPkts[RTP_MAX_NUM_OUTPUT_PORTS];

    /** Number of slice aggregated and delivered */
    uint32_t unitAggregated[RTP_MAX_NUM_OUTPUT_PORTS];

} RTPR_Rcvr_Stats;

/* Structure to maintain ethernet Rx data information */
typedef struct _RTPR_RxBuf {

    /* Actual size of Rx data */
    uint32_t filledSize;

    /* The size of data already processed and copied to output port */
    uint32_t copiedSize;

    /* The configured stream params */
    RTP_PayloadInfo params;

    /* The Rx data */
    uint8_t rxData[MAX_ETH_RX_BUF_SIZE];

} RTPR_RxBuf;

/* Structure to maintain entries in port wait queue */
typedef struct _RTPR_RxWaitBuf {

    /* The sequence number of RTP payload packet */
    uint16_t pktSeqNum;

    /* The Rx buffer pool index */
    uint16_t bufIdx;

} RTPR_RxWaitBuf;

/* RTPL component internal structure for state management and */
/* data processing                                            */
typedef struct _RTPR_CompType {

    /* The component state */
    RTPR_StateType state;

    /* Number of output ports activated */
    uint32_t numOutPort;

    /* Variable to track the packet sequence numbers */
    uint16_t pktSeqNum[RTP_MAX_NUM_OUTPUT_PORTS];

    /* Stream/buffer configuration for payload extraction */
    RTP_PayloadInfo bufParams[RTP_MAX_NUM_OUTPUT_PORTS];

    /* The actual media size already filled into output buffer */
    uint32_t medBufFilledSize[RTP_MAX_NUM_OUTPUT_PORTS];

    /* Memory to manage the buffers for Rx data */
    uint32_t allocFlags[RTPR_ALLOC_FLAG_COUNT];

    /* Memory to manage the runtime stats for RTPL component */
    RTPR_Rcvr_Stats rcvrStats;

    /* Variable to track the packet sequence number during processing */
    uint16_t procPktSeqNum[RTP_MAX_NUM_OUTPUT_PORTS];

    /* Buffer for Rx data */
    RTPR_RxBuf rxBuf[NUM_RX_PKT_BUFFERS];

} RTPR_CompType;

/* RTPL component Handle */
static RTPR_CompType RTPR_Comp = {
    .state = RTPR_STATE_RESET,
    .pktSeqNum = {0U},
    .medBufFilledSize = {0UL},
    .procPktSeqNum = {0U},
};

/* Circular Queue to queue the incoming Rx data */
CIRCQ_V2_DEFINE(RTPR_FillQ, 5UL, uint8_t, RTPR_FillQBuf,
                     RTPR_FillQIdx, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");

/* Circular Queue to Rx packet for Port-0 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>0)
CIRCQ_V2_DEFINE(RTPR_ProcQ0, 4UL, uint8_t, RTPR_ProcQBuf0,
                     RTPR_ProcQIdx0, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
CIRCQ_V2_DEFINE(RTPR_WaitQ0, 4UL, RTPR_RxWaitBuf, RTPR_WaitQBuf0,
                     RTPR_WaitQIdx0, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
#endif

/* Circular Queue to Rx packet for Port-1 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>1)
CIRCQ_V2_DEFINE(RTPR_ProcQ1, 4UL, uint8_t, RTPR_ProcQBuf1,
                     RTPR_ProcQIdx1, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
CIRCQ_V2_DEFINE(RTPR_WaitQ1, 4UL, RTPR_RxWaitBuf, RTPR_WaitQBuf1,
                     RTPR_WaitQIdx1, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
#endif

/* Circular Queue to Rx packet for Port-2 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>2)
CIRCQ_V2_DEFINE(RTPR_ProcQ2, 4UL, uint8_t, RTPR_ProcQBuf2,
                     RTPR_ProcQIdx2, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
CIRCQ_V2_DEFINE(RTPR_WaitQ2, 4UL, RTPR_RxWaitBuf, RTPR_WaitQBuf2,
                     RTPR_WaitQIdx2, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
#endif

/* Circular Queue to Rx packet for Port-3 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>3)
CIRCQ_V2_DEFINE(RTPR_ProcQ3, 4UL, uint8_t, RTPR_ProcQBuf3,
                     RTPR_ProcQIdx3, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
CIRCQ_V2_DEFINE(RTPR_WaitQ3, 4UL, RTPR_RxWaitBuf, RTPR_WaitQBuf3,
                     RTPR_WaitQIdx3, CIRCQ_OVERWRITE_DISABLE,
                     ".data.rtp");
#endif

extern const CIRCQ_Type *RTPR_ProcQ[];
/* Array of Circular Queues used for component ports */
const CIRCQ_Type *RTPR_ProcQ[RTP_MAX_NUM_OUTPUT_PORTS] = {
#if (RTP_MAX_NUM_OUTPUT_PORTS>0)
    &RTPR_ProcQ0,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>1)
    &RTPR_ProcQ1,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>2)
    &RTPR_ProcQ2,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>3)
    &RTPR_ProcQ3,
#endif
};

extern const CIRCQ_Type *RTPR_WaitQ[];
/* Array of Circular Queues used for component ports */
const CIRCQ_Type *RTPR_WaitQ[RTP_MAX_NUM_OUTPUT_PORTS] = {
#if (RTP_MAX_NUM_OUTPUT_PORTS>0)
    &RTPR_WaitQ0,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>1)
    &RTPR_WaitQ1,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>2)
    &RTPR_WaitQ2,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>3)
    &RTPR_WaitQ3,
#endif
};

extern MSYS_BufferType RTPR_MediaBuffer[RTP_MAX_NUM_OUTPUT_PORTS][MSGQ_SIZE];
/* Array of buffers for the output port Q */
MSYS_BufferType RTPR_MediaBuffer[RTP_MAX_NUM_OUTPUT_PORTS][MSGQ_SIZE];

/***********************************************************************
 * RTPL <==> Framework interaction
 *
 * Interface for component<-->framework interactions are defined.
 * Framework uses a message queue to send commands to a component, where
 * framework shall be the client and component shall be the server. Any
 * component shall support the following commands from the framework:
 *  1) MSYS_COMP_CMD_INIT
 *  2) MSYS_COMP_CMD_CONFIG
 *  3) MSYS_COMP_CMD_START
 *  4) MSYS_COMP_CMD_STOP
 *  5) MSYS_COMP_CMD_DEINIT
 **********************************************************************/

/** Define and initialize MSGQ_HdrQType instance for RTPL component */
MSGQ_DEFINE_HDRQ(RTPR_CmdMsgHdrQ);

extern MSYS_CmdType RTPR_CmdMsg[];
/** Array for commands to RTPL component */
MSYS_CmdType RTPR_CmdMsg[MSGQ_SIZE];

/***********************************************************************
 FUNCTION NAME: RTPR_ReportError

 ARGUMENTS: aApiID -> API ID invoking the logging
            aErr -> Error code to be logged
            aVal0, aVal1, aVal2, aVal3 -> Values

 DESCRIPTION: Function to be used for error logging

 RETURNS: None

***********************************************************************/
static void RTPR_ReportError(uint8_t aApiID, int32_t aErr,
                             uint32_t aVal0, uint32_t aVal1,
                             uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_RTR_ID, 0U, aApiID, aErr, 4UL, values);
}

/***********************************************************************
 FUNCTION NAME: RTPR_GetCmdMsg

 ARGUMENTS:
             i-> The index of messageQ from which MSG needs to be got

 DESCRIPTION: Function to get the MSGQ_MsgHdrType from CommandQList

 RETURNS: The Message queue header

***********************************************************************/
static void * RTPR_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &RTPR_CmdMsg[i];
    }
    return msg;
}

/* Define the command queue for the RTPL component, on which */
/* framework shall post the different commands for the       */
/* RTPL component                                            */
MSGQ_DEFINE (RTPR_CmdQ, RTPR_Task, RTPR_CMDQ_EVENT,
             MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType,
             &RTPR_CmdMsgHdrQ, RTPR_GetCmdMsg);

/* Creation of output port-0 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>0)
static void *RTPR_GetDataMsg0(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &RTPR_MediaBuffer[0][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(RTPR_OutPort0, 0UL, RTPR_DataMsgHdrQ0,
                        RTPR_Task, RTPR_GetDataMsg0,
                        MSYS_COMP_ID_RTPR, SystemEvent8);
#endif

/* Creation of output port-1 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>1)
static void *RTPR_GetDataMsg1(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &RTPR_MediaBuffer[1][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(RTPR_OutPort1, 1UL, RTPR_DataMsgHdrQ1,
                        RTPR_Task, RTPR_GetDataMsg1,
                        MSYS_COMP_ID_RTPR, SystemEvent9);
#endif

/* Creation of output port-2 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>2)
static void *RTPR_GetDataMsg2(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &RTPR_MediaBuffer[2][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(RTPR_OutPort2, 2UL, RTPR_DataMsgHdrQ2,
                        RTPR_Task, RTPR_GetDataMsg2,
                        MSYS_COMP_ID_RTPR, SystemEvent10);
#endif

/* Creation of output port-3 */
#if (RTP_MAX_NUM_OUTPUT_PORTS>3)
static void *RTPR_GetDataMsg3(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &RTPR_MediaBuffer[3][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(RTPR_OutPort3, 3UL, RTPR_DataMsgHdrQ3,
                        RTPR_Task, RTPR_GetDataMsg3,
                        MSYS_COMP_ID_RTPR, SystemEvent11);
#endif

extern const MSYS_OutPortType* RTPR_OutPort[];
/* Pool of output port structure */
const MSYS_OutPortType* RTPR_OutPort[RTP_MAX_NUM_OUTPUT_PORTS] = {
#if (RTP_MAX_NUM_OUTPUT_PORTS>0)
    &RTPR_OutPort0,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>1)
    &RTPR_OutPort1,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>2)
    &RTPR_OutPort2,
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>3)
    &RTPR_OutPort3,
#endif
};

/***********************************************************************
 FUNCTION NAME: RTPR_SendCmd

 ARGUMENTS:
             aCmd   -> The command-response structure with command
             aClientMask  -> The client mask to be used to signal on
                       completion
             aHdr   -> The pointer to message header

 DESCRIPTION: Function to post the commands to the RTP Command Queue

 RETURNS: The Message queue return code

***********************************************************************/
static int32_t RTPR_SendCmd(const MSYS_CmdType *const aCmd,
                            const uint32_t aClientMask,
                            const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd) && (MSYS_COMP_ID_RTPR == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the RTPL component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            int32_t status = 0U;
            status = BCM_ActivateTask(RTPR_Task);
            if (0U != status) {
                RTPR_ReportError(RTPR_SEND_COMMAND_API_ID, retVal,
                                (uint32_t)__LINE__,
                                (uint32_t)status, 0UL, 0UL);
            }
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&RTPR_CmdQ, (const void * const)aCmd,
                              aClientMask, aHdr);
    }
    return retVal;
}

/***********************************************************************
 FUNCTION NAME: RTPR_RecvResp

 ARGUMENTS:
             aHdr -> The pointer to message header
             aCmd -> The command-response structure with response

 DESCRIPTION: Function to receive response for the commands posted

 RETURNS: The Message queue return code

***********************************************************************/
static int32_t RTPR_RecvResp(const MSGQ_MsgHdrType* const aHdr,
                             MSYS_CmdType *const aCmd)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the component's command queue */
        retVal = MSGQ_RecvMsg(&RTPR_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: RTPR_GetMediaOutPort

 ARGUMENTS: aIndex -> Output port index

 DESCRIPTION: Function to get the MSYS_OutPortType where the message
              needs to be posted to

 RETURNS: For valid index       Pointer to Media outport structure
          For invalid index     NULL
***********************************************************************/
static const MSYS_OutPortType* RTPR_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port = NULL;

    if (RTP_MAX_NUM_OUTPUT_PORTS > aIndex) {
        port = RTPR_OutPort[aIndex];
    } else {
        RTPR_ReportError(RTPR_GET_MEDIA_OUTPORT_API_ID,
                         BCM_ERR_INVAL_PARAMS,
                        (uint32_t) __LINE__, aIndex, 0UL, 0UL);
    }

    return port;
}

/***********************************************************************
 FUNCTION NAME: RTPR_GetMediaInPort

 ARGUMENTS: aIndex -> Input port index

 DESCRIPTION: Function to get the MSYS_InPortType

 RETURNS: For valid index       Pointer to Media input structure
          For invalid index     NULL
***********************************************************************/
static const MSYS_InPortType* RTPR_GetMediaInPort(const uint32_t aIndex)
{
    return NULL;
}

/* Component function table for RTPR */
static const MSYS_CompFnTblType RTPR_CompFnTable =
{
    RTPR_SendCmd,
    RTPR_RecvResp,
    RTPR_GetMediaOutPort,
    RTPR_GetMediaInPort,
};

/* Function to return Component function table for component */
const MSYS_CompFnTblType* RTPR_GetCompFnTable(void)
{
    return &RTPR_CompFnTable;
};

/***********************************************************************
 FUNCTION NAME: RTPR_FlushBuffers

 ARGUMENTS: None

 DESCRIPTION: Function to dequeue the out buffers to the next component
              and clear all CIRCQ's

 RETURNS: None
***********************************************************************/
static void RTPR_FlushBuffers (void)
{
    uint8_t portIdx = 0U;
    uint32_t mediaBufIdx = 0UL;
    int32_t retVal = BCM_ERR_OK;
    MSYS_BufferType *mediaBufferPtr = NULL;

    /* For each output port, clear the port MsgQ and CIRCQ */
    for (portIdx = 0U; portIdx < RTP_MAX_NUM_OUTPUT_PORTS; portIdx++) {

        /* Initialize circular Q of each output port to reset */
        /* read and write pointers                            */
        retVal = CIRCQ_Reset(RTPR_ProcQ[portIdx]);
        if (BCM_ERR_OK != retVal) {
            RTPR_ReportError(RTPR_FLUSH_BUFFERS_API_ID, retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        /* Check on all messages in output port MsgQ */
        retVal = MSGQ_GetMsgIdx(&RTPR_OutPort[portIdx]->dataQ,
                                &mediaBufIdx);
        if (BCM_ERR_OK != retVal){
            continue;
        }

        /* Update the media data size as zero and flush them */
        mediaBufferPtr = &RTPR_MediaBuffer[portIdx][mediaBufIdx];
        mediaBufferPtr->buffer[0].size = 0UL;

        retVal = MSGQ_CompleteMsgIdx (&RTPR_OutPort[portIdx]->dataQ,
                                      mediaBufIdx);
        if (BCM_ERR_OK != retVal) {
            RTPR_ReportError(RTPR_FLUSH_BUFFERS_API_ID, retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

    }

    /* Initialize the ethernet Rx CIRCQ to reset RW pointers */
    retVal = CIRCQ_Reset(&RTPR_FillQ);
    if (BCM_ERR_OK != retVal) {
        RTPR_ReportError(RTPR_FLUSH_BUFFERS_API_ID, retVal,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }

    return;
}

/***********************************************************************
 FUNCTION NAME: RTPR_AtomicAlloc

 ARGUMENTS: None

 DESCRIPTION: Function to allocate a free buffer for Rx data

 RETURNS: Index on available free buffer
***********************************************************************/
static uint32_t RTPR_AtomicAlloc(void)
{
    uint32_t exclusive = 0UL, val = 0UL;
    uint32_t idx = 0xFFUL;
    uint32_t i = 0UL;

    for (i = 0UL; i < RTPR_ALLOC_FLAG_COUNT; i++) {
        do {
            exclusive = 0UL;
            val = CORTEX_LdrEx(&RTPR_Comp.allocFlags[i]);
            if (0UL == val) {
                idx = 0xFFUL;
            } else {
                idx = 31UL - (uint32_t) __builtin_clz(val);
                val &= ~(1UL << idx);
                exclusive = CORTEX_StrEx(&RTPR_Comp.allocFlags[i], val);
            }
        } while (1UL == exclusive);
        if (idx != 0xFFUL) {
            idx = ((32UL * i) + idx);
            break;
        }
    }
    return idx;
}

/***********************************************************************
 FUNCTION NAME: RTPR_ResetStateHandler

 ARGUMENTS: None

 DESCRIPTION: Function to handle transition of component from
              RESET to INIT state.

 RETURNS: None
***********************************************************************/
static void RTPR_ResetStateHandler(void)
{
    int32_t retVal;
    /* Setting initial value to detect location */
    /* of error from error log                  */
    uint32_t i = RTP_MAX_NUM_OUTPUT_PORTS+1UL;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&RTPR_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPR_CmdMsg[cmdQIdx];

        /* If a INIT command is received */
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {

            /* Initialize circular Q for receiving Rx data */
            retVal = CIRCQ_Reset(&RTPR_FillQ);
            if (BCM_ERR_OK != retVal) {
                goto end;
            }
            for (i = 0UL; i<RTP_MAX_NUM_OUTPUT_PORTS; i++) {
                /* Initialize circular Q of each output port */
                retVal = CIRCQ_Reset(RTPR_ProcQ[i]);
                if (BCM_ERR_OK != retVal) {
                    goto end;
                }
                RTPR_Comp.medBufFilledSize[i] = 0UL;
            }

            /* Allocation flag handling based on the number of */
            /* NUM_RX_PKT_BUFFERS declared                     */
#if (0UL < (RTPR_ALLOC_FLAG_COUNT - 1UL)) /* MISRA */
            for (i = 0UL; i < (RTPR_ALLOC_FLAG_COUNT - 1UL); i++) {
                (void)ATOMIC_Set(&RTPR_Comp.allocFlags[i], 0xFFFFFFFFUL);
            }
#endif

            if ((NUM_RX_PKT_BUFFERS & 0x1FUL) == 0UL) {
                (void)ATOMIC_Set(&RTPR_Comp.allocFlags[RTPR_ALLOC_FLAG_COUNT - 1UL],
                          0xFFFFFFFFUL);
            } else {
                (void)ATOMIC_Set(&RTPR_Comp.allocFlags[RTPR_ALLOC_FLAG_COUNT - 1UL],
                          (1UL << (NUM_RX_PKT_BUFFERS & 0x1FUL)) - 1UL);
            }

            /* Clear memory region of component handle */
            BCM_MemSet(&RTPR_Comp.pktSeqNum, 0U, sizeof(RTPR_Comp.pktSeqNum));
            BCM_MemSet(&RTPR_Comp.procPktSeqNum, 0U, sizeof(RTPR_Comp.procPktSeqNum));

            /* Update the component state */
            RTPR_Comp.state = RTPR_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        retVal = MSGQ_CompleteMsgIdx(&RTPR_CmdQ, cmdQIdx);

end:
        if (BCM_ERR_OK != retVal) {
            RTPR_ReportError(RTPR_RESET_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, i, cmdQIdx, 0UL);
        }
    }
}

/**
*/
typedef union _RTPR_ConfigDataType {
    const uint8_t* hdr;
    const RTPR_Config *rtprImg;
} RTPR_ConfigDataType;

/***********************************************************************
 FUNCTION NAME: RTPR_InitStateHandler

 ARGUMENTS: None

 DESCRIPTION: Function to handle transition of component from
              INIT to READY state.

 RETURNS: None
***********************************************************************/
static void RTPR_InitStateHandler(void)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint32_t cntVal = 0UL;
    RTPR_ConfigDataType config;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&RTPR_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPR_CmdMsg[cmdQIdx];
        config.hdr = processCmd->cfgData;

        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Check if the number of allocated ports is same     */
            /* as the number of ports for which the configuration */
            /* is received                                        */
            if (RTP_MAX_NUM_OUTPUT_PORTS == config.rtprImg->numOutPort){
                RTPR_Comp.numOutPort = config.rtprImg->numOutPort;

                /* Store the stream configuration */
                for (cntVal = 0; cntVal < config.rtprImg->numOutPort; cntVal++) {
                    RTPR_Comp.bufParams[cntVal].strParams =
                                        config.rtprImg->outStreamCfg[cntVal];
                }

                RTPR_Comp.state = RTPR_STATE_READY;
                processCmd->response = BCM_ERR_OK;
            } else {
                processCmd->response = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component */
            RTPR_Comp.state = RTPR_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&RTPR_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            RTPR_ReportError(RTPR_INIT_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/***********************************************************************
 FUNCTION NAME: RTPR_ReadyStateHandler

 ARGUMENTS: None

 DESCRIPTION: Function to handle transition of component from
              READY to RUNNING state.

 RETURNS: None
***********************************************************************/
static void RTPR_ReadyStateHandler(void)
{
    int32_t retVal;
    int32_t ret;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint32_t cntVal = 0UL;
    RTPR_ConfigDataType config;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&RTPR_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPR_CmdMsg[cmdQIdx];
        config.hdr = processCmd->cfgData;

        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Check if the number of allocated ports is same     */
            /* as the number of ports for which the configuration */
            /* is received                                        */
            if (RTP_MAX_NUM_OUTPUT_PORTS == config.rtprImg->numOutPort){
                RTPR_Comp.numOutPort = config.rtprImg->numOutPort;

                /* Store the stream configuration */
                for (cntVal = 0; cntVal < config.rtprImg->numOutPort; cntVal++) {
                    RTPR_Comp.bufParams[cntVal].strParams =
                                        config.rtprImg->outStreamCfg[cntVal];
                }
                processCmd->response = BCM_ERR_OK;
            } else {
                processCmd->response = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case MSYS_COMP_CMD_START:
            RTPR_Comp.state = RTPR_STATE_RUNNING;
            /* Signal the task this component to start processing */
            ret = BCM_SetEvent(RTPR_Task, RTPR_PORT_EVENTS);
            if (BCM_ERR_OK != ret) {
                RTPR_ReportError(RTPR_READY_STATE_HANDLER_API_ID,
                                ret,
                                (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            }
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            RTPR_Comp.state = RTPR_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&RTPR_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            RTPR_ReportError(RTPR_READY_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
static void RTPR_GetSystemTimeForPayload (uint32_t netPktTime,
                                          uint64_t *sysOutTime);

/***********************************************************************
 FUNCTION NAME: RTP_GetSystemTimeForPayload

 ARGUMENTS: netTime -> Network packet timestamp
            sysTime -> System time value for display

 DESCRIPTION: This function converts the network time to system time to
              be used for display.

 RETURNS: None

***********************************************************************/
static void RTPR_GetSystemTimeForPayload (uint32_t netPktTime,
                                          uint64_t *sysOutTime)
{
    int32_t retVal = BCM_ERR_OK;
    uint64_t netTimeNs = 0ULL;
    uint32_t netTimeUL = 0UL;

    uint32_t timeDiff = 0UL;
    BCM_TimeType sysTime = {0UL, 0UL};
    uint64_t sysTimeNs = 0ULL;

    ETHER_TimestampType tsVal = {0ULL, 0UL};
    ETHER_TimestampQualType tsQuality = 0UL;

    uint64_t ctrlIdx = NIF_CNTRLIDTYPE_INVALID;

    retVal = NIF_GetCtrlIdx(0U, BCM_NET_ID, 0U, &ctrlIdx);
    if (BCM_ERR_OK != retVal) {
        RTPR_ReportError(RTPR_GETSYSTEMTIME_API_ID, retVal,
                       (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        goto error;
    }

    /* Get the network time to be used for timestamp validation */
    retVal = NIF_GetTime(ctrlIdx, &tsVal, &tsQuality);
    if (BCM_ERR_OK != retVal) {
        RTPR_ReportError(RTPR_GETSYSTEMTIME_API_ID, retVal,
                       (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        goto error;
    }

    /* Get the system time */
    BCM_GetTime(&sysTime);

    /* Get the Current network time in nanoseconds */
    netTimeNs = (uint64_t)((tsVal.seconds * (uint64_t)BCM_NS_PER_SEC) +
                           (uint64_t)tsVal.nanoseconds);

    /* Get the 32 bit value of current network time */
    netTimeUL = (uint32_t)(netTimeNs & 0x00000000FFFFFFFFUL);

    /* Check the timestamp difference */
    if (netTimeUL < netPktTime) {
        timeDiff = netPktTime - netTimeUL;
    }
    else if (netTimeUL > netPktTime) {
        timeDiff = netPktTime + (uint32_t)
                                (0x100000000ULL - (uint64_t)netTimeUL);
    }
    else {
        timeDiff = 0UL;
    }

    /* Get the current system time in ns */
    sysTimeNs = (uint64_t)(((uint64_t)sysTime.s * (uint64_t)BCM_NS_PER_SEC)
                             + (uint64_t)sysTime.ns);

    /* Checking timestamp threshold - COMP1722_TS_DIFF_THRESHOLD */
    if (timeDiff < RTPR_TS_DIFF_THRESHOLD) {
        /* Case of a valid timestamp */
        *sysOutTime = sysTimeNs + (uint64_t)timeDiff;
    }
    else {
        /* Case of a past timestamp */
        *sysOutTime = sysTimeNs - (0x100000000ULL - (uint64_t)timeDiff);
    }

error:
    return;
}
#endif

/***********************************************************************
 FUNCTION NAME: RTPR_SortbufAndProcess

 ARGUMENTS: portIdx[in]     -> Port index for processing
            isQFull[in]     -> Boolean to indicate if WaitQ is FULL

 DESCRIPTION: Function to sort the entries in waitQ and process them.
              The expected packets are moved into process Q.

 RETURNS: Void
***********************************************************************/
static void RTPR_SortbufAndProcess(uint32_t portIdx, uint32_t isQFull)
{
    uint32_t cqLen = 0UL;
    uint32_t numEntry = 0UL;
    uint16_t expSeqNum = 0U;
    int32_t retVal = BCM_ERR_OK;
    uint32_t lineVal = 0UL;
    RTPR_RxWaitBuf bufArr[NUM_RX_PKT_WAIT_BUFFERS];
    RTPR_RxWaitBuf key;
    int32_t i = 0L, j = 0L;

    BCM_MemSet(bufArr, 0U, sizeof(bufArr));

    /* Pop all entries from WaitQ at once */
    cqLen = CIRCQ_Pop(RTPR_WaitQ[portIdx],
                     (char*)&bufArr,
                      NUM_RX_PKT_WAIT_BUFFERS);
    if (0UL == cqLen){
        goto error;
    }


    /* Sort all CIRCQ entries using insertion sort */
     for (i = 1L; (uint32_t)i < cqLen; i++)
     {
         key = bufArr[i];
         j = i-1L;

         while (j >= 0L && bufArr[j].pktSeqNum > key.pktSeqNum)
         {
             bufArr[j+1L] = bufArr[j];
             j = j - 1L;
         }
         bufArr[j+1L] = key;
     }

    /* Check and find out the next expected packet sequence number */
    if (1UL == isQFull){
        /* Pick the lowest packet number from the sorted list */
        expSeqNum = bufArr[0U].pktSeqNum;
    } else {
        expSeqNum = RTPR_Comp.pktSeqNum[portIdx];
    }

    /* Check and process the entries in wait Q. If they are continous */
    /* then move them to processQ. In case of discontinuity, push     */
    /* those entries back into the WaitQ till the expected            */
    /* packet arrives                                                 */
    for (numEntry = 0UL; numEntry < cqLen; numEntry++){
        if (expSeqNum == bufArr[numEntry].pktSeqNum){
            /* Send packet to process Q */
            retVal = CIRCQ_Push(RTPR_ProcQ[portIdx],
                               (char*)&bufArr[numEntry].bufIdx,
                               1UL);
            if (BCM_ERR_OK != retVal){
                RTPR_Comp.rcvrStats.cQdrop[portIdx]++;
                lineVal = __LINE__;
                goto error;
            } else {
                expSeqNum++;
            }
        } else {
            /* Sequence number discontinuity, push remaining entries */
            /* back into waitQ                                       */
            retVal = CIRCQ_Push(RTPR_WaitQ[portIdx],
                               (char*)&bufArr[numEntry],
                               cqLen-numEntry);
            if (BCM_ERR_OK != retVal){
                RTPR_Comp.rcvrStats.waitCQdrop[portIdx]++;
                lineVal = __LINE__;
                goto error;
            }
            break;
        }
    }

    /* Update the next expected sequence number after processing */
    RTPR_Comp.pktSeqNum[portIdx] = expSeqNum;

error:
    if (BCM_ERR_OK != retVal){
        RTPR_ReportError(RTPR_SORT_BUF_DATA_PROCESS_API_ID,
                         retVal,lineVal,portIdx,isQFull,0UL);
    }
    return;
}

/***********************************************************************
 FUNCTION NAME: RTPR_PostDataToPortQ

 ARGUMENTS: aBuf[in]        -> The pointer to the rx buffer with parse
                               information filled by library
            bufmemIndex[in] -> Pointer to memory buffer pool

 DESCRIPTION: Function to detect if any packet sequence number and
              check if they are out of order. The packets which are
              in-order or which are ready for processing are placed in
              the process Q. Packets which are out of order are placed
              in the wait Q. If the wait Q gets full, it indicates that
              there was a packet loss. Appropriate handling for all
              such packet arrival scenarios is done.

 RETURNS: Void
***********************************************************************/
static void RTPR_PostDataToPortQ(RTPR_RxBuf *const aBuf,
                                 uint8_t bufmemIndex)
{
    uint32_t freeBuf = 1UL;
    uint32_t processWaitQ = 0UL;
    uint32_t portIdx;
    int32_t retVal = BCM_ERR_OK;
    uint32_t localCnt = 0UL;
    RTPR_RxWaitBuf bufVal;
    uint32_t lineVal = 0UL;

    for (portIdx = 0UL; portIdx < RTP_MAX_NUM_OUTPUT_PORTS; portIdx++){

        /* Get the output port for this Rx packet */
        if (aBuf->params.info.SSRCid ==
                RTPR_Comp.bufParams[portIdx].strParams.strInfo.SSRCValue){

            /* If a out of sequence packet is detected, add to wait Q */
            if (RTPR_Comp.pktSeqNum[portIdx] !=
                    (uint16_t)aBuf->params.info.mediaPkt.packetSeqNum){

                bufVal.bufIdx = bufmemIndex;
                bufVal.pktSeqNum = (uint16_t)aBuf->params.info.mediaPkt.packetSeqNum;

                /* For a packet that is delivered out of order or  */
                /* a jump in sequence number is kept in the wait Q */
                /* until the expected packet is received           */
                retVal = CIRCQ_Push(RTPR_WaitQ[portIdx],
                                   (char*)&bufVal,
                                   1UL);
                if (BCM_ERR_OK != retVal){
                    RTPR_Comp.rcvrStats.waitCQdrop[portIdx]++;
                    lineVal = __LINE__;
                } else {
                    freeBuf = 0UL;
                }
            } else {
                /* For an expected packet, push the data to the */
                /* port process CIRCQ                           */
                retVal = CIRCQ_Push(RTPR_ProcQ[portIdx],
                                   (char*)&bufmemIndex,
                                   1UL);
                if (BCM_ERR_OK == retVal) {
                    RTPR_Comp.pktSeqNum[portIdx]++;
                    freeBuf = 0UL;
                } else {
                    RTPR_Comp.rcvrStats.cQdrop[portIdx]++;
                    lineVal = __LINE__;
                }
                /* If there are packets in wait Q and the expected */
                /* packet was recieved, then set flag              */
                processWaitQ = 1UL;
            }
            break;
        }
    }

    /* Check and free the buffer */
    if (freeBuf == 1UL) {
        ATOMIC_SetBit(&RTPR_Comp.allocFlags[bufmemIndex>>5UL],
                     bufmemIndex & 0x1FUL);
        goto error;
    }

    /* Check for number of items in waitQ */
    retVal = CIRCQ_GetFilledSize(RTPR_WaitQ[portIdx], &localCnt);
    if ((BCM_ERR_OK != retVal) || (0UL == localCnt)){
        lineVal = __LINE__;
        goto error;
    }

    /* Case when the wait Q is full. This can happen when a single */
    /* packet is lost or if a lot of packets are reordered. In our */
    /* setup, it will be mostly due to lost packets as not more    */
    /* than 3 packet depth get reordered.                          */
    if (NUM_RX_PKT_WAIT_BUFFERS == localCnt){

        RTPR_SortbufAndProcess(portIdx, 1UL);

    /* Case when the expected packet was received so processing the */
    /* reordered packets which were delivered early                 */
    } else if ((0UL < localCnt) && (1UL == processWaitQ)){

        RTPR_SortbufAndProcess(portIdx, 0UL);

    } else {
        /* Nothing */
        lineVal = __LINE__;
        goto error;
    }

error:
    if (BCM_ERR_OK != retVal){
        RTPR_ReportError(RTPR_POST_DATA_TO_PORTQ_API_ID,
                         retVal,lineVal,0UL,0UL,0UL);
    }
    return;

}

/***********************************************************************
 FUNCTION NAME: RTPR_RunStateHandler

 ARGUMENTS: mask -> Event mask which specifies the event received

 DESCRIPTION: Function to handle data in RUNNING state.

 RETURNS: None
***********************************************************************/

static void RTPR_RunStateHandler(EventMaskType mask)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    MSYS_BufferType * mediaBufferPtr;
    uint8_t idxArray[NUM_RX_PKT_BUFFERS] = {0};
    uint32_t localCnt;
    uint32_t indexVal = 0;
    uint32_t i,j;

    /* Check if there are any commands in the command queue. */
    /* If there are any, process the command first.          */
    retVal = MSGQ_GetMsgIdx(&RTPR_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPR_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            RTPR_Comp.state = RTPR_STATE_READY;
            RTPR_FlushBuffers();
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            RTPR_Comp.state = RTPR_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }

        /* Mark the processed command as complete */
        retVal = MSGQ_CompleteMsgIdx(&RTPR_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            RTPR_ReportError(RTPR_RUN_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }

    /* Process incoming Rx data in if running state */
    if (RTPR_STATE_RUNNING == RTPR_Comp.state) {
        RTPR_RxBuf *rxBuf;
        INET_IPAddress ipAddr;
        INET_Port ipPort;
        int32_t  udpRet       = BCM_ERR_OK;
        uint16_t dataSize     = 0U;
        uint8_t  moreData     = 0U;
        uint32_t payloadID    = 0UL;
        uint32_t portIdx      = 0UL;

        for (portIdx = 0UL; portIdx < RTPR_Comp.numOutPort; portIdx++) {
            uint8_t portProcess   = 0U;

            /* Identify which all ports have data to be read */
            if ((0UL == portIdx) && (0UL != (mask & RTPR_RX0_EVENT))){
                portProcess = 1U;
            } else if ((1UL == portIdx) && (0UL != (mask & RTPR_RX1_EVENT))){
                portProcess = 1U;
            } else if ((2UL == portIdx) && (0UL != (mask & RTPR_RX2_EVENT))){
                portProcess = 1U;
            } else if ((3UL == portIdx) && (0UL != (mask & RTPR_RX3_EVENT))){
                portProcess = 1U;
            } else {
                /* Nothing */
                portProcess = 0U; /* MISRA */
            }

            /* If a particular port has data to be read */
            if ((1U == portProcess) && (portIdx < RTP_MAX_NUM_OUTPUT_PORTS)){
                /* First process RTP stream data on the port */
                payloadID = RTPR_Comp.bufParams[portIdx].strParams.strInfo.rtpPayloadID;

                /* Read until UDP stack has provided all buffers */
                do {

                    /* Allocate a local buffer */
                    indexVal = RTPR_AtomicAlloc();

                    /* If a valid free buffer is available */
                    if (indexVal < NUM_RX_PKT_BUFFERS) {
                        /* Reset the args to UDP Recv API */
                        dataSize = (uint16_t)MAX_ETH_RX_BUF_SIZE;
                        moreData = 0U;

                        /* Get a Rx buffer from UDP stack */
                        udpRet = UDP_Recv(BCM_RTR_ID, payloadID,
                                          RTPR_Comp.rxBuf[indexVal].rxData,
                                          &dataSize,
                                          &ipAddr, &ipPort, &moreData);
                        if ((BCM_ERR_OK == udpRet) &&
                            (0U < dataSize)) {

                            RTPR_Comp.rcvrStats.pktRxCount++;
                            RTPR_Comp.rxBuf[indexVal].filledSize =
                                                        (uint32_t)dataSize;
                            RTPR_Comp.rxBuf[indexVal].copiedSize = 0UL;
                            BCM_MemSet(&RTPR_Comp.rxBuf[indexVal].params, 0U,
                                       sizeof(RTPR_Comp.rxBuf[indexVal].params));

                            /* Copy Rx data details to CIRCQ */
                            retVal = CIRCQ_Push(&RTPR_FillQ, (char*)&indexVal, 1UL);
                            if (BCM_ERR_OK != retVal) {
                                /* Increment stats in case data push failed */
                                RTPR_Comp.rcvrStats.dropCount++;
                                ATOMIC_SetBit(&RTPR_Comp.allocFlags[indexVal>>5UL],
                                             indexVal & 0x1FUL);
                            }
                        } else {
                            ATOMIC_SetBit(&RTPR_Comp.allocFlags[indexVal>>5UL],
                                         indexVal & 0x1FUL);
                        }
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
                        /* Check for stream RTCP data for the port on other payload ID */
                        if ((RTPR_Comp.bufParams[portIdx].strParams.strInfo.rtpPayloadID == payloadID) &&
                            (0UL == moreData)) {
                            payloadID = RTPR_Comp.bufParams[portIdx].strParams.rtcpPayloadID;
                            moreData = 1U;
                        }
#endif
                    } else {
                        /* Increment stats in case no Rx buffer was available */
                        RTPR_Comp.rcvrStats.dropCount++;
                        break;
                    }
                } while (1U == moreData);
            }
        }

        /* Pick Rx data got from ethernet */
        localCnt = CIRCQ_Pop(&RTPR_FillQ,
                           (char*)idxArray,
                            NUM_RX_PKT_BUFFERS);

        /* For the number of Rx buffers in the CIRCQ */
        for (i = 0UL; i < localCnt; i++) {

            rxBuf = &RTPR_Comp.rxBuf[idxArray[i]];
            /* Use RTP library and get the payload details */
            retVal = RTP_PacketParse(rxBuf->rxData,
                                     rxBuf->filledSize,
                                     &rxBuf->params);

            /* In case of media payload within RTP packets */
            if ((BCM_ERR_OK == retVal) &&
                (PACKET_RTP_MEDIA_PAYLOAD ==
                                rxBuf->params.info.packetType)) {

            /* Update the current size of Rx packet processed */
            rxBuf->filledSize =
                    rxBuf->params.info.mediaPkt.payloadOffset +
                    rxBuf->params.info.mediaPkt.payloadLen;
            rxBuf->copiedSize =
                    rxBuf->params.info.mediaPkt.payloadOffset;

            /* Invoke the function to handle and post the buffer */
            /* into either wait Q or process Q based on the      */
            /* sequence number (support for reordering)          */
            RTPR_PostDataToPortQ(rxBuf,idxArray[i]);

#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
            }else if ((BCM_ERR_OK == retVal) &&
                (PACKET_RTCP_1733_SR_PAYLOAD ==
                                rxBuf->params.info.packetType)) {
                uint64_t sysOutTime = 0ULL;
                uint32_t mediaBufIdx;

                /* Get local clock time for this network time PTS */
                /* received in the RTCP packet                    */
                RTPR_GetSystemTimeForPayload(
                            rxBuf->params.info.sr1733Pkt.networkRefTime,
                            &sysOutTime);

                /* Get the output port for this RTCP packet */
                for (j=0; j < RTP_MAX_NUM_OUTPUT_PORTS; j++ ) {
                    if (rxBuf->params.info.SSRCid ==
                            RTPR_Comp.bufParams[j].strParams.strInfo.SSRCValue) {

                        /* Track the number of RTCP packet received */
                        RTPR_Comp.rcvrStats.numRxRTCPPkts[j]++;

                        retVal = MSGQ_GetMsgIdx(&RTPR_OutPort[j]->dataQ, &mediaBufIdx);
                        if (retVal != BCM_ERR_OK) {
                            break;
                        }

                        /* RTCP packet is sent just before the last RTP packet for a */
                        /* slice that is being aggregated. Considering that, in the  */
                        /* RTPL, slice under aggregation will contain the same       */
                        /* timestamp as present in the 1722 SR packet                */
                        mediaBufferPtr = &RTPR_MediaBuffer[j][mediaBufIdx];

                        /* Check if the RTP media payload packet under process */
                        /* contains the same RTP timestamp                     */
                        if ((uint32_t)mediaBufferPtr->timestamp ==
                            rxBuf->params.info.sr1733Pkt.rtpRefTime){
                            mediaBufferPtr->timestamp = sysOutTime;
                        }
                    }
                }
                ATOMIC_SetBit(&RTPR_Comp.allocFlags[idxArray[i]>>5UL],
                             idxArray[i] & 0x1FUL);
#endif
            } else {
                /* Increment the error stats for RTP library errors */
                RTPR_Comp.rcvrStats.numRxPktHdrErrors++;
                ATOMIC_SetBit(&RTPR_Comp.allocFlags[idxArray[i]>>5UL],
                             idxArray[i] & 0x1FUL);
            }
        }

        /* For all the output ports, check the port CIRCQ for data */
        /* and process them                                        */
        for (j=0; j<RTP_MAX_NUM_OUTPUT_PORTS; j++ ) {
            indexVal = 0UL;

            localCnt = CIRCQ_Peek(RTPR_ProcQ[j],
                                 (char*)&idxArray,
                                  NUM_RX_PKT_BUFFERS);

            /* For all the data packets in the output port CIRCQ */
            while (localCnt > indexVal) {
                uint32_t mediaBufIdx;
                uint32_t copySize;
                uint32_t outSize;
                uint32_t inSize;

                /* Get the actual Rx data packet */
                rxBuf = &RTPR_Comp.rxBuf[idxArray[indexVal]];
                inSize = rxBuf->filledSize - rxBuf->copiedSize;
                retVal = MSGQ_GetMsgIdx(&RTPR_OutPort[j]->dataQ, &mediaBufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Track the number of times output buffer was NA */
                    RTPR_Comp.rcvrStats.numRxOutBufNA[j]++;
                    break;
                }

                /* Process the data onto the output port buffer */
                mediaBufferPtr = &RTPR_MediaBuffer[j][mediaBufIdx];
                outSize = mediaBufferPtr->buffer[0].size - RTPR_Comp.medBufFilledSize[j];
                copySize =  BCM_MIN(inSize, outSize);

                /* validate mediabuffer */
                if ((NULL != mediaBufferPtr->buffer[0].data)
                    && (mediaBufferPtr->buffer[0].size >=
                          (RTPR_Comp.medBufFilledSize[j] + copySize))){

                    /* For H264 data, add the NAL start code prefix */
                    if (RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE ==
                            RTPR_Comp.bufParams[j].strParams.strInfo.payloadFmt) {

                        /* On a new output buffer and when a slice */
                        /* start is detected, add NAL start prefix */
                        if ((0UL == RTPR_Comp.medBufFilledSize[j]) &&
                            (1UL == rxBuf->params.info.mediaPkt.isSliceAggnStart)){

                            uint8_t nalCode[5] = {0x0U,0x0U,0x0U,0x1U,0x65U};
                            uint32_t nalCodeSize = 0UL;

                            /* For SPS/PPS, add the NAL prefix */
                            if ((7UL == rxBuf->params.info.mediaPkt.NALType) ||
                                (8UL == rxBuf->params.info.mediaPkt.NALType)){
                                nalCodeSize = 4UL;
                            } else {
                                /* For IDR, add the NAL prefix + IDC */
                                nalCodeSize = 5UL;
                            }
                            /* Fill the output port buffer */
                            BCM_MemCpy(&mediaBufferPtr->buffer[0].data[RTPR_Comp.medBufFilledSize[j]],
                                       &nalCode, nalCodeSize);
                            RTPR_Comp.medBufFilledSize[j] += nalCodeSize;
                            /* Store the RTP timestamp for usage by RTCP to map it */
                            /* into local system clock domain                      */
                            mediaBufferPtr->timestamp = rxBuf->params.info.mediaPkt.rtpTS;
                        }
                    }

                    /* Fill the output port buffer */
                    BCM_MemCpy(&mediaBufferPtr->buffer[0].data[RTPR_Comp.medBufFilledSize[j]],
                            &rxBuf->rxData[rxBuf->copiedSize], copySize);
                    rxBuf->copiedSize += copySize;
                    RTPR_Comp.medBufFilledSize[j] += copySize;

                    /* In case this Rx packet marks end of aggregation */
                    if (1UL == rxBuf->params.info.mediaPkt.isSliceAggnEnd) {
                        mediaBufferPtr->buffer[0].size =
                                        RTPR_Comp.medBufFilledSize[j];
                    }

                    /* In case the buffer is completely processed */
                    if (rxBuf->copiedSize == rxBuf->filledSize) {
                        uint32_t ignore = 0UL;
                        uint32_t val;

                        /* Detect packet losses */
                        if (RTPR_Comp.procPktSeqNum[j] !=
                                (uint16_t)rxBuf->params.info.mediaPkt.packetSeqNum) {
                            ULOG_ERR("RTPR [%u] > PKT DROP: Expected %u Actual %u \n",j,
                                            RTPR_Comp.procPktSeqNum[j],
                                            rxBuf->params.info.mediaPkt.packetSeqNum);
                            RTPR_Comp.rcvrStats.missCount[j]++;
                            RTPR_Comp.procPktSeqNum[j] =
                                    (uint16_t)rxBuf->params.info.mediaPkt.packetSeqNum + 1U;
                        } else {
                            RTPR_Comp.procPktSeqNum[j]++;
                        }

                        mediaBufferPtr->numChannels = 1UL;
                        val = CIRCQ_Pop(RTPR_ProcQ[j],
                                       (char*)&ignore, 1UL);
                        if (1UL == val) {
                            ATOMIC_SetBit(&RTPR_Comp.allocFlags[ignore>>5UL],
                                        (ignore&0x1FUL));
                            indexVal++;
                        } else {
                            RTPR_ReportError(RTPR_RUN_STATE_HANDLER_API_ID,
                                             BCM_ERR_INVAL_PARAMS,
                                             (uint32_t)__LINE__,
                                             val, 0UL, 0UL);
                        }
                    }
                    mediaBufferPtr->response = BCM_ERR_OK;
                } else {
                    mediaBufferPtr->response = BCM_ERR_INVAL_PARAMS;
                }
                if ((RTPR_Comp.medBufFilledSize[j] == mediaBufferPtr->buffer[0].size)
                    || (mediaBufferPtr->response == BCM_ERR_INVAL_PARAMS)) {
                    RTPR_Comp.medBufFilledSize[j] = 0UL;

                    /* Track the number of slices delivered */
                    RTPR_Comp.rcvrStats.unitAggregated[j]++;

                    retVal = MSGQ_CompleteMsgIdx(&RTPR_OutPort[j]->dataQ, mediaBufIdx);
                    if (BCM_ERR_OK != retVal) {
                        RTPR_ReportError(RTPR_RUN_STATE_HANDLER_API_ID,
                                        (int32_t)retVal,
                                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                    }
                }
            }
        }
    }
    return;
}

/* Task function for RTPL component task */
TASK(RTPR_Task)
{
    BCM_EventMaskType eventMask = 0UL;
    int32_t status = 0U;

    do {
        /* Wait for all component events */
        status = BCM_WaitEvent(RTPR_CMDQ_EVENT     |
                           RTPR_UDP_RX_EVENTS  |
                           RTPR_PORT_EVENTS);
        if (BCM_ERR_OK != status) {
            RTPR_ReportError(RTPR_ERIKA_TASK_API_ID, status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
        status = BCM_GetEvent(RTPR_Task, &eventMask);
        if (BCM_ERR_OK != status) {
            RTPR_ReportError(RTPR_ERIKA_TASK_API_ID, status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
        status = BCM_ClearEvent(eventMask);
        if (BCM_ERR_OK != status) {
            RTPR_ReportError(RTPR_ERIKA_TASK_API_ID, status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        /* Call the state handling helper function depending on */
        /* the state that this RTPL component is in             */
        switch(RTPR_Comp.state) {
        case RTPR_STATE_RESET:
            RTPR_ResetStateHandler();
            break;
        case RTPR_STATE_INIT:
            RTPR_InitStateHandler();
            break;
        case RTPR_STATE_READY:
            RTPR_ReadyStateHandler();
            break;
        case RTPR_STATE_RUNNING:
            RTPR_RunStateHandler((EventMaskType)eventMask);
            break;
        default:
            /* Do nothing */
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (RTPR_STATE_RESET != RTPR_Comp.state);

    status = BCM_TerminateTask();
    if (BCM_ERR_OK != status) {
        RTPR_ReportError(RTPR_ERIKA_TASK_API_ID, status,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }
}

