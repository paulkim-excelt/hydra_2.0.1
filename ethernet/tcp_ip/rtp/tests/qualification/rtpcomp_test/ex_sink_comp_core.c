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
File Name  : src_comp.c
Description: This file contains a sample source component that can be used as a
             reference.
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO
#include <string.h>
#include <media_port.h>
#include "component.h"
#include <msg_queue.h>
#include <circ_queue.h>
#include "ulog.h"
#include <bcm_err.h>
#include <osil/bcm_osil.h>
#include "ee.h"

#include <ex_sink_comp.h>

/* Circular Queue to keep track of port input buffers */
#define BUF_SIZE          (40*1024)
typedef struct _BufMem {
    uint8_t bufIdx;
} BufMem;

#define NUM_INPUT_BUFFERS 2UL

static BufMem dataPkts0[NUM_INPUT_BUFFERS];
static uint8_t COMP_SECTION(".dma.bcm.test") dataBuf0[NUM_INPUT_BUFFERS][BUF_SIZE];

static BufMem dataPkts1[NUM_INPUT_BUFFERS];
static uint8_t COMP_SECTION(".dma.bcm.test") dataBuf1[NUM_INPUT_BUFFERS][BUF_SIZE];

static BufMem dataPkts2[NUM_INPUT_BUFFERS];
static uint8_t COMP_SECTION(".dma.bcm.test") dataBuf2[NUM_INPUT_BUFFERS][BUF_SIZE];

static BufMem dataPkts3[NUM_INPUT_BUFFERS];
static uint8_t COMP_SECTION(".dma.bcm.test") dataBuf3[NUM_INPUT_BUFFERS][BUF_SIZE];

CIRCQ_INDEX_DEFINE(bufQIdx0, 4UL);
CIRCQ_DEFINE(bufQ_0, &bufQIdx0, 4UL,
                  sizeof(BufMem), dataPkts0, CIRCQ_OVERWRITE_DISABLE);

CIRCQ_INDEX_DEFINE(bufQIdx1, 4UL);
CIRCQ_DEFINE(bufQ_1, &bufQIdx1, 4UL,
                  sizeof(BufMem), dataPkts1, CIRCQ_OVERWRITE_DISABLE);

CIRCQ_INDEX_DEFINE(bufQIdx2, 4UL);
CIRCQ_DEFINE(bufQ_2, &bufQIdx2, 4UL,
                  sizeof(BufMem), dataPkts2, CIRCQ_OVERWRITE_DISABLE);

CIRCQ_INDEX_DEFINE(bufQIdx3, 4UL);
CIRCQ_DEFINE(bufQ_3, &bufQIdx3, 4UL,
                  sizeof(BufMem), dataPkts3, CIRCQ_OVERWRITE_DISABLE);

#define MAX_RTP_DUMP_SIZE (80*1024)

uint8_t COMP_SECTION(".dma.bcm.test") RTP1722DumpArray0 [MAX_RTP_DUMP_SIZE];
uint32_t ExSnk_Port0DataSize = 0UL;
static uint32_t ExSnk_DoneCnt0 = 0UL;

uint8_t COMP_SECTION(".dma.bcm.test") RTP1722DumpArray1 [MAX_RTP_DUMP_SIZE];
uint32_t ExSnk_Port1DataSize = 0UL;
static uint32_t ExSnk_DoneCnt1 = 0UL;

uint8_t COMP_SECTION(".dma.bcm.test") RTP1722DumpArray2 [MAX_RTP_DUMP_SIZE];
uint32_t ExSnk_Port2DataSize = 0UL;
static uint32_t ExSnk_DoneCnt2 = 0UL;

uint8_t COMP_SECTION(".dma.bcm.test") RTP1722DumpArray3 [MAX_RTP_DUMP_SIZE];
uint32_t ExSnk_Port3DataSize = 0UL;
static uint32_t ExSnk_DoneCnt3 = 0UL;

static void ExSnk_FillBufferDone0(MSYS_BufferType *const aMediaBuffer)
{
    BufMem buf;
    if ((aMediaBuffer->buffer[0].size != 0UL) &&
        ((ExSnk_Port0DataSize+aMediaBuffer->buffer[0].size) < MAX_RTP_DUMP_SIZE)){

        (void)memcpy(&RTP1722DumpArray0[ExSnk_Port0DataSize],
                     aMediaBuffer->buffer[0].data,
                     aMediaBuffer->buffer[0].size);

        ExSnk_Port0DataSize += aMediaBuffer->buffer[0].size;
    }

    if (NULL != aMediaBuffer->buffer[0].data) {
        buf.bufIdx = aMediaBuffer->bufIdx;
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_0,
                               (const char*)&buf,
                               1UL)) {
            ULOG_ERR("Ex Sink> FBD ERROR!!!\n");
        }
    }

    ExSnk_DoneCnt0++;
}

