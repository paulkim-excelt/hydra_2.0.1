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



##  @defgroup grp_vidxcvr Video Transceiver
#   @ingroup grp_applications
#
#   @addtogroup grp_vidxcvr
#   @{
#   @image html vidxcvr_overview.jpg "Video Transceiver Overview"
#   This application is a combination of Video Transmitter and Receiver. It sets up all the components used
#   in Video Transmitter and Receiver. It encodes Camera data and sends it over BroadR-Reach. In parallel,
#   it also receives encoded video stream over BroadR-Reach, decode and display the content on LVDS
#   panel.
#
#   @section sec_vidxcvr_target Running the application on target
#   -# Follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger
#   -# This application supports transmitting and receiving video data at the same time
#   -# This application should be run on two BCM89109 EVK boards (Rev-C) with Camera and
#      Display installed on both of them.
#   -# This application by default is compiled with BRPHY master mode and GPTP master mode
#      (<i>vidxcvr</i> application binary delivered as part APP_binaries.tar.gz is compiled in this mode)
#   -# To connect to devices back-to-back, one device needs to be in BRPHY master mode and
#      another device in BRPHY slave mode, so a new <i>vidxcvr</i> application shall be compiled
#      with BRPHY slave and GPTP slave configuration.
#      + Go to build folder
#      @code{.sh}
#      make clean
#      make board=<board_name> comp=vidxcvr BRCM_SYS_CFG_NAME=<config_file> where:
#         1. <board_name> bcm89109_ddr_ov10640_evk
#         2. <config_file> is system configuration file (with configurations for BRPHY
#            slave mode and GPTP slave mode).
#      eg., make board=bcm89109_ddr_ov10640_evk comp=vidxcvr
#      @endcode
#   -# Flash first device with master mode configuration (<i>vidxcvr</i> application binary
#      delivered as part APP_binaries.tar.gz ) and second device with slave mode configuration
#      (application binary generated from the previous step)
#   -# Connect the two boards back to back using BR cable and use shell on both the devices
#      to see UART log.
#   -# Reset both boards. Contents captured by one board can now be seen on other board’s display
#      board and vice-versa.
#
#   @file applications/vidxcvr/comp.mk
#   @brief Makefile for VidXcvr
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC := $(BRCM_SDK_ROOT)/applications/vidxcvr/vidxcvr.c

BRCM_CFLAGS += -DLDC_MEMPOOL_SIZE=\(\(10*1280*16*3\)/2\)
BRCM_CFLAGS += -DLDC_MP_MATRIX_DATA_SIZE=\(\(8*1280*720\)/256\)
BRCM_CFLAGS += -DAVBT_MAX_NUM_INPUT_PORTS=1UL
BRCM_CFLAGS += -DAVBL_MAX_NUM_OUTPUT_PORTS=1UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_ID=MSYS_COMP_ID_AVBT
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_PORTID=0UL

BRCM_CFLAGS += -DENABLE_MAC1=1

BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_CFLAGS += -DETS_MAX_INTERFACES=1UL

BRCM_COMP_NAME := vidxcvr
BRCM_COMP_DEPENDS := pplib avce isys 1722 ldc ets vgc_display avcd fqtss
BRCM_COMP_DEPENDS += isys_awb isys_agc isys_cc isys_denoise isys_defpix isys_sharpen isys_bl isys_tm
BRCM_COMP_TYPE := app


ifeq ($(BRCM_CAMERA_SENSOR),ov10640)
BRCM_APP_CFG_XML := bcm8910x_ov10640
else ifeq ($(BRCM_CAMERA_SENSOR),ox3a)
BRCM_APP_CFG_XML := bcm8910x_ox3a
endif

