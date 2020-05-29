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
File Name  : ex_sink_comp_core.c
Description: This file contains a sink component for camera_qt.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO
#include <string.h>
#include <media_port.h>
#include "component.h"
#include <msg_queue.h>
#include "ulog.h"
#include <bcm_err.h>
#include "ee.h"
#include <isys.h>
#include <isys_tuner.h>
#include <osil/bcm_osil.h>

#include "ex_sink_comp.h"

/* Macros */
#define APP_Y_SIZE  (MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL * 2UL)
#define APP_UV_SIZE ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH >> 1) * 8UL)

/* Frames to wait for */
#define APP_TARGET_FC (10UL)

/* Return number of slices for "x" frames */
#define NUM_SLICES(x) ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH/16UL)*(x))

#define EXSNK_CMDQ_SERVER_EVENT         SystemEvent0
#define EXSNK_DATA_PROCESS_ACK_EVENT    SystemEvent1
#define EXSNK_STOP_DATA_PROCESS_EVENT   SystemEvent0
#define EXSNK_AUX_PROCESS_SIGNAL        SystemEvent1

/* Globals */
static uint32_t gExSnkDoneCnt = 0UL;
static uint32_t gBufIdx = 0UL;
static uint32_t gStatsNum = 0UL;

/* Allocation for app buffers */
static uint8_t COMP_SECTION(".cam.yuv.uncached") gYBuffer[2][APP_Y_SIZE]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") gUBuffer[2][APP_UV_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") gVBuffer[2][APP_UV_SIZE] COMP_ALIGN(256);

static uint8_t COMP_SECTION(".cam.yuv.uncached") gMetaBuf[10240]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") gISYS_StatsBuf[10240]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") gSensorStatsBuf[7680]  COMP_ALIGN(256);

#ifdef ENABLE_CAM_MODULE_OV10640
const uint32_t gCB_Y_CRC = 0xbb84366fUL;
const uint32_t gCB_U_CRC = 0xabe544daUL;
const uint32_t gCB_V_CRC = 0xe58431baUL;
const uint32_t gCB_RawCRC = 0xac82a822UL;
#elif defined ENABLE_CAM_MODULE_OX3A10
const uint32_t gCB_Y_CRC = 0x6060113eUL;
const uint32_t gCB_U_CRC = 0x34a215a0UL;
const uint32_t gCB_V_CRC = 0xe66beda7UL;
const uint32_t gCB_RawCRC = 0xac82a822UL;
#elif defined ENABLE_CAM_MODULE_AR0143
const uint32_t gCB_Y_CRC = 0x54cf44f2UL;
const uint32_t gCB_U_CRC = 0x14a9911bUL;
const uint32_t gCB_V_CRC = 0x6612ee05UL;
const uint32_t gCB_RawCRC = 0xac82a822UL;
#endif

extern uint32_t camera_qt_test_num;

#define CRC32_POLY (0x04C11DB7UL)
static uint32_t Reverse(uint32_t aValue)
{
    uint32_t val = aValue;

    val = ((val & 0x55555555UL) << 1UL) | ((val >> 1UL) & 0x55555555UL);
    val = ((val & 0x33333333UL) << 2UL) | ((val >> 2UL) & 0x33333333UL);
    val = ((val & 0x0F0F0F0FUL) << 4UL) | ((val >> 4UL) & 0x0F0F0F0FUL);
    val = (val << 24UL) | ((val & 0xFF00UL) << 8UL) |
        ((val >> 8UL) & 0xFF00UL) | (val >> 24UL);

    return val;
}

static uint32_t CalculateCRC(const uint8_t *const aAddr,
                             uint32_t aLen)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;

    i = 0UL;
    crc = 0UL;
    while (aLen != 0UL) {
        byte = aAddr[i];
        byte = Reverse(byte);
        for (j = 0UL; j <= 7UL; j++) {
            if (((int32_t)(crc ^ byte)) < 0L) {
                crc = (crc << 1UL) ^ CRC32_POLY;
            } else {
                crc = crc << 1UL;
            }
            byte = byte << 1UL;
        }
        i++;
        aLen--;
    }

    return Reverse(~crc);
}

