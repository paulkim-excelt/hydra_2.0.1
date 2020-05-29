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


def arith_test():
    context = brcm_openvx.vxCreateContext()
    if context is None:
        print 'vxCreateContext Failed'
        print 'Exiting arith_test'
        return None
    brcm_openvx.vxSetName(context, 'arith_test')
    images = [brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx. VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,1280,720, brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx. VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,1280,720, brcm_openvx.VX_DF_IMAGE_S16)]
    for image in images:
        if image is None:
            print 'vxCreateImage Failed'
            print 'Exiting arith_test'
            return None
    brcm_openvx.vxSetName(images[0],'in1')
    brcm_openvx.vxSetName(images[1],'in2')
    brcm_openvx.vxSetName(images[2],'out')
    brcm_openvx.vxSetName(images[3],'in1_s16')
    brcm_openvx.vxSetName(images[4],'in2_s16')
    brcm_openvx.vxSetName(images[5],'out_s16')
    scalar_u8 = brcm_openvx.vxCreateScalar(context,brcm_openvx.VX_TYPE_UINT8,2)
    scalar_s16 = brcm_openvx.vxCreateScalar(context,brcm_openvx. VX_TYPE_INT16,2)
    mean = brcm_openvx.vxCreateScalar(context,brcm_openvx.VX_TYPE_UINT32,0)
    stddev = brcm_openvx.vxCreateScalar(context,brcm_openvx.VX_TYPE_UINT32,0)
    brcm_openvx.vxSetName(scalar_u8,'scalar_u8')
    brcm_openvx.vxSetName(scalar_s16,'scalar_s16')
    brcm_openvx.vxSetName(mean,'scalar_mean')
    brcm_openvx.vxSetName(stddev,'scalar_stddev')
    if scalar_u8 is None or scalar_s16 is None or mean is None or stddev is None:
        print 'vxCreateScalar failed'
        print 'Exiting arith_test'
        return None
    graph_arithu8 = brcm_openvx.vxCreateGraph(context)
    if graph_arithu8 is None:
        print 'vxCreateGraph u8 Failed'
    else:
        brcm_openvx.vxSetName(graph_arithu8,'arithu8')
        copyNode = brcm_openvx.vxCopyExtNode(graph_arithu8,images[0],images[2])
        if copyNode is None:
            print 'vxCopyExtNode u8 Failed'
        addNode = brcm_openvx.vxAddNode(graph_arithu8,images[2], images[1], brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[2])
        if addNode is None:
            print 'vxAddNode u8 Failed'
        subNode = brcm_openvx.vxSubtractNode(graph_arithu8,images[2], images[1], brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[2])
        if subNode is None:
            print 'vxSubtractNode u8 Failed'
        absdiffNode = brcm_openvx.vxAbsDiffNode(graph_arithu8,images[2], images[1], images[2])
        if absdiffNode is None:
            print 'vxAbsDiffNode u8 Failed'
        andNode = brcm_openvx.vxAndNode(graph_arithu8,images[2], images[1], images[2])
        if andNode is None:
            print 'vxAndNode u8 Failed'
        orNode = brcm_openvx.vxOrNode(graph_arithu8,images[2], images[1], images[2])
        if andNode is None:
            print 'vxOrNode u8 Failed'
        xorNode = brcm_openvx.vxXorNode(graph_arithu8,images[2], images[1], images[2])
        if andNode is None:
            print 'vxXorNode u8 Failed'
        minNode = brcm_openvx.vxMinExtNode(graph_arithu8,images[2], images[1], images[2])
        if minNode is None:
            print 'vxMinExtNode u8 Failed'
        maxNode = brcm_openvx.vxMaxExtNode(graph_arithu8,images[2], images[1], images[2])
        if maxNode is None:
            print 'vxMaxExtNode u8 Failed'
        multiplyNode = brcm_openvx.vxMultiplyNode(graph_arithu8,images[2], images[1], 1, brcm_openvx.VX_CONVERT_POLICY_SATURATE, 0, images[2])
        if multiplyNode is None:
            print 'vxMultiplyNode u8 Failed'
        scaleaddNode = brcm_openvx.vxScaleAddExtNode(graph_arithu8,images[2], images[1], 2, images[2])
        if scaleaddNode is None:
            print 'vxScaleAddExtNode u8 Failed'
        compareNode = brcm_openvx.vxCompareExtNode(graph_arithu8,images[2], images[1], brcm_openvx.VX_CMP_EQ, images[2])
        if compareNode is None:
            print 'vxCompareExtNode u8 Failed'
        notNode = brcm_openvx.vxNotNode(graph_arithu8,images[2], images[2])
        if notNode is None:
            print 'vxNotNode u8 Failed'
        meanStdDevNode = brcm_openvx.vxMeanStdDevNode(graph_arithu8,images[0],mean,stddev)
        if meanStdDevNode is None:
            print 'vxMeanStdDevNode u8 Failed'
        status = brcm_openvx.vxVerifyGraph(graph_arithu8)
        if (status != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph u8 Failed'
    graph_ariths16 = brcm_openvx.vxCreateGraph(context)
    if graph_ariths16 is None:
        print 'vxCreateGraph s16 Failed'
    else:
        brcm_openvx.vxSetName(graph_ariths16,'ariths16')
        notNodes16 = brcm_openvx.vxNotNode(graph_ariths16,images[3], images[5])
        if notNodes16 is None:
            print 'vxNotNode s16 Failed'
        addNodes16 = brcm_openvx.vxAddNode(graph_ariths16,images[5], images[4], brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[5])
        if addNodes16 is None:
            print 'vxAddNode s16 Failed'
        subNodes16 = brcm_openvx.vxSubtractNode(graph_ariths16,images[5], images[4], brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[5])
        if subNodes16 is None:
            print 'vxSubtractNode s16 Failed'
        absdiffNodes16 = brcm_openvx.vxAbsDiffNode(graph_ariths16,images[5], images[4], images[5])
        if absdiffNodes16 is None:
            print 'vxAbsDiffNode s16 Failed'
        andNodes16 = brcm_openvx.vxAndNode(graph_ariths16,images[5], images[4], images[5])
        if andNodes16 is None:
            print 'vxAndNode s16 Failed'
        orNodes16 = brcm_openvx.vxOrNode(graph_ariths16,images[5], images[4], images[5])
        if andNodes16 is None:
            print 'vxOrNode s16 Failed'
        xorNodes16 = brcm_openvx.vxXorNode(graph_ariths16,images[5], images[4], images[5])
        if andNodes16 is None:
            print 'vxXorNode s16 Failed'
        minNodes16 = brcm_openvx.vxMinExtNode(graph_ariths16,images[5], images[4], images[5])
        if minNodes16 is None:
            print 'vxMinExtNode s16 Failed'
        maxNodes16 = brcm_openvx.vxMaxExtNode(graph_ariths16,images[5], images[4], images[5])
        if maxNodes16 is None:
            print 'vxMaxExtNode s16 Failed'
        multiplyNodes16 = brcm_openvx.vxMultiplyNode(graph_ariths16,images[5], images[4], 1, brcm_openvx.VX_CONVERT_POLICY_SATURATE, 0, images[5])
        if multiplyNodes16 is None:
            print 'vxMultiplyNode s16 Failed'
        scaleaddNodes16 = brcm_openvx.vxScaleAddExtNode(graph_ariths16,images[5], images[4], 2, images[5])
        if scaleaddNodes16 is None:
            print 'vxScaleAddExtNode s16 Failed'
        compareNodes16 = brcm_openvx.vxCompareExtNode(graph_ariths16,images[5], images[4], brcm_openvx.VX_CMP_EQ, images[2])
        if compareNodes16 is None:
            print 'vxCompareExtNode s16 Failed'
        status = brcm_openvx.vxVerifyGraph(graph_ariths16)
        if (status != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph s16 Failed'
    graph_arithu8s = brcm_openvx.vxCreateGraph(context)
    if graph_arithu8s is None:
        print 'vxCreateGraph u8 scalar Failed'
    else:
        brcm_openvx.vxSetName(graph_arithu8s,'arithu8s')
        setNodes = brcm_openvx.vxSetExtNode(graph_arithu8s,images[2], scalar_u8)
        if setNodes is None:
            print 'vxSetExtNode u8 scalar Failed'
        tempAddNode = brcm_openvx.vxAddNode(graph_arithu8s,images[2], images[0], brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[2])
        if tempAddNode is None:
            print 'vxAddNode u8 scalar Failed'
        addNodes = brcm_openvx.vxAddScalarExtNode(graph_arithu8s,images[2], scalar_u8, brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[2])
        if addNodes is None:
            print 'vxAddScalarExtNode u8 scalar Failed'
        subNodes = brcm_openvx.vxSubtractScalarExtNode(graph_arithu8s,images[2], scalar_u8, brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[2], brcm_openvx.VX_FALSE)
        if subNodes is None:
            print 'vxSubtractScalarExtNode u8 scalar Failed'
        subRevNodes = brcm_openvx.vxSubtractScalarExtNode(graph_arithu8s,images[2], scalar_u8, brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[2], brcm_openvx.VX_TRUE)
        if subRevNodes is None:
            print 'vxSubtractScalarExtNode u8 scalar Failed'
        absdiffNodes = brcm_openvx.vxAbsDiffScalarExtNode(graph_arithu8s,images[2], scalar_u8, images[2])
        if absdiffNodes is None:
            print 'vxAbsDiffScalarExtNode u8 scalar Failed'
        andNodes = brcm_openvx.vxAndScalarExtNode(graph_arithu8s,images[2], scalar_u8, images[2])
        if andNodes is None:
            print 'vxAndScalarExtNode u8 scalar Failed'
        orNodes = brcm_openvx.vxOrScalarExtNode(graph_arithu8s,images[2], scalar_u8, images[2])
        if andNodes is None:
            print 'vxOrScalarExtNode u8 scalar Failed'
        xorNodes = brcm_openvx.vxXorScalarExtNode(graph_arithu8s,images[2], scalar_u8, images[2])
        if andNodes is None:
            print 'vxXorScalarExtNode u8 scalar Failed'
        minNodes = brcm_openvx.vxMinScalarExtNode(graph_arithu8s,images[2], scalar_u8, images[2])
        if minNodes is None:
            print 'vxMinScalarExtNode u8 scalar Failed'
        maxNodes = brcm_openvx.vxMaxScalarExtNode(graph_arithu8s,images[2], scalar_u8, images[2])
        if maxNodes is None:
            print 'vxMaxScalarExtNode u8 scalar Failed'
        multiplyNodes = brcm_openvx.vxMultiplyScalarExtNode(graph_arithu8s,images[2], scalar_u8, 1, brcm_openvx.VX_CONVERT_POLICY_SATURATE, 0, images[2])
        if multiplyNodes is None:
            print 'vxMultiplyScalarExtNode u8 scalar Failed'
        compareNodes = brcm_openvx.vxCompareScalarExtNode(graph_arithu8s,images[2], scalar_u8, brcm_openvx.VX_CMP_EQ, images[2])
        if compareNodes is None:
            print 'vxCompareScalarExtNode u8 scalar Failed'
        status = brcm_openvx.vxVerifyGraph(graph_arithu8s)
        if (status != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph u8 scalar Failed'
    graph_ariths16s = brcm_openvx.vxCreateGraph(context)
    if graph_ariths16s is None:
        print 'vxCreateGraph s16 scalar Failed'
    else:
        brcm_openvx.vxSetName(graph_ariths16s,'ariths16s')
        setNodes16s = brcm_openvx.vxSetExtNode(graph_ariths16s,images[5], scalar_s16)
        if setNodes16s is None:
            print 'vxSetExtNode s16 scalar Failed'
        tempAddNode16s = brcm_openvx.vxAddNode(graph_ariths16s,images[5], images[3], brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[5])
        if tempAddNode16s is None:
            print 'vxAddNode s16 scalar Failed'
        addNodes16s = brcm_openvx.vxAddScalarExtNode(graph_ariths16s,images[5], scalar_s16, brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[5])
        if addNodes16s is None:
            print 'vxAddScalarExtNode s16 scalar Failed'
        subNodes16s = brcm_openvx.vxSubtractScalarExtNode(graph_ariths16s,images[5], scalar_s16, brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[5], brcm_openvx.VX_FALSE)
        if subNodes16s is None:
            print 'vxSubtractScalarExtNode s16 scalar Failed'
        subRevNodes16s = brcm_openvx.vxSubtractScalarExtNode(graph_ariths16s,images[5], scalar_s16, brcm_openvx.VX_CONVERT_POLICY_SATURATE, images[5], brcm_openvx.VX_TRUE)
        if subRevNodes16s is None:
            print 'vxSubtractScalarExtNode s16 scalar Failed'
        absdiffNodes16s = brcm_openvx.vxAbsDiffScalarExtNode(graph_ariths16s,images[5], scalar_s16, images[5])
        if absdiffNodes16s is None:
            print 'vxAbsDiffScalarExtNode s16 scalar Failed'
        andNodes16s = brcm_openvx.vxAndScalarExtNode(graph_ariths16s,images[5], scalar_s16, images[5])
        if andNodes16s is None:
            print 'vxAndScalarExtNode s16 scalar Failed'
        orNodes16s = brcm_openvx.vxOrScalarExtNode(graph_ariths16s,images[5], scalar_s16, images[5])
        if andNodes16s is None:
            print 'vxOrScalarExtNode s16 scalar Failed'
        xorNodes16s = brcm_openvx.vxXorScalarExtNode(graph_ariths16s,images[5], scalar_s16, images[5])
        if andNodes16s is None:
            print 'vxXorScalarExtNode s16 scalar Failed'
        minNodes16s = brcm_openvx.vxMinScalarExtNode(graph_ariths16s,images[5], scalar_s16, images[5])
        if minNodes16s is None:
            print 'vxMinScalarExtNode s16 scalar Failed'
        maxNodes16s = brcm_openvx.vxMaxScalarExtNode(graph_ariths16s,images[5], scalar_s16, images[5])
        if maxNodes16s is None:
            print 'vxMaxScalarExtNode s16 scalar Failed'
        multiplyNodes16s = brcm_openvx.vxMultiplyScalarExtNode(graph_ariths16s,images[5], scalar_s16, 1, brcm_openvx.VX_CONVERT_POLICY_SATURATE, 0, images[5])
        if multiplyNodes16s is None:
            print 'vxMultiplyScalarExtNode s16 scalar Failed'
        compareNodes16s = brcm_openvx.vxCompareScalarExtNode(graph_ariths16s,images[5], scalar_s16, brcm_openvx.VX_CMP_EQ, images[2])
        if compareNodes16s is None:
            print 'vxCompareScalarExtNode s16 scalar s16 scalar Failed'
        status = brcm_openvx.vxVerifyGraph(graph_ariths16s)
        if (status != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph s16 scalar Failed'
    brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)



def colorconvert_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext Failed'
        print 'Exiting ColorConvert Test'
        return None
    brcm_openvx.vxSetName(context, 'colorconvert_test')
    images = [brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_RGB),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_RGBX),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_BGR_EXT),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_BGRX_EXT),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_GRAY_EXT),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_IYUV),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_RGBX),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_RGB),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_NV12),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_NV21),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_YUYV),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_UYVY),
              ]
    for image in images:
        if image is None:
            print 'vxCreateImage Failed'
            print 'Exiting ColorConvert_test'
            return None
    brcm_openvx.vxSetName(images[0],'inRGB')
    brcm_openvx.vxSetName(images[1],'inRGBX')
    brcm_openvx.vxSetName(images[2],'inBGR')
    brcm_openvx.vxSetName(images[3],'inBGRX')
    brcm_openvx.vxSetName(images[4],'outGray')
    brcm_openvx.vxSetName(images[5],'outIYUV')
    brcm_openvx.vxSetName(images[6],'outRGBX')
    brcm_openvx.vxSetName(images[7],'outRGB')
    brcm_openvx.vxSetName(images[8],'inNV12')
    brcm_openvx.vxSetName(images[9],'inNV21')
    brcm_openvx.vxSetName(images[10],'inYUYV')
    brcm_openvx.vxSetName(images[11],'inUYVY')
    graph_clrConvfrmRGB = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmRGB is None:
        print 'vxCreateGraph for graph_clrConvfrmRGB Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmRGB,'clrConvfrmRGB')

        RGBtoGRAYNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmRGB,images[0], images[4])
        if RGBtoGRAYNode is None:
            print 'RGBtoGRAYNode Failed'

        RGBtoIYUVNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmRGB,images[0],  images[5])
        if RGBtoIYUVNode is None:
            print 'RGBtoIYUVNode Failed'

        RGBtoRGBXNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmRGB,images[0],  images[6])
        if RGBtoRGBXNode is None:
            print 'RGBtoRGBXNode Failed'

        IYUVtoRGBNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmRGB,images[5], images[7])
        if IYUVtoRGBNode is None:
            print 'IYUVtoRGBNode Failed'

        status1 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmRGB)
        if (status1 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for clrConvfrmRGB Failed'

    graph_clrConvfrmRGBX = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmRGBX is None:
        print 'vxCreateGraph for graph_clrConvfrmRGBX Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmRGBX,'clrConvfrmRGBX')

        RGBXtoIYUVNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmRGBX,images[1],  images[5])
        if RGBXtoIYUVNode is None:
            print 'RGBXtoIYUVNode Failed'

        RGBXtoRGBNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmRGBX,images[1],  images[7])
        if RGBXtoRGBNode is None:
            print 'RGBXtoRGBNode Failed'

        IYUVtoRGBXNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmRGBX,images[5], images[6])
        if IYUVtoRGBXNode is None:
            print 'IYUVtoRGBXNode Failed'

        status2 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmRGBX)
        if (status2 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for clrConvfrmRGBX Failed'

    graph_clrConvfrmBGR = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmBGR is None:
        print 'vxCreateGraph for graph_clrConvfrmBGR Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmBGR,'clrConvfrmBGR')

        BGRtoGRAYNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmBGR,images[2], images[4])
        if BGRtoGRAYNode is None:
            print 'BGRtoGRAYNode Failed'

        BGRtoIYUVNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmBGR,images[2],  images[5])
        if BGRtoIYUVNode is None:
            print 'BGRtoIYUVNode Failed'

        BGRtoRGBXNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmBGR,images[2],  images[6])
        if BGRtoRGBXNode is None:
            print 'BGRtoRGBXNode Failed'

        status3 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmBGR)
        if (status3 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for clrConvfrmBGR Failed'

    graph_clrConvfrmBGRX = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmBGRX is None:
        print 'vxCreateGraph for graph_clrConvfrmBGRX Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmBGRX,'clrConvfrmBGRX')

        BGRXtoIYUVNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmBGRX,images[3],  images[5])
        if BGRXtoIYUVNode is None:
            print 'BGRXtoIYUVNode Failed'

        BGRXtoRGBNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmBGRX,images[3],  images[7])
        if BGRXtoRGBNode is None:
            print 'BGRXtoRGBNode Failed'

        status4 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmBGRX)
        if (status4 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for graph_clrConvfrmBGRX Failed'

    graph_clrConvfrmNV12 = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmNV12 is None:
        print 'vxCreateGraph for graph_clrConvfrmNV12 Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmNV12,'clrConvfrmNV12')
        NV12toRGBNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmNV12, images[8], images[7])
        if NV12toRGBNode is None:
            print 'NV12toRGBNode Failed'
        NV12toRGBXNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmNV12, images[8], images[6])
        if NV12toRGBXNode is None:
            print 'NV12toRGBXNode Failed'
        status5 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmNV12)
        if (status5 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for graph_clrConvfrmNV12 Failed'

    graph_clrConvfrmNV21 = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmNV21 is None:
        print 'vxCreateGraph for graph_clrConvfrmNV21 Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmNV21,'clrConvfrmNV21')
        NV21toRGBNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmNV21,images[9], images[7])
        if NV21toRGBNode is None:
            print 'NV21toRGBNode Failed'
        NV21toRGBXNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmNV21,images[9],  images[6])
        if NV21toRGBXNode is None:
            print 'NV21toRGBXNode Failed'
        status6 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmNV21)
        if (status6 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for graph_clrConvfrmNV21 Failed'

    graph_clrConvfrmYUYV = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmYUYV is None:
        print 'vxCreateGraph for graph_clrConvfrmYUYV Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmYUYV,'clrConvfrmYUYV')
        YUYVtoRGBNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmYUYV,images[10], images[7])
        if YUYVtoRGBNode is None:
            print 'YUYVtoRGBNode Failed'
        YUYVtoRGBXNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmYUYV,images[10],  images[6])
        if YUYVtoRGBXNode is None:
            print 'YUYVtoRGBXNode Failed'
        status7 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmYUYV)
        if (status7 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for graph_clrConvfrmYUYV Failed'

    graph_clrConvfrmUYVY = brcm_openvx.vxCreateGraph(context)
    if graph_clrConvfrmUYVY is None:
        print 'vxCreateGraph for graph_clrConvfrmUYVY Failed'
    else:
        brcm_openvx.vxSetName(graph_clrConvfrmUYVY,'clrConvfrmUYVY')
        UYVYtoRGBNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmUYVY,images[11], images[7])
        if UYVYtoRGBNode is None:
            print 'UYVYtoRGBNode Failed'
        UYVYtoRGBXNode = brcm_openvx.vxColorConvertNode(graph_clrConvfrmUYVY,images[11],  images[6])
        if UYVYtoRGBXNode is None:
            print 'UYVYtoRGBXNode Failed'
        status8 = brcm_openvx.vxVerifyGraph(graph_clrConvfrmUYVY)
        if (status8 != brcm_openvx.VX_SUCCESS):
            print 'vxVerifyGraph for graph_clrConvfrmUYVY Failed'

    if (status1 == brcm_openvx.VX_SUCCESS or status2 == brcm_openvx.VX_SUCCESS or status3 == brcm_openvx.VX_SUCCESS
        or status4 == brcm_openvx.VX_SUCCESS or status5 == brcm_openvx.VX_SUCCESS or status6 == brcm_openvx.VX_SUCCESS
        or status7 == brcm_openvx.VX_SUCCESS or status8 == brcm_openvx.VX_SUCCESS ):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    else:
        print 'Exiting Colorconvert test'
    brcm_openvx.vxReleaseContext(context)


