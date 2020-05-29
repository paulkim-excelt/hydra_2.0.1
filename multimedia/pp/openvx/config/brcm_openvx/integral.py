
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

import brcm_openvx
import image
import utils
import graph
import logging
import defn

class VXIntegralImageNode(graph.VXNode):
    def __init__(self, input, output):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type == brcm_openvx.VX_DF_IMAGE_U32)
        ret = ret0 & ret1
        if ret is not True:
            logger.error('VXIntegralImageNode: input and output type constraints are not met')
            raise TypeError
        if ((input.width % 8 != 0) or (input.height < 2)):
            logger.error('VXIntegralImageNode: input and output resolution constraints are not met inW = %s inH = %s'%(input.width,input.height))
            raise ValueError

        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.setNumJobs(1)

    def createMailBoxLocal(self):
        msg = []
        (inStore, inW, inH, inS) = self.input.getDetails()
        (outStore, outW, outH, outS) = self.output.getDetails()
        sliceHeight = int(inH)
        outputImageOffset = 0
        inputImageOffset  = 0
        #need to check  input,output for same width,height
        slice_info = utils.VENDOR_SLICE_FULL_FRAME
        #integral works only for single core
        cmd = graph.VXCommand()
        cmd.appendStorage(outStore, outputImageOffset, 0)
        cmd.appendStorage(inStore, inputImageOffset, 4)
        cmd.appendMsgType(utils.VENDOR_MSG_INTEGRAL)
        cmd.appendMbox32le(outputImageOffset)                       #dst
        cmd.appendMbox32le(inputImageOffset)                        #src
        cmd.appendMbox32be(sliceHeight)
        cmd.appendMbox32be(outW)
        cmd.appendMbox32be(outS)
        cmd.appendMbox32be(inS)
        cmd.appendMbox32be(0)                                       #pad dont care
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
        logger = logging.getLogger(__name__)
        if (totalCoreCount > 1):
                logger.warning('vxIntegralImageNode: runs in single core only')
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal()
        for j in range(1,totalCoreCount):
            cmd = graph.VXCommand()
            cmd.appendDummyMsg()
            msg = msg + cmd.getData()
        return msg

def vxIntegralImageNode_int(graph, input, output):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    if (ret0 & ret1 & ret2) is False:
        logger.error('vxIntegralImageNode: one or all parameters are wrong')
        raise AttributeError
    integralImgNode = VXIntegralImageNode(input['VX_DATA'],output['VX_DATA'])
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = integralImgNode
    graph['VX_DATA'].node.append(node)
    return node

