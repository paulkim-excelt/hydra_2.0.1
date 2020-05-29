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
File Name  : ex_src_comp_core.c
Description: This file implements a source component that can be used to test
             video renderer sink component
 ******************************************************************************/
#define GetModuleLogLevel()       ULOG_LVL_INFO
#include <string.h>
#include <media_port.h>
#include "component.h"
#include <msg_queue.h>
#include <osil/bcm_osil.h>
#include "ulog.h"
#include <bcm_err.h>
#include "ee.h"
#include "atomic.h"
#include "bcm_time.h"
#include "vgc_mwb.h"
#include "vgc_osil.h"
#include "cache.h"

#include "ex_src_comp.h"

#define HEIGHT  720UL
#define WIDTH   1280UL

#define MAX_COLOURS  3UL
static uint32_t DeInit = 0UL;

/* Maximum number of output ports */
#define EXSRC_MAX_NUM_OUT_PORT  2UL

MSYS_BufferType ExSrc_MediaBufferVideo[MSGQ_SIZE];
void* ExSrc_GetDataMsgVideo(uint32_t i)
{
    void *dataMsg;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBufferVideo[i];
    } else {
        dataMsg = NULL;
    }

    return dataMsg;
}

MSYS_DECLARE_OUT_PORT(ExSrc_OutPortVideo, 0UL, ExSrc_DataMsgHdrQVideo,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsgVideo,
                        MSYS_COMP_ID_EX_SRC, SystemEvent8);

MSYS_BufferType ExSrc_MediaBufferGfx[MSGQ_SIZE];
void* ExSrc_GetDataMsgGfx(uint32_t i)
{
    void *dataMsg;
    if (MSGQ_SIZE > i) {
        dataMsg = (void *)&ExSrc_MediaBufferGfx[i];
    } else {
        dataMsg = NULL;
    }

    return dataMsg;
}

MSYS_DECLARE_OUT_PORT(ExSrc_OutPortGfx, 1UL, ExSrc_DataMsgHdrQGfx,
                        ExSrc_DataProcessTask, ExSrc_GetDataMsgGfx,
                        MSYS_COMP_ID_EX_SRC, SystemEvent9);

/* Component Command Queue */
MSGQ_DEFINE_HDRQ(ExSrc_CmdMsgHdrQ);

/* Compoenent CmdQ Server Event */
#define EXSRC_CMDQ_SERVER_EVENT         SystemEvent0
#define EXSRC_STOP_DATA_PROCESS_EVENT   SystemEvent0
#define EXSRC_DATA_PROCESS_ACK_EVENT    SystemEvent1
#define EXSRC_GET_STATS_EVENT           SystemEvent2
#define EXSRC_DISP_RESP_EVENT           SystemEvent3

MSYS_CmdType ExSrc_CmdMsgQ[MSGQ_SIZE];

void* ExSrc_GetCmdMsg(uint32_t i)
{
    void *cmdMsg;
    if(MSGQ_SIZE > i) {
        cmdMsg = (void *)&ExSrc_CmdMsgQ[i];
    } else {
        cmdMsg = NULL;
    }

    return cmdMsg;
}

MSGQ_DEFINE(ExSrc_CmdQ, ExSrc_CmdProcessTask, EXSRC_CMDQ_SERVER_EVENT,
            MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &ExSrc_CmdMsgHdrQ, ExSrc_GetCmdMsg);

