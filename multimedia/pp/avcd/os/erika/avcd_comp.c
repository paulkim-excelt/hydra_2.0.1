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
File Name  : avcd_comp.c
Description: This file contains avc decoder component that can be used as a
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
#include <avcd_lib.h>
#include <avcd.h>
#include <cache.h>
#include <compiler.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>

static void AVCD_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_AVD_ID, 0U, aApiID, aErr, 4UL, values);
}

#define AVCD_FILL_BUFFER_DONE_API_ID        (0x86U) /* AVCD_FillBufferDone */
#define AVCD_GET_CMD_MSG_API_ID             (0x87U) /* AVCD_GetCmdMsg */
#define AVCD_GET_DATA_MSG_API_ID            (0x88U) /* AVCD_GetDataMsg */
#define AVCD_PROCESS_INPORT_API_ID          (0x89U) /* AVCD_ProcessInPort */

#define AVCD_RESET_STATE_HANDLER_API_ID     (0x40U) /* AVCD_ResetStateHandler */
#define AVCD_INIT_STATE_HANDLER_API_ID      (0x41U) /* AVCD_InitStateHandler */
#define AVCD_READY_STATE_HANDLER_API_ID     (0x42U) /* AVCD_ReadyStateHandler */
#define AVCD_RUN_STATE_HANDLER_API_ID       (0x43U) /* AVCD_RunStateHandler */
#define AVCD_TASK_API_ID                    (0x44U) /* AVCD_Task */

#define AVCD_SEND_CMD_API_ID                (0xC0U) /* AVCD_SendCmd */
#define AVCD_RECV_RESP_API_ID               (0xC1U) /* AVCD_RecvResp */
#define AVCD_GET_NUM_MEDIAOUTPORT_API_ID    (0xC2U) /* AVCD_GetNumMediaOutPort */
#define AVCD_GET_MEDIAOUTPORT_API_ID        (0xC3U) /* AVCD_GetMediaOutPort */
#define AVCD_GET_MEDIAINPORT_API_ID         (0xC4U) /* AVCD_GetMediaInPort */

#define AVCD_STATE_RESET   (0UL)
#define AVCD_STATE_INIT    (1UL)
#define AVCD_STATE_READY   (2UL)
#define AVCD_STATE_RUNNING (3UL)
#define AVCD_STATE_ERROR   (4UL)

#define AVCD_COMP_CIRCQ_SIZE   (32UL)

#define AVCD_IN_PORT_IDX0      (0UL)
#define AVCD_IN_PORT_IDX1      (1UL)
#define AVCD_IN_PORT_IDX2      (2UL)
#define AVCD_IN_PORT_IDX3      (3UL)

#define AVCD_OUT_PORT_IDX0      (0UL)
#define AVCD_OUT_PORT_IDX1      (1UL)
#define AVCD_OUT_PORT_IDX2      (2UL)
#define AVCD_OUT_PORT_IDX3      (3UL)

#ifdef AVCD_ENABLE_ALL_PORTS
#define AVCD_MAX_NUM_IN_PORT   (4UL)
#define AVCD_MAX_NUM_OUT_PORT  (4UL)
#define AVCD_MAX_NUM_STREAMS   (4UL)
#else
#define AVCD_MAX_NUM_IN_PORT   (1UL)
#define AVCD_MAX_NUM_OUT_PORT  (1UL)
#define AVCD_MAX_NUM_STREAMS   (1UL)
#endif

typedef struct _AVCD_CompContext {
    uint32_t state;
    uint32_t stopInProgress;
    uint64_t timestamp[AVCD_MAX_NUM_OUT_PORT];
    uint32_t checkForFrameStart[AVCD_MAX_NUM_OUT_PORT];
    uint32_t FrameErrorStatus[AVCD_MAX_NUM_OUT_PORT];
    uint32_t outMB[AVCD_MAX_NUM_OUT_PORT];
    AVCDLib_Context context;
}AVCD_CompContext;

extern AVCD_CompContext GAVCD_CompContext;
AVCD_CompContext GAVCD_CompContext;

typedef struct _AVCD_BufInfo
{
    uint64_t timestamp;
    uint32_t size;
    uint32_t bufIdx;
    uint32_t portIdx;
    uint32_t frameNum;
    uint32_t frameStart;
    uint32_t frameEnd;
    AVCDLib_StreamInfo* streamInfo;
} AVCD_BufInfo;

#define INIT_AVCD_BUF_INFO   \
{                           \
    .timestamp = 0ULL,      \
    .size = 0UL,            \
    .bufIdx = 0UL,          \
}

