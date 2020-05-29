/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_avce AVC Encoder
    @ingroup grp_media_system

    @addtogroup grp_avce
    @{
    @section sec_avce_overview Overview
    @image html avce_overview.jpg
    AVC Encoder is accelerated on Pixel Processor. AVC Encoder consists of AVCE
    library and AVCE media component layers. AVCE library controls the firmware
    running on Pixel Processor.
    <BR>AVCE media component accept, processes and acknowledge commands from media
    framework. AVCE media component also controls/handles the data flow using
    the media framework layers. AVCE Media component sends configuration header
    bitstream (SPS and PPS) in the beginning. At every frame boundary, SPS/PPS
    is added to the bitstream alternatively.

    @subsection subsec_avce_example Sample
    @todo ADD STATE MACHINE

    @section sec_avce_fsm State Machine
    @todo ADD STATE MACHINE

    @section sec_avce_seq Sequence Diagrams
    @todo ADD SEQUENCE DIAGRAM

    @section sec_avce_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   Update CPU load here                            |
    | Memory Bandwidth  |   Update Memory Bandwidth here                    |
    | HW Utilization    |   Update Hardware utilization here                |
    @todo

    @limitations None

    @file avce.h
    @brief avce component interface function headers
    This file contains the interface function definitions for avce component

    @version 0.65 Added doxygen comments
*/
#ifndef AVCE_H
#define AVCE_H

#include <stdint.h>
#include <component.h>
#include <circ_queue.h>


/**
    @name AVCE API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_ARCH_AVCE_GETFNTABLE_PROC         (0x00U) /**< @brief #AVCE_GetCompFnTable */
/** @} */


/** @brief Get the functon table

    Get the function table

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   void

    @retval     Function table

    @post None

    @trace  #BRCM_ARCH_AVCE_GETFNTABLE_PROC  #BRCM_REQ_AVCE_MEDIA_COMPONENT

    @limitations None
*/
extern const MSYS_CompFnTblType* AVCE_GetCompFnTable(void);

#endif /* AVCE_H */
/** @} */
