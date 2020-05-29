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
import pyramid
import struct
import utils
import image
import graph
import logging
import arith
import defn

OPENVX_RESIZE_MAX_WIDTH = 1280+128

class VXScaleNode(graph.VXNode):
    def __init__(self, input, output, type):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret = ret0 & ret1
        if ret is not True:
            logger.error('VXScaleNode: input and output type of VXScaleNode are not same')
            raise TypeError
        if (output.width > input.width) or (output.height > input.height):
            logger.error('VXScaleNode: input and output are wrong wrt height/width or both resolutions are same inW = %s inH = %s outW = %s outH = %s'%(input.width, input.height, output.width, output.height))
            raise ValueError
        if ((output.width > OPENVX_RESIZE_MAX_WIDTH) or (output.height > OPENVX_RESIZE_MAX_WIDTH)):
            logger.error('VXScaleNode: verify: max resolution constraints are not met outW = %s outH = %s'%(outW,outH))
            raise ValueError
        if ((2*output.width < input.width) or (2*output.height < input.height)):
            logger.error('VXScaleNode: input-output img downscaling ratio is more than allowedinW = %s inH = %s outW = %s outH = %s'%(input.width, input.height, output.width, output.height))
            raise ValueError
        logger.info('VXScaleNode: Billinear Interpolation type is only supported')
        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.type = type
        self.setNumJobs(1)

    def setMask(self, m0, split):
        m1 = 0
        if (split != 0):
            for i in range(0,split):
                m1 = m1 | ((15-i)<<(28-i*4))
            for i in range(split,8):
                tmp=(m0>>(28-i*4))&0x0000000f;
                m1 = m1 | ((tmp-8)<<(28-i*4))
        else:
            m1=0xfedcba98;
        return m1
    def presetCoefs(self, width_out, delta):
        pos_old = 0
        shift = 16 #fixed-point precision
        shift8 = 8 #fixed-point precision
        dx = 0
        idx = 0
        mask0 = 0
        mask1 = 0
        split = 0
        base = 0
        base64 = 0
        basecnt = 0
        coeff = []
        step = []
        masks = []
        baseaddr8 = []
        coeff.append(1)
        step.append(0)
        k=delta
        mask0=0xf0000000
        for i in range(1,width_out):
            pos = (k >> shift)
            dx = (k-(pos<<shift))>>(shift-shift8)
            step.append(pos-pos_old)
            pos_old = pos
            k += delta
            if (dx==0):
                dx=1;
            if (dx > 0xff):
                dx=0xff;
            coeff.append(dx);
            j = i%8
            if (j == 0):
                mask1 = self.setMask(mask0, split)
                masks.append(mask0)
                masks.append(mask1)
                mask0=0
                split=0
                base64 |= base<<(56-8*basecnt)
                idx -= 8
                base+=1
                if (idx>=8):
                    if (idx+step[i]>=16):
                        idx-=16;
                        base+=2;
                    else:
                        idx-=8;
                        base+=1;
                else:
                    if (idx>=0):
                        if (idx+step[i]>=8):
                            idx-=8;
                            base+=1;
                basecnt+=1;
                if (basecnt==8):
                    baseaddr8.append(base64)
                    base64=0;
                    basecnt=0;
                idx+=step[i];
                mask0|=(15-idx)<<(28-j*4);
            else:
                idx+=step[i];
                mask0|=(15-(idx%16))<<(28-j*4);
                if(idx>=16 and (split==0)):
                    split=j;
        if (mask0):
            mask1 = self.setMask(mask0,split)
            masks.append(mask0)
            masks.append(mask1)
            for basecnt in range(basecnt,8) :
                base64|=base<<(56-8*basecnt);
            baseaddr8.append(base64)
        step.append(0)
        return (coeff,step,masks,baseaddr8)
    def compute(self, graphScale):
        fone = 1 << 8
        dy = int(((float)(self.input.getHeight()<<16)/self.output.getHeight() + 0.5))
        (coefs_y, step_y, masks, baseaddr8) = self.presetCoefs(self.output.getHeight(), dy)
        dx = int(((float)(self.input.getWidth()<<16)/self.output.getWidth() + 0.5))
        (coefs_x, step_x, masks, baseaddr8) = self.presetCoefs(self.output.getWidth(), dx)
        scaleCoeff = []
        self.overlayOffset = []
        self.overlayOffset.append(0)
        for i in range(0, self.output.getHeight()):
            coeff = (((fone-coefs_y[i]) << 16) | coefs_y[i]) <<8;
            scaleCoeff.append(coeff);
            scaleCoeff.append(step_y[i+1]);
            self.overlayOffset.append(self.overlayOffset[i] + self.input.getStride() * step_y[i+1]);
        base8addrcnt=0;
        for i in range(0, int(len(masks)/2)):
            x= i*8;
            if (i%8 == 0):
                scaleCoeff.append(baseaddr8[base8addrcnt] >> 32);
                scaleCoeff.append(baseaddr8[base8addrcnt] & 0xffffffff);
                base8addrcnt += 1
            scaleCoeff.append(masks[2*i+1])
            scaleCoeff.append(masks[2*i])
            coeff = coefs_x[x:x+8]
            for j in range(len(coeff),8):
                coeff.append(1)
            if len(coeff) != 0 :
                scaleCoeff.append(((fone-coeff[0])<<24) | ((fone-coeff[1])<<16) | ((fone-coeff[2])<<8) | (fone-coeff[3]))
                scaleCoeff.append(((fone-coeff[4])<<24) | ((fone-coeff[5])<<16) | ((fone-coeff[6])<<8) | (fone-coeff[7]))
                scaleCoeff.append(((coeff[0])<<24) | ((coeff[1])<<16) | ((coeff[2])<<8) | (coeff[3]))
                scaleCoeff.append(((coeff[4])<<24) | ((coeff[5])<<16) | ((coeff[6])<<8) | (coeff[7]))
        self.table = []
        for each in scaleCoeff:
            self.table.append(struct.pack('>I',each))
        (self.tableName,self.tableOffset) = graphScale.allocateTable(4*len(self.table))
    def getTableData(self):
        return self.table

    def createMailBox(self,totalCoreCount):
        msg = []
        (outStore, outW, outH, outS) = self.output.getDetails()
        (inStore, inW, inH, inS) = self.input.getDetails()
        sliceHeightRound = int(outH)/int(totalCoreCount)
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(outH) % int(totalCoreCount))

            cmd = graph.VXCommand()
            offset =  coreNum*int(sliceHeightRound*outS)
            cmd.appendStorage(outStore, offset, 0)
            offset =  self.overlayOffset[coreNum*sliceHeightRound]
            cmd.appendStorage(inStore, offset, 4)
            cmd.appendTable(self.tableName, self.tableOffset + 8*coreNum*sliceHeightRound, 32)
            cmd.appendTable(self.tableName, self.tableOffset + 8*outH, 36)

            cmd.appendMsgType(39)
            cmd.appendMbox32le(0) #src1
            cmd.appendMbox32le(0) #src2
            cmd.appendMbox32be(outS)
            cmd.appendMbox32be(inS)
            cmd.appendMbox32be(inH)
            cmd.appendMbox32be(inW)
            cmd.appendMbox32be(sliceHeight)
            cmd.appendMbox32be(outS)
            cmd.appendMbox32le(0)
            cmd.appendMbox32le(0)
            cmd.appendMbox32be(0) #pad[0]
            cmd.appendMbox32be(0) #pad[1]
            cmd.appendMbox32be(0) #pad[2]
            cmd.appendMbox32be(0) #pad[3]
            msg = msg + cmd.getData()
        return msg


