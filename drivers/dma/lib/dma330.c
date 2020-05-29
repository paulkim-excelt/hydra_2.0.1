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
#include <string.h>
#include <chip_config.h>
#include <bcm_utils.h>
#include <atomic.h>
#include <interrupt.h>
#include <bcm_err.h>
#include <dma_osil.h>
#include <cache.h>
#include <cortex.h>
#include "dma330.h"

#define DMAMOV_SAR_OPCODE       (0x00BCUL)
#define DMAMOV_DAR_OPCODE       (0x02BCUL)
#define DMAMOV_CCR_OPCODE       (0x01BCUL)


#define CHANNEL_MASK(idx)     (0x1UL << (idx))
#define CHANN_MASK_ALL      (CHANNEL_MASK(DMA_CHANNEL_ID_MAX) - 1)

typedef uint8_t DMAMOV_RegType;
#define DMAMOV_SAR      (0x00U)
#define DMAMOV_CCR      (0x01U)
#define DMAMOV_DAR      (0x02U)

typedef uint8_t DMALP_CntType;
#define LP_COUNTER_0    (0x0U)
#define LP_COUNTER_1    (0x1U)

typedef uint32_t DMA_CondType;
#define DMA_COND_SINGLE     (0x0U)
#define DMA_COND_BURST      (0x1U)
#define DMA_COND_ANY        (0x2U)

#define DMA_PERIP_ID_MASK   (0x1FU)
#define DMA_PERIP_ID_SHIFT  (3U)

#define DMA330_LP_COUNT_MAX     (256)
#define DMA330_NUM_LP           (2)

#define CCR_SRC_INC_MASK            (0x1UL)
#define CCR_SRC_BURST_SZ_MASK       (0x7UL << 1UL)
#define CCR_SRC_BURST_SZ_SHIFT      (1UL)
#define CCR_SRC_BURST_LEN_MASK      (0xFUL << 4UL)
#define CCR_SRC_BURST_LEN_SHIFT     (4UL)
#define CCR_SRC_PROT_CNTRL_MASK     (7UL << 8UL)
#define CCR_SRC_PROT_CNTRL_SHIFT    (8UL)
#define CCR_SRC_CACHE_CNTRL_MASK    (7UL << 11UL)
#define CCR_SRC_CACHE_CNTRL_SHIFT   (11UL)
#define CCR_DEST_INC_MASK           (0x1UL << 14UL)
#define CCR_DEST_BURST_SZ_MASK      (0x7UL << 15UL)
#define CCR_DEST_BURST_SZ_SHIFT     (15UL)
#define CCR_DEST_BURST_LEN_MASK     (0xFUL << 18UL)
#define CCR_DEST_BURST_LEN_SHIFT    (18UL)
#define CCR_DEST_PROT_CNTRL_MASK    (7UL << 22UL)
#define CCR_DEST_PROT_CNTRL_SHIFT   (22UL)
#define CCR_DEST_CACHE_CNTRL_MASK   (7UL << 25UL)
#define CCR_DEST_CACHE_CNTRL_SHIFT  (25UL)

typedef uint32_t DMA_ChannStateType;
#define DMA_CHANN_STATE_UNINIT      (0x0UL)
#define DMA_CHANN_STATE_IDLE        (0x1UL)
#define DMA_CHANN_STATE_SETUP       (0x2UL)
#define DMA_CHANN_STATE_BUSY        (0x3UL)
#define DMA_CHANN_STATE_ERR         (0x4UL)

typedef struct {
    volatile DMA_ChannStateType      state;
    volatile DMA_XferStatusType      xferStatus;
    uint32_t                codeSz;
    uint32_t                xferReqSz;
    uint32_t                trigIdx;
    uint32_t                error;
    uint32_t                intCnt;
    uint32_t                abortCnt;
    uint32_t                trigCnt;
} DMA_ChannRWType;

typedef struct {
    uint32_t                init;
    DMA_ChannRWType         channel[DMA_CHANNEL_ID_MAX + 1UL];
} DMA_RWDevType;


static DMA_RWDevType COMP_SECTION(".data.drivers") DMA_Dev = {
    .init = FALSE,
};

