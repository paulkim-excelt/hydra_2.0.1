/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_vgc_ifc Interface
    @ingroup grp_vgc

    @addtogroup grp_vgc_ifc
    @{

    @section sec_vgc_overview Overview
    @image html vgc_overview.jpg
    The VGC component consists of an API, a VGC Driver and various sub-component driver modules. They are collectively controlled and exposed through an API named “VGC” as depicted in Figure.

    @section sec_vgc_fsm State Machine
    @image html vgc_state_machine.jpg
    Figure shows valid states and transitions of VGC driver. The same may be used as a reference while designing Client.

    States and descriptions are as below:
    -# UNINIT
        <br>After reset, the default state of VGC would be uninitialized state. VGC would enter this state after De-initialization sequence is executed.
    -# IDLE
        <br>VGC would be initialized and a pipeline would be setup based on input VGC configuration and be ready to stream data over to display or write-back to memory. This state is achieved when the VGC is initialized for the first time or whenever VGC completes composing a frame and is ready to accept another request.
    -# COMPOSING
        <br>VGC would be composing all enabled surfaces.

    @section sec_vgc_seq Sequence Diagrams
    @subsection subsec_vgc_seq_start    Start-up Sequence
    @image html vgc_seq_start.jpg
    Typical sequence of starting-up the composer is as shown in the sequence diagram

    @subsection subsec_vgc_seq_stop    Stop Sequence
    @image html vgc_seq_stop.jpg
    Typical sequence of stopping the composer is as shown in the sequence diagram

    @section sec_vgc_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                             |
    | Memory Bandwidth  |   TBD                                                |
    | HW Utilization    |   TBD                                             |

    @includedoc vgc.dep

    @limitations
    -# Interrupt Latency: OS and system architecture guarantees worst case interrupt latency of around fifty microseconds
    -# Color Format: Graphics buffers have alpha plane indicating blend factor
    -# Overlay: A configurable background is overlaid with Video (which forms the back plane) over which graphics (which forms the front plane) is overlaid.
    -# Access Protection
        -# Processor Memory Protection and/or Management Unit should be configured to provide read and write access to registers of all IP blocks under VGC
        -# IO Memory Protection and/or Management Unit should be configured to provide read (optionally write) access to buffer memory.
    -# PLL Initialization: Display PLL needs to be initialized prior to initializing VGC. Refer SADS[1] and MCU Driver Interface Specification[5] for details.

    |   References                                                      |
    |-------------------------------------------------------------------|
    |   @anchor ref_uml_spec UML 2.5 specification                      |

    @file vgc.h
    @brief VGC interface
    This header file contains the interface functions for VGC component.
    VGC exposes interfaces necessary to setup data pipeline and control composition by posting updated surface buffer attributes. Detailed interface along with necessary state machine and sequence diagrams is described in this header file.
    @version 0.53 Derived from AUTOSW_VGC_Driver_Interface_Specification_0.53.docx
*/

#ifndef VGC_H
#define VGC_H

#ifndef ENABLE_VGC_DISPLAY
#include <vgc_mwb.h>
#else
#include <vgc_display.h>
#endif

#endif /* VGC_H */

/** @} */
