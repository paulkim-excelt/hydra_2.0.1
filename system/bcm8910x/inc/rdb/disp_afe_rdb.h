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
    @file disp_afe_rdb.h
    @brief RDB File for DISP_AFE

    @version 2018May25_rdb
*/

#ifndef DISP_AFE_RDB_H
#define DISP_AFE_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t DISP_AFE_LDO_CTRL_TYPE;
#define DISP_AFE_LDO_CTRL_CORERDY_MASK (0x400000UL)
#define DISP_AFE_LDO_CTRL_CORERDY_SHIFT (22UL)
#define DISP_AFE_LDO_CTRL_LDOCNTLEN_MASK (0x200000UL)
#define DISP_AFE_LDO_CTRL_LDOCNTLEN_SHIFT (21UL)
#define DISP_AFE_LDO_CTRL_LDORSTB_MASK (0x100000UL)
#define DISP_AFE_LDO_CTRL_LDORSTB_SHIFT (20UL)
#define DISP_AFE_LDO_CTRL_LP_EN_MASK (0x80000UL)
#define DISP_AFE_LDO_CTRL_LP_EN_SHIFT (19UL)
#define DISP_AFE_LDO_CTRL_LDO_PU_MASK (0x40000UL)
#define DISP_AFE_LDO_CTRL_LDO_PU_SHIFT (18UL)
#define DISP_AFE_LDO_CTRL_LDOCNTL_MASK (0x3ffffUL)
#define DISP_AFE_LDO_CTRL_LDOCNTL_SHIFT (0UL)




typedef uint32_t DISP_AFE_DATA_LANE_CTRL_TYPE;
#define DISP_AFE_DATA_LANE_CTRL_SPARE_CONTROL_MASK (0x800000UL)
#define DISP_AFE_DATA_LANE_CTRL_SPARE_CONTROL_SHIFT (23UL)
#define DISP_AFE_DATA_LANE_CTRL_SLVDS_PCLK_DUTY_CYCLE_SEL_MASK (0x400000UL)
#define DISP_AFE_DATA_LANE_CTRL_SLVDS_PCLK_DUTY_CYCLE_SEL_SHIFT (22UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_HS_CLK_EN_MASK (0x200000UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_HS_CLK_EN_SHIFT (21UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_RESET_MASK (0x100000UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_RESET_SHIFT (20UL)
#define DISP_AFE_DATA_LANE_CTRL_LANE_PD_MASK (0xf0000UL)
#define DISP_AFE_DATA_LANE_CTRL_LANE_PD_SHIFT (16UL)
#define DISP_AFE_DATA_LANE_CTRL_TERMRES_EN_MASK (0xf000UL)
#define DISP_AFE_DATA_LANE_CTRL_TERMRES_EN_SHIFT (12UL)
#define DISP_AFE_DATA_LANE_CTRL_PIO_TX_EN_MASK (0xf00UL)
#define DISP_AFE_DATA_LANE_CTRL_PIO_TX_EN_SHIFT (8UL)
#define DISP_AFE_DATA_LANE_CTRL_PD_MASK (0x80UL)
#define DISP_AFE_DATA_LANE_CTRL_PD_SHIFT (7UL)
#define DISP_AFE_DATA_LANE_CTRL_PD_CLK_MASK (0x40UL)
#define DISP_AFE_DATA_LANE_CTRL_PD_CLK_SHIFT (6UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_FS2X_EN_CLK_MASK (0x20UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_FS2X_EN_CLK_SHIFT (5UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_TERMRES_EN_CLK_MASK (0x10UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_TERMRES_EN_CLK_SHIFT (4UL)
#define DISP_AFE_DATA_LANE_CTRL_PIO_TX_EN_CLK_MASK (0x8UL)
#define DISP_AFE_DATA_LANE_CTRL_PIO_TX_EN_CLK_SHIFT (3UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_TXDDRCLK2EN_MASK (0x4UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_TXDDRCLK2EN_SHIFT (2UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_TXDDRCLKEN_MASK (0x2UL)
#define DISP_AFE_DATA_LANE_CTRL_DPHY_TXDDRCLKEN_SHIFT (1UL)
#define DISP_AFE_DATA_LANE_CTRL_PIOLANE_TX0_EN_MASK (0x1UL)
#define DISP_AFE_DATA_LANE_CTRL_PIOLANE_TX0_EN_SHIFT (0UL)




