/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
File Name  : avbl_comp.c
Description: This file contains the implementation of AVB Listener
             component.
 ******************************************************************************/

#include <component.h>
#include <media_port.h>
#include <msg_queue.h>
#include <circ_queue.h>
#include <system.h>
#include <atomic.h>
#include <ee.h>
#include <bcm_utils.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>
#include "avb_listener_cfg.h"

#include <avb_core.h>
#include <avb_listener.h>

#ifdef ULOG
#include "ulog.h"
#define GetModuleLogLevel() ULOG_LVL_ERROR
#else
#define ULOG_ERR(...)
#endif

/** AVBL internal functions IDs for error reporting */
#define AVBL_RX_PKT_INDICATION_API_ID           0xB0U
#define AVBL_RESET_STATE_HANDLER_API_ID         0xB1U
#define AVBL_INIT_STATE_HANDLER_API_ID          0xB2U
#define AVBL_READY_STATE_HANDLER_API_ID         0xB4U
#define AVBL_RUN_STATE_HANDLER_API_ID           0xB5U
#define AVBL_FLUSH_BUFFERS_API_ID               0xB6U
#define AVBL_AVBL_TASK_API_ID                   0xB7U
#define AVBL_COMP_SEND_CMD_PROC                 0xB8U
#define AVBL_COMP_GET_MEDOUTPORT_PROC           0xB9U

#define MAX_ETH_RX_BUF_SIZE      (1500UL)

#ifndef AVBL_MAX_NUM_OUTPUT_PORTS
/* Number of output ports defined by default */
#define AVBL_MAX_NUM_OUTPUT_PORTS (1UL)
#endif

/* Count of buffers used to receive and copy incoming Rx data */
#define NUM_RX_PKT_BUFFERS (AVBL_MAX_NUM_OUTPUT_PORTS*8UL)

/* Flag to manage the Rx buffer pool */
#define AVBL_ALLOC_FLAG_COUNT    ((NUM_RX_PKT_BUFFERS + 31UL) >> 5UL)

/** Maximum number of Channels for AVBL component's output port */
#define AVBL_CHANNEL_MAX         (1UL)

/***********************************************************************
 * Data specific to AVBL component
 **********************************************************************/
/**
 * AVBL Component State Machine.
 * AVBL component shall be in AVBL_STATE_RESET by default.
 * The state machine for AVBL component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * AVBL_STATE_RESET    MSYS_COMP_CMD_INIT       AVBL_STATE_INIT
 * AVBL_STATE_INIT     MSYS_COMP_CMD_CONFIG     AVBL_STATE_READY
 * AVBL_STATE_READY    MSYS_COMP_CMD_CONFIG     AVBL_STATE_READY
 * AVBL_STATE_READY    MSYS_COMP_CMD_START      AVBL_STATE_RUNNING
 * AVBL_STATE_READY    MSYS_COMP_CMD_DEINIT     AVBL_STATE_RESET
 * AVBL_STATE_RUNNING  MSYS_COMP_CMD_STOP       AVBL_STATE_READY
 * AVBL_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     AVBL_STATE_RESET
 */
typedef uint32_t AVBL_StateType;
#define AVBL_STATE_RESET   (0UL)   /**< Sink component Reset state */
#define AVBL_STATE_INIT    (1UL)   /**< Sink component Init state */
#define AVBL_STATE_READY   (2UL)   /**< Sink component Ready state */
#define AVBL_STATE_RUNNING (3UL)   /**< Sink component Running state */

/* AVBL component event masks */
/** Event mask for the component's command Queue */
#define AVBL_CMDQ_EVENT        SystemEvent0
/** Event mask for Ethernet Rx event */
#define AVBL_RX_EVENT          SystemEvent1
/** Event mask for all the output ports for the component */
#define AVBL_PORT_EVENTS     (SystemEvent8 | \
                              SystemEvent9 | \
                              SystemEvent10 | \
                              SystemEvent11)

/** @struct  AVB_Lstr_Stats
    @brief Statistics maintained by the AVB listener component.
    Can be logged using the AVB_PrintStatistics() API.
*/
typedef struct _AVB_Lstr_Stats {

    /** Number of 1722 Rx packets received */
    uint32_t pktRxCount;

    /** Number of Rx packet that were dropped as EthRx CIRCQ was full */
    uint32_t dropCount;

    /* Number of invalid 1722 packets */
    uint32_t invalidRxCount;

    /* Port CIRCQ drop stats */
    uint32_t cQdrop[AVBL_MAX_NUM_OUTPUT_PORTS];

    /* Number of times packet sequence discontinuity was detected */
    uint32_t missCount[AVBL_MAX_NUM_OUTPUT_PORTS];

    /** Number of Rx packets which failed 1722 header parse */
    uint32_t numRxPktHdrErrors;

    /** Number of times output port buffer was unavailable */
    uint32_t numRxOutBufNA[AVBL_MAX_NUM_OUTPUT_PORTS];

    /** Number of Rx packets whose stream ID did not match with cfg */
    uint32_t numUnmatchedStreamPkts;
} AVB_Lstr_Stats;

