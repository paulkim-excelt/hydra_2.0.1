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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
####################################################################################

##  @defgroup grp_vgc VGC
#   @ingroup grp_drivers
#
#   @defgroup grp_vgc_media_comp VGC Media Component
#   @ingroup grp_media_system
#
#   @defgroup grp_vgcd_service VGC Display Service
#   @ingroup grp_system_sw
#
#   @addtogroup grp_vgc
#   @{
#   @section sec_vgc_overview Overview
#   Detailed Overview to be populated here.
#
#
#   @file multimedia/display/vgc/comp0.mk
#   @brief Makefile for VGC driver
#   @version 0.1 Initial version
#   @}
#

BRCM_CONFIGURATION_ITEM += comp0.mk
BRCM_CONFIGURATION_ITEM += inc/vgc_display.h
BRCM_CONFIGURATION_ITEM += inc/vgc_common.h
BRCM_CONFIGURATION_ITEM += inc/osil/vgc_osil.h
BRCM_CONFIGURATION_ITEM += os/erika/vgc_osil.c
BRCM_CONFIGURATION_ITEM += inc/vgc_display_service.h
BRCM_CONFIGURATION_ITEM += os/erika/vgc_display.c
BRCM_CONFIGURATION_ITEM += inc/vgc_comp.h
BRCM_CONFIGURATION_ITEM += os/erika/vgc_comp.c
BRCM_CONFIGURATION_ITEM += doc/vgc_req.c
BRCM_CONFIGURATION_ITEM += doc/vgcc_req.c
BRCM_CONFIGURATION_ITEM += doc/vgcd_req.c
BRCM_CONFIGURATION_ITEM += lib/dsi/dsi_api.h
BRCM_CONFIGURATION_ITEM += lib/dsi/dsi.c
BRCM_CONFIGURATION_ITEM += lib/cmp/cmp.c
BRCM_CONFIGURATION_ITEM += lib/cmp/cmp_api.h
BRCM_CONFIGURATION_ITEM += lib/gfd/gfd.c
BRCM_CONFIGURATION_ITEM += lib/gfd/gfd_api.h
BRCM_CONFIGURATION_ITEM += lib/mfd/mfd.c
BRCM_CONFIGURATION_ITEM += lib/mfd/mfd_api.h
BRCM_CONFIGURATION_ITEM += lib/pv/pv.c
BRCM_CONFIGURATION_ITEM += lib/pv/pv_api.h
BRCM_CONFIGURATION_ITEM += lib/afe/afe.c
BRCM_CONFIGURATION_ITEM += lib/afe/afe_api.h
BRCM_CONFIGURATION_ITEM += lib/misc/misc.c
BRCM_CONFIGURATION_ITEM += lib/misc/misc_api.h
BRCM_CONFIGURATION_ITEM += lib/vgc.c
BRCM_CONFIGURATION_ITEM += lib/vgc_disp_drv.c


BRCM_SRC :=
BRCM_CFLAGS := -DENABLE_VGC_DISPLAY
BRCM_INC :=

#Enable this for debug statistics. Strictly for DEV build ONLY!
#BRCM_CFLAGS += -DVGC_DEBUG_SAMPLES=<no. of samples>

#Path to display git from repo
DISPLAY_PATH := $(BRCM_SDK_ROOT)/multimedia/display/vgc

INC_PATH := $(DISPLAY_PATH)/inc
SRC_PATH := $(DISPLAY_PATH)/lib

#Includes
BRCM_INC += $(INC_PATH)
BRCM_INC += $(INC_PATH)/osil
BRCM_INC += $(SRC_PATH)
BRCM_INC += $(SRC_PATH)/cmp
BRCM_INC += $(SRC_PATH)/gfd
BRCM_INC += $(SRC_PATH)/mfd
BRCM_INC += $(SRC_PATH)/pv
BRCM_INC += $(SRC_PATH)/afe
BRCM_INC += $(SRC_PATH)/misc
BRCM_INC += $(SRC_PATH)/dsi

#Sources
BRCM_SRC += $(SRC_PATH)/vgc.c
BRCM_SRC += $(SRC_PATH)/vgc_disp_drv.c
BRCM_SRC += $(SRC_PATH)/cmp/cmp.c
BRCM_SRC += $(SRC_PATH)/gfd/gfd.c
BRCM_SRC += $(SRC_PATH)/mfd/mfd.c
BRCM_SRC += $(SRC_PATH)/pv/pv.c
BRCM_SRC += $(SRC_PATH)/afe/afe.c
BRCM_SRC += $(SRC_PATH)/misc/misc.c
BRCM_SRC += $(SRC_PATH)/dsi/dsi.c

BRCM_COMP_NAME := vgc_display
BRCM_COMP_DEPENDS := utils
BRCM_COMP_TYPE := lib

