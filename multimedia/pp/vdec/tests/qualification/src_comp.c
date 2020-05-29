/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
#include "atomic.h"

#include <ex_src_comp.h>
#include <osil/bcm_osil.h>

/* These following will be defined and loaded in avcd_test.c */
extern uint8_t BsBuffer[];
extern uint32_t BsBufSize;
extern uint32_t slice_offset[];
extern uint32_t maxNumSlices;

uint8_t *curBS[4];
uint32_t curSize[4];
uint32_t selector[4];

#define EXSRC_MAX_NUM_OUT_PORT  (VDEC_COMP_NUM_STREAMS)


MSYS_BufferType ExSrc_MediaBuffer[EXSRC_MAX_NUM_OUT_PORT][MSGQ_SIZE];

#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
void* ExSrc_GetDataMsg0(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[0UL][i];
    }

    return dataMsg;
}
#endif

#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
void* ExSrc_GetDataMsg1(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[1UL][i];
    }

    return dataMsg;
}
#endif

#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
void* ExSrc_GetDataMsg2(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[2UL][i];
    }

    return dataMsg;
}
#endif

#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
void* ExSrc_GetDataMsg3(uint32_t i)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBuffer[3UL][i];
    }

    return dataMsg;
}
#endif

#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(ExSrc_OutPort0, 0UL, ExSrc_DataMsgHdrQ0,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg0,
                        MSYS_COMP_ID_EX_SRC, SystemEvent8);
#endif

#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(ExSrc_OutPort1, 1UL, ExSrc_DataMsgHdrQ1,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg1,
                        MSYS_COMP_ID_EX_SRC, SystemEvent9);
#endif

#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(ExSrc_OutPort2, 2UL, ExSrc_DataMsgHdrQ2,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg2,
                        MSYS_COMP_ID_EX_SRC, SystemEvent10);
#endif

#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
MSYS_DECLARE_OUT_PORT(ExSrc_OutPort3, 3UL, ExSrc_DataMsgHdrQ3,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsg3,
                        MSYS_COMP_ID_EX_SRC, SystemEvent11);
#endif

/* Component Command Queue */
MSGQ_DEFINE_HDRQ(ExSrc_CmdMsgHdrQ);

/* Compoenent CmdQ Server Event */
#define EXSRC_CMDQ_SERVER_EVENT         SystemEvent0
#define EXSRC_STOP_DATA_PROCESS_EVENT   SystemEvent0
#define EXSRC_DATA_PROCESS_ACK_EVENT    SystemEvent1

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
         1UL, MSYS_CmdType, &ExSrc_CmdMsgHdrQ, ExSrc_GetCmdMsg);

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

#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
    case 0UL:
        outPort = &ExSrc_OutPort0;
        break;
#endif
#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
    case 1UL:
        outPort = &ExSrc_OutPort1;
        break;
#endif
#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
    case 2UL:
        outPort = &ExSrc_OutPort2;
        break;
#endif
#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
    case 3UL:
        outPort = &ExSrc_OutPort3;
        break;
#endif
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
#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
            MSYS_OutPortInit(&ExSrc_OutPort0);
#endif
#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
            MSYS_OutPortInit(&ExSrc_OutPort1);
#endif
#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
            MSYS_OutPortInit(&ExSrc_OutPort2);
#endif
#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
            MSYS_OutPortInit(&ExSrc_OutPort3);
#endif
            processCmd->response = BCM_ERR_OK;
            ULOG_INFO("ExSrc_ProcessCmd: %u\n", processCmd->cmd);
            break;

        case MSYS_COMP_CMD_CONFIG:
            /* FIXME: Erika specific */
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
            break;
        case MSYS_COMP_CMD_START:
            ULOG_INFO("ExSrc_ProcessCmd: %u\n", processCmd->cmd);
            BCM_SetEvent(ExSrc_DataProcessTask, SystemEvent8|SystemEvent9|SystemEvent10|SystemEvent11);
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

volatile uint32_t AlarmVal = 0UL;
void ExSrc_DataProcessTask_Cb()
{
    ATOMIC_Set(&AlarmVal, 1UL);
    SetEvent(ExSrc_DataProcessTask, SystemEvent8);
}
static uint32_t sliceNum = 0;

