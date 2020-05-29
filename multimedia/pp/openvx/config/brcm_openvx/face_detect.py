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
import numbers
import image
import struct
import math
import graph
import scale
import pyramid
import utils
import logging
import numbers
import defn

FACEDETECT_MAX_SCALE_LVLS  = 25
FACEDETECT_MAX_FACES       = 2944
FACEDETECT_COL_STEPX_BYTES = 5
FACEDETECT_WINSIZE_X       = 24
MAX_TOTAL_CORES            = 8


class VXFaceDetect(graph.VXNode):
    def __init__(self, graphFaceDetect, image, rectangles, num_rect, scale, minSize):
        logger = logging.getLogger(__name__)
        ret0 = (image.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (rectangles.type == brcm_openvx.VX_TYPE_RECTANGLE)
        ret2 = (num_rect.type == brcm_openvx.VX_TYPE_INT32 - brcm_openvx.VX_TYPE_START)
        ret = ret0 & ret1 &  ret2
        if ret is not True:
            logger.error('VXFaceDetect: input and output type constraints are not met ret0 = %s ret1 = %s ret2 = %s'%(ret0,ret1,ret2))
            raise TypeError
        if (image.width % 8 != 0):
            logger.error('VXFaceDetect: input resolution constraints are not met')
            raise ValueError
        graph.VXNode.__init__(self)
        self.image = image
        self.rect = rectangles
        self.num_rect = num_rect
        self.scale = scale
        self.minSize = minSize
        self.hlfsclDwnNodes = []
        self.scaleNode = []
        self.storage = []
        self.factor = []
        self.resizeImg = []
        self.graph = graphFaceDetect
    def compute(self):
        logger = logging.getLogger(__name__)
        (imStore, width, height, step) = self.image.getDetails()
        self.width = width
        self.height = height
        self.dstOffset = (FACEDETECT_MAX_FACES+8) #8 * FACEDETECT_MAX_FACES/total_cores + 8
        self.results = image.VXImage(self.graph, (self.dstOffset*MAX_TOTAL_CORES),1, brcm_openvx.VX_DF_IMAGE_U8, None)
        ret0 = isinstance(self.scale, numbers.Number)
        ret1 = isinstance(self.minSize, numbers.Number)
        ret = ret0 & ret1;
        if ret is not True:
            logger.error('VXFaceDetect: minSize/Scale is not a valid number')
            raise TypeError
        ret0 = (self.scale > 1.0) and (self.scale < 2.0)
        ret2 = (self.minSize <= self.height)
        ret = ret0 & ret2
        if ret is not True:
            logger.error('VXFaceDetect:compute:scale/minSize fails')
            raise ValueError

        wndwSize = 20
        pwrNfctr = int(math.ceil((math.log10(self.minSize) - math.log10(wndwSize)) / math.log10(self.scale)))
        frstFctr = math.pow(self.scale, pwrNfctr)
        tmpSize = round(min(self.width,self.height)/ frstFctr)
        self.resizeCntr = int(math.ceil((math.log10(tmpSize) - math.log10(wndwSize)) / math.log10(self.scale)))
        if (self.resizeCntr > FACEDETECT_MAX_SCALE_LVLS):
            return False
        self.numSclDwn = int(math.floor(math.log10(frstFctr)/math.log10(2.0)))
        self.chunkCnt = []
        for i in range (self.resizeCntr):
            fctr = frstFctr * math.pow(self.scale,i)
            self.factor.append(fctr)
            szW = round(self.width / fctr)
            szH = round(self.height / fctr)
            chunkLoopCnt = szW - (FACEDETECT_WINSIZE_X - FACEDETECT_COL_STEPX_BYTES);
            chnkCnt = 0
            while (1):
                chunkLoopCnt = chunkLoopCnt - FACEDETECT_COL_STEPX_BYTES;
                if (chunkLoopCnt < 0):
                    break
                chnkCnt = chnkCnt + 1
            #end of while
            self.chunkCnt.append(int(chnkCnt))

            if (self.chunkCnt[i] == 0):
                self.resizeCntr = i
                break;
            img = image.VXImage(self.graph, szW, szH, brcm_openvx.VX_DF_IMAGE_U8, None)
            self.resizeImg.append(img)
        #end for


        if (self.numSclDwn > 0):
            self.storage.append(self.graph.allocateStorage(imStore.getSize()/4))     #tmp for downscale
            self.storage.append(self.graph.allocateStorage(imStore.getSize()/4))
        inp = self.image
        sclW = self.width
        sclH = self.height
        for i in range (self.numSclDwn):
            sclW = sclW/2
            sclH = sclH/2
            out = image.VXImage(self.graph, sclW, sclH, brcm_openvx.VX_DF_IMAGE_U8, self.storage[i&1])
            node = pyramid.VXHalfScaleGaussianNode(inp,out,None,5)
            self.hlfsclDwnNodes.append(node)
            inp = out


        for i in range (self.resizeCntr):
            node = scale.VXScaleNode(inp,self.resizeImg[i],brcm_openvx.VX_INTERPOLATION_BILINEAR)
            self.scaleNode.append(node)
            inp = self.resizeImg[i]
        if (self.chunkCnt[0]==0):
            logger.error('VXFaceDetect:compute:chunkCnt cannot be zero fails')
            raise ValueError
        self.setNumJobs(self.numSclDwn + (2*self.resizeCntr))
        #end of while
        return True
    #end of compute
    def getTableData(self):
        table = []
        for i in range(len(self.scaleNode)):
            for each in self.scaleNode[i].getTableData():
                table.append(each)
        return table
    def createMailBoxLocal(self, total_cores, idx):
        msg = []
        scaleFactor = max(1, min(int((self.factor[idx])*256), 65535))
        (resultStore,resW,resH,resStep) = self.results.getDetails()
        (reSizeStore, width, height, step) = self.resizeImg[idx].getDetails()
        startPixel = [ 0, 0, 0, 0, 0, 0, 0, 0 ]
        chunksPerCore = round(self.chunkCnt[idx]/total_cores);
        if (chunksPerCore is 0):
            chunksPerCore = 1
        if (chunksPerCore != 1):
            self.maxRslts = total_cores
        else:
            self.maxRslts = self.chunkCnt[idx]
        if (self.maxRslts > total_cores):
            self.maxRslts = total_cores
        for cnt in range(self.maxRslts):
            startPixel[cnt] = cnt * chunksPerCore * FACEDETECT_COL_STEPX_BYTES;
            chunkStartOffset = startPixel[cnt]

            if ((cnt + 1) ==  total_cores):
                chunkW = width - chunkStartOffset
            else:
                chunkW = (chunksPerCore - 1) * FACEDETECT_COL_STEPX_BYTES + FACEDETECT_WINSIZE_X
            cmd = graph.VXCommand()

            cmd.appendStorage(resultStore, (self.dstOffset*cnt), 0)
            cmd.appendStorage(reSizeStore, chunkStartOffset, 4)
            cmd.appendHaarFrontFaceTable(24)
            cmd.appendHaarRect(self.rect.getStorage(), resultStore, self.dstOffset*cnt, scaleFactor, self.num_rect, chunkStartOffset )
            cmd.appendMsgType(utils.VENDOR_MSG_HAAR)   #haar

            cmd.appendMbox32le(0)   #dst/points
            cmd.appendMbox32le(0)   #src/img

            cmd.appendMbox32be(height)
            cmd.appendMbox32be(chunkW)
            cmd.appendMbox32be(resStep)
            cmd.appendMbox32be(step)
            cmd.appendMbox32le(0) #haar Tbl
            cmd.appendMbox32le(0) #norm_fctr
            cmd.appendMbox32le(0) #pad[0]
            cmd.appendMbox32le(0) #pad[1]
            cmd.appendMbox32le(0) #pad[2]
            cmd.appendMbox32le(0) #pad[3]
            cmd.appendMbox32le(0) #pad[4]
            cmd.appendMbox32le(0) #pad[5]
            msg = msg + cmd.getData()
        for j in range(self.maxRslts,total_cores,1):
            cmd = VXCommand()
            cmd.appendDummyMsg()
            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,total_cores):
        if (self.chunkCnt[0] == 0):
            return None
        msg = []
        for i in range(self.numSclDwn):
            msg = msg + self.hlfsclDwnNodes[i].createMailBox(total_cores)
        for i in range(self.resizeCntr):
            msg = msg + self.scaleNode[i].createMailBox(total_cores)
            msg = msg + self.createMailBoxLocal(total_cores, i)
        return msg


def vxFaceDetectNode_int(graph, image, rectangles, num_rect, scale, minSize):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(image)
    ret2 = defn.verifyArray(rectangles)
    ret3 = defn.verifyScalar(num_rect)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxFaceDetect: one or all parameters are wrong')
        raise AttributeError
    faceDetect = VXFaceDetect(graph['VX_DATA'], image['VX_DATA'], rectangles['VX_DATA'], num_rect['VX_DATA'], scale, minSize)
    if faceDetect.compute() == False:
        logger.error('vxFaceDetect: compute failed')
        raise ValueError
    for i in range (faceDetect.resizeCntr):
        faceDetect.scaleNode[i].compute(graph['VX_DATA'])
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = faceDetect
    graph['VX_DATA'].node.append(node)
    return node


