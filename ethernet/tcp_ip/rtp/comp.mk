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

##  @defgroup grp_rtplib RTP-RTCP Library
#   @ingroup grp_comms_sw
#
#   @addtogroup grp_rtplib
#   @{
#   @image html rtp_library_overview.jpg "Component Overview"
#
#   The RTP library comprises interface for the RTP packetizer and
#   parser functionality. It also has interfaces for RTCP sender report
#   creation and parsing. Support for audio is currently not present.
#   Creation and receive of multiple RTP/RTCP streams is supported by
#   accepting per-stream configuration.
#
#   H264 video payload packetization is supported using the below mentioned
#   payload formats specified by @ref sec_ref_rfc_6184.
#   - Fragmentation Units, Type A: This payload type allows fragmenting a
#    NAL unit into several RTP packets. Only slice based
#    fragmentation is supported.
#
#   Support for compound RTCP Sender Report with SDES and 1733 packets
#   are present. Receiver report of RTCP is not supported it is not
#   mandated by 1733 specification. For more details on the payload types,
#   please refer @ref sec_ref_rfc_6184.
#
#   @section sec_comp_seq Sequence Diagrams
#   @image html rtp_lib_sequence_diagram.jpg "Sequence diagram"
#
#   @limitations
#   1733 support for clock synchronization and presentation is provided
#   along with RTP media streaming. This section lists down the limitations
#   of 1733 RTCP.
#   -# RTCP Sender Report is a compound packet with the Canonical End-Point
#   Identifier for SDES and 1733
#   -# 1733 Sender Report is supported for synchronization of H264 video
#   payload RTP transmission as per RFC6184
#   -# NTP-RTP timestamp relation in default RTCP Sender Report is not updated
#   -# The gmTimeBaseIndicator and gmIdentity fields of 1733 is not updated
#   -# RTCP Receiver Reports are not supported as its not part of 1733
#   -# PTS in 1733 packets are computed based on a static presentation
#   offset of 100ms from capture to display
#   -# The timestamps derived from 1733 Sender Report are currently not
#   used by the display pipeline for synchronization
#   -# No Audio support
#
#   @}

BRCM_SRC :=
BRCM_CFLAGS += -DRTP_ENABLE_RTCP_1733_SUPPORT=1
BRCM_INC :=

BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/inet/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/include
BRCM_INC += $(BRCM_SDK_ROOT)/applications/include
BRCM_INC += $(BRCM_SDK_ROOT)/applications/include/os/erika

BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/lib/rtp_rtcp.c

BRCM_COMP_NAME := rtp
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

