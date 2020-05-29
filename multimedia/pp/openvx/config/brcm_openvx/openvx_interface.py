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

#
####################################################################################
##@addtogroup grp_openvx_py_pkg
##@{

import struct
import brcm_openvx
import os
import logging
import inspect
import hashlib
import numbers
import image
import utils
import arith
import bitdepth
import graph
import logging
import face_detect
import scalar
import accumulate
import array
import colorconvert
import context
import corner
import filter
import histogram
import integral
import morph
import pedestrian_detect
import process
import pyramid
import scalar
import scale
import threshold
import track
import defn





##@brief Sets the name to the object

##@This api sets the name to the object.
##@behavior Sync, Re-entrant
##@pre None
##@param[in]   obj        Reference to the context/graph /image/pyramid/scalar/array object
##@param[in]   name       name to be given for the object
##<BR>Return values are documented in reverse-chronological order
##@retval         None    None
##
##@post None
##@trace vxSetName
##@limitations
def vxSetName(obj, name):
    defn.vxSetName_int(obj, name)


##@brief Accumulation of two images

##It accumulates the pixel data from input and accum images and updates the accumulated values on ##@the accumulation image.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    Input image in VX_DF_IMAGE_U8 format
##@param[out]     accum    Accumulation image in VX_DF_IMAGE_U16 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxAccumulateImageNode
##@limitations All the input and output images should be of the same size.

def vxAccumulateImageNode(graph, input, accum):
    return accumulate.vxAccumulateImageNode_int(graph, input, accum)

##@brief creates a Rectangle

##<BR>Creates a reference to an array object.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   context    Reference to the overall context
##@param[in]      start_x    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      start_y    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      end_x    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      end_y    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_rect is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCreateRectangle
##@limitations None

def vxCreateRectangle(context, start_x, start_y, end_x,end_y ):
    return array.vxCreateRectangle_int(context, start_x, start_y, end_x,end_y )


##@brief creates an Array

##<BR>Creates a reference to an array object.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   context    Reference to the overall context
##@param[in]      item_type  The type of objects to hold. Types allowed are: VX_TYPE_RECTANGLE, VX_TYPE_KEYPOINT
##@param[in]      capacity   The maximal number of items that the array can hold.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_array is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCreateArray
##@limitations None
def vxCreateArray(context, item_type, capacity):
    return array.vxCreateArray_int(context, item_type, capacity)


##@brief Absolute difference between two images

##<BR>Creates an arithmetic absolute difference node. It computes the absolute difference between two images at pixel level.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxAbsDiffNode
##@limitations All the input and output images should be of the same format and size
def vxAbsDiffNode(graph, in1, in2, out):
    return arith.vxAbsDiffNode_int(graph, in1, in2, out)

##@brief Addition of two images

##<BR>Creates an arithmetic addition node. It performs pixel level addition of two images.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      policy    Result overflow handling is controlled with this parameter. VX_CONVERT_POLICY_SATURATE is supported
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxAddNode
##@limitations Results that are beyond the range supported will be saturated. All the input and output images should be of the same format and size
def vxAddNode(graph, in1, in2, policy, out):
    return arith.vxAddNode_int(graph, in1, in2, policy, out)

##@brief Subtraction of two images

##<BR>Creates an arithmetic subtraction node. It performs subtraction between two images at pixel level
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      policy    Result overflow handling is controlled with this parameter. VX_CONVERT_POLICY_SATURATE is supported
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxSubtractNode
##@limitations Results that are beyond the range supported will be saturated. All the input and output images should be of the same format and size
def vxSubtractNode(graph, in1, in2, policy, out):
    return arith.vxSubtractNode_int(graph, in1, in2, policy, out)

##@brief Bitwise AND of two images

##<BR>Creates a bitwise AND node. It performs a bitwise AND operation between two images at pixel level
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxAndNode
##@limitations All the input and output images should be of the same format and size
def vxAndNode(graph, in1, in2, out):
    return arith.vxAndNode_int(graph, in1, in2, out)

##@brief Bitwise OR of two images

##<BR>Creates a bitwise OR node. It performs a bitwise OR operation between two images at pixel level
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxOrNode
##@limitations All the input and output images should be of the same format and size
def vxOrNode(graph, in1, in2, out):
    return arith.vxOrNode_int(graph, in1, in2, out)

##@brief Bitwise OR of two images

##<BR>Creates a bitwise X-OR node. It performs a bitwise X-OR operation between two images at pixel level
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxXorNode
##@limitations All the input and output images should be of the same format and size
def vxXorNode(graph, in1, in2, out):
    return arith.vxXorNode_int(graph, in1, in2, out)

##@brief Minimum of two images

##<BR>Creates an arithmetic minimum node. It finds the minimum value between two images at a pixel level
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMinExtNode
##@limitations All the input and output images should be of the same format and size
def vxMinExtNode(graph, in1, in2, out):
    return arith.vxMinExtNode_int(graph, in1, in2, out)

##@brief Maximum of two images

##<BR>Creates an arithmetic maximum node. It finds the maximum value between two images at a pixel level
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMaxExtNode
##@limitations All the input and output images should be of the same format and size
def vxMaxExtNode(graph, in1, in2, out):
    return arith.vxMaxExtNode_int(graph, in1, in2, out)

##@brief Multiply two images and scales it

