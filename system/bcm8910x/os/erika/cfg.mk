# /*****************************************************************************
# Copyright 2016-2018 Broadcom Limited.  All rights reserved.
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
#******************************************************************************/
ifeq ($(call iseeopt, __BCM8910X__), yes)

EEOPT += __BCM8910X__
OPT_CC += -D__BCM8910X__
OPT_ASM += -D__BCM8910X__

OPT_ASM += -D__ENABLE_BCM_SVC__ -D__ENABLE_BCM_CORTEX_MX_PSP__
OPT_CC += -D__ENABLE_BCM_SVC__ -D__ENABLE_BCM_CORTEX_MX_PSP__
OPT_CC += -D__GCC__

OPT_LINK += -Wl,-e,EE_cortex_mx_default_reset_ISR
OPT_LINK += -L$(EEBASE)/pkg/
OPT_LINK += -L$(BRCM_SDK_ROOT)/system/bcm8910x
TARGET_NAME = c_rX

OPT_ASM += -D__ENABLE_MPU__ -D__ENABLE_ICACHE__ -D__ENABLE_DCACHE__
OPT_CC += -D__ENABLE_MPU__ -D__ENABLE_ICACHE__ -D__ENABLE_DCACHE__
EEOPT += __ENABLE_MPU__ __ENABLE_ICACHE__ __ENABLE_DCACHE__

# Macro to enable clearing EE_th_terminate_nextask[current] data
# incase of error in ChainTask()
CFLAGS += -D__BCM_CHAINTASK_CLR_NEXTTASK_ON_ERR__

include $(BRCM_SDK_ROOT)/system/os/erika/cfg.mk


INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/bcm8910x/chip
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/bcm8910x/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/bcm8910x/inc/rdb

# arch files
CORTEX_MCU_STARTUP += $(BRCM_SDK_ROOT)/system/bcm8910x/chip/common/startup.S

# mcu common files
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/chip/common/svc.S
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/chip/common/svc_handlers.c
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/chip/common/exceptions.S

CFLAGS += -DSYS_TASK_NAME=BCM8910X_SystemTask

