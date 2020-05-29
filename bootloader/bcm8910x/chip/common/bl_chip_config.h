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

#ifndef BL_BCM8910X_CONFIG_H
#define BL_BCM8910X_CONFIG_H

#include <inttypes.h>
#include <cfg_rdb.h>
#include <uart_rdb.h>
#include <qspi_rdb.h>
#include <memc_chip_misc_rdb.h>
#include <amac_rdb.h>
#include <mdio_rdb.h>

#define BL_CPU_CLOCK           (400000000UL)   /* 400 MHz */
#define BL_NS_PER_CPU_CYCLE    (1000000000UL / BL_CPU_CLOCK)

/* chip version */
#define BL_CHIP_VERSION_A0     (0x0U)
#define BL_CHIP_VERSION_B0     (0x1U)

/* AVT */
#define BL_AVT_BASE            (AVT_MTSCTRL_0)
#define BL_AVT_CLOCK           (50000000UL)    /**< 50MHz */

#define BL_SRAM_REGION_START   (0x20000000)

#define BL_CNTRL_MAX_IMG_COUNT   (64UL)

/* UART */
#define BL_UART_MAX_HW_ID      (UART_MAX_HW_ID)
#define BL_URT0_BASE           (URT0_BASE)
#define BL_URT1_BASE           (URT1_BASE)
#define BL_URT2_BASE           (URT2_BASE)
#define BL_UART_CLOCK          (25000000UL)
#define BL_LOG_UART_HW_ID      (0UL)

/* Timer */
#define BL_TIM_MAX_CHAN_ID     (2UL)
#define BL_SP804_MAX_CHAN_ID   (2UL)
#define BL_SP804_0_BASE        (TIM_TIMER1LOAD)
#define BL_SP804_1_BASE        (TIM_TIMER2LOAD)
#define BL_TIMER_CLOCK         (50000000UL)
#define BL_TIM_CHANID_0        (0UL)
#define BL_TIM_LOAD_REG_OFFSET (0UL)
#define BL_TIM_CURR_REG_OFFSET (4UL)
#define BL_TIM_CTRL_REG_OFFSET (8UL)
#define BL_SP804_LOAD_TIME_REG (BL_SP804_0_BASE + BL_TIM_LOAD_REG_OFFSET)
#define BL_SP804_CURR_TIME_REG (BL_SP804_0_BASE + BL_TIM_CURR_REG_OFFSET)
#define BL_SP804_CTRL_REG      (BL_SP804_0_BASE + BL_TIM_CTRL_REG_OFFSET)

/* CFG */
#define BL_CFG_BASE            (CFG_BASE)

/* GPIO */
/** Maximum numer of GPIO ports */
#define BL_GPIO_MAX_PORTS      (2UL)
#define BL_GPIO_MAX_CHANNELS   (30UL)

#define BL_GPIO_FA0BASE        (GIO0_GPIO_FA0_DIN)
#define BL_GPIO_FA1BASE        (GIO1_GPIO_FA1_DIN)

/* AMAC */
#define BL_AMAC_BASE           (AMAC_BASE)

/* QSPI */
#define BL_QSPI_MAX_HW_ID      (QSPI_MAX_HW_ID)
#define BL_QSPI0_BASE          (QSPI0_BASE)
#define BL_QSPI1_BASE          (QSPI1_BASE)
#define BL_QSPI0_INT_NUM       (42UL)
#define BL_QSPI1_INT_NUM       (43UL)

/* FLASH */
#define BL_FLASH_MAX_HW_ID     (2UL)

/* ethernet */
#define BL_ETHER_HW_ID_MAX     (1UL)
#define BL_ETHER_CHAN_ID_MAX   (1UL)  /**< Maximum TX Channel ID */
#define BL_ETHER_TX_CHAN_CNT   (1UL)  /**< Maximum Ethernet TX channels */
#define BL_ETHER_RX_CHAN_CNT   (1UL)  /**< Maximum Ethernet RX channels */

