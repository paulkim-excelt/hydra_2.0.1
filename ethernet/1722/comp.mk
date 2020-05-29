#
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
####################################################################################

##  @defgroup grp_avbt AVB Talker
#   @ingroup grp_media_system
#
#   @addtogroup grp_avbt
#   @{
#   @brief AVB Talker is the media component which is responsible for creation and
#   transmission of 1722 packets.
#   @file ethernet/1722/comp.mk
#   @brief Makefile for AVB library
#   @version 0.1 Initial version
#   @}
#
#   @defgroup grp_avbl AVB Listener
#   @ingroup grp_media_system
#
#   @addtogroup grp_avbl
#   @{
#   @brief AVB Listener is the media component which is responsible for reception and
#   parsing the 1722 packets.
#   @}
#
#   @defgroup grp_avb 1722 Library
#   @ingroup grp_comms_sw
#
#   @addtogroup grp_avb
#   @{
#   AVB library which implements the 1722 header addition and header parsing utility
#   functions. These APIs are used by the AVB media components.
#   @}

ifeq ($(CHIP_FAMILY),bcm8910x)
BRCM_CONFIGURATION_ITEM := comp.mk
BRCM_CONFIGURATION_ITEM += doc/1722_req.c
BRCM_CONFIGURATION_ITEM += doc/avbt_req.c
BRCM_CONFIGURATION_ITEM += inc/avb_core.h
BRCM_CONFIGURATION_ITEM += inc/crf.h
BRCM_CONFIGURATION_ITEM += lib/avb_core.c
BRCM_CONFIGURATION_ITEM += lib/crf.c
BRCM_CONFIGURATION_ITEM += lib/avb_internal.h
BRCM_CONFIGURATION_ITEM += lib/1722_talker.h
BRCM_CONFIGURATION_ITEM += lib/1722_talker.c
BRCM_CONFIGURATION_ITEM += lib/1722_listener.h
BRCM_CONFIGURATION_ITEM += lib/1722_listener.c
BRCM_CONFIGURATION_ITEM += inc/avb_talker.h
BRCM_CONFIGURATION_ITEM += inc/avb_talker_cfg.h
BRCM_CONFIGURATION_ITEM += os/erika/talker/avbt_comp.h
BRCM_CONFIGURATION_ITEM += os/erika/talker/avbt_comp.c
endif

ifeq ($(ENABLE_1722), TRUE)
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/1722/inc
BRCM_INC += $(BRCM_SDK_ROOT)/applications/include
BRCM_INC += $(BRCM_SDK_ROOT)/applications/include/os/erika

BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/1722/lib/1722_talker.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/1722/lib/1722_listener.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/1722/lib/avb_core.c
ifeq ($(ENABLE_CRF), TRUE)
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/1722/lib/crf.c
endif
endif

BRCM_COMP_NAME := 1722
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

