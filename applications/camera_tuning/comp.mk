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

##  @defgroup grp_camera_tuning Camera Tuning
#   @ingroup grp_applications
#
#   @addtogroup grp_camera_tuning
#   @{
#   This application is used for camera tuning from external PC.
#
#   @section sec_camtuning_target Running the application on target
#   -# Follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger
#   -# Make the IP address as static (192.168.10.2)
#   -# <b>While the application is running, reading/writing to memory, registers and
#      i2c devices shall be done after discussion with Broadcom.</b>
#   -# Setup UART as mentioned in @ref sec_qsg_pc_setupuart
#   -# Setup Ethernet connection as mentioned in @ref subsubsec_bcm8910x_ethernet_conn
#   -# <b>camera_tuning_utility.py</b> is used for IO/I2C Read/Write and Frame capture.
#   -# Below comands can be used for different resolutions:
#       - <i>python camera_tuning_utility.py 1280 720 420P bcm89103_ar143_evk_camera_tuning_erika.map</i>
#       - <i>python camera_tuning_utility.py 1280 720 420P bcm89107_ox3a_evk_camera_tuning_erika.map</i>
#       - <i>python camera_tuning_utility.py 1280 720 420P bcm89109_ddr_ov10640_evk_camera_tuning_erika.map</i>
#   -# When target receives a command packet, it is printed on UART.
#
#   @file applications/camera_tuning/comp.mk
#   @brief Makefile for Camera Tuning application
#   @version 0.1 Initial version
#   @}
#


BRCM_SRC :=
BRCM_SRC += $(BRCM_SDK_ROOT)/applications/camera_tuning/vidxmtr.c
BRCM_SRC += $(BRCM_SDK_ROOT)/applications/camera_tuning/brcm_cmd_if.c

BRCM_CFLAGS += -DAVCE_TOTAL_YUV_BUFFERS=2UL
BRCM_CFLAGS += -DAVBT_MAX_NUM_INPUT_PORTS=1UL
BRCM_CFLAGS += -DENABLE_MAC0=1
BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_CFLAGS += -DCAMERA_TUNING=1UL
ifneq ($(chip), bcm89103)
BRCM_CFLAGS += -DCAMERA_TUNING_FC=1UL
endif
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_ID=MSYS_COMP_ID_AVBT
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_PORTID=0UL

BRCM_COMP_NAME := camera_tuning
BRCM_COMP_DEPENDS := pplib avce isys 1722 inet
BRCM_COMP_DEPENDS += isys_awb isys_agc isys_cc isys_denoise isys_defpix isys_sharpen isys_bl isys_tm
BRCM_COMP_TYPE := app

ifeq ($(BRCM_CAMERA_SENSOR),ov10640)
BRCM_APP_CFG_XML := bcm8910x_ov10640
else ifeq ($(BRCM_CAMERA_SENSOR),ox3a)
BRCM_APP_CFG_XML := bcm8910x_ox3a
else ifeq ($(BRCM_CAMERA_SENSOR),ar143)
BRCM_APP_CFG_XML := bcm8910x_ar143
else ifeq ($(BRCM_CAMERA_SENSOR),ov9716)
BRCM_APP_CFG_XML := bcm8910x_ov9716
endif

