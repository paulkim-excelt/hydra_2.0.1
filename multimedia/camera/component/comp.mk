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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################


##  @defgroup grp_camera Camera
#   @ingroup grp_media_system
#
#   @addtogroup grp_camera
#   @{
#   @section sec_camera_overview Overview
#   @image html camera_overview.jpg "Camera Media Component hierarchy"
#   Above figure depicts a block diagram representation of Camera system.
#   Camera Media Component abstracts different imaging components to provide
#   YUV streaming functionality.
#   @subsection subsec_camera_dual_pass Dual Pass
#   Use "CAMERA_ENABLE_DUAL_PASS" macro in application make file for enabling
#   dual pass mode in camera media component. This feature can only be enabled
#   for boards with DDR enabled.
#   @subsection subsec_camera_hv_format Human Vision formats
#   Following are the format macros which can be passed for static memory
#   allocation,
#   -# CAMERA_ISP_HV_OUT_FORMAT_422I  (YUYV, YVYU, UYVY, VYUY)
#   -# CAMERA_ISP_HV_OUT_FORMAT_422P  (YUV 422 PLANAR)
#   -# CAMERA_ISP_HV_OUT_FORMAT_444P  (YUV 444 PLANAR)
#   -# CAMERA_ISP_HV_OUT_FORMAT_420SP (YUV 420 SEMI-PLANAR)
#   -# CAMERA_ISP_HV_OUT_FORMAT_420P  (YUV 420 PLANAR (default))
#
#   @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
#   @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
#   @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
#   @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
#   @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
#   @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
#   @trace #BRCM_SWREQ_CAMERA_CONFIGURATOR
#   @trace #BRCM_SWREQ_CAMERA_OX3A_12DCG_12VS_HDR_720P_30FPS
#   @trace #BRCM_SWREQ_CAMERA_OX3A_12DCG_12VS_HDR_720P_30FPS_CB
#   @trace #BRCM_SWREQ_CAMERA_OV10640_CSI_CB_BYPASS_720P_30FPS
#
#   @limitations None
#   @file multimedia/camera/component/comp.mk
#   @brief Makefile for Camera Media Component
#   @version 0.1 Initial version
#   @}
#

BRCM_CONFIGURATION_ITEM := comp.mk
#BRCM_CONFIGURATION_ITEM += doc/camera_req.c
#BRCM_CONFIGURATION_ITEM += inc/camera.h
#BRCM_CONFIGURATION_ITEM += os/erika/camera_internal.h
#BRCM_CONFIGURATION_ITEM += os/erika/camera_comp.c
#BRCM_CONFIGURATION_ITEM += tests/qualification/camera/camera_qt.c

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

BRCM_COMP_NAME := camera
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := doc

