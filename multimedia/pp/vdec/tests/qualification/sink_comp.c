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
File Name  : src_comp.c
Description: This file contains a sample source component that can be used as a
             reference.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO
#include <string.h>
#include <media_port.h>
#include "component.h"
#include <msg_queue.h>
#include "ulog.h"
#include "bcm_err.h"
#include "ee.h"
#include "bcm_time.h"
#include "bcm_test.h"
#include "cache.h"
#include <ex_sink_comp.h>
#include <crc.h>
#include <osil/bcm_osil.h>

#define NUMBER_OF_SINK_BUFFERS  (2UL)

#define SINK_WIDTH  (640UL)
#define SINK_HEIGHT (400UL)
#define SINK_WIDTH_C (SINK_WIDTH >> 1UL)
#define SINK_HEIGHT_C (SINK_HEIGHT >> 1UL)
#define SINK_FRAME_SIZE_Y (SINK_WIDTH * SINK_HEIGHT)
#define SINK_FRAME_SIZE_C (SINK_WIDTH_C * SINK_HEIGHT_C)
#define SINK_FRAME_SIZE (SINK_FRAME_SIZE_Y + SINK_FRAME_SIZE_C + SINK_FRAME_SIZE_C)

#define EXSNK_NUM_IN_PORTS (VDEC_COMP_NUM_STREAMS)


#if (1UL == EXSNK_NUM_IN_PORTS)
#define EXSNK_INPORT_EVENTS (SystemEvent4)
#endif
#if (2UL == EXSNK_NUM_IN_PORTS)
#define EXSNK_INPORT_EVENTS (SystemEvent4|SystemEvent5)
#endif
#if (3UL == EXSNK_NUM_IN_PORTS)
#define EXSNK_INPORT_EVENTS (SystemEvent4|SystemEvent5|SystemEvent6)
#endif
#if (4UL == EXSNK_NUM_IN_PORTS)
#define EXSNK_INPORT_EVENTS (SystemEvent4|SystemEvent5|SystemEvent6|SystemEvent7)
#endif

COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.data") outImage[EXSNK_NUM_IN_PORTS][NUMBER_OF_SINK_BUFFERS][SINK_FRAME_SIZE];

static uint32_t ExSnk_DoneCnt = 0UL;

static BCM_TimeType localTime;
static BCM_TimeType GlobalTime = {0};
static uint32_t gFrameCount[4UL]={0,0,0,0};
static uint32_t testCnt[4UL]={0,0,0,0};
static void ExSnk_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    if (aMediaBuffer->buffer[0].size != 0UL) {
        BCM_GetTime(&localTime);
        gFrameCount[aMediaBuffer->bufIdx]++;
        if(GlobalTime.s != localTime.s) {
            ULOG_INFO("FPS: (%u,%u,%u,%u)  \n", gFrameCount[0UL],
                                             gFrameCount[1UL],
                                             gFrameCount[2UL],
                                             gFrameCount[3UL]);
            gFrameCount[0UL]=0;
            gFrameCount[1UL]=0;
            gFrameCount[2UL]=0;
            gFrameCount[3UL]=0;
            GlobalTime = localTime;
        }
    }
    ExSnk_DoneCnt++;
}

#if (0UL < EXSNK_NUM_IN_PORTS)
MSYS_DECLARE_IN_PORT(ExSnk_InPort0, 0UL, MSYS_COMP_ID_EX_SNK, SystemEvent4,
                       ExSnk_InPortRW0, ExSnk_FillBufferDone);
#endif
#if (1UL < EXSNK_NUM_IN_PORTS)
MSYS_DECLARE_IN_PORT(ExSnk_InPort1, 1UL, MSYS_COMP_ID_EX_SNK, SystemEvent5,
                       ExSnk_InPortRW1, ExSnk_FillBufferDone);
#endif
#if (2UL < EXSNK_NUM_IN_PORTS)
MSYS_DECLARE_IN_PORT(ExSnk_InPort2, 2UL, MSYS_COMP_ID_EX_SNK, SystemEvent6,
                       ExSnk_InPortRW2, ExSnk_FillBufferDone);
#endif
#if (3UL < EXSNK_NUM_IN_PORTS)
MSYS_DECLARE_IN_PORT(ExSnk_InPort3, 3UL, MSYS_COMP_ID_EX_SNK, SystemEvent7,
                       ExSnk_InPortRW3, ExSnk_FillBufferDone);
