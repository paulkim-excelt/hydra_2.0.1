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
import graph
import context
import scale
import filter
import arith
import bitdepth
import utils
import logging
import defn
import numbers


class VXPyramid():
    def __init__(self, aContext, levels, scale, width, height, type):
        logger =logging.getLogger(__name__)
        if ((levels < 1) or (width < 1) or (height < 1) or (type < brcm_openvx.VX_DF_IMAGE_RGB) or (type > brcm_openvx.VX_DF_IMAGE_HSV_EXT)):
            logger.error('VXPyramid":validate: param fails')
            raise ValueError
        if ((scale != brcm_openvx.VX_SCALE_PYRAMID_ORB) and (scale != brcm_openvx.VX_SCALE_PYRAMID_HALF)):
            logger.error('VXPyramid":validate: scale factor fails')
            raise ValueError
        self.levels = levels
        self.scale = scale
        self.width = width
        self.height = height
        self.type = type
        w = width
        h = height
        offsets = [0]
        for i in range(levels-1):
            offsets.append(offsets[i] + image.getImageSize(w, h, type))
            w = int(round(w * scale))
            h = int(round(h * scale))
            if (((w<=1) or (h<=1))and (i!=levels-2)):
                logger.error('VXPyramid":width and height is less than one')
                raise ValueError
        totalSize = offsets[levels-1] + image.getImageSize(w, h, type)
        self.storage = aContext.allocateStorage(totalSize)
        w = width
        h = height
        self.images = []
        for i in range(levels):
            intWdth = (int)((w+7)/8)*8 #height limitations? check further TBD
            self.images.append( image.VXImage(aContext, intWdth, h, type, context.VXStorage(0, 0, offsets[i], self.storage)))
            #print 'intWidth = %s w = %s, h = %s' %(intWdth,w,h)
            w = int(round(w * scale))
            h = int(round(h * scale))

    def getNumLevels(self):
        return self.levels
    def getImage(self, level):
        if level < self.levels:
            return self.images[level]
        else:
            None
    def getStorageDetails(self):
        return self.storage.getDetails()
    def setName(self,name):
        pass