static DMA330_RegsType * const DMA330_REGS = (DMA330_RegsType *) DMA_BASE;

static uint8_t COMP_SECTION(".data.drivers") DMA330_FlushCode[32] COMP_ALIGN(32);
static void DMA330_Mov(volatile uint8_t *const aCode, uint32_t *aOffset,
                        DMAMOV_RegType aReg, uint32_t aVal)
{
#define DMAMOV          (0xBCU)     /**< DMA MOVE instruction */
    aCode[(*aOffset)++] = DMAMOV;
    aCode[(*aOffset)++] = aReg;
    aCode[(*aOffset)++] = (uint8_t)(aVal & 0xFFU);
    aCode[(*aOffset)++] = (uint8_t)(aVal >> 8U) & 0xFFU;
    aCode[(*aOffset)++] = (uint8_t)(aVal >> 16U) & 0xFFU;
    aCode[(*aOffset)++] = (uint8_t)(aVal >> 24U) & 0xFFU;
}

static void DMA330_SrcInc(volatile uint8_t *const aCode, uint32_t *aOffset,
                         int16_t aVal)
{
#define DMASRCINC      (0x54U)     /**< DMA SRC INC instruction */
#define DMASRCINC_NEG  (0x58U)     /**< DMA SRC INC instruction */
    if (aVal >=0) {
        aCode[(*aOffset)++] = DMASRCINC;
    } else {
        aCode[(*aOffset)++] = DMASRCINC_NEG;
    }
    aCode[(*aOffset)++] = (uint8_t)(aVal & 0xFFU);
    aCode[(*aOffset)++] = (uint8_t)(aVal >> 8U) & 0xFFU;
}

static void DMA330_DstInc(volatile uint8_t *const aCode, uint32_t *aOffset,
                         int16_t aVal)
{
#define DMADSTINC      (0x56U)     /**< DMA DST INC instruction */
#define DMADSTINC_NEG  (0x5EU)     /**< DMA DST INC instruction */
    if (aVal >=0) {
        aCode[(*aOffset)++] = DMADSTINC;
    } else {
        aCode[(*aOffset)++] = DMADSTINC_NEG;
    }
    aCode[(*aOffset)++] = (uint8_t)(aVal & 0xFFU);
    aCode[(*aOffset)++] = (uint8_t)(aVal >> 8U) & 0xFFU;
}

static void DMA330_CCR(volatile uint8_t *const aCode, uint32_t *aOffset,
                        const DMA_BurstSize aBurstSz,
                        const DMA_BurstLen aBurstLen,
                        const DMA_XferType aXferType)
{
    uint32_t ccr = 0UL;

    /* Source settings */
    ccr |= ((aBurstSz << CCR_SRC_BURST_SZ_SHIFT) & CCR_SRC_BURST_SZ_MASK);
    ccr |= ((aBurstLen << CCR_SRC_BURST_LEN_SHIFT) & CCR_SRC_BURST_LEN_MASK);
    ccr |= (0x2UL << CCR_SRC_PROT_CNTRL_SHIFT);

    /* Destination settings */
    ccr |= ((((uint32_t)aBurstSz) << CCR_DEST_BURST_SZ_SHIFT) &  CCR_DEST_BURST_SZ_MASK);
    ccr |= ((((uint32_t)aBurstLen) << CCR_DEST_BURST_LEN_SHIFT) & CCR_DEST_BURST_LEN_MASK);
    ccr |= (0x2UL << CCR_DEST_PROT_CNTRL_SHIFT);

    /* Cache settings */
    ccr |= (0x3UL << CCR_DEST_CACHE_CNTRL_SHIFT);

    switch (aXferType) {
    case DMA_XFER_TYPE_MEM_TO_MEM:
        ccr |= CCR_SRC_INC_MASK;
        ccr |= CCR_DEST_INC_MASK;
        break;
    case DMA_XFER_TYPE_MEM_TO_PERIPH:
        ccr |= CCR_SRC_INC_MASK;
        break;
    case DMA_XFER_TYPE_PERIPH_TO_MEM:
        ccr |= CCR_DEST_INC_MASK;
        break;
    case DMA_XFER_TYPE_PERIPH_TO_PERIPH:
        break;
    default:
        break;
    }
    DMA330_Mov(aCode, aOffset, DMAMOV_CCR, ccr);
}

