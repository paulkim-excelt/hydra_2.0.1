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

#  CONFIDENTIAL AND PROPRIETARY INFORMATION
#  Copyright 2004 - 2016 videantis GmbH
#  All Rights Reserved
#
# This document contains confidential and proprietary information of videantis
# GmbH and is protected by copyright, trade secret and other local, state,
# federal, and international laws. Its receipt or possession does not convey
# any rights to reproduce, transfer, disclose or publish its contents, or to
# manufacture, commercially or non-commercially use or sell anything it may
# describe or contain. Reproduction, disclosure or any use without specific
# written authorization of videantis GmbH or an individual license agreement
# with videantis GmbH is strictly forbidden.
#
####################################################################################

import brcm_openvx
import image
import struct
import math
import graph
import logging
import numbers
import defn

class OpticalFlowPyrLKNode(graph.VXNode):
    def __init__(self, graph, old_images, new_images, old_points, new_points_estimates, new_points, max_count, epsilon, winsize):
        logger = logging.getLogger(__name__)
        ret0 = (old_images.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret1 = (new_images.type == brcm_openvx.VX_DF_IMAGE_U8)
        ret2 = (old_points.type == brcm_openvx.VX_TYPE_KEYPOINT)
        ret3 = (new_points.type == brcm_openvx.VX_TYPE_KEYPOINT)
        ret4 = (new_points_estimates.type == brcm_openvx.VX_TYPE_KEYPOINT)
        ret = (ret0 & ret1 & ret2 & ret3 & ret4)
        if ret is not True:
            logger.error('OpticalFlowPyrLKNode: input/output type constraints are not met')
            raise  TypeError
        if ((old_images.width != new_images.width) or (old_images.height != new_images.height) or (old_images.width %8 != 0)):
            logger.error('OpticalFlowPyrLKNode: image resolution constraints are not met oldimgW = %s oldimgH = %s newImgW = %s newImgH = %s'%(old_images.width,old_images.height,new_images.width,new_images.height))
            raise ValueError

        self.oldPyramid = old_images
        self.newPyramid = new_images
        self.old_points = old_points
        self.new_points_estimates = new_points_estimates
        self.new_points = new_points
        self.max_count = max_count
        self.epsilon = epsilon
        self.winsize = winsize
        self.pointCount = 3000
        self.setNumJobs(new_images.getNumLevels())
        self.max_level = new_images.getNumLevels() - 1
        high_res_img = new_images.getImage(0)
        (imgStore, imgW, imgH, imgS) = high_res_img.getDetails()
        self.tmpOld = image.VXImage(graph, (imgW >> 4) - 1 , (imgH >> 4) - 1, brcm_openvx.VX_DF_IMAGE_S32C2, None)
        self.tmpNew = image.VXImage(graph, (imgW >> 4) - 1 , (imgH >> 4) - 1, brcm_openvx.VX_DF_IMAGE_S32C2, None)
    def createMailBoxLocal(self, total_cores, prev_img, curr_img, level):
        msg = []
        num_points_per_core_round =  int(self.pointCount)/int(total_cores)
        num_key_points_per_core_round =  int(self.tmpOld.getWidth() * self.tmpOld.getHeight() )/int(total_cores)
        (prevStore, prevW, prevH, prevS) = prev_img.getDetails()
        (currStore, currW, currH, currS) = curr_img.getDetails()
        (prevPtsStore, prevPtsW, prevPtsH, prevPtsS) = self.old_points.getDetails()
        (currPtsStore, currPtsW, currPtsH, currPtsS) = self.new_points.getDetails()
        for core_num in range(total_cores):
            num_points_per_core = int(num_points_per_core_round)
            num_key_points_per_core = int(num_key_points_per_core_round)
            if core_num == total_cores - 1:
                num_points_per_core = num_points_per_core + (int(self.pointCount)%int(total_cores))
                num_key_points_per_core = num_key_points_per_core + (int(self.tmpOld.getWidth() * self.tmpOld.getHeight())%int(total_cores))
            cmd = graph.VXCommand()
            cmd.appendStorage(prevStore, 0, 8)
            cmd.appendStorage(currStore, 0, 12)
            offset = int(((core_num*num_points_per_core_round))* 8)
            keyOffset = int(((core_num*num_key_points_per_core_round))* 28)
            cmd.appendStorage(self.tmpOld.getStorage(), offset, 24)
            if (level == self.max_level):
                cmd.appendOptFlowPreProc(prevPtsStore, currPtsStore, keyOffset, self.tmpOld.getStorage(), offset, min(num_key_points_per_core,num_points_per_core))
            offset = int(((core_num*num_points_per_core_round))* 8)
            cmd.appendStorage(self.tmpNew.getStorage(), offset, 28)
            if (level == 0):
                cmd.appendOptFlowPostProc(currPtsStore, prevPtsStore, keyOffset, self.tmpNew.getStorage(), offset, min(num_key_points_per_core,num_points_per_core))
                cmd.appendOptFlowAddPoints(currPtsStore, self.new_points_estimates.getStorage(), min(self.new_points_estimates.getMaxCount(), self.new_points.getMaxCount()))
            cmd.appendMsgType(23)
            cmd.appendMbox32be(0) #pad0_h
            cmd.appendMbox32be(self.winsize)
            cmd.appendMbox32le(0)
            cmd.appendMbox32le(0)
            cmd.appendMbox32be(prevS)
            cmd.appendMbox32be(prevH)
            cmd.appendMbox32le(0)
            cmd.appendMbox32le(0)
            cmd.appendMbox32be(num_points_per_core)
            cmd.appendMbox32be(0)
            cmd.appendMbox32be(level)
            cmd.appendMbox32be(self.max_level)
            cmd.appendMbox32be(self.epsilon)
            cmd.appendMbox32be(self.max_count)
            msg = msg + cmd.getData()
        return msg

    def createMailBox(self,total_cores):
        msg = []
        for i in range(self.getNumJobs()):
            levelID = self.getNumJobs() - i - 1;
            prev_img = self.oldPyramid.getImage(levelID)
            curr_img = self.newPyramid.getImage(levelID)
            msg = msg + self.createMailBoxLocal(total_cores,prev_img,curr_img,levelID)
        return msg


def vxOpticalFlowPyrLKNode_int(graph, old_images, new_images, old_points, new_points_estimates, new_points, termination, epsilon, num_iterations, use_initial_estimate, window_dimension):
    #validate parameters
    logger = logging.getLogger(__name__)
    ret0 = defn.verifyGraph(graph)
    ret1 = defn.verifyPyramid(old_images)
    ret2 = defn.verifyPyramid(new_images)
    ret3 = defn.verifyArray(old_points)
    ret4 = defn.verifyArray(new_points)
    ret5 = len(window_dimension) == 2
    ret6 = defn.verifyArray(new_points_estimates)
    ret7 = window_dimension[0] == window_dimension[1]
    ret8 = (window_dimension[0] == 11) | (window_dimension[0] == 13)
    ret = ret0 & ret1 & ret2 & ret3 & ret4 & ret5 & ret6 & ret7 & ret8
    if ret is not True:
        logger.error('vxOpticalFlowPyrLKNode: one or all parameters are wrong')
        raise AttributeError
    max_count = 30
    final_epsilon = 0.01
    if ((termination < brcm_openvx.VX_TERM_CRITERIA_ITERATIONS) or (termination >brcm_openvx.VX_TERM_CRITERIA_BOTH)):
        logger.error('vxOpticalFlowPyrLKNode: termination not supported')
        raise ValueError

    if ((isinstance(epsilon,numbers.Number) == False) or (isinstance(num_iterations,numbers.Number)==False)):
        logger.error('vxOpticalFlowPyrLKNode: epsilon is not a valid number')
        raise TypeError
    if termination is brcm_openvx.VX_TERM_CRITERIA_ITERATIONS or termination is brcm_openvx.VX_TERM_CRITERIA_BOTH:
        max_count = min(max(num_iterations, 0), 100)

    if termination is brcm_openvx.VX_TERM_CRITERIA_EPSILON or termination is brcm_openvx.VX_TERM_CRITERIA_BOTH:
        final_epsilon = min(max(epsilon, 0.), 10.);

    final_epsilon = final_epsilon * final_epsilon
    logger.info('vxOpticalFlowPyrLKNode:use_initial_estimate is not used currently')
    final_epsilon = int(final_epsilon * (1 << 28))
    node = defn.create_dict(defn.VX_NODE,graph)
    node['VX_DATA'] = OpticalFlowPyrLKNode(graph['VX_DATA'], old_images['VX_DATA'], new_images['VX_DATA'], old_points['VX_DATA'],new_points_estimates['VX_DATA'] , new_points['VX_DATA'],
                                        max_count, final_epsilon,window_dimension[0])
    graph['VX_DATA'].node.append(node)
    return node

