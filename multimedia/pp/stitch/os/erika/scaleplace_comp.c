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
File Name  : scalePlace_comp.c
Description: This file contains a sample source component that can be used as a
             reference.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO

#include <string.h>
#include <media_port.h>
#include <component.h>
#include <msg_queue.h>
#include <circ_queue.h>
#include <system.h>
#include <bcm_err.h>
#include <ee.h>
#include <pp.h>
#include <stitch.h>
#include <stitch_lib.h>
#include <cache.h>
#include <compiler.h>
#include <bcm_utils.h>
#include <syscfg.h>
#include <bcm_time.h>
#include <atomic.h>
#include <imgl.h>
#include <ulog.h>
#include <osil/bcm_osil.h>
static void STCH_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_STC_ID, 0U, aApiID, aErr, 4UL, values);
}
#define STCH_FILL_BUFFER_DONE_API_ID        (0x83U) /* STCH_FillBufferDone */
#define STCH_GET_CMD_MSG_API_ID             (0x84U) /* STCH_GetCmdMsg */
#define STCH_GET_DATA_MSG_API_ID            (0x85U) /* STCH_GetDataMsg */
#define STCH_FILL_BUFFER_API_ID             (0x86U) /* STCH_FillBuffer */
#define STCH_SEND_CONFIG_BS_API_ID          (0x87U) /* STCH_SendConfigBS */

#define STCH_RESET_STATE_HANDLER_API_ID     (0x40U) /* STCH_ResetStateHandler */
#define STCH_INIT_STATE_HANDLER_API_ID      (0x41U) /* STCH_InitStateHandler */
#define STCH_READY_STATE_HANDLER_API_ID     (0x42U) /* STCH_ReadyStateHandler */
#define STCH_RUN_STATE_HANDLER_API_ID       (0x43U) /* STCH_RunStateHandler */
#define STCH_TASK_API_ID                    (0x44U) /* STCH_Task */


#define STCH_STATE_RESET   (0UL)
#define STCH_STATE_INIT    (1UL)
#define STCH_STATE_READY   (2UL)
#define STCH_STATE_RUNNING (3UL)
#define STCH_STATE_ERROR   (4UL)

typedef uint32_t STCH_StateType;


/* Sink component even masks */
/** Event mask for Sink component's command Queue */
#define STCH_CMDQ_EVENT        SystemEvent0

/** Event mask for all the input ports for Sink component */
#define STCH_PORT_EVENTS     (SystemEvent4 | SystemEvent8 | PP_GPEvent)

/** Maximum number of Channels for Sink component's input port */
#define STCH_CHANNEL_MAX       (3UL)

/** Structure to hold component information */
typedef struct _STCH_CompType {
    STCH_StateType state;
    uint32_t fillReqCnt;    /**< Number of MSYS_FillBuffer requests made */
    uint32_t fillDoneCnt;   /**< Number of MSYS_FillBuffer requests serviced successfully */
    uint32_t fillCnclCnt;   /**< Number of MSYS_FillBuffer requests cancelled */
    uint32_t nextBufIdx;    /**< Index to the next buffer to be used */
    uint32_t dataBufCnt;    /**< Number of Data Buffers processed */
    uint32_t seqNum;
    STCHLib_Context context; /**< stitch lib context */
    uint32_t width;
    uint32_t height;
    uint64_t timestamp;
    uint32_t totalMBRows;
    uint32_t dstLumaOffset;
    uint32_t dstChromaOffset;
    uint32_t yOffset;
    uint32_t xOffset;
    uint32_t task;
} STCH_CompContext;


typedef struct _GSTCH_BufInfo
{
    uint32_t mbRow;
    uint32_t bufIdx;
    uint32_t job;
    uint32_t nJobs[2UL];
    uint32_t frameNum;
    uint16_t frameStart;
    uint16_t frameEnd;
} GSTCH_BufInfo;

