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
    @defgroup grp_vidxmtr_qt Qualification Tests
    @ingroup grp_vidxmtr

    @addtogroup grp_vidxmtr_qt
    @{
    @section test_setup_overview Test Setup

    @image html vidxmtr_topology_1.jpg VIDXMTR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in VIDXMTR_TOPOLOGY_1
    -# Verify flashing image on external flash
    -# Flashing and device boot up should be succesful #BRCM_SWQTST_VIDXMTR_EXTFLASH_POS_CASE0

    @file vidxmtr_manual_qt.c
    @brief vidxmtr Qualification Test template
    This source file contains the requirement tests for vidxmtr
    @version 0.1 Initial version migrated from XLS
    @internal
        Topologies source ppt is available in below link
        https://drive.google.com/drive/folders/1ap6MGK4ltyv1ZL2S64krT4GsNZwM381a
    @endinternal
*/

/**
    @brief vidxmtr flashing image and boot

    @pre Erase internal flash (FLASH_HW_ID_0) before flashing to external (FLASH_HW_ID_1)

    @test Verify flashing image to external flash (FLASH_HW_ID_1) and device boot up with vidxmtr application

    @post Flashing and device boot up should be succesful

    @functional     Yes

    @type           Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS


*/
#define BRCM_SWQTST_VIDXMTR_EXTFLASH_POS_CASE0   (0UL)

/**
    @brief vidxmtr application (1722 streaming)

    @pre BRCM_SWQTST_VIDXMTR_EXTFLASH_POS_CASE0

    @test Verify if vidxmtr's video is played on VLC listener

    @post Video playback should be seen on VLC listener

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1   (1UL)

/**
    @todo Add WIRESHARK case to verify 1722 packets
*/

/**
    @brief vidxmtr application (1722 streaming) @30FPS

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify frame drops with 1722 streaming vidxmtr app at 30fps

    @post There should not be any number miss, ex: for a recording of 12 second the capture should display 0 to 360

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXMTR_FPS_POS_CASE2   (2UL)

/**
    @brief vidxmtr application (1722 streaming) with H264 encode.

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify codec info on VLC listener

    @post Codec Information should be H264-MPEG4-AVC(Part10)

    @functional  Yes

    @type        Pos

    @image html vidxmtr_codec_info.jpg VLC_CODEC_INFO

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3   (3UL)

/**
    @brief vidxmtr application (1722 streaming) with 1280x720p resolution

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify if resolution info on VLC listener is 1280x720p

    @post Resolution should be 1280x720p

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4   (4UL)

/**
    @brief vidxmtr application smooth playback

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify if video playback is smooth in VLC listener

    @post Playback should be smooth (no video pauses/jerks)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5   (5UL)

/**
    @brief vidxmtr application artifacts

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify if any visual artifacts are seen in VLC listener

    @post No visual artifacts should be seen (tearing, color slice, blockiness, pixelization, noise)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6   (6UL)

/**
    @brief vidxmtr application under low light condition

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify video playback under low light condition on VLC listener

    @post AWB and AGC should be stabilized (no osciallations), True colors should be displayed

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_LOWLIGHT_POS_CASE7   (7UL)

/**
    @brief vidxmtr application with app stop/start.

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify app stop/start commands through UART.

    @post Streaming should stop and resume after executing "app stop" and "app start" respectively

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_APPSTOPSTART_POS_CASE8   (8UL)

/**
    @brief vidxmtr application with app start 1080p

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify "app start 1080p" command through UART

    @post Streaming should stop and resume after executing "app stop" and "app start 1080p" respectively

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1080p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_APPSTART1080P_POS_CASE9   (9UL)

/**
    @brief vidxmtr application with app start 1280p

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify "app start 1280p" command through UART

    @post Streaming should stop and resume after executing "app stop" and "app start 1280p" respectively

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1280p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_APPSTART1280P_POS_CASE10   (10UL)

/**
    @brief vidxmtr application color accuracy

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify color accuracy with vidxmtr application

    @post There should not be any color shifts

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_COLOR_POS_CASE11   (11UL)

/**
    @brief vidxmtr application (1722 streaming) with 1080p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART1080P_POS_CASE9

    @test Verify if video resolution is 1080p

    @post Resolution should be 1920x1080p

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1080p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_RESOLUTION1080P_POS_CASE12   (12UL)

/**
    @brief vidxmtr application smooth playback with 1080p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART1080P_POS_CASE9

    @test Verify if video playback is smooth with 1080p resolution

    @post Playback should be smooth (no video pauses/jerks)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1080p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_SMOOTHPB1080P_POS_CASE13   (13UL)

/**
    @brief vidxmtr application artifacts with 1080p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART1080P_POS_CASE9

    @test Verify if any visual artifacts are seen in VLC listener with 1080p
    resolution

    @post No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1080p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_ARTIFACTS1080P_POS_CASE14   (14UL)

/**
    @brief vidxmtr application (1722 streaming) with 1280p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART1280P_POS_CASE10

    @test Verify if video resolution is 1280p

    @post Resolution should be 1620x1280p

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1280p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_RESOLUTION1280P_POS_CASE15   (15UL)

/**
    @brief vidxmtr application smooth playback with 1280p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART1280P_POS_CASE10

    @test Verify if video playback is smooth with 1280p resolution

    @post Playback should be smooth (no video pauses/jerks)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1280p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_SMOOTHPB1280P_POS_CASE16   (16UL)

/**
    @brief vidxmtr application artifacts with 1280p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART1280P_POS_CASE10

    @test Verify if any visual artifacts are seen in VLC listener with 1280p
    resolution

    @post No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 1280p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_ARTIFACTS1280P_POS_CASE17   (17UL)

/**
    @brief vidxmtr application with half lens covered under good lighting condition

    @pre BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5

    @test Verify video playback under good light condition on VLC listener

    @post Video displayed onto VLC listener should not have any visual artifacts(distortion, noise, tearing, rolling effect).

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_HALFLENSCOVERGOODLIGHT_POS_CASE18   (18UL)

/**
    @brief vidxmtr application with half lens covered under low light condition

    @pre BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5

    @test Verify video playback under low light condition on VLC listener

    @post Video displayed onto VLC listener should not have any visual artifacts(distortion, noise, tearing, rolling effect).

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_HALFLENSCOVERLOWLIGHT_POS_CASE19   (19UL)

/**
    @brief vidxmtr application (1722 streaming) with 968p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART1080P_POS_CASE9

    @test Verify if video resolution is 968p

    @post Resolution should be 1344x968p

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 968p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_RESOLUTION968P_POS_CASE20   (20UL)

/**
    @brief vidxmtr application(1722 streaming) long duration test.

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Monitor Video streaming from Vidxmtr to VLC Listener for 8+ hours.

    @post Video Playback from vidxmtr should be streamed onto VLC Listener after 8+ hours.

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 968p requirement once populated

    @TBD Link to 1080p requirement once populated

    @TBD Link to 1280p requirement once populated

*/

#define BRCM_SWQTST_VIDXMTR_STABILITY_POS_CASE21   (21UL)

/**
    @brief vidxmtr application with app start 968p

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Verify "app start 968" command through UART

    @post Streaming should stop and resume after executing "app stop" and "app start 968p" respectively

    @functional  Yes

    @type        Pos

    @trace BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 968p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_APPSTART968P_POS_CASE22   (22UL)

/**
    @brief vidxmtr application smooth playback with 968p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART968P_POS_CASE22

    @test Verify if video playback is smooth with 968p resolution

    @post Playback should be smooth (no video pauses/jerks)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 968p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_SMOOTHPB968P_POS_CASE23   (23UL)

/**
    @brief vidxmtr application artifacts with 968p resolution

    @pre BRCM_SWQTST_VIDXMTR_APPSTART968P_POS_CASE22

    @test Verify if any visual artifacts are seen in VLC listener with 968p
    resolution

    @post No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise)

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD Link to 968p requirement once populated
*/

#define BRCM_SWQTST_VIDXMTR_ARTIFACTS968P_POS_CASE24   (24UL)

/**
    @brief vidxmtr video streaming with power cycle

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test power cycle vidxmtr and verify if vidxmtr's video is played on VLC listener

    @post Streaming should resume once vidxmtr powred on.

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

*/

#define BRCM_SWQTST_VIDXMTR_POWERONOFF_POS_CASE25   (25UL)

/**
    @brief vidxmtr video streaming with BR cable plug-in plug-out

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Plug-out->Plug-In BR cable 5 times and verify if vidxmtr's video is played on VLC listener

    @post Streaming should stop on BR cable plug-out and resume back on BR cable plug-in

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @functional  Yes

    @type        Neg

*/

#define BRCM_SWQTST_VIDXMTR_PLUGINPLUGOUT_BR_NEG_CASE26   (26UL)

/**
    @brief Validate vidxmtr application app start/stop state machine via issuing app start command twice without app stop.

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Run app start command twice on UART console and verify video playback on VLC listener.

    @post On Executing "app start" command twice, vidxmtr app should continue to run without issues and video playback should be seen on VLC listener.

    @functional  Yes

    @type        Neg

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

*/

#define BRCM_SWQTST_VIDXMTR_APPSTART_TWICE_NEG_CASE27   (27UL)

/**
    @brief Validate vidxmtr application app start/stop state machine via issuing app stop command twice without app start.

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Run app stop command twice on UART console and verify video playback on VLC listener.

    @post On Executing "app stop" command twice ,video playback shouldn't be seen on VLC listener.

    @functional  Yes

    @type        Neg

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

*/

#define BRCM_SWQTST_VIDXMTR_APPSTOP_TWICE_NEG_CASE28   (28UL)

/**
    @brief vidxmtr application app start command with invalid resolution.

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Run "app start 10p" command on UART console and verify video playback on VLC listener.

    @post Streaming should stop and resume with default resolution(1280x720) after executing "app stop" and "app start 10p" respectively.

    @functional  Yes

    @type        Neg

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

*/

#define BRCM_SWQTST_VIDXMTR_INVALID_RESOLUTION_NEG_CASE29    (29UL)


/**
    @brief Validate vidxmtr application start time.

    @pre BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1

    @test Reboot device and check the Video Transmission start time on UART console .

    @post application start time should be less than x.

    @functional  No

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS

    @TBD application start time validation criteria
*/

#define BRCM_SWQTST_VIDXMTR_APPSTART_TIME_PERF_CASE30    (30UL)

/**
    @brief vidxmtr application with 3 times light on/off

    @pre BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5

    @test On/off lights 3 times, verify video playback on VLC listener.

    @post AWB and AGC should be stabilized (no osciallations) on moving from good light to low light condition and vice versa, True colors should be displayed.

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_LIGHTONOFF_POS_CASE31    (31UL)

/**
    @brief vidxmtr flashing image and boot

    @pre Erase internal flash (FLASH_HW_ID_0).

    @test Verify flashing image to internal flash (FLASH_HW_ID_0) with vidxmtr application

    @post Flashing should fail.

    @functional  Yes

    @type        Neg

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/
#define BRCM_SWQTST_VIDXMTR_INTFLASH_NEG_CASE32   (32UL)

/**
    @brief vidxmtr application with full lens covered.

    @pre BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5

    @test Cover full lens and verify video playback on VLC listener

    @post Video displayed onto VLC listener should not have any visual artifacts(distortion, noise, tearing, rolling effect).

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_FULLLENSCOVER_POS_CASE33   (33UL)

/**
    @brief vidxmtr application with 3 times full lens cover/uncover.

    @pre BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5

    @test cover/uncover full lens 3 times and verify video playback on VLC listener.

    @post AWB and AGC should be stabilized (no osciallations) on cover/uncover lens and vice versa, True colors should be displayed.

    @functional  Yes

    @type        Pos

    @trace #BRCM_REQ_VIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_VIDXMTR_LENSCOVERUNCOVER_POS_CASE34   (34UL)


/** @brief vidxmtr with moving-traffic video

    End to end video streaming at 720p 30fps
    
    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   Sanity


*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SANITY0   (0UL)

/** @brief vidxmtr with moving-traffic video

    End to end video streaming at 720p 30fps under good light condition with Half lens covered.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6
    -# In VLC listener check video playback under good light condition with Half lens covered.
    -# Video displayed onto VLC listener should not have any visual artifacts
    (distortion, noise, tearing, rolling effect) #BRCM_SWQTST_VIDXMTR_HALFLENSCOVERGOODLIGHT_POS_CASE18

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN0   (0UL)

/** @brief vidxmtr with moving-traffic video

    End to end video streaming at 720p 30fps under low lighting condition

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6
    -# In VLC listener check video playback under low light condition
    -# AWB and AGC should be stabilized (no osciallations), True colors should
    be displayed #BRCM_SWQTST_VIDXMTR_LOWLIGHT_POS_CASE7
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN1   (1UL)

/** @brief vidxmtr with moving-traffic video

    End to end video streaming at 720p 30fps under low light condition with Half lens covered.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6
    -# In VLC listener check video playback under low light condition with Half lens covered.
    -# Video displayed onto VLC listener should not have any visual artifacts
    (distortion, noise, tearing, rolling effect) #BRCM_SWQTST_VIDXMTR_HALFLENSCOVERLOWLIGHT_POS_CASE19
    -# AWB and AGC should be stabilized (no osciallations), True colors should
    be displayed #BRCM_SWQTST_VIDXMTR_LOWLIGHT_POS_CASE7

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN2   (2UL)

/** @brief vidxmtr with moving-traffic video

    End to end video streaming at 720p 30fps with full lens cover/uncover.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6
    -# Cover full lens and verify video playback on VLC listener.
    -# Video displayed onto VLC listener should not have any visual artifacts with full lens cover.
    (distortion, noise, tearing, rolling effect) #BRCM_SWQTST_VIDXMTR_FULLLENSCOVER_POS_CASE33
    -# Cover/Uncover full lens 3 times and verify video playback on VLC listener.
    -# AWB and AGC should be stabilized (no osciallations), True colors should
    be displayed #BRCM_SWQTST_VIDXMTR_LENSCOVERUNCOVER_POS_CASE34.
    -# Uncover the lens and check video playback on VLC listener.
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN3   (3UL)

/** @brief vidxmtr with moving-traffic video

    vidxmtr application with app stop/start.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Issue "app stop" command through UART.
    -# After 5 secs, issue "app start" command through UART.
    -# Streaming should stop and resume after executing "app stop" and
    "app start" respectively #BRCM_SWQTST_VIDXMTR_APPSTOPSTART_POS_CASE8
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN4   (4UL)

/** @brief vidxmtr with moving-traffic video

    vidxmtr application with power on/off.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Power cycle vidxmtr.
    -# Streaming should resume after vidxmtr powerd on 
    #BRCM_SWQTST_VIDXMTR_POWERONOFF_POS_CASE25
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# Repeat above steps 10 times.

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN5   (5UL)

/** @brief vidxmtr with 30fps-numbered clip

    30 fps numbered clip should be streamed with out frame drops

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to the 30FPS numbered clip played on PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Use External camera to record numbered clip played on VLC
    -# Copy the recorded clip to PC and check the recorded video frame by frame
    using avidemux tool
    -# There should not be any number miss, ex: for a recording of 12 second the
    capture should display 0 to 360 #BRCM_SWQTST_VIDXMTR_FPS_POS_CASE2

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN6   (6UL)

/** @brief vidxmtr with moving-traffic video

    Validate vidxmtr application behavior with BR cable plug-in plug-out.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Plug-out->Plug-In BR cable 5 times and check video playback on VLC listener.
    -# Streaming should stop on BR cable plug-out and resume back on BR cable plug-in
    #BRCM_SWQTST_VIDXMTR_PLUGINPLUGOUT_BR_NEG_CASE26
    -# After 5 times BR cable plug-out/plug-in, check video playback on VLC listener.
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/

#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN7   (7UL)

/** @brief vidxmtr with moving-traffic video

    Validate vidxmtr application app start/stop state machine.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Issue "app stop" command through UART.
    -# After 5 secs, issue "app start" command through UART.
    -# Streaming should stop and resume after executing "app stop" and
    "app start" respectively #BRCM_SWQTST_VIDXMTR_APPSTOPSTART_POS_CASE8
    -# After 5 secs, again issue "app start" command through UART.
    -# In VLC listener check video playback
    -# On Executing "app start" command twice, vidxmtr app should continue to run without issues.
    and video playback should be seen on VLC listener. #BRCM_SWQTST_VIDXMTR_APPSTART_TWICE_NEG_CASE27
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# Issue "app stop" command through UART.
    -# After 5 secs, again issue "app stop" command through UART and check video playback on VLC listener.
    -# On Executing "app stop" command twice ,video playback shouldn't be seen on VLC listener.
    #BRCM_SWQTST_VIDXMTR_APPSTOP_TWICE_NEG_CASE28

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN8   (8UL)

/** @brief vidxmtr with moving-traffic video

    Validate vidxmtr application app start command with invalid resolution.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start 10p" command through UART.
    -# Streaming should stop and resume with default resolution(1280x720) after
    executing "app stop" and "app start 10p" respectively. #BRCM_SWQTST_VIDXMTR_INVALID_RESOLUTION_NEG_CASE29
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN9   (9UL)

/** @brief vidxmtr with moving-traffic video

    Validate Video Transmission start time with default resolution.

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Reboot device and check the Video Transmission start time on UART console.
    -# Video Transmission start time should be less than x #BRCM_SWQTST_VIDXMTR_APPSTART_TIME_PERF_CASE30

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto    No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN10   (10UL)

/** @brief vidxmtr application flashing.

    Validate vidxmtr application flashing on internal memory.

    @pre test_setup_overview

    Steps:
    -#Erase internal flash (FLASH_HW_ID_0) Flash vidxmtr image to internal flash (FLASH_HW_ID_0).
    -#Validate flashing logs on UART console.
    -#Flashing should fail on internal flash(FLASH_HW_ID_0).
    #BRCM_SWQTST_VIDXMTR_INTFLASH_NEG_CASE32

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto    No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_720P_SHORTRUN11   (11UL)

/** @brief vidxmtr application color accuracy

    verify color accuracy with vidxmtr application

    @image html color-chart.png COLOR-CHART

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to COLOR-CHART
    -# There should not be any color shifts #BRCM_SWQTST_VIDXMTR_COLOR_POS_CASE11

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto    No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_COLOR_SHORTRUN0   (0UL)

/** @brief 1080p vidxmtr with moving-traffic video

    End to end video streaming at 1080p 30fps

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start 1080p" command through UART
    -# Streaming should stop and resume after executing "app stop" and
    "app start 1080p" respectively #BRCM_SWQTST_VIDXMTR_APPSTART1080P_POS_CASE9
    -# In VLC listener check for the codec info (VLC->Tools->Codec Information)
    -# Resolution should be 1920x1080p
    #BRCM_SWQTST_VIDXMTR_RESOLUTION1080P_POS_CASE12
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB1080P_POS_CASE13
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS1080P_POS_CASE14

    @board bcm89103_ox3a_evk
    @board bcm89107_ox3a_evk

    @auto    No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_1080P_SHORTRUN0   (0UL)

/** @brief 1280p vidxmtr with moving-traffic video

    End to end video streaming at 1280p 30fps

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start 1280p" command through UART
    -# Streaming should stop and resume after executing "app stop" and
    "app start 1280p" respectively #BRCM_SWQTST_VIDXMTR_APPSTART1280P_POS_CASE10
    -# In VLC listener check for the codec info (VLC->Tools->Codec Information)
    -# Resolution should be 1620x1280p
    #BRCM_SWQTST_VIDXMTR_RESOLUTION1280P_POS_CASE15
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB1280P_POS_CASE16
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS1280P_POS_CASE17

    @board bcm89103_ox3a_evk
    @board bcm89107_ox3a_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_1280P_SHORTRUN0   (0UL)

/** @brief vidxmtr stability with moving-traffic video

    End to end video streaming at 968p 30fps continuously for 8 hours

    @pre #test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start 968" command through UART
    -# Streaming should stop and resume after executing "app stop" and
    "app start 968p" respectively #BRCM_SWQTST_VIDXMTR_APPSTART968P_POS_CASE22
    -# In VLC listener check for the codec info (VLC->Tools->Codec Information)
    -# Resolution should be 1344x968p
    #BRCM_SWQTST_VIDXMTR_RESOLUTION968P_POS_CASE20
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB968P_POS_CASE23
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS968P_POS_CASE24

    @board bcm89103_ar143_evk

    @auto   No

    @type   ShortRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_968P_SHORTRUN0   (0UL)

/** @brief vidxmtr stability with moving-traffic video

    End to end video streaming at 720p 30fps continuously for 8 hours

    @pre test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6
    -# Let the setup run for 8 hours
    -# Run steps 5 to 12 #BRCM_SWQTST_VIDXMTR_STABILITY_POS_CASE21

    @board bcm89103_ox3a_evk
    @board bcm89103_ov9716_evk
    @board bcm89103_ar143_evk
    @board bcm89106_ox3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

    @auto   No

    @type   LongRun

*/

#define BRCM_SWQTSEQ_VIDXMTR_720P_LONGRUN0   (0UL)

/** @brief vidxmtr stability with moving-traffic video

    End to end video streaming at 968p 30fps continuously for 8 hours

    @pre #test_setup_overview Test Setup

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start 968" command through UART
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Resolution should be 1344x968p #BRCM_SWQTST_VIDXMTR_RESOLUTION968P_POS_CASE20
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB_POS_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS_POS_CASE6
    -# Let the setup run for 8 hours
    -# Run steps 10 to 12 #BRCM_SWQTST_VIDXMTR_STABILITY_POS_CASE21

    @board bcm89103_ar143_evk

    @auto   No

    @type   LongRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_968P_LONGRUN0   (0UL)

/** @brief vidxmtr stability with moving-traffic video

    End to end video streaming at 1080p 30fps continuously for 8 hours

    @pre #test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start 1080p" command through UART
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Resolution should be 1920x1080p #BRCM_SWQTST_VIDXMTR_RESOLUTION1080P_POS_CASE12
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB1080P_POS_CASE13
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS1080P_POS_CASE14
    -# Let the setup run for 8 hours
    -# Run steps 10 to 12 #BRCM_SWQTST_VIDXMTR_STABILITY_POS_CASE21

    @board bcm89103_ox3a_evk
    @board bcm89107_ox3a_evk

    @auto   No

    @type   LongRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_1080P_LONGRUN0   (0UL)

/** @brief vidxmtr stability with moving-traffic video

    End to end video streaming at 1080p 30fps continuously for 8 hours

    @pre #test_setup_overview

    Steps:
    -# Point vidxmtr camera viewpoint to moving traffic video clip played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_VIDXMTR_CODEC_POS_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_VIDXMTR_RESOLUTION_POS_CASE4
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start 1280p" command through UART
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_VIDXMTR_VLCPB_POS_CASE1
    -# Resolution should be 1620x1280p #BRCM_SWQTST_VIDXMTR_RESOLUTION1280P_POS_CASE15
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_VIDXMTR_SMOOTHPB1280P_POS_CASE16
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_VIDXMTR_ARTIFACTS1280P_POS_CASE17
    -# Let the setup run for 8 hours
    -# Run steps 10 to 12 #BRCM_SWQTST_VIDXMTR_STABILITY_POS_CASE21

    @board bcm89103_ox3a_evk
    @board bcm89107_ox3a_evk

    @auto   No

    @type   LongRun

*/
#define BRCM_SWQTSEQ_VIDXMTR_1280P_LONGRUN0   (0UL)
