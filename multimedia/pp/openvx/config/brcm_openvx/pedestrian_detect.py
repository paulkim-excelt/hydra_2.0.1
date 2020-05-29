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
import scale
import numbers
import image
import struct
import math
import graph
import defn
import logging

class VXPedestrianDetect(graph.VXNode):
    def __init__(self, graph_PedDetect, image, rectangles, num_rect, scale, levels, quality):
        logger = logging.getLogger(__name__)
        ret0 = (image.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (rectangles.type == brcm_openvx.VX_TYPE_RECTANGLE)
        ret2 = (num_rect.type == (brcm_openvx.VX_TYPE_INT32 - brcm_openvx.VX_TYPE_START))
        ret = ret0 & ret1 & ret2
        if ret is not True:
            logger.error('VXPedestrianDetect:input/output type constraints are not met')
            raise TypeError
        if ((image.width % 8 != 0)):
            logger.error('VXPedestrianDetect: width should be multiple of 8 ')
            raise ValueError
        logger = logging.getLogger(__name__)
        ret0 = isinstance(scale, numbers.Number)
        ret1 = isinstance(levels, numbers.Number)
        ret2 = isinstance(quality, numbers.Number)
        ret = ret0 & ret1 & ret2;
        if ret is not True:
            logger.error('VXPedestrianDetect: scale/level/quality is not a number')
            raise TypeError
        ret0 = (scale > 1.0) and (scale < 2.0)
        ret1 = (levels < 64)
        ret2 = (quality >= 1.0) and (quality <= 3.0)
        ret = ret0 & ret1 & ret2;
        if ret is not True:
            logger.error('VXPedestrianDetect: scale/level/quality fails')
            raise ValueError
        graph.VXNode.__init__(self)
        self.image = image
        self.rect = rectangles
        self.num_rect = num_rect
        self.scale = scale
        self.levels = levels
        self.quality = quality
        (imStore, width, height, step) = self.image.getDetails()
        self.width = width
        self.height = height
        self.images = []
        self.scaleNodes = []
        self.storage = []
        self.setNumJobs(2*levels)
    def compute(self, graph_PedDetect):
        (imStore, width, height, step) = self.image.getDetails()
        self.storage.append(graph_PedDetect.allocateStorage(imStore.getSize()))
        self.storage.append(graph_PedDetect.allocateStorage(imStore.getSize()))
        self.storage = self.storage * int(self.levels/2)
        w = width
        h = height
        for i in range(0, self.levels):
            w = int(w/self.scale)
            h = int(h/self.scale)
            if (w < 48) or (h < 96):
                self.levels = i
                self.setNumJobs(2*self.levels)
                break
            tempImage = image.VXImage(graph_PedDetect, w, h, brcm_openvx.VX_DF_IMAGE_U8, self.storage[i])
            self.images.append(tempImage)
            if i==0:
                tempScaleNode = scale.VXScaleNode(self.image,self.images[i], brcm_openvx.VX_INTERPOLATION_BILINEAR)
            else:
                tempScaleNode = scale.VXScaleNode(self.images[i-1],self.images[i], brcm_openvx.VX_INTERPOLATION_BILINEAR)
            tempScaleNode.compute(graph_PedDetect)
            self.scaleNodes.append(tempScaleNode)
        histobufWidth = (width >> 3) - 1
        histobufHeight = (height >> 3) - 1
        mbx = width >> 4
        mby = height >> 4
        if ((histobufWidth & 1) == 0):
            mbx += 1
        if ((histobufHeight & 1) == 0):
            mby += 1
        self.histogram = image.VXImage(graph_PedDetect, 36*2*mbx, 2*mby, brcm_openvx.VX_DF_IMAGE_S16, None)
        self.intHog = image.VXImage(graph_PedDetect, 36*2*mbx, mby + 1, brcm_openvx.VX_DF_IMAGE_S32, None)
        self.results = image.VXImage(graph_PedDetect, 48*2, 8, brcm_openvx.VX_DF_IMAGE_S32, None)
        self.syncMatStorage = graph_PedDetect.allocateStorage(((1088/16) + 1 + 1 + 16) * 2 * 4)
        return True
    def getTableData(self):
        table = []
        for i in range(len(self.scaleNodes)):
            for each in self.scaleNodes[i].getTableData():
                table.append(each)
        return table
    def createMailBoxLocal(self, total_cores, img):
        msg = []
        (imStore, w, h, s) = img.getDetails()
        for core_num in range(total_cores):
            scale = max(1, min(int((self.width*1.0/w)*256), 65535))
            cmd = graph.VXCommand()
            cmd.appendStorage(self.histogram.getStorage(), 0, 0)
            cmd.appendStorage(imStore, 0, 4)
            cmd.appendStorage(self.results.getStorage(), core_num*46*8, 8)
            cmd.appendHogLut(24, 32)
            cmd.appendStorage(self.intHog.getStorage(), 0, 36)
            if core_num==0:
                cmd.appendHogSync(self.syncMatStorage, 48, w)
            else:
                cmd.appendStorage(self.syncMatStorage, 0, 48)
            cmd.appendHogRect(self.rect.getStorage(), self.results.getStorage(), core_num*46*8, scale, self.num_rect)
            cmd.appendMsgType(35)
            cmd.appendMbox32le(0) #dst/hist
            cmd.appendMbox32le(0) #src/img
            cmd.appendMbox32le(0) #result
            cmd.appendMbox32be((h << 16) | w) #w and h
            cmd.appendMbox32be(self.histogram.getStride())
            cmd.appendMbox32be(s)
            cmd.appendMbox32le(0) #SQRT Lut
            cmd.appendMbox32be((total_cores << 16) | core_num)
            cmd.appendMbox32le(0) #Ped SVM
            cmd.appendMbox32le(0) #int Hog
            cmd.appendMbox32be((96 << 16) | 48) #w and h
            cmd.appendMbox32be(int(self.quality * (1<<27)))
            cmd.appendMbox32le(0) #sync
            cmd.appendMbox32be(0xDBBEB000) #irho
            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,total_cores):
        msg = []
        for i in range(self.levels):
            msg = msg + self.scaleNodes[i].createMailBox(total_cores)
            msg = msg + self.createMailBoxLocal(total_cores, self.images[i])
        return msg

def vxPedestrianDetectNode_int(graph, image, rectangles, num_rect, scale, levels, quality):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(image)
    ret2 = defn.verifyArray(rectangles)
    ret3 = defn.verifyScalar(num_rect)
    ret = ret0 & ret1 & ret2 & ret3;
    if ret is not True:
        logger.error('vxPedestrianDetect: one or all parameters are wrong')
        raise AttributeError
    ped_detect = VXPedestrianDetect(graph['VX_DATA'], image['VX_DATA'], rectangles['VX_DATA'], num_rect['VX_DATA'], scale, levels, quality)
    ped_detect.compute(graph['VX_DATA'])
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = ped_detect
    graph['VX_DATA'].node.append(node)
    return node