static void DMA330_Load(volatile uint8_t *const aCode, uint32_t *aOffset)
{
#define DMALD           (0x04U)     /**< DMA LOAD instruction */
    aCode[(*aOffset)++] = DMALD;
}


static void DMA330_Store(volatile uint8_t *const aCode, uint32_t *aOffset)
{
#define DMAST           (0x08U)
    aCode[(*aOffset)++] = DMAST;
}

static void DMA330_LoadPeriph(volatile uint8_t *const aCode, uint32_t *aOffset,
                                DMA_CondType aCond,
                                DMA_PeriphIDType aPheripID)
{
#define DMALDP          (0x25U)
    switch (aCond) {
    case DMA_COND_SINGLE: /* bs = 0 */
        aCode[(*aOffset)++] = DMALDP;
        break;
    default: /* deafult is BURST, bs = 1 */
        aCode[(*aOffset)++] = DMALDP | (0x1U << 0x1U);
        break;
    }
    aCode[(*aOffset)++] = (aPheripID & DMA_PERIP_ID_MASK) << DMA_PERIP_ID_SHIFT;
}

static void DMA330_StorePeriph(volatile uint8_t *const aCode, uint32_t *aOffset,
                                DMA_CondType aCond,
                                DMA_PeriphIDType aPheripID)
{
#define DMASTP          (0x29U)
    switch (aCond) {
    case DMA_COND_SINGLE: /* bs = 0 */
        aCode[(*aOffset)++] = DMASTP;
        break;
    default: /* deafult is BURST, bs = 1 */
        aCode[(*aOffset)++] = DMASTP | (0x1U << 0x1U);
        break;
    }
    aCode[(*aOffset)++] = (aPheripID & DMA_PERIP_ID_MASK) << DMA_PERIP_ID_SHIFT;
}

static void DMA330_WaitForPeriph(volatile uint8_t *const aCode,  uint32_t *aOffset,
                                DMA_CondType aCond,
                                DMA_PeriphIDType aPheripID)
{
#define DMAWFP          (0x30U)     /**< DMA WAIT PERIPHERAL instruction */
    switch (aCond) {
    case DMA_COND_SINGLE: /* bs = 0, p = 0 */
        aCode[(*aOffset)++] = DMAWFP;
        break;
    case DMA_COND_BURST: /* bs = 1, p = 0 */
        aCode[(*aOffset)++] = DMAWFP | (0x1U << 0x1U);
        break;
    default: /* bs = 0, p = 1 */
        aCode[(*aOffset)++] = DMAWFP | (0x1UL << 0U);
        break;
    }
    aCode[(*aOffset)++] = (aPheripID & DMA_PERIP_ID_MASK) << DMA_PERIP_ID_SHIFT;
}

static void DMA330_FlushPeriph(volatile uint8_t *const aCode, uint32_t *aOffset,
        DMA_PeriphIDType aPheripID)
{
#define DMAFLUSHP       (0x35U)
    aCode[(*aOffset)++] = DMAFLUSHP;
    aCode[(*aOffset)++] = (aPheripID & DMA_PERIP_ID_MASK) << DMA_PERIP_ID_SHIFT;
}

static void DMA330_Loop(volatile uint8_t *const aCode, uint32_t *aOffset,
                        DMALP_CntType aLoopReg,
                        uint32_t aCounter)
{
#define DMALP           (0x20U)     /**< DMA LOOP COUTER instruction */
    aCode[(*aOffset)++] = DMALP | (aLoopReg << 1U);
    aCode[(*aOffset)++] = (uint8_t)aCounter;
}

static void DMA330_LoopEnd(volatile uint8_t *const aCode, uint32_t *aOffset,
                            DMALP_CntType aLoopReg,
                            uint32_t aBackJump)
{
#define DMALPEND        (0x28U)     /**< DMA LOOP END instruction */
    aCode[(*aOffset)++] = DMALPEND | (1U << 4U) | (aLoopReg << 2U);
    aCode[(*aOffset)++] = (uint8_t)aBackJump;
}

