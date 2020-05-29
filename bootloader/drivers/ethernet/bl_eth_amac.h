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
    @addtogroup grp_bl_eth_impl
    @{
    @file bl_eth_amac.h
    @brief Ethernet Controller driver internal header

    @version 0.30 Imported from docx
*/

#ifndef BL_ETH_AMAC_H
#define BL_ETH_AMAC_H

#include <bl_ethernet.h>
#include <cortex.h>

/**
    @name Ethernet Controller Driver API IDs
    @{
    @brief API IDs for Ethernet Controller
*/
#define BRCM_DSGN_BL_ETHER_CHANN_STATE_TYPE         (0x80U) /**< @brief #BL_ChannStateType */
#define BRCM_DSGN_BL_ETHER_CHANN_ERR_TYPE           (0x81U) /**< @brief #BL_ChannErrType */
#define BRCM_DSGN_BL_ETHER_XMT_CHANNEL_TYPE         (0x82U) /**< @brief #BL_XmtChannelType */
#define BRCM_DSGN_BL_ETHER_RCV_CHANNEL_TYPE         (0x83U) /**< @brief #BL_RcvChannelType */
#define BRCM_DSGN_BL_ETHER_DMA_DESC_TYPE            (0x84U) /**< @brief #BL_DmaDescType */
#define BRCM_DSGN_BL_ETHER_AMAC_RX_PKT_INFO_TYPE    (0x85U) /**< @brief #BL_AMAC_RxPktInfoType */
#define BRCM_DSGN_BL_ETHER_RX_PKT_TYPE              (0x86U) /**< @brief #BL_RxPktType */
#define BRCM_DSGN_BL_ETHER_TX_PKT_TYPE              (0x87U) /**< @brief #BL_TxPktType */
#define BRCM_DSGN_BL_ETHER_PKT_STATES_TYPE          (0x88U) /**< @brief #BL_PKT_BUFF_STATE_FREE */
#define BRCM_DSGN_BL_ETHER_RX_PKT_BUFF_INFO_TYPE    (0x89U) /**< @brief #BL_RxPktBuffInfoType */
#define BRCM_DSGN_BL_ETHER_TX_PKT_BUFF_INFO_TYPE    (0x8AU) /**< @brief #BL_TxPktBuffInfoType */
#define BRCM_DSGN_BL_ETHER_TX_CHANN_INFO_TYPE       (0x8BU) /**< @brief #BL_TxChannInfoType */
#define BRCM_DSGN_BL_ETHER_RX_CHANN_INFO_TYPE       (0x8CU) /**< @brief #BL_RxChannInfoType */
/** @} */

typedef volatile struct {
uint32_t    DEVCONTROL;    /**< OFFSET:0 */
uint32_t    DEVSTATUS;    /**< OFFSET:4 */
uint32_t    RSVD1;
uint32_t    BISTSTATUS;    /**< OFFSET:12 */
uint32_t    RSVD2[4];
uint32_t    INTSTATUS;    /**< OFFSET:32 */
/* Check the mask for all interrupts */
#define     INTSTATUS_ALL_MASK                      (0xFF01FDFFUL)
#define     INTSTATUS_DESCRERR_MASK                 (0x00000400UL)
#define     INTSTATUS_DESCRERR_SHIFT                (10UL)
#define     INTSTATUS_DATAERR_MASK                  (0x00000800UL)
#define     INTSTATUS_DATAERR_SHIFT                 (11UL)
#define     INTSTATUS_DESCPROTOERR_MASK             (0x00001000UL)
#define     INTSTATUS_DESCPROTOERR_SHIFT            (12UL)
#define     INTSTATUS_RCVDESCUF_MASK                (0x00002000UL)
#define     INTSTATUS_RCVDESCUF_SHIFT               (13UL)
#define     INTSTATUS_RCVFIFOOF_MASK                (0x00004000UL)
#define     INTSTATUS_RCVFIFOOF_SHIFT               (14UL)
#define     INTSTATUS_XMTFIFOUF_MASK                (0x00008000UL)
#define     INTSTATUS_XMTFIFOUF_SHIFT               (15UL)
#define     INTSTATUS_RCVINT_MASK                   (0x00010000UL)
#define     INTSTATUS_RCVINT_SHIFT                  (16UL)
#define     INTSTATUS_XMTINT_0_MASK                 (0x01000000UL)
#define     INTSTATUS_XMTINT_0_SHIFT                (24UL)
#define     INTSTATUS_XMTINT_1_MASK                 (0x02000000UL)
#define     INTSTATUS_XMTINT_1_SHIFT                (25UL)
#define     INTSTATUS_XMTINT_2_MASK                 (0x04000000UL)
#define     INTSTATUS_XMTINT_2_SHIFT                (26UL)
#define     INTSTATUS_XMTINT_3_MASK                 (0x08000000UL)
#define     INTSTATUS_XMTINT_3_SHIFT                (27UL)
#define     INTSTATUS_RXQECCCORRECTED_MASK          (0x10000000UL)
#define     INTSTATUS_RXQECCCORRECTED_SHIFT         (28UL)
#define     INTSTATUS_RXQECCUNCORRECTED_MASK        (0x20000000UL)
#define     INTSTATUS_RXQECCUNCORRECTED_SHIFT       (29UL)
#define     INTSTATUS_TXQECCCORRECTED_MASK          (0x80000000UL)
#define     INTSTATUS_TXQECCCORRECTED_SHIFT         (30UL)
#define     INTSTATUS_TXQECCUNCORRECTED_MASK        (0x40000000UL)
#define     INTSTATUS_TXQECCUNCORRECTED_SHIFT       (31UL)

uint32_t    INTMASK;    /**< OFFSET:36 */
#define     INTMASK_ALL_MASK                        (0xFF01FDFFUL)
#define     INTMASK_DESCRERR_MASK                   (0x00000400UL)
#define     INTMASK_DESCRERR_SHIFT                  (10UL)
#define     INTMASK_DATAERR_MASK                    (0x00000800UL)
#define     INTMASK_DATAERR_SHIFT                   (11UL)
#define     INTMASK_DESCPROTOERR_MASK               (0x00001000UL)
#define     INTMASK_DESCPROTOERR_SHIFT              (12UL)
#define     INTMASK_RCVDESCUF_MASK                  (0x00002000UL)
#define     INTMASK_RCVDESCUF_SHIFT                 (13UL)
#define     INTMASK_RCVFIFOOF_MASK                  (0x00004000UL)
#define     INTMASK_RCVFIFOOF_SHIFT                 (14UL)
#define     INTMASK_XMTFIFOUF_MASK                  (0x00008000UL)
#define     INTMASK_XMTFIFOUF_SHIFT                 (15UL)
#define     INTMASK_RCVINT_MASK                     (0x00010000UL)
#define     INTMASK_RCVINT_SHIFT                    (16UL)
#define     INTMASK_XMTINT_0_MASK                   (0x01000000UL)
#define     INTMASK_XMTINT_0_SHIFT                  (24UL)
#define     INTMASK_XMTINT_1_MASK                   (0x02000000UL)
#define     INTMASK_XMTINT_1_SHIFT                  (25UL)
#define     INTMASK_XMTINT_2_MASK                   (0x04000000UL)
#define     INTMASK_XMTINT_2_SHIFT                  (26UL)
#define     INTMASK_XMTINT_3_MASK                   (0x08000000UL)
#define     INTMASK_XMTINT_3_SHIFT                  (27UL)
#define     INTMASK_RXQECCCORRECTED_MASK            (0x10000000UL)
#define     INTMASK_RXQECCCORRECTED_SHIFT           (28UL)
#define     INTMASK_RXQECCUNCORRECTED_MASK          (0x20000000UL)
#define     INTMASK_RXQECCUNCORRECTED_SHIFT         (29UL)
#define     INTMASK_TXQECCCORRECTED_MASK            (0x80000000UL)
#define     INTMASK_TXQECCCORRECTED_SHIFT           (30UL)
#define     INTMASK_TXQECCUNCORRECTED_MASK          (0x40000000UL)
#define     INTMASK_TXQECCUNCORRECTED_SHIFT         (31UL)

uint32_t    GPTIMER;    /**< OFFSET:40 */
uint32_t    RSVD3[53];
uint32_t    INTRCVLAZY;    /**< OFFSET:256 */
#define     INTRCVLAZY_FRAMECOUNT_MASK              (0xF0000000UL)
#define     INTRCVLAZY_FRAMECOUNT_SHIFT             (24UL)
#define     INTRCVLAZY_TIMEOUT_MASK                 (0x0FFFFFFFUL)
#define     INTRCVLAZY_TIMEOUT_SHIFT                (0UL)
uint32_t    FLOWCNTL_TH;    /**< OFFSET:260 */
uint32_t    RSVD4[62];
uint32_t    XMTCTRL_0;    /**< OFFSET:512 */
#define     XMTCTRL_XMTEN_MASK                      (0x00000001UL)
#define     XMTCTRL_XMTEN_SHIFT                     (0UL)
#define     XMTCTRL_TXSUSPEND_MASK                  (0x00000002UL)
#define     XMTCTRL_TXSUSPEND_SHIFT                 (1UL)
#define     XMTCTRL_BURSTBL_ALIGNEN_MASK               (0x00000020UL)
#define     XMTCTRL_BURSTBL_ALIGNEN_SHIFT              (5UL)
#define     XMTCTRL_TXPARITYCHECKDIS_MASK           (0x00000800UL)
#define     XMTCTRL_TXPARITYCHECKDIS_SHIFT          (11UL)
#define     XMTCTRL_DMALOOPBACKMODE_MASK            (0x00000004UL)
#define     XMTCTRL_DMALOOPBACKMODE_SHIFT           (2UL)

uint32_t    XMTPTR_0;    /**< OFFSET:516 */
#define     XMTPTR_LASTDSCR_MASK                    (0x00001fffUL)
#define     XMTPTR_LASTDSCR_SHIFT                   (0UL)

uint32_t    XMTADDR_LOW_0;    /**< OFFSET:520 */
uint32_t    XMTADDR_HIGH_0;    /**< OFFSET:524 */
uint32_t    XMTSTATUS0_0;    /**< OFFSET:528 */
#define     XMTSTATUS0_XMTSTATE_MASK                (0xf0000000UL)
#define     XMTSTATUS0_XMTSTATE_SHIFT               (28UL)
#define     XMTSTATUS0_CURRDSCR_MASK                (0x00001fffUL)
#define     XMTSTATUS0_CURRDSCR_SHIFT               (0UL)

uint32_t    XMTSTATUS1_0;    /**< OFFSET:532 */
#define     XMTSTATUS1_XMTERR_MASK                  (0xf0000000UL)
#define     XMTSTATUS1_XMTERR_SHIFT                 (28UL)
#define     XMTSTATUS1_ACTIVEDESCR_MASK             (0x00001fffUL)
#define     XMTSTATUS1_ACTIVEDESCR_SHIFT            (0UL)

uint32_t    RSVD5[2];
uint32_t    RCVCTRL;    /**< OFFSET:544 */
#define     RCVCTRL_RXPARITYCHECKDIS_MASK           (0x00000800UL)
#define     RCVCTRL_RXPARITYCHECKDIS_SHIFT          (11UL)
#define     RCVCTRL_OFLOWCONTINUE_MASK              (0x00000400UL)
#define     RCVCTRL_OFLOWCONTINUE_SHIFT             (10UL)
#define     RCVCTRL_RCVOFFSET_MASK                  (0x000000FEUL)
#define     RCVCTRL_RCVOFFSET_SHIFT                 (1UL)
#define     RCVCTRL_RCVEN_MASK                      (0x0000001UL)
#define     RCVCTRL_RCVEN_SHIFT                     (0UL)

uint32_t    RCVPTR;    /**< OFFSET:548 */
#define     RCVPTR_LASTDSCR_MASK                    (0x00001fffUL)
#define     RCVPTR_LASTDSCR_SHIFT                   (0UL)

uint32_t    RCVADDR_LOW;    /**< OFFSET:552 */
uint32_t    RCVADDR_HIGH;    /**< OFFSET:556 */
uint32_t    RCVSTATUS0;    /**< OFFSET:560 */
#define     RCVSTATUS0_CURRDSCR_MASK                (0x00001fffUL)
#define     RCVSTATUS0_CURRDSCR_SHIFT               (0)
#define     RCVSTATUS0_RCVSTATE_MASK                (0xf0000000UL)
#define     RCVSTATUS0_RCVSTATE_SHIFT               (28)

uint32_t    RCVSTATUS1;    /**< OFFSET:564 */
#define     RCVSTATUS1_ACTIVEDESCR_MASK             (0x00001fffUL)
#define     RCVSTATUS1_ACTIVEDESCR_SHIFT            (0UL)
#define     RCVSTATUS1_RCVERR_MASK                  (0xf0000000UL)
#define     RCVSTATUS1_RCVERR_SHIFT                 (28UL)

uint32_t    RSVD6[2];
uint32_t    XMTCTRL_1;    /**< OFFSET:576 */
uint32_t    XMTPTR_1;    /**< OFFSET:580 */
uint32_t    XMTADDR_LOW_1;    /**< OFFSET:584 */
uint32_t    XMTADDR_HIGH_1;    /**< OFFSET:588 */
uint32_t    XMTSTATUS0_1;    /**< OFFSET:592 */
uint32_t    XMTSTATUS1_1;    /**< OFFSET:596 */
uint32_t    RSVD7[10];
uint32_t    XMTCTRL_2;    /**< OFFSET:640 */
uint32_t    XMTPTR_2;    /**< OFFSET:644 */
uint32_t    XMTADDR_LOW_2;    /**< OFFSET:648 */
uint32_t    XMTADDR_HIGH_2;    /**< OFFSET:652 */
uint32_t    XMTSTATUS0_2;    /**< OFFSET:656 */
uint32_t    XMTSTATUS1_2;    /**< OFFSET:660 */
uint32_t    RSVD8[10];
uint32_t    XMTCTRL_3;    /**< OFFSET:704 */
uint32_t    XMTPTR_3;    /**< OFFSET:708 */
uint32_t    XMTADDR_LOW_3;    /**< OFFSET:712 */
uint32_t    XMTADDR_HIGH_3;    /**< OFFSET:716 */
uint32_t    XMTSTATUS0_3;    /**< OFFSET:720 */
uint32_t    XMTSTATUS1_3;    /**< OFFSET:724 */
uint32_t    RSVD9[2];
uint32_t    CKSUM_CONTROL;    /**< OFFSET:736 */
uint32_t    RXPRSR_VLAN;    /**< OFFSET:740 */
uint32_t    RXPRSR_L3;    /**< OFFSET:744 */
uint32_t    RXPRSR_MAXLEN;    /**< OFFSET:748 */
uint32_t    RSVD10[4];
uint32_t    TX_GD_OCTETS_LO;    /**< OFFSET:768 */
uint32_t    TX_GD_OCTETS_HI;    /**< OFFSET:772 */
uint32_t    TX_GD_PKTS;    /**< OFFSET:776 */
uint32_t    TX_ALL_OCTETS_LO;    /**< OFFSET:780 */
uint32_t    TX_ALL_OCTETS_HI;    /**< OFFSET:784 */
uint32_t    TX_ALL_PKTS;    /**< OFFSET:788 */
uint32_t    TX_BRDCAST;    /**< OFFSET:792 */
uint32_t    TX_MULT;    /**< OFFSET:796 */
uint32_t    TX_64;    /**< OFFSET:800 */
uint32_t    TX_65_127;    /**< OFFSET:804 */
uint32_t    TX_128_255;    /**< OFFSET:808 */
uint32_t    TX_256_511;    /**< OFFSET:812 */
uint32_t    TX_512_1023;    /**< OFFSET:816 */
uint32_t    TX_1024_1522;    /**< OFFSET:820 */
uint32_t    TX_1523_2047;    /**< OFFSET:824 */
uint32_t    TX_2048_4095;    /**< OFFSET:828 */
uint32_t    TX_4096_8191;    /**< OFFSET:832 */
uint32_t    TX_8192_MAX;    /**< OFFSET:836 */
uint32_t    TX_JAB;    /**< OFFSET:840 */
uint32_t    TX_OVER;    /**< OFFSET:844 */
uint32_t    TX_FRAG;    /**< OFFSET:848 */
uint32_t    TX_UNDERRUN;    /**< OFFSET:852 */
uint32_t    RSVD11[22];
uint32_t    RX_GD_OCTETS_LO;    /**< OFFSET:944 */
uint32_t    RX_GD_OCTETS_HI;    /**< OFFSET:948 */
uint32_t    RX_GD_PKTS;    /**< OFFSET:952 */
uint32_t    RX_ALL_OCTETS_LO;    /**< OFFSET:956 */
uint32_t    RX_ALL_OCTETS_HI;    /**< OFFSET:960 */
uint32_t    RX_ALL_PKTS;    /**< OFFSET:964 */
uint32_t    RX_BRDCAST;    /**< OFFSET:968 */
uint32_t    RX_MULT;    /**< OFFSET:972 */
uint32_t    RX_64;    /**< OFFSET:976 */
uint32_t    RX_65_127;    /**< OFFSET:980 */
uint32_t    RX_128_255;    /**< OFFSET:984 */
uint32_t    RX_256_511;    /**< OFFSET:988 */
uint32_t    RX_512_1023;    /**< OFFSET:992 */
uint32_t    RX_1024_1522;    /**< OFFSET:996 */
uint32_t    RX_1523_2047;    /**< OFFSET:1000 */
uint32_t    RX_2048_4095;    /**< OFFSET:1004 */
uint32_t    RX_4096_8191;    /**< OFFSET:1008 */
uint32_t    RX_8192_MAX;    /**< OFFSET:1012 */
uint32_t    RX_JAB;    /**< OFFSET:1016 */
uint32_t    RX_OVR;    /**< OFFSET:1020 */
uint32_t    RX_FRAG;    /**< OFFSET:1024 */
uint32_t    RX_DROP;    /**< OFFSET:1028 */
uint32_t    RX_CRC_BL_ALIGN;    /**< OFFSET:1032 */
uint32_t    RX_UND;    /**< OFFSET:1036 */
uint32_t    RX_CRC;    /**< OFFSET:1040 */
uint32_t    RX_BL_ALIGN;    /**< OFFSET:1044 */
uint32_t    RX_SYM;    /**< OFFSET:1048 */
uint32_t    RX_PAUS;    /**< OFFSET:1052 */
uint32_t    RX_CNTRL;    /**< OFFSET:1056 */
uint32_t    RXUNICASTPKTS;    /**< OFFSET:1064 */
uint32_t    RSVD12[4];
} BL_AMAC_RegType;

/* ================================================================
 *  AMAC Configuration
 *================================================================*/

#define BL_ALIGN(x, y)                 \
    ((((uint32_t)(x)) + (((uint32_t)(y)) - 1)) & ~(uint32_t)((y) - 1))

#define BL_ETH_MTU                                     (1500UL)
/* Ethernet frame data (without CRC) for Ethernet frame with
   minimum data length */
#define BL_ETH_MIN_FRM_SIZE_NO_CRC                     (60UL)
/* Ethernet frame data (with CRC) for Ethernet frame with minimum data length */
#define BL_ETH_MIN_FRM_SIZE                            (64UL)
#define BL_ETH_MAC_ADDR_SIZE                           (6UL)
#define BL_ETH_TYPE_SIZE                               (2UL)
#define BL_ETH_HDR_SIZE                \
    ((2UL * BL_ETH_MAC_ADDR_SIZE) + BL_ETH_TYPE_SIZE)
#define BL_ETH_CRC_SIZE                                (4UL)
#define BL_ETH_VLAN_TAG_SIZE                           (4UL)

#define BL_ETH_FRM_SIZE_NO_CRC         (BL_ETH_MTU + BL_ETH_HDR_SIZE + BL_ETH_VLAN_TAG_SIZE)
#define BL_ETH_FRAME_SIZE              (BL_ETH_MTU + BL_ETH_HDR_SIZE + BL_ETH_VLAN_TAG_SIZE\
                                    + BL_ETH_CRC_SIZE)