##<BR>Creates an arithmetic multiplication with scaling node. It performs multiplication per pixel between two images and scales the result by the scale value
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scale    Scaling factor for the output. Not supported in current release
##@param[in]      overflow_policy    Result overflow handling is controlled with this parameter. VX_CONVERT_POLICY_SATURATE is supported
##@param[in]      rounding_policy    Not supported in current release. refer ##@vx_round_policy
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMultiplyNode
##@limitations All the input and output images should be of the same format and size
def vxMultiplyNode(graph, in1, in2, scale, overflow_policy, rounding_policy, out):
    return arith.vxMultiplyNode_int(graph, in1, in2, scale, overflow_policy, rounding_policy, out)

##@brief Compare two images with compare operator

##<BR>Creates an arithmetic compare node. It performs comparison between two images with given compare operator at pixel level. If comparison is successful, 0xFF is written in the output and 0 otherwise
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      compare_operation    Compare operator to be used. Refer to VX COMPARE OPERATOR for details
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCompareExtNode
##@limitations All the input and output images should be of the same format and size
def vxCompareExtNode(graph, in1, in2, compare_operation, out):
    return arith.vxCompareExtNode_int(graph, in1, in2, compare_operation, out)

##@brief Scales first image and adds it with second image

##<BR>Creates an arithmetic compare node. It performs comparison between two images with given compare operator at pixel level. If comparison is successful, 0xFF is written in the output and 0 otherwise
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1      First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      in2      Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scale    Scale value to be used with in1
##@param[out]     out      Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxScaleAddExtNode
##@limitations Results that are beyond the range supported will be saturated. All the input and output images should be of the same format and size
def vxScaleAddExtNode(graph, in1, in2, scale, out):
    return arith.vxScaleAddExtNode_int(graph, in1, in2, scale, out)

#scalarObj type
##@brief Absolute difference between image against a value

##<BR>Creates an arithmetic absolute difference node. It computes the absolute difference between input image at pixel level and given scalar
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxAbsDiffScalarExtNode
##@limitations All the input and output images should be of the same format and size
def vxAbsDiffScalarExtNode(graph, in1, scalarObj, out):
    return arith.vxAbsDiffScalarExtNode_int(graph, in1, scalarObj, out)


##@brief Adding Image with a Scalar Value

##<BR>Creates an arithmetic addition node. It performs addition of input image at pixel level with given scalar.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[in]      policy    Result overflow handling is controlled with this parameter. VX_CONVERT_POLICY_SATURATE is supported
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxAddScalarExtNode
##@limitations Results that are beyond the range supported will be saturated. All the input and output images should be of the same format and size
def vxAddScalarExtNode(graph, in1, scalarObj, policy, out):
    return arith.vxAddScalarExtNode_int(graph, in1, scalarObj, policy, out)

##@brief Subtraction of images against a scalar value

##<BR>Creates an arithmetic subtraction node. It performs subtraction between input image at pixel level and given scalar
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph     Reference to the implementation graph
##@param[in]      in1       First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[in]      policy    Result overflow handling is controlled with this parameter. VX_CONVERT_POLICY_SATURATE is supported
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxSubtractScalarExtNode
##@limitations Results that are beyond the range supported will be saturated. All the input and output images should be of the same format and size
def vxSubtractScalarExtNode(graph, in1, scalarObj, policy, out, reverse):
    return arith.vxSubtractScalarExtNode_int(graph, in1, scalarObj, policy, out, reverse)

##@brief Bitwise AND of an image with a scalar value

##<BR>Creates a bitwise AND node. It performs a bitwise AND operation between input image at pixel level and given scalar
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph     Reference to the implementation graph
##@param[in]      in1       First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[out]     out       Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxAndScalarExtNode
##@limitations All the input and output images should be of the same format and size
def vxAndScalarExtNode(graph, in1, scalarObj, out):
    return arith.vxAndScalarExtNode_int(graph, in1, scalarObj, out)

##@brief Bitwise OR of an image with a scalar value

##<BR>Creates a bitwise OR node. It performs a bitwise OR operation between input image at pixel level and given scalar
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph     Reference to the implementation graph
##@param[in]      in1       First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[out]     out       Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxOrScalarExtNode
##@limitations All the input and output images should be of the same format and size
def vxOrScalarExtNode(graph, in1, scalarObj, out):
    return arith.vxOrScalarExtNode_int(graph, in1, scalarObj, out)

##@brief Bitwise X-OR of an image with a scalar value

##<BR>Creates a bitwise X-OR node. It performs a bitwise X-OR operation between input image at pixel level and given scalar
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph     Reference to the implementation graph
##@param[in]      in1       First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[out]     out       Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxXorScalarExtNode
##@limitations All the input and output images should be of the same format and size
def vxXorScalarExtNode(graph, in1, scalarObj, out):
    return arith.vxXorScalarExtNode_int(graph, in1, scalarObj, out)

##@brief Minimum value between an image and scalar value

##<BR>Creates an arithmetic minimum node. It finds the minimum value between input image at pixel level and given scalar at a pixel level.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph     Reference to the implementation graph
##@param[in]      in1       First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[out]     out       Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMinScalarExtNode
##@limitations All the input and output images should be of the same format and size
def vxMinScalarExtNode(graph, in1, scalarObj, out):
    return arith.vxMinScalarExtNode_int(graph, in1, scalarObj, out)

##@brief Maximum value between an image and scalar value

##<BR>Creates an arithmetic maximum node. It finds the maximum value between input image at pixel level and given scalar at a pixel level.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph     Reference to the implementation graph
##@param[in]      in1       First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar    Scalar input. This value shall be wrapped into in1 data format.
##@param[out]     out       Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMaxScalarExtNode
##@limitations All the input and output images should be of the same format and size
def vxMaxScalarExtNode(graph, in1, scalarObj, out):
    return arith.vxMaxScalarExtNode_int(graph, in1, scalarObj, out)

