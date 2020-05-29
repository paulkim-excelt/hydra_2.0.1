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
File Name  : venc_comp.c
Description: This file contains video encoder component that can be used as a
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
#include <venc.h>
#include <venc_lib.h>
#include <imgl.h>
#include "mcu.h"

#include <cache.h>
#include <compiler.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_INFO

static void VENC_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_VEN_ID, 0U, aApiID, aErr, 4UL, values);
}

#define VENC_SEND_CMD_API_ID                (0xC0U) /**< VENC_SendCmd */
#define VENC_RECV_RESP_API_ID               (0xC1U) /**< VENC_RecvResp */
#define VENC_GET_MEDIA_OUTPORT_API_ID       (0xC2U) /**< VENC_GetMediaOutPort */
#define VENC_GET_MEDIA_INPORT_API_ID        (0xC3U) /**< VENC_GetMediaInPort */

#define VENC_FILL_BUFFER_DONE_API_ID        (0x83U) /* VENC_FillBufferDone */
#define VENC_GET_CMD_MSG_API_ID             (0x84U) /* VENC_GetCmdMsg */
#define VENC_GET_DATA_MSG_API_ID            (0x85U) /* VENC_GetDataMsg */
#define VENC_PROCESS_INPORT_API_ID          (0x86U) /* VENC_ProcessInPort */
#define VENC_SEND_SPS_API_ID                (0x87U) /* VENC_SendSPS */
#define VENC_SEND_PPS_API_ID                (0x87U) /* VENC_SendPPS */


#define VENC_RESET_STATE_HANDLER_API_ID     (0x40U) /* VENC_ResetStateHandler */
#define VENC_INIT_STATE_HANDLER_API_ID      (0x41U) /* VENC_InitStateHandler */
#define VENC_READY_STATE_HANDLER_API_ID     (0x42U) /* VENC_ReadyStateHandler */
#define VENC_RUN_STATE_HANDLER_API_ID       (0x43U) /* VENC_RunStateHandler */
#define VENC_TASK_API_ID                    (0x44U) /* VENC_Task */


#define VENC_STATE_RESET   (0UL)
#define VENC_STATE_INIT    (1UL)
#define VENC_STATE_READY   (2UL)
#define VENC_STATE_RUNNING (3UL)
#define VENC_STATE_ERROR   (4UL)


#define VENC_CIRCQ_LOG2_SIZE        (4UL)
#define VENC_BSINFO_NUM_BUFFERS     (4UL)


#ifdef VENC_ENABLE_P_FRAME
#define VENC_TEMP_MEM_SIZE          ((192UL * 1024UL) + 1024UL + (4UL * MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT * 3UL / 2UL) + (3UL * MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT))
#ifndef VENC_ENC_CHUNK_SIZE
#define VENC_ENC_CHUNK_SIZE         (512UL * 1024UL)
#endif
#else
#define VENC_TEMP_MEM_SIZE          ((128UL * 1024UL) + 1024UL + (4UL * MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL * 3UL / 2UL))
#ifndef VENC_ENC_CHUNK_SIZE
#define VENC_ENC_CHUNK_SIZE         (4864UL)
#endif
#endif


#ifndef VENC_TOTAL_YUV_BUFFERS
#define VENC_TOTAL_YUV_BUFFERS      (2UL)
#endif

#define VENC_ENC_BS_SIZE            (8UL * VENC_ENC_CHUNK_SIZE)

typedef struct _VENC_CompContext {
    uint32_t state;
    uint32_t stopInProgress;
    uint32_t stopFlushIssued;
    uint64_t timestamp;
    uint32_t frameNum;
    uint32_t width;
    uint32_t height;
    IMGL_LoadStatusType imglStatus;
    VENCLib_Context context;
}VENC_CompContext;


typedef struct _VENC_BufInfo
{
    uint64_t timestamp;
    uint32_t mbRowNum;
    uint32_t frameNum;
    uint32_t width;
    uint32_t height;
} VENC_BufInfo;

static VENC_BufInfo GVENC_BufInfo[VENC_TOTAL_YUV_BUFFERS];