#if (4UL == AVCD_MAX_NUM_IN_PORT )
#define AVCD_TEMP_MEM_SIZE (((AVCD_MAX_NUM_IN_PORT+2UL)*((MEDIA_SYSTEM_MAX_VIDEO_WIDTH+15UL)>>4UL)*1024UL) + (AVCD_MAX_NUM_IN_PORT*5120UL))
#define AVCD_BS_BUF_SIZE (16UL*1024UL)
#define AVCD_TOTAL_BS_BUFFERS  (3UL)
#else
#define AVCD_TEMP_MEM_SIZE ((2UL*((MEDIA_SYSTEM_MAX_VIDEO_WIDTH+15UL)>>4UL)*1024UL) + (AVCD_MAX_NUM_IN_PORT*5120UL))
#define AVCD_BS_BUF_SIZE (48UL*1024UL)
#define AVCD_TOTAL_BS_BUFFERS  (2UL)
#endif
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.AVCD_TempMem") AVCD_TempMem[AVCD_TEMP_MEM_SIZE];

/* declare YUV buffers */
static COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.AVCD_BSData") AVCD_BSData[AVCD_MAX_NUM_IN_PORT][AVCD_TOTAL_BS_BUFFERS][AVCD_BS_BUF_SIZE];

static AVCDLib_StreamInfo AVCD_StreamData[AVCD_MAX_NUM_IN_PORT][AVCD_TOTAL_BS_BUFFERS];

static CIRCQ_IndexType AVCD_circQFillIdx[AVCD_MAX_NUM_OUT_PORT] = {
#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE),
#endif
#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE),
#endif
#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE),
#endif
#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE)
#endif
};
static AVCD_BufInfo AVCD_circQFillBuffer[AVCD_MAX_NUM_OUT_PORT][AVCD_COMP_CIRCQ_SIZE];
extern const CIRCQ_Type AVCD_CIRCQFill[];
const CIRCQ_Type AVCD_CIRCQFill[AVCD_MAX_NUM_OUT_PORT] = {
#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQFillIdx[AVCD_OUT_PORT_IDX0]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQFillBuffer[AVCD_OUT_PORT_IDX0]),
                    (CIRCQ_OVERWRITE_DISABLE)),
#endif
#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQFillIdx[AVCD_OUT_PORT_IDX1]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQFillBuffer[AVCD_OUT_PORT_IDX1]),
                    (CIRCQ_OVERWRITE_DISABLE)),
#endif
#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQFillIdx[AVCD_OUT_PORT_IDX2]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQFillBuffer[AVCD_OUT_PORT_IDX2]),
                    (CIRCQ_OVERWRITE_DISABLE)),
#endif
#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQFillIdx[AVCD_OUT_PORT_IDX3]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQFillBuffer[AVCD_OUT_PORT_IDX3]),
                    (CIRCQ_OVERWRITE_DISABLE))
#endif
};

static CIRCQ_IndexType AVCD_circQProcessIdx[AVCD_MAX_NUM_OUT_PORT] = {
#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
    CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE),
#endif
#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
    CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE),
#endif
#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
    CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE),
#endif
#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
    CIRCQ_INDEX_INIT(AVCD_COMP_CIRCQ_SIZE)
#endif
};

static AVCD_BufInfo AVCD_circQProcessBuffer[AVCD_MAX_NUM_OUT_PORT][AVCD_COMP_CIRCQ_SIZE];
extern const CIRCQ_Type AVCD_CIRCQProcess[];

const CIRCQ_Type AVCD_CIRCQProcess[AVCD_MAX_NUM_OUT_PORT] = {
#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQProcessIdx[AVCD_OUT_PORT_IDX0]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQProcessBuffer[AVCD_OUT_PORT_IDX0]),
                    (CIRCQ_OVERWRITE_DISABLE)),
#endif
#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQProcessIdx[AVCD_OUT_PORT_IDX1]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQProcessBuffer[AVCD_OUT_PORT_IDX1]),
                    (CIRCQ_OVERWRITE_DISABLE)),
#endif
#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQProcessIdx[AVCD_OUT_PORT_IDX2]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQProcessBuffer[AVCD_OUT_PORT_IDX2]),
                    (CIRCQ_OVERWRITE_DISABLE)),
#endif
#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
                    CIRCQ_INIT((&AVCD_circQProcessIdx[AVCD_OUT_PORT_IDX3]), (AVCD_COMP_CIRCQ_SIZE),
                    (sizeof(AVCD_BufInfo)), ((char *)AVCD_circQProcessBuffer[AVCD_OUT_PORT_IDX3]),
                    (CIRCQ_OVERWRITE_DISABLE))
#endif
};

static void AVCD_FillBufferDone(MSYS_BufferType *const aMediaBuffer);

#if (AVCD_IN_PORT_IDX0 < AVCD_MAX_NUM_IN_PORT)
MSYS_DECLARE_IN_PORT(AVCD_InPort0, 0UL, MSYS_COMP_ID_AVCD, SystemEvent4, AVCD_InPortRW0, AVCD_FillBufferDone);
#endif
#if (AVCD_IN_PORT_IDX1 < AVCD_MAX_NUM_IN_PORT)
MSYS_DECLARE_IN_PORT(AVCD_InPort1, 1UL, MSYS_COMP_ID_AVCD, SystemEvent5, AVCD_InPortRW1, AVCD_FillBufferDone);
#endif
#if (AVCD_IN_PORT_IDX2 < AVCD_MAX_NUM_IN_PORT)
MSYS_DECLARE_IN_PORT(AVCD_InPort2, 2UL, MSYS_COMP_ID_AVCD, SystemEvent6, AVCD_InPortRW2, AVCD_FillBufferDone);
#endif
#if (AVCD_IN_PORT_IDX3 < AVCD_MAX_NUM_IN_PORT)
MSYS_DECLARE_IN_PORT(AVCD_InPort3, 3UL, MSYS_COMP_ID_AVCD, SystemEvent7, AVCD_InPortRW3, AVCD_FillBufferDone);
#endif

