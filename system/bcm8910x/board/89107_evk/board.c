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

#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <stdint.h>
#include <ulog.h>
#include <bcm_utils.h>
#include <pinmux.h>
#include <mcu.h>
#include <osil/log_osil.h>
#include <clk.h>
#include <board.h>
#include <ee.h>
#include <dma.h>
#include <osil/bcm_osil.h>
#include <gpio.h>
#ifdef ENABLE_PTM
#include <imgl.h>
#include <ptm_osil.h>
#endif
#if defined(ENABLE_I2S)
#include <osil/audio_osil.h>
#endif
#if defined(ENABLE_LIN)
#include <osil/lin_osil.h>
#endif
#if defined (ENABLE_SD)
#include <sdhc_osil.h>
#endif
#if defined(ENABLE_PWM)
#include <pwm.h>
#endif
#if defined(ENABLE_WLAN)
#include <wl_sdio_osil.h>
#endif /* ENABLE_WLAN */

#if defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10)
#include <camera_sensor_osil.h>
#endif /* defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10) */

#define IIC_HW_ID_CAM_SENSOR        (3UL)
#define IIC_HW_ID_0                 (0UL)

#define PWM_CHANN_ID_DISPLAY_DIMMING    (0UL)

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
#if defined(ENABLE_CAMERA_SENSOR) || defined (ENABLE_IIC_TEST) || defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST)
    {
        .cfgID = MCU_CLK_CFG_ID_CAM,
        .clkRef = {
            .clkID = MCU_CLK_ID_NCO2,
            .freq = 27000000UL,
            .cntrl = {},
        },
    },
#endif  /* ENABLE_CAMERA_SENSOR || ENABLE_IIC_TEST || ENABLE_VGCC_DISPLAY || ENABLE_VGC_TEST */
#if defined(ENABLE_I2S)
    {
        .cfgID = MCU_CLK_CFG_ID_AUDIO_MCLK_0,
        .clkRef = {
            .clkID = MCU_CLK_ID_NCO0,
            .freq = 24576000UL,
            .cntrl = {},
        },
    },
    {
        .cfgID = MCU_CLK_CFG_ID_AUDIO_MCLK_1,
        .clkRef = {
            .clkID = MCU_CLK_ID_NCO1,
            .freq = 24576000UL,
            .cntrl = {},
        },
    },
#endif  /* ENABLE_I2S */

#if defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST) || defined(ENABLE_CSI_TX_PATH)
#define VGC_PLLDISP_KP                  (0x8U)
#define VGC_PLLDISP_KI                  (0x3U)
#define VGC_PLLDISP_KA                  (0x0U)
#define VGC_PLLDISP_CTRL2               (0x8C40U)
#define VGC_PLLDISP_MDIV                ( 4U) /* Post divider */
#define VGC_PLLDISP_PDIV                ( 1U) /* Predivider */
#define VGC_PLLDISP_NDIV_FRAC_19_16     (0x0U)
#define VGC_PLLDISP_NDIV_FRAC_15_0      (0x0U)

#if defined(ENABLE_CSI_TX_PATH)
#define VGC_PLLDISP_REFCLK              ( 0U) /* 1 = nco, 0 = xtal */
#define VGC_PLLDISP_NDIV_INT            (0x60U) /* Multiplier */
#elif defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST)
#define VGC_PLLDISP_REFCLK              ( 1U) /* 1 = nco, 0 = xtal */
#define VGC_PLLDISP_NDIV_INT            (0x4DU) /* Multiplier */
#endif /* ENABLE_CSI_TX_PATH */

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
#endif /* ENABLE_VGCC_DISPLAY || ENABLE_VGC_TEST || ENABLE_CSI_TX_PATH */
    {
        .cfgID = MCU_CLK_CFG_ID_MDIO,   /* MDIO @ 25MHZ */
        .clkRef = {
            .clkID = MCU_CLK_ID_MDIO,
            .freq = FREQ_MHZ(25),
        },
    },
};

static const MCU_ConfigType MCU_Config = {
    .clkSrcFailNotification = MCU_CLK_SRC_NOTIFICATION_EN,
    .clkCfgTbl = ClkCfgTbl,
    .clkCfgTblSize = (sizeof(ClkCfgTbl) / sizeof(MCU_ClkCfgType)),
    .ramCfgTbl = NULL,
    .ramCfgTblSize = 0UL,
};

const MCU_ConfigType *mcu_cfg_table = &MCU_Config;

