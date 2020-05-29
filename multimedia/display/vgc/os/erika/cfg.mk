#
# Copyright 2016-2017 Broadcom Limited.  All rights reserved.
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
ifeq ($(call iseeopt, __BCM__), yes)

ifeq ($(call iseeopt, ENABLE_VGC), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/display/vgc/os/erika/vgc_osil.c
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/display/vgc/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/display/vgc/inc/osil
endif	# ENABLE_VGC

ifeq ($(call iseeopt, ENABLE_VGCC), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/display/vgc/os/erika/vgc_comp.c
CFLAGS += -DVGC_ClientTask=VGCC_Task

ifeq ($(call iseeopt, ENABLE_VGCC_DISPLAY), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/multimedia/display/vgc/os/erika/vgc_display.c
CFLAGS += -DVGCD_TASK_ID=VGCC_Task
else
CFLAGS += -DENABLE_VGCC_OUTPUT_PORT
endif   # ENABLE_VGCC_DISPLAY

ifeq ($(call iseeopt, ENABLE_VGCC_VIDEO), yes)
CFLAGS += -DENABLE_VIDEO_PORT

ifeq ($(call iseeopt, ENABLE_VGCC_FRAME), yes)
CFLAGS += -DVGCC_INPORT_VIDEO_BUFFERS=1UL
endif

ifeq ($(call iseeopt, ENABLE_VGCC_VID_12BPP), yes)
CFLAGS += -DVGC_VID_BPP=12UL
else ifeq ($(call iseeopt, ENABLE_VGCC_VID_16BPP), yes)
CFLAGS += -DVGC_VID_BPP=16UL
else ifeq ($(call iseeopt, ENABLE_VGCC_VID_24BPP), yes)
CFLAGS += -DVGC_VID_BPP=24UL
else ifeq ($(call iseeopt, ENABLE_VGCC_VID_32BPP), yes)
CFLAGS += -DVGC_VID_BPP=32UL
else
CFLAGS += -DVGC_VID_BPP=48UL
endif   # ENABLE_VGCC_VID_12BPP

endif   # ENABLE_VGCC_VIDEO

ifeq ($(call iseeopt, ENABLE_VGCC_GRAPHICS), yes)
CFLAGS += -DENABLE_GRAPHICS_PORT

ifeq ($(call iseeopt, ENABLE_VGCC_GFX_1BPP), yes)
CFLAGS += -DVGC_GFX_BPP=1UL
else ifeq ($(call iseeopt, ENABLE_VGCC_GFX_2BPP), yes)
CFLAGS += -DVGC_GFX_BPP=2UL
else ifeq ($(call iseeopt, ENABLE_VGCC_GFX_4BPP), yes)
CFLAGS += -DVGC_GFX_BPP=4UL
else ifeq ($(call iseeopt, ENABLE_VGCC_GFX_8BPP), yes)
CFLAGS += -DVGC_GFX_BPP=8UL
else ifeq ($(call iseeopt, ENABLE_VGCC_GFX_16BPP), yes)
CFLAGS += -DVGC_GFX_BPP=16UL
else
CFLAGS += -DVGC_GFX_BPP=32UL
endif   # ENABLE_VGCC_GFX_1BPP

ifeq ($(call iseeopt, ENABLE_VGCC_DISPLAY), yes)
CFLAGS += -DVGC_WIDTH=DISPLAY_WIDTH
CFLAGS += -DVGC_HEIGHT=DISPLAY_HEIGHT
else
CFLAGS += -DVGC_WIDTH=MEDIA_SYSTEM_MAX_VIDEO_WIDTH
CFLAGS += -DVGC_HEIGHT=VGC_IL_LINES_PER_MB
endif

endif   # ENABLE_VGCC_GRAPHICS

endif	# ENABLE_VGCC

endif   # __BCM__