/* RX Packet receive offset. This is status region at the beginning of
 * receive frame buffer which is filled by RX DMA
 */
#define BL_AMAC_RXDMA_PKT_RCVOFFSET                    (4UL)

/*
 * Workaround for hardware JIRA: HWBCM89100-288
 * Align TX to 32-byte boundary and RX buffer to 28-byte(actually AMAC
 * DMA uses first 4-byte in the RX buffer to packet status, so actual
 * data write starts at buff+4
 */
#define BL_AMAC_RXDMA_PKT_SIZE         \
    BL_ALIGN(BL_ETH_FRAME_SIZE + BL_AMAC_RXDMA_PKT_RCVOFFSET, 32)
#define BL_AMAC_TXDMA_PKT_SIZE         \
    BL_ALIGN(BL_ETH_FRAME_SIZE, 32)

#define BL_TXDMA_FLAG_CRC_MASK                         (0x3UL)
#define BL_TXDMA_FLAG_CRC_APPEND                       (0x0UL)
#define BL_TXDMA_FLAG_CRC_OVERWRITE                    (0x1UL)
#define BL_TXDMA_FLAG_CRC_FORWARD                      (0x2UL)
#define BL_TXDMA_FLAG_CRC_RVSD                         (0x3UL)

/**
    @name BL_ChannStateType
    @{
    @brief Type for Channel state

    @trace #BRCM_DSGN_BL_ETHER_CHANN_STATE_TYPE #BRCM_REQ_BL_ETHER_CHANN_STATE
 */
