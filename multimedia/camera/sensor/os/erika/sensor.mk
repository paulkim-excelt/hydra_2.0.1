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
#
####################################################################################

ifeq ($(call iseeopt, __BCM__), yes)

OPT_CC += -DENABLE_CAM=1 -DENABLE_CAMERA_SENSOR=1
OPT_ASM += -DENABLE_CAM -DENABLE_CAMERA_SENSOR
OPT_CC += -DCSN_TASK=CAMERA_TASK

INCLUDE_PATH += $(BRCM_SDK_ROOT)/common/include
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/inc/

EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/camera_sensor.c
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/erika/camera_sensor_osil_erika.c
ifeq ($(CHIP_FAMILY), bcm8910x)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/board/bcm8910x/camera_sensor_board.c
endif
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_common.c
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_common_config.c
ifeq ($(call iseeopt, ENABLE_CAM_MODULE_AR0143), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_ar0143.c
endif
ifeq ($(call iseeopt, ENABLE_CAM_MODULE_OX3A10), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_ox03a10.c
endif
ifeq ($(call iseeopt, ENABLE_CAM_MODULE_OV10640), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_ov10640.c
endif
ifeq ($(call iseeopt, ENABLE_CAM_MODULE_OV9716), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_ov9716.c
endif

endif   # __BCM__