##@brief Multiply two images and scales it

##<BR>Creates an arithmetic multiplication with scaling node. It performs multiplication per pixel between input image at pixel level and given scalar and scales the result by the scale value
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1      First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalar   Scalar input. This value shall be wrapped into in1 data format.
##@param[in]      scale    Scaling factor for the output. Not supported in current release
##@param[in]      overflow_policy    Result overflow handling is controlled with this parameter. VX_CONVERT_POLICY_SATURATE is supported
##@param[in]      rounding_policy    Not supported in current release. refer ##@vx_round_policy
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMultiplyNode
##@limitations All the input and output images should be of the same format and size
def vxMultiplyScalarExtNode(graph, in1, scalarObj, scale, overflow_policy, rounding_policy, out):
    return arith.vxMultiplyScalarExtNode_int(graph, in1, scalarObj, scale, overflow_policy, rounding_policy, out)

##@brief Compares an image against a scalar value with compare operator

##<BR>Creates an arithmetic compare node. It performs comparison between input image at pixel level and given scalar. If comparison is successful, 0xFF is written in the output and 0 otherwise
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      in1    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      scalarObj    Second Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      compare_operation    Compare operator to be used. Refer to VX COMPARE OPERATOR for details
##@param[out]     out    Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCompareScalarExtNode
##@limitations All the input and output images should be of the same format and size
def vxCompareScalarExtNode(graph, in1, scalarObj, compare_operation, out):
    return arith.vxCompareScalarExtNode_int(graph, in1, scalarObj, compare_operation, out)

##@brief Bitwise Not on input image

##<BR>Creates a bitwise NOT node. It performs a bitwise NOT operation on input image.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     output   Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxNotNode
##@limitations All the input and output images should be of the same format and size
def vxNotNode(graph, input, output):
    return arith.vxNotNode_int(graph, input, output)

##@brief sets a single scalar value for every pixel in the image

##<BR>Creates a set Image node. Sets every element of input image with a given value.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    First Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[out]     output   Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxSetExtNode
##@limitations None
def vxSetExtNode(graph, inout, value):
    return arith.vxSetExtNode_int(graph, inout, value)

##@brief copies input image to output image

##<BR>Creates a copy node. It copies the contents of the input image to the output image.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    First Input image in one of the following formats: VX_DF_IMAGE_U8
##@param[out]     output   Output image in one of the following formats: VX_DF_IMAGE_U8
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCopyExtNode
##@limitations All the input and output images should be of the same format and size
def vxCopyExtNode(graph,input,output):
    return arith.vxCopyExtNode_int(graph,input,output)

##@brief computes the mean and standard deviation

##<BR>Creates a MeanStdDev node. It computes the mean and standard deviation of the input pixels
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    First Input image in one of the following formats: VX_DF_IMAGE_U8
##@param[out]     output   Output image in one of the following formats: VX_DF_IMAGE_U8
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMeanStdDevNode
##@limitations None
def vxMeanStdDevNode(graph, input, mean, stddev):
    return arith.vxMeanStdDevNode_int(graph, input, mean, stddev)

##@brief Convert bit depth format of the array

##<BR>Convert an array from one format to another. This API accelerates only two conversions. They are VX_DF_IMAGE_U8 to VX_DF_IMAGE_S16 and viceversa
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16
##@param[in]      output   Output image in one of the following formats: VX_DF_IMAGE_S16, VX_DF_IMAGE_U8
##@param[out]     policy   Result overflow handling is controlled with this parameter. VX_CONVERT_POLICY_SATURATE is supported
##@param[out]     shift    shifting factor in scalar. Not in use.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxConvertDepthNode
##@limitations Input and output should be of same size
def vxConvertDepthNode(graph, input, output, policy, shift):
    return bitdepth.vxConvertDepthNode_int(graph, input, output, policy, shift)


##@brief converts the color format of the input image

##@Convert an array from one format to another. This API accelerates only two conversions. They are VX_DF_IMAGE_U8 to VX_DF_IMAGE_S16 and viceversa
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    Input image in one of the formats mentioned below in table
##@param[out]     output   Output image in one of the formats mentioned below in table corresponding to the input

##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##    Input Format               Output Format
##  VX_DF_IMAGE_IYUV            VX_DF_IMAGE_RGB
##  VX_DF_IMAGE_NV12            VX_DF_IMAGE_RGB
##  VX_DF_IMAGE_NV21            VX_DF_IMAGE_RGB
##  VX_DF_IMAGE_YUYV            VX_DF_IMAGE_RGB
##  VX_DF_IMAGE_UYVY            VX_DF_IMAGE_RGB
##  VX_DF_IMAGE_IYUV            VX_DF_IMAGE_RGBX
##  VX_DF_IMAGE_NV12            VX_DF_IMAGE_RGBX
##  VX_DF_IMAGE_NV21            VX_DF_IMAGE_RGBX
##  VX_DF_IMAGE_YUYV            VX_DF_IMAGE_RGBX
##  VX_DF_IMAGE_UYVY            VX_DF_IMAGE_RGBX
##  VX_DF_IMAGE_RGB             VX_DF_IMAGE_IYUV
##  VX_DF_IMAGE_RGBX            VX_DF_IMAGE_IYUV
##  VX_DF_IMAGE_BGRX            VX_DF_IMAGE_BGR
##  VX_DF_IMAGE_RGB             VX_DF_IMAGE_GRAY
##  VX_DF_IMAGE_RGB             VX_DF_IMAGE_RGBX
##  VX_DF_IMAGE_RGBX            VX_DF_IMAGE_RGB
##  VX_DF_IMAGE_BGR             VX_DF_IMAGE_GRAY
##  VX_DF_IMAGE_BGR             VX_DF_IMAGE_RGBX
##  VX_DF_IMAGE_BGRX            VX_DF_IMAGE_IYUV
##  VX_DF_IMAGE_BGR             VX_DF_IMAGE_IYUV

