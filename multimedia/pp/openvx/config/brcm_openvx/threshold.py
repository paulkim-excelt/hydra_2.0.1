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
import defn
import numbers

class Threshold():
    def __init__(self, thresholdType, dataType):
        logger = logging.getLogger(__name__)
        if ((thresholdType !=  brcm_openvx.VX_THRESHOLD_TYPE_BINARY) and
            (thresholdType !=  brcm_openvx.VX_THRESHOLD_TYPE_RANGE)):
            logger.error('vxCreateThreshold: thresholdType is not proper')
            raise ValueError
        self.thresholdType = thresholdType - brcm_openvx.VX_THRESHOLD_TYPE_START

        if ((dataType < brcm_openvx.VX_TYPE_CHAR) or (dataType > brcm_openvx.VX_TYPE_SCALAR_MAX)):
            logger.error('vxCreateThreshold: dataType is not proper')
            raise TypeError
        self.threshVal =0
        self.lowerVal = 0
        self.upperVal = 0
        self.trueVal = 0
        self.falseVal = 0
        self.dataType = dataType
    def setName(self,name):
        pass

def vxCreateThreshold_int(context, thresholdType, dataType):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyContext(context)
    ret1 = isinstance(thresholdType, numbers.Number)
    ret2 = isinstance(dataType, numbers.Number)
    if ((ret0 != True) or (ret1 != True) or (ret2 != True)):
        logger.error('vxCreateThreshold: one or all parameters are wrong')
        raise Exception
    thresh = defn.create_dict(defn.VX_THRESHOLD,context)
    thresh['VX_DATA'] = Threshold(thresholdType, dataType)
    context['VX_THRESHOLD'].append(thresh)
    return thresh

def vxSetThresholdAttribute_int(thresh, attribute, content, size):
    logger = logging.getLogger(__name__)
    ret = brcm_openvx.VX_FAILURE
    if (defn.verifyThreshold(thresh) is not True):
        logger.error('vxSetThresholdAttribute:threshold cannot be verified')
        raise ValueError
    try:
        if (isinstance(content[0],numbers.Number) is False):
            logger.error('vxSetThresholdAttribute:content doesnt have valid number')
            raise ValueError
    except:
        logger.error('vxSetThresholdAttribute:int object has no attribute _getitem_')
        raise TypeError
    logger.info('Size is not used currently,always considered 1')
    if (attribute == brcm_openvx.VX_THRESHOLD_THRESHOLD_VALUE ):
        thresh['VX_DATA'].threshVal = content[0]
        ret = brcm_openvx.VX_SUCCESS
    elif (attribute == brcm_openvx.VX_THRESHOLD_THRESHOLD_UPPER ):
        thresh['VX_DATA'].upperVal = content[0]
        ret = brcm_openvx.VX_SUCCESS
    elif (attribute == brcm_openvx.VX_THRESHOLD_THRESHOLD_LOWER ):
        thresh['VX_DATA'].lowerVal = content[0]
        ret = brcm_openvx.VX_SUCCESS
    elif (attribute == brcm_openvx.VX_THRESHOLD_TRUE_VALUE ):
        thresh['VX_DATA'].trueVal = content[0]
        ret = brcm_openvx.VX_SUCCESS
    elif (attribute == brcm_openvx.VX_THRESHOLD_FALSE_VALUE ):
        thresh['VX_DATA'].falseVal = content[0]
        ret = brcm_openvx.VX_SUCCESS
    elif (attribute == brcm_openvx.VX_THRESHOLD_TYPE):
        thresh['VX_DATA'].thresholdType = content[0]
    if (ret is brcm_openvx.VX_FAILURE):
        logger.error('vxSetThresholdAttribute: this is not proper attribute')
        raise ValueError
    return ret

def vxReleaseThreshold_int(threshold):
    logger = logging.getLogger(__name__)
    if (defn.verifyThreshold(threshold) is not True):
        logger.error('vxReleaseThreshold: Threshold is not proper dictionary')
        raise Exception
    threshold['VX_DATA'] = None


