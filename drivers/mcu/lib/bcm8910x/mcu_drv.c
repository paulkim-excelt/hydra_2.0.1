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

/**
    @addtogroup grp_mcudrv_impl
    @{

    @limitations None

    @file mcu_drv.c
    @brief MCU driver low level implementation
    This header file contains the MCU driver implementation

    @version 0.86 Imported from docx
*/

#include <inttypes.h>
#include <string.h>
#include <compiler.h>
#include <cortex_mx.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <clk.h>
#include <system.h>
#include <mcu.h>
#include <mcu_osil.h>

#include <avt.h>
#include "cfg_rdb.h"
#include "mcu_rdb.h"
#include "mcu_cpu_rdb.h"
#include "brphy_mii_rdb.h"

/**
    @name MCU Implementation Design IDs
    @{
    @brief Implementation Design IDs for MCU Driver
*/
#define BRCM_SWDSGN_MCU_CLK_CFG_MAX_MACRO                     (0x80U) /**< @brief #MCU_CLK_CFG_MAX */
#define BRCM_SWDSGN_MCU_RAM_CFG_MAX_MACRO                     (0x81U) /**< @brief #MCU_RAM_CFG_MAX */
#define BRCM_SWDSGN_MCU_DRV_CONFIG_TYPE                       (0x82U) /**< @brief #MCU_DrvConfigType */
#define BRCM_SWDSGN_MCU_DEV_TYPE                              (0x83U) /**< @brief #MCU_DevType */
#define BRCM_SWDSGN_MCU_CHIPMISC_REGS_GLOBAL                  (0x84U) /**< @brief #MCU_CHIPMISC_REGS */
#define BRCM_SWDSGN_MCU_CPU_REGS_GLOBAL                       (0x85U) /**< @brief #MCU_CPU_REGS */
#define BRCM_SWDSGN_MCU_DEV_GLOBAL                            (0x86U) /**< @brief #MCU_Dev */
#define BRCM_SWDSGN_MCU_DRV_CFG_GLOBAL                        (0x87U) /**< @brief #MCU_DrvCfg */
#define BRCM_SWDSGN_MCU_ISINITIALIZED_PROC                    (0x88U) /**< @brief #MCU_isInitialized */
#define BRCM_SWDSGN_MCU_DRV_QSPICLKINIT_PROC                  (0x89U) /**< @brief #MCU_DrvQspiClkInit */
#define BRCM_SWDSGN_MCU_DRV_CLKINITINT_PROC                   (0x8AU) /**< @brief #MCU_DrvClkInitInt */
#define BRCM_SWDSGN_MCU_DRV_PERIPHINIT_PROC                   (0x8BU) /**< @brief #MCU_DrvPeriphInit */
#define BRCM_SWDSGN_MCU_DRV_GETRESETMODE_PROC                 (0x8CU) /**< @brief #MCU_DrvGetResetMode */
#define BRCM_SWDSGN_MCU_DRV_SETRESETMODE_PROC                 (0x8DU) /**< @brief #MCU_DrvSetResetMode */
#define BRCM_SWDSGN_MCU_DRV_GETFWBOOTINFO_PROC                (0x8EU) /**< @brief #MCU_DrvGetFWBootInfo */
#define BRCM_SWDSGN_MCU_SDPADCFG_PROC                         (0x8FU) /**< @brief #MCU_SDPadCfg */
#define BRCM_SWDSGN_MCU_BT656PADCFG_PROC                      (0x90U) /**< @brief #MCU_BT656PadCfg */
#define BRCM_SWDSGN_MCU_DRV_SETDISPHSSRC_PROC                 (0x91U) /**< @brief #MCU_DrvSetDispHsSrc */
#define BRCM_SWDSGN_MCU_DRV_DISPHSTRIGGER_PROC                (0x92U) /**< @brief #MCU_DrvDispHsTrigger */
#define BRCM_SWDSGN_MCU_DRV_GETRESETREASON_PROC               (0x93U) /**< @brief #MCU_DrvGetResetReason */
#define BRCM_SWDSGN_MCU_DRV_GETRESETRAWVALUE_PROC             (0x94U) /**< @brief #MCU_DrvGetResetRawValue */
#define BRCM_SWDSGN_MCU_DRV_RESETREQ_PROC                     (0x95U) /**< @brief #MCU_DrvResetReq */
#define BRCM_SWDSGN_MCU_GETPLLSTATUS_PROC                     (0x96U) /**< @brief #MCU_GetPllStatus */
#define BRCM_SWDSGN_MCU_ACTIVATEPLLCLOCKS_PROC                (0x97U) /**< @brief #MCU_ActivatePllClocks */
#define BRCM_SWDSGN_MCU_CLKINIT_PROC                          (0x98U) /**< @brief #MCU_ClkInit */
#define BRCM_SWDSGN_MCU_RAMSECTIONINIT_PROC                   (0x99U) /**< @brief #MCU_RamSectionInit */
#define BRCM_SWDSGN_MCU_DRV_GETVERSION_PROC                   (0x9AU) /**< @brief #MCU_DrvGetVersion */
#define BRCM_SWDSGN_MCU_INIT_PROC                             (0x9BU) /**< @brief #MCU_Init */
#define BRCM_SWDSGN_MCU_GETCLKFREQ_PROC                       (0x9CU) /**< @brief #MCU_GetClkFreq */
#define BRCM_SWDSGN_MCU_DRV_ADJUSTCLKFREQ_PROC                (0x9DU) /**< @brief #MCU_DrvAdjustClkFreq */
#define BRCM_SWDSGN_MCU_SVCIO_TYPE                            (0x9EU) /**< @brief #MCU_SVCIOType */
#define BRCM_SWDSGN_MCU_SYSCMDHANDLER_PROC                    (0x9FU) /**< @brief #MCU_SysCmdHandler */
#define BRCM_SWDSGN_MCU_INTGETPLLSTATUS_PROC                  (0xA0U) /**< @brief #MCU_IntGetPllStatus */
#define BRCM_SWDSGN_MCU_DRV_GETBOOTTIMEINFO_PROC              (0xA1U) /**< @brief #MCU_DrvGetBootTimeInfo */
#define BRCM_SWDSGN_MCU_SETBOOTTIMEINFO_PROC                  (0xA2U) /**< @brief #MCU_SetBootTimeInfo */

