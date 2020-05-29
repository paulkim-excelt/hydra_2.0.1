/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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

#ifndef INCLUDE_BCM89103_EVK_BOARD_H
#define INCLUDE_BCM89103_EVK_BOARD_H

#include <bcm8910x/board/common/board.h>

#include <i2c_osil.h>
#include <can.h>
#include <uart.h>
#include <mcu.h>
#include <osil/flash_osil.h>
#include <osil/flsmgr_osil.h>
#include <pinmux.h>
#include <wdt.h>
#if defined(ENABLE_TIMER_SP804)
#include <gpt.h>
#endif

#define LOG_UART_HW_ID          (0UL)
#define MPU_TABLE_SIZE_BOARD    (0UL)

#define FLASH0_SIZE                     (2UL * 1024UL * 1024UL)
#define FLASH0_SUBSECTOR_SIZE           (4UL * 1024UL)
#define FLASH0_SPEED                    (FLASH_SPEED_80M)
#define FLASH0_READ_LANE_CFG            (FLASH_READ_LANE_QUAD)
#define FLASH0_FLASH_ID                 (FLASH_FLSID_WINBOND_W25Q16)
#define FLASH0_FLSABLE_SEC_START_ADDR   (1024UL * 1024UL)

#define FLASH1_SIZE                     (64UL * 1024UL * 1024UL)
#define FLASH1_SUBSECTOR_SIZE           (4UL * 1024UL)
#define FLASH1_SPEED                    (FLASH_SPEED_25M)
#define FLASH1_READ_LANE_CFG            (FLASH_READ_LANE_SINGLE)
#define FLASH1_FLASH_ID                 (FLASH_FLSID_MICRON_N25Q512A)
#define FLASH1_FLSABLE_SEC_START_ADDR   (1024UL * 1024UL)

#if (FLASH0_SPEED == FLASH_SPEED_80M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC400_80MHZ)
#elif (FLASH0_SPEED == FLASH_SPEED_66M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC400_66MHZ)
#elif (FLASH0_SPEED == FLASH_SPEED_50M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC400_50MHZ)
#elif (FLASH0_SPEED == FLASH_SPEED_25M)
#define FLASH0_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI0_SRC200_25MHZ)
#else
#error "Invalid flash1 speed"
#endif

#if (FLASH1_SPEED == FLASH_SPEED_80M)
#define FLASH1_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI1_SRC400_80MHZ)
#elif (FLASH1_SPEED == FLASH_SPEED_66M)
#define FLASH1_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI1_SRC400_66MHZ)
#elif (FLASH1_SPEED == FLASH_SPEED_50M)
#define FLASH1_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI1_SRC400_50MHZ)
#elif (FLASH1_SPEED == FLASH_SPEED_25M)
#define FLASH1_CLK_CFG_ID       (MCU_CLK_CFG_ID_QSPI1_SRC200_25MHZ)
#else
#error "Invalid flash1 speed"
#endif


extern const MCU_ConfigType         *mcu_cfg_table;

extern const IIC_ConfigType             IIC_Config[];
extern const uint32_t                   IIC_ConfigTblSz;

extern const FLASH_CfgTblType           *flash_cfg_table;
extern const uint32_t                   flash_cfg_table_sz;
extern const MCU_ClkCfgIDType       *flash_clk_table;
extern const MCU_ClkCfgIDType       flash_clk_table_sz;

extern const FLSMGR_CfgType * const     flsmgr_cfg_table;
extern const uint32_t                   flsmgr_cfg_table_sz;

extern const PINMUX_ConfigType          PINMUX_Config[];

extern const WDT_CfgType                WDT_Config[];

extern void Board_Init(void);
#endif /* INCLUDE_BCM89103_EVK_BOARD_H */
