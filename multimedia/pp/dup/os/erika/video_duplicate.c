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
File Name  : src_comp.c
Description: This file contains a sample source component that can be used as a
             reference.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO

#include <component.h>
#include <media_port.h>
#include <msg_queue.h>
#include <ulog.h>
#include <bcm_utils.h>
#include <ee.h>
#include <video_dup.h>
#include <pp.h>
#include <osil/bcm_osil.h>

/******************************************************************************
 * Data specific to sink component
 *****************************************************************************/
/**
 * Sink Component State Machine.
 * Sink component shall be in VDUP_STATE_RESET by default. The state machine
 * for sink component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * VDUP_STATE_RESET    MSYS_COMP_CMD_INIT       VDUP_STATE_INIT
 * VDUP_STATE_INIT     MSYS_COMP_CMD_CONFIG     VDUP_STATE_READY
 * VDUP_STATE_READY    MSYS_COMP_CMD_CONFIG     VDUP_STATE_READY
 * VDUP_STATE_READY    MSYS_COMP_CMD_START      VDUP_STATE_RUNNING
 * VDUP_STATE_READY    MSYS_COMP_CMD_DEINIT     VDUP_STATE_RESET
 * VDUP_STATE_RUNNING  MSYS_COMP_CMD_STOP       VDUP_STATE_READY
 * VDUP_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     VDUP_STATE_RESET
 */
typedef uint32_t VDUP_StateType;
#define VDUP_STATE_RESET   (0UL)   /**< Intermediate component Reset state */
#define VDUP_STATE_INIT    (1UL)   /**< Intermediate component Init state */
#define VDUP_STATE_READY   (2UL)   /**< Intermediate component Ready state */
#define VDUP_STATE_RUNNING (3UL)   /**< Intermediate component Running state */

/* Sink component even masks */
/** Event mask for Sink component's command Queue */
#define VDUP_CMDQ_EVENT        SystemEvent0

/** Event mask for all the input ports for Sink component */
#define VDUP_PORT_EVENTS     (SystemEvent4 | SystemEvent8 | SystemEvent9)

/** Maximum number of Channels for Sink component's input port */
#define VDUP_CHANNEL_MAX       (3UL)

/** Structure to hold component information */
typedef struct _VDUP_CompType {
    VDUP_StateType state;
    uint32_t frameAssembly;
    uint32_t stopInProgress;
} VDUP_CompType;

static VDUP_CompType VDUP_Comp = {
    .state = VDUP_STATE_RESET,
    .frameAssembly = 0UL,
    .stopInProgress = 0UL,
};
typedef struct _VDUP_BufInfoType
{
    uint32_t mbRow;
    uint32_t bufIdx;
    uint32_t job[2UL];
    uint32_t nJobs[2UL];
    uint32_t frameNum;
    uint16_t frameStart;
    uint16_t frameEnd;
} VDUP_BufInfoType;

#define INIT_VDUP_BUF_INFO      \
{                               \
    .mbRow = 0UL,               \
    .bufIdx = 0UL,              \
    .job[0UL] = 0xFFFFFFFFUL,   \
    .job[1UL] = 0xFFFFFFFFUL,   \
    .nJobs[0UL] = 0UL,          \
    .nJobs[1UL] = 1UL,          \
};

#define VDUP_CIRCQ_LOG2_SIZE    (4UL)
CIRCQ_V2_DEFINE(VDUP_FillQ, VDUP_CIRCQ_LOG2_SIZE, uint32_t,
                        VDUP_FillQBuf, VDUP_FillQIdx, CIRCQ_OVERWRITE_DISABLE, ".data");
CIRCQ_V2_DEFINE(VDUP_ProcQ, VDUP_CIRCQ_LOG2_SIZE, uint32_t,
                        VDUP_ProcQBuf, VDUP_ProcQIdx, CIRCQ_OVERWRITE_DISABLE, ".data");
VDUP_BufInfoType VDUP_BufInfo[1 << VDUP_CIRCQ_LOG2_SIZE];
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
MSGQ_DEFINE_HDRQ(VDUP_CmdMsgHdrQ);


/** Array for commands to Sink component */
MSYS_CmdType VDUP_CmdMsg[MSGQ_SIZE];

/** GetMessage function to get the ith message */
void * VDUP_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &VDUP_CmdMsg[i];
    }

    return msg;
}

/* Define the command queue for the sink component, on which framework shall
 * post the different commands for the sink component.
 */
