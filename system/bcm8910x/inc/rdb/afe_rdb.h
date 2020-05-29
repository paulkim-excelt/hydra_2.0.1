/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    @file afe_rdb.h
    @brief RDB File for AFE

    @version 2018May25_rdb
*/

#ifndef AFE_RDB_H
#define AFE_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint16_t AFE_RXCONFIG_0_TYPE;
#define AFE_RXCONFIG_0_15_0_MASK (0xffffU)
#define AFE_RXCONFIG_0_15_0_SHIFT (0U)




typedef uint16_t AFE_RXCONFIG_1_TYPE;
#define AFE_RXCONFIG_1_31_23_MASK (0xffffU)
#define AFE_RXCONFIG_1_31_23_SHIFT (0U)




typedef uint16_t AFE_RXCONFIG_2_TYPE;
#define AFE_RXCONFIG_2_47_32_MASK (0xffffU)
#define AFE_RXCONFIG_2_47_32_SHIFT (0U)




typedef uint16_t AFE_RXCONFIG_3_TYPE;
#define AFE_RXCONFIG_3_63_48_MASK (0xffffU)
#define AFE_RXCONFIG_3_63_48_SHIFT (0U)




typedef uint16_t AFE_RXCONFIG_4_TYPE;
#define AFE_RXCONFIG_4_79_64_MASK (0xffffU)
#define AFE_RXCONFIG_4_79_64_SHIFT (0U)




typedef uint16_t AFE_RXCONFIG5_LP_TYPE;
#define AFE_RXCONFIG5_LP_RXCONFIG_86_80_MASK (0xfe00U)
#define AFE_RXCONFIG5_LP_RXCONFIG_86_80_SHIFT (9U)
#define AFE_RXCONFIG5_LP_RX_LP_0_MASK (0x1c0U)
#define AFE_RXCONFIG5_LP_RX_LP_0_SHIFT (6U)
#define AFE_RXCONFIG5_LP_RX_LP_1_MASK (0x38U)
#define AFE_RXCONFIG5_LP_RX_LP_1_SHIFT (3U)
#define AFE_RXCONFIG5_LP_MODE_FORCE_MASK (0x7U)
#define AFE_RXCONFIG5_LP_MODE_FORCE_SHIFT (0U)




typedef uint16_t AFE_TX_CONFIG_0_TYPE;
#define AFE_TX_CONFIG_0_15_0_MASK (0xffffU)
#define AFE_TX_CONFIG_0_15_0_SHIFT (0U)




typedef uint16_t AFE_TX_CONFIG_1_TYPE;
#define AFE_TX_CONFIG_1_BW_TUNE_MASK (0xf800U)
#define AFE_TX_CONFIG_1_BW_TUNE_SHIFT (11U)
#define AFE_TX_CONFIG_1_CONFIG_26_16_MASK (0x7ffU)
#define AFE_TX_CONFIG_1_CONFIG_26_16_SHIFT (0U)




typedef uint16_t AFE_VDAC_ICTRL_0_TYPE;
#define AFE_VDAC_ICTRL_0_CURRENT_CTRL_15_0_MASK (0xffffU)
#define AFE_VDAC_ICTRL_0_CURRENT_CTRL_15_0_SHIFT (0U)




typedef uint16_t AFE_VDAC_ICTRL_1_TYPE;
#define AFE_VDAC_ICTRL_1_CURRENT_CTRL_31_16_MASK (0xffffU)
#define AFE_VDAC_ICTRL_1_CURRENT_CTRL_31_16_SHIFT (0U)




typedef uint16_t AFE_VDAC_ICTRL_2_TYPE;
#define AFE_VDAC_ICTRL_2_CURRENT_CTRL_51_36_MASK (0xffffU)
#define AFE_VDAC_ICTRL_2_CURRENT_CTRL_51_36_SHIFT (0U)




typedef uint16_t AFE_VDAC_OTHERS_0_TYPE;
#define AFE_VDAC_OTHERS_0_CURRENT_CTRL_35_32_OTHERS_MASK (0xffffU)
#define AFE_VDAC_OTHERS_0_CURRENT_CTRL_35_32_OTHERS_SHIFT (0U)