/* Structure to maintain ethernet Rx data information */
typedef struct _AVBL_RxBuf {

    /* Actual size of Rx data */
    uint32_t filledSize;

    /* The size of data already processed and copied to output port */
    uint32_t copiedSize;

    /* The configured stream params */
    AVB_1722LstrBufParamsType params;

    /* The Rx data */
    uint8_t rxData[MAX_ETH_RX_BUF_SIZE];

} AVBL_RxBuf;

/* AVBL component internal structure for state management and */
/* data processing                                            */
typedef struct _AVBL_CompType {

    /* The component state */
    AVBL_StateType state;

    /* Number of Data Buffers processed */
    uint32_t dataBufCnt;

    /* Variable to track the packet sequence numbers */
    uint8_t pktSeqNum[AVBL_MAX_NUM_OUTPUT_PORTS];

    /* Component configuration */
    AVBL_Stream1722CfgType compCfg[AVBL_MAX_NUM_OUTPUT_PORTS];

    /* The actual media size already filled into output buffer */
    uint32_t medBufFilledSize[AVBL_MAX_NUM_OUTPUT_PORTS];

    /* Memory to manage the buffers for Rx data */
    uint32_t allocFlags[AVBL_ALLOC_FLAG_COUNT];

    /* Memory to manage the runtime stats for AVBL component */
    AVB_Lstr_Stats lstrStats;

    /* The pool of buffer for Rx data */
    AVBL_RxBuf rxBuf[NUM_RX_PKT_BUFFERS];

} AVBL_CompType;

/* AVBL component Handle */
static AVBL_CompType AVBL_Comp = {
    .state = AVBL_STATE_RESET,
    .dataBufCnt = 0UL,
    .pktSeqNum = {1U},
    .medBufFilledSize = {0UL},
};

/* Circular Queue to queue the incoming Rx data */
CIRCQ_V2_DEFINE(AVBL_FillQ, 6UL, uint8_t, AVBL_FillQBuf,
                     AVBL_FillQIdx, CIRCQ_OVERWRITE_DISABLE,
                     ".data.avb");

/* Circular Queue to Rx packet for Port-0 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>0)
CIRCQ_V2_DEFINE(AVBL_ProcQ0, 4UL, uint8_t, AVBL_ProcQBuf0,
                     AVBL_ProcQIdx0, CIRCQ_OVERWRITE_DISABLE,
                     ".data.avb");
#endif

/* Circular Queue to Rx packet for Port-1 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>1)
CIRCQ_V2_DEFINE(AVBL_ProcQ1, 4UL, uint8_t, AVBL_ProcQBuf1,
                     AVBL_ProcQIdx1, CIRCQ_OVERWRITE_DISABLE,
                     ".data.avb");
#endif

/* Circular Queue to Rx packet for Port-2 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>2)
CIRCQ_V2_DEFINE(AVBL_ProcQ2, 4UL, uint8_t, AVBL_ProcQBuf2,
                     AVBL_ProcQIdx2, CIRCQ_OVERWRITE_DISABLE,
                     ".data.avb");
#endif

/* Circular Queue to Rx packet for Port-3 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>3)
CIRCQ_V2_DEFINE(AVBL_ProcQ3, 4UL, uint8_t, AVBL_ProcQBuf3,
                        AVBL_ProcQIdx3, CIRCQ_OVERWRITE_DISABLE, ".data.avb");
#endif

extern const CIRCQ_Type *AVBL_ProcQ[];
/* Array of Circular Queues used for component ports */
const CIRCQ_Type *AVBL_ProcQ[AVBL_MAX_NUM_OUTPUT_PORTS] = {
#if (AVBL_MAX_NUM_OUTPUT_PORTS>0)
    &AVBL_ProcQ0,
#endif
#if (AVBL_MAX_NUM_OUTPUT_PORTS>1)
    &AVBL_ProcQ1,
#endif
#if (AVBL_MAX_NUM_OUTPUT_PORTS>2)
    &AVBL_ProcQ2,
#endif
#if (AVBL_MAX_NUM_OUTPUT_PORTS>3)
    &AVBL_ProcQ3,
#endif
};

extern MSYS_BufferType AVBL_MediaBuffer[AVBL_MAX_NUM_OUTPUT_PORTS][MSGQ_SIZE];
/* Array of buffers for the output port Q */
MSYS_BufferType AVBL_MediaBuffer[AVBL_MAX_NUM_OUTPUT_PORTS][MSGQ_SIZE];

/***********************************************************************
 * AVBL <==> Framework interaction
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

/** Define and initialize MSGQ_HdrQType instance for AVBL component */
MSGQ_DEFINE_HDRQ(AVBL_CmdMsgHdrQ);

extern MSYS_CmdType AVBL_CmdMsg[];
/** Array for commands to AVBL component */
MSYS_CmdType AVBL_CmdMsg[MSGQ_SIZE];

