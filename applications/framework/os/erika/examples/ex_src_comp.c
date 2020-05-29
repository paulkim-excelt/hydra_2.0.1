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
File Name  : ex_sink_comp.c
Description: This file contains an example sink component that can be used as a
             reference.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO

#include <component.h>
#include <media_port.h>
#include <msg_queue.h>
#include <ulog.h>
#include <atomic.h>
#include <ee.h>
#include <ex_src_comp.h>
#include <osil/bcm_osil.h>

/******************************************************************************
 * Data specific to sink component
 *****************************************************************************/
/**
 * Sink Component State Machine.
 * Sink component shall be in EXSRC_STATE_RESET by default. The state machine
 * for sink component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * EXSRC_STATE_RESET    MSYS_COMP_CMD_INIT       EXSRC_STATE_INIT
 * EXSRC_STATE_INIT     MSYS_COMP_CMD_CONFIG     EXSRC_STATE_READY
 * EXSRC_STATE_READY    MSYS_COMP_CMD_CONFIG     EXSRC_STATE_READY
 * EXSRC_STATE_READY    MSYS_COMP_CMD_START      EXSRC_STATE_RUNNING
 * EXSRC_STATE_READY    MSYS_COMP_CMD_DEINIT     EXSRC_STATE_RESET
 * EXSRC_STATE_RUNNING  MSYS_COMP_CMD_STOP       EXSRC_STATE_READY
 * EXSRC_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     EXSRC_STATE_RESET
 */
typedef uint32_t ExSrc_StateType;
#define EXSRC_STATE_RESET   (0UL)   /**< Sink component Reset state */
#define EXSRC_STATE_INIT    (1UL)   /**< Sink component Init state */
#define EXSRC_STATE_READY   (2UL)   /**< Sink component Ready state */
#define EXSRC_STATE_RUNNING (3UL)   /**< Sink component Running state */

/* Sink component even masks */
/** Event mask for Sink component's command Queue */
#define EXSRC_CMDQ_EVENT        SystemEvent0
/** Event mask for all the input ports for Sink component */
#define EXSRC_INPORT_EVENTS     (SystemEvent8)

/** Maximum number of Channels for Sink component's input port */
#define EXSRC_CHANNEL_MAX       (1UL)

extern uint8_t txData[1000];
extern uint32_t txPktCount;
/** Structure to hold Sink component information */
typedef struct _ExSrc_CompType {
    ExSrc_StateType state;
    uint32_t dataBufCnt;    /**< Number of Data Buffers processed */
} ExSrc_CompType;

static ExSrc_CompType ExSrc_Comp = {
    .state = EXSRC_STATE_RESET,
    .dataBufCnt = 0UL,
};

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
MSGQ_DEFINE_HDRQ(ExSrc_CmdMsgHdrQ);

/** Array for commands to Sink component */
static MSYS_CmdType ExSrc_CmdMsg[MSGQ_SIZE];

/** GetMessage function to get the ith message */
static void * ExSrc_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &ExSrc_CmdMsg[i];
    }

    return msg;
}

/* Define the command queue for the sink component, on which framework shall
 * post the different commands for the sink component.
 */
MSGQ_DEFINE (ExSrc_CmdQ, ExSrc_Task, EXSRC_CMDQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType,
             &ExSrc_CmdMsgHdrQ, ExSrc_GetCmdMsg);

/* Framework shall use this API to post a command to this sink component */
int32_t ExSrc_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd) && (MSYS_COMP_ID_EX_SRC == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the sink component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            (void)BCM_ActivateTask(ExSrc_Task);
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&ExSrc_CmdQ, (const void * const)aCmd, aMask, aHdr);
    }
    return retVal;
}

/* Framework shall use this API to get the response of a command that was
 * posted to this Sink component
 */