/* ExSnk_FillBufferDone */
static void ExSnk_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    static uint32_t notMatched = 0UL;

    if (0UL == gExSnkDoneCnt) {
        uint32_t yCRC, uCRC, vCRC;
        uint32_t yRefCRC, uRefCRC, vRefCRC;

        notMatched = 0UL;

        yRefCRC = gCB_Y_CRC;
        uRefCRC = gCB_U_CRC;
        vRefCRC = gCB_V_CRC;

        yCRC = CalculateCRC(aMediaBuffer->buffer[0].data, aMediaBuffer->buffer[0].size);
        if (2UL == camera_qt_test_num) {
            if (yCRC != gCB_RawCRC) {
                notMatched = 1UL;
                ULOG_ERR("rawCRC = 0x%x \n", yCRC);
            }
        } else {
            uCRC = CalculateCRC(aMediaBuffer->buffer[1].data, aMediaBuffer->buffer[1].size);
            vCRC = CalculateCRC(aMediaBuffer->buffer[2].data, aMediaBuffer->buffer[2].size);
            if ((yCRC != yRefCRC) ||
                    (uCRC != uRefCRC) ||
                    (vCRC != vRefCRC)) {
                notMatched = 1UL;
                ULOG_ERR("yCRC = 0x%x \n", yCRC);
                ULOG_ERR("uCRC = 0x%x \n", uCRC);
                ULOG_ERR("vCRC = 0x%x \n", vCRC);
            }
        }

        if (notMatched) {
            BCM_SetEvent(CameraTestTask, CAMERA_TEST_EVENT_FAIL);
        }
    }

    if ((0UL == notMatched) &&
        (NUM_SLICES(APP_TARGET_FC) < gExSnkDoneCnt)) {
        if ((1UL != camera_qt_test_num) ||
            (APP_TARGET_FC < gStatsNum)) {
            BCM_SetEvent(CameraTestTask, CAMERA_TEST_EVENT_PASS);
        }
    }

    if (0UL != aMediaBuffer->frameEnd) {
        if (1UL == camera_qt_test_num) {
            BCM_SetEvent(ExSnk_AuxDataProcessTask, EXSNK_AUX_PROCESS_SIGNAL);
        }
    }

    gExSnkDoneCnt++;
}

/* ExSnk_Aux_FillBufferDone */
static void ExSnk_Aux_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    ISYS_StatisticsType *stats = (ISYS_StatisticsType *) aMediaBuffer->buffer[1].data;

    if (BCM_ERR_OK == aMediaBuffer->response) {
        gStatsNum = stats->frameNum;
    } else {
        ULOG_ERR("ExSnk_Aux_FillBufferDone: failed(%d) \n", aMediaBuffer->response);
    }
}

MSYS_DECLARE_IN_PORT(ExSnk_InPort, 0UL, MSYS_COMP_ID_EX_SNK, SystemEvent4,
                       ExSnk_InPortRW, ExSnk_FillBufferDone);

MSYS_DECLARE_IN_PORT(ExSnk_Aux_InPort, 1UL, MSYS_COMP_ID_EX_SNK, SystemEvent5,
                       ExSnk_Aux_InPortRW, ExSnk_Aux_FillBufferDone);

/* Component Command Queue */
MSGQ_DEFINE_HDRQ(ExSnk_CmdMsgHdrQ);
/* Component CmdQ Server Event */

MSYS_CmdType ExSnk_CmdMsgQ[MSGQ_SIZE];

void* ExSnk_GetCmdMsg(uint32_t i)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&ExSnk_CmdMsgQ[i];
    }

    return cmdMsg;
}

MSGQ_DEFINE(ExSnk_CmdQ, ExSnk_CmdProcessTask, EXSNK_CMDQ_SERVER_EVENT,
            MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &ExSnk_CmdMsgHdrQ, ExSnk_GetCmdMsg);

/* Post a command to the command queue */
int32_t ExSnk_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_EX_SNK == aCmd->compID)
        && (NULL != aHdr)) {
        /* Post the command to the component's Command Queue */
        retVal = MSGQ_SendMsg(&ExSnk_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        ULOG_ERR("ExSnk_SendCmd: BCM_ERR_INVAL_PARAMS\n");
    }

    return retVal;
}

/* Receive response for the command posted previously */
int32_t ExSnk_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        retVal = MSGQ_RecvMsg(&ExSnk_CmdQ, aHdr, (void* const) aCmd);
    } else {
        ULOG_ERR("ExSnk_SendCmd: Invalid parameters\n");
    }

    return retVal;
}

/* Get the number of output ports */
uint32_t ExSnk_GetNumMediaOutPort(void)
{
    return 0UL;
}

/* Get the MSYS_OutPortType where the message needs to be posted to */
const MSYS_OutPortType* ExSnk_GetOutPort(const uint32_t aIndex)
{
    ULOG_ERR("ExSnk_GetOutPort: unknown PortIdx %u\n", aIndex);
    return NULL;
}