#define BRCM_SWDSGN_MCU_PIOCFG_QSPICLKCTRL_TIMEOUT_CNT_MACRO (0x84U) /**< @brief #MCU_PIOCFG_QSPICLKCTRL_TIMEOUT_CNT */
#define BRCM_SWDSGN_MCU_PIOCFG_REGS_GLOBAL                   (0x86U) /**< @brief #MCU_PIOCFG_REGS */
#define BRCM_SWDSGN_MCU_PIOCFG_PERIPHRESET_PROC              (0x87U) /**< @brief #MCU_PiocfgPeriphReset */
#define BRCM_SWDSGN_MCU_PIOCFG_PERIPHPOWERUPALL_PROC         (0x89U) /**< @brief #MCU_PiocfgPeriphPowerUpAll */
#define BRCM_SWDSGN_MCU_PIOCFG_ENABLEQSPIINTERRUPT_PROC      (0x8BU) /**< @brief #MCU_PiocfgEnableQSPIInterrupt */
#define BRCM_SWDSGN_MCU_PIOCFG_DISABLEQSPIINTERRUPT_PROC     (0x8CU) /**< @brief #MCU_PiocfgDisableQSPIInterrupt */
#define BRCM_SWDSGN_MCU_PIOCFG_ENABLEALLQSPIINTERRUPT_PROC   (0x8DU) /**< @brief #MCU_PiocfgEnableAllQSPIInterrupt */
#define BRCM_SWDSGN_MCU_PIOCFG_DISABLEQSPIERRINTERRUPT_PROC  (0x8FU) /**< @brief #MCU_PiocfgDisableQSPIErrInterrupt */
#define BRCM_SWDSGN_MCU_PIOCFG_SETQSPICLK_PROC               (0x90U) /**< @brief #MCU_PiocfgSetQSPIClk */
#define BRCM_SWDSGN_MCU_PIOCFG_PERIPHID_TYPE                 (0x91U) /**< @brief #MCU_PiocfgPeriphIDType */
#define BRCM_SWDSGN_MCU_PIOCFG_QSPICLK_SRC_TYPE              (0x92U) /**< @brief #MCU_PiocfgQSPIClkSrcType */
#define BRCM_SWDSGN_MCU_PIOCFG_QSPICLK_DIV_TYPE              (0x93U) /**< @brief #MCU_PiocfgQSPIClkDivType */
#define BRCM_SWDSGN_MCU_BRPHY_REGS_GLOBAL                    (0x94U) /**< @brief #MCU_BRPHY_REGS */
#define BRCM_SWDSGN_MCU_SCRATCH_RAMDUMP_MODE_MASK_MACRO      (0x95U) /**< @brief #MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK */
#define BRCM_SWDSGN_MCU_GETBLBOOTINFO_PROC                   (0x96U) /**< @brief #MCU_GetBLBootInfo */
#define BRCM_SWDSGN_MCU_GETDEFAULTRESET_PROC                 (0x97U) /**< @brief #MCU_GetDefaultResetMode */
#define BRCM_SWDSGN_MCU_GETDWNLDMODE_PROC                    (0x98U) /**< @brief #MCU_GetDwnldMode */
#define BRCM_SWDSGN_MCU_DRV_SETFWBOOTINFO_PROC               (0x99U) /**< @brief #MCU_DrvSetFWBootInfo */
/** @} */

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
 */
#define MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK       (0x80000000UL) /**< Ramdump mode mask */

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
#define MCU_CLK_CFG_MAX             (30UL)

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
#define MCU_RAM_CFG_MAX             (10UL)

/**
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
#define MCU_PIOCFG_QSPICLKCTRL_TIMEOUT_CNT         (64UL)

/**
    @name MCU_PiocfgPeriphIDType
    @{
    @brief Peripheral ID type
    The IDs are mapped to IRQ line numbers

    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
typedef uint32_t MCU_PiocfgPeriphIDType;
#define MCU_PIOCFG_PERIPHID_TMR0          (36UL)  /**< @brief TMR0(CCT) ID */
#define MCU_PIOCFG_PERIPHID_QSPI0         (42UL)  /**< @brief QSPI 0 ID */
#define MCU_PIOCFG_PERIPHID_QSPI1         (43UL)  /**< @brief QSPI 1 ID */
#define MCU_PIOCFG_PERIPHID_AVT           (46UL)  /**< @brief AVT ID*/
#define MCU_PIOCFG_PERIPHID_CAN           (55UL)  /**< @brief CAN ID */
#define MCU_PIOCFG_PERIPHID_ADC           (57UL)  /**< @brief ADC ID */
#define MCU_PIOCFG_PERIPHID_I2C0          (58UL)  /**< @brief I2C Channel 0 ID */
#define MCU_PIOCFG_PERIPHID_I2C1          (59UL)  /**< @brief I2C Channel 1 ID */
#define MCU_PIOCFG_PERIPHID_I2C2          (60UL)  /**< @brief I2C Channel 2 ID */
#define MCU_PIOCFG_PERIPHID_I2C3          (61UL)  /**< @brief I2C Channel 3 ID */
#define MCU_PIOCFG_PERIPHID_SPI0          (62UL)  /**< @brief SPI 0 */
#define MCU_PIOCFG_PERIPHID_SPI1          (63UL)  /**< @brief SPI 1 */
#define MCU_PIOCFG_PERIPHID_SPI2          (64UL)  /**< @brief SPI 2 */
#define MCU_PIOCFG_PERIPHID_URT0          (65UL)  /**< @brief UART Channel 0 ID */
#define MCU_PIOCFG_PERIPHID_URT1          (66UL)  /**< @brief UART Channel 1 ID */
#define MCU_PIOCFG_PERIPHID_URT2          (67UL)  /**< @brief UART Channel 2 ID */
#define MCU_PIOCFG_PERIPHID_I2S           (68UL)  /**< @brief I2S ID */
#define MCU_PIOCFG_PERIPHID_TIM0_PCLK     (76UL)  /**< @brief Timer Channel 0 PCLK ID */
#define MCU_PIOCFG_PERIPHID_WDT_PCLK      (77UL)  /**< @brief WatchDog Timer PCLK ID */
#define MCU_PIOCFG_PERIPHID_PWM           (78UL)  /**< @brief PWM ID */
#define MCU_PIOCFG_PERIPHID_MMI           (90UL)  /**< @brief MMI ID */
/** @} */

/**
    @name MCU_PiocfgQSPIClkSrcType
    @{
    @brief QSPI clock source selection type

    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
typedef uint32_t MCU_PiocfgQSPIClkSrcType;
#define MCU_PIOCFG_QSPICLK_SRC_25M     (0UL)   /**< @brief 25MHz reference clock */
#define MCU_PIOCFG_QSPICLK_SRC_100M    (1UL)   /**< @brief 100MHz ACLK clock */
#define MCU_PIOCFG_QSPICLK_SRC_200M    (2UL)   /**< @brief 200MHz clock */
#define MCU_PIOCFG_QSPICLK_SRC_400M    (3UL)   /**< @brief 400MHz clock */
/** @} */