MSGQ_DEFINE (VDUP_CmdQ, VDUP_Task, VDUP_CMDQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType,
             &VDUP_CmdMsgHdrQ, VDUP_GetCmdMsg);

/* Framework shall use this API to post a command to this sink component */
static int32_t VDUP_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;

    if ((NULL != aCmd) && (MSYS_COMP_ID_VDUP == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the sink component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            BCM_ActivateTask(VDUP_Task);
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&VDUP_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the response of a command that was
 * posted to this Sink component
 */
static int32_t VDUP_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Sink component's command queue */
        retVal = MSGQ_RecvMsg(&VDUP_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

#define VDUP_MAX_NUM_OUT_PORT (2UL) /* Number of output ports */

/* Framework shall use this API to get the number of output ports for this sink
 * component.
 */
static uint32_t VDUP_GetNumMediaOutPort(void)
{
    /* Since this is a sink component, there are no output ports */
    return VDUP_MAX_NUM_OUT_PORT;
}


static MSYS_BufferType VDUP_MediaBuffer[VDUP_MAX_NUM_OUT_PORT][MSGQ_SIZE];

void *VDUP_GetDataMsg0(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &VDUP_MediaBuffer[0UL][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(VDUP_OutPort0, 0UL, VDUP_DataMsgHdrQ0,
                        VDUP_Task, VDUP_GetDataMsg0,
                        MSYS_COMP_ID_VDUP, SystemEvent8);

void *VDUP_GetDataMsg1(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &VDUP_MediaBuffer[1UL][i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(VDUP_OutPort1, 1UL, VDUP_DataMsgHdrQ1,
                        VDUP_Task, VDUP_GetDataMsg1,
                        MSYS_COMP_ID_VDUP, SystemEvent9);

const MSYS_OutPortType * VDUP_OutPort[VDUP_MAX_NUM_OUT_PORT] = {
    &VDUP_OutPort0,
    &VDUP_OutPort1,
};

/* Framework shall use this API to get the MSYS_OutPortType of this component where
 * MSYS_FillBuffer() request shall be posted (if any).
 */
static const MSYS_OutPortType* VDUP_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port = NULL;
    /* Since this is sink component there are no output ports */
    switch (aIndex) {
    case 0UL:
        port = &VDUP_OutPort0;
        break;
    case 1UL:
        port = &VDUP_OutPort1;
        break;
    default:
        break;
    }
    return port;
}

static const MSYS_InPortType* VDUP_GetMediaInPort(const uint32_t aIndex);

/* Component function table for lens distortion correction */
static const MSYS_CompFnTblType VDUP_CompFnTable =
{
    VDUP_SendCmd,
    VDUP_RecvResp,
    VDUP_GetMediaOutPort,
    VDUP_GetMediaInPort,
};

const MSYS_CompFnTblType* VDUP_GetCompFnTable(void)
{
    return &VDUP_CompFnTable;
};

/******************************************************************************
 * Sink component <--> Other component interaction
 * Interfaces for component<-->component interactions are defined in
 * media_port.h. Components work in pull model, where the sink component raises
 * a MSYS_FillBuffer() request to the framework which shall route the request to the
 * corresponding source component's output port. Note that any output<-->input
 * port mapping shall be provided by the Application that uses the components.
 *****************************************************************************/

/* declare YUV buffers */
#ifdef VDUP_ENABLE_FRAME_MODE
#define VDUP_HEIGHT     (MEDIA_SYSTEM_MAX_VIDEO_HEIGHT)
#else
#define VDUP_HEIGHT     (16UL)
#endif
#define VDUP_NUM_BUFFERS    (3UL)
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.VDUP_YUVData") VDUP_YUVData[VDUP_NUM_BUFFERS][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * VDUP_HEIGHT * 3UL / 2UL];

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.VDUP_UVData") VDUP_UVData[MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 4UL];

static void VDUP_FillBuffer(uint32_t aIdx);

/* This callback shall be invoked from framework to this Sink component to
 * notify completion of buffer.
 */
static void VDUP_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    if (NULL != aMediaBuffer) {
        if ((VDUP_CHANNEL_MAX == aMediaBuffer->numChannels)
            && (BCM_ERR_OK == aMediaBuffer->response)
            && (0UL != aMediaBuffer->buffer[0UL].size)) {
            uint32_t bufIdx = aMediaBuffer->bufIdx;
            VDUP_BufInfo[bufIdx].mbRow = aMediaBuffer->seqNum;
            VDUP_BufInfo[bufIdx].bufIdx = aMediaBuffer->bufIdx;
            VDUP_BufInfo[bufIdx].frameNum = aMediaBuffer->frameNum;
            VDUP_BufInfo[bufIdx].frameStart = aMediaBuffer->frameStart;
            VDUP_BufInfo[bufIdx].frameEnd = aMediaBuffer->frameEnd;
            VDUP_BufInfo[bufIdx].job[0UL] = 0xFFFFFFFFUL;
            VDUP_BufInfo[bufIdx].job[1UL] = 0xFFFFFFFFUL;
#ifdef VDUP_ENABLE_FRAME_MODE
            VDUP_BufInfo[bufIdx].nJobs[0UL] = (MEDIA_SYSTEM_MAX_VIDEO_HEIGHT >> 4UL);
#else
            VDUP_BufInfo[bufIdx].nJobs[0UL] = 1UL;
#endif
            VDUP_BufInfo[bufIdx].nJobs[1UL] = 1UL;
            CIRCQ_Push(&VDUP_FillQ, (const char*)&bufIdx, 1UL);
        } else {
            VDUP_FillBuffer(aMediaBuffer->bufIdx);
        }
    }
}

/* Define input port 0 for this sink component (this component uses only one
 * input port
 */
MSYS_DECLARE_IN_PORT(VDUP_InPort, 0UL, MSYS_COMP_ID_VDUP, SystemEvent4,
                      VDUP_InPortRW, VDUP_FillBufferDone);

/** @brief Get the MSYS_InPortType where the message needs to be posted to
 *
 * @details Get the MSYS_InPortType where the message needs to be posted to
 * (triggered when the sink component requests this component to fill the media
 *  buffer)
 *
 * @param[in]   aIndex          Port Index
 *
 * @retval      NULL aIndex is not valid
 * @retval      pointer to the media inport
 */
static const MSYS_InPortType* VDUP_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        inPort = &VDUP_InPort;
        break;
    default:
        break;
    }

    return inPort;
}

static void VDUP_FillBuffer(uint32_t aIdx)
{
    if (0UL == VDUP_Comp.stopInProgress) {
        /*TODO Validate timestamp  received from the source component */
        const MSYS_BufferType mediaBuffer = {
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
            .numChannels = VDUP_CHANNEL_MAX,
            .response = BCM_ERR_UNKNOWN,
            .seqNum = 0UL,
            .bufIdx = aIdx,

            .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
            .buffer[0].size = MEDIA_SYSTEM_MAX_VIDEO_WIDTH * VDUP_HEIGHT,
            .buffer[0].data = &VDUP_YUVData[aIdx][0UL],
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = VDUP_HEIGHT,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH,

            .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
            .buffer[1].size = (MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL) * (VDUP_HEIGHT / 2UL),
            .buffer[1].data = &VDUP_YUVData[aIdx][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * VDUP_HEIGHT],
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = VDUP_HEIGHT / 2UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL,

            .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
            .buffer[2].size = (MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL) * (VDUP_HEIGHT / 2UL),
            .buffer[2].data = &VDUP_YUVData[aIdx][(MEDIA_SYSTEM_MAX_VIDEO_WIDTH * VDUP_HEIGHT) + ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL) * (VDUP_HEIGHT / 2UL))],
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = VDUP_HEIGHT / 2UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL,
        };
        (void)MSYS_FillBuffer(&VDUP_InPort, &mediaBuffer);
    }
}

/******************************************************************************
 * Sink component Functionality.
 *
 * Sink component uses a single task, that shall be the server task for the
 * framework<-->component message queue. The same task handles all interaction
 * with other components as well.
 *****************************************************************************/

/* Helper function that handles all transitions from Reset state */
static void VDUP_ResetStateHandler()
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&VDUP_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VDUP_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
            VDUP_Comp.frameAssembly = 0UL;
            VDUP_Comp.stopInProgress = 0UL;
            BCM_MemSet(VDUP_UVData, 0x80, sizeof(VDUP_UVData));
            VDUP_Comp.state = VDUP_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        retVal = MSGQ_CompleteMsgIdx(&VDUP_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Init state */
static void VDUP_InitStateHandler()
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&VDUP_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VDUP_CmdMsg[cmdQIdx];
        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            VDUP_Comp.state = VDUP_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component.
             */
            VDUP_Comp.state = VDUP_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&VDUP_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Ready state */
static void VDUP_ReadyStateHandler()
{
    uint32_t i;
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&VDUP_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VDUP_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_START:
            VDUP_Comp.state = VDUP_STATE_RUNNING;
            for (i = 0UL; i < VDUP_NUM_BUFFERS; i++) {
                VDUP_FillBuffer(i);
            }
            /* Signal the input port of this Sink component to start processing */
            BCM_SetEvent(VDUP_Task, VDUP_PORT_EVENTS);
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            VDUP_Comp.state = VDUP_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&VDUP_CmdQ, cmdQIdx);
    }
}