/* Process the command received on the command queue */
void ExSnk_ProcessCmd(void)
{
    int32_t retVal;
    uint32_t cmdMsgIdx;
    MSYS_CmdType *processCmd;
    int32_t status;

    retVal = MSGQ_GetMsgIdx(&ExSnk_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSnk_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;

        switch(processCmd->cmd)
        {
        case MSYS_COMP_CMD_INIT:
        case MSYS_COMP_CMD_DEINIT:
            processCmd->response = BCM_ERR_OK;
            ULOG_VERB("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            break;
        case MSYS_COMP_CMD_CONFIG:
            gExSnkDoneCnt = 0UL;
            gBufIdx = 0UL;
            /* FIXME: Erika specific */
            status = BCM_ActivateTask(ExSnk_DataProcessTask);
            if (BCM_ERR_OK != status) {
                ULOG_ERR("ExSnk_ProcessCmd: data process start failed (%d)\n",
                         status);
                processCmd->response = BCM_ERR_UNKNOWN;
            } else {
                BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                processCmd->response = BCM_ERR_OK;
            }
            if (1UL == camera_qt_test_num) {
                status = BCM_ActivateTask(ExSnk_AuxDataProcessTask);
                if (BCM_ERR_OK != status) {
                    ULOG_ERR("ExSnk_ProcessCmd: data process start failed (%d)\n",
                             status);
                    processCmd->response = BCM_ERR_UNKNOWN;
                } else {
                    BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                    BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                    processCmd->response = BCM_ERR_OK;
                }
            }
            break;
        case MSYS_COMP_CMD_START:
            ULOG_VERB("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            BCM_SetEvent(ExSnk_DataProcessTask, SystemEvent4);
            if (1UL == camera_qt_test_num) {
                BCM_SetEvent(ExSnk_AuxDataProcessTask, SystemEvent5);
            }
            processCmd->response = BCM_ERR_OK;
        break;

        case MSYS_COMP_CMD_STOP:
            /* FIXME: Erika specific */
            /* NOTE: The data processing task has a single waiting point.
             * Depending on the event which is triggered, the data processing
             * task has to proceed. In this case, it has to cancel all the
             * posted messages and then terminate.
             */
            BCM_SetEvent(ExSnk_DataProcessTask, EXSNK_STOP_DATA_PROCESS_EVENT);
            BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
            BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
            if (1UL == camera_qt_test_num) {
                BCM_SetEvent(ExSnk_AuxDataProcessTask, EXSNK_STOP_DATA_PROCESS_EVENT);
                BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
            }
            processCmd->response = BCM_ERR_OK;
            break;

        default:
            ULOG_ERR("ExSnk_ProcessCmd: unknown command (%u)\n",
                     processCmd->cmd);
            processCmd->response = BCM_ERR_INVAL_PARAMS;
            break;
        }   /* end of  switch(processCmd->cmd)*/

        retVal = MSGQ_CompleteMsgIdx(&ExSnk_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSnk_ProcessCmd: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
    }
}

TASK(ExSnk_CmdProcessTask)
{
    do {
        BCM_WaitEvent(EXSNK_CMDQ_SERVER_EVENT);
        BCM_ClearEvent(EXSNK_CMDQ_SERVER_EVENT);
        ExSnk_ProcessCmd();
    } while(1UL);
}

void ExSnk_ProcessData()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;
    uint32_t yStride;

    if (2UL == camera_qt_test_num) {
        yStride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 2UL;
    } else {
        yStride = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
    }

    do {
        mediaBuffer.timestamp = 0ULL;
        if (0UL == gBufIdx++) {
            mediaBuffer.buffer[0].data = (uint8_t *) &gYBuffer[0];
            mediaBuffer.buffer[0].size = APP_Y_SIZE;
            mediaBuffer.buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = yStride;
            mediaBuffer.buffer[1].data = (uint8_t *) &gUBuffer[0];
            mediaBuffer.buffer[1].size = APP_UV_SIZE;
            mediaBuffer.buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;
            mediaBuffer.buffer[2].data = (uint8_t *) &gVBuffer[0];
            mediaBuffer.buffer[2].size = APP_UV_SIZE;
            mediaBuffer.buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;
        } else {
            mediaBuffer.buffer[0].data = (uint8_t *) &gYBuffer[1];
            mediaBuffer.buffer[0].size = APP_Y_SIZE;
            mediaBuffer.buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = yStride;
            mediaBuffer.buffer[1].data = (uint8_t *) &gUBuffer[1];
            mediaBuffer.buffer[1].size = APP_UV_SIZE;
            mediaBuffer.buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;
            mediaBuffer.buffer[2].data = (uint8_t *) &gVBuffer[1];
            mediaBuffer.buffer[2].size = APP_UV_SIZE;
            mediaBuffer.buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;
        }
        if (2UL == camera_qt_test_num) {
            mediaBuffer.numChannels = 1UL;
        } else {
            mediaBuffer.numChannels = 3UL;
        }
        mediaBuffer.response = BCM_ERR_UNKNOWN;

        retVal = MSYS_FillBuffer(&ExSnk_InPort, &mediaBuffer);
    } while (retVal == BCM_ERR_OK);
}

TASK(ExSnk_DataProcessTask)
{
    uint32_t loop = 1UL;
    BCM_EventMaskType mask = 0UL;
    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    ULOG_VERB("ExSnk_DataProcessTask: Waiting for events\n");
    do {
        BCM_WaitEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent4);
        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent\n");
        BCM_GetEvent(ExSnk_DataProcessTask, &mask);
        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent %x\n",mask);
        if ((mask & EXSNK_STOP_DATA_PROCESS_EVENT) == EXSNK_STOP_DATA_PROCESS_EVENT) {
            ULOG_VERB("ExSnk_DataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent4);
            MSYS_InPortCancelAll(&ExSnk_InPort);
            loop = 0UL;
        } else if ((mask & SystemEvent4) == SystemEvent4) {
            ULOG_VERB("ExSnk_DataProcessTask: Input port event received\n");
            BCM_ClearEvent(SystemEvent4);
            MSYS_InPortProcess(&ExSnk_InPort);
            ExSnk_ProcessData();
        }
    } while(0UL != loop);

    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}

void ExSnk_ProcessAuxData()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;

    mediaBuffer.timestamp = 0ULL;
    mediaBuffer.buffer[0].data = (uint8_t *) &gMetaBuf[0];
    mediaBuffer.buffer[0].size = sizeof(gMetaBuf);
    mediaBuffer.buffer[1].data = (uint8_t *) &gISYS_StatsBuf[0];
    mediaBuffer.buffer[1].size = sizeof(gISYS_StatsBuf);
    mediaBuffer.buffer[2].data = (uint8_t *) &gSensorStatsBuf[0];
    mediaBuffer.buffer[2].size = sizeof(gSensorStatsBuf);
    mediaBuffer.numChannels = 3UL;
    mediaBuffer.response = BCM_ERR_UNKNOWN;

    retVal = MSYS_FillBuffer(&ExSnk_Aux_InPort, &mediaBuffer);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ExSnk_ProcessAuxData: MSYS_FillBuffer failed(%d)\n", retVal);
    }
}