static void ExSnk_FillBufferDone1(MSYS_BufferType *const aMediaBuffer)
{
    BufMem buf;
    if ((aMediaBuffer->buffer[0].size != 0UL) &&
        ((ExSnk_Port1DataSize+aMediaBuffer->buffer[0].size) < MAX_RTP_DUMP_SIZE)){

        (void)memcpy(&RTP1722DumpArray1[ExSnk_Port1DataSize],
                     aMediaBuffer->buffer[0].data,
                     aMediaBuffer->buffer[0].size);

        ExSnk_Port1DataSize += aMediaBuffer->buffer[0].size;
    }

    if (NULL != aMediaBuffer->buffer[0].data) {
        buf.bufIdx = aMediaBuffer->bufIdx;
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_1,
                               (const char*)&buf,
                               1UL)) {
        }
    }

    ExSnk_DoneCnt1++;
}

static void ExSnk_FillBufferDone2(MSYS_BufferType *const aMediaBuffer)
{
    BufMem buf;
    if ((aMediaBuffer->buffer[0].size != 0UL) &&
        ((ExSnk_Port2DataSize+aMediaBuffer->buffer[0].size) < MAX_RTP_DUMP_SIZE)){

        (void)memcpy(&RTP1722DumpArray2[ExSnk_Port2DataSize],
                     aMediaBuffer->buffer[0].data,
                     aMediaBuffer->buffer[0].size);

        ExSnk_Port2DataSize += aMediaBuffer->buffer[0].size;
    }

    if (NULL != aMediaBuffer->buffer[0].data) {
        buf.bufIdx = aMediaBuffer->bufIdx;
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_2,
                               (const char*)&buf,
                               1UL)) {
        }
    }

    ExSnk_DoneCnt2++;
}

static void ExSnk_FillBufferDone3(MSYS_BufferType *const aMediaBuffer)
{
    BufMem buf;
    if ((aMediaBuffer->buffer[0].size != 0UL) &&
        ((ExSnk_Port3DataSize+aMediaBuffer->buffer[0].size) < MAX_RTP_DUMP_SIZE)){

        (void)memcpy(&RTP1722DumpArray3[ExSnk_Port3DataSize],
                     aMediaBuffer->buffer[0].data,
                     aMediaBuffer->buffer[0].size);

        ExSnk_Port3DataSize += aMediaBuffer->buffer[0].size;
    }

    if (NULL != aMediaBuffer->buffer[0].data) {
        buf.bufIdx = aMediaBuffer->bufIdx;
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_3,
                               (const char*)&buf,
                               1UL)) {
        }
    }

    ExSnk_DoneCnt3++;
}

MSYS_DECLARE_IN_PORT(ExSnk_InPort0, 0UL, MSYS_COMP_ID_EX_SNK, SystemEvent4,
                       ExSnk_InPortRW0, ExSnk_FillBufferDone0);

MSYS_DECLARE_IN_PORT(ExSnk_InPort1, 1UL, MSYS_COMP_ID_EX_SNK, SystemEvent5,
                       ExSnk_InPortRW1, ExSnk_FillBufferDone1);

MSYS_DECLARE_IN_PORT(ExSnk_InPort2, 2UL, MSYS_COMP_ID_EX_SNK, SystemEvent6,
                       ExSnk_InPortRW2, ExSnk_FillBufferDone2);

MSYS_DECLARE_IN_PORT(ExSnk_InPort3, 3UL, MSYS_COMP_ID_EX_SNK, SystemEvent7,
                       ExSnk_InPortRW3, ExSnk_FillBufferDone3);