/**
    @name MCU_PiocfgQSPIClkDivType
    @{

    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
typedef uint32_t MCU_PiocfgQSPIClkDivType; /**< QSPI clock divider selection */
#define MCU_PIOCFG_QSPICLK_DIV_1       (0UL)   /**< @brief QSPI clock divider 1 */
#define MCU_PIOCFG_QSPICLK_DIV_2       (1UL)   /**< @brief QSPI clock divider 2 */
#define MCU_PIOCFG_QSPICLK_DIV_3       (2UL)   /**< @brief QSPI clock divider 3 */
#define MCU_PIOCFG_QSPICLK_DIV_4       (3UL)   /**< @brief QSPI clock divider 4 */
#define MCU_PIOCFG_QSPICLK_DIV_5       (4UL)   /**< @brief QSPI clock divider 5 */
#define MCU_PIOCFG_QSPICLK_DIV_6       (5UL)   /**< @brief QSPI clock divider 6 */
#define MCU_PIOCFG_QSPICLK_DIV_7       (6UL)   /**< @brief QSPI clock divider 7 */
#define MCU_PIOCFG_QSPICLK_DIV_8       (7UL)   /**< @brief QSPI clock divider 8 */
/** @} */

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
typedef struct _MCU_DrvConfigType{
    uint32_t                clkFailNotify;               /**< @brief clkFailNotify */
    MCU_ClkCfgType      clkCfgTbl[MCU_CLK_CFG_MAX];      /**< @brief clkCfgTbl */
    uint32_t                clkCfgTblSize;               /**< @brief clkCfgTblSize */
    MCU_RamSecCfgType   ramCfgTbl[MCU_RAM_CFG_MAX];      /**< @brief ramCfgTbl */
    uint32_t                ramCfgTblSize;               /**< @brief ramCfgTblSize */
} MCU_DrvConfigType;

/**
    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
    @trace  #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
typedef struct _MCU_DevType{
    uint32_t init;  /**< @brief driver Initialized */
} MCU_DevType;

/**
    @brief Misc Register base address

    @trace  #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
    @trace  #BRCM_SWARCH_MCU_BT656PADCFG_PROC
    @trace  #BRCM_SWARCH_MCU_SDPADCFG_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static MCU_RDBType* const MCU_CHIPMISC_REGS = (MCU_RDBType *)MCU_BASE;

/**
    @brief CPU Register base address

    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static MCU_CPU_RDBType* const MCU_CPU_REGS = (MCU_CPU_RDBType *)MCU_CPU_BASE;

/**
    @brief CPU Register base address

    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
static BRPHY_MII_RDBType* const MCU_BRPHY_REGS = (BRPHY_MII_RDBType *)BRPHY0_BR_CL22_IEEE_BASE;

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static CFG_RDBType *const MCU_PIOCFG_REGS = (CFG_RDBType * const)CFG_BASE;

/**
    @brief MCU Device

    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
    @trace  #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static MCU_DevType MCU_Dev COMP_SECTION(".data.drivers") = { .init = FALSE, };

/**
    @brief MCU Driver Configuration

    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static MCU_DrvConfigType MCU_DrvCfg COMP_SECTION(".data.drivers");

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static void MCU_PiocfgPeriphReset(MCU_PiocfgPeriphIDType aId)
{
    uint32_t mask = 0UL;

    switch (aId) {
    case MCU_PIOCFG_PERIPHID_URT0:
        mask = CFG_PERIPH_RESET_URT_0_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_URT1:
        mask = CFG_PERIPH_RESET_URT_1_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_URT2:
        mask = CFG_PERIPH_RESET_URT_2_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_AVT:
        mask = CFG_PERIPH_RESET_AVT_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_I2C0:
        mask = CFG_PERIPH_RESET_I2C_0_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_I2C1:
        mask = CFG_PERIPH_RESET_I2C_1_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_I2C2:
        mask = CFG_PERIPH_RESET_I2C_2_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_I2C3:
        mask = CFG_PERIPH_RESET_I2C_3_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_CAN:
        mask = CFG_PERIPH_RESET_CAN_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_QSPI0:
        mask = CFG_PERIPH_RESET_QSPI_0_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_QSPI1:
        mask = CFG_PERIPH_RESET_QSPI_1_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_I2S:
        mask = CFG_PERIPH_RESET_I2S_0_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_TMR0:
        mask = CFG_PERIPH_RESET_TMR0_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_ADC:
        mask = CFG_PERIPH_RESET_ADC_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_SPI0:
        mask = CFG_PERIPH_RESET_SPI_0_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_SPI1:
        mask = CFG_PERIPH_RESET_SPI_1_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_SPI2:
        mask = CFG_PERIPH_RESET_SPI_2_MASK;
        break;
    case MCU_PIOCFG_PERIPHID_MMI:
        mask = CFG_PERIPH_RESET_MMI_MASK;
        break;
    default:
        break;
    }
    if (mask != 0UL) {
        MCU_PIOCFG_REGS->periph_reset |= mask;
        MCU_PIOCFG_REGS->periph_reset &= ~(mask);
    }
}

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static void MCU_PiocfgPeriphPowerUpAll(void)
{
    /* All peripherals are Power Up when bit is '0' but
       TIM0_REFDIV4_PCLKEN & WDT_REFDIV4_PCLKEN bit are '1' */
    MCU_PIOCFG_REGS->periph_pwrdn = (CFG_PERIPH_PWRDN_WDT_REFDIV4_PCLKEN_MASK
                                | CFG_PERIPH_PWRDN_TIM0_REFDIV4_PCLKEN_MASK);
}

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static void MCU_PiocfgEnableQSPIInterrupt(MCU_PiocfgPeriphIDType aId, uint32_t aIntrMask)
{
    if (MCU_PIOCFG_PERIPHID_QSPI0 == aId) {
        MCU_PIOCFG_REGS->qspi_io_control_0 &= (~aIntrMask);
        MCU_PIOCFG_REGS->qspi_io_control_0 |= aIntrMask;
    }

    if (MCU_PIOCFG_PERIPHID_QSPI1 == aId) {
        MCU_PIOCFG_REGS->qspi_io_control_1 &= (~aIntrMask);
        MCU_PIOCFG_REGS->qspi_io_control_1 |= aIntrMask;
    }
}

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static void MCU_PiocfgDisableQSPIInterrupt(MCU_PiocfgPeriphIDType aId, uint32_t aIntrMask)
{
    if (MCU_PIOCFG_PERIPHID_QSPI0 == aId) {
        MCU_PIOCFG_REGS->qspi_io_control_0 &= (~aIntrMask);
    }

    if (MCU_PIOCFG_PERIPHID_QSPI1 == aId) {
        MCU_PIOCFG_REGS->qspi_io_control_1 &= (~aIntrMask);
    }
}

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static void MCU_PiocfgEnableAllQSPIInterrupt(MCU_PiocfgPeriphIDType aId)
{
    uint32_t intrMask = CFG_QSPI_IO_CONTROL_ENABLE_MSPI_HALT_SET_TRANSACTION_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_MSPI_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_OVERREAD_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_SESSION_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_IMPATIENT_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_TRUNCATED_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_FULLNESS_REACHED_MASK;

    MCU_PiocfgEnableQSPIInterrupt(aId, intrMask);
}

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static void MCU_PiocfgDisableQSPIErrInterrupt(MCU_PiocfgPeriphIDType aId)
{
    uint32_t intrMask = CFG_QSPI_IO_CONTROL_ENABLE_MSPI_HALT_SET_TRANSACTION_DONE_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_OVERREAD_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_IMPATIENT_MASK
                        | CFG_QSPI_IO_CONTROL_ENABLE_SPI_LR_TRUNCATED_MASK;

    MCU_PiocfgDisableQSPIInterrupt(aId, intrMask);
}

