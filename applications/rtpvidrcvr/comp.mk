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


##  @defgroup grp_rtpvidrcvr RTP Video Receiver
#   @ingroup grp_applications
#
#   @addtogroup grp_rtpvidrcvr
#   @{
#   @image html rtpvidrcvr_overview.jpg "RTP Video Receiver Overview"
#   This application (RTPVidRcvr) initializes RTP Listener, AVC decoder, Graphics Source and VGC (Display)
#   components to receive encoded video content over UDP using INET Stack, decode and display the
#   content with Graphics blending. This application uses the standard Media Framework as described in
#   @ref grp_framework.
#
#   @section sec_rtpvidrcvr_target Running the application on target
#   -# Follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger
#   -# This application can run only on BCM89107 EVK and BCM89109 EVK boards (Rev-C) with Display.
#   -# This application by default compiled with BRPHY in slave mode.
#   -# Setup a BCM8910X_EVK (Rev-C) board with Camera and <i>rtpvidxmtr</i> flashed on it.
#   -# Connect BCM89107/BCM89109 EVK board to BCM8910X_EVK board (back to back) using BRPHY cable
#   -# Reset both boards. Content captured by BCM8910X_EVK camera can now be seen on
#      BCM89107/BCM89109 EVK display board.
#
#   @note This application uses UDP for streaming compressed data.
#
#   @file applications/rtpvidrcvr/comp.mk
#   @brief Makefile for RTP VidRcvr
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC := $(BRCM_SDK_ROOT)/applications/rtpvidrcvr/rtpvidrcvr.c

BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL

BRCM_CFLAGS += -DENABLE_MAC0=1
BRCM_CFLAGS += -DBRPHY_CONN_MODE_SLAVE=1
BRCM_CFLAGS += -DRTP_MAX_NUM_OUTPUT_PORTS=1UL
BRCM_CFLAGS += -DRTP_ENABLE_RTCP_1733_SUPPORT=1
BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_CFLAGS += -DETS_MAX_INTERFACES=1UL

BRCM_SYSCFG_MAC_CFG := config1
BRCM_SYSCFG_XCVR_CFG := slave
BRCM_COMP_NAME := rtpvidrcvr
BRCM_COMP_DEPENDS := vgc_display pplib avcd rtp inet ets
BRCM_COMP_TYPE := app

BRCM_APP_CFG_XML := bcm8910x

