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

#include <stdint.h>
#include <bcm_utils.h>
#include <pinmux.h>
#include <mcu.h>
#include <osil/log_osil.h>
#include <clk.h>
#include <board.h>
#include <ulog.h>
#include <gpio.h>
#if defined(ENABLE_1722) && defined(ENABLE_AVB_TALKER)
#include <avb_talker.h>
#endif
#if defined(ENABLE_1722) && defined(ENABLE_AVB_LISTENER)
#include <avb_listener.h>
#endif
#if defined(ENABLE_ETS) || defined(ENABLE_ETS_TEST)
#include <ets_osil.h>
#endif
#if defined(ENABLE_INET)
#include <inet_osil.h>
#endif
#ifdef ENABLE_PTM
#include <imgl.h>
#include <ptm_osil.h>
#endif

#define GetModuleLogLevel()         (ULOG_LVL_ERROR)
#define IIC_HW_ID_CAM_SENSOR        (0UL)

#if defined(ENABLE_SHELL) && !defined(ENABLE_UART_TEST)
extern void UARTConsole_UARTRxCb(char *aData, uint32_t aSize);
#endif

#if defined(ENABLE_UART_TEST)
extern uint32_t Test_UARTTxCb(char *const aData, uint32_t aSize);
extern void Test_UARTRxCb(char *aData, uint32_t aSize);
#endif

#define FREQ_MHZ(x)      ((x) * 1000000)
/**
 * MCU configuration table
 */

/**
 * Clock configuration table
 */
static const MCU_ClkCfgType ClkCfgTbl[] = {
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI0_SRC400_80MHZ,   /* QSPI0 @ 80MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(80),
            .cntrl = {[0] = 0x3, [1] = 0x4},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI0_SRC400_66MHZ,   /* QSPI0 @ 66MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(66),
            .cntrl = {[0] = 0x3, [1] = 0x5},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI0_SRC400_50MHZ,   /* QSPI0 @ 50MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(50),
            .cntrl = {[0] = 0x3, [1] = 0x7},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI0_SRC200_25MHZ,   /* QSPI0 @ 25MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(25),
            .cntrl = {[0] = 0x2, [1] = 0x7},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI1_SRC400_80MHZ,   /* QSPI1 @ 80MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(80),
            .cntrl = {[0] = 0x3, [1] = 0x4},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI1_SRC400_66MHZ,   /* QSPI1 @ 66MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(66),
            .cntrl = {[0] = 0x3, [1] = 0x5},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI1_SRC400_50MHZ,   /* QSPI1 @ 50MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(50),
            .cntrl = {[0] = 0x3, [1] = 0x7},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_QSPI1_SRC200_25MHZ,   /* QSPI1 @ 25MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_QSPI,
            .freq = FREQ_MHZ(25),
            .cntrl = {[0] = 0x2, [1] = 0x7},
        },
    },
#if defined(ENABLE_CAMERA_SENSOR) || defined(ENABLE_IIC_TEST) || defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST)
    {
        .cfgID = MCU_CLK_CFG_ID_CAM,
        .clkRef = {
            .clkID = MCU_CLK_ID_NCO2,
            .freq = 27000000UL,
            .cntrl = {},
        },
    },
#endif  /* ENABLE_CAMERA_SENSOR || ENABLE_IIC_TEST || ENABLE_VGCC_DISPLAY || ENABLE_VGC_TEST */

#if defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST)
#define VGC_PLLDISP_KP                  (0x8U)
#define VGC_PLLDISP_KI                  (0x3U)
#define VGC_PLLDISP_KA                  (0x0U)
#define VGC_PLLDISP_CTRL2               (0x8C40U)
#define VGC_PLLDISP_REFCLK              ( 1U) /* 1 = nco, 0 = xtal */
#define VGC_PLLDISP_NDIV_INT            (0x4DU) /* Multiplier */
#define VGC_PLLDISP_MDIV                ( 4U) /* Post divider */
#define VGC_PLLDISP_PDIV                ( 1U) /* Predivider */
#define VGC_PLLDISP_NDIV_FRAC_19_16     (0x0U)
#define VGC_PLLDISP_NDIV_FRAC_15_0      (0x0U)

#define VGC_PLL_DISP_PINSTRAP           (VGC_PLLDISP_REFCLK << 4U)
#define VGC_PLL_DISP_CTRL1              (0x0000U)
#define VGC_PLL_DISP_CTRL2              (VGC_PLLDISP_CTRL2)
#define VGC_PLL_DISP_CTRL3              (0x0000U)
#define VGC_PLL_DISP_NDIV_FRAC          (VGC_PLLDISP_NDIV_FRAC_15_0)
#define VGC_PLL_DISP_NDIV               ((((VGC_PLLDISP_NDIV_FRAC_19_16 & 0xFU) << 10U) | (VGC_PLLDISP_NDIV_INT & 0x3FFU)))
#define VGC_PLL_DISP_CFG1               (0x0007U)
#define VGC_PLL_DISP_CFG2               ((VGC_PLLDISP_PDIV & 0xFU) | (VGC_PLLDISP_KP << 10U)| (VGC_PLLDISP_KI <<7U) | (VGC_PLLDISP_KA<<4U))
#define VGC_PLL_DISP_CFG3               ((VGC_PLLDISP_MDIV & 0xFFU) << 4U)

    {
        .cfgID = MCU_CLK_CFG_ID_DISP,
        .clkRef = {
            .clkID = MCU_CLK_ID_PLL_DISPLAY,
            .freq = 0UL,
            .cntrl = {
                VGC_PLL_DISP_PINSTRAP,   /* pinstrap[4]            */
                VGC_PLL_DISP_CTRL1,      /* ctrl1                  */
                VGC_PLL_DISP_CTRL2,      /* ctrl2                  */
                VGC_PLL_DISP_CTRL3,      /* ctrl3                  */
                VGC_PLL_DISP_NDIV_FRAC,  /* ndiv_frac              */
                VGC_PLL_DISP_NDIV,       /* ndiv frac[19:16], int  */
                VGC_PLL_DISP_CFG1,       /* cfg1                   */
                VGC_PLL_DISP_CFG2,       /* cfg2: pdiv             */
                VGC_PLL_DISP_CFG3        /* cfg3[11:4] : mdiv      */
            },
        },
    },
#endif /* ENABLE_VGCC_DISPLAY || ENABLE_VGC_TEST */
};

