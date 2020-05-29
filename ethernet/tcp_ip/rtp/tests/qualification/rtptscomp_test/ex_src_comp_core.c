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
File Name  : src_comp.c
Description: This file contains a sample source component that can be used as a
             reference.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO
#include <string.h>
#include <media_port.h>
#include <component.h>
#include <msg_queue.h>
#include <cache.h>

#include <ulog.h>
#include <bcm_err.h>
#include <eecfg.h>
#include "ee.h"
#include <atomic.h>
#include <imgl.h>

#include <ex_src_comp.h>
#include <osil/bcm_osil.h>

/* Maximum number of output ports */
#define EXSRC_MAX_NUM_OUT_PORT   (4UL)
#define AVB_AVTP_PAYLOAD_SIZE    (1440UL)

/* BRCM RTP Test Data Config */
#define BRCM_RTP_TESTDATA_CONFIG (0xFF00UL)

MSYS_BufferType ExSrc_MediaBuffer[EXSRC_MAX_NUM_OUT_PORT][MSGQ_SIZE];

#define RTPS_TEST_MAX_NAL_DATA_SIZE (500*1024)
static uint8_t COMP_SECTION(".dma.bcm.test") enc_data_array [RTPS_TEST_MAX_NAL_DATA_SIZE];
uint32_t enc_data_array_size = 0UL;

/* Structure to fetch NAL details */
typedef struct _nalDataInfo {
    /* NAL type of data fetched */
    uint32_t nalType;
#define EXSRC_TEST_NAL_SPS       0UL
#define EXSRC_TEST_NAL_PPS       1UL
#define EXSRC_TEST_NAL_START     2UL
#define EXSRC_TEST_NAL_MID       3UL

    /* Length of NAL data fetched */
    uint32_t nalLen;

    /* Frame start indicator */
    uint32_t isFrameStart;

    /* Frame end indicator */
    uint32_t isFrameEnd;

} nalDataInfo;

/* Processed data offset */
uint32_t dataOffset[EXSRC_MAX_NUM_OUT_PORT] = {0UL, 0UL, 0UL, 0UL};

/* Function to get a NAL unit from the H264 data buffer */
void GetNALDataFromBuffer (nalDataInfo *info, uint32_t offsetBytes)
{
    uint32_t procBytes = 0UL;
    uint32_t first = 1UL;

    while ((offsetBytes + procBytes) < enc_data_array_size) {

        /* Check NAL Start Code (4 bytes) */
        if((enc_data_array[(offsetBytes+procBytes)]   == 0x0) &&
           (enc_data_array[(offsetBytes+procBytes)+1] == 0x0) &&
           (enc_data_array[(offsetBytes+procBytes)+2] == 0x0) &&
           (enc_data_array[(offsetBytes+procBytes)+3] == 0x1))
        {
            if ((0UL == procBytes) && (1UL == first)) {

                info->nalType = EXSRC_TEST_NAL_START;

                if (enc_data_array[(offsetBytes+procBytes)+4] == 0x67) {
                    info->nalType = EXSRC_TEST_NAL_SPS;
                } else if (enc_data_array[(offsetBytes+procBytes)+4] == 0x68) {
                    info->nalType = EXSRC_TEST_NAL_PPS;
                } else if((enc_data_array[(offsetBytes+procBytes)+4] & 0x1F) == 0x5) {

                    /* A high on the MSB means start of a new frame (1 MSB Bit) */
                    if(0x80 == (enc_data_array[(offsetBytes+procBytes)+5] & 0x80))
                    {
                        info->isFrameStart = 1UL;
                    }
                } else {
                    /* Nothing */
                }
                first = 0UL;

            } else if ((0UL < procBytes) && (1UL == first)) {

                info->nalType = EXSRC_TEST_NAL_MID;
                info->nalLen = procBytes;
                if((enc_data_array[(offsetBytes+procBytes)+4] & 0x1F) == 0x5) {

                    /* A high on the MSB means start of a new frame (1 MSB Bit) */
                    if(0x80 == (enc_data_array[(offsetBytes+procBytes)+5] & 0x80))
                    {
                        info->isFrameEnd= 1UL;
                    }
                }
                break;
            } else if ((0UL < procBytes) && (0UL == first)) {
                info->nalLen = procBytes;
                if((enc_data_array[(offsetBytes+procBytes)+4] & 0x1F) == 0x5) {

                    /* A high on the MSB means start of a new frame (1 MSB Bit) */
                    if((0x80 == (enc_data_array[(offsetBytes+procBytes)+5] & 0x80)) &&
                       ((EXSRC_TEST_NAL_START == info->nalType) ||
                        (EXSRC_TEST_NAL_MID == info->nalType)))
                    {
                        info->isFrameEnd= 1UL;
                    }
                }
                break;
            } else {
                /* Nothing */
            }
        }
        procBytes++;
    }

    if((offsetBytes + procBytes) == enc_data_array_size){
        info->nalLen = procBytes;
        if (1U == first) {
            info->nalType = EXSRC_TEST_NAL_MID;
        }
        info->isFrameEnd= 1UL;
    }
    return;
}