class VXGaussianPyramidNode(graph.VXNode):
    def __init__(self, graphPyr, input, pyramid):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (pyramid.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret = (ret0 & ret1)
        if ret is not True:
            logger.error('VXGaussianPyramidNode: input/pyramid type constraints are not met')
            raise TypeError
        if ((input.width % 16 != 0) or (input.height %8 != 0)) :
            logger.error('VXGaussianPyramidNode: input/pyramid resolution constraints are not met inW = %s inH = %s' %(input.width,input.height))
            raise ValueError
        if ((pyramid.scale != brcm_openvx.VX_SCALE_PYRAMID_ORB)
        and (pyramid.scale != brcm_openvx.VX_SCALE_PYRAMID_HALF)):
            logger.error('VXGaussianPyramidNode: pyramid scale constraints are not met scale = %s'%(pyramid.scale))
            raise ValueError
        outImg = pyramid.getImage(0)
        if ((input.width != outImg.width) or (input.height != outImg.height)):
            logger.error('VXGaussianPyramidNode: input W=%s H=%s and first level pyramid W=%s H=%s'%(input.width,input.height,outImg.width,outImg.height))
            raise ValueError
        graph.VXNode.__init__(self)
        self.pyramid = pyramid
        self.scaleNode = []
        self.gausspyrORBNode = []
        self.copyNode = arith.VXArithS(input,None,pyramid.getImage(0),arith.OPCODE_ARITH_COPY,0,0);
        if (self.pyramid.scale is brcm_openvx.VX_SCALE_PYRAMID_ORB):
            (inputStore, inputW, inputH, inputS) = input.getDetails()
            size = inputStore.getSize()
            self.storage = graphPyr.allocateStorage(size)
            for i in range(pyramid.getNumLevels()-1):
                input = self.pyramid.getImage(i)
                output = self.pyramid.getImage(i+1)
                (inputStore, inputW, inputH, inputS) = input.getDetails()
                tmpImg = image.VXImage(graphPyr, inputW, inputH, brcm_openvx.VX_DF_IMAGE_U8, self.storage)
                node  =filter.VXGaussian5x5(input,tmpImg)
                self.gausspyrORBNode.append(node)
                node = scale.VXScaleNode(tmpImg,output,brcm_openvx.VX_INTERPOLATION_BILINEAR)
                self.scaleNode.append(node)
            self.setNumJobs(pyramid.getNumLevels()*2-1)
        else:
            self.setNumJobs(pyramid.getNumLevels())

    def getTableData(self):
        table = []
        for i in range(len(self.scaleNode)):
            for each in self.scaleNode[i].getTableData():
                table.append(each)
        return table
    def createMailBoxLocal(self,input,output,total_cores):
        msg = []
        (inputStore, inputW, inputH, inputS) = input.getDetails()
        (outputStore, outputW, outputH, outputS) = output.getDetails()
        #print str(inputName)+":Width=" +str(inputW) + " Height="+ str(inputH) + " Stride="+ str(inputS) + " Offset=" + str(inputOffset)
        #print str(outputName)+":Width=" +str(outputW) + " Height="+ str(outputH) + " Stride="+ str(outputS)+ " Offset=" + str(outputOffset)
        slice_height_round = int(inputH)/int(total_cores)
        slice_height_round = slice_height_round - (slice_height_round%2)
        for core_num in range(total_cores):
            slice_height = slice_height_round
            if core_num == total_cores - 1:
                slice_height = int(inputH) - ((total_cores - 1)*slice_height_round)
            cmd = graph.VXCommand()
            offset = (core_num*int(slice_height_round*inputS))
            cmd.appendStorage(inputStore, offset, 4)
            offset = (core_num*int((slice_height_round/2)*outputS))
            cmd.appendStorage(outputStore, offset, 0)
            cmd.appendMsgType(4)
            cmd.appendMbox32le(0) #dst
            cmd.appendMbox32le(0) #src
            cmd.appendMbox32be(slice_height) #src
            cmd.appendMbox32be(inputW) #src
            cmd.appendMbox32be(outputS) #src
            cmd.appendMbox32be(inputS) #src
        #mask0
            cmd.appendMbox8(1)
            cmd.appendMbox8(4)
            cmd.appendMbox8(6)
            cmd.appendMbox8(4)
            cmd.appendMbox8(1)
            cmd.appendMbox8(4)
            cmd.appendMbox8(16)
            cmd.appendMbox8(24)
        #mask1
            cmd.appendMbox8(16)
            cmd.appendMbox8(4)
            cmd.appendMbox8(6)
            cmd.appendMbox8(24)
            cmd.appendMbox8(36)
            cmd.appendMbox8(24)
            cmd.appendMbox8(6)
            cmd.appendMbox8(4)
        #mask2
            cmd.appendMbox8(16)
            cmd.appendMbox8(24)
            cmd.appendMbox8(16)
            cmd.appendMbox8(4)
            cmd.appendMbox8(1)
            cmd.appendMbox8(4)
            cmd.appendMbox8(6)
            cmd.appendMbox8(4)
            if total_cores > 1:
                if core_num == 0:
                    slice = 1
                elif core_num == total_cores - 1:
                    slice = 4
                else:
                    slice = 2
            else:
                slice = 0
            cmd.appendMbox32be(slice) #slice_info
        #cpad[3]
            cmd.appendMbox8(0)
            cmd.appendMbox8(0)
            cmd.appendMbox8(0)
        #mmask_last
            cmd.appendMbox8(1)
            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,total_cores):
        msg = []
        # create a mailbox for copy
        msg = msg + self.copyNode.createMailBox(total_cores)
        if (self.pyramid.scale is brcm_openvx.VX_SCALE_PYRAMID_ORB):
            for i in range(self.pyramid.levels-1):
                msg = msg + self.gausspyrORBNode[i].createMailBox(total_cores)
                msg = msg + self.scaleNode[i].createMailBox(total_cores)
        else:
            for i in range(self.pyramid.levels-1):
                input = self.pyramid.getImage(i)
                output = self.pyramid.getImage(i+1)
                msg = msg + self.createMailBoxLocal(input,output,total_cores)
        return msg


