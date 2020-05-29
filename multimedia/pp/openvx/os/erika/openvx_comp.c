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
File Name  : ovc_comp.c
Description: This file contains openvx media component that can be used as a
             reference.
 ******************************************************************************/
#include <string.h>
#include <media_port.h>
#include <component.h>
#include <msg_queue.h>
#include <circ_queue.h>
#include <system.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <ee.h>
#include <pp.h>
#include <openvx_lib.h>
#include <openvx.h>
#include <cache.h>
#include <compiler.h>
#include <bcm_utils.h>
#include <imgl.h>
#include <ulog.h>
#include <osil/bcm_osil.h>
#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <ovx_interface.h>

static void OVX0_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
//    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
//    BCM_ReportError(BCM_OVX0_ID, 0U, aApiID, aErr, 4UL, values);
}

/*
* Macros for Error Reporting
*/
#define OVX0_SEND_CMD_API_ID                   (0xC0U) /**< OVX0_SendCmd */
#define OVX0_RECV_RESP_API_ID                  (0xC1U) /**< OVX0_RecvResp */
#define OVX0_GET_NUM_MEDIA_OUTPORT_API_ID      (0xC2U) /**< OVX0_GetNumMediaOutPort */
#define OVX0_GET_MEDIA_OUTPORT_API_ID          (0xC3U) /**< OVX0_GetMediaOutPort */
#define OVX0_GET_MEDIA_INPORT_API_ID           (0xC4U) /**< OVX0_GetMediaInPort */

#define OVX0_FILL_BUFFER_DONE_API_ID        (0x83U) /* OVX0_FillBufferDone */
#define OVX0_GET_CMD_MSG_API_ID             (0x84U) /* OVX0_GetCmdMsg */
#define OVX0_GET_DATA_MSG_API_ID            (0x85U) /* OVX0_GetDataMsg */
#define OVX0_PROCESS_INPORT_API_IDD         (0x86U) /* OVX0_ProcessInPort */

#define OVX0_RESET_STATE_HANDLER_API_ID     (0x40U) /* OVX0_ResetStateHandler */
#define OVX0_INIT_STATE_HANDLER_API_ID      (0x41U) /* OVX0_InitStateHandler */
#define OVX0_READY_STATE_HANDLER_API_ID     (0x42U) /* OVX0_ReadyStateHandler */
#define OVX0_RUN_STATE_HANDLER_API_ID       (0x43U) /* OVX0_RunStateHandler */
#define OVX0_TASK_API_ID                    (0x44U) /* OVX0_Task */

#define OVX0_STATE_RESET   (0UL)
#define OVX0_STATE_INIT    (1UL)
#define OVX0_STATE_READY   (2UL)
#define OVX0_STATE_RUNNING (3UL)
#define OVX0_STATE_ERROR   (4UL)

typedef struct _OVX0_CompContext {
    OpenVXLib_Context context;
    uint32_t state;
    uint32_t stopInProgress;
    IMGL_LoadStatusType imglStatus;
    uint32_t configRecvd;
    uint32_t frameNum;
    uint32_t currentMBNum;
    uint32_t width;
    uint32_t height;
    uint32_t jobIdx;
    uint32_t graphicsState;
#define GFX_IDLE        (0UL)
#define GFX_INVALIDATE  (1UL)
#define GFX_BUSY        (2UL)
    uint32_t graphicsClean;
} OVX0_CompContext;

static OVX0_CompContext GOVX0_CompContext;

typedef struct _OVX0_BufInfoType {
    uint32_t frameNum;
    uint32_t bufIdx;
} OVX0_BufInfoType;

#define INIT_OVX0_BUF_INFO   \
{                           \
    .frameNum = 0UL,        \
    .bufIdx = 0UL,          \
};

uint32_t GOVX0_ColorMap[5UL] = {
    0x00000000, /*Transparent Background */
    0xFF0000FF, /* Red */
    0xFFFF00FF, /* Green */
    0xFF00FF00, /* cyan */
    0x00000000
};