/* Component Command Queue */
//TODO: ExSnk_CmdProcessTask task has to be defined in code.c
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
            ULOG_INFO("ExSnk_ProcessCmd: %u\n", processCmd->cmd);
            ExSnk_Port0DataSize = 0UL;
            ExSnk_Port1DataSize = 0UL;
            ExSnk_Port2DataSize = 0UL;
            ExSnk_Port3DataSize = 0UL;
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
#if (RTP_MAX_NUM_OUTPUT_PORTS>0)
            BCM_SetEvent(ExSnk_DataProcessTask, SystemEvent4);
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>1)
            BCM_SetEvent(ExSnk_DataProcessTask, SystemEvent5);
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>2)
            BCM_SetEvent(ExSnk_DataProcessTask, SystemEvent6);
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>3)
            BCM_SetEvent(ExSnk_DataProcessTask, SystemEvent7);
#endif
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
            ExSnk_Port0DataSize = 0UL;
            ExSnk_Port1DataSize = 0UL;
            ExSnk_Port2DataSize = 0UL;
            ExSnk_Port3DataSize = 0UL;
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

static uint32_t ExSnk_idx0 = 0UL;
static uint32_t ExSnk_idx1 = 0UL;
static uint32_t ExSnk_idx2 = 0UL;
static uint32_t ExSnk_idx3 = 0UL;

void ExSnk_ProcessData_0()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;
    BufMem buf;

    buf.bufIdx = 0U;

    do {
        mediaBuffer.timestamp = 0ULL;
        if (1UL != CIRCQ_Pop(&bufQ_0, (char* const)&buf, 1UL)) {
            break;
        }
        mediaBuffer.buffer[0].data = &(dataBuf0[buf.bufIdx][0]);

        mediaBuffer.buffer[0].size = BUF_SIZE;
        mediaBuffer.bufIdx = buf.bufIdx;
        mediaBuffer.numChannels = 1UL;
        mediaBuffer.response = BCM_ERR_UNKNOWN;
        ExSnk_idx0++;

        retVal = MSYS_FillBuffer(&ExSnk_InPort0, &mediaBuffer);
    } while (retVal == BCM_ERR_OK);
}

void ExSnk_ProcessData_1()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;
    BufMem buf;

    buf.bufIdx = 0U;

    do {
        mediaBuffer.timestamp = 0ULL;
        if (1UL != CIRCQ_Pop(&bufQ_1, (char* const)&buf, 1UL)) {
            break;
        }
        mediaBuffer.buffer[0].data = &(dataBuf1[buf.bufIdx][0]);

        mediaBuffer.buffer[0].size = BUF_SIZE;
        mediaBuffer.bufIdx = buf.bufIdx;
        mediaBuffer.numChannels = 1UL;
        mediaBuffer.response = BCM_ERR_UNKNOWN;
        ExSnk_idx1++;

        retVal = MSYS_FillBuffer(&ExSnk_InPort1, &mediaBuffer);
    } while (retVal == BCM_ERR_OK);
}

void ExSnk_ProcessData_2()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;
    BufMem buf;

    buf.bufIdx = 0U;

    do {
        mediaBuffer.timestamp = 0ULL;
        if (1UL != CIRCQ_Pop(&bufQ_2, (char* const)&buf, 1UL)) {
            break;
        }
        mediaBuffer.buffer[0].data = &(dataBuf2[buf.bufIdx][0]);

        mediaBuffer.buffer[0].size = BUF_SIZE;
        mediaBuffer.bufIdx = buf.bufIdx;
        mediaBuffer.numChannels = 1UL;
        mediaBuffer.response = BCM_ERR_UNKNOWN;
        ExSnk_idx2++;

        retVal = MSYS_FillBuffer(&ExSnk_InPort2, &mediaBuffer);
    } while (retVal == BCM_ERR_OK);
}

void ExSnk_ProcessData_3()
{
    int32_t retVal;
    MSYS_BufferType mediaBuffer;
    BufMem buf;

    buf.bufIdx = 0U;

    do {
        mediaBuffer.timestamp = 0ULL;
        if (1UL != CIRCQ_Pop(&bufQ_3, (char* const)&buf, 1UL)) {
            break;
        }
        mediaBuffer.buffer[0].data = &(dataBuf3[buf.bufIdx][0]);

        mediaBuffer.buffer[0].size = BUF_SIZE;
        mediaBuffer.bufIdx = buf.bufIdx;
        mediaBuffer.numChannels = 1UL;
        mediaBuffer.response = BCM_ERR_UNKNOWN;
        ExSnk_idx3++;

        retVal = MSYS_FillBuffer(&ExSnk_InPort3, &mediaBuffer);
    } while (retVal == BCM_ERR_OK);
}