/***********************************************************************
 FUNCTION NAME: AVBL_ReportError

 ARGUMENTS: aApiID -> API ID invoking the logging
            aErr -> Error code to be logged
            aVal0, aVal1, aVal2, aVal3 -> Values

 DESCRIPTION: API to be used for error logging

 RETURNS: None

***********************************************************************/
static void AVBL_ReportError(uint8_t aApiID, int32_t aErr,
                             uint32_t aVal0, uint32_t aVal1,
                             uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_AVB_ID, 0U, aApiID, aErr, 4UL, values);
}

/***********************************************************************
 FUNCTION NAME: AVBL_GetCmdMsg

 ARGUMENTS:
             i-> The index of messageQ from which MSG needs to be got

 DESCRIPTION: API to get the MSGQ_MsgHdrType from CommandQList

 RETURNS: The Message queue header

***********************************************************************/
static void * AVBL_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AVBL_CmdMsg[i];
    }
    return msg;
}

/* Define the command queue for the AVBL component, on which */
/* framework shall post the different commands for the       */
/* AVBL component                                            */
MSGQ_DEFINE (AVBL_CmdQ, AVBL_Task, AVBL_CMDQ_EVENT,
             MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType,
             &AVBL_CmdMsgHdrQ, AVBL_GetCmdMsg);

/* Creation of output port-0 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>0)
static void *AVBL_GetDataMsg0(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AVBL_MediaBuffer[0][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(AVBL_OutPort0, 0UL, AVBL_DataMsgHdrQ0,
                        AVBL_Task, AVBL_GetDataMsg0,
                        MSYS_COMP_ID_AVBL, SystemEvent8);
#endif

/* Creation of output port-1 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>1)
static void *AVBL_GetDataMsg1(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AVBL_MediaBuffer[1][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(AVBL_OutPort1, 1UL, AVBL_DataMsgHdrQ1,
                        AVBL_Task, AVBL_GetDataMsg1,
                        MSYS_COMP_ID_AVBL, SystemEvent9);
#endif

/* Creation of output port-2 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>2)
static void *AVBL_GetDataMsg2(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AVBL_MediaBuffer[2][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(AVBL_OutPort2, 2UL, AVBL_DataMsgHdrQ2,
                        AVBL_Task, AVBL_GetDataMsg2,
                        MSYS_COMP_ID_AVBL, SystemEvent10);
#endif

/* Creation of output port-3 */
#if (AVBL_MAX_NUM_OUTPUT_PORTS>3)
static void *AVBL_GetDataMsg3(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AVBL_MediaBuffer[3][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(AVBL_OutPort3, 3UL, AVBL_DataMsgHdrQ3,
                        AVBL_Task, AVBL_GetDataMsg3,
                        MSYS_COMP_ID_AVBL, SystemEvent11);
#endif

extern const MSYS_OutPortType* AVBL_OutPort[];
/* Pool of output port structure */
const MSYS_OutPortType* AVBL_OutPort[AVBL_MAX_NUM_OUTPUT_PORTS] = {
#if (AVBL_MAX_NUM_OUTPUT_PORTS>0)
    &AVBL_OutPort0,
#endif
#if (AVBL_MAX_NUM_OUTPUT_PORTS>1)
    &AVBL_OutPort1,
#endif
#if (AVBL_MAX_NUM_OUTPUT_PORTS>2)
    &AVBL_OutPort2,
#endif
#if (AVBL_MAX_NUM_OUTPUT_PORTS>3)
    &AVBL_OutPort3,
#endif
};

/***********************************************************************
 FUNCTION NAME: AVBL_SendCmd

 ARGUMENTS:
             aCmd   -> The command-response structure with command
             aClientMask  -> The client mask to be used to signal on
                       completion
             aHdr   -> The pointer to message header

 DESCRIPTION: API to post the commands to the AVB Command Queue

 RETURNS: The Message queue return code

***********************************************************************/
static int32_t AVBL_SendCmd(const MSYS_CmdType *const aCmd,
                            const uint32_t aClientMask,
                            const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd) && (MSYS_COMP_ID_AVBL == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the AVBL component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            int32_t status = 0U;
            status = BCM_ActivateTask(AVBL_Task);
            if (0U != status) {
                AVBL_ReportError(AVBL_COMP_SEND_CMD_PROC, retVal,
                                (uint32_t)__LINE__,
                                (uint32_t)status, 0UL, 0UL);
            }
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&AVBL_CmdQ, (const void * const)aCmd,
                              aClientMask, aHdr);
    }
    return retVal;
}

