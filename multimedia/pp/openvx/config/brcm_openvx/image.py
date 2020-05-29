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
import context
import logging
import numbers
import defn

def getImageStride(width, type):
    min_step = int((defn.VX_IMAGE_BIT_WIDTH[type]/8.)* int(width))
    if ((type == brcm_openvx.VX_DF_IMAGE_IYUV)
    or (type == brcm_openvx.VX_DF_IMAGE_NV12)
    or (type == brcm_openvx.VX_DF_IMAGE_NV21) ):
        min_step = (min_step * 2)/3
    return (min_step + 127) & (-128)

def getImageSize(width, height, type):
    size = getImageStride(width, type) * int(height)
    if ((type == brcm_openvx.VX_DF_IMAGE_IYUV)
    or (type == brcm_openvx.VX_DF_IMAGE_NV12)
    or (type == brcm_openvx.VX_DF_IMAGE_NV21) ):
        size = (size * 3)/2
    return size

class VXImage():
    def __init__(self, context_graph, width, height, type, storage):
        logger = logging.getLogger(__name__)
        if ((width <= 0) or (height <=0) or (type < brcm_openvx.VX_DF_IMAGE_RGB) or (type > brcm_openvx.VX_DF_IMAGE_HSV_EXT)):
            logger.error('VXImage: width/height/type doesnot have a proper value w = %s h= %s type = %s' %(width,height,type))
            raise ValueError
        self.width = int(width)
        self.height = int(height)
        self.type = type
        self.step = getImageStride(width, type)
        if storage == None:
            self.storage = context_graph.allocateStorage(getImageSize(width, height, type))
        else:
            self.storage = storage
    def getDetails(self):
        return (self.storage, self.width, self.height, self.step)
    def getWidth(self):
        return self.width
    def getHeight(self):
        return self.height
    def getStride(self):
        return self.step
    def getStorage(self):
        return self.storage
    def getStorageDetails(self):
        return self.storage.getDetails()
    def setName(self,name):
        pass


def vxCreateImage_int(context, width, height, type):
    logger = logging.getLogger(__name__)
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreateImage: cannot verify context')
        raise AttributeError
    if ((isinstance(width,numbers.Number)==False) or (isinstance(height,numbers.Number)==False)):
        logger.error('vxCreateImage: width and height are not valid')
        raise TypeError
    img = defn.create_dict(defn.VX_IMAGE,context)
    img['VX_DATA'] = VXImage(context['VX_DATA'], width, height, type, None)
    context['VX_IMAGE'].append(img)
    return img


def vxReleaseImage(image):
    #verify image
    logger = logging.getLogger(__name__)
    image['VX_DATA'] = None


def vxCreateVirtualImage_int(graph, width, height, type):
    ret = defn.verifyGraph(graph)
    if ret is not True:
        logger.error('vxCreateVirtualImage: cannot verify graph')
        raise AttributeError
    vert = defn.create_dict(defn.VX_TEMP,graph)
    vert['VX_DATA'] = VXImage(graph['VX_DATA'], width, height, type, None)
    graph['VX_IMAGE'].append(vert)
    return vert

def vxCreateImageFromROI_int(img, rect):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyImage(img)
    ret1 = defn.verifyRect(rect)
    if (ret0 & ret1) is not True:
        logger.error('vxCreateImageFromROI : verification fails')
        raise AttributeError
    ROIimg = defn.create_dict(defn.VX_VIRT,img['VX_PARENT'])
    if ((rect['VX_DATA'].start_x < 0) or (rect['VX_DATA'].end_x > img['VX_DATA'].width)):
        logger.error('vxCreateImageFromROI : rect width params fails')
        raise ValueError
    if ((rect['VX_DATA'].start_y < 0) or (rect['VX_DATA'].end_y > img['VX_DATA'].height)):
        logger.error('vxCreateImageFromROI : rect height params fails')
        raise ValueError

    width = rect['VX_DATA'].end_x - rect['VX_DATA'].start_x + 1
    height = rect['VX_DATA'].end_y - rect['VX_DATA'].start_y + 1
    step = img['VX_DATA'].step
    startX = rect['VX_DATA'].start_x
    startY = rect['VX_DATA'].start_y

    offset = (startY * step) + (startX)
    ROIimg['VX_DATA'] = VXImage(img['VX_PARENT'], width, height,img['VX_DATA'].type, context.VXStorage(0, 0, offset, img['VX_DATA'].storage))
    ROIimg['VX_DATA'].step = img['VX_DATA'].step
    img['VX_PARENT']['VX_VIRT'].append(ROIimg)
    return ROIimg