typedef uint32_t BL_ChannStateType;      /**< @brief typedef for ethernet channel state */
#define BL_CHANN_STATE_DISABLED    (0UL) /**< @brief Disabled state */
#define BL_CHANN_STATE_ACTIVE      (1UL) /**< @brief Active state */
#define BL_CHANN_STATE_IDLE        (2UL) /**< @brief Idle state */
#define BL_CHANN_STATE_STOPPED     (3UL) /**< @brief Stop state */
#define BL_CHANN_STATE_SUSPPEND    (4UL) /**< @brief Suspend state */
/** @} */

/**
    @name BL_ChannErrType
    @{
    @brief Type for Channel error

    @trace #BRCM_DSGN_BL_ETHER_CHANN_ERR_TYPE #BRCM_REQ_BL_ETHER_CHANN_ERR
 */
typedef uint32_t BL_ChannErrType;            /**< @brief typedef for channel error */
#define BL_CHANN_ERR_NOERR             (0UL) /**< @brief No error */
#define BL_CHANN_ERR_PROT_ERR          (1UL) /**< @brief Protocol error */
#define BL_CHANN_ERR_FIFO_UNDERRUN     (2UL) /**< @brief FIFO underrun error */
#define BL_CHANN_ERR_TRANSFER_ERR      (3UL) /**< @brief Transfer error */
#define BL_CHANN_ERR_DESC_READ_ERR     (4UL) /**< @brief Descriptor read error */
#define BL_CHANN_ERR_CORE_ERR          (5UL) /**< @brief Core error */
#define BL_CHANN_ERR_DESC_PARITY_ERR   (6UL) /**< @brief Descriptor parity error */
/** @} */