/***********************************************************************
 FUNCTION NAME: AVBL_RecvResp

 ARGUMENTS:
             aHdr -> The pointer to message header
             aCmd -> The command-response structure with response

 DESCRIPTION: API to receive response for the commands posted

 RETURNS: The Message queue return code

***********************************************************************/
static int32_t AVBL_RecvResp(const MSGQ_MsgHdrType* const aHdr,
                             MSYS_CmdType *const aCmd)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Sink component's command queue */
        retVal = MSGQ_RecvMsg(&AVBL_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: AVBL_GetMediaInPort

 ARGUMENTS: aIndex -> Input port index

 DESCRIPTION: Function to get the MSYS_InPortType

 RETURNS: For valid index       Pointer to Media input structure
          For invalid index     NULL
***********************************************************************/
static const MSYS_InPortType* AVBL_GetMediaInPort(const uint32_t aIndex)
{
    return NULL;
}

/***********************************************************************
 FUNCTION NAME: AVBL_GetMediaOutPort

 ARGUMENTS: aIndex -> Output port index

 DESCRIPTION: API to get the MSYS_OutPortType where the message
              needs to be posted to

 RETURNS: For valid index       Pointer to Media outport structure
          For invalid index     NULL
***********************************************************************/
static const MSYS_OutPortType* AVBL_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port = NULL;

    if (AVBL_MAX_NUM_OUTPUT_PORTS > aIndex) {
        port = AVBL_OutPort[aIndex];
    } else {
        AVBL_ReportError(AVBL_COMP_GET_MEDOUTPORT_PROC,
                         BCM_ERR_INVAL_PARAMS,
                        (uint32_t) __LINE__, aIndex, 0UL, 0UL);
    }

    return port;
}

/* Component function table for AVB */
static const MSYS_CompFnTblType AVBL_CompFnTable =
{
    AVBL_SendCmd,
    AVBL_RecvResp,
    AVBL_GetMediaOutPort,
    AVBL_GetMediaInPort,
};

/* Function to return Component function table for component */
const MSYS_CompFnTblType* AVBL_GetCompFnTable(void)
{
    return &AVBL_CompFnTable;
};

/***********************************************************************
 FUNCTION NAME: AVBL_FlushBuffers

 ARGUMENTS: None

 DESCRIPTION: API to dequeue the out buffers to the next component
              and clear all CIRCQ's

 RETURNS: None
***********************************************************************/
static void AVBL_FlushBuffers (void)
{
    uint8_t portIdx = 0U;
    uint32_t mediaBufIdx = 0UL;
    int32_t retVal = BCM_ERR_OK;
    MSYS_BufferType *mediaBufferPtr = NULL;

    /* For each output port, clear the port MsgQ and CIRCQ */
    for (portIdx = 0U; portIdx < AVBL_MAX_NUM_OUTPUT_PORTS; portIdx++) {

        /* Initialize circular Q of each output port to reset */
        /* read and write pointers                            */
        retVal = CIRCQ_Reset(AVBL_ProcQ[portIdx]);
        if (BCM_ERR_OK != retVal) {
            AVBL_ReportError(AVBL_FLUSH_BUFFERS_API_ID, retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        /* Check on all messages in output port MsgQ */
        retVal = MSGQ_GetMsgIdx(&AVBL_OutPort[portIdx]->dataQ,
                                &mediaBufIdx);
        if (BCM_ERR_OK != retVal){
            continue;
        }

        /* Update the media data size as zero and flush them */
        mediaBufferPtr = &AVBL_MediaBuffer[portIdx][mediaBufIdx];
        mediaBufferPtr->buffer[0].size = 0UL;

        retVal = MSGQ_CompleteMsgIdx (&AVBL_OutPort[portIdx]->dataQ,
                                      mediaBufIdx);
        if (BCM_ERR_OK != retVal) {
            AVBL_ReportError(AVBL_FLUSH_BUFFERS_API_ID, retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

    }

    /* Initialize the ethernet Rx CIRCQ to reset RW pointers */
    retVal = CIRCQ_Reset(&AVBL_FillQ);
    if (BCM_ERR_OK != retVal) {
        AVBL_ReportError(AVBL_FLUSH_BUFFERS_API_ID, retVal,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }

    return;
}

/***********************************************************************
 FUNCTION NAME: AVBL_AtomicAlloc

 ARGUMENTS: None

 DESCRIPTION: API to allocate a free buffer for Rx data

 RETURNS: Index on available free buffer
***********************************************************************/
static uint32_t AVBL_AtomicAlloc(void)
{
    uint32_t exclusive = 0UL, val = 0UL;
    uint32_t index = 0xFFUL;
    uint32_t i = 0UL;

    for (i = 0UL; i < AVBL_ALLOC_FLAG_COUNT; i++) {
        do {
            exclusive = 0UL;
            val = CORTEX_LdrEx(&AVBL_Comp.allocFlags[i]);
            if (0UL == val) {
                index = 0xFFUL;
            } else {
                index = 31UL - (uint32_t) __builtin_clz(val);
                val &= ~(1UL << index);
                exclusive = CORTEX_StrEx(&AVBL_Comp.allocFlags[i], val);
            }
        } while (1UL == exclusive);
        if (index != 0xFFUL) {
            index = ((32UL * i) + index);
            break;
        }
    }
    return index;
}

/***********************************************************************
 FUNCTION NAME: AVBL_RxPktIndication

 ARGUMENTS:
            ctrlIdx -> NIF controller identifier
            pktBuf -> Pointer to L2 packet buffer
            pktLen -> Length of L2 packet
            srcMacAddr -> Source MAC address
            destMacAddr -> Destination MAC address
            priority -> Packet priority

 DESCRIPTION: API to handle Rx notifications from Ethernet

 RETURNS: None
***********************************************************************/
void AVBL_RxPktIndication (NIF_CntrlIDType ctrlIdx,
                           const uint8_t *const pktBuf,
                           uint16_t pktLen,
                           const uint8_t *const srcMacAddr,
                           const uint8_t *const destMacAddr,
                           uint32_t priority)
{
    int32_t retVal = BCM_ERR_OK;
    int32_t ret = BCM_ERR_OK;

    /* Increment stats for every Rx packet received */
    AVBL_Comp.lstrStats.pktRxCount++;

    /* Rx data is applicable only for 1722 listener */
    if ((NULL != pktBuf)
        && (0U != pktLen)
        && (MAX_ETH_RX_BUF_SIZE >= (uint32_t)pktLen)
        && (AVBL_STATE_RUNNING == AVBL_Comp.state)) {

        uint32_t index = AVBL_AtomicAlloc();

        /* If a valid free buffer is available */
        if (index < NUM_RX_PKT_BUFFERS) {

            AVBL_Comp.rxBuf[index].filledSize = pktLen;
            AVBL_Comp.rxBuf[index].copiedSize = 0UL;

            BCM_MemSet(&AVBL_Comp.rxBuf[index].params,
                       0U, sizeof(AVBL_Comp.rxBuf[index].params));
            /* Copy the incoming data */
            BCM_MemCpy(&AVBL_Comp.rxBuf[index].rxData, pktBuf, pktLen);

            /* Copy Rx data details to CIRCQ */
            ret = CIRCQ_Push(&AVBL_FillQ, (char*)&index, 1UL);
            if (BCM_ERR_OK != ret) {
                /* Increment stats in case data push failed */
                AVBL_Comp.lstrStats.dropCount++;
            }
        } else {
            /* Increment stats in case no Rx buffer was available */
            AVBL_Comp.lstrStats.dropCount++;
            ret = BCM_ERR_NOMEM;
        }
        /* Trigger AVBL task to process Rx data added into CIRCQ */
        retVal = BCM_SetEvent(AVBL_Task, AVBL_RX_EVENT);
    } else {
        /* Increment stats in case of an invalid Rx buffer */
        AVBL_Comp.lstrStats.invalidRxCount++;
    }

    /* Error Check and logging */
    if ((BCM_ERR_OK != retVal) || (BCM_ERR_OK != ret)){
        AVBL_ReportError(BRCM_ARCH_AVBL_COMP_RX_PKT_INDICATION_PROC,
                        (int32_t)ret,
                        (uint32_t) __LINE__, (uint32_t)retVal, 0UL, 0UL);
    }
    return;
}

/***********************************************************************
 FUNCTION NAME: AVBL_TxPktConfirmation

 ARGUMENTS:
            ctrlIdx -> NIF controller identifier
            bufIdx -> Buffer identifier for the L2 packet

 DESCRIPTION: API to handle Tx Confirmation notification

 RETURNS: None
***********************************************************************/
void AVBL_TxPktConfirmation (NIF_CntrlIDType ctrlIdx,
                             uint32_t bufIdx)
{
    return;
}

/***********************************************************************
 FUNCTION NAME: AVBL_ResetStateHandler

 ARGUMENTS: None

 DESCRIPTION: API to handle transition of component from
              RESET to INIT state.

 RETURNS: None
***********************************************************************/
static void AVBL_ResetStateHandler(void)
{
    int32_t retVal;
    /* Setting initial value to detect location */
    /* of error from error log                  */
    uint32_t i = AVBL_MAX_NUM_OUTPUT_PORTS+1UL;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&AVBL_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBL_CmdMsg[cmdQIdx];

        /* If a INIT command is received */
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {

            /* Initialize circular Q for receiving Rx data */
            retVal = CIRCQ_Reset(&AVBL_FillQ);
            if (BCM_ERR_OK != retVal) {
                goto end;
            }
            for (i = 0UL; i<AVBL_MAX_NUM_OUTPUT_PORTS; i++) {
                /* Initialize circular Q of each output port */
                retVal = CIRCQ_Reset(AVBL_ProcQ[i]);
                if (BCM_ERR_OK != retVal) {
                    goto end;
                }
                AVBL_Comp.medBufFilledSize[i] = 0UL;
            }

            /* Allocation flag handling based on the number of */
            /* NUM_RX_PKT_BUFFERS declared                     */
#if (0UL < (AVBL_ALLOC_FLAG_COUNT - 1UL)) /* MISRA */
            for (i = 0UL; i < (AVBL_ALLOC_FLAG_COUNT - 1UL); i++) {
                (void)ATOMIC_Set(&AVBL_Comp.allocFlags[i], 0xFFFFFFFFUL);
            }
#endif

            if ((NUM_RX_PKT_BUFFERS & 0x1FUL) == 0UL) {
                (void)ATOMIC_Set(&AVBL_Comp.allocFlags[AVBL_ALLOC_FLAG_COUNT - 1UL],
                          0xFFFFFFFFUL);
            } else {
                (void)ATOMIC_Set(&AVBL_Comp.allocFlags[AVBL_ALLOC_FLAG_COUNT - 1UL],
                          (1UL << (NUM_RX_PKT_BUFFERS & 0x1FUL)) - 1UL);
            }

            /* Clear memory region of stats */
            BCM_MemSet(&AVBL_Comp.lstrStats, 0U, sizeof(AVB_Lstr_Stats));

            /* Update the component state */
            AVBL_Comp.state = AVBL_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        retVal = MSGQ_CompleteMsgIdx(&AVBL_CmdQ, cmdQIdx);

end:
        if (BCM_ERR_OK != retVal) {
            AVBL_ReportError(AVBL_RESET_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, i, cmdQIdx, 0UL);
        }
    }
}

/**
*/
typedef union _AVBL_ConfigDataType {
    const uint8_t* hdr;
    const AVBL_1722ConfigType *cfg;
} AVBL_ConfigDataType;

/***********************************************************************
 FUNCTION NAME: AVBL_InitStateHandler

 ARGUMENTS: None

 DESCRIPTION: API to handle transition of component from
              INIT to READY state.

 RETURNS: None
***********************************************************************/
static void AVBL_InitStateHandler(void)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint32_t cntVal = 0UL;
    AVBL_ConfigDataType config;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&AVBL_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBL_CmdMsg[cmdQIdx];
        config.hdr = processCmd->cfgData;

        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Check if the number of allocated ports is same     */
            /* as the number of ports for which the configuration */
            /* is received                                        */
            if (AVBL_MAX_NUM_OUTPUT_PORTS == config.cfg->numOutPort){
                /* Store the stream configuration */
                for (cntVal = 0; cntVal < config.cfg->numOutPort; cntVal++) {
                    AVBL_Comp.compCfg[cntVal] =
                                        config.cfg->outStreamCfg[cntVal];
                }

                AVBL_Comp.state = AVBL_STATE_READY;
                processCmd->response = BCM_ERR_OK;
            } else {
                processCmd->response = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component */
            AVBL_Comp.state = AVBL_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&AVBL_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AVBL_ReportError(AVBL_INIT_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/***********************************************************************
 FUNCTION NAME: AVBL_ReadyStateHandler

 ARGUMENTS: None

 DESCRIPTION: API to handle transition of component from
              READY to RUNNING state.

 RETURNS: None
***********************************************************************/
static void AVBL_ReadyStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint32_t cntVal = 0UL;
    AVBL_ConfigDataType config;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&AVBL_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBL_CmdMsg[cmdQIdx];

        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_START:
            AVBL_Comp.state = AVBL_STATE_RUNNING;
            /* Signal the task this component to start processing */
            retVal = BCM_SetEvent(AVBL_Task, AVBL_PORT_EVENTS);
            if (BCM_ERR_OK != retVal) {
                AVBL_ReportError(AVBL_READY_STATE_HANDLER_API_ID,
                                retVal,
                                (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            }
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_CONFIG:
            config.hdr = processCmd->cfgData;
            /* Check if the number of allocated ports is same     */
            /* as the number of ports for which the configuration */
            /* is received                                        */
            if (AVBL_MAX_NUM_OUTPUT_PORTS == config.cfg->numOutPort){
                /* Store the stream configuration */
                for (cntVal = 0UL; cntVal < config.cfg->numOutPort; cntVal++) {
                    AVBL_Comp.compCfg[cntVal] =
                                        config.cfg->outStreamCfg[cntVal];
                }
                processCmd->response = BCM_ERR_OK;
            } else {
                processCmd->response = BCM_ERR_INVAL_PARAMS;
            }
            break;
        case MSYS_COMP_CMD_DEINIT:
            AVBL_Comp.state = AVBL_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&AVBL_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AVBL_ReportError(AVBL_READY_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/***********************************************************************
 FUNCTION NAME: AVBL_RunStateHandler

 ARGUMENTS: None

 DESCRIPTION: API to handle data in RUNNING state.

 RETURNS: None
***********************************************************************/
static void AVBL_RunStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    MSYS_BufferType * mediaBufferPtr;
    uint8_t idxArray[NUM_RX_PKT_BUFFERS] = {0};
    uint32_t localCnt;
    uint32_t index = 0;
    uint32_t i,j;

    /* Check if there are any commands in the command queue. */
    /* If there are any, process the command first.          */
    retVal = MSGQ_GetMsgIdx(&AVBL_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBL_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            AVBL_Comp.state = AVBL_STATE_READY;
            AVBL_FlushBuffers();
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            AVBL_Comp.state = AVBL_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }

        /* Mark the processed command as complete */
        retVal = MSGQ_CompleteMsgIdx(&AVBL_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AVBL_ReportError(AVBL_RUN_STATE_HANDLER_API_ID,
                            (int32_t)retVal,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }

    /* Process incoming Rx dat in if running state */
    if (AVBL_STATE_RUNNING == AVBL_Comp.state) {
        AVBL_RxBuf *rxBuf;

        /* Pick Rx data got from ethernet */
        localCnt = CIRCQ_Pop(&AVBL_FillQ,
                           (char*)idxArray,
                            NUM_RX_PKT_BUFFERS);

        /* For the number of Rx buffers in the CIRCQ */
        for (i = 0UL; i < localCnt; i++) {

            rxBuf = &AVBL_Comp.rxBuf[idxArray[i]];
            /* Use AVB library and get the payload details */
            retVal = AVB_Extract1722Header(rxBuf->rxData,
                                rxBuf->filledSize, &rxBuf->params);
            if (BCM_ERR_OK == retVal) {
                uint32_t matched = 0UL;

                /* Update the current size of Rx packet processed */
                rxBuf->filledSize = rxBuf->params.payDetails.payLoadOffset+
                                            rxBuf->params.payDetails.payLoadLen;
                rxBuf->copiedSize = rxBuf->params.payDetails.payLoadOffset;

                for (j=0; j < AVBL_MAX_NUM_OUTPUT_PORTS; j++ ) {

                    /* Get the output port for this Rx packet */
                    if (rxBuf->params.payDetails.streamID ==
                            AVBL_Comp.compCfg[j].streamID) {

                        matched = 1UL;
                        /* Push the data to the port CIRCQ */
                        retVal = CIRCQ_Push(AVBL_ProcQ[j],
                                           (char*)&idxArray[i],
                                           1UL);
                        if (BCM_ERR_OK == retVal) {
                            /* Check for packet loss and update stats */
                            if (AVBL_Comp.pktSeqNum[j] !=
                                            rxBuf->params.payDetails.pktSeqNum) {
                                AVBL_Comp.lstrStats.missCount[j]++;
                                AVBL_Comp.pktSeqNum[j] =
                                            rxBuf->params.payDetails.pktSeqNum;
                            }
                            AVBL_Comp.pktSeqNum[j]++;
                        } else {
                            AVBL_Comp.lstrStats.cQdrop[j]++;
                            ATOMIC_SetBit(&AVBL_Comp.allocFlags[idxArray[i]>>5UL],
                                         idxArray[i] & 0x1FUL);
                        }
                    }
                }
                if (matched == 0UL) {
                    /* Increment the error stats for unexpected stream packets */
                    AVBL_Comp.lstrStats.numUnmatchedStreamPkts++;
                    ATOMIC_SetBit(&AVBL_Comp.allocFlags[idxArray[i]>>5UL],
                                 idxArray[i] & 0x1FUL);
                }
            } else {
                /* Increment the error stats for AVB library errors */
                AVBL_Comp.lstrStats.numRxPktHdrErrors++;
                ATOMIC_SetBit(&AVBL_Comp.allocFlags[idxArray[i]>>5UL],
                             idxArray[i] & 0x1FUL);
            }
        }

        /* For all the output ports, check the port CIRCQ for data */
        /* and process them                                        */
        for (j=0; j<AVBL_MAX_NUM_OUTPUT_PORTS; j++ ) {
            index = 0UL;

            localCnt = CIRCQ_Peek(AVBL_ProcQ[j],
                                (char*)&idxArray,
                                 NUM_RX_PKT_BUFFERS);

            /* For all the data packets in the output port CIRCQ */
            while (localCnt > index) {
                uint32_t mediaBufIdx;
                uint32_t copySize;
                uint32_t outSize;
                uint32_t inSize;

                /* Get the actual Rx data packet */
                rxBuf = &AVBL_Comp.rxBuf[idxArray[index]];
                inSize = rxBuf->filledSize - rxBuf->copiedSize;
                retVal = MSGQ_GetMsgIdx(&AVBL_OutPort[j]->dataQ, &mediaBufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Track the number of times output buffer was NA */
                    AVBL_Comp.lstrStats.numRxOutBufNA[j]++;
                    break;
                }

                mediaBufferPtr = &AVBL_MediaBuffer[j][mediaBufIdx];

                if (0UL == AVBL_Comp.medBufFilledSize[j]) {
                    uint8_t startData[5] = {0x0U, 0x0U, 0x0U, 0x1U, 0x65U};
                    uint32_t startDataSz = 0UL;

                    if (AVB_PAYLOAD_H264_SPS_PPS_NAL == rxBuf->params.payDetails.payType) {
                        /* Copy only the NAL start code for SPS/PPS NAL */
                        startDataSz = 4UL;
                    } else if (AVB_PAYLOAD_H264_IDR_START == rxBuf->params.payDetails.payType) {
                        /* Copy the NAL start code with NAL reference byte for IDR slice */
                        startDataSz = 5UL;
                    } else {
                        /* Nothing */
                    }

                    /* Fill the output port buffer with NAL start code */
                    /* which is needed by the H264 decoder             */
                    BCM_MemCpy(mediaBufferPtr->buffer[0].data,
                               startData, startDataSz);
                    AVBL_Comp.medBufFilledSize[j] += startDataSz;
                }

                /* Process the data onto the output port buffer */
                outSize = mediaBufferPtr->buffer[0].size - AVBL_Comp.medBufFilledSize[j];
                copySize =  BCM_MIN(inSize, outSize);

                /* validate mediabuffer */
                if ((NULL != mediaBufferPtr->buffer[0].data)
                    && (mediaBufferPtr->buffer[0].size >=
                          (AVBL_Comp.medBufFilledSize[j] + copySize))){

                    /* Fill the output port buffer */
                    BCM_MemCpy(&mediaBufferPtr->buffer[0].data[AVBL_Comp.medBufFilledSize[j]],
                            &rxBuf->rxData[rxBuf->copiedSize], copySize);
                    rxBuf->copiedSize += copySize;
                    AVBL_Comp.medBufFilledSize[j] += copySize;

                    /* In case this Rx packet marks end of aggregation */
                    if (rxBuf->params.payDetails.isSliceEnd == 1UL) {
                        mediaBufferPtr->buffer[0].size =
                                        AVBL_Comp.medBufFilledSize[j];
                    }

                    /* In case the output port buffer is completely filled */
                    if (rxBuf->copiedSize == rxBuf->filledSize) {
                        uint32_t ignore = 0UL;
                        uint32_t val;
                        mediaBufferPtr->timestamp = rxBuf->params.payDetails.pktSysTime;
                        mediaBufferPtr->numChannels = 1UL;
                        val = CIRCQ_Pop(AVBL_ProcQ[j],
                                      (char*)&ignore, 1UL);
                        if (1UL == val) {
                            ATOMIC_SetBit(&AVBL_Comp.allocFlags[ignore>>5UL],
                                        (ignore&0x1FUL));
                            index++;
                        } else {
                            AVBL_ReportError(AVBL_RUN_STATE_HANDLER_API_ID,
                                             BCM_ERR_INVAL_PARAMS,
                                             (uint32_t)__LINE__,
                                             val, 0UL, 0UL);
                        }
                    }
                    mediaBufferPtr->response = BCM_ERR_OK;
                } else {
                    mediaBufferPtr->response = BCM_ERR_INVAL_PARAMS;
                }
                if ((AVBL_Comp.medBufFilledSize[j] == mediaBufferPtr->buffer[0].size)
                    || (mediaBufferPtr->response == BCM_ERR_INVAL_PARAMS)) {
                    AVBL_Comp.medBufFilledSize[j] = 0UL;
                    retVal = MSGQ_CompleteMsgIdx(&AVBL_OutPort[j]->dataQ, mediaBufIdx);
                    if (BCM_ERR_OK != retVal) {
                        AVBL_ReportError(AVBL_RUN_STATE_HANDLER_API_ID,
                                        (int32_t)retVal,
                                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                    }
                }
            }
        }
    }
}

/* Task function for AVBL component task */
TASK(AVBL_Task)
{
    BCM_EventMaskType eventMask = 0UL;
    int32_t status = 0U;

    do {
        /* Wait for all component events */
        status = BCM_WaitEvent(AVBL_CMDQ_EVENT |
                           AVBL_RX_EVENT   |
                           AVBL_PORT_EVENTS);
        if (BCM_ERR_OK != status) {
            AVBL_ReportError(AVBL_AVBL_TASK_API_ID, (int32_t)status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
        status = BCM_GetEvent(AVBL_Task, &eventMask);
        if (BCM_ERR_OK != status) {
            AVBL_ReportError(AVBL_AVBL_TASK_API_ID, (int32_t)status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
        status = BCM_ClearEvent(eventMask);
        if (BCM_ERR_OK != status) {
            AVBL_ReportError(AVBL_AVBL_TASK_API_ID, (int32_t)status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        /* Call the state handling helper function depending on */
        /* the state that this AVBL component is in             */
        switch(AVBL_Comp.state) {
        case AVBL_STATE_RESET:
            AVBL_ResetStateHandler();
            break;
        case AVBL_STATE_INIT:
            AVBL_InitStateHandler();
            break;
        case AVBL_STATE_READY:
            AVBL_ReadyStateHandler();
            break;
        case AVBL_STATE_RUNNING:
            AVBL_RunStateHandler();
            break;
        default:
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (AVBL_STATE_RESET != AVBL_Comp.state);

    status = BCM_TerminateTask();
    if (BCM_ERR_OK != status) {
        AVBL_ReportError(AVBL_AVBL_TASK_API_ID, status,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }
}
