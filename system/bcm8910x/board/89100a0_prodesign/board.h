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

#ifndef INCLUDE_BCM89100A0_PRODESIGN_BOARD_H
#define INCLUDE_BCM89100A0_PRODESIGN_BOARD_H

#include <bcm8910x/board/common/board.h>
#include <i2c_osil.h>
#include <can.h>
#include <uart.h>
#include <mcu.h>
#include <osil/flash_osil.h>
#include <osil/flsmgr_osil.h>
#include <pinmux.h>
#include <wdt.h>
#include <arm.h>

#define LOG_UART_HW_ID          (0UL)
#define MPU_TABLE_SIZE_BOARD    (0UL)
extern const MPU_TableType MPU_BoardTable[MPU_TABLE_SIZE_BOARD];

extern const MCU_ConfigType         *mcu_cfg_table;

extern const IIC_CfgType                 *i2c_cfg_table;
extern const uint32_t                   i2c_cfg_table_sz;

extern const UART_TxCbType              *uart_tx_cb_table;
extern const uint32_t                   uart_tx_cb_table_sz;

extern const UART_RxCbType              *uart_rx_cb_table;
extern const uint32_t                   uart_rx_cb_table_sz;

extern const FLASH_CfgTblType           *flash_cfg_table;
extern const uint32_t                   flash_cfg_table_sz;
extern const MCU_ClkCfgIDType       *flash_clk_table;
extern const MCU_ClkCfgIDType       flash_clk_table_sz;

extern const UART_ConfigType            UartConfig;

extern const FLSMGR_CfgType * const     flsmgr_cfg_table;
extern const uint32_t                   flsmgr_cfg_table_sz;

extern const PINMUX_ConfigType          PINMUX_Config[];

extern const WDT_CfgType                WDT_Config[];

extern void Board_Init(void);

#endif /* INCLUDE_BCM89100A0_PRODESIGN_BOARD_H */