def vxScaleImageNode_int (graph, src, dst, type):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(src)
    ret2 = defn.verifyImage(dst)
    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxScaleImageNode: one or all parameters are wrong')
        raise AttributeError
    node = defn.create_dict(defn.VX_NODE,graph)

    if (type != brcm_openvx.VX_INTERPOLATION_BILINEAR):
        logger.error('VXScaleNode: interpolation type = %s is not supported'%(type))
        raise ValueError
    logger.info('VXScaleNode: Allows downscaling and upscaling upto a factor of 2: 0.5 <= x <= 2')
    if ((dst['VX_DATA'].width > src['VX_DATA'].width) and (dst['VX_DATA'].height > src['VX_DATA'].height)):
        if ((src['VX_DATA'].width > OPENVX_RESIZE_MAX_WIDTH/2) or (src['VX_DATA'].height > OPENVX_RESIZE_MAX_WIDTH/2)):
            logger.error('VXScaleNode: verify: max input resolution constraints are not met inW = %s inH = %s'%(src['VX_DATA'].width,src['VX_DATA'].height))
            raise ValueError
        if ((dst['VX_DATA'].width > 2*src['VX_DATA'].width) or (dst['VX_DATA'].height > 2*src['VX_DATA'].height)):
            logger.error('VXScaleNode: input-output img upscaling ratio is more than allowed outW = %s outH = %s'%(dst['VX_DATA'].width,dst['VX_DATA'].height))
            raise ValueError
        tmpImg = image.vxCreateVirtualImage_int(graph,src['VX_DATA'].width*2,
                                    src['VX_DATA'].height*2,src['VX_DATA'].type)
        upsampleNode = pyramid.VXUpSampleBy2x2Node(src['VX_DATA'], tmpImg['VX_DATA'])
        node['VX_DATA'] = upsampleNode
        graph['VX_DATA'].node.append(node)
        if ((dst['VX_DATA'].width < tmpImg['VX_DATA'].width) and
            (dst['VX_DATA'].height < tmpImg['VX_DATA'].height)):
            node = defn.create_dict(defn.VX_NODE,graph)
            scaleNode = VXScaleNode(tmpImg['VX_DATA'], dst['VX_DATA'], type)
            scaleNode.compute(graph['VX_DATA'])
            node['VX_DATA'] = scaleNode
            graph['VX_DATA'].node.append(node)
        if ((dst['VX_DATA'].width == tmpImg['VX_DATA'].width) and
            (dst['VX_DATA'].height == tmpImg['VX_DATA'].height)):
            node = defn.create_dict(defn.VX_NODE,graph)
            copyNode = arith.VXArithS(tmpImg['VX_DATA'],None, dst['VX_DATA'],arith.OPCODE_ARITH_COPY,0,0);
            node['VX_DATA'] = copyNode
            graph['VX_DATA'].node.append(node)
    elif ((dst['VX_DATA'].width == src['VX_DATA'].width) and (dst['VX_DATA'].height == src['VX_DATA'].height)):
        node = defn.create_dict(defn.VX_NODE,graph)
        copyNode = arith.VXArithS(src['VX_DATA'],None, dst['VX_DATA'],arith.OPCODE_ARITH_COPY,0,0);
        node['VX_DATA'] = copyNode
        graph['VX_DATA'].node.append(node)
    else:
        scaleNode = VXScaleNode(src['VX_DATA'], dst['VX_DATA'], type)
        scaleNode.compute(graph['VX_DATA'])
        node['VX_DATA'] = scaleNode
        graph['VX_DATA'].node.append(node)
    return node