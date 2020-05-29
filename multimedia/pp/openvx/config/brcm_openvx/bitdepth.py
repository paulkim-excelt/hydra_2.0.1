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


BTDPTHCONVERSION_SUPPORTED = 0
BTDPTHCONVERSION_NOT_SUPPORTED = 1

class VXConvertDepthNode(graph.VXNode):
    def __init__(self, input, output):
        logger = logging.getLogger(__name__)
        if ((input.width != output.width) or (input.height != output.height)):
            logger.error('VXConvertDepthNode: input and output resolution are not same inW = %s inHt = %s outW=%s outHt = %s'%(input.width, input.height, output.width, output.height))
            raise ValueError
        if ((input.width %8 != 0) or (input.height < 2 )):
            logger.error('VXConvertDepthNode: resolution constraints are not met inW = %s inHt = %s'%(input.width,input.height))
            raise ValueError
        if (((input.type == brcm_openvx.VX_DF_IMAGE_U8)  and (output.type == brcm_openvx.VX_DF_IMAGE_S16))
            or ((output.type == brcm_openvx.VX_DF_IMAGE_U8) and (input.type == brcm_openvx.VX_DF_IMAGE_S16))):
            logger.info('vxConvertDepthNode: Shift factor is not supported')
            logger.info('vxConvertDepthNode: VX_CONVERT_POLICY_SATURATE overflow policy is supported')
        else:
            logger.error('vxConvertDepthNode:%s to %s This conversion is not supported'%(input.type, output.type))
            raise TypeError

        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.setNumJobs(1)

    def createMailBoxLocal(self,totalCoreCount):
        msg = []
        (inStore, inW, inH, inS) = self.input.getDetails()
        (outStore, outW, outH, outS) = self.output.getDetails()
        sliceHeight = int(inH)/int(totalCoreCount)
        if ((self.input.type == brcm_openvx.VX_DF_IMAGE_U8)  and (self.output.type == brcm_openvx.VX_DF_IMAGE_S16)):
            type = 1
        else:
            type = 0
            outW = outW * 2

        for coreNum in range(totalCoreCount):
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(inH) % int(totalCoreCount))

            outputImageOffset = (coreNum*int(sliceHeight*outS))
            inputImageOffset  = (coreNum*int(sliceHeight*inS))
            if totalCoreCount==1:
                slice_info = utils.VENDOR_SLICE_FULL_FRAME

            elif (totalCoreCount > 1):
                if coreNum==0 :
                    slice_info = utils.VENDOR_SLICE_TOP_SLICE

                elif (coreNum < totalCoreCount - 1):
                    slice_info = utils.VENDOR_SLICE_MIDDLE_SLICE

                else:
                    slice_info = utils.VENDOR_SLICE_BOTTOM_SLICE
            cmd = graph.VXCommand()
            cmd.appendStorage(outStore, outputImageOffset, 0)
            cmd.appendStorage(inStore, inputImageOffset, 4)
            cmd.appendMsgType(21)                                       #msgIdcvConvertTo
            cmd.appendMbox32le(outputImageOffset)                       #dst
            cmd.appendMbox32le(inputImageOffset)                        #src
            cmd.appendMbox32be(sliceHeight)
            cmd.appendMbox32be(outW)
            cmd.appendMbox32be(outS)
            cmd.appendMbox32be(inS)
            cmd.appendMbox32be(type)
            cmd.appendMbox32be(slice_info)
            cmd.appendMbox32be(0)                                       #pad[0]dont care
            cmd.appendMbox32be(0)                                       #pad[1]dont care
            cmd.appendMbox32be(0)                                       #pad[2]dont care
            cmd.appendMbox32be(0)                                       #pad[3]dont care
            cmd.appendMbox32be(0)                                       #pad[4]dont care
            cmd.appendMbox32be(0)                                       #pad[5]dont care

            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(totalCoreCount)
        return msg

def vxConvertDepthNode_int(graph, input, output, policy, shift):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    if (ret0 & ret1 & ret2) is False:
        logger.error('vxConvertDepthNode: one or all parameters are wrong')
        raise AttributeError
    btDpthConvNode = VXConvertDepthNode(input['VX_DATA'],output['VX_DATA'])
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = btDpthConvNode
    graph['VX_DATA'].node.append(node)
    return node

