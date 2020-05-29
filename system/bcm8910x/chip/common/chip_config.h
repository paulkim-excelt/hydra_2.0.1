/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
#ifndef BCM8910X_CONFIG_H
#define BCM8910X_CONFIG_H

#include <dma330_rdb.h>
#include <spi_rdb.h>
#include <sd_host_ip_rdb.h>
#include <sd_host_rdb.h>

#define CPU_CLOCK                 (400000000UL)  /**< 400 MHz */
#define NS_PER_CPU_CYCLE          (1000000000UL / CPU_CLOCK)
#define TIM_MAX_CHAN_ID           (2UL)  /**< TODO: Remove */
#define SP804_MAX_CHAN_ID         (2UL)  /**< TODO: Remove */
#define SP804_0_BASE              (TIM_TIMER1LOAD)  /**< TODO: Remove */
#define SP804_1_BASE              (TIM_TIMER2LOAD)  /**< TODO: Remove */
#define WDT_MAX_HW_ID             (1UL)  /**< TODO: Remove */
#define WDT0_BASE                 (WDT0_WDOGLOAD)  /**< TODO: Remove */
#define PWM_MAX_CHAN_CNT          (4UL)
#define MCAN_MAX_HW_ID            (1UL)  /**< TODO: Remove */
#define MCAN_BASE                 (CAN_CAN0_CREL)  /**< TODO: Remove */
#define MCAN_MSG_RAM_SIZE         (2048UL)
#define LIN_MAX_HW_ID             (UART_MAX_HW_ID)  /**< TODO: Remove */
#define ADC_MAX_HW_ID             (1UL)  /**< TODO: Remove */
#define ADC0_BASE                 (ADC_ADC_CTRL)  /**< TODO: Remove */
#define IIC_MAX_HW_ID             (4UL)  /**< TODO: Remove */
#define CHIP_VERSION_A0           (0x0UL)
#define CHIP_VERSION_B0           (0x1UL)
#define DMA_BASE                  (DMA330_BASE)  /**< TODO: Remove */
#define DMA_PERIPH_UART0_RX       (13U)
#define DMA_PERIPH_UART0_TX       (12U)
#define DMA_PERIPH_I2S1_RX        (4U )
#define DMA_PERIPH_I2S0_RX        (3U )
#define DMA_PERIPH_I2S1_TX        (1U )
#define DMA_PERIPH_I2S0_TX        (0U )
#define CCT_MAX_CHAN_CNT          (4UL)  /**< TODO: Remove */
#define CCT_BASE                  (TMR_CCT_CTRLA)  /**< TODO: Remove */
#define GPIO_MAX_PORTS            (2UL)
#define GPIO_FA0BASE              (GIO0_GPIO_FA0_DIN)  /**< TODO: Remove */
#define GPIO_FA1BASE              (GIO1_GPIO_FA1_DIN)  /**< TODO: Remove */
#define CMP_BASE                  (CMP_0_REVISION)  /**< Compositor TODO: Remove */
#define MFD_BASE                  (MFD_0_REVISION_ID)  /**< MPEG video feeder TODO: Remove */
#define GFD_BASE                  (GFD_0_REVISION)  /**< Graphics feeder TODO: Remove */
#define PV_BASE                   (PV_C)  /**< PixelValve TODO: Remove */
#define DSI_BASE                  (DSI_CTRL)  /**< DSI DISPLAY TODO: Remove */
#define AFE_BASE                  (DISP_AFE_LDO_CTRL)  /**< LVDS DISPLAY TODO: Remove */
#define FMISC_DISPLAY_BASE        (FMISC_DISPLAY_ISPO_CTRL)  /**< FMISC TODO: Remove */
#define SD_HW_ID_MAX              (1UL)  /**< TODO: Remove */
#define SDHC_HOST_IP_BASE         (SD_HOST_IP_BASE)  /**< TODO: Remove */
#define SDHC_HOST_BASE            (SD_HOST_BASE)  /**< TODO: Remove */
#define SDHC_CHIP_CAP0            (0x57EE0030UL | (0xD0UL << 8UL))
#define SDHC_CHIP_CAP1            (0x1002177)

#define OTP_HW_ID_0                     (0UL)
#define OTP_MAC_ADDR_0_OCTET123_ADDR    (151UL)
#define OTP_MAC_ADDR_0_OCTET456_ADDR    (152UL)
#define OTP_MAC_ADDR_1_OCTET123_ADDR    (153UL)
#define OTP_MAC_ADDR_1_OCTET456_ADDR    (154UL)

#endif /* BCM8910X_CONFIG_H */
