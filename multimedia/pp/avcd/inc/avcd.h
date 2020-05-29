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
    @defgroup grp_avcd AVC Decoder
    @ingroup grp_media_system

    @addtogroup grp_avcd
    @{
    @section sec_avcd_overview Overview
    @image html avcd_overview.jpg
    AVC Decoder is accelerated on Pixel Processor. AVC Decoder consists of AVCD
    library and AVCD media component layers. AVCD library controls the firmware
    running on Pixel Processor.
    <BR>AVCD media component accept, processes and acknowledge commands from media
    framework. AVCD media component also controls/handles the data flow using
    the media framework layers.

    @section sec_avcd_fsm State Machine

    @section sec_avcd_seq Sequence Diagrams

    @section sec_avcd_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   Update CPU load here                            |
    | Memory Bandwidth  |   Update Memory Bandwidth here                    |
    | HW Utilization    |   Update Hardware utilization here                |

    @limitations None

    @file avcd.h
    @brief AVC Decoder component interface function headers

    This file contains the interface function declarations for AVC Decoer component

    @version 0.65 Added doxygen comments
*/

#ifndef AVCD_H
#define AVCD_H

#include<stdint.h>
#include <component.h>
#include <circ_queue.h>

/**
    @name AVCD API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_ARCH_AVCD_GETFNTABLE_PROC         (0x00U) /**< @brief #AVCD_GetCompFnTable */
/** @} */


/** @brief Get the functon table

    Get the function table

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   void

    @retval     Function table

    @post None

    @trace  #BRCM_ARCH_AVCD_GETFNTABLE_PROC  #BRCM_REQ_AVCD_MEDIA_COMPONENT

    @limitations None
*/
extern const MSYS_CompFnTblType* AVCD_GetCompFnTable(void);

#endif /* AVCD_H */
/** @} */
