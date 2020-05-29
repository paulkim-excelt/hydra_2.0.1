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

##  @defgroup grp_vdec_demo VDEC Demo
#   @ingroup grp_applications
#
#   @addtogroup grp_vdec_demo
#   @{
#   This application initializes RTP Listener, VDEC decoder, Graphics Source and VGC (Display)
#   components to receive H.264 Hi10 encoded video content over UDP using INET Stack, decode and display the
#   content with Graphics blending. This application uses the standard Media Framework as described in
#   @ref grp_framework.
#
#   @section sec_vdec_demo_target Running the application on target
#   -# Follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger
#   -# This application can run only on BCM89107 EVK and BCM89109 EVK boards (Rev-C) with Display.
#   -# Setup Ethernet connection as mentioned in @ref subsec_bcm8910x_ethernet_conn , with IP address
#      set to 192.168.10.2
#   -# Compile the application using gcc and verify that the app "brcm_rtp_streamer" is created
#      <i>gcc <repo>/ethernet/tcp_ip/utils/host/src/brcm_rtp_streamer.c -o brcm_rtp_streamer</i>
#   -# Execute the application on Linux PC using the below command
#    <i>sudo ./brcm_rtp_streamer -f <file name> -i <interface> -r <frame_rate> -s <num_slice_per_frame> </i>
#    where:
#       <file name> : Raw H264 file to be used for streaming
#       <interface> : Network interface e.g. eth0, eno1 etc.
#       <frame_rate> : The number of H264 video frames that needs to be transmitted per second
#       <num_slice_per_frame> : Number of h264 slices/NALs in a video frame_rate
#
#
#    Example usage: # sudo ./brcm_rtp_streamer -f parkrun_1280x720p.264.bin -i eth0 -r 30 -s 1
#
#   Similar instructions are available at <repo>/ethernet/tcp_ip/utils/host/src/brcm_rtp_streamer_readme.txt
#
#   @note This application uses UDP for streaming compressed data.
#
#   @file applications/vdec_demo/comp.mk
#   @brief Makefile for VDEC demo
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC := $(BRCM_SDK_ROOT)/applications/vdec_demo/vdec_demo.c

BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL
BRCM_CFLAGS += -DPP_CODEC_MODE=1UL
BRCM_CFLAGS += -DVDEC_NUM_REF_BUFFERS=2UL
BRCM_CFLAGS += -DVDEC_REF_MEM_DATA_SIZE=5242880UL
BRCM_CFLAGS += -DVDEC_COMP_NUM_STREAMS=1UL
BRCM_CFLAGS += -DVDEC_NUM_IN_BUFFERS_PER_PORT=2UL
BRCM_CFLAGS += -DVDEC_IN_BUFFER_SIZE=262144UL

BRCM_CFLAGS += -DENABLE_SHELL_APP=1

BRCM_CFLAGS += -DENABLE_MAC0=1
BRCM_CFLAGS += -DBRPHY_CONN_MODE_SLAVE=1
BRCM_CFLAGS += -DRTP_MAX_NUM_OUTPUT_PORTS=1UL
BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_SYSCFG_MAC_CFG := config1
BRCM_SYSCFG_XCVR_CFG := slave

BRCM_COMP_NAME := vdec_demo
BRCM_COMP_DEPENDS := vgc_display vdec pplib rtp inet
BRCM_COMP_TYPE := app

BRCM_APP_CFG_XML := bcm8910x

