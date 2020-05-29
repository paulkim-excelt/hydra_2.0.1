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
    @defgroup grp_vidrcvr_qt Qualification Tests
    @ingroup grp_vidrcvr

    @addtogroup grp_vidrcvr_qt
    @{

    @file vidrcvr_manual_qt.c
    @brief vidrcvr Qualification Test template
    This source file contains the requirement tests for vidrcvr
    @version 1.0 Changelist1
    @version 0.1 Changelist0
    @internal
        Topologies source ppt is available in below link
        https://drive.google.com/drive/folders/1ap6MGK4ltyv1ZL2S64krT4GsNZwM381a
    @endinternal
*/

/**
    @brief vidrcvr flashing image to internal flash and boot

    @test Verify flashing image to internal flash (FLASH_HW_ID_0) and device
    boot up with vidrcvr application

    @post Flashing and device boot up should be succesful from internal flash

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/
#define BRCM_SWQTST_VIDRCVR_INTFLASH_CASE0   (0UL)

/**
    @brief vidrcvr flashing image to external flash and boot

    @pre Erase internal flash (FLASH_HW_ID_0) before flashing to external (FLASH_HW_ID_1)

    @test Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with vidrcvr application

    @post Flashing and device boot up should be succesful from external flash

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/
#define BRCM_SWQTST_VIDRCVR_EXTFLASH_CASE7   (7UL)

/**
    @brief vidrcvr application (1722 streaming)

    @pre BRCM_SWQTST_VIDRCVR_INTFLASH_CASE0

    @test Verify if vidxmtr's video is played on vidrcvr DUT

    @post Video playback should be seen on vidrcvr DUT

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/
#define BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1   (1UL)

/**
    @brief vidrcvr application (1722 streaming) @30FPS

    @pre BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1

    @test Verify frame drops with 1722 vidrcvr app at 30fps

    @post There should not be any number miss, ex: for a recording of 12 second
    the capture should display 0 to 360

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/
#define BRCM_SWQTST_VIDRCVR_FPS_CASE2   (2UL)

/**
    @brief vidrcvr application smooth playback

    @pre BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1

    @test Verify if video playback is smooth in vidrcvr

    @post Playback should be smooth (no video pauses/jerks)

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/

#define BRCM_SWQTST_VIDRCVR_SMOOTHPB_CASE3   (3UL)

/**
    @brief vidrcvr application artifacts

    @pre BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1

    @test Verify if any visual artifacts are seen in vidrcvr DUT

    @post No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise)

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/

#define BRCM_SWQTST_VIDRCVR_ARTIFACTS_CASE4   (4UL)

/**
    @brief vidrcvr application with app stop/start 5 times

    @pre BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1

    @test Verify app stop/start commands through UART for 5 times

    @post Streaming should stop and resume after executing "app stop" and
    "app start" respectively

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/

#define BRCM_SWQTST_VIDRCVR_APPSTOPSTART_CASE5   (5UL)

/**
    @brief vidrcvr application color accuracy

    @pre BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1

    @test Verify color accuracy with vidrcvr application

    @post There should not be any color shifts

    @trace #BRCM_SWREQ_VIDRCVR_FUNCTIONALITY
*/

#define BRCM_SWQTST_VIDRCVR_COLOR_CASE6   (6UL)

/** @brief vidrcvr with moving-traffic video, with internal flash

    End to end video streaming at 720p 30fps when image is flashed to
    internal flash

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDRCVR_TOPOLOGY_1
    -# Verify flashing image to internal flash (FLASH_HW_ID_0) and device boot
    up with vidrcvr application
    -# Flashing and device boot up should be succesful from internal flash
    #BRCM_SWQTST_VIDRCVR_INTFLASH_CASE0
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on vidrcvr DUT
    #BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1
    -# Check video playback on vidrcvr DUT
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDRCVR_SMOOTHPB_CASE3
    -# On vidrcvr DUT check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDRCVR_ARTIFACTS_CASE4
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start" command through UART
    -# Repeat above two steps 5 times
    -# Streaming should stop and resume after executing "app stop" and
    "app start" respectively #BRCM_SWQTST_VIDRCVR_APPSTOPSTART_CASE5

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_720P_SEQ0   (0UL)

/** @brief vidrcvr with 30fps-numbered clip, with internal flash

    30 fps numbered clip should be streamed with out frame drops when image is
    flashed to internal flash

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in Toplogy 1
    -# Verify flashing image to internal flash (FLASH_HW_ID_0) and device boot
    up with vidrcvr application
    -# Flashing and device boot up should be succesful from internal flash
    #BRCM_SWQTST_VIDRCVR_INTFLASH_CASE0
    -# Point vidxmtr camera viewpoint to the 30FPS numbered clip played on PC
    -# Video playback should be seen on vidrcvr DUT
    #BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1
    -# Use External camera to record numbered clip played on vidrcvr
    -# Copy the recorded clip to PC and check the recorded video frame by frame
    using avidemux tool
    -# There should not be any number miss, ex: for a recording of 12 second the
    capture should display 0 to 360 #BRCM_SWQTST_VIDRCVR_FPS_CASE2

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_720P_SEQ1   (1UL)

/** @brief vidrcvr application color accuracy with internal flash

    verify color accuracy with vidxmtr and vidrcvr applications when image is
    flashed to internal flash

    @image html color-chart.png COLOR-CHART

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in Toplogy 1
    -# Verify flashing image to internal flash (FLASH_HW_ID_0) and device boot
    up with vidrcvr application
    -# Flashing and device boot up should be succesful from internal flash
    #BRCM_SWQTST_VIDRCVR_INTFLASH_CASE0
    -# Point vidxmtr camera viewpoint to the color-chart
    -# There should not be any color shifts #BRCM_SWQTST_VIDRCVR_COLOR_CASE6

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_COLOR_SEQ2   (2UL)

/** @brief vidrcvr stability with moving-traffic video, with internal flash

    End to end video streaming at 720p 30fps continuously for 8 hours when image
    is flashed to internal flash

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDRCVR_TOPOLOGY_1
    -# Verify flashing image to internal flash (FLASH_HW_ID_0) and device boot
    up with vidrcvr application
    -# Flashing and device boot up should be succesful from internal flash
    #BRCM_SWQTST_VIDRCVR_INTFLASH_CASE0
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on vidrcvr DUT
    #BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1
    -# Check video playback on vidrcvr DUT
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDRCVR_SMOOTHPB_CASE3
    -# On vidrcvr DUT check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDRCVR_ARTIFACTS_CASE4
    -# Let the setup run for 8 hours
    -# Run steps 5 to 9

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_STABILITY_SEQ3   (3UL)

/** @brief vidrcvr with moving-traffic video, with external flash

    End to end video streaming at 720p 30fps when image is flashed to
    external flash

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDRCVR_TOPOLOGY_1
    -# Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with vidrcvr application
    -# Flashing and device boot up should be succesful from external flash
    #BRCM_SWQTST_VIDRCVR_EXTFLASH_CASE7
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on vidrcvr DUT
    #BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1
    -# Check video playback on vidrcvr DUT
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDRCVR_SMOOTHPB_CASE3
    -# On vidrcvr DUT check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDRCVR_ARTIFACTS_CASE4
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start" command through UART
    -# Repeat above two steps 5 times
    -# Streaming should stop and resume after executing "app stop" and
    "app start" respectively #BRCM_SWQTST_VIDRCVR_APPSTOPSTART_CASE5

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_720P_SEQ4   (4UL)

/** @brief vidrcvr with 30fps-numbered clip, with external flash

    30 fps numbered clip should be streamed with out frame drops when image is
    flashed to external flash

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in Toplogy 1
    -# Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with vidrcvr application
    -# Flashing and device boot up should be succesful from external flash
    #BRCM_SWQTST_VIDRCVR_EXTFLASH_CASE7
    -# Point vidxmtr camera viewpoint to the 30FPS numbered clip played on PC
    -# Video playback should be seen on vidrcvr DUT
    #BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1
    -# Use External camera to record numbered clip played on vidrcvr
    -# Copy the recorded clip to PC and check the recorded video frame by frame
    using avidemux tool
    -# There should not be any number miss, ex: for a recording of 12 second the
    capture should display 0 to 360 #BRCM_SWQTST_VIDRCVR_FPS_CASE2

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_720P_SEQ5   (5UL)

/** @brief vidrcvr application color accuracy with external flash

    verify color accuracy with vidxmtr and vidrcvr applications when image is
    flashed to external flash

    @image html color-chart.png COLOR-CHART

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in Toplogy 1
    -# Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with vidrcvr application
    -# Flashing and device boot up should be succesful from external flash
    #BRCM_SWQTST_VIDRCVR_EXTFLASH_CASE7
    -# Point vidxmtr camera viewpoint to COLOR-CHART
    -# There should not be any color shifts #BRCM_SWQTST_VIDRCVR_COLOR_CASE6

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_COLOR_SEQ6   (6UL)

/** @brief vidrcvr stability with moving-traffic video, with external flash

    End to end video streaming at 720p 30fps continuously for 8 hours when image
    is flashed to external flash

    @image html vidrcvr_topology_1.jpg VIDRCVR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDRCVR_TOPOLOGY_1
    -# Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with vidrcvr application
    -# Flashing and device boot up should be succesful from external flash
    #BRCM_SWQTST_VIDRCVR_EXTFLASH_CASE7
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on vidrcvr DUT
    #BRCM_SWQTST_VIDRCVR_RCVRPB_CASE1
    -# Check video playback on vidrcvr DUT
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDRCVR_SMOOTHPB_CASE3
    -# On vidrcvr DUT check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDRCVR_ARTIFACTS_CASE4
    -# Let the setup run for 8 hours
    -# Run steps 5 to 9

    @board bcm89107_evk
    @board bcm89109_ddr_evk

*/
#define BRCM_SWQTSEQ_VIDRCVR_STABILITY_SEQ7   (7UL)
