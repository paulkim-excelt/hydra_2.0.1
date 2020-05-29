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
import defn
import utils
import graph
import logging




clrconv_supportlist = [
#VIRT RGB  RGBX NV12 NV21 UYVY YUYV IYUV YUV4 U8   U16  S16  U32  S32  S32C S16C U64  BGR  BGRX GRAY HSV
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #VIRT  - 0
[0xFF,0xFF,0x04,0xFF,0xFF,0xFF,0xFF,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF], #RGB   - 1
[0xFF,0x05,0xFF,0xFF,0xFF,0xFF,0xFF,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0xFF], #RGBX  - 2
[0xFF,0x20,0x21,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #NV12  - 3
[0xFF,0x40,0x41,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #NV21  - 4
[0xFF,0x10,0x11,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #UYVY  - 5
[0xFF,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #YUYV  - 6
[0xFF,0x10,0x11,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #IYUV  - 7
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #YUV4  - 8
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #U8    - 9
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #U16   - 10
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #S16   - 11
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #U32   - 12
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #S32   - 13
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #S32C2 - 14
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #S16C2 - 15
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #U64   - 16
[0xFF,0xFF,0x14,0xFF,0xFF,0xFF,0xFF,0x12,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x10,0xFF], #BGR   - 17
[0xFF,0x15,0xFF,0xFF,0xFF,0xFF,0xFF,0x13,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x11,0xFF], #BGRX  - 18
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #GRAY  - 19
[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF], #HSV   - 20
]

COLORCONVERSION_SUPPORTED = 0
COLORCONVERSION_NOT_SUPPORTED = 1

class VXColorConvertNode(graph.VXNode):
    def __init__(self, input, output):
        logger = logging.getLogger(__name__)
        if ((input.width != output.width) or (input.height != output.height)):
            logger.error('VXColorConvertNode : input and output resolutions are not same inW = %s inHt = %s outW = %s outHt = %s'%(input.width, input.height, output.width, output.height))
            raise ValueError
        if ((input.width%16 !=0) or (input.height < 4) or (input.height % 2 !=0)):
            logger.error('VXColorConvertNode: input/output resolution constraints are not met inW = %s inHt=%s'%(input.width, input.height))
            raise ValueError
        code = clrconv_supportlist[input.type][output.type]
        if code == 0xFF:   # this check would ensure verify input-output type compatibility
            logger.error('VXColorConvertNode : This color conversion is not supported')
            raise TypeError
        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.code = clrconv_supportlist[self.input.type][self.output.type]
        self.setNumJobs(1)

    def createMailBoxLocal(self,totalCoreCount):
        msg = []
        (inStorage, inW, inH, inS) = self.input.getDetails()
        (outStorage, outW, outH, outS) = self.output.getDetails()
        sliceHeightRound = int(inH)/int(totalCoreCount)
        sliceHtRnddiv4 = sliceHeightRound & ~3
        diff = sliceHeightRound - sliceHtRnddiv4
        clrType = self.code >> 4
        convType = self.code & 0xF

        for coreNum in range(totalCoreCount):
            if ((self.input.type != brcm_openvx.VX_DF_IMAGE_UYVY)
                and (self.input.type != brcm_openvx.VX_DF_IMAGE_YUYV)):  #msgIdcvCvtYUYV2RGB
                sliceHeight = sliceHtRnddiv4
            else:
                sliceHeight =  sliceHeightRound
                diff = 0
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(inH) % int(totalCoreCount)) + (diff * int(totalCoreCount))

            outputYImageOffset = (coreNum*int(sliceHeight*outS))
            inputImageOffset  = (coreNum*int(sliceHeight*inS))
            #need to check  input,output for same width,height

            if (self.output.type == brcm_openvx.VX_DF_IMAGE_IYUV):
                outputUImageOffset = (outH*outS) + ((coreNum * sliceHeight * outS)/4)
                outputVImageOffset = (outH*outS) + ((coreNum * sliceHeight * outS)/4) + ((outH*outS)/4)
            else:
                outputUImageOffset = 0
                outputVImageOffset = 0

            if (self.input.type == brcm_openvx.VX_DF_IMAGE_IYUV):
                inputUImageOffset = (inH*inS) + ((coreNum * sliceHeight * inS)/4)
                inputVImageOffset = (inH*inS) + ((coreNum * sliceHeight * inS)/4) + ((inH*inS)/4)
            if ((self.input.type == brcm_openvx.VX_DF_IMAGE_NV12)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_NV21)):
                inputUImageOffset = (inH*inS) + ((coreNum * sliceHeight * inS)/2)
                inputVImageOffset = inputUImageOffset


            cmd = graph.VXCommand()
            cmd.appendStorage(outStorage, outputYImageOffset, 0)
            if (self.output.type == brcm_openvx.VX_DF_IMAGE_IYUV):
                cmd.appendStorage(outStorage, outputUImageOffset, 32)
                cmd.appendStorage(outStorage, outputVImageOffset, 36)

            cmd.appendStorage(inStorage, inputImageOffset, 4)
            if ((self.input.type == brcm_openvx.VX_DF_IMAGE_NV12)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_NV21)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_IYUV)):
                cmd.appendStorage(inStorage, inputUImageOffset, 8)
                cmd.appendStorage(inStorage, inputVImageOffset, 12)
            if ((self.input.type == brcm_openvx.VX_DF_IMAGE_NV12)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_NV21)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_IYUV)):
                cmd.appendMsgType(17)                                   #msgIdcvRGB2YUV
            elif ((self.input.type == brcm_openvx.VX_DF_IMAGE_UYVY)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_YUYV)):  #msgIdcvCvtYUYV2RGB
                cmd.appendMsgType(36)
            else:
                cmd.appendMsgType(29)                                   #msgIdcvRGB2YUV
            cmd.appendMbox32le(outputYImageOffset)                      #dst
            cmd.appendMbox32le(inputImageOffset)                        #src1
            if ((self.input.type == brcm_openvx.VX_DF_IMAGE_NV12)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_NV21)
                or (self.input.type == brcm_openvx.VX_DF_IMAGE_IYUV)):
                cmd.appendMbox32le(inputUImageOffset)                   #src_u if IYUV
                cmd.appendMbox32le(inputVImageOffset)                   #src_v if IYUV
            cmd.appendMbox32be(sliceHeight)
            cmd.appendMbox32be(outW)
            cmd.appendMbox32be(outS)
            cmd.appendMbox32be(inS)

            cmd.appendMbox32be(convType)
            cmd.appendMbox32be(clrType)
            if ((self.input.type != brcm_openvx.VX_DF_IMAGE_NV12)
                and (self.input.type != brcm_openvx.VX_DF_IMAGE_NV21)
                and (self.input.type != brcm_openvx.VX_DF_IMAGE_IYUV)):
                cmd.appendMbox32le(outputUImageOffset)                  #dst_u if IYUV, pad otherwise
                cmd.appendMbox32le(outputVImageOffset)                  #dst_v if IYUV, pad otherwise
            cmd.appendMbox32be(0)                                       #pad[0]dont care
            cmd.appendMbox32be(0)                                       #pad[1]dont care
            cmd.appendMbox32be(0)                                       #pad[2]dont care
            cmd.appendMbox32be(0)                                       #pad[3]dont care

            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(totalCoreCount)
        return msg


def vxColorConvertNode_int(graph, input, output):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    if (ret0 & ret1 & ret2) is False:
        logger.error('vxColorConvertNode: One or all parameters are wrong')
        raise AttributeError
    clrConvNode = VXColorConvertNode(input['VX_DATA'],output['VX_DATA'])
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = clrConvNode
    graph['VX_DATA'].node.append(node)
    return node
