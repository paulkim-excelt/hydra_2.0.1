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
import scalar
import logging
import numbers
import defn
import context

OPCODE_ARITH_ADD = 0
OPCODE_ARITH_SUB = 1
OPCODE_ARITH_AND = 2
OPCODE_ARITH_OR = 3
OPCODE_ARITH_XOR = 4
OPCODE_ARITH_MIN = 5
OPCODE_ARITH_MAX = 6
OPCODE_ARITH_SET = 7
OPCODE_ARITH_CMP = 8
OPCODE_ARITH_MUL = 9
OPCODE_ARITH_ABSDIFF = 10
OPCODE_ARITH_SCALEADD = 11
OPCODE_ARITH_SUBRS = 12
OPCODE_ARITH_COPY = 13

OPCODE_STAT_SUM = 0
OPCODE_STAT_SUMMEAN = 1
OPCODE_STAT_SUM16 = 2
OPCODE_STAT_SUMMEAN16 = 3

OPCODE_ARITH = {
OPCODE_ARITH_ADD:'ARITH_ADD',
OPCODE_ARITH_SUB:'ARITH_SUB',
OPCODE_ARITH_AND:'ARITH_AND',
OPCODE_ARITH_OR:'ARITH_OR',
OPCODE_ARITH_XOR:'ARITH_XOR',
OPCODE_ARITH_MIN:'ARITH_MIN',
OPCODE_ARITH_MAX:'ARITH_MAX',
OPCODE_ARITH_SET:'ARITH_SET',
OPCODE_ARITH_CMP:'ARITH_CMP',
OPCODE_ARITH_MUL:'ARITH_MUL',
OPCODE_ARITH_ABSDIFF:'ARITH_ABSDIFF',
OPCODE_ARITH_SCALEADD:'ARITH_SCALEADD',
OPCODE_ARITH_SUBRS:'ARITH_SUBRS',
OPCODE_ARITH_COPY:'ARITH_COPY'
}