ifeq ($(call iseeopt, ENABLE_DMA330), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/chip/common/dma_config.c
endif   # ENABLE_DMA330

# 103 files
ifeq ($(call iseeopt, __BCM89103__), yes)
CORTEX_MCU_MODEL = BCM89103
endif   # __BCM89103__

ifeq ($(call iseeopt, __BCM89106__), yes)
CORTEX_MCU_MODEL = BCM89106
endif   # __BCM89106__

# 107 files
ifeq ($(call iseeopt, __BCM89107__), yes)
CORTEX_MCU_MODEL = BCM89107
endif   # __BCM89107__

ifeq ($(call iseeopt, __BCM89105__), yes)
CORTEX_MCU_MODEL = BCM89105
endif   # __BCM89105__

ifeq ($(call iseeopt, __BCM89109__), yes)
CORTEX_MCU_MODEL = BCM89109
endif   # __BCM89109__

# board files
# common files
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/common/board.c
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/common/testcfg.c

ifeq ($(call iseeopt, __BCM89103_EVK__), yes)
CORTEX_MCU_LINKERSCRIPT = $(BRCM_SDK_ROOT)/system/bcm8910x/board/89103_evk/gnu.ld
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/89103_evk/board.c
endif   # __BCM89103_EVK__

ifeq ($(call iseeopt, __BCM89106_EVK__), yes)
CORTEX_MCU_LINKERSCRIPT = $(BRCM_SDK_ROOT)/system/bcm8910x/board/89106_evk/gnu.ld
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/89106_evk/board.c
CFLAGS += -DDDR_SIZE_MB=32
ifeq ($(call iseeopt, ENABLE_I2S), yes)
CFLAGS += -DAUDIO_PLAY_PORT0_HW_ID=0UL
CFLAGS += -DAUDIO_PLAY_PORT1_HW_ID=1UL
CFLAGS += -DAUDIO_PLAY_PORT2_HW_ID=2UL
CFLAGS += -DAUDIO_REC_PORT0_HW_ID=0UL
CFLAGS += -DAUDIO_REC_PORT1_HW_ID=1UL
CFLAGS += -DAUDIO_REC_PORT2_HW_ID=2UL
ifeq ($(call iseeopt, ENABLE_AUDIO_PLAY_PORT0), yes)
CFLAGS += -DENABLE_I2S0_PLAY
endif   # ENABLE_AUDIO_PLAY_PORT0
ifeq ($(call iseeopt, ENABLE_AUDIO_PLAY_PORT1), yes)
CFLAGS += -DENABLE_I2S1_PLAY
endif   # ENABLE_AUDIO_PLAY_PORT1
ifeq ($(call iseeopt, ENABLE_AUDIO_PLAY_PORT2), yes)
CFLAGS += -DENABLE_I2S2_PLAY
endif   # ENABLE_AUDIO_PLAY_PORT2
ifeq ($(call iseeopt, ENABLE_AUDIO_REC_PORT0), yes)
CFLAGS += -DENABLE_I2S0_REC
endif   # ENABLE_AUDIO_REC_PORT0
ifeq ($(call iseeopt, ENABLE_AUDIO_REC_PORT1), yes)
CFLAGS += -DENABLE_I2S1_REC
endif   # ENABLE_AUDIO_REC_PORT1
ifeq ($(call iseeopt, ENABLE_AUDIO_REC_PORT2), yes)
CFLAGS += -DENABLE_I2S2_REC
endif   # ENABLE_AUDIO_REC_PORT2
endif   # ENABLE_I2S
endif   # __BCM89106_EVK__

ifeq ($(call iseeopt, __BCM89107_EVK__), yes)
CORTEX_MCU_LINKERSCRIPT = $(BRCM_SDK_ROOT)/system/bcm8910x/board/89107_evk/gnu.ld
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/89107_evk/board.c
CFLAGS += -DDDR_SIZE_MB=64
ifeq ($(call iseeopt, ENABLE_I2S), yes)
CFLAGS += -DAUDIO_PLAY_PORT0_HW_ID=0UL
CFLAGS += -DAUDIO_PLAY_PORT1_HW_ID=1UL
CFLAGS += -DAUDIO_PLAY_PORT2_HW_ID=2UL
CFLAGS += -DAUDIO_REC_PORT0_HW_ID=0UL
CFLAGS += -DAUDIO_REC_PORT1_HW_ID=1UL
CFLAGS += -DAUDIO_REC_PORT2_HW_ID=2UL
ifeq ($(call iseeopt, ENABLE_AUDIO_PLAY_PORT0), yes)
CFLAGS += -DENABLE_I2S0_PLAY
endif	# ENABLE_AUDIO_PLAY_PORT0
ifeq ($(call iseeopt, ENABLE_AUDIO_PLAY_PORT1), yes)
CFLAGS += -DENABLE_I2S1_PLAY
endif	# ENABLE_AUDIO_PLAY_PORT1
ifeq ($(call iseeopt, ENABLE_AUDIO_PLAY_PORT2), yes)
CFLAGS += -DENABLE_I2S2_PLAY
endif	# ENABLE_AUDIO_PLAY_PORT2
ifeq ($(call iseeopt, ENABLE_AUDIO_REC_PORT0), yes)
CFLAGS += -DENABLE_I2S0_REC
endif	# ENABLE_AUDIO_REC_PORT0
ifeq ($(call iseeopt, ENABLE_AUDIO_REC_PORT1), yes)
CFLAGS += -DENABLE_I2S1_REC
endif	# ENABLE_AUDIO_REC_PORT1
ifeq ($(call iseeopt, ENABLE_AUDIO_REC_PORT2), yes)
CFLAGS += -DENABLE_I2S2_REC
endif	# ENABLE_AUDIO_REC_PORT2
endif	# ENABLE_I2S
endif	# __BCM89107_EVK__

ifeq ($(call iseeopt, __BCM89105_DDR_EVK__), yes)
CORTEX_MCU_LINKERSCRIPT = $(BRCM_SDK_ROOT)/system/bcm8910x/board/89105_ddr_evk/gnu.ld
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/89105_ddr_evk/board.c
CFLAGS += -DDDR_SIZE_MB=64
endif	# __BCM89105_DDR_EVK__

ifeq ($(call iseeopt, __BCM89109_DDR_EVK__), yes)
CORTEX_MCU_LINKERSCRIPT = $(BRCM_SDK_ROOT)/system/bcm8910x/board/89109_ddr_evk/gnu.ld
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/89109_ddr_evk/board.c
CFLAGS += -DDDR_SIZE_MB=64
endif	# __BCM89109_DDR_EVK__

ifeq ($(call iseeopt, __BCM89100A0_PRODESIGN__), yes)
CORTEX_MCU_LINKERSCRIPT = $(BRCM_SDK_ROOT)/system/bcm8910x/board/89100a0_prodesign/gnu.ld
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/board/89100a0_prodesign/board.c
endif	# __BCM89100A0_PRODESIGN__

# erika file
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/chip/common/early_init.c
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/os/erika/late_init.c
EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/os/erika/bcm8910x_system.c

EE_SRCS += $(BRCM_SDK_ROOT)/system/bcm8910x/os/erika/hooks.c
# Set Systick to 3.125 milli seconds for AVB
CFLAGS += -DSYS_TICK_US=3125UL
CFLAGS += -DWDS_CHECK_TIME_US=100000UL

# Set MDIO timer channel ID and clock expiry time
CFLAGS += -DMDIO_TIMER_CHANID=1UL -DMDIO_TIMER_EXPIRY_TICKS=500UL


endif	# __BCM8910x__