#if defined(ENABLE_I2S) && !defined(ENABLE_AUDIO_TEST)
static const Audio_CfgType Audio_Config[] = {
#if defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC)
    {
        .hwID = 0UL,
        .masterClk = 24576000UL,
        .masterClkSrc = AUDIO_MCLK_SRC_NCO_0,
        .freq = 48000UL,
        .infMode = AUDIO_INF_MODE_BCLK_WS_MASTER,
        .numSlot = 2UL,
        .slotWidth = 32UL,
        .wordSelWidth = 32UL,
        .txDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .txSlotMask = 0x3UL,
        .txDataJust = AUDIO_INF_DATA_JUST_LEFT,
        .rxDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .rxSlotMask = 0x3UL,
        .rxDataJust = AUDIO_INF_DATA_JUST_LEFT,
#if defined(ENABLE_I2S0_PLAY)
        .playBuf = &Audio_Play0Buf[0UL][0UL],
        .playXferReq = Audio_Play0DMAXfer,
        .playBufSize = AUDIO_PLAY0_BUF_SIZE,
        .playBufNum = AUDIO_PLAY0_BUF_MAX_NUM,
#endif
#if defined(ENABLE_I2S0_REC)
        .recBuf = &Audio_Rec0Buf[0UL][0UL],
        .recBufSize = AUDIO_REC0_BUF_SIZE,
        .recBufNum = AUDIO_REC0_BUF_MAX_NUM,
        .recXferReq = Audio_Rec0DMAXfer,
#endif
    },
#endif  /* defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC) */
#if defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S1_REC)
    {
        .hwID = 1UL,
        .masterClk = 24576000UL,
        .masterClkSrc = AUDIO_MCLK_SRC_NCO_0,
        .freq = 48000UL,
        .infMode = AUDIO_INF_MODE_BCLK_WS_MASTER,
        .numSlot = 2UL,
        .slotWidth = 32UL,
        .wordSelWidth = 32UL,
        .txDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .txSlotMask = 0x3UL,
        .txDataJust = AUDIO_INF_DATA_JUST_LEFT,
        .rxDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .rxSlotMask = 0x3UL,
        .rxDataJust = AUDIO_INF_DATA_JUST_LEFT,
#if defined(ENABLE_I2S1_PLAY)
        .playBuf = &Audio_Play1Buf[0UL][0UL],
        .playBufSize = AUDIO_PLAY1_BUF_SIZE,
        .playBufNum = AUDIO_PLAY1_BUF_MAX_NUM,
        .playXferReq = Audio_Play1DMAXfer,
#endif
#if defined(ENABLE_I2S1_REC)
        .recBuf = &Audio_Rec1Buf[0UL][0UL],
        .recBufSize = AUDIO_REC1_BUF_SIZE,
        .recBufNum = AUDIO_REC1_BUF_MAX_NUM,
        .recXferReq = Audio_Rec1DMAXfer,
#endif
    },
#endif  /* defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S1_REC) */
#if defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC)
    {
        .hwID = 2UL,
        .masterClk = 24576000UL,
        .masterClkSrc = AUDIO_MCLK_SRC_NCO_0,
        .freq = 48000UL,
        .infMode = AUDIO_INF_MODE_BCLK_WS_MASTER,
        .numSlot = 2UL,
        .slotWidth = 32UL,
        .wordSelWidth = 32UL,
        .txDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .txSlotMask = 0x3UL,
        .txDataJust = AUDIO_INF_DATA_JUST_LEFT,
        .rxDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .rxSlotMask = 0x3UL,
        .rxDataJust = AUDIO_INF_DATA_JUST_LEFT,
#if defined(ENABLE_I2S2_PLAY)
        .playBuf = &Audio_Play2Buf[0UL][0UL],
        .playBufSize = AUDIO_PLAY2_BUF_SIZE,
        .playBufNum = AUDIO_PLAY2_BUF_MAX_NUM,
        .playXferReq = Audio_Play2DMAXfer,
#endif
#if defined(ENABLE_I2S2_REC)
        .recBuf = &Audio_Rec2Buf[0UL][0UL],
        .recBufSize = AUDIO_REC2_BUF_SIZE,
        .recBufNum = AUDIO_REC2_BUF_MAX_NUM,
        .recXferReq = Audio_Rec2DMAXfer,
#endif
    },
#endif  /* defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC) */
};

const Audio_CfgType *const Audio_CfgTbl = Audio_Config;
const uint32_t Audio_CfgTblSz = (sizeof(Audio_Config)/sizeof(Audio_CfgType));

#endif  /* defined(ENABLE_I2S) && !defined(ENABLE_AUDIO_TEST) */

#if defined(ENABLE_PWM) && !defined(ENABLE_PWM_TEST) && defined(ENABLE_VGC_LVDS)
static PWM_ChanCfgType PWM_ChannCfg[ ] = {
    {
        .chanID = PWM_CHANN_ID_DISPLAY_DIMMING,
        .mode = PWM_CHANMODE_FULL_DRIVE,
        .class = PWM_CHANCLASS_VARIABLE,
        .polarity = PWM_CHANPOLARITY_ACTIVE_HIGH,
        .prescaler = 0UL,
        .period = 0xFFFF,
        .dutyCycle = 0xFFFF,
    },
};

static PWM_ConfigType PWM_Cfg = {
    .chanCfgTbl = PWM_ChannCfg,
    .chanCfgTblSz = (sizeof(PWM_ChannCfg)/sizeof(PWM_ChanCfgType)),
};