class VXArith(graph.VXNode):
    def __init__(self, in1, in2, out, opcode, extParam,scalarPresent=False):
        logger = logging.getLogger(__name__)
        if (opcode == OPCODE_ARITH_COPY):
            if ((in1.type !=brcm_openvx.VX_DF_IMAGE_U8 ) or (in1.type != out.type)):
                logger.error('%s: input and output type constraints are not met'%(OPCODE_ARITH[opcode]))
                raise TypeError


        ret0 = (in1.type ==brcm_openvx.VX_DF_IMAGE_U8)
        if scalarPresent is False:
            ret1 = (in2.type ==brcm_openvx.VX_DF_IMAGE_U8)
        else:
            ret1 = True

        ret2 = (out.type ==brcm_openvx.VX_DF_IMAGE_U8)
        retU8 = ret0 & ret1 & ret2

        ret0 = (in1.type ==brcm_openvx.VX_DF_IMAGE_S16)
        if scalarPresent is False:
            ret1 = (in2.type ==brcm_openvx.VX_DF_IMAGE_S16)
        else:
            ret1 = True

        if (opcode != OPCODE_ARITH_CMP):
            ret2 = (out.type ==brcm_openvx.VX_DF_IMAGE_S16)
        retS16 = ret0 & ret1 & ret2

        ret = (retU8 | retS16)
        if ret is not True:
            logger.error('%s: input and output type constraints are not met'%(OPCODE_ARITH[opcode]))
            raise TypeError
        if (in1.width %8 !=0) or (in1.height < 2):
            logger.error('%s: input resolution constraints are not met'%(OPCODE_ARITH[opcode]))
            raise ValueError
        if ((scalarPresent is False) and ((in1.width != in2.width) or (in1.height != in2.height))):
            logger.error('%s: input resolutions are not same '%(OPCODE_ARITH[opcode]))
            raise ValueError
        if ((in1.width != out.width ) or (in1.height != out.height)):
            logger.error('%s: input and output resolutions are not same'%(OPCODE_ARITH[opcode]))
            raise ValueError
        graph.VXNode.__init__(self)
        if (in1.type == brcm_openvx.VX_DF_IMAGE_U8):
            self.bitMode =  0
        else:
            self.bitMode = 1
        self.in1 = in1
        self.in2 = in2
        self.out = out
        self.setNumJobs(1)
        self.opcode = opcode
        self.extParam = extParam

    def createMailBoxLocal(self,totalCoreCount,scalarType,scalarObj=None):
        msg = []
        (outStore, outW, outH, outS) = self.out.getDetails()
        sliceHeightRound = int(outH)/int(totalCoreCount)
        for coreNum in range(totalCoreCount):
            sliceHeight = sliceHeightRound
            if coreNum == totalCoreCount - 1:
                sliceHeight = sliceHeight + (int(outH) % int(totalCoreCount))

            cmd = graph.VXCommand()
            offset = coreNum*int(sliceHeightRound*outS)
            cmd.appendStorage(outStore, offset, 28)

            inImage1Step = 0
            inImage2Step = 0
            #This checks are maintained in sync with vendor delivery
            if self.in1 is not None:
                (storage, w, h, s) = self.in1.getDetails()
                inImage1Step = s
                offset = (coreNum*int(sliceHeightRound*s))
                cmd.appendStorage(storage, offset, 0)
            if self.in2 is not None:
                (storage, w, h, s) = self.in2.getDetails()
                inImage2Step = s
                offset  = (coreNum*int(sliceHeightRound*s))
                cmd.appendStorage(storage, offset, 4)
            scalarValue = 0
            if scalarObj is not None:
                if isinstance(scalarObj,scalar.VXScalar):
                    (scalarName,scalarType,scalarOffset,scalarSize,scalarValue) = scalarObj.getDetails()
                    cmd.appendScalar(scalarName, scalarOffset, 40, graph.WRITE_TYPE_32BE, defn.VX_SCALAR_TYPES[scalarType])
                else:
                    scalarValue = scalarObj
            cmd.appendMsgType(10)
            cmd.appendMbox32le(0) #src1
            cmd.appendMbox32le(0) #src2
            cmd.appendMbox32be(sliceHeight)
            cmd.appendMbox32be(outW * (1 + self.bitMode))
            cmd.appendMbox32be(inImage1Step)
            cmd.appendMbox32be(inImage2Step)
            cmd.appendMbox32be(outS)
            cmd.appendMbox32le(0)
            cmd.appendMbox32be(self.extParam) #extParam
            cmd.appendMbox32be(self.opcode) #opcode
            cmd.appendMbox32be(scalarValue) #scalarVal
            cmd.appendMbox32be(scalarType) #scalarType
            cmd.appendMbox32be(0) #pad[0]
            cmd.appendMbox32be(self.bitMode) #bitMode
            msg = msg + cmd.getData()
        return msg

