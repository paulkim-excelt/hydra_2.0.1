/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_vgc_media_comp_ifc Interface
    @ingroup grp_vgc_media_comp

    @addtogroup grp_vgc_media_comp_ifc
    @{

    @section sec_vgcc_overview Overview
    @image html vgcc_overview.jpg
    The VGC component exposes the API to hook to media framework on one side and interacts with VGC driver library on the other side.

    @section sec_vgcc_fsm State Machine
    @image html vgcc_state_machine.jpg
    Figure shows valid states and transitions of VGC component. The same may be used as a reference while designing/developing Client/Application.

    States and descriptions are as below:
    -# RESET
        <br>Default state of the component is RESET. VGC component would enter this state upon de-initialization from READY or RUNNING states
    -# INIT
        <br>When in RESET state, upon receiving Initialization command from framework, VGC component would initialize VGC driver if output port is enabled and enter INIT state
    -# READY
        <br>Upon receiving Configure command from framework in INIT state, VGC component would configure itself and enable required Surfaces and enter READY state.
        <br>Upon receiving Stop command from framework in RUNNING state, VGC component would avoid requesting any further buffers from enabled input ports and enter READY state
    -# RUNNING
        <br>Upon receiving Start command in READY state, VGC component would enter RUNNING state by initializing circular queues for message exchange and request buffers from all enabled input ports

    @section sec_vgc_seq Sequence Diagrams

    @section sec_vgc_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                             |
    | Memory Bandwidth  |   TBD                                                |
    | HW Utilization    |   TBD                                             |

    @includedoc vgc_comp.dep

    @limitations
    -# In the case where display is enabled, it is assumed that VGC_Init API with necessary configuration and VGC_Start API have been called
    -# Access Protection
        -# Processor Memory Protection and/or Management Unit should be configured to provide read and write access to registers of all IP blocks under VGC
        -# IO Memory Protection and/or Management Unit should be configured to provide read (optionally write) access to buffer memory.
    -# PLL Initialization
        Display PLL needs to be initialized prior to initializing VGC. Refer SADS[1] and MCU Driver Interface Specification[5] for details.
    -# VGC component can either be configured to output to memory or to a display panel and not both together
    -# Supported input video format is YUV 420 Planar 8-bit per component
    -# Supported graphics pixel formats are 32-bits per pixel ARGB and 4-bits per pixel with 32bit ARGB palette
    -# Supported output format when writing back to memory is YUV 420 Planar 8-bit per component

    |   References                                                      |
    |-------------------------------------------------------------------|
    |   @anchor ref_uml_spec UML 2.5 specification                      |

    @file vgc_comp.h
    @brief Interface for VGC component
    This header file contains the interface functions for VGC component.

    @version 0.1 Draft version
*/

#ifndef VGCC_COMP_H
#define VGCC_COMP_H

#include "msg_queue.h"
#include "component.h"
#include "media_port.h"

/**
    @name VGCC API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_SWARCH_VGCC_GETFNTABLE_PROC         (0x00U) /**< @brief #VGCC_GetCompFnTable */
/** @} */


/** @brief Get the functon table

    Get the function table

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   void

    @retval     Function table

    @post None

    @trace  #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations None
*/
extern const MSYS_CompFnTblType* VGCC_GetCompFnTable(void);

#endif /* VGCC_COMP_H */

/** @} */