/**
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static int32_t MCU_PiocfgSetQSPIClk(MCU_PiocfgPeriphIDType aId,
                    MCU_PiocfgQSPIClkSrcType aClkSrc,
                    MCU_PiocfgQSPIClkDivType aClkDiv)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i = 0UL;

    if ((aClkSrc > MCU_PIOCFG_QSPICLK_SRC_400M)
        || (aClkDiv > MCU_PIOCFG_QSPICLK_DIV_8)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    switch (aId) {
    case MCU_PIOCFG_PERIPHID_QSPI0:
        MCU_PIOCFG_REGS->qspi_clk_ctrl &= (~(CFG_QSPI_CLK_CTRL_DIV_SEL_0_MASK
                                        | CFG_QSPI_CLK_CTRL_CLK_SEL_0_MASK));
        MCU_PIOCFG_REGS->qspi_clk_ctrl |=
                                ((aClkSrc << CFG_QSPI_CLK_CTRL_CLK_SEL_0_SHIFT)
                                 & CFG_QSPI_CLK_CTRL_CLK_SEL_0_MASK);
        MCU_PIOCFG_REGS->qspi_clk_ctrl |=
                                ((aClkDiv << CFG_QSPI_CLK_CTRL_DIV_SEL_0_SHIFT)
                                 & CFG_QSPI_CLK_CTRL_DIV_SEL_0_MASK);
        while(((MCU_PIOCFG_REGS->qspi_clk_ctrl & CFG_QSPI_CLK_CTRL_SELDIV_DONE_0_MASK)
                != CFG_QSPI_CLK_CTRL_SELDIV_DONE_0_MASK)
            && (i < MCU_PIOCFG_QSPICLKCTRL_TIMEOUT_CNT)) {
            i++;
        }
        break;
    case MCU_PIOCFG_PERIPHID_QSPI1:
        MCU_PIOCFG_REGS->qspi_clk_ctrl &= (~(CFG_QSPI_CLK_CTRL_DIV_SEL_1_MASK
                                        | CFG_QSPI_CLK_CTRL_CLK_SEL_1_MASK));
        MCU_PIOCFG_REGS->qspi_clk_ctrl |=
                                ((aClkSrc << CFG_QSPI_CLK_CTRL_CLK_SEL_1_SHIFT)
                                 & CFG_QSPI_CLK_CTRL_CLK_SEL_1_MASK);
        MCU_PIOCFG_REGS->qspi_clk_ctrl |=
                                ((aClkDiv << CFG_QSPI_CLK_CTRL_DIV_SEL_1_SHIFT)
                                 & CFG_QSPI_CLK_CTRL_DIV_SEL_1_MASK);
        while(((MCU_PIOCFG_REGS->qspi_clk_ctrl & CFG_QSPI_CLK_CTRL_SELDIV_DONE_1_MASK)
                != CFG_QSPI_CLK_CTRL_SELDIV_DONE_1_MASK)
                && (i < MCU_PIOCFG_QSPICLKCTRL_TIMEOUT_CNT)) {
            i++;
        }
        break;
    default:
        /* TODO: Add abort */
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }

    if ((BCM_ERR_OK == retVal) && (i == MCU_PIOCFG_QSPICLKCTRL_TIMEOUT_CNT)) {
        retVal = BCM_ERR_TIME_OUT;
    }
err:
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
    @trace  #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static uint32_t MCU_isInitialized(void)
{
    return MCU_Dev.init;
}