void* ExSrc_GetDataMsg_Port0(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[0][i];
    }

    return dataMsg;
}

void* ExSrc_GetDataMsg_Port1(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[1][i];
    }

    return dataMsg;
}

void* ExSrc_GetDataMsg_Port2(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[2][i];
    }

    return dataMsg;
}

void* ExSrc_GetDataMsg_Port3(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[3][i];
    }

    return dataMsg;
}

MSYS_DECLARE_OUT_PORT(ExSrc_OutPort0, 0UL, ExSrc_DataMSGQ_HdrQType0,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg_Port0,
                        MSYS_COMP_ID_EX_SRC, SystemEvent8);
MSYS_DECLARE_OUT_PORT(ExSrc_OutPort1, 1UL, ExSrc_DataMSGQ_HdrQType1,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg_Port1,
                        MSYS_COMP_ID_EX_SRC,SystemEvent9);
MSYS_DECLARE_OUT_PORT(ExSrc_OutPort2, 2UL, ExSrc_DataMSGQ_HdrQType2,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg_Port2,
                        MSYS_COMP_ID_EX_SRC, SystemEvent10);
MSYS_DECLARE_OUT_PORT(ExSrc_OutPort3, 3UL, ExSrc_DataMSGQ_HdrQType3,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg_Port3,
                        MSYS_COMP_ID_EX_SRC, SystemEvent11);


/* Component Command Queue */
//TODO: ExSrc_CmdProcessTask task has to be defined in code.c
MSGQ_DEFINE_HDRQ(ExSrc_CmdMSGQ_HdrQType);

/* Compoenent CmdQ Server Event */
#define EXSRC_CMDQ_SERVER_EVENT         SystemEvent0
#define EXSRC_STOP_DATA_PROCESS_EVENT   SystemEvent0
#define EXSRC_DATA_PROCESS_ACK_EVENT    SystemEvent1
#define EXSRC_PTDATA_LOAD_EVENT         SystemEvent2

#define EXSRC_DATA_PROCESS_OUTPORT0     SystemEvent8
#define EXSRC_DATA_PROCESS_OUTPORT1     SystemEvent9
#define EXSRC_DATA_PROCESS_OUTPORT2     SystemEvent10
#define EXSRC_DATA_PROCESS_OUTPORT3     SystemEvent11

MSYS_CmdType ExSrc_CmdMsgQ[MSGQ_SIZE];

void* ExSrc_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&ExSrc_CmdMsgQ[i];
    }

    return cmdMsg;
}

MSGQ_DEFINE(ExSrc_CmdQ, ExSrc_CmdProcessTask, EXSRC_CMDQ_SERVER_EVENT,
            MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &ExSrc_CmdMSGQ_HdrQType, ExSrc_GetCmdMsg);

/* Post a command to the command queue */
int32_t ExSrc_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask, const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_EX_SRC == aCmd->compID)
        && (NULL != aHdr)) {
        /* Post the command to the component's Command Queue */
        retVal = MSGQ_SendMsg(&ExSrc_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        ULOG_ERR("ExSrc_SendCmd: BCM_ERR_INVAL_PARAMS\n");
    }

    return retVal;
}

/* Receive response for the command posted previously */
int32_t ExSrc_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        retVal = MSGQ_RecvMsg(&ExSrc_CmdQ, aHdr, (void* const) aCmd);
    } else {
        ULOG_ERR("ExSrc_SendCmd: Invalid parameters\n");
    }

    return retVal;
}

/* Get the number of output ports */
uint32_t ExSrc_GetNumMediaOutPort(void)
{
    return EXSRC_MAX_NUM_OUT_PORT;
}

