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
File Name  : audio_record.c
Description: This file contains implementation of audio record component.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO

#include <component.h>
#include <media_port.h>
#include <msg_queue.h>
#include <ulog.h>
#include <atomic.h>
#include <ee.h>
#include <audio_record.h>
#include <audio.h>
#include <osil/audio_osil.h>
#include <bcm_utils.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>

/* Audio record component internal API IDs for error reporting */
/** AudioRecord_ProcessMediaPort API ID */
#define AUDIO_REC_PROC_MEDIA_PORT           (0x80U)
/** AudioRecord_ResetStateHandler API ID */
#define AUDIO_REC_RESET_STATE_HANDLER       (0x81U)
/** AudioRecord_InitStateHandler API ID */
#define AUDIO_REC_INIT_STATE_HANDLER        (0x82U)
/** AudioRecord_ReadyStateHandler API ID */
#define AUDIO_REC_READY_STATE_HANDLER       (0x83U)
/** AudioRecord_RunStateHandler API ID */
#define AUDIO_REC_RUN_STATE_HANDLER         (0x84U)
/** AudioRecord Task API ID */
#define AUDIO_REC_TASK                      (0x85U)
/** AudioRecord_SendCmd API ID */
#define AUDIO_REC_SEND_CMD                  (0x86U)

/******************************************************************************
 * Data specific to source component
 *****************************************************************************/
/**
 * Source Component State Machine.
 * Source component shall be in AREC_STATE_RESET by default. The state machine
 * for sink component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * AREC_STATE_RESET    MSYS_COMP_CMD_INIT       AREC_STATE_INIT
 * AREC_STATE_INIT     MSYS_COMP_CMD_CONFIG     AREC_STATE_READY
 * AREC_STATE_READY    MSYS_COMP_CMD_CONFIG     AREC_STATE_READY
 * AREC_STATE_READY    MSYS_COMP_CMD_START      AREC_STATE_RUNNING
 * AREC_STATE_READY    MSYS_COMP_CMD_DEINIT     AREC_STATE_RESET
 * AREC_STATE_RUNNING  MSYS_COMP_CMD_STOP       AREC_STATE_READY
 * AREC_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     AREC_STATE_RESET
 */
typedef uint32_t AudioRecord_StateType;
#define AREC_STATE_RESET   (0UL)   /**< Source component Reset state */
#define AREC_STATE_INIT    (1UL)   /**< Source component Init state */
#define AREC_STATE_READY   (2UL)   /**< Source component Ready state */
#define AREC_STATE_RUNNING (3UL)   /**< Source component Running state */

typedef uint32_t AudioRecord_PortNumType;

typedef uint32_t AudioRecord_PortMaskType;

/** Maximum number of output ports supported by this component */
#define AREC_OUTPORT_MAX  (3UL)


/* Source component even masks */
/** Event mask for Source component's command Queue */
#define AREC_CMDQ_EVENT       SystemEvent0

static const uint32_t AudioRecord_RecCompEvent[AREC_OUTPORT_MAX] =
    {
        AREC_0_COMP_EVENT,
        AREC_1_COMP_EVENT,
        AREC_2_COMP_EVENT
    };

/** Event mask for audio driver play channel - play completion */
#define AREC_REC_COMP_EVENTS  (AudioRecord_RecCompEvent[0UL]   \
                                | AudioRecord_RecCompEvent[1UL]  \
                                | AudioRecord_RecCompEvent[2UL])

/** Event mask for all the output ports for Source component */
#define AREC_OUTPORT_EVENTS   (SystemEvent8     \
                                | SystemEvent9    \
                                | SystemEvent10)

/** Event for audio source */
#define AREC_ALL_EVENTS       (AREC_CMDQ_EVENT          \
                                | AREC_REC_COMP_EVENTS    \
                                | AREC_OUTPORT_EVENTS)

/** Maximum number of Channels for Source component's input port */
#define AREC_CHANNEL_MAX       (1UL)

/** Structure to hold Source component information */
typedef struct _AudioRecord_PortType {
    Audio_HwIDType hwID;    /**< I2S hardware ID */
    uint32_t dataBufCnt;    /**< Number of Data Buffers processed */
    Audio_LenType bufSize;  /**< size of buffer allocated in driver */
} AudioRecord_PortType;

typedef struct _AudioRecord_CompType {
    AudioRecord_StateType state;
    AudioRecord_PortNumType numPorts;
    const AudioRecord_PortMaskType portMask;
    AudioRecord_PortType port[AREC_OUTPORT_MAX];
} AudioRecord_CompType;

