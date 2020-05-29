#
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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################
# File Name: Makefile
# DESCRIPTION: Makefile to be invoked while running "make". This would do initial
#              pre-requisite which is required to build products and invoke
#              product makefile to build accordingly.
#####################################################################################

ifeq ($(BRCM_SYSCFG_XCVR_CFG),)
BRCM_SYSCFG_XCVR_CFG = master
endif

ifeq ($(BRCM_SYSCFG_MAC_CFG),)
BRCM_SYSCFG_MAC_CFG = default
endif

BRCM_APP_TARGET_DEPS := $(BRCM_APP_PREBUILDS)
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/bcm8910x_$(BRCM_SYSCFG_XCVR_CFG).bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/bcm8910x_swr.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).text.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.privileged.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.elf
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.map
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl_signed.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.img
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).jdebug
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).cmm
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/bl_cfg.txt
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/sys_cfg.txt
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/userdata.txt

BRCM_BL_NUM_COPIES ?= 2
BRCM_SYSCFG_NUM_COPIES ?= 2
BRCM_FW_NUM_COPIES ?= 2
BRCM_SECURE_KEY_COPIES ?= 1

ifeq ($(BRCM_FW_NUM_COPIES),)
BRCM_FW_NUM_COPIES := 2
endif

BRCM_PT_CREATOR_OPTS  = --sector_sz=0x1000 --pt_base=0x0 --edc 1
# assuming BL load address and BL size is withing first 256KB of SRAM, set pt load address offset to 256KB
BRCM_PT_CREATOR_OPTS += --pla 0x20040000
BRCM_PT_CREATOR_OPTS += --repo $(BRCM_SDK_ROOT) --outdir $(BRCM_APP_OBJ_DIR)/images --out=$(board)_$(comp)_$(OS)
BRCM_PT_CREATOR_OPTS += --max_num_img_entries $(ITBL_MAX_NUM_ENTRIES)
BRCM_PT_CREATOR_OPTS += --bl_cnt $(BRCM_BL_NUM_COPIES) --bl_cfg $(BRCM_APP_OBJ_DIR)/images/bl_cfg.txt --bl_entry_pt $(BRCM_BL_EP)
BRCM_PT_CREATOR_OPTS += --fw_cnt $(BRCM_FW_NUM_COPIES) --fw_cfg $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt --fw_entry_pt $(BRCM_FW_EP)
BRCM_PT_CREATOR_OPTS += --syscfg_cnt $(BRCM_SYSCFG_NUM_COPIES) --sys_cfg $(BRCM_APP_OBJ_DIR)/images/sys_cfg.txt
BRCM_PT_CREATOR_OPTS += --userdata $(BRCM_APP_OBJ_DIR)/images/userdata.txt

BRCM_PT_CREATOR_OPTS += --bl_sign 1 --pub_key $(BRCM_SDK_ROOT)/build/keys/public_key.bin --pub_key_la 0x20032000
BRCM_PT_CREATOR_OPTS += --priv_key $(BRCM_SDK_ROOT)/build/keys/private_key.dat
BRCM_PT_CREATOR_OPTS += --key_cnt $(BRCM_SECURE_KEY_COPIES)

BRCM_APP_CFG_DEP :=
ifneq ($(wildcard $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml),)
BRCM_APP_CFG_DEP += $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml
endif

