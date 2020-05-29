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


##  @defgroup grp_isys ISYS
#   @ingroup grp_camera
#
#   @addtogroup grp_isys
#   @{
#   @section sec_camera_isys_overview Overview
#   @todo Add overview
#
#   @trace #BRCM_SWREQ_ISYS_CONFIGURATION
#   @trace #BRCM_SWREQ_ISYS
#   @trace #BRCM_SWREQ_ISYS_DP
#   @trace #BRCM_SWREQ_ISYS_TUNER_UPDATE
#   @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
#   @trace #BRCM_SWREQ_ISYS_PWL
#   @trace #BRCM_SWREQ_ISYS_SLOPE
#
#   @limitations None
#   @file multimedia/camera/isys/comp.mk
#   @brief Makefile for ISYS library
#   @version 0.1 Initial version
#   @}
#
#   @defgroup grp_unicam UNICAM
#   @ingroup grp_drivers
#
#   @addtogroup grp_unicam
#   @{
#   @section sec_unicam_overview Overview
#   @todo Add overview
#
#   @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
#   @trace #BRCM_SWREQ_UNICAM_INIT
#   @trace #BRCM_SWREQ_UNICAM
#   @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
#   @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER
#
#   @limitations None
#   @file multimedia/camera/isys/comp.mk
#   @brief Makefile for UNICAM driver
#   @version 0.1 Initial version
#   @}
#
#   @defgroup grp_isp ISP
#   @ingroup grp_drivers
#

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

ifeq ($(ENABLE_CAM), TRUE)

BRCM_CONFIGURATION_ITEM := comp.mk

BRCM_CFLAGS += -DISP_DRV_ENABLE_WA_644

#UNICAM Trace
BRCM_CONFIGURATION_ITEM += doc/unicam_req.c
BRCM_CONFIGURATION_ITEM += inc/osil/unicam_osil.h
BRCM_CONFIGURATION_ITEM += inc/unicam.h
BRCM_CONFIGURATION_ITEM += lib/unicam/unicam.c
BRCM_CONFIGURATION_ITEM += lib/unicam/unicam_hw.c
BRCM_CONFIGURATION_ITEM += os/erika/unicam_osil_erika.c

#ISP Trace
BRCM_CONFIGURATION_ITEM += doc/isp_req.c
BRCM_CONFIGURATION_ITEM += inc/isp_configs.h
BRCM_CONFIGURATION_ITEM += inc/isp_image_v2.h
BRCM_CONFIGURATION_ITEM += inc/isp_image.h
BRCM_CONFIGURATION_ITEM += inc/isp_stats.h
BRCM_CONFIGURATION_ITEM += inc/isp_params.h
BRCM_CONFIGURATION_ITEM += inc/isp.h
BRCM_CONFIGURATION_ITEM += inc/osil/isp_osil.h
BRCM_CONFIGURATION_ITEM += lib/isp/isp.c
BRCM_CONFIGURATION_ITEM += lib/isp/isp_hw_types.h
BRCM_CONFIGURATION_ITEM += lib/isp/isp_hw.c
BRCM_CONFIGURATION_ITEM += os/erika/isp_osil_erika.c

#ISYS Trace
BRCM_CONFIGURATION_ITEM += doc/isys_req.c
BRCM_CONFIGURATION_ITEM += inc/isys.h
BRCM_CONFIGURATION_ITEM += inc/isys_tuner.h
BRCM_CONFIGURATION_ITEM += inc/isys_types.h
BRCM_CONFIGURATION_ITEM += inc/cam_mode_image.h
BRCM_CONFIGURATION_ITEM += inc/osil/isys_osil.h
#BRCM_CONFIGURATION_ITEM += lib/isp_image_v2/isp_image_v2.c
BRCM_CONFIGURATION_ITEM += lib/fmwk/isys.c

#PWL Trace
BRCM_CONFIGURATION_ITEM += lib/pwl/pwl.h
BRCM_CONFIGURATION_ITEM += lib/pwl/pwl.c

#Slope Trace
BRCM_CONFIGURATION_ITEM += lib/slope/slope.h
BRCM_CONFIGURATION_ITEM += lib/slope/slope.c

BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/inc
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/inc/osil
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/fmwk
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/isp
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/unicam
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/pwl
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/slope
BRCM_INC += $(BRCM_SDK_ROOT)/common/include
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/inc

#Framework Sources
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/fmwk/isys.c

#ISP Sources
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/isp/isp.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/isp/isp_hw.c

#ISP Config Sources
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/isp_image_v2/isp_image_v2.c

#UNICAM Sources
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/unicam/unicam.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/unicam/unicam_hw.c

#PWL & Slope sources
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/pwl/pwl.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/camera/isys/lib/slope/slope.c

endif

BRCM_COMP_NAME := isys

BRCM_COMP_TYPE := lib