/**
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static int32_t MCU_DrvQspiClkInit(const MCU_ClkCfgType *const aCfg)
{
    MCU_PiocfgPeriphIDType periphID;
    MCU_PiocfgQSPIClkSrcType src;
    MCU_PiocfgQSPIClkDivType div;
    int32_t err = BCM_ERR_OK;

    switch(aCfg->cfgID) {
        case MCU_CLK_CFG_ID_QSPI0_SRC400_80MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI0;
            src = MCU_PIOCFG_QSPICLK_SRC_400M;
            div = MCU_PIOCFG_QSPICLK_DIV_5;
            break;
        case MCU_CLK_CFG_ID_QSPI0_SRC400_66MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI0;
            src = MCU_PIOCFG_QSPICLK_SRC_400M;
            div = MCU_PIOCFG_QSPICLK_DIV_6;
            break;
        case MCU_CLK_CFG_ID_QSPI0_SRC400_50MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI0;
            src = MCU_PIOCFG_QSPICLK_SRC_400M;
            div = MCU_PIOCFG_QSPICLK_DIV_8;
            break;
        case MCU_CLK_CFG_ID_QSPI0_SRC200_25MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI0;
            src = MCU_PIOCFG_QSPICLK_SRC_200M;
            div = MCU_PIOCFG_QSPICLK_DIV_8;
            break;
        case MCU_CLK_CFG_ID_QSPI1_SRC400_80MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI1;
            src = MCU_PIOCFG_QSPICLK_SRC_400M;
            div = MCU_PIOCFG_QSPICLK_DIV_5;
            break;
        case MCU_CLK_CFG_ID_QSPI1_SRC400_66MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI1;
            src = MCU_PIOCFG_QSPICLK_SRC_400M;
            div = MCU_PIOCFG_QSPICLK_DIV_6;
            break;
        case MCU_CLK_CFG_ID_QSPI1_SRC400_50MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI1;
            src = MCU_PIOCFG_QSPICLK_SRC_400M;
            div = MCU_PIOCFG_QSPICLK_DIV_8;
            break;
        case MCU_CLK_CFG_ID_QSPI1_SRC200_25MHZ:
            periphID = MCU_PIOCFG_PERIPHID_QSPI1;
            src = MCU_PIOCFG_QSPICLK_SRC_200M;
            div = MCU_PIOCFG_QSPICLK_DIV_8;
            break;
        default:
            err = BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (err == BCM_ERR_OK) {
        MCU_PiocfgSetQSPIClk(periphID, src, div);
    }
    return err;
}

/**
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static int32_t MCU_DrvClkInitInt(const MCU_ClkCfgType *const aCfg)
{
    int32_t err = BCM_ERR_OK;

    switch (aCfg->clkRef.clkID) {
    case MCU_CLK_ID_PLL_SYS:
        break;
    case MCU_CLK_ID_PLL_DISPLAY:
        /*
         * Write to the configuration register
         */
        MCU_CHIPMISC_REGS->clk_pinstrap = (uint16_t)aCfg->clkRef.cntrl[0];
        MCU_CHIPMISC_REGS->plldisp_ctrl_1 = (uint16_t)aCfg->clkRef.cntrl[1];
        MCU_CHIPMISC_REGS->plldisp_ctrl_2 = (uint16_t)aCfg->clkRef.cntrl[2];
        MCU_CHIPMISC_REGS->plldisp_ctrl_3 = (uint16_t)aCfg->clkRef.cntrl[3];
        MCU_CHIPMISC_REGS->plldisp_ndiv_frac = (uint16_t)aCfg->clkRef.cntrl[4];
        MCU_CHIPMISC_REGS->plldisp_ndiv = (uint16_t)aCfg->clkRef.cntrl[5];
        MCU_CHIPMISC_REGS->plldisp_cfg_1 = (uint16_t)aCfg->clkRef.cntrl[6];
        MCU_CHIPMISC_REGS->plldisp_cfg_2 = (uint16_t)aCfg->clkRef.cntrl[7];
        MCU_CHIPMISC_REGS->plldisp_cfg_3 = (uint16_t)aCfg->clkRef.cntrl[8];
        break;
    case MCU_CLK_ID_PLL_VID:
        break;
    case MCU_CLK_ID_PLL_DDR:
        break;
    case MCU_CLK_ID_QSPI:
        err = MCU_DrvQspiClkInit(aCfg);
        break;
    case MCU_CLK_ID_NCO0:
        /* Set frequency for NCO channel 0 */
        err = AVT_SetNCOFreq(AVT_NCO_CHANID_0, aCfg->clkRef.freq);
        break;
    case MCU_CLK_ID_NCO1:
        /* Set frequency for NCO channel 1 */
        err = AVT_SetNCOFreq(AVT_NCO_CHANID_1, aCfg->clkRef.freq);
        break;
    case MCU_CLK_ID_NCO2:
        /* Set frequency for NCO channel 2 */
        err = AVT_SetNCOFreq(AVT_NCO_CHANID_2, aCfg->clkRef.freq);
        break;
    default:
        err = BCM_ERR_INVAL_PARAMS;
        break;
    }
    return err;
}

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
static void MCU_DrvPeriphInit(void)
{
    /* Power up all peripherals */
    MCU_PiocfgPeriphPowerUpAll();

    /* Reset all peripherals */
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_URT0);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_URT1);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_URT2);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_AVT);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_I2C0);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_I2C1);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_I2C2);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_I2C3);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_CAN);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_QSPI0);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_QSPI1);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_I2S);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_TMR0);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_ADC);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_SPI0);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_SPI1);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_SPI2);
    MCU_PiocfgPeriphReset(MCU_PIOCFG_PERIPHID_MMI);
    MCU_PiocfgEnableAllQSPIInterrupt(MCU_PIOCFG_PERIPHID_QSPI0);
    MCU_PiocfgEnableAllQSPIInterrupt(MCU_PIOCFG_PERIPHID_QSPI1);
    MCU_PiocfgDisableQSPIErrInterrupt(MCU_PIOCFG_PERIPHID_QSPI0);
    MCU_PiocfgDisableQSPIErrInterrupt(MCU_PIOCFG_PERIPHID_QSPI1);
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
static MCU_ResetModeType MCU_DrvGetResetMode(void)
{
    uint32_t dwnldMode;
    MCU_ResetModeType resetMode;

    uint32_t reg =  MCU_CPU_REGS->dbg_scratch;
    dwnldMode = ((reg & MCU_FW_BOOT_INFO_DWNLD_MODE_MASK) >> MCU_FW_BOOT_INFO_DWNLD_MODE_SHIFT);
    if ((reg & MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK) != 0UL) {
        resetMode = MCU_RESETMODE_RAMDUMP;
    } else if (dwnldMode != 0UL) {
        switch (dwnldMode) {
        case MCU_FW_BOOT_INFO_DWNLD_MODE_TFTP:
            resetMode = MCU_RESETMODE_DWNLD_TFTP;
            break;
        case MCU_FW_BOOT_INFO_DWNLD_MODE_RAM:
            resetMode = MCU_RESETMODE_DWNLD_RAM;
            break;
        default:
            resetMode = MCU_RESETMODE_NONE;
            break;
        }
    } else {
        resetMode = MCU_RESETMODE_NONE;
    }

    return resetMode;
}

/**
    @trace  #BRCM_SWARCH_MCU_GETBLBOOTINFO_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
int32_t MCU_GetBLBootInfo(MCU_BLBootInfoType * const aBootInfo)
{
    return BCM_ERR_NOSUPPORT;
}

/**
    @trace  #BRCM_SWARCH_MCU_GETDEFAULTRESETMODE_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
MCU_ResetModeType MCU_GetDefaultResetMode(void)
{
    return MCU_RESETMODE_DWNLD_TFTP;
}

/**
    @trace  #BRCM_SWARCH_MCU_GETDWNLDMODE_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
MCU_DwnldModeType MCU_GetDwnldMode(void)
{
    MCU_DwnldModeType mode;
    uint32_t reg =  MCU_CPU_REGS->dbg_scratch;
    if ((reg & MCU_FW_BOOT_INFO_DWNLD_MODE_PARTIAL_MASK) != 0UL) {
        mode = MCU_DWNLD_MODE_PARTIAL;
    } else {
        mode = MCU_DWNLD_MODE_VIRGIN;
    }

    return mode;
}

/**
  TODO:
  @trace  #BRCM_SWARCH_MCU_DRVSETFWBOOTINFO_PROC
  @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
void MCU_DrvSetFWBootInfo(MCU_FWBootInfoType aBootInfo)
{
    MCU_CPU_REGS->dbg_scratch &= ~(MCU_SET_FW_BOOT_INFO_MASK);
    MCU_CPU_REGS->dbg_scratch |= (aBootInfo & MCU_SET_FW_BOOT_INFO_MASK);
}

/**
  @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
  @trace  #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8910X
*/
static void MCU_DrvSetResetMode(MCU_ResetModeType aResetMode)
{
    switch (aResetMode) {
    case MCU_RESETMODE_DWNL:
        MCU_CPU_REGS->dbg_scratch |= (MCU_FW_BOOT_INFO_SW_RESET_MASK
                | (MCU_FW_BOOT_INFO_DWNLD_MODE_TFTP << MCU_FW_BOOT_INFO_DWNLD_MODE_SHIFT));
        break;
    case MCU_RESETMODE_RAMDUMP:
        MCU_CPU_REGS->dbg_scratch |= MCU_DBG_SCRATCH_RAMDUMP_MODE_MASK;
        break;
    default:
        break;
    }
}

