#
# Copyright 2017 Broadcom Limited.  All rights reserved.
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
# File Name: prebuild.mk
# DESCRIPTION: makefile to generate prebuild files required for build.
#####################################################################################

BRCM_APP_PREBUILDS = $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.bin
BRCM_APP_PREBUILDS += $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.jdebug

$(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.jdebug: $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.bin $(BRCM_SDK_ROOT)/build/bcm8910x/bcm8910x_load_bl.jdebug
	$(eval BRCM_BL_EP := $(word 1,$(shell grep -R CORTEX_MX_RESET_HANDLER $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.map)))
	$(eval BRCM_BL_LA := $(word 1,$(shell grep -R __text_start__ $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.map)))
	$(QUIET)cp $(BRCM_SDK_ROOT)/build/bcm8910x/bcm8910x_load_bl.jdebug $@
	$(QUIET)sed -i 's/BOOTLOADER_BIN_NAME/$(BRCM_BOOTLOADER_VARIANT)_bl.bin/g' $@
	$(QUIET)sed -i 's/BOOTLOADER_ELF_NAME/$(BRCM_BOOTLOADER_VARIANT)_bl.elf/g' $@
	$(QUIET)sed -i 's/BOOTLOADER_LOAD_ADDRESS/$(BRCM_BL_LA)/g' $@
	$(QUIET)sed -i 's/BOOTLOADER_ENTRY_POINT/$(BRCM_BL_EP)/g' $@

$(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.bin: $(BRCM_CHIP_LIBS_LIST)
	$(QUIET)$(MAKE) -C $(BRCM_SDK_ROOT)/bootloader/build TARGET_DIR=$(BRCM_BOOTLOADER_DIR) board=$(BRCM_BOOTLOADER_VARIANT) BRCM_BL_OBJ_DIR=$(BRCM_BOOTLOADER_OBJ_DIR) VERBOSE=$(VERBOSE) ITBL_MAX_NUM_ENTRIES=$(ITBL_MAX_NUM_ENTRIES)

