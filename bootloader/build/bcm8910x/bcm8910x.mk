##############################################################################
# Copyright 2017-2019 Broadcom Limited.  All rights reserved.
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
GNU_CC := 1
CHIP_FAMILY := bcm8910x
CPU := BL_CORTEX_M7

CCFLAGS += -D__BCM__  -D__BCM8910X__ -DBOOTROM_SUPPORTS_PT=1

BL_ENABLE_MPU := TRUE
BL_ENABLE_ICACHE := TRUE
BL_ENABLE_DCACHE := TRUE
BL_ENABLE_ETH_AMAC := TRUE
BL_ENABLE_FLASH := TRUE
BL_ENABLE_TIME := TRUE
BL_ENABLE_AVT := TRUE
BL_ENABLE_UART_PL011 := TRUE
BL_BCM_TIME_USES_GPT := TRUE
BL_ENABLE_LOG := TRUE
BL_ENABLE_PTM := TRUE
BL_ENABLE_GPIO := TRUE
BL_ENABLE_MDIO := TRUE
BL_ENABLE_SWR_CONFIG := TRUE
BL_ENABLE_TFTP_DWNLD_MODE := TRUE
BL_ENABLE_RAM_DWNLD_MODE := TRUE
BL_ENABLE_INET := TRUE
BL_ENABLE_SECURE_BOOT := TRUE
BL_ENABLE_TEST := FALSE
BL_ENABLE_FLASH_TEST := TRUE
BL_ENABLE_ETH_TEST := TRUE
BL_ENABLE_PTM_TEST := TRUE
BL_ENABLE_GPT := TRUE
BL_ENABLE_WATCHDOG_SP805 := TRUE

ifeq ($(BL_BCM_TIME_USES_AVT), TRUE)
CCFLAGS += -DBL_BCM_TIME_USES_AVT
endif

