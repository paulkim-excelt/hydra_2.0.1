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
#include <ee.h>
#include <ex_sink_comp.h>
#include <osil/bcm_osil.h>

/******************************************************************************
 * Data specific to sink component
 *****************************************************************************/
/**
 * Sink Component State Machine.
 * Sink component shall be in EXSNK_STATE_RESET by default. The state machine
 * for sink component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * EXSNK_STATE_RESET    MSYS_COMP_CMD_INIT       EXSNK_STATE_INIT
 * EXSNK_STATE_INIT     MSYS_COMP_CMD_CONFIG     EXSNK_STATE_READY
 * EXSNK_STATE_READY    MSYS_COMP_CMD_CONFIG     EXSNK_STATE_READY
 * EXSNK_STATE_READY    MSYS_COMP_CMD_START      EXSNK_STATE_RUNNING
 * EXSNK_STATE_READY    MSYS_COMP_CMD_DEINIT     EXSNK_STATE_RESET
 * EXSNK_STATE_RUNNING  MSYS_COMP_CMD_STOP       EXSNK_STATE_READY
 * EXSNK_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     EXSNK_STATE_RESET
 */
typedef uint32_t ExSnk_StateType;
#define EXSNK_STATE_RESET   (0UL)   /**< Sink component Reset state */
#define EXSNK_STATE_INIT    (1UL)   /**< Sink component Init state */
#define EXSNK_STATE_READY   (2UL)   /**< Sink component Ready state */
#define EXSNK_STATE_RUNNING (3UL)   /**< Sink component Running state */

/* Sink component even masks */
/** Event mask for Sink component's command Queue */
#define EXSNK_CMDQ_EVENT        SystemEvent0

/** Event mask for all the input ports for Sink component */
#define EXINT_PORT_EVENTS     (SystemEvent4)

/** Maximum number of Channels for Sink component's input port */
#define EXSNK_CHANNEL_MAX       (1UL)

extern uint8_t rxData[1000];
extern uint32_t rxPktCount;
/** Structure to hold Sink component information */
typedef struct _ExSnk_CompType {
    ExSnk_StateType state;
    uint32_t fillReqCnt;    /**< Number of MSYS_FillBuffer requests made */
    uint32_t fillDoneCnt;   /**< Number of MSYS_FillBuffer requests serviced successfully */
    uint32_t fillCnclCnt;   /**< Number of MSYS_FillBuffer requests cancelled */
    uint32_t nextBufIdx;    /**< Index to the next buffer to be used */
    uint8_t  buffers[16UL];/**< Character buffer for output */
} ExSnk_CompType;

static ExSnk_CompType ExSnk_Comp = {
    .state = EXSNK_STATE_RESET,
    .fillReqCnt = 0UL,
    .fillDoneCnt = 0UL,
    .fillCnclCnt = 0UL,
    .nextBufIdx = 0UL,
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
MSGQ_DEFINE_HDRQ(ExSnk_CmdMsgHdrQ);

/** Array for commands to Sink component */
static MSYS_CmdType ExSnk_CmdMsg[MSGQ_SIZE];

/** GetMessage function to get the ith message */
static void * ExSnk_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &ExSnk_CmdMsg[i];
    }

    return msg;
}

/* Define the command queue for the sink component, on which framework shall
 * post the different commands for the sink component.
 */
MSGQ_DEFINE (ExSnk_CmdQ, ExSnk_Task, EXSNK_CMDQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType,
             &ExSnk_CmdMsgHdrQ, ExSnk_GetCmdMsg);