/* BRPHY */
#define BL_BRPHY_1588_BASE         (TOP_1588_SLICE_ENABLE)
#define BL_BRPHY_MISCREG_BASE      (TOP_MISCREG_SPARE_REG_0)
#define BL_BRPHY_CORE_BASE         (CORE_BASE10)
#define BL_BRPHY_CL22_IEEE_BASE    (BRPHY0_BR_CL22_IEEE_MII_CTRL)

/* CPU */
#define BL_CPU_REG_BASE        (CPU_DCACHE_ERR)
#define BL_CHIP_MISC_BASE      (MEMC_CHIP_MISC_BASE)

/* MDIO */
#define BL_MDIO_BASE            (MDIO_BASE)

/* DDR */
#define BL_MEMC_HW_ID_0                     (0UL)

/* OTP */
#define BL_OTP_HW_ID_MAX                   (1UL)
#define BL_OTP_BASE                        (OTP_CONTROL)
#define BL_OTP_ADDR_START                  (0UL)
#define BL_OTP_DATA_MASK_WITH_ECC          (0x3FFFFFFUL)
#define BL_OTP_ADDR_END                    (255UL)
#define BL_OTP_ADDR_BRCM_REG_START         (12UL)
#define BL_OTP_ADDR_BRCM_REG_END           (63UL)
#define BL_OTP_ADDR_CUST_REG_START         (64UL)
#define BL_OTP_ADDR_CUST_REG_END           (BL_OTP_ADDR_END)
#define BL_OTP_SEC_BOOT_EN_ROW             (BL_OTP_ADDR_CUST_REG_START)
#define BL_OTP_SEC_BOOT_EN_MASK            (0x1UL)
#define BL_OTP_ADDR_RSA_PUB_START          (BL_OTP_ADDR_CUST_REG_START + 1UL)
#define BL_OTP_ECC_BITS                    (6UL)

/* security */
#define BL_OTP_SEC_HW_ID                   (0UL)
#define BL_OTP_SEC_BOOT_EN_ROW             (BL_OTP_ADDR_CUST_REG_START)
#define BL_OTP_SEC_BOOT_EN_MASK            (0x1UL)
#define BL_OTP_ADDR_RSA_PUB_START          (BL_OTP_ADDR_CUST_REG_START + 1UL)
/** RSA KEY BITS:
 * Each row has 32 OTP bits out of which 6 bits are reserved
 * for ECC (only rows starting after BRCM region)
 * Another bits are reserved to make it aligned at
 * byte boundary
 */
#define BL_RSA_KEY_BYTES_PER_ROW           (3UL)

/**
 * For C0 version, complete public key shall be present in the OTP
 */
#define BL_OTP_SECURE_KEY_SIZE                 (256UL)

#define BL_OTP_HW_ID_0                     (0UL)
#define BL_OTP_MAC_ADDR_0_OCTET123_ADDR    (151UL)
#define BL_OTP_MAC_ADDR_0_OCTET456_ADDR    (152UL)
#define BL_OTP_MAC_ADDR_1_OCTET123_ADDR    (153UL)
#define BL_OTP_MAC_ADDR_1_OCTET456_ADDR    (154UL)

extern uint16_t BL_TargetInfo;
extern uint16_t BL_HostInfo;
#define BL_DWNLD_TARGET_SPARE_REG  (&BL_TargetInfo)
#define BL_DWNLD_HOST_SPARE_REG    (&BL_HostInfo)

static MDIO_RDBType *const BL_MDIO_REGS = (MDIO_RDBType *const)MDIO_BASE;
static CFG_RDBType *const BL_CFG_RDB_REGS = (CFG_RDBType *const)CFG_BASE;
static MEMC_CHIP_MISC_RDBType *const BL_CHIP_MISC_RDB_REGS = (MEMC_CHIP_MISC_RDBType *const)MEMC_CHIP_MISC_BASE;

#endif /* BL_BCM8910X_CONFIG_H */
