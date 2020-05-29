#
# Copyright 2018-2019 Broadcom Limited.  All rights reserved.
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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################



##  @defchapter grp_introduction Introduction
#
#   @addtogroup grp_introduction
#   @{
#   This document covers Requirements, Architecture, Design and Test specification for BCM8910X.
#   <BR>BCM8910X software architecture follows a modular and layered approach to address
#   -# portability across operating systems
#   -# reduced memory footprint for each use-case
#   -# Safety and Security at component and system level
#   -# Optimized power for each use-case
#
#   For a quick startup guide, user may refer to @ref page_bcm8910x_qsg
#
#   The word “Shall/Should” is used to indicate that the feature is mandatory.
#   <BR>The word “Will/Would” is used to indicate that the feature is mandatory to be implemented/followed.
#   <BR>The word “May/Might” is used to indicate that the feature is optional.
#
#   @}
#
#   @defchapter grp_bcm8910x_qsg Starter Guide
#
#   @addtogroup grp_bcm8910x_qsg
#   @{
#   This chapter is intended to serve as a Starter Guide for BCM8910X
#   Automotive Software Development Kit. Details include
#       -# Build & Environment setup
#       -# Flashing and Debugging environment setup
#       -# Running and Debugging an application
#
#   Refer to @ref page_bcm8910x_qsg for complete details.
#
#   @}
#
#   @defchapter grp_bcm8910x References & Examples
#
#
#   @addtogroup grp_bcm8910x
#   @{
#   This chapter describes
#       -# Example configurations
#       -# External references
#   @}
#
#   @defchapter grp_bcm8910x_safety_manual Safety Manual
#
#   @addtogroup grp_bcm8910x_safety_manual
#   @{
#   This chapter contains safety recommendations for BCM8910X SDK
#   integration into a product. If this chapter is empty, please
#   contact Broadcom to recieve detailed information.
#
#   This chapter describes safety related aspects for
#   -# Pixel Processor Safety
#   -# Power-On Self Test
#   -# Memory Allocation & Protection
#   -# Interrupts & Exceptions
#   -# Watchdog Service (Software Watchdog)
#   -# Media System
#
#   @}
#
#   @defchapter grp_bcm8910x_init Initialization
#
#   @addtogroup grp_bcm8910x_init
#   @{
#   This chapter describes initialization sequence for BCM8910X EVKs including
#       -# Early Init
#           -# Cortex-M7 processor initialization including stack pointers setup,
#               MPU configuration, exception/interrupt vector installation etc
#           -# BSS initialization
#           -# Starting OS
#       -# Late Init
#           -# Idle Task creation (for WFI and profiling)
#           -# NVM subsystem initialization
#           -# Communication subsystem initialization
#           -# Application initialization (including media system initialization)
#
#   @file system/bcm8910x/comp.mk
#   @brief Makefile for BCM8910X Initialization
#   @version 0.1 Initial version
#   @}
#

BRCM_COMP_REQ_SRC += doc/bcm8910x_req.c
BRCM_COMP_REQ_SRC += doc/bcm8910x_rdb_req.c
BRCM_COMP_DOC_SRC += doc/introduction_page.c
BRCM_COMP_DOC_SRC += doc/references_page.c
BRCM_COMP_DOC_SRC += doc/qsg_page.c
BRCM_COMP_DOC_SRC += doc/board_page.c
BRCM_COMP_DOC_SRC += doc/otp_programming_page.c
BRCM_COMP_DOC_SRC += doc/vlc_page.c
ifeq ($(safety),1)
BRCM_COMP_DOC_SRC += doc/safety_page.c
endif
BRCM_COMP_DOC_SRC += chip/common/svc_handlers.c
BRCM_COMP_DOC_SRC += doc/architecture.vsd
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_non_autosar.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_flash_layout.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_app_list.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_sec_boot.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_nonsec_boot.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_autosar.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_sram_map.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8910x_arch_dram_map.jpg


