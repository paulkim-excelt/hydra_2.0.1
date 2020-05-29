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
File Name  : dewarp_comp.c
Description: This file contains avc encoder component that can be used as a
             reference.
 ******************************************************************************/
#include <string.h>
#include <media_port.h>
#include <component.h>
#include <msg_queue.h>
#include <lw_queue.h>
#include <system.h>
#include <bcm_err.h>
#include <ee.h>
#include <pp.h>
#include <ldc_lib.h>
#include <ldc.h>
#include <cache.h>
#include <bcm_utils.h>
#include <imgl.h>
#include <atomic.h>
#include <compiler.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_ERROR
#include <ulog.h>

static void LDC_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_LDC_ID, 0U, aApiID, aErr, 4UL, values);
}

#define LDC_SEND_CMD_API_ID                   (0x0DU) /**< LDC_SendCmd */
#define LDC_RECV_RESP_API_ID                  (0x0EU) /**< LDC_RecvResp */
#define LDC_GET_NUM_MEDIA_OUTPORT_API_ID      (0x0FU) /**< LDC_GetNumMediaOutPort */
#define LDC_GET_MEDIA_OUTPORT_API_ID          (0x10U) /**< LDC_GetMediaOutPort */
#define LDC_GET_MEDIA_OUTPORT_CONFIG_API_ID   (0x11U) /**< LDC_GetMediaOutPortConfig */
#define LDC_PRINT_STATISTICS_API_ID           (0x12U) /**< LDC_PrintStatistics */
#define LDC_GET_MEDIA_INPORT_API_ID           (0x13U) /**< LDC_GetMediaInPort */


#define LDC_RESET_CIRC_QUEUE_API_ID        (0x82U) /* LDC_ResetLWQ */
#define LDC_FILL_BUFFER_DONE_API_ID        (0x83U) /* LDC_FillBufferDone */
#define LDC_GET_CMD_MSG_API_ID             (0x84U) /* LDC_GetCmdMsg */
#define LDC_GET_DATA_MSG_API_ID            (0x85U) /* LDC_GetDataMsg */
#define LDC_PROCESS_INPORT_API_ID          (0x86U) /* LDC_ProcessInPort */
#define LDC_SEND_CONFIG_BS_API_ID          (0x87U) /* LDC_SendConfigBS */

#define LDC_RESET_STATE_HANDLER_API_ID     (0x40U) /* LDC_ResetStateHandler */
#define LDC_INIT_STATE_HANDLER_API_ID      (0x41U) /* LDC_InitStateHandler */
#define LDC_READY_STATE_HANDLER_API_ID     (0x42U) /* LDC_ReadyStateHandler */
#define LDC_RUN_STATE_HANDLER_API_ID       (0x43U) /* LDC_RunStateHandler */
#define LDC_TASK_API_ID                    (0x44U) /* LDC_Task */


#define LDC_STATE_RESET   (0UL)
#define LDC_STATE_INIT    (1UL)
#define LDC_STATE_READY   (2UL)
#define LDC_STATE_RUNNING (3UL)
#define LDC_STATE_ERROR   (4UL)


static uint8_t COMP_SECTION(".bss.ldc") COMP_ALIGN(256) LDC_Matrix[LDC_MP_MATRIX_DATA_SIZE];

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.slowmem.LDC_MemPool") LDC_MemPool[LDC_MEMPOOL_SIZE];

/* declare YUV buffers */
#ifndef LDC_TOTAL_YUV_BUFFERS
#define LDC_TOTAL_YUV_BUFFERS (3UL)
#endif
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.LDC_InBuf") LDC_InBuf[LDC_TOTAL_YUV_BUFFERS][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL * 3UL / 2UL];


typedef struct _LDC_CompContext {
    uint32_t state;
    uint32_t stopInProgress;
    uint32_t stopFlushIssued;
    uint32_t dropsDetected;
    uint32_t mediaBufferIndex;
    uint64_t timestamp;
    uint32_t frameNum;
    uint32_t numMB;
    uint32_t nextOutMB;
    uint32_t nextInMB;
    LWQ_BufferType fillQ;
    LWQ_BufferType procQ;
    uint32_t bufInfo[LDC_TOTAL_YUV_BUFFERS];
    LDCLib_Context context;
}LDC_CompContext;

static COMP_SECTION(".bss.ldc") LDC_CompContext GLDC_CompContext;