/**
    @name BL_XmtChannelType
    @{
    @brief Type for transmit Channel

    @trace #BRCM_DSGN_BL_ETHER_XMT_CHANNEL_TYPE #BRCM_REQ_BL_ETHER_XMT_CHANNEL
 */
typedef uint32_t BL_XmtChannelType; /**< @brief typedef for transmit channel */
#define BL_AMAC_XMT_CH_0   (0UL)    /**< @brief Transmit Channel 0 */
#define BL_AMAC_XMT_CH_1   (1UL)    /**< @brief Transmit Channel 1 */
#define BL_AMAC_XMT_CH_2   (2UL)    /**< @brief Transmit Channel 2 */
#define BL_AMAC_XMT_CH_3   (3UL)    /**< @brief Transmit Channel 3 */
/** @} */

/**
    @name BL_RcvChannelType
    @{
    @brief Type for receive Channel

    @trace #BRCM_DSGN_BL_ETHER_RCV_CHANNEL_TYPE #BRCM_REQ_BL_ETHER_RCV_CHANNEL
 */
typedef uint32_t BL_RcvChannelType; /**< @brief typedef for receive channel */
#define BL_AMAC_RCV_CH_0   (0UL)    /**< @brief Receive Channel 0 */
/** @} */

/* Invalid buffer index */
#define BL_INVAL_BUFF_IDX                              (0xFFFFFFFFUL)

