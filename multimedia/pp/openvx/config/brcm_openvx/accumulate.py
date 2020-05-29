
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
import image
import pyramid
import utils
import arith
import bitdepth
import graph
import logging
import defn

class VXAccumulate(graph.VXNode):
    def __init__(self, input, tmp, accum):
        logger = logging.getLogger(__name__)
        ret0 = (input.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (accum.type == brcm_openvx.VX_DF_IMAGE_S16)
        ret = ret0 & ret1
        if ret is not True:
            logger.error('vxAccumulateImageNode: input and output type constraints are not met ret0=%s ret1 = %s' %(ret0,ret1))
            raise TypeError
        if ((input.width != accum.width) or (input.height != accum.height)):
            logger.error('vxAccumulateImageNode: input and output resolutions are not same inpW = %s outW = %s inH = %s outH = %s'
                %(input.width, accum.width, input.height, accum.height))
            raise ValueError
        if ((input.height < 2) or (input.width % 8 != 0)):
            logger.error('vxAccumulateImageNode:width/height constraints are not met inpH = %s inpW=%s'%(input.height,input.width))
            raise ValueError
        graph.VXNode.__init__(self)
        self.btDpthConvNode = bitdepth.VXConvertDepthNode(input,tmp)
        self.addNode = arith.VXArithV(tmp,accum,accum,arith.OPCODE_ARITH_ADD,0)
        self.setNumJobs(self.btDpthConvNode.getNumJobs() + self.addNode.getNumJobs())
    def createMailBox(self,total_cores):
        msg = []
        msg = msg + self.btDpthConvNode.createMailBox(total_cores)
        msg = msg + self.addNode.createMailBox(total_cores)
        return msg


def vxAccumulateImageNode_int(graph, input, accum):
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyImage(input)
    ret2 = defn.verifyImage(accum)
    if (ret0 & ret1 & ret2) is False:
        logger.error('vxAccumulateImageNode:one or all parameters are wrong ret0 = %s, ret1= %s,ret2 = %s' %(ret0,ret1,ret2))
        raise AttributeError
    tmpImg =image.vxCreateVirtualImage_int(graph,accum['VX_DATA'].width,
                                    accum['VX_DATA'].height,accum['VX_DATA'].type)
    for img in tmpImg:
        if img is None:
            logger.error('vxAccumulateImageNode:tmpImg fails')
            raise AttributeError
    accumNode = VXAccumulate(input['VX_DATA'], tmpImg['VX_DATA'],accum['VX_DATA'])
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = accumNode
    graph['VX_DATA'].node.append(node)
    return node

