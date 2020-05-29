##############################################################################
# Copyright 2016-2019 Broadcom Limited.  All rights reserved.
#
# This program is the proprietary software of Broadcom Limited and/or its
# licensors, and may only be used, duplicated, modified or distributed pursuant
# to the terms and conditions of a separate, written license agreement executed
# between you and Broadcom (an "Authorized License").
#
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
# LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
# IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
# 1. This program, including its structure, sequence and organization,
#    constitutes the valuable trade secrets of Broadcom, and you shall use all
#    reasonable efforts to protect the confidentiality thereof, and to use this
#    information only in connection with your use of Broadcom integrated
#    circuit products.
#
# 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
#    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
#    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
#    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
#    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
#    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
#    SOFTWARE.
#
# 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
#    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
#    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
#    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
################################################################################
BRCM_CPU_MODEL := CORTEX_M7
BRCM_CPU_FAMILY := CORTEX_MX
BRCM_CPU_FREQUENCY := 400000000
BRCM_CPU_IRQ_STACK := 2048

LIB_GNU_CC ?= 0
LIB_RVCT_CC ?= 0

ifeq ($(LIB_GNU_CC),0)
ifeq ($(LIB_RVCT_CC),0)
LIB_GNU_CC = 1
endif
endif

BRCM_APP_EXTENSION := zip

CHIP_FAMILY := bcm8910x
CPU := cortex-m7

BRCM_CHIP_BASIC_DEPENDS := mcudrv utils arm

BRCM_CHIP_RDB_INC := $(BRCM_SDK_ROOT)/system/bcm8910x/inc/rdb

DEFAULT_COMP_DEPENDENCIES := mcudrv adcdrv avtdrv candrv pwmdrv dmadrv gpiodrv memcdrv
DEFAULT_COMP_DEPENDENCIES += pinmuxdrv i2cdrv spidrv flashdrv ccudrv timdrv
DEFAULT_COMP_DEPENDENCIES += uartdrv wdtdrv ethcntlr ethxcvr vtmondrv otpdrv
DEFAULT_COMP_DEPENDENCIES += mdiodrv utils time console queue arm rsa sha dbgmem wds

DEFAULT_BRCM_INC := $(BRCM_SDK_ROOT)/system/utils/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/time/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/console/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/queue/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/rpc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/rpc/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/nvm/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/imgl/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/nvm/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/cpu/arm/inc

DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/comms/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/nif/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/fqtss/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/common/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8910x
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8910x/inc/
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8910x/chip
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/include
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/adc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/adc/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/avt/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/avt/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/can/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/can/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/ccu/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/ccu/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dma/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dma/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/memc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/flash/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/flash/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/gpio/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/gpio/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/i2c/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/i2c/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/lin/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/lin/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/uart/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/uart/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/bcm8910x
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/osil/bcm8910x
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/pinmux/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/pinmux/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/pwm/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/pwm/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/sd/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/sd/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/spi/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/spi/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/timer/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/timer/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/vtmon/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/vtmon/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/watchdog/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/watchdog/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/otp/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/otp/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dbgmem/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dbgmem/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/wds/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/wds/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/mdio/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/mdio/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/controller/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/controller/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/switch/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/switch/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/security/rsa/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/security/sha/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/avcd/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/avce/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/dup/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/ldc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/pgx/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/ppr/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/stitch/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/venc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/vdec/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/audio/controller/inc

ENABLE_FPU := TRUE
ENABLE_ADC := TRUE
ENABLE_CAN_MCAN := TRUE
ENABLE_PWM := TRUE
ENABLE_LIN := TRUE
ENABLE_DMA330 := TRUE
ENABLE_GPIO_GIO_V2 := TRUE
ENABLE_ETH := TRUE
ENABLE_ETH_TIME := TRUE
ENABLE_ETH_AMAC := TRUE
ENABLE_ETH_BRPHY := TRUE
ENABLE_IIC_BSC := TRUE
ENABLE_SPI_PL022 := TRUE
ENABLE_FLASH := TRUE
ENABLE_FLSMGR := TRUE
ENABLE_CCT := TRUE
ENABLE_TIMER_SP804 := TRUE
ENABLE_TIMER_SYSTICK := FALSE
ENABLE_UART_PL011 := TRUE
ENABLE_WATCHDOG_SP805 := TRUE
ENABLE_PP := TRUE
ENABLE_KLOG := TRUE
ENABLE_ULOG := TRUE
ENABLE_1722 := TRUE
ENABLE_CRF := TRUE
ENABLE_MSG_QUEUE := TRUE
ENABLE_CPU_BLOCK := TRUE
# Since AVT block is required to get system time, always enable AVT Block
ENABLE_AVT :=TRUE
ENABLE_UART_CONSOLE := TRUE
ENABLE_CAM := TRUE
ENABLE_ETS := TRUE
ENABLE_INET := TRUE
ENABLE_PTM := TRUE
ENABLE_VTMON := TRUE
ENABLE_I2S := TRUE
ENABLE_OTP := TRUE
ENABLE_DBGMEM := TRUE

#GPIO Expander chip PCA9673 is a I2C client
ENABLE_PCA9673 := TRUE

CCFLAGS += -D__BCM__  -D__BCM8910X__
# Flash max identifier
CCFLAGS += -DFLASH_MAX_HW_ID=2UL

# BCM8910X family operate as gPTP end points
CCFLAGS += -DETS_MAX_INTERFACES=1UL

CCFLAGS += -DCOMMS_MAX_STREAM_POLICER_ENTRIES=0UL

#BCM8910X family has one Ethernet Transceiver
CCFLAGS += -DETHXCVR_HW_ID_MAX=1UL

ITBL_MAX_NUM_ENTRIES ?= 48
CCFLAGS += -DITBL_MAX_NUM_ENTRIES=$(ITBL_MAX_NUM_ENTRIES)UL

# maximum size of the IPC message payload = 512 - IPC header size
CCFLAGS += -DRPC_MSG_PAYLOAD_SZ=496UL
#---------------------------------------------
#Architecture related components
#---------------------------------------------

BRCM_ENABLED_COMP += utils
BRCM_ENABLED_COMP += bcm8910x drivers autosw comms msys applications bootloader tools

BRCM_ENABLED_COMP += imgl nvm
BRCM_ENABLED_COMP += configurator

BRCM_ENABLED_COMP += wds

BRCM_ENABLED_COMP += mcudrv avtdrv adcdrv candrv pwmdrv memcdrv
BRCM_ENABLED_COMP += dmadrv gpiodrv pinmuxdrv i2cdrv spidrv flashdrv
BRCM_ENABLED_COMP += ccudrv timdrv uartdrv wdtdrv vtmondrv otpdrv mdiodrv

BRCM_ENABLED_COMP += arm
BRCM_ENABLED_COMP += queue time console

BRCM_ENABLED_COMP += ethcntlr ethxcvr
BRCM_ENABLED_COMP += ets 1722 rtp rtcp mpegts fqtss inet nif tftp rfs

BRCM_ENABLED_COMP += avce camera pplib ldc venc

BRCM_ENABLED_COMP += csn isys isys_agc isys_awb isys_awb_gw isys_denoise
BRCM_ENABLED_COMP += isys_cc isys_defpix isys_sharpen isys_bl isys_tm

BRCM_ENABLED_COMP += vgc_display
BRCM_ENABLED_COMP += vgc_mwb

BRCM_ENABLED_COMP += sha rsa dbgmem wds

BRCM_ENABLED_COMP += map_parser rdbgen

ifeq ($(demo),1)
BRCM_ENABLED_COMP += pgx
endif