typedef uint32_t DISP_AFE_SLVDS_TX0_ACTRL_TYPE;
#define DISP_AFE_SLVDS_TX0_ACTRL_TX_DATA_OV_EN_MASK (0x8000000UL)
#define DISP_AFE_SLVDS_TX0_ACTRL_TX_DATA_OV_EN_SHIFT (27UL)
#define DISP_AFE_SLVDS_TX0_ACTRL_TX_DATA_OV_MASK (0x7f00000UL)
#define DISP_AFE_SLVDS_TX0_ACTRL_TX_DATA_OV_SHIFT (20UL)
#define DISP_AFE_SLVDS_TX0_ACTRL_TX0_EN_MASK (0x10000UL)
#define DISP_AFE_SLVDS_TX0_ACTRL_TX0_EN_SHIFT (16UL)
#define DISP_AFE_SLVDS_TX0_ACTRL_TX0_ACTRL_MASK (0xffffUL)
#define DISP_AFE_SLVDS_TX0_ACTRL_TX0_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_SLVDS_TX1_ACTRL_TYPE;
#define DISP_AFE_SLVDS_TX1_ACTRL_EN_MASK (0x10000UL)
#define DISP_AFE_SLVDS_TX1_ACTRL_EN_SHIFT (16UL)
#define DISP_AFE_SLVDS_TX1_ACTRL_ACTRL_MASK (0xffffUL)
#define DISP_AFE_SLVDS_TX1_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_SLVDS_TX2_ACTRL_TYPE;
#define DISP_AFE_SLVDS_TX2_ACTRL_EN_MASK (0x10000UL)
#define DISP_AFE_SLVDS_TX2_ACTRL_EN_SHIFT (16UL)
#define DISP_AFE_SLVDS_TX2_ACTRL_ACTRL_MASK (0xffffUL)
#define DISP_AFE_SLVDS_TX2_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_SLVDS_TX3_ACTRL_TYPE;
#define DISP_AFE_SLVDS_TX3_ACTRL_EN_MASK (0x10000UL)
#define DISP_AFE_SLVDS_TX3_ACTRL_EN_SHIFT (16UL)
#define DISP_AFE_SLVDS_TX3_ACTRL_ACTRL_MASK (0xffffUL)
#define DISP_AFE_SLVDS_TX3_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_SLVDS_TX_ACTRL_CLK_TYPE;
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_TX_DATA_CLK_MASK (0xff00000UL)
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_TX_DATA_CLK_SHIFT (20UL)
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_RESET_CLK_MASK (0x20000UL)
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_RESET_CLK_SHIFT (17UL)
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_TX_CLK_EN_MASK (0x10000UL)
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_TX_CLK_EN_SHIFT (16UL)
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_TX_ACTRLCLK_MASK (0xffffUL)
#define DISP_AFE_SLVDS_TX_ACTRL_CLK_TX_ACTRLCLK_SHIFT (0UL)




typedef uint32_t DISP_AFE_PIO_LANE0_ACTRL_TYPE;
#define DISP_AFE_PIO_LANE0_ACTRL_ACTRL_MASK (0xffUL)
#define DISP_AFE_PIO_LANE0_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_PIO_LANE1_ACTRL_TYPE;
#define DISP_AFE_PIO_LANE1_ACTRL_ACTRL_MASK (0xffUL)
#define DISP_AFE_PIO_LANE1_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_PIO_LANE2_ACTRL_TYPE;
#define DISP_AFE_PIO_LANE2_ACTRL_ACTRL_MASK (0xffUL)
#define DISP_AFE_PIO_LANE2_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_PIO_LANE3_ACTRL_TYPE;
#define DISP_AFE_PIO_LANE3_ACTRL_ACTRL_MASK (0xffUL)
#define DISP_AFE_PIO_LANE3_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_PIO_CLK_ACTRL_TYPE;
#define DISP_AFE_PIO_CLK_ACTRL_ACTRL_MASK (0xffUL)
#define DISP_AFE_PIO_CLK_ACTRL_ACTRL_SHIFT (0UL)




