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
File Name  : ex_sink_comp_core.c
Description: This file implements sink component for the test application.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO
#include <string.h>
#include <media_port.h>
#include <component.h>
#include <msg_queue.h>
#include <ulog.h>
#include <bcm_err.h>
#include <ee.h>
#include <isys.h>
#include <osil/bcm_osil.h>

#include "ex_sink_comp.h"

/* Macros */
#define APP_Y_SIZE  (MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL)
#define APP_UV_SIZE ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH >> 1) * 8UL)

/* Number of frames to wait before declaring pass */
#define NUM_FRAMES_WAIT (10UL)

/* Globals */
static uint32_t gHV_ExSnkDoneCnt = 0UL;
static uint32_t gCV_ExSnkDoneCnt = 0UL;

/* Allocation for app buffers */
static uint8_t COMP_SECTION(".cam.yuv.uncached") gYBuffer[APP_Y_SIZE]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") gUBuffer[APP_UV_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") gVBuffer[APP_UV_SIZE] COMP_ALIGN(256);

static uint8_t COMP_SECTION(".cam.raw.slowmem") gCV_YBuffer[MEDIA_SYSTEM_MAX_VIDEO_WIDTH*MEDIA_SYSTEM_MAX_VIDEO_HEIGHT]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.raw.slowmem") gCV_UBuffer[MEDIA_SYSTEM_MAX_VIDEO_WIDTH*MEDIA_SYSTEM_MAX_VIDEO_HEIGHT/4] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.raw.slowmem") gCV_VBuffer[MEDIA_SYSTEM_MAX_VIDEO_WIDTH*MEDIA_SYSTEM_MAX_VIDEO_HEIGHT/4] COMP_ALIGN(256);

/* ExSnk_HV_FillBufferDone */
static void ExSnk_HV_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    static uint32_t frameCount = 0UL;
    gHV_ExSnkDoneCnt++;

    if (frameCount != (gHV_ExSnkDoneCnt/(MEDIA_SYSTEM_MAX_VIDEO_HEIGHT/16UL))) {
        frameCount = gHV_ExSnkDoneCnt/(MEDIA_SYSTEM_MAX_VIDEO_HEIGHT/16UL);

        if ((0UL == (frameCount % 30UL)) && (0 != frameCount)) {
            ULOG_INFO("Rxd HV frames \n");
        }
    }

    if ((NUM_FRAMES_WAIT < frameCount) && (NUM_FRAMES_WAIT < gCV_ExSnkDoneCnt)) {
        BCM_SetEvent(CameraDP_TestTask, CAMERA_TEST_EVENT_PASS);
    }
}

/* ExSnk_CV_FillBufferDone */
static void ExSnk_CV_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    gCV_ExSnkDoneCnt++;
    if (0UL == (gCV_ExSnkDoneCnt % 30UL)) {
        ULOG_INFO("Rxd CV frames \n");
    }
}

MSYS_DECLARE_IN_PORT(ExSnk_HV_InPort, 0UL, MSYS_COMP_ID_EX_SNK, SystemEvent4,
                       ExSnk_HV_InPortRW, ExSnk_HV_FillBufferDone);

MSYS_DECLARE_IN_PORT(ExSnk_CV_InPort, 1UL, MSYS_COMP_ID_EX_SNK, SystemEvent5,
                       ExSnk_CV_InPortRW, ExSnk_CV_FillBufferDone);