/**
 * AMAC channel error
 */
#define BL_CHAN_ERR_MIN                                (1UL)
#define BL_CHAN_DESC_PROTO_ERR                         (BL_CHAN_ERR_MIN)
#define BL_CHAN_FIFO_UN_ON_ERR                         (2UL)
#define BL_CHAN_DATA_TRANS_ERR                         (3UL)
#define BL_CHAN_DESC_READ_ERR                          (4UL)
#define BL_CHAN_CORE_ERR                               (5UL)
#define BL_CHAN_DESC_PARITY_ERR                        (6UL)
#define BL_CHAN_ERR_MAX                                (BL_CHAN_DESC_PARITY_ERR)

/**
    @brief AMAC DMA descriptor structure

    @trace #BRCM_DSGN_BL_ETHER_DMA_DESC_TYPE #BRCM_REQ_ETHER_DMA_DESC
 */
volatile typedef struct {
    uint32_t rsvd1          :20;    /**< @brief reserved */
    uint32_t flags          :8;     /**< @brief used by core */
    uint32_t eot            :1;     /**< @brief End of table */
#define BL_DMA_DESC_EOT_EN     (1UL)
#define BL_DMA_DESC_EOT_DIS    (0UL)
    uint32_t intOnComp      :1;     /**< @brief interrupt on completion */
#define BL_DMA_DESC_IOC_EN     (1UL)
#define BL_DMA_DESC_IOC_DIS    (0UL)
    uint32_t eof            :1;     /**< @brief end of frame */
#define BL_DMA_DESC_EOF_EN     (1UL)
#define BL_DMA_DESC_EOF_DIS    (0UL)
    uint32_t sof            :1;     /**< @brief start of frame */
#define BL_DMA_DESC_SOF_EN     (1UL)
#define BL_DMA_DESC_SOF_DIS    (0UL)
    uint32_t bufCount       :15;    /**< @brief buffer count */
#define BL_DMA_DESC_BUFCOUNT_ZERO  (0UL)
    uint32_t rsvd2          :1;     /**< @brief reserved */
    uint32_t addrExt        :2;     /**< @brief address external (PCIE) */
    uint32_t rsvd3          :1;     /**< @brief reserved */
    uint32_t rsvd4          :13;    /**< @brief reserved */
    uint32_t dataPtrLow;            /**< @brief data buffer pointer low address */
#define BL_DMA_DESC_DATAPTRLOW_ZERO (0UL)
    uint32_t dataPtrHigh;           /**< @brief data buffer pointer high address */
#define BL_DMA_DESC_DATAPTRHIGH_ZERO (0UL)
} BL_DmaDescType;