def scale_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext Scale_test Failed'
        print 'Exiting Scale_test'
        return None
    brcm_openvx.vxSetName(context, 'scale_test')
    images = [brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,960,540,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,500,480,brcm_openvx.VX_DF_IMAGE_U8)]
    for image in images:
        if image is None:
            print 'vxCreateImage Scale_test Failed'
            print 'Exiting Scale_test'
            return None
    brcm_openvx.vxSetName(images[0],'in')
    brcm_openvx.vxSetName(images[1],'out')
    brcm_openvx.vxSetName(images[2],'in1')
    brcm_openvx.vxSetName(images[3],'out1')
    graph_scale = brcm_openvx.vxCreateGraph(context)
    if graph_scale is None:
        print 'vxCreateGraph Scale_test Failed'
        print 'Exiting Scale_test'
    brcm_openvx.vxSetName(graph_scale,'scale')
    scaleDown = brcm_openvx.vxScaleImageNode(graph_scale, images[0], images[1], brcm_openvx.VX_INTERPOLATION_BILINEAR)
    if scaleDown is None:
        print 'vxScaleImageNode - Downscale fails'
    scaleUp = brcm_openvx.vxScaleImageNode(graph_scale, images[2], images[3], brcm_openvx.VX_INTERPOLATION_BILINEAR)
    if scaleUp is None:
        print 'vxScaleImageNode - Upscale fails'
    status = brcm_openvx.vxVerifyGraph(graph_scale)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph Scale_test Failed'
    brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def threshold_test():
    threshValU8 = [0xA0]
    threshTrueValU8 = [0xFF]
    threshValS16 = [0xB0]
    threshTrueValS16 = [0xFF]
    threshValS32 = [0x14]
    threshTrueValS32 = [0xFF]
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext threshold_test Failed'
        print 'exiting threshold_test'
        return None
    brcm_openvx.vxSetName(context, 'threshold_test')
    images = [brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx. VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_S32),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_S32)]
    for image in images:
        if image is None:
            print 'vxCreateImage Failed'
            print 'Exiting threshold_test'
            return None
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'outU8')
    brcm_openvx.vxSetName(images[2],'inS16')
    brcm_openvx.vxSetName(images[3],'outS16')
    brcm_openvx.vxSetName(images[4],'inS32')
    brcm_openvx.vxSetName(images[5],'outS32')
    thresholds = [brcm_openvx.vxCreateThreshold(context,brcm_openvx.VX_THRESHOLD_TYPE_BINARY,brcm_openvx.VX_TYPE_UINT8),
                  brcm_openvx.vxCreateThreshold(context,brcm_openvx.VX_THRESHOLD_TYPE_BINARY,brcm_openvx.VX_TYPE_INT16),
                  brcm_openvx.vxCreateThreshold(context,brcm_openvx.VX_THRESHOLD_TYPE_BINARY,brcm_openvx.VX_TYPE_INT32)    ]
    for threshold in thresholds:
        if threshold is None:
            print 'vxCreatethreshold is Failed'
            print 'exiting threshold_test'
            return None
    brcm_openvx.vxSetThresholdAttribute(thresholds[0],brcm_openvx.VX_THRESHOLD_THRESHOLD_VALUE,threshValU8, len(threshValU8))
    brcm_openvx.vxSetThresholdAttribute(thresholds[1],brcm_openvx.VX_THRESHOLD_THRESHOLD_VALUE,threshValS16, len(threshValS16))
    brcm_openvx.vxSetThresholdAttribute(thresholds[2],brcm_openvx.VX_THRESHOLD_THRESHOLD_VALUE,threshValS32, len(threshValS32))
    brcm_openvx.vxSetThresholdAttribute(thresholds[0],brcm_openvx.VX_THRESHOLD_TRUE_VALUE,threshTrueValU8, len(threshTrueValU8))
    brcm_openvx.vxSetThresholdAttribute(thresholds[1],brcm_openvx.VX_THRESHOLD_TRUE_VALUE,threshTrueValS16, len(threshTrueValS16))
    brcm_openvx.vxSetThresholdAttribute(thresholds[2],brcm_openvx.VX_THRESHOLD_TRUE_VALUE,threshTrueValS32, len(threshTrueValS32))
    brcm_openvx.vxSetName(thresholds[0],'thresholdU8')
    brcm_openvx.vxSetName(thresholds[1],'thresholdS16')
    brcm_openvx.vxSetName(thresholds[2],'thresholdS32')
    graph_threshold = brcm_openvx.vxCreateGraph(context)
    if graph_threshold is None:
        print 'vxCreateGraph Failed'
        print 'Exiting threshold_test'
        return None
    brcm_openvx.vxSetName(graph_threshold,'thresholdBinary')
    thresholdU8Node = brcm_openvx.vxThresholdNode(graph_threshold,images[0],  thresholds[0],images[1])
    if thresholdU8Node is None:
        print 'vxThresholdU8 Failed'
    thresholdS16Node = brcm_openvx.vxThresholdNode(graph_threshold,images[2],  thresholds[1],images[3])
    if thresholdS16Node is None:
        print 'vxThresholdS16 Failed'
    thresholdS32Node = brcm_openvx.vxThresholdNode(graph_threshold,images[4],  thresholds[2],images[5])
    if thresholdS32Node is None:
        print 'vxThresholdS32 Failed'
    status = brcm_openvx.vxVerifyGraph(graph_threshold)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph threshold_test Failed'
        return None
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def filter_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext Failed'
        print 'Exiting filter_test'
        return None
    brcm_openvx.vxSetName(context, 'filter_test')
    images = [brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8)]
    for image in images:
        if image is None:
            print 'vxCreateImage Failed'
            print 'Exiting filter_test'
            return None
    brcm_openvx.vxSetName(images[0],'in_sobel')
    brcm_openvx.vxSetName(images[1],'grad_x')
    brcm_openvx.vxSetName(images[2],'grad_y')
    brcm_openvx.vxSetName(images[3],'inU8')
    brcm_openvx.vxSetName(images[4],'canny')
    brcm_openvx.vxSetName(images[5],'medianU8')
    brcm_openvx.vxSetName(images[6],'gaussianU8')
    brcm_openvx.vxSetName(images[7],'inS16')
    brcm_openvx.vxSetName(images[8],'medianS16')
    brcm_openvx.vxSetName(images[9],'gaussianS16')
    brcm_openvx.vxSetName(images[10],'blurU8')

    threshold = brcm_openvx.vxCreateThreshold(context,brcm_openvx.VX_THRESHOLD_TYPE_RANGE,brcm_openvx.VX_TYPE_UINT8)
    if threshold is None:
        print 'vxCreateThreshold filter_test Failed'
        print 'Exiting filter_test'
        return None
    ret = brcm_openvx.vxSetThresholdAttribute(threshold,brcm_openvx.VX_THRESHOLD_THRESHOLD_UPPER,[100],1)
    if ret is not brcm_openvx.VX_SUCCESS:
        print 'vxSetThresholdAttribute:VX_THRESHOLD_THRESHOLD_UPPER Failed'

    ret = brcm_openvx.vxSetThresholdAttribute(threshold,brcm_openvx.VX_THRESHOLD_THRESHOLD_LOWER,[20],1)
    if ret is not brcm_openvx.VX_SUCCESS:
        print 'vxSetThresholdAttribute:VX_THRESHOLD_THRESHOLD_LOWER Failed'

    graph_filter = brcm_openvx.vxCreateGraph(context)
    if graph_filter is None:
        print 'vxCreateGraph Failed'
        print 'Exiting filter_test'
        return None
    brcm_openvx.vxSetName(graph_filter,'filter')

    sobelNode = brcm_openvx.vxSobel3x3Node(graph_filter,images[0],images[1],images[2])
    if sobelNode is None:
        print 'vxSobel3x3Node Failed'

    cannyNode = brcm_openvx.vxCannyEdgeDetectorNode(graph_filter,images[3],threshold,3,brcm_openvx.VX_NORM_L1,images[4])
    if cannyNode is None:
        print 'vxCannyEdgeDetectorNode Failed'

    medianFilterU8Node = brcm_openvx.vxMedian3x3Node(graph_filter,images[3], images[5])
    if medianFilterU8Node is None:
        print 'medianFilterU8Node Failed'

    gaussianFilterU8Node = brcm_openvx.vxGaussian3x3Node(graph_filter,images[3], images[6])
    if gaussianFilterU8Node is None:
        print 'gaussianFilterU8Node Failed'

    medianFilterS16Node = brcm_openvx.vxMedian3x3Node(graph_filter,images[7],  images[8])
    if medianFilterS16Node is None:
        print 'medianFilterS16Node Failed'

    gaussianFilterS16Node = brcm_openvx.vxGaussian3x3Node(graph_filter,images[7],  images[9])
    if gaussianFilterS16Node is None:
        print 'gaussianFilterS16Node Failed'

    blurU8Node = brcm_openvx.vxBlur3x3ExtNode(graph_filter,images[3], images[10])
    if blurU8Node is None:
        print 'blurU8Node Failed'

    status = brcm_openvx.vxVerifyGraph(graph_filter)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph filter_test Failed'
        return None
    brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)

