#
# Copyright 2016-2018 Broadcom Limited.  All rights reserved.
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


##  @defgroup grp_pplib PP Driver
#   @ingroup grp_drivers
#
#   @addtogroup grp_pplib
#   @{
#   @section sec_pplib_overview Overview
#   @image html pplib_overview.jpg "PP Driver Overview"
#   Detailed Overview to be populated here.
#
#   @trace #BRCM_SWREQ_PP_INIT
#
#   @file multimedia/pp/pplib/comp.mk
#   @brief Makefile for PP Driver
#   @version 0.1 Initial version
#   @}
#

BRCM_CONFIGURATION_ITEM := comp.mk
BRCM_CONFIGURATION_ITEM += doc/pplib_req.c
BRCM_CONFIGURATION_ITEM += inc/pp.h
BRCM_CONFIGURATION_ITEM += inc/sp.h
BRCM_CONFIGURATION_ITEM += inc/osil/pp_osil.h
BRCM_CONFIGURATION_ITEM += lib/ppdriver.h
BRCM_CONFIGURATION_ITEM += lib/pp.c
BRCM_CONFIGURATION_ITEM += lib/ppdriver.c
BRCM_CONFIGURATION_ITEM += os/erika/pplib_osil.c

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

ifeq ($(chip),bcm89103)
BRCM_CFLAGS += -DPP_NUM_CORES=4
else ifeq ($(chip),bcm89105)
BRCM_CFLAGS += -DPP_NUM_CORES=8
else ifeq ($(chip),bcm89106)
BRCM_CFLAGS += -DPP_NUM_CORES=8
else ifeq ($(chip),bcm89107)
BRCM_CFLAGS += -DPP_NUM_CORES=8
else ifeq ($(chip),bcm89109)
BRCM_CFLAGS += -DPP_NUM_CORES=8
endif

ifeq ($(ENABLE_PP), TRUE)
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/inc
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/lib
BRCM_INC += $(BRCM_SDK_ROOT)/multimedia/pp/include/osil
BRCM_INC += $(BRCM_SDK_ROOT)/common/include
BRCM_INC += $(BRCM_SDK_ROOT)/system/arch/arm/cortex_mx/inc

BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/lib/pp.c
BRCM_SRC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/lib/ppdriver.c
endif

ifeq ($(ENABLE_DDR), TRUE)
BRCM_CFLAGS += -DPP_ENABLE_DRAM_ERRATA=1UL
endif
BRCM_COMP_NAME := pplib
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

