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
 File Name: bl_piocfg.h
 Description: This file describes the pio config driver interface.
******************************************************************************/

#ifndef BL_PIOCFG_H
#define BL_PIOCFG_H

/* Includes */
#include <stdint.h>

#define RGMII_SPEED_100MBPS                                 (0x1UL)
#define RGMII_SPEED_1000MBPS                                (0x2UL)
#define CFG_AMAC_IO_CTRL_SRC_SYNC_MASK                      (0x00000004UL)
#define CFG_AMAC_IO_CTRL_SRC_SYNC_SHIFT                     (2UL)
#define CFG_AMAC_IO_CTRL_DEST_SYNC_MASK                     (0x00000008UL)
#define CFG_AMAC_IO_CTRL_DEST_SYNC_SHIFT                    (3UL)
#define CFG_AMAC_IO_CTRL_TX_CLK_OUT_MASK                    (0x00000010UL)
#define CFG_AMAC_IO_CTRL_TX_CLK_OUT_SHIFT                   (4UL)
#define CFG_AMAC_IO_CTRL_RGMII_MASK                         (0x00000020UL)
#define CFG_AMAC_IO_CTRL_RGMII_SHIFT                        (5UL)
#define CFG_AMAC_IO_CTRL_CLK_250_EN_MASK                    (0x00000040UL)
#define CFG_AMAC_IO_CTRL_CLK_250_EN_SHIFT                   (6UL)

typedef uint32_t BL_PIOCFG_PeriphIDType;   /**< Peripheral ID Type */
/* The IDs are mapped to IRQ line numbers */
#define BL_PIOCFG_ID_QSPI0         (42UL)  /**< QSPI 0 ID */
#define BL_PIOCFG_ID_QSPI1         (43UL)  /**< QSPI 1 ID */
#define BL_PIOCFG_ID_AVT           (46UL)  /**< AVT ID*/
#define BL_PIOCFG_ID_I2C0          (58UL)  /**< I2C Channel 0 ID */
#define BL_PIOCFG_ID_I2C1          (59UL)  /**< I2C Channel 1 ID */
#define BL_PIOCFG_ID_I2C2          (60UL)  /**< I2C Channel 2 ID */
#define BL_PIOCFG_ID_I2C3          (61UL)  /**< I2C Channel 3 ID */
#define BL_PIOCFG_ID_URT0          (65UL)  /**< UART Channel 0 ID */
#define BL_PIOCFG_ID_URT1          (66UL)  /**< UART Channel 1 ID */
#define BL_PIOCFG_ID_URT2          (67UL)  /**< UART Channel 2 ID */
#define BL_PIOCFG_ID_I2S           (68UL)  /**< I2S ID */
#define BL_PIOCFG_ID_TIM0_PCLK     (76UL)  /**< Timer Channel 0 PCLK ID */
#define BL_PIOCFG_ID_WDT_PCLK      (77UL)  /**< WatchDog Timer PCLK ID */
#define BL_PIOCFG_ID_CAN           (55UL)  /**< CAN ID */
#define BL_PIOCFG_ID_MMI           (90UL)  /**< MMI ID */

typedef uint32_t BL_PIOCFG_QSPIClkSrcType; /**< QSPI clock source selection type */
#define BL_PIOCFG_QSPI_CLK_SRC_25M     (0UL)   /**< 25MHz reference clock */
#define BL_PIOCFG_QSPI_CLK_SRC_100M    (1UL)   /**< 100MHz ACLK clock */
#define BL_PIOCFG_QSPI_CLK_SRC_200M    (2UL)   /**< 200MHz clock */
#define BL_PIOCFG_QSPI_CLK_SRC_400M    (3UL)   /**< 400MHz clock */

typedef uint32_t BL_PIOCFG_QSPIClkDivType; /**< QSPI clock divider selection */
#define BL_PIOCFG_QSPI_CLK_DIV_1       (0UL)   /**< QSPI clock divider 1 */
#define BL_PIOCFG_QSPI_CLK_DIV_2       (1UL)   /**< QSPI clock divider 2 */
#define BL_PIOCFG_QSPI_CLK_DIV_3       (2UL)   /**< QSPI clock divider 3 */
#define BL_PIOCFG_QSPI_CLK_DIV_4       (3UL)   /**< QSPI clock divider 4 */
#define BL_PIOCFG_QSPI_CLK_DIV_5       (4UL)   /**< QSPI clock divider 5 */
#define BL_PIOCFG_QSPI_CLK_DIV_6       (5UL)   /**< QSPI clock divider 6 */
#define BL_PIOCFG_QSPI_CLK_DIV_7       (6UL)   /**< QSPI clock divider 7 */
#define BL_PIOCFG_QSPI_CLK_DIV_8       (7UL)   /**< QSPI clock divider 8 */

extern void BL_PIOCFG_PeriphReset(BL_PIOCFG_PeriphIDType aId);

extern void BL_PIOCFG_PeriphPowerDown(BL_PIOCFG_PeriphIDType aId);

extern void BL_PIOCFG_PeriphPowerUpAll(void);

#endif /* BL_PIO_CFG_H */
