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
File Name  : audio_play.c
Description: This file contains implementation of audio play component.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO

#include <component.h>
#include <media_port.h>
#include <msg_queue.h>
#include <ee.h>
#include <ulog.h>
#include <circ_queue.h>
#include <audio_play.h>
#include <audio.h>
#include <audio_osil.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <osil/bcm_osil.h>

/* Audio play component internal API IDs for error reporting */
/** AudioPlay_PlayBuf API ID */
#define AUDIO_PLAY_PLAY_BUF                 (0x80U)
/** AudioPlay_ProcessFilledBufInfoQ API ID */
#define AUDIO_PLAY_PROC_FILL_BUF_INFO_Q     (0x81U)
/** AudioPlay_ProcessFillBuffer API ID */
#define AUDIO_PLAY_PROC_FILL_BUF            (0x82U)
/** AudioPlay_In0FillBufferDone API ID */
#define AUDIO_PLAY_IN0_FILL_BUFF_DONE       (0x83U)
/** AudioPlay_In1FillBufferDone API ID */
#define AUDIO_PLAY_IN1_FILL_BUFF_DONE       (0x84U)
/** AudioPlay_In2FillBufferDone API ID */
#define AUDIO_PLAY_IN2_FILL_BUFF_DONE       (0x85U)
/** AudioPlay_ProcessMediaPort API ID */
#define AUDIO_PLAY_PROC_MEDIA_PORT          (0x86U)
/** AudioPlay_InitBufInfoQ API ID */
#define AUDIO_PLAY_INIT_BUF_INFO_Q          (0x87U)
/** AudioPlay_DeInitBufInfoQ API ID */
#define AUDIO_PLAY_DEINIT_BUF_INFO_Q        (0x88U)
/** AudioPlay_ResetStateHandler API ID */
#define AUDIO_PLAY_RESET_STATE_HANDLER      (0x89U)
/** AudioPlay_InitStateHandler API ID */
#define AUDIO_PLAY_INIT_STATE_HANDLER       (0x90U)
/** AudioPlay_ReadyStateHandler API ID */
#define AUDIO_PLAY_READY_STATE_HANDLER      (0x91U)
/** AudioPlay_RunStateHandler API ID */
#define AUDIO_PLAY_RUN_STATE_HANDLER        (0x92U)
/** AudioPlay Task API ID */
#define AUDIO_PLAY_TASK                     (0x93U)
/** AudioPlay_SendCmd API ID */
#define AUDIO_PLAY_SEND_CMD                 (0x94U)

/******************************************************************************
 * Data specific to sink component
 *****************************************************************************/
/** Audio play start threshold factor */
#define AUDIO_PLAY_START_THRES_FACTOR   (4UL)

/**
 * Sink Component State Machine.
 * Sink component shall be in APLY_STATE_RESET by default. The state machine
 * for sink component is as follows:
 *
 * Current State        Command             New State
 * ------------------------------------------------------------
 * APLY_STATE_RESET    MSYS_COMP_CMD_INIT       APLY_STATE_INIT
 * APLY_STATE_INIT     MSYS_COMP_CMD_CONFIG     APLY_STATE_READY
 * APLY_STATE_READY    MSYS_COMP_CMD_CONFIG     APLY_STATE_READY
 * APLY_STATE_READY    MSYS_COMP_CMD_START      APLY_STATE_RUNNING
 * APLY_STATE_READY    MSYS_COMP_CMD_DEINIT     APLY_STATE_RESET
 * APLY_STATE_RUNNING  MSYS_COMP_CMD_STOP       APLY_STATE_READY
 * APLY_STATE_RUNNING  MSYS_COMP_CMD_DEINIT     APLY_STATE_RESET
 */
typedef uint32_t AudioPlay_StateType;
#define APLY_STATE_RESET   (0UL)   /**< Sink component Reset state */
#define APLY_STATE_INIT    (1UL)   /**< Sink component Init state */
#define APLY_STATE_READY   (2UL)   /**< Sink component Ready state */
#define APLY_STATE_RUNNING (3UL)   /**< Sink component Running state */

typedef uint32_t AudioPlay_PortNumType;

typedef uint32_t AudioPlay_PortMaskType;

/** Maximum number of ports supported by this component */
#define APLY_PORTS_MAX         (3UL)

/* Sink component event masks */
/** Event mask for Sink component's command Queue */
#define APLY_CMDQ_EVENT       SystemEvent0

static const uint32_t AudioPlay_PlayCompEvent[APLY_PORTS_MAX] =
{
    APLY_0_COMP_EVENT,
    APLY_1_COMP_EVENT,
    APLY_2_COMP_EVENT
};

/** Event mask for audio driver play channel - play completion */
#define APLY_PLAY_COMP_EVENTS (AudioPlay_PlayCompEvent[0]    \
                                | AudioPlay_PlayCompEvent[1]   \
                                | AudioPlay_PlayCompEvent[2])

/** Event mask for all the input ports for Sink component */
#define APLY_IN_PORT_EVENTS   (SystemEvent4      \
                                | SystemEvent5     \
                                | SystemEvent6)

/** Event for audio sink */
#define APLY_ALL_EVENTS       (APLY_CMDQ_EVENT          \
                                | APLY_PLAY_COMP_EVENTS   \
                                | APLY_IN_PORT_EVENTS)

/** Maximum number of Channels for Sink component's input port */
#define APLY_CHANNEL_MAX       (1UL)

typedef struct {
    uint32_t readIdx;  /**< Read start index in data buffer */
    uint32_t bufIdx;   /**< Buffer index in array */
    /** Filled size in buffer */
    uint32_t bufFillSize;
    /** Pointer to data buffer */
    uint8_t * buf;
} AudioPlay_BufInfoType;

/** Structure to hold Sink component information */
typedef struct _AudioPlay_PortType {
    Audio_HwIDType hwID;    /**< I2S hardware ID */
    uint32_t fillReqCnt;    /**< Count of MSYS_FillBuffer requests made */
    uint32_t fillDoneCnt;   /**< Count of MSYS_FillBuffer requests serviced */
    uint32_t fillCnclCnt;   /**< Count of MSYS_FillBuffer requests cancelled */
    uint32_t bufCnt;        /**< Count of buffers in component buffer pool */
    uint32_t bufSize;       /**< Buffer size of component buffers */
    uint8_t * const buf;    /**< Buffer pointer */
    uint32_t drvBufSize;    /**< Audio driver play buffer size */
    /** Pointer to buffer info array */
    AudioPlay_BufInfoType * const bufInfo;
} AudioPlay_PortType;