#endif

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

static const PINMUX_PinModeCfgType UART0PinModeCfg[] = {
    {PINMUX_PIN_MODE_UART0, 0UL},
};

#if defined (ENABLE_LIN)
static const PINMUX_PinModeCfgType UART1PinModeCfg[] = {
    {PINMUX_PIN_MODE_UART1, 0UL},
};
#endif

#if defined (ENABLE_CAN_MCAN)
static const PINMUX_PinModeCfgType CANPinModeCfg[] = {
    {PINMUX_PIN_MODE_CAN, 0UL}
};
#endif

#if defined(ENABLE_IIC_TEST) || defined(ENABLE_PCA9673)
static const PINMUX_PinModeCfgType IIC0PinModeCfg[] = {
    {PINMUX_PIN_MODE_IIC, 0UL},
};
#endif /* defined(ENABLE_IIC_TEST) || defined(ENABLE_PCA9673) */

#if defined(ENABLE_SPI_PL022)
static const PINMUX_PinModeCfgType SPIPinModeCfg[] = {
    {PINMUX_PIN_MODE_SPI, 0UL},
};
#endif

#ifdef ENABLE_PWM
static const PINMUX_PinModeCfgType PWMPinModeCfg[] = {
    {PINMUX_PIN_MODE_PWM, 0UL}
};
#endif

#if defined ENABLE_VGC_LVDS || (defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10))
static const PINMUX_PinModeCfgType GPIOPinModeCfg[] = {
    {PINMUX_PIN_MODE_GPIO, 0UL},
};
#endif

#ifdef ENABLE_SD
/* Enable Pullup on CS and WP pins */
static const PINMUX_PinModeCfgType SDWPCDPinsModeCfg[] = {
    {
        PINMUX_PIN_MODE_SDIO,
        PINMUX_HW_CONFIG(PINMUX_PIN_HW_CFG_RES_EN,
                PINMUX_PIN_HW_CFG_RES_PULL_UP, 0UL, 0UL),
    },
};

static const PINMUX_PinModeCfgType SDPinModeCfg[] = {
    {PINMUX_PIN_MODE_SDIO, 0UL},
};
#endif

static const PINMUX_PinModeCfgType CAMPinModeCfg[] = {
    {
        PINMUX_PIN_MODE_IIC,
        0UL,
    },
};

#if (defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC)) || (defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S1_REC)) || (defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC))
static const PINMUX_PinModeCfgType I2SPinModeCfg[] = {
    {
        PINMUX_PIN_MODE_I2S,
        PINMUX_HW_CONFIG(PINMUX_PIN_HW_CFG_RES_DIS, 0UL,
                                PINMUX_PIN_HW_CFG_DRIVE_2MA,
                                PINMUX_PIN_HW_CFG_SLEW_RATE_NORMAL),
    },
};
#endif

#if !defined(ENABLE_AUDIO_TEST) && (defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC))
static const PINMUX_PinModeCfgType AVTPinModeCfg[] = {
    {
        PINMUX_PIN_MODE_AVT_MCLK,
        PINMUX_HW_CONFIG(PINMUX_PIN_HW_CFG_RES_DIS, 0UL,
                                PINMUX_PIN_HW_CFG_DRIVE_2MA,
                                PINMUX_PIN_HW_CFG_SLEW_RATE_NORMAL),
    },
};
#endif

