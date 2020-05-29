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
import logging
import graph
import arith
import numbers
import defn


VX_DILATE  = 0
VX_ERODE   = 1

VX_MORPH_TYPE={
0:'VX_DILATE',
1:'VX_ERODE'
}

class VXMorph(graph.VXNode):
    def __init__(self, input, output, tmpImg, type, iterations):
        logger = logging.getLogger(__name__)
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret = ret0 & ret1
        if ret is not True:
            ret2 = (input.type == brcm_openvx.VX_DF_IMAGE_S16)
            ret3 = (output.type == brcm_openvx.VX_DF_IMAGE_S16)
            ret = ret2 & ret3
            if ret is not True:
                ret2 = (input.type == brcm_openvx.VX_DF_IMAGE_S32)
                ret3 = (output.type == brcm_openvx.VX_DF_IMAGE_S32)
                ret = ret2 & ret3
                if ret is not True:
                    logger.error('VXMorph: %s: input or output type constraints are not met'%(VX_MORPH_TYPE[type]))
                    raise TypeError
        if ((input.width != output.width) or (input.height != output.height)):
            logger.error('VXMorph:%s:  input or output resolutions are not same inW = %s inH = %s outW = %s outH = %s'%(VX_MORPH_TYPE[type], input.width,input.height, output.width,output.height))
            raise ValueError
        if ((input.width % 8 != 0) or (input.height < 4)):
            logger.error('VXMorph:%s: input or output resolution constraints are not met inW = %s inH = %s'%(VX_MORPH_TYPE[type], input.width,input.height))
            raise ValueError
        if (iterations < 1):
            logger.error('VXMorph:%s: iterations cannot be zero'%(VX_MORPH_TYPE[type]))
            raise ValueError


        if (input.type == brcm_openvx.VX_DF_IMAGE_U8 ): #U8/U16/S32
            self.bitMode = 0
        elif (input.type == brcm_openvx.VX_DF_IMAGE_S16 ):
            self.bitMode = 1
        elif (input.type == brcm_openvx.VX_DF_IMAGE_S32):
            self.bitMode = 2
        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.setNumJobs(iterations)
        self.type = type            #Dilate/Erode
        self.tmpImg = tmpImg

        if type == VX_DILATE:
            self.mask = 0xFF
            self.mask_last = 0xFF
        else:
            self.mask = 0
            self.mask_last = 0

    def createMailBoxLocal(self,totalCoreCount,inp,outp):
        msg = []
        (inStore, inW, inH, inS) = inp.getDetails()
        (outStore, outW, outH, outS) = outp.getDetails()
        sliceHeightRound = int(outH)/int(totalCoreCount)
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(outH) % int(totalCoreCount))
            outputImageOffset = (coreNum*int(sliceHeightRound*outS))
            inputImageOffset  = (coreNum*int(sliceHeightRound*inS))
            #need to check  input,output for same width,height apart from type
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


            cmd.appendMsgType(2) #msgIdcvDilateErode
            cmd.appendMbox32le(outputImageOffset) #dst
            cmd.appendMbox32le(inputImageOffset)#src1

            cmd.appendMbox32be(sliceHeight)
            cmd.appendMbox32be(outW)
            cmd.appendMbox32be(outS)
            cmd.appendMbox32be(inS)
            for i in range(8):
                cmd.appendMbox8(self.mask) #mask

            cmd.appendMbox32be(0)               #threshold_val   - dont care
            cmd.appendMbox8(0)                   #cpad[0] -- dontcare
            cmd.appendMbox8(0)                   #cpad[1] - dontcare
            cmd.appendMbox8(0)                   #cpad[2] - dontcare
            cmd.appendMbox8(self.mask_last)       #mask_last


            cmd.appendMbox32be(slice_info) #slice_info
            cmd.appendMbox32be(self.type)       #type.. dilate/erode
            cmd.appendMbox32be(0)                #yoffset.. dontcare
            cmd.appendMbox32be(self.bitMode)    #0 = 8bit mode, 1 = 16bit mode
            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,totalCoreCount):
        msg = []
        in0 = self.input
        out0 = self.output
        if (self.getNumJobs()== 1):
                msg = msg + self.createMailBoxLocal(totalCoreCount,in0,out0)

        else:
            if ((self.getNumJobs() & 1) == 0):
                mInput = out0
                mOutput = self.tmpImg
            else:
                mInput = self.tmpImg
                mOutput = out0

            for i in range(self.getNumJobs()):

                if (i==0):
                    out0 = mOutput
                else:
                    tmpswp = mOutput
                    mOutput = mInput
                    mInput = tmpswp
                    in0 = mInput
                    out0 = mOutput

                msg = msg + self.createMailBoxLocal(totalCoreCount,in0,out0)
        return msg

