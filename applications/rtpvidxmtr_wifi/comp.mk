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

##  @defgroup grp_rtpvidxmtr_wifi Wifi RTP Video Transmitter
#   @ingroup grp_applications
#
#   @addtogroup grp_rtpvidxmtr_wifi
#   @{
#   @image html rtpvidxmtr_overview.jpg "RTP Video Transmitter Overview"
#   This application (RTPVidXmtr) initializes Camera, Lens Distortion Correction, AVC Encoder and RTP
#   Sender (which uses INET Stack for UDP support) components to stream encoded video content over
#   WiFi. This application uses the standard Media Framework as described in @ref grp_framework.
#   Camera, LDC, Graphics Source, VGC (Blend), AVC Encoder and RTP Sender hook into the Media
#   Framework as six components connected as below (from left to right). This application connects
#   to a predefined wifi access point and streams the data over wifi.
#
#   This application by default uses below wifi access point configuration:
#   -# Access point name: PEOTEST
#   -# Securtiry mode : WPA2 Personal
#   -# Security key: PASSKEY1234
#
#   To disable WPA2 security, application needs to be recompiled with DENABLE_WLAN_WPA2 flag disabled in comp.mk.
#   When WPA2 is disabled, application uses below wifi access point configuration:
#   -# Access point name: PEOTEST
#   -# Securtiry mode : Open
#
#   When application starts, it will try to connect to given access point and start
#   streaming the encoded stream over RTP
#
#    @section sec_rtpvidxmtr_wifi_hardware_setup   Setting up the board for wifi card
#   -# Make sure RD2 register on module board is placed in position “B”
#   -# Make sure J43 is 1-2 is shorted on EVK base board (J43 is placed near SD Card slot)
#   -# Make sure that nothing is shorted on J92 header (J92 header is near JTAG port on the board)
#   -# SW11 dip switch configuration: 1: ON postion 2: OFF position (SW11 is placed near SD card slot)
#   -# Connect WIFI module card on SD slot
#
#   @section sec_ubuntu_wifi        Setting up the Ubuntu machine as wifi rtp receiver
#   -# Ubuntu machine shall be configured as WiFi hotspot (with access point name and security mode as
#      configured in the transmitter application). This application by default is configured to use
#      "POETEST" as access point
#   -# Change the name of the machine same as SSID name which trasmitter uses to connect to.
#       -# Go to Settings --> Details and change Device name field to “POETEST”
#   -# On Ubuntu machine bydefault hotspot name will be the name of the machine.
#      To configure the name of the hotspot (SSID) and password of access point,
#      open Network Connections editor tool by entering the following in either Terminal:
#      $ sudo nm-connection-editor
#      Above command with open "network connections" window. Create a new network connection
#      by clicking on "+" button below. Choose connection type as "wifi" and click on "create"
#   @image html vidxmtr_wifi_ubuntu_hotspot_configuration.jpg "Ubuntu hotspot setting"
#      Edit the connection settings as:
#      -# In connection name text box enter "HotSpot"
#      -# In <i>"Wifi"</i> Tab:
#           -# SSID text box enter "POETEST" (rtpvidxmtr_wifi application configuration uses
#              POETEST as AP configuration)
#           -# Select "Mode" as HotSpot
#           -# Select Band as "2.4 GHz"
#           -# Channel can be selected as “6” or can be left to “default”
#           -# In the device row, select the MAC address of the device (this shall not be left blank)
#           -# Keep MTU option as “Automatic”
#      -# In <i>Wi-Fi security</i> tab:
#           -# if WPA2 is enabled, select security as "WPA & WPA2 Personal" and in password text box enter "PASSKEY1234"
#           -# if WPA2 is disabled (Open mode), select the security as "None"
#      -# Save the configurtion and exit
#      -# Turn on the wifi hotspot:
#           -# open the Wi-Fi Settings from system menu.
#           -# From the menu on Wi-Fi Settings, select Turn On Wi-Fi Hotspot. WiFi access point should be up and running.
#           @image html vidxmtr_wifi_ubuntu_hotspot_turnon.jpg "Turning on Wifi HotSpot"
#
#   @section sec_rtpvidxmtr_wifi_target Running the application on target
#   @b Prerequisite:
#   -# Setup wifi card as mentioned in @ref sec_rtpvidxmtr_wifi_hardware_setup
#   -$ Flash this application on BCM89107 EVK board mounted with OX3A camera (follow the steps mentioned in Quick Start Guide @ref subsec_qsg_flash_segger)
#   -# To stream to VLC running on Ubuntu machine:
#       -# Setup wifi hotspot as mentioned in @ref sec_ubuntu_wifi
#       -# Setup VLC for rtp streaming as mentioned in @ref sec_vlc_setup_rtp
#   -# To stream to BCM8910X EVK wifi receiver, power up the board flashed with @b rtpvidrcvr_wifi (Refer to @ref grp_rtpvidrcvr_wifi)
#
#   @note This application uses UDP for streaming compressed data.
#
#   @file applications/rtpvidxmtr_wifi/comp.mk
#   @brief Makefile for Wifi RTP Video Transmitter application
#   @version 0.1 Initial version
#   @}
#


BRCM_SRC := $(BRCM_SDK_ROOT)/applications/rtpvidxmtr_wifi/rtpvidxmtr.c

BRCM_CFLAGS += -DLDC_MEMPOOL_SIZE=\(\(10*1280*16*3\)/2\)
BRCM_CFLAGS += -DLDC_MP_MATRIX_DATA_SIZE=\(\(8*1280*720\)/256\)
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL

BRCM_CFLAGS += -DENABLE_MAC1=1
BRCM_CFLAGS += -DRTP_MAX_NUM_INPUT_PORTS=1UL
BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_CFLAGS += -DETS_MAX_INTERFACES=1UL

#BRCM_CFLAGS += -DENABLE_STATIC_ARP
BRCM_CFLAGS += -DENABLE_WLAN_WPA2

BRCM_COMP_NAME := rtpvidxmtr_wifi
BRCM_COMP_DEPENDS := pplib avce isys rtp ldc vgc ets inet wlandrv sddrv
BRCM_COMP_DEPENDS += isys_awb isys_agc isys_cc isys_denoise isys_defpix isys_sharpen isys_bl isys_tm
BRCM_COMP_TYPE := app

ifeq ($(BRCM_CAMERA_SENSOR),ov10640)
BRCM_APP_CFG_XML := bcm8910x_ov10640
else ifeq ($(BRCM_CAMERA_SENSOR),ox3a)
BRCM_APP_CFG_XML := bcm8910x_ox3a
endif