static const PINMUX_PinCfgType GIO0PinMuxPins[] = {
#if !defined(ENABLE_AUDIO_TEST)
#if defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC)
    /* AVT MCLK 0 */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_0, PINMUX_PIN_MODE_AVT_MCLK, 0U, 1UL, &AVTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* AVT MCLK 1 */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_1, PINMUX_PIN_MODE_AVT_MCLK, 0U, 1UL, &AVTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else /* defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC) */
    /* QSPI pins */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_0, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_1, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_2, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_3, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else /* !defined(ENABLE_AUDIO_TEST) */
#if defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S1_REC)
    /* I2S 1 pins */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_0, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_1, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_2, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_3, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif /* defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S1_REC) */
#endif /* !defined(ENABLE_AUDIO_TEST) */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_4, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_5, PINMUX_PIN_MODE_QSPI, 0U, 1UL, &QSPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#if defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC)
    /* I2S0 - BCLK Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_6, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* I2S0 - WS Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_7, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* I2S0 - SDO Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_8, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* I2S0 - SDI Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_9, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#elif defined(ENABLE_SPI_PL022) && defined(ENABLE_SPI0)
    /* SPI0 CLK Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_6, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* SPI0 CS Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_7, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* SPI0 RX Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_8, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* SPI0 TX Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_9, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    /* UART0 Tx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_6, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UART0PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* UART0 Rx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_7, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UART0PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#if defined(ENABLE_CAN_MCAN) && defined(ENABLE_LIN)
#error "Invalid pin configuration for CAN/LIN"
#elif defined (ENABLE_CAN_MCAN)
    /* CAN Tx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_8, PINMUX_PIN_MODE_CAN, 0U, 1UL, &CANPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* CAN Rx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_9, PINMUX_PIN_MODE_CAN, 0U, 1UL, &CANPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#elif defined (ENABLE_LIN)
    /* UART1 Tx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_8, PINMUX_PIN_MODE_UART1, 0U, 1UL, &UART1PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* UART1 Rx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_9, PINMUX_PIN_MODE_UART1, 0U, 1UL, &UART1PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#elif defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10)
    {PINMUX_PIN_DIRECTION_IN, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_8, PINMUX_PIN_MODE_GPIO, PINMUX_PIN_LEVEL_HIGH, 1UL, &GPIOPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE, PINMUX_PIN_INTR_TRIG_FALLING_EDG},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_9, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_8, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_9, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif
#endif /* defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC) */
#if defined(ENABLE_IIC_TEST) || defined(ENABLE_PCA9673)
    /* IIC-0 SDA */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_10, PINMUX_PIN_MODE_IIC, 0U, 1UL, &IIC0PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* IIC-0 SCL */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_11, PINMUX_PIN_MODE_IIC, 0U, 1UL, &IIC0PinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_10, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_11, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif /* defined(ENABLE_IIC_TEST) || defined(ENABLE_PCA9673) */
    /* Disabled pins */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_12, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_13, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_14, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_15, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_16, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_17, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#ifdef ENABLE_PWM
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_18, PINMUX_PIN_MODE_PWM, 0U, 1UL, &PWMPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_19, PINMUX_PIN_MODE_PWM, 0U, 1UL, &PWMPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_18, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_19, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif
    /* Camera IIC SDA */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_20, PINMUX_PIN_MODE_IIC,0U,1UL, &CAMPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* Camera IIC SCL */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_21, PINMUX_PIN_MODE_IIC, 0U, 1UL, &CAMPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#if defined(ENABLE_SD) && defined(ENABLE_SPI_PL022) && defined(ENABLE_SPI1)
#error "SDIO and SPI1 pinmux sharing conflict"
#endif
#if defined(ENABLE_SD) && defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC)
#error "SDIO and I2S2 pinmux sharing conflict"
#endif
#ifdef ENABLE_SD
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_22, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDWPCDPinsModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_23, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDWPCDPinsModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_24, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_25, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_26, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_27, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_28, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_29, PINMUX_PIN_MODE_SDIO, 0U, 1UL, &SDPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
#if defined(ENABLE_SPI_PL022) && defined(ENABLE_SPI1)
    /* SPI0 CLK Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_26, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* SPI0 CS Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_27, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* SPI0 RX Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_28, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* SPI0 TX Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_29, PINMUX_PIN_MODE_SPI, 0U, 1UL, &SPIPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
#if defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC)
    /* I2S 2 - BCLK */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_26, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_26, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif /* defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC) */
#if defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC)
    /* I2S 2 - WCLK */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_27, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
#ifdef ENABLE_VGC_LVDS
    /* GPIO27 for Display */
    {PINMUX_PIN_DIRECTION_OUT, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_27, PINMUX_PIN_MODE_GPIO, PINMUX_PIN_LEVEL_HIGH, 1UL, &GPIOPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_27, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif /* ENABLE_VGC_LVDS */
#endif /* defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC) */
#if defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC)
    /* I2S 2 - DO */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_28, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_28, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif /* defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC) */
#if defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC)
    /* I2S 2 - DI */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_29, PINMUX_PIN_MODE_I2S, 0U, 1UL, &I2SPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#else
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_29, PINMUX_PIN_MODE_DISABLE, 0U, 1UL, &PinModeDisableCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
#endif /* defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC) */
#endif
#endif /* ENABLE_SD */
};

const PINMUX_ConfigType PINMUX_Config[] = {
    {
        sizeof(GIO0PinMuxPins) / sizeof(PINMUX_PinCfgType),
        &GIO0PinMuxPins[0],
    },
};

#if defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10)
static void BOARD_GpioIrqHandlerCb(GPIO_ChannelType aChannelId)
{
    if (GPIO_CHANNEL_8 == aChannelId) {
        CSN_SafeStateCb();
    }
}
#endif /* defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10) */

const GPIO_IntrCbType GPIO_IntrCbTbl[GPIO_MAX_CHANNELS] = {
    NULL,                   /* Channel 0 */
    NULL,                   /* Channel 1 */
    NULL,                   /* Channel 2 */
    NULL,                   /* Channel 3 */
    NULL,                   /* Channel 4 */
    NULL,                   /* Channel 5 */
    NULL,                   /* Channel 6 */
    NULL,                   /* Channel 7 */
#if defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10)
    BOARD_GpioIrqHandlerCb, /* Channel 8 */
#else
    NULL,                   /* Channel 8 */
#endif /* defined(ENABLE_CAMERA_SENSOR) && defined(ENABLE_CAM_MODULE_OX3A10) */
};
#endif /* !defined(ENABLE_PINMUX_TEST) && !defined(ENABLE_GPIO_TEST) */

/**
 * Board i2c configuration
 */
const IIC_ConfigType IIC_Config[] = {
    /* Camera sensor */
    {
        .id = IIC_HW_ID_CAM_SENSOR,
        .speed = IIC_SPEED_MAX,
        .txFifoMode = 0,
    },
    /* IIC - 0 */
    {
        .id = IIC_HW_ID_0,
        .speed = IIC_SPEED_100K,
        .txFifoMode = 0,
    },
};

const uint32_t IIC_ConfigTblSz = (sizeof(IIC_Config) / sizeof(IIC_ConfigType));

/* Board flash configurations */
#if defined(ENABLE_FLASH) && !defined(ENABLE_FLASH_TEST) && !defined(ENABLE_FLSMGR_TEST)
static const FLASH_FlsableSecType fls0FlsableSecList[] = {
    {FLASH0_FLSABLE_SEC_START_ADDR, 16UL}
};

static const FLASH_FlsableSecType fls1FlsableSecList[] = {
    {FLASH1_FLSABLE_SEC_START_ADDR, 1008UL}
};

static const FLASH_CfgTblType flash_cfg_tbl[] = {
    {
        .hwID = FLASH_HW_ID_0,
        .config = {
            .size = FLASH0_SIZE, /* 2MB */
            .pageSize = 256UL,
            .sectorSize = (64UL * 1024UL), /* 64KB */
            .subSectorSize = FLASH0_SUBSECTOR_SIZE, /* 4KB */
            .SPIMode = FLASH_SPI_MODE_3,
            .speed = FLASH0_SPEED,
            .readLane = FLASH0_READ_LANE_CFG,
            .flsableSecListSize = (sizeof(fls0FlsableSecList)/
                                    sizeof(FLASH_FlsableSecType)),
            .flsableSecList = fls0FlsableSecList,
            .flsID = FLASH0_FLASH_ID,
        },
    },
    {
        .hwID = FLASH_HW_ID_1,
        .config = {
            .size = FLASH1_SIZE, /* 64MB */
            .pageSize = 256UL,
            .sectorSize = (64UL * 1024UL),  /* 64KB */
            .subSectorSize = FLASH1_SUBSECTOR_SIZE, /* 4KB */
            .SPIMode = FLASH_SPI_MODE_3,
            .speed = FLASH1_SPEED,
            .readLane = FLASH1_READ_LANE_CFG,
            .flsableSecListSize = (sizeof(fls1FlsableSecList)/
                                    sizeof(FLASH_FlsableSecType)),
            .flsableSecList = fls1FlsableSecList,
            .flsID = FLASH1_FLASH_ID,
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

#if defined(ENABLE_VGCC_DISPLAY)
const VGC_DispCfgType VGC_Cfg = {
    .misc = {
#if defined(ENABLE_VGC_BT656) && defined(ENABLE_VGC_LVDS)
        .output = VGC_OUTPUT_BT656 | VGC_OUTPUT_LVDS,
        .yuvToRgbBypass = TRUE,
#elif defined(ENABLE_VGC_BT656)
        .output = VGC_OUTPUT_BT656,
        .yuvToRgbBypass = TRUE,
#elif defined(ENABLE_VGC_LVDS)
        .output = VGC_OUTPUT_LVDS,
        .yuvToRgbBypass = TRUE,
#elif defined(ENABLE_CSI_TX_PATH)
        .output = VGC_OUTPUT_DSI,
        .yuvToRgbBypass = FALSE,
#else
#error "No output display mode has been enabled\n"
#endif
        .lvdsCrCbSwap = TRUE,
        .iifCrCbSwap  = TRUE,
    },
    .cmp = {
        .bg = {0U, 0U, 0U, 0U, VGC_PIXFMT_YUV444P},
        .panel = {PANEL_WIDTH, PANEL_HEIGHT},
    },
    .pv = {
        .isCont = TRUE,
#ifdef ENABLE_CSI_TX_PATH
        .horz = {30UL, 30UL, PANEL_WIDTH, 30UL},
        .vert = {4UL, 4UL, PANEL_HEIGHT, 4UL},
#else
        .horz = {124UL, 123UL, PANEL_WIDTH, 123UL},
        .vert = {10UL, 10UL, PANEL_HEIGHT, 10UL},
#endif
    },
    .gfd = {
        .csc = {0x0840, 0x1020, 0x0320, 0x0200, 0x0000, 0xFB40, 0xF6C0, 0x0E00, 0x1000, 0x0000, 0x0E00, 0xF440, 0xFDC0, 0x1000, 0x0000},
    },
#ifdef ENABLE_VGC_BT656
    .bt656 = {
        .lanes = 4UL,
    },
#endif
#ifdef ENABLE_VGC_LVDS
    .lvds = {
        .mode = VGC_DISP_LVDSMODE_24_1,
        .de = VGC_DISP_LVDSDE_HACT_VACT,
        .isSyncEnabled = 0U,
    },
#endif
#ifdef ENABLE_CSI_TX_PATH
    .dsi = {
        .lanes = 4UL,
        .vChannId=0,
        .nullBlkPkts=0,
        .rgb888Id = 0x24,
        .ctrl = (VGC_DSI_CTRL_HSDT_EOT_EN | VGC_DSI_CTRL_LP_STOP_LINE | VGC_DSI_CTRL1_EN |
                VGC_DSI_CTRL_CSI2_MODE_EN | VGC_DSI_CTRL_DATA_TYPE_OV_EN|
                VGC_DSI_CTRL_HFP_MAN_NBLP | VGC_DSI_CTRL_HSA_MAN_NBLP |
                VGC_DSI_CTRL_VBLP_MAN_NBLP | VGC_DSI_CTRL_VBLP_MAN_NBLP),
        .t0 = 0x00201008,
        .t1 = 0x00001008,
        .t2 = 0x00000080,
        .t3 = 0x0164B236,
        .t4 = 0x00006859,
        .t5 = 0x00000080,
        .t6 = 0x19071405,
        .t7 = 0x00000020,
    },
#endif
};
#endif  /* ENABLE_VGCC_DISPLAY */

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

#if defined(ENABLE_LIN) && !defined(ENABLE_LIN_TEST)
void LIN_WakeUpNotification(LIN_HwIDType aId)
{

}
#endif

/* Use the event reserved for System_LateInit */
#define BOARD_INIT_EVENT_MASK (SystemEvent1)

#ifdef ENABLE_PCA9673
const PCA9673ChanCfgType PCA9673ChanConfigTbl[] = {
    {PCA9673_P0_CHANNEL_4, PCA9673_HIGH},
    {PCA9673_P0_CHANNEL_5, PCA9673_HIGH},
    {PCA9673_P0_CHANNEL_6, PCA9673_LOW},
#if defined(ENABLE_ETH_RGMII)
    {PCA9673_P1_CHANNEL_4, PCA9673_LOW},
#endif
};

const PCA9673ChanCfgType *const PCA9673ChanCfgTbl = PCA9673ChanConfigTbl;
const uint32_t PCA9673ChanCfgTblSize = sizeof(PCA9673ChanConfigTbl) /
                                        sizeof(PCA9673ChanCfgType);
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

#if defined(ENABLE_VGC_BT656)
#define IIC_ADV7391           (0x2AUL)
#define IIC_FPGA_0            (0x40UL)

static void ADV7391Config(void)
{
    /* Configure in SAV/EAV/NTSC mode
       8-Bit 720p YCrCb In (EAV/SAV), YPrPb Out */
    uint8_t data[10] = {
        0x17U, 0x02U,
        0x00U, 0x1CU,
        0x01U, 0x20U,
        0x30U, 0x2CU,
        0x31U, 0x01U
    };
    uint32_t k;
    int32_t ret;
    IIC_PktType pkt[5];
    uint32_t boardMsgQInfo = 0xFFFFFFFFUL;

    for (k = 0UL; k<5UL; k++)
    {
        pkt[k].flags = 0UL;
        pkt[k].slaveAddr = IIC_ADV7391;
        pkt[k].length = 2UL;
        pkt[k].data = &data[k << 1UL];

        ret = IIC_Transfer(IIC_HW_ID_0, &pkt[k], 1UL, BOARD_INIT_EVENT_MASK, &boardMsgQInfo);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s err:%d\n", __func__, ret);
        }
        ret = BCM_WaitEvent(BOARD_INIT_EVENT_MASK);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s Error while waiting for event, status=%d\n", __func__, ret);
        }
        ret = BCM_ClearEvent(BOARD_INIT_EVENT_MASK);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s Error while clearing event, status=%d\n", __func__, ret);
        }

        ret = IIC_GetStatus(IIC_HW_ID_0, boardMsgQInfo);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s IIC_RecvResponse[%d] returned %d %d\n", __func__, k, ret, __LINE__);
        }

    }
}