##@post None
##@trace vxColorConvertNode
##@limitations None

def vxColorConvertNode(graph, input, output):
    return colorconvert.vxColorConvertNode_int(graph, input, output)

##@brief Release Context

##@Releases the OpenVX object context. All reference counted objects is garbage-collected by the return of this call. This will release all the graphs, Images, pyramid nodes, threshold objects, scalar and arrays linked to the context. No calls are possible using the parameter context after the context has been released until a new reference from vxCreateContext is returned. All outstanding references to OpenVX objects from this context are invalid after this call.
##@behavior Sync, Re-entrant
##@pre None
##@param[in]   context    Reference to the implementation graph
##<BR>Return values are documented in reverse-chronological order
##@retval         None     None
##
##@post None
##@trace vxReleaseContext
##@limitations

def vxReleaseContext(ctxt):
    context.vxReleaseContext_int(ctxt)

##@brief Computes Harris Corners of an image

##@Computes the Harris Corners of an image. The Harris Corners are computed with several parameters. The normalized Sobel kernels used for the gradient computation
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    Input image in VX_DF_IMAGE_U8 format
##@param[in]      strength_thresh    Input image in VX_DF_IMAGE_U8 format
##@param[in]      min_distance    Input image in VX_DF_IMAGE_U8 format
##@param[in]      sensitivity    Input image in VX_DF_IMAGE_U8 format
##@param[in]      gradient_size    Input image in VX_DF_IMAGE_U8 format
##@param[in]      block_size    Input image in VX_DF_IMAGE_U8 format
##@param[in]      corners    Input image in VX_DF_IMAGE_U8 format
##@param[in]      num_corners    Input image in VX_DF_IMAGE_U8 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxHarrisCornersNode
##@limitations

def vxHarrisCornersNode(graph, input, strength_thresh, min_distance, sensitivity, gradient_size, block_size, corners, num_corners):
    return corner.vxHarrisCornersNode_int(graph, input, strength_thresh, min_distance, sensitivity, gradient_size, block_size, corners, num_corners)


##@brief Detecting Faces in the input image

##@It Creates a Face detect Node.This implementation uses a fixed classifier cascade, and so can only detect faces. It detects objects of different size in an image and returns an array of rectangles
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph         Reference to the implementation graph
##@param[in]      image         Input image in VX_DF_IMAGE_U8 format
##@param[out]     rectangles    rectangles are of type VX_TYPE_RECTANGLE, to return the bounding box rectangles of the features found
##@param[inout]   num_rect      On entry of the function, the value indicates the size of the array provided for returning the          bounding   box rectangle for features. On exit of the function, the value indicates how many bounding box rectangles for features are valid in the array pointed.
##@param[in]      scale         > 1.0. Downscaling factor used between two levels of the image pyramid processed by the classifier.
##@param[in]      minSize       Minimum size of features to be detected, measured in pixel. The classifier uses a square region for detection, so minsize describes the size in either horizontal of vertical direction
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxFaceDetectNode
##@limitations

def vxFaceDetectNode(graph, image, rectangles, num_rect, scale, minSize):
    return face_detect.vxFaceDetectNode_int(graph, image, rectangles, num_rect, scale, minSize)



##@brief Creates a Lucas Kanade optical flow tracking Node.

##It selects trackable features in an image and calculate optical flow using the Lucas-Kanade pyramidal tracking
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph                     Reference to the implementation graph
##@param[in]      old_images                Input of first (old) image pyramid in VX_DF_IMAGE_U8
##@param[in]      new_images                Input of destination (new) image pyramid VX_DF_IMAGE_U8
##@param[in]      old_points                An array of key points are defined at the old_images high resolution pyramid
##@param[in]      new_points_estimates      An array of estimation on what is the output key points are defined at the new_images high resolution pyramid
##@param[in]      new_points                An image of type VX_DF_IMAGE_16SC2 and number of rows=1. On exit of the function, the array contains the coordinates of tracked features. Point coordinates are stored as pairs of 16-bit signed values, with the first value the y coordinate and the second value the x coordinate of a feature point.
##@param[in]      termination               Termination criteria. Refer to vx_termination_criteria for details
##@param[in]      epsilon                   Field epsilon for minimum movement of search window between iterations. Typical values are between 0.01 and 0.05
##@param[in]      num_iterations            maximum number of iterations for iterative search
##@param[in]      use_initial_estimate      scalar value, can be either TRUE or FALSE.
##@param[in]      window_dimension          Size of the window on which to perform the algorithm
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxOpticalFlowPyrLKNode
##@limitations

def vxOpticalFlowPyrLKNode(graph, old_images, new_images, old_points, new_points_estimates, new_points, termination, epsilon, num_iterations, use_initial_estimate, window_dimension):
    return track.vxOpticalFlowPyrLKNode_int(graph, old_images, new_images, old_points, new_points_estimates, new_points, termination, epsilon, num_iterations, use_initial_estimate, window_dimension)