static AudioRecord_CompType AudioRecord_Comp = {
    .state = AREC_STATE_RESET,
    .numPorts = AREC_OUTPORT_MAX,
    .portMask = 0UL
#if defined(ENABLE_AUDIO_REC_PORT0)
        | 0x1UL     /* port 0 enable mask */
#endif
#if defined(ENABLE_AUDIO_REC_PORT1)
        | 0x2UL     /* port 1 enable mask */
#endif
#if defined(ENABLE_AUDIO_REC_PORT2)
        | 0x4UL     /*port 2 enable mask */
#endif
        | 0x0UL,    /* portMask */
    #if (AREC_OUTPORT_MAX == 0)
    #error "AREC_OUTPORT_MAX == 0"
    #endif
    #if (AREC_OUTPORT_MAX > 0)
    .port[0UL] = {
    .hwID = AUDIO_REC_PORT0_HW_ID,
    .dataBufCnt = 0UL,
    },
    #endif
    #if (AREC_OUTPORT_MAX > 1)
    .port[1UL] = {
    .hwID = AUDIO_REC_PORT1_HW_ID,
    .dataBufCnt = 0UL,
    },
    #endif
    #if (AREC_OUTPORT_MAX > 2)
    .port[2UL] = {
    .hwID = AUDIO_REC_PORT2_HW_ID,
    .dataBufCnt = 0UL,
    },
    #endif
    #if (AREC_OUTPORT_MAX > 3)
    #error "AREC_OUTPORT_MAX > 3 is not supported by component"
    #endif
};

static void AudioRecord_ReportError(uint8_t aApiID, int32_t aErr,
                                uint32_t aVal0, uint32_t aVal1,
                                uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_ARC_ID, 0U, aApiID, aErr, 4UL, values);
}

/******************************************************************************
 * Source <==> Framework interaction
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

/** Define and initialize MSGQ_HdrQType instance for Source component */
MSGQ_DEFINE_HDRQ(AudioRecord_CmdMsgHdrQ);

/** Array for commands to Source component */
static MSYS_CmdType AudioRecord_CmdMsg[MSGQ_SIZE];

/** GetMessage function to get the ith message */
static void * AudioRecord_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AudioRecord_CmdMsg[i];
    }

    return msg;
}

/* Define the command queue for the sink component, on which framework shall
 * post the different commands for the sink component.
 */
MSGQ_DEFINE (AudioRecord_CmdQ, AudioRecord_Task, AREC_CMDQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE,
                MSYS_CmdType, &AudioRecord_CmdMsgHdrQ, AudioRecord_GetCmdMsg);

/* Framework shall use this API to post a command to this sink component */
int32_t AudioRecord_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t status;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd) && (MSYS_COMP_ID_AREC == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the sink component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            status = BCM_ActivateTask(AudioRecord_Task);
            if (BCM_ERR_OK != status) {
                AudioRecord_ReportError(AUDIO_REC_SEND_CMD,
                        status, __LINE__, 0UL, 0UL, 0UL);
            }
        }

        /* Post the command to the Source component's command queue */
        retVal = MSGQ_SendMsg(&AudioRecord_CmdQ, (const void * const)aCmd,
                            aMask, aHdr);
    }
    return retVal;
}

/* Framework shall use this API to get the response of a command that was
 * posted to this Source component
 */
int32_t AudioRecord_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Source component's command queue */
        retVal = MSGQ_RecvMsg(&AudioRecord_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the number of output ports for this sink
 * component.
 */
uint32_t AudioRecord_GetNumMediaOutPort(void)
{
    return AREC_OUTPORT_MAX;
}

/* Media buffer message queue for port 0 */
static MSYS_BufferType AudioRecord_MediaBuffer0[MSGQ_SIZE];

static void *AudioRecord_GetDataMsg0(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AudioRecord_MediaBuffer0[i];
    }
    return msg;
}

MSYS_DECLARE_OUT_PORT(AudioRecord_OutPort0, 0UL, AudioRecord_DataMsgHdrQ0,
                        AudioRecord_Task, AudioRecord_GetDataMsg0,
                        MSYS_COMP_ID_AREC, SystemEvent8);

/* Media buffer message queue for port 1 */
static MSYS_BufferType AudioRecord_MediaBuffer1[MSGQ_SIZE];

static void *AudioRecord_GetDataMsg1(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AudioRecord_MediaBuffer1[i];
    }
    return msg;
}

MSYS_DECLARE_OUT_PORT(AudioRecord_OutPort1, 1UL, AudioRecord_DataMsgHdrQ1,
                        AudioRecord_Task, AudioRecord_GetDataMsg1,
                        MSYS_COMP_ID_AREC, SystemEvent9);

