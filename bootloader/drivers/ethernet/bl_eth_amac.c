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

/**
    @defgroup grp_bl_eth_impl Implementation
    @ingroup grp_bl_eth

    @addtogroup grp_bl_eth_impl
    @{
    @file bl_eth_amac.c
    @brief This file implements AMAC layer of Ethernet driver

    @version 0.30 Imported from docx
*/

#include <inttypes.h>
#include <string.h>
#include <bl_log.h>
#include <bl_chip_config.h>
#include <cortex_mx.h>
#include <cache.h>
#include <bl_compiler.h>
#include <bl_bcm_err.h>
#include <bl_utils.h>
#include <bcm_time.h>
#include "bl_eth_amac.h"
#include "bl_eth_unimac.h"

/**
    @name Ethernet Controller Driver API IDs
    @{
    @brief API IDs for Ethernet Controller
*/
#define BRCM_DSGN_BL_ETHER_TXCH0_BUFF_COUNT_MACRO       (0x80U) /**< @brief #BL_TXCH0_BUFF_COUNT */
#define BRCM_DSGN_BL_ETHER_RX_BUFF_COUNT_MACRO          (0x81U) /**< @brief #BL_RX_BUFF_COUNT */
#define BRCM_DSGN_BL_ETHER_DESC_TABLE_DESC_CNT_MACRO    (0x82U) /**< @brief #BL_DESC_TABLE_DESC_CNT */
#define BRCM_DSGN_BL_ETHER_TX_CHAN_DESC_CNT_MACRO       (0x83U) /**< @brief #BL_TX_CHAN_DESC_CNT */
#define BRCM_DSGN_BL_ETHER_XMT_INIT_PROC                (0x84U) /**< @brief #BL_XmtInit */
#define BRCM_DSGN_BL_ETHER_RCV_INIT_PROC                (0x85U) /**< @brief #BL_ETHER_RcvInit */
#define BRCM_DSGN_BL_ETHER_XMT_COMP_IRQ_HANDLER_PROC    (0x86U) /**< @brief #BL_XmtCompIRQHandler */
#define BRCM_DSGN_BL_ETHER_RCV_COMP_IRQ_HANDLER_PROC    (0x87U) /**< @brief #BL_RcvCompIRQHandler */
#define BRCM_DSGN_BL_ETHER_DMA_ERR_IRQ_HANDLER_PROC     (0x88U) /**< @brief #BL_DMAErrIRQHandler */
#define BRCM_DSGN_BL_ETHER_XMT_ERR_IRQ_HANDLER_PROC     (0x89U) /**< @brief #BL_XmtErrIRQHandler */
#define BRCM_DSGN_BL_ETHER_RCV_ERR_IRQ_HANDLER_PROC     (0x8AU) /**< @brief #BL_RcvErrIRQHandler */
#define BRCM_DSGN_BL_ETHER_PROCESS_XMTD_PKTS_PROC       (0x8BU) /**< @brief #BL_ProcessXmtdPkts */
#define BRCM_DSGN_BL_ETHER_PROCESS_RCVD_PKTS_PROC       (0x8CU) /**< @brief #BL_ProcessRcvdPkts */
#define BRCM_DSGN_BL_ETHER_XMT_WAIT_TX_DONE_PROC        (0x8DU) /**< @brief #BL_XmtWaitTxDone */
/** @} */

#define BL_GetModuleLogLevel()         (BL_LOG_LVL_INFO)

#define BL_ETHER_HW_ID_0               (0UL)
#define BL_AMAC_RESET_DELAY_CNT        (100UL)
#define BL_AMAC_TX_WAIT_DELAY_US       (100UL)
#define BL_AMAC_TX_DELAY_COUNT         (5UL)
#define BL_DMA_DESC_ERR_MASK           (INTSTATUS_DESCRERR_MASK|   \
                                    INTSTATUS_DATAERR_MASK|     \
                                    INTSTATUS_DESCPROTOERR_MASK)

#define BL_TX_PARITY_EN        (0UL) /**< TX parity enable */
#define BL_TX_PARITY_DIS       (1UL) /**< TX parity disable */

#define BL_RX_DESC_OFFSET(descIdx)     ((sizeof(BL_DmaDescType) * (descIdx)) & 0x1FFFUL)
#define BL_TX_DESC_OFFSET(descIdx)     ((sizeof(BL_DmaDescType) * (descIdx)) & 0x1FFFUL)


/**
    @brief Transmit channel buffer count

    @todo Move these Macros as part of the application config file & should be resolved compile/link time

    @trace #BRCM_DSGN_BL_ETHER_TXCH0_BUFF_COUNT_MACRO #BRCM_REQ_BL_ETHER_TXCH0_BUFF_COUNT
*/
#define BL_TXCH0_BUFF_COUNT        (4UL)

/**
    @brief Receive channel buffer count

    @todo Move these Macros as part of the application config file & should be resolved compile/link time

    @trace #BRCM_DSGN_BL_ETHER_RX_BUFF_COUNT_MACRO #BRCM_REQ_BL_ETHER_RX_BUFF_COUNT
*/
#define BL_RX_BUFF_COUNT           (8UL)

/**
    @brief Number of descriptor in descriptor table for each channel

    @trace #BRCM_DSGN_BL_ETHER_DESC_TABLE_DESC_CNT_MACRO #BRCM_REQ_BL_ETHER_DESC_TABLE_DESC_CNT
*/
#define BL_DESC_TABLE_DESC_CNT     (16UL)

/* Descriptor table for all the channels are allocated for 16 entries making
   total descriptor table size to 256Bytes */
#define BL_DESC_TABLE_SIZE         (BL_DESC_TABLE_DESC_CNT * BL_DMA_DESC_SIZE)

/**
    @brief Number of descriptor allocated for transmit channel.
    Number of descriptor shall be 1 more than transmit channel buffers.
    This is required as the DMA descriptor queue expects last DMA descriptor + 1 = current DMA descriptor when queue is
    full & last descriptor shall not equal to current descriptor otherwise DMA will become IDLE. So, to satisfy above
    both condition and in case of queue all buffers immediately. It is required to allocate 1 extra DMA descriptor slot.

    @trace #BRCM_DSGN_BL_ETHER_TX_CHAN_DESC_CNT_MACRO #BRCM_REQ_BL_ETHER_TX_CHAN_DESC_CNT
*/
#define BL_TX_CHAN_DESC_CNT(txChanIdx) (BL_TxChannInfo[txChanIdx].pktBuffCnt + 1UL)

/* Start address of DMA descriptor table shall be 8KByte aligned.
   NOTE: With 4K alignment DMA Descriptor protocol error is observed */
static BL_DmaDescType BL_TxCh0DmaDescTbl[BL_DESC_TABLE_DESC_CNT] BL_COMP_ALIGN(8192);

/* Buffer are to be aligned to 32byte boundary and should have 32byte
   aligned size */
static BL_TxPktType BL_TxCh0PktBuf[BL_TXCH0_BUFF_COUNT] BL_COMP_ALIGN(32);

static BL_TxPktType *BL_TxChPktBuf[BL_ETHER_TX_CHAN_CNT] = {BL_TxCh0PktBuf};
static BL_RxPktType BL_RxPkt[BL_RX_BUFF_COUNT] BL_COMP_ALIGN(32);

const static BL_DmaDescType BL_RxDmaDescTbl[BL_RX_BUFF_COUNT] BL_COMP_ALIGN(8192) =
{
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_DIS, &(BL_RxPkt[0UL].rxPktInfo)),
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_DIS, &(BL_RxPkt[1UL].rxPktInfo)),
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_DIS, &(BL_RxPkt[2UL].rxPktInfo)),
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_DIS, &(BL_RxPkt[3UL].rxPktInfo)),
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_DIS, &(BL_RxPkt[4UL].rxPktInfo)),
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_DIS, &(BL_RxPkt[5UL].rxPktInfo)),
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_DIS, &(BL_RxPkt[6UL].rxPktInfo)),
    BL_INIT_RXDESC(BL_DMA_DESC_EOT_EN,  &(BL_RxPkt[7UL].rxPktInfo)),
};

/* Global variables */
static BL_AMAC_RegType * const BL_AMAC_REGS = (BL_AMAC_RegType *)BL_AMAC_BASE;

/* Packet buffer information */
static BL_TxPktBuffInfoType BL_COMP_SECTION(".data.drivers") BL_TxCh0PktBuffInfo[BL_TXCH0_BUFF_COUNT];
static BL_RxPktBuffInfoType BL_COMP_SECTION(".data.drivers") BL_RxPktBuffInfo[BL_RX_BUFF_COUNT];

static BL_RxChannInfoType BL_COMP_SECTION(".data.drivers")
    BL_RxChannInfo[BL_ETHER_RX_CHAN_CNT] =
    {
        BL_INIT_RX_CHANNEL_INFO(BL_ETHER_STATE_UNINIT, BL_RxPktBuffInfo,
                            0UL, 0UL, 0UL, BL_RxDmaDescTbl, BL_FALSE),
    };

static BL_TxChannInfoType BL_COMP_SECTION(".data.drivers")
    BL_TxChannInfo[BL_ETHER_TX_CHAN_CNT] =
    {
        BL_INIT_TX_CHANNEL_INFO(BL_ETHER_STATE_UNINIT, BL_TxCh0PktBuffInfo,
            BL_TXCH0_BUFF_COUNT, 0UL, 0UL, 0UL, 0UL, BL_TxCh0DmaDescTbl, BL_FALSE),
    };

static BL_COMP_NEVER_INLINE void BL_AMAC_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_AMC_ID, aInstanceID, 0UL, aErr, 4UL, values);
}

