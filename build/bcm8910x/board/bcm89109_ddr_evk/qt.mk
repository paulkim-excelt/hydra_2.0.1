##############################################################################
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
################################################################################

#Below auto tests are enabled for bcm89105_ddr_evk:
#---------------------------------------------
ifeq ($(LIB_RVCT_CC),1)
BRCM_BOARD_ENABLED_APP += fmwk_qt
else
BRCM_BOARD_ENABLED_APP += avcd_qt vdec_qt lvdslib_qt display_qt mwb_qt openvx_qt
BRCM_BOARD_ENABLED_APP += stitchlib_qt venc_ip_qt

BRCM_BOARD_ENABLED_APP += utils_qt time_qt  ldclib_qt pplib_qt ppdma_qt avce_qt
BRCM_BOARD_ENABLED_APP += avblistenerlib_qt avbtalkerlib_qt crflib_qt avbcomp_qt rtpslib_qt tftp_qt rfs_qt
BRCM_BOARD_ENABLED_APP += rtpcomp_qt mpegtslib_qt rtpts_qt pinmux_qt avt_qt gpio_qt ethernet_qt
BRCM_BOARD_ENABLED_APP += can_qt ptm_qt dma_qt vtmon_qt flash_qt queue_qt etscomp_qt
BRCM_BOARD_ENABLED_APP += add_example isys_qt wdt_qt timer_qt inetudp_qt spi_qt ethxcvr_qt
BRCM_BOARD_ENABLED_APP += adc_qt sha_qt rsa_qt pwm_qt venc_qt fmwk_qt i2c_qt
endif
