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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################

ifeq ($(call iseeopt, ENABLE_OPENVX), yes)
OPT_CC += -D__ENABLE_OPENVX__
OPT_ASM += -D__ENABLE_OPENVX__
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/include
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/include/osil

INCLUDE_PATH += $(BRCM_SDK_ROOT)/applications/include
CFLAGS += -DENABLE_OVC0=1
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/os/erika/openvx_comp.c
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/os/erika

ifeq ($(call iseeopt, ENABLE_PED_DETECT), yes)
CFLAGS += -DENABLE_PED_DETECT=1
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/config/pedestrian_detect
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/os/erika/ped_detect_comp.c
endif# ENABLE_PED_DETECT

ifeq ($(call iseeopt, ENABLE_OPTICAL_FLOW), yes)
CFLAGS += -DENABLE_OPTICAL_FLOW=1
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/config/optical_flow
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/os/erika/optical_flow_comp.c
endif# ENABLE_OPTICAL_FLOW

ifeq ($(call iseeopt, ENABLE_FACE_DETECT), yes)
CFLAGS += -DENABLE_FACE_DETECT=1
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/config/face_detect
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/os/erika/face_detect_comp.c
endif# ENABLE_FACE_DETECT
endif   # ENABLE_OPENVX
