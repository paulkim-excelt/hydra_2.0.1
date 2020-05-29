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


##  @defgroup grp_csn Camera Sensor
#   @ingroup grp_camera
#
##  @defgroup grp_csn_module Camera Sensor Module
#   @ingroup grp_csn
#
#   @addtogroup grp_csn
#   @{
#   @section sec_csn_overview Overview
#   @todo Add overview
#
#   @trace #BRCM_SWREQ_CSN_CONFIGURATION
#   @trace #BRCM_SWREQ_CSN
#   @trace #BRCM_SWREQ_CSN_PARAM_UPDATE
#   @trace #BRCM_SWREQ_CSN_PARSE_ED
#   @trace #BRCM_SWREQ_CSN_MODULE
#
#   @limitations None
#   @file multimedia/camera/sensor/comp.mk
#   @brief Makefile for Camera Sensor
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

BRCM_CONFIGURATION_ITEM := comp.mk
BRCM_CONFIGURATION_ITEM += doc/camera_sensor_req.c
BRCM_CONFIGURATION_ITEM += inc/camera_sensor_osil.h
BRCM_CONFIGURATION_ITEM += inc/camera_sensor_board.h
#BRCM_CONFIGURATION_ITEM += inc/camera_sensor_image.h
#BRCM_CONFIGURATION_ITEM += inc/camera_sensor.h
#BRCM_CONFIGURATION_ITEM += inc/camera_sensor_module.h
BRCM_CONFIGURATION_ITEM += os/board/bcm8910x/camera_sensor_board.c
BRCM_CONFIGURATION_ITEM += os/erika/camera_sensor_osil_erika.c
#BRCM_CONFIGURATION_ITEM += os/common/camera_sensor.c

BRCM_CFLAGS :=
BRCM_INC :=

BRCM_COMP_NAME := csn
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := doc

