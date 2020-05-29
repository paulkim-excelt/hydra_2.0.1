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
import utils
import image
import graph
import logging
import numbers
import defn

class VXDistribution():
    def __init__(self,context,numBins,offset,range):
        self.width = numBins
        self.height = 1
        self.type = brcm_openvx.VX_DF_IMAGE_U32
        self.numBins = numBins
        self.offset = offset
        self.range = range
        self.image = brcm_openvx.vxCreateImage(context, self.width, self.height, self.type)
        brcm_openvx.vxSetName(self.image,'hist_dist')
    def setName(self,name):
        pass

class Histogram(graph.VXNode):
    def __init__(self,input,distribution,graph_hist):
        logger = logging.getLogger(__name__)
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (distribution.type ==brcm_openvx.VX_DF_IMAGE_U32)
        ret = ret0 & ret1;
        if ret is not True:
            logger.error('Histogram: input/output type constraints are not met')
            raise TypeError
        if ((input.width % 8 != 0) or (input.height < 2)):
            logger.error('Histogram: input resolution constraints are not met inW = %s inH = %s'%(input.width,input.height))
            raise ValueError
        graph.VXNode.__init__(self)
        self.input = input
        self.temp = brcm_openvx.vxCreateVirtualImage(graph_hist, 256, 8, brcm_openvx.VX_DF_IMAGE_U64)
        self.out = self.temp['VX_DATA']
        self.distribution = distribution
        self.setNumJobs(1)
    def createMailBoxLocal(self,totalCoreCount):
        msg = []
        (inStore, inW, inH, inS) = self.input.getDetails()
        sliceHeightRound = int(inH)/int(totalCoreCount)

        (outStore, outW, outH, outS) = self.out.getDetails()
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(inH) % int(totalCoreCount))

            cmd = graph.VXCommand()
            currInOffset = coreNum*int(sliceHeightRound*inS)
            cmd.appendStorage(inStore, currInOffset, 0)
            currOutOffset = (coreNum*outS)
            cmd.appendStorage(outStore, currOutOffset, 28)

            cmd.appendMsgType(16)
            cmd.appendMbox32le(currInOffset) #src1
            cmd.appendMbox32le(0) #src2 ..NA
            cmd.appendMbox32be(sliceHeight) #ysize
            cmd.appendMbox32be(inW) #xsize
            cmd.appendMbox32be(inS) #src1_step
            cmd.appendMbox32be(0) #src2_step
            #dst_step is ignored to be aligned with vendor delivery
            cmd.appendMbox32be(0) #dst_step
            cmd.appendMbox32le(currOutOffset) #dst
            cmd.appendMbox32be(0) #pad1.. dont care
            cmd.appendMbox32be(0) #type
            cmd.appendMbox32be(0) #scalarVal
            cmd.appendMbox32be(1) #scalarTypeflag
            cmd.appendMbox32be(0) #pad[0].. dont care
            cmd.appendMbox32be(0) #pad[1].. dont care
            msg = msg + cmd.getData()
        return msg
    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(totalCoreCount)
        return msg

def vxCreateDistribution_int(context,numBins,offset,range):
    logger = logging.getLogger('__name__')
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreateDistribution: Cannot verify context')
        raise AttributeError
    if ((isinstance(numBins,numbers.Number)==False) or
        (isinstance(range,numbers.Number)== False) or
        (isinstance(offset,numbers.Number)==False)):
        logger.error('vxCreateDistribution: bins/offset/range  not numbers')
        raise TypeError
    if numBins != 256:
        logger.error('vxCreateDistribution: numBins not in range')
        raise ValueError
    distribution = defn.create_dict(defn.VX_DISTRIBUTION,context)
    distribution['VX_DATA'] = VXDistribution(context,numBins,offset,range)
    context['VX_DISTRIBUTION'].append(distribution)
    return distribution

def vxReleaseDistribution_int(distribution):
    logger = logging.getLogger('__name__')
    ret = defn.verifyDistribution(distribution)
    if ret is not True:
        logger.error('vxCreateDistribution: Cannot verify dictionary')
    distribution['VX_DATA'] = None

def vxHistogramNode_int(graph, input, distribution):
    logger = logging.getLogger('__name__')
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyDistribution(distribution)
    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxHistogramNode: one or all parameters are wrong')
        raise AttributeError
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = Histogram(input['VX_DATA'],distribution['VX_DATA'],graph)
    graph['VX_DATA'].node.append(node)
    return node