typedef struct _AudioPlay_CompType {
    AudioPlay_StateType state;
    AudioPlay_PortNumType numPorts;
    const AudioPlay_PortMaskType portMask;
    AudioPlay_PortType port[APLY_PORTS_MAX];
} AudioPlay_CompType;

/* Audio Play component buffers */
#if defined(ENABLE_AUDIO_PLAY_PORT0)
static uint8_t COMP_SECTION(".audio.cached.AudioPlay0Buf") AudioPlay0Buf[AUDIO_PLAY_PORT0_BUF_CNT][AUDIO_PLAY_PORT0_BUF_SIZE];
static AudioPlay_BufInfoType AudioPlay0BufInfo[AUDIO_PLAY_PORT0_BUF_CNT];
#endif /* ENABLE_AUDIO_PLAY_PORT0 */
#if defined(ENABLE_AUDIO_PLAY_PORT1)
static uint8_t COMP_SECTION(".audio.cached.AudioPlay0Buf") AudioPlay1Buf[AUDIO_PLAY_PORT1_BUF_CNT][AUDIO_PLAY_PORT1_BUF_SIZE];
static AudioPlay_BufInfoType AudioPlay1BufInfo[AUDIO_PLAY_PORT1_BUF_CNT];
#endif /* ENABLE_AUDIO_PLAY_PORT1 */
#if defined(ENABLE_AUDIO_PLAY_PORT2)
static uint8_t COMP_SECTION(".audio.cached.AudioPlay0Buf") AudioPlay2Buf[AUDIO_PLAY_PORT2_BUF_CNT][AUDIO_PLAY_PORT2_BUF_SIZE];
static AudioPlay_BufInfoType AudioPlay2BufInfo[AUDIO_PLAY_PORT2_BUF_CNT];
#endif /* ENABLE_AUDIO_PLAY_PORT2 */

static AudioPlay_CompType AudioPlay_Comp = {
    .state = APLY_STATE_RESET,
    .numPorts = APLY_PORTS_MAX,
    .portMask = 0UL
#if defined(ENABLE_AUDIO_PLAY_PORT0)
        | 0x1UL
#endif
#if defined(ENABLE_AUDIO_PLAY_PORT1)
        | 0x2UL
#endif
#if defined(ENABLE_AUDIO_PLAY_PORT2)
        | 0x4UL
#endif
        | 0x0UL, /* portMask */
    #if (APLY_PORTS_MAX == 0)
    #error "APLY_PORTS_MAX == 0"
    #endif
    #if (APLY_PORTS_MAX > 0)
    .port[0UL] = {
    .hwID = AUDIO_PLAY_PORT0_HW_ID,
    .fillReqCnt = 0UL,
    .fillDoneCnt = 0UL,
    .fillCnclCnt = 0UL,
    .bufCnt = AUDIO_PLAY_PORT0_BUF_CNT,
    .bufSize = AUDIO_PLAY_PORT0_BUF_SIZE,
    .drvBufSize = (~0UL), /* Set to max unsigned long value */
#if defined(ENABLE_AUDIO_PLAY_PORT0)
    .buf = &(AudioPlay0Buf[0UL][0UL]),
    .bufInfo = AudioPlay0BufInfo,
#endif
    },
    #endif
    #if (APLY_PORTS_MAX > 1)
    .port[1UL] = {
    .hwID = AUDIO_PLAY_PORT1_HW_ID,
    .fillReqCnt = 0UL,
    .fillDoneCnt = 0UL,
    .fillCnclCnt = 0UL,
    .bufCnt = AUDIO_PLAY_PORT1_BUF_CNT,
    .bufSize = AUDIO_PLAY_PORT1_BUF_SIZE,
    .drvBufSize = (~0UL), /* Set to max unsigned long value */
#if defined(ENABLE_AUDIO_PLAY_PORT1)
    .buf = &(AudioPlay1Buf[0UL][0UL]),
    .bufInfo = AudioPlay1BufInfo,
#endif
    },
    #endif
    #if (APLY_PORTS_MAX > 2)
    .port[2UL] = {
    .hwID = AUDIO_PLAY_PORT2_HW_ID,
    .fillReqCnt = 0UL,
    .fillDoneCnt = 0UL,
    .fillCnclCnt = 0UL,
    .bufCnt = AUDIO_PLAY_PORT2_BUF_CNT,
    .bufSize = AUDIO_PLAY_PORT2_BUF_SIZE,
    .drvBufSize = (~0UL), /* Set to max unsigned long value */
#if defined(ENABLE_AUDIO_PLAY_PORT1)
    .buf = &(AudioPlay2Buf[0UL][0UL]),
    .bufInfo = AudioPlay2BufInfo,
#endif
    },
    #endif
    #if (APLY_PORTS_MAX > 3)
    #error "APLY_PORTS_MAX > 3 is not supported by component"
    #endif
};

/* Audio play component empty and filled buffer queue */
#if defined(ENABLE_AUDIO_PLAY_PORT0)
/* CIRCQ buffer size should be 2 power x */
#define AUDIO_PLAY_PORT0_CIRC_SIZE  (16UL)
#if (AUDIO_PLAY_PORT0_CIRC_SIZE < (AUDIO_PLAY_PORT0_BUF_CNT + 1UL))
#error "AUDIO_PLAY_PORT0_CIRC_SIZE < (AUDIO_PLAY_PORT0_BUF_CNT + 1UL)"
#endif
static AudioPlay_BufInfoType *
        AudioPlay0EmptyBufInfoQBuf[AUDIO_PLAY_PORT0_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay0EmptyBufInfoQIdx, AUDIO_PLAY_PORT0_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay0EmptyBufInfoQ, &AudioPlay0EmptyBufInfoQIdx,
        AUDIO_PLAY_PORT0_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay0EmptyBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);

