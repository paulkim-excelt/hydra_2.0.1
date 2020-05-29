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
File Name  : avce_comp.c
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
#include <avce_lib.h>
#include <osil/avce_osil.h>
#include <avce.h>
#include <cache.h>
#include <compiler.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>
#define GetModuleLogLevel()     ULOG_LVL_INFO


static void AVCE_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_AVE_ID, 0U, aApiID, aErr, 4UL, values);
}

#define AVCE_FILL_BUFFER_DONE_API_ID        (0x83U) /* AVCE_FillBufferDone */
#define AVCE_GET_CMD_MSG_API_ID             (0x84U) /* AVCE_GetCmdMsg */
#define AVCE_GET_DATA_MSG_API_ID            (0x85U) /* AVCE_GetDataMsg */
#define AVCE_PROCESS_INPORT_API_ID          (0x86U) /* AVCE_ProcessInPort */
#define AVCE_SEND_SPS_API_ID                (0x87U) /* AVCE_SendSPS */
#define AVCE_SEND_PPS_API_ID                (0x87U) /* AVCE_SendPPS */


#define AVCE_RESET_STATE_HANDLER_API_ID     (0x40U) /* AVCE_ResetStateHandler */
#define AVCE_INIT_STATE_HANDLER_API_ID      (0x41U) /* AVCE_InitStateHandler */
#define AVCE_READY_STATE_HANDLER_API_ID     (0x42U) /* AVCE_ReadyStateHandler */
#define AVCE_RUN_STATE_HANDLER_API_ID       (0x43U) /* AVCE_RunStateHandler */
#define AVCE_TASK_API_ID                    (0x44U) /* AVCE_Task */

#define AVCE_SEND_CMD_API_ID                (0xC0U) /* AVCE_SendCmd */
#define AVCE_RECV_RESP_API_ID               (0xC1U) /* AVCE_RecvResp */
#define AVCE_GET_NUM_MEDIAOUTPORT_API_ID    (0xC2U) /* AVCE_GetNumMediaOutPort */
#define AVCE_GET_MEDIAOUTPORT_API_ID        (0xC3U) /* AVCE_GetMediaOutPort */
#define AVCE_GET_MEDIA_INPORT_API_ID        (0xC4U) /* AVCE_GetMediaInPort */

#define AVCE_STATE_RESET   (0UL)
#define AVCE_STATE_INIT    (1UL)
#define AVCE_STATE_READY   (2UL)
#define AVCE_STATE_RUNNING (3UL)
#define AVCE_STATE_ERROR   (4UL)

#ifndef AVCE_TOTAL_YUV_BUFFERS
#define AVCE_TOTAL_YUV_BUFFERS  (4UL)
#endif

#define AVCE_COMP_CIRCQ_SIZE    (8UL)

#ifndef AVCE_STRIDE_ALIGN_VALUE
#define AVCE_STRIDE_ALIGN_VALUE (64UL)
#endif

#define AVCE_STRIDE_ALIGN_MASK   (AVCE_STRIDE_ALIGN_VALUE - 1UL)

#ifndef AVCE_SYSTEM_VIDEO_HEIGHT
#define AVCE_SYSTEM_VIDEO_HEIGHT (MEDIA_SYSTEM_MAX_VIDEO_HEIGHT)
#endif

#ifndef AVCE_SYSTEM_VIDEO_WIDTH
#define AVCE_SYSTEM_VIDEO_WIDTH  (MEDIA_SYSTEM_MAX_VIDEO_WIDTH)
#endif

#ifndef MEDIA_SYSTEM_SLICE_HEIGHT
#define AVCE_SYSTEM_SLICE_HEIGHT (16UL)
#else
#define AVCE_SYSTEM_SLICE_HEIGHT (MEDIA_SYSTEM_SLICE_HEIGHT)
#endif

typedef struct _AVCE_CompContext {
    uint32_t state;
    uint32_t stopInProgress;
    uint32_t stopFlushIssued;
    uint64_t timestamp;
    uint32_t configBSInfo; /* 0 - SPS: 1 - PPS:*/
    uint32_t filledSize;
    uint32_t frameNum;
    uint32_t numMBRowsSubmitted;
    AVCELib_Context context;
}AVCE_CompContext;

static AVCE_CompContext GAVCE_CompContext;

typedef struct _AVCE_BufInfo
{
    uint64_t timestamp;
    uint32_t mbRowNum;
    uint32_t bufIdx;
    uint32_t width;
    uint32_t height;
    uint32_t numMbRows;
    uint32_t frameNum;
} AVCE_BufInfo;

#define INIT_AVCE_BUF_INFO  \
{                           \
    .timestamp = 0ULL,      \
    .mbRowNum = 0UL,        \
    .bufIdx = 0UL,          \
    .width = 0UL,           \
    .height = 0UL,          \
    .numMbRows = 0UL,       \
    .frameNum = 0UL         \
}

typedef struct _AVCE_BufStatus
{
    uint32_t numMBRowsSubmitted;
    uint32_t numMBRowsProcessed;
} AVCE_BufStatus;

#if AVCE_SYSTEM_SLICE_HEIGHT > (16UL)
static AVCE_BufStatus AVCE_YUVDataStatus[AVCE_TOTAL_YUV_BUFFERS];
#endif

/* declare YUV buffers */
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.AVCE_YUVData") AVCE_YUVData[AVCE_TOTAL_YUV_BUFFERS][(((AVCE_SYSTEM_VIDEO_WIDTH + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * AVCE_SYSTEM_SLICE_HEIGHT) + ((((AVCE_SYSTEM_VIDEO_WIDTH / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * (AVCE_SYSTEM_SLICE_HEIGHT / 2UL)) * 2UL];

CIRCQ_INDEX_DEFINE(AVCE_circQFillIdx,AVCE_COMP_CIRCQ_SIZE);
static AVCE_BufInfo AVCE_circQFillBuffer[AVCE_COMP_CIRCQ_SIZE];
static const CIRCQ_Type AVCE_CircQFill = CIRCQ_INIT(&AVCE_circQFillIdx, AVCE_COMP_CIRCQ_SIZE,
                    sizeof(AVCE_BufInfo), (char *)AVCE_circQFillBuffer,
                    CIRCQ_OVERWRITE_DISABLE);

CIRCQ_INDEX_DEFINE(AVCE_circQSubmitIdx,AVCE_COMP_CIRCQ_SIZE);
static AVCE_BufInfo AVCE_circQSubmitBuffer[AVCE_COMP_CIRCQ_SIZE];
static const CIRCQ_Type AVCE_CIRCQSubmit = CIRCQ_INIT(&AVCE_circQSubmitIdx, AVCE_COMP_CIRCQ_SIZE,
                    sizeof(AVCE_BufInfo), (char *)AVCE_circQSubmitBuffer,
                    CIRCQ_OVERWRITE_DISABLE);