/* Media buffer message queue for port 2 */
static MSYS_BufferType AudioRecord_MediaBuffer2[MSGQ_SIZE];

static void *AudioRecord_GetDataMsg2(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AudioRecord_MediaBuffer2[i];
    }
    return msg;
}

MSYS_DECLARE_OUT_PORT(AudioRecord_OutPort2, 2UL, AudioRecord_DataMsgHdrQ2,
                        AudioRecord_Task, AudioRecord_GetDataMsg2,
                        MSYS_COMP_ID_AREC, SystemEvent10);

static MSYS_BufferType * const AudioRecord_MediaBuffer[AREC_OUTPORT_MAX] =
    {
        AudioRecord_MediaBuffer0,
        AudioRecord_MediaBuffer1,
        AudioRecord_MediaBuffer2
    };

static const MSYS_OutPortType * const AudioRecord_OutPort[AREC_OUTPORT_MAX] =
    {
        &AudioRecord_OutPort0,
        &AudioRecord_OutPort1,
        &AudioRecord_OutPort2
    };

/* Framework shall use this API to get the MSYS_OutPortType of this component where
 * MSYS_FillBuffer() request shall be posted (if any).
 */
const MSYS_OutPortType* AudioRecord_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port = NULL;

    switch (aIndex) {
    case 0UL:
        port = AudioRecord_OutPort[aIndex];
        break;
    case 1UL:
        port = AudioRecord_OutPort[aIndex];
        break;
    case 2UL:
        port = AudioRecord_OutPort[aIndex];
        break;
    default:
        break;
    }
    return port;
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
 * Source component Functionality.
 *
 * Source component uses a single task, that shall be the server task for the
 * framework<-->component message queue. The same task handles all interaction
 * with other components as well.
 *****************************************************************************/

static void AudioRecord_ProcessMediaPort(uint32_t aPortNum)
{
    int32_t retVal;
    uint32_t mediaBufIdx;
    MSYS_BufferType *mediaBufferPtr;
    Audio_BufInfoType audioBufInfo;
    uint32_t bufSize;

    do {
        retVal = MSGQ_GetMsgIdx(&(AudioRecord_OutPort[aPortNum]->dataQ),
                                &mediaBufIdx);
        if ((retVal != BCM_ERR_OK) && (retVal != BCM_ERR_BUSY)) {
            AudioRecord_ReportError(AUDIO_REC_PROC_MEDIA_PORT, retVal,
                    aPortNum, __LINE__, 0UL, 0UL);
            break;
        }
        if (retVal == BCM_ERR_BUSY) {
            break;
        }
        mediaBufferPtr = &(AudioRecord_MediaBuffer[aPortNum][mediaBufIdx]);

        /* validate mediabuffer */
        if ((AREC_CHANNEL_MAX == mediaBufferPtr->numChannels)
                && (MSYS_BUFFER_FORMAT_AUDIO_PCM == mediaBufferPtr->format)
                && (MSYS_CHANNEL_TYPE_LR == mediaBufferPtr->buffer[0UL].channelType)
                && (NULL != mediaBufferPtr->buffer[0UL].data)
                && (0UL != mediaBufferPtr->buffer[0UL].size)){

            /* TODO: Add logic to completely fill them media buffer */
            bufSize = BCM_MIN(mediaBufferPtr->buffer[0UL].size,
                        AudioRecord_Comp.port[aPortNum].bufSize);

            retVal = Audio_DeQueueRecBuf(AudioRecord_Comp.port[aPortNum].hwID,
                                        bufSize, &audioBufInfo);
            if (BCM_ERR_EAGAIN != retVal) {
                if ((BCM_ERR_OK != retVal) || (NULL == audioBufInfo.buf)) {
                    AudioRecord_ReportError(AUDIO_REC_PROC_MEDIA_PORT, retVal,
                            aPortNum, __LINE__, 0UL, 0UL);
                } else {
                    BCM_MemCpy(mediaBufferPtr->buffer[0].data, audioBufInfo.buf,
                            bufSize);
                    retVal = Audio_QueueRecBuf(
                                AudioRecord_Comp.port[aPortNum].hwID, bufSize,
                                audioBufInfo.buf);
                    if (BCM_ERR_OK != retVal) {
                        AudioRecord_ReportError(AUDIO_REC_PROC_MEDIA_PORT,
                                retVal, aPortNum, __LINE__, 0UL, 0UL);
                    }
                    mediaBufferPtr->buffer[0UL].size = bufSize;
                    mediaBufferPtr->response = retVal;
                }
            }
        } else {
            mediaBufferPtr->response = BCM_ERR_INVAL_PARAMS;
            AudioRecord_ReportError(AUDIO_REC_PROC_MEDIA_PORT,
                    BCM_ERR_INVAL_PARAMS, aPortNum, __LINE__, 0UL, 0UL);
        }
        if (BCM_ERR_OK == retVal) {
            AudioRecord_Comp.port[aPortNum].dataBufCnt++;
            retVal = MSGQ_CompleteMsgIdx(&(AudioRecord_OutPort[aPortNum]->dataQ),
                                        mediaBufIdx);
        }
    } while (BCM_ERR_OK == retVal);
}