static AudioPlay_BufInfoType *
            AudioPlay0FilledBufInfoQBuf[AUDIO_PLAY_PORT0_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay0FilledBufInfoQIdx, AUDIO_PLAY_PORT0_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay0FilledBufInfoQ, &AudioPlay0FilledBufInfoQIdx,
        AUDIO_PLAY_PORT0_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay0FilledBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);

static AudioPlay_BufInfoType *
            AudioPlay0FillReqBufInfoQBuf[AUDIO_PLAY_PORT0_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay0FillReqBufInfoQIdx, AUDIO_PLAY_PORT0_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay0FillReqBufInfoQ, &AudioPlay0FillReqBufInfoQIdx,
        AUDIO_PLAY_PORT0_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay0FillReqBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);
#endif

#if defined(ENABLE_AUDIO_PLAY_PORT1)
/* CIRCQ buffer size should be 2 power x */
#define AUDIO_PLAY_PORT1_CIRC_SIZE  (16UL)
#if (AUDIO_PLAY_PORT1_CIRC_SIZE < (AUDIO_PLAY_PORT1_BUF_CNT + 1UL))
#error "AUDIO_PLAY_PORT1_CIRC_SIZE < (AUDIO_PLAY_PORT1_BUF_CNT + 1UL)"
#endif
static AudioPlay_BufInfoType *
        AudioPlay1EmptyBufInfoQBuf[AUDIO_PLAY_PORT1_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay1EmptyBufInfoQIdx, AUDIO_PLAY_PORT1_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay1EmptyBufInfoQ, &AudioPlay1EmptyBufInfoQIdx,
        AUDIO_PLAY_PORT1_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay1EmptyBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);

static AudioPlay_BufInfoType *
            AudioPlay1FilledBufInfoQBuf[AUDIO_PLAY_PORT1_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay1FilledBufInfoQIdx, AUDIO_PLAY_PORT1_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay1FilledBufInfoQ, &AudioPlay1FilledBufInfoQIdx,
        AUDIO_PLAY_PORT1_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay1FilledBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);

static AudioPlay_BufInfoType *
            AudioPlay1FillReqBufInfoQBuf[AUDIO_PLAY_PORT1_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay1FillReqBufInfoQIdx, AUDIO_PLAY_PORT1_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay1FillReqBufInfoQ, &AudioPlay1FillReqBufInfoQIdx,
        AUDIO_PLAY_PORT1_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay1FillReqBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);
#endif

#if defined(ENABLE_AUDIO_PLAY_PORT2)
/* CIRCQ buffer size should be 2 power x */
#define AUDIO_PLAY_PORT2_CIRC_SIZE  (16UL)
#if (AUDIO_PLAY_PORT2_CIRC_SIZE < (AUDIO_PLAY_PORT2_BUF_CNT + 1UL))
#error "AUDIO_PLAY_PORT2_CIRC_SIZE < (AUDIO_PLAY_PORT2_BUF_CNT + 1UL)"
#endif
static AudioPlay_BufInfoType *
        AudioPlay2EmptyBufInfoQBuf[AUDIO_PLAY_PORT2_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay2EmptyBufInfoQIdx, AUDIO_PLAY_PORT2_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay2EmptyBufInfoQ, &AudioPlay2EmptyBufInfoQIdx,
        AUDIO_PLAY_PORT2_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay2EmptyBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);

static AudioPlay_BufInfoType *
            AudioPlay2FilledBufInfoQBuf[AUDIO_PLAY_PORT2_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay2FilledBufInfoQIdx, AUDIO_PLAY_PORT2_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay2FilledBufInfoQ, &AudioPlay2FilledBufInfoQIdx,
        AUDIO_PLAY_PORT2_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay2FilledBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);

static AudioPlay_BufInfoType *
            AudioPlay2FillReqBufInfoQBuf[AUDIO_PLAY_PORT2_CIRC_SIZE];
CIRCQ_INDEX_DEFINE(AudioPlay2FillReqBufInfoQIdx, AUDIO_PLAY_PORT2_CIRC_SIZE);
static CIRCQ_DEFINE(AudioPlay2FillReqBufInfoQ, &AudioPlay2FillReqBufInfoQIdx,
        t
        AUDIO_PLAY_PORT2_CIRC_SIZE, sizeof(AudioPlay_BufInfoType *),
        AudioPlay2FillReqBufInfoQBuf, CIRCQ_OVERWRITE_DISABLE);
#endif

static const CIRCQ_Type * const AudioPlayEmptyBufInfoQ[APLY_PORTS_MAX] =
    {
#if (APLY_PORTS_MAX > 0)
#if defined(ENABLE_AUDIO_PLAY_PORT0)
        &AudioPlay0EmptyBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT0 */
#endif /* (APLY_PORTS_MAX > 0) */
#if (APLY_PORTS_MAX > 1)
#if defined(ENABLE_AUDIO_PLAY_PORT1)
        &AudioPlay1EmptyBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT1 */
#endif /* (APLY_PORTS_MAX > 1) */
#if (APLY_PORTS_MAX > 1)
#if defined(ENABLE_AUDIO_PLAY_PORT2)
        &AudioPlay2EmptyBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT2 */
#endif /* (APLY_PORTS_MAX > 2) */
    };

static const CIRCQ_Type * const AudioPlayFilledBufInfoQ[APLY_PORTS_MAX] =
    {
#if (APLY_PORTS_MAX > 0)
#if defined(ENABLE_AUDIO_PLAY_PORT0)
        &AudioPlay0FilledBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT0 */
#endif /* (APLY_PORTS_MAX > 0) */
#if (APLY_PORTS_MAX > 1)
#if defined(ENABLE_AUDIO_PLAY_PORT1)
        &AudioPlay1FilledBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT1 */
#endif /* (APLY_PORTS_MAX > 1) */
#if (APLY_PORTS_MAX > 2)
#if defined(ENABLE_AUDIO_PLAY_PORT2)
        &AudioPlay2FilledBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT2 */
#endif /* (APLY_PORTS_MAX > 2) */
    };