CIRCQ_INDEX_DEFINE(AVCE_circQProcessIdx,AVCE_COMP_CIRCQ_SIZE);
static AVCE_BufInfo AVCE_circQProcessBuffer[AVCE_COMP_CIRCQ_SIZE];
static const CIRCQ_Type AVCE_CircQProcess = CIRCQ_INIT(&AVCE_circQProcessIdx, AVCE_COMP_CIRCQ_SIZE,
                    sizeof(AVCE_BufInfo), (char *)AVCE_circQProcessBuffer,
                    CIRCQ_OVERWRITE_DISABLE);

CIRCQ_INDEX_DEFINE(AVCE_circQEmptyIdx,AVCE_COMP_CIRCQ_SIZE);
static AVCE_BufInfo AVCE_circQEmptyBuffer[AVCE_COMP_CIRCQ_SIZE];
static const CIRCQ_Type AVCE_CircQEmpty = CIRCQ_INIT(&AVCE_circQEmptyIdx, AVCE_COMP_CIRCQ_SIZE,
                    sizeof(AVCE_BufInfo), (char *)AVCE_circQEmptyBuffer,
                    CIRCQ_OVERWRITE_DISABLE);

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.AVCE_TempMem") AVCE_CoreContexBuf[AVCE_LIB_MAX_NUM_CORES][AVCE_LIB_CORE_CONTEXT_BUF_SIZE];


static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.AVCE_TempMem") AVCE_LibIntBuf[AVCE_LIB_INTERNAL_USE_BUF_SIZE];

#ifndef AVCE_TEMP_BS_BUFSIZE
#define AVCE_TEMP_BS_BUFSIZE (16UL*1024UL)
#endif

#ifndef AVCE_NUM_TEMP_BS_BUF
#define AVCE_NUM_TEMP_BS_BUF (2UL)
#endif

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.AVCE_TempBSMem") AVCE_TempBsBuf[AVCE_NUM_TEMP_BS_BUF][AVCE_TEMP_BS_BUFSIZE];

/* OSIL API */
uint8_t* AVCELib_GetCoreContextBuf(const uint32_t aCoreIndex)
{
    if (AVCE_LIB_MAX_NUM_CORES > aCoreIndex) {
        return &AVCE_CoreContexBuf[aCoreIndex][0UL];
    } else {
        return NULL;
    }
}

void AVCELib_GetTempBsBuf(const uint32_t aBufIndex,AVCELib_TempBsBuffer* const aTempBsBuf)
{
    if (NULL != aTempBsBuf) {
        if (AVCE_NUM_TEMP_BS_BUF > aBufIndex) {
            aTempBsBuf->idx = aBufIndex;
            aTempBsBuf->size = AVCE_TEMP_BS_BUFSIZE;
            aTempBsBuf->buffer = &AVCE_TempBsBuf[aBufIndex][0UL];
        } else {
            aTempBsBuf->idx = 0xFFFFFFFFUL;
            aTempBsBuf->size = 0UL;
            aTempBsBuf->buffer = NULL;
        }
    }
}
/**
 *  @brief Call back API for fill Buffer request
 *
 *  @details This API will be called when fill buffer request is served
 *
 *  @param [in] aMediaBuffer Media buffer structure pointer
 *
 *  @return None
 */
static void AVCE_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    int32_t ret;
    uint32_t localCnt;
    AVCE_BufInfo bufInfo = INIT_AVCE_BUF_INFO;

    if (0UL != aMediaBuffer->buffer[0].size) {
#ifdef AVCE_InPortSe
        ret = WDS_NotifyState(&AVCE_InPortSe, aMediaBuffer->seqNum);
        if (ret != BCM_ERR_OK) {
            /* Log error */
        }
#endif
        /* pop from submit Q */
        localCnt = CIRCQ_Pop(&AVCE_CIRCQSubmit,(char *)&bufInfo,1UL);
        if (1UL == localCnt ) {
            bufInfo.mbRowNum = aMediaBuffer->seqNum;
            if (GAVCE_CompContext.frameNum  == 0xFFFFFFFFUL){
                GAVCE_CompContext.frameNum = aMediaBuffer->frameNum;
            }
            bufInfo.timestamp = aMediaBuffer->timestamp;
            bufInfo.frameNum = aMediaBuffer->frameNum;
#if AVCE_SYSTEM_SLICE_HEIGHT > (16UL)
            bufInfo.numMbRows = ((aMediaBuffer->buffer[0].size / (aMediaBuffer->buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE])) + 15UL) >> 4UL;
            AVCE_YUVDataStatus[bufInfo.bufIdx].numMBRowsSubmitted = 0UL;
            AVCE_YUVDataStatus[bufInfo.bufIdx].numMBRowsProcessed = 0UL;
#endif
            /* push it to fill queue */
            ret = CIRCQ_Push(&AVCE_CircQFill,(char*)(&bufInfo),1UL);
            if (BCM_ERR_OK != ret) {
                AVCE_ReportError(AVCE_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            } else {
#if AVCE_SYSTEM_SLICE_HEIGHT == (16UL)
                /* on success */
                if ((0U != aMediaBuffer->frameEnd) &&
                    (0UL != ((aMediaBuffer->seqNum + 1UL) % GAVCE_CompContext.context.numPPCores))){
                    /* End of frame and slice number non multiple of number of pp cores */
                    bufInfo.bufIdx = AVCE_TOTAL_YUV_BUFFERS;
                    bufInfo.mbRowNum = aMediaBuffer->seqNum + 1UL;
                    bufInfo.frameNum = aMediaBuffer->frameNum;
                    /* push it to fill queue */
                    ret = CIRCQ_Push(&AVCE_CircQFill,(char*)(&bufInfo),1UL);
                    if (BCM_ERR_OK != ret) {
                        AVCE_ReportError(AVCE_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                    }
                }
#endif
            }
        }

    }
}

MSYS_DECLARE_IN_PORT(AVCE_InPort, 0UL, MSYS_COMP_ID_AVCE, SystemEvent4, AVCE_InPortRW, AVCE_FillBufferDone);


/* Compoenent CmdQ Server Event */
#define AVCE_CMDQ_SERVER_EVENT            SystemEvent0
#define AVCE_INPORT_EVENT                 SystemEvent4
#define AVCE_OUTPORT_EVENT                SystemEvent8
#define AVCE_DATA_MAILBOX_EVENT           PP_GPEvent

extern MSYS_CmdType AVCE_CmdMsgQ[];
MSYS_CmdType AVCE_CmdMsgQ[MSGQ_SIZE];
/**
 *  @brief Get command message
 *
 *  @details This API is used by the framework to query command message
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to command message structure
 */
static void* AVCE_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&AVCE_CmdMsgQ[i];
    }

    return cmdMsg;
}

extern MSYS_BufferType AVCE_MediaBuffer[];

MSYS_BufferType AVCE_MediaBuffer[MSGQ_SIZE];
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
static void* AVCE_GetDataMsg(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&AVCE_MediaBuffer[i];
    }

    return dataMsg;
}
static MSGQ_HdrQType AVCE_CmdMsgHdrQ = MSGQ_INIT_HDRQ;
extern const MSGQ_Type AVCE_CmdQ;
const MSGQ_Type AVCE_CmdQ = MSGQ_INIT(AVCE_Task, AVCE_CMDQ_SERVER_EVENT,
           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &AVCE_CmdMsgHdrQ, AVCE_GetCmdMsg);