#define INIT_STCH_BUF_INFO(aBufInfo)           \
    aBufInfo.mbRow = 0UL;                      \
    aBufInfo.bufIdx = 0UL;                     \
    aBufInfo.job = 0xFFFFFFFFUL;               \
    aBufInfo.nJobs[0UL] = 0UL;                 \
    aBufInfo.nJobs[1UL] = 1UL;                 \

GSTCH_BufInfo STCH_BufInfo;

static STCH_CompContext GSTCH_CompContext = {
    .state = STCH_STATE_RESET,
    .fillReqCnt = 0UL,
    .fillDoneCnt = 0UL,
    .fillCnclCnt = 0UL,
    .nextBufIdx = 0UL,
    .dataBufCnt = 0UL,
};

#define STCH_CIRCQ_LOG2_SIZE (4UL)
CIRCQ_V2_DEFINE(STCH_CIRCQInFill, STCH_CIRCQ_LOG2_SIZE, uint8_t,
                     STCH_CIRCQInFillBuffer, STCH_CIRCQInFillIdx, CIRCQ_OVERWRITE_DISABLE, ".data");
CIRCQ_V2_DEFINE(STCH_CIRCQInProcess, STCH_CIRCQ_LOG2_SIZE, uint8_t,
                     STCH_CIRCQInProcessBuffer, STCH_CIRCQInProcessIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

#define STCH_IN_YUV_BUFFERS (4UL)
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.STCH_YUVData") STCH_YUVData[STCH_IN_YUV_BUFFERS][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT * 3UL / 2UL];

CIRCQ_V2_DEFINE(STCH_CIRCQTempFill, STCH_CIRCQ_LOG2_SIZE, uint8_t,
                     STCH_CIRCQFillBuffer, STCH_CIRCQFillIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

CIRCQ_V2_DEFINE(STCH_CIRCQTempSubmit, STCH_CIRCQ_LOG2_SIZE, uint8_t,
                     STCH_CIRCQSubmitBuffer, STCH_CIRCQSubmitIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

CIRCQ_V2_DEFINE(STCH_CIRCQTempReady, STCH_CIRCQ_LOG2_SIZE, uint8_t,
                     STCH_CIRCQReadyBuffer, STCH_CIRCQReadyIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

#define STCH_TEMP_YUV_BUFFERS (2UL)
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.STCH_YUVData") STCH_TempYUVData[STCH_TEMP_YUV_BUFFERS][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT * 3UL / 2UL];

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.STCH_YUVData") STCH_ByHalfData[(MEDIA_SYSTEM_MAX_VIDEO_WIDTH>>1) * (MEDIA_SYSTEM_MAX_VIDEO_HEIGHT>>1) * 3UL / 2UL];
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
MSGQ_DEFINE_HDRQ(STCH_CmdMsgHdrQ);

/** Array for commands to Sink component */
MSYS_CmdType STCH_CmdMsg[MSGQ_SIZE];

/** GetMessage function to get the ith message */
void * STCH_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &STCH_CmdMsg[i];
    }

    return msg;
}

extern const MSGQ_Type STCH_CmdQ;
const MSGQ_Type STCH_CmdQ = MSGQ_INIT(STCH_Task, STCH_CMDQ_EVENT,
           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &STCH_CmdMsgHdrQ, STCH_GetCmdMsg);


/* Define the command queue for the sink component, on which framework shall
 * post the different commands for the sink component.
 *//*
MSGQ_DEFINE (STCH_CmdQ, STCH_Task, STCH_CMDQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE, Cmd,
             &STCH_CmdMsgHdrQ, STCH_GetCmdMsg);
*/
/* Framework shall use this API to post a command to this sink component */
int32_t STCH_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;

    if ((NULL != aCmd) && (MSYS_COMP_ID_STCH == aCmd->compID) && (NULL != aHdr)) {
        /* Activate the sink component task if an INIT is received */
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {

            BCM_ActivateTask(STCH_Task);
        }

        /* Post the command to the Sink component's command queue */
        retVal = MSGQ_SendMsg(&STCH_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Framework shall use this API to get the response of a command that was
 * posted to this Sink component
 */
int32_t STCH_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the Sink component's command queue */
        retVal = MSGQ_RecvMsg(&STCH_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

#define STCH_MAX_NUM_OUT_PORT (1UL) /* Number of output ports */

/* Framework shall use this API to get the number of output ports for this sink
 * component.
 */
uint32_t STCH_GetNumMediaOutPort(void)
{
    /* Since this is a sink component, there are no output ports */
    return STCH_MAX_NUM_OUT_PORT;
}


static MSYS_BufferType STCH_MediaBuffer[MSGQ_SIZE];

void *STCH_GetDataMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &STCH_MediaBuffer[i];
    }
    return msg;
}
MSYS_DECLARE_OUT_PORT(STCH_OutPort, 0UL, STCH_DataMSGQ_HdrQType,
                        STCH_Task, STCH_GetDataMsg,
                        MSYS_COMP_ID_STCH, SystemEvent8);

/* Framework shall use this API to get the MSYS_OutPortType of this component where
 * MSYS_FillBuffer() request shall be posted (if any).
 */
const MSYS_OutPortType* STCH_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port = NULL;
    /* Since this is sink component there are no output ports */
    switch (aIndex) {
    case 0UL:
        port = &STCH_OutPort;
        break;
    default:
        break;
    }
    return port;
}

/* Component function table for lens distortion correction */
static const MSYS_CompFnTblType STCH_CompFnTable =
{
    STCH_SendCmd,
    STCH_RecvResp,
    STCH_GetMediaOutPort,
    NULL,
};

const MSYS_CompFnTblType* STCH_GetCompFnTable(void)
{
    return &STCH_CompFnTable;
};

void STCH_AlarmCb()
{
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
static void STCH_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    uint8_t bufIdx;
    if ((NULL != aMediaBuffer)
        && (STCH_CHANNEL_MAX == aMediaBuffer->numChannels)) {
        if (BCM_ERR_OK == aMediaBuffer->response) {
            if ((0UL != aMediaBuffer->buffer[0UL].size)) {
                bufIdx = aMediaBuffer->bufIdx;
                INIT_STCH_BUF_INFO(STCH_BufInfo);
                (void)CIRCQ_Push(&STCH_CIRCQInFill, (char *)&bufIdx,1UL);
                GSTCH_CompContext.fillDoneCnt++;
            } else {
                /* Buffer size 0 implies that this buffer was cancelled */
                GSTCH_CompContext.fillCnclCnt++;
            }
        }
    }
}

/* Define input port 0 for this sink component (this component uses only one
 * input port
 */
MSYS_DECLARE_IN_PORT(STCH_InPort, 0UL, MSYS_COMP_ID_STCH, SystemEvent4,
                      STCH_InPortRW, STCH_FillBufferDone);

/******************************************************************************
 * Sink component Functionality.
 *
 * Sink component uses a single task, that shall be the server task for the
 * framework<-->component message queue. The same task handles all interaction
 * with other components as well.
 *****************************************************************************/

/* Helper function that handles all transitions from Reset state */
static void STCH_ResetStateHandler()
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &STCH_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
            GSTCH_CompContext.context.state = STCH_LIB_STATE_RESET;
            retVal = STCHLib_Init(&GSTCH_CompContext.context);
            if (BCM_ERR_OK != retVal) {
                STCH_ReportError(STCH_RESET_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
            GSTCH_CompContext.width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            GSTCH_CompContext.height = MEDIA_SYSTEM_MAX_VIDEO_HEIGHT; /* TODO: */
            GSTCH_CompContext.timestamp = 0ULL;
            GSTCH_CompContext.state = STCH_STATE_INIT;
            GSTCH_CompContext.totalMBRows = (MEDIA_SYSTEM_MAX_VIDEO_HEIGHT + 15UL) >> 4UL;
            GSTCH_CompContext.yOffset = GSTCH_CompContext.height-(GSTCH_CompContext.height>>2);
            GSTCH_CompContext.xOffset = (GSTCH_CompContext.width-(GSTCH_CompContext.width>>2))>> 1;
            GSTCH_CompContext.dstLumaOffset = (GSTCH_CompContext.yOffset * GSTCH_CompContext.width) + GSTCH_CompContext.xOffset;
            GSTCH_CompContext.dstChromaOffset =  ((GSTCH_CompContext.yOffset >> 1UL) * (GSTCH_CompContext.width >> 1UL)) + (GSTCH_CompContext.xOffset >> 1UL);
            GSTCH_CompContext.task = 0UL;

            retVal = MSYS_OutPortInit(&STCH_OutPort);
            if (BCM_ERR_OK != retVal) {
                STCH_ReportError(STCH_RESET_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        retVal = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdQIdx);
        if (BCM_ERR_OK != retVal) {
            STCH_ReportError(STCH_READY_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}


static int32_t STCH_PTMLoadData(uint16_t id,uint8_t* aBuffer, uint32_t aMaxSize, uint32_t* aReadSize)
{
    BCM_EventMaskType mask;
    int32_t status;

    int32_t ret;
    IMGL_LoadStatusType imglStatus;

    ret = IMGL_LoadImg(id, aBuffer, 0UL, aMaxSize,
            SystemEvent0, &imglStatus);

    if (ret == BCM_ERR_OK) {
        BCM_WaitEvent(SystemEvent0);
        BCM_GetEvent(STCH_Task, &mask);
        BCM_ClearEvent(SystemEvent0);
        status = IMGL_GetStatus(&imglStatus);
        if (status == BCM_ERR_OK) {
            *aReadSize = imglStatus.size;
            ret = BCM_ERR_OK;
        } else {
            STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    } else {
        STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }
    return ret;
}

/* Helper function that handles all transitions from Init state */
static void STCH_InitStateHandler()
{
    int32_t ret;
    MSYS_CmdType *processCmd = NULL;
    uint32_t i;
    uint32_t imgSz = 0;
    uint32_t frmSize = (GSTCH_CompContext.width*GSTCH_CompContext.height*3)>>1;
    uint32_t cmdMsgIdx=0UL;

    ret = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &STCH_CmdMsg[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            switch(processCmd->configID) {
            case MSYS_CONFIG_STCH_FW:
                ret = STCHLib_Boot(&GSTCH_CompContext.context, (uint16_t)MSYS_CONFIG_STCH_FW,
                                    processCmd->cfgData, processCmd->size);
                if (BCM_ERR_OK != ret) {
                    STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
                INIT_STCH_BUF_INFO(STCH_BufInfo);
                processCmd->response = ret;
                break;
            case MSYS_CONFIG_STCH_DATA:
            #if 1
                ret = STCH_PTMLoadData((uint16_t)processCmd->configID, STCH_TempYUVData[0], frmSize , &imgSz);
                if ( frmSize != imgSz) {
                    STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            #else
                ret = BCM_ERR_OK;
            #endif
                for (i=1;i<STCH_TEMP_YUV_BUFFERS;i++){
                    BCM_MemCpy(STCH_TempYUVData[i],STCH_TempYUVData[0],frmSize);
                }
                processCmd->response = ret;
                if (GSTCH_CompContext.context.state == STCH_LIB_STATE_IDLE) {
                    GSTCH_CompContext.state = STCH_STATE_READY;
                }
            break;
            default:
            break;
            }


        }
        ret = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}
static void STCH_FillBuffer(uint32_t aBufIdx)
{
    int32_t  ret;
    if (aBufIdx < STCH_IN_YUV_BUFFERS) {
        const MSYS_BufferType mediaBuffer = {
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
            .numChannels = STCH_CHANNEL_MAX,
            .seqNum = 0UL,
            .bufIdx = aBufIdx,

            .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
            .buffer[0].size = GSTCH_CompContext.width * 16UL,
            .buffer[0].data = &STCH_YUVData[aBufIdx][0UL],
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GSTCH_CompContext.width,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GSTCH_CompContext.height,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GSTCH_CompContext.width,

            .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
            .buffer[1].size = (GSTCH_CompContext.width >> 1UL) * 8UL,
            .buffer[1].data = &STCH_YUVData[aBufIdx][GSTCH_CompContext.width * GSTCH_CompContext.height],
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GSTCH_CompContext.width >> 1UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GSTCH_CompContext.height >> 1UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GSTCH_CompContext.width >> 1UL,

            .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
            .buffer[2].size = (GSTCH_CompContext.width >> 1UL) * 8UL,
            .buffer[2].data = &STCH_YUVData[aBufIdx][(GSTCH_CompContext.width * GSTCH_CompContext.height) + ((GSTCH_CompContext.width >> 1) * (GSTCH_CompContext.height >> 1UL))],
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GSTCH_CompContext.width >> 1UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GSTCH_CompContext.height >> 1UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GSTCH_CompContext.width >> 1UL,
        };
        ret = MSYS_FillBuffer(&STCH_InPort, &mediaBuffer);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_FILL_BUFFER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        } else {
            GSTCH_CompContext.fillReqCnt++;
        }
    }
}
/* Helper function that handles all transitions from Ready state */
static void STCH_ReadyStateHandler()
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint32_t i;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &STCH_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_START:
            GSTCH_CompContext.state = STCH_STATE_RUNNING;
            for (i=0;i<STCH_TEMP_YUV_BUFFERS;i++) {
                CIRCQ_Push(&STCH_CIRCQTempFill, (char*)&i, 1UL);
            }
            for (i=0;i<STCH_IN_YUV_BUFFERS; i++) {
                STCH_FillBuffer(i);
            }
            /* Signal the input port of this Sink component to start processing */
            BCM_SetEvent(STCH_Task, STCH_PORT_EVENTS);
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            retVal = STCHLib_DeInit(&GSTCH_CompContext.context);
            if (BCM_ERR_OK != retVal) {
                STCH_ReportError(STCH_READY_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
            GSTCH_CompContext.state = STCH_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdQIdx);
    }
}


/* Helper function that handles all transitions from Running state */
static void STCH_RunStateHandler()
{
    int32_t retVal;
    uint32_t cmdQIdx;
    uint32_t isEmpty;
    uint32_t cnt;
    uint8_t bufIdx;
    uint8_t tempBufIdx;
    MSYS_CmdType *processCmd = NULL;
    MSYS_BufferType *mediaBufferPtr;
    uint32_t mediaBufIdx;
    uint32_t flags;
    STCHLib_YUVBuffer inYuvBuffer;
    STCHLib_YUVBuffer outYuvBuffer;
    STCHLib_YUVBuffer byHalfYuvBuffer;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &STCH_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            retVal = CIRCQ_IsEmpty(&STCH_CIRCQInFill, &isEmpty);
            if (isEmpty == TRUE) {
                /* Cancel all mediabuffers and move to Ready state */
                MSYS_InPortCancelAll(&STCH_InPort);
                GSTCH_CompContext.state = STCH_STATE_READY;
                processCmd->response = BCM_ERR_OK;
                /* Mark the processed command as complete */
                retVal = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdQIdx);
            }
            break;
        case MSYS_COMP_CMD_DEINIT:
            retVal = CIRCQ_IsEmpty(&STCH_CIRCQInFill, &isEmpty);
            if (isEmpty == TRUE) {
                /* Cancel all mediabuffers and move to Ready state */
                MSYS_InPortCancelAll(&STCH_InPort);
                GSTCH_CompContext.state = STCH_STATE_READY;
                processCmd->response = BCM_ERR_OK;
                /* Mark the processed command as complete */
                retVal = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdQIdx);
            }
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            /* Mark the processed command as complete */
            retVal = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdQIdx);
            break;
        }

    }

    /* If we are still in the running state, send MSYS_FillBuffer() requests to
     * source component.
     */
    if (STCH_STATE_RUNNING == GSTCH_CompContext.state) {

        /* process the events */
        flags = STCHLib_Process(&GSTCH_CompContext.context);
        if ((0UL != (flags&STCH_LIB_DATA_PROCESSED))){
            if (1UL == GSTCH_CompContext.task) {
                /* submit the buffer */
                (void)CIRCQ_Pop(&STCH_CIRCQInProcess, (char*)&bufIdx, 1UL);
                STCH_FillBuffer(bufIdx);
                (void)CIRCQ_Pop(&STCH_CIRCQTempSubmit, (char*)&tempBufIdx, 1UL);
                (void)CIRCQ_Push(&STCH_CIRCQTempReady, (char*)&tempBufIdx, 1UL);
                GSTCH_CompContext.task = 0UL;
            }
            else if (0UL == GSTCH_CompContext.task){
                GSTCH_CompContext.task = 1UL;
            }
        }
        retVal = MSGQ_GetMsgIdx(&STCH_OutPort.dataQ, &mediaBufIdx);
        if (retVal == BCM_ERR_OK) {
            cnt = CIRCQ_Peek(&STCH_CIRCQTempReady, (char*)&tempBufIdx, 1UL);
            if (1UL == cnt) {
            mediaBufferPtr = &STCH_MediaBuffer[mediaBufIdx];
                if (STCH_BufInfo.job != 0xFFFFFFFFUL) {
                    retVal = PP_DMAStatus(STCH_BufInfo.job);
                    if (BCM_ERR_OK == retVal) {
                        mediaBufferPtr->response = BCM_ERR_OK;
                        mediaBufferPtr->seqNum = GSTCH_CompContext.seqNum;
                        retVal = MSGQ_CompleteMsgIdx(&STCH_OutPort.dataQ, mediaBufIdx);
                        GSTCH_CompContext.seqNum++;
                        if (GSTCH_CompContext.totalMBRows == GSTCH_CompContext.seqNum) {
                            GSTCH_CompContext.seqNum = 0UL;
                            CIRCQ_Pop(&STCH_CIRCQTempReady, (char*)&tempBufIdx, 1UL);
                            CIRCQ_Push(&STCH_CIRCQTempFill, (char*)&tempBufIdx, 1UL);
                        }
                        STCH_BufInfo.job = 0xFFFFFFFFUL;
                    }
                }
            }
        }

        /* Process any filled mediabuffers available at the input port */
        retVal = MSYS_InPortProcess(&STCH_InPort);
        cnt = CIRCQ_Peek(&STCH_CIRCQInFill, (char*)&bufIdx, 1UL);
        if (1UL == cnt) {
            /* submit it to the buffer in temp fill queue */
            cnt = CIRCQ_Peek(&STCH_CIRCQTempFill, (char*)&tempBufIdx, 1UL);
            if (1UL == cnt) {
                /* call stitch */
                inYuvBuffer.lumaAddr = &STCH_YUVData[bufIdx][0UL];
                inYuvBuffer.cbAddr = &STCH_YUVData[bufIdx][GSTCH_CompContext.width * GSTCH_CompContext.height];
                inYuvBuffer.crAddr = &STCH_YUVData[bufIdx][(GSTCH_CompContext.width * GSTCH_CompContext.height) + ((GSTCH_CompContext.width >> 1UL) * (GSTCH_CompContext.height >> 1UL))];

                inYuvBuffer.lumaStride = (uint16_t)GSTCH_CompContext.width;
                inYuvBuffer.chromaStride = (uint16_t)(GSTCH_CompContext.width >> 1UL);
                inYuvBuffer.lumaWidth = (uint16_t)GSTCH_CompContext.width;
                inYuvBuffer.lumaHeight = (uint16_t)GSTCH_CompContext.height;
                inYuvBuffer.chromaWidth = (uint16_t)(GSTCH_CompContext.width >> 1UL);
                inYuvBuffer.chromaHeight = (uint16_t)(GSTCH_CompContext.height >> 1UL);


                byHalfYuvBuffer.lumaAddr = &STCH_ByHalfData[0UL];
                byHalfYuvBuffer.cbAddr = &STCH_ByHalfData[(GSTCH_CompContext.width>>1) * (GSTCH_CompContext.height>>1)];
                byHalfYuvBuffer.crAddr = &STCH_ByHalfData[(GSTCH_CompContext.width>>1) * (GSTCH_CompContext.height>>1) + ((GSTCH_CompContext.width >> 2UL) * (GSTCH_CompContext.height >> 2UL))];

                byHalfYuvBuffer.lumaStride = (uint16_t)GSTCH_CompContext.width>>1;
                byHalfYuvBuffer.chromaStride = (uint16_t)(GSTCH_CompContext.width >> 2UL);
                byHalfYuvBuffer.lumaWidth = (uint16_t)GSTCH_CompContext.width>>1;
                byHalfYuvBuffer.lumaHeight = (uint16_t)GSTCH_CompContext.height>>1;
                byHalfYuvBuffer.chromaWidth = (uint16_t)(GSTCH_CompContext.width >> 2UL);
                byHalfYuvBuffer.chromaHeight = (uint16_t)(GSTCH_CompContext.height >> 2UL);

                outYuvBuffer.lumaAddr = &STCH_TempYUVData[tempBufIdx][GSTCH_CompContext.dstLumaOffset];
                outYuvBuffer.cbAddr = &STCH_TempYUVData[tempBufIdx][(GSTCH_CompContext.width * GSTCH_CompContext.height) + GSTCH_CompContext.dstChromaOffset];
                outYuvBuffer.crAddr = &STCH_TempYUVData[tempBufIdx][(GSTCH_CompContext.width * GSTCH_CompContext.height) + ((GSTCH_CompContext.width >> 1UL) * (GSTCH_CompContext.height >> 1UL)) + GSTCH_CompContext.dstChromaOffset];

                outYuvBuffer.lumaStride = (uint16_t)(GSTCH_CompContext.width);
                outYuvBuffer.chromaStride = (uint16_t)(GSTCH_CompContext.width >> 1UL);
                if (0UL == GSTCH_CompContext.task){
                    retVal = STCHLib_DecimateCopy(&GSTCH_CompContext.context,&inYuvBuffer,&byHalfYuvBuffer);
                    if (retVal != BCM_ERR_OK) {
                        STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
                    }
                }
                else if (1UL == GSTCH_CompContext.task){
                    retVal = STCHLib_DecimateCopy(&GSTCH_CompContext.context,&byHalfYuvBuffer,&outYuvBuffer);
                    if (retVal == BCM_ERR_OK) {
                        (void)CIRCQ_Pop(&STCH_CIRCQInFill, (char*)&bufIdx, 1UL);
                        (void)CIRCQ_Push(&STCH_CIRCQInProcess, (char*)&bufIdx, 1UL);
                        (void)CIRCQ_Pop(&STCH_CIRCQTempFill, (char*)&tempBufIdx, 1UL);
                        (void)CIRCQ_Push(&STCH_CIRCQTempSubmit, (char*)&tempBufIdx, 1UL);
                    } else {
                        STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
                    }

                }
            }
        }

        retVal = MSGQ_GetMsgIdx(&STCH_OutPort.dataQ, &mediaBufIdx);
        if (retVal == BCM_ERR_OK) {
            cnt = CIRCQ_Peek(&STCH_CIRCQTempReady, (char*)&tempBufIdx, 1UL);
            if (1UL == cnt) {
            mediaBufferPtr = &STCH_MediaBuffer[mediaBufIdx];

                if (STCH_BufInfo.job == 0xFFFFFFFFUL){
                    mediaBufferPtr = &STCH_MediaBuffer[mediaBufIdx];

                    PP_DMABufferType inBuffer;
                    PP_DMABufferType outBuffer;

                    inBuffer.numCH = 3;
                    inBuffer.channel[0].addr = &STCH_TempYUVData[tempBufIdx][GSTCH_CompContext.seqNum * GSTCH_CompContext.width * 16UL];
                    inBuffer.channel[0].width = GSTCH_CompContext.width;
                    inBuffer.channel[0].height = 16UL;
                    inBuffer.channel[0].stride = GSTCH_CompContext.width;

                    inBuffer.channel[1].addr = &STCH_TempYUVData[tempBufIdx][((GSTCH_CompContext.width* GSTCH_CompContext.height)+(GSTCH_CompContext.seqNum * (GSTCH_CompContext.width >>1)* 8UL))];
                    inBuffer.channel[1].width = GSTCH_CompContext.width/2;
                    inBuffer.channel[1].height = 8UL;
                    inBuffer.channel[1].stride = GSTCH_CompContext.width/2;
                    inBuffer.channel[2].addr = &STCH_TempYUVData[tempBufIdx][((GSTCH_CompContext.width* GSTCH_CompContext.height)+(GSTCH_CompContext.width* GSTCH_CompContext.height>>2)+(GSTCH_CompContext.seqNum * (GSTCH_CompContext.width>>1 )* 8UL))];
                    inBuffer.channel[2].width = GSTCH_CompContext.width/2;
                    inBuffer.channel[2].height = 8UL;
                    inBuffer.channel[2].stride = GSTCH_CompContext.width/2;

                    outBuffer.numCH = 3;
                    outBuffer.channel[0].width = GSTCH_CompContext.width;
                    outBuffer.channel[0].height = 16UL;
                    outBuffer.channel[0].stride = GSTCH_CompContext.width;
                    outBuffer.channel[0].addr = mediaBufferPtr->buffer[0UL].data;
                    outBuffer.channel[1].width = GSTCH_CompContext.width>>1;
                    outBuffer.channel[1].height = 8UL;
                    outBuffer.channel[1].stride = GSTCH_CompContext.width>>1;
                    outBuffer.channel[1].addr = mediaBufferPtr->buffer[1UL].data;
                    outBuffer.channel[2].width = GSTCH_CompContext.width>>1;
                    outBuffer.channel[2].height = 8UL;
                    outBuffer.channel[2].stride = GSTCH_CompContext.width>>1;
                    outBuffer.channel[2].addr = mediaBufferPtr->buffer[2UL].data;

                    retVal = PP_DMAStart(&inBuffer, &outBuffer, &STCH_BufInfo.job);
                    if (0xFFFFFFFFUL == STCH_BufInfo.job) {
                        ULOG_INFO("scale and place problem in dma buffer jobid\n");
                    }



                }
            }
        }
    }
}

TASK(STCH_Task)
{
    BCM_EventMaskType eventMask;

    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        BCM_WaitEvent(STCH_CMDQ_EVENT | STCH_PORT_EVENTS);
        BCM_GetEvent(STCH_Task, &eventMask);
        BCM_ClearEvent(eventMask);

        /* Call the state handling helper function depending on the state that
         * this sink component is in.
         */
        switch(GSTCH_CompContext.state)
        {
        case STCH_STATE_RESET:
            STCH_ResetStateHandler();
            break;
        case STCH_STATE_INIT:
            STCH_InitStateHandler();
            break;
        case STCH_STATE_READY:
            STCH_ReadyStateHandler();
            break;
        case STCH_STATE_RUNNING:
            STCH_RunStateHandler();
            break;
        default:
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
    } while (STCH_STATE_RESET != GSTCH_CompContext.state);

    BCM_TerminateTask();
}

