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
#include <compiler.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <dma.h>
#include <dma_osil.h>
#include <bcm_test.h>
#include "ee.h"
#include <string.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_INFO
#define TEST_BUF_SIZE           (255*1024)

static int32_t DMAIT1_Result = BCM_AT_NOT_STARTED;

static uint8_t COMP_SECTION(".pp.uncached.test") SrcBuf[TEST_BUF_SIZE];
static uint8_t COMP_SECTION(".pp.uncached.test") DestBuf[TEST_BUF_SIZE];
static uint8_t COMP_SECTION(".dma.fw") DMA_CodeRegion[512UL] COMP_ALIGN(64);

const DMA_ChannCfgType DMA_ChannCfgTbl [] = {
    {
        .compID = BCM_DMA_ID,
        .instanceID = 0UL,
        .xferType = DMA_XFER_TYPE_MEM_TO_MEM,
        .burstSz = DMA_BURST_SZ_8,
        .burstLen = 15UL,
        .codeAddr = DMA_CodeRegion,
        .codeSize = 512UL,
    },
};

const DMA_ConfigType DMA_CfgTbl = {
    .channCfg = DMA_ChannCfgTbl,
    .channCfgSz = sizeof(DMA_ChannCfgTbl) / sizeof(DMA_ChannCfgType),
};

TASK(TaskDMATest)
{
    uint32_t i;
    int32_t ret;
    DMA_XferStatusType  status;
    DMA_XferReqType req[4];
    ULOG_INFO("DMA TEST: START\n");

    BCM_MemSet(DestBuf, 0x0, TEST_BUF_SIZE);

    for (i = 0UL; i < 4; i++) {
        BCM_MemSet(SrcBuf + i * (TEST_BUF_SIZE/4), 0xFF & ~(0x1 << i), TEST_BUF_SIZE/4);

        req[i].srcAddr = (uint32_t)(SrcBuf + i * (TEST_BUF_SIZE/4));
        req[i].destAddr = (uint32_t)(DestBuf + i * (TEST_BUF_SIZE/4));
        req[i].rowSize = TEST_BUF_SIZE/4;
        req[i].nRows = 1UL;
        req[i].srcSkip = 0;
        req[i].dstSkip = 0;
    }

    ULOG_INFO("DMA TEST: SEQ\n");
    ret = DMA_Mem2MemXfer(DMA_CHANNEL_ID_0, &req[0], 4UL);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("DMA_XferRequest failed (%d)!!\n",ret);
        goto err_exit;
    }

    BCM_WaitEvent(DMAEvent);
    BCM_ClearEvent(DMAEvent);

    ret = DMA_GetXferStatus(DMA_CHANNEL_ID_0, &status);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("DMA_XferRequest failed!! %d\n", ret);
        DMAIT1_Result = ret;
    } else if (status == DMA_XFER_STATUS_ERROR){
        ULOG_ERR("DMA Tranfer error..\n");
        DMAIT1_Result = BCM_ERR_UNKNOWN;
    }
err_exit:
    if (BCM_AT_EXECUTING == DMAIT1_Result) {
        if (0UL == memcmp(SrcBuf, DestBuf, TEST_BUF_SIZE)) {
            DMAIT1_Result = BCM_ERR_OK;
            ULOG_INFO("DMA Tranfer success..\n");
        } else {
            DMAIT1_Result = BCM_ERR_UNKNOWN;
            ULOG_ERR("DMA Tranfer failed..\n");
        }
    }
    BCM_TerminateTask();
}

void ApplicationInit()
{
    DMA_Init();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex) {
    case 1UL:
        if (BCM_AT_EXECUTING != DMAIT1_Result) {
            DMAIT1_Result = BCM_AT_EXECUTING;
            BCM_ActivateTask(TaskDMATest);
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;

    switch(aIndex)
    {
    case 1UL:
        retVal  = DMAIT1_Result;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