/* Get the MSYS_OutPortType where the message needs to be posted to */
const MSYS_OutPortType* ExSrc_GetOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
    case 0UL:
        outPort = &ExSrc_OutPort0;
        break;
    case 1UL:
        outPort = &ExSrc_OutPort1;
        break;
    case 2UL:
        outPort = &ExSrc_OutPort2;
        break;
    case 3UL:
        outPort = &ExSrc_OutPort3;
        break;
    default:
        ULOG_ERR("ExSrc_GetOutPort: unknown PortIdx %u\n", aIndex);
        break;
    }

    return outPort;
}

/* Process the command received on the command queue */
void ExSrc_ProcessCmd(void)
{
    int32_t retVal;
    uint32_t cmdMsgIdx;
    MSYS_CmdType *processCmd;
    int32_t status;

    retVal = MSGQ_GetMsgIdx(&ExSrc_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSrc_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;

        switch(processCmd->cmd)
        {
        case MSYS_COMP_CMD_INIT:
        case MSYS_COMP_CMD_DEINIT:
            processCmd->response = BCM_ERR_OK;
            ULOG_INFO("ExSrc_ProcessCmd: %u\n", processCmd->cmd);
            break;

        case MSYS_COMP_CMD_CONFIG: {
            int32_t ptmStatus;
            IMGL_LoadStatusType imglStatus;
            int32_t ret;

            status = BCM_ActivateTask(ExSrc_DataProcessTask);
            if (BCM_ERR_OK != status) {
                ULOG_ERR("ExSrc_ProcessCmd: data process start failed (%d)\n",
                         status);
                processCmd->response = BCM_ERR_UNKNOWN;
            } else {
                BCM_WaitEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
                BCM_ClearEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
                processCmd->response = BCM_ERR_OK;
            }

            retVal = IMGL_LoadImg(BRCM_RTP_TESTDATA_CONFIG,
                                 enc_data_array,
                                 0UL, RTPS_TEST_MAX_NAL_DATA_SIZE, SystemEvent2, &imglStatus);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("ExSrc_ProcessCmd > IMGL_LoadImg(%d) failed \n", retVal);
                break;
            }
            ret = BCM_WaitEvent(SystemEvent2);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("ExSrc_ProcessCmd >  BCM_WaitEvent Failed %d\n",ret);
                break;
            }
            ret = BCM_ClearEvent(SystemEvent2);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("ExSrc_ProcessCmd >  BCM_ClearEvent Failed %d\n",ret);
                break;
            }
            ptmStatus = IMGL_GetStatus(&imglStatus);
            if (ptmStatus != BCM_ERR_OK) {
                ULOG_ERR("ExSrc_ProcessCmd > ptmStatus(%d) failed \n", ptmStatus);
                retVal = BCM_ERR_UNKNOWN;
                break;
            } else {
                enc_data_array_size = imglStatus.size;
            }
            break;
        }
        case MSYS_COMP_CMD_START:
            ULOG_INFO("ExSrc_ProcessCmd: %u\n", processCmd->cmd);
            BCM_SetEvent(ExSrc_DataProcessTask, SystemEvent8);
            BCM_SetEvent(ExSrc_DataProcessTask, SystemEvent9);
            BCM_SetEvent(ExSrc_DataProcessTask, SystemEvent10);
            BCM_SetEvent(ExSrc_DataProcessTask, SystemEvent11);

            processCmd->response = BCM_ERR_OK;
        break;

        case MSYS_COMP_CMD_STOP:
            /* FIXME: Erika specific */
            /* NOTE: The data processing task has a single waiting point.
             * Depending on the event which is triggered, the data processing
             * task has to proceed. In this case, it has to cancel all the
             * posted messages and then terminate.
             */
            BCM_SetEvent(ExSrc_DataProcessTask, EXSRC_STOP_DATA_PROCESS_EVENT);
            BCM_WaitEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
            BCM_ClearEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
            processCmd->response = BCM_ERR_OK;
            break;

        default:
            ULOG_ERR("ExSrc_ProcessCmd: unknown command (%u)\n",
                     processCmd->cmd);
            processCmd->response = BCM_ERR_INVAL_PARAMS;
            break;
        }   /* end of  switch(processCmd->cmd)*/

        retVal = MSGQ_CompleteMsgIdx(&ExSrc_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSrc_ProcessCmd: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
    }
}

