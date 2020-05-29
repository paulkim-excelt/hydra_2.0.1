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


import brcm_openvx.__init__
from random import randint
import logging



VX_SCALAR_STRINGS = [
'NULL',
'char_t',
'int8_t',
'uint8_t',
'int16_t',
'uint16_t',
'int32_t',
'uint32_t',
'int64_t',
'uint64_t',
'float',
'double',
'NULL',
'uint32_t',
'NULL',
'uint8_t',
'NULL',
'NULL',
'NULL',
'NULL',
'NULL',
]

#scalar read identifiers
VX_NULL   = 0
VX_INT8   = 1
VX_UINT8  = 2
VX_INT16  = 3
VX_UINT16 = 4
VX_INT32  = 5
VX_UINT32 = 6
VX_INT64  = 7
VX_UINT64 = 8




VX_SCALAR_TYPES = [
VX_NULL,        #
VX_INT8,
VX_INT8,
VX_UINT8,
VX_INT16,
VX_UINT16,
VX_INT32,
VX_UINT32,
VX_INT64,
VX_UINT64,
VX_INT32,
VX_INT64,
VX_NULL,
VX_UINT32,
VX_NULL,
VX_UINT8,
VX_NULL,
VX_NULL,
VX_NULL,
VX_NULL,
VX_NULL
]


VX_IMAGE_BIT_WIDTH = {
brcm_openvx.__init__.VX_DF_IMAGE_VIRT:0,
brcm_openvx.__init__.VX_DF_IMAGE_RGB:24,
brcm_openvx.__init__.VX_DF_IMAGE_RGBX:32,
brcm_openvx.__init__.VX_DF_IMAGE_NV12:12,
brcm_openvx.__init__.VX_DF_IMAGE_NV21:12,
brcm_openvx.__init__.VX_DF_IMAGE_UYVY:16,
brcm_openvx.__init__.VX_DF_IMAGE_YUYV:16,
brcm_openvx.__init__.VX_DF_IMAGE_IYUV:12,
brcm_openvx.__init__.VX_DF_IMAGE_YUV4:24,
brcm_openvx.__init__.VX_DF_IMAGE_U8:8,
brcm_openvx.__init__.VX_DF_IMAGE_U16:16,
brcm_openvx.__init__.VX_DF_IMAGE_S16:16,
brcm_openvx.__init__.VX_DF_IMAGE_U32:32,
brcm_openvx.__init__.VX_DF_IMAGE_S32:32,
brcm_openvx.__init__.VX_DF_IMAGE_S32C2:64,
brcm_openvx.__init__.VX_DF_IMAGE_S16C2:32,
brcm_openvx.__init__.VX_DF_IMAGE_U64:64,
brcm_openvx.__init__.VX_DF_IMAGE_BGR_EXT:24,
brcm_openvx.__init__.VX_DF_IMAGE_BGRX_EXT:32,
brcm_openvx.__init__.VX_DF_IMAGE_GRAY_EXT:8,
brcm_openvx.__init__.VX_DF_IMAGE_HSV_EXT:24
}

VX_IMAGE_NUM_CHN = {
brcm_openvx.__init__.VX_DF_IMAGE_VIRT:0,
brcm_openvx.__init__.VX_DF_IMAGE_RGB:3,
brcm_openvx.__init__.VX_DF_IMAGE_RGBX:4,
brcm_openvx.__init__.VX_DF_IMAGE_NV12:2,
brcm_openvx.__init__.VX_DF_IMAGE_NV21:2,
brcm_openvx.__init__.VX_DF_IMAGE_UYVY:2,
brcm_openvx.__init__.VX_DF_IMAGE_YUYV:2,
brcm_openvx.__init__.VX_DF_IMAGE_IYUV:3,
brcm_openvx.__init__.VX_DF_IMAGE_YUV4:3,
brcm_openvx.__init__.VX_DF_IMAGE_U8:1,
brcm_openvx.__init__.VX_DF_IMAGE_U16:1,
brcm_openvx.__init__.VX_DF_IMAGE_S16:1,
brcm_openvx.__init__.VX_DF_IMAGE_U32:1,
brcm_openvx.__init__.VX_DF_IMAGE_S32:1,
brcm_openvx.__init__.VX_DF_IMAGE_S32C2:1,
brcm_openvx.__init__.VX_DF_IMAGE_S16C2:1,
brcm_openvx.__init__.VX_DF_IMAGE_U64:1,
brcm_openvx.__init__.VX_DF_IMAGE_BGR_EXT:3,
brcm_openvx.__init__.VX_DF_IMAGE_BGRX_EXT:4,
brcm_openvx.__init__.VX_DF_IMAGE_GRAY_EXT:1,
brcm_openvx.__init__.VX_DF_IMAGE_HSV_EXT:3
}
#dictionaries
VX_NONE         = -1
VX_CONTEXT      = 0
VX_GRAPH        = 1
VX_IMAGE        = 2
VX_TEMP         = 3
VX_PYRAMID      = 4
VX_NODE         = 5
VX_THRESHOLD    = 6
VX_DISTRIBUTION = 7
VX_SCALAR       = 8
VX_ARRAY        = 9
VX_VIRT         = 10
VX_RECT         = 11
VX_MAX          = VX_RECT

