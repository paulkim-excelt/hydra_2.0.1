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


##  @defgroup grp_rtpvidxmtr RTP Video Transmitter
#   @ingroup grp_applications
#
#   @addtogroup grp_rtpvidxmtr
#   @{
#   @image html rtpvidxmtr_overview.jpg "RTP Video Transmitter Overview"
#   This application (RTPVidXmtr) initializes Camera, Lens Distortion Correction, AVC Encoder and RTP
#   Sender (which uses INET Stack for UDP support) components to stream encoded video content over
#   BroadR-Reach. This application uses the standard Media Framework as described in @ref grp_framework.
#   Camera, LDC, Graphics Source, VGC (Blend), AVC Encoder and RTP Sender hook into the Media
#   Framework as six components connected as below (from left to right). This application sets up the data
#   chain for the six components and starts streaming of encoded video over BroadR-Reach.
#
#   @section sec_rtpvidxmtr_target Running the application on target
#   @b Prerequisite:
#   -# Setup Ethernet connection as mentioned in @ref subsec_bcm8910x_ethernet_conn
#   -# Setup VLC player as mentioned in @ref page_bcm8910x_qsg_vlc OR <i>rtpvidxmtr</i>
#      application on another EVK board
#   -# Follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger
#
#   @note This application uses UDP for streaming compressed data.
#
#   @file applications/rtpvidxmtr/comp.mk
#   @brief Makefile for RTP VidXmtr
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC := $(BRCM_SDK_ROOT)/applications/rtpvidxmtr/rtpvidxmtr.c

BRCM_CFLAGS += -DLDC_MEMPOOL_SIZE=\(\(10*1280*16*3\)/2\)
BRCM_CFLAGS += -DLDC_MP_MATRIX_DATA_SIZE=\(\(8*1280*720\)/256\)
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL

BRCM_CFLAGS += -DENABLE_MAC1=1
BRCM_CFLAGS += -DRTP_MAX_NUM_INPUT_PORTS=1UL
BRCM_CFLAGS += -DRTP_ENABLE_RTCP_1733_SUPPORT=1
BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_CFLAGS += -DETS_MAX_INTERFACES=1UL

BRCM_COMP_NAME := rtpvidxmtr
BRCM_COMP_DEPENDS := pplib avce isys rtp ldc vgc_mwb ets inet
BRCM_COMP_DEPENDS += isys_awb_gw isys_agc isys_cc isys_denoise isys_defpix isys_sharpen isys_bl isys_tm
BRCM_COMP_TYPE := app

ifeq ($(BRCM_CAMERA_SENSOR),ov10640)
BRCM_APP_CFG_XML := bcm8910x_ov10640
else ifeq ($(BRCM_CAMERA_SENSOR),ox3a)
BRCM_APP_CFG_XML := bcm8910x_ox3a
else ifeq ($(BRCM_CAMERA_SENSOR),ar143)
BRCM_APP_CFG_XML := bcm8910x_ar143
endif