/**
  @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
  @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
static int32_t MCU_DrvGetFWBootInfo(MCU_FWBootInfoType *const aBootInfo)
{
    int32_t err = BCM_ERR_INVAL_PARAMS;
    if (NULL != aBootInfo) {
        *aBootInfo = MCU_CPU_REGS->dbg_scratch;
        err = BCM_ERR_OK;
    }
    return err;
}

/** @} */

/**
    @addtogroup grp_mcudrv_ifc
    @{
*/

/**
    @trace  #BRCM_SWARCH_MCU_SDPADCFG_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
void MCU_SDPadCfg(void)
{
    MCU_CHIPMISC_REGS->pad_cfg |= MCU_PAD_CFG_SDIO_MODEHV_MASK;
}

/**
    @trace  #BRCM_SWARCH_MCU_BT656PADCFG_PROC
    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
void MCU_BT656PadCfg(void)
{
    MCU_CHIPMISC_REGS->pad_cfg |= MCU_PAD_CFG_DISP_IF_EN_MASK;
    MCU_CHIPMISC_REGS->test_pin_sel_2 = MCU_TEST_PIN_SEL_2_GTX_CLK_MASK
        | MCU_TEST_PIN_SEL_2_TX_EN_MASK | MCU_TEST_PIN_SEL_2_TXD_3_MASK
        | MCU_TEST_PIN_SEL_2_TXD_2_MASK | MCU_TEST_PIN_SEL_2_TXD_1_MASK
        | MCU_TEST_PIN_SEL_2_TXD_0_MASK;
}

/**
  @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
  @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
static void MCU_DrvSetDispHsSrc(MCU_CpuDHSChType aChannel,
                          MCU_CpuDHSIDType aSrcId)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t mask = 0UL;

    if ((aChannel == MCU_CPUDHSCH_0) || (aChannel == MCU_CPUDHSCH_1)) {
        switch (aSrcId) {
        case MCU_CPUDHSID_CPU:
            if (aChannel == MCU_CPUDHSCH_0) {
                mask = MCU_CPU_DHSCTL_DHS0SELCPU_MASK;
            } else {
                mask = MCU_CPU_DHSCTL_DHS1SELCPU_MASK;
            }

            break;
        case MCU_CPUDHSID_DISPLAY:
            if (aChannel == MCU_CPUDHSCH_0) {
                mask = MCU_CPU_DHSCTL_DHS0SELDISPLAY_MASK;
            } else {
                mask = MCU_CPU_DHSCTL_DHS1SELDISPLAY_MASK;
            }
            break;
        case MCU_CPUDHSID_ISP:
            if (aChannel == MCU_CPUDHSCH_0) {
                mask = MCU_CPU_DHSCTL_DHS0SELIMG_MASK;
            } else {
                mask = MCU_CPU_DHSCTL_DHS1SELIMG_MASK;
            }
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != ret) {
        goto exit;
    }

    /* Before setting the bits, clear it first */
    if (MCU_CPUDHSCH_0 == aChannel) {
        MCU_CPU_REGS->dhs_ctl &= ~MCU_CPU_DHS_CTL_0_SEL_MASK;
        MCU_CPU_REGS->dhs_ctl |= mask;
    } else {
        MCU_CPU_REGS->dhs_ctl &= ~MCU_CPU_DHS_CTL_1_SEL_MASK;
        MCU_CPU_REGS->dhs_ctl |= mask;
    }
exit:
    return;
}

/**
  @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
  @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
static void MCU_DrvDispHsTrigger(MCU_CpuDHSChType aChannel, uint32_t aBufAttr)
{
    switch (aChannel) {
    case MCU_CPUDHSCH_0:
        MCU_CPU_REGS->dhs_ctl &= ~(MCU_CPU_DHS_CTL_0_BA_MASK
                                    | MCU_CPU_DHS_CTL_0_ACK_MASK
                                    | MCU_CPU_DHS_CTL_0_READY_MASK);
        MCU_CPU_REGS->dhs_ctl |= (((aBufAttr << MCU_CPU_DHS_CTL_0_BA_SHIFT)
                    & MCU_CPU_DHS_CTL_0_BA_MASK) | MCU_CPU_DHS_CTL_0_READY_MASK);
        break;
    case MCU_CPUDHSCH_1:
        MCU_CPU_REGS->dhs_ctl &= ~(MCU_CPU_DHS_CTL_1_BA_MASK
                                    | MCU_CPU_DHS_CTL_1_ACK_MASK
                                    | MCU_CPU_DHS_CTL_1_READY_MASK);
        MCU_CPU_REGS->dhs_ctl |= (((aBufAttr << MCU_CPU_DHS_CTL_1_BA_SHIFT)
                    & MCU_CPU_DHS_CTL_1_BA_MASK) | MCU_CPU_DHS_CTL_1_READY_MASK);
        break;
    default:
        break;
    }
}

/**
  @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
  @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
static MCU_ResetReasonType MCU_DrvGetResetReason(void)
{
    MCU_ResetReasonType reason;
    uint32_t dbgScratchReg = MCU_CPU_REGS->dbg_scratch;
    uint32_t nmiStatusReg = MCU_CPU_REGS->nmi_sts_shd;

    if (MCU_isInitialized() == TRUE) {
        if ((nmiStatusReg & MCU_CPU_NMI_STS_SHD_WDOG_SHD_MASK)
                == MCU_CPU_NMI_STS_SHD_WDOG_SHD_MASK) {
            reason = MCU_RESETREASON_WD;
        } else {
            if (dbgScratchReg == 0UL) {
                reason = MCU_RESETREASON_POWER_ON;
            } else if ((dbgScratchReg & MCU_FW_BOOT_INFO_SW_RESET_MASK) != 0UL) {
                reason = MCU_RESETREASON_SW;
            } else {
                reason = MCU_RESETREASON_UNDEFINED;
            }
        }
    } else {
        reason = MCU_RESETREASON_UNDEFINED;
    }

    return reason;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
static uint32_t MCU_DrvGetResetRawValue(void)
{
    return MCU_CPU_REGS->dbg_scratch;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8910X
*/
static void MCU_DrvResetReq(MCU_ResetReqType aResetReq)
{
    if (MCU_isInitialized() == TRUE) {
        switch (aResetReq) {
        case MCU_RESETREQ_GLOBAL:
            MCU_CPU_REGS->dbg_scratch |= MCU_FW_BOOT_INFO_SW_RESET_MASK;
            CORTEX_MX_SystemReset();
            break;
        case MCU_RESETREQ_LOCAL:
            MCU_CPU_REGS->dbg_scratch |= MCU_FW_BOOT_INFO_SW_RESET_MASK;
            CORTEX_MX_VectorReset();
            break;
        default:
            break;
        }
    }
}

