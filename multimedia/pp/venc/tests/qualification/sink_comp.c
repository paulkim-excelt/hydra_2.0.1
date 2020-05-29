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
File Name  : sink_comp.c
Description: This file contains a test sink component that can be used as a
             test sink component.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO
#include <string.h>
#include <media_port.h>
#include "component.h"
#include <msg_queue.h>
#include <bcm_utils.h>
#include "ulog.h"
#include "bcm_err.h"
#include "ee.h"
#include "bcm_time.h"
#include "bcm_test.h"

#include <ex_sink_comp.h>

#include <cache.h>
#include <crc.h>
#include <osil/bcm_osil.h>

#define CRC32_POLY             (0x04C11DB7UL)
#define NUMBER_OF_SINK_BUFFERS (4UL)

static uint32_t ExSnk_DoneCnt = 0UL;

static uint32_t SliceCount = 0;

static BCM_TimeType localTime;
static BCM_TimeType GlobalTime = {0};
static uint32_t gFrameCount=0;
static uint32_t BPS=0;
extern volatile int32_t venc_qt1_result;

#ifdef VENC_ENABLE_P_FRAME
#define TOTAL_CRC_CHECK_COUNT  (4UL)
#define SINK_MAX_SLICE_SIZE    (512*1024)
#define SINK_BUFFER_SIZE       (16UL*1024UL)
static uint32_t RefCRC[TOTAL_CRC_CHECK_COUNT] = {
0x072a857f,0x013a0ed6,0x8c85444e,0xa5943714
};
static uint8_t COMP_ALIGN(32) COMP_SECTION(".pp.slowmem.test")  sliceBuffer[SINK_MAX_SLICE_SIZE];

#else
#define TOTAL_CRC_CHECK_COUNT  (48UL)
#define SINK_MAX_SLICE_SIZE    (4UL*1024UL)
#define SINK_BUFFER_SIZE       (1440UL)
static uint32_t RefCRC[TOTAL_CRC_CHECK_COUNT] = {
0x072a857f,0x013a0ed6,0xe31e7eea,0x752a094a,
0x4f52e4af,0xbc491026,0x49155295,0xd42e8ee2,
0xdd202bf1,0xd3523165,0x75f867ff,0x7cb115f8,
0xff3e8b7e,0xf677f979,0x50ddafe3,0xcc9862bd,
0xe7f49874,0x95ff8e42,0x04da65e5,0xc5d110ba,
0x5dbd891a,0x9cb6fc45,0xac019085,0xde0a86b3,
0xf592d435,0x91701f66,0xe5683084,0x3d3c8523,
0xce2771aa,0x8a217d12,0x44dda251,0xc06aed2a,
0x640a345e,0xc2a062c4,0xcbe910c3,0x48668e45,
0x412ffc42,0xe785aad8,0x5cda5b73,0x77b6a1ba,
0x05bdb78c,0x94985c2b,0x55932974,0xcdffb0d4,
0x0cf4c58b,0x3c43a94b,0x4e48bf7d,0x7d6084dd
};

static uint8_t COMP_SECTION(".pp.slowmem.test") sliceBuffer[SINK_MAX_SLICE_SIZE];
#endif



static uint32_t sliceBufferSize;
static uint8_t  COMP_ALIGN(32) COMP_SECTION(".bcm.test") mediaBuf[NUMBER_OF_SINK_BUFFERS][SINK_BUFFER_SIZE];

