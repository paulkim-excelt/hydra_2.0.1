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

#include <string.h>
#include <chip_config.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <spi.h>
#include <spi_osil.h>
#include <interrupt.h>
#include "pl022.h"
#include "spi_drv.h"

#define SPI_FIFO_SIZE               (8UL)
#define SPI_TIMEOUT_VAL_MAX         (100000UL)

#define SPI_XFER_STATE_IDLE         (0UL)
#define SPI_XFER_STATE_BUSY         (1UL)

#define XFER_PKT_XFER_ID_MASK       (0xFFFFUL)
#define XFER_PKT_XFER_ID_SHIFT      (0UL)
#define XFER_PKT_SEQ_ID_MASK        (0xFF0000UL)
#define XFER_PKT_SEQ_ID_SHIFT       (16UL)
#define XFER_PKT_PRIORITY_MASK      (0x3000000UL)
#define XFER_PKT_PRIORITY_SHIFT     (24UL)
#define XFER_PKT_LAST_IND_MASK      (0x4000000UL)
#define XFER_PKT_LAST_IND_SHIFT     (26UL)

#define BITRATE_CPSDVSR_CTRL_MIN    (196850UL)

typedef struct {
    SPI_StateType state;
    uint32_t xferState;
    uint32_t xferPktQ[SPI_XFER_PKTS_MAX]; /* [0:15] - Xfer Pkt ID,
                                             [16:23] - Seq ID,
                                             [24:25] - Priority
                                             [26] - flag that tells whether its a last packet */
    SPI_SeqStatusType seqStatus[SPI_SEQ_MAX];
    SPI_XferPktStatusType xferPktStatus[SPI_XFER_PKTS_MAX]; /* Listed in index order */
    const SPI_SeqType *currentSeq;
    const SPI_XferPktType *currentXferPkt;
    SPI_DataPktType *currentDataPkt;
    uint32_t txIdx;
    uint32_t rxIdx;
    uint32_t currentDataPktIdx;
    uint32_t xferPktCnt;
    uint32_t dataPktCnt;
    uint32_t bufSize;
} SPIDrv_DevType;

typedef struct {
    SPI_FRMFormatType frameFormat;
    uint32_t bitRate;
    uint32_t state;
#define PL022_STATE_UNINIT          (SPI_STATE_UNINIT)
#define PL022_STATE_IDLE            (SPI_STATE_IDLE)
#define PL022_STATE_BUSY            (SPI_STATE_BUSY)
} PL022_ConfigType;

static PL022_RegsType *const PL022_REGS[SPI_MAX_HW_ID] = {
#if (SPI_MAX_HW_ID == 0)
#error "SPI_MAX_HW_ID = 0"
#endif /* SPI_MAX_HW_ID == 0 */
    (PL022_RegsType *)SPI0_BASE,
#if (SPI_MAX_HW_ID > 1)
    (PL022_RegsType *)SPI1_BASE,
#endif /* SPI_MAX_HW_ID > 1 */
#if (SPI_MAX_HW_ID > 2)
    (PL022_RegsType *)SPI2_BASE,
#endif /* SPI_MAX_HW_ID > 2 */
#if (SPI_MAX_HW_ID > 3)
#error "SPI_MAX_HW_ID > 3"
#endif /* SPI_MAX_HW_ID > 3 */
};

static SPIDrv_DevType COMP_SECTION(".data.drivers") SPIDrv_Dev = {
    .state = SPI_STATE_UNINIT,
};

static PL022_ConfigType COMP_SECTION(".data.drivers")
    PL022_Config[SPI_MAX_HW_ID] = {
#if (SPI_MAX_HW_ID == 0)
#error "SPI_MAX_HW_ID = 0"
#endif /* SPI_MAX_HW_ID == 0 */
    {
        .state = PL022_STATE_UNINIT,
    },
#if (SPI_MAX_HW_ID > 1)
    {
        .state = PL022_STATE_UNINIT,
    },
#endif /* SPI_MAX_HW_ID > 1 */
#if (SPI_MAX_HW_ID > 2)
    {
        .state = PL022_STATE_UNINIT,
    },
#endif /* SPI_MAX_HW_ID > 2 */
#if (SPI_MAX_HW_ID > 3)
#error "SPI_MAX_HW_ID > 3"
#endif  /* SPI_MAX_HW_ID > 3 */
};

static void PL022_WaitTxFifoEmpty(SPI_HwIDType aHwID)
{
    uint32_t timeout = SPI_TIMEOUT_VAL_MAX;

    while ((SSPSR_TFE_MASK != (PL022_REGS[aHwID]->SSPSR & SSPSR_TFE_MASK))
            && (0UL != timeout)) {
        timeout--;
    }
}