static const CIRCQ_Type * const AudioPlayFillReqBufInfoQ[APLY_PORTS_MAX] =
    {
#if (APLY_PORTS_MAX > 0)
#if defined(ENABLE_AUDIO_PLAY_PORT0)
        &AudioPlay0FillReqBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT0 */
#endif /* (APLY_PORTS_MAX > 0) */
#if (APLY_PORTS_MAX > 1)
#if defined(ENABLE_AUDIO_PLAY_PORT1)
        &AudioPlay1FillReqBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT1 */
#endif /* (APLY_PORTS_MAX > 1) */
#if (APLY_PORTS_MAX > 2)
#if defined(ENABLE_AUDIO_PLAY_PORT2)
        &AudioPlay2FillReqBufInfoQ,
#else
        NULL,
#endif /* ENABLE_AUDIO_PLAY_PORT2 */
#endif /* (APLY_PORTS_MAX > 2) */
    };

static void AudioPlay_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                                uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_APY_ID, 0U, aApiID, aErr, 4UL, values);
}

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
MSGQ_DEFINE_HDRQ(AudioPlay_CmdMsgHdrQ);

/** Array for commands to Sink component */
static MSYS_CmdType AudioPlay_CmdMsg[MSGQ_SIZE];

/** GetMessage function to get the ith message */
static void * AudioPlay_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &AudioPlay_CmdMsg[i];
    }

    return msg;
}

/* Define the command queue for the sink component, on which framework shall
 * post the different commands for the sink component.
 */
MSGQ_DEFINE (AudioPlay_CmdQ, AudioPlay_Task, APLY_CMDQ_EVENT,
            MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &AudioPlay_CmdMsgHdrQ, AudioPlay_GetCmdMsg);

/* Framework shall use this API to post a command to this sink component */
int32_t AudioPlay_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t status;
    int32_t retVal;

    if ((NULL != aCmd) && (MSYS_COMP_ID_APLY == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the sink component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            status = BCM_ActivateTask(AudioPlay_Task);
            if (BCM_ERR_OK != status) {
                AudioPlay_ReportError(AUDIO_PLAY_SEND_CMD,
                        status, __LINE__, 0UL, 0UL, 0UL);
            }
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&AudioPlay_CmdQ, (const void * const)aCmd,
                                aMask, aHdr);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the response of a command that was
 * posted to this Sink component
 */
int32_t AudioPlay_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Sink component's command queue */
        retVal = MSGQ_RecvMsg(&AudioPlay_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the number of output ports for this sink
 * component.
 */
uint32_t AudioPlay_GetNumMediaOutPort(void)
{
    /* Since this is a sink component, there are no output ports */
    return 0UL;
}

/* Framework shall use this API to get the MSYS_OutPortType of this component where
 * MSYS_FillBuffer() request shall be posted (if any).
 */
const MSYS_OutPortType* AudioPlay_GetMediaOutPort(const uint32_t aIndex)
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
static int32_t AudioPlay_PlayBuf(AudioPlay_PortNumType aPort,
                                AudioPlay_BufInfoType *const aBufInfo)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t bufFillSize = 0UL;
    uint32_t bufSize = 0UL;
    uint8_t * buf = NULL;
    uint32_t i;

    bufFillSize = aBufInfo->bufFillSize;
    i = aBufInfo->readIdx;

    while (i < bufFillSize) {
        if ((bufFillSize - i) > AudioPlay_Comp.port[aPort].drvBufSize) {
            bufSize = AudioPlay_Comp.port[aPort].drvBufSize;
        } else {
            bufSize = bufFillSize - i;
        }

        retVal = Audio_DeQueuePlayBuf(AudioPlay_Comp.port[aPort].hwID,
                                    bufSize, &buf);
        if (BCM_ERR_EAGAIN == retVal) {
            break;
        } else if (BCM_ERR_OK == retVal) {
            BCM_MemCpy(buf, &(aBufInfo->buf[i]), bufSize);

            retVal = Audio_QueuePlayBuf(AudioPlay_Comp.port[aPort].hwID,
                                        bufSize, buf);
            if (BCM_ERR_OK != retVal) {
                AudioPlay_ReportError(AUDIO_PLAY_PLAY_BUF, retVal,
                        aPort, __LINE__, 0UL, 0UL);
                break;
            }
            i += bufSize;
            aBufInfo->readIdx = i;
        } else {
            AudioPlay_ReportError(AUDIO_PLAY_PLAY_BUF, retVal,
                    aPort, __LINE__, 0UL, 0UL);
            break;
        }
    }
    if ((BCM_ERR_OK == retVal) && (i < aBufInfo->bufFillSize)) {
        retVal = BCM_ERR_EAGAIN;
    }

    return retVal;
}

static void AudioPlay_ProcessFilledBufInfoQ(AudioPlay_PortNumType aPort)
{
    int32_t retVal;
    AudioPlay_BufInfoType *bufInfo;
    uint32_t i;

    while(1UL) {
        i = CIRCQ_Peek(AudioPlayFilledBufInfoQ[aPort],(char *)(&bufInfo), 1UL);
        if (1UL == i) {
            retVal = AudioPlay_PlayBuf(aPort, bufInfo);
            if (BCM_ERR_OK == retVal) {
                i = CIRCQ_Pop(AudioPlayFilledBufInfoQ[aPort],
                        (char *)(&bufInfo), 1UL);
                if (1UL != i) {
                    AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF_INFO_Q,
                        BCM_ERR_UNKNOWN, aPort, __LINE__, i, 0UL);
                    break;
                } else {
                    retVal = CIRCQ_Push(AudioPlayEmptyBufInfoQ[aPort],
                            (char *)(&bufInfo), 1UL);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF_INFO_Q,
                                retVal, aPort, __LINE__, 0UL, 0UL);
                        break;
                    }
                }
            } else if (BCM_ERR_EAGAIN == retVal) {
                break;
            } else {
                AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF_INFO_Q,
                        retVal, aPort, __LINE__, 0UL, 0UL);
                break;
            }
        } else {
            break;
        }
    };
}