#define BL_DMA_DESC_SIZE        sizeof(BL_DmaDescType)

#define BL_SetDescFields(desc, _flags, _eot, _ic, _eof, _sof, _bc, _dl, _dh)   \
    do {                                                                    \
        desc.flags = _flags;                                                \
        desc.eot = _eot;                                                    \
        desc.intOnComp = _ic;                                               \
        desc.sof = _sof;                                                    \
        desc.eof = _eof;                                                    \
        desc.bufCount = _bc;                                                \
        desc.dataPtrLow = _dl;                                              \
        desc.dataPtrHigh = _dh;                                             \
        CORTEX_DMB();                                                              \
        CORTEX_ISB();                                                              \
    } while (0)

#define BL_SetDescField(desc, field, value)                                    \
    do {                                                                    \
        (desc.field = value);                                               \
        CORTEX_DMB();                                                              \
        CORTEX_ISB();                                                              \
    } while (0)

#define BL_INIT_RXDESC(aEot, aRxPktPtr)                                        \
{                                                                           \
    .flags = 0UL,                                                           \
    .eot = (aEot),                                                          \
    .intOnComp = BL_DMA_DESC_IOC_DIS,                                          \
    .eof = BL_DMA_DESC_EOF_DIS,                                                \
    .sof = BL_DMA_DESC_SOF_DIS,                                                \
    .bufCount = BL_AMAC_RXPKTTYPE_SIZE,                                        \
    .addrExt = 0UL,                                                         \
    .dataPtrLow = (uint32_t)(aRxPktPtr),                                    \
    .dataPtrHigh = BL_DMA_DESC_DATAPTRHIGH_ZERO                                \
}

