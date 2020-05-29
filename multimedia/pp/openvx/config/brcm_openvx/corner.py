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

import brcm_openvx
import numbers
import image
import struct
import math
from graph import *
import arith
import logging
import numbers


class VXHarrisCorners(VXNode):
    def __init__(self,graph, input, strength_thresh, sensitivity, points, min_distance):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (points.type ==brcm_openvx.VX_TYPE_KEYPOINT)
        ret = ret0 & ret1;
        if ret is not True:
            logger.error('vxHarrisCornersNode: input/output type constraints are not met')
            raise TypeError
        if ((input.width % 8 != 0) or (input.height < 4)):
            logger.error('vxHarrisCornersNode: input resolution constraints are not met inW=%s inHt=%s'%(input.width,input.height))
            raise ValueError
        ret0 = isinstance(sensitivity, numbers.Number)
        ret1 = isinstance(min_distance, numbers.Number)
        ret2 = isinstance(strength_thresh, numbers.Number)
        ret = ret0 & ret1 & ret2;
        if ret is not True:
            logger.error('vxHarrisCornersNode:sensitivity/minDist/strength fails')
            raise TypeError
        ret0 = (min_distance < 2)
        ret1 = (sensitivity < 5) and (sensitivity > 1)
        ret2 = (strength_thresh >= 1000) and (strength_thresh <= 10000)
        ret = ret0 & ret1 & ret2;
        if ret is not True:
            logger.error('vxHarrisCornersNode:sensitivity/minDist/strength fails')
            raise ValueError
        self.input = input
        self.quality = strength_thresh
        self.min_distance = min_distance
        self.sensitivity = sensitivity
        self.points = points
        self.corners = None
        self.setNumJobs(1)
        mbx = input.width >> 4
        mby = input.height >> 4
        self.corners = image.VXImage(graph, mbx - 1, mby - 1 , brcm_openvx.VX_DF_IMAGE_S32C2, None)

    def createMailBox(self,total_cores):
        msg = []
        (inputStorage, inputW, inputH, inputS) = self.input.getDetails()
        (outputStorage, outputW, outputH, outputS) = self.corners.getDetails()
        slice_height_round = int(math.ceil(float((inputH-16)/16)/float(total_cores)) * 16)
        for core_num in range(total_cores):
            slice_height = slice_height_round
            if core_num == total_cores - 1:
                slice_height = int((((inputH-16)/16) - ((total_cores - 1)*slice_height_round/16))*16)
            cmd = VXCommand()
            offset = int(((core_num*slice_height_round)+8)*inputS)
            cmd.appendStorage(inputStorage, offset, 4)
            offset = (core_num*int((slice_height_round/16)*outputS))
            cmd.appendStorage(outputStorage, offset, 0)
            cmd.appendStorage(outputStorage, 0, 8)
            pointsOffset = (core_num*int((slice_height_round/16)* ((inputW-16)/16)) * 28)
            cmd.appendHarrisCorner(self.points.getStorage(), pointsOffset, outputStorage, offset, outputS/8, slice_height/16, outputW, core_num*slice_height_round)
            cmd.appendMsgType(38)
            cmd.appendMbox32le(0) #dst
            cmd.appendMbox32le(0) #dst
            cmd.appendMbox32le(0) #result
            cmd.appendMbox32be((long(slice_height)<<16)|inputW) #ysize_xsize
            cmd.appendMbox32be(outputS)
            cmd.appendMbox32be(inputS)
            cmd.appendMbox32be(self.sensitivity)  #harris_k
            cmd.appendMbox32be(self.quality)
            for i in range(6):
                cmd.appendMbox32be(0)#pad
            msg = msg + cmd.getData()
        return msg


def vxHarrisCornersNode_int(graph, input, strength_thresh, min_distance, sensitivity, gradient_size, block_size, corners, num_corners):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyArray(corners)
    ret = ret0 & ret1 & ret2;
    if ret is not True:
        logger.error('vxHarrisCornersNode: one or all parameters are wrong')
        raise AttributeError
    corner_detect = VXHarrisCorners(graph['VX_DATA'],input['VX_DATA'], strength_thresh, sensitivity, corners['VX_DATA'],min_distance)
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = corner_detect
    graph['VX_DATA'].node.append(node)
    return node