static void PL022_Configure(SPI_HwIDType aHwID,
                            PL022_ConfigType *const aConfig)
{
    uint32_t ratio;
    uint32_t scr, cpsdvr;
    uint32_t frameFormat = SSPCR0_FRF_MOTOROLA;

    /* Mask the interrupts */
    PL022_REGS[aHwID]->SSPIMSC &= ~(SSPIMSC_TXIM_MASK | SSPIMSC_RXIM_MASK
        | SSPIMSC_RTIM_MASK | SSPIMSC_RORIM_MASK);
    switch (aConfig->frameFormat) {
    case SPI_FRM_FORMAT_MOTOROLA:
        frameFormat = SSPCR0_FRF_MOTOROLA;
        break;
    case SPI_FRM_FORMAT_TI:
        frameFormat = SSPCR0_FRF_TI;
        break;
    case SPI_FRM_FORMAT_NATIONAL:
        frameFormat = SSPCR0_FRF_NATIONAL;
        break;
    default:
        /* Never reach here */
        break;
    }
    ratio = (SPI_CLOCK / (aConfig->bitRate));
    if (BITRATE_CPSDVSR_CTRL_MIN <= aConfig->bitRate) {
        scr = 0UL;
        cpsdvr = ratio;
    } else {
        cpsdvr = 254UL;
        scr = (ratio / cpsdvr) - 1UL;
        if (0UL != (ratio % cpsdvr)) {
            scr = scr + 1UL;
        }
    }
    PL022_REGS[aHwID]->SSPCR0 = (((scr << SSPCR0_SCR_SHIFT) & SSPCR0_SCR_MASK)
                                 | SSPCR0_SPH_MASK | SSPCR0_SPO_MASK
                                 | ((frameFormat << SSPCR0_FRF_SHIFT) & SSPCR0_FRF_MASK));
    PL022_REGS[aHwID]->SSPCPSR = cpsdvr;
}

static int32_t PL022_Xfer(SPI_HwIDType aHwID,
                          SPI_DataPktType *const aDataPkt,
                          uint32_t *const aSize,
                          uint32_t aIsAsyncXfer)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i = 0UL;
    uint32_t accessWidth;
    uint32_t size = *aSize;
    uint16_t *srcBuf16;
    uint32_t txData;
    uint16_t rxData;
    uint32_t rxIdx;

    if (9UL > aDataPkt->dataWidth) {
        accessWidth = 8UL;
    } else {
        accessWidth = 16UL;
    }
    /* Read from the RX FIFO */
    if (NULL != aDataPkt->destBuf) {
        rxIdx = SPIDrv_Dev.rxIdx * 2UL;
        while (((PL022_REGS[aHwID]->SSPSR & SSPSR_RNE_MASK) == SSPSR_RNE_MASK)
            && (SPIDrv_Dev.rxIdx < aDataPkt->xferSize)) {
            if (8UL == accessWidth) {
                aDataPkt->destBuf[SPIDrv_Dev.rxIdx] = (uint8_t)PL022_REGS[aHwID]->SSPDR;
            } else {
                rxData = (uint16_t)PL022_REGS[aHwID]->SSPDR;
                aDataPkt->destBuf[rxIdx] = (uint8_t)(rxData & 0xFFU);
                aDataPkt->destBuf[rxIdx+1UL] = (uint8_t)((rxData & 0xFF00U) >> 8U);
                rxIdx += 2UL;
            }
            SPIDrv_Dev.rxIdx++;
        }
    }
    /* Fill the TX FIFO */
    if (8UL == accessWidth) {
        while (((PL022_REGS[aHwID]->SSPSR & SSPSR_TNF_MASK) == SSPSR_TNF_MASK)
                && (size != 0UL)) {
            if (NULL != aDataPkt->srcBuf) {
                txData = (uint32_t)aDataPkt->srcBuf[SPIDrv_Dev.txIdx];
            } else {
                txData = aDataPkt->defaultTxValue;
            }
            PL022_REGS[aHwID]->SSPDR = (txData & ((1UL << aDataPkt->dataWidth) - 1UL));
            SPIDrv_Dev.txIdx++;
            size--;
        }
    } else {
        srcBuf16 = (uint16_t *)&aDataPkt->srcBuf[SPIDrv_Dev.txIdx * 2UL];
        while (((PL022_REGS[aHwID]->SSPSR & SSPSR_TNF_MASK) == SSPSR_TNF_MASK)
                && (size != 0UL)) {
            if (NULL != aDataPkt->srcBuf) {
                txData = srcBuf16[i];
                i++;
            } else {
                txData = aDataPkt->defaultTxValue;
            }
            PL022_REGS[aHwID]->SSPDR = (txData & ((1UL << aDataPkt->dataWidth) - 1UL));
            SPIDrv_Dev.txIdx++;
            size--;
        }
    }
    if ((SPIDrv_Dev.txIdx == aDataPkt->xferSize) && (SPIDrv_Dev.rxIdx < aDataPkt->xferSize)) {
        retVal = BCM_ERR_EAGAIN;
    }
    *aSize = *aSize - size;
    if (TRUE == aIsAsyncXfer) {
        /* Unmask Tx interrupt */
        PL022_REGS[aHwID]->SSPIMSC |= SSPIMSC_TXIM_MASK;
    }

    return retVal;
}