static void AudioPlay_ProcessFillBuffer(AudioPlay_PortNumType aPort,
                                        MSYS_BufferType *const aMediaBuffer)
{
    int32_t retVal = BCM_ERR_OK;
    AudioPlay_BufInfoType *bufInfo;
    uint32_t i;

    if ((aPort < AudioPlay_Comp.numPorts) && (NULL != aMediaBuffer)) {
        while (1UL) {
            i = CIRCQ_Peek(AudioPlayFilledBufInfoQ[aPort],
                        (char *)(&bufInfo), 1UL);
            if (1UL == i) {
                retVal = AudioPlay_PlayBuf(aPort, bufInfo);
                if (BCM_ERR_OK == retVal) {
                    i = CIRCQ_Pop(AudioPlayFilledBufInfoQ[aPort],
                                (char *)(&bufInfo), 1UL);
                    if (1UL != i) {
                        AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                                BCM_ERR_UNKNOWN, aPort, __LINE__, i, 0UL);
                        break;
                    } else {
                        retVal = CIRCQ_Push(AudioPlayEmptyBufInfoQ[aPort],
                                (char *)(&bufInfo), 1UL);
                        if (BCM_ERR_OK != retVal) {
                            AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                                    retVal, aPort, __LINE__, 0UL, 0UL);
                            break;
                        }
                    }
                } else if (BCM_ERR_EAGAIN == retVal) {
                    break;
                } else {
                    AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                            retVal, aPort, __LINE__, 0UL, 0UL);
                    break;
                }
            } else {
                break;
            }
        }

        if (BCM_ERR_OK == retVal) {
            i = CIRCQ_Pop(AudioPlayFillReqBufInfoQ[aPort],
                        (char *)(&bufInfo), 1UL);
            if (1UL == i) {
                bufInfo->readIdx = 0UL;
                bufInfo->bufIdx = aMediaBuffer->bufIdx;
                bufInfo->bufFillSize = aMediaBuffer->buffer[0UL].size;
                bufInfo->buf = aMediaBuffer->buffer[0UL].data;

                retVal = AudioPlay_PlayBuf(aPort, bufInfo);
                if (BCM_ERR_OK == retVal) {
                    retVal = CIRCQ_Push(AudioPlayEmptyBufInfoQ[aPort],
                            (char *)(&bufInfo), 1UL);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                                retVal, aPort, __LINE__, 0UL, 0UL);
                    }
                } else if (BCM_ERR_EAGAIN == retVal) {
                    retVal = CIRCQ_Push(AudioPlayFilledBufInfoQ[aPort],
                                        (char *)(&bufInfo), 1UL);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                                retVal, aPort, __LINE__, 0UL, 0UL);
                    }
                } else {
                    AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                            retVal, aPort, __LINE__, 0UL, 0UL);
                }
            } else {
                AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                        BCM_ERR_UNKNOWN, aPort, __LINE__, i, 0UL);
            }
        } else if (BCM_ERR_EAGAIN == retVal) {
            i = CIRCQ_Pop(AudioPlayFillReqBufInfoQ[aPort],
                    (char *)(&bufInfo), 1UL);
            if (1UL == i) {
                bufInfo->readIdx = 0UL;
                bufInfo->bufIdx = aMediaBuffer->bufIdx;
                bufInfo->bufFillSize = aMediaBuffer->buffer[0UL].size;
                bufInfo->buf = aMediaBuffer->buffer[0UL].data;

                retVal = CIRCQ_Push(AudioPlayFilledBufInfoQ[aPort],
                        (char *)(&bufInfo), 1UL);
                if (BCM_ERR_OK != retVal) {
                    AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                            retVal, aPort, __LINE__, 0UL, 0UL);
                }
            }
        } else {
            AudioPlay_ReportError(AUDIO_PLAY_PROC_FILL_BUF,
                    retVal, aPort, __LINE__, 0UL, 0UL);
        }
    }
}

/* This callback shall be invoked from framework to this Sink component to
 * notify completion of buffer.
 */
static void AudioPlay_In0FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    uint32_t portNum = 0UL;

    if ((NULL != aMediaBuffer)
        && (APLY_CHANNEL_MAX == aMediaBuffer->numChannels)
        && (MSYS_BUFFER_FORMAT_AUDIO_PCM == aMediaBuffer->format)
        && (MSYS_CHANNEL_TYPE_LR == aMediaBuffer->buffer[0UL].channelType)) {
        if (/*(BCM_ERR_OK == aMediaBuffer->response)
            && */(NULL != aMediaBuffer->buffer[0UL].data)
                && (aMediaBuffer->buffer[0UL].size <=
                    AudioPlay_Comp.port[portNum].bufSize)) {

            if (0UL != aMediaBuffer->buffer[0UL].size) {
                AudioPlay_ProcessFillBuffer(portNum, aMediaBuffer);
                AudioPlay_Comp.port[portNum].fillDoneCnt++;
            } else {
                /* Buffer size 0 implies that this buffer was cancelled */
                AudioPlay_Comp.port[portNum].fillCnclCnt++;
            }
        } else {
            AudioPlay_ReportError(AUDIO_PLAY_IN0_FILL_BUFF_DONE,
                    BCM_ERR_INVAL_PARAMS, portNum, __LINE__, 0UL, 0UL);
        }
    } else {
        AudioPlay_ReportError(AUDIO_PLAY_IN0_FILL_BUFF_DONE,
                BCM_ERR_INVAL_PARAMS, portNum, __LINE__, 0UL, 0UL);
    }
}