static void DMA330_Sev(volatile uint8_t *const aCode, uint32_t *aOffset,
                        uint8_t aEventNum)
{
#define DMASEV          (0x34U)
    aCode[(*aOffset)++] = DMASEV;
    aCode[(*aOffset)++] = (aEventNum & 0x1FU) << 3U;
}

static void DMA330_WriteBarrier(volatile uint8_t *const aCode, uint32_t *aOffset)
{
#define DMAWMB          (0x13U)     /**< DMA WRITE MEM BARRIER instruction */
    aCode[(*aOffset)++] = DMAWMB;
}

static void DMA330_End(volatile uint8_t *const aCode, uint32_t *aOffset)
{
#define DMAEND          (0x00U)
    aCode[(*aOffset)++] = DMAEND;
}
#define DMALDP          (0x25U)
#define DMAKILL         (0x01U)     /**< DMA KILL instruction */

static inline void DMA_SetChannState(DMA_ChannelIDType aID,
        DMA_ChannStateType state)
{
    (void)ATOMIC_Set(&DMA_Dev.channel[aID].state, state);
}

static inline DMA_XferStatusType DMA_GetChannState(DMA_ChannelIDType aID)
{
    return ATOMIC_Get(&DMA_Dev.channel[aID].state);
}

#ifdef DMA_PL330_DEBUG
static void DMA330_DumpInstrs(volatile const uint8_t *const addr, uint32_t size)
{
    uint32_t i;

    for (i = 0UL; i < size; i++) {
        KLOG_INFO("DMA UCODE : %02x\n", addr[i]);
    }
}
#else
#define DMA330_DumpInstrs(addr, size)
#endif

static uint32_t DMA_BurstSzToBytes(DMA_BurstSize aWidth)
{
    uint32_t bytes = 0;
    if (aWidth <= DMA_BURST_SZ_MAX) {
        bytes = 1UL << aWidth;
    }
    return bytes;
}

static uint32_t DMA330_IsPeripXfer(const DMA_ChannCfgType *aChann)
{
    uint32_t  isPerip = TRUE;
    if (DMA_XFER_TYPE_MEM_TO_MEM == aChann->xferType) {
        isPerip = FALSE;
    }
    return isPerip;
}

static void DMA330_Trigger(DMA_ChannelIDType aChann, volatile uint8_t *const aAddr,
        uint32_t aIntEn)
{
    INTR_FlagType mask = INTR_Suspend();
    CORTEX_DSB();
    DMA330_REGS->DBGINST0 = (aChann << 24UL) | (0xa0UL << 16UL) |
        (1UL << 17UL) | (aChann << 8UL);
    DMA330_REGS->DBGINST1 = (uint32_t)aAddr;
    DMA330_REGS->DBGCMD = 0UL;
    if (aIntEn == TRUE) {
        DMA330_REGS->INTEN |= CHANNEL_MASK(aChann);
    } else {
        DMA330_REGS->INTEN |= ~CHANNEL_MASK(aChann);
    }
    DMA_Dev.channel[aChann].trigCnt++;
    INTR_Resume(mask);
}

static void DMA330_Kill(DMA_ChannelIDType aChann)
{
    DMA330_REGS->INTEN &= ~CHANNEL_MASK(aChann);
    DMA330_REGS->DBGINST0 = (0x1UL << 16UL) | (aChann << 8UL) | (1UL);
    DMA330_REGS->DBGINST1 = 0UL;
    DMA330_REGS->DBGCMD = 0UL;
}