/* Post a command to the command queue */
int32_t ExSrc_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask, const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_EX_SRC == aCmd->compID)
        && (NULL != aHdr)) {
        /* Post the command to the component's Command Queue */
        retVal = MSGQ_SendMsg(&ExSrc_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        ULOG_ERR("ExSrc_SendCmd: BCM_ERR_INVAL_PARAMS\n");
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/* Receive response for the command posted previously */
int32_t ExSrc_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        retVal = MSGQ_RecvMsg(&ExSrc_CmdQ, aHdr, (void* const) aCmd);
    } else {
        ULOG_ERR("ExSrc_SendCmd: Invalid parameters\n");
        retVal = BCM_ERR_INVAL_PARAMS;
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
    const MSYS_OutPortType *outPort;

    switch(aIndex)
    {
    case 0UL:
        outPort = &ExSrc_OutPortVideo;
        break;
    case 1UL:
        outPort = &ExSrc_OutPortGfx;
        break;
    default:
        outPort = NULL;
        ULOG_ERR("ExSrc_GetOutPort: unknown PortIdx %u\n", aIndex);
        break;
    }

    return outPort;
}

/* Process the command received on the command queue */
void ExSrc_ProcessCmd(void)
{
    int32_t retVal;
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd;
    int32_t status;

    retVal = MSGQ_GetMsgIdx(&ExSrc_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &ExSrc_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;

        ULOG_INFO("ExSrc_ProcessCmd: %u\n", processCmd->cmd);
        switch(processCmd->cmd)
        {
        case MSYS_COMP_CMD_INIT:
            processCmd->response = BCM_ERR_OK;
            break;

        case MSYS_COMP_CMD_DEINIT:
            processCmd->response = BCM_ERR_OK;
            DeInit = 0UL;
            break;

        case MSYS_COMP_CMD_CONFIG:
            status = BCM_ActivateTask(ExSrc_DataProcessTask);
            if (BCM_ERR_OK != status) {
                ULOG_ERR("ExSrc_ProcessCmd: data process config failed (%d)\n",
                         status);
                processCmd->response = BCM_ERR_UNKNOWN;
            } else {
                status = BCM_WaitEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
                if (BCM_ERR_OK != status) {
                    /* log error */
                }
                status = BCM_ClearEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
                if (BCM_ERR_OK != status) {
                    /* log error */
                }
                processCmd->response = BCM_ERR_OK;
            }
            break;
        case MSYS_COMP_CMD_START:
            status = BCM_SetEvent(ExSrc_DataProcessTask, SystemEvent8 | SystemEvent9);
            if (BCM_ERR_OK != status) {
                /* log error */
            }
            processCmd->response = BCM_ERR_OK;
        break;

        case MSYS_COMP_CMD_STOP:
            /* NOTE: The data processing task has a single waiting point.
             * Depending on the event which is triggered, the data processing
             * task has to proceed. In this case, it has to cancel all the
             * posted messages and then terminate.
             */
            status = BCM_SetEvent(ExSrc_DataProcessTask, EXSRC_STOP_DATA_PROCESS_EVENT);
            if (BCM_ERR_OK != status) {
                /* log error */
            }
            status = BCM_WaitEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
            if (BCM_ERR_OK != status) {
                /* log error */
            }
            status = BCM_ClearEvent(EXSRC_DATA_PROCESS_ACK_EVENT);
            if (BCM_ERR_OK != status) {
                /* log error */
            }
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
    BCM_EventMaskType mask = 0U;
    int32_t status;
    DeInit = 0UL;

    do {
        status = BCM_WaitEvent(EXSRC_CMDQ_SERVER_EVENT);
        if (BCM_ERR_OK != status) {
            /* log error */
        }
        status = BCM_GetEvent(ExSrc_CmdProcessTask, &mask);
        if (BCM_ERR_OK != status) {
            /* log error */
        } else {
            if (0UL != (mask & EXSRC_CMDQ_SERVER_EVENT)) {
                status = BCM_ClearEvent(EXSRC_CMDQ_SERVER_EVENT);
                if (E_OK != status) {
                    /* log error */
                }
                ExSrc_ProcessCmd();
            }
        }
    } while(0UL == DeInit);
    status = BCM_TerminateTask();
    if (BCM_ERR_OK != status) {
        /* log error */
    }
}

volatile uint32_t AlarmVal = 0UL;
void ExSrc_DataProcessTask_Cb()
{
    StatusType status;
    ATOMIC_Set(&AlarmVal, 1UL);
    status = SetEvent(ExSrc_DataProcessTask, SystemEvent8 | SystemEvent9);
    if (E_OK  != status) {
        /* log error */
    }
}

typedef struct _YUV {
    uint8_t y[16UL][WIDTH];
    uint8_t u[16UL][WIDTH];
    uint8_t v[16UL][WIDTH];
} YUV;

static uint8_t pixel;
void fill_rect(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2, uint8_t *aBuff)
{
    //YUV *yuv = (YUV *)aBuff;
    int32_t i,j;

    for (i = 0; i < WIDTH*16UL; i++)
        *aBuff++ = 0x1F;//pixel;
    for (i = 0; i < WIDTH*4UL*2; i++)
        *aBuff++ = 0x80;
    /* For each line in the window */
    for (i = aY1; i <= aY2; i++) {
        for (j = aX1; j <= aX2; j++) {
            //yuv->y[i][j] = aPix->y_r;
            //yuv->u[i][j] = aPix->cb_g;
            //yuv->v[i][j] = aPix->cr_b;
        }
    }
    DCACHE_CleanAll();
}

static uint32_t mbRowNumVideo = 0;
static uint32_t mbRowNumGfx = 0;

void ExSrc_ProcessData(const MSYS_OutPortType * const outPort)
{
    int32_t retVal;
    uint32_t mediaBufferIdx = 0UL;
    MSYS_BufferType * mediaBuffer;

    retVal = MSGQ_GetMsgIdx(&outPort->dataQ, &mediaBufferIdx);
    if (BCM_ERR_OK != retVal) {
        ULOG_VERB("MSGQ_GetMsgIdx failed %d for %d\n", retVal,outPort->portIndex);
        /* log info */
    } else {
        if (outPort->portIndex == 0UL) {
            mediaBuffer = &ExSrc_MediaBufferVideo[mediaBufferIdx];
            /* validate mediabuffer */
            if ((NULL != mediaBuffer->buffer[0].data)
                    && (0UL != mediaBuffer->buffer[0].size)){
                fill_rect(  0,  0,  320, 16, mediaBuffer->buffer[0].data);
                mediaBuffer->seqNum = mbRowNumVideo;
                mbRowNumVideo++;
                if (mbRowNumVideo == 45) {
                    pixel++;
                    mbRowNumVideo = 0;
                }
                mediaBuffer->response = BCM_ERR_OK;
            } else {
                mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
            }
        } else {
            mediaBuffer = &ExSrc_MediaBufferGfx[mediaBufferIdx];
            /* validate mediabuffer */
            if ((NULL != mediaBuffer->buffer[0].data)
                && (0UL != mediaBuffer->buffer[0].size)){
                if (mbRowNumGfx & 4) {
                    if (MSYS_CHANNEL_TYPE_ARGB == mediaBuffer->buffer[0].channelType) {
                        memset(mediaBuffer->buffer[0].data, 0x00, mediaBuffer->buffer[0].size);
                    } else {
                        memset(mediaBuffer->buffer[0].data, 0x00, mediaBuffer->buffer[0].size);
                        memset(mediaBuffer->buffer[1].data, 0x00, mediaBuffer->buffer[1].size);
                    }
                } else {
                    if (MSYS_CHANNEL_TYPE_ARGB == mediaBuffer->buffer[0].channelType) {
                        memset(mediaBuffer->buffer[0].data, 0xFF, mediaBuffer->buffer[0].size);
                    } else {
                        memset(mediaBuffer->buffer[0].data, 0x00, mediaBuffer->buffer[0].size);
                        memset(mediaBuffer->buffer[1].data, 0xFF, mediaBuffer->buffer[1].size);
                    }
                }
                DCACHE_CleanAll();
                mediaBuffer->seqNum = mbRowNumGfx;
                mbRowNumGfx++;
                if (mbRowNumGfx == 45) {
                    pixel++;
                    mbRowNumGfx = 0;
                }
                mediaBuffer->response = BCM_ERR_OK;
            } else {
                mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
            }
        }
        retVal = MSGQ_CompleteMsgIdx(&outPort->dataQ, mediaBufferIdx);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
        }
    }
}

TASK(ExSrc_DataProcessTask)
{
    int32_t retVal;
    uint32_t mediaBufferIdx = 0UL;
    BCM_EventMaskType mask = 0U;
    int32_t status;
    uint32_t loop = 1UL;

    /*dummy call to MSGQ_GetMsgIdx to ensure buffers are freed up*/
    retVal = MSGQ_GetMsgIdx(&ExSrc_OutPortVideo.dataQ, &mediaBufferIdx);
    if (BCM_ERR_OK != retVal) {
        /* log info */
    }
    retVal = MSGQ_GetMsgIdx(&ExSrc_OutPortGfx.dataQ, &mediaBufferIdx);
    if (BCM_ERR_OK != retVal) {
        /* log info */
    }

    status = BCM_SetEvent(ExSrc_CmdProcessTask, EXSRC_DATA_PROCESS_ACK_EVENT);
    if (BCM_ERR_OK != status) {
        /* log error */
    }

    ULOG_INFO("ExSrc_DataProcessTask: Waiting for events\n");
    do {
        status = BCM_WaitEvent(EXSRC_STOP_DATA_PROCESS_EVENT | SystemEvent8 | SystemEvent9);
        if (BCM_ERR_OK != status) {
            /* log error */
        }
        status = BCM_GetEvent(ExSrc_DataProcessTask, &mask);
        if (BCM_ERR_OK != status) {
            /* log error */
        }
        if ((mask & EXSRC_STOP_DATA_PROCESS_EVENT) == EXSRC_STOP_DATA_PROCESS_EVENT) {
            ULOG_INFO("ExSrc_DataProcessTask: Stop Cmd received\n");
            status = BCM_ClearEvent(EXSRC_STOP_DATA_PROCESS_EVENT);
            if (BCM_ERR_OK != status) {
                /* log error */
            }
            retVal = MSGQ_GetMsgIdx(&ExSrc_OutPortVideo.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == retVal) {
                MSYS_BufferType * mediaBuffer;
                mediaBuffer = &ExSrc_MediaBufferVideo[mediaBufferIdx];
                mediaBuffer->buffer[0].size = 0UL;
                mediaBuffer->response = BCM_ERR_OK;
                retVal = MSGQ_CompleteMsgIdx(&ExSrc_OutPortVideo.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("ExSrc_ProcessData: MSGQ_CompleteMsgIdx failed (%d)\n", retVal);
                }
            }
            loop = 0UL;
        }
        if ((mask & SystemEvent8) == SystemEvent8) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port event notification received\n");
            status = BCM_ClearEvent(SystemEvent8);
            if (BCM_ERR_OK != status) {
                /* log error */
            }
            ExSrc_ProcessData(&ExSrc_OutPortVideo);
        }
        if ((mask & SystemEvent9) == SystemEvent9) {
            ULOG_VERB("ExSrc_DataProcessTask: Out port event notification received\n");
            status = BCM_ClearEvent(SystemEvent9);
            if (BCM_ERR_OK != status) {
                /* log error */
            }
            ExSrc_ProcessData(&ExSrc_OutPortGfx);
        }
    } while(0UL != loop);

    status = BCM_SetEvent(ExSrc_CmdProcessTask, EXSRC_DATA_PROCESS_ACK_EVENT);
    if (BCM_ERR_OK != status) {
        /* log error */
    }
    status = BCM_TerminateTask();
    if (BCM_ERR_OK != status) {
        /* log error */
    }
}
