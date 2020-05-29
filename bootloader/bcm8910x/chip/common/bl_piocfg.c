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

/******************************************************************************
 File Name: bl_piocfg.c
 Description: This file implements the pio config driver.
******************************************************************************/

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <bl_chip_config.h>
#include <bl_bcm_err.h>
#include <bl_utils.h>
#include <bl_ethernet.h>
#include <bl_eth_xcvr.h>
#include "bl_piocfg.h"
#include <cfg_rdb.h>

#define BL_INVAL_CFG                       (0xFFU)  /**< Invalid Configuration */
#define BL_PAD_GRP_MASK                    (0x7UL)   /**< Mask for pad group */

static CFG_RDBType *const BL_CFG_REGS = (CFG_RDBType * const)BL_CFG_BASE;

void BL_PIOCFG_PeriphReset(BL_PIOCFG_PeriphIDType aId)
{
    uint32_t mask = 0UL;

    switch (aId) {
    case BL_PIOCFG_ID_URT0:
        mask = CFG_PERIPH_RESET_URT_0_MASK;
        break;
    case BL_PIOCFG_ID_URT1:
        mask = CFG_PERIPH_RESET_URT_1_MASK;
        break;
    case BL_PIOCFG_ID_URT2:
        mask = CFG_PERIPH_RESET_URT_2_MASK;
        break;
    case BL_PIOCFG_ID_AVT:
        mask = CFG_PERIPH_RESET_AVT_MASK;
        break;
    case BL_PIOCFG_ID_I2C0:
        mask = CFG_PERIPH_RESET_I2C_0_MASK;
        break;
    case BL_PIOCFG_ID_I2C1:
        mask = CFG_PERIPH_RESET_I2C_1_MASK;
        break;
    case BL_PIOCFG_ID_I2C2:
        mask = CFG_PERIPH_RESET_I2C_2_MASK;
        break;
    case BL_PIOCFG_ID_I2C3:
        mask = CFG_PERIPH_RESET_I2C_3_MASK;
        break;
    case BL_PIOCFG_ID_CAN:
        mask = CFG_PERIPH_RESET_CAN_MASK;
        break;
    case BL_PIOCFG_ID_QSPI0:
        mask = CFG_PERIPH_RESET_QSPI_0_MASK;
        break;
    case BL_PIOCFG_ID_QSPI1:
        mask = CFG_PERIPH_RESET_QSPI_1_MASK;
        break;
    case BL_PIOCFG_ID_MMI:
        mask = CFG_PERIPH_RESET_MMI_MASK;
        break;
    default:
        break;
    }
    if (mask != 0UL) {
        BL_CFG_REGS->periph_reset |= mask;
        BL_CFG_REGS->periph_reset &= ~(mask);
    }
}

void BL_PIOCFG_PeriphPowerDown(BL_PIOCFG_PeriphIDType aId)
{
    uint32_t mask = 0UL;

    switch (aId) {
    case BL_PIOCFG_ID_TIM0_PCLK:
        mask = CFG_PERIPH_PWRDN_TIM0_REFDIV4_PCLKEN_MASK;
        break;
    case BL_PIOCFG_ID_WDT_PCLK:
        mask = CFG_PERIPH_PWRDN_WDT_REFDIV4_PCLKEN_MASK;
        break;
    default:
        break;
    }
    if (mask != 0UL) {
        BL_CFG_REGS->periph_pwrdn |= mask;
    }
}

void BL_PIOCFG_PeriphPowerUpAll(void)
{
    /* All peripherals are Power Up when bit is '0' but
       TIM0_REFDIV4_PCLKEN & WDT_REFDIV4_PCLKEN bit are '1' */
    BL_CFG_REGS->periph_pwrdn = (CFG_PERIPH_PWRDN_WDT_REFDIV4_PCLKEN_MASK
                                | CFG_PERIPH_PWRDN_TIM0_REFDIV4_PCLKEN_MASK);
}

int32_t BL_PIOCFG_SetEthIOCntrl(uint32_t aRGMIIEnable,
                                BL_ETHER_XcvrSpeedType aSpeed)
{
    uint32_t amacIOCntrl;
    uint32_t rgmiiCntrl;
    int32_t err = BL_BCM_ERR_OK;

    amacIOCntrl = CFG_AMAC_IO_CTRL_CLK_ENABLE_MASK|\
                  CFG_AMAC_IO_CTRL_SRC_SYNC_MASK|\
                  CFG_AMAC_IO_CTRL_DEST_SYNC_MASK;

    switch (aSpeed) {
    case BL_ETHER_XCVR_SPEED_10MBPS:
        /* 10 Mbps RGMII is not supported */
        if (BL_TRUE == aRGMIIEnable) {
            err = BL_BCM_ERR_NOSUPPORT;
        }
        break;
    case BL_ETHER_XCVR_SPEED_100MBPS:
        rgmiiCntrl = (CFG_RGMII_CTRL_EN_MASK |\
                CFG_RGMII_CTRL_BYPASS_IMP_2NS_DEL_MASK|\
                (RGMII_SPEED_100MBPS << CFG_RGMII_CTRL_SPEED_SHIFT));
        break;
    case BL_ETHER_XCVR_SPEED_1000MBPS:
        rgmiiCntrl = (CFG_RGMII_CTRL_EN_MASK |\
                CFG_RGMII_CTRL_BYPASS_IMP_2NS_DEL_MASK|\
                (RGMII_SPEED_1000MBPS << CFG_RGMII_CTRL_SPEED_SHIFT));

        amacIOCntrl = CFG_AMAC_IO_CTRL_CLK_ENABLE_MASK |\
                      CFG_AMAC_IO_CTRL_SRC_SYNC_MASK|\
                      CFG_AMAC_IO_CTRL_CLK_250_EN_MASK;
        amacIOCntrl |= (0xFUL << CFG_AMAC_IO_CTRL_AWUSER_SHIFT);
        amacIOCntrl |= (0xFUL << CFG_AMAC_IO_CTRL_ARUSER_SHIFT);
        break;
    default:
        err = BL_BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (BL_BCM_ERR_OK == err) {
        /* enable RGMII and configure RGMII speed */
        if (BL_TRUE == aRGMIIEnable) {
            BL_CFG_REGS->rgmii_ctrl = rgmiiCntrl;
        } else {
            /* make sure that RGMII is not enabled */
            BL_CFG_REGS->rgmii_ctrl = 0UL;
        }

        /* set AMAC clock configuration */
        BL_CFG_REGS->amac_io_ctrl = amacIOCntrl;
    }

    return err;
}