def hist_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext hist_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'hist_test')
    images = [brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8)]
    for image in images:
        if image is None:
            print 'vxCreateImage Failed'
            return None
    brcm_openvx.vxSetName(images[0],'in')

    distribution = brcm_openvx.vxCreateDistribution(context,256,0,256)
    if distribution is None:
            print 'vxCreateDistribution Failed'
            return None
    brcm_openvx.vxSetName(distribution,'hist_dist')

    graph_hist = brcm_openvx.vxCreateGraph(context)
    if graph_hist is None:
        print 'vxCreateGraph Failed'
        return None
    brcm_openvx.vxSetName(graph_hist,'hist')

    histNode = brcm_openvx.vxHistogramNode(graph_hist,images[0],distribution)
    if histNode is None:
        print 'vxHistogramNode Failed'
        return None
    status = brcm_openvx.vxVerifyGraph(graph_hist)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph Failed'
        return None
    brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def morph_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext morph_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'morph_test')
    images = [brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx. VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S32),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S32),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S32)]
    for image in images:
        if image is None:
            print 'vxCreateImage morph_test Failed'
            return None
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'outU8')
    brcm_openvx.vxSetName(images[2],'inU16')
    brcm_openvx.vxSetName(images[3],'outU16')
    brcm_openvx.vxSetName(images[4],'in1S32')
    brcm_openvx.vxSetName(images[5],'in2S32')
    brcm_openvx.vxSetName(images[6],'outS32')
    graph_morphU8 = brcm_openvx.vxCreateGraph(context)
    if graph_morphU8 is None:
        print 'vxCreateGraph morph_test U8 Failed'
        return None
    brcm_openvx.vxSetName(graph_morphU8,'morphU8')
    DilateU8Node = brcm_openvx.vxDilate3x3Node(graph_morphU8,images[0], images[1])
    if DilateU8Node is None:
        print 'DilateU8Node Failed'
    ErodeU8Node = brcm_openvx.vxErode3x3Node(graph_morphU8,images[1], images[0])
    if ErodeU8Node is None:
        print 'ErodeU8Node Failed'
    MorphU8Node1 = brcm_openvx.vxMorphologyExtNode(graph_morphU8,images[0], images[1],brcm_openvx.VX_MORPH_BLACKHAT_EXT,4)
    if MorphU8Node1 is None:
        print 'MorphU8Node1 Failed'
    MorphU8Node2 = brcm_openvx.vxMorphologyExtNode(graph_morphU8,images[1], images[0],brcm_openvx.VX_MORPH_TOPHAT_EXT,3)
    if MorphU8Node2 is None:
        print 'MorphU8Node2 Failed'
    MorphU8Node3 = brcm_openvx.vxMorphologyExtNode(graph_morphU8,images[0], images[1],brcm_openvx.VX_MORPH_GRADIENT_EXT,2)
    if MorphU8Node3 is None:
        print 'MorphU8Node3 Failed'
    MorphU8Node4 = brcm_openvx.vxMorphologyExtNode(graph_morphU8,images[1], images[0],brcm_openvx.VX_MORPH_OPEN_EXT,1)
    if MorphU8Node4 is None:
        print 'MorphU8Node4 Failed'
    MorphU8Node5 = brcm_openvx.vxMorphologyExtNode(graph_morphU8,images[0], images[1],brcm_openvx.VX_MORPH_CLOSE_EXT,2)
    if MorphU8Node5 is None:
        print 'MorphU8Node5 Failed'
    status = brcm_openvx.vxVerifyGraph(graph_morphU8)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph morph U8 Failed'

    graph_morphU16 = brcm_openvx.vxCreateGraph(context)
    if graph_morphU16 is None:
        print 'vxCreateGraph morphU16 Failed'
        return None
    brcm_openvx.vxSetName(graph_morphU16,'morphU16')
    DilateU16Node = brcm_openvx.vxDilate3x3Node(graph_morphU16,images[2], images[3])
    if DilateU16Node is None:
        print 'DilateU16Node Failed'
    ErodeU16Node = brcm_openvx.vxErode3x3Node(graph_morphU16,images[3], images[2])
    if ErodeU16Node is None:
        print 'ErodeU16Node Failed'
    MorphU16Node1 = brcm_openvx.vxMorphologyExtNode(graph_morphU16,images[2], images[3],brcm_openvx.VX_MORPH_OPEN_EXT,3)
    if MorphU16Node1 is None:
        print 'MorphU16Node1 Failed'
    MorphU16Node2 = brcm_openvx.vxMorphologyExtNode(graph_morphU16,images[3], images[2],brcm_openvx.VX_MORPH_CLOSE_EXT,2)
    if MorphU16Node2 is None:
        print 'MorphU16Node2 Failed'
    MorphU16Node3 = brcm_openvx.vxMorphologyExtNode(graph_morphU16,images[2], images[3],brcm_openvx.VX_MORPH_ERODE_EXT,5)
    if MorphU16Node3 is None:
        print 'MorphU16Node3 Failed'
    status = brcm_openvx.vxVerifyGraph(graph_morphU16)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph morphU16 Failed'
    graph_morphS32 = brcm_openvx.vxCreateGraph(context)
    if graph_morphS32 is None:
        print 'vxCreateGraph morphS32 Failed'
    brcm_openvx.vxSetName(graph_morphS32,'morphS32')
    DilateS32Node = brcm_openvx.vxDilate3x3Node(graph_morphS32,images[4], images[5])
    if DilateS32Node is None:
        print 'DilateS32Node Failed'

    ErodeS32Node = brcm_openvx.vxErode3x3Node(graph_morphS32,images[5], images[6])
    if ErodeS32Node is None:
        print 'ErodeS32Node Failed'
    status = brcm_openvx.vxVerifyGraph(graph_morphS32)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph morph32 Failed'
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def bitdepthconvert_test ():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext bitdepthconvert_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'bitdepthConv_test')
    images = [brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_S16),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_S16),
              ]
    for image in images:
        if image is None:
            print 'vxCreateImage bitdepthconvert_test Failed'
            return None
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'inS16')
    brcm_openvx.vxSetName(images[2],'outU8')
    brcm_openvx.vxSetName(images[3],'outS16')
    scalar_val = brcm_openvx.vxCreateScalar(context,brcm_openvx.VX_TYPE_INT32,8)
    brcm_openvx.vxSetName(scalar_val,'scalar_val')
    graph_bitdepthConv = brcm_openvx.vxCreateGraph(context)
    if graph_bitdepthConv is None:
        print 'vxCreateGraph for graph_bitdepthConv Failed'
    brcm_openvx.vxSetName(graph_bitdepthConv,'bitdepthConv')
    U8toS16Node = brcm_openvx.vxConvertDepthNode(graph_bitdepthConv,images[0], images[3], brcm_openvx.VX_CONVERT_POLICY_SATURATE,scalar_val)
    if U8toS16Node is None:
        print 'U8toS16Node Failed'
    S16toU8Node = brcm_openvx.vxConvertDepthNode(graph_bitdepthConv,images[1],images[2],brcm_openvx.VX_CONVERT_POLICY_SATURATE,scalar_val)
    if S16toU8Node is None:
        print 'S16toU8Node Failed'
    status = brcm_openvx.vxVerifyGraph(graph_bitdepthConv)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph for graph_bitdepthConv Failed'
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def IntegralImg_test ():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext integralImg_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'integralImg_test')
    images = [brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,640,480,brcm_openvx.VX_DF_IMAGE_U32)]
    for image in images:
        if image is None:
            print 'vxCreateImage integralImg_test Failed'
            return None
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'outU32')

    graph_integralImg = brcm_openvx.vxCreateGraph(context)
    if graph_integralImg is None:
        print 'vxCreateGraph for graph_integralImg Failed'
    brcm_openvx.vxSetName(graph_integralImg,'integralImg')
    integralImgNode = brcm_openvx.vxIntegralImageNode(graph_integralImg,images[0], images[1])
    if integralImgNode is None:
        print 'integralImgNode Failed'
    status = brcm_openvx.vxVerifyGraph(graph_integralImg)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph for graph_integralImg Failed'
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def accumulateImg_test ():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext accumulateImg_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'accumulateImg_test')
    images = [brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,256,256,brcm_openvx.VX_DF_IMAGE_S16)]
    for image in images:
        if image is None:
            print 'vxCreateImage  accumulateImg_test Failed'
            return None
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'accums16')

    graph_accumImg = brcm_openvx.vxCreateGraph(context)
    if graph_accumImg is None:
        print 'vxCreateGraph for graph_accumImg Failed'
    brcm_openvx.vxSetName(graph_accumImg,'accumImg')
    accumulateImgNode = brcm_openvx.vxAccumulateImageNode(graph_accumImg,images[0], images[1])
    if accumulateImgNode is None:
        print 'accumulateImgNode Failed'
    status = brcm_openvx.vxVerifyGraph(graph_accumImg)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph for graph_accumImg Failed'
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def gaussianpyramid_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext pyramid_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'pyramid_test')
    image = brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8)
    if image is None:
        print 'vxCreateImage pyramid_test Failed'
        return None
    brcm_openvx.vxSetName(image,'fullImage')
    pyrImgORB = brcm_openvx.vxCreatePyramid(context, 3, brcm_openvx.VX_SCALE_PYRAMID_ORB, 1280, 720, brcm_openvx.VX_DF_IMAGE_U8)
    if pyrImgORB is None:
        print 'vxCreatePyramid Failed'
    brcm_openvx.vxSetName(pyrImgORB,'ImgPyramidORB')
    pyrImgHalfScale = brcm_openvx.vxCreatePyramid(context, 3, brcm_openvx.VX_SCALE_PYRAMID_HALF, 1280, 720, brcm_openvx.VX_DF_IMAGE_U8)
    if pyrImgHalfScale is None:
        print 'vxCreatePyramid Failed'
    brcm_openvx.vxSetName(pyrImgHalfScale,'ImgPyramidHalfScale')

    graph_gaussianpyramid = brcm_openvx.vxCreateGraph(context);
    if graph_gaussianpyramid is None:
        print 'vxCreateGraph Failed'
    brcm_openvx.vxSetName(graph_gaussianpyramid,'gaussianPyramid')

    GauPyrNode2 = brcm_openvx.vxGaussianPyramidNode(graph_gaussianpyramid, image, pyrImgHalfScale)
    if GauPyrNode2 is None:
        print 'vxGaussianPyramidNode HalfScale Failed'
    GauPyrNode1 = brcm_openvx.vxGaussianPyramidNode(graph_gaussianpyramid, image, pyrImgORB)
    if GauPyrNode1 is None:
        print 'vxGaussianPyramidNode ORB Failed'
    status = brcm_openvx.vxVerifyGraph(graph_gaussianpyramid)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph for graph_gaussianpyramid Failed'
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def hlfsclGaussian_test ():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext halfscaleGaussian_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'halfscaleGaussian_test')
    images = [brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,640,360,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,640,360,brcm_openvx.VX_DF_IMAGE_U8)]
    for image in images:
        if image is None:
            print 'vxCreateImage halfscaleGaussian_test  Failed'
            return None
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'hlfsclGaussOut3x3')
    brcm_openvx.vxSetName(images[2],'hlfsclGaussOut5x5')

    graph_hlfsclGaussian = brcm_openvx.vxCreateGraph(context)
    if graph_hlfsclGaussian is None:
        print 'vxCreateGraph for graph_hlfsclGaussian Failed'
    brcm_openvx.vxSetName(graph_hlfsclGaussian,'hlfsclGaussian')
    hlfsclGaussian3x3Node = brcm_openvx.vxHalfScaleGaussianNode(graph_hlfsclGaussian,images[0], images[1], 3)
    if hlfsclGaussian3x3Node is None:
        print 'hlfsclGaussian3x3Node Failed'
    hlfsclGaussian5x5Node = brcm_openvx.vxHalfScaleGaussianNode(graph_hlfsclGaussian,images[0], images[2], 5)
    if hlfsclGaussian5x5Node is None:
        print 'hlfsclGaussian5x5Node Failed'
    status = brcm_openvx.vxVerifyGraph(graph_hlfsclGaussian)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph for graph_hlfsclGaussian Failed'
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)



