/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
File Name  : src_comp.c
Description: This file contains a sample source component that can be used as a
             reference.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO

#include <component.h>
#include <msg_queue.h>
#include <ulog.h>
#include <bcm_utils.h>
#include <ee.h>
#include <ex_int_comp.h>
#include <media_port.h>
#include <circ_queue.h>
#include <osil/bcm_osil.h>

/******************************************************************************
 * Data specific to sink component
 *****************************************************************************/
/**
 * Sink Component State Machine.
 * Sink component shall be in EXINT_STATE_RESET by default. The state machine
 * for sink component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * EXINT_STATE_RESET    MSYS_COMP_CMD_INIT       EXINT_STATE_INIT
 * EXINT_STATE_INIT     MSYS_COMP_CMD_CONFIG     EXINT_STATE_READY
 * EXINT_STATE_READY    MSYS_COMP_CMD_CONFIG     EXINT_STATE_READY
 * EXINT_STATE_READY    MSYS_COMP_CMD_START      EXINT_STATE_RUNNING
 * EXINT_STATE_READY    MSYS_COMP_CMD_DEINIT     EXINT_STATE_RESET
 * EXINT_STATE_RUNNING  MSYS_COMP_CMD_STOP       EXINT_STATE_READY
 * EXINT_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     EXINT_STATE_RESET
 */
typedef uint32_t ExInt_StateType;
#define EXINT_STATE_RESET   (0UL)   /**< Intermediate component Reset state */
#define EXINT_STATE_INIT    (1UL)   /**< Intermediate component Init state */
#define EXINT_STATE_READY   (2UL)   /**< Intermediate component Ready state */
#define EXINT_STATE_RUNNING (3UL)   /**< Intermediate component Running state */

/* Sink component even masks */
/** Event mask for Sink component's command Queue */
#define EXINT_CMDQ_EVENT        SystemEvent0

/** Event mask for all the input ports for Sink component */
#define EXINT_PORT_EVENTS     (SystemEvent4 | SystemEvent8)

/** Maximum number of Channels for Sink component's input port */
#define EXINT_CHANNEL_MAX       (1UL)

/** Structure to hold component information */
typedef struct _ExInt_CompType {
    ExInt_StateType state;
    uint32_t fillReqCnt;    /**< Number of MSYS_FillBuffer requests made */
    uint32_t fillDoneCnt;   /**< Number of MSYS_FillBuffer requests serviced successfully */
    uint32_t fillCnclCnt;   /**< Number of MSYS_FillBuffer requests cancelled */
    uint32_t nextBufIdx;    /**< Index to the next buffer to be used */
    uint32_t dataBufCnt;    /**< Number of Data Buffers processed */
    uint8_t  buffers[16UL];/**< Character buffer for output */
} ExInt_CompType;

static ExInt_CompType ExInt_Comp = {
    .state = EXINT_STATE_RESET,
    .fillReqCnt = 0UL,
    .fillDoneCnt = 0UL,
    .fillCnclCnt = 0UL,
    .nextBufIdx = 0UL,
    .dataBufCnt = 0UL,
};

#define EXSNK_CIRCQ_SIZE    (16UL)
static uint8_t ExInt_CIRCQBuf[EXSNK_CIRCQ_SIZE];
CIRCQ_INDEX_DEFINE(ExSnk_CIRCQIdx, EXSNK_CIRCQ_SIZE);
static const CIRCQ_Type ExSnk_CIRCQ = CIRCQ_INIT(&ExSnk_CIRCQIdx, EXSNK_CIRCQ_SIZE,
                                sizeof(uint8_t), ExInt_CIRCQBuf, CIRCQ_OVERWRITE_DISABLE);

/******************************************************************************
 * Sink <==> Framework interaction
 *
 * Interface for component<-->framework interactions are defined in component.h
 * Framwork uses a message queue to send commands to a component, where
 * framework shall be the client and component shall be the server. Any
 * component shall support the following commands from the framework:
 *  1) MSYS_COMP_CMD_INIT
 *  2) MSYS_COMP_CMD_CONFIG
 *  3) MSYS_COMP_CMD_START
 *  4) MSYS_COMP_CMD_STOP
 *  5) MSYS_COMP_CMD_DEINIT
 *****************************************************************************/

/** Define and initialize MSGQ_HdrQType instance for Sink component */
MSGQ_DEFINE_HDRQ(ExInt_CmdMsgHdrQ);

/** Array for commands to Sink component */
static MSYS_CmdType ExInt_CmdMsg[MSGQ_SIZE];

/** GetMessage function to get the ith message */
static void * ExInt_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &ExInt_CmdMsg[i];
    }

    return msg;
}

/* Define the command queue for the sink component, on which framework shall
 * post the different commands for the sink component.
 */
MSGQ_DEFINE (ExInt_CmdQ, ExInt_Task, EXINT_CMDQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType,
             &ExInt_CmdMsgHdrQ, ExInt_GetCmdMsg);

/* Framework shall use this API to post a command to this sink component */
int32_t ExInt_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;

    if ((NULL != aCmd) && (MSYS_COMP_ID_EX_INT == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the sink component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            (void)BCM_ActivateTask(ExInt_Task);
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&ExInt_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the response of a command that was
 * posted to this Sink component
 */
int32_t ExInt_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Sink component's command queue */
        retVal = MSGQ_RecvMsg(&ExInt_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

static MSYS_BufferType ExInt_MediaBuffer[MSGQ_SIZE];

static void *ExInt_GetDataMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &ExInt_MediaBuffer[i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT_V2(ExInt_OutPort, 0UL, ExInt_DataMsgHdrQ,
                          ExInt_Task, ExInt_GetDataMsg,
                          MSYS_COMP_ID_EX_INT, SystemEvent8, ".data.ExInt_DataMsgHdrQ");

/* Framework shall use this API to get the MSYS_OutPortType of this component where
 * MSYS_FillBuffer() request shall be posted (if any).
 */
const MSYS_OutPortType* ExInt_GetOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port;
    /* Since this is sink component there are no output ports */
    switch (aIndex) {
    case 0UL:
        port = &ExInt_OutPort;
        break;
    default:
        port = NULL;
        break;
    }
    return port;
}

/******************************************************************************
 * Sink component <--> Other component interaction
 * Interfaces for component<-->component interactions are defined in
 * media_port.h. Components work in pull model, where the sink component raises
 * a MSYS_FillBuffer() request to the framework which shall route the request to the
 * corresponding source component's output port. Note that any output<-->input
 * port mapping shall be provided by the Application that uses the components.
 *****************************************************************************/

/* This callback shall be invoked from framework to this Sink component to
 * notify completion of buffer.
 */
static void ExInt_FillBufferDone(const MSYS_InPortType *const aPort,
                                    MSYS_BufferType *const aMediaBuffer)
{
    if ((NULL != aMediaBuffer)
        && (EXINT_CHANNEL_MAX == aMediaBuffer->numChannels)) {
        if (BCM_ERR_OK == aMediaBuffer->response) {
            if ((0UL != aMediaBuffer->buffer[0UL].size)) {
                (void)CIRCQ_Push(&ExSnk_CIRCQ, (const char*)aMediaBuffer->buffer[0UL].data,
                                        aMediaBuffer->buffer[0UL].size);
                ExInt_Comp.fillDoneCnt++;
            } else {
                /* Buffer size 0 implies that this buffer was cancelled */
                ExInt_Comp.fillCnclCnt++;
            }
        }
    }
}

/* Define input port 0 for this sink component (this component uses only one
 * input port
 */
MSYS_DECLARE_IN_PORT_V2(ExInt_InPort, 0UL, MSYS_COMP_ID_EX_INT, SystemEvent4,
                         ExInt_InPortRW, ExInt_FillBufferDone, ".data.ExInt_InPortRW");

/* Framework shall use this API to get the MSYS_InPortType of this component */
const MSYS_InPortType* ExInt_GetInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *port = NULL;
    /* Since this is sink component there are no output ports */
    switch (aIndex) {
    case 0UL:
        port = &ExInt_InPort;
        break;
    default:
        port = NULL;
        break;
    }
    return port;
}

/******************************************************************************
 * Sink component Functionality.
 *
 * Sink component uses a single task, that shall be the server task for the
 * framework<-->component message queue. The same task handles all interaction
 * with other components as well.
 *****************************************************************************/

/* Helper function that handles all transitions from Reset state */
static void ExInt_ResetStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&ExInt_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExInt_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
            (void)MSYS_InPortInit(&ExInt_InPort);
            (void)MSYS_OutPortInit(&ExInt_OutPort);
            ExInt_Comp.state = EXINT_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        (void)MSGQ_CompleteMsgIdx(&ExInt_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Init state */
static void ExInt_InitStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&ExInt_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExInt_CmdMsg[cmdQIdx];
        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            ExInt_Comp.state = EXINT_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component.
             */
            ExInt_Comp.state = EXINT_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        (void)MSGQ_CompleteMsgIdx(&ExInt_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Ready state */
static void ExInt_ReadyStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&ExInt_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExInt_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_START:
            ExInt_Comp.state = EXINT_STATE_RUNNING;
            /* Signal the input port of this Sink component to start processing */
            (void)BCM_SetEvent(ExInt_Task, EXINT_PORT_EVENTS);
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            ExInt_Comp.state = EXINT_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        (void)MSGQ_CompleteMsgIdx(&ExInt_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Running state */
static void ExInt_RunStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    uint32_t isEmpty;
    uint32_t cnt;
    char val;
    MSYS_CmdType *processCmd = NULL;
    MSYS_BufferType mediaBuffer;
    MSYS_BufferType *mediaBufferPtr;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&ExInt_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExInt_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            retVal = CIRCQ_IsEmpty(&ExSnk_CIRCQ, &isEmpty);
            if (isEmpty == TRUE) {
                /* Cancel all mediabuffers and move to Ready state */
                (void)MSYS_InPortCancelAll(&ExInt_InPort);
                ExInt_Comp.state = EXINT_STATE_READY;
                processCmd->response = BCM_ERR_OK;
                /* Mark the processed command as complete */
                retVal = MSGQ_CompleteMsgIdx(&ExInt_CmdQ, cmdQIdx);
            }
            break;
        case MSYS_COMP_CMD_DEINIT:
            retVal = CIRCQ_IsEmpty(&ExSnk_CIRCQ, &isEmpty);
            if (isEmpty == TRUE) {
                /* Cancel all mediabuffers and move to Ready state */
                (void)MSYS_InPortCancelAll(&ExInt_InPort);
                ExInt_Comp.state = EXINT_STATE_READY;
                processCmd->response = BCM_ERR_OK;
                /* Mark the processed command as complete */
                retVal = MSGQ_CompleteMsgIdx(&ExInt_CmdQ, cmdQIdx);
            }
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            /* Mark the processed command as complete */
            retVal = MSGQ_CompleteMsgIdx(&ExInt_CmdQ, cmdQIdx);
            break;
        }

    }

    /* If we are still in the running state, send MSYS_FillBuffer() requests to
     * source component.
     */
    if (EXINT_STATE_RUNNING == ExInt_Comp.state) {
        do {
            /*TODO Validate timestamp  received from the source component */
            mediaBuffer.timestamp = 0ULL;
            mediaBuffer.buffer[0].data = &ExInt_Comp.buffers[(ExInt_Comp.nextBufIdx++) & 0xFUL];
            mediaBuffer.buffer[0].size = 1UL;
            mediaBuffer.numChannels = EXINT_CHANNEL_MAX;
            mediaBuffer.response = BCM_ERR_UNKNOWN;
            mediaBuffer.bufIdx = 0UL;

            retVal = MSYS_FillBuffer(&ExInt_InPort, &mediaBuffer);
            if (retVal == BCM_ERR_OK) {
                ExInt_Comp.fillReqCnt++;
            }
        } while (BCM_ERR_OK == retVal);

        /* Process any filled mediabuffers available at the input port */
        (void)MSYS_InPortProcess(&ExInt_InPort);

        do {
            cnt = CIRCQ_Peek(&ExSnk_CIRCQ, &val, 1UL);
            if (1UL == cnt) {
                uint32_t mediaBufIdx;
                retVal = MSGQ_GetMsgIdx(&ExInt_OutPort.dataQ, &mediaBufIdx);
                if (retVal != BCM_ERR_OK) {
                    break;
                }
                cnt = CIRCQ_Pop(&ExSnk_CIRCQ, &val, 1UL);
                mediaBufferPtr = &ExInt_MediaBuffer[mediaBufIdx];
                /* validate mediabuffer */
                if ((NULL != mediaBufferPtr->buffer[0].data) && (0UL != mediaBufferPtr->buffer[0].size)){
                    /* Fill the buffer */
                    mediaBufferPtr->buffer[0].data[0UL] = (uint8_t)val;
                    mediaBufferPtr->buffer[0].size = 1UL;
                    mediaBufferPtr->response = BCM_ERR_OK;
                } else {
                    mediaBufferPtr->response = BCM_ERR_INVAL_PARAMS;
                }

                retVal = MSGQ_CompleteMsgIdx(&ExInt_OutPort.dataQ, mediaBufIdx);
            }
        } while (cnt == 1UL);
    }
}

TASK(ExInt_Task)
{
    BCM_EventMaskType eventMask;

    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        (void)BCM_WaitEvent(EXINT_CMDQ_EVENT | EXINT_PORT_EVENTS);
        (void)BCM_GetEvent(ExInt_Task, &eventMask);
        (void)BCM_ClearEvent(eventMask);

        /* Call the state handling helper function depending on the state that
         * this sink component is in.
         */
        switch(ExInt_Comp.state)
        {
        case EXINT_STATE_RESET:
            ExInt_ResetStateHandler();
            break;
        case EXINT_STATE_INIT:
            ExInt_InitStateHandler();
            break;
        case EXINT_STATE_READY:
            ExInt_ReadyStateHandler();
            break;
        default: /* EXINT_STATE_RUNNING */
            ExInt_RunStateHandler();
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (EXINT_STATE_RESET != ExInt_Comp.state);

    (void)BCM_TerminateTask();
}