TASK(ExSrc_CmdProcessTask)
{
    do {
        BCM_WaitEvent(EXSRC_CMDQ_SERVER_EVENT);
        BCM_ClearEvent(EXSRC_CMDQ_SERVER_EVENT);
        ExSrc_ProcessCmd();
    } while(1UL);
}

volatile uint32_t AlarmVal0 = 0UL;
volatile uint32_t AlarmVal1 = 0UL;
volatile uint32_t AlarmVal2 = 0UL;
volatile uint32_t AlarmVal3 = 0UL;
static uint32_t portIdForTrigger = 0UL;

void ExSrc_DataProcessTask_Cb()
{
    if (0UL == portIdForTrigger){
        ATOMIC_Set(&AlarmVal0, 1UL);
        portIdForTrigger = 1UL;
        SetEvent(ExSrc_DataProcessTask, SystemEvent8);
    } else if (1UL == portIdForTrigger){
        ATOMIC_Set(&AlarmVal1, 1UL);
        portIdForTrigger = 2UL;
        SetEvent(ExSrc_DataProcessTask, SystemEvent9);
    } else if (2UL == portIdForTrigger){
        ATOMIC_Set(&AlarmVal2, 1UL);
        portIdForTrigger = 3UL;
        SetEvent(ExSrc_DataProcessTask, SystemEvent10);
    } else {
        ATOMIC_Set(&AlarmVal3, 1UL);
        portIdForTrigger = 0UL;
        SetEvent(ExSrc_DataProcessTask, SystemEvent11);
    }
    return;
}