static void AudioPlay_In1FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    uint32_t portNum = 1UL;

    if ((NULL != aMediaBuffer)
        && (APLY_CHANNEL_MAX == aMediaBuffer->numChannels)
        && (MSYS_BUFFER_FORMAT_AUDIO_PCM == aMediaBuffer->format)
        && (MSYS_CHANNEL_TYPE_LR == aMediaBuffer->buffer[0UL].channelType)) {
        if (/*(BCM_ERR_OK == aMediaBuffer->response)
            && */(NULL != aMediaBuffer->buffer[0UL].data)
                && (aMediaBuffer->buffer[0UL].size <=
                    AudioPlay_Comp.port[portNum].bufSize)) {

            if (0UL != aMediaBuffer->buffer[0UL].size) {
                AudioPlay_ProcessFillBuffer(portNum, aMediaBuffer);
                AudioPlay_Comp.port[portNum].fillDoneCnt++;
            } else {
                /* Buffer size 0 implies that this buffer was cancelled */
                AudioPlay_Comp.port[portNum].fillCnclCnt++;
            }
        } else {
            AudioPlay_ReportError(AUDIO_PLAY_IN1_FILL_BUFF_DONE,
                    BCM_ERR_INVAL_PARAMS, portNum, __LINE__, 0UL, 0UL);
        }
    } else {
        AudioPlay_ReportError(AUDIO_PLAY_IN1_FILL_BUFF_DONE,
                BCM_ERR_INVAL_PARAMS, portNum, __LINE__, 0UL, 0UL);
    }
}

static void AudioPlay_In2FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    uint32_t portNum = 2UL;

    if ((NULL != aMediaBuffer)
        && (APLY_CHANNEL_MAX == aMediaBuffer->numChannels)
        && (MSYS_BUFFER_FORMAT_AUDIO_PCM == aMediaBuffer->format)
        && (MSYS_CHANNEL_TYPE_LR == aMediaBuffer->buffer[0UL].channelType)) {
        if (/*(BCM_ERR_OK == aMediaBuffer->response)
            && */(NULL != aMediaBuffer->buffer[0UL].data)
                && (aMediaBuffer->buffer[0UL].size <=
                    AudioPlay_Comp.port[portNum].bufSize)) {

            if (0UL != aMediaBuffer->buffer[0UL].size) {
                AudioPlay_ProcessFillBuffer(portNum, aMediaBuffer);
                AudioPlay_Comp.port[portNum].fillDoneCnt++;
            } else {
                /* Buffer size 0 implies that this buffer was cancelled */
                AudioPlay_Comp.port[portNum].fillCnclCnt++;
            }
        } else {
            AudioPlay_ReportError(AUDIO_PLAY_IN2_FILL_BUFF_DONE,
                    BCM_ERR_INVAL_PARAMS, portNum, __LINE__, 0UL, 0UL);
        }
    } else {
        AudioPlay_ReportError(AUDIO_PLAY_IN2_FILL_BUFF_DONE,
                BCM_ERR_INVAL_PARAMS, portNum, __LINE__, 0UL, 0UL);
    }
}

/* Define input ports for this sink component (this component uses only one
 * input port
 */
MSYS_DECLARE_IN_PORT(AudioPlay_In0Port, 0UL, MSYS_COMP_ID_APLY, SystemEvent4,
        AudioPlay_In0PortRW, AudioPlay_In0FillBufferDone);

MSYS_DECLARE_IN_PORT(AudioPlay_In1Port, 1UL, MSYS_COMP_ID_APLY, SystemEvent5,
            AudioPlay_In1PortRW, AudioPlay_In1FillBufferDone);

MSYS_DECLARE_IN_PORT(AudioPlay_In2Port, 2UL, MSYS_COMP_ID_APLY, SystemEvent6,
            AudioPlay_In2PortRW, AudioPlay_In2FillBufferDone);

static const MSYS_InPortType * const AudioPlay_InPort[APLY_PORTS_MAX] =
            {&AudioPlay_In0Port, &AudioPlay_In1Port, &AudioPlay_In2Port};

/******************************************************************************
 * Sink component Functionality.
 *
 * Sink component uses a single task, that shall be the server task for the
 * framework<-->component message queue. The same task handles all interaction
 * with other components as well.
 *****************************************************************************/
static void AudioPlay_ProcessMediaPort(AudioPlay_PortNumType aPort)
{
    int32_t retVal;
    AudioPlay_BufInfoType *bufInfo = NULL;
    uint32_t i;

    /* Process any buffer in the filled buffer queue */
    AudioPlay_ProcessFilledBufInfoQ(aPort);

    /* Process any filled mediabuffers available at the input port */
    retVal = MSYS_InPortProcess(AudioPlay_InPort[aPort]);
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
        AudioPlay_ReportError(AUDIO_PLAY_PROC_MEDIA_PORT,
                retVal, aPort, __LINE__, 0UL, 0UL);
    }

    do {
        retVal = BCM_ERR_OK;
        i = CIRCQ_Peek(AudioPlayEmptyBufInfoQ[aPort], (char *)(&bufInfo), 1UL);
        if (1UL == i) {
            const MSYS_BufferType mediaBuffer = {
                /* TODO: Add timestamp support later */
                .timestamp = 0ULL,
                .format = MSYS_BUFFER_FORMAT_AUDIO_PCM,
                .numChannels = APLY_CHANNEL_MAX,
                .response = BCM_ERR_UNKNOWN,
                .bufIdx = bufInfo->bufIdx,
                .buffer[0].channelType = MSYS_CHANNEL_TYPE_LR,
                .buffer[0].data = bufInfo->buf,
                .buffer[0].size = AudioPlay_Comp.port[aPort].bufSize,
            };
            retVal = MSYS_FillBuffer(AudioPlay_InPort[aPort], &mediaBuffer);
            if (BCM_ERR_OK == retVal) {
                i = CIRCQ_Pop(AudioPlayEmptyBufInfoQ[aPort],
                            (char *)(&bufInfo), 1UL);
                if (1UL != i) {
                    AudioPlay_ReportError(AUDIO_PLAY_PROC_MEDIA_PORT,
                            BCM_ERR_UNKNOWN, aPort, __LINE__, i, 0UL);
                } else {
                    retVal = CIRCQ_Push(AudioPlayFillReqBufInfoQ[aPort],
                                        (char *)(&bufInfo), 1UL);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_PROC_MEDIA_PORT,
                                retVal, aPort, __LINE__, 0UL, 0UL);
                    }
                }
                AudioPlay_Comp.port[aPort].fillReqCnt++;
            } else if (BCM_ERR_BUSY == retVal) {
                /* Don't remove buffer from empty queue */
            } else {
                AudioPlay_ReportError(AUDIO_PLAY_PROC_MEDIA_PORT,
                        retVal, aPort, __LINE__, 0UL, 0UL);
                break;
            }
        } else {
            break;
        }
    } while (BCM_ERR_OK == retVal);
}