/**
    @brief AMAC RX packet information

    @trace #BRCM_DSGN_BL_ETHER_AMAC_RX_PKT_INFO_TYPE #BRCM_REQ_ETHER_AMAC_RX_PKT_INFO
 */
typedef struct {
    uint16_t frameLen;      /**< @brief Frame length */
    uint16_t pktType    :2; /**< @brief Packet type */
    uint16_t vlanTagDet :1; /**< @brief VLAN Tag */
    uint16_t crcErr     :1; /**< @brief CRC Error */
    uint16_t overSz     :1; /**< @brief Over size error */
    uint16_t ctfHit     :1; /**< @brief CTF hit */
    uint16_t ctfErr     :1; /**< @brief CTF error */
    uint16_t pktOvflw   :1; /**< @brief Packet overflow */
    uint16_t descCntr   :4; /**< @brief Descriptor counter */
    uint16_t dataType   :4; /**< @brief Data type */
} BL_AMAC_RxPktInfoType;

/**
    @brief Receive packet structure

    @trace #BRCM_DSGN_BL_ETHER_RX_PKT_TYPE #BRCM_REQ_ETHER_RX_PKT
 */
typedef struct {
    uint8_t rsvd[28];                      /**< @brief Reserved fields to align to 32Bytes boundary */ 
    volatile BL_AMAC_RxPktInfoType rxPktInfo; /**< @brief RX packet info */
    uint8_t pktBuff[BL_AMAC_RXDMA_PKT_SIZE];  /**< @brief RX packet buffer */
} BL_RxPktType;

#define BL_AMAC_RXPKTTYPE_SIZE    (sizeof(BL_RxPktType))

/**
    @brief Transmit  packet structure

    @trace #BRCM_DSGN_BL_ETHER_TX_PKT_TYPE #BRCM_REQ_ETHER_TX_PKT
 */
typedef struct {
    uint8_t pktBuff[BL_AMAC_TXDMA_PKT_SIZE]; /**< @brief TX packet buffer */
} BL_TxPktType;

/**
    @name Packet buffer states
    @{
    @brief State of Packet buffer

    @trace #BRCM_DSGN_BL_ETHER_PKT_STATES_TYPE #BRCM_REQ_BL_ETHER_PKT_STATES
 */
#define BL_PKT_BUFF_STATE_FREE         (0UL) /**< @brief Free */
#define BL_PKT_BUFF_STATE_ALLOC        (1UL) /**< @brief With client */
#define BL_PKT_BUFF_STATE_QUEUED       (2UL) /**< @brief Queued to DMA */
#define BL_PKT_BUFF_STATE_DEQUEUED     (3UL) /**< @brief Dequeued after DMA completion */
/** @} */

/**
    @brief Structure for RX Packet buffer info

    @trace #BRCM_DSGN_BL_ETHER_RX_PKT_BUFF_INFO_TYPE #BRCM_REQ_ETHER_RX_PKT_BUFF_INFO
 */