#define OVX0_CIRCQ_LOG2SIZE  (2UL)
CIRCQ_V2_DEFINE(OVX0_FillQ, OVX0_CIRCQ_LOG2SIZE, uint8_t, OVX0_FillQBuf, OVX0_FillQIdx, CIRCQ_OVERWRITE_DISABLE, ".data");
CIRCQ_V2_DEFINE(OVX0_ProcQ, OVX0_CIRCQ_LOG2SIZE, uint8_t, OVX0_ProcQBuf, OVX0_ProcQIdx, CIRCQ_OVERWRITE_DISABLE, ".data");

OVX0_BufInfoType GOVX0_BufInfo[1UL << OVX0_CIRCQ_LOG2SIZE];
/* declare YUV buffers */
#ifndef OVX0_TOTAL_YUV_BUFFERS
#define OVX0_TOTAL_YUV_BUFFERS  (1UL << OVX0_CIRCQ_LOG2SIZE)
#endif

static COMP_ALIGN(256)
uint8_t COMP_SECTION(".openvx.uncached.GOVX0_YUVData")
GOVX0_YUVData[OVX0_TOTAL_YUV_BUFFERS][MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT * 3UL / 2UL];
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".noload.data.GOVX0_Graphics")
GOVX0_Graphics[(MEDIA_SYSTEM_MAX_VIDEO_WIDTH >> 1UL) * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT];
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".noload.data.GOVX0_GraphicsZeroBuf")
GOVX0_GraphicsZeroBuf [(MEDIA_SYSTEM_MAX_VIDEO_WIDTH >> 1UL) * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT];