#endif
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
         1UL, MSYS_CmdType, &ExSnk_CmdMsgHdrQ, ExSnk_GetCmdMsg);

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
            ULOG_INFO("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            break;

        case MSYS_COMP_CMD_CONFIG:
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
            break;
        case MSYS_COMP_CMD_START:
            ULOG_INFO("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            gFrameCount[0UL]=0;
            gFrameCount[1UL]=0;
            gFrameCount[2UL]=0;
            gFrameCount[3UL]=0;
            BCM_SetEvent(ExSnk_DataProcessTask, EXSNK_INPORT_EVENTS);
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

static uint32_t ExSnk_idx[4] = {0UL,0UL,0UL,0UL};

void ExSnk_ProcessData(const MSYS_InPortType* const aInPort)
{
    int32_t i;
    int32_t retVal;
    MSYS_BufferType mediaBuffer;

    do {
        mediaBuffer.bufIdx = aInPort->portIndex;
        mediaBuffer.timestamp = 0ULL;
        mediaBuffer.numChannels = 3UL;
        for (i=0;i<3;i++) {
            mediaBuffer.buffer[i].attribs[0UL] = ExSnk_idx[aInPort->portIndex] & 0xFFUL;
            if (i==0) {
                mediaBuffer.buffer[i].data = &outImage[aInPort->portIndex][ExSnk_idx[aInPort->portIndex]][0];
                mediaBuffer.buffer[i].size = (SINK_FRAME_SIZE_Y);
                mediaBuffer.buffer[i].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = (SINK_WIDTH);
                mediaBuffer.buffer[i].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = (SINK_HEIGHT);
                mediaBuffer.buffer[i].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = (SINK_WIDTH);
            } else {
                mediaBuffer.buffer[i].data = &outImage[aInPort->portIndex][ExSnk_idx[aInPort->portIndex]][(SINK_FRAME_SIZE_Y) + ((i-1)*(SINK_FRAME_SIZE_C))];
                mediaBuffer.buffer[i].size = (SINK_FRAME_SIZE_C);
                mediaBuffer.buffer[i].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = (SINK_WIDTH_C);
                mediaBuffer.buffer[i].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = (SINK_HEIGHT_C);
                mediaBuffer.buffer[i].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = (SINK_WIDTH_C);
            }
        }

        mediaBuffer.response = BCM_ERR_UNKNOWN;

        retVal = MSYS_FillBuffer(aInPort, &mediaBuffer);
        if (BCM_ERR_OK == retVal) {
            ExSnk_idx[aInPort->portIndex] = (ExSnk_idx[aInPort->portIndex]+1) % NUMBER_OF_SINK_BUFFERS;
        }
    } while (retVal == BCM_ERR_OK);
}

TASK(ExSnk_DataProcessTask)
{
    uint32_t loop = 1UL;
    BCM_EventMaskType mask;
    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    //ExSnk_ProcessData();
    ULOG_INFO("ExSnk_DataProcessTask: Waiting for events\n");
    ExSnk_idx[0] = 0UL;
    ExSnk_idx[1] = 0UL;
    ExSnk_idx[2] = 0UL;
    ExSnk_idx[3] = 0UL;
    testCnt[0] = 0UL;
    testCnt[1] = 0UL;
    testCnt[2] = 0UL;
    testCnt[3] = 0UL;

    do {
        BCM_WaitEvent(EXSNK_STOP_DATA_PROCESS_EVENT | EXSNK_INPORT_EVENTS);
        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent\n");
        BCM_GetEvent(ExSnk_DataProcessTask, &mask);
        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent %x\n",mask);
        if ((mask & EXSNK_STOP_DATA_PROCESS_EVENT) == EXSNK_STOP_DATA_PROCESS_EVENT) {
            ULOG_INFO("ExSnk_DataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSNK_STOP_DATA_PROCESS_EVENT | EXSNK_INPORT_EVENTS);

#if (0UL < EXSNK_NUM_IN_PORTS)
            MSYS_InPortCancelAll(&ExSnk_InPort0);
#endif
#if (1UL < EXSNK_NUM_IN_PORTS)
            MSYS_InPortCancelAll(&ExSnk_InPort1);
#endif
#if (2UL < EXSNK_NUM_IN_PORTS)
            MSYS_InPortCancelAll(&ExSnk_InPort2);
#endif
#if (3UL < EXSNK_NUM_IN_PORTS)
            MSYS_InPortCancelAll(&ExSnk_InPort3);
#endif
            loop = 0UL;
        } else {

#if (0UL < EXSNK_NUM_IN_PORTS)
            if ((mask & SystemEvent4) == SystemEvent4) {
                ULOG_VERB("ExSnk_DataProcessTask: Input port event received\n");
                BCM_ClearEvent(SystemEvent4);
                MSYS_InPortProcess(&ExSnk_InPort0);
                ExSnk_ProcessData(&ExSnk_InPort0);
            }
#endif
#if (1UL < EXSNK_NUM_IN_PORTS)
            if ((mask & SystemEvent5) == SystemEvent5) {
                ULOG_VERB("ExSnk_DataProcessTask: Input port event received\n");
                BCM_ClearEvent(SystemEvent5);
                MSYS_InPortProcess(&ExSnk_InPort1);
                ExSnk_ProcessData(&ExSnk_InPort1);
            }
#endif
#if (2UL < EXSNK_NUM_IN_PORTS)
            if ((mask & SystemEvent6) == SystemEvent6) {
                ULOG_VERB("ExSnk_DataProcessTask: Input port event received\n");
                BCM_ClearEvent(SystemEvent6);
                MSYS_InPortProcess(&ExSnk_InPort2);
                ExSnk_ProcessData(&ExSnk_InPort2);
            }
#endif
#if (3UL < EXSNK_NUM_IN_PORTS)
            if ((mask & SystemEvent7) == SystemEvent7) {
                ULOG_VERB("ExSnk_DataProcessTask: Input port event received\n");
                BCM_ClearEvent(SystemEvent7);
                MSYS_InPortProcess(&ExSnk_InPort3);
                ExSnk_ProcessData(&ExSnk_InPort3);
            }
#endif
        }
    } while(0UL != loop);

    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}