static int32_t AudioPlay_InitBufInfoQ(void)
{
    int32_t retVal = BCM_ERR_OK;
    AudioPlay_BufInfoType *bufInfo = NULL;
    uint32_t i, j;

    /* Put all the buffers in empty buffer queue */
    for (i = 0UL; i < AudioPlay_Comp.numPorts; i++) {
        if ((AudioPlay_Comp.portMask & (0x1UL << i)) != 0UL) {
            for (j = 0UL; j < AudioPlay_Comp.port[i].bufCnt; j++) {
                bufInfo = &(AudioPlay_Comp.port[i].bufInfo[j]);
                bufInfo->readIdx = 0UL;
                bufInfo->bufIdx = j;
                bufInfo->buf = &(AudioPlay_Comp.port[i].buf[j*AUDIO_PLAY_PORT0_BUF_SIZE]);
                retVal = CIRCQ_Push(AudioPlayEmptyBufInfoQ[i],
                                    (char *)(&bufInfo),
                                    1UL);
                if (BCM_ERR_OK != retVal) {
                    AudioPlay_ReportError(AUDIO_PLAY_INIT_BUF_INFO_Q,
                            retVal, i, __LINE__, j, 0UL);
                    break;
                }
            }
        }
    }

    return retVal;
}

static void AudioPlay_DeInitBufInfoQ(void)
{
    uint32_t i;
    int32_t retVal;

    /* Remove all the buffers from empty buffer queue */
    for (i = 0UL; i < AudioPlay_Comp.numPorts; i++) {
        if ((AudioPlay_Comp.portMask & (0x1UL << i)) != 0UL) {
            retVal = CIRCQ_Reset(AudioPlayFilledBufInfoQ[i]);
            if (BCM_ERR_OK == retVal) {
                AudioPlay_ReportError(AUDIO_PLAY_DEINIT_BUF_INFO_Q,
                        retVal, i, __LINE__, 0UL, 0UL);
                break;
            }
            retVal = CIRCQ_Reset(AudioPlayEmptyBufInfoQ[i]);
            if (BCM_ERR_OK == retVal) {
                AudioPlay_ReportError(AUDIO_PLAY_DEINIT_BUF_INFO_Q,
                        retVal, i, __LINE__, 0UL, 0UL);
                break;
            }
        }
    }
}

/* Helper function that handles all transitions from Reset state */
static void AudioPlay_ResetStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&AudioPlay_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioPlay_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
            retVal = AudioPlay_InitBufInfoQ();
            if (BCM_ERR_OK == retVal) {
                AudioPlay_Comp.state = APLY_STATE_INIT;
                processCmd->response = BCM_ERR_OK;
            } else {
                processCmd->response = BCM_ERR_OK;
            }
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        retVal = MSGQ_CompleteMsgIdx(&AudioPlay_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AudioPlay_ReportError(AUDIO_PLAY_RESET_STATE_HANDLER,
                    retVal, 0UL, __LINE__, 0UL, 0UL);
        }
    }
}

/* Helper function that handles all transitions from Init state */
static void AudioPlay_InitStateHandler(void)
{
    int32_t retVal;
    uint32_t i;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    Audio_PlayCfgType cfg;
    Audio_InfoType info;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&AudioPlay_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioPlay_CmdMsg[cmdQIdx];
        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure audio play with config data (processCmd->CfgData).
             */
            for (i = 0UL; i < AudioPlay_Comp.numPorts; i++) {
                if ((AudioPlay_Comp.portMask & (0x1UL << i)) != 0UL) {
                    retVal = Audio_GetPlayInfo(AudioPlay_Comp.port[i].hwID,
                                                &info);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_INIT_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }

                    AudioPlay_Comp.port[i].drvBufSize = info.bufSize;
                    cfg.sampleWidth = 32UL;
                    cfg.samplePack = AUDIO_PACK_4B_MSB_ALIGN_LE;
                    cfg.numChan = 2UL;
                    cfg.startThreshold = (AUDIO_PLAY_START_THRES_FACTOR
                                            * info.bufSize);

                    retVal = Audio_PlayCfg(AudioPlay_Comp.port[i].hwID, &cfg);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_INIT_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }
                }
            }
            if (BCM_ERR_OK == retVal) {
                AudioPlay_Comp.state = APLY_STATE_READY;
            }
            processCmd->response = retVal;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component.
             */
            AudioPlay_DeInitBufInfoQ();
            AudioPlay_Comp.state = APLY_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&AudioPlay_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AudioPlay_ReportError(AUDIO_PLAY_INIT_STATE_HANDLER,
                    retVal, 0UL, __LINE__, 0UL, 0UL);
        }
    }
}

/* Helper function that handles all transitions from Ready state */
static void AudioPlay_ReadyStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    uint32_t i;
    MSYS_CmdType *processCmd = NULL;
    int32_t retStatus = BCM_ERR_OK;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&AudioPlay_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioPlay_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_START:
            /* Start audio play */
            for (i = 0UL; i < AudioPlay_Comp.numPorts; i++) {
                if ((AudioPlay_Comp.portMask & (0x1UL << i)) != 0UL) {
                    retVal = Audio_PlayStart(AudioPlay_Comp.port[i].hwID);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_READY_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        processCmd->response = retVal;
                        break;
                    }
                }
            }
            if (BCM_ERR_OK == retVal) {
                AudioPlay_Comp.state = APLY_STATE_RUNNING;
                /* Signal the input port of this Sink component to
                 * start processing */
                retStatus = BCM_SetEvent(AudioPlay_Task, APLY_IN_PORT_EVENTS);
                if (BCM_ERR_OK != retStatus) {
                    AudioPlay_ReportError(AUDIO_PLAY_READY_STATE_HANDLER,
                            retVal, 0UL, __LINE__, retStatus, 0UL);
                }
                processCmd->response = retVal;
            } else {
                processCmd->response = retVal;
            }
            break;
        case MSYS_COMP_CMD_DEINIT:
            AudioPlay_DeInitBufInfoQ();
            AudioPlay_Comp.state = APLY_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&AudioPlay_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            AudioPlay_ReportError(AUDIO_PLAY_READY_STATE_HANDLER,
                    retVal, 0UL, __LINE__, 0UL, 0UL);
        }
    }
}

