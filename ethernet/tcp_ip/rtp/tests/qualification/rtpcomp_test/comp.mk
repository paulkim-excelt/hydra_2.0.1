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
####################################################################################
#
####################################################################################
# File Name:  comp.mk
# Descritpion: This file defines the component name, dependencies and type
####################################################################################


BRCM_FW_NUM_COPIES = 1

EEOPT += ENABLE_RTP
BRCM_CFLAGS += -DENABLE_FMWK_EX_SRC_COMP=1
BRCM_CFLAGS += -DENABLE_FMWK_EX_SNK_COMP=1
BRCM_CFLAGS += -DENABLE_SHELL_AUTO_TEST_FUNC=1
BRCM_CFLAGS += -DENABLE_MAC1=1
ifeq ($(ENABLE_DDR), TRUE)
BRCM_CFLAGS += -DRTP_MAX_NUM_INPUT_PORTS=4UL
BRCM_CFLAGS += -DRTP_MAX_NUM_OUTPUT_PORTS=4UL
BRCM_APP_CFG_XML=fourports_application
else
BRCM_CFLAGS += -DRTP_MAX_NUM_INPUT_PORTS=2UL
BRCM_CFLAGS += -DRTP_MAX_NUM_OUTPUT_PORTS=2UL
BRCM_APP_CFG_XML=twoports_application
endif
BRCM_CFLAGS += -DRTP_ENABLE_RTCP_1733_SUPPORT=1
BRCM_CFLAGS += -DRTPCOMP_LOOPBACK_TEST=1

BRCM_INC += $(BRCM_SDK_ROOT)/applications/include
BRCM_INC += $(BRCM_SDK_ROOT)/applications/include/examples
BRCM_INC += $(BRCM_SDK_ROOT)/system/include

BRCM_SRC := $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/tests/qualification/rtpcomp_test/rtpcomp_test.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/tests/qualification/rtpcomp_test/ex_src_comp_core.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/tests/qualification/rtpcomp_test/ex_sink_comp_core.c

BRCM_COMP_NAME := rtpcomp_qt
BRCM_COMP_DEPENDS := inet rtp
BRCM_COMP_TYPE := app