CIRCQ_V2_DEFINE(VENC_CIRCQFill, VENC_CIRCQ_LOG2_SIZE, uint8_t,
                     VENC_CIRCQFillBuffer, VENC_CIRCQFillIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

CIRCQ_V2_DEFINE(VENC_CIRCQSubmit, VENC_CIRCQ_LOG2_SIZE, uint8_t,
                     VENC_CIRCQSubmitBuffer, VENC_CIRCQSubmitIdx, CIRCQ_OVERWRITE_DISABLE, ".data");


typedef struct _VENC_BSChunkInfo
{
    uint8_t *addr;
    uint32_t offset;
    uint32_t size;
    uint64_t timestamp;
    uint32_t frameNum;
    uint32_t syncStart;
    uint32_t syncEnd;
    uint16_t frameStart;
    uint16_t frameEnd;
    uint8_t strInfo;
} VENC_BSChunkInfo;


static VENC_BSChunkInfo GVENC_BSChunkInfo[VENC_BSINFO_NUM_BUFFERS];

CIRCQ_V2_DEFINE(VENC_CIRCQBSFill, VENC_CIRCQ_LOG2_SIZE, uint8_t,
                     VENC_CIRCQBSFillBuffer, VENC_CIRCQBSFillIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

CIRCQ_V2_DEFINE(VENC_CIRCQBSEmpty, VENC_CIRCQ_LOG2_SIZE, uint8_t,
                     VENC_CIRCQBSEmptyBuffer, VENC_CIRCQBSEmptyIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

static VENC_CompContext GVENC_CompContext;

#define VENC_PP_BINARY_DATA_SIZE (64UL*1024UL)
const MSGQ_MsgHdrType* GPPBinaryHdr;

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.slowmem.VENC_TempMem") VENC_TempMem[VENC_TEMP_MEM_SIZE];

/* declare YUV buffers */
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.VENC_YUVData") VENC_YUVData[VENC_TOTAL_YUV_BUFFERS][ MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL * 3UL / 2UL];

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.cached.EncBuffer") VENC_EncBuffer[VENC_ENC_BS_SIZE];

static
uint8_t COMP_SECTION(".pp.cached.BSChunks") GVENC_BSChunk[VENC_BSINFO_NUM_BUFFERS][VENC_ENC_CHUNK_SIZE];


/**
 *  @brief Call back API for fill Buffer request
 *
 *  @details This API will be called when fill buffer request is served
 *
 *  @param [in] aMediaBuffer Media buffer structure pointer
 *
 *  @return None
 */
static void VENC_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t bufIdx;

    if (0UL != aMediaBuffer->buffer[0].size) {
        bufIdx = aMediaBuffer->bufIdx;
        GVENC_BufInfo[bufIdx].timestamp = aMediaBuffer->timestamp;
        GVENC_BufInfo[bufIdx].frameNum = aMediaBuffer->frameNum;
        GVENC_BufInfo[bufIdx].mbRowNum =  aMediaBuffer->seqNum;

        /* push it to fill queue */
        retVal = CIRCQ_Push(&VENC_CIRCQFill,(char*)(&bufIdx),1UL);
        if (BCM_ERR_OK != retVal) {
            VENC_ReportError(VENC_FILL_BUFFER_DONE_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

MSYS_DECLARE_IN_PORT(VENC_InPort, 0UL, MSYS_COMP_ID_VENC, SystemEvent4, VENC_InPortRW, VENC_FillBufferDone);


/* Compoenent CmdQ Server Event */
#define VENC_CMDQ_SERVER_EVENT            SystemEvent0
#define VENC_PTM_SERVER_EVENT             SystemEvent1
#define VENC_INPORT_EVENT                 SystemEvent4
#define VENC_OUTPORT_EVENT                SystemEvent8
#define VENC_DATA_MAILBOX_EVENT           PP_GPEvent

extern MSYS_CmdType VENC_CmdMsgQ[];
MSYS_CmdType VENC_CmdMsgQ[MSGQ_SIZE];
/**
 *  @brief Get command message
 *
 *  @details This API is used by the framework to query command message
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to command message structure
 */
static void* VENC_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&VENC_CmdMsgQ[i];
    }

    return cmdMsg;
}

extern MSYS_BufferType VENC_MediaBuffer[];

MSYS_BufferType VENC_MediaBuffer[MSGQ_SIZE];
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
static void* VENC_GetDataMsg(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&VENC_MediaBuffer[i];
    }

    return dataMsg;
}
static MSGQ_HdrQType VENC_CmdMsgHdrQ = MSGQ_INIT_HDRQ;
extern const MSGQ_Type VENC_CmdQ;
const MSGQ_Type VENC_CmdQ = MSGQ_INIT(VENC_Task, VENC_CMDQ_SERVER_EVENT,
           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &VENC_CmdMsgHdrQ, VENC_GetCmdMsg);

MSYS_DECLARE_OUT_PORT(VENC_OutPort, 0UL, VENC_DataMSGQ_HdrQType,
                        VENC_Task, VENC_GetDataMsg,
                        MSYS_COMP_ID_VENC, SystemEvent8);

#define VENC_MAX_NUM_OUT_PORT  1UL

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
int32_t VENC_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_VENC == aCmd->compID)
        && (NULL != aHdr)) {
        if (aCmd->cmd == MSYS_COMP_CMD_INIT) {
            retVal = BCM_ActivateTask(VENC_Task);
            if (BCM_ERR_OK != retVal) {
                VENC_ReportError(VENC_SEND_CMD_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        ret = MSGQ_SendMsg(&VENC_CmdQ, (const void * const)aCmd, aMask,
                            aHdr);
    } else {
        VENC_ReportError(VENC_SEND_CMD_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
int32_t VENC_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        ret = MSGQ_RecvMsg(&VENC_CmdQ, aHdr, (void* const) aCmd);
    } else {
        VENC_ReportError(VENC_RECV_RESP_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}
/**
 *  @brief Get the num of outports of venc component
 *
 *  @details Get the number of media outports of venc component
 *
 *  @return Number of media outports of venc component
 *
 */
uint32_t VENC_GetNumMediaOutPort(void)
{
    return VENC_MAX_NUM_OUT_PORT;
}

/** @brief Get the MSYS_OutPortType where the message needs to be posted to
 *
 * @details Get the MSYS_OutPortType where the message needs to be posted to (triggered
 * when the sink component requests venc component to fill the media buffer)
 *
 * @param[in]   aIndex          Port Index
 *
 * @retval      NULL aIndex is not valid
 * @retval      pointer to the media outport
 */
const MSYS_OutPortType* VENC_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        outPort = &VENC_OutPort;
        break;
    default:
        VENC_ReportError(VENC_GET_MEDIA_OUTPORT_API_ID, BCM_ERR_INVAL_PARAMS,
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
static const MSYS_InPortType* VENC_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        inPort = &VENC_InPort;
        break;
    default:
        VENC_ReportError(VENC_GET_MEDIA_INPORT_API_ID, BCM_ERR_INVAL_PARAMS,
                        __LINE__, 0UL, 0UL, 0UL);
        break;
    }

    return inPort;
}

/* Component function table for venc */
static const MSYS_CompFnTblType VENC_CompFnTable =
{
    VENC_SendCmd,
    VENC_RecvResp,
    VENC_GetMediaOutPort,
    VENC_GetMediaInPort,
};

const MSYS_CompFnTblType* VENC_GetCompFnTable(void)
{
    return &VENC_CompFnTable;
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
static void VENC_ProcessInPort(const MSYS_InPortType * const aInPort,uint32_t aBufIdx)
{
    int32_t  ret;

    if (VENC_TOTAL_YUV_BUFFERS > aBufIdx) {
        const MSYS_BufferType mediaBuffer = {
            .frameNum = 0UL,
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
            .numChannels = 3UL,
            .bufIdx = aBufIdx,

            .seqNum = 0UL,

            .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
            .buffer[0].size = GVENC_CompContext.context.width * 16UL,
            .buffer[0].data = &VENC_YUVData[aBufIdx][0UL],
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GVENC_CompContext.context.width,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 16UL,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GVENC_CompContext.context.width,


            .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
            .buffer[1].size = (GVENC_CompContext.context.width / 2UL) * 8UL,
            .buffer[1].data = &VENC_YUVData[aBufIdx][GVENC_CompContext.context.width * 16UL],
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GVENC_CompContext.context.width / 2UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 16UL / 2UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GVENC_CompContext.context.width / 2UL,

            .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
            .buffer[2].size = (GVENC_CompContext.context.width / 2UL) * 8UL,
            .buffer[2].data = &VENC_YUVData[aBufIdx][(GVENC_CompContext.context.width * 16UL) + (GVENC_CompContext.context.width * 4UL)],
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GVENC_CompContext.context.width / 2UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 16UL / 2UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GVENC_CompContext.context.width / 2UL,

        };

        ret = MSYS_FillBuffer(aInPort, &mediaBuffer);
        if (BCM_ERR_OK != ret) {
            VENC_ReportError(VENC_PROCESS_INPORT_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
static void VENC_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;
    uint32_t localCnt;
    uint8_t bufIdx = 0xFF;
    uint8_t bsBufIdx = 0xFF;

    ret = MSGQ_GetMsgIdx(&VENC_CmdQ, &cmdMsgIdx);
    GVENC_CompContext.frameNum  = 0xFFFFFFFFUL;
    if (BCM_ERR_OK == ret) {
        processCmd = &VENC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            GVENC_CompContext.timestamp = 0ULL;
            GVENC_CompContext.stopInProgress = 0UL;
            GVENC_CompContext.stopFlushIssued = 0UL;
            GVENC_CompContext.state = VENC_STATE_INIT;
            GVENC_CompContext.imglStatus.hdr = NULL;
            GVENC_CompContext.context.state = VENC_LIB_STATE_RESET;
            ret = VENCLib_Init(&GVENC_CompContext.context);
            if(BCM_ERR_OK != ret) {
                VENC_ReportError(VENC_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            ret = MSYS_OutPortInit(&VENC_OutPort);
            if(BCM_ERR_OK != ret) {
                VENC_ReportError(VENC_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            do {
                localCnt = CIRCQ_Pop(&VENC_CIRCQFill, (char *)&bufIdx,1UL);
            } while(1UL == localCnt);
            do {
                localCnt = CIRCQ_Pop(&VENC_CIRCQSubmit, (char *)&bufIdx,1UL);
            } while(1UL == localCnt);
            do {
                localCnt = CIRCQ_Pop(&VENC_CIRCQBSFill, (char *)&bsBufIdx,1UL);
            } while(1UL == localCnt);
            do {
                localCnt = CIRCQ_Pop(&VENC_CIRCQBSEmpty, (char *)&bsBufIdx,1UL);
            } while(1UL == localCnt);
            processCmd->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&VENC_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            VENC_ReportError(VENC_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

typedef union _VENC_ConfigDataType {
    const uint8_t* hdr;
    const VENCLib_Config *cfg;
} VENC_ConfigDataType;

/**
 *  @brief Init state handler
 *
 *  @details Init state handling
 *
 *  @return None
 */
static void VENC_InitStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;
    VENC_ConfigDataType config;

    ret = MSGQ_GetMsgIdx(&VENC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &VENC_CmdMsgQ[cmdMsgIdx];
        config.hdr = processCmd->cfgData;
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            switch (processCmd->configID & 0xFFF0) {
            case MSYS_CONFIG_VENC_MOD:
                if ((config.cfg->width <= MEDIA_SYSTEM_MAX_VIDEO_WIDTH)
                    && (config.cfg->height <= MEDIA_SYSTEM_MAX_VIDEO_HEIGHT)) {
                    ret = VENCLib_Configure(&GVENC_CompContext.context, config.cfg,0UL,VENC_TempMem,VENC_EncBuffer,VENC_ENC_BS_SIZE);
                    if(BCM_ERR_OK != ret) {
                        VENC_ReportError(VENC_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                    }
                    processCmd->response = ret;
                }
            break;
            case MSYS_CONFIG_VENC_FW0:
                ret = VENCLib_Boot(&GVENC_CompContext.context,VENC_LIB_FW0_ID,processCmd->cfgData,processCmd->size,VENC_TempMem);
                if (BCM_ERR_OK != ret) {
                    VENC_ReportError(VENC_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
                processCmd->response = ret;
            break;
            case MSYS_CONFIG_VENC_FW1:
                if (GVENC_CompContext.imglStatus.hdr == NULL) {
                    ret = IMGL_LoadImg((uint16_t)processCmd->configID, VENC_TempMem + 1024, 0UL,
                                        VENC_PP_BINARY_DATA_SIZE, VENC_PTM_SERVER_EVENT, &GVENC_CompContext.imglStatus);
                    if (BCM_ERR_OK == ret) {
                        processCmd->response = BCM_ERR_BUSY;
                    } else {
                        processCmd->response = BCM_ERR_UNKNOWN;
                    }
                } else {
                    uint32_t lSize = 0UL;
                    int32_t ptmStatus;
                    ptmStatus = IMGL_GetStatus(&GVENC_CompContext.imglStatus);
                    if (ptmStatus == BCM_ERR_NOMEM) {
                        GVENC_CompContext.imglStatus.hdr = NULL;
                        processCmd->response = BCM_ERR_NOMEM;
                    } else if (ptmStatus == BCM_ERR_BUSY) {
                        /* Do Nothing */
                        processCmd->response = BCM_ERR_BUSY;
                    } else if (ptmStatus == BCM_ERR_OK) {
                        lSize = GVENC_CompContext.imglStatus.size;
                        GVENC_CompContext.imglStatus.hdr = NULL;
                        ret = VENCLib_Boot(&GVENC_CompContext.context,VENC_LIB_FW1_ID,VENC_TempMem + 1024,lSize,VENC_TempMem);
                        if (BCM_ERR_OK != ret) {
                            VENC_ReportError(VENC_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                        }
                        processCmd->response = BCM_ERR_OK;
                    } else {
                        GVENC_CompContext.imglStatus.hdr = NULL;
                        processCmd->response = BCM_ERR_UNKNOWN;
                    }
                }
            break;
            default:
            break;
            }
            if (GVENC_CompContext.context.state == VENC_LIB_STATE_IDLE) {
                GVENC_CompContext.state = VENC_STATE_READY;
            }
        }
        if (BCM_ERR_BUSY != processCmd->response) {
            ret = MSGQ_CompleteMsgIdx(&VENC_CmdQ, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                VENC_ReportError(VENC_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
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
static void VENC_ReadyStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t retVal;
    int32_t ret = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&VENC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &VENC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            uint32_t i;
            /* BS info buffers */
            for (i = 0UL; i < VENC_BSINFO_NUM_BUFFERS; i++) {
                GVENC_BSChunkInfo[i].addr = &GVENC_BSChunk[i][0UL];
                GVENC_BSChunkInfo[i].offset = 0UL;
                ret = CIRCQ_Push(&VENC_CIRCQBSEmpty,(char*)(&i),1UL);
                if (BCM_ERR_OK != ret) {
                    VENC_ReportError(VENC_READY_STATE_HANDLER_API_ID, ret, __LINE__, i, 0UL, 0UL);
                }
            }
            /* submit buffers */
            for (i = 0UL; i < VENC_TOTAL_YUV_BUFFERS; i++) {
                VENC_ProcessInPort(&VENC_InPort,i);
            }

            retVal = BCM_SetEvent(VENC_Task, VENC_INPORT_EVENT|VENC_OUTPORT_EVENT);
            if (BCM_ERR_OK != retVal) {
                VENC_ReportError(VENC_READY_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
            GVENC_CompContext.state = VENC_STATE_RUNNING;
            processCmd->response = BCM_ERR_OK;
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            ret = VENCLib_DeInit(&GVENC_CompContext.context);
            if (BCM_ERR_OK != ret) {
                VENC_ReportError(VENC_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GVENC_CompContext.state = VENC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        ret = MSGQ_CompleteMsgIdx(&VENC_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            VENC_ReportError(VENC_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/**
 *  @brief Run state handler
 *
 *  @details Run state handling
 *
 *  @return None
 */
static void VENC_RunStateHandler(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t cnt;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;
    uint32_t cmdMsgIdx = 0xFFFFFFFFUL;
    MSYS_CmdType *processCmd = NULL;
    uint32_t flags;
    uint32_t localCnt;
    VENCLib_YUVBuffer yuvBuffer;
    VENCLib_BitStream bsInfo;
    uint8_t bufIdx = 0xFF;

    uint8_t bsBufIdx = 0xFF;

    ret = MSGQ_GetMsgIdx(&VENC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &VENC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            GVENC_CompContext.stopInProgress = 1UL;
        }
    }

    bsInfo.bsBuffer = VENC_EncBuffer;
    do {
        flags = VENCLib_Process(&GVENC_CompContext.context, VENC_TempMem, &bsInfo);
        if (0UL != (flags & VENC_LIB_MBROW_PROCESSED)) {
            localCnt = CIRCQ_Pop(&VENC_CIRCQSubmit, (char *)&bufIdx,1UL);
            if (1UL == localCnt ) {
                VENC_ProcessInPort(&VENC_InPort,bufIdx);
            }
        }
        if (0UL != (flags & VENC_LIB_BITSTREAM_FILLED)) {
            if (VENC_ENC_CHUNK_SIZE >= bsInfo.size) {
                /* push the details to info Q */
                localCnt = CIRCQ_Pop(&VENC_CIRCQBSEmpty, (char *)&bsBufIdx,1UL);
                if (1UL == localCnt ) {
                    /* take the data to cir Q buffer */
                    DCACHE_InvalRange(((uint32_t)(VENC_EncBuffer + bsInfo.offset)) & 0xFFFFFFE0, (bsInfo.size + 31) & (~31));
                    /* push the details to info Q */
                    BCM_MemCpy(GVENC_BSChunkInfo[bsBufIdx].addr,VENC_EncBuffer + bsInfo.offset,bsInfo.size);
                    /* Fill the details */
                    GVENC_BSChunkInfo[bsBufIdx].timestamp = GVENC_CompContext.timestamp;
                    GVENC_BSChunkInfo[bsBufIdx].frameNum = GVENC_CompContext.frameNum;
                    GVENC_BSChunkInfo[bsBufIdx].size = bsInfo.size;
                    GVENC_BSChunkInfo[bsBufIdx].offset = 0UL;
                    GVENC_BSChunkInfo[bsBufIdx].frameStart = bsInfo.frameStart;
                    GVENC_BSChunkInfo[bsBufIdx].frameEnd = bsInfo.frameEnd;
                    GVENC_BSChunkInfo[bsBufIdx].syncStart = 1UL;
                    GVENC_BSChunkInfo[bsBufIdx].syncEnd = 1UL;
                    GVENC_BSChunkInfo[bsBufIdx].strInfo = *(VENC_EncBuffer + bsInfo.offset + 4UL);
                    ret = CIRCQ_Push(&VENC_CIRCQBSFill,(char *)&bsBufIdx, 1UL);
                    if (BCM_ERR_OK != ret) {
                        VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                    }
                } else {
                    VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            } else {
                /* No space */
                VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, bsInfo.size, 0UL, 0UL);
            }
        }
    } while (flags != 0);

    localCnt = CIRCQ_Peek(&VENC_CIRCQFill, (char *)&bufIdx,1UL);
    if (1UL == localCnt ) {
        /* try submitting it */
        yuvBuffer.lumaAddr = &VENC_YUVData[bufIdx][0UL];
        yuvBuffer.cbAddr = &VENC_YUVData[bufIdx][GVENC_CompContext.context.width*16UL];
        yuvBuffer.crAddr = &VENC_YUVData[bufIdx][(GVENC_CompContext.context.width*16UL)
                            + ((GVENC_CompContext.context.width/2UL)*8UL)];
        yuvBuffer.lumaStride = (uint16_t)GVENC_CompContext.context.width;
        yuvBuffer.chromaStride = (uint16_t)(GVENC_CompContext.context.width/2UL);
        yuvBuffer.mbRow = (uint8_t)GVENC_BufInfo[bufIdx].mbRowNum;
        yuvBuffer.frameNo = GVENC_BufInfo[bufIdx].frameNum;

        GVENC_CompContext.timestamp = GVENC_BufInfo[bufIdx].timestamp;
        GVENC_CompContext.frameNum = GVENC_BufInfo[bufIdx].frameNum;
        ret = VENCLib_Encode(&GVENC_CompContext.context, &yuvBuffer, VENC_TempMem);
        if(BCM_ERR_OK == ret) {
            localCnt = CIRCQ_Pop(&VENC_CIRCQFill, (char *)&bufIdx,1UL);
            if (1UL != localCnt ) {
                VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, BCM_ERR_NOMEM, __LINE__, 0UL, 0UL, 0UL);
            }
            /* move the buffer to process q */
            ret = CIRCQ_Push(&VENC_CIRCQSubmit, (char *)&bufIdx,1UL);
            if (BCM_ERR_OK != ret) {
                VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
        } else if (BCM_ERR_EAGAIN != ret) {
            VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            localCnt = CIRCQ_Pop(&VENC_CIRCQFill, (char *)&bufIdx,1UL);
            VENC_ProcessInPort(&VENC_InPort,bufIdx);
        }
    }

    ret = MSGQ_GetMsgIdx(&VENC_OutPort.dataQ, &mediaBufferIdx);
    if (BCM_ERR_OK == ret) {
        /* get the bit stream info */
        localCnt = CIRCQ_Peek(&VENC_CIRCQBSFill, (char *)&bsBufIdx,1UL);
        if (1UL == localCnt) {
            mediaBuffer = &VENC_MediaBuffer[mediaBufferIdx];
            /* validate mediabuffer */
            if ((NULL != mediaBuffer->buffer[0].data) &&
                (0UL != mediaBuffer->buffer[0].size)) {
                /* Fill the buffer */
                /* pop form fill queue */
                if (GVENC_BSChunkInfo[bsBufIdx].size < mediaBuffer->buffer[0].size) {
                    cnt = GVENC_BSChunkInfo[bsBufIdx].size;
                } else {
                    cnt = mediaBuffer->buffer[0].size;
                }
                BCM_MemCpy(mediaBuffer->buffer[0].data,GVENC_BSChunkInfo[bsBufIdx].addr + GVENC_BSChunkInfo[bsBufIdx].offset,cnt);
                if (0UL != cnt ) {
                        GVENC_BSChunkInfo[bsBufIdx].offset += cnt;
                        mediaBuffer->timestamp = GVENC_BSChunkInfo[bsBufIdx].timestamp;
                        mediaBuffer->frameNum = GVENC_BSChunkInfo[bsBufIdx].frameNum;
                        mediaBuffer->frameStart = GVENC_BSChunkInfo[bsBufIdx].frameStart;
                        GVENC_BSChunkInfo[bsBufIdx].frameStart = 0UL;
                        mediaBuffer->syncStart = GVENC_BSChunkInfo[bsBufIdx].syncStart;
                        GVENC_BSChunkInfo[bsBufIdx].syncStart = 0UL;
                        GVENC_BSChunkInfo[bsBufIdx].syncEnd = 0UL;
                        GVENC_BSChunkInfo[bsBufIdx].size = GVENC_BSChunkInfo[bsBufIdx].size - cnt;
                        mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = GVENC_BSChunkInfo[bsBufIdx].strInfo;
                        if (0UL == GVENC_BSChunkInfo[bsBufIdx].size) {
                            mediaBuffer->syncEnd = 1UL;
                            mediaBuffer->frameEnd = GVENC_BSChunkInfo[bsBufIdx].frameEnd;
                            GVENC_BSChunkInfo[bsBufIdx].frameEnd = 0UL;
                            /* push this bitsteam info into empty list */
                            GVENC_BSChunkInfo[bsBufIdx].strInfo = 0UL;
                            GVENC_BSChunkInfo[bsBufIdx].frameNum = 0UL;
                            GVENC_BSChunkInfo[bsBufIdx].timestamp = 0ULL;
                            GVENC_BSChunkInfo[bsBufIdx].offset = 0UL;
                            (void)CIRCQ_Pop(&VENC_CIRCQBSFill, (char *)&bsBufIdx,1UL);
                            ret = CIRCQ_Push(&VENC_CIRCQBSEmpty,(char *)&bsBufIdx, 1UL);
                            if (BCM_ERR_OK != ret) {
                                VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                            }
                        } else {
                            mediaBuffer->syncEnd = 0UL;
                            mediaBuffer->frameEnd = 0UL;
                        }



                    mediaBuffer->buffer[0].size = cnt;
                    mediaBuffer->response = BCM_ERR_OK;

                    ret = MSGQ_CompleteMsgIdx(&VENC_OutPort.dataQ, mediaBufferIdx);
                }
            } else {
                mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
                ret = MSGQ_CompleteMsgIdx(&VENC_OutPort.dataQ, mediaBufferIdx);
            }
        }
        ret = MSGQ_GetMsgIdx(&VENC_OutPort.dataQ, &mediaBufferIdx);
    }

    ret = MSYS_InPortProcess(&VENC_InPort);
    if (BCM_ERR_OK != ret &&  BCM_ERR_BUSY != ret && BCM_ERR_NOMEM != ret) {
        VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    if((0UL != GVENC_CompContext.stopInProgress)
        && (VENC_LIB_STATE_IDLE == GVENC_CompContext.context.state)) {
        localCnt = CIRCQ_Peek(&VENC_CIRCQBSFill, (char *)&bsBufIdx,1UL);
        if (0UL == localCnt) {
            ret = MSYS_InPortCancelAll(&VENC_InPort);
            if (BCM_ERR_OK != ret) {
                VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }

            GVENC_CompContext.stopInProgress = 0UL;
            GVENC_CompContext.stopFlushIssued = 0UL;
            ret = MSGQ_GetMsgIdx(&VENC_OutPort.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == ret) {
                mediaBuffer = &VENC_MediaBuffer[mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                mediaBuffer->frameNum = GVENC_CompContext.frameNum;
                ret = MSGQ_CompleteMsgIdx(&VENC_OutPort.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != ret) {
                    VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }

            GVENC_CompContext.state = VENC_STATE_READY;
            if (cmdMsgIdx < MSGQ_SIZE) {
                VENC_CmdMsgQ[cmdMsgIdx].response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&VENC_CmdQ, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    VENC_ReportError(VENC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
            GVENC_CompContext.state = VENC_STATE_READY;
        }
    }
}

TASK(VENC_Task)
{

    int32_t ret;
    BCM_EventMaskType mask = 0UL;
 
    do {
        ret = BCM_WaitEvent(VENC_CMDQ_SERVER_EVENT|VENC_PTM_SERVER_EVENT|VENC_DATA_MAILBOX_EVENT|VENC_INPORT_EVENT|VENC_OUTPORT_EVENT);
        if (BCM_ERR_OK != ret) {
            VENC_ReportError(VENC_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(VENC_Task,&mask);
        if (BCM_ERR_OK != ret) {
            VENC_ReportError(VENC_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            VENC_ReportError(VENC_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }

        switch(GVENC_CompContext.state) {
        case VENC_STATE_RESET:
            VENC_ResetStateHandler();
            break;
        case VENC_STATE_INIT:
            VENC_InitStateHandler();
            break;
        case VENC_STATE_READY:
            VENC_ReadyStateHandler();
            break;
        case VENC_STATE_RUNNING:
            VENC_RunStateHandler();
            break;
        case VENC_STATE_ERROR:
            break;
        default:
            break;
        }
    } while(VENC_STATE_RESET != GVENC_CompContext.state);

    ret = BCM_TerminateTask();
    VENC_ReportError(VENC_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
}