static uint32_t BL_ModIncIdx(uint32_t aIdx,
                             uint32_t aMaxCount)
{
    uint32_t bIdx = aIdx + 1UL;

    if (aMaxCount == bIdx) {
        bIdx = 0UL;
    }

    return bIdx;
}

static uint32_t BL_ModDecIdx(uint32_t aIdx,
                             uint32_t aMaxCount)
{
    uint32_t bIdx = aIdx;

    if (0UL == bIdx) {
        bIdx = aMaxCount - 1UL;
    } else {
        bIdx = bIdx - 1UL;
    }

    return bIdx;
}

static void BL_XmtClearInterruptStatus(BL_XmtChannelType aXmtCh)
{
    uint32_t intStatus;

    /* Interrupt status bits are Write '1' on clear */

    intStatus = BL_AMAC_REGS->INTSTATUS;
    switch (aXmtCh) {
    case BL_AMAC_XMT_CH_0:
        intStatus |= INTSTATUS_XMTINT_0_MASK;
        break;
    default:
        break;
    }

    BL_AMAC_REGS->INTSTATUS = intStatus;
}

static void BL_XmtGetStatus0Reg(BL_XmtChannelType aXmtCh,
                                uint32_t *const aVal)
{
    *aVal = 0UL;
    switch (aXmtCh) {
    case BL_AMAC_XMT_CH_0:
        *aVal = BL_AMAC_REGS->XMTSTATUS0_0;
        break;
    default:
        break;
    }
}

static void BL_XmtGetStatus1Reg(BL_XmtChannelType aXmtCh,
                                uint32_t *const aVal)
{
    *aVal = 0UL;
    switch (aXmtCh) {
    case BL_AMAC_XMT_CH_0:
        *aVal = BL_AMAC_REGS->XMTSTATUS1_0;
        break;
    default:
        break;
    }
}

static uint32_t BL_XmtGetCntrlReg(BL_XmtChannelType aXmtCh)
{
    uint32_t xmtCntrl = 0UL;

    switch (aXmtCh) {
    case BL_AMAC_XMT_CH_0:
        xmtCntrl = BL_AMAC_REGS->XMTCTRL_0;
        break;
    default:
        break;
    }

    return xmtCntrl;
}

static void BL_XmtSetCntrlReg(BL_XmtChannelType aXmtCh,
                              uint32_t value)
{
    switch (aXmtCh) {
    case BL_AMAC_XMT_CH_0:
        BL_AMAC_REGS->XMTCTRL_0 = value;
        break;
    default:
        break;
    }
}

static void BL_XmtSetLastDesc(BL_XmtChannelType aXmtCh,
                              uint32_t offset)
{
    switch (aXmtCh) {
    case BL_AMAC_XMT_CH_0:
        BL_AMAC_REGS->XMTPTR_0 = offset & XMTPTR_LASTDSCR_MASK;
        break;
    default:
        break;
    }
}

static uint32_t BL_XmtGetLastDesc(BL_XmtChannelType aXmtCh)
{
    uint32_t lastDesc = 0UL;
    switch (aXmtCh) {
    case BL_AMAC_XMT_CH_0:
        lastDesc = BL_AMAC_REGS->XMTPTR_0 & XMTPTR_LASTDSCR_MASK;
        break;
    default:
        break;
    }

    return lastDesc;
}

static uint32_t BL_XmtGetCurrDesc(BL_XmtChannelType aXmtCh)
{
    uint32_t status0;

    BL_XmtGetStatus0Reg(aXmtCh, &status0);
    status0 = (status0 & XMTSTATUS0_CURRDSCR_MASK) >> XMTSTATUS0_CURRDSCR_SHIFT;

    return status0;
}

static void BL_XmtGetState(BL_XmtChannelType aXmtCh,
                           BL_ChannStateType *const aState)
{
    uint32_t status0;
    BL_XmtGetStatus0Reg(aXmtCh, &status0);
    status0 = (status0 & XMTSTATUS0_XMTSTATE_MASK) >> XMTSTATUS0_XMTSTATE_SHIFT;
    *aState = status0;
}

static void BL_XmtGetErr(BL_XmtChannelType aXmtCh,
                         BL_ChannErrType *const aErr)
{
    uint32_t status1;
    BL_XmtGetStatus1Reg(aXmtCh, &status1);
    status1 = (status1 & XMTSTATUS1_XMTERR_MASK) >> XMTSTATUS1_XMTERR_SHIFT;
    *aErr = status1;
}

static void BL_XmtSetParityDisable(BL_XmtChannelType aXmtCh,
                                   uint32_t aTxParityDis)
{
    uint32_t xmtCntrl;

    /* Set Tx Parity check */
    xmtCntrl = BL_XmtGetCntrlReg(aXmtCh);
    if (BL_TX_PARITY_DIS == aTxParityDis) {
        xmtCntrl |= XMTCTRL_TXPARITYCHECKDIS_MASK;
    } else {
        xmtCntrl &= ~XMTCTRL_TXPARITYCHECKDIS_MASK;
    }

    BL_XmtSetCntrlReg(aXmtCh, xmtCntrl);
}