ifeq ($(board),bcm89103_ar143_evk)
BRCM_COMP_APP_OPT += ENABLE_CAM_MODULE_AR0143
BRCM_COMP_APP_OPT += __BCM89103_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH0
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89103_ov9716_evk)
BRCM_COMP_APP_OPT += ENABLE_CAM_MODULE_OV9716
BRCM_COMP_APP_OPT += __BCM89103_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH0
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89103_ox3a_evk)
BRCM_COMP_APP_OPT += ENABLE_CAM_MODULE_OX3A10
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += __BCM89103_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH0
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89103_evk)
BRCM_COMP_APP_OPT += __BCM89103_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH0
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89105_ddr_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89106_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89106_ox3a_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_CAM_MODULE_OX3A10
BRCM_COMP_APP_OPT += __BCM89106_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89107_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_FLASH0
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89107_ar143_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_CAM_MODULE_AR0143
BRCM_COMP_APP_OPT += __BCM89107_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH0
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89107_ox3a_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_CAM_MODULE_OX3A10
BRCM_COMP_APP_OPT += __BCM89107_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH0
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89109_ddr_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_FLASH1
else ifeq ($(board),bcm89109_ddr_ov10640_evk)
BRCM_COMP_APP_OPT += ENABLE_PCA9673
BRCM_COMP_APP_OPT += ENABLE_DDR
BRCM_COMP_APP_OPT += ENABLE_CAM_MODULE_OV10640
BRCM_COMP_APP_OPT += __BCM89109_DDR_EVK__
BRCM_COMP_APP_OPT += ENABLE_FLASH1
endif

ifeq ($(chip),bcm89103)
BRCM_COMP_APP_OPT += AVCE_ENABLE_LOW_COMPLEXITY
endif

BRCM_COMP_APP_OPT += __BCM8910X__
BRCM_COMP_APP_OPT += ENABLE_DBGMEM
BRCM_COMP_APP_OPT += ENABLE_MSG_QUEUE
BRCM_COMP_APP_OPT += ENABLE_OTP
BRCM_COMP_APP_OPT += ENABLE_AVT
BRCM_COMP_APP_OPT += ENABLE_GPIO_GIO_V2
BRCM_COMP_APP_OPT += ENABLE_PINMUX
BRCM_COMP_APP_OPT += ENABLE_IIC_BSC
BRCM_COMP_APP_OPT += ENABLE_VTMON
BRCM_COMP_APP_OPT += ENABLE_CCT
BRCM_COMP_APP_OPT += ENABLE_PWM
BRCM_COMP_APP_OPT += ENABLE_DMA330
BRCM_COMP_APP_OPT += ENABLE_TIMER_SP804
BRCM_COMP_APP_OPT += ENABLE_HRTIMER
BRCM_COMP_APP_OPT += ENABLE_WATCHDOG_SP805
BRCM_COMP_APP_OPT += ENABLE_SYSTEM_MONITOR
BRCM_COMP_APP_OPT += ENABLE_FPU
BRCM_COMP_APP_OPT += ENABLE_THREAD_PROTECTION

BRCM_COMP_APP_OPT += ENABLE_ETH_AMAC
BRCM_COMP_APP_OPT += ENABLE_ETH_BRPHY
BRCM_COMP_APP_OPT += P1588_PORT_MASK=0x1UL
BRCM_COMP_APP_OPT += ETHER_TX_CHAN_CNT=4UL
BRCM_COMP_APP_OPT += ETHER_RX_CHAN_CNT=1UL

BRCM_COMP_APP_OPT += ENABLE_UART0
BRCM_COMP_APP_OPT += ENABLE_UART1

BRCM_COMP_APP_INTR += AVT_SYS_TS AVT_SYS_SP
AVT_SYS_TS.function = AVT_SysSnapshotIRQHandler
AVT_SYS_SP.function = AVT_SysPeriodicIRQHandler


BRCM_COMP_APP_INTR += I2C02VIC I2C12VIC I2C22VIC I2C32VIC
I2C02VIC.function = IIC_IRQHandler0
I2C12VIC.function = IIC_IRQHandler1
I2C22VIC.function = IIC_IRQHandler2
I2C32VIC.function = IIC_IRQHandler3

BRCM_COMP_APP_INTR += VTMON
VTMON.function = VTMON_IRQHandler

BRCM_COMP_APP_INTR += TMR0_CH0 TMR0_CH1 TMR0_CH2 TMR0_CH3 TMR0_UDC TMR0_ETR
TMR0_CH0.function = CCT_CH0_IRQHandler
TMR0_CH1.function = CCT_CH1_IRQHandler
TMR0_CH2.function = CCT_CH2_IRQHandler
TMR0_CH3.function = CCT_CH3_IRQHandler
TMR0_UDC.function = CCT_UDC_IRQHandler
TMR0_ETR.function = CCT_ETR_IRQHandler

BRCM_COMP_APP_INTR += DMA330_0 DMA330_1 DMA330_2 DMA330_3 DMA330_4 DMA330_5 DMA330_6 DMA330_7 DMA330_ABORT
DMA330_0.function = DMA_CH0_IRQHandler
DMA330_1.function = DMA_CH1_IRQHandler
DMA330_2.function = DMA_CH2_IRQHandler
DMA330_3.function = DMA_CH3_IRQHandler
DMA330_4.function = DMA_CH4_IRQHandler
DMA330_5.function = DMA_CH5_IRQHandler
DMA330_6.function = DMA_CH6_IRQHandler
DMA330_7.function = DMA_CH7_IRQHandler
DMA330_ABORT.function = DMA_Abort_IRQHandler