static void FPGA0Config(void)
{
    int32_t ret;
    IIC_PktType pkt;
    uint8_t FpgaVal[] = {0U, 0x13U, 0x00U, 0x00U, 0x44U};
    uint32_t boardMsgQInfo = 0xFFFFFFFFUL;

    pkt.flags = 0UL;
    pkt.slaveAddr = IIC_FPGA_0;
    pkt.length = 5UL;
    pkt.data = &FpgaVal[0];

    ret = IIC_Transfer(IIC_HW_ID_0, &pkt, 1UL, BOARD_INIT_EVENT_MASK, &boardMsgQInfo);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s err:%d\n", __func__, ret);
    }
    ret = BCM_WaitEvent(BOARD_INIT_EVENT_MASK);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s Error while waiting for event, status=%d\n", __func__, ret);
    }
    ret = BCM_ClearEvent(BOARD_INIT_EVENT_MASK);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s Error while clearing event, status=%d\n", __func__, ret);
    }

    ret = IIC_GetStatus(IIC_HW_ID_0, boardMsgQInfo);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s IIC_RecvResponse returned %d, line:%d\n", __func__, ret, __LINE__);
    }
}
#endif

#if defined(ENABLE_WLAN) && !defined(ENABLE_WLAN_TEST)
const WLETH_CfgType WLETH_Cfg = {
    .hwID = 0UL,
    .macAddr = NULL,
};