/**
    @trace  #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
    @trace  #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
MCU_PllStatusType MCU_IntGetPllStatus(void)
{
    MCU_PllStatusType status = MCU_PLLSTATUS_UNDEFINED;
    uint32_t sysPllLock = FALSE;
    uint32_t dispPllLock = FALSE;
    uint32_t vidPllLock = FALSE;
    uint16_t reg;

    if (MCU_isInitialized() == TRUE) {
        reg = MCU_CHIPMISC_REGS->pllsys_status;
        if ((reg & MCU_PLLSYS_STATUS_LOCK_MASK) != 0U) {
            sysPllLock = TRUE;
        }

        reg = MCU_CHIPMISC_REGS->plldisp_status;
        if ((reg & MCU_PLLDISP_STATUS_LOCK_MASK) != 0U) {
            dispPllLock = TRUE;
        }

        reg = MCU_CHIPMISC_REGS->pllvid_status;
        if ((reg & MCU_PLLVID_STATUS_LOCK_MASK) != 0U) {
            vidPllLock = TRUE;
        }
        if ((sysPllLock == TRUE) && (dispPllLock == TRUE) &&
                (vidPllLock == TRUE)) {
            status = MCU_PLLSTATUS_LOCKED;
        } else {
            status = MCU_PLLSTATUS_UNLOCKED;
        }
    }
    return status;
}

/**
    @trace  #BRCM_SWARCH_MCU_GETPLLSTATUS_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
MCU_PllStatusType MCU_GetPllStatus(void)
{
    return MCU_IntGetPllStatus();
}

/**
    @trace  #BRCM_SWARCH_MCU_ACTIVATEPLLCLOCKS_PROC
    @trace  #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8910X
*/
int32_t MCU_ActivatePllClocks(void)
{
    int32_t err = BCM_ERR_OK;

    if (MCU_isInitialized() == TRUE) {
        if (MCU_IntGetPllStatus() != MCU_PLLSTATUS_LOCKED) {
            err = BCM_ERR_INVAL_STATE;
        }
    } else {
        err = BCM_ERR_UNINIT;
    }
    return err;
}

/**
    @trace  #BRCM_SWARCH_MCU_CLKINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
int32_t MCU_ClkInit(MCU_ClkCfgIDType aCfgID)
{
    int err = BCM_ERR_OK;
    uint32_t i;

    if (MCU_isInitialized() == TRUE) {
        /* lookup if aCfgID is present
         * in MCU configuration data
         */
        for (i = 0UL; i < MCU_DrvCfg.clkCfgTblSize; i++) {
            if (MCU_DrvCfg.clkCfgTbl[i].cfgID == aCfgID) {
                err = MCU_DrvClkInitInt(&MCU_DrvCfg.clkCfgTbl[i]);
                break;
            }
        }
    } else {
        err = BCM_ERR_UNINIT;
    }
    return err;
}