static const MCU_ConfigType MCU_Config = {
    .clkSrcFailNotification = MCU_CLK_SRC_NOTIFICATION_EN,
    .clkCfgTbl = ClkCfgTbl,
    .clkCfgTblSize = (sizeof(ClkCfgTbl) / sizeof(MCU_ClkCfgType)),
    .ramCfgTbl = NULL,
    .ramCfgTblSize = 0UL,
};

const MCU_ConfigType *mcu_cfg_table = &MCU_Config;

#if !defined(ENABLE_PINMUX_TEST) && !defined(ENABLE_GPIO_TEST)
/**
 * Pin Mux Configuration
 */
static const PINMUX_PinModeCfgType PinModeDisableCfg[] = {
    {PINMUX_PIN_MODE_DISABLE, 0U},
};

static const PINMUX_PinModeCfgType QSPIPinModeCfg[] = {
    {PINMUX_PIN_MODE_QSPI, 0U},
};

static const PINMUX_PinModeCfgType UARTPinModeCfg[] = {
    {PINMUX_PIN_MODE_UART0, 0UL},
};

static const PINMUX_PinModeCfgType CANPinModeCfg[] = {
    {PINMUX_PIN_MODE_CAN, 0UL}
};

static const PINMUX_PinModeCfgType CAMPinModeCfg[] = {
    {PINMUX_PIN_MODE_IIC, 0UL},
};

static const PINMUX_PinCfgType GIO0PinMuxPins[] = {
    /* QSPI pins */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_0, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_1, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_2, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_3, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_4, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_5, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* UART0 Tx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_6, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UARTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* UART0 Rx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_7, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UARTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* CAN Tx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_8, PINMUX_PIN_MODE_CAN, 0U, 1UL, &CANPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* CAN Rx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_9, PINMUX_PIN_MODE_CAN, 0U, 1UL, &CANPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* disabled pins */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_10, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_11, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_12, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_13, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_14, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_15, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_16, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_17, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_18, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_19, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* Camera IIC SDA */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_20, PINMUX_PIN_MODE_IIC,0U,1UL, &CAMPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* Camera IIC SCL */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_21, PINMUX_PIN_MODE_IIC, 0U, 1UL, &CAMPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
};

