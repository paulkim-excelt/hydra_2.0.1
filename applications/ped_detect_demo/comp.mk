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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################

##  @defgroup grp_ped_detect_demo Pedestrian Detection Demo
#   @ingroup grp_applications
#
#   @addtogroup grp_ped_detect_demo
#   @{
#   @image html ped_detect_demo_overview.jpg "Pedestrian Detect Demo Overview"
#
#   @section sec_ped_detect_demo_target Running the application on target
#   -# Follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger
#   -# This application can run only on BCM89106 EVK and BCM89107 EVK boards (Rev-C) with Display.
#   -# Setup UART as mentioned in @ref sec_qsg_pc_setupuart
#
#   @note
#   For detecting pedestrians, hold the board in portrait mode with power supply connection at the bottom.
#
#   @file applications/ped_detect_demo/comp.mk
#   @brief Makefile for Pedestrian Detection Demo
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC := $(BRCM_SDK_ROOT)/applications/ped_detect_demo/ped_detect_demo.c

BRCM_CFLAGS += -DLDC_MEMPOOL_SIZE=\(\(10*1280*16*3\)/2\)
BRCM_CFLAGS += -DLDC_MP_MATRIX_DATA_SIZE=\(\(8*1280*720\)/256\)
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_ID=MSYS_COMP_ID_AVBT
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_PORTID=0UL
BRCM_CFLAGS += -DAVBT_MAX_NUM_INPUT_PORTS=1UL
BRCM_CFLAGS += -DCAMERA_ENABLE_DUAL_PASS

BRCM_COMP_NAME := ped_detect_demo
BRCM_COMP_DEPENDS := pplib openvx isys vgc_display ldc avce 1722
BRCM_COMP_DEPENDS += isys_awb isys_agc isys_cc isys_denoise isys_defpix isys_sharpen isys_bl isys_tm
BRCM_COMP_TYPE := app

ifeq ($(BRCM_CAMERA_SENSOR),ox3a)
ifeq ($(board),bcm89106_ox3a_evk)
BRCM_APP_CFG_XML := bcm89106_ox3a
else
BRCM_APP_CFG_XML := bcm8910x_ox3a
endif
endif

