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

import brcm_openvx
import struct
import defn
import logging

POST_PROC_CMD_OFFSET = 32768

CMD_WRITE_IMAGE = 1
CMD_WRITE_SCALAR = 2
CMD_WRITE_TABLE = 3
CMD_WRITE_HOG_TABLE = 4
CMD_WRITE_HOG_SYNC = 5
CMD_CALC_OPTICALFLOW_PREPROC = 6
CMD_WRITE_HAAR_TABLE = 7

CMD_READ_SCALAR = POST_PROC_CMD_OFFSET + CMD_WRITE_SCALAR
CMD_READ_AVG = CMD_READ_SCALAR + 1
CMD_READ_STDDEV = CMD_READ_AVG + 1
CMD_WRITE_HOG_RECT = CMD_READ_STDDEV + 1
CMD_CALC_HARRIS_CORNER = CMD_WRITE_HOG_RECT + 1
CMD_CALC_OPTICALFLOW_POSTPROC_TRACK = CMD_CALC_HARRIS_CORNER + 1
CMD_CALC_OPTICALFLOW_POSTPROC_DETECT = CMD_CALC_OPTICALFLOW_POSTPROC_TRACK + 1
CMD_WRITE_HAAR_RECT = CMD_CALC_OPTICALFLOW_POSTPROC_DETECT+1


LEN_WRITE_IMAGE = 8
LEN_WRITE_SCALAR = 8

LEN_READ_AVG = 12
LEN_READ_STDDEV = 12

WRITE_TYPE_32BE = 1
WRITE_TYPE_32LE = 2
WRITE_TYPE_16BE = 3
WRITE_TYPE_16LE = 4
WRITE_TYPE_8 = 5

