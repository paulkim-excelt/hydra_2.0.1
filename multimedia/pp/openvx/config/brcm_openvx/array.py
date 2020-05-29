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
import logging
import numbers
import defn


class VXArray():
    def __init__(self, context_graph, type, maxCount, offset=0,storage = None):
        logger = logging.getLogger(__name__)
        if (type != brcm_openvx.VX_TYPE_RECTANGLE) and (type != brcm_openvx.VX_TYPE_KEYPOINT):
            logger.error('vxCreateArray: VXArray type is wrong %s' %(type))
            raise TypeError
        if (maxCount < 1):
            logger.warning('vxCreateArray: max Count for array is zero')
            raise ValueError
        self.type = type
        self.maxCount = maxCount;
        self.offset = offset
        self.storage = None
        self.data_type = 'uint8_t'
        if storage == None:
            if type == brcm_openvx.VX_TYPE_RECTANGLE:
                self.storage = context_graph.allocateStorage(16 * maxCount)
                self.data_type = 'OpenVXLib_Rectangle'
            if type == brcm_openvx.VX_TYPE_KEYPOINT:
                self.storage = context_graph.allocateStorage(28 * maxCount)
                self.data_type = 'OpenVXLib_KeyPoint'
        else:
            self.storage = storage
    def verify(self):
        if self.storage == None:
            return False
        else:
            return True
    def getDetails(self):
        (name,size, offset) = self.storage.getDetails()
        return (self.storage,self.type,self.maxCount,self.offset)
    def getStorage(self):
        return self.storage
    def getStorageDetails(self):
        return self.storage.getDetails()
    def getMaxCount(self):
        return self.maxCount
    def getDataType(self):
        return (self.data_type, self.maxCount)
    def setName(self,name):
        pass

def vxCreateArray_int(context, item_type, capacity):
    logger = logging.getLogger(__name__)
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreateArray: Context cannot be verified')
        raise AttributeError
    if ((isinstance(item_type,numbers.Number)==False) or (isinstance(capacity,numbers.Number)==False)):
        logger.error('vxCreateArray: item type/capacity is not integer type')
        raise TypeError
    lArray = VXArray(context['VX_DATA'],item_type,capacity)
    if lArray.verify() is not True:
        logger.error('vxCreateArray: VXArray cannot be created')
        raise AttributeError

    arr = defn.create_dict(defn.VX_ARRAY,context)
    arr['VX_DATA'] = lArray
    context['VX_ARRAY'].append(arr)
    return arr


def vxReleaseArray(arr):
    arr['VX_DATA'] = None



class VXRectangle():
    def __init__(self, start_x=0, start_y=0, end_x=0,end_y=0):
        logger = logging.getLogger(__name__)
        if ((start_x < 0) or (start_y < 0) or (end_x < 0) or (end_y < 0)):
            logger.error('VXRectangle: rect values are not proper')
            raise ValueError
        if ((start_x > end_x) or (start_y > end_y)):
            logger.error('VXRectangle: rect values are not proper')
            raise ValueError
        self.start_y =start_y
        self.start_x =start_x
        self.end_x = end_x
        self.end_y = end_y

    def setName(self,name):
        pass



def vxCreateRectangle_int(context, start_x, start_y, end_x,end_y ):
    logger = logging.getLogger(__name__)
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreateRectangle: Context cannot be verified')
        raise AttributeError
    if ((isinstance(start_x,numbers.Number)==False) or (isinstance(start_y,numbers.Number)==False) or (isinstance(end_x,numbers.Number)==False) or (isinstance(end_y,numbers.Number)==False)):
        logger.error('vxCreateRectangle: rect values are not int')
        raise TypeError
    rect = defn.create_dict(defn.VX_RECT,context)
    rect['VX_DATA'] = VXRectangle(start_x, start_y, end_x,end_y)
    context['VX_RECT'].append(rect)
    return rect