static void DMA330_ChannStateHandler(DMA_ChannelIDType aID, uint32_t aErr)
{
    DMA_XferStatusType xferStatus;
    DMA_ChannStateType state;
    const DMA_ChannCfgType *channCfg;

    state = DMA_GetChannState(aID);
    if (state == DMA_CHANN_STATE_BUSY) {
        channCfg = &DMA_CfgTbl.channCfg[aID];
        if (aErr != 0UL) {
            xferStatus = DMA_XFER_STATUS_ERROR;
        } else {
            xferStatus = DMA_XFER_STATUS_SUCCESS;
        }

        if (DMA330_IsPeripXfer(channCfg) == TRUE) {
            if (aErr != 0UL) {
                DMA_SetChannState(aID, DMA_CHANN_STATE_ERR);
            } else {
                DMA_SetChannState(aID, DMA_CHANN_STATE_SETUP);
            }
        } else {
            if (aErr != 0UL) {
                DMA_SetChannState(aID, DMA_CHANN_STATE_ERR);
            } else {
                DMA_SetChannState(aID, DMA_CHANN_STATE_IDLE);
            }
        }
        if (channCfg->xferDoneCb != NULL) {
            channCfg->xferDoneCb(aID, DMA_Dev.channel[aID].trigIdx,
                    xferStatus);
        }
    } else {
        /* Report error */
    }
}