typedef uint16_t AFE_HPF_TRIM_OTHERS_TYPE;
#define AFE_HPF_TRIM_OTHERS_RESERVED_MASK (0xfc00U)
#define AFE_HPF_TRIM_OTHERS_RESERVED_SHIFT (10U)
#define AFE_HPF_TRIM_OTHERS_RX_SAMPLE_WIDTH_MASK (0x380U)
#define AFE_HPF_TRIM_OTHERS_RX_SAMPLE_WIDTH_SHIFT (7U)
#define AFE_HPF_TRIM_OTHERS_IDAC_FINE_TUNE_MASK (0x70U)
#define AFE_HPF_TRIM_OTHERS_IDAC_FINE_TUNE_SHIFT (4U)
#define AFE_HPF_TRIM_OTHERS_SOFT_SEL_TRIM_HPF_MASK (0x8U)
#define AFE_HPF_TRIM_OTHERS_SOFT_SEL_TRIM_HPF_SHIFT (3U)
#define AFE_HPF_TRIM_OTHERS_TRIM_HPF_MASK (0x7U)
#define AFE_HPF_TRIM_OTHERS_TRIM_HPF_SHIFT (0U)




typedef uint16_t AFE_TX_EXTRA_CONFIG_0_TYPE;
#define AFE_TX_EXTRA_CONFIG_0_15_0_MASK (0xffffU)
#define AFE_TX_EXTRA_CONFIG_0_15_0_SHIFT (0U)




typedef uint16_t AFE_TX_EXTRA_CONFIG_1_TYPE;
#define AFE_TX_EXTRA_CONFIG_1_31_16_MASK (0xffffU)
#define AFE_TX_EXTRA_CONFIG_1_31_16_SHIFT (0U)




typedef uint16_t AFE_TX_EXTRA_CONFIG_2_TYPE;
#define AFE_TX_EXTRA_CONFIG_2_47_32_MASK (0xffffU)
#define AFE_TX_EXTRA_CONFIG_2_47_32_SHIFT (0U)




typedef uint16_t AFE_TEMPSEN_OTHERS_TYPE;
#define AFE_TEMPSEN_OTHERS_TEMPSEN_MASK (0xfffcU)
#define AFE_TEMPSEN_OTHERS_TEMPSEN_SHIFT (2U)
#define AFE_TEMPSEN_OTHERS_EXTRA_10BT_MASK (0x3U)
#define AFE_TEMPSEN_OTHERS_EXTRA_10BT_SHIFT (0U)




typedef uint16_t AFE_FUTURE_RSV_TYPE;
#define AFE_FUTURE_RSV_RSV_MASK (0xffffU)
#define AFE_FUTURE_RSV_RSV_SHIFT (0U)




typedef volatile struct COMP_PACKED _AFE_RDBType {
    AFE_RXCONFIG_0_TYPE rxconfig_0; /* OFFSET: 0x0 */
    AFE_RXCONFIG_1_TYPE rxconfig_1; /* OFFSET: 0x2 */
    AFE_RXCONFIG_2_TYPE rxconfig_2; /* OFFSET: 0x4 */
    AFE_RXCONFIG_3_TYPE rxconfig_3; /* OFFSET: 0x6 */
    AFE_RXCONFIG_4_TYPE rxconfig_4; /* OFFSET: 0x8 */
    AFE_RXCONFIG5_LP_TYPE rxconfig5_lp; /* OFFSET: 0xa */
    AFE_TX_CONFIG_0_TYPE tx_config_0; /* OFFSET: 0xc */
    AFE_TX_CONFIG_1_TYPE tx_config_1; /* OFFSET: 0xe */
    AFE_VDAC_ICTRL_0_TYPE vdac_ictrl_0; /* OFFSET: 0x10 */
    AFE_VDAC_ICTRL_1_TYPE vdac_ictrl_1; /* OFFSET: 0x12 */
    AFE_VDAC_ICTRL_2_TYPE vdac_ictrl_2; /* OFFSET: 0x14 */
    AFE_VDAC_OTHERS_0_TYPE vdac_others_0; /* OFFSET: 0x16 */
    AFE_HPF_TRIM_OTHERS_TYPE hpf_trim_others; /* OFFSET: 0x18 */
    AFE_TX_EXTRA_CONFIG_0_TYPE tx_extra_config_0; /* OFFSET: 0x1a */
    AFE_TX_EXTRA_CONFIG_1_TYPE tx_extra_config_1; /* OFFSET: 0x1c */
    AFE_TX_EXTRA_CONFIG_2_TYPE tx_extra_config_2; /* OFFSET: 0x1e */
    AFE_TEMPSEN_OTHERS_TYPE tempsen_others; /* OFFSET: 0x20 */
    AFE_FUTURE_RSV_TYPE future_rsv; /* OFFSET: 0x22 */
} AFE_RDBType;


#define AFE_BASE                        (0x410F23C0UL)



#define AFE_MAX_HW_ID                   (1UL)

#endif /* AFE_RDB_H */