static SPI_StateType PL022_GetHWState(SPI_HwIDType aHwID)
{
    SPI_StateType state;

    if (SPI_MAX_HW_ID <=  aHwID) {
        state = PL022_STATE_UNINIT;
    } else {
        state = PL022_Config[aHwID].state;
    }

    return state;
}

static void PL022_DeInit(SPI_HwIDType aHwID)
{
    if (PL022_STATE_UNINIT != PL022_Config[aHwID].state) {
        PL022_Config[aHwID].state = PL022_STATE_UNINIT;
        /* Wait for TX FIFO to become empty */
        PL022_WaitTxFifoEmpty(aHwID);
        /* Clear RX FIFO */
        while((PL022_REGS[aHwID]->SSPSR & SSPSR_RNE_MASK) == SSPSR_RNE_MASK) {
            PL022_REGS[aHwID]->SSPDR;
        }
        /* Power on reset state */
        PL022_REGS[aHwID]->SSPCR0 = 0UL;
        PL022_REGS[aHwID]->SSPCR1 = 0UL;
        PL022_REGS[aHwID]->SSPDR = 0UL;
        PL022_REGS[aHwID]->SSPCPSR = 0UL;
        PL022_REGS[aHwID]->SSPIMSC = 0UL;
    }
}

static void PL022_Init(SPI_HwIDType aHwID)
{
    if (PL022_STATE_UNINIT == PL022_Config[aHwID].state) {
        /* Mask the interrupts */
        PL022_REGS[aHwID]->SSPIMSC &= ~(SSPIMSC_TXIM_MASK | SSPIMSC_RXIM_MASK
                | SSPIMSC_RTIM_MASK | SSPIMSC_RORIM_MASK);
        /* enable SSP */
        PL022_REGS[aHwID]->SSPCR1 |= SSPCR1_SSE_MASK;
        PL022_REGS[aHwID]->SSPCR1 &= ~SSPCR1_MS_MASK;
        PL022_Config[aHwID].state = PL022_STATE_IDLE;
    }
}

static uint32_t SPIDrv_FrameQPkt(const SPI_XferPktType *const aXferPkt,
                                 const SPI_SeqType *const aSeq,
                                 uint32_t aIsLastPkt)
{
    uint32_t pkt;

    pkt = ((((uint32_t)aXferPkt->id << XFER_PKT_XFER_ID_SHIFT) & XFER_PKT_XFER_ID_MASK)
             | (((uint32_t)aSeq->id << XFER_PKT_SEQ_ID_SHIFT) & XFER_PKT_SEQ_ID_MASK)
             | (((uint32_t)aXferPkt->priority << XFER_PKT_PRIORITY_SHIFT) & XFER_PKT_PRIORITY_MASK)
             | (((uint32_t)aIsLastPkt << XFER_PKT_LAST_IND_SHIFT) & XFER_PKT_LAST_IND_MASK));

    return pkt;
}

