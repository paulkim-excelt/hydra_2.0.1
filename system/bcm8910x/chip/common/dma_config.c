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

#include <stdint.h>
#include <bcm_utils.h>
#include <dma_osil.h>
#include <chip_config.h>
#include <syscfg.h>
#ifdef ENABLE_I2S
#include <audio_osil.h>
#endif
#if !defined(ENABLE_DMA_TEST)
#define DMA_CODE_REG_SZ                     (512UL)             /**< 512 bytes for microcode */

#if defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S2_PLAY)
static uint8_t COMP_SECTION(".dma.fw") DMA_I2STxCode[DMA_CODE_REG_SZ] COMP_ALIGN(64);
#endif
#if defined(ENABLE_I2S0_REC) || defined(ENABLE_I2S1_REC) || defined(ENABLE_I2S2_REC)
static uint8_t COMP_SECTION(".dma.fw") DMA_I2SRxCode[DMA_CODE_REG_SZ] COMP_ALIGN(64);
#endif

static const DMA_ChannCfgType DMA_ChannCfgTbl[] = {
#ifdef ENABLE_I2S0_PLAY
    {
        .compID = BCM_IIS_ID,
        .instanceID = 0UL,
        .periphID = DMA_PERIPH_I2S0_TX,
        .xferType = DMA_XFER_TYPE_MEM_TO_PERIPH,
        .burstSz = DMA_BURST_SZ_4,  /* 4-bytes per beat (32-bit fifo register) */
        .burstLen = 7UL,              /* 8 beats per burst (half FIFO threshold) */
        .xferDoneCb = I2S0_DmaTxDoneCb,
        .codeAddr = DMA_I2STxCode,
        .codeSize = DMA_CODE_REG_SZ,
    },
#endif
#ifdef ENABLE_I2S0_REC
    {
        .compID = BCM_IIS_ID,
        .instanceID = 0UL,
        .periphID = DMA_PERIPH_I2S0_RX,
        .xferType = DMA_XFER_TYPE_PERIPH_TO_MEM,
        .burstSz = DMA_BURST_SZ_4,  /* 4-bytes per beat (32-bit fifo register) */
        .burstLen = 7UL,              /* 8 beats per burst (half FIFO threshold) */
        .xferDoneCb = I2S0_DmaRxDoneCb,
        .codeAddr = DMA_I2SRxCode,
        .codeSize = DMA_CODE_REG_SZ,
    },
#endif
    /* I2S 1 play is used for integration test */
#ifdef ENABLE_I2S1_PLAY
    {
        .compID = BCM_IIS_ID,
        .instanceID = 1UL,
        .periphID = DMA_PERIPH_I2S1_TX,
        .xferType = DMA_XFER_TYPE_MEM_TO_PERIPH,
        .burstSz = DMA_BURST_SZ_4,  /* 4-bytes per beat (32-bit fifo register) */
        .burstLen = 7UL,              /* 8 beats per burst (half FIFO threshold) */
        .xferDoneCb = I2S1_DmaTxDoneCb,
        .codeAddr = DMA_I2STxCode,
        .codeSize = DMA_CODE_REG_SZ,
    },
#endif
    /* I2S 1 record is used for integration test */
#ifdef ENABLE_I2S1_REC
    {
        .compID = BCM_IIS_ID,
        .instanceID = 1UL,
        .periphID = DMA_PERIPH_I2S1_RX,
        .xferType = DMA_XFER_TYPE_PERIPH_TO_MEM,
        .burstSz = DMA_BURST_SZ_4,  /* 4-bytes per beat (32-bit fifo register) */
        .burstLen = 7UL,              /* 8 beats per burst (half FIFO threshold) */
        .xferDoneCb = I2S1_DmaRxDoneCb,
        .codeAddr = DMA_I2SRxCode,
        .codeSize = DMA_CODE_REG_SZ,
    },
#endif
#ifdef ENABLE_I2S2_REC
    {
        .compID = BCM_IIS_ID,
        .instanceID = 2UL,
        .periphID = DMA_PERIPH_I2S2_RX,
        .xferType = DMA_XFER_TYPE_PERIPH_TO_MEM,
        .burstSz = DMA_BURST_SZ_4,  /* 4-bytes per beat (32-bit fifo register) */
        .burstLen = 7UL,              /* 8 beats per burst (half FIFO threshold) */
        .xferDoneCb = I2S2_DmaRxDoneCb,
        .codeAddr = DMA_I2SRxCode,
        .codeSize = DMA_CODE_REG_SZ,
    },
#endif
};

const DMA_ConfigType DMA_CfgTbl = {
    .channCfg = DMA_ChannCfgTbl,
    .channCfgSz = sizeof(DMA_ChannCfgTbl) / sizeof(DMA_ChannCfgType),
};

#endif

uint32_t SysCfg_GetDMAChannel(uint64_t aCompID, uint32_t aInstanceID,
                        DMA_XferType aXferType)
{
    uint32_t chan = 0xFFFFFFFFUL;
    uint32_t i;
    for (i = 0UL; i < DMA_CfgTbl.channCfgSz; i++) {
        if ((aCompID == DMA_CfgTbl.channCfg[i].compID)
            && (aInstanceID == DMA_CfgTbl.channCfg[i].instanceID)
            && (aXferType == DMA_CfgTbl.channCfg[i].xferType)) {
                chan = i;
                break;
            }
    }
    return chan;
}
