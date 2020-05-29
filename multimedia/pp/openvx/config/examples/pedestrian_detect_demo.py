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
brcm_openvx.vxSetName(context, 'pedestrian_detect')

image0 = brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8)
if image0 is None:
    print 'vxCreateImage Failed'
brcm_openvx.vxSetName(image0,'Input')

rectangles = brcm_openvx.vxCreateArray(context, brcm_openvx.VX_TYPE_RECTANGLE, 128)
brcm_openvx.vxSetName(rectangles,'Output_Rectangles')

num_rect = brcm_openvx.vxCreateScalar(context, brcm_openvx.VX_TYPE_INT32, 0)
brcm_openvx.vxSetName(num_rect,'Num_Rectangles')

graph = brcm_openvx.vxCreateGraph(context);
if graph is None:
    print 'vxCreateGraph Failed'
brcm_openvx.vxSetName(graph,'process')

image1 = brcm_openvx.vxCreateVirtualImage(graph,640,360,brcm_openvx.VX_DF_IMAGE_U8)
if image1 is None:
    print 'vxCreateVirtualImage Failed'

scale_node = brcm_openvx.vxScaleImageNode(graph, image0, image1, brcm_openvx.VX_INTERPOLATION_BILINEAR)
if scale_node is None:
    print 'vxScaleNode Failed'

ped_detect = brcm_openvx.vxPedestrianDetectNode(graph, image1, rectangles, num_rect, 1.1, 12, 2.0)
if ped_detect is None:
    print 'vxPedestrianDetect Failed'

status = brcm_openvx.vxVerifyGraph(graph)
if (status != brcm_openvx.VX_SUCCESS):
    print 'vxVerifyGraph Failed'

brcm_openvx.vxGenerateConfig(context, 'pedestrian_detect', 4)

brcm_openvx.vxReleaseContext(context);