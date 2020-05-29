/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
 File Name: bl_eth_unimac.h
 Description: This file implements interface for UNIMAC layer of Ethernet driver.
******************************************************************************/

#ifndef BL_ETH_UNIMAC_H
#define BL_ETH_UNIMAC_H

#include <bl_eth_xcvr.h>

typedef volatile struct {
uint32_t    UMAC_IPG_HD_BKP_CNTL;    /**< OFFSET:2052 */
uint32_t    UMAC_CMDCFG;    /**< OFFSET:2056 */
#define UMAC_CMDCFG_RUNT_FILTER_DIS_MASK        (0x40000000UL)
#define UMAC_CMDCFG_RUNT_FILTER_DIS_SHIFT       (30UL)
#define UMAC_CMDCFG_OOB_EFC_EN_MASK             (0x20000000UL)
#define UMAC_CMDCFG_OOB_EFC_EN_SHIFT            (29UL)
#define UMAC_CMDCFG_IGNORE_TX_PAUSE_MASK        (0x10000000UL)
#define UMAC_CMDCFG_IGNORE_TX_PAUSE_SHIFT       (28UL)
#define UMAC_CMDCFG_FD_TX_URUN_FIXEN_MASK       (0x08000000UL)
#define UMAC_CMDCFG_FD_TX_URUN_FIXEN_SHIFT      (27UL)
#define UMAC_CMDCFG_RX_ERR_DISC_MASK            (0x04000000UL)
#define UMAC_CMDCFG_RX_ERR_DISC_SHIFT           (26UL)
#define UMAC_CMDCFG_LINE_LOOPBACK_MASK          (0x02000000UL)
#define UMAC_CMDCFG_LINE_LOOPBACK_SHIFT         (25UL)
#define UMAC_CMDCFG_NO_LGTH_CHECK_MASK          (0x01000000UL)
#define UMAC_CMDCFG_NO_LGTH_CHECK_SHIFT         (24UL)
#define UMAC_CMDCFG_CNTL_FRM_ENA_MASK           (0x00800000UL)
#define UMAC_CMDCFG_CNTL_FRM_ENA_SHIFT          (23UL)
#define UMAC_CMDCFG_ENA_EXT_CONFIG_MASK         (0x00400000UL)
#define UMAC_CMDCFG_ENA_EXT_CONFIG_SHIFT        (22UL)
#define UMAC_CMDCFG_EN_INTL_TX_CRS_MASK         (0x00200000UL)
#define UMAC_CMDCFG_EN_INTL_TX_CRS_SHIFT        (21UL)
#define UMAC_CMDCFG_SW_OVERRIDE_RX_MASK         (0x00040000UL)
#define UMAC_CMDCFG_SW_OVERRIDE_RX_SHIFT        (18UL)
#define UMAC_CMDCFG_SW_OVERRIDE_TX_MASK         (0x00020000UL)
#define UMAC_CMDCFG_SW_OVERRIDE_TX_SHIFT        (17UL)
#define UMAC_CMDCFG_MAC_LOOP_CON_MASK           (0x00010000UL)
#define UMAC_CMDCFG_MAC_LOOP_CON_SHIFT          (16UL)
#define UMAC_CMDCFG_LOOP_ENA_MASK               (0x00008000UL)
#define UMAC_CMDCFG_LOOP_ENA_SHIFT              (15UL)
#define UMAC_CMDCFG_FCS_CRRPT_URUNEN_MASK       (0x00004000UL)
#define UMAC_CMDCFG_FCS_CRRPT_URUNEN_SHIFT      (14UL)
#define UMAC_CMDCFG_SW_RESET_MASK               (0x00002000UL)
#define UMAC_CMDCFG_SW_RESET_SHIFT              (13UL)
#define UMAC_CMDCFG_OVERFLOW_EN_MASK            (0x00001000UL)
#define UMAC_CMDCFG_OVERFLOW_EN_SHIFT           (12UL)
#define UMAC_CMDCFG_RX_LOWLATENCY_EN_MASK       (0x00000800UL)
#define UMAC_CMDCFG_RX_LOWLATENCY_EN_SHIFT      (11UL)
#define UMAC_CMDCFG_HD_ENA_MASK                 (0x00000400UL)
#define UMAC_CMDCFG_HD_ENA_SHIFT                (10UL)
/*** HD_ENA specific macro ***/
#define UMAC_CMDCFG_HD_HALF_DUPLEX              (1UL)
#define UMAC_CMDCFG_HD_FULL_DUPLEX              (0UL)
/********************************/
#define UMAC_CMDCFG_TX_ADDR_INS_MASK            (0x00000200UL)
#define UMAC_CMDCFG_TX_ADDR_INS_SHIFT           (9UL)
#define UMAC_CMDCFG_PAUSE_IGNORE_MASK           (0x00000100UL)
#define UMAC_CMDCFG_PAUSE_IGNORE_SHIFT          (8UL)
#define UMAC_CMDCFG_PAUSE_FWD_MASK              (0x00000080UL)
#define UMAC_CMDCFG_PAUSE_FWD_SHIFT             (7UL)
#define UMAC_CMDCFG_CRC_FWD_MASK                (0x00000040UL)
#define UMAC_CMDCFG_CRC_FWD_SHIFT               (6UL)
#define UMAC_CMDCFG_PAD_EN_MASK                 (0x00000020UL)
#define UMAC_CMDCFG_PAD_EN_SHIFT                (5UL)
#define UMAC_CMDCFG_PROMIS_EN_MASK              (0x00000010UL)
#define UMAC_CMDCFG_PROMIS_EN_SHIFT             (4UL)
#define UMAC_CMDCFG_ETH_SPEED_MASK              (0x0000000cUL)
#define UMAC_CMDCFG_ETH_SPEED_SHIFT             (2UL)
/*** ETH_SPEED specific macro ***/
#define UMAC_CMDCFG_ETH_SPEED_10MBPS            (0UL)
#define UMAC_CMDCFG_ETH_SPEED_100MBPS           (1UL)
#define UMAC_CMDCFG_ETH_SPEED_1GBPS             (2UL)
#define UMAC_CMDCFG_ETH_SPEED_2_5GBPS           (3UL)
/********************************/
#define UMAC_CMDCFG_RX_ENA_MASK                 (0x00000002UL)
#define UMAC_CMDCFG_RX_ENA_SHIFT                (1UL)
#define UMAC_CMDCFG_TX_ENA_MASK                 (0x00000001UL)
#define UMAC_CMDCFG_TX_ENA_SHIFT                (0UL)

uint32_t    UMAC_MAC_0;    /**< OFFSET:2060 */
uint32_t    UMAC_MAC_1;    /**< OFFSET:2064 */
uint32_t    UMAC_FRM_LENGTH;    /**< OFFSET:2068 */
uint32_t    UMAC_PAUSE_QUANT;    /**< OFFSET:2072 */
uint32_t    UMAC_RESERVED_0[8];
uint32_t    UMAC_TX_TS_SEQ_ID;    /**< OFFSET:2108 */
uint32_t    UMAC_RESERVED_1[1];
uint32_t    UMAC_MAC_MODE;    /**< OFFSET:2116 */
uint32_t    UMAC_TAG_0;    /**< OFFSET:2120 */
uint32_t    UMAC_TAG_1;    /**< OFFSET:2124 */
uint32_t    UMAC_RX_PAUSE_QUANTA_SCALE;    /**< OFFSET:2128 */
uint32_t    UMAC_TX_PREAMBLE;   /**< OFFSET:2132 */
uint32_t    UMAC_RESERVED_2[1];   /**< OFFSET:2136 */
uint32_t    UMAC_TX_IPG_LENGTH;   /**< OFFSET:2140 */
uint32_t    UMAC_PFC_XOFF_TIMER;   /**< OFFSET:2144 */
uint32_t    UMAC_UMAC_EEE_CTRL;   /**< OFFSET:2148 */
uint32_t    UMAC_MII_EEE_DELAY_ENTRY_TIMER;   /**< OFFSET:2152 */
uint32_t    UMAC_GMII_EEE_DELAY_ENTRY_TIMER;   /**< OFFSET:2156 */
uint32_t    UMAC_UMAC_EEE_REF_COUNT;   /**< OFFSET:2160 */
uint32_t    UMAC_RESERVED_3[1];   /**< OFFSET:2164 */
uint32_t    UMAC_UMAC_RX_PKT_DROP_STATUS; /**< OFFSET:2168 */
uint32_t    UMAC_UMAC_SYMMETRIC_IDLE_THRESHOLD; /**< OFFSET:2172 */
uint32_t    UMAC_MII_EEE_WAKE_TIMER; /**< OFFSET:2176 */
uint32_t    UMAC_GMII_EEE_WAKE_TIMER; /**< OFFSET:2180 */
uint32_t    UMAC_UMAC_REV_ID; /**< OFFSET:2184 */
uint32_t    UMAC_RESERVED_4[157];   /**< OFFSET: */
uint32_t    UMAC_MAC_PFC_TYPE; /**< OFFSET: */
uint32_t    UMAC_MAC_PFC_OPCODE; /**< OFFSET: */
uint32_t    UMAC_MAC_PFC_DA_0; /**< OFFSET: */
uint32_t    UMAC_MAC_PFC_DA_1; /**< OFFSET: */
uint32_t    UMAC_MACSEC_CNTRL; /**< OFFSET: */
uint32_t    UMAC_TS_STATUS_CNTRL; /**< OFFSET: */
uint32_t    UMAC_TX_TS_DATA; /**< OFFSET: */
uint32_t    UMAC_RESERVED_5[19];   /**< OFFSET: */
uint32_t    UMAC_PAUSE_CONTROL; /**< OFFSET: */
uint32_t    UMAC_FLUSH_CONTROL; /**< OFFSET: */
uint32_t    UMAC_RXFIFO_STAT; /**< OFFSET: */
uint32_t    UMAC_TXFIFO_STAT; /**< OFFSET: */
uint32_t    UMAC_MAC_PFC_CTRL; /**< OFFSET: */
uint32_t    UMAC_MAC_PFC_REFRESH_CTRL; /**< OFFSET: */
} BL_UNIMAC_RegType;

extern void BL_UNIMAC_EnableRxReset(uint32_t enable);

#endif /* ETH_UNIMAC_H */