BRCM_COMP_APP_INTR += ADC
ADC.function = ADC_IRQHandler


BRCM_COMP_APP_INTR += SPI0 SPI1 SPI2
SPI0.function = SPI0_IRQHandler
SPI1.function = SPI1_IRQHandler
SPI2.function = SPI2_IRQHandler

BRCM_COMP_APP_INTR += GPIO0 GPIO1
GPIO0.function = GPIO_IRQ0Handler
GPIO1.function = GPIO_IRQ1Handler

BRCM_COMP_APP_EVENT += SystemEvent0
BRCM_COMP_APP_EVENT += SystemEvent1
BRCM_COMP_APP_EVENT += SystemEvent2
BRCM_COMP_APP_EVENT += SystemEvent3
BRCM_COMP_APP_EVENT += SystemEvent4
BRCM_COMP_APP_EVENT += SystemEvent5
BRCM_COMP_APP_EVENT += SystemEvent6
BRCM_COMP_APP_EVENT += SystemEvent7
BRCM_COMP_APP_EVENT += SystemEvent8
BRCM_COMP_APP_EVENT += SystemEvent9
BRCM_COMP_APP_EVENT += SystemEvent10
BRCM_COMP_APP_EVENT += SystemEvent11
BRCM_COMP_APP_EVENT += SystemEvent12
BRCM_COMP_APP_EVENT += SystemEvent13
BRCM_COMP_APP_EVENT += SystemEvent14
BRCM_COMP_APP_EVENT += DMAEvent
BRCM_COMP_APP_EVENT += ShutdownEvent

BRCM_COMP_APP_EVENT += UDPTxEvent
BRCM_COMP_APP_EVENT += UDPRxEvent

BRCM_COMP_APP_TASK += BCM8910X_SystemTask
BCM8910X_SystemTask.priority = 16
BCM8910X_SystemTask.autostart = true
BCM8910X_SystemTask.stack = 2048
BCM8910X_SystemTask.group = BCM_GROUPID_SYS
BCM8910X_SystemTask.event += SystemEvent0
BCM8910X_SystemTask.event += SystemEvent1
BCM8910X_SystemTask.event += SystemEvent2
BCM8910X_SystemTask.event += SystemEvent3
BCM8910X_SystemTask.event += SystemEvent6 # Subsystem Notifications to Sytem Task(MsgQ)

BRCM_COMP_APP_TASK += TaskSerialIO
TaskSerialIO.priority = 13
TaskSerialIO.stack = 2048
TaskSerialIO.event += SystemEvent0
TaskSerialIO.event += SystemEvent1
TaskSerialIO.event += SystemEvent2
TaskSerialIO.event += SystemEvent3
TaskSerialIO.event += SystemEvent4
TaskSerialIO.event += SystemEvent5
TaskSerialIO.event += SystemEvent6
TaskSerialIO.event += SystemEvent7
TaskSerialIO.event += SystemEvent8
TaskSerialIO.event += SystemEvent11

BRCM_COMP_APP_COUNTER += SystemTimer
SystemTimer.mincycle = 1
SystemTimer.maxallowedvalue = 2147483647
SystemTimer.ticksperbase = 1
SystemTimer.secondspertick = 1

BRCM_COMP_APP_COUNTER += HRTimer
HRTimer.mincycle = 1
HRTimer.maxallowedvalue = 2147483647
HRTimer.ticksperbase = 1
HRTimer.secondspertick = 1

BRCM_COMP_APP_ALARM += SystemMonitor
SystemMonitor.counter = SystemTimer
SystemMonitor.callback = SYS_NotfnAlarmCb
SystemMonitor.autostart = true
SystemMonitor.alarmtime = 32
SystemMonitor.cycletime = 32

BRCM_COMP_APP_CFG += bcm8910x_swr
bcm8910x_swr.pid = 0x3
bcm8910x_swr.imgid = 0x9010
bcm8910x_swr.maxsz = 0x1000
bcm8910x_swr.file = system/bcm8910x/config/bcm8910x_swr.bin


BRCM_COMP_NAME := bcm8910x
BRCM_COMP_DEPENDS := comms nvm console
BRCM_COMP_DEPENDS += mcudrv utils arm
BRCM_COMP_TYPE := doc

