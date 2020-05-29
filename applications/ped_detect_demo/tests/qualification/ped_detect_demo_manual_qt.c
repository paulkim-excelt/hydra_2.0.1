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

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PRORCVRED "AS IS"
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
    @defgroup grp_ped_detect_demo_qt Qualification Tests
    @ingroup grp_ped_detect_demo

    @addtogroup grp_ped_detect_demo_qt
    @{

    @file ped_detect_demo_manual_qt.c
    @brief ped_detect_demoQualification Test template
    This source file contains the requirement tests for ped_detect_demo
    @version 1.0 Changelist1
    @version 0.1 Changelist0
*/

/**
    @brief ped_detect_demoapplication flashing to external flash and boot

    @test Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with ped_detect_demo application

    @post Flashing and device boot up should  be succesful from external
    flash

    @trace #BRCM_SWREQ_PED_DETECT_DEMO_FUNCTIONALITY
*/
#define BRCM_SWQTST_PED_DETECT_DEMO_EXTFLASH_CASE0   (0UL)

/**
    @brief ped_detect_demo application video playback on local display.

    @pre BRCM_SWQTST_PED_DETECT_DEMO_EXTFLASH_CASE0

    @test Verify if video is played on DUT local display.

    @post Video playback should be seen on DUT local display

    @trace #BRCM_SWREQ_PED_DETECT_DEMO_FUNCTIONALITY
*/

#define BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1   (1UL)

/**
    @brief ped_detect_demoapplication @30FPS

    @pre BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1

    @test Verify frame drops with ped_detect_demo at 30fps

    @post There should not be any number miss, ex: for a recording of 12
    second the capture should display 0 to 360.

    @trace #BRCM_SWREQ_PED_DETECT_DEMO_FUNCTIONALITY
*/
#define BRCM_SWQTST_PED_DETECT_DEMO_FPS_CASE2   (2UL)

/**
    @brief ped_detect_demoapplication smooth playback

    @pre BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1

    @test Verify if video playback is smooth in DUT local display.

    @post Playback should be smooth (no video pauses/jerks) in DUT
    local display

    @trace #BRCM_SWREQ_PED_DETECT_DEMO_FUNCTIONALITY
*/

#define BRCM_SWQTST_PED_DETECT_DEMO_SMOOTHPB_CASE3   (3UL)

/**
    @brief ped_detect_demoapplication artifacts

    @pre BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1

    @test Verify if any visual artifacts are seen in DUT local Video display

    @post No visual artifacts should be seen (tearing, color slice,
    blockiness,pixelization, noise)

    @trace #BRCM_SWREQ_PED_DETECT_DEMO_FUNCTIONALITY
*/

#define BRCM_SWQTST_PED_DETECT_DEMO_ARTIFACTS_CASE4   (4UL)

/**
    @brief ped_detect_demoapplication with app stop/start 5 times

    @pre BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1

    @test Verify app stop/start commands through UART for 5 times

    @post Video playback on local display should stop and resume after
    executing "app stop" and "app start" respectively

    @trace #BRCM_SWREQ_PED_DETECT_DEMO_FUNCTIONALITY
*/

#define BRCM_SWQTST_PED_DETECT_DEMO_APPSTOPSTART_CASE5   (5UL)

/**
    @brief ped_detect_demo application to detect pedestrian

    @pre BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1

    @test Verify pedestrian detection

    @post Should detect pedestrians (RED rectangle) in the camera feed
    (post LDC).

    @image html ped_det_disp.jpg PED_DETECT_DEMO width=600px height=400px

    @trace #BRCM_SWREQ_PED_DETECT_DEMO_FUNCTIONALITY
*/
#define BRCM_SWQTST_PED_DETECT_DEMO_PDET_CASE6   (6UL)

/**
    @todo ped_detect_demo application CPUPROFILE

*/

/**
    @todo ped_detect_demo app flashing to internal memory should fail
*/



/** @brief ped_detect_demo with camera focus to traffic-video with people
    movement played on PC,with external flash to check pedestrian detect.

    Should detect pedestrians in the camera feed (post LDC)

    @image html ped_detect.jpg PED_DETECT_DEMO_TOPOLOGY_1

    Steps:
    -# Verify flashing image to external flash (FLASH_HW_ID_1) and
    device boot up with ped_detect_demo application
    -# Flashing and device boot up should  be succesful from external flash
    #BRCM_SWQTST_PED_DETECT_DEMO_EXTFLASH_CASE0
    -# Point DUT camera viewpoint to people movement in a traffic Video clip
    played in PC.
    -# Video playback should be seen on DUT
    #BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1
    -# Check video playback on ped_detect_demo DUT
    -# Playback should be smooth (no Video pauses/jerks)
    #BRCM_SWQTST_PED_DETECT_DEMO_SMOOTHPB_CASE3
    -# On ped_detect_demo DUT local display check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_PED_DETECT_DEMO_ARTIFACTS_CASE4
    -# Check for pedestrian detect.
    -# Should detect pedestrians (RED rectangle) in the camera feed
    (post LDC). #BRCM_SWQTST_PED_DETECT_DEMO_PDET_CASE6
    @image html ped_det_disp.jpg PED_DETECT_DEMO width=600px height=400px
    TBD- Need to take screenshot with good camera and update the image.
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start" command through UART
    -# Repeat above two steps 5 times
    -# Video playback on local display should stop and resume after
    executing "app stop" and "app start" respectively
    #BRCM_SWQTST_PED_DETECT_DEMO_APPSTOPSTART_CASE5

    @board bcm89107_ox3a_evk


*/
#define BRCM_SWQTSEQ_PED_DETECT_DEMO_720P_SEQ0   (0UL)

/** @brief ped_detect_demowith 30fps numbered clip played on PC with external
    flash

    30 fps numbered clip should be streamed with out frame drops when
    image is flashed to external flash

    @image html ped_detect.jpg PED_DETECT_DEMO_TOPOLOGY_1

    Steps:
    -# Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with ped_detect_demo application
    -# Flashing and device boot up should be succesful from external flash
    #BRCM_SWQTST_PED_DETECT_DEMO_EXTFLASH_CASE0
    -# Point camera viewpoint to the 30FPS numbered clip played on PC
    -# Video playback should be seen on ped_detect_demo DUT
    #BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1
    -# Use external camera to record numbered clip played on ped_detect_demo
    -# Copy the recorded clip to PC and check the recorded Video frame by
    frame using aRCVRemux tool
    -# There should not be any number miss, ex: for a recording of 12 second
    the capture should display 0 to 360
    #BRCM_SWQTST_PED_DETECT_DEMO_FPS_CASE2

    @board bcm89107_ox3a_evk

*/
#define BRCM_SWQTSEQ_PED_DETECT_DEMO_720P_SEQ1   (1UL)

/** @brief ped_detect_demo application stability with moving-traffic Video with
    external flash

    Video playback at 720p 30fps and  detect pedestrians continuously for
    8 hours when image is flashed to external flash

    @image html ped_detect.jpg PED_DETECT_DEMO_TOPOLOGY_1

    Steps:
    -# Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with ped_detect_demo application
    -# Flashing and device boot up should  be succesful from external flash
    #BRCM_SWQTST_PED_DETECT_DEMO_EXTFLASH_CASE0
    -# Point DUT camera viewpoint to people movement in a traffic Video clip
    played in PC.
    -# Video playback should be seen on DUT
    #BRCM_SWQTST_PED_DETECT_DEMO_RCVRPB_CASE1
    -# Check Video playback on ped_detect_demo DUT
    -# Playback should be smooth (no Video pauses/jerks)
    #BRCM_SWQTST_PED_DETECT_DEMO_SMOOTHPB_CASE3
    -# On ped_detect_demo DUT local display check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_PED_DETECT_DEMO_ARTIFACTS_CASE4
    -# Check for pedestrian detection.
    -# Should detect pedestrians (RED rectangle) in the camera feed
    (post LDC).#BRCM_SWQTST_PED_DETECT_DEMO_PDET_CASE6
    -# Let the setup run for 8 hours
    -# Run steps 4 to 10

    @image html ped_det_disp.jpg PED_DETECT_DEMO width=600px height=400px

    @board bcm89107_ox3a_evk

*/
#define BRCM_SWQTSEQ_PED_DETECT_DEMO_STABILITY_SEQ2   (2UL)

/** @} */