#if defined (ENABLE_WLAN_STA)
const WLXCVR_CfgType WLETH_XcvrCfg = {
    .xcvrID = 0UL,
    .busID = 0UL,
    .wlanCfg = {
        .interface = WLXCVR_INF_STA,
        .country = WLXCVR_COUNTRY_INDIA,
        .apCfg = {
            .ssid = {'P','O', 'E', 'T', 'E', 'S', 'T'},
            .ssidLen = 7UL,
#ifdef ENABLE_WLAN_WPA2
            .security = WLXCVR_SECURITY_WPA2,
            .key = {'P', 'A', 'S', 'S', 'K', 'E', 'Y', '1', '2', '3', '4'},
            .keyLen = 11UL,
#else
            .security = WLXCVR_SECURITY_OPEN,
            .key = {0},
            .keyLen = 0UL,
#endif
            .channel = 0UL,
        },
    },
};
#elif defined (ENABLE_WLAN_AP)
const WLXCVR_CfgType WLETH_XcvrCfg = {
    .xcvrID = 0UL,
    .busID = 0UL,
    .wlanCfg = {
        .interface = WLXCVR_INF_AP,
        .country = WLXCVR_COUNTRY_INDIA,
        .apCfg = {
            .ssid = {'P','O', 'E', 'T', 'E', 'S', 'T'},
            .ssidLen = 7UL,
            .security = WLXCVR_SECURITY_OPEN,
            .key = {0U},
            .keyLen = 0UL,
            .channel = 6UL,
        },
    },
};