/* Helper function that handles all transitions from Running state */
static void VDUP_RunStateHandler()
{
    int32_t retVal;
    uint32_t cmdQIdx;
    uint32_t isEmpty;
    uint32_t cnt;
    uint32_t val = 0UL;
    uint32_t mediaBufIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&VDUP_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VDUP_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            VDUP_Comp.stopInProgress = 1UL;
            break;
        case MSYS_COMP_CMD_DEINIT:
            VDUP_Comp.stopInProgress = 1UL;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            /* Mark the processed command as complete */
            retVal = MSGQ_CompleteMsgIdx(&VDUP_CmdQ, cmdQIdx);
            break;
        }

    }

    /* Process any filled mediabuffers available at the input port */
    retVal = MSYS_InPortProcess(&VDUP_InPort);

    cnt = CIRCQ_Peek(&VDUP_FillQ, (char*)&val, 1UL);
    if (1UL == cnt) {
        if (VDUP_BufInfo[val].job[0UL] != 0xFFFFFFFFUL) {
            retVal = PP_DMAStatus(VDUP_BufInfo[val].job[0UL]);
            if (BCM_ERR_OK == retVal) {
                VDUP_BufInfo[val].job[0UL] = 0xFFFFFFFFUL;
#ifdef VDUP_ENABLE_FRAME_MODE
                VDUP_BufInfo[val].frameEnd = (0UL == VDUP_BufInfo[val].nJobs[0UL]) ? 0U : 1U;
#endif
                retVal = MSGQ_GetMsgIdx(&VDUP_OutPort[0UL]->dataQ, &mediaBufIdx);
                if (BCM_ERR_OK == retVal) {
                    VDUP_MediaBuffer[0UL][mediaBufIdx].seqNum = VDUP_BufInfo[val].mbRow;
                    VDUP_MediaBuffer[0UL][mediaBufIdx].frameNum = VDUP_BufInfo[val].frameNum;
                    VDUP_MediaBuffer[0UL][mediaBufIdx].frameStart = VDUP_BufInfo[val].frameStart;
                    VDUP_MediaBuffer[0UL][mediaBufIdx].frameEnd = VDUP_BufInfo[val].frameEnd;
                    VDUP_MediaBuffer[0UL][mediaBufIdx].response = BCM_ERR_OK;
                    retVal = MSGQ_CompleteMsgIdx(&VDUP_OutPort[0UL]->dataQ, mediaBufIdx);
                }
#ifdef VDUP_ENABLE_FRAME_MODE
                VDUP_BufInfo[val].mbRow++;
                VDUP_BufInfo[val].frameStart = 0U;
#endif
            }
        }
        if (VDUP_BufInfo[val].job[1UL] != 0xFFFFFFFFUL) {
            retVal = PP_DMAStatus(VDUP_BufInfo[val].job[1UL]);
            if (BCM_ERR_OK == retVal) {
                VDUP_BufInfo[val].job[1UL] = 0xFFFFFFFFUL;
#ifdef VDUP_ENABLE_FRAME_MODE
                retVal = MSGQ_GetMsgIdx(&VDUP_OutPort[1UL]->dataQ, &mediaBufIdx);
                if (BCM_ERR_OK == retVal) {
                    VDUP_MediaBuffer[1UL][mediaBufIdx].frameNum = VDUP_BufInfo[val].frameNum;
                    VDUP_MediaBuffer[1UL][mediaBufIdx].frameStart = 1U;
                    VDUP_MediaBuffer[1UL][mediaBufIdx].frameEnd = 1U;
                    VDUP_MediaBuffer[1UL][mediaBufIdx].response = BCM_ERR_OK;
                    retVal = MSGQ_CompleteMsgIdx(&VDUP_OutPort[1UL]->dataQ, mediaBufIdx);
                }
#else
                if (VDUP_BufInfo[val].mbRow == ((MEDIA_SYSTEM_MAX_VIDEO_HEIGHT >> 4UL) - 1UL)) {
                    retVal = MSGQ_GetMsgIdx(&VDUP_OutPort[1UL]->dataQ, &mediaBufIdx);
                    if (BCM_ERR_OK == retVal) {
                        VDUP_MediaBuffer[1UL][mediaBufIdx].frameNum = VDUP_BufInfo[val].frameNum;
                        VDUP_MediaBuffer[1UL][mediaBufIdx].frameStart = 1U;
                        VDUP_MediaBuffer[1UL][mediaBufIdx].frameEnd = 1U;
                        VDUP_MediaBuffer[1UL][mediaBufIdx].response = BCM_ERR_OK;
                        retVal = MSGQ_CompleteMsgIdx(&VDUP_OutPort[1UL]->dataQ, mediaBufIdx);
                    }
                }
#endif
            }
        }
    }

    cnt = CIRCQ_Peek(&VDUP_FillQ, (char*)&val, 1UL);
    if (1UL == cnt) {
        if ((0UL == VDUP_BufInfo[val].nJobs[0UL]) &&
            (0xFFFFFFFFUL == VDUP_BufInfo[val].job[0UL]) &&
            (0xFFFFFFFFUL == VDUP_BufInfo[val].job[1UL])) {
            /* All the required number of jobs are submitted and are completed */
            /* dont worry about port 1 */
            /* So free the buffer */
            cnt = CIRCQ_Pop(&VDUP_FillQ, (char*)&val, 1UL);
            VDUP_FillBuffer(val);
        }

    }
    cnt = CIRCQ_Peek(&VDUP_FillQ, (char*)&val, 1UL);
    if (1UL == cnt) {
        PP_DMABufferType inBuffer;
        PP_DMABufferType outBuffer;

        retVal = MSGQ_GetMsgIdx(&VDUP_OutPort[0UL]->dataQ, &mediaBufIdx);
        if ((retVal == BCM_ERR_OK) && (VDUP_BufInfo[val].nJobs[0UL] > 0UL) && (0xFFFFFFFFUL == VDUP_BufInfo[val].job[0UL])) {
            inBuffer.numCH = 3;
            inBuffer.channel[0].addr = &VDUP_YUVData[val][0];
            inBuffer.channel[0].width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            inBuffer.channel[0].height = 16UL;
            inBuffer.channel[0].stride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            inBuffer.channel[1].addr = &VDUP_YUVData[val][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * VDUP_HEIGHT];
            inBuffer.channel[1].width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
            inBuffer.channel[1].height = 8UL;
            inBuffer.channel[1].stride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
            inBuffer.channel[2].addr = &VDUP_YUVData[val][(MEDIA_SYSTEM_MAX_VIDEO_WIDTH * VDUP_HEIGHT) + ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL) * (VDUP_HEIGHT / 2UL))];
            inBuffer.channel[2].width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
            inBuffer.channel[2].height = 8UL;
            inBuffer.channel[2].stride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