static void LDC_FillBuffer(uint32_t aIndex);

static void LDC_ResetLWQ(void)
{
    int32_t ret;
    uint8_t index;

    GLDC_CompContext.nextInMB = 0UL;
    GLDC_CompContext.nextOutMB = 0UL;
    GLDC_CompContext.mediaBufferIndex = 0xFFFFFFFFUL;
    while (0UL < LWQ_Length(GLDC_CompContext.procQ)) {
        ret = LWQ_Pop(&GLDC_CompContext.procQ, &index);
        if (BCM_ERR_OK == ret) {
            LDC_FillBuffer(index);
        }
    }
    while (0UL < LWQ_Length(GLDC_CompContext.fillQ)) {
        ret = LWQ_Pop(&GLDC_CompContext.fillQ, &index);
        if (BCM_ERR_OK == ret) {
            LDC_FillBuffer(index);
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
static void LDC_FillBufferDone(const MSYS_InPortType * const aPort,
                                MSYS_BufferType *const aMediaBuffer)
{
    int32_t ret = BCM_ERR_OK;

    if ((aMediaBuffer != NULL)
        && (BCM_ERR_OK == aMediaBuffer->response)
        && (0UL != aMediaBuffer->buffer[0].size)) {
#ifdef LDC_InPortSe
        ret = WDS_NotifyState(&LDC_InPortSe, aMediaBuffer->seqNum);
        if (ret != BCM_ERR_OK) {
            /* Log error */
        }
#endif
        if (GLDC_CompContext.nextInMB == aMediaBuffer->seqNum) {
            GLDC_CompContext.bufInfo[aMediaBuffer->bufIdx] = GLDC_CompContext.nextInMB;
            ret = LWQ_Push(&GLDC_CompContext.fillQ, aMediaBuffer->bufIdx);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GLDC_CompContext.nextInMB = (GLDC_CompContext.nextInMB + 1UL) % ((GLDC_CompContext.context.height + 15UL)>>4UL);
            GLDC_CompContext.timestamp = aMediaBuffer->timestamp;
            if (0xFFFFFFFFUL == GLDC_CompContext.frameNum) {
                GLDC_CompContext.frameNum = aMediaBuffer->frameNum;
            }
        } else {
            if (GLDC_CompContext.nextInMB != 0UL) {
                GLDC_CompContext.dropsDetected++;
                LDC_ReportError(LDC_FILL_BUFFER_DONE_API_ID, ret, __LINE__, GLDC_CompContext.nextInMB,
                                    aMediaBuffer->seqNum, 0xFFFFFFFFUL);
            }
            GLDC_CompContext.nextInMB = 0UL;
            LDC_ResetLWQ();
            ret = LDCLib_ResetBufState(&GLDC_CompContext.context);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            LDC_FillBuffer(aMediaBuffer->bufIdx);
        }
    }
}

MSYS_DECLARE_IN_PORT_V2(LDC_InPort, 0UL, MSYS_COMP_ID_LDC, SystemEvent4,
                           LDC_InPortRW, LDC_FillBufferDone, ".data.ldc");


/* Compoenent CmdQ Server Event */
#define LDC_CMDQ_SERVER_EVENT            SystemEvent0
#define LDC_PTM_SERVER_EVENT             SystemEvent1
#define LDC_INPORT_EVENT                 SystemEvent4
#define LDC_OUTPORT_EVENT                SystemEvent8
#define LDC_DATA_MAILBOX_EVENT           PP_GPEvent

static COMP_SECTION(".bss.ldc") MSYS_CmdType LDC_CmdMsgQ[MSGQ_SIZE];
/**
 *  @brief Get command message
 *
 *  @details This API is used by the framework to query command message
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to command message structure
 */
static void* LDC_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&LDC_CmdMsgQ[i];
    }

    return cmdMsg;
}

static COMP_SECTION(".bss.ldc") MSYS_BufferType LDC_MediaBuffer[MSGQ_SIZE];
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
static void* LDC_GetDataMsg(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&LDC_MediaBuffer[i];
    }

    return dataMsg;
}
MSGQ_DEFINE_V2(LDC_CmdQ, LDC_Task, LDC_CMDQ_SERVER_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE,
                            MSYS_CmdType, LDC_CmdMsgHdrQ, LDC_GetCmdMsg, ".data.ldc");