TASK(ExSnk_AuxDataProcessTask)
{
    uint32_t loop = 1UL;
    BCM_EventMaskType mask = 0UL;
    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    ULOG_VERB("ExSnk_AuxDataProcessTask: Waiting for events\n");
    do {
        BCM_WaitEvent(EXSNK_STOP_DATA_PROCESS_EVENT |
                  SystemEvent5 |
                  EXSNK_AUX_PROCESS_SIGNAL);
        ULOG_VERB("ExSnk_AuxDataProcessTask: BCM_GetEvent\n");
        BCM_GetEvent(ExSnk_AuxDataProcessTask, &mask);
        ULOG_VERB("ExSnk_AuxDataProcessTask: BCM_GetEvent %x\n",mask);
        if ((mask & EXSNK_STOP_DATA_PROCESS_EVENT) == EXSNK_STOP_DATA_PROCESS_EVENT) {
            ULOG_VERB("ExSnk_AuxDataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSNK_STOP_DATA_PROCESS_EVENT |
                       SystemEvent5 |
                       EXSNK_AUX_PROCESS_SIGNAL);
            MSYS_InPortCancelAll(&ExSnk_Aux_InPort);
            loop = 0UL;
        } else {
            if ((mask & SystemEvent5) == SystemEvent5) {
                BCM_ClearEvent(SystemEvent5);
                MSYS_InPortProcess(&ExSnk_Aux_InPort);
            }
            if ((mask & EXSNK_AUX_PROCESS_SIGNAL) == EXSNK_AUX_PROCESS_SIGNAL) {
                BCM_ClearEvent(EXSNK_AUX_PROCESS_SIGNAL);
                ExSnk_ProcessAuxData();
            }
        }
    } while(0UL != loop);

    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}
