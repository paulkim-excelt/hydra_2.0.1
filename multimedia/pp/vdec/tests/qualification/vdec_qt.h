/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_vdec_qt VDEC Qualification Tests
    @ingroup grp_vdec

    @addtogroup grp_vdec_qt
    @{

    @file vdec_qt.h
    @brief This header file contains internal interfaces for vdec qualification
           tests.
    @version 0.1 Initial Version
*/

#ifndef VDEC_QT_H
#define VDEC_QT_H

/**
    @brief Initialization of VDEC Library

    @pre Library state should be #VDEC_STATE_RESET

    @test Initialize VDEC Library

    @post Should return #BCM_ERR_OK. Library state should be #VDEC_STATE_INIT

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_INIT     (0UL)

/**
    @brief Configuration of VDEC Library

    @pre Library state should be #VDEC_STATE_INIT

    @test Configure VDEC Library

    @post Should return #BCM_ERR_OK. Library state should be #VDEC_STATE_IDLE
          and stream state should be #VDEC_STREAMSTATE_INIT

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_CONFIG   (1UL)

/**
    @brief Get stream info from VDEC Library SPS

    @pre Library state should be #VDEC_STATE_IDLE or #VDEC_STATE_RUNNING and
         stream state should be #VDEC_STREAMSTATE_INIT or #VDEC_STREAMSTATE_READY

    @test Get stream info from library for SPS and store in slice info strcuture

    @post Should return #BCM_ERR_OK. In slice info, slice type should be
          #VDEC_SLICE_SPS. Width and height should be correct

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_STREAMINFO_SPS    (2UL)

/**
    @brief Get stream info from VDEC Library PPS

    @pre Library state should be #VDEC_STATE_IDLE or #VDEC_STATE_RUNNING and
         stream state should be #VDEC_STREAMSTATE_INIT or #VDEC_STREAMSTATE_READY

    @test Get stream info from library for PPS and store in slice info strcuture

    @post Should return #BCM_ERR_OK. In slice info, slice type should be
          #VDEC_SLICE_PPS. stream state should be #VDEC_STREAMSTATE_READY

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_STREAMINFO_PPS    (3UL)

/**
    @brief Get stream info from VDEC Library Video

    @pre Library state should be #VDEC_STATE_IDLE or #VDEC_STATE_RUNNING.
         stream state should be #VDEC_STREAMSTATE_READY

    @test Get stream info from library for Video slice and store in slice info
          strcuture

    @post Should return #BCM_ERR_OK. In slice info, slice type should be
          #VDEC_SLICE_VIDEO. New frame information should be correct along with
          MB row and MB col details.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_STREAMINFO_VIDEO    (4UL)

/**
    @brief Submit for decoding

    @pre Library state should be #VDEC_STATE_IDLE and stream state should
         be #VDEC_STREAMSTATE_READY

    @test Submit buffer to library for decoding(entire frame in single slice)

    @post Should return #BCM_ERR_OK. Library state should be #VDEC_STATE_RUNNING
          Stream state should be #VDEC_STREAMSTATE_BUSY

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_DECODE    (5UL)

/**
    @brief Process decode events in library

    @pre Library state should be #VDEC_STATE_RUNNING

    @test Process decode completion

    @post Should return flags with #VDEC_FLAGS_DECODE_COMPLETED and
          #VDEC_FLAGS_FRAME_AVAILABLE set. Stream state should be
          #VDEC_STREAMSTATE_READY

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_PROCESS0    (6UL)

/**
    @brief Get frame from library

    @pre Library state should be #VDEC_STATE_RUNNING and copy state should be
         #VDEC_COPYSTATE_INIT or #VDEC_COPYSTATE_READY

    @test Get frame request to libary

    @post Should return #BCM_ERR_OK. Library state should be #VDEC_STATE_RUNNING.
          Copy state should be #VDEC_COPYSTATE_BUSY

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_COPYFRAME    (7UL)

/**
    @brief Process fill events in library

    @pre Library state should be #VDEC_STATE_RUNNING and Copy state should be
         #VDEC_COPYSTATE_BUSY

    @test Process get frame request completion

    @post Should return flags with #VDEC_FLAGS_FRAME_DATA_COPY_DONE set.
          Library state should be #VDEC_STATE_IDLE and Copy state should be
          #VDEC_COPYSTATE_READY.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_PROCESS1    (8UL)

/**
    @brief De-Init library

    @pre Library state should be #VDEC_STATE_IDLE

    @test De-Initialization of library

    @post Should return #BCM_ERR_OK.Library state should be #VDEC_STATE_RESET

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define BRCM_SWQTST_VDEC_LIBRARY_DEINIT    (9UL)

/**
    @brief Initialization of VDEC Component

    @pre Media system should be defined

    @test Initialize the media system

    @post Should return #BCM_ERR_OK.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#define BRCM_SWQTST_VDEC_COMPONENT_INIT     (10UL)

/**
    @brief Configuration of VDEC Component

    @pre Media system should be initialized

    @test Configure the media system

    @post Should return #BCM_ERR_OK.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#define BRCM_SWQTST_VDEC_COMPONENT_CONFIG   (11UL)

/**
    @brief Starting of VDEC Component

    @pre Media system should be configured

    @test Start the media system

    @post Should return #BCM_ERR_OK.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#define BRCM_SWQTST_VDEC_COMPONENT_START   (12UL)

/**
    @brief Stopping of VDEC Component

    @pre Media system should be started

    @test Stop the media system

    @post Should return #BCM_ERR_OK.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#define BRCM_SWQTST_VDEC_COMPONENT_STOP   (13UL)

/**
    @brief De-Initialization of VDEC Component

    @pre Media system should be stopped or initialized

    @test De-Initialize the media system

    @post Should return #BCM_ERR_OK.

    @functional     Yes

    @type           Pos

    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#define BRCM_SWQTST_VDEC_COMPONENT_DEINIT   (14UL)

/** @brief VDEC Test sequence 0

    @code{.c}
    loop for 5 times
        ret = MSYS_Init()
        For expected results refer to #BRCM_SWQTST_VDEC_COMPONENT_INIT

        ret = MSYS_Configure()
        For expected results refer to #BRCM_SWQTST_VDEC_COMPONENT_CONFIG

        ret = MSYS_Start()
        For expected results refer to #BRCM_SWQTST_VDEC_COMPONENT_START

        Wait for alarm

        ret = MSYS_Stop()
        For expected results refer to #BRCM_SWQTST_VDEC_COMPONENT_STOP

        ret = MSYS_DeInit()
        For expected results refer to #BRCM_SWQTST_VDEC_COMPONENT_DEINIT

    @endcode

    @board bcm89107_evk
    @board bcm89105_ddr_evk
    @board bcm89109_ddr_evk

    @auto           Yes

    @type           Sanity

    @testproc #BRCM_SWQTST_VDEC_COMPONENT_INIT
    @testproc #BRCM_SWQTST_VDEC_COMPONENT_CONFIG
    @testproc #BRCM_SWQTST_VDEC_COMPONENT_START
    @testproc #BRCM_SWQTST_VDEC_COMPONENT_STOP
    @testproc #BRCM_SWQTST_VDEC_COMPONENT_DEINIT

*/
#define BRCM_SWQTSEQ_VDEC_SEQ0   (0UL)

/** @brief VDEC Test sequence 1

    @code{.c}
    ret = VDEC_Init()
    For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_INIT

    ret = VDEC_Configure()
    For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_CONFIG

    loop for all frames

        ret = VDEC_GetStreamInfo(slice sps)
        For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_STREAMINFO_SPS

        ret = VDEC_GetStreamInfo(slice pps)
        For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_STREAMINFO_PPS

        ret = VDEC_GetStreamInfo(slice video)
        For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_STREAMINFO_VIDEO

        ret = VDEC_Decode(slice)
        For expected results refer to  #BRCM_SWQTST_VDEC_LIBRARY_DECODE

        wait for PP event

        flags = VDEC_Process()
        For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_PROCESS0

        ret = VDEC_CopyFrameData()
        For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_COPYFRAME

        wait for PP event

        flags = VDEC_Process()
        For expected results refer to #BRCM_SWQTST_VDEC_LIBRARY_PROCESS1

    ret = VDEC_DeInit()
    For expected results refer to  #BRCM_SWQTST_VDEC_LIBRARY_DEINIT

    @endcode

    @board bcm89107_evk
    @board bcm89105_ddr_evk
    @board bcm89109_ddr_evk

    @auto           Yes

    @type           Sanity

    @testproc #BRCM_SWQTST_VDEC_LIBRARY_INIT
    @testproc #BRCM_SWQTST_VDEC_LIBRARY_CONFIG
    @testproc #BRCM_SWQTST_VDEC_LIBRARY_STREAMINFO
    @testproc #BRCM_SWQTST_VDEC_LIBRARY_DECODE
    @testproc #BRCM_SWQTST_VDEC_LIBRARY_PROCESS0
    @testproc #BRCM_SWQTST_VDEC_LIBRARY_FILLBUFFER
    @testproc #BRCM_SWQTST_VDEC_LIBRARY_PROCESS1
    @testproc #BRCM_SWQTST_VDEC_LIBRARY_DEINIT

*/
#define BRCM_SWQTSEQ_VDEC_SEQ1   (1UL)

#endif /* VDEC_QT_H */

/** @} */