static int32_t SPIDrv_XferPktQPush(const SPI_SeqType *const aSeq)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i, j, idx, startIdx;
    uint32_t isLastPkt = FALSE;
    SPI_XferPktPrioType priority;
    INTR_FlagType flag;
    const SPI_XferPktType *xferPkt;

    flag = INTR_Suspend();
    if (SPI_XFER_PKTS_MAX == (SPIDrv_Dev.xferPktCnt + aSeq->xferPktListSz)) {
        retVal = BCM_ERR_NOMEM;
        goto err;
    }
    xferPkt = aSeq->xferPktList;
    if (0UL < SPIDrv_Dev.xferPktCnt) {
        if (SPIDrv_Dev.currentXferPkt->priority > xferPkt->priority) {
            retVal = BCM_ERR_BUSY;
            goto err;
        }
        /* Check if the sequence shares the jobs with the ongoing sequence */
        for (i = 0UL; i < aSeq->xferPktListSz; i++) {
            for (j = 0UL; j < SPIDrv_Dev.xferPktCnt; j++) {
                if (xferPkt->id == ((SPIDrv_Dev.xferPktQ[j] & XFER_PKT_XFER_ID_MASK)
                            >> XFER_PKT_XFER_ID_SHIFT)) {
                    retVal = BCM_ERR_BUSY;
                    break;
                }
            }
            if (BCM_ERR_OK != retVal) {
                break;
            }
            xferPkt++;
        }
    }
    if (BCM_ERR_OK != retVal) {
        goto err;
    }
    xferPkt = aSeq->xferPktList;
    /* Check if queue is empty */
    if (0UL == SPIDrv_Dev.xferPktCnt) {
        for (i = 0UL; i < aSeq->xferPktListSz; i++) {
            if ((aSeq->xferPktListSz - 1UL) == i) {
                isLastPkt = TRUE;
            }
            SPIDrv_Dev.xferPktQ[i] = SPIDrv_FrameQPkt(xferPkt, aSeq, isLastPkt);
            xferPkt++;
        }
        SPIDrv_Dev.xferPktCnt = aSeq->xferPktListSz;
        goto err;
    }
    startIdx = 1UL;
    isLastPkt = FALSE;
    for (i = 0UL; i < aSeq->xferPktListSz; i++) {
        /* Find where to insert the new entry */
        for (idx = startIdx; idx < SPIDrv_Dev.xferPktCnt; idx++) {
            priority = ((SPIDrv_Dev.xferPktQ[idx] & XFER_PKT_PRIORITY_MASK)
                    >> XFER_PKT_PRIORITY_SHIFT);
            if (xferPkt->priority > priority) {
                break;
            }
        }
        /* Do the right shift */
        for (j = SPIDrv_Dev.xferPktCnt; j > idx; j--) {
            SPIDrv_Dev.xferPktQ[j] = SPIDrv_Dev.xferPktQ[j-1UL];
        }
        if ((aSeq->xferPktListSz - 1UL) == i) {
            isLastPkt = TRUE;
        }
        SPIDrv_Dev.xferPktQ[idx] = SPIDrv_FrameQPkt(xferPkt, aSeq, isLastPkt);
        startIdx = idx;
        SPIDrv_Dev.xferPktCnt++;
        SPIDrv_Dev.xferPktStatus[xferPkt->id] = SPI_XFER_PKT_STATUS_QUEUED;
        xferPkt++;
    }

err:
    INTR_Resume(flag);
    return retVal;
}

static void SPIDrv_XferPktQPop(void)
{
    uint32_t i;
    INTR_FlagType flag;

    flag = INTR_Suspend();
    if (1UL == SPIDrv_Dev.xferPktCnt) {
        SPIDrv_Dev.xferPktQ[0] = 0UL;
    } else {
        for (i = 0UL; i < (SPIDrv_Dev.xferPktCnt - 1UL); i++) {
            SPIDrv_Dev.xferPktQ[i] = SPIDrv_Dev.xferPktQ[i+1UL];
            SPIDrv_Dev.xferPktQ[i+1UL] = 0UL;
        }
    }
    SPIDrv_Dev.xferPktCnt--;
    INTR_Resume(flag);
}

static int32_t SPIDrv_XferPktQCancel(SPI_SeqIDType aSeqID)
{
    int32_t retVal = BCM_ERR_NOT_FOUND;
    uint32_t i;
    uint32_t seqFound = FALSE;
    SPI_SeqIDType seqID;
    SPI_XferPktIDType xferID;
    INTR_FlagType flag;

    flag = INTR_Suspend();
    for (i = 1UL; i < SPIDrv_Dev.xferPktCnt; ) {
        seqID = (uint8_t)((SPIDrv_Dev.xferPktQ[i] & XFER_PKT_SEQ_ID_MASK) >>
                XFER_PKT_SEQ_ID_SHIFT);
        if (seqID == aSeqID) {
            xferID = (uint16_t)((SPIDrv_Dev.xferPktQ[i] & XFER_PKT_XFER_ID_MASK) >>
                    XFER_PKT_XFER_ID_SHIFT);
            if ((SPIDrv_Dev.xferPktCnt - 1UL) > i) {
                SPIDrv_Dev.xferPktQ[i] = SPIDrv_Dev.xferPktQ[i+1UL];
                SPIDrv_Dev.xferPktQ[i+1UL] = 0UL;
            } else {
                SPIDrv_Dev.xferPktQ[i] = 0UL;
            }
            SPIDrv_Dev.xferPktCnt--;
            SPIDrv_Dev.xferPktStatus[xferID] = SPI_XFER_PKT_STATUS_FAILED;
            seqFound = TRUE;
        } else {
            i++;
        }
    }
    if (TRUE == seqFound) {
        SPIDrv_Dev.seqStatus[aSeqID] = SPI_SEQ_STATUS_CANCELLED;
        seqID = (uint8_t)((SPIDrv_Dev.xferPktQ[0] & XFER_PKT_SEQ_ID_MASK) >>
                XFER_PKT_SEQ_ID_SHIFT);
        if (seqID == aSeqID) {
            /* set the on-going packet as the last packet */
            SPIDrv_Dev.xferPktQ[0] |= XFER_PKT_LAST_IND_MASK;
            retVal = BCM_ERR_BUSY;
        } else {
            retVal = BCM_ERR_OK;
        }
    }
    INTR_Resume(flag);

    return retVal;
}

