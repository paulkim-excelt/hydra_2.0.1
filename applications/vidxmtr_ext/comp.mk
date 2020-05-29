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
#
####################################################################################

##  @defgroup grp_vidxmtr_ext Video Transmitter (External)
#   @ingroup grp_applications
#
#   @addtogroup grp_vidxmtr_ext
#   @{
#   For DDR based platforms, the encoder would give out MP profile bit streams.
#   For non DDR based platforms, it would give out High profile intra only bit
#   streams.
#
#   @file applications/vidxmtr_ext/comp.mk
#   @brief Makefile for vidxmtr with external encoder support.
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC := $(BRCM_SDK_ROOT)/applications/vidxmtr/vidxmtr.c

BRCM_SRC :=
BRCM_SRC += $(BRCM_SDK_ROOT)/applications/vidxmtr_ext/vidxmtr.c

BRCM_CFLAGS += -DAVBT_MAX_NUM_INPUT_PORTS=1UL
BRCM_CFLAGS += -DENABLE_MAC0=1
BRCM_CFLAGS += -DENABLE_SHELL_APP=1
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_WIDTH=1280UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_HEIGHT=720UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_MAX_VIDEO_FPS=30UL
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_ID=MSYS_COMP_ID_AVBT
BRCM_CFLAGS += -DMEDIA_SYSTEM_STREAM_LOG_COMP_PORTID=0UL
BRCM_CFLAGS += -DETS_MAX_INTERFACES=1UL
BRCM_CFLAGS += -DPP_CODEC_MODE=1UL
ifeq ($(ENABLE_DDR), TRUE)
BRCM_CFLAGS += -DVENC_ENABLE_P_FRAME=1UL
endif
BRCM_INC += $(BRCM_SDK_ROOT)/common/include

BRCM_COMP_NAME := vidxmtr_ext
BRCM_COMP_DEPENDS := pplib venc isys 1722
BRCM_COMP_DEPENDS += isys_awb_gw isys_agc isys_cc isys_denoise isys_defpix isys_sharpen isys_bl isys_tm ets
BRCM_COMP_TYPE := app

ifeq ($(BRCM_CAMERA_SENSOR), ox3a)
BRCM_APP_CFG_XML := bcm8910x_ox3a
endif #ifeq ($(BRCM_CAMERA_SENSOR), 0x3a)