#else
#error "neither ENABLE_WLAN_AP or ENABLE_WLAN_STA is defined"
#endif
const char WLAN_NvramCfg[] __attribute__((aligned(8))) =
    //# NVRAM file for Muruta 4339 Board
    "NVRAMRev=$Rev: 12345 $"                                          "\x00"
    "sromrev=11""\x00"
    "boardrev=0x1100""\x00"
    "boardtype=0x06c9""\x00"
    "boardflags=0x10081401""\x00"
    "boardflags2=0x00000000""\x00"
    "boardflags3=0x08001188""\x00"
    "macaddr=00:90:4c:c5:12:38""\x00"
    "ccode=0""\x00"
    "regrev=0""\x00"
    "antswitch=0""\x00"
    "pdgain2g=7""\x00"
    "pdgain5g=7""\x00"
    "tworangetssi2g=0""\x00"
    "tworangetssi5g=0""\x00"
    "vendid=0x14e4""\x00"
    "devid=0x43ae""\x00"
    "manfid=0x2d0""\x00"
    "#prodid=0x052e""\x00"
    "nocrc=1""\x00"
    "otpimagesize=502""\x00"
    "xtalfreq=37400""\x00"
    "extpagain2g=2""\x00"
    "pdetrange2g=2""\x00"
    "extpagain5g=2""\x00"
    "pdetrange5g=2""\x00"
    "rxgains2gelnagaina0=2""\x00"
    "rxgains2gtrisoa0=6""\x00"
    "rxgains2gtrelnabypa0=1""\x00"
    "rxgains5gelnagaina0=4""\x00"
    "rxgains5gtrisoa0=4""\x00"
    "rxgains5gtrelnabypa0=1""\x00"
    "rxchain=1""\x00"
    "txchain=1""\x00"
    "aa2g=1""\x00"
    "aa5g=1""\x00"
    "tssipos5g=0""\x00"
    "tssipos2g=0""\x00"
    "pa2ga0=0xff40,0x16a4,0xfd36""\x00"
    "pa2gccka0=0xff48,0x18bf,0xfcf2""\x00"
    "pa5ga0=0xff54,0x16ec,0xfd45,0xff52,0x16a1,0xfd4d,0xff46,0x15b7,0xfd5a,0xff56,0x156d,0xfd79""\x00"
    "pa5gbw40a0=0xff59,0x17ca,0xfd30,0xff42,0x163d,0xfd4b,0xff31,0x152e,0xfd55,0xff60,0x16d8,0xfd54""\x00"
    "pa5gbw80a0=0xff56,0x1740,0xfd3f,0xff50,0x1738,0xfd3d,0xff4d,0x16b4,0xfd42,0xff58,0x1634,0xfd60""\x00"
    "maxp2ga0=74""\x00"
    "maxp5ga0=74,74,74,74""\x00"
    "cckbw202gpo=0x0000""\x00"
    "cckbw20ul2gpo=0x0000""\x00"
    "mcsbw202gpo=0xaa888888""\x00"
    "mcsbw402gpo=0xaa888888""\x00"
    "dot11agofdmhrbw202gpo=0x6666""\x00"
    "ofdmlrbw202gpo=0x0066""\x00"
    "tssifloor2g=500""\x00"
    "mcsbw205glpo=0xaa866666""\x00"
    "mcsbw405glpo=0xaa866666""\x00"
    "mcsbw805glpo=0xaa866666""\x00"
    "mcsbw205gmpo=0xaa866666""\x00"
    "mcsbw405gmpo=0xaa866666""\x00"
    "mcsbw805gmpo=0xaa866666""\x00"
    "mcsbw205ghpo=0xaa866666""\x00"
    "mcsbw405ghpo=0xaa866666""\x00"
    "mcsbw805ghpo=0xaa866666""\x00"
    "mcslr5glpo=0x0000""\x00"
    "mcslr5gmpo=0x0000""\x00"
    "mcslr5ghpo=0x0000""\x00"
    "sb20in40hrpo=0x0""\x00"
    "sb20in80and160hr5glpo=0x0""\x00"
    "sb40and80hr5glpo=0x0""\x00"
    "sb20in80and160hr5gmpo=0x0""\x00"
    "sb40and80hr5gmpo=0x0""\x00"
    "sb20in80and160hr5ghpo=0x0""\x00"
    "sb40and80hr5ghpo=0x0""\x00"
    "sb20in40lrpo=0x0""\x00"
    "sb20in80and160lr5glpo=0x0""\x00"
    "sb40and80lr5glpo=0x0""\x00"
    "sb20in80and160lr5gmpo=0x0""\x00"
    "sb40and80lr5gmpo=0x0""\x00"
    "sb20in80and160lr5ghpo=0x0""\x00"
    "sb40and80lr5ghpo=0x0""\x00"
    "dot11agduphrpo=0x0""\x00"
    "dot11agduplrpo=0x0""\x00"
    "phycal_tempdelta=25""\x00"
    "cckdigfilttype=2""\x00"
    "swctrlmap_5g=0x00080008,0x00500010,0x00100008,0x000000,0x078""\x00"
    "swctrlmap_2g=0x00010001,0x00220002,0x00020001,0x042202,0x1ff""\x00"
    "swctrlmapext_5g=0x00000000,0x00000000,0x00000000,0x000000,0x000""\x00"
    "swctrlmapext_2g=0x00000000,0x00000000,0x00000000,0x000000,0x000""\x00"
    "muxenab=0x10""\x00"
    "rssicorrnorm_c0=-2,0""\x00"
    "rssicorrnorm5g_c0=3,5,2,3,4,2,2,3,1,2,3,1""\x00"
    "ed_thresh2g=-77""\x00"
    "ed_thresh5g=-77""\x00"
    "paparambwver=1""\x00"
    "\x00\x00";