MAX_SUBTASK_LENGTH = 16
class VXCommand():
    def __init__(self):
        self.internalMsg = []
        self.jobIdx = 0 #this should be incremented for every job and written to job struct
        self.numPreProc = 0
        self.numPostProc = 0
        self.maxSubTasks = 8
        self.internalMsg.append(struct.pack('>H',0xAAAA))
    def appendMboxS8(self, value):
        self.internalMsg.append(struct.pack('b',value))
    def appendMbox8(self, value):
        self.internalMsg.append(struct.pack('B',value))
    def appendMbox32be(self, value):
        self.internalMsg.append(struct.pack('>I',value))
    def appendMbox32le(self, value):
        self.internalMsg.append(struct.pack('I',value))
    def appendMbox16be(self, value):
        self.internalMsg.append(struct.pack('>H',value))
    def appendMbox16le(self, value):
        self.internalMsg.append(struct.pack('H',value))
    def getData(self):
        return self.internalMsg
    def padCmdData(self,numBytes):
        for i in range(MAX_SUBTASK_LENGTH - numBytes):
            self.appendMbox8(0)
    def appendScalar(self, scalarID, scalarOffset, mBoxOffset, scalarWriteType, scalarReadType):
        self.appendMbox16le(CMD_WRITE_SCALAR)
        self.appendMbox8(scalarID)
        self.appendMbox8(mBoxOffset)
        self.appendMbox16le(scalarWriteType)
        self.appendMbox16le(scalarReadType)
        self.padCmdData(LEN_WRITE_SCALAR)
        self.numPreProc = self.numPreProc + 1
    def appendStorage(self, storage, offset, mBoxOffset):
        self.appendMbox16le(CMD_WRITE_IMAGE)
        (sName, sSize, sOffset) = storage.getDetails()
        self.appendMbox8(sName)
        self.appendMbox8(mBoxOffset)
        self.appendMbox32le(offset + sOffset)
        self.padCmdData(LEN_WRITE_IMAGE)
        self.numPreProc = self.numPreProc + 1
    def appendTable(self, tableID, tableOffset, mBoxOffset):
        self.appendMbox16le(CMD_WRITE_TABLE)
        self.appendMbox8(tableID)
        self.appendMbox8(mBoxOffset)
        self.appendMbox32le(tableOffset)
        self.padCmdData(8)
        self.numPreProc = self.numPreProc + 1
    def appendHogLut(self, sqrtOffset, svmOffset):
        self.appendMbox16le(CMD_WRITE_HOG_TABLE)
        self.appendMbox8(sqrtOffset)
        self.appendMbox8(svmOffset)
        self.padCmdData(4)
        self.numPreProc = self.numPreProc + 1
    def appendHaarFrontFaceTable(self, haarCscdOffset):
        self.appendMbox16le(CMD_WRITE_HAAR_TABLE)
        self.appendMbox8(haarCscdOffset)
        self.padCmdData(3)
        self.numPreProc = self.numPreProc + 1
    def appendHogSync(self, storage, mBoxOffset, width):
        self.appendMbox16le(CMD_WRITE_HOG_SYNC)
        (sName, sSize, sOffset) = storage.getDetails()
        self.appendMbox8(sName)
        self.appendMbox8(mBoxOffset)
        self.appendMbox16le(width)
        self.appendMbox32le(sOffset)
        self.padCmdData(10)
        self.numPreProc = self.numPreProc + 1
    def appendHogRect(self, rectangles, result, offset, scale, num_rect):
        self.appendMbox16le(CMD_WRITE_HOG_RECT)
        (rectName, rectSize, rectOffset) = rectangles.getDetails()
        (rName, rSize, rOffset) = result.getDetails()
        (name,type, numRectoffset,size,value) = num_rect.getDetails()
        self.appendMbox8(rectName)
        self.appendMbox8(rName)
        self.appendMbox8(name)
        self.appendMbox8(type)
        self.appendMbox16le(scale)
        self.appendMbox32le(rOffset + offset)
        self.appendMbox32le(numRectoffset)
        self.padCmdData(16)
        self.numPostProc = self.numPostProc + 1
    def appendHaarRect(self, rectangles, result, offset, scale, num_rect,stPxl):
        self.appendMbox16le(CMD_WRITE_HAAR_RECT)
        (rectName, rectSize, rectOffset) = rectangles.getDetails()
        (rName, rSize, rOffset) = result.getDetails()
        (name,type, numRectoffset,size,value) = num_rect.getDetails()
        self.appendMbox8(rectName)
        self.appendMbox8(rName)
        self.appendMbox8(name)
        self.appendMbox8(type)
        self.appendMbox16le(scale)
        self.appendMbox32le(rOffset + offset)
        self.appendMbox32le(stPxl)
        self.padCmdData(16)
        self.numPostProc = self.numPostProc + 1
    def appendHarrisCorner(self, keypoints, keypoints_offset, pp_corners, pp_corners_offset, step, rows, cols, coreVOffset):
        self.appendMbox16le(CMD_CALC_HARRIS_CORNER)
        (kpName, kpSize, kpOffset) = keypoints.getDetails()
        (ppName, ppSize, ppOffset) = pp_corners.getDetails()
        self.appendMbox8(kpName)
        self.appendMbox8(step)
        self.appendMbox8(rows)
        self.appendMbox8(cols)
        self.appendMbox16le(coreVOffset)
        self.appendMbox32le(kpOffset + keypoints_offset)
        self.appendMbox32le(ppOffset + pp_corners_offset)
        self.padCmdData(16)
        self.numPostProc = self.numPostProc + 1
    def appendOptFlowPreProc(self, keyStore, otherKeyStore, keyOffset_ref, tmpStore, tmpOffset_ref, numPoints):
        self.appendMbox16le(CMD_CALC_OPTICALFLOW_PREPROC)
        (keyName, keySize, keyOffset) = keyStore.getDetails()
        (OtherKeyName, OtherKeySize, OtherKeyOffset) = otherKeyStore.getDetails()
        (tmpName, tmpSize, tmpOffset) = tmpStore.getDetails()
        self.appendMbox8(keyName)
        self.appendMbox8(OtherKeyName)
        self.appendMbox32le(keyOffset + keyOffset_ref)
        self.appendMbox32le(tmpOffset + tmpOffset_ref)
        self.appendMbox32le(numPoints)
        self.padCmdData(16)
        self.numPreProc = self.numPreProc + 1
    def appendOptFlowPostProc(self, keyStore, otherKeyStore, keyOffset_ref, tmpStore, tmpOffset_ref, numPoints):
        self.appendMbox16le(CMD_CALC_OPTICALFLOW_POSTPROC_TRACK)
        (keyName, keySize, keyOffset) = keyStore.getDetails()
        (OtherKeyName, OtherKeySize, OtherKeyOffset) = otherKeyStore.getDetails()
        (tmpName, tmpSize, tmpOffset) = tmpStore.getDetails()
        self.appendMbox8(keyName)
        self.appendMbox8(OtherKeyName)
        self.appendMbox32le(keyOffset + keyOffset_ref)
        self.appendMbox32le(tmpOffset + tmpOffset_ref)
        self.appendMbox32le(numPoints)
        self.padCmdData(16)
        self.numPostProc = self.numPostProc + 1
    def appendOptFlowAddPoints(self, trackStore, detectStore, count):
        self.appendMbox16le(CMD_CALC_OPTICALFLOW_POSTPROC_DETECT)
        (trackName, trackSize, trackOffset) = trackStore.getDetails()
        (detectName, detectSize, detectOffset) = detectStore.getDetails()
        self.appendMbox8(trackName)
        self.appendMbox8(detectName)
        self.appendMbox32le(trackOffset)
        self.appendMbox32le(detectOffset)
        self.appendMbox32le(count)
        self.padCmdData(16)
        self.numPostProc = self.numPostProc + 1

    def readAvg(self, inName,inOffset,outName,size):
        self.appendMbox16le(CMD_READ_AVG)
        self.appendMbox8(inName)
        self.appendMbox8(outName)
        self.appendMbox32le(inOffset)
        self.appendMbox32le(size)
        self.padCmdData(LEN_READ_AVG)
        self.numPostProc = self.numPostProc + 1
    def readStdDev(self, inName,inOffset,outName,size):
        self.appendMbox16le(CMD_READ_STDDEV)
        self.appendMbox8(inName)
        self.appendMbox8(outName)
        self.appendMbox32le(inOffset)
        self.appendMbox32le(size)
        self.padCmdData(LEN_READ_STDDEV)
        self.numPostProc = self.numPostProc + 1
    def appendMsgType(self, msgType):
        for i in range(self.maxSubTasks - (self.numPreProc + self.numPostProc)):
            self.padCmdData(0)
        self.appendMbox8(self.numPreProc)
        self.appendMbox8(self.numPostProc)
        self.appendMbox32le(msgType)
    def appendDummyMsg(self):
        self.appendMsgType(0xFFFFFFFF)
        for i in range(14):
            self.appendMbox32be(0)