def DilateErode3x3Node (graph, input, output,tmpImg, type,iterations):
    logger = logging.getLogger(__name__)
    node = defn.create_dict(defn.VX_NODE,graph)
    if tmpImg is not None:
        newTmpImg = tmpImg['VX_DATA']
    else:
        newTmpImg = None

    node['VX_DATA'] = VXMorph(input['VX_DATA'],output['VX_DATA'],newTmpImg, type, iterations)
    graph['VX_DATA'].node.append(node)
    return node


def vxDilate3x3Node_int (graph, input, output):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)

    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxDilate3x3Node: one or all parameters are wrong')
        raise AttributeError

    return DilateErode3x3Node(graph, input,output,None,VX_DILATE,1)

def vxErode3x3Node_int (graph, input, output):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)

    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxErode3x3Node: one or all parameters are wrong')
        raise AttributeError

    return DilateErode3x3Node(graph, input, output,None,VX_ERODE,1)

def vxMorphologyExtNode_int (graph, input, output, operation, iterations):

    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    ret3 = isinstance(iterations,numbers.Number)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxMorphologyExtNode: one or all parameters are wrong')
        raise AttributeError
    if ((operation< brcm_openvx.VX_MORPH_ERODE_EXT) or (operation > brcm_openvx.VX_MORPH_BLACKHAT_EXT)):
        logger.error('VXMorph: this operation is not supported')
        raise ValueError

    if (input['VX_DATA'].type == brcm_openvx.VX_DF_IMAGE_S32 ):
        logger.error('vxMorphologyExtNode: type constraints are not met')
        raise TypeError

    tmpImg =[image.vxCreateVirtualImage_int(graph,input['VX_DATA'].width,
                                    input['VX_DATA'].height,input['VX_DATA'].type),
            image.vxCreateVirtualImage_int(graph,input['VX_DATA'].width,
                                    input['VX_DATA'].height,input['VX_DATA'].type)]
    for img in tmpImg:
        if img is None:
            logger.error('vxMorphologyExtNode: virtual imgs cannot be created')
            raise AttributeError

    if (operation == brcm_openvx.VX_MORPH_ERODE_EXT):

        return DilateErode3x3Node(graph,input,output,tmpImg[0],VX_ERODE,iterations)

    elif (operation == brcm_openvx.VX_MORPH_DILATE_EXT):
        return DilateErode3x3Node(graph,input,output,tmpImg[0],VX_DILATE,iterations)

    elif (operation == brcm_openvx.VX_MORPH_OPEN_EXT):
        DilateErode3x3Node(graph,input,tmpImg[0],output,VX_ERODE,iterations)#can create one more temp image instead of reusin output as tmp
        return DilateErode3x3Node(graph,tmpImg[0],output,tmpImg[1],VX_DILATE,iterations)

    elif (operation == brcm_openvx.VX_MORPH_CLOSE_EXT):
        DilateErode3x3Node(graph,input,tmpImg[0],output,VX_DILATE,iterations)
        return DilateErode3x3Node(graph,tmpImg[0],output,tmpImg[1],VX_ERODE,iterations)

    elif (operation == brcm_openvx.VX_MORPH_GRADIENT_EXT):
        DilateErode3x3Node(graph,input,tmpImg[0],output,VX_DILATE,iterations)
        DilateErode3x3Node(graph,input,tmpImg[1],output,VX_ERODE,iterations)
        return arith.vxSubtractNode_int(graph,tmpImg[0],tmpImg[1], 0, output)    #subtract u16 s not supported

    elif (operation == brcm_openvx.VX_MORPH_TOPHAT_EXT):
        DilateErode3x3Node(graph,input,tmpImg[0],output,VX_ERODE,iterations)
        DilateErode3x3Node(graph, tmpImg[0],tmpImg[1],output,VX_DILATE,iterations)
        return arith.vxSubtractNode_int(graph,input, tmpImg[1], 0, output) #subtract u16 s not supported

    elif (operation == brcm_openvx.VX_MORPH_BLACKHAT_EXT):
        DilateErode3x3Node(graph,input,tmpImg[0],output,VX_DILATE,iterations)
        DilateErode3x3Node(graph, tmpImg[0],tmpImg[1],output,VX_ERODE,iterations)
        return arith.vxSubtractNode_int(graph, tmpImg[1],input, 0, output)#subtract u16 s not supported