class VXThresholdNode(graph.VXNode):
    def __init__(self, input, output, thresh):
        logger = logging.getLogger(__name__)
        ret2 = True
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret2 = (thresh.dataType ==brcm_openvx.VX_TYPE_UINT8)
        ret = ret0 & ret1 & ret2
        if ret is False:
            ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_S16)
            ret1 = (output.type ==brcm_openvx.VX_DF_IMAGE_S16)
            ret2 = (thresh.dataType ==brcm_openvx.VX_TYPE_INT16)
            ret = ret0 & ret1 & ret2
            if ret is False:
                ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_S32)
                ret1 = (output.type ==brcm_openvx.VX_DF_IMAGE_S32)
                ret2 = (thresh.dataType ==brcm_openvx.VX_TYPE_INT32)
                ret = ret0 & ret1 & ret2
                if ret is False:
                    logger.error('VXThresholdNode: input/output type constraints are not met')
                    raise TypeError
        if ((input.width != output.width) or (input.height != output.height)):
            logger.error('VXThresholdNode: input/output resolution are not same inW = %s inH = %s outW = %s outH = %s'%(input.width,input.height,output.width,output.height))
            raise ValueError
        if ((input.width % 8 != 0) or (input.height < 2)):
            logger.error('VXThresholdNode: input/output resolution constraints are not met inW = %s inH = %s'%(input.width, input.height))
            raise ValueError
        logger.info('VXThresholdNode: only VX_THRESHOLD_TYPE_BINARY is supported')

        graph.VXNode.__init__(self)
        if (input.type == brcm_openvx.VX_DF_IMAGE_U8):
            self.bitMode = 0
        elif (input.type == brcm_openvx.VX_DF_IMAGE_S16):
            self.bitMode = 8
        else:
            self.bitMode = 16
        self.input = input
        self.output = output
        self.thresh = thresh
        self.setNumJobs(1)
        if self.bitMode == 0:
            self.max = (1<<8) -1
        elif self.bitMode == 8:
            self.max = (1<<16) -1
        else:
            self.max = 0xffffffff

    def createMailBoxLocal(self, totalCoreCount):
        msg = []
        (outStore, outW, outH, outS) = self.output.getDetails()
        (inStore, inW, inH, inS) = self.input.getDetails()
        sliceHeightRound = int(outH)/int(totalCoreCount)
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(outH) % int(totalCoreCount))
            outputImageOffset = (coreNum*int(sliceHeightRound*outS))
            inputImageOffset  = (coreNum*int(sliceHeightRound*inS))
            rc = self.thresh.threshVal
            if rc < 0:
                rc = 0
            if rc > self.max:
                rc = self.max;
            self.threshold_val = rc
            rc = self.thresh.trueVal
            if rc < 0:
                rc = 0
            if rc > self.max:
                rc = self.max
            self.maxVal = rc

            cmd = graph.VXCommand()
            cmd.appendStorage(outStore, outputImageOffset, 0)
            cmd.appendStorage(inStore, inputImageOffset, 4)

            cmd.appendMsgType(0)#msgIdcvThreshold
            cmd.appendMbox32le(0) #dst
            cmd.appendMbox32le(0) #input
            cmd.appendMbox32be(sliceHeight)
            cmd.appendMbox32be(outW)
            cmd.appendMbox32be(outS)
            cmd.appendMbox32be(inS)
            cmd.appendMbox32be(self.threshold_val)
            cmd.appendMbox32be(self.thresh.thresholdType)
            cmd.appendMbox32be(self.maxVal)
            cmd.appendMbox32be(self.bitMode)#0 = 8bit mode, 1 = 16bit mode
            cmd.appendMbox32be(0)
            cmd.appendMbox32be(0)
            cmd.appendMbox32be(0)
            cmd.appendMbox32be(0)
            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,numCores):
        return self.createMailBoxLocal(numCores)




def vxThresholdNode_int(graph, input, thresh, output):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyThreshold(thresh)
    ret3 = defn.verifyImage(output)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is False:
        logger.warning('VXThresholdNode: one or all parameters are wrong')
        raise AttributeError
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = VXThresholdNode(input['VX_DATA'],output['VX_DATA'],thresh['VX_DATA'])
    graph['VX_DATA'].node.append(node)
    return node