MSYS_DECLARE_OUT_PORT_V2(LDC_OutPort, 0UL, LDC_DataMsgHdrQ, LDC_Task, LDC_GetDataMsg,
                            MSYS_COMP_ID_LDC, SystemEvent8, ".data.ldc");

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
static int32_t LDC_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_LDC == aCmd->compID)
        && (NULL != aHdr)) {
        if (aCmd->cmd == MSYS_COMP_CMD_INIT) {
            retVal = BCM_ActivateTask(LDC_Task);
            if (BCM_ERR_OK != retVal) {
                LDC_ReportError(LDC_SEND_CMD_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        ret = MSGQ_SendMsg(&LDC_CmdQ, (const void * const)aCmd, aMask,
                            aHdr);
    } else {
        LDC_ReportError(LDC_SEND_CMD_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
static int32_t LDC_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        ret = MSGQ_RecvMsg(&LDC_CmdQ, aHdr, (void* const) aCmd);
    } else {
        LDC_ReportError(LDC_SEND_CMD_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}
/**
 *  @brief Get the num of outports of dewarp component
 *
 *  @details Get the number of media outports of dewarp component
 *
 *  @return Number of media outports of dewarp component
 *
 */
static uint32_t LDC_GetNumMediaOutPort(void)
{
    return 1UL;
}

/** @brief Get the MSYS_OutPortType where the message needs to be posted to
 *
 * @details Get the MSYS_OutPortType where the message needs to be posted to (triggered
 * when the sink component requests dewarp component to fill the media buffer)
 *
 * @param[in]   aIndex          Port Index
 *
 * @retval      NULL aIndex is not valid
 * @retval      pointer to the media outport
 */
static const MSYS_OutPortType* LDC_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        outPort = &LDC_OutPort;
        break;
    default:
        LDC_ReportError(LDC_GET_MEDIA_OUTPORT_API_ID, BCM_ERR_INVAL_PARAMS,
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
static const MSYS_InPortType* LDC_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        inPort = &LDC_InPort;
        break;
    default:
        LDC_ReportError(LDC_GET_MEDIA_INPORT_API_ID, BCM_ERR_INVAL_PARAMS,
                        __LINE__, 0UL, 0UL, 0UL);
        break;
    }

    return inPort;
}

/* Component function table for lens distortion correction */
static const MSYS_CompFnTblType LDC_CompFnTable =
{
    LDC_SendCmd,
    LDC_RecvResp,
    LDC_GetMediaOutPort,
    LDC_GetMediaInPort,
};

const MSYS_CompFnTblType* LDC_GetCompFnTable(void)
{
    return &LDC_CompFnTable;
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
static void LDC_FillBuffer(uint32_t aIndex)
{
    int32_t  ret;

    if(0UL == GLDC_CompContext.stopInProgress) {
        const MSYS_BufferType mediaBuffer = {
        .timestamp = 0ULL,
        .frameNum = 0UL,
        .version = sizeof(MSYS_BufferType),
        .response = BCM_ERR_UNKNOWN,
        .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
        .numChannels = 3UL,
        .seqNum = 0UL,
        .bufIdx = aIndex,

        .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
        .buffer[0].size = GLDC_CompContext.context.width * 16UL,
        .buffer[0].data = &LDC_InBuf[aIndex][0UL],
        .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GLDC_CompContext.context.width,
        .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 16UL,
        .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GLDC_CompContext.context.width,

        .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
        .buffer[1].size = (GLDC_CompContext.context.width / 2UL) * 8UL,
        .buffer[1].data = &LDC_InBuf[aIndex][GLDC_CompContext.context.width*16UL],
        .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GLDC_CompContext.context.width / 2UL,
        .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 16UL / 2UL,
        .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GLDC_CompContext.context.width / 2UL,

        .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
        .buffer[2].size = (GLDC_CompContext.context.width / 2UL) * 8UL,
        .buffer[2].data = &LDC_InBuf[aIndex][GLDC_CompContext.context.width*16UL
                                                    + GLDC_CompContext.context.width*4UL],
        .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GLDC_CompContext.context.width / 2UL,
        .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 16UL / 2UL,
        .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GLDC_CompContext.context.width / 2UL,
        };

        ret = MSYS_FillBuffer(&LDC_InPort, &mediaBuffer);
        if (BCM_ERR_OK != ret) {
            LDC_ReportError(LDC_PROCESS_INPORT_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    } else {
        LDC_ReportError(LDC_PROCESS_INPORT_API_ID, BCM_ERR_UNKNOWN, __LINE__, 0UL, 0UL, 0UL);
    }
}
/**
 *  @brief Reset state handler
 *
 *  @details Reset state handling
 *
 *  @return None
 */
static void LDC_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&LDC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &LDC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            GLDC_CompContext.context.state = LDC_LIB_STATE_RESET;
            ret = LDCLib_Init(&GLDC_CompContext.context);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GLDC_CompContext.timestamp = 0ULL;
            GLDC_CompContext.frameNum = 0xFFFFFFFFUL;
            GLDC_CompContext.stopInProgress = 0UL;
            GLDC_CompContext.stopFlushIssued = 0UL;
            GLDC_CompContext.mediaBufferIndex = 0xFFFFFFFFUL;
            GLDC_CompContext.numMB = 0UL;
            GLDC_CompContext.nextOutMB = 0UL;
            GLDC_CompContext.nextInMB = 0UL;
            GLDC_CompContext.state = LDC_STATE_INIT;
            LWQ_Init(&GLDC_CompContext.fillQ);
            LWQ_Init(&GLDC_CompContext.procQ);
            ret = MSYS_OutPortInit(&LDC_OutPort);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            processCmd->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&LDC_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            LDC_ReportError(LDC_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

typedef union _LDC_ConfigDataType {
    const uint8_t* hdr;
    const LDCLib_Config *cfg;
} LDC_ConfigDataType;

static void LDC_ConfigLib(MSYS_CmdType *aCmd)
{
    int32_t ret;
    LDC_ConfigDataType config;

    config.hdr = aCmd->cfgData;

    switch(aCmd->configID & 0xFFF0UL) {
    case MSYS_CONFIG_LDC_MOD:
        if ((config.cfg->input_width <= ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH + 15UL) & (~15UL)))
            && (config.cfg->input_height <= ((MEDIA_SYSTEM_MAX_VIDEO_HEIGHT + 15UL) & (~15UL)))) {
            ret = LDCLib_Configure(&GLDC_CompContext.context, config.cfg, LDC_Matrix, LDC_MEMPOOL_SIZE);
            if (BCM_ERR_OK == ret) {
                DCACHE_CleanRange((uint32_t)LDC_Matrix, LDC_MP_MATRIX_DATA_SIZE);
            } else {
                LDC_ReportError(LDC_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            aCmd->response = ret;
        }
    break;
    case MSYS_CONFIG_LDC_FW:
        ret = LDCLib_Boot(&GLDC_CompContext.context,
                                aCmd->configID,
                                aCmd->cfgData,
                                aCmd->size);
        if (BCM_ERR_OK != ret) {
            LDC_ReportError(LDC_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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

static void LDC_InitStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;


    ret = MSGQ_GetMsgIdx(&LDC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &LDC_CmdMsgQ[cmdMsgIdx];

        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {

            LDC_ConfigLib(processCmd);

            if (GLDC_CompContext.context.state == LDC_LIB_STATE_IDLE) {
                GLDC_CompContext.state = LDC_STATE_READY;
            }
        }

        if (BCM_ERR_BUSY != processCmd->response) {
            ret = MSGQ_CompleteMsgIdx(&LDC_CmdQ, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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

static void LDC_ReadyStateHandler(void)
{
    uint32_t i;
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&LDC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &LDC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
#ifdef LDC_InPortSe
            {
                WDS_ConfigType wdsCfg;
                uint32_t i;
                wdsCfg.mode = WDS_MODE_LOGICAL | WDS_MODE_DEADLINE | WDS_MODE_FREQ;
                wdsCfg.deadlineInMs = 1000UL/MEDIA_SYSTEM_MAX_VIDEO_FPS;
                wdsCfg.minFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) - 1UL;
                wdsCfg.maxFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) + 1UL;
                wdsCfg.lStart = 0UL;
                wdsCfg.lEnd = ((GLDC_CompContext.context.height + 15UL) >> 4UL) - 1UL;
                BCM_MemSet(LDC_InPortSe.buffer, WDS_INVALID_GRAPH_BUF_ENTRY,
                            LDC_InPortSe.maxState * LDC_InPortSe.maxTransitionsPerState);
                for (i = wdsCfg.lStart; i < wdsCfg.lEnd; i++) {
                    LDC_InPortSe.buffer[i] = (WDS_StateType)(i + 1UL);
                }
                ret = WDS_EnableSE(&LDC_InPortSe, &wdsCfg);
                if (BCM_ERR_OK != ret) {
                    /* Log error */
                }
            }
#endif
            for(i=0UL;i<LDC_TOTAL_YUV_BUFFERS;i++) {
                LDC_FillBuffer(i);
            }
            GLDC_CompContext.state = LDC_STATE_RUNNING;
            processCmd->response = BCM_ERR_OK;
        } else if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            processCmd->response = BCM_ERR_OK;
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            ret = LDCLib_DeInit(&GLDC_CompContext.context);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GLDC_CompContext.state = LDC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            LDC_ConfigLib(processCmd);
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        ret = MSGQ_CompleteMsgIdx(&LDC_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            LDC_ReportError(LDC_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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

static void LDC_RunStateHandler(void)
{
    int32_t ret;
    uint32_t flags;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;
    uint8_t index = 0U;

    LDCLib_YUVBuffer yuvBuffer;
    uint32_t cmdMsgIdx = 0xFFFFFFFFUL;
    MSYS_CmdType *processCmd = NULL;

    ret = MSGQ_GetMsgIdx(&LDC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &LDC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            GLDC_CompContext.stopInProgress = 1UL;
        }
    }

    ret = MSYS_InPortProcess(&LDC_InPort);
    if ((BCM_ERR_OK != ret) &&  (BCM_ERR_BUSY != ret) && (BCM_ERR_NOMEM != ret)) {
        LDC_ReportError(LDC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    flags = LDCLib_Process(&GLDC_CompContext.context);
    if (0UL != (flags & LDC_LIB_COPY_DONE)) {
        ret = LWQ_Pop(&GLDC_CompContext.procQ, &index);
        if (BCM_ERR_OK == ret) {
            LDC_FillBuffer(index);
        }
    }
    if (0UL != (flags & LDC_LIB_CORRECT_DONE)) {
        ret = MSGQ_GetMsgIdx(&LDC_OutPort.dataQ, &mediaBufferIdx);
        if (BCM_ERR_OK == ret) {
            mediaBuffer = &LDC_MediaBuffer[mediaBufferIdx];
            mediaBuffer->response = BCM_ERR_OK;
            mediaBuffer->seqNum = GLDC_CompContext.nextOutMB;
            mediaBuffer->timestamp = GLDC_CompContext.timestamp;
            mediaBuffer->frameNum = GLDC_CompContext.frameNum;
            mediaBuffer->frameStart = (0UL == mediaBuffer->seqNum) ? 1U : 0U;
            mediaBuffer->frameEnd = 0U;
            GLDC_CompContext.mediaBufferIndex = 0xFFFFFFFFUL;
            GLDC_CompContext.numMB++;
            GLDC_CompContext.nextOutMB = (GLDC_CompContext.nextOutMB+1UL) % ((GLDC_CompContext.context.height+15UL)>>4UL) ;
            if (0UL == GLDC_CompContext.nextOutMB) {
                mediaBuffer->frameEnd = 1U;
                GLDC_CompContext.frameNum = 0xFFFFFFFFUL;
            }
            ret = MSGQ_CompleteMsgIdx(&LDC_OutPort.dataQ, mediaBufferIdx);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
        }
    }

    if (0UL < LWQ_Length(GLDC_CompContext.fillQ)) {
        index = LWQ_PeekFirst(GLDC_CompContext.fillQ);
        yuvBuffer.lumaAddr = &LDC_InBuf[index][0UL];
        yuvBuffer.cbAddr = &LDC_InBuf[index][GLDC_CompContext.context.width*16UL];
        yuvBuffer.crAddr = &LDC_InBuf[index][GLDC_CompContext.context.width*16UL
                                                        + GLDC_CompContext.context.width*4UL];
        yuvBuffer.mbRow = (uint8_t)GLDC_CompContext.bufInfo[index];
        yuvBuffer.lumaStride = (uint16_t)GLDC_CompContext.context.width;
        yuvBuffer.chromaStride = (uint16_t)(GLDC_CompContext.context.width /2UL);
        ret = LDCLib_CopyBuffer(&GLDC_CompContext.context, LDC_MemPool, &yuvBuffer);

        if (BCM_ERR_OK == ret) {
            ret = LWQ_Pop(&GLDC_CompContext.fillQ, &index);
            LDC_FillBuffer(index);
        } else if (BCM_ERR_BUSY == ret) {
            ret = LWQ_Pop(&GLDC_CompContext.fillQ, &index);
            if (BCM_ERR_OK == ret) {
                ret = LWQ_Push(&GLDC_CompContext.procQ, index);
                if (BCM_ERR_OK != ret) {
                    LDC_ReportError(LDC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        } else {
        }
    }

    ret = MSGQ_GetMsgIdx(&LDC_OutPort.dataQ, &mediaBufferIdx);
    if ((BCM_ERR_OK == ret) && (GLDC_CompContext.mediaBufferIndex == 0xFFFFFFFFUL)) {
        /* try submitting it */
        yuvBuffer.lumaAddr = LDC_MediaBuffer[mediaBufferIdx].buffer[0UL].data;
        yuvBuffer.cbAddr = LDC_MediaBuffer[mediaBufferIdx].buffer[1UL].data;
        yuvBuffer.crAddr = LDC_MediaBuffer[mediaBufferIdx].buffer[2UL].data;
        yuvBuffer.mbRow = (uint8_t)GLDC_CompContext.nextOutMB;
        yuvBuffer.lumaStride = (uint16_t)LDC_MediaBuffer[mediaBufferIdx].buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
        yuvBuffer.chromaStride = (uint16_t)LDC_MediaBuffer[mediaBufferIdx].buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
        ret = LDCLib_Correct(&GLDC_CompContext.context, LDC_MemPool,
                                    LDC_Matrix, &yuvBuffer);
        if (BCM_ERR_BUSY == ret) {
            GLDC_CompContext.mediaBufferIndex = mediaBufferIdx;
        }
    }

    if(0UL != GLDC_CompContext.stopInProgress) {
        if (GLDC_CompContext.nextOutMB == 0UL) {
            ret = MSYS_InPortCancelAll(&LDC_InPort);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            LDC_ResetLWQ();
            ret = LDCLib_ResetBufState(&GLDC_CompContext.context);
            if (BCM_ERR_OK != ret) {
                LDC_ReportError(LDC_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GLDC_CompContext.stopInProgress = 0UL;
            GLDC_CompContext.stopFlushIssued = 0UL;
            ret = MSGQ_GetMsgIdx(&LDC_OutPort.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == ret) {
                mediaBuffer = &LDC_MediaBuffer[mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&LDC_OutPort.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != ret) {
                    LDC_ReportError(LDC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
            GLDC_CompContext.state = LDC_STATE_READY;
#ifdef LDC_InPortSe
            {
                ret = WDS_DisableSE(&LDC_InPortSe);
                if (BCM_ERR_OK != ret) {
                    /* Log error */
                }
            }
#endif
            if (cmdMsgIdx < MSGQ_SIZE) {
                LDC_CmdMsgQ[cmdMsgIdx].response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&LDC_CmdQ, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    LDC_ReportError(LDC_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
            GLDC_CompContext.state = LDC_STATE_READY;
        }
    }
}

TASK(LDC_Task)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;

    do {
        ret = BCM_WaitEvent(LDC_CMDQ_SERVER_EVENT | LDC_PTM_SERVER_EVENT
                | LDC_DATA_MAILBOX_EVENT | LDC_INPORT_EVENT | LDC_OUTPORT_EVENT);
        if (BCM_ERR_OK != ret) {
            LDC_ReportError(LDC_TASK_API_ID, (int32_t)ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(LDC_Task,&mask);
        if (BCM_ERR_OK != ret) {
            LDC_ReportError(LDC_TASK_API_ID, (int32_t)ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            LDC_ReportError(LDC_TASK_API_ID, (int32_t)ret, __LINE__, 0UL, 0UL, 0UL);
        }

        switch(GLDC_CompContext.state) {
        case LDC_STATE_RESET:
            LDC_ResetStateHandler();
            break;
        case LDC_STATE_INIT:
            LDC_InitStateHandler();
            break;
        case LDC_STATE_READY:
            LDC_ReadyStateHandler();
            break;
        case LDC_STATE_RUNNING:
            LDC_RunStateHandler();
            break;
        case LDC_STATE_ERROR:
            break;
        default:
            break;
        }
    } while(LDC_STATE_RESET != GLDC_CompContext.state);

    ret = BCM_TerminateTask();
    LDC_ReportError(LDC_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
}