class VXNode():
    def __init__(self):
        self.numJobs = 0
    def setNumJobs(self, nJobs):
        self.numJobs = nJobs
    def getNumJobs(self):
        return self.numJobs
    def createMailBox(self, numCores):
        raise Exception('Invalid Node')
        msg = []
        return msg
    def getTableData(self):
        tbl = []
        return tbl

class VXGraph():
    def __init__(self):
        self.node = []
        self.temp = []
        self.tempSize = 0
        self.tableSize = 0
    def allocateStorage(self, size):
        storage = brcm_openvx.context.VXStorage(0, size, self.tempSize)
        self.tempSize = self.tempSize + size
        return storage
    def getTotalTempSize(self):
        return self.tempSize
    def allocateTable(self, size):
        offset = self.tableSize
        self.tableSize += size
        return (0, offset)
    def getTotalTableSize(self):
        return self.tempSize
    def setName(self,name):
        pass


def vxCreateGraph_int(context):
    logger = logging.getLogger(__name__)
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreateGraph: verifyContext failed')
        return None
    grph = defn.create_dict(defn.VX_GRAPH,context)
    grph['VX_DATA'] = VXGraph()
    grph['VX_IMAGE'] = []
    grph['VX_SCALAR'] = []
    grph['VX_ARRAY'] = []
    grph['VX_VIRT'] = []
    context['VX_GRAPH'].append(grph)
    return grph


def vxReleaseNode(node):
    #verify node
    node['VX_DATA'] = None

def vxReleaseGraph(graph):
    #verify graph
    graph['VX_IMAGE'] = []
    graph['VX_SCALAR'] = []
    graph['VX_ARRAY'] = []
    graph['VX_VIRT'] = []
    graph['VX_DATA'].temp = []
    graph['VX_DATA'] = None

def vxVerifyGraph_int(graph):
    logger = logging.getLogger(__name__)
    ret = defn.verifyGraph(graph)
    if ret is not True:
        logger.error('vxVerifyGraph: Cannot be verified')
        raise AttributeError
    return brcm_openvx.VX_SUCCESS