/* Helper function that handles all transitions from Reset state */
static void AudioRecord_ResetStateHandler(void)
{
    int32_t retVal;
    uint32_t i;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&AudioRecord_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioRecord_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
            for (i = 0UL; i < AudioRecord_Comp.numPorts; i++) {
                if ((AudioRecord_Comp.portMask & (0x1UL << i)) != 0UL) {
                    retVal = MSYS_OutPortInit(AudioRecord_OutPort[i]);
                    if(BCM_ERR_OK != retVal) {
                        AudioRecord_ReportError(AUDIO_REC_RESET_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }
                }
            }
            if (BCM_ERR_OK == retVal) {
                AudioRecord_Comp.state = AREC_STATE_INIT;
            }
            processCmd->response = retVal;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        retVal = MSGQ_CompleteMsgIdx(&AudioRecord_CmdQ, cmdQIdx);
        if(BCM_ERR_OK != retVal) {
            AudioRecord_ReportError(AUDIO_REC_RESET_STATE_HANDLER,
                    retVal, 0UL, __LINE__, 0UL, 0UL);
        }
    }
}

/* Helper function that handles all transitions from Init state */
static void AudioRecord_InitStateHandler(void)
{
    int32_t retVal;
    uint32_t i;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    Audio_RecCfgType cfg;
    Audio_InfoType info;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&AudioRecord_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioRecord_CmdMsg[cmdQIdx];
        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            for (i = 0UL; i < AudioRecord_Comp.numPorts; i++) {
                if ((AudioRecord_Comp.portMask & (0x1UL << i)) != 0UL) {
                    cfg.sampleWidth = 24UL;
                    cfg.samplePack = AUDIO_PACK_4B_MSB_ALIGN_LE;
                    cfg.numChan = 2UL;

                    retVal = Audio_GetRecInfo(AudioRecord_Comp.port[i].hwID,
                                            &info);
                    if (BCM_ERR_OK != retVal) {
                        AudioRecord_ReportError(AUDIO_REC_INIT_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }

                    cfg.startThreshold = info.bufSize;
                    AudioRecord_Comp.port[i].bufSize = info.bufSize;

                    retVal = Audio_RecCfg(AudioRecord_Comp.port[i].hwID, &cfg);
                    if (BCM_ERR_OK != retVal) {
                        AudioRecord_ReportError(AUDIO_REC_INIT_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }
                }
            }
            if (BCM_ERR_OK == retVal) {
                AudioRecord_Comp.state = AREC_STATE_READY;
            }
            processCmd->response = retVal;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component.
             */
            AudioRecord_Comp.state = AREC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&AudioRecord_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AudioRecord_ReportError(AUDIO_REC_INIT_STATE_HANDLER,
                    retVal, 0UL, __LINE__, 0UL, 0UL);
        }
    }
}

uint32_t AudioRecord_OutPortEvent[3UL] = {
SystemEvent8,
SystemEvent9,
SystemEvent10
};
/* Helper function that handles all transitions from Ready state */
static void AudioRecord_ReadyStateHandler(void)
{
    int32_t retVal;
    uint32_t i;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    int32_t retStatus = BCM_ERR_OK;
    BCM_EventMaskType eventMask = 0UL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&AudioRecord_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioRecord_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_START:
            /* Start audio record */
            for (i = 0UL; i < AudioRecord_Comp.numPorts; i++) {
                if ((AudioRecord_Comp.portMask & (0x1UL << i)) != 0UL) {
                    retVal = Audio_RecStart(AudioRecord_Comp.port[i].hwID);
                    if (BCM_ERR_OK != retVal) {
                        AudioRecord_ReportError(AUDIO_REC_READY_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }
                }
            }
            if (BCM_ERR_OK == retVal) {
                AudioRecord_Comp.state = AREC_STATE_RUNNING;
                /* Signal the output port of this Source component to
                 * start processing */
                for (i = 0UL; i < AudioRecord_Comp.numPorts; i++) {
                    if ((AudioRecord_Comp.portMask & (0x1UL << i)) != 0UL) {
                        eventMask |= AudioRecord_OutPortEvent[i];
                    }
                }
                retStatus = BCM_SetEvent(AudioRecord_Task, eventMask);
                if (BCM_ERR_OK != retStatus) {
                    AudioRecord_ReportError(AUDIO_REC_READY_STATE_HANDLER,
                            BCM_ERR_UNKNOWN, 0UL, __LINE__, retStatus, 0UL);
                }
            }
            processCmd->response = retVal;
            break;
        case MSYS_COMP_CMD_DEINIT:
            AudioRecord_Comp.state = AREC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&AudioRecord_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AudioRecord_ReportError(AUDIO_REC_READY_STATE_HANDLER,
                    retVal, 0UL, __LINE__, 0UL, 0UL);
        }
    }
}

