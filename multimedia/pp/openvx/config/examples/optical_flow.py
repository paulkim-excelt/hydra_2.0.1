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

context = brcm_openvx.vxCreateContext();
if context is None:
    print 'vxCreateContext Failed'
brcm_openvx.vxSetName(context, 'opticalflow')
image = brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8)
if image is None:
    print 'vxCreateImage Failed'
brcm_openvx.vxSetName(image,'currImage')
corners = brcm_openvx.vxCreateArray(context,brcm_openvx.VX_TYPE_KEYPOINT, 79*44)
brcm_openvx.vxSetName(corners,'Corners')
prevpts = brcm_openvx.vxCreateArray(context,brcm_openvx.VX_TYPE_KEYPOINT, 79*44)
brcm_openvx.vxSetName(prevpts,'PrevPoints')
currpts = brcm_openvx.vxCreateArray(context,brcm_openvx.VX_TYPE_KEYPOINT, 79*44)
brcm_openvx.vxSetName(currpts,'CurrPoints')
pyrPrev = brcm_openvx.vxCreatePyramid(context, 3, brcm_openvx.VX_SCALE_PYRAMID_HALF, 1280, 720, brcm_openvx.VX_DF_IMAGE_U8)
if pyrPrev is None:
    print 'vxCreatePyramid Failed'
brcm_openvx.vxSetName(pyrPrev,'PrevPyramid')
pyrNext = brcm_openvx.vxCreatePyramid(context, 3, brcm_openvx.VX_SCALE_PYRAMID_HALF, 1280, 720, brcm_openvx.VX_DF_IMAGE_U8)
if pyrPrev is None:
    print 'vxCreatePyramid Failed'
brcm_openvx.vxSetName(pyrNext,'CurrPyramid')

graph = brcm_openvx.vxCreateGraph(context);
if graph is None:
    print 'vxCreateGraph Failed'
brcm_openvx.vxSetName(graph,'process')


GauPyrNode = brcm_openvx.vxGaussianPyramidNode(graph, image, pyrNext)
if GauPyrNode is None:
    print 'vxGaussianPyramidNode Failed'
HarrisCornersNode = brcm_openvx.vxHarrisCornersNode(graph, image, 3000, 1, 4, None, None, corners, None)
if HarrisCornersNode is None:
    print 'vxHarrisCornersNode Failed'


OFNode = brcm_openvx.vxOpticalFlowPyrLKNode(graph,pyrPrev,pyrNext,prevpts,corners,currpts,brcm_openvx.VX_TERM_CRITERIA_BOTH,0.03,5,None,[13,13])
if OFNode is None:
    print 'vxOpticalFlowPyrLKNode Failed'


status = brcm_openvx.vxVerifyGraph(graph)
if (status != brcm_openvx.VX_SUCCESS):
    print 'vxVerifyGraph Failed'
brcm_openvx.vxGenerateConfig(context, 'optical_flow', 2, True)

brcm_openvx.vxReleaseContext(context);