static void OVX0_FillBufferDone(MSYS_BufferType *const aMediaBuffer);
MSYS_DECLARE_IN_PORT(OVX0_InPort, 0UL, MSYS_COMP_ID_OVX0, SystemEvent4, OVX0_InPortRW, OVX0_FillBufferDone);
static void OVX0_FillBuffer(uint32_t aBufIdx)
{
    if (GOVX0_CompContext.stopInProgress == 0) {
        const MSYS_BufferType mediaBuffer = {
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
            .numChannels = 3UL,
            .seqNum = 0UL,
            .bufIdx = aBufIdx,

            .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
            .buffer[0].size = GOVX0_CompContext.width * GOVX0_CompContext.height,
            .buffer[0].data = &GOVX0_YUVData[aBufIdx][0UL],
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GOVX0_CompContext.width,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GOVX0_CompContext.height,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GOVX0_CompContext.width,


            .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
            .buffer[1].size = (GOVX0_CompContext.width / 2UL) * (GOVX0_CompContext.height / 2UL),
            .buffer[1].data = &GOVX0_YUVData[aBufIdx][(GOVX0_CompContext.width * GOVX0_CompContext.height)],
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GOVX0_CompContext.width / 2UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GOVX0_CompContext.height / 2UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GOVX0_CompContext.width / 2UL,

            .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
            .buffer[2].size = (GOVX0_CompContext.width / 2UL) * (GOVX0_CompContext.height / 2UL),
            .buffer[2].data = &GOVX0_YUVData[aBufIdx][(GOVX0_CompContext.width * GOVX0_CompContext.height)
                                + ((GOVX0_CompContext.width / 2UL) * (GOVX0_CompContext.height / 2UL)) ],
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = GOVX0_CompContext.width / 2UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = GOVX0_CompContext.height / 2UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = GOVX0_CompContext.width / 2UL,

        };
        MSYS_FillBuffer(&OVX0_InPort, &mediaBuffer);
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
static void OVX0_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    int32_t ret;

    if (0UL != aMediaBuffer->buffer[0].size) {
        uint8_t bufIdx = (uint8_t)aMediaBuffer->bufIdx;
        GOVX0_BufInfo[bufIdx].frameNum = aMediaBuffer->frameNum;
        GOVX0_BufInfo[bufIdx].bufIdx = bufIdx;
        ret = CIRCQ_Push(&OVX0_FillQ,(char*)(&bufIdx),1UL);
        if (BCM_ERR_OK != ret) {
            OVX0_ReportError(OVX0_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/* Compoenent CmdQ Server Event */
#define OVX0_CMDQ_SERVER_EVENT            SystemEvent0
#define OVX0_INPORT_EVENT                 SystemEvent4
#define OVX0_OUTPORT_EVENT                SystemEvent8
#define OVX0_MAILBOX_EVENT                PP_GPEvent
extern MSYS_CmdType OVX0_CmdMsgQ[];
MSYS_CmdType OVX0_CmdMsgQ[MSGQ_SIZE];
/**
 *  @brief Get command message
 *
 *  @details This API is used by the framework to query command message
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to command message structure
 */
static void* OVX0_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&OVX0_CmdMsgQ[i];
    }

    return cmdMsg;
}

extern MSYS_BufferType OVX0_MediaBufferGraphics[];

MSYS_BufferType OVX0_MediaBufferGraphics[MSGQ_SIZE];
static void* OVX0_GetDataMsg(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&OVX0_MediaBufferGraphics[i];
    }

    return dataMsg;
}
static MSGQ_HdrQType OVX0_CmdMsgHdrQ = MSGQ_INIT_HDRQ;
extern const MSGQ_Type OVX0_CmdQ;
const MSGQ_Type OVX0_CmdQ = MSGQ_INIT(OVX0_Task, OVX0_CMDQ_SERVER_EVENT,
           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &OVX0_CmdMsgHdrQ, OVX0_GetCmdMsg);

MSYS_DECLARE_OUT_PORT(OVX0_OutPortGraphics, 0UL, OVX0_DataMsgHdrQ,
                        OVX0_Task, OVX0_GetDataMsg,
                        MSYS_COMP_ID_OVX0, SystemEvent8);
#define OVX0_MAX_NUM_OUT_PORT  1UL

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
static int32_t OVX0_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_OVX0 == aCmd->compID)
        && (NULL != aHdr)) {
        if (aCmd->cmd == MSYS_COMP_CMD_INIT) {
            retVal = BCM_ActivateTask(OVX0_Task);
            if (BCM_ERR_OK != retVal) {
                OVX0_ReportError(OVX0_SEND_CMD_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        ret = MSGQ_SendMsg(&OVX0_CmdQ, (const void * const)aCmd, aMask,
                            aHdr);
    } else {
        OVX0_ReportError(OVX0_SEND_CMD_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
static int32_t OVX0_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        ret = MSGQ_RecvMsg(&OVX0_CmdQ, aHdr, (void* const) aCmd);
    } else {
        OVX0_ReportError(OVX0_RECV_RESP_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}
/**
 *  @brief Get the num of outports of avce component
 *
 *  @details Get the number of media outports of avce component
 *
 *  @return Number of media outports of avce component
 *
 */
static uint32_t OVX0_GetNumMediaOutPort(void)
{
    return OVX0_MAX_NUM_OUT_PORT;
}

/** @brief Get the MSYS_OutPortType where the message needs to be posted to
 *
 * @details Get the MSYS_OutPortType where the message needs to be posted to (triggered
 * when the sink component requests avce component to fill the media buffer)
 *
 * @param[in]   aIndex          Port Index
 *
 * @retval      NULL aIndex is not valid
 * @retval      pointer to the media outport
 */
static const MSYS_OutPortType* OVX0_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        outPort = &OVX0_OutPortGraphics;
        break;
    default:
        OVX0_ReportError(OVX0_GET_MEDIA_OUTPORT_API_ID, BCM_ERR_INVAL_PARAMS,
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
static const MSYS_InPortType* OVX0_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        inPort = &OVX0_InPort;
        break;
    default:
        OVX0_ReportError(OVX0_GET_MEDIA_INPORT_API_ID, BCM_ERR_INVAL_PARAMS,
                        __LINE__, 0UL, 0UL, 0UL);
        break;
    }

    return inPort;
}

/* Component function table for OpenVX Component */
static const MSYS_CompFnTblType OVX0_CompFnTable =
{
    OVX0_SendCmd,
    OVX0_RecvResp,
    OVX0_GetMediaOutPort,
    OVX0_GetMediaInPort,
};

const MSYS_CompFnTblType* OVX0_GetCompFnTable(void)
{
    return &OVX0_CompFnTable;
};

/**
 *  @brief Reset state handler
 *
 *  @details Reset state handling
 *
 *  @return None
 */
static void OVX0_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&OVX0_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &OVX0_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            /* memset gaphics zero buffer */
            BCM_MemSet(GOVX0_GraphicsZeroBuf,0UL, sizeof (GOVX0_GraphicsZeroBuf));
            BCM_MemSet(GOVX0_Graphics,0UL,(MEDIA_SYSTEM_MAX_VIDEO_WIDTH >> 1UL) * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT);
            GOVX0_CompContext.currentMBNum = 0UL;
            GOVX0_CompContext.width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            GOVX0_CompContext.height = MEDIA_SYSTEM_MAX_VIDEO_HEIGHT;
            GOVX0_CompContext.stopInProgress = 0UL;
            GOVX0_CompContext.imglStatus.hdr = NULL;
            GOVX0_CompContext.state = OVX0_STATE_INIT;
            GOVX0_CompContext.graphicsState = GFX_IDLE;
            GOVX0_CompContext.graphicsClean = 0UL;
            GOVX0_CompContext.configRecvd = 0UL;
            GOVX0_CompContext.jobIdx = 0UL;
            ret = MSYS_OutPortInit(&OVX0_OutPortGraphics);
            OVX0_GetComponent()->initialize();
            if(BCM_ERR_OK != ret) {
                OVX0_ReportError(OVX0_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            CIRCQ_Reset(&OVX0_FillQ);
            CIRCQ_Reset(&OVX0_ProcQ);
            processCmd->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&OVX0_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            OVX0_ReportError(OVX0_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
#define OCV0_PP_BINARY_SIZE (200UL*1024UL)
uint8_t COMP_SECTION(".openvx.uncached.GOVX0_PPBinary") GOVX0_PPBinary[OCV0_PP_BINARY_SIZE];

#define OVX0_TABLE_SIZE (64UL*1024UL)
uint8_t COMP_SECTION(".pp.uncached.GOVX0_Table") GOVX0_Table[OVX0_TABLE_SIZE];

#ifndef OVX0_NUM_PP_CORES
#define OVX0_NUM_PP_CORES 4UL
#endif

static void OVX0_InitStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;
    uint32_t lSize = 0UL;

    ret = MSGQ_GetMsgIdx(&OVX0_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &OVX0_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            if (GOVX0_CompContext.imglStatus.hdr == NULL) {
                uint8_t *data = NULL;
                switch (processCmd->configID & 0xF) {
                case 0UL:
                    data = GOVX0_PPBinary;
                    lSize = OCV0_PP_BINARY_SIZE;
                break;
                case 1UL:
                    data = GOVX0_Table;
                    lSize = OVX0_TABLE_SIZE;
                break;
                default:
                    data = OVX0_GetComponent()->getGraph();
                    lSize = OVX0_GetComponent()->getGraphSize();
                break;
                }
                ret = IMGL_LoadImg((uint16_t)processCmd->configID, data, 0UL, lSize,
                                    SystemEvent1, &GOVX0_CompContext.imglStatus);
                if (BCM_ERR_OK == ret) {
                    processCmd->response = BCM_ERR_BUSY;
                } else {
                    processCmd->response = BCM_ERR_UNKNOWN;
                }
            } else {
                int32_t ptmStatus;
                ptmStatus = IMGL_GetStatus(&GOVX0_CompContext.imglStatus);
                if (ptmStatus == BCM_ERR_NOMEM) {
                    GOVX0_CompContext.imglStatus.hdr = NULL;
                    processCmd->response = BCM_ERR_NOMEM;
                } else if (ptmStatus == BCM_ERR_BUSY) {
                    /* Do Nothing */
                    processCmd->response = BCM_ERR_BUSY;
                } else if (ptmStatus == BCM_ERR_OK) {
                    lSize = GOVX0_CompContext.imglStatus.size;
                    ULOG_INFO("config %x %x\n", processCmd->configID, GOVX0_CompContext.configRecvd);
                    switch (processCmd->configID & 0xF) {
                    case 0UL:
                        GOVX0_CompContext.context.state = OPENVX_LIB_STATE_RESET;
                        ret = OpenVXLib_Init(&GOVX0_CompContext.context);
                        if  (BCM_ERR_OK == ret) {
                            ret = OpenVXLib_Boot(&GOVX0_CompContext.context, OVX0_NUM_PP_CORES, GOVX0_PPBinary, lSize);
                            if  (BCM_ERR_OK == ret) {
                                GOVX0_CompContext.configRecvd |= 1UL;
                            }
                        }
                    break;
                    case 1UL:
                        GOVX0_CompContext.configRecvd |= 2UL;
                    break;
                    default:
                        GOVX0_CompContext.configRecvd |= 4UL;
                    break;
                    }
                    GOVX0_CompContext.imglStatus.hdr = NULL;
                    ULOG_INFO("config %x %x\n", processCmd->configID, GOVX0_CompContext.configRecvd);
                    processCmd->response = BCM_ERR_OK;
                } else {
                    GOVX0_CompContext.imglStatus.hdr = NULL;
                    processCmd->response = BCM_ERR_UNKNOWN;
                }
            }
        }
        if (GOVX0_CompContext.configRecvd == 0x7UL) {
            GOVX0_CompContext.state = OVX0_STATE_READY;
        }
        if (BCM_ERR_BUSY != processCmd->response) {
            ret = MSGQ_CompleteMsgIdx(&OVX0_CmdQ, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                OVX0_ReportError(OVX0_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
static void OVX0_ReadyStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t retVal;
    int32_t ret = BCM_ERR_OK;
    uint32_t i;

    ret = MSGQ_GetMsgIdx(&OVX0_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &OVX0_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            retVal = BCM_SetEvent(OVX0_Task, OVX0_INPORT_EVENT|OVX0_OUTPORT_EVENT);
            if (BCM_ERR_OK != retVal) {
                OVX0_ReportError(OVX0_READY_STATE_HANDLER_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
            /* submit a buffer to fill */
            for(i = 0UL;i < OVX0_TOTAL_YUV_BUFFERS; i++) {
                OVX0_FillBuffer(i);
            }
            GOVX0_CompContext.state = OVX0_STATE_RUNNING;
            processCmd->response = BCM_ERR_OK;
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            GOVX0_CompContext.state = OVX0_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        ret = MSGQ_CompleteMsgIdx(&OVX0_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            OVX0_ReportError(OVX0_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}
void drawline(int color, int x0, int y0, int x1, int y1) {

    if ((x0 >= MEDIA_SYSTEM_MAX_VIDEO_WIDTH) || (y0 >= MEDIA_SYSTEM_MAX_VIDEO_HEIGHT)) {
        return;
    }

    if ((x0<0) || (y0<0)) {
        return;
    }

    if ((x1 >= MEDIA_SYSTEM_MAX_VIDEO_WIDTH) || (y1 >= MEDIA_SYSTEM_MAX_VIDEO_HEIGHT)) {
        return;
    }

    if ((x1<0) || (y1<0)) {
        return;
    }

    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
        GOVX0_Graphics[(y0*(MEDIA_SYSTEM_MAX_VIDEO_WIDTH>>1UL)) + (x0>>1UL)] = color << ((x0&1UL)<<2UL);
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

/**
 *  @brief Run state handler
 *
 *  @details Run state handling
 *
 *  @return None
 */
//#define ENABLE_OPENVX_PROFILING
#ifdef ENABLE_OPENVX_PROFILING
BCM_TimeType GOVX0_t1, GOVX0_t2;
#endif
static void OVX0_RunStateHandler(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t localCnt = 0UL;
    uint8_t bufIdx;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;
    uint32_t cmdMsgIdx = 0xFFFFFFFFUL;
    MSYS_CmdType *processCmd = NULL;


    ret = MSGQ_GetMsgIdx(&OVX0_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &OVX0_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            GOVX0_CompContext.stopInProgress = 1UL;
        }
    }

    if (GOVX0_CompContext.context.state != OPENVX_LIB_STATE_IDLE)
        localCnt = CIRCQ_Peek(&OVX0_ProcQ, (char *)&bufIdx, 1UL);
        if (1UL == localCnt) {
            uint32_t flags = OpenVXLib_Process(&GOVX0_CompContext.context,
                                    OVX0_GetComponent()->getGraph(),
                                    OVX0_GetComponent()->getStorageInfo(GOVX0_YUVData[bufIdx],GOVX0_CompContext.jobIdx),
                                    GOVX0_Table,
                                    OVX0_GetComponent()->uniqueID);
            if (0UL != (flags & OPENVX_LIB_ERROR)) {
                OVX0_ReportError(OVX0_RUN_STATE_HANDLER_API_ID, BCM_ERR_UNKNOWN, __LINE__, 0UL, 0UL, 0UL);
            }
            if (0UL != (flags & OPENVX_LIB_DONE)) {
#ifdef ENABLE_OPENVX_PROFILING
                BCM_GetTime(&GOVX0_t2);
                GOVX0_t1 = BCM_GetTimeAbsDiff(GOVX0_t1,GOVX0_t2);
                ULOG_INFO("OpenVX took %d\n", GOVX0_t1.ns);
#endif
                localCnt = CIRCQ_Pop(&OVX0_ProcQ, (char *)&bufIdx,1UL);
                if (1UL == localCnt ) {
                    OVX0_FillBuffer(bufIdx);
                }
                if (GOVX0_CompContext.graphicsState == GFX_IDLE) {
                    /* Draw Rectangles here */
                    BCM_MemSet(GOVX0_Graphics, 0, sizeof(GOVX0_Graphics));
                    if (GOVX0_CompContext.graphicsClean == 1UL) {
                        GOVX0_CompContext.graphicsClean = 0UL;
                        GOVX0_CompContext.graphicsState = GFX_INVALIDATE;
                    }
                    if (OVX0_GetComponent()->render(GOVX0_CompContext.jobIdx) > 0UL) {
                        GOVX0_CompContext.graphicsState = GFX_INVALIDATE;
                        GOVX0_CompContext.graphicsClean = 1UL;
                    }
                }
                GOVX0_CompContext.jobIdx++;
            }
        }


    ret = MSYS_InPortProcess(&OVX0_InPort);
    if (BCM_ERR_OK != ret &&  BCM_ERR_BUSY != ret) {
        OVX0_ReportError(OVX0_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }
    if (GOVX0_CompContext.context.state == OPENVX_LIB_STATE_IDLE) {
        localCnt = CIRCQ_Peek(&OVX0_FillQ, (char *)&bufIdx,1UL);
        if (localCnt == 1) {
            ret = OpenVXLib_Start(&GOVX0_CompContext.context,
                                  OVX0_GetComponent()->getGraph(),
                                  OVX0_GetComponent()->getStorageInfo(GOVX0_YUVData[bufIdx],GOVX0_CompContext.jobIdx),
                                  GOVX0_Table,
                                  OVX0_GetComponent()->uniqueID);
            if (BCM_ERR_OK == ret) {
#ifdef ENABLE_OPENVX_PROFILING
                BCM_GetTime(&GOVX0_t1);
#endif
                CIRCQ_Pop(&OVX0_FillQ, (char *)&bufIdx, 1UL);
                CIRCQ_Push(&OVX0_ProcQ, (char *)&bufIdx, 1UL);
            } else {
                ULOG_ERR("OVX0: Failed to start graph\n");
            }

        }
    }
    if (GOVX0_CompContext.graphicsState == GFX_INVALIDATE) {
        GOVX0_CompContext.graphicsState = GFX_BUSY;
    }
    if (GOVX0_CompContext.graphicsState == GFX_BUSY) {
        ret = MSGQ_GetMsgIdx(&OVX0_OutPortGraphics.dataQ, &mediaBufferIdx);
        if (BCM_ERR_OK == ret) {
            mediaBuffer = &OVX0_MediaBufferGraphics[mediaBufferIdx];
            BCM_MemCpy(mediaBuffer->buffer[0].data,
                        GOVX0_Graphics,
                        (MEDIA_SYSTEM_MAX_VIDEO_WIDTH >> 1UL) * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT);
            mediaBuffer->buffer[0].size = (MEDIA_SYSTEM_MAX_VIDEO_WIDTH >> 1UL) * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT;
            BCM_MemCpy(mediaBuffer->buffer[1].data,GOVX0_ColorMap,16UL);
            mediaBuffer->seqNum = 0UL;
            mediaBuffer->frameNum = 0UL;
            mediaBuffer->frameStart = 1U;
            mediaBuffer->response = BCM_ERR_OK;
            GOVX0_CompContext.graphicsState = GFX_IDLE;
            mediaBuffer->frameEnd = 1U;
            ret = MSGQ_CompleteMsgIdx(&OVX0_OutPortGraphics.dataQ, mediaBufferIdx);
            if (BCM_ERR_OK != ret) {
                OVX0_ReportError(OVX0_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
        }
    }

    if(1UL == GOVX0_CompContext.stopInProgress ) {
        /* TODO: this state can be init also */
        if (NULL == GOVX0_CompContext.imglStatus.hdr){
            ret = MSYS_InPortCancelAll(&OVX0_InPort);
            if (BCM_ERR_OK != ret) {
                OVX0_ReportError(OVX0_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GOVX0_CompContext.stopInProgress = 0UL;
            ret = MSGQ_GetMsgIdx(&OVX0_OutPortGraphics.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == ret) {
                mediaBuffer = &OVX0_MediaBufferGraphics[mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&OVX0_OutPortGraphics.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != ret) {
                    OVX0_ReportError(OVX0_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
            if (cmdMsgIdx < MSGQ_SIZE) {
                OVX0_CmdMsgQ[cmdMsgIdx].response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&OVX0_CmdQ, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    OVX0_ReportError(OVX0_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
            GOVX0_CompContext.state = OVX0_STATE_READY;
        }
    } else {

    }

}

TASK(OVX0_Task)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;

    do {
        ret = BCM_WaitEvent(OVX0_CMDQ_SERVER_EVENT | SystemEvent1 |
                            OVX0_INPORT_EVENT | OVX0_OUTPORT_EVENT | OVX0_MAILBOX_EVENT);
        if (BCM_ERR_OK != ret) {
            OVX0_ReportError(OVX0_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(OVX0_Task,&mask);
        if (BCM_ERR_OK != ret) {
            OVX0_ReportError(OVX0_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            OVX0_ReportError(OVX0_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }

        switch(GOVX0_CompContext.state) {
        case OVX0_STATE_RESET:
            OVX0_ResetStateHandler();
            break;
        case OVX0_STATE_INIT:
            OVX0_InitStateHandler();
            break;
        case OVX0_STATE_READY:
            OVX0_ReadyStateHandler();
            break;
        case OVX0_STATE_RUNNING:
            OVX0_RunStateHandler();
            break;
        case OVX0_STATE_ERROR:
            break;
        default:
            break;
        }
    } while(OVX0_STATE_RESET != GOVX0_CompContext.state);

    ret = BCM_TerminateTask();
    OVX0_ReportError(OVX0_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
}
