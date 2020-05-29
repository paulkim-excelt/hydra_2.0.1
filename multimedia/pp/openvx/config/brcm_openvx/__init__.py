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

##@defgroup grp_openvx_py_pkg OpenVX Python Package
##@ingroup grp_openvx

##@addtogroup grp_openvx_py_pkg
##@{
##@section sec_openvx_overview Overview
##OpenVX is an open, royalty-free standard for cross platform acceleration of computer vision applications. OpenVX enables performance and power-optimized computer vision processing for ADAS applications.
##<BR>To minimize CPU utilization on Broadcom hardware, OpenVX acceleration is achieved in two stages.First stage consists of Broadcom OpenVX Python package which exposes Khronos-OpenVX like APIs to ease development of new graphs for end users. This package assists in generating configuration data needed for accelerating the graph on Broadcom hardware. Second stage consists of accelerating the graph with the generated configuration data. A thin OpenVX library runs on Broadcom CPU to assist executing the graph using the configuration data.
##<BR>
##@image html openvx_overview0.jpg
##<BR>Broadcom OpenVX Python package is Khronos-OpenVX like python utility to generate configuration data for accelerating OpenVX graphs on Broadcom hardware. Client implements one/more OpenVX graphs within one OpenVX application (python) and generates the configuration data needed for acceleration on Broadcom hardware.
##<BR>
##@image html openvx_overview1.jpg
##<BR>The generated configuration is used on Broadcom hardware by corresponding application to ##accelerate the graphs. The application on the hardware uses Broadcom OpenVX Library interface (instead of OpenVX interface provided by Khronos group).
##@subsection subsec_openvx_example Sample
##@todo ADD STATE MACHINE
##@section sec_openvx_fsm State Machine
##@todo ADD STATE MACHINE
##@section sec_openvx_seq Sequence Diagrams
##@todo ADD SEQUENCE DIAGRAM
##@section sec_openvx_perf Performance & Memory
##| Performance       | Description                                       |
##|:-----------------:|:-------------------------------------------------:|
##| CPU Load          |   Update CPU load here                            |
##| Memory Bandwidth  |   Update Memory Bandwidth here                    |
##| HW Utilization    |   Update Hardware utilization here                |
##todo
##@limitations None
##@file __init__.py
##@brief This files has variables defined at the package level
##This file contains the content to export selected portions of the package under more convenient name, hold convenience functions, etc.

__version__=0.92


from  openvx_interface import *
from random import randint
import logging
logging.basicConfig(level=logging.WARNING)

#FORMAT = "[%(funcName)20s() ] %(message)s"
#logging.basicConfig(format=FORMAT,level=logging.WARNING)


##@name vx_status
##@{
##@brief This defines status codes

VX_SUCCESS = 0  ##<@brief Indicates a Success
VX_FAILURE = 1  ##<@brief Indicates a failure
##@}


##@name vx_bool
##@{
##@brief This defines boolean selection macros

VX_TRUE = 1     ##<@brief True Value
VX_FALSE = 0    ##<@brief False Value
##@}



##@name vx_scalar_type
##@{
##@brief This is to be used to create a vx_scalar_type object of appropriate type

#scalar data types
VX_TYPE_INVALID       = 0x10000000 ##@Invalid type value.
VX_TYPE_START         = 0x10000000 ##@start type value.
VX_TYPE_CHAR          = 0x10000001 ##@vx_char
VX_TYPE_INT8          = 0x10000002 ##@vx_int8
VX_TYPE_UINT8         = 0x10000003 ##@vx_uint8
VX_TYPE_INT16         = 0x10000004 ##@vx_int16
VX_TYPE_UINT16        = 0x10000005 ##@vx_uint16
VX_TYPE_INT32         = 0x10000006 ##@vx_int32
VX_TYPE_UINT32        = 0x10000007 ##@vx_uint32
VX_TYPE_INT64         = 0x10000008 ##@vx_int64
VX_TYPE_UINT64        = 0x10000009 ##@vx_uint64
VX_TYPE_FLOAT32       = 0x1000000A ##@vx_float32
VX_TYPE_FLOAT64       = 0x1000000B ##@vx_float64
VX_TYPE_ENUM          = 0x1000000C ##@vx_enum
VX_TYPE_SIZE          = 0x1000000D ##@vx_size
VX_TYPE_DF_IMAGE      = 0x1000000E ##@vx_df_image
VX_TYPE_BOOL          = 0x1000000F ##@vx_bool
VX_TYPE_SCALAR_MAX    = 0x10000010 ##@Floating value for comparison between OpenVX scalars and OpenVX structs.
VX_TYPE_RECTANGLE     = 0x10000011 ##@vx_rectangle_t
VX_TYPE_KEYPOINT      = 0x10000012 ##@vx_keypoint_t
VX_TYPE_COORDINATES2D = 0x10000013 ##@vx_coordinates2d_t
VX_TYPE_COORDINATES3D = 0x10000014 ##@vx_coordinates3d_t
##@}