/* Component Command Queue */
MSGQ_DEFINE_HDRQ(ExSnk_CmdMsgHdrQ);
/* Compoenent CmdQ Server Event */
#define EXSNK_CMDQ_SERVER_EVENT         SystemEvent0
#define EXSNK_STOP_DATA_PROCESS_EVENT   SystemEvent0
#define EXSNK_DATA_PROCESS_ACK_EVENT    SystemEvent1

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
            gHV_ExSnkDoneCnt = 0UL;
            gCV_ExSnkDoneCnt = 0UL;
            ULOG_VERB("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            break;
        case MSYS_COMP_CMD_CONFIG:
            /* FIXME: Erika specific */
            status = BCM_ActivateTask(ExSnk_HV_DataProcessTask);
            if (BCM_ERR_OK != status) {
                ULOG_ERR("ExSnk_ProcessCmd: data process start failed (%d)\n",
                         status);
                processCmd->response = BCM_ERR_UNKNOWN;
            } else {
                BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                processCmd->response = BCM_ERR_OK;
            }
            status = BCM_ActivateTask(ExSnk_CV_DataProcessTask);
            if (BCM_ERR_OK != status) {
                ULOG_ERR("ExSnk_ProcessCmd: data process start failed (%d)\n",
                         status);
                processCmd->response = BCM_ERR_UNKNOWN;
            } else {
                BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
                processCmd->response = BCM_ERR_OK;
            }
            break;
        case MSYS_COMP_CMD_START:
            ULOG_VERB("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            BCM_SetEvent(ExSnk_HV_DataProcessTask, SystemEvent4);
            BCM_SetEvent(ExSnk_CV_DataProcessTask, SystemEvent5);
            processCmd->response = BCM_ERR_OK;
        break;

        case MSYS_COMP_CMD_STOP:
            /* FIXME: Erika specific */
            /* NOTE: The data processing task has a single waiting point.
             * Depending on the event which is triggered, the data processing
             * task has to proceed. In this case, it has to cancel all the
             * posted messages and then terminate.
             */
            BCM_SetEvent(ExSnk_HV_DataProcessTask, EXSNK_STOP_DATA_PROCESS_EVENT);
            BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
            BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
            BCM_SetEvent(ExSnk_CV_DataProcessTask, EXSNK_STOP_DATA_PROCESS_EVENT);
            BCM_WaitEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
            BCM_ClearEvent(EXSNK_DATA_PROCESS_ACK_EVENT);
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

void ExSnk_HV_ProcessData()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;

    do {
        mediaBuffer.timestamp = 0ULL;
        mediaBuffer.buffer[0].data = (uint8_t *) &gYBuffer;
        mediaBuffer.buffer[0].size = APP_Y_SIZE;
        mediaBuffer.buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
        mediaBuffer.buffer[1].data = (uint8_t *) &gUBuffer;
        mediaBuffer.buffer[1].size = APP_UV_SIZE;
        mediaBuffer.buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;
        mediaBuffer.buffer[2].data = (uint8_t *) &gVBuffer;
        mediaBuffer.buffer[2].size = APP_UV_SIZE;
        mediaBuffer.buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;
        mediaBuffer.numChannels = 3UL;
        mediaBuffer.response = BCM_ERR_UNKNOWN;

        retVal = MSYS_FillBuffer(&ExSnk_HV_InPort, &mediaBuffer);
    } while (retVal == BCM_ERR_OK);
}

TASK(ExSnk_HV_DataProcessTask)
{
    uint32_t loop = 1UL;
    BCM_EventMaskType mask = 0UL;
    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    ULOG_VERB("ExSnk_HV_DataProcessTask: Waiting for events\n");
    do {
        BCM_WaitEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent4);
        ULOG_VERB("ExSnk_HV_DataProcessTask: BCM_GetEvent\n");
        BCM_GetEvent(ExSnk_HV_DataProcessTask, &mask);
        ULOG_VERB("ExSnk_HV_DataProcessTask: BCM_GetEvent %x\n",mask);
        if ((mask & EXSNK_STOP_DATA_PROCESS_EVENT) == EXSNK_STOP_DATA_PROCESS_EVENT) {
            ULOG_VERB("ExSnk_HV_DataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent4);
            MSYS_InPortCancelAll(&ExSnk_HV_InPort);
            loop = 0UL;
        } else if ((mask & SystemEvent4) == SystemEvent4) {
            ULOG_VERB("ExSnk_HV_DataProcessTask: Input port event received\n");
            BCM_ClearEvent(SystemEvent4);
            MSYS_InPortProcess(&ExSnk_HV_InPort);
            ExSnk_HV_ProcessData();
        }
    } while(0UL != loop);

    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}

/* ExSnk_CV_ProcessData */
void ExSnk_CV_ProcessData()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;

    do {
        mediaBuffer.timestamp = 0ULL;

        mediaBuffer.buffer[0].data = (uint8_t *) &gCV_YBuffer;
        mediaBuffer.buffer[0].size = MEDIA_SYSTEM_MAX_VIDEO_WIDTH*MEDIA_SYSTEM_MAX_VIDEO_HEIGHT;
        mediaBuffer.buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
        mediaBuffer.buffer[1].data = (uint8_t *) &gCV_UBuffer;
        mediaBuffer.buffer[1].size = MEDIA_SYSTEM_MAX_VIDEO_WIDTH*MEDIA_SYSTEM_MAX_VIDEO_HEIGHT/4;
        mediaBuffer.buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;
        mediaBuffer.buffer[2].data = (uint8_t *) &gCV_VBuffer;
        mediaBuffer.buffer[2].size = MEDIA_SYSTEM_MAX_VIDEO_WIDTH*MEDIA_SYSTEM_MAX_VIDEO_HEIGHT/4;
        mediaBuffer.buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = MEDIA_SYSTEM_MAX_VIDEO_WIDTH/2UL;

        mediaBuffer.numChannels = 3UL;
        mediaBuffer.response = BCM_ERR_UNKNOWN;

        retVal = MSYS_FillBuffer(&ExSnk_CV_InPort, &mediaBuffer);
    } while (retVal == BCM_ERR_OK);
}

/* ExSnk_CV_DataProcessTask */
TASK(ExSnk_CV_DataProcessTask)
{
    uint32_t loop = 1UL;
    BCM_EventMaskType mask = 0UL;
    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    ULOG_VERB("ExSnk_CV_DataProcessTask: Waiting for events\n");
    do {
        BCM_WaitEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent5);
        ULOG_VERB("ExSnk_CV_DataProcessTask: BCM_GetEvent\n");
        BCM_GetEvent(ExSnk_CV_DataProcessTask, &mask);
        ULOG_VERB("ExSnk_CV_DataProcessTask: BCM_GetEvent %x\n",mask);
        if ((mask & EXSNK_STOP_DATA_PROCESS_EVENT) == EXSNK_STOP_DATA_PROCESS_EVENT) {
            ULOG_VERB("ExSnk_CV_DataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent5);
            MSYS_InPortCancelAll(&ExSnk_CV_InPort);
            loop = 0UL;
        } else if ((mask & SystemEvent5) == SystemEvent5) {
            ULOG_VERB("ExSnk_CV_DataProcessTask: Input port event received\n");
            BCM_ClearEvent(SystemEvent5);
            MSYS_InPortProcess(&ExSnk_CV_InPort);
            ExSnk_CV_ProcessData();
        }
    } while(0UL != loop);

    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}
