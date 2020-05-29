#
# Copyright 2019 Broadcom Limited.  All rights reserved.
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

BRCM_SRC :=

BRCM_CFLAGS += -DENABLE_CAM=1 -DENABLE_CAMERA_SENSOR=1

BRCM_CFLAGS += -DDISABLE_TUNERS
BRCM_CFLAGS += -DUNICAM_TASK=CAMERA_TASK
BRCM_CFLAGS += -DISP_TASK=CAMERA_TASK
BRCM_CFLAGS += -DUNICAM_DISABLE_TUNERS
BRCM_CFLAGS += -DISP_DISABLE_TUNERS
BRCM_CFLAGS += -DCSN_TASK=CAMERA_TASK

BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/os/erika/isp_osil_erika.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/os/erika/unicam_osil_erika.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/camera_sensor.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/erika/camera_sensor_osil_erika.c
ifeq ($(CHIP_FAMILY), bcm8910x)
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/board/bcm8910x/camera_sensor_board.c
endif
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_common.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_common_config.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/os/common/modules/module_ox03a10.c

BRCM_CFLAGS += -DCAMERA_TASK=BayerStreamTask

BRCM_INC += $(BRCM_SDK_ROOT)/common/include
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/include
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/include/osil
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/sensor/inc/
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/component/os/erika/inc

BRCM_CFLAGS += -DENABLE_SHELL_AUTO_TEST_FUNC=1

CAMERA_CFG = ox03a10_csi_720p_16_dcg_12_vs_isp_hdr_bypass

BRCM_CFLAGS += -DMAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMAX_NUM_RAW_CHANNELS=2UL
BRCM_CFLAGS += -DRAW_OUT_BPP=16UL

BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/configurations/outputs/$(CAMERA_CFG)/isp_image.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/configurations/outputs/$(CAMERA_CFG)/unicam_image.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/configurations/outputs/$(CAMERA_CFG)/cam_mode_image.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/configurations/outputs/$(CAMERA_CFG)/sensor_common_image.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/configurations/outputs/$(CAMERA_CFG)/sensor_mode_colorbar_image.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/component/tests/qualification/bayer_stream/bayer_stream.c

BRCM_COMP_NAME := bayer_stream_qt
BRCM_COMP_DEPENDS := isys pplib
BRCM_COMP_TYPE := app

BRCM_APP_CFG_XML := bcm8910x_ox3a