static const MSYS_InPortType* MediaInPortPtr[AVCD_MAX_NUM_IN_PORT] = {
#if (AVCD_IN_PORT_IDX0 < AVCD_MAX_NUM_IN_PORT)
    &AVCD_InPort0,
#endif
#if (AVCD_IN_PORT_IDX1 < AVCD_MAX_NUM_IN_PORT)
    &AVCD_InPort1,
#endif
#if (AVCD_IN_PORT_IDX2 < AVCD_MAX_NUM_IN_PORT)
    &AVCD_InPort2,
#endif
#if (AVCD_IN_PORT_IDX3 < AVCD_MAX_NUM_IN_PORT)
    &AVCD_InPort3
#endif
};
static void AVCD_ProcessInPort(const MSYS_InPortType * const aInPort,uint32_t bufIdx);
/**
 *  @brief Call back API for fill Buffer request
 *
 *  @details This API will be called when fill buffer request is served
 *
 *  @param [in] aMediaBuffer Media buffer structure pointer
 *
 *  @return None
 */
static void AVCD_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    int32_t ret;
    uint32_t portIdx;
    AVCD_BufInfo bufInfo;

    portIdx = (aMediaBuffer->bufIdx) >> 16UL;
    /* pop from submit Q */
    if (0UL != aMediaBuffer->buffer[0UL].size) {
        bufInfo.size = aMediaBuffer->buffer[0UL].size;
        bufInfo.bufIdx = aMediaBuffer->bufIdx & 0xFFFFUL;
        bufInfo.timestamp = aMediaBuffer->timestamp;
        bufInfo.portIdx = portIdx;
        bufInfo.frameNum = aMediaBuffer->frameNum;
        bufInfo.frameStart = aMediaBuffer->frameStart;
        bufInfo.frameEnd = aMediaBuffer->frameEnd;

        bufInfo.streamInfo =  &AVCD_StreamData[portIdx][bufInfo.bufIdx];
        ret = AVCDLib_GetStreamInfo(&GAVCD_CompContext.context, portIdx, &AVCD_BSData[portIdx][bufInfo.bufIdx][0UL], 32UL, bufInfo.streamInfo);
        if (BCM_ERR_OK != ret) {
            AVCD_ReportError(AVCD_FILL_BUFFER_DONE_API_ID, ret, __LINE__, bufInfo.size, 0UL, 0UL);
        }
        if((AVCD_CONFIG_SLICE == bufInfo.streamInfo->sliceType) ||
            (AVCD_UNKNOWN_SLICE == bufInfo.streamInfo->sliceType))
        {
            /* call fill buffer */
            AVCD_ProcessInPort(MediaInPortPtr[portIdx],bufInfo.bufIdx);
        } else {
            if(1UL == GAVCD_CompContext.checkForFrameStart[portIdx]) {
                if ((0UL == bufInfo.streamInfo->mbRow) && (0UL == bufInfo.streamInfo->mbCol)) {
                    GAVCD_CompContext.checkForFrameStart[portIdx] = 0UL;
                }
            }

            if (0UL == GAVCD_CompContext.checkForFrameStart[portIdx]) {
                /* push it to fill queue */
                ret = CIRCQ_Push(&AVCD_CIRCQFill[portIdx],(char*)(&bufInfo),1UL);
                if (BCM_ERR_OK != ret) {
                    AVCD_ReportError(AVCD_FILL_BUFFER_DONE_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            } else {
               /* call fill buffer */
                AVCD_ProcessInPort(MediaInPortPtr[portIdx],bufInfo.bufIdx);
            }

        }
    } else {
        if (0UL == GAVCD_CompContext.stopInProgress) {
            AVCD_ProcessInPort(MediaInPortPtr[portIdx], aMediaBuffer->bufIdx & 0xFFFFUL);
        }
    }
}
/* Compoenent CmdQ Server Event */
#define AVCD_CMDQ_SERVER_EVENT            SystemEvent0
#define AVCD_INPORT_EVENT                 SystemEvent4|SystemEvent5|SystemEvent6|SystemEvent7
#define AVCD_OUTPORT_EVENT                SystemEvent8|SystemEvent9|SystemEvent10|SystemEvent11
#define AVCD_DATA_MAILBOX_EVENT           PP_GPEvent

extern MSYS_CmdType AVCD_CmdMsgQ[];
MSYS_CmdType AVCD_CmdMsgQ[MSGQ_SIZE];
/**
 *  @brief Get command message
 *
 *  @details This API is used by the framework to query command message
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to command message structure
 */
static void* AVCD_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&AVCD_CmdMsgQ[i];
    }

    return cmdMsg;
}