def laplacianpyramidrecon_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext laplacianpyramidrecon_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'lp_recon_test')
    inpW = 1280
    inpH = 720
    image = brcm_openvx.vxCreateImage(context,inpW,inpH,brcm_openvx.VX_DF_IMAGE_U8)
    if image is None:
        print 'vxCreateImage laplacianpyramidrecon_test Failed'
        return None
    brcm_openvx.vxSetName(image,'fullImg')
    lvl = 3
    laplacian = brcm_openvx.vxCreatePyramid(context, lvl, brcm_openvx.VX_SCALE_PYRAMID_HALF, inpW, inpH, brcm_openvx.VX_DF_IMAGE_S16)
    if laplacian is None:
        print 'vxCreatePyramid laplacianPyrdRecon Failed'
    brcm_openvx.vxSetName(laplacian,'Laplacian')
    loW = inpW/pow(2,lvl)
    loH = inpH/pow(2,lvl)
    outputImg = brcm_openvx.vxCreateImage(context,loW,loH,brcm_openvx.VX_DF_IMAGE_S16)
    if outputImg is None:
        print 'vxCreateImage laplacianPyrdRecon Failed'
        return None
    brcm_openvx.vxSetName(outputImg,'outImg')
    outputReconImg = brcm_openvx.vxCreateImage(context,inpW,inpH,brcm_openvx.VX_DF_IMAGE_U8)
    if outputReconImg is None:
        print 'vxCreateImage laplacianPyrdRecon Failed'
        return None
    brcm_openvx.vxSetName(outputReconImg,'outReconImg')
    graph_laplacianpyramid = brcm_openvx.vxCreateGraph(context)
    if graph_laplacianpyramid is None:
        print 'vxCreateGraph laplacianPyrdRecon Failed'
        return None
    brcm_openvx.vxSetName(graph_laplacianpyramid,'laplacianPyrdRecon')

    laplacePyrNode = brcm_openvx.vxLaplacianPyramidNode(graph_laplacianpyramid, image, laplacian,outputImg)
    if laplacePyrNode is None:
        print 'vxLaplacianPyramidNode Failed'
        return None
    laplaceReconNode = brcm_openvx.vxLaplacianReconstructNode(graph_laplacianpyramid, laplacian, outputImg,outputReconImg)
    if laplaceReconNode is None:
        print 'vxLaplacianReconstructNode Failed'
        return None
    status = brcm_openvx.vxVerifyGraph(graph_laplacianpyramid)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph for graph_laplacianpyramid Failed'
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)