TASK(ExSnk_DataProcessTask)
{
    uint32_t loop = 1UL;
    BCM_EventMaskType mask;
    int32_t rc = 0;
    uint32_t i=0;
    BufMem buf;

    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);

    ULOG_INFO("ExSnk_DataProcessTask: Waiting for events\n");
    rc = CIRCQ_Reset(&bufQ_0);
    if (BCM_ERR_OK != rc) {
        ULOG_ERR("%s: error in CIRCQ_Reset 0\n", __func__);
    }
    rc = CIRCQ_Reset(&bufQ_1);
    if (BCM_ERR_OK != rc) {
        ULOG_ERR("%s: error in CIRCQ_Reset 1\n", __func__);
    }
    rc = CIRCQ_Reset(&bufQ_2);
    if (BCM_ERR_OK != rc) {
        ULOG_ERR("%s: error in CIRCQ_Reset 2\n", __func__);
    }
    rc = CIRCQ_Reset(&bufQ_3);
    if (BCM_ERR_OK != rc) {
        ULOG_ERR("%s: error in CIRCQ_Reset 3\n", __func__);
    }

    /* Push all buffers needed for input port into queue */
    for (; i < NUM_INPUT_BUFFERS; i++) {
        buf.bufIdx = i;
        ULOG_ERR("Init : %d\n",buf.bufIdx);
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_0, (const char*)&buf, 1UL)) {
            ULOG_VERB("%s: error in CIRCQ_Push 0 \n", __func__);
        }
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_1, (const char*)&buf, 1UL)) {
            ULOG_VERB("%s: error in CIRCQ_Push 1\n", __func__);
        }
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_2, (const char*)&buf, 1UL)) {
            ULOG_VERB("%s: error in CIRCQ_Push 2\n", __func__);
        }
        if(BCM_ERR_OK != CIRCQ_Push(&bufQ_3, (const char*)&buf, 1UL)) {
            ULOG_VERB("%s: error in CIRCQ_Push 3\n", __func__);
        }
    }

    do {
        BCM_WaitEvent(EXSNK_STOP_DATA_PROCESS_EVENT |
                  SystemEvent4 |
                  SystemEvent5 |
                  SystemEvent6 |
                  SystemEvent7);

        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent\n");
        BCM_GetEvent(ExSnk_DataProcessTask, &mask);
        ULOG_VERB("ExSnk_DataProcessTask: BCM_GetEvent %x\n",mask);
        if ((mask & EXSNK_STOP_DATA_PROCESS_EVENT) == EXSNK_STOP_DATA_PROCESS_EVENT) {
            ULOG_INFO("ExSnk_DataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSNK_STOP_DATA_PROCESS_EVENT | SystemEvent4);
            MSYS_InPortCancelAll(&ExSnk_InPort0);
            MSYS_InPortCancelAll(&ExSnk_InPort1);
            loop = 0UL;
#if (RTP_MAX_NUM_OUTPUT_PORTS>0)
        } else if ((mask & SystemEvent4) == SystemEvent4) {
            ULOG_VERB("ExSnk_DataProcessTask: Input port0 event received\n");
            BCM_ClearEvent(SystemEvent4);
            MSYS_InPortProcess(&ExSnk_InPort0);
            ExSnk_ProcessData_0();
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>1)
        } else if ((mask & SystemEvent5) == SystemEvent5) {
            ULOG_VERB("ExSnk_DataProcessTask: Input port1 event received\n");
            BCM_ClearEvent(SystemEvent5);
            MSYS_InPortProcess(&ExSnk_InPort1);
            ExSnk_ProcessData_1();
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>2)
        } else if ((mask & SystemEvent6) == SystemEvent6) {
            ULOG_VERB("ExSnk_DataProcessTask: Input port2 event received\n");
            BCM_ClearEvent(SystemEvent6);
            MSYS_InPortProcess(&ExSnk_InPort2);
            ExSnk_ProcessData_2();
#endif
#if (RTP_MAX_NUM_OUTPUT_PORTS>3)
        } else if ((mask & SystemEvent7) == SystemEvent7) {
            ULOG_VERB("ExSnk_DataProcessTask: Input port3 event received\n");
            BCM_ClearEvent(SystemEvent7);
            MSYS_InPortProcess(&ExSnk_InPort3);
            ExSnk_ProcessData_3();
#endif
        } else {
        }
    } while(0UL != loop);

    BCM_SetEvent(ExSnk_CmdProcessTask, EXSNK_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}