/**
    @trace  #BRCM_SWARCH_MCU_RAMSECTIONINIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
void MCU_RamSectionInit(MCU_RamSectionIDType aID)
{
    if (MCU_isInitialized() == TRUE) {
        if (aID < MCU_DrvCfg.ramCfgTblSize) {
            BCM_MemSet(MCU_DrvCfg.ramCfgTbl[aID].baseAddr,
                        (int32_t)MCU_DrvCfg.ramCfgTbl[aID].defaultVal,
                       MCU_DrvCfg.ramCfgTbl[aID].size);
        }
    }
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
static void MCU_DrvGetVersion(MCU_VersionType *const aVersion)
{

    uint16_t lsb;
    uint16_t msb;

    lsb = MCU_BRPHY_REGS->phy_id_lsb;
    msb = MCU_BRPHY_REGS->phy_id_msb;

    aVersion->rev = (uint32_t)((lsb & BRPHY_MII_PHY_ID_LSB_REVISION_MASK) >>
            BRPHY_MII_PHY_ID_LSB_REVISION_SHIFT);
    aVersion->model = ((lsb & BRPHY_MII_PHY_ID_LSB_MODEL_MASK) >>
            BRPHY_MII_PHY_ID_LSB_MODEL_SHIFT);
    aVersion->manuf = (msb & BRPHY_MII_PHY_ID_MSB_OUI_MSB_MASK);
    aVersion->manuf |= ((lsb & BRPHY_MII_PHY_ID_LSB_OUI_LSB_MASK) >>
            BRPHY_MII_PHY_ID_LSB_OUI_LSB_SHIFT) << 19U;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
	@trace  #BRCM_SWARCH_MCU_GETBOOTTIMEINFO_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
static int32_t MCU_DrvGetBootTimeInfo(MCU_BootTimeInfoType *const aBootTime)
{
    int32_t retVal = BCM_ERR_OK;

    if (NULL != aBootTime) {
        aBootTime->blBootTime = MCU_CHIPMISC_REGS->spare_reg_0 & 0xFFU;
        aBootTime->xcvrInitTime = (MCU_CHIPMISC_REGS->spare_reg_0 & 0xFF00U) >> 8U;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
	@trace #BRCM_SWARCH_MCU_SETBOOTTIMEINFO_PROC
    @trace #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8910X
*/
int32_t MCU_SetBootTimeInfo(const MCU_BootTimeInfoType *const aBootTime)
{
    int32_t retVal = BCM_ERR_OK;

    if (NULL != aBootTime) {
        MCU_CHIPMISC_REGS->spare_reg_0 = ((aBootTime->blBootTime & 0xFFU) |
                ((aBootTime->xcvrInitTime & 0xFFU) << 8U));
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_MCU_INIT_PROC
    @trace  #BRCM_SWREQ_MCU_INIT_BCM8910X
*/
void MCU_Init(const MCU_ConfigType *const aConfig)
{
    if (MCU_isInitialized() == FALSE) {
        if (aConfig != NULL) {
            BCM_MemSet(&MCU_DrvCfg, 0x0, sizeof(MCU_DrvCfg));
            if ((aConfig->clkCfgTbl != NULL) && (aConfig->clkCfgTblSize > 0UL) &&
                    (aConfig->clkCfgTblSize < MCU_CLK_CFG_MAX)) {
                /* copy the clock configuration table */
                BCM_MemCpy(&MCU_DrvCfg.clkCfgTbl[0], aConfig->clkCfgTbl,
                        aConfig->clkCfgTblSize * sizeof(MCU_ClkCfgType));
               MCU_DrvCfg.clkCfgTblSize = aConfig->clkCfgTblSize;
               MCU_DrvCfg.clkFailNotify = aConfig->clkSrcFailNotification;
            }
            if ((aConfig->ramCfgTbl != NULL) && (aConfig->ramCfgTblSize > 0UL) &&
                    (aConfig->ramCfgTblSize < MCU_RAM_CFG_MAX)) {
                BCM_MemCpy(&MCU_DrvCfg.ramCfgTbl[0], aConfig->ramCfgTbl,
                        aConfig->ramCfgTblSize * sizeof(MCU_RamSecCfgType));
               MCU_DrvCfg.ramCfgTblSize = aConfig->ramCfgTblSize;
            }
        }

        MCU_DrvPeriphInit();
        MCU_Dev.init = TRUE;
    }
}

/**
    @trace  #BRCM_SWARCH_MCU_GETCLKFREQ_PROC
    @trace  #BRCM_SWREQ_MCU_QUERY_BCM8910X
*/
uint32_t MCU_GetClkFreq(MCU_ClkCfgIDType aCfgID)
{
    uint32_t i;
    uint32_t clkFreq;

    clkFreq = 0UL;


    if (MCU_isInitialized() != TRUE) {
        goto err;
    }

    for (i = 0UL; i < MCU_DrvCfg.clkCfgTblSize; ++i) {
        if (MCU_DrvCfg.clkCfgTbl[i].cfgID == aCfgID) {
            clkFreq = MCU_DrvCfg.clkCfgTbl[i].clkRef.freq;
            break;
        }
    }

err:
    return clkFreq;
}

/**
    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_DYNAMIC_CONFIG_BCM8910X
*/
static int32_t MCU_DrvAdjustClkFreq(MCU_ClkCfgIDType aCfgID, int32_t aCorrection)
{
    int32_t ret;
    uint32_t i;

    ret = BCM_ERR_INVAL_PARAMS;

    if (MCU_isInitialized() != TRUE) {
        ret = BCM_ERR_UNINIT;
        goto err;
    }

    for (i = 0UL; i < MCU_DrvCfg.clkCfgTblSize; ++i) {
        if (MCU_DrvCfg.clkCfgTbl[i].cfgID == aCfgID) {
            ret = BCM_ERR_OK;
            break;
        }
    }

    if (BCM_ERR_OK == ret) {
        switch(MCU_DrvCfg.clkCfgTbl[i].clkRef.clkID) {
        case MCU_CLK_ID_NCO0:
            ret = AVT_TuneNCOFreq(AVT_NCO_CHANID_0, aCorrection);
            break;

        case MCU_CLK_ID_NCO1:
            ret = AVT_TuneNCOFreq(AVT_NCO_CHANID_1, aCorrection);
            break;

        case MCU_CLK_ID_NCO2:
            ret = AVT_TuneNCOFreq(AVT_NCO_CHANID_2, aCorrection);
            break;

        default:
            ret = BCM_ERR_NOSUPPORT;
            break;
        }
    }

err:
    return ret;
}


/**
    @brief Union to avoid MISRA Required error for Type conversion

    @trace  #BRCM_SWARCH_MCU_IO_TYPE
    @trace  #BRCM_SWREQ_MCU_KERNEL_INTERFACE
*/
typedef union _MCU_SVCIOType {
    uint8_t *data;
    MCU_IOType *io;
} MCU_SVCIOType;

/**
    @code{.c}
    if aSysIO.mcuIO is not NULL
        if aMagicID is MCU_SVC_MAGIC_ID
            aSysIO.mcuIO.retVal = MCU_CmdHandler(aCmd, aSysIO.mcuIO)
        else
            aSysIO.mcuIO.retVal = BCM_ERR_INVAL_MAGIC
    @endcode

    @trace  #BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_MCU_KERNEL_HANDLER_BCM8910X
*/
void MCU_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    MCU_SVCIOType mcu;
    mcu.data = aSysIO;
    int32_t ret = BCM_ERR_OK;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_MCU_ID) {
            switch (aCmd) {
            case MCU_CMD_RESET_REQ:
                MCU_DrvResetReq(mcu.io->resetReq);
                break;
            case MCU_CMD_GET_RESET_REASON:
                mcu.io->resetReason = MCU_DrvGetResetReason();
                break;
            case MCU_CMD_GET_RAW_VAL:
                mcu.io->resetRaw = MCU_DrvGetResetRawValue();
                break;
            case MCU_CMD_SET_LPM_MODE:
                break;
            case MCU_CMD_SET_DHS_SRC:
                MCU_DrvSetDispHsSrc(mcu.io->channel, mcu.io->srcID);
                break;
            case MCU_CMD_DHS_TRIGGER:
                MCU_DrvDispHsTrigger(mcu.io->channel, mcu.io->bufAttr);
                break;
            case MCU_CMD_GET_VERSION:
                MCU_DrvGetVersion(mcu.io->version);
                break;
            case MCU_CMD_GET_RESET_MODE:
                mcu.io->resetMode = MCU_DrvGetResetMode();
                break;
            case MCU_CMD_SET_RESET_MODE:
                MCU_DrvSetResetMode(mcu.io->resetMode);
                break;
            case MCU_CMD_GET_FW_BOOT_INFO:
                ret = MCU_DrvGetFWBootInfo(mcu.io->bootInfo);
                break;
            case MCU_CMD_ADJUST_CLK:
                ret = MCU_DrvAdjustClkFreq(mcu.io->clkCfgID, mcu.io->correction);
                break;
            case MCU_CMD_GET_BOOT_TIME:
                ret = MCU_DrvGetBootTimeInfo(mcu.io->bootTime);
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
            mcu.io->retVal = ret;
        } else {
            ret = BCM_ERR_INVAL_MAGIC;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != ret) {
        const uint32_t values[4UL] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_MCU_ID, 0U, BRCM_SWARCH_MCU_SYSCMDHANDLER_PROC, ret, 4UL, values);
    }
}

/** @} */