#ifdef EXSRC_SAMPLE_DATA
static uint8_t Buffer[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
static int8_t ExSrc_idx0 = 0UL;
static int8_t ExSrc_idx1 = 0UL;
static int8_t ExSrc_idx2 = 0UL;
static int8_t ExSrc_idx3 = 0UL;
#endif

void ExSrc_ProcessData_Port0(const MSYS_OutPortType * const outPort)
{
    int32_t retVal;
    uint32_t mediaBufferIdx;
    uint32_t localAlarmVal;
    MSYS_BufferType * mediaBuffer;

    static uint64_t frame_count = 0;
    static uint32_t nal_offset = 0;
    nalDataInfo nalInfo;
    uint32_t copySize = 0UL;

    memset(&nalInfo, 0, sizeof(nalInfo));

    retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
    localAlarmVal = ATOMIC_Set(&AlarmVal0, 0UL);

    if(nal_offset == enc_data_array_size){
        nal_offset = 0UL;
    }

    while ((BCM_ERR_OK == retVal) && (1UL == localAlarmVal)) {
        mediaBuffer = &ExSrc_MediaBuffer[0][mediaBufferIdx];

        /* validate mediabuffer */
        if ((NULL != mediaBuffer->buffer[0].data) &&
            (0UL != mediaBuffer->buffer[0].size)){

#ifdef EXSRC_SAMPLE_DATA
            /* Fill the buffer */
            mediaBuffer->buffer[0].data[0UL] = Buffer[ExSrc_idx0++];
            if (ExSrc_idx0 >= 26UL) {
                ExSrc_idx0 = 0UL;
                mediaBuffer->syncEnd = 1UL;
            }
            mediaBuffer->buffer[0].size = 1UL;
#else
            /* Invalidate buffer as it is cached */
            DCACHE_InvalRange((uint32_t)mediaBuffer->buffer[0].data,
                                   (uint32_t)mediaBuffer->buffer[0].size);

            /* Get the current NAL to be processed */
            GetNALDataFromBuffer(&nalInfo, nal_offset);

            /* Determine the data size to be copied to port buffer */
            if (nalInfo.nalLen <= mediaBuffer->buffer[0].size){
                copySize = nalInfo.nalLen;
            } else {
                copySize = mediaBuffer->buffer[0].size;
            }

            (void) memcpy(mediaBuffer->buffer[0].data,
                          &enc_data_array[nal_offset],copySize);

            /* Update the NAL offset size */
            nal_offset += copySize;
            mediaBuffer->buffer[0].size = copySize;

            /* Set the SYnc and Frame markers for payload */
            if (EXSRC_TEST_NAL_SPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x67UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_PPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x68UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_START == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 1UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else if (EXSRC_TEST_NAL_MID == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 0UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else {
                /* Nothing */
            }

            frame_count++;
#endif /* EXSRC_SAMPLE_DATA */
            mediaBuffer->response = BCM_ERR_OK;
        } else {
            mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
        }

        retVal = MSGQ_CompleteMsgIdx(&outPort->dataQ, mediaBufferIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSrc_ProcessData_Port0: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
        retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
        localAlarmVal = ATOMIC_Set(&AlarmVal0, 0UL);
    }
}

void ExSrc_ProcessData_Port1(const MSYS_OutPortType * const outPort)
{
    int32_t retVal;
    uint32_t mediaBufferIdx;
    uint32_t localAlarmVal;
    MSYS_BufferType * mediaBuffer;

    static uint64_t frame_count = 0;
    static uint32_t nal_offset = 0;
    nalDataInfo nalInfo;
    uint32_t copySize = 0UL;

    memset(&nalInfo, 0, sizeof(nalInfo));

    retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
    localAlarmVal = ATOMIC_Set(&AlarmVal1, 0UL);

    if(nal_offset == enc_data_array_size){
        nal_offset = 0UL;
    }

    while ((BCM_ERR_OK == retVal) && (1UL == localAlarmVal)) {
        mediaBuffer = &ExSrc_MediaBuffer[1][mediaBufferIdx];

        /* validate mediabuffer */
        if ((NULL != mediaBuffer->buffer[0].data) &&
            (0UL != mediaBuffer->buffer[0].size)){

#ifdef EXSRC_SAMPLE_DATA
            /* Fill the buffer */
            mediaBuffer->buffer[0].data[0UL] = Buffer[ExSrc_idx1++];
            if (ExSrc_idx1 >= 26UL) {
                ExSrc_idx1 = 0UL;
                mediaBuffer->syncEnd = 1UL;
            }
            mediaBuffer->buffer[0].size = 1UL;
#else
            /* Invalidate buffer as it is cached */
            DCACHE_InvalRange((uint32_t)mediaBuffer->buffer[0].data,
                                   (uint32_t)mediaBuffer->buffer[0].size);

            /* Get the current NAL to be processed */
            GetNALDataFromBuffer(&nalInfo, nal_offset);

            /* Determine the data size to be copied to port buffer */
            if (nalInfo.nalLen <= mediaBuffer->buffer[0].size){
                copySize = nalInfo.nalLen;
            } else {
                copySize = mediaBuffer->buffer[0].size;
            }

            (void) memcpy(mediaBuffer->buffer[0].data,
                          &enc_data_array[nal_offset],copySize);

            /* Update the NAL offset size */
            nal_offset += copySize;
            mediaBuffer->buffer[0].size = copySize;

            /* Set the SYnc and Frame markers for payload */
            if (EXSRC_TEST_NAL_SPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x67UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_PPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x68UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_START == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 1UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else if (EXSRC_TEST_NAL_MID == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 0UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else {
                /* Nothing */
            }

            frame_count++;
#endif /* EXSRC_SAMPLE_DATA */
            mediaBuffer->response = BCM_ERR_OK;
        } else {
            mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
        }

        retVal = MSGQ_CompleteMsgIdx(&outPort->dataQ, mediaBufferIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSrc_ProcessData_Port1: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
        retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
        localAlarmVal = ATOMIC_Set(&AlarmVal1, 0UL);
    }
}

void ExSrc_ProcessData_Port2(const MSYS_OutPortType * const outPort)
{
    int32_t retVal;
    uint32_t mediaBufferIdx;
    uint32_t localAlarmVal;
    MSYS_BufferType * mediaBuffer;

    static uint64_t frame_count = 0;
    static uint32_t nal_offset = 0;
    nalDataInfo nalInfo;
    uint32_t copySize = 0UL;

    memset(&nalInfo, 0, sizeof(nalInfo));

    retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
    localAlarmVal = ATOMIC_Set(&AlarmVal2, 0UL);

    if(nal_offset == enc_data_array_size){
        nal_offset = 0UL;
    }

    while ((BCM_ERR_OK == retVal) && (1UL == localAlarmVal)) {
        mediaBuffer = &ExSrc_MediaBuffer[2][mediaBufferIdx];

        /* validate mediabuffer */
        if ((NULL != mediaBuffer->buffer[0].data) &&
            (0UL != mediaBuffer->buffer[0].size)){

#ifdef EXSRC_SAMPLE_DATA
            /* Fill the buffer */
            mediaBuffer->buffer[0].data[0UL] = Buffer[ExSrc_idx2++];
            if (ExSrc_idx2 >= 26UL) {
                ExSrc_idx2 = 0UL;
                mediaBuffer->syncEnd = 1UL;
            }
            mediaBuffer->buffer[0].size = 1UL;
#else
            /* Invalidate buffer as it is cached */
            DCACHE_InvalRange((uint32_t)mediaBuffer->buffer[0].data,
                                   (uint32_t)mediaBuffer->buffer[0].size);

            /* Get the current NAL to be processed */
            GetNALDataFromBuffer(&nalInfo, nal_offset);

            /* Determine the data size to be copied to port buffer */
            if (nalInfo.nalLen <= mediaBuffer->buffer[0].size){
                copySize = nalInfo.nalLen;
            } else {
                copySize = mediaBuffer->buffer[0].size;
            }

            (void) memcpy(mediaBuffer->buffer[0].data,
                          &enc_data_array[nal_offset],copySize);

            /* Update the NAL offset size */
            nal_offset += copySize;
            mediaBuffer->buffer[0].size = copySize;

            /* Set the SYnc and Frame markers for payload */
            if (EXSRC_TEST_NAL_SPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x67UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_PPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x68UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_START == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 1UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else if (EXSRC_TEST_NAL_MID == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 0UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else {
                /* Nothing */
            }

            frame_count++;
#endif /* EXSRC_SAMPLE_DATA */
            mediaBuffer->response = BCM_ERR_OK;
        } else {
            mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
        }

        retVal = MSGQ_CompleteMsgIdx(&outPort->dataQ, mediaBufferIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSrc_ProcessData_Port1: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
        retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
        localAlarmVal = ATOMIC_Set(&AlarmVal2, 0UL);
    }
}


void ExSrc_ProcessData_Port3(const MSYS_OutPortType * const outPort)
{
    int32_t retVal;
    uint32_t mediaBufferIdx;
    uint32_t localAlarmVal;
    MSYS_BufferType * mediaBuffer;

    static uint64_t frame_count = 0;
    static uint32_t nal_offset = 0;
    nalDataInfo nalInfo;
    uint32_t copySize = 0UL;

    memset(&nalInfo, 0, sizeof(nalInfo));

    retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
    localAlarmVal = ATOMIC_Set(&AlarmVal3, 0UL);

    if(nal_offset == enc_data_array_size){
        nal_offset = 0UL;
    }

    while ((BCM_ERR_OK == retVal) && (1UL == localAlarmVal)) {
        mediaBuffer = &ExSrc_MediaBuffer[3][mediaBufferIdx];

        /* validate mediabuffer */
        if ((NULL != mediaBuffer->buffer[0].data) &&
            (0UL != mediaBuffer->buffer[0].size)){

#ifdef EXSRC_SAMPLE_DATA
            /* Fill the buffer */
            mediaBuffer->buffer[0].data[0UL] = Buffer[ExSrc_idx3++];
            if (ExSrc_idx3 >= 26UL) {
                ExSrc_idx3 = 0UL;
                mediaBuffer->syncEnd = 1UL;
            }
            mediaBuffer->buffer[0].size = 1UL;
#else
            /* Invalidate buffer as it is cached */
            DCACHE_InvalRange((uint32_t)mediaBuffer->buffer[0].data,
                                   (uint32_t)mediaBuffer->buffer[0].size);

            /* Get the current NAL to be processed */
            GetNALDataFromBuffer(&nalInfo, nal_offset);

            /* Determine the data size to be copied to port buffer */
            if (nalInfo.nalLen <= mediaBuffer->buffer[0].size){
                copySize = nalInfo.nalLen;
            } else {
                copySize = mediaBuffer->buffer[0].size;
            }

            (void) memcpy(mediaBuffer->buffer[0].data,
                          &enc_data_array[nal_offset],copySize);

            /* Update the NAL offset size */
            nal_offset += copySize;
            mediaBuffer->buffer[0].size = copySize;

            /* Set the SYnc and Frame markers for payload */
            if (EXSRC_TEST_NAL_SPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x67UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_PPS == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x68UL;
                mediaBuffer->syncStart = 1UL;
                mediaBuffer->syncEnd = 1UL;
                mediaBuffer->frameStart = 0UL;
                mediaBuffer->frameEnd = 0UL;
            } else if (EXSRC_TEST_NAL_START == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 1UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else if (EXSRC_TEST_NAL_MID == nalInfo.nalType){
                mediaBuffer->streamInfo[MSYS_STREAM_INFO_0] = 0x45UL;
                mediaBuffer->syncStart = 0UL;
                if (1UL == nalInfo.isFrameStart){
                    mediaBuffer->frameStart = 1UL;
                }
                if ((copySize == nalInfo.nalLen) && (1UL == nalInfo.isFrameEnd)){
                    mediaBuffer->frameEnd = 1UL;
                }
                if (copySize == nalInfo.nalLen){
                    mediaBuffer->syncEnd = 1UL;
                }
            } else {
                /* Nothing */
            }

            frame_count++;
#endif /* EXSRC_SAMPLE_DATA */
            mediaBuffer->response = BCM_ERR_OK;
        } else {
            mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
        }

        retVal = MSGQ_CompleteMsgIdx(&outPort->dataQ, mediaBufferIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSrc_ProcessData_Port1: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
        retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
        localAlarmVal = ATOMIC_Set(&AlarmVal3, 0UL);
    }
}


TASK(ExSrc_DataProcessTask)
{
    uint32_t loop = 1UL;
    int32_t retVal;
    uint32_t mediaBufferIdx;
    BCM_EventMaskType mask;

    /*dummy call to MSGQ_GetMsgIdx to ensure buffers are freed up*/
    retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort0.dataQ, &mediaBufferIdx);
    retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort1.dataQ, &mediaBufferIdx);
    retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort2.dataQ, &mediaBufferIdx);
    retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort3.dataQ, &mediaBufferIdx);

    BCM_SetEvent(ExSrc_CmdProcessTask, EXSRC_DATA_PROCESS_ACK_EVENT);

    ULOG_INFO("ExSrc_DataProcessTask: Waiting for events\n");
    do {
        BCM_WaitEvent(EXSRC_STOP_DATA_PROCESS_EVENT |
                  SystemEvent8 |
                  SystemEvent9 |
                  SystemEvent10 |
                  SystemEvent11);

        BCM_GetEvent(ExSrc_DataProcessTask, &mask);
        if ((mask & EXSRC_STOP_DATA_PROCESS_EVENT) == EXSRC_STOP_DATA_PROCESS_EVENT) {
            ULOG_INFO("ExSrc_DataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSRC_STOP_DATA_PROCESS_EVENT);
            retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort0.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == retVal) {
                MSYS_BufferType * mediaBuffer;
                mediaBuffer = &ExSrc_MediaBuffer[0][mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                retVal = MSGQ_CompleteMsgIdx(&ExSrc_OutPort0.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
                }
            }
            retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort1.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == retVal) {
                MSYS_BufferType * mediaBuffer;
                mediaBuffer = &ExSrc_MediaBuffer[1][mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                retVal = MSGQ_CompleteMsgIdx(&ExSrc_OutPort1.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
                }
            }
            retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort2.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == retVal) {
                MSYS_BufferType * mediaBuffer;
                mediaBuffer = &ExSrc_MediaBuffer[2][mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                retVal = MSGQ_CompleteMsgIdx(&ExSrc_OutPort2.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
                }
            }
            retVal = MSGQ_GetMsgIdx(&ExSrc_OutPort3.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == retVal) {
                MSYS_BufferType * mediaBuffer;
                mediaBuffer = &ExSrc_MediaBuffer[3][mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                retVal = MSGQ_CompleteMsgIdx(&ExSrc_OutPort3.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
                }
            }
            loop = 0UL;
        }
        if ((mask & SystemEvent8) == SystemEvent8) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port 0 event notification received\n");
            BCM_ClearEvent(SystemEvent8);
            ExSrc_ProcessData_Port0(&ExSrc_OutPort0);
        }
        if ((mask & SystemEvent9) == SystemEvent9) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port 1 event notification received\n");
            BCM_ClearEvent(SystemEvent9);
            ExSrc_ProcessData_Port1(&ExSrc_OutPort1);
        }
        if ((mask & SystemEvent10) == SystemEvent10) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port 2 event notification received\n");
            BCM_ClearEvent(SystemEvent10);
            ExSrc_ProcessData_Port2(&ExSrc_OutPort2);
        }
        if ((mask & SystemEvent11) == SystemEvent11) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port 3 event notification received\n");
            BCM_ClearEvent(SystemEvent11);
            ExSrc_ProcessData_Port3(&ExSrc_OutPort3);
        }
    } while(0UL != loop);

    BCM_SetEvent(ExSrc_CmdProcessTask, EXSRC_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}