VX_ID_STRINGS = [
'VX_CONTEXT',
'VX_GRAPH',
'VX_IMAGE',
'VX_TEMP',
'VX_PYRAMID',
'VX_NODE',
'VX_THRESHOLD',
'VX_DISTRIBUTION',
'VX_SCALAR',
'VX_ARRAY',
'VX_VIRT',
'VX_RECT'
];


def create_dict(id,parent):
    d = None
    if id <= VX_MAX:
        d = {}
        d['VX_ID'] = id
        d['VX_PARENT'] = parent
        d['VX_DATA'] = None
        d['VX_NAME'] = VX_ID_STRINGS[id] + '_'+ str(randint(1000, 9999))
    return d

def verifyLocal(value,id):
    logger = logging.getLogger(__name__)
    ret = False
    if isinstance(value, dict):
        if 'VX_ID' in value:
            if value['VX_ID'] is id:
                ret = True
    if ret is False:
        logger.debug('verifyLocal: fails')
    return ret

def verifyContext(context):
    return verifyLocal(context,VX_CONTEXT)


def verifyImage(image):
    logger = logging.getLogger(__name__)
    ret1 = verifyLocal(image,VX_IMAGE)
    ret2 = verifyLocal(image,VX_TEMP)
    ret3 = verifyLocal(image,VX_VIRT)
    ret = ret1 | ret2 | ret3
    if ret is False:
        logger.debug('verifyImage: fails')
    return ret

def verifyPyramid(pyramid):
    return verifyLocal(pyramid,VX_PYRAMID)

def verifyThreshold(threshold):
    return verifyLocal(threshold,VX_THRESHOLD)

def verifyDistribution(distribution):
    return verifyLocal(distribution,VX_DISTRIBUTION)

def verifyScalar(scalar):
    return verifyLocal(scalar,VX_SCALAR)

def verifyArray(array):
    return verifyLocal(array,VX_ARRAY)

def verifyRect(rect):
    return verifyLocal(rect,VX_RECT)

def verifyGraph(graph):
    return verifyLocal(graph,VX_GRAPH)



def getDataType(image):
    dtype = None
    ret = verifyLocal(image,VX_IMAGE)
    if ret is True:
        dtype = image['VX_DATA'].type
    ret = verifyLocal(image,VX_TEMP)
    if ret is True:
        bits = VX_IMAGE_BIT_WIDTH[image['VX_DATA'].type]
        if bits != 0:
            dtype = image['VX_DATA'].type
    return dtype

def updateDataType(image,dtype):

    ret = verifyLocal(image,VX_TEMP)
    if ret is True and dtype is not None:
        bits = VX_IMAGE_BIT_WIDTH[image['VX_DATA'].type]
        if bits < VX_IMAGE_BIT_WIDTH[dtype]:
            image['VX_DATA'].type = dtype

def vxSetName_int(obj, name):
    logger = logging.getLogger(__name__)
    if isinstance(obj, dict):
        if 'VX_ID' in obj:
            if (obj['VX_ID'] <= VX_MAX) and (obj['VX_ID'] > VX_NONE):
                if 'VX_NAME' in obj:
                    obj['VX_NAME'] = name
                if 'VX_DATA' in obj:
                    if obj['VX_DATA'] != None:
                        obj['VX_DATA'].setName(name)
                    else:
                        logger.error('vxSetName: obj[Data] is None')
                        raise AttributeError
                else:
                    logger.error('vxSetName: dictionary doesnt have Data')
                    raise AttributeError
            else:
                logger.error('vxSetName: obj[ID] is wrong')
                raise AttributeError
        else:
            logger.error('vxSetName: obj[ID] is not available')
            raise AttributeError
    else:
        logger.error('vxSetName: obj doesnt have dictionary')
        raise AttributeError