int32_t DMA330_PeriphFlush(DMA_ChannelIDType aID)
{
#define DMA330_FLUSHP_TIMEOUT   (20UL)
    DMA_ChannStateType state;
    const DMA_ChannCfgType *chann;
    INTR_FlagType flag;
    uint32_t channState;
    uint32_t count = 0UL;
    uint32_t offset = 0UL;
    int32_t ret = BCM_ERR_OK;

    if (DMA_Dev.init == FALSE) {
        ret = BCM_ERR_UNINIT;
        goto err_exit;
    }

    if (aID > DMA_CHANNEL_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }
    state = DMA_GetChannState(aID);

    /* Is this channel configured by system? */
    if (state == DMA_CHANN_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    /* flush now allowed while channel is busy */
    if ((state == DMA_CHANN_STATE_BUSY) ||
            (state == DMA_CHANN_STATE_ERR)) {
        ret = BCM_ERR_INVAL_STATE;
        goto err_exit;
    }

    chann = &DMA_CfgTbl.channCfg[aID];

    /* is DMA channel busy? */
    channState = DMA330_REGS->CSR_CPC[aID * 2UL] & CSR_CHANNEL_STATUS_MASK;
    if (channState == CHANNEL_STATE_STOPPED) {
        flag = INTR_Suspend();
        DMA330_FlushPeriph(DMA330_FlushCode, &offset, chann->periphID);
        DMA330_End(DMA330_FlushCode, &offset);
        DCACHE_SysCleanRange((uint32_t)DMA330_FlushCode, sizeof(DMA330_FlushCode));
        DMA330_Trigger(aID, DMA330_FlushCode, FALSE);
        /* wait for the DMA to complete */
        do {
            channState = DMA330_REGS->CSR_CPC[aID * 2UL] & CSR_CHANNEL_STATUS_MASK;
            count++;
        } while ((count < DMA330_FLUSHP_TIMEOUT) &&
                (channState != CHANNEL_STATE_STOPPED));
        if (count == DMA330_FLUSHP_TIMEOUT) {
            ret = BCM_ERR_TIME_OUT;
        }
        INTR_Resume(flag);
    }

err_exit:
    return ret;
}

static int32_t DMA330_XferSetupInternal(DMA_ChannelIDType aID,
                                        const DMA_ChannCfgType *const aChann,
                                        DMA_XferReqType *const aXferReq,
                                        uint32_t aReqSZ)
{
    uint32_t i,j;
    uint32_t srcWid;
    uint32_t destWid;
    uint32_t offset;
    uint32_t loopCnt;
    uint32_t loop_start0;
    uint32_t loop_start1;
    volatile uint8_t *addrBase;
    int32_t ret = BCM_ERR_OK;

    addrBase = aChann->codeAddr;
    DMA_Dev.channel[aID].codeSz = 0UL;
    offset = 0UL;

    for (i=0; i < aReqSZ; i++) {
        srcWid = DMA_BurstSzToBytes(aChann->burstSz);
        destWid = DMA_BurstSzToBytes(aChann->burstSz);
        if ((srcWid == 0UL) || (destWid == 0UL)) {
            ret = BCM_ERR_INVAL_STATE;
            goto err_exit;
        }
        if (offset > aChann->codeSize) {
            ret = BCM_ERR_NOMEM;
            goto err_exit;
        }

        DMA330_Mov(addrBase, &offset, DMAMOV_SAR, aXferReq[i].srcAddr);
        DMA330_Mov(addrBase, &offset, DMAMOV_DAR, aXferReq[i].destAddr);
        DMA330_CCR(addrBase, &offset, 
                    aChann->burstSz, aChann->burstLen, aChann->xferType);
        for (j=0UL; j < aXferReq[i].nRows; j++) {
            loopCnt = (aXferReq[i].rowSize) / (srcWid * (aChann->burstLen + 1UL));
            if (loopCnt > 0xFFFFUL) {
                ret = BCM_ERR_NOMEM;
                goto err_exit;
            }

            if (0UL != (loopCnt & 0xFF00UL)) {
                DMA330_Loop(addrBase, &offset, LP_COUNTER_1, (loopCnt >> 0x8UL) -1U); /* DMA330 increments the counter by 1 */
                loop_start1 = offset;
                DMA330_Loop(addrBase, &offset, LP_COUNTER_0, 255U); /* DMA330 increments the counter by 1 */
                loop_start0 = offset;
                switch (aChann->xferType) {
                case DMA_XFER_TYPE_MEM_TO_PERIPH:
                    DMA330_Load(addrBase, &offset);
                    DMA330_WaitForPeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    DMA330_StorePeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    break;
                case DMA_XFER_TYPE_PERIPH_TO_MEM:
                    DMA330_WaitForPeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    DMA330_LoadPeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    DMA330_Store(addrBase, &offset);
                    break;
                case DMA_XFER_TYPE_MEM_TO_MEM:
                    DMA330_Load(addrBase, &offset);
                    DMA330_Store(addrBase, &offset);
                    break;
                default:
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                DMA330_LoopEnd(addrBase, &offset, LP_COUNTER_0, offset - loop_start0);
                DMA330_LoopEnd(addrBase, &offset, LP_COUNTER_1, offset - loop_start1);
                loopCnt &= 0xFFUL;
            }
            if (0UL != loopCnt) {
                DMA330_Loop(addrBase, &offset, LP_COUNTER_0, loopCnt - 1U);/* DMA330 increments the counter by 1 */
                loop_start0 = offset;
                switch (aChann->xferType) {
                case DMA_XFER_TYPE_MEM_TO_PERIPH:
                    DMA330_Load(addrBase, &offset);
                    DMA330_WaitForPeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    DMA330_StorePeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    break;
                case DMA_XFER_TYPE_PERIPH_TO_MEM:
                    DMA330_WaitForPeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    DMA330_LoadPeriph(addrBase, &offset, DMA_COND_BURST, aChann->periphID);
                    DMA330_Store(addrBase, &offset);
                    break;
                case DMA_XFER_TYPE_MEM_TO_MEM:
                    DMA330_Load(addrBase, &offset);
                    DMA330_Store(addrBase, &offset);
                    break;
                default:
                    ret = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                DMA330_LoopEnd(addrBase, &offset, LP_COUNTER_0, offset - loop_start0);
            }
            switch (aChann->xferType) {
            case DMA_XFER_TYPE_MEM_TO_PERIPH:
                DMA330_SrcInc(addrBase, &offset, aXferReq[i].srcSkip);
                break;
            case DMA_XFER_TYPE_PERIPH_TO_MEM:
                DMA330_DstInc(addrBase, &offset, aXferReq[i].dstSkip);
                break;
            case DMA_XFER_TYPE_MEM_TO_MEM:
                DMA330_SrcInc(addrBase, &offset, aXferReq[i].srcSkip);
                DMA330_DstInc(addrBase, &offset, aXferReq[i].dstSkip);
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
        }
        if (TRUE == DMA330_IsPeripXfer(aChann)) {
            DMA330_Sev(addrBase, &offset, (uint8_t)aID);
            DMA330_End(addrBase, &offset);
        }
        if (DMA_Dev.channel[aID].codeSz == 0UL) {
            DMA_Dev.channel[aID].codeSz = offset;
        }
    }
    if (FALSE == DMA330_IsPeripXfer(aChann)) {
        DMA330_WriteBarrier(addrBase, &offset);
        DMA330_Sev(addrBase, &offset, (uint8_t)aID);
        DMA330_End(addrBase, &offset);
    }

    DMA330_DumpInstrs(addrBase, offset);

err_exit:
    return ret;
}

int32_t DMA330_XferTrigger(DMA_ChannelIDType aID, uint32_t aReqIdx)
{
    DMA_ChannStateType state;
    const DMA_ChannCfgType *chann;
    volatile uint8_t *addrBase;
    int32_t ret = BCM_ERR_OK;

    if (DMA_Dev.init == FALSE) {
        ret = BCM_ERR_UNINIT;
        goto err_exit;
    }

    if (aID > DMA_CHANNEL_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }
    state = DMA_GetChannState(aID);

    /* Is this channel configured by system? */
    if (state == DMA_CHANN_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    /**
     * is request setup by DMA?
     */
    if (state != DMA_CHANN_STATE_SETUP) {
        ret = BCM_ERR_INVAL_STATE;
        goto err_exit;
    }

    chann = &DMA_CfgTbl.channCfg[aID];
    /* aReqIdx withing the setup region? */
    if (aReqIdx >=  DMA_Dev.channel[aID].xferReqSz) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    /* set channel state busy and trigger the DMA*/
    DMA_SetChannState(aID, DMA_CHANN_STATE_BUSY);
    DMA_Dev.channel[aID].trigIdx = aReqIdx;
    addrBase = chann->codeAddr + (aReqIdx * DMA_Dev.channel[aID].codeSz);
    DMA330_Trigger(aID, addrBase, TRUE);

err_exit:
    return ret;
}

int32_t DMA330_XferSetup(DMA_ChannelIDType aID, DMA_XferReqType aXferReq[],
        uint32_t aReqSZ)
{
    DMA_ChannStateType state;
    const DMA_ChannCfgType *chann;
    uint32_t peripXfer;
    int32_t ret = BCM_ERR_OK;

    if (DMA_Dev.init == FALSE) {
        ret = BCM_ERR_UNINIT;
        goto err;
    }

    if ((aID > DMA_CHANNEL_ID_MAX) || (aXferReq == NULL) ||
            (aReqSZ == 0UL)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    state = DMA_GetChannState(aID);

    /* Is this channel configured by system? */
    if (state == DMA_CHANN_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    chann = &DMA_CfgTbl.channCfg[aID];
    peripXfer = DMA330_IsPeripXfer(chann);

    if (peripXfer == FALSE) {
        if ((state != DMA_CHANN_STATE_IDLE)
            && (state != DMA_CHANN_STATE_ERR)
            && (state != DMA_CHANN_STATE_SETUP)){
            ret = BCM_ERR_INVAL_STATE;
            goto err;
        }
    } else {
        if ((state == DMA_CHANN_STATE_BUSY) ||
                (state == DMA_CHANN_STATE_ERR)) {
            ret = BCM_ERR_INVAL_STATE;
            goto err;
        }
    }

    /**
     * set channel state busy while we are setting
     * up the descriptor and microcode
     */

    switch(chann->xferType) {
    case DMA_XFER_TYPE_MEM_TO_MEM:
    case DMA_XFER_TYPE_MEM_TO_PERIPH:
    case DMA_XFER_TYPE_PERIPH_TO_MEM:
        ret = DMA330_XferSetupInternal(aID, chann, aXferReq, aReqSZ);
        break;
    case DMA_XFER_TYPE_PERIPH_TO_PERIPH:
        ret = BCM_ERR_NOSUPPORT;
        break;
    default:
        ret = BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (ret == BCM_ERR_OK) {
        DMA_Dev.channel[aID].xferReqSz = aReqSZ;
        DMA_SetChannState(aID, DMA_CHANN_STATE_SETUP);
    } else {
        DMA_SetChannState(aID, DMA_CHANN_STATE_IDLE);
    }

err:
    return ret;
}

int32_t DMA330_Init(void)
{
    uint32_t i;
    int32_t err = BCM_ERR_OK;
    const DMA_ConfigType * config = &DMA_CfgTbl;

    /**
     * Verify the configuration parameters and if
     * its valid, set the DMA driver state to Initialized
     */

    if (DMA_Dev.init == FALSE) {
        if (config->channCfgSz <= (DMA_CHANNEL_ID_MAX + 1UL)) {
            for (i = 0UL; i < config->channCfgSz; i++) {
                if (config->channCfg[i].xferType > DMA_XFER_TYPE_MAX) {
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                if (config->channCfg[i].burstSz > DMA_BURST_SZ_MAX) {
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                if ((TRUE == DMA330_IsPeripXfer(&config->channCfg[i])) &&
                        (config->channCfg[i].xferDoneCb == NULL)) {
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                if ((config->channCfg[i].codeAddr == NULL) || (config->channCfg[i].codeSize == 0UL)) {
                    err = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                /**
                 * channel initialized and idle now
                 */
                DMA_SetChannState(i, DMA_CHANN_STATE_IDLE);
            }
            if (err == BCM_ERR_OK) {
                DMA_Dev.init = TRUE;
            }
        }
    }
    return err;
}

int32_t DMA330_GetTransferStatus(DMA_ChannelIDType aID,
                                 DMA_XferStatusType *const aStatus)
{
    DMA_ChannStateType state;
    int ret = BCM_ERR_OK;

    if (DMA_Dev.init != FALSE) {
        if ((aID < DMA_CHANNEL_ID_MAX)
            && (aStatus != NULL)) {
            state = DMA_GetChannState(aID);
            switch (state) {
            case DMA_CHANN_STATE_UNINIT:
                ret = BCM_ERR_INVAL_STATE;
                *aStatus = DMA_XFER_STATUS_SUCCESS;
                break;
            case DMA_CHANN_STATE_IDLE:
                *aStatus = DMA_XFER_STATUS_SUCCESS;
                break;
            case DMA_CHANN_STATE_SETUP:
                *aStatus = DMA_XFER_STATUS_SUCCESS;
                break;
            case DMA_CHANN_STATE_BUSY:
                *aStatus = DMA_XFER_STATUS_BUSY;
                break;
            case DMA_CHANN_STATE_ERR:
                *aStatus = DMA_XFER_STATUS_ERROR;
                break;
            default:
                ret = BCM_ERR_INVAL_STATE;
                break;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_UNINIT;
    }
    return ret;
}

uint32_t DMADrv_ChannIRQHandler(DMA_ChannelIDType aID)
{
    uint32_t signalClient = FALSE;
    uint32_t intStatus;
    const DMA_ChannCfgType *channCfg;

    intStatus = DMA330_REGS->INTMIS;
    if (aID < DMA_CHANNEL_ID_MAX) {
        if ((intStatus & CHANNEL_MASK(aID)) != 0UL) {
            DMA330_REGS->INTCLR = CHANNEL_MASK(aID);
            channCfg = &DMA_CfgTbl.channCfg[aID];
            DMA_Dev.channel[aID].intCnt++;
            DMA_Dev.channel[aID].error = 0UL;
            DMA330_REGS->INTEN &= ~CHANNEL_MASK(aID);
            DMA330_ChannStateHandler(aID , DMA_Dev.channel[aID].error);
            if (DMA330_IsPeripXfer(channCfg) == FALSE) {
                signalClient = TRUE;
            }
        }
    } else {
        /* report error */
    }
    return signalClient;
}

uint32_t DMADrv_AbortIRQHandler(void)
{
    uint32_t i;
    uint32_t faultStatus;
    faultStatus = DMA330_REGS->FSRC;

    for (i = 0UL; i < DMA_CHANNEL_ID_MAX; i++) {
        if ((faultStatus & CHANNEL_MASK(i)) != 0UL) {
            DMA_Dev.channel[i].abortCnt++;
            DMA_Dev.channel[i].error = 1UL;
            DMA330_Kill(i);
            DMA330_REGS->INTEN &= ~CHANNEL_MASK(i);
            DMA330_ChannStateHandler(i, DMA_Dev.channel[i].error);
        }
    }
    return faultStatus;
}