#ifdef VDUP_ENABLE_FRAME_MODE
            inBuffer.channel[0].addr += MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL * VDUP_BufInfo[val].mbRow;
            inBuffer.channel[1].addr = &VDUP_UVData[0UL];
            inBuffer.channel[2].addr = &VDUP_UVData[0UL];
#endif
            outBuffer.numCH = 3;
            outBuffer.channel[0].width = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
            outBuffer.channel[0].height = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
            outBuffer.channel[0].stride = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
            outBuffer.channel[0].addr = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[0UL].data;
            outBuffer.channel[1].width = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
            outBuffer.channel[1].height = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
            outBuffer.channel[1].stride = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
            outBuffer.channel[1].addr = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[1UL].data;
            outBuffer.channel[2].width = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[2UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
            outBuffer.channel[2].height = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[2UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
            outBuffer.channel[2].stride = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[2UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
            outBuffer.channel[2].addr = VDUP_MediaBuffer[0UL][mediaBufIdx].buffer[2UL].data;

            retVal = PP_DMAStart(&inBuffer, &outBuffer, &VDUP_BufInfo[val].job[0UL]);
            if (BCM_ERR_OK == retVal) {
                VDUP_BufInfo[val].nJobs[0UL]--;
            }
        }

        retVal = MSGQ_GetMsgIdx(&VDUP_OutPort[1UL]->dataQ, &mediaBufIdx);
        if ((retVal == BCM_ERR_OK) && (VDUP_BufInfo[val].nJobs[1UL] > 0UL) && (0xFFFFFFFFUL == VDUP_BufInfo[val].job[1UL])) {
            inBuffer.numCH = 3;
            inBuffer.channel[0].addr = &VDUP_YUVData[val][0];
            inBuffer.channel[0].width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            inBuffer.channel[0].height = VDUP_HEIGHT;
            inBuffer.channel[0].stride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            inBuffer.channel[1].addr = &VDUP_YUVData[val][MEDIA_SYSTEM_MAX_VIDEO_WIDTH*VDUP_HEIGHT];
            inBuffer.channel[1].width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
            inBuffer.channel[1].height = VDUP_HEIGHT / 2UL;
            inBuffer.channel[1].stride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
            inBuffer.channel[2].addr = &VDUP_YUVData[val][(MEDIA_SYSTEM_MAX_VIDEO_WIDTH*VDUP_HEIGHT) + ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL) * (VDUP_HEIGHT / 2UL))];
            inBuffer.channel[2].width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
            inBuffer.channel[2].height = VDUP_HEIGHT / 2UL;
            inBuffer.channel[2].stride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2;
            outBuffer.numCH = 3;
            outBuffer.channel[0].width = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
            outBuffer.channel[0].height = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
            outBuffer.channel[0].stride = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
            outBuffer.channel[0].addr = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[0UL].data;
            outBuffer.channel[1].width = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
            outBuffer.channel[1].height = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
            outBuffer.channel[1].stride = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
            outBuffer.channel[1].addr = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[1UL].data;
            outBuffer.channel[2].width = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[2UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
            outBuffer.channel[2].height = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[2UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
            outBuffer.channel[2].stride = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[2UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
            outBuffer.channel[2].addr = VDUP_MediaBuffer[1UL][mediaBufIdx].buffer[2UL].data;
#ifndef VDUP_ENABLE_FRAME_MODE
            outBuffer.channel[0].addr += (outBuffer.channel[0].stride * VDUP_HEIGHT * VDUP_BufInfo[val].mbRow);
            outBuffer.channel[1].addr += (outBuffer.channel[1].stride * (VDUP_HEIGHT / 2UL) * VDUP_BufInfo[val].mbRow);
            outBuffer.channel[2].addr += (outBuffer.channel[2].stride * (VDUP_HEIGHT / 2UL) * VDUP_BufInfo[val].mbRow);
#endif
            retVal = PP_DMAStart(&inBuffer, &outBuffer, &VDUP_BufInfo[val].job[1UL]);
            if (BCM_ERR_OK == retVal) {
                VDUP_BufInfo[val].nJobs[1UL]--;
            }
        }
    }

    if (VDUP_Comp.stopInProgress == 1UL) {
        retVal = CIRCQ_IsEmpty(&VDUP_FillQ, &isEmpty);
        if (isEmpty == TRUE) {
            /* Cancel all mediabuffers and move to Ready state */
            MSYS_InPortCancelAll(&VDUP_InPort);
            VDUP_Comp.state = VDUP_STATE_READY;
            VDUP_Comp.stopInProgress = 0UL;
            processCmd->response = BCM_ERR_OK;
            /* Mark the processed command as complete */
            retVal = MSGQ_CompleteMsgIdx(&VDUP_CmdQ, cmdQIdx);
        }
    }
}

TASK(VDUP_Task)
{
    BCM_EventMaskType eventMask = 0UL;

    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        BCM_WaitEvent(VDUP_CMDQ_EVENT | VDUP_PORT_EVENTS | PP_GPEvent);
        BCM_GetEvent(VDUP_Task, &eventMask);
        BCM_ClearEvent(eventMask);

        /* Call the state handling helper function depending on the state that
         * this sink component is in.
         */
        switch(VDUP_Comp.state)
        {
        case VDUP_STATE_RESET:
            VDUP_ResetStateHandler();
            break;
        case VDUP_STATE_INIT:
            VDUP_InitStateHandler();
            break;
        case VDUP_STATE_READY:
            VDUP_ReadyStateHandler();
            break;
        case VDUP_STATE_RUNNING:
            VDUP_RunStateHandler();
            break;
        default:
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (VDUP_STATE_RESET != VDUP_Comp.state);

    BCM_TerminateTask();
}