typedef uint32_t DISP_AFE_PIOLANE_LANE0_ACTRL_TYPE;
#define DISP_AFE_PIOLANE_LANE0_ACTRL_ACTRL_MASK (0xffUL)
#define DISP_AFE_PIOLANE_LANE0_ACTRL_ACTRL_SHIFT (0UL)




typedef volatile struct COMP_PACKED _DISP_AFE_RDBType {
    DISP_AFE_LDO_CTRL_TYPE ldo_ctrl; /* OFFSET: 0x0 */
    DISP_AFE_DATA_LANE_CTRL_TYPE data_lane_ctrl; /* OFFSET: 0x4 */
    DISP_AFE_SLVDS_TX0_ACTRL_TYPE slvds_tx0_actrl; /* OFFSET: 0x8 */
    DISP_AFE_SLVDS_TX1_ACTRL_TYPE slvds_tx1_actrl; /* OFFSET: 0xc */
    DISP_AFE_SLVDS_TX2_ACTRL_TYPE slvds_tx2_actrl; /* OFFSET: 0x10 */
    DISP_AFE_SLVDS_TX3_ACTRL_TYPE slvds_tx3_actrl; /* OFFSET: 0x14 */
    DISP_AFE_SLVDS_TX_ACTRL_CLK_TYPE slvds_tx_actrl_clk; /* OFFSET: 0x18 */
    DISP_AFE_PIO_LANE0_ACTRL_TYPE pio_lane0_actrl; /* OFFSET: 0x1c */
    DISP_AFE_PIO_LANE1_ACTRL_TYPE pio_lane1_actrl; /* OFFSET: 0x20 */
    DISP_AFE_PIO_LANE2_ACTRL_TYPE pio_lane2_actrl; /* OFFSET: 0x24 */
    DISP_AFE_PIO_LANE3_ACTRL_TYPE pio_lane3_actrl; /* OFFSET: 0x28 */
    DISP_AFE_PIO_CLK_ACTRL_TYPE pio_clk_actrl; /* OFFSET: 0x2c */
    DISP_AFE_PIOLANE_LANE0_ACTRL_TYPE piolane_lane0_actrl; /* OFFSET: 0x30 */
} DISP_AFE_RDBType;


#define DISP_AFE_BASE                   (0x400A1E00UL)



#define DISP_AFE_MAX_HW_ID              (1UL)


#define DISP_AFE_PIO_ACTRL_SLEW_SHIFT   (3UL)


#define DISP_AFE_LDO_EN                 (0x00748488UL)


#define DISP_AFE_LVDS_ACTRL_VAL         (0x000152C7UL)


#define DISP_AFE_DRIVE_STRENGTH_BT656   (6UL)


#define DISP_AFE_DRIVE_STRENGTH_DSI     (7UL)


#define DISP_AFE_DRIVE_SLEWRATE_BT656   (0UL)


#define DISP_AFE_DRIVE_SLEWRATE_DSI     (0UL)


#define DISP_AFE_PIO_ACTRL_BT656        (((DISP_AFE_DRIVE_SLEWRATE_BT656 << DISP_AFE_PIO_ACTRL_SLEW_SHIFT)   \
                                         | DISP_AFE_DRIVE_STRENGTH_BT656))


#define DISP_AFE_PIO_ACTRL_DSI          (((DISP_AFE_DRIVE_SLEWRATE_DSI << DISP_AFE_PIO_ACTRL_SLEW_SHIFT)   \
                                         | DISP_AFE_DRIVE_STRENGTH_DSI))

#endif /* DISP_AFE_RDB_H */