uint32_t AudioPlay_InPortEvent[3UL] = {
SystemEvent4,
SystemEvent5,
SystemEvent6
};
/* Helper function that handles all transitions from Running state */
static void AudioPlay_RunStateHandler(EventMaskType aEventMask)
{
    int32_t retVal;
    uint32_t i, j;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    AudioPlay_BufInfoType *bufInfo;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&AudioPlay_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AudioPlay_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            for (i = 0UL; i < AudioPlay_Comp.numPorts; i++) {
                if ((AudioPlay_Comp.portMask & (0x1UL << i)) != 0UL) {
                    /* Process any buffer in the filled buffer queue */
                    AudioPlay_ProcessFilledBufInfoQ(i);
                    /* Cancel all mediabuffers */
                    retVal = MSYS_InPortCancelAll(AudioPlay_InPort[i]);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_RUN_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }
                    /* Check filled buffer queue */
                    j = CIRCQ_Peek(AudioPlayFilledBufInfoQ[i],
                                (char *)(&bufInfo), 1UL);
                    if (0UL == j) {
                        /* Stop audio play */
                        retVal = Audio_PlayStop(AudioPlay_Comp.port[i].hwID);
                        if (BCM_ERR_OK != retVal) {
                            AudioPlay_ReportError(AUDIO_PLAY_RUN_STATE_HANDLER,
                                    retVal, i, __LINE__, 0UL, 0UL);
                            break;
                        }
                    }
                }
            }

            if (BCM_ERR_OK == retVal) {
                AudioPlay_Comp.state = APLY_STATE_READY;
            }
            processCmd->response = retVal;
            break;
        case MSYS_COMP_CMD_DEINIT:
            for (i = 0UL; i < AudioPlay_Comp.numPorts; i++) {
                if ((AudioPlay_Comp.portMask & (0x1UL << i)) != 0UL) {
                    /* Process any buffer in the filled buffer queue */
                    AudioPlay_ProcessFilledBufInfoQ(i);
                    /* Cancel all mediabuffers */
                    retVal = MSYS_InPortCancelAll(AudioPlay_InPort[i]);
                    if (BCM_ERR_OK != retVal) {
                        AudioPlay_ReportError(AUDIO_PLAY_RUN_STATE_HANDLER,
                                retVal, i, __LINE__, 0UL, 0UL);
                        break;
                    }
                    /* Check filled buffer queue */
                    j = CIRCQ_Peek(AudioPlayFilledBufInfoQ[i],
                                (char *)(&bufInfo), 1UL);
                    if (0UL == j) {
                        /* Stop audio play */
                        retVal = Audio_PlayStop(AudioPlay_Comp.port[i].hwID);
                        if (BCM_ERR_OK != retVal) {
                            AudioPlay_ReportError(AUDIO_PLAY_RUN_STATE_HANDLER,
                                    retVal, i, __LINE__, 0UL, 0UL);
                            break;
                        }
                    }
                }
            }
            if (BCM_ERR_OK == retVal) {
                AudioPlay_DeInitBufInfoQ();
                AudioPlay_Comp.state = APLY_STATE_RESET;
            }
            processCmd->response = retVal;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }

        if (BCM_ERR_OK == retVal) {
            /* Mark the processed command as complete */
            retVal = MSGQ_CompleteMsgIdx(&AudioPlay_CmdQ, cmdQIdx);
            if (BCM_ERR_OK != retVal) {
                AudioPlay_ReportError(AUDIO_PLAY_RUN_STATE_HANDLER,
                        retVal, 0UL, __LINE__, 0UL, 0UL);
            }
        }
    }

    /* If we are still in the running state, send MSYS_FillBuffer() requests to
     * source component.
     */
    if (APLY_STATE_RUNNING == AudioPlay_Comp.state) {
        for (i = 0UL; i < AudioPlay_Comp.numPorts; i++) {
            if ((AudioPlay_Comp.portMask & (0x1UL << i)) != 0UL) {
                if (((AudioPlay_InPortEvent[i] & aEventMask) != 0UL)
                        || ((AudioPlay_PlayCompEvent[i] & aEventMask) != 0UL)) {
                    AudioPlay_ProcessMediaPort(i);
                }
            }
        }
    }
}

TASK(AudioPlay_Task)
{
    int32_t status;
    BCM_EventMaskType eventMask = 0UL;

    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        status = BCM_WaitEvent(APLY_ALL_EVENTS);
        if (BCM_ERR_OK != status) {
            AudioPlay_ReportError(AUDIO_PLAY_TASK,
                    status, __LINE__, 0UL, 0UL, 0UL);
        }

        status = BCM_GetEvent(AudioPlay_Task, &eventMask);
        if (BCM_ERR_OK != status) {
            AudioPlay_ReportError(AUDIO_PLAY_TASK,
                    status, __LINE__, 0UL, 0UL, 0UL);
        }

        status = BCM_ClearEvent(eventMask);
        if (BCM_ERR_OK != status) {
            AudioPlay_ReportError(AUDIO_PLAY_TASK,
                    status, __LINE__, 0UL, 0UL, 0UL);
        }


        /* Call the state handling helper function depending on the state that
         * this sink component is in.
         */
        switch(AudioPlay_Comp.state) {
        case APLY_STATE_RESET:
            AudioPlay_ResetStateHandler();
            break;
        case APLY_STATE_INIT:
            AudioPlay_InitStateHandler();
            break;
        case APLY_STATE_READY:
            AudioPlay_ReadyStateHandler();
            break;
        case APLY_STATE_RUNNING:
            AudioPlay_RunStateHandler((EventMaskType)eventMask);
            break;
        default:
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (APLY_STATE_RESET != AudioPlay_Comp.state);

    status = BCM_TerminateTask();
    if (BCM_ERR_OK != status) {
        AudioPlay_ReportError(AUDIO_PLAY_TASK,
                (int32_t)status, __LINE__, 0UL, 0UL, 0UL);
    }

}
