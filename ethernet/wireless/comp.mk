#
# Copyright 2018-2019 Broadcom Limited.  All rights reserved.
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
####################################################################################

##  @defgroup grp_wireless Wireless Ethernet
#   @ingroup grp_comms_sw
#
#   @addtogroup grp_wireless
#   @{
#   @section Overview Wireless Ethernet Overview
#   @image html wireless_ethernet_overview.jpg "Wireless Ethernet architecture"
#
#   Wireless ethernet component consist of two interfaces: wireless controller
#   interface (wl_eth.h) and wireless transceive interface (wl_eth_xcvr.h).
#
#   Wireless controller interface provides APIs to initialize the controller,
#   change the mode of the controller, set the mac address of the wireless chip
#   and send the ethernet packet over the air.
#
#   Transceiver interface provides APIs to initialize the wireless transceiver
#   (downloading of the wireless firmware, initializing the chip etc),
#   set/get the Radio parameters of the chip and set the mode (on/off)
#   of the transceiver.
#
#
#	@limitations
#   @file ethernet/wireless/drivers/80211/comp.mk
#   @brief Makefile for wireless ethernet stack
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

SRC_DIR := $(BRCM_SDK_ROOT)/ethernet/wireless/lib
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/wireless/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/wireless/inc/osil
BRCM_INC += $(SRC_DIR)/

BRCM_CFLAGS += -DENABLE_BCM_4335
BRCM_CFLAGS += -DWLETH_HW_ID_MAX=1UL
BRCM_CFLAGS += -DWLETH_XCVR_ID_MAX=1UL

BRCM_SRC += $(SRC_DIR)/wl_eth.c
BRCM_SRC += $(SRC_DIR)/wl_drv.c
BRCM_SRC += $(SRC_DIR)/wl_sdpcm.c
BRCM_SRC += $(SRC_DIR)/wl_sdio.c
BRCM_SRC += $(SRC_DIR)/wl_logging.c

BRCM_COMP_NAME := wlandrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

