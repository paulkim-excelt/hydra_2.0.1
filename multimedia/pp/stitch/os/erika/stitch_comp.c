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
File Name  : stitch_comp.c
Description: This file contains avc encoder component that can be used as a
             reference.
 ******************************************************************************/
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
#include <osil/bcm_osil.h>

static void STCH_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_STC_ID, 0U, aApiID, aErr, 4UL, values);
}

/*
* Macros for Error Reporting
*/
#define STCH_SEND_CMD_API_ID                (0xC0U) /**< STCH_SendCmd */
#define STCH_RECV_RESP_API_ID               (0xC1U) /**< STCH_RecvResp */
#define STCH_GET_NUM_MEDIA_OUTPORT_API_ID   (0xC2U) /**< STCH_GetNumMediaOutPort */
#define STCH_GET_MEDIA_OUTPORT_API_ID       (0xC3U) /**< STCH_GetMediaOutPort */
#define STCH_GET_MEDIA_INPORT_API_ID        (0xC4U) /**< STCH_GetMediaInPort */

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

#define STCH_IN_YUV_BUFFERS_PER_PORT  (4UL)
#define STCH_TEMP_YUV_FRAMES_PER_PORT (4UL)

#define STCH_COMP_CIRCQ_SIZE         (16UL)
#define STCH_COMP_TEMP_CIRCQ_SIZE    (4UL)

#define STCH_MAX_NUM_IN_PORT   (4UL)

typedef struct _STCH_BufInfo
{
    uint32_t bufIdx;
} STCH_BufInfo;

#define INIT_STCH_BUF_INFO   \
{                           \
    .timestamp = 0ULL,      \
    .bufIdx = 0UL,          \
}

typedef struct _STCH_CompContext {
    uint32_t state;
    uint32_t width;
    uint32_t height;
    uint64_t timestamp;
    uint64_t nextTimestamp;
    uint32_t stopInProgress;
    STCHLib_Context context;
    uint32_t totalMBRows;
    uint32_t seqNum;
    uint32_t outCounter;
    uint32_t stats[4UL];
    uint32_t mbRowChoice;
    uint32_t currFinishedJobs;
    uint32_t currTotalJobs;
    uint32_t currSubmittedJobs;
    uint32_t srcLumaOffset;
    uint32_t dstLumaOffsets[4UL];
    uint32_t dstChromaOffsets[4UL];
    uint32_t PPBusy;
    uint32_t copyHeight;
}STCH_CompContext;

static STCH_CompContext GSTCH_CompContext;

/* declare YUV buffers */
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.STCH_YUVData") STCH_YUVData[STCH_MAX_NUM_IN_PORT][STCH_IN_YUV_BUFFERS_PER_PORT][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT * 3UL / 2UL];

uint8_t COMP_SECTION(".dram.test.data") STCH_YUVFrame[STCH_MAX_NUM_IN_PORT][STCH_TEMP_YUV_FRAMES_PER_PORT][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT * 3UL / 8UL];