#define MINIMUM_SLICE_SIZE 0
void ExSrc_ProcessData(const MSYS_OutPortType * const outPort)
{
    int32_t retVal;
    uint32_t mediaBufferIdx;
    MSYS_BufferType * mediaBuffer;

    retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);

    while ((BCM_ERR_OK == retVal) /*&& (1UL == localAlarmVal)*/) {
        mediaBuffer = &ExSrc_MediaBuffer[outPort->portIndex][mediaBufferIdx];

        mediaBuffer->seqNum = sliceNum;
        mediaBuffer->response = BCM_ERR_OK;
        mediaBuffer->buffer[0].size = 0;
        if(NULL != mediaBuffer->buffer[0].data){
            /* find the resync */
            mediaBuffer->buffer[0].size = slice_offset[selector[outPort->portIndex]++];
            if(selector[outPort->portIndex] == maxNumSlices) {
                selector[outPort->portIndex] = 0;
            }

            if ( mediaBuffer->buffer[0].size > MINIMUM_SLICE_SIZE) {
                memcpy(mediaBuffer->buffer[0].data,&curBS[outPort->portIndex][0], mediaBuffer->buffer[0].size);
            }
            curBS[outPort->portIndex] += mediaBuffer->buffer[0].size;
            curSize[outPort->portIndex] -= mediaBuffer->buffer[0].size;
            if (curSize[outPort->portIndex] == 0) {
                /* end of stream */
                curBS[outPort->portIndex] = BsBuffer;
                curSize[outPort->portIndex] = BsBufSize;
            }
        }
        if( mediaBuffer->buffer[0].size > MINIMUM_SLICE_SIZE)
        {
            retVal = MSGQ_CompleteMsgIdx(&outPort->dataQ, mediaBufferIdx);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
            }
        }
        retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
    }
}
void ExSrc_ProcessStop(const MSYS_OutPortType * const outPort)
{
    int32_t retVal;
    uint32_t mediaBufferIdx;
    retVal = MSGQ_GetMsgIdx(&(outPort->dataQ), &mediaBufferIdx);
    if (BCM_ERR_OK == retVal) {
        MSYS_BufferType * mediaBuffer;
        mediaBuffer = &ExSrc_MediaBuffer[outPort->portIndex][mediaBufferIdx];
        mediaBuffer->buffer[0].size = 0UL;
        mediaBuffer->buffer[1].size = 0UL;
        mediaBuffer->buffer[2].size = 0UL;
        mediaBuffer->response = BCM_ERR_OK;
        retVal = MSGQ_CompleteMsgIdx(&(outPort->dataQ), mediaBufferIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
    }
}
TASK(ExSrc_DataProcessTask)
{
    uint32_t loop = 1UL;
    uint32_t mediaBufferIdx;
    BCM_EventMaskType mask;

    /*dummy call to MSGQ_GetMsgIdx to ensure buffers are freed up*/
#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
    MSGQ_GetMsgIdx(&ExSrc_OutPort0.dataQ, &mediaBufferIdx);
#endif
#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
    MSGQ_GetMsgIdx(&ExSrc_OutPort1.dataQ, &mediaBufferIdx);
#endif
#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
    MSGQ_GetMsgIdx(&ExSrc_OutPort2.dataQ, &mediaBufferIdx);
#endif
#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
    MSGQ_GetMsgIdx(&ExSrc_OutPort3.dataQ, &mediaBufferIdx);
#endif
    BCM_SetEvent(ExSrc_CmdProcessTask, EXSRC_DATA_PROCESS_ACK_EVENT);

    curBS[0] = BsBuffer;
    curSize[0] = BsBufSize;
    curBS[1] = BsBuffer;
    curSize[1] = BsBufSize;
    curBS[2] = BsBuffer;
    curSize[2] = BsBufSize;
    curBS[3] = BsBuffer;
    curSize[3] = BsBufSize;
    selector[0] = 0;
    selector[1] = 0;
    selector[2] = 0;
    selector[3] = 0;

    ULOG_INFO("ExSrc_DataProcessTask: Waiting for events\n");
    do {
        BCM_WaitEvent(EXSRC_STOP_DATA_PROCESS_EVENT
#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
            | SystemEvent8
#endif
#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
            |SystemEvent9
#endif
#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
            |SystemEvent10
#endif
#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
            |SystemEvent11
#endif
            );
        BCM_GetEvent(ExSrc_DataProcessTask, &mask);

#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
        if ((mask & SystemEvent8) == SystemEvent8) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port event notification received\n");
            BCM_ClearEvent(SystemEvent8);
            ExSrc_ProcessData(&ExSrc_OutPort0);
        }
#endif
#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
        if ((mask & SystemEvent9) == SystemEvent9) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port event notification received\n");
            BCM_ClearEvent(SystemEvent9);
            ExSrc_ProcessData(&ExSrc_OutPort1);
        }
#endif
#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
        if ((mask & SystemEvent10) == SystemEvent10) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port event notification received\n");
            BCM_ClearEvent(SystemEvent10);
            ExSrc_ProcessData(&ExSrc_OutPort2);
        }
#endif
#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
        if ((mask & SystemEvent11) == SystemEvent11) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port event notification received\n");
            BCM_ClearEvent(SystemEvent11);
            ExSrc_ProcessData(&ExSrc_OutPort3);
        }
#endif
        if ((mask & EXSRC_STOP_DATA_PROCESS_EVENT) == EXSRC_STOP_DATA_PROCESS_EVENT) {
            ULOG_INFO("ExSrc_DataProcessTask: Stop Cmd received\n");
            BCM_ClearEvent(EXSRC_STOP_DATA_PROCESS_EVENT);

#if (0UL < EXSRC_MAX_NUM_OUT_PORT)
            ExSrc_ProcessStop(&ExSrc_OutPort0);
#endif
#if (1UL < EXSRC_MAX_NUM_OUT_PORT)
            ExSrc_ProcessStop(&ExSrc_OutPort1);
#endif
#if (2UL < EXSRC_MAX_NUM_OUT_PORT)
            ExSrc_ProcessStop(&ExSrc_OutPort2);
#endif
#if (3UL < EXSRC_MAX_NUM_OUT_PORT)
            ExSrc_ProcessStop(&ExSrc_OutPort3);
#endif
            loop = 0UL;
        }

    } while(0UL != loop);

    BCM_SetEvent(ExSrc_CmdProcessTask, EXSRC_DATA_PROCESS_ACK_EVENT);
    BCM_TerminateTask();
}