class VXHalfScaleGaussianNode(VXGaussianPyramidNode):
    def __init__(self, input, output, tmp, kernel_size):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8 )
        ret1 = (output.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret = ret0 & ret1
        if ret is not True:
            logger.error('VXHalfScaleGaussianNode: input/output type constraints are not met')
            raise TypeError
        if ((input.width % 16 != 0) or (input.height %8 != 0)) :
            logger.error('VXHalfScaleGaussianNode: input resolution constraints are not met inW = %s inH = %s'%(input.width,input.height))
            raise ValueError
        if ((input.width != 2*output.width)
        or (input.height != 2*output.height)):
            logger.error('VXHalfScaleGaussianNode: input/output resolution constraints are not met inW = %s inH = %s outW = %s outH = %s'%(input.width,input.height,output.width,output.height))
            raise ValueError
        if ((kernel_size != 3) and (kernel_size!= 5)):
            logger.error('VXHalfScaleGaussianNode: Kernel size  = %s is not supported'%(kernel_size))
            raise ValueError
        self.input = input
        self.output = output
        self.scaleNode = []
        self.gaussfilterNode = []
        self.kernel_size = kernel_size
        if kernel_size == 3:
            node  = filter.VXSmooth(input,tmp,filter.VENDOR_FILTER_GAUSSIAN,0)
            self.gaussfilterNode.append(node)
            node = scale.VXScaleNode(tmp,output,brcm_openvx.VX_INTERPOLATION_BILINEAR)
            self.scaleNode.append(node)
            self.setNumJobs(2)
        elif kernel_size == 5:
            self.setNumJobs(1)

    def getTableData(self):
        table = []
        if self.kernel_size == 3:
            for i in range(len(self.scaleNode)):
                for each in self.scaleNode[i].getTableData():
                    table.append(each)
        return table

    def createMailBox(self,total_cores):
        msg = []
        if (self.kernel_size == 3):
                msg = msg + self.gaussfilterNode[0].createMailBox(total_cores)
                msg = msg + self.scaleNode[0].createMailBox(total_cores)
        else:
            for i in range(self.getNumJobs()):
                msg = msg + self.createMailBoxLocal(self.input,self.output,total_cores)
        return msg


class VXLaplacianPyramidNode(graph.VXNode):
    def __init__(self, graphLP, input, laplacian, output):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (laplacian.type == brcm_openvx.VX_DF_IMAGE_S16)
        ret2 = (output.type == brcm_openvx.VX_DF_IMAGE_S16)
        ret = (ret0 & ret1 & ret2);
        if ret is not True:
            logger.error('VXLaplacianPyramidNode: input/output type constraints are not met')
            raise TypeError
        if (laplacian.scale != brcm_openvx.VX_SCALE_PYRAMID_HALF):
            logger.error('VXLaplacianPyramidNode: scale constraints are not met %s'%(laplacian.scale))
            raise ValueError
        if ((input.width % 16 != 0) or (input.height < 8)):
            logger.error('VXLaplacianPyramidNode: input/output resolution constraints are not met inW = %s inH = %s'%(input.width, input.height))
            raise ValueError
        outImg = laplacian.getImage(0)
        if ((input.width != outImg.width) or (input.height != outImg.height)):
            logger.error('VXLaplacianPyramidNode: input W=%s H=%s and first level pyramid W=%s H=%s'%(input.width,input.height,outImg.width,outImg.height))
            raise ValueError
        graph.VXNode.__init__(self)
        self.input = input
        self.pyramid = laplacian
        self.output = output
        (inputStore, inputW, inputH, inputS) = input.getDetails()
        self.storage = []

        self.outF = []
        self.setNumJobs(self.pyramid.getNumLevels()+1)
        for i in range(1,self.pyramid.levels):
            input = self.pyramid.getImage(i)
            (inputStore, inputW, inputH, inputS) = input.getDetails()
            tmpImg = image.VXImage(graphLP, inputW, inputH, brcm_openvx.VX_DF_IMAGE_U8, None)
            self.outF.append(tmpImg)
        (inputStore, inputW, inputH, inputS) = output.getDetails()
        tmpImg = image.VXImage(graphLP, inputW, inputH, brcm_openvx.VX_DF_IMAGE_U8, None)
        self.outF.append(tmpImg)
        self.sclImg = bitdepth.VXConvertDepthNode(tmpImg,output)

    def createMailBoxLocal(self,input,outH,outF,total_cores):
        msg = []
        (inputStore, inputW, inputH, inputS) = input.getDetails()
        (outHStore, outHW, outHH, outHS) = outH.getDetails()
        (outFStore, outFW, outFH, outFS) = outF.getDetails()
        slice_height_round = int(inputH)/int(total_cores)
        slice_height_round = slice_height_round - (slice_height_round%2)
        for core_num in range(total_cores):
            slice_height = slice_height_round
            if core_num == total_cores - 1:
                slice_height = int(inputH) - ((total_cores - 1)*slice_height_round)
            cmd = graph.VXCommand()
            offset = (core_num*int(slice_height_round*inputS))
            cmd.appendStorage(inputStore, offset, 4)
            offset = (core_num*int(slice_height_round*outHS))
            cmd.appendStorage(outHStore, offset, 0)
            offset = (core_num*int((slice_height_round/2)*outFS))
            cmd.appendStorage(outFStore, offset, 36)
            cmd.appendMsgType(utils.VENDOR_MSG_LPDOWN) # LPDown
            cmd.appendMbox32le(0) #dst
            cmd.appendMbox32le(0) #src
            cmd.appendMbox32be(slice_height) #src
            cmd.appendMbox32be(inputW) #src
            cmd.appendMbox32be(outHS)
            cmd.appendMbox32be(inputS)
            cmd.appendMbox32be(0) #pad[0]
            if total_cores > 1:
                if core_num == 0:
                    slice = 1
                elif core_num == total_cores - 1:
                    slice = 4
                else:
                    slice = 2
            else:
                slice = 0
            cmd.appendMbox32be(slice) #slice_info
            cmd.appendMbox32be(0) #pad[1]
            cmd.appendMbox32le(0) #outF
            cmd.appendMbox32be(0) #pad[1]
            cmd.appendMbox32be(outFS)
            cmd.appendMbox32be(0) #pad[2]
            cmd.appendMbox32be(0) #pad[3]
            msg = msg + cmd.getData()
        return msg
    def createMailBox(self,total_cores):
        msg = []
        for i in range(self.pyramid.levels):
            if i is 0:
                input = self.input
            else:
                input = self.outF[i-1]
            outH = self.pyramid.getImage(i)
            outF = self.outF[i]
            msg = msg + self.createMailBoxLocal(input,outH,outF,total_cores)
        msg = msg + self.sclImg.createMailBox(total_cores)
        return msg

class VXUpSampleBy2x2Node(graph.VXNode):
    def __init__(self, input, output):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (output.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret = (ret0 & ret1)
        if ret is not True:
            logger.error('VXUpSampleBy2x2Node: input/output type constraints are not met')
            raise TypeError
        if ((input.width %16 != 0) or (input.height %2 != 0) or (input.height < 8)):
            logger.error('VXUpSampleBy2x2Node: input type constraints are not met inW = %s inH = %s'%(input.width, input.height))
            raise ValueError
        if ((input.width*2 != output.width) or (input.height*2 != output.height)):
            logger.error('VXUpSampleBy2x2Node: input/output resolution constraints are not met inW = %s inH = %s outW = %s outH = %s'%(input.width, input.height, output.width, output.height))
            raise ValueError
        graph.VXNode.__init__(self)
        self.input = input
        self.output = output
        self.setNumJobs(1)

    def createMailBoxLocal(self,totalCoreCount):
        msg = []
        (inputStore, inputW, inputH, inputS) = self.input.getDetails()
        (outputStore, outputW, outputH, outputS) = self.output.getDetails()
        slice_height_round = int(inputH)/int(totalCoreCount)
        slice_height_round = slice_height_round - (slice_height_round%2)
        for coreNum in range(totalCoreCount):
            slice_height = slice_height_round
            if coreNum == totalCoreCount - 1:
                slice_height = int(inputH) - ((totalCoreCount - 1)*slice_height_round)
            cmd = graph.VXCommand()
            offset = (coreNum*int(slice_height_round*inputS))
            cmd.appendStorage(inputStore, offset, 4)
            offset = (coreNum*int((slice_height_round*2)*outputS))
            cmd.appendStorage(outputStore, offset, 0)
            cmd.appendMsgType(28)       #upsample
            cmd.appendMbox32le(0) #dst
            cmd.appendMbox32le(0) #src
            cmd.appendMbox32be(slice_height) #src
            cmd.appendMbox32be(inputW) #src
            cmd.appendMbox32be(outputS) #src
            cmd.appendMbox32be(inputS) #src
        #mask0
            cmd.appendMbox8(1)
            cmd.appendMbox8(4)
            cmd.appendMbox8(6)
            cmd.appendMbox8(4)
            cmd.appendMbox8(1)
            cmd.appendMbox8(4)
            cmd.appendMbox8(16)
            cmd.appendMbox8(24)
        #mask1
            cmd.appendMbox8(16)
            cmd.appendMbox8(4)
            cmd.appendMbox8(6)
            cmd.appendMbox8(24)
            cmd.appendMbox8(36)
            cmd.appendMbox8(24)
            cmd.appendMbox8(6)
            cmd.appendMbox8(4)
        #mask2
            cmd.appendMbox8(16)
            cmd.appendMbox8(24)
            cmd.appendMbox8(16)
            cmd.appendMbox8(4)
            cmd.appendMbox8(1)
            cmd.appendMbox8(4)
            cmd.appendMbox8(6)
            cmd.appendMbox8(4)
            if totalCoreCount==1:
                slice_info = utils.VENDOR_SLICE_FULL_FRAME
            elif (totalCoreCount > 1):
                if coreNum==0 :
                    slice_info = utils.VENDOR_SLICE_TOP_SLICE
                elif (coreNum < totalCoreCount - 1):
                    slice_info = utils.VENDOR_SLICE_MIDDLE_SLICE
                else:
                    slice_info = utils.VENDOR_SLICE_BOTTOM_SLICE
            cmd.appendMbox32be(slice_info) #slice_info
        #cpad[3]
            cmd.appendMbox8(0)
            cmd.appendMbox8(0)
            cmd.appendMbox8(0)
        #mmask_last
            cmd.appendMbox8(1)
            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,total_cores):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(total_cores)
        return msg


class VXLaplacianReconstructNode(graph.VXNode):
    def __init__(self, graphLPR, laplacian, input, output):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_S16)
        ret1 = (laplacian.type == brcm_openvx.VX_DF_IMAGE_S16)
        ret2 = (output.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret = (ret0 & ret1 & ret2);
        if ret is not True:
            logger.error('VXLaplacianReconstructNode: input/output type constraints are not met')
            raise TypeError
        if (laplacian.scale != brcm_openvx.VX_SCALE_PYRAMID_HALF):
            logger.error('VXLaplacianReconstructNode: scale =%s is not supported '%(laplacian.scale))
            raise  ValueError
        graph.VXNode.__init__(self)
        self.laplacian = laplacian
        self.levels = laplacian.getNumLevels()
        chkImglast = laplacian.getImage(0)
        if ((chkImglast.width != output.width) or (chkImglast.height != output.height)):
            logger.error('VXLaplacianReconstructNode: final pyramid image and output image doesnt match')
            raise ValueError
        chkImgFirst = laplacian.getImage(self.levels-1)
        if ((chkImgFirst.width != (input.width*2)) or (chkImgFirst.height != (input.height*2))):
            logger.error('VXLaplacianReconstructNode: first level pyramid image and input image doesnt match appropriately')
            raise ValueError
        self.storage = []
        self.input = input
        self.output = output
        self.upscaleNode = []
        self.outConvNode = []
        self.sclImg=[]
        self.addNode = []

        (outStore, width, height, step) = output.getDetails()
        size = outStore.getSize()

        self.storage.append(graphLPR.allocateStorage(size*2))
        self.storage.append(graphLPR.allocateStorage(size))

        for i in range (self.levels):
            nX = self.levels-i-1
            gImg = laplacian.getImage(nX)
            (imStore, width, height, step) = gImg.getDetails()
            upsclImgtmp = image.VXImage(graphLPR, width, height, brcm_openvx.VX_DF_IMAGE_U8, self.storage[1])
            (imStore, inW, inH, step) = input.getDetails()
            outImg = image.VXImage(graphLPR, inW, inH, brcm_openvx.VX_DF_IMAGE_U8, outStore)
            node = bitdepth.VXConvertDepthNode(input,outImg)
            self.outConvNode.append(node)
            node  = VXUpSampleBy2x2Node(outImg,upsclImgtmp)
            self.upscaleNode.append(node)
            out16Img = image.VXImage(graphLPR, width, height, brcm_openvx.VX_DF_IMAGE_S16, self.storage[0])
            node = bitdepth.VXConvertDepthNode(upsclImgtmp,out16Img)
            self.sclImg.append(node)
            node = arith.VXArithV(gImg,out16Img,out16Img,arith.OPCODE_ARITH_ADD,0)
            self.addNode.append(node)
            input = out16Img
        (outStore, width, height, step) = output.getDetails()
        output = image.VXImage(graphLPR, width, height, brcm_openvx.VX_DF_IMAGE_U8, outStore)
        self.lastNode = bitdepth.VXConvertDepthNode(out16Img,output)
        self.setNumJobs((self.levels)*4 + 1)

    def createMailBox(self,total_cores):
        msg = []
        for i in range(self.levels):
            msg = msg + self.outConvNode[i].createMailBox(total_cores)
            msg = msg + self.upscaleNode[i].createMailBox(total_cores)
            msg = msg + self.sclImg[i].createMailBox(total_cores)
            msg = msg + self.addNode[i].createMailBox(total_cores)
        msg = msg + self.lastNode.createMailBox(total_cores)
        return msg

def vxCreatePyramid_int(context,levels,scale,width,height,type):
    logger = logging.getLogger(__name__)
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreatePyramid: Context cannot be verified')
        raise AttributeError
    if ((isinstance(levels,numbers.Number)==False) or (isinstance(scale,numbers.Number)==False)):
        logger.error('vxCreatePyramid: levels/scale are not numbers')
        raise TypeError
    if ((isinstance(width,numbers.Number)==False) or (isinstance(height,numbers.Number)==False) or (isinstance(type,numbers.Number)==False)):
        logger.error('vxCreatePyramid: levels/scale are not numbers')
        raise TypeError
    pyrd = defn.create_dict(defn.VX_PYRAMID,context)
    pyrd['VX_DATA'] = VXPyramid(context['VX_DATA'], levels, scale, width, height, type)
    context['VX_PYRAMID'].append(pyrd)
    return pyrd

def vxReleasePyramid(pyramid):
    #verify pyramid
    pyramid['VX_DATA'] = None

def vxGetPyramidLevel_int(pyramid,idx):
    logger = logging.getLogger(__name__)
    ret = defn.verifyPyramid(pyramid)
    if ret is not True:
        logger.error('vxGetPyramidLevel: pyramid cannot be verified')
        raise AttributeError
    if idx > (pyramid['VX_DATA'].levels - 1):
        logger.error('vxGetPyramidLevel: level doesnt exist')
        raise AttributeError
    image = defn.create_dict(defn.VX_IMAGE,pyramid)
    image['VX_DATA'] = pyramid['VX_DATA'].images[idx]
    pyramid['VX_PARENT']['VX_IMAGE'].append(image)
    return image

def vxGaussianPyramidNode_int(graph, input, pyramiid):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyPyramid(pyramiid)
    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxGaussianPyramidNode: one or all parameters are wrong')
        raise AttributeError
    node = defn.create_dict(defn.VX_NODE,graph)
    gaussPyrNode = VXGaussianPyramidNode(graph['VX_DATA'], input['VX_DATA'], pyramiid['VX_DATA'])
    if (pyramiid['VX_DATA'].scale is brcm_openvx.VX_SCALE_PYRAMID_ORB):
        for i in range(pyramiid['VX_DATA'].levels - 1):
            gaussPyrNode.scaleNode[i].compute(graph['VX_DATA'])
    node['VX_DATA'] = gaussPyrNode
    graph['VX_DATA'].node.append(node)
    return node

def vxHalfScaleGaussianNode_int(graph, input, output, kernel_size):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    ret3 = isinstance(kernel_size, numbers.Number)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxHalfScaleGaussianNode: one or all parameters are wrong')
        raise AttributeError

    tmpImg =image.vxCreateVirtualImage_int(graph,input['VX_DATA'].width,
                                    input['VX_DATA'].height,input['VX_DATA'].type)
    for imag in tmpImg:
        if imag is None:
            logger.error('vxHalfScaleGaussianNode: vxCreateVirtualImage cannot be created ')
            raise AttributeError

    node = defn.create_dict(defn.VX_NODE,graph)
    hlfSclGaussNode = VXHalfScaleGaussianNode(input['VX_DATA'], output['VX_DATA'], tmpImg['VX_DATA'], kernel_size)
    if kernel_size is 3:
        hlfSclGaussNode.scaleNode[0].compute(graph['VX_DATA'])
    node['VX_DATA'] = hlfSclGaussNode
    graph['VX_DATA'].node.append(node)
    return node

def vxLaplacianPyramidNode_int(graph, input, laplacian, output):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyPyramid(laplacian)
    ret3 = defn.verifyImage(output)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxLaplacianPyramidNode: one or all parameters are wrong')
        raise AttributeError

    node = defn.create_dict(defn.VX_NODE,graph)
    laplacianNode = VXLaplacianPyramidNode(graph['VX_DATA'],input['VX_DATA'], laplacian['VX_DATA'],output['VX_DATA'])
    node['VX_DATA'] = laplacianNode
    graph['VX_DATA'].node.append(node)
    return node

def vxUpSampleBy2x2Node_int(graph, input, output):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    ret = ret0 & ret1 & ret2;
    if ret is not True:
        logger.error('vxUpSampleBy2x2Node: one or all parameters are wrong')
        raise AttributeError

    upsampleNode = VXUpSampleBy2x2Node(input['VX_DATA'], output['VX_DATA'])
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = upsampleNode
    graph['VX_DATA'].node.append(node)
    return node

def vxLaplacianReconstructNode_int(graph, laplacian, input, output):
    logger = logging.getLogger(__name__)
    #validate parameters
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyPyramid(laplacian)
    ret3 = defn.verifyImage(output)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxLaplacianReconstructNode: one or all parameters are wrong')
        raise AttributeError

    node = defn.create_dict(defn.VX_NODE,graph)
    laplacianNode = VXLaplacianReconstructNode(graph['VX_DATA'],laplacian['VX_DATA'],input['VX_DATA'], output['VX_DATA'])

    node['VX_DATA'] = laplacianNode
    graph['VX_DATA'].node.append(node)
    return node


