/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_vidxcvr_qt Qualification Tests
    @ingroup grp_vidxcvr

    @addtogroup grp_vidxcvr_qt
    @{

    @file vidxcvr_manual_qt.c
    @brief vidxcvr Qualification Test template
    This source file contains the requirement tests for vidxcvr
    @version 0.1 Migration from XLS
    @internal
        Topologies source ppt is available in below link
        https://drive.google.com/drive/folders/1ap6MGK4ltyv1ZL2S64krT4GsNZwM381a
    @endinternal
    @todo Test data used in cases/sequences to be uploaded to team drive,
    also point to tools like avidemux and internal tools like serial writer
*/

/**
    @brief vidxcvr flashing image to external flash and boot

    @pre Erase internal flash (FLASH_HW_ID_0) before flashing to
    external (FLASH_HW_ID_1)

    @test Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with vidxcvr application

    @post Flashing and device boot up should be successful

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0   (0UL)

/**
    @brief vidxcvr flashing image internal flash and boot
    @todo add a sequence for this

    @test Verify flashing image to internal flash (FLASH_HW_ID_0) is failing
    with vidxcvr application

    @post Flashing to internal flash should fail

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXCVR_INTFLASH_CASE9   (0UL)

/**
    @brief Build vidxcvr application in gPTP slave mode and BR role as slave

    @test Verify building vidxcvr app in gPTP slave mode and BR role as slave

    @todo Give link to QSG on how to build this app in SLAVE mode

    @post Building vidxcvr application in gPTP slave mode and BR role as slave
    should be successful

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXCVR_SLAVE_CASE1   (1UL)

/**
    @brief vidxcvr application (1722 streaming)

    @pre BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0

    @test Verify if video playback is seen on both vidxcvr devices

    @post Video playback should be seen on both vidxcvrs

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2   (2UL)

/**
    @brief vidxcvr application (1722 streaming) @30FPS

    @pre BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2

    @test Verify frame drops with 1722 streaming vidxcvr app at 30fps

    @post There should not be any number miss, ex: for a recording of 12 second
    the capture should display 0 to 360
    @todo correct above line "e.g. instead of ex and "missed" in all files once
    everything gets merged

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXCVR_FPS_CASE3   (3UL)

/**
    @brief vidxcvr application smooth playback

    @pre BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2

    @test Verify if video playback is smooth with vidxcvr

    @post Playback should be smooth (no video pauses/jerks)

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXCVR_SMOOTHPB_CASE4   (4UL)

/**
    @brief vidxcvr application artifacts

    @pre BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2

    @test Verify if any visual artifacts are seen in vidxcvr DUT

    @post No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise)
    @todo define terms tearing, color slice, blockiness, pixelization, noise

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXCVR_ARTIFACTS_CASE5   (5UL)

/**
    @brief vidxcvr application under low light condition

    @pre BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2

    @test Verify video playback under low light condition on vidxcvr DUT

    @post AWB and AGC should be stabilized (no osciallations), True colors
    should be displayed
    @todo Find right word for "True colors" from imaging team and correct it in
    all files

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXCVR_LOWLIGHT_CASE6   (6UL)

/**
    @brief vidxcvr application with app stop/start 5 times

    @pre BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2

    @test Verify app stop/start commands through UART for 5 times

    @post Streaming should stop and resume after executing "app stop" and
    "app start" respectively

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXCVR_APPSTOPSTART_CASE7   (7UL)

/**
    @brief vidxcvr application color accuracy

    @pre BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2

    @test Verify color accuracy with vidxcvr application

    @post There should not be any color shifts

    @trace #BRCM_REQ_VIDXCVR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXCVR_COLOR_CASE8   (8UL)

/** @brief vidxcvr with moving-traffic video

    End to end video streaming at 720p 30fps

    @image html vidxcvr_topology_1.jpg VIDXCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDXCVR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application (default build) on DUT1
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Build vidxcvr application in gPTP slave mode and BR role as slave for
    DUT2 (default is GM/Master)
    -# Image generation should be successful
    #BRCM_SWQTST_VIDXCVR_SLAVE_CASE1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application on DUT2
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Point both vidxcvr camera viewpoints to moving traffic video clip played
    in PC
    -# Video playback should be seen on both vidxcvr DUTs
    #BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2
    -# Check video playback on both vidxcvr DUTs
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXCVR_SMOOTHPB_CASE4
    -# Check for visual artifacts on both vidxcvr DUTs
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXCVR_ARTIFACTS_CASE5
    -# Check video playback on both vidxcvr DUTs under low light condition
    -# AWB and AGC should be stabilized (no osciallations), True colors should
    be displayed #BRCM_SWQTST_VIDXCVR_LOWLIGHT_CASE6
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start" command through UART
    -# Repeat above two steps 5 times
    -# Streaming should stop and resume after executing "app stop" and
    "app start" respectively #BRCM_SWQTST_VIDXCVR_APPSTOPSTART_CASE7

    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_VIDXCVR_720P_SEQ0   (0UL)

/** @brief vidxcvr with 30fps-numbered clip

    30 fps numbered clip should be streamed with out frame drops

    @image html vidxcvr_topology_1.jpg VIDXCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDXCVR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application (default build) on DUT1
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Build vidxcvr application in gPTP slave mode and BR role as slave for
    DUT2 (default is GM/Master)
    -# Image generation should be successful
    #BRCM_SWQTST_VIDXCVR_SLAVE_CASE1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application on DUT2
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Point vidxcvr camera viewpoints to the 30FPS numbered clip played on PC
    -# Video playback should be seen on both vidxcvr DUTs
    #BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2
    -# Use External camera to record numbered clip played on vidxcvr DUT
    -# Copy the recorded clip to PC and check the recorded video frame by frame
    using avidemux tool
    -# There should not be any number miss, ex: for a recording of 12 second the
    capture should display 0 to 360 #BRCM_SWQTST_VIDXCVR_FPS_CASE3

    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_VIDXCVR_720P_SEQ1   (1UL)

/** @brief vidxcvr application color accuracy

    verify color accuracy with vidxcvr application

    @image html color-chart.png COLOR-CHART

    @image html vidxcvr_topology_1.jpg VIDXCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDXCVR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application (default build) on DUT1
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Build vidxcvr application in gPTP slave mode and BR role as slave for
    DUT2 (default is GM/Master)
    -# Image generation should be successful
    #BRCM_SWQTST_VIDXCVR_SLAVE_CASE1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application on DUT2
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Point vidxcvr camera viewpoints to COLOR-CHART
    -# There should not be any color shifts on both DUTs
    #BRCM_SWQTST_VIDXCVR_COLOR_CASE8

    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_VIDXCVR_COLOR_SEQ2   (2UL)

/** @brief vidxcvr stability with moving-traffic video

    End to end bidirectional video streaming at 720p 30fps continuously for
    8 hours

    @image html vidxcvr_topology_1.jpg VIDXCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDXCVR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application (default build) on DUT1
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Build vidxcvr application in gPTP slave mode and BR role as slave for
    DUT2 (default is GM/Master)
    -# Image generation should be successful
    #BRCM_SWQTST_VIDXCVR_SLAVE_CASE1
    -# Verify flashing image on external flash and device boot up with vidxcvr
    application on DUT2
    -# Flashing and device boot up should be successful
    #BRCM_SWQTST_VIDXCVR_EXTFLASH_CASE0
    -# Point vidxcvr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on vidxcvr DUT
    #BRCM_SWQTST_VIDXCVR_XCVRPB_CASE2
    -# On both vidxcvr DUTs check video playback
    -# Playback should be smooth on both vidxcvr DUTs (no video pauses/jerks)
    #BRCM_SWQTST_VIDXCVR_SMOOTHPB_CASE4
    -# On both vidxcvr DUTs check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXCVR_ARTIFACTS_CASE5
    -# Let the setup run for 8 hours
    -# Run steps 9 to 13

    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_VIDXCVR_STABILITY_SEQ3   (3UL)

/** @} */