##@brief Creates a reference to threshold object of given type

##Creates a reference to threshold object of given type
##@behavior Sync, Re-entrant
##@pre None
##@param[in]      context        Reference to the overall context.
##@param[in]      thresholdType    Type of threshold to create
##@param[in]      dataType            Data type of threshold values
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_threshold   An object of type vx_node is returned on success
##@retval         None           None object is returned on failure
##
##@post None
##@trace vxCreateThreshold
##@limitations

def vxCreateThreshold(context, thresholdType, dataType):
    return threshold.vxCreateThreshold_int(context, thresholdType, dataType)

##@brief Sets attributes on the threshold object

##Sets attributes on the threshold object
##@behavior Sync, Re-entrant
##@pre None
##@param[in]      threshold      The Threshold Object. A thresholding object contains the types and limit values of the thresholding required
##@param[in]      attribute      Threshold attributes to be set
##@param[in]      ptr            The pointer to the value which needs to be set for the attribute.
##@param[out]     size           the size of the data pointed by pointer
##<BR>Return values are documented in reverse-chronological order
##@retval         None           On Success
##@retval         VX_FAILURE     When the attribute could not be set or called with inappropriate values
##
##@post None
##@trace vxSetThresholdAttribute
##@limitations

def vxSetThresholdAttribute(thresh, attribute, content, size):
    return threshold.vxSetThresholdAttribute_int(thresh, attribute, content, size)



##@brief Releases a reference to threshold object

##Releases a reference to threshold object
##@behavior Sync, Re-entrant
##@pre None
##@param[in]      threshold      Reference to threshold object
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxReleaseThreshold
##@limitations

def vxReleaseThreshold(threshold):
    threshold.vxReleaseThreshold_int(threshold)

##@brief Thresholds an input image

##Thresholds an input image and produces an output Boolean image by VX_THRESHOLD_TYPE_BINARY
##@behavior Sync, Re-entrant
##@pre None
##@param[in]      graph      Reference to the implementation graph
##@param[in]      input      Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16, VX_DF_IMAGE_S32
##@param[in]      thresh     A thresholding object contains the types and limit values of the thresholding required.
##@param[out]     output     Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_S16, VX_DF_IMAGE_S32
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxThresholdNode
##@limitations Input and output should be of same format and size

def vxThresholdNode(graph, input, thresh, output):
    return threshold.vxThresholdNode_int(graph, input, thresh, output)

##@brief Resizes an image within the range of 0.5<x<2

##Resizes an image using the bilinear or bi-cubic interpolation method. It supports upscaling and downscaling of images, i.e., the destination image can be smaller or bigger than the source image in both dimensions.
##@behavior Sync, Re-entrant
##@pre None
##@param[in]      graph      Reference to the implementation graph
##@param[in]      src        Input image in VX_DF_IMAGE_U8 format
##@param[out]     dst        Output image in VX_DF_IMAGE_U8 format
##@param[in]      type       Interpolation modes supported. Only Bilinear mode is supported in this release.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node    An object of type vx_node is returned on success.
##@retval         None       None object is returned on failure.
##
##@post None
##@trace vxScaleImageNode
##@limitations



def vxScaleImageNode (graph, src, dst, type):
    return scale.vxScaleImageNode_int(graph, src, dst, type)

##@brief Creates a top-level object context for OpenVX

##@This creates a top-level object context for OpenVX.
##@behavior Sync, Re-entrant
##@pre None
##@param   None
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_context  Reference to the implementation context vx_context
##
##@post None
##@trace vxCreateContext
##@limitations

def vxCreateContext():
    return context.vxCreateContext_int()

##@brief Creates a Sobel filter node

##@Creates a Sobel filter node. It computes the gradients in x and y direction using sobel filter.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph       Reference to the implementation graph
##@param[in]      input       Input image in VX_DF_IMAGE_U8 format
##@param[out]     output_x    The output gradient in the x direction of VX_DF_IMAGE_S16 format.
##@param[out]     output_y    The output gradient in the y direction of VX_DF_IMAGE_S16 format.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxSobel3x3Node
##@limitations

def vxSobel3x3Node(graph, input, output_x, output_y):
    return filter.vxSobel3x3Node_int(graph, input, output_x, output_y)

##@brief Creates a Canny edge detection node

##@Creates a Canny edge detection node. It finds edges in an image using the Canny algorithm
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph             Reference to the implementation graph
##@param[in]      input             Input image in VX_DF_IMAGE_U8 format
##@param[out]     hyst              Threshold parameters to use for hysteresis. This should be created with threshold type as VX_THRESHOLD_TYPE_RANGE
##@param[out]     gradient_size     Gradient size to be used. Current release supports fixed size of 3.
##@param[out]     norm_type         Normalization method to use. Only VX_NORM_L1 is supported in current release.
##@param[out]     output            Output image in VX_DF_IMAGE_U8 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCannyEdgeDetectorNode
##@limitations The input and output images should be of the same size.
def vxCannyEdgeDetectorNode(graph, input, hyst, gradient_size, norm_type, output):
    #validate parameters
    return filter.vxCannyEdgeDetectorNode_int(graph, input, hyst, gradient_size, norm_type, output)

##@brief Creates a median filter node