##@name VX_MORPH_EXT
##@{
##@brief This defines the list of morphology operations supported

VX_MORPH_ERODE_EXT    = 0x20000000  ##@Erosion
VX_MORPH_DILATE_EXT   = 0x20000001  ##@Dilation
VX_MORPH_OPEN_EXT     = 0x20000002  ##@Erosion followed by dilation
VX_MORPH_CLOSE_EXT    = 0x20000003  ##@Dilation followed by erosion
VX_MORPH_GRADIENT_EXT = 0x20000004  ##@Difference between the dilation and the erosion of a given image
VX_MORPH_TOPHAT_EXT   = 0x20000005  ##@Difference between input image and Opening of the image
VX_MORPH_BLACKHAT_EXT = 0x20000006  ##@Difference between the closing of the input image and input image.
##@}

##@name VX_DF_IMAGE
##@{
##@brief Describes the pixel format and color space

VX_DF_IMAGE_VIRT  = 0 ##@A virtual image of no defined type.
VX_DF_IMAGE_RGB   = 1 ##@A single plane of 24-bit pixel as 3 interleaved 8-bit units of R then G then B data. This uses the BT709 full range by default.
VX_DF_IMAGE_RGBX  = 2##@A single plane of 32-bit pixel as 4 interleaved 8-bit units of R then G then B data, then a don’t care byte. This uses the BT709 full range by default.
VX_DF_IMAGE_NV12  = 3##@A 2-plane YUV format of Luma (Y) and interleaved UV data at 4:2:0 sampling. This uses the BT709 full range by default.
VX_DF_IMAGE_NV21  = 4 ##@A 2-plane YUV format of Luma (Y) and interleaved VU data at 4:2:0 sampling. This uses the BT709 full range by default.
VX_DF_IMAGE_UYVY  = 5##@A single plane of 32-bit macro pixel of U0, Y0, V0, Y1 bytes. This uses the BT709 full range by default.
VX_DF_IMAGE_YUYV  = 6##@A single plane of 32-bit macro pixel of Y0, U0, Y1, V0 bytes. This uses the BT709 full range by default.
VX_DF_IMAGE_IYUV  = 7##@A 3 plane of 8-bit 4:2:0 sampled Y, U, V planes. This uses the BT709 full range by default.
VX_DF_IMAGE_YUV4  = 8##@A 3 plane of 8 bit 4:4:4 sampled Y, U, V planes. This uses the BT709 full range by default.
VX_DF_IMAGE_U8    = 9##@A single plane of unsigned 8-bit data. The range of data is not specified, as it may be extracted from a YUV or generated.
VX_DF_IMAGE_U16   = 10##@A single plane of unsigned 16-bit data. The range of data is not specified, as it may be extracted from a YUV or generated.
VX_DF_IMAGE_S16   = 11##@A single plane of signed 16-bit data. The range of data is not specified, as it may be extracted from a YUV or generated.
VX_DF_IMAGE_U32   = 12##@A single plane of unsigned 32-bit data. The range of data is not specified, as it may be extracted from a YUV or generated.
VX_DF_IMAGE_S32   = 13##@A single plane of signed 32-bit data. The range of data is not specified, as it may be extracted from a YUV or generated
VX_DF_IMAGE_S32C2 = 14##@A biplane of signed 32 bit data.
VX_DF_IMAGE_S16C2 = 15##@A biplane of signed 16 bit data.
VX_DF_IMAGE_U64   = 16##@A single plane of unsigned 64-bit data.
VX_DF_IMAGE_BGR_EXT = 17 ##@A single plane of 24-bit pixel as 3 interleaved 8-bit units of B then G then R data.
VX_DF_IMAGE_BGRX_EXT = 18##@A single plane of 32-bit pixel as 4 interleaved 8-bit units of B then G then R data, then a don’t care byte.
VX_DF_IMAGE_GRAY_EXT = 19##@a Monochrome plane of 8 bit per pixel
VX_DF_IMAGE_HSV_EXT = 20##@ HSV format data
#If we change the values of the above macros,colorconvert.py should be revisited once
##@}



