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
    @defgroup grp_rtpvidxmtr_qt Qualification Tests
    @ingroup grp_rtpvidxmtr

    @addtogroup grp_rtpvidxmtr_qt
    @{

    @file rtpvidxmtr_manual_qt.c
    @brief rtpvidxmtr Qualification Test template
    This source file contains the requirement tests for rtpvidxmtr

    @version 0.1 Initial Version from XLS
    @internal
        Topologies source ppt is available in below link
        https://drive.google.com/drive/folders/1ap6MGK4ltyv1ZL2S64krT4GsNZwM381a
    @endinternal
*/

/**
    @brief rtpvidxmtr flashing image and boot

    @pre Erase internal flash (FLASH_HW_ID_0) before flashing to
    external (FLASH_HW_ID_1)

    @test Verify flashing image to external flash (FLASH_HW_ID_1) and device
    boot up with rtpvidxmtr application

    @post Flashing and device boot up should be succesful

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/
#define BRCM_SWQTST_RTPVIDXMTR_EXTFLASH_CASE0   (0UL)

/**
    @brief rtpvidxmtr application

    @pre BRCM_SWQTST_RTPVIDXMTR_EXTFLASH_CASE0

    @test Verify if rtpvidxmtr's video is played on VLC listener

    @post Video playback should be seen on VLC listener

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/
#define BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1   (1UL)

/**
    @todo Add WIRESHARK case to verify rtp packets
*/

/**
    @brief rtpvidxmtr application @30FPS

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify frame drops with rtpvidxmtr app at 30fps

    @post There should not be any number miss, ex: for a recording of 12 second
    the capture should display 0 to 360

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/
#define BRCM_SWQTST_RTPVIDXMTR_FPS_CASE2   (2UL)

/**
    @brief rtpvidxmtr application with H264 encode.

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify codec info on VLC listener

    @post Codec Information should be H264-MPEG4-AVC(Part10)

    @image html rtpvidxmtr_codec_info.jpg VLC_CODEC_INFO
    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_RTPVIDXMTR_CODEC_CASE3   (3UL)

/**
    @brief rtpvidxmtr application with 1280x720p resolution

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify if resolution info on VLC listener is 1280x720p

    @post Resolution should be 1280x720p

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_RTPVIDXMTR_RESOLUTION_CASE4   (4UL)

/**
    @brief rtpvidxmtr application smooth playback

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify if video playback is smooth in VLC listener

    @post Playback should be smooth (no video pauses/jerks)

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_RTPVIDXMTR_SMOOTHPB_CASE5   (5UL)

/**
    @brief rtpvidxmtr application artifacts

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify if any visual artifacts are seen in VLC listener

    @post No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise)

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_RTPVIDXMTR_ARTIFACTS_CASE6   (6UL)

/**
    @brief rtpvidxmtr application under low light condition

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify video playback under low light condition on VLC listener

    @post AWB and AGC should be stabilized (no osciallations), True colors
    should be displayed

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_RTPVIDXMTR_LOWLIGHT_CASE7   (7UL)

/**
    @brief rtpvidxmtr application with app stop/start 5 times

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify app stop/start commands through UART for 5 times

    @post Streaming should stop and resume after executing "app stop" and
    "app start" respectively

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_RTPVIDXMTR_APPSTOPSTART_CASE8   (8UL)

/**
    @brief rtpvidxmtr application color accuracy

    @pre BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1

    @test Verify color accuracy with rtpvidxmtr application

    @post There should not be any color shifts

    @trace #BRCM_REQ_RTPVIDXMTR_720P30_30MBPS
*/

#define BRCM_SWQTST_RTPVIDXMTR_COLOR_CASE9   (9UL)

/** @brief rtpvidxmtr with moving-traffic video

    End to end video streaming at 720p 30fps

    @image html rtpvidxmtr_topology_1.jpg RTPVIDXMTR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in RTPVIDXMTR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with
    rtpvidxmtr application
    -# Flashing and device boot up should be succesful
    #BRCM_SWQTST_RTPVIDXMTR_EXTFLASH_CASE0
    -# Point rtpvidxmtr camera viewpoint to moving traffic video clip
    played in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_RTPVIDXMTR_CODEC_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_RTPVIDXMTR_RESOLUTION_CASE4
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_RTPVIDXMTR_SMOOTHPB_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_RTPVIDXMTR_ARTIFACTS_CASE6
    -# In VLC listener check video playback under low light condition
    -# AWB and AGC should be stabilized (no osciallations), True colors should
    be displayed #BRCM_SWQTST_RTPVIDXMTR_LOWLIGHT_CASE7
    -# Issue "app stop" command through UART
    -# After 5 secs, issue "app start" command through UART
    -# Repeat above two steps 5 times
    -# Streaming should stop and resume after executing "app stop" and
    "app start" respectively #BRCM_SWQTST_RTPVIDXMTR_APPSTOPSTART_CASE8

    @board bcm89103_ox3a_evk
    @board bcm89103_ar143_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_RTPVIDXMTR_720P_SEQ0   (0UL)

/** @brief rtpvidxmtr with 30fps-numbered clip

    30 fps numbered clip should be streamed with out frame drops

    @image html rtpvidxmtr_topology_1.jpg RTPVIDXMTR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in RTPVIDXMTR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with
    rtpvidxmtr application
    -# Flashing and device boot up should be succesful
    #BRCM_SWQTST_RTPVIDXMTR_EXTFLASH_CASE0
    -# Point rtpvidxmtr camera viewpoint to the 30FPS numbered clip played on PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1
    -# Use External camera to record numbered clip played on VLC
    -# Copy the recorded clip to PC and check the recorded video frame by frame
    using avidemux tool
    -# There should not be any number miss, ex: for a recording of 12 second the
    capture should display 0 to 360 #BRCM_SWQTST_RTPVIDXMTR_FPS_CASE2

    @board bcm89103_ox3a_evk
    @board bcm89103_ar143_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_RTPVIDXMTR_720P_SEQ1   (1UL)

/** @brief rtpvidxmtr application color accuracy

    verify color accuracy with rtpvidxmtr application

    @image html color-chart.png COLOR-CHART

    @image html rtpvidxmtr_topology_1.jpg RTPVIDXMTR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in RTPVIDXMTR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with
    rtpvidxmtr application
    -# Flashing and device boot up should be succesful
    #BRCM_SWQTST_RTPVIDXMTR_EXTFLASH_CASE0
    -# Point rtpvidxmtr camera viewpoint to COLOR-CHART
    -# There should not be any color shifts #BRCM_SWQTST_RTPVIDXMTR_COLOR_CASE9

    @board bcm89103_ox3a_evk
    @board bcm89103_ar143_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_RTPVIDXMTR_COLOR_SEQ2   (2UL)

/** @brief rtpvidxmtr stability with moving-traffic video

    End to end video streaming at 720p 30fps continuously for 8 hours

    @image html rtpvidxmtr_topology_1.jpg RTPVIDXMTR_TOPOLOGY_1

    Steps:
    -# Make setup as shown in RTPVIDXMTR_TOPOLOGY_1
    -# Verify flashing image on external flash and device boot up with
    rtpvidxmtr application
    -# Flashing and device boot up should be succesful
    #BRCM_SWQTST_RTPVIDXMTR_EXTFLASH_CASE0
    -# Point rtpvidxmtr camera viewpoint to moving traffic video clip played
    in PC
    -# Video playback should be seen on VLC listener
    #BRCM_SWQTST_RTPVIDXMTR_VLCPB_CASE1
    -# In VLC listener check for the codec and
    resolution info (VLC->Tools->Codec Information)
    -# Codec Information should be H264-MPEG4-AVC(Part10)
    #BRCM_SWQTST_RTPVIDXMTR_CODEC_CASE3
    -# Resolution should be 1280x720p #BRCM_SWQTST_RTPVIDXMTR_RESOLUTION_CASE4
    -# In VLC listener check video playback
    -# Playback should be smooth (no video pauses/jerks)
    #BRCM_SWQTST_RTPVIDXMTR_SMOOTHPB_CASE5
    -# In VLC listener check for visual artifacts
    -# No visual artifacts should be seen (tearing, color slice, blockiness,
    pixelization, noise) #BRCM_SWQTST_RTPVIDXMTR_ARTIFACTS_CASE6
    -# Let the setup run for 8 hours
    -# Run steps 5 to 12

    @board bcm89103_ox3a_evk
    @board bcm89103_ar143_evk
    @board bcm89107_ox3a_evk
    @board bcm89109_ddr_ov10640_evk

*/
#define BRCM_SWQTSEQ_RTPVIDXMTR_STABILITY_SEQ3   (3UL)