##@Smoothens an image by blurring with Median 3x3 filter
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph             Reference to the implementation graph
##@param[in]      input             Input image in VX_DF_IMAGE_U8, VX_DF_IMAGE_S16 format
##@param[out]     output            Output image in VX_DF_IMAGE_U8, VX_DF_IMAGE_S16 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMedian3x3Node
##@limitations The input and output images should be of the same size.
def vxMedian3x3Node(graph, input, output):
    return filter.vxMedian3x3Node_int(graph, input, output)

##@brief Creates a gaussian filter node

##@Smoothens image by blurring with Gaussian 3x3 filter.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph             Reference to the implementation graph
##@param[in]      input             Input image in VX_DF_IMAGE_U8, VX_DF_IMAGE_S16 format
##@param[out]     output            Output image in VX_DF_IMAGE_U8, VX_DF_IMAGE_S16 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxGaussian3x3Node
##@limitations The input and output images should be of the same size.
def vxGaussian3x3Node(graph, input, output):
    return filter.vxGaussian3x3Node_int(graph, input, output)

##@brief Creates a median filter node

##@Smoothens an image by blur filter
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph             Reference to the implementation graph
##@param[in]      input             Input image in VX_DF_IMAGE_U8 format
##@param[out]     output            Output image in VX_DF_IMAGE_U8 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
#@trace vxBlur3x3ExtNode
##@limitations The input and output images should be of the same size.
def vxBlur3x3ExtNode(graph, input, output):
    return filter.vxBlur3x3ExtNode_int(graph, input, output)


##@brief Creates a Gaussian 5x5 filter node

##@Smoothens an image using Gaussian 5x5 filter
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph             Reference to the implementation graph
##@param[in]      input             Input image in VX_DF_IMAGE_U8 format
##@param[out]     output            Output image in VX_DF_IMAGE_U8 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxGaussian5x5ExtNode
##@limitations The input and output images should be of the same size.
def vxGaussian5x5ExtNode(graph, input, output):
    return filter.vxGaussian5x5ExtNode_int(graph, input, output)

##@brief Creates a graph

##@Creates an empty graph
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   context       Reference to the overall  context
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_context is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCreateGraph
##@limitations

def vxCreateGraph(context):
    return graph.vxCreateGraph_int(context)

##@brief Verifies the state of the graph

##@Verifies the state of the graph before it is executed. This is useful to catch programmer errors and contract errors. If not verified, the graph verifies before being processed. Memory for data objects is not guaranteed to exist before this call. This is not supported in this release.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph       Reference to the overall  graph
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  VX_SUCCESS on success.
##@retval         None     VX_FAILURE is returned on failure.
##
##@post None
##@trace vxVerifyGraph
##@limitations
def vxVerifyGraph(grph):
    return graph.vxVerifyGraph_int(grph)

##@brief Creates Distribution

##@Creates a reference to a 1D Distribution of a consecutive interval [offset, offset + range - 1] defined by a start offset and valid range, divided equally into numBins parts.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   context    Reference to the implementation context.
##@param[in]      numBins    The number of bins in the distribution. Current release supports fixed 256 bins.
##@param[in]      offset     The start offset into the range value that marks the beginning of the 1D Distribution. Current release fixes this at 0
##@param[in]      range      The total number of the consecutive values of the distribution interval. Current release fixes this at 256
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_distribution  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCreateDistribution
##@limitations

def vxCreateDistribution(context,numBins,offset,range):
    return histogram.vxCreateDistribution_int(context,numBins,offset,range)

##@brief Releases a reference to a distribution object

##@Releases a reference to a distribution object
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   distribution    Reference to the distribution
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_distribution  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxReleaseDistribution
##@limitations
def vxReleaseDistribution(distribution):
    histogram.vxReleaseDistribution_int(distribution)

##@brief Creates a Histogram node

##@Creates a Histogram node. It computes the histogram of the input images and stores it in output distribution.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph             Reference to the implementation graph
##@param[in]      input             Input image in VX_DF_IMAGE_U8 format
##@param[out]      distribution      The output distribution
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node           An object of type vx_node is returned on success.
##@retval         None              None object is returned on failure.
##
##@post None
##@trace vxHistogramNode
##@limitations     Current release supports only one combination of parameters for distribution (numBins=256,offset=0,range=256)
def vxHistogramNode(graph, input, distribution):
    return histogram.vxHistogramNode_int(graph, input, distribution)

##@brief Creates an opaque reference to an image buffer.

##It creates an opaque reference to an image buffer.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   context    Reference to implementation context.
##@param[in]      width      Image width in pixels
##@param[in]      height     Image height in pixels
##@param[in]      type       Code that represents the format of the image and the color space. Refer to VX_DF_IMAGE formats for details.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_image   An object of type vx_image is returned on success
##@retval         None       None object is returned on failure.
##
##@post None
##@trace vxCreateImage
##@limitations None
def vxCreateImage(context, width, height, type):
    return image.vxCreateImage_int(context, width, height, type)

##@brief Creates a reference to an image buffer with no direct access to user.

##Creates an opaque reference to an image buffer with no direct user access. This function allows setting the image width, height, or format.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to implementation graph.
##@param[in]      width      Image width in pixels
##@param[in]      height     Image height in pixels
##@param[in]      type       Code that represents the format of the image and the color space. Refer to VX_DF_IMAGE formats for details.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_image   An object of type vx_image is returned on success
##@retval         None       None object is returned on failure.
##
##@post None
##@trace vxCreateImage
##@limitations None
def vxCreateVirtualImage(graph, width, height, type):
    return image.vxCreateVirtualImage_int(graph, width, height, type)


##@brief Creates an opaque reference to a region of interest from the image buffer.