const uint32_t WLAN_NvramCfgSz = sizeof(WLAN_NvramCfg);
#endif

#if defined(ENABLE_SD) && !defined(ENABLE_SD_TEST) && defined(ENABLE_WLAN) && !defined(ENABLE_WLAN_TEST)
const SD_ClientInfoTblType SD_ClientTbl[] = {
    {
    .clientID = 0x10,
    .opDoneInd = WLETH_SdioOpDoneInd,
    .cardIntrInd = WLETH_SdioCardIntrInd,
    },
};

const SD_ConfigType SD_Cfg = {
    .slotID = 0UL,
    .cardType = SD_CARD_TYPE_IO,
    .freq = 25000000UL,
    .detectVolt = SD_BUS_VOLT_1V8,
    .clientCbTbl = SD_ClientTbl,
    .clientCbTblSz = sizeof(SD_ClientTbl) / sizeof(SD_ClientInfoTblType),
};

int32_t WLETH_SdioGetClientID(SD_SlotIDType aID,
        SD_ClientIDType *const aClientID)
{
    *aClientID = SD_ClientTbl[aID].clientID;
    return BCM_ERR_OK;
}
#endif


void Board_Init(void)
{
#ifdef ENABLE_CSI_TX_PATH
    ConfigMax9295A_Init();
#endif

#if defined(ENABLE_PCA9673) && !defined(ENABLE_IIC_TEST)
    PCA9673Init();
#endif
#if defined(ENABLE_VGC_BT656)
    ADV7391Config();
    FPGA0Config();
#endif

#if defined(ENABLE_PWM) && !defined(ENABLE_PWM_TEST) && defined(ENABLE_VGC_LVDS)
    PWM_Init(PWM_HW_ID_0, &PWM_Cfg);
    PWM_StartChannel(PWM_HW_ID_0, PWM_CHANN_ID_DISPLAY_DIMMING);
#endif
}
