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
    @addtogroup grp_mcudrv_ifc
    @{

    @file clk.h
    @brief Clock interface
    This header file contains the interface for Clock

    @version 0.86 Imported from docx
*/

#ifndef BCM8910X_CLK_H
#define BCM8910X_CLK_H

/**
    @name Clock API IDs
    @{
    @brief API IDs for Clock
*/
#define BRCM_SWARCH_MCU_CLK_ID_MACRO                (0x21U) /**< @brief #MCU_CLK_ID_PLL_SYS  */
#define BRCM_SWARCH_MCU_CLK_CFG_ID_MACRO            (0x22U) /**< @brief #MCU_CLK_CFG_ID_DISP */
/** @} */

/**
    @name Clock IDs
    @{
    @brief Clock IDs for BCM8910X

    @trace  #BRCM_SWREQ_MCU_INIT
 */
#define MCU_CLK_ID_PLL_SYS          (0x1UL) /**< @brief System PLL clock ID */
#define MCU_CLK_ID_PLL_DISPLAY      (0x2UL) /**< @brief Display PLL clock ID */
#define MCU_CLK_ID_PLL_VID          (0x3UL) /**< @brief Video PLL clock ID */
#define MCU_CLK_ID_PLL_DDR          (0x4UL) /**< @brief DDR PLL clock ID */
#define MCU_CLK_ID_QSPI             (0x5UL) /**< @brief QSPI clock ID */
#define MCU_CLK_ID_NCO0             (0x6UL) /**< @brief NCO0 clock ID */
#define MCU_CLK_ID_NCO1             (0x7UL) /**< @brief NCO1 clock ID */
#define MCU_CLK_ID_NCO2             (0x8UL) /**< @brief NCO2 clock ID */
#define MCU_CLK_ID_MDIO             (0x9UL) /**< @brief MDIO clock ID */
#define MCU_CLK_ID_MAX              (MCU_CLK_ID_MDIO) /**< @brief Max clock ID */
/** @} */

/**
 * Clock configuration IDs
 * 0x00 -- 0x0F: Reserved for system pll settings
 * 0x10 -- 0x1F: reserved for display PLL settings
 * 0x20 -- 0x2F: reserved for video pll settings
 * 0x30 -- 0x3F: reserved for DDR pll settings
 * 0x40 -- 0x4F: reserved for QSPI0 clock setttings
 * 0x50 -- 0x5F: reserved for QSPI1 clock settings
 * 0x60 -- 0x6F: reserved for Camera clock settings
 * 0x70 -- 0x7F: reserved for Audio clock settings
 * 0x80 -- 0x8F: reserved for MDIO clock settings
 */

/**
    @name Clock Config IDs
    @{
    @brief Clock Config IDs for BCM8910X

    @trace  #BRCM_SWREQ_MCU_INIT
*/
#define MCU_CLK_CFG_ID_DISP                         (0x10UL)
#define MCU_CLK_CFG_ID_QSPI0_SRC400_80MHZ           (0x40UL)
#define MCU_CLK_CFG_ID_QSPI0_SRC400_66MHZ           (0x41UL)
#define MCU_CLK_CFG_ID_QSPI0_SRC400_50MHZ           (0x42UL)
#define MCU_CLK_CFG_ID_QSPI0_SRC200_25MHZ           (0x43UL)
#define MCU_CLK_CFG_ID_QSPI1_SRC400_80MHZ           (0x50UL)
#define MCU_CLK_CFG_ID_QSPI1_SRC400_66MHZ           (0x51UL)
#define MCU_CLK_CFG_ID_QSPI1_SRC400_50MHZ           (0x52UL)
#define MCU_CLK_CFG_ID_QSPI1_SRC200_25MHZ           (0x53UL)
#define MCU_CLK_CFG_ID_CAM                          (0x60UL)
#define MCU_CLK_CFG_ID_AUDIO_MCLK_0                 (0x70UL)
#define MCU_CLK_CFG_ID_AUDIO_MCLK_1                 (0x71UL)
#define MCU_CLK_CFG_ID_MDIO                         (0x80UL)
/** @} */

#endif /* BCM8910X_CLK_H */

/** @} */