class VXMeanStdDev(graph.VXNode):
    def __init__(self, input, mean, stddev, graphMean):
        logger = logging.getLogger(__name__)
        graph.VXNode.__init__(self)
        ret0 = (input.type ==brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (mean.type ==brcm_openvx.VX_TYPE_UINT32 - brcm_openvx.VX_TYPE_START)
        ret2 = (stddev.type ==brcm_openvx.VX_TYPE_UINT32 - brcm_openvx.VX_TYPE_START)
        ret = ret0 & ret1 & ret2
        if ret is False:
            logger.error('VXMeanStdDev: input/output type constraints are not met ret0 = %s,    ret1 = %s, ret2 = %s'%(ret0, ret1, ret2))
            raise TypeError
        if (input.width %8 !=0) or (input.height < 2):
            logger.error('VXMeanStdDev: input resolution constraints are not met inW = %s, inHt = %s'%(input.width ,input.height))
            raise ValueError
        self.input = input
        self.mean = mean
        self.stddev = stddev
        self.graph = graphMean
        self.setNumJobs(2) # 0 - mean 1-stddev
    def createMailBoxLocal(self,jobNum):
        (inStorage, inW, inH, inS) = self.input.getDetails()
        tempName = 0
        tempOffset = self.graph.getTotalTempSize()
        tempS = 128 # This is to be in sync with vendor delivery
        (meanName,meanType,meanOffset,meanSize,meanValue) = self.mean.getDetails()

        cmd = graph.VXCommand()
        #write src1
        cmd.appendStorage(inStorage, 0, 0)
        #write dst
        extParam = 0
        if jobNum == 0:
            tempOffset = tempOffset + (meanOffset * 8)
            cmd.appendStorage(context.VXStorage(tempName, 0, tempOffset), 0, 28)
            #mean
            opcode = OPCODE_STAT_SUM
            #post proc mean
            cmd.readAvg(tempName,tempOffset,meanName,inW*inH)
        if jobNum == 1:
            (stddevName,stddevType,stddevOffset,stddevSize,stddevValue) = self.stddev.getDetails()
            tempOffset = tempOffset + (stddevOffset * 8)
            cmd.appendStorage(context.VXStorage(tempName, 0, tempOffset), 0, 28)
            #stddev
            opcode = OPCODE_STAT_SUMMEAN
            #update extParam == pre proc
            cmd.appendScalar(meanName, meanOffset, 32, graph.WRITE_TYPE_32BE, defn.VX_SCALAR_TYPES[meanType])
            #postproc
            cmd.readStdDev(tempName,tempOffset,stddevName,inW*inH)
        cmd.appendMsgType(utils.VENDOR_MSG_STAT)
        cmd.appendMbox32le(0) #src1
        cmd.appendMbox32le(0) #src2
        cmd.appendMbox32be(inH)
        cmd.appendMbox32be(inW)
        cmd.appendMbox32be(inS)
        cmd.appendMbox32be(0)
        cmd.appendMbox32be(tempS)
        cmd.appendMbox32le(tempOffset)
        cmd.appendMbox32be(extParam) #extParam
        cmd.appendMbox32be(opcode) #opcode
        cmd.appendMbox32be(0) #scalarVal
        cmd.appendMbox32be(1) #scalarType
        cmd.appendMbox32be(0) #pad[0]
        cmd.appendMbox32be(0) #pad[1]

        return cmd.getData()
    def createMailBox(self,totalCoreCount):
        msg = []
        for i in range(self.getNumJobs()):
            msg = msg + self.createMailBoxLocal(i)
            for j in range(1,totalCoreCount):
                cmd = graph.VXCommand()
                cmd.appendDummyMsg()
                msg = msg + cmd.getData()
        return msg
class VXArithV(VXArith):
    def __init__(self,in1,in2,out,opcode,extParam):
        VXArith.__init__(self, in1, in2, out, opcode, extParam)
    def createMailBox(self,totalCoreCount):
        return self.createMailBoxLocal(totalCoreCount,0)

class VXArithS(VXArith):
    def __init__(self,in1,in2,out,opcode,extParam,scalarObj):
        VXArith.__init__(self, in1, in2, out, opcode, extParam, scalarPresent=True)
        self.scalarObj = scalarObj
    def createMailBox(self,totalCoreCount):
        return self.createMailBoxLocal(totalCoreCount,1,self.scalarObj)

def vxArithNode(graph, in1, in2, out, opcode,extParam):
    logger = logging.getLogger(__name__)
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = VXArithV(in1['VX_DATA'],in2['VX_DATA'],out['VX_DATA'],opcode,extParam)
    graph['VX_DATA'].node.append(node)
    return node

def vxArithScalarExtNode(graph, in1, scalarObj, out, opcode,extParam):
    logger = logging.getLogger(__name__)
    node = defn.create_dict(defn.VX_NODE,graph)
    if  isinstance(scalarObj,dict):
        ret = False
        if (in1['VX_DATA'].type == brcm_openvx.VX_DF_IMAGE_U8):
            ret = (scalarObj['VX_DATA'].type == brcm_openvx.VX_TYPE_UINT8 - brcm_openvx.VX_TYPE_START)
        elif (in1['VX_DATA'].type == brcm_openvx.VX_DF_IMAGE_S16):
            ret = (scalarObj['VX_DATA'].type == brcm_openvx.VX_TYPE_INT16 - brcm_openvx.VX_TYPE_START)

        if ret is not True:
            logger.error('%s: input and output type constraints are not met'%(OPCODE_ARITH[opcode]))
            raise TypeError
        scalarValue = scalarObj['VX_DATA']
    else:
        scalarValue = scalarObj
    node['VX_DATA'] = VXArithS(in1['VX_DATA'],None,out['VX_DATA'],opcode,extParam,scalarValue)
    graph['VX_DATA'].node.append(node)
    return node

def vxAbsDiffNode_int(graph, in1, in2, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxAbsDiffNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_ABSDIFF, 0)

def vxAddNode_int(graph, in1, in2, policy, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxAddNode: one or all parameters are wrong')
        raise AttributeError
    logger.info('vxAddNode always uses VX_CONVERT_POLICY_SATURATE policy')
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_ADD, 0)

def vxSubtractNode_int(graph, in1, in2, policy, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxSubtractNode: one or all parameters are wrong')
        raise AttributeError
    logger.info('vxSubtractNode always uses VX_CONVERT_POLICY_SATURATE policy')
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_SUB, 0)

def vxAndNode_int(graph, in1, in2, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxAndNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_AND, 0)

def vxOrNode_int(graph, in1, in2, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxOrNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_OR, 0)

def vxXorNode_int(graph, in1, in2, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxXorNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_XOR, 0)

def vxMinExtNode_int(graph, in1, in2, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxMinExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_MIN, 0)

def vxMaxExtNode_int(graph, in1, in2, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxMaxExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_MAX, 0)

def vxMultiplyNode_int(graph, in1, in2, scale, overflow_policy, rounding_policy, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxMultiplyNode: one or all parameters are wrong')
        raise AttributeError
    logger.info('vxMultiplyNode always uses VX_CONVERT_POLICY_SATURATE policy')
    logger.info('vxMultiplyNode does not support any rounding policy')
    logger.info('vxMultiplyNode does not support any scale factor')
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_MUL, scale)

def vxCompareExtNode_int(graph, in1, in2, compare_operation, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxCompareExtNode: one or all parameters are wrong')
        raise AttributeError
    if (isinstance(compare_operation,numbers.Number) == False):
        logger.error('vxCompareExtNode: compare_operation is not a number')
        raise TypeError
    if ((compare_operation < brcm_openvx.VX_CMP_EQ) or (compare_operation > brcm_openvx.VX_CMP_NE)):
        logger.error('vxCompareExtNode: compare operator is wrong')
        raise ValueError
    compare_operation = compare_operation - brcm_openvx.VX_CMP_EQ
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_CMP, compare_operation)

def vxScaleAddExtNode_int(graph, in1, in2, scale, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyImage(in2)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxScaleAddExtNode: one or all parameters are wrong')
        raise AttributeError
    if (isinstance(scale, numbers.Number)==False):
        logger.error('vxScaleAddExtNode: scale is not a number')
        raise TypeError
    scalefactor = int(scale)
    return vxArithNode(graph, in1, in2, out, OPCODE_ARITH_SCALEADD, scalefactor)

def vxAbsDiffScalarExtNode_int(graph, in1, scalarObj, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxAbsDiffScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_ABSDIFF, 0)

def vxAddScalarExtNode_int(graph, in1, scalarObj, policy, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxAddScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    logger.info('vxAddScalarExtNode always uses VX_CONVERT_POLICY_SATURATE policy')
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_ADD, 0)

def vxSubtractScalarExtNode_int(graph, in1, scalarObj, policy, out, reverse):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxSubtractScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    if ((reverse != 0) and (reverse != 1)):
        logger.error('vxSubtractScalarExtNode: reverse should be a bool value 1 or 0')
        raise ValueError
    logger.info('vxSubtractScalarExtNode: bool argument is %s'%(reverse))
    logger.info('vxSubtractScalarExtNode always uses VX_CONVERT_POLICY_SATURATE policy')
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_SUB, reverse)

def vxAndScalarExtNode_int(graph, in1, scalarObj, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxAndScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_AND, 0)

def vxOrScalarExtNode_int(graph, in1, scalarObj, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxOrScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_OR, 0)

def vxXorScalarExtNode_int(graph, in1, scalarObj, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxXorScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_XOR, 0)

def vxMinScalarExtNode_int(graph, in1, scalarObj, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxMinScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_MIN, 0)

def vxMaxScalarExtNode_int(graph, in1, scalarObj, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxMaxScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_MAX, 0)

def vxMultiplyScalarExtNode_int(graph, in1, scalarObj, scale, overflow_policy, rounding_policy, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxMultiplyScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    logger.info('vxMultiplyScalarExtNode always uses VX_CONVERT_POLICY_SATURATE policy')
    logger.info('vxMultiplyScalarExtNode does not support any rounding policy')
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_MUL, scale)

def vxCompareScalarExtNode_int(graph, in1, scalarObj, compare_operation, out):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(in1)
    ret2 = defn.verifyScalar(scalarObj)
    ret3 = defn.verifyImage(out)
    ret = ret0 & ret1 & ret2 & ret3
    if ret is not True:
        logger.error('vxCompareScalarExtNode: one or all parameters are wrong')
        raise AttributeError
    if (isinstance(compare_operation,numbers.Number) == False):
        logger.error('vxCompareScalarExtNode: compare_operation is not a number')
        raise TypeError
    if ((compare_operation < brcm_openvx.VX_CMP_EQ) or (compare_operation > brcm_openvx.VX_CMP_NE)):
        logger.error('vxCompareScalarExtNode: compare operator is wrong')
        raise ValueError
    compare_operation = compare_operation - brcm_openvx.VX_CMP_EQ
    return vxArithScalarExtNode(graph, in1, scalarObj, out, OPCODE_ARITH_CMP, compare_operation)

def vxNotNode_int(graph, input, output):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxNotNode: one or all parameters are wrong')
        raise AttributeError
    if (input['VX_DATA'].type == brcm_openvx.VX_DF_IMAGE_S16) is True:
        scalarObj = 0xFFFF
    else:
        scalarObj = 0xFF
    return vxArithScalarExtNode(graph, input, scalarObj, output, OPCODE_ARITH_XOR, 0)

def vxSetExtNode_int(graph, inout, value):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(inout)
    ret2 = defn.verifyScalar(value)
    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxSetExtNode: parameters are wrong ret0 = %s, ret1= %s, ret2 = %s' %(ret0,ret1, ret2))
        raise AttributeError
    return vxArithScalarExtNode(graph, inout, value, inout, OPCODE_ARITH_SET, 0)

def vxCopyExtNode_int(graph,input,output):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(output)
    ret = ret0 & ret1 & ret2
    if ret is not True:
        logger.error('vxCopyExtNode: one or all parameters are wrong')
        raise AttributeError
    return vxArithScalarExtNode(graph, input, 0, output, OPCODE_ARITH_COPY, 0)

def vxMeanStdDevNode_int(graph, input, mean, stddev):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyScalar(mean)
    ret3 = defn.verifyScalar(stddev)
    ret = ret0 & ret1 & ret2 & ret3

    if ret is False:
        logger.error('vxMeanStdDevNode: one or all parameters are wrong')
        raise AttributeError
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = VXMeanStdDev(input['VX_DATA'],mean['VX_DATA'],stddev['VX_DATA'],graph['VX_DATA'])
    if node['VX_DATA'] is None:
        logger.error('vxMeanStdDevNode: Parameter constraints are not met')
        raise AttributeError
    graph['VX_DATA'].node.append(node)
    return node

