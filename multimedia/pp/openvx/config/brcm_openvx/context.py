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
import graph
import image
import pyramid
import histogram
import threshold
import scalar
import array
import logging
import defn

class VXStorage():
    def __init__(self, name, size, offset, storage = None):
        logger = logging.getLogger(__name__)
        if (size < 0):
            logger.error('validate:Storage size cannot be zero')
            raise MemoryError
        if storage == None:
            self.name = name
            self.size = size
            self.offset = offset
        else:
            self.name = storage.name
            self.size = storage.size
            self.offset = offset
    def getDetails(self):
        return (self.name, self.size, self.offset)
    def getSize(self):
        return self.size

class VXContext():
    def __init__(self):
        self.nextStorageName = 1
    def allocateStorage(self, size):
        logger = logging.getLogger(__name__)
        storage = VXStorage(self.nextStorageName, size, 0)
        self.nextStorageName = self.nextStorageName + 1
        return storage
    def setName(self,name):
        pass
    def getMaxDataEntries(self):
        return self.nextStorageName

def vxCreateContext_int():
    ctxt = defn.create_dict(defn.VX_CONTEXT,None)
    ctxt['VX_DATA'] = VXContext()
    ctxt['VX_GRAPH'] = []
    ctxt['VX_IMAGE'] = []
    ctxt['VX_PYRAMID'] = []
    ctxt['VX_THRESHOLD'] = []
    ctxt['VX_DISTRIBUTION'] = []
    ctxt['VX_SCALAR'] = []
    ctxt['VX_ARRAY'] = []
    ctxt['VX_VIRT'] = []
    ctxt['VX_RECT'] = []
    return ctxt


def vxReleaseContext_int(context):
    logger = logging.getLogger(__name__)
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreateImage: cannot verify context')
        raise AttributeError
    for grph in context['VX_GRAPH']:
        graph.vxReleaseGraph(grph)
    for img in context['VX_IMAGE']:
        image.vxReleaseImage(img)
    for pyrd in context['VX_PYRAMID']:
        pyramid.vxReleasePyramid(pyrd)
    for thresh in context['VX_THRESHOLD']:
        threshold.vxReleaseThreshold_int(thresh)
    for distribution in context['VX_DISTRIBUTION']:
        histogram.vxReleaseDistribution_int(distribution)
    for sclr in context['VX_SCALAR']:
        scalar.vxReleaseScalar(sclr)
    for arr in context['VX_ARRAY']:
        array.vxReleaseArray(arr)
    context['VX_GRAPH'] = []
    context['VX_IMAGE'] = []
    context['VX_PYRAMID'] = []
    context['VX_THRESHOLD'] = []
    context['VX_DISTRIBUTION'] = []
    context['VX_SCALAR'] = []
    context['VX_ARRAY'] = []
    context['VX_VIRT'] = []
    context['VX_RECT'] = []
    context['VX_DATA'] = None