/* Helper function that handles all transitions from Running state */
static void AudioRecord_RunStateHandler(EventMaskType aEventMask)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    uint32_t i;
    MSYS_CmdType *processCmd = NULL;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&AudioRecord_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioRecord_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            /* Stop audio record */
            for (i = 0UL; i < AudioRecord_Comp.numPorts; i++) {
                if ((AudioRecord_Comp.portMask & (0x1UL << i)) != 0UL) {
                    retVal = Audio_RecStop(AudioRecord_Comp.port[i].hwID);
                    if (BCM_ERR_OK != retVal) {
                        AudioRecord_ReportError(AUDIO_REC_RUN_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }
                }
            }
            if (BCM_ERR_OK == retVal) {
                /* Complete mediabuffers that are currently processed and
                 * move to Ready state */
                AudioRecord_Comp.state = AREC_STATE_READY;
            }
            processCmd->response = retVal;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Complete mediabuffers that are currently processed and
             * move to Ready state */
            for (i = 0UL; i < AudioRecord_Comp.numPorts; i++) {
                AudioRecord_Comp.port[i].dataBufCnt = 0UL;
            }
            AudioRecord_Comp.state = AREC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }

        /* Mark the processed command as complete */
        retVal = MSGQ_CompleteMsgIdx(&AudioRecord_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AudioRecord_ReportError(AUDIO_REC_RUN_STATE_HANDLER,
                    retVal, 0UL, __LINE__, 0UL, 0UL);
        }
    }

    /* If we are still in the running state, send MSYS_FillBuffer() requests to
     * source component.
     */
    if (AREC_STATE_RUNNING == AudioRecord_Comp.state) {
        for (i = 0UL; i < AudioRecord_Comp.numPorts; i++) {
            if ((AudioRecord_Comp.portMask & (0x1UL << i)) != 0UL) {
                if (((AudioRecord_OutPortEvent[i] & aEventMask) != 0UL)
                        || ((AudioRecord_RecCompEvent[i] & aEventMask) != 0UL)) {
                    AudioRecord_ProcessMediaPort(i);
                }
            }
        }
    }
}

TASK(AudioRecord_Task)
{
    int32_t status;
    BCM_EventMaskType eventMask = 0UL;

    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        status = BCM_WaitEvent(AREC_ALL_EVENTS);
        if (BCM_ERR_OK != status) {
            AudioRecord_ReportError(AUDIO_REC_TASK,
                    status, __LINE__, 0UL, 0UL, 0UL);
        }

        status = BCM_GetEvent(AudioRecord_Task, &eventMask);
        if (BCM_ERR_OK != status) {
            AudioRecord_ReportError(AUDIO_REC_TASK,
                    status, __LINE__, 0UL, 0UL, 0UL);
        }

        status = BCM_ClearEvent(eventMask);
        if (BCM_ERR_OK != status) {
            AudioRecord_ReportError(AUDIO_REC_TASK,
                    status, __LINE__, 0UL, 0UL, 0UL);
        }

        /* Call the state handling helper function depending on the state that
         * this sink component is in.
         */
        switch(AudioRecord_Comp.state) {
        case AREC_STATE_RESET:
            AudioRecord_ResetStateHandler();
            break;
        case AREC_STATE_INIT:
            AudioRecord_InitStateHandler();
            break;
        case AREC_STATE_READY:
            AudioRecord_ReadyStateHandler();
            break;
        case AREC_STATE_RUNNING:
            AudioRecord_RunStateHandler(eventMask);
            break;
        default:
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (AREC_STATE_RESET != AudioRecord_Comp.state);

    status = BCM_TerminateTask();
    if (BCM_ERR_OK != status) {
        AudioRecord_ReportError(AUDIO_REC_TASK,
                (int32_t)status, __LINE__, 0UL, 0UL, 0UL);
    }


}