const PINMUX_ConfigType PINMUX_Config[] = {
    {
        sizeof(GIO0PinMuxPins) / sizeof(PINMUX_PinCfgType),
        &GIO0PinMuxPins[0],
    },
};

const GPIO_IntrCbType GPIO_IntrCbTbl[GPIO_MAX_CHANNELS] = {
};
#endif /* !defined(ENABLE_PINMUX_TEST) && !defined(ENABLE_GPIO_TEST) */

/**
 * Board MPU configuration
 */
const MPU_TableType MPU_BoardTable[MPU_TABLE_SIZE_BOARD] = {};

/**
 * Board i2c configuration
 */
static const IIC_CfgType i2c_cfg_tbl[] = {
    /* Camera sensor */
    {.id = IIC_HW_ID_CAM_SENSOR,
     .config = {
         .speed = IIC_SPEED_400K,
         .txFifoMode = 0,
     }
    },
};

const IIC_CfgType *i2c_cfg_table = i2c_cfg_tbl;
const uint32_t i2c_cfg_table_sz = (sizeof(i2c_cfg_tbl) / sizeof(IIC_CfgType));

#if !defined(ENABLE_UART_TEST)
/**
 * UART configuration
 */
const UART_ConfigType UartConfig = {
    .baud = UART_BAUD_115200,
    .txFifoLvl = UART_TX_FIFO_LVL_1DIV8,
    .rxFifoLvl = UART_RX_FIFO_LVL_1DIV8,
    .stopBits = UART_STOP1,
    .parity = UART_PARITY_NONE,
    .loopBackMode = 0UL,
};
#endif

static const UART_TxCbType uart_tx_cb_tbl[] = {
#if (defined(ENABLE_KLOG) || defined(ENABLE_ULOG)) && !defined(ENABLE_UART_TEST)
    Log_UARTTxCb,
#endif
#if defined(ENABLE_UART_TEST)
    Test_UARTTxCb,
#endif
#if (defined(ENABLE_KLOG) || defined(ENABLE_ULOG)) && !defined(ENABLE_UART_TEST)
    Log_UARTTxCb,
#endif
#if defined(ENABLE_UART_TEST)
    Test_UARTTxCb,
#endif
#if (defined(ENABLE_KLOG) || defined(ENABLE_ULOG)) && !defined(ENABLE_UART_TEST)
    Log_UARTTxCb,
#endif
#if defined(ENABLE_UART_TEST)
    Test_UARTTxCb,
#endif
};

static const UART_RxCbType uart_rx_cb_tbl[] = {
#if defined(ENABLE_SHELL) && !defined(ENABLE_UART_TEST)
    UARTConsole_UARTRxCb,
#endif
#if defined(ENABLE_UART_TEST)
    Test_UARTRxCb,
#endif
#if defined(ENABLE_SHELL) && !defined(ENABLE_UART_TEST)
    UARTConsole_UARTRxCb,
#endif
#if defined(ENABLE_UART_TEST)
    Test_UARTRxCb,
#endif
#if defined(ENABLE_SHELL) && !defined(ENABLE_UART_TEST)
    UARTConsole_UARTRxCb,
#endif
#if defined(ENABLE_UART_TEST)
    Test_UARTRxCb,
#endif
};

const UART_TxCbType *uart_tx_cb_table = uart_tx_cb_tbl;
const uint32_t uart_tx_cb_table_sz = (sizeof(uart_tx_cb_tbl) / sizeof(UART_TxCbType));

const UART_RxCbType *uart_rx_cb_table = uart_rx_cb_tbl;
const uint32_t uart_rx_cb_table_sz = (sizeof(uart_rx_cb_tbl) / sizeof(UART_RxCbType));

/* Board flash configurations */
#if defined(ENABLE_FLASH) && !defined(ENABLE_FLASH_TEST) && !defined(ENABLE_FLSMGR_TEST)

/**
 * Move this to make file
 */
#define FLASH0_SPEED    (FLASH_SPEED_80M)
#define FLASH1_SPEED    (FLASH_SPEED_25M)

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

static const FLASH_FlsableSecType fls0FlsableSecList[] = {
};

static const FLASH_FlsableSecType fls1FlsableSecList[] = {
};