$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).jdebug: $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf $(BRCM_SDK_ROOT)/build/bcm8910x/bcm8910x_load_app.jdebug
	$(eval BRCM_BL_EP := $(word 1,$(shell grep -R CORTEX_MX_RESET_HANDLER $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.map)))
	$(eval BRCM_BL_LA := $(word 1,$(shell grep -R __text_start__ $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.map)))
	$(eval BRCM_FW_EP := $(word 1,$(shell grep -R EE_cortex_mx_default_reset_ISR $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(eval BRCM_FW_EXE0_LA := $(word 1,$(shell grep -R __text_start__ $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(eval BRCM_FW_EXE1_LA := $(word 1,$(shell grep -R __stack_start__ $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(eval BRCM_FW_EXE2_LA := $(word 1,$(shell grep -R __data_start__ $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_SDK_ROOT)/build/bcm8910x/bcm8910x_load_app.jdebug $@
	$(QUIET)sed -i 's/BOOTLOADER_BIN_NAME/$(BRCM_BOOTLOADER_VARIANT)_bl.bin/g' $@
	$(QUIET)sed -i 's/BOOTLOADER_ELF_NAME/$(BRCM_BOOTLOADER_VARIANT)_bl.elf/g' $@
	$(QUIET)sed -i 's/BOOTLOADER_LOAD_ADDRESS/$(BRCM_BL_LA)/g' $@
	$(QUIET)sed -i 's/BOOTLOADER_ENTRY_POINT/$(BRCM_BL_EP)/g' $@
	$(QUIET)sed -i 's/APPLICATION_ELF_NAME/$(board)_$(comp)_$(OS).elf/g' $@
	$(QUIET)sed -i 's/APPLICATION_ENTRY_POINT/$(BRCM_FW_EP)/g' $@
	$(QUIET)sed -i 's/EXE0_BIN_NAME/$(board)_$(comp)_$(OS).text.bin/g' $@
	$(QUIET)sed -i 's/EXE1_BIN_NAME/$(board)_$(comp)_$(OS).data.privileged.bin/g' $@
	$(QUIET)sed -i 's/EXE2_BIN_NAME/$(board)_$(comp)_$(OS).data.bin/g' $@
	$(QUIET)sed -i 's/EXE0_LOAD_ADDRESS/$(BRCM_FW_EXE0_LA)/g' $@
	$(QUIET)sed -i 's/EXE1_LOAD_ADDRESS/$(BRCM_FW_EXE1_LA)/g' $@
	$(QUIET)sed -i 's/EXE2_LOAD_ADDRESS/$(BRCM_FW_EXE2_LA)/g' $@

$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).cmm: $(BRCM_SDK_ROOT)/build/bcm8910x/bcm8910x_load_app.cmm
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_SDK_ROOT)/build/bcm8910x/bcm8910x_load_app.cmm $@
	$(QUIET)sed -i 's/APPLICATION_ELF_NAME/$(board)_$(comp)_$(OS).elf/g' $@

$(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.elf: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.elf $@

$(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.map: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.map $@

$(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl_signed.bin: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)python $(BRCM_SDK_ROOT)/build/common/sign_img.py -i $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.bin -o $@ -k $(BRCM_SDK_ROOT)/build/keys/private_key.dat -f 512

$(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.bin: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_BOOTLOADER_DIR)/$(BRCM_BOOTLOADER_VARIANT)_bl.bin $@

$(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.img: $(BRCM_APP_PREBUILDS) $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl_signed.bin
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)python $(BRCM_SDK_ROOT)/build/bcm8910x/rom_tftp_img_creator.py --infile $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl_signed.bin --outfile $@ --edc 0x1 --hdrlen 256


$(BRCM_APP_OBJ_DIR)/images/bcm8910x_$(BRCM_SYSCFG_XCVR_CFG).bin: $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/config/bcm8910x/bcm8910x_$(BRCM_SYSCFG_XCVR_CFG).bin
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $< $@

$(BRCM_APP_OBJ_DIR)/images/bcm8910x_swr.bin: $(BRCM_SDK_ROOT)/system/bcm8910x/config/bcm8910x_swr.bin
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $< $@

%/images/bl_cfg.txt %/images/sys_cfg.txt %/images/app_cfg.txt: $(BRCM_APP_CFG_DEP) $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).jdebug $(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.img
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)echo "0x0 0x10000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(BRCM_BOOTLOADER_VARIANT)_bl.img) 0x2001CF40" > $(BRCM_APP_OBJ_DIR)/images/bl_cfg.txt
	$(QUIET)echo "0x9000 0x1000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/bcm8910x_$(BRCM_SYSCFG_XCVR_CFG).bin)" > $(BRCM_APP_OBJ_DIR)/images/sys_cfg.txt
	$(QUIET)echo "0x0 0x40000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).text.bin) $(BRCM_FW_EXE0_LA)" > $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
	$(QUIET)echo "0x1 0x20000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.privileged.bin) $(BRCM_FW_EXE1_LA)" >> $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
	$(QUIET)echo "0x2 0x30000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.bin) $(BRCM_FW_EXE2_LA)" >> $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
	$(QUIET)$(BRCM_SDK_ROOT)/build/common/xmlparse.py -a -b $(BRCM_APP_OBJ_DIR)/application.xml -c $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml -s $(BRCM_SDK_ROOT)/build/common/schema.xsd -o $(BRCM_APP_OBJ_DIR)/images -r $(BRCM_SDK_ROOT);
	$(QUIET)if grep -q 'multimedia/pp/openvx/config/vid_vmp_cv.bin' $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt ; then \
	  cp $(BRCM_SDK_ROOT)/multimedia/pp/openvx/config/table.bin.license.txt $(BRCM_APP_OBJ_DIR)/images/ ; \
	  cp $(BRCM_SDK_ROOT)/multimedia/pp/openvx/config/vid_vmp_cv.bin.license.txt $(BRCM_APP_OBJ_DIR)/images/ ; \
	fi

$(BRCM_APP_OBJ_DIR)/images/userdata.txt:
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)echo "0xBC10" >> $@

$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf: $(BRCM_APP_OBJ_DIR)/c_rX.bin
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_APP_OBJ_DIR)/c_rX.elf $@
	$(QUIET)cp $(BRCM_APP_OBJ_DIR)/c_rX.map $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).map

$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).%.bin: $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf
	$(QUIET)$(eval BRCM_TMP_VAR= $(subst .bin,,$(subst $(board)_$(comp)_$(OS),, $(shell basename $@ ))))
	$(QUIET)$(OBJCOPY) --output-target binary --only-section $(BRCM_TMP_VAR) $< $@

$(BRCM_APP_TARGET): prebuilds $(BRCM_APP_TARGET_DEPS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)python $(BRCM_SDK_ROOT)/build/common/pt_creator.py $(BRCM_PT_CREATOR_OPTS)
	$(QUIET)zip -q -j $@ $(BRCM_APP_OBJ_DIR)/images/*

FORCE:

