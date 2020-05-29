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
import image
import utils
import graph
import logging
import numbers
import defn

VENDOR_FILTER_MEDIAN = 0
VENDOR_FILTER_GAUSSIAN = 1
VENDOR_FILTER_BLUR = 2

VX_FILTER_TYPE= {
VENDOR_FILTER_MEDIAN:'MEDIAN_FILTER',
VENDOR_FILTER_GAUSSIAN:'GAUSSIAN_FILTER',
VENDOR_FILTER_BLUR:'BLUR_FILTER'
}

class VXSobel(graph.VXNode):
    def __init__(self,input, output_x, output_y, mask_x, mask_y, mode):
        logger = logging.getLogger(__name__)
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output_x.type ==brcm_openvx.VX_DF_IMAGE_S16)
        ret2 = (output_y.type ==brcm_openvx.VX_DF_IMAGE_S16)
        ret = ret0 & ret1 & ret2;
        if ret is not True:
            logger.error('VXSobel: input /output type constraints are not met')
            raise TypeError
        if ((input.width != output_x.width != output_y.width) or (input.height != output_x.height != output_y.height)):
            logger.error('VXSobel: input /output resolution are not same inW = %s inHt=%s outW=%s outH=%s'%(input.width, input.height, output.width, output.height))
            raise ValueError
        if ((input.width % 8 != 0) or (input.height < 4)):
            logger.error('VXSobel: input /output resolution constraints are not met inW = %s inH = %s'%(input.width, input.height))
            raise ValueError
        graph.VXNode.__init__(self)
        self.input = input
        self.jobs = []
        self.mode = mode
        if output_x is not None:
            self.jobs.append((output_x,list(mask_x)))
        if output_y is not None:
            self.jobs.append((output_y,list(mask_y)))
        self.setNumJobs(len(self.jobs))
    def createMailBoxLocal(self,totalCoreCount,job):
        output = job[0]
        mask = job[1]
        msg = []
        (inStore, inW, inH, inS) = self.input.getDetails()
        sliceHeightRound = int(inH)/int(totalCoreCount)

        (outStore, outW, outH, outS) = output.getDetails()
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(inH) % int(totalCoreCount))

            cmd = graph.VXCommand()
            currInOffset = (coreNum*int(sliceHeightRound*inS))
            cmd.appendStorage(inStore, currInOffset, 4)
            currOutOffset = (coreNum*int(sliceHeightRound*outS))
            cmd.appendStorage(outStore, currOutOffset, 0)
            cmd.appendMsgType(utils.VENDOR_MSG_SOBEL)
            cmd.appendMbox32le(currOutOffset) #dst
            cmd.appendMbox32le(currInOffset) #src
            cmd.appendMbox32be(sliceHeight) #ysize
            xsize = inW
            if self.mode == utils.VENDOR_SOBEL_16bit:
                xsize = xsize * 2
            cmd.appendMbox32be(xsize) #xsize
            cmd.appendMbox32be(outS) #dst_step
            cmd.appendMbox32be(inS) #src_step
            for i in range(8):
                cmd.appendMboxS8(mask[i]) #mask
            cmd.appendMbox32be(0) #pad1
            cmd.appendMbox8(0) #cpad[0]
            cmd.appendMbox8(0) #cpad[1]
            cmd.appendMbox8(0) #cpad[2]
            cmd.appendMboxS8(mask[8]) #mask_last
            cmd.appendMbox32be(self.mode) #mode
            if totalCoreCount > 1:
                if coreNum == 0:
                    sliceType = utils.VENDOR_SLICE_TOP_SLICE
                elif coreNum == totalCoreCount - 1:
                    sliceType = utils.VENDOR_SLICE_BOTTOM_SLICE
                else:
                    sliceType = utils.VENDOR_SLICE_MIDDLE_SLICE
            else:
                sliceType = utils.VENDOR_SLICE_FULL_FRAME
            cmd.appendMbox32be(sliceType) #slice_info
            cmd.appendMbox32be(0) #pad[0]
            cmd.appendMbox32be(0) #pad[1]
            msg = msg + cmd.getData()
        return msg
    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(totalCoreCount,self.jobs[i])
        return msg


def vxSobel3x3Node_int(graph, input, output_x, output_y):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output_x)
    ret3 = defn.verifyImage(output_y)

    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxSobel3x3Node: one or all parameters are wrong')
        raise AttributeError
    node = defn.create_dict(defn.VX_NODE,graph)
    mask_x = [-1,0,1,-2,0,2,-1,0,1,1]
    mask_y = [-1,-2,-1,0,0,0,1,2,1,1]
    node['VX_DATA'] = VXSobel(input['VX_DATA'], output_x['VX_DATA'], output_y['VX_DATA'], mask_x, mask_y, utils.VENDOR_SOBEL_NORMAL)
    graph['VX_DATA'].node.append(node)
    return node


class VXCanny(graph.VXNode):
    def __init__(self,input, threshold, gradient_size, norm_type, output):
        logger = logging.getLogger(__name__)
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret2 = (threshold.dataType ==brcm_openvx.VX_TYPE_UINT8)
        ret = ret0 & ret1 #& ret2
        if ret is not True:
            logger.error('VXCanny: input/output type constraints are not met')
            raise TypeError
        if ((input.width != output.width) or (input.height != output.height)):
            logger.error('VXCanny: input and output resolutions are not same inW = %s inH=%s outW=%s outH=%s'%(input.width, input.height, output.width, output.height))
            raise ValueError
        if ((input.width % 8 !=0) or (input.height < 4)):
            logger.error('VXCanny: input and output resolution constraints are not met inW = %s inH = %s'%(input.width, input.height))
            raise ValueError
        if (gradient_size != 3):
            gradient_size = 3
        logger.info('Only Gradient Size 3 is supported')
        logger.info('Only VX_Norm_L1 is supported')
        graph.VXNode.__init__(self)
        self.input = input
        self.threshold = threshold
        self.gradient_size = gradient_size
        self.norm_type = norm_type
        self.output = output
        self.setNumJobs(1)
    def createMailBoxLocal(self,totalCoreCount):
        msg = []
        (inStore, inW, inH, inS) = self.input.getDetails()
        sliceHeightRound = int(inH)/int(totalCoreCount)

        (outStore, outW, outH, outS) = self.output.getDetails()
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(inH) % int(totalCoreCount))

            cmd = graph.VXCommand()
            currInOffset =  (coreNum*int(sliceHeightRound*inS))
            cmd.appendStorage(inStore, currInOffset, 4)
            currOutOffset = (coreNum*int(sliceHeightRound*outS))
            cmd.appendStorage(outStore, currOutOffset, 0)
            cmd.appendMsgType(utils.VENDOR_MSG_CANNY)
            cmd.appendMbox32le(currOutOffset) #dst
            cmd.appendMbox32le(currInOffset) #src
            cmd.appendMbox32be(sliceHeight) #ysize
            cmd.appendMbox32be(inW) #xsize
            cmd.appendMbox32be(outS) #dst_step
            cmd.appendMbox32be(inS) #src_step
            cmd.appendMbox32be(self.threshold.lowerVal) #lowt
            cmd.appendMbox32be(self.threshold.upperVal) #hight
            cmd.appendMbox32be(0) #aperture
            cmd.appendMbox32be(0) #flags
            if totalCoreCount > 1:
                if coreNum == 0:
                    sliceType = utils.VENDOR_SLICE_TOP_SLICE
                elif coreNum == totalCoreCount - 1:
                    sliceType = utils.VENDOR_SLICE_BOTTOM_SLICE
                else:
                    sliceType = utils.VENDOR_SLICE_MIDDLE_SLICE
            else:
                sliceType = utils.VENDOR_SLICE_FULL_FRAME
            cmd.appendMbox32be(sliceType) #slice_info
            cmd.appendMbox32be(0) #pad[0]
            cmd.appendMbox32be(0) #pad[1]
            cmd.appendMbox32be(0) #pad[2]
            msg = msg + cmd.getData()
        return msg
    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(totalCoreCount)
        return msg


def vxCannyEdgeDetectorNode_int(graph, input, hyst, gradient_size, norm_type, output):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyThreshold(hyst)
    ret3 = defn.verifyImage(output)
    ret4 = isinstance(gradient_size,numbers.Number)
    ret = ret0 & ret1 & ret2 & ret3 &  ret4
    if ret is not True:
        logger.error('vxCannyEdgeDetectorNode: one or all parameters are wrong')
        raise AttributeError
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = VXCanny(input['VX_DATA'], hyst['VX_DATA'], gradient_size, norm_type, output['VX_DATA'])
    graph['VX_DATA'].node.append(node)
    return node