##@name vx_round_policy
##@{
##@brief This defines rounding policy macros

VX_ROUND_POLICY_TO_ZERO = 0x20000010 ##@When scaling, this truncates the least significant values that are lost in operations.
VX_ROUND_POLICY_TO_NEAREST_EVEN = 0x20000011 ##@When scaling, this rounds to nearest even output value.
##@}

##@name vx_convert_policy
##@{
##@brief This defines conversion policy macros

VX_CONVERT_POLICY_WRAP     = 0x20000020 ##@Results are the least significant bits of the output operand, as if stored in two’s complement binary format in the size of its bit-depth.
VX_CONVERT_POLICY_SATURATE = 0x20000021 ##@Results are saturated to the bit depth of the output operand.
##@}

##@name vx_termination_criteria
##@{
##@brief This defines a list of termination criteria

VX_TERM_CRITERIA_ITERATIONS = 0x20000030 ##@Indicates a termination after a set number of iterations.
VX_TERM_CRITERIA_EPSILON    = 0x20000031 ##@Indicates a termination after matching against the value of eplison provided to the function.
VX_TERM_CRITERIA_BOTH       = 0x20000032 ##@Indicates that both an iterations and eplison method are employed. Whichever one matches first causes the termination.
##@}

##@name VX_PYRAMID_SCALE
##@{
##@brief This defines pyramid scale factor

VX_SCALE_PYRAMID_HALF =  (0.5) ##@Use to indicate a half-scale pyramid.
VX_SCALE_PYRAMID_ORB  = (0.8408964) ##@Use to indicate a ORB scaled pyramid whose scaling factor is 1/4thsqrt(2)
##@}

##@name VX COMPARE OPERATOR
##@{
##@brief This defines a list operators for compare operation

VX_CMP_EQ = 0x20000040 ##@in1 is equal to in2.
VX_CMP_GT = 0x20000041 ##@in1 is greater than in2.
VX_CMP_GE = 0x20000042 ##@in1 is greater than or equal to in2
VX_CMP_LT = 0x20000043 ##@in1 is less than in2.
VX_CMP_LE = 0x20000044 ##@in1 is less than or equal to in2.
VX_CMP_NE = 0x20000045 ##@in1 is unequal to in2.
##@}



##@name VX INTERPOLATION TYPE
##@{
##@brief This defines the interpolation types used for resizing the image

VX_INTERPOLATION_NEAREST_NEIGHBOR = 0x20000050  ##@Reserved
VX_INTERPOLATION_BILINEAR         = 0x20000051  ##@Output values are defined by bilinear interpolation between the pixels whose centers are closest to the sample position, weighted linearly by the distance of the sample from the pixel centers.
##@}

##@name VX_THRESHOLD_TYPE
##@{
##@brief Possible threshold types supported in thresholding

VX_THRESHOLD_TYPE_START  = 0x20000060
VX_THRESHOLD_TYPE_BINARY = 0x20000060   ##@Threshold with only one value
VX_THRESHOLD_TYPE_RANGE  = 0x20000061   ##@Threshold with two values: upper and lower, not supported yet
##@}

##@name vx_threshold-attribute
##@{
##@brief Threshold attributes

VX_THRESHOLD_TYPE            = 0x20000070   ##@Threshold type
VX_THRESHOLD_THRESHOLD_VALUE = 0x20000071   ##@Threshold value if it is a binary type. Use a vx_int32 parameter
VX_THRESHOLD_THRESHOLD_LOWER = 0x20000072   ##@Lower bound when the threshold type is range. Use a vx_int32 parameter
VX_THRESHOLD_THRESHOLD_UPPER = 0x20000073   ##@Upper bound when the threshold type is range. Use a vx_int32 parameter
VX_THRESHOLD_TRUE_VALUE      = 0x20000074   ##@The value of the TRUE threshold (default value is 255). Use a vx_int32 parameter
VX_THRESHOLD_FALSE_VALUE     = 0x20000075   ##@The value of the FALSE threshold (default value is 0). Use a vx_int32 parameter.
VX_THRESHOLD_DATA_TYPE       = 0x20000076   ##@The data type of the threshold’s value. Described in VX_THRESHOLD_TYPE
##@}

##@name vx_norm_type
##@{
##@brief This defines the normalization type

VX_NORM_L1                   = 0x20000080 ##@The L1 normalization.
VX_NORM_L2                   = 0x20000081 ##@The L2 normalization.
##@}


##@}