typedef volatile struct {
    uint32_t    state;      /**< @brief State of the packet buffer */
    BL_RxPktType * pktBuff;    /**< @brief Pointer to packet buffer */
} BL_RxPktBuffInfoType;

/**
    @brief Structure for TX Packet buffer info

    @trace #BRCM_DSGN_BL_ETHER_TX_PKT_BUFF_INFO_TYPE #BRCM_REQ_ETHER_TX_PKT_BUFF_INFO
 */
typedef volatile struct {
    uint32_t    state;      /**< @brief State of the packet buffer */
    uint8_t *   pktBuff;    /**< @brief Pointer to packet buffer */
} BL_TxPktBuffInfoType;

/**
    @brief Structure for Tx Channel info

    @trace #BRCM_DSGN_BL_ETHER_TX_CHANN_INFO_TYPE #BRCM_REQ_ETHER_TX_CHANN_INFO
 */
typedef struct {
    volatile BL_ETHER_StateType    state;       /**< @brief State of the channel */
    BL_TxPktBuffInfoType * const   pktBuffInfo; /**< @brief Pointer buffer info array */
    uint32_t                    pktBuffCnt;  /**< @brief Count of pktBuffers */
    uint32_t                    allocIdx;    /**< @brief allocIdx */
    uint32_t                    deallocIdx;  /**< @brief deallocIdx */
    uint32_t                    queueIdx;    /**< @brief queueIdx */
    uint32_t                    dequeueIdx;  /**< @brief dequeueIdx */
    BL_DmaDescType * const         dmaDescTbl;  /**< @brief DMA descriptor table */
#define BL_TX_BUFF_IDX_Q_SIZE          (16UL)
    uint32_t buffIdxQ[BL_TX_BUFF_IDX_Q_SIZE];   /**< @brief Tx buffer idx Q */
    uint32_t                    isErr;       /**< @brief Flag to indicate error */
} BL_TxChannInfoType;

/**
    @brief Structure for RX Channel info

    @trace #BRCM_DSGN_BL_ETHER_RX_CHANN_INFO_TYPE #BRCM_REQ_ETHER_RX_CHANN_INFO
 */
/**< Structure for RX channel info */
typedef struct {
    volatile BL_ETHER_StateType  state;        /**< @brief State of the channel */
    BL_RxPktBuffInfoType * const pktBuffInfo;  /**< @brief Pointer buffer info array */
    uint32_t                allocIdx;       /**< @brief allocIdx */
    uint32_t                queueIdx;       /**< @brief queueIdx */
    uint32_t                dequeueIdx;     /**< @brief dequeueIdx */
    const BL_DmaDescType * const dmaDescTbl;   /**< @brief DMA descriptor table */
#define BL_RX_BUFF_IDX_Q_SIZE          (16UL)
    uint32_t buffIdxQ[BL_RX_BUFF_IDX_Q_SIZE];  /**< @brief Rx buffer idx Q */
    uint32_t                isErr;          /**< @brief Flag to indicate error */
} BL_RxChannInfoType;

#define BL_INIT_TX_CHANNEL_INFO(aTxChanState, aPktBuffInfo, aPktBuffCnt,   \
                            aAllocIdx, aDeallocIdx, aQueueIdx,          \
                            aDequeueIdx, aDmaDescTbl, aIsErr)           \
{                                                                       \
    .state = aTxChanState,                                              \
    .pktBuffInfo = aPktBuffInfo,                                        \
    .pktBuffCnt = aPktBuffCnt,                                          \
    .allocIdx = aAllocIdx,                                              \
    .deallocIdx = aDeallocIdx,                                          \
    .queueIdx = aQueueIdx,                                              \
    .dequeueIdx = aDequeueIdx,                                          \
    .dmaDescTbl = aDmaDescTbl,                                          \
    .isErr = aIsErr,                                                    \
}

#define BL_INIT_RX_CHANNEL_INFO(aRxState, aPktBuffInfo, aAllocIdx,         \
                            aQueueIdx, aDequeueIdx, aDmaDescTbl,        \
                            aIsErr)                                     \
{                                                                       \
    .state = aRxState,                                                  \
    .pktBuffInfo = aPktBuffInfo,                                        \
    .allocIdx = aAllocIdx,                                              \
    .queueIdx = aQueueIdx,                                              \
    .dequeueIdx = aDequeueIdx,                                          \
    .dmaDescTbl = aDmaDescTbl,                                          \
    .isErr = aIsErr,                                                    \
}

#endif /* BL_ETH_AMAC_H */

/** @} */