##It creates an opaque reference to a region of interest from the image buffer.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   img       Reference to implementation context.
##@param[in]      rect      REct is an object which has ROI details: start_x, start_y and end_x, end_y of the image
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_image   An object of type vx_image is returned on success
##@retval         None       None object is returned on failure.
##
##@post None
##@trace vxCreateImageFromROI
##@limitations None
def vxCreateImageFromROI (img, rect):
    return image.vxCreateImageFromROI_int(img, rect)


##@brief Computes the integral image of the input

##@Computes the integral image of the input. Each output pixel is the sum of the corresponding input pixel and all other pixels above and to the left of it.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]    context             Reference to the implementation graph
##@param[in]       input               Input image in VX_DF_IMAGE_U8 format
##@param[out]      output              The output distribution
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node           An object of type vx_node is returned on success.
##@retval         None              None object is returned on failure.
##
##@post None
##@trace vxIntegralImageNode
##@limitations     Input and output should be of same size. Runs only on single core.
def vxIntegralImageNode(graph, input, output):
    return integral.vxIntegralImageNode_int(graph, input, output)


##@brief Dilates an image using a 3x3 kernel

##@ Grows the white space in the image, by dilating an image using a 3x3 kernel
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_U16, VX_DF_IMAGE_S32
##@param[out]     output   Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_U16, VX_DF_IMAGE_S32
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxDilate3x3Node
##@limitations Input and output should be of same format and size

def vxDilate3x3Node (graph, input, output):
    return morph.vxDilate3x3Node_int(graph, input, output)


##@brief Erodes an image using a 3x3 kernel

##@ Shrinks the white space in the image, by eroding an image using a 3x3 kernel
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_U16, VX_DF_IMAGE_S32
##@param[out]     output   Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_U16, VX_DF_IMAGE_S32
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxErode3x3Node
##@limitations Input and output should be of same format and size

def vxErode3x3Node (graph, input, output):
    return morph.vxErode3x3Node_int(graph, input, output)

##@brief Performs morphological transformations

##@ Performs morphological transformations using 3x3 kernel for the mode selected for given number of iterations.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph    Reference to the implementation graph
##@param[in]      input    Input image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_U16, VX_DF_IMAGE_S32
##@param[out]     output   Output image in one of the following formats: VX_DF_IMAGE_U8, VX_DF_IMAGE_U16, VX_DF_IMAGE_S32
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxMorphologyExtNode
##@limitations Input and output should be of same format and size
def vxMorphologyExtNode (graph, input, output, operation, iterations):
        return morph.vxMorphologyExtNode_int(graph, input, output, operation, iterations)


##@brief Creates a Pedestrian detect Node
##@Creates a Pedestrian detect Node. It detects objects of different size in an image and returns an array of rectangles using HOG descriptors.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]    graph                Reference to the implementation graph
##@param[in]       image                Input image in VX_DF_IMAGE_U8 format
##@param[out]      rectangles           rectangles are of type VX_TYPE_RECTANGLE, to return the bounding box rectangles of the features found
##@param[inout]    num_rect             On entry of the function, the value indicates the size of the array provided for returning the bounding box rectangle for features. On exit of the function, the value indicates how many bounding box rectangles for features are valid in the array pointed.
##@param[in]       scale                > 1.0. Downscaling factor used between two levels of the image pyramid processed by the classifier.
##@param[in]       levels               number of levels
##@param[in]       quality              Threshold for the distance between features and SVM classifying plane. Typical values are 1.0 … 3.0. Smaller values result in more features found.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node           An object of type vx_node is returned on success.
##@retval         None              None object is returned on failure.
##
##@post None
##@trace vxPedestrianDetectNode
##@limitations
def vxPedestrianDetectNode(graph, image, rectangles, num_rect, scale, levels, quality):
    return pedestrian_detect.vxPedestrianDetectNode_int(graph, image, rectangles, num_rect, scale, levels, quality)




##@brief Generates Configuration data

##@It generates the configuration data for the context and the number of cores in the directory specified.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   context      Reference to the implementation graph
##@param[in]      directory    Input image in VX_DF_IMAGE_U8 format
##@param[in]      numcores     Accumulation image in VX_DF_IMAGE_U16 format
##<BR>Return values are documented in reverse-chronological order
##@retval         None         None
##
##@post None
##@trace  vxGenerateConfig
##@limitations None

def vxGenerateConfig(context,directory,numcores,dump_header=False):
    process.vxGenerateConfig_int(context,directory,numcores,dump_header=False)


##@brief Accumulation of two images

##@Creates a reference to a pyramid object of the supplied number of levels.
##@behavior Sync, Re-entrant
##@pre None
##@param[in]   context          Reference to the overall context
##@param[in]   levels           Number of levels desired. This shall be a non-zero value.
##@param[in]   scale            Used to indicate the scale between pyramid levels. Refer to vx_scalar_type for details
##@param[in]   width            Width of the 0th level image in pixels
##@param[in]   height           Height of the 0th level image in pixels
##@param[in]   type             Format type of all images in the pyramid. Refer to vx_scalar_type for details
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_pyramid    An object of type vx_pyramid is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxCreatePyramid
##@limitations
def vxCreatePyramid(context,levels,scale,width,height,type):
    return pyramid.vxCreatePyramid_int(context,levels,scale,width,height,type)


##@brief Retrieves a level of the pyramid as a vx_image, which can be used elsewhere.