class VXSmooth(graph.VXNode):
    def __init__(self,input, output, mode,bitMode):
        logger = logging.getLogger(__name__)
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret = ret0 & ret1
        if ret is not True:
            ret2 = (input.type ==brcm_openvx.VX_DF_IMAGE_S16)
            ret3 = (output.type ==brcm_openvx.VX_DF_IMAGE_S16)
            ret = ret2 & ret3
            if ret is not True:
                logger.error('VXSmooth: %s: input and output type constraints are not met'%(VX_FILTER_TYPE[mode]))
                raise TypeError
        if (mode == VENDOR_FILTER_BLUR) and (bitMode is 1):
            logger.error('Blur is not possible with bitmode 1')
            raise TypeError
        if ((input.width != output.width) or (input.height != output.height)):
            logger.error('VXSmooth:%s: input and output resolutions are not same inW = %s inH=%s outW = %s outH = %s'%(VX_FILTER_TYPE[mode], input.width, input.height, output.width, output.height))
            raise ValueError
        if ((input.height < 4) or (input.width % 8 != 0)):
            logger.error('VXSmooth: %s: input cannot be so small inW = %s inH = %s'%(VX_FILTER_TYPE[mode], input.width, input.height))
            raise ValueError
        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.setNumJobs(1)
        self.mask = [16,32,16,32,64,32,16,32]
        self.maskLast = 16
        self.bitMode = bitMode
        self.smoothType = 2
        if mode == VENDOR_FILTER_MEDIAN:
            if bitMode == 0:
                self.smoothType = 1
            else:
                self.smoothType = 4
        if mode == VENDOR_FILTER_GAUSSIAN:
            if bitMode == 0:
                self.smoothType = 0
            else:
                self.smoothType = 3

    def createMailBoxLocal(self,totalCoreCount):
        msg = []

        (inName, inW, inH, inS) = self.input.getDetails()
        sliceHeightRound = int(inH)/int(totalCoreCount)

        (outName, outW, outH, outS) = self.output.getDetails()
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(inH) % int(totalCoreCount))

            cmd = graph.VXCommand()
            currInOffset =  (coreNum*int(sliceHeightRound*inS))
            cmd.appendStorage(inName, currInOffset, 4)
            currOutOffset =  (coreNum*int(sliceHeightRound*outS))
            cmd.appendStorage(outName, currOutOffset, 0)

            cmd.appendMsgType(utils.VENDOR_MSG_SMOOTH)
            cmd.appendMbox32le(currOutOffset)                   #dst
            cmd.appendMbox32le(currInOffset)                    #src
            cmd.appendMbox32be(sliceHeight)                     #ysize
            cmd.appendMbox32be(inW * (self.bitMode + 1))        #inW
            cmd.appendMbox32be(outS)                            #dst_step
            cmd.appendMbox32be(inS)                             #src_step
            for mask in self.mask:
                cmd.appendMbox8(mask)                           #mask
            cmd.appendMbox32be(0)                               #pad1
            cmd.appendMbox8(0)                                  #cpad[0]
            cmd.appendMbox8(0)                                  #cpad[1]
            cmd.appendMbox8(0)                                  #cpad[2]
            cmd.appendMbox8(self.maskLast)                      #mask_last
            cmd.appendMbox32be(self.smoothType)                 #mode
            if totalCoreCount > 1:
                if coreNum == 0:
                    sliceType = utils.VENDOR_SLICE_TOP_SLICE
                elif coreNum == totalCoreCount - 1:
                    sliceType = utils.VENDOR_SLICE_BOTTOM_SLICE
                else:
                    sliceType = utils.VENDOR_SLICE_MIDDLE_SLICE
            else:
                sliceType = utils.VENDOR_SLICE_FULL_FRAME
            cmd.appendMbox32be(sliceType)                       #slice_info
            cmd.appendMbox32be(0)                               #pad[0]
            cmd.appendMbox32be(0)                               #pad[1]
            msg = msg + cmd.getData()
        return msg
    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(totalCoreCount)
        return msg