extern MSYS_BufferType AVCD_MediaBuffer[AVCD_MAX_NUM_OUT_PORT][MSGQ_SIZE];
MSYS_BufferType AVCD_MediaBuffer[AVCD_MAX_NUM_OUT_PORT][MSGQ_SIZE];
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
static void* AVCD_GetDataMsg0(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&AVCD_MediaBuffer[0][i];
    }

    return dataMsg;
}
#endif
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
static void* AVCD_GetDataMsg1(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&AVCD_MediaBuffer[1][i];
    }

    return dataMsg;
}
#endif
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
static void* AVCD_GetDataMsg2(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&AVCD_MediaBuffer[2][i];
    }

    return dataMsg;
}
#endif
/**
 *  @brief Get data message
 *
 *  @details This API is used by the framework to query media buffer
 *
 *  @param [in] i Message index
 *
 *  @return Pointer to media buffer
 */
#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
static void* AVCD_GetDataMsg3(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&AVCD_MediaBuffer[3][i];
    }

    return dataMsg;
}
#endif
MSGQ_DEFINE_HDRQ(AVCD_CmdMsgHdrQ);
extern const MSGQ_Type AVCD_CmdQ;
const MSGQ_Type AVCD_CmdQ = MSGQ_INIT(AVCD_Task, AVCD_CMDQ_SERVER_EVENT,
           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &AVCD_CmdMsgHdrQ, AVCD_GetCmdMsg);

#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(AVCD_OutPort0, 0UL, AVCD_DataMsgHdrQ0,
                        AVCD_Task, AVCD_GetDataMsg0,
                        MSYS_COMP_ID_AVCD, SystemEvent8);
#endif

#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(AVCD_OutPort1, 1UL, AVCD_DataMsgHdrQ1,
                        AVCD_Task, AVCD_GetDataMsg1,
                        MSYS_COMP_ID_AVCD, SystemEvent9);
#endif

#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(AVCD_OutPort2, 2UL, AVCD_DataMsgHdrQ2,
                        AVCD_Task, AVCD_GetDataMsg2,
                        MSYS_COMP_ID_AVCD, SystemEvent10);
#endif

#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(AVCD_OutPort3, 3UL, AVCD_DataMsgHdrQ3,
                        AVCD_Task, AVCD_GetDataMsg3,
                        MSYS_COMP_ID_AVCD, SystemEvent11);
#endif


 static const MSYS_OutPortType* MediaOutPortPtr[AVCD_MAX_NUM_OUT_PORT] = {
#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
    &AVCD_OutPort0,
#endif
#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
    &AVCD_OutPort1,
#endif
#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
    &AVCD_OutPort2,
#endif
#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
    &AVCD_OutPort3
#endif
};

/** @brief Post a command to the command queue of the component
 *
 *  @details     Post a command to the command queue of the component
 *
 *  @param[in]   aMSYS_CmdType    Pointer to Cmd structure
 *  @param[in]   aMask   Mask to signal client task
 *  @param[out]  aHdr    Handle to get response, valid only when return value
 *                       is BCM_ERR_OK
 *
 *  @retval      BCM_ERR_INVAL_PARAMS   aCmd is NULL
 *  @retval      BCM_ERR_INVAL_PARAMS   Invalid magic ID of aCmd
 *  @retval      BCM_ERR_INVAL_PARAMS   aMask is zero
 *  @retval      BCM_ERR_INVAL_PARAMS   aHdr is NULL
 *  @retval      others as retured by MSGQ_SendMsg()
 */