/* Framework shall use this API to post a command to this sink component */
int32_t ExSnk_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;

    if ((NULL != aCmd) && (MSYS_COMP_ID_EX_SNK == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the sink component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            (void)BCM_ActivateTask(ExSnk_Task);
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&ExSnk_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the response of a command that was
 * posted to this Sink component
 */
int32_t ExSnk_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Sink component's command queue */
        retVal = MSGQ_RecvMsg(&ExSnk_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the MSYS_OutPortType of this component where
 * MSYS_FillBuffer() request shall be posted (if any).
 */
const MSYS_OutPortType* ExSnk_GetOutPort(const uint32_t aIndex)
{
    /* Since this is sink component there are no output ports */
    return NULL;
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
static void ExSnk_FillBufferDone(const MSYS_InPortType *const aPort,
                                    MSYS_BufferType *const aMediaBuffer)
{
    if ((NULL != aMediaBuffer)
        && (EXSNK_CHANNEL_MAX == aMediaBuffer->numChannels)) {
        if (BCM_ERR_OK == aMediaBuffer->response) {
            if ((0UL != aMediaBuffer->buffer[0UL].size)) {
                /*TODO: Output the completed buffer to the console or something */
	            rxData[rxPktCount++] =  aMediaBuffer->buffer[0UL].data[0UL];
                ExSnk_Comp.fillDoneCnt++;
            } else {
                /* Buffer size 0 implies that this buffer was cancelled */
                ExSnk_Comp.fillCnclCnt++;
            }
        }
    }
}

/* Define input port 0 for this sink component (this component uses only one
 * input port
 */
MSYS_DECLARE_IN_PORT_V2(ExSnk_InPort, 0UL, MSYS_COMP_ID_EX_SNK, SystemEvent4,
                         ExSnk_InPortRW, ExSnk_FillBufferDone, ".data.ExSnk_InPortRW");

/* Framework shall use this API to get the MSYS_InPortType of this component */
const MSYS_InPortType* ExSnk_GetInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *port;
    /* Since this is sink component there are no output ports */
    switch (aIndex) {
    case 0UL:
        port = &ExSnk_InPort;
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
static void ExSnk_ResetStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&ExSnk_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSnk_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
            (void)MSYS_InPortInit(&ExSnk_InPort);
            ExSnk_Comp.state = EXSNK_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        (void)MSGQ_CompleteMsgIdx(&ExSnk_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Init state */
static void ExSnk_InitStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&ExSnk_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSnk_CmdMsg[cmdQIdx];
        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            ExSnk_Comp.state = EXSNK_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component.
             */
            ExSnk_Comp.state = EXSNK_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        (void)MSGQ_CompleteMsgIdx(&ExSnk_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Ready state */
static void ExSnk_ReadyStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&ExSnk_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSnk_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_START:
            ExSnk_Comp.state = EXSNK_STATE_RUNNING;
            /* Signal the input port of this Sink component to start processing */
            (void)BCM_SetEvent(ExSnk_Task, EXINT_PORT_EVENTS);
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            ExSnk_Comp.state = EXSNK_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        (void)MSGQ_CompleteMsgIdx(&ExSnk_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Running state */
static void ExSnk_RunStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    MSYS_BufferType mediaBuffer;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&ExSnk_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSnk_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            /* Cancel all mediabuffers and move to Ready state */
            (void)MSYS_InPortCancelAll(&ExSnk_InPort);
            ExSnk_Comp.state = EXSNK_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Cancell all mediabuffers and move to Reset state */
            (void)MSYS_InPortCancelAll(&ExSnk_InPort);
            ExSnk_Comp.state = EXSNK_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }

        /* Mark the processed command as complete */
        (void)MSGQ_CompleteMsgIdx(&ExSnk_CmdQ, cmdQIdx);
    }

    /* If we are still in the running state, send MSYS_FillBuffer() requests to
     * source component.
     */
    if (EXSNK_STATE_RUNNING == ExSnk_Comp.state) {
        do {
            /*TODO Validate timestamp  received from the source component */
            mediaBuffer.timestamp = 0ULL;
            mediaBuffer.buffer[0].data = &ExSnk_Comp.buffers[(ExSnk_Comp.nextBufIdx++) & 0xFUL];
            mediaBuffer.buffer[0].size = 1UL;
            mediaBuffer.numChannels = EXSNK_CHANNEL_MAX;
            mediaBuffer.response = BCM_ERR_UNKNOWN;
            mediaBuffer.bufIdx = 0UL;

            retVal = MSYS_FillBuffer(&ExSnk_InPort, &mediaBuffer);
            if (retVal == BCM_ERR_OK) {
                ExSnk_Comp.fillReqCnt++;
            }
        } while (BCM_ERR_OK == retVal);

        /* Process any filled mediabuffers available at the input port */
        (void)MSYS_InPortProcess(&ExSnk_InPort);
    }
}

TASK(ExSnk_Task)
{
    BCM_EventMaskType eventMask;

    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        (void)BCM_WaitEvent(EXSNK_CMDQ_EVENT | EXINT_PORT_EVENTS);
        (void)BCM_GetEvent(ExSnk_Task, &eventMask);
        (void)BCM_ClearEvent(eventMask);

        /* Call the state handling helper function depending on the state that
         * this sink component is in.
         */
        switch(ExSnk_Comp.state)
        {
        case EXSNK_STATE_RESET:
            ExSnk_ResetStateHandler();
            break;
        case EXSNK_STATE_INIT:
            ExSnk_InitStateHandler();
            break;
        case EXSNK_STATE_READY:
            ExSnk_ReadyStateHandler();
            break;
        default: /*EXSNK_STATE_RUNNING */
            ExSnk_RunStateHandler();
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (EXSNK_STATE_RESET != ExSnk_Comp.state);

    (void)BCM_TerminateTask();
}