##@Retrieves a level of the pyramid as a vx_image, which can be used elsewhere.
##@behavior Sync, Re-entrant
##@pre None
##@param[in]   pyramid          Reference to the overall context
##@param[in]   idx              Number of levels desired. This shall be a non-zero value.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_image      Image according to the level specified
##@retval         None          None object is returned on failure.
##
##@post None
##@trace vxGetPyramidLevel
##@limitations
def vxGetPyramidLevel(pyramid,idx):
    return pyramid.vxGetPyramidLevel_int(pyramid,idx)
##@endif

##@brief creates a gaussian Image pyramid node

##@Creates a node for a Gaussian Image Pyramid. This vision function creates the Gaussian image pyramid from the input image using the particular Gaussian 5x5 Kernel for VX_SCALE_PYRAMID_HALF. And for VX_SCALE_PYRAMID_ORB, this API filters the data using Gaussian 5x5 filter and scales the image down to form the pyramid according to the levels mentioned.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph      Reference to the implementation graph
##@param[in]      input      Input image in VX_DF_IMAGE_U8 format
##@param[out]     pyramiid    Pyramid of type VX_DF_IMAGE_U8 to store the result.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxGaussianPyramidNode
##@limitations Only kernel size 3 and 5 are supported
def vxGaussianPyramidNode(graph, input, pyramiid):
    return pyramid.vxGaussianPyramidNode_int(graph, input, pyramiid)

##@brief creates a half scale gaussian node

##@Creates a node for the Halfscale Gaussian Image node. This API performs a Gaussian Blur on an image then half-scales it for kernel size=3. For kernel Size =5, it calls the Gaussian pyramid functionality internally
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph         Reference to the implementation graph
##@param[in]      input         Input image in VX_DF_IMAGE_U8 format
##@param[out]     output        Pyramid of type VX_DF_IMAGE_U8 to store the result
##@param[in]      kernel_size   size of the filter kernel used.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxHalfScaleGaussianNode
##@limitations Kernel Size = 3 and 5 are supported. VX_INTERPOLATION_BILINEAR mode is supported in the release
def vxHalfScaleGaussianNode(graph, input, output, kernel_size):
    return pyramid.vxHalfScaleGaussianNode_int(graph, input, output, kernel_size)

##@brief Creates a node for a Laplacian Image Pyramid

##@Creates a node for a Laplacian Image Pyramid. This vision function creates the Laplacian image pyramid from the input image using the particular Gaussian 3x3 Kernel for VX_SCALE_PYRAMID_HALF. The input image is smoothed with a Gaussian 3x3-filter and down sampled by rejecting even rows and columns. It forms an output laplacian pyramid and the smoothed lowest resolution of the image.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]    graph         Reference to the implementation graph
##@param[in]       input         Input image in VX_DF_IMAGE_U8 format
##@param[out]      laplacian     Pyramid of type VX_DF_IMAGE_S16 to store the result.
##@param[out]      output        The lowest resolution image of type VX_DF_IMAGE_S16 necessary to reconstruct the input image from the pyramid
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxLaplacianPyramidNode
##@limitations Only Gaussian 3x3 filter is used.

def vxLaplacianPyramidNode(graph, input, laplacian, output):
    return pyramid.vxLaplacianPyramidNode_int(graph, input, laplacian, output)

##@brief Creates a node for a Laplacian Image Pyramid

##@Creates a node for a Upsampling.This API upsamples the input image by twice wrt height and width as its output image
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]    graph         Reference to the implementation graph
##@param[in]       input         Input image in VX_DF_IMAGE_U8 format
##@param[out]      output        Output image in VX_DF_IMAGE_U8 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxUpSampleBy2x2Node
##@limitations
def vxUpSampleBy2x2Node(graph, input, output):
    return pyramid.vxUpSampleBy2x2Node_int(graph, input, output)

##@brief Creates a node to reconstruct the image from the laplacian pyramid

##@Creates a node to reconstruct the image from the laplacian pyramid. This vision function reconstructs the image of the highest possible resolution using a Laplacian pyramid and the lowest resolution image. The input image is added to the last level of the Laplacian pyramid.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   graph         Reference to the implementation graph
##@param[in]      laplacian     The laplacian pyramid of type VX_DF_IMAGE_S16
##@param[in]      input         The lowest resolution image of type VX_DF_IMAGE_S16 necessary to reconstruct the input image from the pyramid.
##@param[out]     output        The output image of type VX_DF_IMAGE_U8 with the highest possible resolution reconstructed from the Laplacian pyramid.
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_node  An object of type vx_node is returned on success.
##@retval         None     None object is returned on failure.
##
##@post None
##@trace vxLaplacianReconstructNode
##@limitations

def vxLaplacianReconstructNode(graph, laplacian, input, output):
    return pyramid.vxLaplacianReconstructNode_int(graph, laplacian, input, output)


##@brief Creates a scalar object

##@It accumulates the pixel data from input and accum images and updates the accumulated values on ##@the accumulation image.
##@behavior Sync, Re-entrant
##@pre None
##@param[inout]   context       Reference to the implementation graph
##@param[in]      data_type     Input image in VX_DF_IMAGE_U8 format
##@param[in]      Init_value    Input image in VX_DF_IMAGE_U8 format
##<BR>Return values are documented in reverse-chronological order
##@retval         vx_scalar     An object of type vx_scalar is returned on success
##@retval         None          None object is returned on failure.
##
##@post None
##@trace vxCreateScalar
##@limitations
def vxCreateScalar(context, data_type, initial_value):
    return scalar.vxCreateScalar_int(context, data_type, initial_value)

##@}