def upsampleby2x2_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext upsampleby2x2_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'upsampleby2x2_test')
    images = [brcm_openvx.vxCreateImage(context,640,360,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8)]
    for image in images:
        if image is None:
            print 'vxCreateImage upsampleby2x2_test Failed'
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'outU8')

    graph_upsampleby2x2 = brcm_openvx.vxCreateGraph(context)
    if graph_upsampleby2x2 is None:
        print 'vxCreateGraph for graph_upsampleby2x2 Failed'
        return None
    brcm_openvx.vxSetName(graph_upsampleby2x2,'upsampleby2x2')
    upsampleby2x2Node = brcm_openvx.vxUpSampleBy2x2Node(graph_upsampleby2x2,images[0], images[1])
    if upsampleby2x2Node is None:
        print 'upsampleby2x2Node upsampleby2x2_test Failed'
        return None
    status = brcm_openvx.vxVerifyGraph(graph_upsampleby2x2)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph for graph_upsampleby2x2 Failed'
        return None
    if (status == brcm_openvx.VX_SUCCESS):
        brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 2)
    brcm_openvx.vxReleaseContext(context)



def cropnscale_test():
    context = brcm_openvx.vxCreateContext();
    if context is None:
        print 'vxCreateContext cropnscale_test Failed'
        return None
    brcm_openvx.vxSetName(context, 'cropnscale_test')
    images = [brcm_openvx.vxCreateImage(context,1280,720,brcm_openvx.VX_DF_IMAGE_U8),
              brcm_openvx.vxCreateImage(context,640,240,brcm_openvx.VX_DF_IMAGE_U8)]
    for image in images:
        if image is None:
            print 'vxCreateImage cropnscale_test Failed'
    start_x = 128
    start_y = 128
    end_x = 1151
    end_y = 383
    rect = brcm_openvx.vxCreateRectangle(context,start_x, start_y, end_x,end_y)
    if rect is None:
        print 'rect fails'
    imgROI = brcm_openvx.vxCreateImageFromROI(images[0],rect)
    if imgROI is None:
        print 'vxCreateROIImage cropnscale_test Failed'
    brcm_openvx.vxSetName(images[0],'inU8')
    brcm_openvx.vxSetName(images[1],'outU8')
    graph_cropnscale = brcm_openvx.vxCreateGraph(context)
    if graph_cropnscale is None:
        print 'vxCreateGraph cropnscale Failed'
        print 'Exiting cropnscale'
    brcm_openvx.vxSetName(graph_cropnscale,'cropnscale')
    cropnscale = brcm_openvx.vxScaleImageNode(graph_cropnscale, imgROI, images[1],  brcm_openvx.VX_INTERPOLATION_BILINEAR)

    if cropnscale is None:
        print 'vxScaleImageNode - cropnscale fails'
    status = brcm_openvx.vxVerifyGraph(graph_cropnscale)
    if (status != brcm_openvx.VX_SUCCESS):
        print 'vxVerifyGraph cropnscale Failed'
    brcm_openvx.vxGenerateConfig(context, 'tests/qualification', 1)
    brcm_openvx.vxReleaseContext(context)

def main():    
    arith_test()
    colorconvert_test()
    scale_test()
    threshold_test()
    filter_test()
    hist_test()
    morph_test()
    bitdepthconvert_test()
    IntegralImg_test()
    accumulateImg_test()
    gaussianpyramid_test()
    hlfsclGaussian_test()
    laplacianpyramidrecon_test()
    cropnscale_test()


if  __name__ =='__main__':main()