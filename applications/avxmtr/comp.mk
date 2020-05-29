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
#
####################################################################################

##  @defgroup grp_avxmtr Audio-Video Transmitter
#   @ingroup grp_applications
#
#   @addtogroup grp_avxmtr
#   @{
#   This application (avxmtr) initializes Camera, Audio-In (mic), Lens Distortion
#   Correction, AVC Encoder and AVB Talker components to stream encoded video content
#   over BroadR-Reach using 1722 protocol.
#
#   @section sec_avxmtr_target Running the application on target
#   -# Follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger
#   -# These applications shall be run with two BCM89107_EVK (Rev-C) boards
#      + <i>avxmtr</i> on one BCM89107_EVK (Rev-C) board with Camera and MIC input
#        (bcm89107_ox3a_evk_avxmtr_erika.zip)
#      + <i>avrcvr</i> on the other BCM89107_EVK (Rev-C) board with display and speaker output
#        (bcm89107_evk_avrcvr_erika.zip)
#   -# I2S port on both the boards shall be configured before running the applications.
#      Refer @ref subsec_bcm8910x_i2s_conn
#   -# <i>avxmtr</i> application by default is compiled with BRPHY master mode and GPTP master mode
#      (avxmtr application binary delivered as part APP_binaries.tar.gz is compiled in this mode)
#   -# <i>avrcvr</i> application by default is compiled with BRPHY slave mode and GPTP slave mode
#      (avrcvr application binary delivered as part APP_binaries.tar.gz is compiled in this mode)
#   -# Flash first device with <i>avxmtr</i> and second device with <i>avrcvr</i> application.
#   -# Connect the two BCM89107_EVK boards back to back using BR cable.
#   -# Reset both boards. Contents captured by one BCM89107_EVK board running <i>avxmtr</i> can now be
#      seen on avrcvr’s display board.
#   -# Audio captured (MIC input) by avxmtr is send to <i>avrcvr</i> device which can be heard over speaker.
#
#   @note While running <i>avxmtr</i> or <i>avrcvr</i> application, UART logs shall not be available since the
#   UART pins are used for I2S port.
#
#   @file applications/avxmtr/comp.mk
#   @brief Makefile for Audio Video Transmitter application
#   @version 0.1 Initial version
#   @}
#


BRCM_CFLAGS += -DLDC_MEMPOOL_SIZE=\(\(10*1280*16*3\)/2\)
BRCM_CFLAGS += -DLDC_MP_MATRIX_DATA_SIZE=\(\(8*1280*720\)/256\)
BRCM_CFLAGS += -DAVBT_MAX_NUM_INPUT_PORTS=2UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_ID=MSYS_COMP_ID_AVBT
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_PORTID=0UL

BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_CFLAGS += -DETS_MAX_INTERFACES=1UL

BRCM_SRC += $(BRCM_SDK_ROOT)/applications/avxmtr/avxmtr.c

BRCM_COMP_NAME := avxmtr
BRCM_COMP_DEPENDS := pplib avce isys 1722 ldc audio ets
BRCM_COMP_DEPENDS += isys_awb isys_agc isys_cc isys_denoise isys_defpix isys_sharpen isys_bl isys_tm
BRCM_COMP_TYPE := app

ifeq ($(BRCM_CAMERA_SENSOR),ox3a)
BRCM_APP_CFG_XML := bcm8910x_ox3a
endif #ifeq ($(BRCM_CAMERA_SENSOR),ox3a)