static CIRCQ_IndexType STCH_circQTempFreeIdx[STCH_MAX_NUM_IN_PORT] = {
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE)
};
static uint8_t STCH_circQTempFreeBuffer[STCH_MAX_NUM_IN_PORT][STCH_COMP_TEMP_CIRCQ_SIZE];
static const CIRCQ_Type STCH_CIRCQTempFree[STCH_MAX_NUM_IN_PORT] = {
	CIRCQ_INIT(&STCH_circQTempFreeIdx[0UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFreeBuffer[0UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQTempFreeIdx[1UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFreeBuffer[1UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQTempFreeIdx[2UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFreeBuffer[2UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQTempFreeIdx[3UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFreeBuffer[3UL], CIRCQ_OVERWRITE_DISABLE)
};

static CIRCQ_IndexType STCH_circQTempFillIdx[STCH_MAX_NUM_IN_PORT] = {
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_TEMP_CIRCQ_SIZE)
};
static uint8_t STCH_circQTempFillBuffer[STCH_MAX_NUM_IN_PORT][STCH_COMP_TEMP_CIRCQ_SIZE];
static const CIRCQ_Type STCH_CIRCQTempFill[STCH_MAX_NUM_IN_PORT] = {
	CIRCQ_INIT(&STCH_circQTempFillIdx[0UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFillBuffer[0UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQTempFillIdx[1UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFillBuffer[1UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQTempFillIdx[2UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFillBuffer[2UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQTempFillIdx[3UL], STCH_COMP_TEMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQTempFillBuffer[3UL], CIRCQ_OVERWRITE_DISABLE)
};

static CIRCQ_IndexType STCH_circQFillIdx[STCH_MAX_NUM_IN_PORT] = {
	CIRCQ_INDEX_INIT(STCH_COMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_CIRCQ_SIZE),
	CIRCQ_INDEX_INIT(STCH_COMP_CIRCQ_SIZE)
};
static uint8_t STCH_circQFillBuffer[STCH_MAX_NUM_IN_PORT][STCH_COMP_CIRCQ_SIZE];
static const CIRCQ_Type STCH_CIRCQFill[STCH_MAX_NUM_IN_PORT] = {
	CIRCQ_INIT(&STCH_circQFillIdx[0UL], STCH_COMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQFillBuffer[0UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQFillIdx[1UL], STCH_COMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQFillBuffer[1UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQFillIdx[2UL], STCH_COMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQFillBuffer[2UL], CIRCQ_OVERWRITE_DISABLE),
	CIRCQ_INIT(&STCH_circQFillIdx[3UL], STCH_COMP_CIRCQ_SIZE, sizeof(uint8_t), (char *)STCH_circQFillBuffer[3UL], CIRCQ_OVERWRITE_DISABLE)
};

static void STCH_FillBuffer(uint32_t aBufIdx);
/**
 *  @brief Call back API for fill Buffer request
 *
 *  @details This API will be called when fill buffer request is served
 *
 *  @param [in] aMediaBuffer Media buffer structure pointer
 *
 *  @return None
 */
static void STCH_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    if (0UL != aMediaBuffer->buffer[0].size) {
        int32_t ret;
        uint8_t bufIdx;
        GSTCH_CompContext.nextTimestamp = aMediaBuffer->timestamp;
        bufIdx = (uint8_t)(aMediaBuffer->bufIdx & 0xFFUL);
        /* push it to fill queue */
        ret = CIRCQ_Push(&STCH_CIRCQFill[aMediaBuffer->bufIdx >> 8UL],(char*)(&bufIdx),1UL);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            /* Rejection due to overflow should be sent back for filling */
            STCH_FillBuffer(aMediaBuffer->bufIdx);
        }
    } else {
        /* submit back if stop is not in progress */
        /* TODO: Implement stop sequence */
        /* STCH_FillBuffer(aMediaBuffer->bufIdx); */
    }
}

MSYS_DECLARE_IN_PORT(STCH_InPort0, 0UL, MSYS_COMP_ID_STCH, SystemEvent4, STCH_InPortRW0, STCH_FillBufferDone);
MSYS_DECLARE_IN_PORT(STCH_InPort1, 1UL, MSYS_COMP_ID_STCH, SystemEvent5, STCH_InPortRW1, STCH_FillBufferDone);
MSYS_DECLARE_IN_PORT(STCH_InPort2, 2UL, MSYS_COMP_ID_STCH, SystemEvent6, STCH_InPortRW2, STCH_FillBufferDone);
MSYS_DECLARE_IN_PORT(STCH_InPort3, 3UL, MSYS_COMP_ID_STCH, SystemEvent7, STCH_InPortRW3, STCH_FillBufferDone);

static const MSYS_InPortType* STCH_MediaInPortPtr[4] = {&STCH_InPort0,&STCH_InPort1,&STCH_InPort2,&STCH_InPort3};
/* Compoenent CmdQ Server Event */
#define STCH_CMDQ_SERVER_EVENT            SystemEvent0
#define STCH_INPORT_EVENT                 (SystemEvent4| SystemEvent5| SystemEvent6| SystemEvent7)
#define STCH_OUTPORT_EVENT                SystemEvent8
#define STCH_DATA_MAILBOX_EVENT           PP_GPEvent

extern MSYS_CmdType STCH_CmdMsgQ[];
MSYS_CmdType STCH_CmdMsgQ[MSGQ_SIZE];
/**
 *  @brief Get command message
 *
 *  @details This API is used by the framework to query command message
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to command message structure
 */
static void* STCH_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&STCH_CmdMsgQ[i];
    }

    return cmdMsg;
}

extern MSYS_BufferType STCH_MediaBuffer[];

MSYS_BufferType STCH_MediaBuffer[MSGQ_SIZE];
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
static void* STCH_GetDataMsg(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&STCH_MediaBuffer[i];
    }

    return dataMsg;
}
static MSGQ_HdrQType STCH_CmdMsgHdrQ = MSGQ_INIT_HDRQ;
extern const MSGQ_Type STCH_CmdQ;
const MSGQ_Type STCH_CmdQ = MSGQ_INIT(STCH_Task, STCH_CMDQ_SERVER_EVENT,
           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &STCH_CmdMsgHdrQ, STCH_GetCmdMsg);

MSYS_DECLARE_OUT_PORT(STCH_OutPort, 0UL, STCH_DataMSGQ_HdrQType,
                        STCH_Task, STCH_GetDataMsg,
                        MSYS_COMP_ID_STCH, SystemEvent8);

#define STCH_MAX_NUM_OUT_PORT  (1UL)

/** @brief Post a command to the command queue of the component
 *
 *  @details     Post a command to the command queue of the component
 *
 *  @param[in]   aCmd    Pointer to Cmd structure
 *  @param[in]   aMask   Mask to signal client task
 *  @param[out]  aHdr    Handle to get response, valid only when return value
 *                       is BCM_ERR_OK
 *
 *  @retval      BCM_ERR_INVAL_PARAMS   aCmd is NULL
 *  @retval      BCM_ERR_INVAL_PARAMS   Invalid magic ID of aCmd
 *  @retval      BCM_ERR_INVAL_PARAMS   aMask is zero
 *  @retval      BCM_ERR_INVAL_PARAMS   aHdr is NULL
 *  @retval      others as retured by MSGQ_SendMsg()
 *
 */
static int32_t STCH_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_STCH == aCmd->compID)
        && (NULL != aHdr)) {
        if (aCmd->cmd == MSYS_COMP_CMD_INIT) {
            retVal = BCM_ActivateTask(STCH_Task);
            if (BCM_ERR_OK != retVal) {
                STCH_ReportError(STCH_SEND_CMD_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        ret = MSGQ_SendMsg(&STCH_CmdQ, (const void * const)aCmd, aMask,
                            aHdr);
    } else {
        STCH_ReportError(STCH_SEND_CMD_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}

/** @brief Receive response for the command posted previously
 *
 *  @details     Receive response to the command posted previously to the
 *               command queue of the component
 *  @param[in]   aHdr    Handle returned by SendCmd API
 *  @param[out]  aCmd    Response from the component for posted command
 *                       (command corresponding to handle in hdr)
 *
 *  @retval      BCM_ERR_INVAL_PARAMS   aCmd is NULL
 *  @retval      BCM_ERR_INVAL_PARAMS   aHdr is NULL
 *  @retval      others as retured by MSGQ_RecvMsg()
 */
static int32_t STCH_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        ret = MSGQ_RecvMsg(&STCH_CmdQ, aHdr, (void* const) aCmd);
    } else {
        STCH_ReportError(STCH_RECV_RESP_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}
/**
 *  @brief Get the num of outports of stitch component
 *
 *  @details Get the number of media outports of stitch component
 *
 *  @return Number of media outports of stitch component
 *
 */
static uint32_t STCH_GetNumMediaOutPort(void)
{
    return STCH_MAX_NUM_OUT_PORT;
}

/** @brief Get the MSYS_OutPortType where the message needs to be posted to
 *
 * @details Get the MSYS_OutPortType where the message needs to be posted to (triggered
 * when the sink component requests stitch component to fill the media buffer)
 *
 * @param[in]   aIndex          Port Index
 *
 * @retval      NULL aIndex is not valid
 * @retval      pointer to the media outport
 */
static const MSYS_OutPortType* STCH_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        outPort = &STCH_OutPort;
        break;
    default:
        STCH_ReportError(STCH_GET_MEDIA_OUTPORT_API_ID, BCM_ERR_INVAL_PARAMS,
                        __LINE__, 0UL, 0UL, 0UL);
        break;
    }

    return outPort;
}

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
static const MSYS_InPortType* STCH_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        inPort = STCH_MediaInPortPtr[0];
        break;
    case 1UL:
        inPort = STCH_MediaInPortPtr[1];
        break;
    case 2UL:
        inPort = STCH_MediaInPortPtr[2];
        break;
    case 3UL:
        inPort = STCH_MediaInPortPtr[3];
        break;
    default:
        STCH_ReportError(STCH_GET_MEDIA_INPORT_API_ID, BCM_ERR_INVAL_PARAMS,
                        __LINE__, 0UL, 0UL, 0UL);
        break;
    }
    return inPort;
}

/* Component function table for lens distortion correction */
static const MSYS_CompFnTblType STCH_CompFnTable =
{
    STCH_SendCmd,
    STCH_RecvResp,
    STCH_GetMediaOutPort,
    STCH_GetMediaInPort,
};

const MSYS_CompFnTblType* STCH_GetCompFnTable(void)
{
    return &STCH_CompFnTable;
};

/**
 *  @brief Process in port requests
 *
 *  @details Submit buffers to source component for filling
 *
 *  @param [in] aInPort Pointer to inport structure
 *
 *  @return None
 */
static void STCH_FillBuffer(uint32_t aBufIdx)
{
    int32_t  ret;

    if (((aBufIdx >> 8UL) < STCH_MAX_NUM_IN_PORT)
        && ((aBufIdx & 0xFFUL) < STCH_IN_YUV_BUFFERS_PER_PORT)) {
        const MSYS_BufferType mediaBuffer = {
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
            .numChannels = 3UL,
            .seqNum = 0UL,
            .bufIdx = aBufIdx,

            .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
            .buffer[0].size = GSTCH_CompContext.width * 16UL,
            .buffer[0].data = &STCH_YUVData[aBufIdx >> 8UL][aBufIdx&0xFFUL][0UL],
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GSTCH_CompContext.width,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GSTCH_CompContext.height,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GSTCH_CompContext.width,

            .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
            .buffer[1].size = (GSTCH_CompContext.width >> 1UL) * 8UL,
            .buffer[1].data = &STCH_YUVData[aBufIdx >> 8UL][aBufIdx&0xFFUL][GSTCH_CompContext.width * GSTCH_CompContext.height],
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GSTCH_CompContext.width >> 1UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GSTCH_CompContext.height >> 1UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GSTCH_CompContext.width >> 1UL,

            .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
            .buffer[2].size = (GSTCH_CompContext.width >> 1UL) * 8UL,
            .buffer[2].data = &STCH_YUVData[aBufIdx >> 8UL][aBufIdx&0xFFUL][(GSTCH_CompContext.width * GSTCH_CompContext.height) + ((GSTCH_CompContext.width >> 1) * (GSTCH_CompContext.height >> 1UL))],
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GSTCH_CompContext.width >> 1UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GSTCH_CompContext.height >> 1UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GSTCH_CompContext.width >> 1UL,
        };
        ret = MSYS_FillBuffer(STCH_MediaInPortPtr[aBufIdx >> 8UL], &mediaBuffer);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_FILL_BUFFER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}
/**
 *  @brief Reset state handler
 *
 *  @details Reset state handling
 *
 *  @return None
 */
static void STCH_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;
    uint8_t bufIdx;
    uint32_t j;

    ret = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &STCH_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            GSTCH_CompContext.context.state = STCH_LIB_STATE_RESET;
            ret = STCHLib_Init(&GSTCH_CompContext.context);
            if (BCM_ERR_OK != ret) {
                STCH_ReportError(STCH_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GSTCH_CompContext.width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            GSTCH_CompContext.height = MEDIA_SYSTEM_MAX_VIDEO_HEIGHT; /* TODO: */
            GSTCH_CompContext.timestamp = 0ULL;
            GSTCH_CompContext.stopInProgress = 0UL;
            GSTCH_CompContext.state = STCH_STATE_INIT;
            GSTCH_CompContext.totalMBRows = (MEDIA_SYSTEM_MAX_VIDEO_HEIGHT + 15UL) >> 4UL; /* TODO */
            GSTCH_CompContext.seqNum = 0UL;
            GSTCH_CompContext.outCounter = 0UL;
            GSTCH_CompContext.stats[0UL] = 0UL;
            GSTCH_CompContext.stats[1UL] = 0UL;
            GSTCH_CompContext.stats[2UL] = 0UL;
            GSTCH_CompContext.stats[3UL] = 0UL;
            GSTCH_CompContext.dstLumaOffsets[0UL] = 0UL;
            GSTCH_CompContext.dstLumaOffsets[1UL] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL;
            GSTCH_CompContext.dstLumaOffsets[2UL] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 8UL;
            GSTCH_CompContext.dstLumaOffsets[3UL] = (MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 8UL) + (MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 2UL);
            GSTCH_CompContext.dstChromaOffsets[0UL] = 0UL;
            GSTCH_CompContext.dstChromaOffsets[1UL] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 4UL;
            GSTCH_CompContext.dstChromaOffsets[2UL] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 2UL;
            GSTCH_CompContext.dstChromaOffsets[3UL] = (MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 2UL) + (MEDIA_SYSTEM_MAX_VIDEO_WIDTH / 4UL);
            GSTCH_CompContext.PPBusy = 0xFFFFFFFFUL;
            GSTCH_CompContext.currTotalJobs = 0UL;
            GSTCH_CompContext.currSubmittedJobs = 0UL;
            GSTCH_CompContext.currFinishedJobs = 0UL;
            GSTCH_CompContext.copyHeight = 0UL;
            ret = MSYS_OutPortInit(&STCH_OutPort);
            if(BCM_ERR_OK != ret) {
                STCH_ReportError(STCH_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }

            bufIdx = 0;

            for(j=0UL; j<STCH_MAX_NUM_IN_PORT; j++) {
                /* make green frame */
                BCM_MemSet(&STCH_YUVData[j][bufIdx][0UL],0,GSTCH_CompContext.width * GSTCH_CompContext.height);
                BCM_MemSet(&STCH_YUVData[j][bufIdx][GSTCH_CompContext.width * GSTCH_CompContext.height],128,((GSTCH_CompContext.width >> 1UL) * (GSTCH_CompContext.height >> 1UL)) * 2UL);

                CIRCQ_Reset(&STCH_CIRCQFill[j]);
                CIRCQ_Push(&STCH_CIRCQFill[j],(char*)(&bufIdx),1UL);
            }

            processCmd->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}
/**
 *  @brief Init state handler
 *
 *  @details Init state handling
 *
 *  @return None
 */
static void STCH_InitStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &STCH_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            switch(processCmd->configID & 0xFFF0UL) {
#if 0
            case BRCM_STCH_MOD_CONFIG:
                if ((processCmd->CfgData.stitchConfig->width <= MEDIA_SYSTEM_MAX_VIDEO_WIDTH)
                    && (processCmd->CfgData.stitchConfig->height <= MEDIA_SYSTEM_MAX_VIDEO_HEIGHT)) {
                    ret = STCHLib_Configure(&GSTCH_CompContext.context, processCmd->CfgData.stitchConfig);
                    if (BCM_ERR_OK != ret) {
                        STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                    }
                    processCmd->response = ret;
                }
            break;
#endif
            case MSYS_CONFIG_STCH_FW:
                ret = STCHLib_Boot(&GSTCH_CompContext.context, (uint16_t)MSYS_CONFIG_STCH_FW,
                                    processCmd->cfgData, processCmd->size);
                if (BCM_ERR_OK != ret) {
                    STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
                processCmd->response = ret;
            break;
            default:
            break;
            }

            if (GSTCH_CompContext.context.state == STCH_LIB_STATE_IDLE) {
                GSTCH_CompContext.state = STCH_STATE_READY;
            }
        }
        ret = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }

}
/**
 *  @brief Ready state handler
 *
 *  @details Ready state handling
 *
 *  @return None
 */
static void STCH_ReadyStateHandler(void)
{
    uint32_t i,j;
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t retVal;
    int32_t ret = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &STCH_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            for(j=0UL; j<STCH_MAX_NUM_IN_PORT; j++) {
				for(i=1UL;i<STCH_IN_YUV_BUFFERS_PER_PORT;i++) {
                    STCH_FillBuffer((j << 8UL) | i);
				}
			}
            retVal = BCM_SetEvent(STCH_Task, STCH_INPORT_EVENT|STCH_OUTPORT_EVENT);
            if (BCM_ERR_OK != retVal) {
                STCH_ReportError(STCH_READY_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
            GSTCH_CompContext.state = STCH_STATE_RUNNING;
            processCmd->response = BCM_ERR_OK;
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            ret = STCHLib_DeInit(&GSTCH_CompContext.context);
            if (BCM_ERR_OK != ret) {
                STCH_ReportError(STCH_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GSTCH_CompContext.state = STCH_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        ret = MSGQ_CompleteMsgIdx(&STCH_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

void STCH_AlarmCb()
{
    if (STCH_STATE_RUNNING == GSTCH_CompContext.state) {
        ATOMIC_Add(&GSTCH_CompContext.outCounter, GSTCH_CompContext.totalMBRows);
        SetEvent(STCH_Task, SystemEvent4);
    }
}

/**
 *  @brief Run state handler
 *
 *  @details Run state handling
 *
 *  @return None
 */
static void STCH_RunStateHandler(EventMaskType aMask)
{
    uint32_t j;
    int32_t ret = BCM_ERR_OK;
    uint32_t localCnt;
    uint32_t flags;
    uint8_t bufIdx;
    uint8_t bufIdxArray[2UL] = {0,0};

    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;

    uint32_t cmdMsgIdx = 0xFFFFFFFFUL;
    MSYS_CmdType *processCmd = NULL;
    STCHLib_YUVBuffer inYuvBuffer;
    STCHLib_YUVBuffer outYuvBuffer;
    uint32_t middleMBRow;

    ret = MSGQ_GetMsgIdx(&STCH_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &STCH_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            GSTCH_CompContext.stopInProgress = 1UL;
        }
    }

    if (0UL != (aMask & SystemEvent4)) {
        ret = MSYS_InPortProcess(STCH_MediaInPortPtr[0UL]);
        if (BCM_ERR_OK != ret &&  BCM_ERR_BUSY != ret) {
            STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
    if (0UL != (aMask & SystemEvent5)) {
        ret = MSYS_InPortProcess(STCH_MediaInPortPtr[1UL]);
        if (BCM_ERR_OK != ret &&  BCM_ERR_BUSY != ret) {
            STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
    if (0UL != (aMask & SystemEvent6)) {
        ret = MSYS_InPortProcess(STCH_MediaInPortPtr[2UL]);
        if (BCM_ERR_OK != ret &&  BCM_ERR_BUSY != ret) {
            STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
    if (0UL != (aMask & SystemEvent7)) {
        ret = MSYS_InPortProcess(STCH_MediaInPortPtr[3UL]);
        if (BCM_ERR_OK != ret &&  BCM_ERR_BUSY != ret) {
            STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }

    if (aMask & PP_GPEvent) {
        flags = STCHLib_Process(&GSTCH_CompContext.context);
        if (0UL != (flags&STCH_LIB_DATA_PROCESSED)){
            GSTCH_CompContext.currFinishedJobs++;
            GSTCH_CompContext.PPBusy = 0xFFFFFFFF;
            if (GSTCH_CompContext.currFinishedJobs == GSTCH_CompContext.currTotalJobs) {
                GSTCH_CompContext.currTotalJobs = 0UL;
                GSTCH_CompContext.currFinishedJobs = 0UL;
                GSTCH_CompContext.currSubmittedJobs = 0UL;
                /* submit the job */
                ret = MSGQ_GetMsgIdx(&STCH_OutPort.dataQ, &mediaBufferIdx);
                if (BCM_ERR_OK == ret) {
                    (void)ATOMIC_Dec(&GSTCH_CompContext.outCounter);
                    mediaBuffer = &STCH_MediaBuffer[mediaBufferIdx];
                    mediaBuffer->seqNum = GSTCH_CompContext.seqNum;
                    mediaBuffer->timestamp = GSTCH_CompContext.timestamp;
                    mediaBuffer->response = BCM_ERR_OK;
                    ret = MSGQ_CompleteMsgIdx(&STCH_OutPort.dataQ, mediaBufferIdx);
                    if (BCM_ERR_OK != ret) {
                        STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                    } else {
                        GSTCH_CompContext.seqNum = (GSTCH_CompContext.seqNum + 1UL) % GSTCH_CompContext.totalMBRows;
                        if (0UL == GSTCH_CompContext.seqNum) {
                            /* switch heads */
                            for (j = 0UL; j < 4UL; j++) {
                                localCnt = CIRCQ_Peek(&STCH_CIRCQFill[j], (char *)bufIdxArray,2UL);
                                if (2UL == localCnt) {
                                    CIRCQ_Pop(&STCH_CIRCQFill[j], (char *)&bufIdx,1UL);
                                    STCH_FillBuffer((j << 8UL) | bufIdx);
                                    GSTCH_CompContext.stats[j]++;
                                    GSTCH_CompContext.timestamp = GSTCH_CompContext.nextTimestamp;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if ((GSTCH_CompContext.PPBusy == 0xFFFFFFFF)
        && (GSTCH_CompContext.outCounter > 0UL)) {
        /* Try submitting jobs */
        ret = MSGQ_GetMsgIdx(&STCH_OutPort.dataQ, &mediaBufferIdx);
        if (BCM_ERR_OK == ret) {
            mediaBuffer = &STCH_MediaBuffer[mediaBufferIdx];
            if (GSTCH_CompContext.currSubmittedJobs == GSTCH_CompContext.currTotalJobs) {
                middleMBRow = GSTCH_CompContext.totalMBRows >> 1UL;
                GSTCH_CompContext.currTotalJobs = 2UL;
                GSTCH_CompContext.copyHeight = 32UL;
                if (0UL ==  (GSTCH_CompContext.totalMBRows & 1UL)) {
                    /* Even */
                    if (GSTCH_CompContext.seqNum < middleMBRow) {
                        /* From 0 and 1 */
                        GSTCH_CompContext.mbRowChoice = 0UL;
                        GSTCH_CompContext.srcLumaOffset = (GSTCH_CompContext.seqNum * 32UL * GSTCH_CompContext.width);
                    } else {
                        /* From 2 and 3 */
                        GSTCH_CompContext.mbRowChoice = 2UL;
                        GSTCH_CompContext.srcLumaOffset = ((GSTCH_CompContext.seqNum - middleMBRow) * 32UL * GSTCH_CompContext.width);
                    }
                } else {
                    /* Odd */
                    if (GSTCH_CompContext.seqNum < middleMBRow) {
                        /* From 0 and 1 */
                        GSTCH_CompContext.mbRowChoice = 0UL;
                        GSTCH_CompContext.srcLumaOffset = (GSTCH_CompContext.seqNum * 32UL * GSTCH_CompContext.width);
                    } else if (GSTCH_CompContext.seqNum > middleMBRow) {
                        /* From 2 and 3 */
                        GSTCH_CompContext.mbRowChoice = 2UL;
                        GSTCH_CompContext.srcLumaOffset = ((GSTCH_CompContext.seqNum - middleMBRow) * 32UL * GSTCH_CompContext.width) - (16UL * GSTCH_CompContext.width);
                    } else {
                        /* From 0,1,2 and 3 */
                        GSTCH_CompContext.mbRowChoice = 0UL;
                        GSTCH_CompContext.currTotalJobs = 4UL;
                        GSTCH_CompContext.copyHeight = 16UL;
                        GSTCH_CompContext.srcLumaOffset = (GSTCH_CompContext.seqNum * 32UL * GSTCH_CompContext.width);
                    }
                }
            }
            /* copy from mbRowChoice + GSTCH_CompContext.currSubmittedJobs to mbrow */
            uint32_t currPortIdx = GSTCH_CompContext.mbRowChoice + GSTCH_CompContext.currSubmittedJobs;
            localCnt = CIRCQ_Peek(&STCH_CIRCQFill[currPortIdx], (char *)&bufIdx,1UL);
            if (1UL == localCnt) {
                inYuvBuffer.lumaAddr = &STCH_YUVData[currPortIdx][bufIdx][0UL];
                inYuvBuffer.cbAddr = &STCH_YUVData[currPortIdx][bufIdx][GSTCH_CompContext.width * GSTCH_CompContext.height];
                inYuvBuffer.crAddr = &STCH_YUVData[currPortIdx][bufIdx][(GSTCH_CompContext.width * GSTCH_CompContext.height) + ((GSTCH_CompContext.width >> 1UL) * (GSTCH_CompContext.height >> 1UL))];
                if ((GSTCH_CompContext.currTotalJobs == 4UL) && (GSTCH_CompContext.currSubmittedJobs >= 2UL)) {
                    /* no change */
                } else {
                    inYuvBuffer.lumaAddr += GSTCH_CompContext.srcLumaOffset;
                    inYuvBuffer.cbAddr += (GSTCH_CompContext.srcLumaOffset >> 2UL);
                    inYuvBuffer.crAddr += (GSTCH_CompContext.srcLumaOffset >> 2UL);
                }

                inYuvBuffer.lumaStride = (uint16_t)GSTCH_CompContext.width;
                inYuvBuffer.chromaStride = (uint16_t)(GSTCH_CompContext.width >> 1UL);
                inYuvBuffer.lumaWidth = (uint16_t)GSTCH_CompContext.width;
                inYuvBuffer.lumaHeight = (uint16_t)GSTCH_CompContext.copyHeight;
                inYuvBuffer.chromaWidth = (uint16_t)(GSTCH_CompContext.width >> 1UL);
                inYuvBuffer.chromaHeight = (uint16_t)(GSTCH_CompContext.copyHeight >> 1UL);

                outYuvBuffer.lumaAddr = mediaBuffer->buffer[0UL].data + GSTCH_CompContext.dstLumaOffsets[GSTCH_CompContext.currSubmittedJobs];
                outYuvBuffer.cbAddr = mediaBuffer->buffer[1UL].data + GSTCH_CompContext.dstChromaOffsets[GSTCH_CompContext.currSubmittedJobs];
                outYuvBuffer.crAddr = mediaBuffer->buffer[2UL].data + GSTCH_CompContext.dstChromaOffsets[GSTCH_CompContext.currSubmittedJobs];
                outYuvBuffer.lumaStride = (uint16_t)(GSTCH_CompContext.width);
                outYuvBuffer.chromaStride = (uint16_t)(GSTCH_CompContext.width >> 1UL);
                ret = STCHLib_DecimateCopy(&GSTCH_CompContext.context,&inYuvBuffer,&outYuvBuffer);
                if (ret == BCM_ERR_OK) {
                    /* store the port index to retrive the buffer index */
                    GSTCH_CompContext.currSubmittedJobs++;
                    GSTCH_CompContext.PPBusy = (GSTCH_CompContext.currSubmittedJobs << 16UL) | (GSTCH_CompContext.seqNum);
                } else {
                    STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            } else {
                /* report error */
                STCH_ReportError(STCH_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
        }
    }
}

TASK(STCH_Task)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;

    do {
        ret = BCM_WaitEvent(STCH_CMDQ_SERVER_EVENT|STCH_DATA_MAILBOX_EVENT|STCH_INPORT_EVENT|STCH_OUTPORT_EVENT);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(STCH_Task,&mask);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            STCH_ReportError(STCH_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }

        switch(GSTCH_CompContext.state) {
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
            STCH_RunStateHandler(mask);
            break;
        case STCH_STATE_ERROR:
            break;
        default:
            break;
        }
    } while(STCH_STATE_RESET != GSTCH_CompContext.state);

    ret = BCM_TerminateTask();
    STCH_ReportError(STCH_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
}