static const FLASH_CfgTblType flash_cfg_tbl[] = {
    {
        .hwID = FLASH_HW_ID_0,
        .config = {
            .size = (2UL * 1024UL * 1024UL), /* 2MB */
            .pageSize = 256UL,
            .sectorSize = (64UL * 1024UL), /* 64KB */
            .SPIMode = FLASH_SPI_MODE_3,
            .speed = FLASH0_SPEED,
            .readLane = FLASH_READ_LANE_SINGLE,
            .flsableSecListSize = (sizeof(fls0FlsableSecList)/
                                    sizeof(FLASH_FlsableSecType)),
            .flsableSecList = fls0FlsableSecList,
            .flsID = FLASH_FLSID_WINBOND_W25Q16,
        },
    },
    {
        .hwID = FLASH_HW_ID_1,
        .config = {
            .size = (64UL * 1024UL * 1024UL), /* 64MB */
            .pageSize = 256UL,
            .sectorSize = (64UL * 1024UL),  /* 64KB */
            .SPIMode = FLASH_SPI_MODE_3,
            .speed = FLASH1_SPEED,
            .readLane = FLASH_READ_LANE_SINGLE,
            .flsableSecListSize = (sizeof(fls1FlsableSecList)/
                                    sizeof(FLASH_FlsableSecType)),
            .flsableSecList = fls1FlsableSecList,
            .flsID = FLASH_FLSID_MICRON_N25Q512A,
        },
    },
};

const FLASH_CfgTblType *flash_cfg_table = flash_cfg_tbl;
const uint32_t flash_cfg_table_sz = (sizeof(flash_cfg_tbl)/
                                    sizeof(FLASH_CfgTblType));

const MCU_ClkCfgIDType flash_clk_tbl[] = {
    FLASH0_CLK_CFG_ID,
    FLASH1_CLK_CFG_ID,
};

const MCU_ClkCfgIDType *flash_clk_table = flash_clk_tbl;
const uint32_t flash_clk_table_sz = (sizeof(flash_clk_tbl) /
                                    sizeof(MCU_ClkCfgIDType));
#endif

/* Board flash manager configurations */
#if defined(ENABLE_FLSMGR) && !defined(ENABLE_FLSMGR_TEST) && !defined(ENABLE_FLASH_TEST)
static const FLSMGR_CfgType flsmgr_cfg_tbl[] = {
    {.flashCfg = &flash_cfg_tbl[0]},
    {.flashCfg = &flash_cfg_tbl[1]},
};

const FLSMGR_CfgType  * const flsmgr_cfg_table = flsmgr_cfg_tbl;
const uint32_t flsmgr_cfg_table_sz = (sizeof(flsmgr_cfg_tbl)/
                                    sizeof(FLSMGR_CfgType));
#endif

#ifdef ENABLE_PTM
static const PTM_LookupTblEntType LookupTbl[] = {
    {
        .flashID = FLASH_HW_ID_0,
        .flashAddr = 0x0,
    },
    {
        .flashID = FLASH_HW_ID_1,
        .flashAddr = 0x0,
    },
};

static const PTM_CfgType PTM_CfgTbl = {
    .ptLoadAddr = NULL,
    .lookupTbl = LookupTbl,
    .lookupTblSize = (sizeof(LookupTbl)/sizeof(PTM_LookupTblEntType)),
};

const PTM_CfgType *PTM_Cfg = &PTM_CfgTbl;
#endif /* ENABLE_PTM */

#if defined(ENABLE_CAN_MCAN) && !defined(ENABLE_CAN_TEST)
void CAN_RxNotification(CAN_HwIDType aId, CAN_MsgInfoType *const aMsgInfo)
{

}
void CAN_TxNotification(CAN_HwIDType aId, CAN_TxMsgObjType *const aXmtdObj)
{

}
void CAN_ErrorNotification(CAN_HwIDType aId, CAN_ErrorType errorCode)
{
}

void CAN_BusOffNotification(CAN_HwIDType aId)
{
}
#endif

#if defined(ENABLE_WATCHDOG_SP805)
const WDT_CfgType WDT_Config[] = {
    {
        WDT_DISABLE_ALLOWED,
        WDT_MODE_OFF,
        10000UL,
    },
};
#endif

void Board_Init(void)
{
}