static int32_t AVCD_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    int32_t retVal = BCM_ERR_OK;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_AVCD == aCmd->compID)
        && (NULL != aHdr)) {
        if (aCmd->cmd == MSYS_COMP_CMD_INIT) {
            retVal = BCM_ActivateTask(AVCD_Task);
            if (BCM_ERR_OK != retVal) {
                AVCD_ReportError(AVCD_SEND_CMD_API_ID, retVal, __LINE__, 0UL, 0UL, 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        ret = MSGQ_SendMsg(&AVCD_CmdQ, (const void * const)aCmd, aMask,
                            aHdr);
    } else {
        AVCD_ReportError(AVCD_SEND_CMD_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
static int32_t AVCD_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        ret = MSGQ_RecvMsg(&AVCD_CmdQ, aHdr, (void* const) aCmd);
    } else {
        AVCD_ReportError(AVCD_RECV_RESP_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }

    return ret;
}
/**
 *  @brief Get the num of outports of avcd component
 *
 *  @details Get the number of media outports of avcd component
 *
 *  @return Number of media outports of avcd component
 */
static uint32_t AVCD_GetNumMediaOutPort(void)
{
    return AVCD_MAX_NUM_OUT_PORT;
}

/** @brief Get the MSYS_OutPortType where the message needs to be posted to
 *
 * @details Get the MSYS_OutPortType where the message needs to be posted to (triggered
 * when the sink component requests avcd component to fill the media buffer)
 *
 * @param[in]   aIndex          Port Index
 *
 * @retval      NULL aIndex is not valid
 * @retval      pointer to the media outport
 */
static const MSYS_OutPortType* AVCD_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
#if (AVCD_OUT_PORT_IDX0 < AVCD_MAX_NUM_OUT_PORT)
    case AVCD_OUT_PORT_IDX0:
        outPort = MediaOutPortPtr[AVCD_OUT_PORT_IDX0];
        break;
#endif
#if (AVCD_OUT_PORT_IDX1 < AVCD_MAX_NUM_OUT_PORT)
    case AVCD_OUT_PORT_IDX1:
        outPort = MediaOutPortPtr[AVCD_OUT_PORT_IDX1];
        break;
#endif
#if (AVCD_OUT_PORT_IDX2 < AVCD_MAX_NUM_OUT_PORT)
    case AVCD_OUT_PORT_IDX2:
        outPort = MediaOutPortPtr[AVCD_OUT_PORT_IDX2];
        break;
#endif
#if (AVCD_OUT_PORT_IDX3 < AVCD_MAX_NUM_OUT_PORT)
    case AVCD_OUT_PORT_IDX3:
        outPort = MediaOutPortPtr[AVCD_OUT_PORT_IDX3];
        break;
#endif
    default:
        AVCD_ReportError(AVCD_GET_MEDIAOUTPORT_API_ID, BCM_ERR_INVAL_PARAMS,
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
static const MSYS_InPortType* AVCD_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    switch(aIndex)
    {
#if (AVCD_IN_PORT_IDX0 < AVCD_MAX_NUM_IN_PORT)
    case 0UL:
        inPort = MediaInPortPtr[0];
        break;
#endif
#if (AVCD_IN_PORT_IDX1 < AVCD_MAX_NUM_IN_PORT)
    case 1UL:
        inPort = MediaInPortPtr[1];
        break;
#endif
#if (AVCD_IN_PORT_IDX2 < AVCD_MAX_NUM_IN_PORT)
    case 2UL:
        inPort = MediaInPortPtr[2];
        break;
#endif
#if (AVCD_IN_PORT_IDX3 < AVCD_MAX_NUM_IN_PORT)
    case 3UL:
        inPort = MediaInPortPtr[3];
        break;
#endif
    default:
        AVCD_ReportError(AVCD_GET_MEDIAINPORT_API_ID, BCM_ERR_INVAL_PARAMS,
                        __LINE__, 0UL, 0UL, 0UL);
        break;
    }

    return inPort;
}

/* Component function table for AVC Decoder */
static const MSYS_CompFnTblType AVCD_CompFnTable =
{
    AVCD_SendCmd,
    AVCD_RecvResp,
    AVCD_GetMediaOutPort,
    AVCD_GetMediaInPort,
};

const MSYS_CompFnTblType* AVCD_GetCompFnTable(void)
{
    return &AVCD_CompFnTable;
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
static void AVCD_ProcessInPort(const MSYS_InPortType * const aInPort,uint32_t bufIdx)
{
    int32_t  ret;

    const MSYS_BufferType mediaBuffer = {
        .frameNum = 0UL,
        .frameStart = 0UL,
        .frameEnd = 0UL,
        .timestamp = 0ULL,
        .version = sizeof(MSYS_BufferType),
        .response = BCM_ERR_UNKNOWN,
        .format = MSYS_BUFFER_FORMAT_RAW_DATA,
        .numChannels = 1UL,
        .seqNum = 0UL,
        .bufIdx = bufIdx | ((aInPort->portIndex) << 16UL),

        .buffer[0UL].channelType = 0UL,
        .buffer[0UL].size = AVCD_BS_BUF_SIZE,
        .buffer[0UL].data = &AVCD_BSData[aInPort->portIndex][bufIdx][0UL],
    };

    ret = MSYS_FillBuffer(aInPort, &mediaBuffer);
    if (BCM_ERR_OK != ret) {
        AVCD_ReportError(AVCD_PROCESS_INPORT_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
    }
}
/**
 *  @brief Reset state handler
 *
 *  @details Reset state handling
 *
 *  @return None
 */
static void AVCD_ResetStateHandler(void)
{
    uint32_t i;
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&AVCD_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCD_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            GAVCD_CompContext.context.state = AVCD_LIB_STATE_RESET;
            ret = AVCDLib_Init(&GAVCD_CompContext.context);
            if (BCM_ERR_OK != ret) {
                AVCD_ReportError(AVCD_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GAVCD_CompContext.stopInProgress = 0UL;
            GAVCD_CompContext.state = AVCD_STATE_INIT;

            for (i=0UL;i<AVCD_MAX_NUM_OUT_PORT;i++) {

                GAVCD_CompContext.FrameErrorStatus[i] = 0UL;
                GAVCD_CompContext.timestamp[i] = 0ULL;

                ret = MSYS_OutPortInit(MediaOutPortPtr[i]);
                if(BCM_ERR_OK != ret) {
                    AVCD_ReportError(AVCD_RESET_STATE_HANDLER_API_ID, ret, __LINE__, i, 0UL, 0UL);
                }

            }
            processCmd->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&AVCD_CmdQ, cmdMsgIdx);
        if(BCM_ERR_OK != ret) {
            AVCD_ReportError(AVCD_RESET_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}
typedef union _AVCD_ConfigDataType {
    const uint8_t* hdr;
    const AVCDLib_Config *cfg;
} AVCD_ConfigDataType;

/**
 *  @brief Init state handler
 *
 *  @details Init state handling
 *
 *  @return None
 */
static void AVCD_InitStateHandler(void)
{
    uint32_t i;
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;
    AVCD_ConfigDataType config;

    ret = MSGQ_GetMsgIdx(&AVCD_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCD_CmdMsgQ[cmdMsgIdx];
        config.hdr = processCmd->cfgData;
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            switch(processCmd->configID) {
            case MSYS_CONFIG_AVCD_MOD:
                for (i=0UL;i<AVCD_MAX_NUM_STREAMS;i++) {
                    ret = AVCDLib_Configure(&GAVCD_CompContext.context, i, config.cfg, AVCD_TEMP_MEM_SIZE, AVCD_MAX_NUM_STREAMS);
                    if (BCM_ERR_OK != ret) {
                        AVCD_ReportError(AVCD_INIT_STATE_HANDLER_API_ID, ret, __LINE__, config.cfg->width, config.cfg->height, config.cfg->numPPCores);
                        }
                    }
                processCmd->response = BCM_ERR_OK;
            break;
            case MSYS_CONFIG_AVCD_FW0:
                ret = AVCDLib_Boot(&GAVCD_CompContext.context,(uint16_t)MSYS_CONFIG_AVCD_FW0,processCmd->cfgData,processCmd->size);
                if (BCM_ERR_OK != ret) {
                    AVCD_ReportError(AVCD_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
                processCmd->response = BCM_ERR_OK;
            break;
            default:
            break;
            }

            if (GAVCD_CompContext.context.state == AVCD_LIB_STATE_IDLE) {
                GAVCD_CompContext.state = AVCD_STATE_READY;
            }
        }
        ret = MSGQ_CompleteMsgIdx(&AVCD_CmdQ, cmdMsgIdx);
        if(BCM_ERR_OK != ret) {
            AVCD_ReportError(AVCD_INIT_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
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
static void AVCD_ReadyStateHandler(void)
{
    uint32_t i;
    uint32_t j;

    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;
    int32_t retVal = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&AVCD_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCD_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            //BCM_GetTime(&t3);
            retVal = BCM_SetEvent(AVCD_Task, AVCD_INPORT_EVENT|AVCD_OUTPORT_EVENT);
            if (BCM_ERR_OK != retVal) {
                AVCD_ReportError(AVCD_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GAVCD_CompContext.state = AVCD_STATE_RUNNING;
            for (i=0UL;i<AVCD_MAX_NUM_OUT_PORT;i++) {
                GAVCD_CompContext.checkForFrameStart[i] = 0UL;
                GAVCD_CompContext.outMB[i] = 0UL;
            }

            /* submit the buffers */
             for (i=0UL;i<AVCD_MAX_NUM_OUT_PORT;i++) {
                for(j=0UL;j<AVCD_TOTAL_BS_BUFFERS;j++) {
                    AVCD_ProcessInPort(MediaInPortPtr[i],j);
                }
             }

            processCmd->response = BCM_ERR_OK;
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            ret = AVCDLib_DeInit(&GAVCD_CompContext.context);
            if (BCM_ERR_OK != ret) {
                AVCD_ReportError(AVCD_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            GAVCD_CompContext.state = AVCD_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        ret = MSGQ_CompleteMsgIdx(&AVCD_CmdQ, cmdMsgIdx);
        if(BCM_ERR_OK != ret) {
            AVCD_ReportError(AVCD_READY_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}
uint32_t AVCD_InPortEvent[4UL] = {
SystemEvent4,
SystemEvent5,
SystemEvent6,
SystemEvent7
};
/**
 *  @brief Run state handler
 *
 *  @details Run state handling
 *
 *  @return None
 */
 static void AVCD_ProcessPorts(const MSYS_InPortType * const aInPort,const MSYS_OutPortType * const aOutPort, uint32_t aFlags, EventMaskType aMask)
 {
    int32_t ret=BCM_ERR_OK;
    uint32_t localCnt;
    AVCD_BufInfo bufInfo[1UL] = {INIT_AVCD_BUF_INFO};
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;
    AVCDLib_YUVBuffer yuvBuffer;
    uint32_t idx = aInPort->portIndex;

    if (0UL != aFlags) {
        if (0UL != (aFlags & (AVCD_LIB_ERROR_DETECTED << idx))) {
            GAVCD_CompContext.FrameErrorStatus[idx] = 1UL;
            GAVCD_CompContext.outMB[idx] = 0UL;
            AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        if (0UL != (aFlags & (AVCD_LIB_SLICE_PROCESSED << idx))) {
            localCnt = CIRCQ_Pop(&AVCD_CIRCQProcess[idx], (char *)&bufInfo[0UL],1UL);
            if (1UL == localCnt ) {
                AVCD_ProcessInPort(MediaInPortPtr[idx],bufInfo[0UL].bufIdx);
            }
        }
        if (0UL != (aFlags & (AVCD_LIB_FILL_BUFFER_DONE << idx))) {
            /* get filled size */
            ret = MSGQ_GetMsgIdx(&MediaOutPortPtr[idx]->dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == ret) {
                mediaBuffer = &AVCD_MediaBuffer[idx][mediaBufferIdx];
                mediaBuffer->timestamp = GAVCD_CompContext.timestamp[idx];
                mediaBuffer->response = BCM_ERR_OK;
                mediaBuffer->seqNum = AVCDLib_GetFilledBufMBRowIdx(&GAVCD_CompContext.context,idx);
                mediaBuffer->frameStart = 1;
                mediaBuffer->frameEnd = 1;
                if (mediaBuffer->seqNum != 0){
                    mediaBuffer->frameStart = 0;
                }

                if (mediaBuffer->seqNum + 1UL != ((GAVCD_CompContext.context.streamContext[idx].height + 15UL)>>4UL)) {
                    mediaBuffer->frameEnd = 0;
                }

#if ENABLE_AVCD_FRAME
                GAVCD_CompContext.outMB[idx] = (mediaBuffer->seqNum + 1UL);
                if (GAVCD_CompContext.outMB[idx] == ((GAVCD_CompContext.context.streamContext[idx].height + 15UL)>>4UL)) {
                    GAVCD_CompContext.outMB[idx] = 0UL; /* Last MB Row received. Reset */
                }
                mediaBuffer->seqNum = 0UL;
                if (0UL == GAVCD_CompContext.outMB[idx]) {
#endif

                mediaBuffer->frameNum = bufInfo[0UL].frameNum;

                ret = MSGQ_CompleteMsgIdx(&MediaOutPortPtr[idx]->dataQ, mediaBufferIdx);
                    if(ret != BCM_ERR_OK) {
                        AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                    }
#if ENABLE_AVCD_FRAME
                }
#endif
            }
        }
    }
    if (GAVCD_CompContext.context.streamContext[idx].fillBufReq != 0UL) {
        ret = MSGQ_GetMsgIdx(&MediaOutPortPtr[idx]->dataQ, &mediaBufferIdx);
        if (BCM_ERR_OK == ret) {
            mediaBuffer = &AVCD_MediaBuffer[idx][mediaBufferIdx];
            mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
            /* validate mediabuffer */
            if ((NULL != mediaBuffer->buffer[0UL].data) &&
                (0UL != mediaBuffer->buffer[0UL].size)) {
#if ENABLE_AVCD_FRAME
                uint32_t yOffset = GAVCD_CompContext.outMB[idx]* mediaBuffer->buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE]*16UL;
                uint32_t uOffset = GAVCD_CompContext.outMB[idx]* mediaBuffer->buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE]*8UL;
                uint32_t vOffset = GAVCD_CompContext.outMB[idx]* mediaBuffer->buffer[2UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE]*8UL;
#else
                uint32_t yOffset = 0UL;
                uint32_t uOffset = 0UL;
                uint32_t vOffset = 0UL;
#endif
                yuvBuffer.yAddr = mediaBuffer->buffer[0UL].data + yOffset;
                yuvBuffer.width = (uint16_t)mediaBuffer->buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
                yuvBuffer.yStride = (uint16_t)mediaBuffer->buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                yuvBuffer.height = (uint16_t)mediaBuffer->buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
                yuvBuffer.uAddr = mediaBuffer->buffer[1UL].data + uOffset;
                yuvBuffer.vAddr = mediaBuffer->buffer[2UL].data + vOffset;
                yuvBuffer.cStride = (uint16_t)mediaBuffer->buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                yuvBuffer.format = (uint16_t)0U;
                mediaBuffer->frameNum = bufInfo[0UL].frameNum;

                ret = AVCDLib_FillBuffer(&GAVCD_CompContext.context,idx,AVCD_TempMem,&yuvBuffer);
                if (BCM_ERR_OK != ret && BCM_ERR_EAGAIN != ret) {
                    AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            } else {
                ret = MSGQ_CompleteMsgIdx(&MediaOutPortPtr[idx]->dataQ, mediaBufferIdx);
                if(ret != BCM_ERR_OK) {
                    AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        }
    }

    if (0UL != (aMask & AVCD_InPortEvent[idx])) {
        ret = MSYS_InPortProcess(aInPort);
        if (BCM_ERR_OK != ret &&  BCM_ERR_BUSY != ret) {
            AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }

    if(0UL == GAVCD_CompContext.context.streamContext[aInPort->portIndex].sliceInProgress) {
        localCnt = CIRCQ_Peek(&AVCD_CIRCQFill[aInPort->portIndex], (char *)&bufInfo, 1UL);
        if (1UL == localCnt ) {
            /* try submitting it */
            ret = AVCDLib_DecodeSlice(&GAVCD_CompContext.context,aInPort->portIndex,&AVCD_BSData[aInPort->portIndex][bufInfo[0UL].bufIdx][0UL], bufInfo[0UL].size,bufInfo[0UL].streamInfo, AVCD_TempMem);
            if(BCM_ERR_OK == ret) {
                localCnt = CIRCQ_Pop(&AVCD_CIRCQFill[aInPort->portIndex], (char *)&bufInfo[0UL],1UL);
                /* move the buffer to process q */
                if (1UL == localCnt) {
                    ret = CIRCQ_Push(&AVCD_CIRCQProcess[aInPort->portIndex], (char *)&bufInfo[0UL],1UL);
                    if (BCM_ERR_OK != ret) {
                        AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                    }
                }
            } else if (BCM_ERR_INVAL_PARAMS == ret){
                localCnt = CIRCQ_Pop(&AVCD_CIRCQFill[aInPort->portIndex], (char *)&bufInfo[0UL],1UL);
                /* move the buffer to process q */
                if (1UL == localCnt) {
                    AVCD_ProcessInPort(MediaInPortPtr[aInPort->portIndex],bufInfo[0UL].bufIdx);
                }
           } else {
               /* Nothing to be done in this case */
               /* Retry will be done */
           }
        }
    }
}

static void AVCD_RunStateHandler(EventMaskType mask)
{
    uint32_t i;
    int32_t ret=BCM_ERR_OK;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;
    AVCDLib_Flags flags = 0UL;

    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;

    ret = MSGQ_GetMsgIdx(&AVCD_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &AVCD_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            GAVCD_CompContext.stopInProgress = 1UL;
        }
    }

    if (0UL != (mask & AVCD_DATA_MAILBOX_EVENT)) {
        flags = AVCDLib_Process(&GAVCD_CompContext.context,AVCD_TempMem);
    }
    for (i=0UL;i<AVCD_MAX_NUM_OUT_PORT;i++) {
        AVCD_ProcessPorts(MediaInPortPtr[i],MediaOutPortPtr[i], flags, mask);
    }

    if(1UL == GAVCD_CompContext.stopInProgress) {
        if (AVCD_LIB_STATE_IDLE == GAVCD_CompContext.context.state){
            for (i=0UL;i<AVCD_MAX_NUM_OUT_PORT;i++) {
                ret = MSYS_InPortCancelAll(MediaInPortPtr[i]);
                if (BCM_ERR_OK != ret) {
                    AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, i, 0UL, 0UL);
                }
            }
            /* This should be cleared after cancelling all the media buffers */
            GAVCD_CompContext.stopInProgress = 0UL;
            for (i=0UL;i<AVCD_MAX_NUM_OUT_PORT;i++) {
                ret = MSGQ_GetMsgIdx(&MediaOutPortPtr[i]->dataQ, &mediaBufferIdx);
                if (BCM_ERR_OK == ret) {
                    mediaBuffer = &AVCD_MediaBuffer[MediaOutPortPtr[i]->portIndex][mediaBufferIdx];
                    mediaBuffer->buffer[0].size = 0UL;
                    mediaBuffer->response = BCM_ERR_OK;
                    ret = MSGQ_CompleteMsgIdx(&MediaOutPortPtr[i]->dataQ, mediaBufferIdx);
                    if (BCM_ERR_OK != ret) {
                        AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, i, 0UL, 0UL);
                    }
                }
            }
            ret = MSGQ_GetMsgIdx(&AVCD_CmdQ, &cmdMsgIdx);
            if (BCM_ERR_OK == ret) {
                processCmd = &AVCD_CmdMsgQ[cmdMsgIdx];
                processCmd->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&AVCD_CmdQ, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    AVCD_ReportError(AVCD_RUN_STATE_HANDLER_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
            GAVCD_CompContext.state = AVCD_STATE_READY;
        }
    }
}

TASK(AVCD_Task)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;

    do {
        ret = BCM_WaitEvent(AVCD_CMDQ_SERVER_EVENT|AVCD_DATA_MAILBOX_EVENT|AVCD_INPORT_EVENT|AVCD_OUTPORT_EVENT);
        if (BCM_ERR_OK != ret) {
            AVCD_ReportError(AVCD_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(AVCD_Task,&mask);
        if (BCM_ERR_OK != ret) {
            AVCD_ReportError(AVCD_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            AVCD_ReportError(AVCD_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
        }

        switch(GAVCD_CompContext.state) {
        case AVCD_STATE_RESET:
            AVCD_ResetStateHandler();
            break;
        case AVCD_STATE_INIT:
            AVCD_InitStateHandler();
            break;
        case AVCD_STATE_READY:
            AVCD_ReadyStateHandler();
            break;
        case AVCD_STATE_RUNNING:
            AVCD_RunStateHandler((EventMaskType)mask);
            break;
        case AVCD_STATE_ERROR:
            break;
        default:
            break;
        }
    } while(AVCD_STATE_RESET != GAVCD_CompContext.state);

    ret = BCM_TerminateTask();
    AVCD_ReportError(AVCD_TASK_API_ID, ret, __LINE__, 0UL, 0UL, 0UL);
}