def createFilterNode(graph, input, output, type):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = brcm_openvx.defn.verifyGraph(graph)
    ret1 = brcm_openvx.defn.verifyImage(input)
    ret2 = brcm_openvx.defn.verifyImage(output)

    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('%s: one or all pararmeters are wrong'%(VX_FILTER_TYPE[type]))
        raise AttributeError

    if input['VX_DATA'].type ==brcm_openvx.VX_DF_IMAGE_S16:
        bitMode = 1
    else:
        bitMode = 0

    if ((type != VENDOR_FILTER_MEDIAN)
        and  (type != VENDOR_FILTER_GAUSSIAN)
        and (type != VENDOR_FILTER_BLUR)):
        logger.error('VXSmooth.filter type fails for %s'%(VX_FILTER_TYPE[type]))
        raise ValueError
    node = brcm_openvx.defn.create_dict(brcm_openvx.defn.VX_NODE,graph)
    node['VX_DATA'] = VXSmooth(input['VX_DATA'], output['VX_DATA'], type,bitMode)
    graph['VX_DATA'].node.append(node)
    return node


def vxMedian3x3Node_int(graph, input, output):
    return createFilterNode(graph, input, output,VENDOR_FILTER_MEDIAN)

def vxGaussian3x3Node_int(graph, input, output):
    return createFilterNode(graph, input, output,VENDOR_FILTER_GAUSSIAN)

def vxBlur3x3ExtNode_int(graph, input, output):
    return createFilterNode(graph, input, output,VENDOR_FILTER_BLUR)


class VXGaussian5x5(graph.VXNode):
    @staticmethod
    def validate(input, output):
        logger = logging.getLogger(__name__)
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret = ret0 & ret1
        if ret is not True:
            logger.error('VXGaussian5x5: input and output type constraints are not met')
            raise TypeError

    def __init__(self,input, output):
        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.setNumJobs(1)
        self.mask = [1,4,6,4,1, 4,16,24,16,4, 6,24,36,24,6, 4,16,24,16,4, 1,4,6,4]
        self.maskLast = 1

    def createMailBoxLocal(self,totalCoreCount):
        msg = []
        (inName, inW, inH, inS) = self.input.getDetails()
        sliceHeightRound = int(inH)/int(totalCoreCount)
        (outName, outW, outH, outS) = self.output.getDetails()
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(inH) % int(totalCoreCount))
            cmd = graph.VXCommand()
            currInOffset =  (coreNum*int(sliceHeightRound*inS))
            cmd.appendStorage(inName, currInOffset, 4)
            currOutOffset =  (coreNum*int(sliceHeightRound*outS))
            cmd.appendStorage(outName, currOutOffset, 0)

            cmd.appendMsgType(utils.VENDOR_MSG_GAUSSIAN)
            cmd.appendMbox32le(currOutOffset)                   #dst
            cmd.appendMbox32le(currInOffset)                    #src
            cmd.appendMbox32be(sliceHeight)                     #ysize
            cmd.appendMbox32be(inW)                             #inW
            cmd.appendMbox32be(outS)                            #dst_step
            cmd.appendMbox32be(inS)                             #src_step
            for mask in self.mask:
                cmd.appendMbox8(mask)                           #mask
            if totalCoreCount > 1:
                if coreNum == 0:
                    sliceType = utils.VENDOR_SLICE_TOP_SLICE
                elif coreNum == totalCoreCount - 1:
                    sliceType = utils.VENDOR_SLICE_BOTTOM_SLICE
                else:
                    sliceType = utils.VENDOR_SLICE_MIDDLE_SLICE
            else:
                sliceType = utils.VENDOR_SLICE_FULL_FRAME
            cmd.appendMbox32be(sliceType)                       #slice_info
            cmd.appendMbox8(0)                                  #cpad[0]
            cmd.appendMbox8(0)                                  #cpad[1]
            cmd.appendMbox8(0)                                  #cpad[2]
            cmd.appendMbox8(self.maskLast)                      #mask_last
            msg = msg + cmd.getData()
        return msg
    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(totalCoreCount)
        return msg

def vxGaussian5x5ExtNode_int(graph, input, output):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)

    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxGaussian5x5ExtNode: one or all parameters have gone wrong')
        raise AttributeError

    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = VXGaussian5x5(input['VX_DATA'], output['VX_DATA'])
    graph['VX_DATA'].node.append(node)
    return node