int32_t ExSrc_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Sink component's command queue */
        retVal = MSGQ_RecvMsg(&ExSrc_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

static MSYS_BufferType ExSrc_MediaBuffer[MSGQ_SIZE];

static void *ExSrc_GetDataMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &ExSrc_MediaBuffer[i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT_V2(ExSrc_OutPort, 0UL, ExSrc_DataMsgHdrQ,
                          ExSrc_Task, ExSrc_GetDataMsg,
                          MSYS_COMP_ID_EX_SRC, SystemEvent8, ".data.ExSrc_DataMsgHdrQ");

/* Framework shall use this API to get the MSYS_OutPortType of this component where
 * MSYS_FillBuffer() request shall be posted (if any).
 */
const MSYS_OutPortType* ExSrc_GetOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port;
    /* Since this is sink component there are no output ports */
    switch (aIndex) {
    case 0UL:
        port = &ExSrc_OutPort;
        break;
    default:
        port = NULL;
        break;
    }
    return port;
}

/* Framework shall use this API to get the MSYS_InPortType of this component */
const MSYS_InPortType* ExSrc_GetInPort(const uint32_t aIndex)
{
    return NULL;
}

/******************************************************************************
 * Source component <--> Other component interaction
 * Interfaces for component<-->component interactions are defined in
 * media_port.h. Components work in pull model, where the sink component raises
 * a MSYS_FillBuffer() request to the framework which shall route the request to the
 * corresponding source component's output port. Note that any output<-->input
 * port mapping shall be provided by the Application that uses the components.
 *****************************************************************************/

/******************************************************************************
 * Sink component Functionality.
 *
 * Sink component uses a single task, that shall be the server task for the
 * framework<-->component message queue. The same task handles all interaction
 * with other components as well.
 *****************************************************************************/

/* Helper function that handles all transitions from Reset state */
static void ExSrc_ResetStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&ExSrc_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSrc_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
            (void)MSYS_OutPortInit(&ExSrc_OutPort);
            ExSrc_Comp.state = EXSRC_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        (void)MSGQ_CompleteMsgIdx(&ExSrc_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Init state */
static void ExSrc_InitStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&ExSrc_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSrc_CmdMsg[cmdQIdx];
        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            ExSrc_Comp.state = EXSRC_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component.
             */
            ExSrc_Comp.state = EXSRC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        (void)MSGQ_CompleteMsgIdx(&ExSrc_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Ready state */
static void ExSrc_ReadyStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&ExSrc_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSrc_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_START:
            ExSrc_Comp.state = EXSRC_STATE_RUNNING;
            /* Signal the input port of this Sink component to start processing */
            (void)BCM_SetEvent(ExSrc_Task, EXSRC_INPORT_EVENTS);
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            ExSrc_Comp.state = EXSRC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        (void)MSGQ_CompleteMsgIdx(&ExSrc_CmdQ, cmdQIdx);
    }
}
static volatile uint32_t AlarmVal = 0UL;
void ExSrc_DataCb(void)
{
    if (EXSRC_STATE_RUNNING == ExSrc_Comp.state) {
        (void)ATOMIC_Inc(&AlarmVal);
        (void)SetEvent(ExSrc_Task, EXSRC_INPORT_EVENTS);
    }
}
static uint8_t Buffer[] = {(uint8_t)'A', (uint8_t)'B', (uint8_t)'C', (uint8_t)'D', (uint8_t)'E',
                           (uint8_t)'F', (uint8_t)'G', (uint8_t)'H', (uint8_t)'I', (uint8_t)'J',
                           (uint8_t)'K', (uint8_t)'L', (uint8_t)'M', (uint8_t)'N', (uint8_t)'O',
                           (uint8_t)'P', (uint8_t)'Q', (uint8_t)'R', (uint8_t)'S', (uint8_t)'T',
                           (uint8_t)'U', (uint8_t)'V', (uint8_t)'W', (uint8_t)'X', (uint8_t)'Y',
                           (uint8_t)'Z', (uint8_t)'\n'};
static int8_t ExSrc_idx = 0;

/* Helper function that handles all transitions from Running state */
static void ExSrc_RunStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint32_t localAlarmVal;
    MSYS_BufferType * mediaBufferPtr;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&ExSrc_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSrc_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            /* Complete mediabuffers that are currently processed and move to Ready state */
            ExSrc_Comp.state = EXSRC_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Complete mediabuffers that are currently processed and move to Ready state */
            ExSrc_Comp.state = EXSRC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }

        /* Mark the processed command as complete */
        retVal = MSGQ_CompleteMsgIdx(&ExSrc_CmdQ, cmdQIdx);
    }

    /* If we are still in the running state, send MSYS_FillBuffer() requests to
     * source component.
     */
    if (EXSRC_STATE_RUNNING == ExSrc_Comp.state) {
        localAlarmVal = ATOMIC_Set(&AlarmVal, 0UL);
        while (localAlarmVal > 0UL) {
            uint32_t mediaBufIdx;
            retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort.dataQ, &mediaBufIdx);
            if (retVal != BCM_ERR_OK) {
                break;
            }
            mediaBufferPtr = &ExSrc_MediaBuffer[mediaBufIdx];

            /* validate mediabuffer */
            if ((NULL != mediaBufferPtr->buffer[0].data) && (0UL != mediaBufferPtr->buffer[0].size)){
                /* Fill the buffer */
                txData[txPktCount++] = Buffer[ExSrc_idx];
                mediaBufferPtr->buffer[0].data[0UL] = Buffer[ExSrc_idx++];
                if (ExSrc_idx >= 27) {
                    ExSrc_idx = 0;
                }
                mediaBufferPtr->buffer[0].size = 1UL;
                mediaBufferPtr->response = BCM_ERR_OK;
            } else {
                mediaBufferPtr->response = BCM_ERR_INVAL_PARAMS;
            }

            retVal = MSGQ_CompleteMsgIdx(&ExSrc_OutPort.dataQ, mediaBufIdx);
        }
    }
}

TASK(ExSrc_Task)
{
    BCM_EventMaskType eventMask = 0UL;

    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        (void)BCM_WaitEvent(EXSRC_CMDQ_EVENT | EXSRC_INPORT_EVENTS);
        (void)BCM_GetEvent(ExSrc_Task, &eventMask);
        (void)BCM_ClearEvent(eventMask);

        /* Call the state handling helper function depending on the state that
         * this sink component is in.
         */
        switch(ExSrc_Comp.state) {
        case EXSRC_STATE_RESET:
            ExSrc_ResetStateHandler();
            break;
        case EXSRC_STATE_INIT:
            ExSrc_InitStateHandler();
            break;
        case EXSRC_STATE_READY:
            ExSrc_ReadyStateHandler();
            break;
        default: /* EXSRC_STATE_RUNNING */
            ExSrc_RunStateHandler();
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (EXSRC_STATE_RESET != ExSrc_Comp.state);

    (void)BCM_TerminateTask();
}
