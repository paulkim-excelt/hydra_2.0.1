#!/usr/bin/python2
# -*- coding: utf-8 -*-
#
# Copyright 2018 Broadcom Limited.  All rights reserved.
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

#  CONFIDENTIAL AND PROPRIETARY INFORMATION
#  Copyright 2004 - 2016 videantis GmbH
#  All Rights Reserved
#
# This document contains confidential and proprietary information of videantis
# GmbH and is protected by copyright, trade secret and other local, state,
# federal, and international laws. Its receipt or possession does not convey
# any rights to reproduce, transfer, disclose or publish its contents, or to
# manufacture, commercially or non-commercially use or sell anything it may
# describe or contain. Reproduction, disclosure or any use without specific
# written authorization of videantis GmbH or an individual license agreement
# with videantis GmbH is strictly forbidden.
#
####################################################################################

import struct
import brcm_openvx


#defines for message type
VENDOR_MSG_SMOOTH = 3
VENDOR_MSG_SOBEL = 5
VENDOR_MSG_ARITH = 10
VENDOR_MSG_RESIZE = 12
VENDOR_MSG_STAT = 15
VENDOR_MSG_CANNY = 19
VENDOR_MSG_INTEGRAL = 14
VENDOR_MSG_HAAR = 18
VENDOR_MSG_GAUSSIAN = 22
VENDOR_MSG_LPDOWN = 26

#defines for slice type
VENDOR_SLICE_FULL_FRAME = 0
VENDOR_SLICE_TOP_SLICE = 1
VENDOR_SLICE_MIDDLE_SLICE = 2
VENDOR_SLICE_BOTTOM_SLICE = 4

#defines for sobel
VENDOR_SOBEL_NORMAL = 0
VENDOR_SOBEL_GRADIENTS_AND_MAGNITUDE = 1
VENDOR_SOBEL_MIX_XY = 2
VENDOR_SOBEL_SCHARR_MIX_XY = 3
VENDOR_SOBEL_16bit = 4



def pack_16be(val):
    return struct.pack('>H',val)
def pack_16le(val):
    return struct.pack('H',val)
def pack_32be(val):
    return struct.pack('>I',val)
def pack_32le(val):
    return struct.pack('I',val)
def pack_u8(val):
    return struct.pack('B',val)
def pack_i8(val):
    return struct.pack('b',val)

def append_32be(msg,val):
    msg.append(pack_32be(val))
def append_32le(msg,val):
    msg.append(pack_32le(val))
def append_16be(msg,val):
    msg.append(pack_16be(val))
def append_16le(msg,val):
    msg.append(pack_16le(val))

def append_i8(msg,data,length):
    for i in range(length):
        msg.append(pack_i8(data[i]))
def append_u8(msg,data,length):
    for i in range(length):
        msg.append(pack_u8(data[i]))

def createMailBoxHeader(msg,images):

    append_32le(msg,(0x494D4147))

    for image in images:
        append_32le(msg,(image[0]))
        append_32le(msg,(image[1]))
        append_32le(msg,(image[2]))

    for i in range(8-len(images)):
        append_32le(msg,(0))
        append_32le(msg,(0))
        append_32le(msg,(0))
    append_32le(msg,(len(images)))
