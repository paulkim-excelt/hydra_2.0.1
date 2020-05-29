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

class VXScalar():
    def __init__(self, context_graph, type, value, scalarNum=0,storage = None):
        logger = logging.getLogger(__name__)
        if ((type < brcm_openvx.VX_TYPE_CHAR) or (type > brcm_openvx.VX_TYPE_SCALAR_MAX)):
            logger.error('invalid type for vxScalar')
            raise ValueError
        self.type = type - brcm_openvx.VX_TYPE_START
        self.value = value
        self.offset = scalarNum
        if storage == None:
            self.storage = context_graph.allocateStorage(8)
        else:
            self.storage = storage
    def getDetails(self):
        (name, size, offset) = self.storage.getDetails()
        return (name,self.type,self.offset,size,self.value)
    def getStorageDetails(self):
        return self.storage.getDetails()
    def getDataType(self):
        return defn.VX_SCALAR_STRINGS[self.type]
    def setName(self,name):
        pass

def vxCreateScalar_int(context, data_type, initial_value):
    logger = logging.getLogger(__name__)
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxCreateScalar: context cannot be verified')
        raise AttributeError
    if (isinstance(initial_value,numbers.Number) == False):
        logger.error('vxCreateScalar: initial_value is not a number')
        raise TypeError
    sclr = defn.create_dict(defn.VX_SCALAR,context)
    sclr['VX_DATA'] = VXScalar(context['VX_DATA'],data_type,initial_value,len(context['VX_SCALAR']))
    context['VX_SCALAR'].append(sclr)
    return sclr


def vxReleaseScalar(scalar):
    scalar['VX_DATA'] = None