static int32_t SPIDrv_ValidateDataPkt(SPI_DataPktType *const aDataPkt)
{
    int32_t retVal = BCM_ERR_OK;

    if (SPI_DATA_PKTS_MAX <= aDataPkt->id) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if ((SPI_DATA_WIDTH_MIN > aDataPkt->dataWidth) ||
            (SPI_DATA_WIDTH_MAX < aDataPkt->dataWidth)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if ((0UL == aDataPkt->srcBufMaxSize) ||
            (0UL == aDataPkt->destBufMaxSize)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

err:
    return retVal;
}

static int32_t SPIDrv_ValidateXferPkt(const SPI_XferPktType *const aXferPkt)
{
    int32_t retVal;
    uint32_t i;
    SPI_DataPktType *dataPkt;
    SPI_DataPktIDType *dataPktIdxList;
    uint32_t dataPktIdxListSz;

    if (SPI_XFER_PKTS_MAX <= aXferPkt->id) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (SPI_MAX_HW_ID <= aXferPkt->hwID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (SPI_FRM_FORMAT_NATIONAL < aXferPkt->frameFormat) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if ((BITRATE_MIN > aXferPkt->bitRate) ||
            (BITRATE_MAX < aXferPkt->bitRate)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (SPI_XFER_PKT_PRIO_MAX < aXferPkt->priority) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    dataPktIdxList = aXferPkt->dataPktIdxList;
    dataPktIdxListSz = aXferPkt->dataPktIdxListSz;
    if ((NULL == dataPktIdxList) || (0UL == dataPktIdxListSz)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (SPI_DATA_PKTS_MAX < dataPktIdxListSz) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    for (i = 0UL; i < dataPktIdxListSz; i++) {
        dataPkt = &SPI_DataPktList[dataPktIdxList[i]];
        /* Data pkt validation */
        retVal = SPIDrv_ValidateDataPkt(dataPkt);
        if (BCM_ERR_OK != retVal) {
            break;
        }
    }

err:
    return retVal;
}

static int32_t SPIDrv_ValidateSeq(const SPI_SeqType *const aSeq)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;
    const SPI_XferPktType *xferPkt;

    if (SPI_SEQ_MAX <= aSeq->id) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    xferPkt = aSeq->xferPktList;
    if ((NULL == xferPkt) || (0UL == aSeq->xferPktListSz)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    /* Xfer pkt priorities shall be in decreasing order in a sequence */
    if (aSeq->xferPktListSz > 1UL) {
        for (i = 0UL; i < (aSeq->xferPktListSz - 1UL); i++) {
            if (xferPkt[i].priority < xferPkt[i+1UL].priority) {
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        }
    }
    if (BCM_ERR_OK != retVal) {
        goto err;
    }
    xferPkt = aSeq->xferPktList;
    for (i = 0UL; i < aSeq->xferPktListSz; i++) {
        /* Xfer pkt validation */
        retVal = SPIDrv_ValidateXferPkt(xferPkt);
        if (BCM_ERR_OK != retVal) {
            break;
        }
        xferPkt++;
    }

err:
    return retVal;
}

static const SPI_SeqType *SPIDrv_GetSeq(SPI_SeqIDType aSeqID)
{
    uint32_t i;
    const SPI_SeqType *seq = SPI_Config.seqList;

    if (SPI_SEQ_MAX <= aSeqID) {
        seq = NULL;
    } else {
        /* check if config corresponding to aSeqID is present */
        for (i = 0UL; i < SPI_Config.seqListSz; i++) {
            if (seq->id == aSeqID) {
                break;
            }
            seq++;
        }
        if (i == SPI_Config.seqListSz) {
            seq = NULL;
        }
    }

    return seq;
}

static void SPIDrv_SetDataWidth(SPI_HwIDType aHwID,
                                SPI_DataWidthType aDataWidth)
{
    /* Set the data size */
    PL022_REGS[aHwID]->SSPCR0 |= aDataWidth;
}

static void SPIDrv_SetupXfer(void)
{
    int32_t err = BCM_ERR_UNKNOWN;
    uint32_t i, j;
    const SPI_SeqType *seq = SPI_Config.seqList;
    const SPI_XferPktType *xferPkt;
    SPI_SeqIDType seqID = (uint8_t)((SPIDrv_Dev.xferPktQ[0] & XFER_PKT_SEQ_ID_MASK)
            >> XFER_PKT_SEQ_ID_SHIFT);
    SPI_XferPktIDType xferID = (uint16_t)((SPIDrv_Dev.xferPktQ[0] & XFER_PKT_XFER_ID_MASK)
            >> XFER_PKT_XFER_ID_SHIFT);

    for (i = 0UL; i < SPI_Config.seqListSz; i++) {
        if (seq->id != seqID) {
            seq++;
            continue;
        }
        xferPkt = seq->xferPktList;
        for (j = 0UL; j < seq->xferPktListSz; j++) {
            if (xferPkt->id != xferID) {
                xferPkt++;
                continue;
            }
            SPIDrv_Dev.currentSeq = seq;
            SPIDrv_Dev.currentXferPkt = xferPkt;
            SPIDrv_Dev.currentDataPktIdx = 0UL;
            SPIDrv_Dev.currentDataPkt =
                &SPI_DataPktList[xferPkt->dataPktIdxList[0]];
            SPIDrv_Dev.txIdx = 0UL;
            SPIDrv_Dev.rxIdx = 0UL;
            SPIDrv_Dev.dataPktCnt = xferPkt->dataPktIdxListSz;
            SPIDrv_Dev.bufSize = SPIDrv_Dev.currentDataPkt->xferSize;
            SPIDrv_Dev.xferPktStatus[xferPkt->id] = SPI_XFER_PKT_STATUS_PENDING;
            PL022_Config[xferPkt->hwID].state = PL022_STATE_BUSY;
            PL022_Config[xferPkt->hwID].frameFormat = xferPkt->frameFormat;
            PL022_Config[xferPkt->hwID].bitRate = xferPkt->bitRate;
            PL022_Configure(xferPkt->hwID, &PL022_Config[xferPkt->hwID]);
            SPIDrv_SetDataWidth(xferPkt->hwID, SPIDrv_Dev.currentDataPkt->dataWidth - 1UL);
            err = BCM_ERR_OK;
            break;
        }
        if (BCM_ERR_OK == err) {
            break;
        }
    }
}

static void SPIDrv_Xfer(uint32_t aIsAsyncXfer)
{
    int32_t retVal;
    uint32_t size;

    do {
        if (SPIDrv_Dev.bufSize <= SPI_FIFO_SIZE) {
            size = SPIDrv_Dev.bufSize;
        } else {
            size = SPI_FIFO_SIZE;
        }
        retVal = PL022_Xfer(SPIDrv_Dev.currentXferPkt->hwID, SPIDrv_Dev.currentDataPkt,
                &size, aIsAsyncXfer);
        SPIDrv_Dev.bufSize -= size;
    } while (BCM_ERR_EAGAIN == retVal);
}

static int32_t SPIDrv_PktHandler(uint32_t aIsAsyncXfer)
{
    int32_t retVal = BCM_ERR_EAGAIN;
    uint32_t idx;

    if (0UL == SPIDrv_Dev.bufSize) {
        SPIDrv_Dev.dataPktCnt--;
        if (0UL == SPIDrv_Dev.dataPktCnt) {
            PL022_Config[SPIDrv_Dev.currentXferPkt->hwID].state = PL022_STATE_IDLE;
            SPIDrv_Dev.xferPktStatus[SPIDrv_Dev.currentXferPkt->id] =
                SPI_XFER_PKT_STATUS_OK;
            /* Send pkt end notification */
            if (TRUE == aIsAsyncXfer) {
                if (NULL != SPIDrv_Dev.currentXferPkt->cb) {
                    SPIDrv_Dev.currentXferPkt->cb();
                }
            }
            /* Check if this is the last packet of the sequence */
            if (((SPIDrv_Dev.xferPktQ[0] & XFER_PKT_LAST_IND_MASK)
                        >> XFER_PKT_LAST_IND_SHIFT) == TRUE) {
                if (SPI_SEQ_STATUS_CANCELLED !=
                        SPIDrv_Dev.seqStatus[SPIDrv_Dev.currentSeq->id]) {
                    SPIDrv_Dev.seqStatus[SPIDrv_Dev.currentSeq->id] = SPI_SEQ_STATUS_OK;
                }
                /* Send seq end notification */
                if (TRUE == aIsAsyncXfer) {
                    if (NULL != SPIDrv_Dev.currentSeq->cb) {
                        SPIDrv_Dev.currentSeq->cb();
                    }
                }
            }
            /* Pop the packet from the queue */
            SPIDrv_XferPktQPop();
            if (0UL == SPIDrv_Dev.xferPktCnt) {
                /* All the packets are transferred */
                retVal = BCM_ERR_OK;
            } else {
                /* Go to next packet */
                SPIDrv_SetupXfer();
                SPIDrv_Xfer(aIsAsyncXfer);
            }
        } else {
            /* Go to next data packet */
            idx = SPIDrv_Dev.currentXferPkt->dataPktIdxList[++SPIDrv_Dev.currentDataPktIdx];
            SPIDrv_Dev.currentDataPkt = &SPI_DataPktList[idx];
            SPIDrv_Dev.bufSize = SPIDrv_Dev.currentDataPkt->xferSize;
            SPIDrv_Dev.txIdx = 0UL;
            SPIDrv_Dev.rxIdx = 0UL;
            SPIDrv_SetDataWidth(SPIDrv_Dev.currentXferPkt->hwID,
                    SPIDrv_Dev.currentDataPkt->dataWidth - 1UL);
            SPIDrv_Xfer(aIsAsyncXfer);
        }
    } else {
        SPIDrv_Xfer(aIsAsyncXfer);
    }

    return retVal;
}

static int32_t SPIDrv_StateHandler(uint32_t aIsAsyncXfer)
{
    int32_t retVal = BCM_ERR_EAGAIN;;

    switch (SPIDrv_Dev.xferState) {
    case SPI_XFER_STATE_IDLE:
        if (0UL != SPIDrv_Dev.xferPktCnt) {
            SPIDrv_Dev.xferState = SPI_XFER_STATE_BUSY;
            SPIDrv_SetupXfer();
            SPIDrv_Xfer(aIsAsyncXfer);
        }
        break;
    case SPI_XFER_STATE_BUSY:
        retVal = SPIDrv_PktHandler(aIsAsyncXfer);
        if (BCM_ERR_OK == retVal) {
            SPIDrv_Dev.state = SPI_STATE_IDLE;
            SPIDrv_Dev.xferState = SPI_XFER_STATE_IDLE;
        }
        break;
    default:
        /* will never reach here */
        retVal = BCM_ERR_UNKNOWN;
        break;
    }

    return retVal;
}

void SPIDrv_IRQEventHandler(SPI_HwIDType aHwID)
{
    int32_t retVal;

    retVal = SPIDrv_StateHandler(TRUE);
    if (BCM_ERR_OK == retVal) {
        /* Mask the TX interrupt */
        PL022_REGS[aHwID]->SSPIMSC &= ~SSPIMSC_TXIM_MASK;
    }
}

void SPIDrv_IRQHandler(SPI_HwIDType aHwID)
{
    /* Mask the TX interrupt */
    PL022_REGS[aHwID]->SSPIMSC &= ~SSPIMSC_TXIM_MASK;
}

SPI_StateType SPIDrv_GetHWState(SPI_HwIDType aHwID)
{
    return PL022_GetHWState(aHwID);
}

SPI_StateType SPIDrv_GetDrvState(void)
{
    return SPIDrv_Dev.state;
}

SPI_SeqStatusType SPIDrv_GetSeqStatus(SPI_SeqIDType aSeqID)
{
    int32_t retVal = BCM_ERR_OK;
    const SPI_SeqType *seq;
    SPI_SeqStatusType status;

    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    seq = SPIDrv_GetSeq(aSeqID);
    if (NULL == seq) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        status = SPIDrv_Dev.seqStatus[aSeqID];
    }

err:
    if (BCM_ERR_OK != retVal) {
        /* The status is undefined in this case */
        status = SPI_SEQ_STATUS_OK;
    }
    return status;
}

SPI_XferPktStatusType SPIDrv_GetXferPktStatus(SPI_XferPktIDType aXferPktID)
{
    SPI_XferPktStatusType status;

    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        status = SPI_XFER_PKT_STATUS_OK;
    } else {
        status = SPIDrv_Dev.xferPktStatus[aXferPktID];
    }

    return status;
}

int32_t SPIDrv_Cancel(SPI_SeqIDType aSeqID)
{
    int32_t retVal = BCM_ERR_OK;
    const SPI_SeqType *seq;

    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    seq = SPIDrv_GetSeq(aSeqID);
    if (NULL == seq) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    retVal = SPIDrv_XferPktQCancel(aSeqID);
    if (BCM_ERR_OK == retVal) {
        /* Give seq end notification callback */
        if (NULL != seq->cb) {
            seq->cb();
        }
    } else {
        retVal = BCM_ERR_OK;
    }

err:
    return retVal;
}

int32_t SPIDrv_SyncXfer(SPI_SeqIDType aSeqID)
{
    int32_t retVal = BCM_ERR_OK;
    const SPI_SeqType *seq;

    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    seq = SPIDrv_GetSeq(aSeqID);
    if (NULL == seq) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (SPI_STATE_BUSY == SPIDrv_Dev.state) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }
    retVal = SPIDrv_XferPktQPush(seq);
    if (BCM_ERR_OK == retVal) {
        SPIDrv_Dev.state = SPI_STATE_BUSY;
        SPIDrv_Dev.xferState = SPI_XFER_STATE_IDLE;
        SPIDrv_Dev.seqStatus[aSeqID] = SPI_SEQ_STATUS_PENDING;
    } else {
        goto err;
    }
    do {
        retVal = SPIDrv_StateHandler(FALSE);
        /* Poll on FIFO empty bit */
        PL022_WaitTxFifoEmpty(SPIDrv_Dev.currentXferPkt->hwID);
    } while (BCM_ERR_EAGAIN == retVal);

err:
    return retVal;
}

int32_t SPIDrv_AsyncXfer(SPI_SeqIDType aSeqID)
{
    int32_t retVal = BCM_ERR_OK;
    const SPI_SeqType *seq;

    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    seq = SPIDrv_GetSeq(aSeqID);
    if (NULL == seq) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (SPI_SEQ_STATUS_PENDING == SPIDrv_Dev.seqStatus[aSeqID]) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }
    /* Check if a transmission is on-going */
    if (SPI_STATE_BUSY == SPIDrv_Dev.state) {
        if (FALSE == SPI_Config.isSeqInterruptible) {
            retVal = BCM_ERR_BUSY;
            goto err;
        }
        if (FALSE == SPIDrv_Dev.currentSeq->isInterruptible) {
            retVal = BCM_ERR_BUSY;
            goto err;
        }
        /* Push to the queue */
        retVal = SPIDrv_XferPktQPush(seq);
        if (BCM_ERR_OK == retVal) {
            SPIDrv_Dev.seqStatus[aSeqID] = SPI_SEQ_STATUS_PENDING;
        }
    } else {
        retVal = SPIDrv_XferPktQPush(seq);
        if (BCM_ERR_OK == retVal) {
            SPIDrv_Dev.state = SPI_STATE_BUSY;
            SPIDrv_Dev.xferState = SPI_XFER_STATE_IDLE;
            SPIDrv_Dev.seqStatus[aSeqID] = SPI_SEQ_STATUS_PENDING;
            (void)SPIDrv_StateHandler(TRUE);
        }
    }

err:
    return retVal;
}

int32_t SPIDrv_SetupBuf(SPI_DataPktIDType aDataPktID,
                        const SPI_DataBufType *const aSrc,
                        SPI_DataBufType *const aDest,
                        SPI_DataSizeType aLen)
{
    int32_t retVal = BCM_ERR_OK;
    SPI_DataPktType *dataPkt;

    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    if ((NULL == aSrc) || (NULL == aDest)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (0UL == aLen) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    if (SPI_DATA_PKTS_MAX <= aDataPktID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    dataPkt = &SPI_DataPktList[aDataPktID];
    if ((dataPkt->srcBufMaxSize < aLen) || (dataPkt->destBufMaxSize < aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    dataPkt->srcBuf = aSrc;
    dataPkt->destBuf = aDest;
    dataPkt->xferSize = aLen;

err:
    return retVal;
}

int32_t SPIDrv_DeInit(void)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t i;

    if (SPI_STATE_BUSY == SPIDrv_Dev.state) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }
    if (SPI_STATE_UNINIT == SPIDrv_Dev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }
    SPIDrv_Dev.state = SPI_STATE_UNINIT;
    for (i = 0U; i < SPI_MAX_HW_ID; i++) {
        PL022_DeInit(i);
    }

err:
    return retVal;
}

int32_t SPIDrv_Init(void)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i, j, k;
    const SPI_SeqType *seq = SPI_Config.seqList;
    const SPI_XferPktType *xferPkt;
    SPI_DataPktType *dataPkt;
    SPI_DataPktIDType *dataPktIdxList;

    if (SPI_STATE_UNINIT != SPIDrv_Dev.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }
    if ((NULL == seq) || (0UL == SPI_Config.seqListSz)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }
    for (i = 0UL; i < SPI_Config.seqListSz; i++) {
        /* Sequence configuration validation */
        retVal = SPIDrv_ValidateSeq(seq);
        if (BCM_ERR_OK != retVal) {
            break;
        }
        seq++;
    }
    if (BCM_ERR_OK != retVal) {
        goto err;
    }
    seq = SPI_Config.seqList;
    for (i = 0UL; i < SPI_Config.seqListSz; i++) {
        xferPkt = seq->xferPktList;
        for (j = 0UL; j < seq->xferPktListSz; j++) {
            dataPktIdxList = xferPkt->dataPktIdxList;
            for (k = 0UL; k < xferPkt->dataPktIdxListSz; k++) {
                dataPkt = &SPI_DataPktList[dataPktIdxList[k]];
                dataPkt->srcBuf = NULL;
                dataPkt->destBuf = NULL;
            }
            PL022_Init(xferPkt->hwID);
            SPIDrv_Dev.xferPktStatus[xferPkt->id] = SPI_XFER_PKT_STATUS_OK;
            xferPkt++;
        }
        SPIDrv_Dev.seqStatus[seq->id] = SPI_SEQ_STATUS_OK;
        seq++;
    }
    SPIDrv_Dev.state = SPI_STATE_IDLE;

err:
    return retVal;
}