MSYS_DECLARE_OUT_PORT(AVCE_OutPort, 0UL, AVCE_DataMsgHdrQ, AVCE_Task, AVCE_GetDataMsg,
                        MSYS_COMP_ID_AVCE, SystemEvent8);

#define AVCE_MAX_NUM_OUT_PORT  1UL

/** @brief Post a command to the command queue of the component

    Post a command to the command queue of the component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aCmd    Pointer to Cmd structure
    @param[in]   aMask   Mask to signal client task
    @param[out]  aHdr    Handle to get response, valid only when return value is BCM_ERR_OK

    Return values are documented in reverse-chronological order
    @retval     Others                  as retured by #MSGQ_SendMsg()
    @retval     #BCM_ERR_INVAL_PARAMS   (aCmd is NULL) or
                                        (Invalid magic ID of aCmd) or
                                        (aMask is zero) or
                                        (aHdr is NULL)

    @post None

    @limitations None
*/
static int32_t AVCE_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_AVCE == aCmd->compID)
        && (NULL != aHdr)) {
        if (aCmd->cmd == MSYS_COMP_CMD_INIT) {
            retVal = BCM_ActivateTask(AVCE_Task);
            if (BCM_ERR_OK != retVal) {
                AVCE_ReportError(AVCE_SEND_CMD_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        ret = MSGQ_SendMsg(&AVCE_CmdQ, (const void * const)aCmd, aMask,
                            aHdr);
    } else {
        AVCE_ReportError(AVCE_SEND_CMD_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}


/** @brief Receive response for the command posted previously

    Receive response to the command posted previously to the command queue of the component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aHdr    Handle returned by SendCmd API
    @param[out]  aCmd    Response from the component for posted command (command corresponding to handle in hdr)

    Return values are documented in reverse-chronological order
    @retval     Others                  as retured by #MSGQ_RecvMsg()
    @retval     #BCM_ERR_INVAL_PARAMS   (aCmd is NULL) or
                                        (aHdr is NULL)

    @post None

    @limitations None
*/
static int32_t AVCE_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        ret = MSGQ_RecvMsg(&AVCE_CmdQ, aHdr, (void* const) aCmd);
    } else {
        AVCE_ReportError(AVCE_RECV_RESP_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}
/** @brief Get the num of outports of avce component

    Get the number of media outports of avce component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   void

    Return values are documented in reverse-chronological order
    @retval      Number of media outports of AVCE media component

    @post None

    @limitations None
*/
static uint32_t AVCE_GetNumMediaOutPort(void)
{
    return AVCE_MAX_NUM_OUT_PORT;
}

/** @brief Get the MSYS_OutPortType where the message needs to be posted to

    Get the MSYS_OutPortType where the message needs to be posted to
    (triggered when the sink component requests avce media component to fill the
    media buffer)

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aIndex        Port Index

    Return values are documented in reverse-chronological order
    @retval        pointer to the media outport
    @retval     NULL   (aIndex is not valid)

    @post None

    @limitations None
*/
static const MSYS_OutPortType* AVCE_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        outPort = &AVCE_OutPort;
        break;
    default:
        AVCE_ReportError(AVCE_GET_MEDIAOUTPORT_API_ID, BCM_ERR_INVAL_PARAMS,
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
static const MSYS_InPortType* AVCE_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        inPort = &AVCE_InPort;
        break;
    default:
        AVCE_ReportError(AVCE_GET_MEDIA_INPORT_API_ID, BCM_ERR_INVAL_PARAMS,
                        __LINE__, 0UL, 0UL, 0UL);
        break;
    }

    return inPort;
}

/* Component function table for AVC Encoder */
static const MSYS_CompFnTblType AVCE_CompFnTable =
{
    AVCE_SendCmd,
    AVCE_RecvResp,
    AVCE_GetMediaOutPort,
    AVCE_GetMediaInPort,
};

const MSYS_CompFnTblType* AVCE_GetCompFnTable(void)
{
    return &AVCE_CompFnTable;
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
static void AVCE_ProcessInPort(const MSYS_InPortType * const aInPort)
{
    uint32_t i;
    int32_t  ret;
    uint32_t localCnt;
    AVCE_BufInfo bufInfo = INIT_AVCE_BUF_INFO;

    for (i = 0UL; i < GAVCE_CompContext.context.numPPCores; i++) {
        localCnt = CIRCQ_Pop(&AVCE_CircQEmpty, (char *)&bufInfo,1UL);
        if ((1UL == localCnt) && (AVCE_TOTAL_YUV_BUFFERS > bufInfo.bufIdx)) {
            const MSYS_BufferType mediaBuffer = {
                .frameNum = 0UL,
                .timestamp = 0ULL,
                .version = sizeof(MSYS_BufferType),
                .response = BCM_ERR_UNKNOWN,
                .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
                .numChannels = 3UL,

                .seqNum = 0UL,

                .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
                .buffer[0].size = GAVCE_CompContext.context.width * AVCE_SYSTEM_SLICE_HEIGHT,
                .buffer[0].data = &AVCE_YUVData[bufInfo.bufIdx][0UL],
                .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GAVCE_CompContext.context.width,
                .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = AVCE_SYSTEM_SLICE_HEIGHT,
                .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = (GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK,

                .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
                .buffer[1].size = (GAVCE_CompContext.context.width / 2UL) * (AVCE_SYSTEM_SLICE_HEIGHT / 2UL),
                .buffer[1].data = &AVCE_YUVData[bufInfo.bufIdx][((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * AVCE_SYSTEM_SLICE_HEIGHT],
                .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GAVCE_CompContext.context.width / 2UL,
                .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = AVCE_SYSTEM_SLICE_HEIGHT / 2UL,
                .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = (((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK),

                .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
                .buffer[2].size = (GAVCE_CompContext.context.width / 2UL) * (AVCE_SYSTEM_SLICE_HEIGHT / 2UL),
                .buffer[2].data = &AVCE_YUVData[bufInfo.bufIdx][(((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * AVCE_SYSTEM_SLICE_HEIGHT)
                                    + ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * (AVCE_SYSTEM_SLICE_HEIGHT / 2UL))],
                .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GAVCE_CompContext.context.width / 2UL,
                .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = AVCE_SYSTEM_SLICE_HEIGHT / 2UL,
                .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = (((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK),

            };

            ret = MSYS_FillBuffer(aInPort, &mediaBuffer);
            if (BCM_ERR_OK != ret) {
                AVCE_ReportError(AVCE_PROCESS_INPORT_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                ret = CIRCQ_Push(&AVCE_CircQEmpty, (char *)&bufInfo,1UL);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_PROCESS_INPORT_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            } else {
                /* push to submit Q */
                ret = CIRCQ_Push(&AVCE_CIRCQSubmit, (char *)&bufInfo,1UL);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_PROCESS_INPORT_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        }
    }
}
#ifdef AVCE_BITSTREAM_DUMP_ENABLE
#define AVCE_BS_BUFFER_SIZE (16*1024*1024)
uint8_t COMP_SECTION(".dram.test.bsBuffer") AVCE_BsBuffer[AVCE_BS_BUFFER_SIZE];
uint32_t AVCE_BsBufferSize;
#endif
void AppendBS(uint8_t *aBuf, uint32_t aSize)
{
#ifdef AVCE_BITSTREAM_DUMP_ENABLE
    if (AVCE_BsBufferSize + aSize < AVCE_BS_BUFFER_SIZE) {
        BCM_MemCpy(&AVCE_BsBuffer[AVCE_BsBufferSize], aBuf, aSize);
        AVCE_BsBufferSize += aSize;
    }
#endif
}

/**
 *  @brief Reset state handler
 *
 *  @details Reset state handling
 *
 *  @return None
 */
static void AVCE_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&AVCE_CmdQ, &cmdMsgIdx);
    GAVCE_CompContext.frameNum  = 0xFFFFFFFFUL;
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCE_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            GAVCE_CompContext.context.state = AVCE_LIB_STATE_RESET;
            ret = AVCELib_Init(&GAVCE_CompContext.context);
            if (BCM_ERR_OK != ret) {
                AVCE_ReportError(AVCE_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GAVCE_CompContext.timestamp = 0ULL;
            GAVCE_CompContext.stopInProgress = 0UL;
            GAVCE_CompContext.stopFlushIssued = 0UL;
            GAVCE_CompContext.filledSize = 0UL;
            GAVCE_CompContext.numMBRowsSubmitted = 0UL;
#ifdef AVCE_BITSTREAM_DUMP_ENABLE
            AVCE_BsBufferSize = 0;
#endif
            GAVCE_CompContext.state = AVCE_STATE_INIT;
            ret = MSYS_OutPortInit(&AVCE_OutPort);
            if(BCM_ERR_OK != ret) {
                AVCE_ReportError(AVCE_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            processCmd->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&AVCE_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

typedef union _AVCE_ConfigDataType {
    const uint8_t* hdr;
    const AVCELib_Config *cfg;
} AVCE_ConfigDataType;

static void AVCE_ConfigLib(MSYS_CmdType *aCmd)
{
    int32_t ret;
    AVCE_ConfigDataType config;

    config.hdr = aCmd->cfgData;

    switch (aCmd->configID & 0xFFF0UL) {
    case MSYS_CONFIG_AVCE_MOD:
        if (((config.cfg->width - config.cfg->cropRight) <= AVCE_SYSTEM_VIDEO_WIDTH)
            && ((config.cfg->height - config.cfg->cropBottom) <= AVCE_SYSTEM_VIDEO_HEIGHT)) {
#ifdef AVCE_ENABLE_LOW_COMPLEXITY
            ret = AVCELib_Configure(&GAVCE_CompContext.context, config.cfg, 1U, AVCE_NUM_TEMP_BS_BUF);
#else
            ret = AVCELib_Configure(&GAVCE_CompContext.context, config.cfg, 0U, AVCE_NUM_TEMP_BS_BUF);
#endif
            if (BCM_ERR_OK != ret) {
                AVCE_ReportError(AVCE_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            aCmd->response = ret;
        }
    break;
    case MSYS_CONFIG_AVCE_FW0:
        ret = AVCELib_Boot(&GAVCE_CompContext.context, (uint16_t)MSYS_CONFIG_AVCE_FW0,
                            aCmd->cfgData, aCmd->size);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        aCmd->response = ret;
    break;
    default:
    break;
    }
}
/**
 *  @brief Init state handler
 *
 *  @details Init state handling
 *
 *  @return None
 */
static void AVCE_InitStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&AVCE_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCE_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            AVCE_ConfigLib(processCmd);

            if (GAVCE_CompContext.context.state == AVCE_LIB_STATE_IDLE) {
                GAVCE_CompContext.state = AVCE_STATE_READY;
            }
        }
        BCM_MemSet(&AVCE_YUVData, 0UL,sizeof(AVCE_YUVData));
        ret = MSGQ_CompleteMsgIdx(&AVCE_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

typedef struct _AVCE_BSBuffer{
    uint32_t cache;
    uint32_t cacheSpace;
    uint8_t  *buffer;
    uint32_t curIdx;
    uint32_t epbState;
} AVCE_BSBuffer;
/*
void AVCE_EPBWriteBits(AVCELib_BSBuffer *aBSBuffer,
                                                  uint32_t aVal,
                                                  uint32_t aSize)
Sync/Async  Sync
Reentrancy  Non-reentrant
Parameter   aBSBuffer (out) Bitstream buffer structure
    aVal (in)   Value to be written
    aSize (in)  Number of bits to be written(aVal shall be represented in aSize
    bits and written)
Return Value    void    -
Description Writes aVal, represented in aSize bits, into AVCELib_BSBuffer
*/
static void AVCE_EPBWriteBits(AVCE_BSBuffer *aBSBuffer,
                              uint32_t aVal,
                              uint32_t aSize)
{
    uint32_t len;
    uint32_t temp;
    while(0UL != aSize) {
        len = BCM_MIN(aBSBuffer->cacheSpace, aSize);
        aBSBuffer->cache = aBSBuffer->cache << len;
        aBSBuffer->cache |= aVal >> (aSize - len);
        aBSBuffer->cacheSpace -= len;
        while (8UL >= aBSBuffer->cacheSpace) {
            temp = aBSBuffer->cache >> (8UL - aBSBuffer->cacheSpace);
            if ((2UL == aBSBuffer->epbState) && (3UL <= temp)) {
                aBSBuffer->buffer[aBSBuffer->curIdx++] = 3U;
                aBSBuffer->epbState = 0UL;
            }
            if (0UL == temp) {
                aBSBuffer->epbState++;
            } else {
                aBSBuffer->epbState = 0UL;
            }
            aBSBuffer->buffer[aBSBuffer->curIdx++] = (uint8_t)temp;
            aBSBuffer->cacheSpace += 0x8UL;
        }
        aSize -= len;
    }
}

/** @brief Send SPS configuration

    The purpose of this API is to generate the SPS configuration Header as specified in H264 Specification
    and send it. This API shall be called at frame boundaries only.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]     void

    @return     void

    @post None

    @limitations
        1) Log2(Maximum Frame number) shall be set to 4
        2) POC type shall be set to 0
        3) Log2(Max POC) shall be set to 4
        4) SPS ID shall be set to 0
        5) Profile shall be set to Constrained Baseline
        6) Frame MBs only flag shall be set to 1
*/
static void AVCE_SendSPS(void)
{
    int32_t ret=BCM_ERR_OK;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;

    ret = MSGQ_GetMsgIdx(&AVCE_OutPort.dataQ, &mediaBufferIdx);
    if (BCM_ERR_OK == ret) {
        mediaBuffer = &AVCE_MediaBuffer[mediaBufferIdx];
        mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
        /* validate mediabuffer */
        if ((NULL != mediaBuffer->buffer[0UL].data) &&
            (32UL <= mediaBuffer->buffer[0UL].size)) {
            AVCE_BSBuffer bsBuffer;
            bsBuffer.cache = 0UL;
            bsBuffer.cacheSpace = 16UL;
            bsBuffer.buffer = mediaBuffer->buffer[0UL].data;
            bsBuffer.curIdx = 0UL;
            bsBuffer.epbState = 0UL;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 1U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0x67U; /* SPS */
            AVCE_EPBWriteBits(&bsBuffer, GAVCE_CompContext.context.profile, 8UL);
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* set0_flag*/
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* set1_flag*/
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL);/* set2_flag*/
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL);/* set3_flag*/
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 4UL);/* reserved */
            AVCE_EPBWriteBits(&bsBuffer, GAVCE_CompContext.context.level, 8UL);/* level */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* sps id*/
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* log2(maxFrmNum) - 4*/
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* POC type*/
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* log2(MaxPOCLsb) - 4*/
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* num_ref_frames*/
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL);/* gaps_allowed */
            AVCE_EPBWriteBits(&bsBuffer, (GAVCE_CompContext.context.width + 15UL) >> 4UL, 63UL - 2UL
                    * (uint32_t)__builtin_clz((GAVCE_CompContext.context.width + 15UL) >> 4UL)); /* width in MBs*/
            AVCE_EPBWriteBits(&bsBuffer, (GAVCE_CompContext.context.height + 15UL) >> 4UL, 63UL - 2UL
                    * (uint32_t)__builtin_clz((GAVCE_CompContext.context.height + 15UL) >> 4UL));/* height in MBs */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* MBs only */
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL); /*8x8 inference */
            if ((GAVCE_CompContext.context.cropRight != 0) || (GAVCE_CompContext.context.cropBottom != 0)) {
                AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);/* cropping */
                AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* crop left not supported*/
                if (GAVCE_CompContext.context.cropRight != 0) {
                    AVCE_EPBWriteBits(&bsBuffer, (1UL + (GAVCE_CompContext.context.cropRight / 2)),
                        63UL - 2UL * (uint32_t)__builtin_clz(1UL + (GAVCE_CompContext.context.cropRight / 2)));
                } else {
                    AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);
                }
                AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* crop top not supported*/
                if (GAVCE_CompContext.context.cropBottom != 0){
                    AVCE_EPBWriteBits(&bsBuffer, (1UL + (GAVCE_CompContext.context.cropBottom / 2)),
                        63UL - 2UL * (uint32_t)__builtin_clz(1UL + (GAVCE_CompContext.context.cropBottom / 2)));
                } else {
                    AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL);
                }
            } else {
                AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL);/* No cropping */
            }
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL);/* Vui_parameters_present_flag */
            AVCE_EPBWriteBits(&bsBuffer, 128UL, 8UL);/* RBSP Trailing bits */
            mediaBuffer->buffer[0UL].size = bsBuffer.curIdx;
            DCACHE_CleanRange((uint32_t)mediaBuffer->buffer[0UL].data,((bsBuffer.curIdx + 31UL) >> 5UL) << 5UL);
            mediaBuffer->timestamp = GAVCE_CompContext.timestamp;
            mediaBuffer->syncStart = 1UL;
            mediaBuffer->syncEnd = 1UL;
            mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = mediaBuffer->buffer[0UL].data[4]; /* first byte of NAL */
            mediaBuffer->frameEnd = 0UL;
            mediaBuffer->frameStart = 0UL;
            mediaBuffer->frameNum = GAVCE_CompContext.frameNum;
            AppendBS(mediaBuffer->buffer[0UL].data, mediaBuffer->buffer[0UL].size);
            /* submit this buffer */
            mediaBuffer->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&AVCE_OutPort.dataQ, mediaBufferIdx);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_SEND_SPS_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/** @brief Send PPS Configuration Stream

    The purpose of this API is to generate the PPS configuration Header as specified in H264 Specification
    and send it. This API shall be called at frame boundaries only

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]     void

    @return     void

    @post None

    @limitations
        1) PPS ID shall be set to 0
        2) Init QP-Y shall be set to 26
        3) Init QP-S shall be set to 26
        4) Init QP-C shall be set to 26
        5) Deblock Control shall be disabled
        6) FMO shall be disabled
        7) Entropy coding shall be CAVLC
        8) Field mode is disabled (only Frames are allowed)
*/
static void AVCE_SendPPS(void)
{
    int32_t ret=BCM_ERR_OK;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;

    ret = MSGQ_GetMsgIdx(&AVCE_OutPort.dataQ, &mediaBufferIdx);
    if (BCM_ERR_OK == ret) {
        mediaBuffer = &AVCE_MediaBuffer[mediaBufferIdx];
        mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
        /* validate mediabuffer */
        if ((NULL != mediaBuffer->buffer[0UL].data) &&
            (32UL <= mediaBuffer->buffer[0UL].size)) {
            AVCE_BSBuffer bsBuffer;
            bsBuffer.cache = 0UL;
            bsBuffer.cacheSpace = 16UL;
            bsBuffer.buffer = mediaBuffer->buffer[0UL].data;
            bsBuffer.curIdx = 0UL;
            bsBuffer.epbState = 0UL;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 1U;
            bsBuffer.buffer[bsBuffer.curIdx++] = 0x68U; /* PPS */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* pps id*/
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* sps id */
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL); /* CAVLC */
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL); /* POC flag */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* Disable FMO */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* ref idx l0 */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* ref idx l1 */
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL); /* wpred */
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 2UL); /* w-bipred */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* Init QP = 26 */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* Init QS = 26 */
            AVCE_EPBWriteBits(&bsBuffer, 1UL, 1UL); /* Chroma QP */
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL); /* dblk ctrl*/
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL); /* constrained intra pred*/
            AVCE_EPBWriteBits(&bsBuffer, 0UL, 1UL); /*Redundant pic count*/
            AVCE_EPBWriteBits(&bsBuffer, 128UL, 8UL);/* RBSP Trailing bits */
            mediaBuffer->buffer[0UL].size = bsBuffer.curIdx;
            DCACHE_CleanRange((uint32_t)mediaBuffer->buffer[0UL].data,((bsBuffer.curIdx + 31UL) >> 5UL) << 5UL);
            mediaBuffer->timestamp = GAVCE_CompContext.timestamp;
            mediaBuffer->syncStart = 1UL;
            mediaBuffer->syncEnd = 1UL;
            mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = mediaBuffer->buffer[0UL].data[4]; /* first byte of NAL */
            mediaBuffer->frameEnd = 0UL;
            mediaBuffer->frameStart = 0UL;
            mediaBuffer->frameNum = GAVCE_CompContext.frameNum;
            AppendBS(mediaBuffer->buffer[0UL].data, mediaBuffer->buffer[0UL].size);
            mediaBuffer->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&AVCE_OutPort.dataQ, mediaBufferIdx);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_SEND_PPS_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
static void AVCE_ReadyStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t retVal;
    int32_t ret = BCM_ERR_OK;
    uint32_t localCnt;
    AVCE_BufInfo bufInfo = INIT_AVCE_BUF_INFO;
    uint32_t i;

    ret = MSGQ_GetMsgIdx(&AVCE_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCE_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            do {
                localCnt = CIRCQ_Pop(&AVCE_CircQFill, (char *)&bufInfo,1UL);
            } while(1UL == localCnt);
            do {
                localCnt = CIRCQ_Pop(&AVCE_CIRCQSubmit, (char *)&bufInfo,1UL);
            } while(1UL == localCnt);
            do {
                localCnt = CIRCQ_Pop(&AVCE_CircQProcess, (char *)&bufInfo,1UL);
            } while(1UL == localCnt);
            do {
                localCnt = CIRCQ_Pop(&AVCE_CircQEmpty, (char *)&bufInfo,1UL);
            } while(1UL == localCnt);
            for(i=0UL;i<AVCE_TOTAL_YUV_BUFFERS;i++) {
                bufInfo.mbRowNum = 0UL;
                bufInfo.bufIdx = i;
                ret = CIRCQ_Push(&AVCE_CircQEmpty, (char *)&bufInfo,1UL);
                if(BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RESET_STATE_HANDLER_API_ID, ret, __LINE__, i, 0UL, 0UL);
                }
            }

            AVCE_SendSPS();
            AVCE_SendPPS();
#ifdef AVCE_InPortSe
            {
                WDS_ConfigType wdsCfg;
                uint32_t i;
                wdsCfg.mode = WDS_MODE_LOGICAL | WDS_MODE_DEADLINE | WDS_MODE_FREQ;
                wdsCfg.deadlineInMs = 1000UL/MEDIA_SYSTEM_MAX_VIDEO_FPS;
                wdsCfg.minFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) - 1UL;
                wdsCfg.maxFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) + 1UL;
                wdsCfg.lStart = 0UL;
                wdsCfg.lEnd = ((GAVCE_CompContext.context.height + 15UL) >> 4UL) - 1UL;
                BCM_MemSet(AVCE_InPortSe.buffer, WDS_INVALID_GRAPH_BUF_ENTRY,
                            AVCE_InPortSe.maxState * AVCE_InPortSe.maxTransitionsPerState);
                for (i = wdsCfg.lStart; i < wdsCfg.lEnd; i++) {
                    AVCE_InPortSe.buffer[i] = (WDS_StateType)(i + 1UL);
                }
                retVal = WDS_EnableSE(&AVCE_InPortSe, &wdsCfg);
                if (BCM_ERR_OK != retVal) {
                    /* Log error */
                }
            }
#endif
            GAVCE_CompContext.configBSInfo = 0UL;
            GAVCE_CompContext.filledSize = 0UL;
            retVal = BCM_SetEvent(AVCE_Task, AVCE_INPORT_EVENT|AVCE_OUTPORT_EVENT);
            if (BCM_ERR_OK != retVal) {
                AVCE_ReportError(AVCE_READY_STATE_HANDLER_API_ID, (int32_t)retVal, __LINE__, 0UL, 0UL, 0UL);
            }
            GAVCE_CompContext.state = AVCE_STATE_RUNNING;
            processCmd->response = BCM_ERR_OK;
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            ret = AVCELib_DeInit(&GAVCE_CompContext.context);
            if (BCM_ERR_OK != ret) {
                AVCE_ReportError(AVCE_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GAVCE_CompContext.state = AVCE_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            AVCE_ConfigLib(processCmd);
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        ret = MSGQ_CompleteMsgIdx(&AVCE_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

#if AVCE_SYSTEM_SLICE_HEIGHT == (16UL)

void AVCE_Encode(void)
{
    uint32_t localCnt;
    int32_t ret;
    AVCE_BufInfo bufInfo[AVCE_LIB_MAX_NUM_CORES] = {INIT_AVCE_BUF_INFO};
    AVCELib_YUVBuffer yuvBuffer[AVCE_LIB_MAX_NUM_CORES];


    localCnt = CIRCQ_Peek(&AVCE_CircQFill, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
    if (GAVCE_CompContext.context.numPPCores == localCnt) {
        uint32_t i;
        uint32_t numMBRows = 0UL;
        if(0UL != (bufInfo[0UL].mbRowNum % GAVCE_CompContext.context.numPPCores)) {
            /* take only one mb row */
            for (i=0UL;i<GAVCE_CompContext.context.numPPCores;i++) {
                if (AVCE_TOTAL_YUV_BUFFERS > bufInfo[0UL].bufIdx) {
                    numMBRows++;
                    yuvBuffer[i].lumaAddr = &AVCE_YUVData[bufInfo[0UL].bufIdx][0UL];
                    yuvBuffer[i].cbAddr = &AVCE_YUVData[bufInfo[0UL].bufIdx][((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 16UL];
                    yuvBuffer[i].crAddr = &AVCE_YUVData[bufInfo[0UL].bufIdx][(((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 16UL)
                                        + ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 8UL)];
                    yuvBuffer[i].lumaStride = (uint16_t)((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK);
                    yuvBuffer[i].chromaStride = (uint16_t)(((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK);
                    yuvBuffer[i].mbRow = (uint8_t)bufInfo[0UL].mbRowNum;
                    yuvBuffer[i].frameNo = (uint8_t)bufInfo[0UL].frameNum;
                    yuvBuffer[i].format = AVCE_LIB_YUV_420P; /*0 -420P : 1 - 420SP */
                    GAVCE_CompContext.timestamp = bufInfo[0UL].timestamp;
                }
            }
            yuvBuffer[0UL].mbRow--;
            /* try submitting it */
            ret = AVCELib_Encode(&GAVCE_CompContext.context, yuvBuffer, numMBRows, AVCE_LibIntBuf);
            if(BCM_ERR_OK == ret) {
                localCnt = CIRCQ_Pop(&AVCE_CircQFill, (char *)bufInfo,1UL);
                if (1UL != localCnt ) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, BCM_ERR_NOMEM, __LINE__, 0UL, 0UL, 0UL);
                }
                /* move the buffer to process q */
                bufInfo[1UL].bufIdx = AVCE_TOTAL_YUV_BUFFERS;
                ret = CIRCQ_Push(&AVCE_CircQProcess, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            } else if (BCM_ERR_EAGAIN != ret) {
                localCnt = CIRCQ_Pop(&AVCE_CircQFill, (char *)bufInfo,1UL);
                ret = CIRCQ_Push(&AVCE_CircQEmpty, (char *)bufInfo,1UL);
            }
        } else {
            for (i=0UL;i<GAVCE_CompContext.context.numPPCores;i++) {
                if (AVCE_TOTAL_YUV_BUFFERS > bufInfo[i].bufIdx) {
                    numMBRows++;
                    yuvBuffer[i].lumaAddr = &AVCE_YUVData[bufInfo[i].bufIdx][0UL];
                    yuvBuffer[i].cbAddr = &AVCE_YUVData[bufInfo[i].bufIdx][((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 16UL];
                    yuvBuffer[i].crAddr = &AVCE_YUVData[bufInfo[i].bufIdx][(((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 16UL)
                                        + ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 8UL)];
                    yuvBuffer[i].lumaStride = (uint16_t)((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK);
                    yuvBuffer[i].chromaStride = (uint16_t)(((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK);
                    yuvBuffer[i].mbRow = (uint8_t)bufInfo[i].mbRowNum;
                    yuvBuffer[i].frameNo = (uint8_t)bufInfo[i].frameNum;
                    yuvBuffer[i].format = AVCE_LIB_YUV_420P; /*0 -420P : 1 - 420SP */
                    GAVCE_CompContext.timestamp = bufInfo[i].timestamp;
                }
            }
            /* try submitting it */
            ret = AVCELib_Encode(&GAVCE_CompContext.context, yuvBuffer, numMBRows, AVCE_LibIntBuf);
            if(BCM_ERR_OK == ret) {
                localCnt = CIRCQ_Pop(&AVCE_CircQFill, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
                if (GAVCE_CompContext.context.numPPCores != localCnt ) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, BCM_ERR_NOMEM, __LINE__, 0UL, 0UL, 0UL);
                }
                /* move the buffer to process q */
                ret = CIRCQ_Push(&AVCE_CircQProcess, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            } else if (BCM_ERR_EAGAIN != ret) {
                localCnt = CIRCQ_Pop(&AVCE_CircQFill, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
                ret = CIRCQ_Push(&AVCE_CircQEmpty, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
            }
        }
    }
}
#else
void AVCE_Encode(void)
{
    uint32_t localCnt;
    int32_t ret;
    AVCE_BufInfo bufInfo = INIT_AVCE_BUF_INFO;
    AVCELib_YUVBuffer yuvBuffer[2UL];

    localCnt = CIRCQ_Peek(&AVCE_CircQFill, (char *)&bufInfo,1UL);
    if (1UL == localCnt ) {
        uint32_t i;
        uint32_t numMbRows;
        uint32_t numMBRowsSubmitted;

        numMBRowsSubmitted = AVCE_YUVDataStatus[bufInfo.bufIdx].numMBRowsSubmitted;
        /* try submitting it */
        yuvBuffer[0UL].lumaAddr = &AVCE_YUVData[bufInfo.bufIdx][0UL];
        yuvBuffer[0UL].cbAddr = &AVCE_YUVData[bufInfo.bufIdx][((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * AVCE_SYSTEM_SLICE_HEIGHT];
        yuvBuffer[0UL].crAddr = &AVCE_YUVData[bufInfo.bufIdx][(((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * AVCE_SYSTEM_SLICE_HEIGHT)
                            + ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * (AVCE_SYSTEM_SLICE_HEIGHT / 2UL))];
        yuvBuffer[0UL].lumaStride = (uint16_t)((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK);
        yuvBuffer[0UL].chromaStride = (uint16_t)(((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK);
        yuvBuffer[0UL].mbRow = (uint8_t)bufInfo.mbRowNum;
        yuvBuffer[0UL].frameNo = bufInfo.frameNum;
        yuvBuffer[0UL].format = AVCE_LIB_YUV_420P; /*0 -420P : 1 - 420SP */

        numMbRows = bufInfo.numMbRows - numMBRowsSubmitted;
        if (numMbRows > GAVCE_CompContext.context.numPPCores) {
            numMbRows = GAVCE_CompContext.context.numPPCores;
        }

        if (1UL == numMbRows) {
            /* offset the address */
            yuvBuffer[0UL].lumaAddr += (((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 16UL) * numMBRowsSubmitted;
            yuvBuffer[0UL].cbAddr += ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 8UL) * numMBRowsSubmitted;
            yuvBuffer[0UL].crAddr += ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 8UL) * numMBRowsSubmitted;
            yuvBuffer[0UL].mbRow += numMBRowsSubmitted;
        } else {
            for (i=1UL;i<numMbRows;i++) {
                /* offset the address */
                yuvBuffer[i].lumaAddr =  yuvBuffer[i - 1UL].lumaAddr + (((GAVCE_CompContext.context.width + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 16UL);
                yuvBuffer[i].cbAddr = yuvBuffer[i - 1UL].cbAddr + ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 8UL);
                yuvBuffer[i].crAddr = yuvBuffer[i - 1UL].crAddr + ((((GAVCE_CompContext.context.width / 2UL) + AVCE_STRIDE_ALIGN_MASK) & ~AVCE_STRIDE_ALIGN_MASK) * 8UL);
                yuvBuffer[i].lumaStride = yuvBuffer[i - 1UL].lumaStride;
                yuvBuffer[i].chromaStride = yuvBuffer[i - 1UL].chromaStride;
                yuvBuffer[i].mbRow = yuvBuffer[i - 1UL].mbRow +  1UL;
                yuvBuffer[i].frameNo = yuvBuffer[i - 1UL].frameNo;
            }
        }

        GAVCE_CompContext.timestamp = bufInfo.timestamp;
        ret = AVCELib_Encode(&GAVCE_CompContext.context, yuvBuffer, numMbRows, AVCE_LibIntBuf);
        if(BCM_ERR_OK == ret) {
            AVCE_YUVDataStatus[bufInfo.bufIdx].numMBRowsSubmitted += numMbRows;
            GAVCE_CompContext.numMBRowsSubmitted = numMbRows;
            if (bufInfo.numMbRows == AVCE_YUVDataStatus[bufInfo.bufIdx].numMBRowsSubmitted) {
                localCnt = CIRCQ_Pop(&AVCE_CircQFill, (char *)&bufInfo,1UL);
                if (1UL != localCnt ) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, BCM_ERR_NOMEM, __LINE__, 0UL, 0UL, 0UL);
                }
                /* move the buffer to process q */
                ret = CIRCQ_Push(&AVCE_CircQProcess, (char *)&bufInfo,1UL);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        } else if (BCM_ERR_EAGAIN != ret) {
            localCnt = CIRCQ_Pop(&AVCE_CircQFill, (char *)&bufInfo,1UL);
            ret = CIRCQ_Push(&AVCE_CircQEmpty, (char *)&bufInfo,1UL);
        }
    }
}
#endif

/**
 *  @brief Run state handler
 *
 *  @details Run state handling
 *
 *  @return None
 */
static void AVCE_RunStateHandler(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t localCnt;
    uint32_t flags;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;
    uint32_t cmdMsgIdx = 0xFFFFFFFFUL;
    MSYS_CmdType *processCmd = NULL;
    uint32_t size;

    ret = MSGQ_GetMsgIdx(&AVCE_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCE_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            GAVCE_CompContext.stopInProgress = 1UL;
        }
    }

    flags = AVCELib_Process(&GAVCE_CompContext.context);
    if (0UL != (flags & AVCE_LIB_MBROW_PROCESSED)) {
#if AVCE_SYSTEM_SLICE_HEIGHT == (16UL)
        AVCE_BufInfo bufInfo[AVCE_LIB_MAX_NUM_CORES] = {INIT_AVCE_BUF_INFO};
        localCnt = CIRCQ_Pop(&AVCE_CircQProcess, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
        if (GAVCE_CompContext.context.numPPCores == localCnt ) {
            ret = CIRCQ_Push(&AVCE_CircQEmpty, (char *)bufInfo,GAVCE_CompContext.context.numPPCores);
            if (BCM_ERR_OK != ret) {
                AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
        }
#else
        AVCE_BufInfo bufInfo = INIT_AVCE_BUF_INFO;
        (void)CIRCQ_Peek(&AVCE_CircQProcess, (char *)&bufInfo,1UL);
        AVCE_YUVDataStatus[bufInfo.bufIdx].numMBRowsProcessed += GAVCE_CompContext.numMBRowsSubmitted;
        GAVCE_CompContext.numMBRowsSubmitted = 0UL;
        if (bufInfo.numMbRows == AVCE_YUVDataStatus[bufInfo.bufIdx].numMBRowsProcessed) {
            localCnt = CIRCQ_Pop(&AVCE_CircQProcess, (char *)&bufInfo,1UL);
            if (1UL == localCnt ) {
                ret = CIRCQ_Push(&AVCE_CircQEmpty, (char *)&bufInfo,1UL);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        }
#endif
    }
    if (0UL != (flags & AVCE_LIB_BITSTREAM_FILLED)) {
        ret = AVCELib_GetBitStreamSize(&GAVCE_CompContext.context, &size);
        if (BCM_ERR_OK == ret) {
            ret = MSGQ_GetMsgIdx(&AVCE_OutPort.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == ret) {
                mediaBuffer = &AVCE_MediaBuffer[mediaBufferIdx];
                mediaBuffer->response = BCM_ERR_OK;
                mediaBuffer->timestamp = GAVCE_CompContext.timestamp;
                mediaBuffer->frameEnd = 0UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->syncStart = 0UL;
                mediaBuffer->syncEnd = 0UL;
                mediaBuffer->frameNum = GAVCE_CompContext.frameNum;
                if (0UL != (flags & AVCE_LIB_START_OF_SLICE)) {
                    mediaBuffer->syncStart = 1UL;
                    if (0UL != (flags & AVCE_LIB_START_OF_FRAME)) {
                        mediaBuffer->frameStart = 1UL;
                    }
                }
                if (0UL != (flags & AVCE_LIB_END_OF_SLICE)) {
                    mediaBuffer->syncEnd = 1UL;
                    if (0UL != (flags & AVCE_LIB_END_OF_FRAME)) {
                        mediaBuffer->frameEnd = 1UL;
                        GAVCE_CompContext.frameNum  = 0xFFFFFFFFUL;
                    }
                }
                mediaBuffer->buffer[0UL].size = size;
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x65U;
                AppendBS(mediaBuffer->buffer[0UL].data, mediaBuffer->buffer[0UL].size);
                ret = MSGQ_CompleteMsgIdx(&AVCE_OutPort.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        }
    }

    if (0UL != (flags & AVCE_LIB_END_OF_FRAME)) {
        if (0UL == GAVCE_CompContext.configBSInfo) {
            AVCE_SendSPS();
            GAVCE_CompContext.configBSInfo = 1UL;
        }
        else {
            AVCE_SendPPS();
            GAVCE_CompContext.configBSInfo = 0UL;
        }
    }

    if (0UL != (flags & AVCE_LIB_BITSTREAM_BUFFER)) {
        ret = MSGQ_GetMsgIdx(&AVCE_OutPort.dataQ, &mediaBufferIdx);
        if (BCM_ERR_OK == ret) {
            mediaBuffer = &AVCE_MediaBuffer[mediaBufferIdx];
            mediaBuffer->response = BCM_ERR_OK;
            ret = AVCELib_FillBitStream(&GAVCE_CompContext.context,
                                        mediaBuffer->buffer[0UL].data,
                                        mediaBuffer->buffer[0UL].size);
            if ((BCM_ERR_OK != ret) && (BCM_ERR_EAGAIN != ret)) {
                AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
        }
    }

    ret = MSYS_InPortProcess(&AVCE_InPort);
    if ((BCM_ERR_OK != ret) &&  (BCM_ERR_BUSY != ret) && (BCM_ERR_NOMEM != ret)) {
        AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    AVCE_Encode();
    if(0UL == GAVCE_CompContext.stopInProgress) {
        AVCE_ProcessInPort(&AVCE_InPort);
    } else {
        if (AVCE_LIB_STATE_IDLE == GAVCE_CompContext.context.state){
            ret = MSYS_InPortCancelAll(&AVCE_InPort);
            if ((BCM_ERR_OK != ret) && (BCM_ERR_CANCELLED != ret)) {
                AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GAVCE_CompContext.stopInProgress = 0UL;
            GAVCE_CompContext.stopFlushIssued = 0UL;
            ret = MSGQ_GetMsgIdx(&AVCE_OutPort.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == ret) {
                mediaBuffer = &AVCE_MediaBuffer[mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                mediaBuffer->frameNum = GAVCE_CompContext.frameNum;
                ret = MSGQ_CompleteMsgIdx(&AVCE_OutPort.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
#ifdef AVCE_InPortSe
            {
                ret = WDS_DisableSE(&AVCE_InPortSe);
                if (BCM_ERR_OK != ret) {
                    /* Log error */
                }
            }
#endif
            GAVCE_CompContext.state = AVCE_STATE_READY;
            if (cmdMsgIdx < MSGQ_SIZE) {
                AVCE_CmdMsgQ[cmdMsgIdx].response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&AVCE_CmdQ, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    AVCE_ReportError(AVCE_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
            GAVCE_CompContext.state = AVCE_STATE_READY;
        }
    }
}

TASK(AVCE_Task)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;

    do {
        ret = BCM_WaitEvent(AVCE_CMDQ_SERVER_EVENT|AVCE_DATA_MAILBOX_EVENT|AVCE_INPORT_EVENT|AVCE_OUTPORT_EVENT);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(AVCE_Task,&mask);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            AVCE_ReportError(AVCE_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }

        switch(GAVCE_CompContext.state) {
        case AVCE_STATE_RESET:
            AVCE_ResetStateHandler();
            break;
        case AVCE_STATE_INIT:
            AVCE_InitStateHandler();
            break;
        case AVCE_STATE_READY:
            AVCE_ReadyStateHandler();
            break;
        case AVCE_STATE_RUNNING:
            AVCE_RunStateHandler();
            break;
        case AVCE_STATE_ERROR:
            break;
        default:
            break;
        }
    } while(AVCE_STATE_RESET != GAVCE_CompContext.state);

    ret = BCM_TerminateTask();
    AVCE_ReportError(AVCE_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
}
