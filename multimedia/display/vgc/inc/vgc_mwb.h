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
    @ingroup grp_vgc_drv
    @addtogroup grp_vgc_drv_ifc
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

    @file vgc_mwb.h
    @brief VGC interface
    This header file contains the interface functions for VGC component.
    VGC exposes interfaces necessary to setup data pipeline and control composition by posting updated surface buffer attributes. Detailed interface along with necessary state machine and sequence diagrams is described in this header file.
    @version 0.53 Derived from AUTOSW_VGC_Driver_Interface_Specification_0.53.docx
*/

#ifndef VGC_MWB_H
#define VGC_MWB_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <system.h>
#include <vgc_common.h>


/**
    @name Component API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_SWARCH_VGC_MWB_MWBCFG_TYPE    (0x01U)   /**< @brief #VGC_MWBOutputCfgType 	*/
#define BRCM_SWARCH_VGC_MWB_CFG_TYPE       (0x02U)   /**< @brief #VGC_MwbCfgType 		*/
#define BRCM_SWARCH_VGC_MWB_STATS_TYPE     (0x03U)   /**< @brief #VGC_MwbStatsType 		*/
#define BRCM_SWARCH_VGC_MWB_INIT_PROC      (0x04U)   /**< @brief #VGC_MwbInit 			*/
#define BRCM_SWARCH_VGC_MWB_PROCESS_PROC   (0x05U)   /**< @brief #VGC_MWBProcess 		*/
#define BRCM_SWARCH_VGC_MWB_GETSTATS_PROC  (0x06U)   /**< @brief #VGC_MwbGetStats 		*/
/** @} */

/**
    @brief  All parameters needed to initialize VGC and its sub-components

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_MWBOutputCfgType {
    uint32_t width;                 /**< @brief Processing width in pixels */
    uint32_t height;                /**< @brief Processing height in pixels */
    uint32_t buffHeight;            /**< @brief Height of the buffer */
    VGC_PixFmtType fmt;             /**< @brief Format of the output buffer as #VGC_PixFmtType */
} VGC_MWBOutputCfgType;

/**
    @brief  All parameters needed to initialize VGC and its sub-components

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_MwbCfgType {
    VGC_MiscCfgType misc;           /**< @brief Miscellaneous configuration as #VGC_MiscCfgType */
    VGC_CMPCfgType cmp;             /**< @brief Configuration for CMP as #VGC_CMPCfgType */
    VGC_GFDCfgType gfd;             /**< @brief Configuration for GFD as #VGC_GFDCfgType */
    VGC_MWBOutputCfgType mwb;       /**< @brief Configuration for memory write back as #VGC_MWBCfgType */
} VGC_MwbCfgType;

/**
    @brief  VGC Statistics information

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_MwbStatsType {
    BCM_TimeType start;                 /**< @brief Statistics Start time as #Time */
    BCM_TimeType end;                   /**< @brief Statistics End time as #Time */
    uint32_t bvbIRQCnt;             /**< @brief Number of BVB interrupts */
    uint32_t eotIRQCnt;             /**< @brief Number of EOT interrupts */
    uint32_t framesDisplayed;       /**< @brief Number of frames displayed */
    uint32_t queueVid;              /**< @brief Number of video buffers queued by Client */
    uint32_t queueGfx;              /**< @brief Number of graphics buffers queued by Client */
    uint32_t repeat1CntVid;         /**< @brief Number of Video Buffers displayed once */
    uint32_t repeat1CntGfx;         /**< @brief Number of Graphics Buffers displayed once */
} VGC_MwbStatsType;

/** @brief Initialize VGC

    This API intializes and configures VGC and all of its subcomponents.
    Memory pointed to by aCfg will not be dereferenced beyond this call and Client is at the liberty to free it up after the call

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aCfg                Pointer to configuration information

    Return values are documented in reverse-chronological order
    @return      void

    @post none

    @trace #BRCM_SWREQ_VGC_MWB

    @limitations none
*/
extern void VGC_MwbInit(const VGC_MwbCfgType *const aCfg);

/** @brief Send buffer to blend

    This API provides a buffer to VGC for blend. Attributes are copied internally and
    hence client is not mandated to retain aSurfAttr beyond the function call

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aVidSurfAttr        Pointer to Video Surface Attributes
    @param[in]   aVidBuffer          Pointer to Video buffer obtained from VGC
    @param[in]   aGfxSurfAttr        Pointer to Graphics Surface Attributes
    @param[in]   aGfxBuffer          Pointer to Graphics buffer obtained from VGC
    @param[in]   aOutBuff            Pointer to output buffer

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_UNINIT          Component is not initialized
    @retval      #BCM_ERR_INVAL_PARAMS    Pixel format of buffer is invalid
                                        or aOutBuff is NULL

    @post none

    @trace #BRCM_SWREQ_VGC_MWB

    @limitations none
*/
extern int32_t VGC_MWBProcess(const VGC_SurfAttrType *const aVidSurfAttr,
                               const VGC_BufferType *const aVidBuffer,
                               const VGC_SurfAttrType *const aGfxSurfAttr,
                               const VGC_BufferType *const aGfxBuffer,
                               const uint8_t *const aOutBuff);

/** @brief Get VGC Statistics

    This API provides VGC's statistics information at a location as
    requested by the client

    @behavior Sync, Non-reentrant

    @pre none

    @param[inout]   aStats          Pointer to memory where statistics can be filled

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK             Statistics information obtained successfully
    @retval      #BCM_ERR_UNINIT         Component is not initialised
    @retval      #BCM_ERR_INVAL_PARAMS   aStats is NULL

    @post none

    @trace #BRCM_SWREQ_VGC_MWB

    @limitations none
*/
extern int32_t VGC_MwbGetStats(VGC_MwbStatsType *const aStats);

#endif /* VGC_MWB_H */

/** @} */