int32_t ExSnk_CheckDataValidity()
{
    int32_t retVal = BCM_ERR_DATA_INTEG;

    if ((TOTAL_CRC_CHECK_COUNT == SliceCount) && (BCM_AT_EXECUTING == venc_qt1_result)) {
        retVal = BCM_ERR_OK;
    }

    return retVal;
}
static void ExSnk_FillBufferDone(MSYS_BufferType *const aMediaBuffer)
{
    if (aMediaBuffer->buffer[0].size != 0UL) {
        BCM_GetTime(&localTime);

        BPS += aMediaBuffer->buffer[0].size;
        if (SliceCount < TOTAL_CRC_CHECK_COUNT) {
            if (aMediaBuffer->syncStart == 1) {
                sliceBufferSize = 0;
            }
            if ((sliceBufferSize + aMediaBuffer->buffer[0].size) < SINK_MAX_SLICE_SIZE) {
                BCM_MemCpy(&sliceBuffer[sliceBufferSize], aMediaBuffer->buffer[0].data, aMediaBuffer->buffer[0].size);
                sliceBufferSize += aMediaBuffer->buffer[0].size;
                if (aMediaBuffer->syncEnd == 1) {
                    uint32_t crcCalc = BCM_CRC32(sliceBuffer, sliceBufferSize, CRC32_POLY);
                    if (RefCRC[SliceCount] != crcCalc) {
                        venc_qt1_result = BCM_ERR_DATA_INTEG;
                        ULOG_ERR("%d mismatch %08x \n",SliceCount,crcCalc);
                    }
                    SliceCount++;
                }
            } else {
                ULOG_ERR("Max slice Size exceeded existing %d new %d\n",sliceBufferSize, aMediaBuffer->buffer[0].size);
            }

        }

        if (GlobalTime.s != localTime.s) {
            GlobalTime = localTime;
            ULOG_INFO("gFrameCount: %u rec frame num :%u BPS: %u \n", aMediaBuffer->frameNum-gFrameCount,aMediaBuffer->frameNum,BPS);
            gFrameCount = aMediaBuffer->frameNum;
            BPS = 0;
        }

    }
    ExSnk_DoneCnt++;
}

MSYS_DECLARE_IN_PORT(ExSnk_InPort, 0UL, MSYS_COMP_ID_EX_SNK, SystemEvent4,
                       ExSnk_InPortRW, ExSnk_FillBufferDone);

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

static uint32_t ExSnk_idx = 0UL;
/* Process the command received on the command queue */
void ExSnk_ProcessCmd(void)
{
    int32_t retVal;
    uint32_t cmdMsgIdx;
    MSYS_CmdType *processCmd;
    StatusType status;

    retVal = MSGQ_GetMsgIdx(&ExSnk_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSnk_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;

        switch(processCmd->cmd)
        {
        case MSYS_COMP_CMD_INIT:
            SliceCount = 0;
        case MSYS_COMP_CMD_DEINIT:
            ExSnk_idx = 0;
            processCmd->response = BCM_ERR_OK;
            ULOG_INFO("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            break;

        case MSYS_COMP_CMD_CONFIG:
            /* FIXME: Erika specific */
            status = BCM_ActivateTask(ExSnk_DataProcessTask);
            if (E_OK != status) {
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
            BCM_SetEvent(ExSnk_DataProcessTask, SystemEvent4);
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


void ExSnk_ProcessData()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;

    do {
        mediaBuffer.timestamp = 0ULL;
        mediaBuffer.numChannels = 1UL;
        mediaBuffer.buffer[0].attribs[0UL] = ExSnk_idx & 0xFFUL;
        mediaBuffer.buffer[0].data = &mediaBuf[ExSnk_idx][0];
        mediaBuffer.buffer[0].size = SINK_BUFFER_SIZE;
        mediaBuffer.response = BCM_ERR_UNKNOWN;

        retVal = MSYS_FillBuffer(&ExSnk_InPort, &mediaBuffer);

        if (BCM_ERR_OK == retVal) {
            ExSnk_idx = (ExSnk_idx+1) % NUMBER_OF_SINK_BUFFERS;
        }
    } while (retVal == BCM_ERR_OK);
}

TASK(ExSnk_DataProcessTask)
{
    uint32_t loop = 1UL;

    BCM_EventMaskType mask = 0UL;
    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);

    ULOG_INFO("ExSnk_DataProcessTask: Waiting for events\n");
    do {
        BCM_WaitEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent4);
        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent\n");
        BCM_GetEvent(ExSnk_DataProcessTask, &mask);
        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent %x\n",mask);
        if ((mask & EXSNK_STOP_DATA_PROCESS_EVENT) == EXSNK_STOP_DATA_PROCESS_EVENT) {
            ULOG_INFO("ExSnk_DataProcessTask: Stop Cmd received\n");
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