static void BL_XmtDisable(BL_XmtChannelType aXmtCh)
{
    uint32_t xmtCntrl;
    BL_ChannStateType state;
    uint32_t count = BL_AMAC_RESET_DELAY_CNT;

    xmtCntrl = BL_XmtGetCntrlReg(aXmtCh);
    xmtCntrl &= ~XMTCTRL_XMTEN_MASK;
    BL_XmtSetCntrlReg(aXmtCh, xmtCntrl);

    /**
     * Wait for channel state to become disabled
     */
    do  {
        BL_XmtGetState(aXmtCh, &state);
        if (BL_CHANN_STATE_DISABLED == state) {
            break;
        }
        count--;
    } while (count > 0UL);

    if (0UL == count) {
        BL_AMAC_ReportError(BL_BCM_ERR_TIME_OUT, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
    }
}

static void BL_XmtEnable(BL_XmtChannelType aXmtCh)
{
    uint32_t xmtCntrl;
    BL_ChannStateType state;
    uint32_t count = BL_AMAC_RESET_DELAY_CNT;

    xmtCntrl = BL_XmtGetCntrlReg(aXmtCh);
    xmtCntrl |= XMTCTRL_XMTEN_MASK;
    BL_XmtSetCntrlReg(aXmtCh, xmtCntrl);

    do  {
        BL_XmtGetState(aXmtCh, &state);
        if (BL_CHANN_STATE_IDLE == state) {
            break;
        }
        count--;
    } while (count > 0UL);

    if (0UL == count) {
        BL_AMAC_ReportError(BL_BCM_ERR_TIME_OUT, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
    }
}

static void BL_IsXmtEnabled(BL_XmtChannelType aXmtCh,
                            uint32_t *const aEnabled)
{
    uint32_t xmtCntrl;

    xmtCntrl = BL_XmtGetCntrlReg(aXmtCh);
    if ((xmtCntrl & XMTCTRL_XMTEN_MASK) == XMTCTRL_XMTEN_MASK) {
        *aEnabled = 1UL;
    } else {
        *aEnabled = 0UL;
    }
}

/*
 * TX channel reset sequence:
 * -> disable the channel (clear XmtEn bit in XmtControl register)
 * -> Poll until XmtState field in XmtStatus0 register indicated that channel
 *    is disabled
 * -> enable the channel (set Xmten bit in XmtControl Register)
 */
static void BL_XmtReset(BL_XmtChannelType aXmtCh)
{
    BL_XmtDisable(aXmtCh);
    BL_XmtEnable(aXmtCh);
}

#ifdef BL_ETH_AMAC_DEBUG
static void BL_XmtSetLoopBackMode(BL_XmtChannelType aXmtCh,
                                  uint32_t aEnable)
{
    uint32_t xmtCntrl;

    xmtCntrl = BL_XmtGetCntrlReg(aXmtCh);
    if (aEnable == 1UL) {
        xmtCntrl |= XMTCTRL_DMALOOPBACKMODE_MASK;
    } else {
        xmtCntrl &= ~XMTCTRL_DMALOOPBACKMODE_MASK;
    }
    BL_XmtSetCntrlReg(aXmtCh, xmtCntrl);
}
#endif

static void BL_XmtSetDescTableAddr(BL_XmtChannelType aXmtCh,
                                   uint32_t aAddrHigh,
                                   uint32_t aAddrLow)
{
    uint32_t enabled;

    BL_IsXmtEnabled(aXmtCh, &enabled);

    if ((enabled == BL_FALSE) && ((aAddrLow & 0xFUL) == 0UL)) {
        switch (aXmtCh) {
            case BL_AMAC_XMT_CH_0:
                BL_AMAC_REGS->XMTADDR_LOW_0 = aAddrLow;
                BL_AMAC_REGS->XMTADDR_HIGH_0 = aAddrHigh;
                break;
            default:
                break;
        }
    }
}

static void BL_RcvGetStatus0Reg(BL_RcvChannelType aRcvCh,
                                uint32_t *const aVal)
{
    *aVal = 0UL;
    switch(aRcvCh) {
    case BL_AMAC_RCV_CH_0:
        *aVal = BL_AMAC_REGS->RCVSTATUS0;
        break;
    default:
        break;
    }
}

static void BL_RcvGetStatus1Reg(BL_RcvChannelType aRcvCh,
                                uint32_t *const aVal)
{
    *aVal = 0UL;
    switch(aRcvCh) {
    case BL_AMAC_RCV_CH_0:
        *aVal = BL_AMAC_REGS->RCVSTATUS1;
        break;
    default:
        break;
    }
}

static uint32_t BL_RcvGetLastDesc(BL_RcvChannelType aRcvCh)
{
    uint32_t lastDesc = 0UL;

    lastDesc = (BL_AMAC_REGS->RCVPTR & RCVPTR_LASTDSCR_MASK)
                    >> RCVPTR_LASTDSCR_SHIFT;
    return lastDesc;
}

static void BL_RcvSetLastDesc(BL_RcvChannelType aRcvCh,
                              uint32_t offset)
{
    BL_AMAC_REGS->RCVPTR = offset & RCVPTR_LASTDSCR_MASK;
}

static uint32_t BL_RcvGetCurrDesc(BL_RcvChannelType aRcvCh)
{
    uint32_t status0;

    BL_RcvGetStatus0Reg(aRcvCh, &status0);
    status0 &= (RCVSTATUS0_CURRDSCR_MASK >> RCVSTATUS0_CURRDSCR_SHIFT);

    return status0;
}

static void BL_RcvGetErr(BL_RcvChannelType aRcvCh,
                         BL_ChannErrType *const aErr)
{
    uint32_t status1;
    BL_RcvGetStatus1Reg(aRcvCh, &status1);
    status1 &= (RCVSTATUS1_RCVERR_MASK >> RCVSTATUS1_RCVERR_SHIFT);
    *aErr = status1;
}

static void BL_RcvClearInterruptStatus(BL_RcvChannelType aRcvCh)
{
    uint32_t intStatus;

    /* Interrupt status bits are Write '1' on clear */
    intStatus = BL_AMAC_REGS->INTSTATUS;
    switch(aRcvCh) {
    case BL_AMAC_RCV_CH_0:
        intStatus |= INTSTATUS_RCVINT_MASK;
        break;
    default:
        break;
    }
    BL_AMAC_REGS->INTSTATUS = intStatus;
}

static void BL_RcvGetState(BL_RcvChannelType aRcvCh,
                           BL_ChannStateType *const aState)
{
    uint32_t status0;
    BL_RcvGetStatus0Reg(aRcvCh, &status0);
    status0 = (status0 & RCVSTATUS0_RCVSTATE_MASK) >>
        RCVSTATUS0_RCVSTATE_SHIFT;
    *aState = status0;
}

static uint32_t BL_RcvGetCntrlReg(BL_RcvChannelType aRcvCh)
{
    uint32_t rcvCntrl = 0UL;

    switch(aRcvCh) {
    case BL_AMAC_RCV_CH_0:
        rcvCntrl = BL_AMAC_REGS->RCVCTRL;
        break;
    default:
        break;
    }

    return rcvCntrl;
}

static void BL_RcvSetCntrlReg(BL_RcvChannelType aRcvCh,
                              uint32_t aValue)
{
    switch (aRcvCh) {
    case BL_AMAC_RCV_CH_0:
        BL_AMAC_REGS->RCVCTRL = aValue;
        break;
    default:
        break;
    }
}

static void BL_RcvEnable(BL_RcvChannelType aRcvCh)
{
    uint32_t rcvCntrl;
    BL_ChannStateType state;
    uint32_t count = BL_AMAC_RESET_DELAY_CNT;

    rcvCntrl = BL_RcvGetCntrlReg(aRcvCh);
    rcvCntrl |= RCVCTRL_RCVEN_MASK;
    BL_RcvSetCntrlReg(aRcvCh, rcvCntrl);

    do  {
        BL_RcvGetState(aRcvCh, &state);
        if (BL_CHANN_STATE_IDLE == state) {
            break;
        }
        count--;
    } while (count > 0UL);

    if (0UL == count) {
        BL_AMAC_ReportError(BL_BCM_ERR_TIME_OUT, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
    }
}

static void BL_RcvDisable(BL_RcvChannelType aRcvCh)
{
    uint32_t rcvCntrl;
    BL_ChannStateType state;
    uint32_t count = BL_AMAC_RESET_DELAY_CNT;

    rcvCntrl = BL_RcvGetCntrlReg(aRcvCh);
    rcvCntrl &= ~RCVCTRL_RCVEN_MASK;
    BL_RcvSetCntrlReg(aRcvCh, rcvCntrl);

    /**
     * Wait for channel state to become disabled
     */
    do  {
        BL_RcvGetState(aRcvCh, &state);
        if (BL_CHANN_STATE_DISABLED == state) {
            break;
        }
        count--;
    } while (count > 0UL);

    if (0UL == count) {
        BL_AMAC_ReportError(BL_BCM_ERR_TIME_OUT, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
    }
}

static void BL_IsRcvEnabled(BL_RcvChannelType aRcvCh,
                            uint32_t *const aEnabled)
{
    uint32_t rcvCntrl;

    rcvCntrl = BL_RcvGetCntrlReg(aRcvCh);

    if ((rcvCntrl & RCVCTRL_RCVEN_MASK) == RCVCTRL_RCVEN_MASK) {
        *aEnabled = 1UL;
    } else {
        *aEnabled = 0UL;
    }
}

static void BL_RcvSetDescTableAddr(BL_RcvChannelType aRcvCh,
                                   uint32_t aAddrHigh,
                                   uint32_t aAddrLow)
{
    uint32_t enabled;

    BL_IsRcvEnabled(aRcvCh, &enabled);

    if ((enabled == BL_FALSE) && ((aAddrLow & 0xFUL) == 0UL)) {
        switch (aRcvCh) {
            case BL_AMAC_RCV_CH_0:
                BL_AMAC_REGS->RCVADDR_LOW = aAddrLow;
                BL_AMAC_REGS->RCVADDR_HIGH = aAddrHigh;
                break;
            default:
                break;
        }
    }
}

static uint32_t BL_CheckUpdateState(uint32_t volatile *const aState,
                                    uint32_t aCurrState,
                                    uint32_t aNextState)
{
    uint32_t updated;

    if (*aState == aCurrState) {
        *aState = aNextState;
        updated = BL_TRUE;
    } else {
        updated = BL_FALSE;
    }
    return updated;
}

/** @brief This function initializes the Ethernet transmit channel, initialize tx channel information and configures the
    TX channel descriptors.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aChan       Channel ID

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_XMT_INIT_PROC  #BRCM_ARCH_BL_ETHER_XMT_INIT

    @limitations None

    @code{.c}
    Initialise transmit channel information
    set BL_TxChannInfo.isErr to BL_FALSE;
    set BL_TxChannInfo.allocIdx to 0x0;
    set BL_TxChannInfo.deallocIdx to 0x0;
    set BL_TxChannInfo.queueIdx to 0x0;
    set BL_TxChannInfo.dequeueIdx to 0x0;

    Initialise Transmit channel packet buffer information
    for i = 0UL to i less than TXCH_BUFF_COUNT:
        set BL_TxChPktBuffInfo[i].state to BL_PKT_BUFF_STATE_FREE;
        set BL_TxChPktBuffInfo[i].pktBuff to pointer of TxChPktBuf[i];
        increment i by 1;

    Initialise Transmit channel DMA descriptor table
    for i = 0UL to i less than BL_DESC_TABLE_DESC_CNT:
        set TxDmaDescTbl[i].flags to 0x3; // Reserved CRC flag
        set TxDmaDescTbl[i].eot to 0x1; // Mark as last descriptor in descriptor table
        set TxDmaDescTbl[i].intOnComp to 0x0; // Disable descriptor completion intexitupt
        clear TxDmaDescTbl[i].sof to 0x0; // Clear start of frame flag
        clear TxDmaDescTbl[i].eof to 0x0; // Clear end of frame flag
        clear TxDmaDescTbl[i].bufCount to 0x0;
        clear TxDmaDescTbl[i].dataPtrLow to 0x0;
        clear TxDmaDescTbl[i].dataPtrHigh to 0x0;
        data memory synchronisation barrier instruction;
        instruction synchronisation barrier;
        increment i by 1;

    disable Transmit channel;
    install Transmit DMA descriptor table;
    disable Transmit channel parity check;
    clear Transmit channel intexitupt status;
    enable Transmit channel;

    set BL_TxChannInfo.state to BL_ETHER_STATE_ACTIVE;
    @endcode
*/
static void BL_XmtInit(BL_ETHER_HwIDType aID,
                       BL_ETHER_ChanIDType aChan)
{
    BL_DmaDescType *dmaDescTbl = BL_TxChannInfo[aChan].dmaDescTbl;

    BL_XmtDisable(aChan);

    /* Set Descriptor table addr */
    BL_XmtSetDescTableAddr(aChan,
                        BL_DMA_DESC_DATAPTRHIGH_ZERO,
                        (uint32_t)dmaDescTbl);

    BL_XmtSetParityDisable(aChan, BL_TX_PARITY_DIS);
    BL_XmtClearInterruptStatus(aChan);
    BL_XmtEnable(aChan);
}

static void BL_XmtChannInfoInit(BL_ETHER_HwIDType aID,
                                BL_ETHER_ChanIDType aChan)
{
    uint32_t i;

    BL_TxChannInfo[aChan].isErr = BL_FALSE;
    BL_TxChannInfo[aChan].allocIdx = 0UL;
    BL_TxChannInfo[aChan].deallocIdx = 0UL;
    BL_TxChannInfo[aChan].queueIdx = 0UL;
    BL_TxChannInfo[aChan].dequeueIdx = 0UL;

    BL_BCM_MemSet((void * const)BL_TxChannInfo[aChan].dmaDescTbl, 0x0,
            BL_DESC_TABLE_SIZE);

    /* Initialise the BL_TxChannInfo */
    for (i = 0UL; i < BL_TxChannInfo[aChan].pktBuffCnt; i++) {
        BL_TxChannInfo[aChan].pktBuffInfo[i].state = BL_PKT_BUFF_STATE_FREE;
        BL_TxChannInfo[aChan].pktBuffInfo[i].pktBuff =
                                                (BL_TxChPktBuf[aChan])[i].pktBuff;
    }

    for (i = 0UL; i <= BL_TxChannInfo[aChan].pktBuffCnt; i++) {
        BL_SetDescFields(BL_TxChannInfo[aChan].dmaDescTbl[i],
                        BL_TXDMA_FLAG_CRC_MASK,
                        BL_DMA_DESC_EOT_EN,
                        BL_DMA_DESC_IOC_DIS,
                        BL_DMA_DESC_EOF_DIS,
                        BL_DMA_DESC_SOF_DIS,
                        BL_DMA_DESC_BUFCOUNT_ZERO,
                        BL_DMA_DESC_DATAPTRLOW_ZERO,
                        BL_DMA_DESC_DATAPTRHIGH_ZERO);
    }
}

static void BL_RcvInit(BL_ETHER_HwIDType aID,
                       BL_ETHER_ChanIDType aChan)
{
    uint32_t regVal = 0UL;
    const BL_DmaDescType *dmaDescTbl = BL_RxChannInfo[aChan].dmaDescTbl;

    BL_RcvDisable(aChan);

    /* Set Descriptor table addr */
    BL_RcvSetDescTableAddr(aChan,
                        BL_DMA_DESC_DATAPTRHIGH_ZERO,
                        (uint32_t)dmaDescTbl);

    /* Reciever control configuration:
     * -> disable parity
     * -> set rcvoffset = 4
     * -> clear overflow continue
     */
    regVal = BL_AMAC_REGS->RCVCTRL;
    regVal &= ~RCVCTRL_RCVOFFSET_MASK;
    regVal |= (0x4UL << RCVCTRL_RCVOFFSET_SHIFT);
    regVal |= RCVCTRL_RXPARITYCHECKDIS_MASK;
    regVal &= ~RCVCTRL_OFLOWCONTINUE_MASK;
    BL_AMAC_REGS->RCVCTRL = regVal;
    /* Receiver atleast 1 frame before generating lazy interrupt */
    BL_AMAC_REGS->INTRCVLAZY = 0x1UL << INTRCVLAZY_FRAMECOUNT_SHIFT;

    BL_RcvClearInterruptStatus(aChan);
    BL_RcvEnable(aChan);

    /* Update RCVPTR to point to EOT descriptor. Keep a room of 1 DMA
       descriptor. */
    BL_RcvSetLastDesc(aChan, BL_RX_DESC_OFFSET(BL_RX_BUFF_COUNT - 1UL));
}

static void BL_RcvChannInfoInit(BL_ETHER_HwIDType aID,
                                BL_ETHER_ChanIDType aChan)
{
    uint32_t j;
    uint32_t minCnt;
    uint32_t queueIdx;

    BL_RxChannInfo[aChan].isErr = BL_FALSE;
    BL_RxChannInfo[aChan].allocIdx = 0UL;
    BL_RxChannInfo[aChan].dequeueIdx = 0UL;

    minCnt = (BL_RX_BUFF_COUNT - 1UL);

    /* Initialise the RxChannETHER_Dev */
    for (j = 0UL; j < BL_RX_BUFF_COUNT; j++) {
        BL_RxChannInfo[aChan].pktBuffInfo[j].pktBuff = &(BL_RxPkt[j]);
        BL_RxChannInfo[aChan].pktBuffInfo[j].state = BL_PKT_BUFF_STATE_FREE;
        if (minCnt > j) {
            BL_RxChannInfo[aChan].pktBuffInfo[j].state = BL_PKT_BUFF_STATE_QUEUED;

            /* Queue buffer index to buffIdxQ */
            BL_RxChannInfo[aChan].buffIdxQ[j] = j;
        }
    }

    /* queueIdx is the index to queue next buffer to buffIdxQ */
    queueIdx = minCnt;
    queueIdx %= BL_RX_BUFF_COUNT;
    BL_RxChannInfo[aChan].queueIdx = queueIdx;
}

static void BL_RcvChannErrHandler(BL_ETHER_HwIDType aID,
                                  BL_ETHER_ChanIDType aChan)
{
    /* Re-initialise channel info and reset channel */
    BL_RcvChannInfoInit(aID, aChan);
    BL_RcvInit(aID, aChan);
    BL_UNIMAC_EnableRxReset(0UL);
}

static void BL_XmtChannErrHandler(BL_ETHER_HwIDType aID,
                                  BL_ETHER_ChanIDType aChan)
{
    /* Re-initialise channel info and reset channel */
    BL_XmtChannInfoInit(aID, aChan);
    BL_XmtReset(aChan);
}

static void BL_DequeueRcvCompPkts(BL_ETHER_ChanIDType aChan)
{
    uint32_t dequeueIdx;
    uint32_t buffIdx;
    uint32_t volatile *state;
    uint32_t currDesc;
    uint32_t currDescIdx;
    uint32_t isUpdated;

    dequeueIdx = BL_RxChannInfo[aChan].dequeueIdx;

    currDesc = BL_RcvGetCurrDesc(aChan);
    currDescIdx = (currDesc/BL_DMA_DESC_SIZE);

    while (currDescIdx != dequeueIdx) {
        buffIdx = BL_RxChannInfo[aChan].buffIdxQ[dequeueIdx];
        state = &(BL_RxChannInfo[aChan].pktBuffInfo[buffIdx].state);
        isUpdated = BL_CheckUpdateState(state, BL_PKT_BUFF_STATE_QUEUED,
                                        BL_PKT_BUFF_STATE_DEQUEUED);
        if (BL_FALSE == isUpdated) {
            BL_AMAC_ReportError(BL_BCM_ERR_INVAL_BUF_STATE, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
            break;
        }
        dequeueIdx = BL_ModIncIdx(dequeueIdx, BL_RX_BUFF_COUNT);
    }
    BL_RxChannInfo[aChan].dequeueIdx = dequeueIdx;
}

static void BL_DequeueXmtCompPkts(BL_ETHER_ChanIDType aChan)
{
    uint32_t dequeueIdx;
    uint32_t buffIdx;
    uint32_t isUpdated;
    uint32_t volatile *state;
    uint32_t currDesc;
    uint32_t currDescIdx;

    if (aChan < BL_ETHER_TX_CHAN_CNT) {
        dequeueIdx = BL_TxChannInfo[aChan].dequeueIdx;
        currDesc = BL_XmtGetCurrDesc(aChan);
        currDescIdx = (currDesc/BL_DMA_DESC_SIZE);

        while (currDescIdx != dequeueIdx) {
            buffIdx = BL_TxChannInfo[aChan].buffIdxQ[dequeueIdx];
            state = &(BL_TxChannInfo[aChan].pktBuffInfo[buffIdx].state);
            isUpdated = BL_CheckUpdateState(state, BL_PKT_BUFF_STATE_QUEUED,
                    BL_PKT_BUFF_STATE_DEQUEUED);
            if (BL_FALSE == isUpdated) {
                BL_AMAC_ReportError(BL_BCM_ERR_INVAL_BUF_STATE, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
                break;
            }
            dequeueIdx = BL_ModIncIdx(dequeueIdx, BL_TX_CHAN_DESC_CNT(aChan));
        }
        BL_TxChannInfo[aChan].dequeueIdx = dequeueIdx;
    }
}

static void BL_RcvDMAErrIRQHandler(BL_ETHER_ChanIDType aChan)
{
    BL_ChannErrType channErr;

    BL_RcvGetErr(aChan, &channErr);
    if ((channErr <= BL_CHAN_ERR_MAX) && (channErr >= BL_CHAN_ERR_MIN)) {
        /* 1. Reset UniMAC
         * 2. Set isErr flag
         */
        BL_UNIMAC_EnableRxReset(1UL);
        BL_RxChannInfo[BL_AMAC_RCV_CH_0].isErr = BL_TRUE;
    }
}

static void BL_XmtDMAErrIRQHandler(BL_ETHER_ChanIDType aChan)
{
    BL_ChannErrType channErr;

    BL_XmtGetErr(aChan, &channErr);
    if ((channErr <= BL_CHAN_ERR_MAX) && (channErr >= BL_CHAN_ERR_MIN)) {
        /* Set isErr flag */
        BL_TxChannInfo[aChan].isErr = BL_TRUE;
    }
}

/** @brief This function is to handle the processing of DMA errors.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIntStatus      Interrupt status

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_DMA_ERR_IRQ_HANDLER_PROC  #BRCM_ARCH_BL_ETHER_DMA_ERR_IRQ_HANDLER

    @limitations None

    @code{.c}
    retreive transmit channel DMA error from hardware;
    if transmit channel DMA error occured:
        set BL_TxChannInfo.isErr to BL_TRUE;

    retreive receive channel DMA error from hardware;
    if receive channel DMA error occured:
        set BL_RxChannInfo.isErr to BL_TRUE;
        reset UniMAC receive logic;
    @endcode
*/
static void BL_DMAErrIRQHandler(uint32_t aIntStatus)
{
    uint32_t i;

    /* check for DMA errors
     * In case of DMA error, reset the free packets and reset
     * the channel
     */
    if ((aIntStatus & BL_DMA_DESC_ERR_MASK) != 0UL) {
        BL_AMAC_ReportError(BL_BCM_ERR_CUSTOM, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);

        /* TODO: use loop with max channel */
        for (i = 0UL; i < BL_ETHER_TX_CHAN_CNT; i++) {
            BL_XmtDMAErrIRQHandler(i);
        }
        BL_RcvDMAErrIRQHandler(BL_AMAC_RCV_CH_0);
    }
}

/** @brief This function is to handle the processing of receive channel errors.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIntStatus      Interrupt status

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_RCV_ERR_IRQ_HANDLER_PROC  #BRCM_ARCH_BL_ETHER_RCV_ERR_IRQ_HANDLER

    @limitations None

    @code{.c}
    if receive descriptor underflow bit is set in aIntStatus
        or receive fifo overflow bit is set in aIntStatus:
            reset UniMAC receive logic;
            set BL_RxChannInfo.isErr to BL_TRUE;
    @endcode
*/
static void BL_RcvErrIRQHandler(uint32_t aIntStatus)
{
    uint32_t mask = (INTSTATUS_RCVDESCUF_MASK | INTSTATUS_RCVFIFOOF_MASK);

    /* Receive DMA descriptor underflow error
     * Reset & deinitialise the receive channel.
     */
    if ((aIntStatus & mask) != 0UL) {
        /* 1. Reset UniMAC
         * 2. Set isErr flag
         */
        BL_UNIMAC_EnableRxReset(1UL);
        BL_RxChannInfo[BL_AMAC_RCV_CH_0].isErr = BL_TRUE;
    }
}

/** @brief This function is to handle the processing of transmit channel errors.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIntStatus      Interrupt status

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_XMT_ERR_IRQ_HANDLER_PROC  #BRCM_ARCH_BL_ETHER_XMT_ERR_IRQ_HANDLER

    @limitations None

    @code{.c}
    if transmit fifo overflow occured:
        report error "Tx fifo overflow"
        set BL_TxChannInfo.isErr to BL_TRUE;
    @endcode
*/
static void BL_XmtErrIRQHandler(uint32_t aIntStatus)
{
    uint32_t i;

    /* transmit fifo overflow?
    */
    if ((aIntStatus & INTSTATUS_XMTFIFOUF_MASK) != 0UL) {
        BL_AMAC_ReportError(BL_BCM_ERR_CUSTOM, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
        /* Set isErr flag */
        for (i = 0UL; i < BL_ETHER_TX_CHAN_CNT; i++) {
            BL_TxChannInfo[i].isErr = BL_TRUE;
        }
    }
}

/** @brief This function is to handle the processing of received packet.

    This function dequeues the already received packets from DMA queue.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIntStatus      Interrupt status

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_RCV_COMP_IRQ_HANDLER_PROC  #BRCM_ARCH_BL_ETHER_RCV_COMP_IRQ_HANDLER

    @limitations None

    @code{.c}
    if receive interrupt bit is not set in aIntStatus:
        goto exit;

    get dequeueIdx from BL_RxChannInfo[aChan].dequeueIdx;
    retreive currDescIdx (current descriptor index) from hardware;

    while currDescIdx not equal to dequeueIdx:
        retreive buffIdx from BL_RxChannInfo.buffIdxQ[dequeueIdx];
        state = &(BL_RxChannInfo[aChan].pktBuffInfo[buffIdx].state);
        if BL_RxPktBuffInfo[buffIdx].state equal to BL_PKT_BUFF_STATE_QUEUED:
            update BL_RxPktBuffInfo[buffIdx].state to BL_PKT_BUFF_STATE_DEQUEUED;
        else:
            report error "invalid rx buff state"
            break;
        module increment dequeueIdx w.r.t BL_RX_BUFF_COUNT;

    set BL_RxChannInfo[aChan].dequeueIdx to dequeueIdx;

    exit:
    @endcode
*/
static void BL_RcvCompIRQHandler(uint32_t aIntStatus)
{
    /* reciever interrupt
     * Process the RX packets
     */
    if ((aIntStatus & INTSTATUS_RCVINT_MASK) != 0UL) {
        BL_DequeueRcvCompPkts(BL_AMAC_RCV_CH_0);
    }
}

/** @brief This function is to handle the processing of transmitted packet.

    This function dequeues the already transmitted packets from DMA queue.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIntStatus      Interrupt status

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_XMT_COMP_IRQ_HANDLER_PROC  #BRCM_ARCH_BL_ETHER_XMT_COMP_IRQ_HANDLER

    @limitations None

    @code{.c}
    identify transmit channel for which transmit completed;
    set dequeueIdx to BL_TxChannInfo.dequeueIdx;
    retreive currDescIdx (current descriptor index) from hardware;

    while currDescIdx not equal to dequeueIdx:
        retreive buffIdx from BL_TxChannInfo.buffIdxQ[dequeueIdx];
        if TxPktBuffInfo[buffIdx].state equal to BL_PKT_BUFF_STATE_QUEUED:
            set TxPktBuffInfo[buffIdx].state to BL_PKT_BUFF_STATE_DEQUEUED;
        else:
            report error "Invalid buffer state";
            break;
        modulo increment dequeueIdx w.r.t BL_TX_CHAN_DESC_CNT;
    @endcode
*/
static void BL_XmtCompIRQHandler(uint32_t aIntStatus)
{
    /* TX complete interrupt
     * free TX packets and give callback
     */
    if ((aIntStatus & (1UL << (INTSTATUS_XMTINT_0_SHIFT))) != 0UL) {
        BL_DequeueXmtCompPkts(BL_AMAC_XMT_CH_0);
    }
    if ((aIntStatus & (1UL << (INTSTATUS_XMTINT_1_SHIFT))) != 0UL) {
        BL_DequeueXmtCompPkts(BL_AMAC_XMT_CH_1);
    }
    if ((aIntStatus & (1UL << (INTSTATUS_XMTINT_2_SHIFT))) != 0UL) {
        BL_DequeueXmtCompPkts(BL_AMAC_XMT_CH_2);
    }
    if ((aIntStatus & (1UL << (INTSTATUS_XMTINT_3_SHIFT))) != 0UL) {
        BL_DequeueXmtCompPkts(BL_AMAC_XMT_CH_3);
    }
}

/** @brief This function is to process transmitted packets. This function move dequeued packets to free state.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aChan   Transmit channel ID

    @retval     #BL_BCM_ERR_OK                 Successfully processed the transmitted packet
    @retval     #BL_BCM_ERR_EAGAIN             Nothing to process
    @retval     #BL_BCM_ERR_INVAL_BUF_STATE    TX buffer in invalid state

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_PROCESS_XMTD_PKTS_PROC  #BRCM_ARCH_BL_ETHER_PROCESS_XMTD_PKTS

    @limitations None

    @code{.c}
    set dequeueIdx to BL_TxChannInfo.dequeueIdx;
    set deallocIdx to BL_TxChannInfo.deallocIdx;

    if deallocIdx equals to dequeueIdx:
        set retVal to BL_BCM_ERR_EAGAIN;
        goto exit;

    retreive buffIdx from BL_TxChannInfo.buffIdxQ[deallocIdx];

    if BL_PKT_BUFF_STATE_DEQUEUED not equal to TxPktBuffInfo[buffIdx].state:
        set retVal to BL_BCM_ERR_INVAL_BUF_STATE;
        goto exit;

    if TxPktBuffInfo[buffIdx].state equals to BL_PKT_BUFF_STATE_DEQUEUED:
        update TxPktBuffInfo[buffIdx].state equals to BL_PKT_BUFF_STATE_FREE;

    modulo increment BL_TxChannInfo.deallocIdx w.r.t BL_TX_CHAN_DESC_CNT

    set retVal to BL_BCM_ERR_OK;
    exit:
        return retVal;
    @endcode
*/
static int32_t BL_ProcessXmtdPkts(BL_ETHER_ChanIDType aChan)
{
    int32_t retVal;
    uint32_t dequeueIdx;
    uint32_t deallocIdx;
    uint32_t buffIdx;
    uint32_t volatile *state;
    uint32_t isUpdated;

    dequeueIdx = BL_TxChannInfo[aChan].dequeueIdx;
    deallocIdx = BL_TxChannInfo[aChan].deallocIdx;

    if (deallocIdx == dequeueIdx) {
        retVal = BL_BCM_ERR_EAGAIN;
        goto err;
    }

    buffIdx = BL_TxChannInfo[aChan].buffIdxQ[deallocIdx];

    if (BL_PKT_BUFF_STATE_DEQUEUED !=
        BL_TxChannInfo[aChan].pktBuffInfo[buffIdx].state) {
        retVal = BL_BCM_ERR_INVAL_BUF_STATE;
        goto err;
    }

    state = &(BL_TxChannInfo[aChan].pktBuffInfo[buffIdx].state);
    isUpdated = BL_CheckUpdateState(state, BL_PKT_BUFF_STATE_DEQUEUED,
            BL_PKT_BUFF_STATE_FREE);
    if (BL_FALSE == isUpdated) {
        retVal = BL_BCM_ERR_INVAL_BUF_STATE;
        BL_AMAC_ReportError(retVal, BL_ETHER_HW_ID_0, 0UL, 0UL, 0UL, __LINE__);
        goto err;
    }

    BL_TxChannInfo[aChan].deallocIdx =
                    BL_ModIncIdx(deallocIdx, BL_TX_CHAN_DESC_CNT(aChan));
    retVal = BL_BCM_ERR_OK;
err:
    return retVal;
}

/** @brief This function is to process received packets. This function raises callback for received packet to the client
    and queues back the free packet to DMA queue.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID     Controller ID
    @param[in]      aChan   Transmit channel ID

    @retval     #BL_BCM_ERR_OK                 Successfully processed the received packet
    @retval     #BL_BCM_ERR_EAGAIN             Nothing to process
    @retval     #BL_BCM_ERR_INVAL_BUF_STATE    RX buffer in invalid state

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_PROCESS_RCVD_PKTS_PROC  #BRCM_ARCH_BL_ETHER_PROCESS_RCVD_PKTS

    @limitations None

    @code{.c}
    set allocIdx to BL_RxChannInfo.allocIdx;
    set dequeueIdx to BL_RxChannInfo.dequeueIdx;

    if dequeueIdx equal to allocIdx:
        set retVal to BL_BCM_ERR_EAGAIN;
        goto exit;

    retreive buffIdx from BL_RxChannInfo.buffIdxQ[allocIdx];

    if BL_RxPktBuffInfo[buffIdx].state not equal to BL_PKT_BUFF_STATE_DEQUEUED:
        set retVal to BL_BCM_ERR_INVAL_BUF_STATE;
        goto exit;

    set rxPkt to BL_RxPktBuffInfo[buffIdx].pktBuff;
    clean data cache for rxPkt;
    set rxPktInfo to rxPkt information populated by hardware;
    if rxPktInfo.crcErr equal to 0x0
        and if rxPktInfo.overSz equal to 0x0
        and if rxPktInfo.pktOvflw equal to 0x0
        and if rxPktInfo.ctfErr equal to 0x0:
            call ETHER_RxInd();
    else:
        report error "Error in rx packet received"

    modulo increment BL_RxChannInfo.allocIdx w.r.t BL_RX_BUFF_COUNT;

    set queueIdx to BL_RxChannInfo[aChan].queueIdx;

    retreive lastDescIdx from hardware;

    set BL_RxPktBuffInfo[queueIdx].state to BL_PKT_BUFF_STATE_QUEUED;
    set BL_RxChannInfo.buffIdxQ[queueIdx] to queueIdx;

    Queue the packet buffer to DMA
    modulo increment lastDescIdx w.r.t BL_RX_BUFF_COUNT
    update lastDescIdx to hardware;

    modulo increment BL_RxChannInfo.queueIdx w.r.t BL_RX_BUFF_COUNT;

    set retVal to BL_BCM_ERR_OK;
    exit:
        return retVal;
    @endcode
*/
static int32_t BL_ProcessRcvdPkts(BL_ETHER_HwIDType aID,
                                  BL_ETHER_ChanIDType aChan)
{
    int32_t retVal;
    uint32_t queueIdx;
    uint32_t dequeueIdx;
    uint32_t allocIdx;
    uint32_t buffIdx;
    uint32_t lastDesc;
    uint32_t lastDescIdx;
    BL_RxPktType *rxPkt;
    BL_AMAC_RxPktInfoType rxPktInfo;

    allocIdx = BL_RxChannInfo[aChan].allocIdx;
    dequeueIdx = BL_RxChannInfo[aChan].dequeueIdx;

    if (dequeueIdx == allocIdx) {
        retVal = BL_BCM_ERR_EAGAIN;
        goto err;
    }

    buffIdx = BL_RxChannInfo[aChan].buffIdxQ[allocIdx];

    if (BL_PKT_BUFF_STATE_DEQUEUED !=
        BL_RxChannInfo[aChan].pktBuffInfo[buffIdx].state) {
        /* TODO: Add asserts/crash */
        retVal = BL_BCM_ERR_INVAL_BUF_STATE;
        goto err;
    }
    rxPkt = BL_RxChannInfo[aChan].pktBuffInfo[buffIdx].pktBuff;
    CORTEX_MX_DCacheInvalRange((uint32_t)rxPkt, sizeof(BL_RxPktType));
    BL_BCM_MemCpy((void *const)&rxPktInfo,
            (void *const)&rxPkt->rxPktInfo,
            sizeof(rxPktInfo));

    if ((0UL == rxPktInfo.crcErr)
        && (0UL == rxPktInfo.overSz)
        && (0UL == rxPktInfo.pktOvflw)
        && (0UL == rxPktInfo.ctfErr)) {
        /* Raise RX receive indication callback with length - CRC length*/
        BL_ETHER_RxInd(aID, &(rxPkt->pktBuff[0]),
                    (rxPkt->rxPktInfo.frameLen - 4U));
    } else {
        BL_AMAC_ReportError(BL_BCM_ERR_DATA_INTEG, aID, 0UL, 0UL, 0UL, __LINE__);
    }

    /* Increment & update allocIdx*/
    BL_RxChannInfo[aChan].allocIdx = BL_ModIncIdx(allocIdx, BL_RX_BUFF_COUNT);

    queueIdx = BL_RxChannInfo[aChan].queueIdx;

    lastDesc = BL_RcvGetLastDesc(aChan);
    lastDescIdx = lastDesc/sizeof(BL_DmaDescType);

    BL_RxChannInfo[aChan].pktBuffInfo[queueIdx].state = BL_PKT_BUFF_STATE_QUEUED;
    BL_RxChannInfo[aChan].buffIdxQ[queueIdx] = queueIdx;

    /* Queue the packet buffer to DMA */
    lastDescIdx = BL_ModIncIdx(lastDescIdx, BL_RX_BUFF_COUNT);
    BL_RcvSetLastDesc(aChan, BL_RX_DESC_OFFSET(lastDescIdx));

    /* Increment & upate queueIdx */
    BL_RxChannInfo[aChan].queueIdx = BL_ModIncIdx(queueIdx, BL_RX_BUFF_COUNT);

    retVal = BL_BCM_ERR_OK;
err:
    return retVal;
}

/** @brief This function is to wait till the completion of transmit packet and once the packet is transmited, dequeue it
    from DMA queue and update the packet state to free.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aChan   Transmit channel ID

    @retval     #BL_BCM_ERR_OK         Successfully processed the transmitted packet
    @retval     #BL_BCM_ERR_CUSTOM     DMA or XMT error
    @retval     #BL_BCM_ERR_TIME_OUT   Timeout

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_XMT_WAIT_TX_DONE_PROC  #BRCM_ARCH_BL_ETHER_XMT_WAIT_TX_DONE

    @limitations None

    @code{.c}
    set count to 1000;

    while count greater than 0UL:
        retreive interrupt status;

        if transmit complete bit set in interrupt status:
            clear transmit complete bit in interrupt status register;
            BL_XmtCompIRQHandler(intStatus);

        if dma error bit set in interrupt status:
            clear dma error bit in interrupt status register;
            BL_DMAErrIRQHandler(intStatus);
            set retVal to BL_BCM_ERR_CUSTOM;

        if fifo error bit set in interrupt status:
            clear fifo error bit in interrupt status register;
            BL_XmtErrIRQHandler(intStatus);
            set retVal to BL_BCM_ERR_CUSTOM;

        if  transmit complete bit set in interrupt status
            or dma error bit set in interrupt status
            or fifo error bit set in interrupt status:
            break;

        decrement count by 1;
        add a delay of 1000 micro seconds;

    if count equal to 0UL:
        set retVal to BL_BCM_ERR_TIME_OUT;
    else:
        set retVal to BL_ProcessXmtdPkts(aChan);

    return retVal;
    @endcode
*/
static int32_t BL_XmtWaitTxDone(BL_ETHER_ChanIDType aChan)
{

    uint32_t intStatus;
    uint32_t txDone;
    uint32_t dmaErr;
    uint32_t fifoErr;
    uint32_t count = BL_AMAC_TX_DELAY_COUNT;
    int32_t retVal = BL_BCM_ERR_OK;

    while (count > 0UL) {
        intStatus = BL_AMAC_REGS->INTSTATUS;
        txDone = intStatus & (1UL << INTSTATUS_XMTINT_0_SHIFT);
        dmaErr = intStatus & BL_DMA_DESC_ERR_MASK;
        fifoErr = intStatus & INTSTATUS_XMTFIFOUF_MASK;

        if (txDone != 0UL) {
            BL_AMAC_REGS->INTSTATUS = txDone;
            BL_XmtCompIRQHandler(intStatus);
        }
        if (dmaErr != 0UL) {
            BL_AMAC_REGS->INTSTATUS = dmaErr;
            BL_DMAErrIRQHandler(intStatus);
            retVal = BL_BCM_ERR_CUSTOM;
        }
        if (fifoErr != 0UL) {
            BL_AMAC_REGS->INTSTATUS = fifoErr;
            BL_XmtErrIRQHandler(intStatus);
            retVal = BL_BCM_ERR_CUSTOM;
        }
        if ((txDone != 0UL) || (dmaErr != 0UL) ||
                (fifoErr != 0UL)) {
            break;
        }
        count--;
        BCM_DelayUs(BL_AMAC_TX_WAIT_DELAY_US);
    };

    if (count == 0UL) {
        retVal = BL_BCM_ERR_TIME_OUT;
    } else {
        retVal = BL_ProcessXmtdPkts(aChan);
    }

    return retVal;
}

/** @brief This function initializes the Ethernet receive channel, initialize rx channel information and configures the
    RX channel descriptors.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aChan       Channel ID

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_RCV_INIT_PROC  #BRCM_ARCH_BL_ETHER_RCV_INIT

    @limitations None

    @code{.c}
    Initialise transmit channel information
    set BL_RxChannInfo.isErr to BL_FALSE;
    set BL_RxChannInfo.allocIdx to 0x0;
    set BL_RxChannInfo.dequeueIdx to 0x0;

    Initialise Receive channel packet buffer information
    for i = 0UL to i greater than BL_RX_BUFF_COUNT:
        if i less than BL_RX_BUFF_COUNT - 0x1:
            set RxChPktBuffInfo[i].state to BL_PKT_BUFF_STATE_QUEUED;
        else:
            set RxChPktBuffInfo[i].state to BL_PKT_BUFF_STATE_FREE;
        set RxChPktBuffInfo[i].pktBuff to pointer of BL_RxPkt[i];
        increment i by 1;

    set BL_RxChannInfo.queueIdx to BL_RX_BUFF_COUNT - 0x1;

    disable Receive channel;
    install Receive DMA descriptor table;
    disable Receive channel parity check;
    clear Receive channel intexitupt status;
    enable Receive channel;

    update RCVPTR(Hardware pointer) to offset of BL_RxDmaDescTbl(BL_RX_BUFF_COUNT - 1UL);

    set BL_RxChannInfo.state to BL_ETHER_STATE_ACTIVE;
    @endcode
*/
static void BL_ETHER_RcvInit(BL_ETHER_HwIDType aID,
                             BL_ETHER_ChanIDType aChan)
{
    if ((BL_ETHER_HW_ID_MAX <= aID) || (BL_ETHER_RX_CHAN_CNT <= aChan)) {
        goto err;
    }

    if (BL_ETHER_STATE_UNINIT != BL_RxChannInfo[aChan].state) {
        goto err;
    }

    BL_RcvChannInfoInit(aID, aChan);
    BL_RcvInit(aID, aChan);
    BL_RxChannInfo[aChan].state = BL_ETHER_STATE_ACTIVE;

err:
    return ;
}

/**
    @code{.c}
    if aID is not less than BL_ETHER_HW_ID_MAX
        or if aChan is not less than BL_ETHER_TX_CHAN_CNT
        or if aBufIdx is NULL
        or if aBuf is NULL
        or if aLenInOut is NULL
        or if aLenInOut requested is 0x0:
            set retVal to BL_BCM_ERR_INVAL_PARAMS;
            goto exit;

    if BL_TxChannInfo.state is not BL_ETHER_STATE_ACTIVE:
        set retVal to BL_BCM_ERR_INVAL_STATE;
        goto exit;


    // Search for a buffer with BL_PKT_BUFF_STATE_FREE
    // allocIdx point to probable to be allocated buffer index
    for i = BL_TxChannInfo.allocIdx to i not equal to BL_TxChannInfo.allocIdx:
        if TxPktBuffInfo[i].state is equal to BL_PKT_BUFF_STATE_FREE:
            set TxPktBuffInfo[i].state to BL_PKT_BUFF_STATE_ALLOC;
            set isFoundFreeBuf to BL_TRUE;
            break;

        modulo increment i w.r.t TXCH_BUFF_COUNT;

    if isFoundFreeBuf is BL_TRUE:
        Modulo increment BL_TxChannInfo.allocIdx w.r.t TXCH_BUFF_COUNT;

        // Free buffer allocated
        set *aBufIdx to i;
        set *aBuf to TxPktBuffInfo[i].pktBuff;

        if aLenInOut requested greater than 1516:
            set aLenInOut to 1516;

        set retVal to BL_BCM_ERR_OK;
    else:
        // No free buffer
        set *aBuf to NULL;
        set *aLenInOut to 0x0;
        set retVal to BL_BCM_ERR_NOMEM;

    exit:
        return retVal;
    @endcode
*/
int32_t BL_ETHER_GetTxBuffer(BL_ETHER_HwIDType aID,
                             BL_ETHER_ChanIDType aChan,
                             uint32_t *const aBufIdx,
                             uint8_t **const aBuf,
                             uint32_t *const aLenInOut)
{
    int32_t retVal;
    uint32_t i;
    uint32_t allocIdx;
    uint32_t volatile *state;
    uint32_t isUpdated;

    if ((BL_ETHER_HW_ID_MAX <= aID) || (BL_ETHER_TX_CHAN_CNT <= aChan)
        || (NULL == aBufIdx) || (NULL == aBuf) || (NULL == aLenInOut)
        || (0UL == *aLenInOut)) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (BL_ETHER_STATE_ACTIVE != BL_TxChannInfo[aChan].state) {
        retVal = BL_BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* allocIdx point to probable to be allocated buffer index */
    allocIdx = BL_TxChannInfo[aChan].allocIdx;

    /* Search for free buffer */
    i = allocIdx;
    do {
        state = &(BL_TxChannInfo[aChan].pktBuffInfo[i].state);
        isUpdated = BL_CheckUpdateState(state, BL_PKT_BUFF_STATE_FREE,
                                            BL_PKT_BUFF_STATE_ALLOC);
        if (BL_TRUE == isUpdated) {
            break;
        }
        i = BL_ModIncIdx(i, BL_TxChannInfo[aChan].pktBuffCnt);
    } while (i != allocIdx);

    if (BL_TRUE == isUpdated) {
        /* Increment the allocIdx */
        BL_TxChannInfo[aChan].allocIdx =
                    BL_ModIncIdx(allocIdx, BL_TxChannInfo[aChan].pktBuffCnt);

        /* Free buffer allocated */
        *aBufIdx = i;
        *aBuf = BL_TxChannInfo[aChan].pktBuffInfo[i].pktBuff;

        /* If packet buffer length requested is more than
           BL_ETH_FRM_SIZE_NO_CRC updated length with
           BL_ETH_FRM_SIZE_NO_CRC. */
        if (BL_ETH_FRM_SIZE_NO_CRC <= *aLenInOut) {
            *aLenInOut = BL_ETH_FRM_SIZE_NO_CRC;
        }
        retVal = BL_BCM_ERR_OK;
    } else {
        /* No free buffer */
        *aBufIdx = BL_INVAL_BUFF_IDX;
        *aBuf = NULL;
        *aLenInOut = 0UL;
        retVal = BL_BCM_ERR_NOMEM;
    }
err:
    return retVal;
}

/**
    @code{.c}
    if aID is not less than BL_ETHER_HW_ID_MAX
        or if aChan is not less than BL_ETHER_TX_CHAN_CNT
        or if aLen is greater than 1516
        or if aBufIdx is greater than TXCH_BUFF_COUNT:
            set retVal to BL_BCM_ERR_INVAL_PARAMS;
            goto exit;

    if BL_TxChannInfo.state is not BL_ETHER_STATE_ACTIVE:
        set retVal to BL_BCM_ERR_INVAL_STATE;
        goto exit;

    if TxPktBuffInfo[aBufIdx].state is not BL_PKT_BUFF_STATE_ALLOC:
        set retVal to BL_BCM_ERR_INVAL_STATE;
        goto exit;

    // If aLen equal to zero, just free the packet
    if aLen is equal to 0x0:
        set TxPktBuffInfo[aBufIdx].state to BL_PKT_BUFF_STATE_FREE;
        set retVal to BL_BCM_ERR_OK;
        goto exit;

    // Check for room in Transmit DMA descriptor table
    retreive lastDescIdx (last descriptor index)
    transmitted from hardware;
    retreive currDescIdx (current descriptor index)
    on-going transfer from hardware;

    modulo increment lastDescIdx w.r.t BL_TX_CHAN_DESC_CNT;
    if lastDescIdx equals to currDescIdx:
        Set retVal to BL_BCM_ERR_BUSY;
        goto exit;

    // Update state of the buffer to queued
    set TxPktBuffInfo[aBufIdx].state to BL_PKT_BUFF_STATE_QUEUED;

    if aLen less than BL_ETH_MIN_FRM_SIZE_NO_CRC:
        set pktLen to BL_ETH_MIN_FRM_SIZE_NO_CRC;
    else:
        set pktLen to aLen;

    // Get the queueIdx for buffer index Q
    get queueIdx from BL_TxChannInfo[aChan].queueIdx;
    modulo increment queueIdx w.r.t BL_TX_CHAN_DESC_CNT;

    enqueue aBufIdx to BL_TxChannInfo.buffIdxQ[queueIdx];

    retreive the lastDescIdx(index next to last descriptor) queued to hardware;
    update TxDmaDescTbl[lastDescIdx] for pktLen,pktBuff;
    enable interrupt on completion for TxDmaDescTbl[lastDescIdx];
    clean data cache for TxDmaDescTbl;
    clean data cache for pktBuff;
    modulo increment lastDescIdx w.r.t BL_TX_CHAN_DESC_CNT;

    if lastDescIdx equals to 0UL:
        mark this descriptor as the end of descriptor table;
        clean data cache for TxDmaDescTbl;

    // Enqueue descriptor to hardware
    update last descriptor pointer to hardware;

    retVal = BL_XmtWaitTxDone(aChan);
    exit:
        return retVal;
    @endcode
*/
int32_t BL_ETHER_Send(BL_ETHER_HwIDType aID,
                      BL_ETHER_ChanIDType aChan,
                      uint32_t aBufIdx,
                      uint32_t aLen)
{
    int32_t retVal;
    uint32_t lastDesc;
    uint32_t currDesc;
    BL_DmaDescType *dmaDescTbl;
    uint8_t *pktBuff;
    uint32_t lastDescIdx;
    uint32_t currDescIdx;
    uint32_t volatile *state;
    uint32_t isUpdated = BL_FALSE;
    uint32_t queueIdx;
    uint32_t newQueueIdx;
    uint32_t pktLen;

    if ((BL_ETHER_HW_ID_MAX <= aID) || (BL_ETH_FRM_SIZE_NO_CRC < aLen)
        || (BL_ETHER_TX_CHAN_CNT <= aChan)
        || (BL_TxChannInfo[aChan].pktBuffCnt <= aBufIdx)) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        BL_AMAC_ReportError(retVal, aID, 0UL, 0UL, 0UL, __LINE__);
        goto err;
    }

    if (BL_ETHER_STATE_ACTIVE != BL_TxChannInfo[aChan].state) {
        retVal = BL_BCM_ERR_INVAL_STATE;
        goto err;
    }

    if (BL_PKT_BUFF_STATE_ALLOC !=
            BL_TxChannInfo[aChan].pktBuffInfo[aBufIdx].state) {
        retVal = BL_BCM_ERR_INVAL_STATE;
        BL_AMAC_ReportError(retVal, aID, 0UL, 0UL, 0UL, __LINE__);
        goto err;
    }

    /* If aLen equal to zero, just free the packet */
    if (0UL == aLen) {
        state = &(BL_TxChannInfo[aChan].pktBuffInfo[aBufIdx].state);
        isUpdated = BL_CheckUpdateState(state, BL_PKT_BUFF_STATE_ALLOC,
                                            BL_PKT_BUFF_STATE_FREE);
        if (BL_FALSE == isUpdated) {
            retVal = BL_BCM_ERR_INVAL_BUF_STATE;
        } else {
            retVal = BL_BCM_ERR_OK;
        }
        goto err;
    }

    /* Check for room in DMA Desc Table */
    lastDesc = BL_XmtGetLastDesc(aChan);
    currDesc = BL_XmtGetCurrDesc(aChan);
    lastDescIdx = lastDesc/sizeof(BL_DmaDescType);
    currDescIdx = currDesc/sizeof(BL_DmaDescType);

    if ((BL_ModIncIdx(lastDescIdx, BL_TX_CHAN_DESC_CNT(aChan)))
            == currDescIdx) {
        retVal = BL_BCM_ERR_BUSY;
        goto err;
    }

    /* Verify and update state of the buffer */
    state = &(BL_TxChannInfo[aChan].pktBuffInfo[aBufIdx].state);
    isUpdated = BL_CheckUpdateState(state, BL_PKT_BUFF_STATE_ALLOC,
                                        BL_PKT_BUFF_STATE_QUEUED);
    if (BL_FALSE == isUpdated) {
        retVal = BL_BCM_ERR_INVAL_BUF_STATE;
        BL_AMAC_ReportError(BL_BCM_ERR_INVAL_BUF_STATE, aID, 0UL, 0UL, 0UL, __LINE__);
        goto err;
    }

    dmaDescTbl = BL_TxChannInfo[aChan].dmaDescTbl;

    if (aLen < BL_ETH_MIN_FRM_SIZE_NO_CRC) {
        pktLen =  BL_ETH_MIN_FRM_SIZE_NO_CRC;
    } else {
        pktLen =  aLen;
    }

    /* Get the queueIdx for buffer index Q */
    queueIdx = BL_TxChannInfo[aChan].queueIdx;
    newQueueIdx = BL_ModIncIdx(queueIdx, BL_TX_CHAN_DESC_CNT(aChan));
    BL_TxChannInfo[aChan].queueIdx = newQueueIdx;

    /* Queue the buffer index to buffIdxQ */
    BL_TxChannInfo[aChan].buffIdxQ[queueIdx] = aBufIdx;

    pktBuff = BL_TxChannInfo[aChan].pktBuffInfo[aBufIdx].pktBuff;
    lastDesc = BL_XmtGetLastDesc(aChan);
    lastDescIdx = lastDesc/BL_DMA_DESC_SIZE;
    BL_SetDescFields(dmaDescTbl[lastDescIdx],
                BL_TXDMA_FLAG_CRC_APPEND,
                BL_DMA_DESC_EOT_DIS,
                BL_DMA_DESC_IOC_EN,
                BL_DMA_DESC_EOF_EN,
                BL_DMA_DESC_SOF_EN,
                pktLen,
                (uint32_t)(pktBuff),
                BL_DMA_DESC_DATAPTRHIGH_ZERO);
    CORTEX_MX_DCacheCleanRange((uint32_t)BL_TxCh0DmaDescTbl, sizeof(BL_TxCh0DmaDescTbl));
    CORTEX_MX_DCacheCleanRange((uint32_t)pktBuff, sizeof(BL_TxPktType));

    lastDescIdx = BL_ModIncIdx(lastDescIdx, BL_TX_CHAN_DESC_CNT(aChan));
    if (lastDescIdx == 0UL) {
        BL_SetDescField(
        dmaDescTbl[BL_ModDecIdx(lastDescIdx, BL_TX_CHAN_DESC_CNT(aChan))],
        eot, BL_DMA_DESC_EOT_EN);
        CORTEX_MX_DCacheCleanRange((uint32_t)BL_TxCh0DmaDescTbl, sizeof(BL_TxCh0DmaDescTbl));
        BL_XmtSetLastDesc(aChan, BL_TX_DESC_OFFSET(lastDescIdx));
    } else {
        BL_XmtSetLastDesc(aChan, BL_TX_DESC_OFFSET(lastDescIdx));
    }
    retVal = BL_XmtWaitTxDone(aChan);
err:
    return retVal;
}

/**
    @code{.c}
    if aID is not less than BL_ETHER_HW_ID_MAX:
        goto exit;

    if aChan is not less than BL_ETHER_TX_CHAN_CNT:
        goto exit;

    if BL_TxChannInfo.state is not BL_ETHER_STATE_UNINIT:
        goto exit;

    BL_XmtInit();
    BL_ETHER_RcvInit();

exit:
    return;
    @endcode
*/
void BL_ETHER_Init(BL_ETHER_HwIDType aID,
                   BL_ETHER_ChanIDType aChan)
{
    if ((BL_ETHER_HW_ID_MAX <= aID) || (BL_ETHER_TX_CHAN_CNT <= aChan)) {
        goto err;
    }

    if (BL_ETHER_STATE_UNINIT != BL_TxChannInfo[aChan].state) {
        goto err;
    }

    BL_XmtChannInfoInit(aID, aChan);
    BL_XmtInit(aID, aChan);
    BL_TxChannInfo[aChan].state = BL_ETHER_STATE_ACTIVE;

    /* initialize RX */
    BL_ETHER_RcvInit(aID, 0UL);

err:
    return ;
}


static void BL_ETHER_IRQHandler(BL_ETHER_HwIDType aID)
{
    uint32_t intStatus;

    intStatus = BL_AMAC_REGS->INTSTATUS;

    if (intStatus != 0UL) {
        /* clear the interrupts */
        BL_AMAC_REGS->INTSTATUS = intStatus;
        /* Transmit and receive completion handlers */
        BL_RcvCompIRQHandler(intStatus);
        BL_XmtCompIRQHandler(intStatus);

        /* Error handlers */
        BL_RcvErrIRQHandler(intStatus);
        BL_DMAErrIRQHandler(intStatus);
        BL_XmtErrIRQHandler(intStatus);
    }
}

/**
    @code{.c}
    retreive interrupt status from hardware;

    if interrupt status not equal to 0:
        clear interrupt status;
        call BL_RcvCompIRQHandler(intStatus);
        call BL_XmtCompIRQHandler(intStatus);
        call BL_RcvErrIRQHandler(intStatus);
        call BL_DMAErrIRQHandler(intStatus);
        call BL_XmtErrIRQHandler(intStatus);

    // Process RX channel
    do:
        set retVal to BL_ProcessRcvdPkts(0);
        if retVal not equal to BL_BCM_ERR_EAGAIN
            and retVal not equal to BL_BCM_ERR_OK:
                report error "error in BL_ProcessRcvdPkts";
    while retVal equal to BL_BCM_ERR_OK ;

    if retVal equal to BL_BCM_ERR_EAGAIN
        and if BL_RxChannInfo.isErr equal to BL_TRUE:
            call BL_ETHER_RcvInit();
            reset UniMAC receive logic;

    // Process TX channel
    do:
        set retVal to BL_ProcessXmtdPkts(0);
        if retVal not equal to BL_BCM_ERR_EAGAIN
            and retVal not equal to BL_BCM_ERR_OK:
                report error "error in BL_ProcessXmtdPkts";
    while retVal equal to BL_BCM_ERR_OK ;

    if retVal equal to BL_BCM_ERR_EAGAIN
        and if BL_TxChannInfo.isErr equal to BL_TRUE:
            call BL_XmtInit();
    @endcode
*/
void BL_ETHER_ProcessPkts(void)
{
    int32_t retVal;
    uint32_t i, j;

    for (i = 0UL; i < BL_ETHER_HW_ID_MAX; i++) {

        BL_ETHER_IRQHandler(i);

        /* process RX channels */
        do {
            retVal = BL_ProcessRcvdPkts(i, BL_AMAC_RCV_CH_0);
            if ((BL_BCM_ERR_EAGAIN != retVal) && (BL_BCM_ERR_OK != retVal)) {
                BL_AMAC_ReportError(retVal, i, 0UL, 0UL, 0UL, __LINE__);
            }
        } while (BL_BCM_ERR_OK == retVal);

        if ((BL_BCM_ERR_EAGAIN == retVal) && (BL_TRUE == BL_RxChannInfo[0UL].isErr)) {
            BL_RcvChannErrHandler(i, BL_AMAC_RCV_CH_0);
        }

        /* Process all TX channels */
        for (j = 0UL; j < BL_ETHER_TX_CHAN_CNT; j++) {
            do {
                retVal = BL_ProcessXmtdPkts(j);
                if ((BL_BCM_ERR_EAGAIN != retVal) && (BL_BCM_ERR_OK != retVal)) {
                    BL_AMAC_ReportError(retVal, i, 0UL, 0UL, 0UL, __LINE__);
                }
            } while (BL_BCM_ERR_OK == retVal);

            if ((BL_BCM_ERR_EAGAIN == retVal) && (BL_TRUE == BL_TxChannInfo[i].isErr)) {
                BL_XmtChannErrHandler(i, j);
            }
        }
    }
}

/** @} */
