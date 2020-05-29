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
#include <stdint.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <dma.h>
#include <dma_osil.h>
#include <osil/bcm_osil.h>
#include "ee.h"

static TaskType TaskID[DMA_CHANNEL_ID_MAX + 1UL] = {
    INVALID_TASK, INVALID_TASK, INVALID_TASK, INVALID_TASK,
    INVALID_TASK, INVALID_TASK, INVALID_TASK, INVALID_TASK};

static void DMA_CH_IRQHandler(DMA_ChannelIDType aChann)
{
    uint32_t signalClient = DMADrv_ChannIRQHandler(aChann);

    if ((signalClient == TRUE) && (TaskID[aChann] != INVALID_TASK)) {
        (void)SetEvent(TaskID[aChann], DMAEvent);
    }
}

ISR2(DMA_CH0_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_0);
}

ISR2(DMA_CH1_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_1);
}

ISR2(DMA_CH2_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_2);
}

ISR2(DMA_CH3_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_3);
}

ISR2(DMA_CH4_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_4);
}

ISR2(DMA_CH5_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_5);
}

ISR2(DMA_CH6_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_6);
}

ISR2(DMA_CH7_IRQHandler)
{
    DMA_CH_IRQHandler(DMA_CHANNEL_ID_7);
}

ISR2(DMA_Abort_IRQHandler)
{
    uint32_t error = 0;
    uint32_t i;
    error = DMADrv_AbortIRQHandler();
    for (i = 0UL; i < DMA_CHANNEL_ID_MAX; i++) {
        if (0UL != (error & (1UL << i))) {
            (void)SetEvent(TaskID[i], DMAEvent);
        }
    }
}

int32_t DMA_SysCmdReq(DMA_CmdType aCmd, DMAIO *const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (aIO != NULL) {
        sysReqIO.sysReqID = SVC_DMA_ID;
        sysReqIO.magicID = SVC_MAGIC_DMA_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t *)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        if (aCmd == DMA_CMD_XFER_REQ) {
            (void) GetTaskID(&TaskID[aIO->channelID]);//TODO: Use system config
        }
        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK == retVal) {
            if (BCM_ERR_OK == sysReqIO.response) {
                retVal = aIO->retVal;
            } else {
                retVal = sysReqIO.response;
            }
        }
    }
    return retVal;
}
/**
    @brief Union to avoid MISRA Required error
    for Type conversion
*/
typedef union _DMA_SVCIOType {
    uint8_t *data;
    DMAIO *io;
} DMA_SVCIOType;

void DMA_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    DMA_SVCIOType dma;
    dma.data = aSysIO;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_DMA_ID) {
            dma.io->retVal = DMA_CmdHandler(aCmd, dma.io);
        } else {
            dma.io->retVal = BCM_ERR_INVAL_MAGIC;
        }
    }
}


