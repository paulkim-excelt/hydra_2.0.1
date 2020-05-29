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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################



##  @defgroup grp_vdec Vendor decoder
#   @ingroup grp_media_system
#
#   @addtogroup grp_vdec
#   @{
#   @section sec_vdec_overview Overview
#   Vendor decoder
#
#   @limitations None
#
#   @file multimedia/pp/vdec/comp.mk
#   @brief Make file for VDEC library
#   @version 0.1 Inital version
#   @}
#

BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/queue/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/time/inc

BRCM_COMP_REQ_SRC += doc/vdec_req.c

BRCM_COMP_EXPORT_HDR += inc/vdec.h
BRCM_COMP_EXPORT_HDR += inc/vdec_lib.h

BRCM_COMP_LIB_SRC += lib/vdec_lib.c

BRCM_COMP_QT_SRC += tests/qualification/vdec_qt.h

BRCM_COMP_APP_OPT += ENABLE_VDEC

BRCM_COMP_APP_SRC += os/erika/vdec_comp.c

BRCM_COMP_APP_TASK += VDEC_Task
VDEC_Task.priority = 10
VDEC_Task.autostart = false
VDEC_Task.stack = 2048
VDEC_Task.event += SystemEvent0
VDEC_Task.event += SystemEvent1
VDEC_Task.event += SystemEvent4
VDEC_Task.event += SystemEvent5
VDEC_Task.event += SystemEvent6
VDEC_Task.event += SystemEvent7
VDEC_Task.event += SystemEvent8
VDEC_Task.event += SystemEvent9
VDEC_Task.event += SystemEvent10
VDEC_Task.event += SystemEvent11
VDEC_Task.event += PP_GPEvent


BRCM_COMP_APP_CFG += vdec_mp_8bpp
vdec_mp_8bpp.pid = 0x5
vdec_mp_8bpp.imgid = 0xDE90
vdec_mp_8bpp.maxsz = 0x5000
vdec_mp_8bpp.file = multimedia/pp/vdec/config/vid_vmp_h264_hi_8bpp_dec.bin

BRCM_COMP_APP_CFG += vdec_mp_nbpp
vdec_mp_nbpp.pid = 0x5
vdec_mp_nbpp.imgid = 0xDE91
vdec_mp_nbpp.maxsz = 0x4400
vdec_mp_nbpp.file = multimedia/pp/vdec/config/vid_vmp_h264_hi_nbpp_dec.bin

BRCM_COMP_APP_CFG += vdec_mp_copy
vdec_mp_copy.pid = 0x5
vdec_mp_copy.imgid = 0xDE92
vdec_mp_copy.maxsz = 0x2800
vdec_mp_copy.file = multimedia/pp/vdec/config/call_vmp_copy_machine.bin

BRCM_COMP_APP_CFG += vdec_sp_cavlc
vdec_sp_cavlc.pid = 0x5
vdec_sp_cavlc.imgid = 0xDEA0
vdec_sp_cavlc.maxsz = 0x10000
vdec_sp_cavlc.file = multimedia/pp/vdec/config/vid_vsp_h264_hi_cavlc_dec.bin

BRCM_COMP_APP_CFG += vdec_sp_cabac
vdec_sp_cabac.pid = 0x5
vdec_sp_cabac.imgid = 0xDEA1
vdec_sp_cabac.maxsz = 0x10000
vdec_sp_cabac.file = multimedia/pp/vdec/config/vid_vsp_h264_hi_cabac_dec.bin

BRCM_COMP_NAME := vdec
BRCM_COMP_DEPENDS := pplib queue time
BRCM_COMP_TYPE := lib

