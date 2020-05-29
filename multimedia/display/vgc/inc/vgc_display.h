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
    @defgroup grp_vgc_drv VGC_DRV
    @ingroup grp_vgc

    @defgroup grp_vgc_drv_ifc Interface
    @ingroup grp_vgc_drv

    @addtogroup grp_vgc_drv_ifc
    @{

    @section sec_vgc_drv_overview Overview
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

    @file vgc_display.h
    @brief VGC interface
    This header file contains the interface functions for VGC component.
    VGC exposes interfaces necessary to setup data pipeline and control composition by posting updated surface buffer attributes. Detailed interface along with necessary state machine and sequence diagrams is described in this header file.
    @version 0.53 Derived from AUTOSW_VGC_Driver_Interface_Specification_0.53.docx
*/

#ifndef VGC_DISPLAY_H
#define VGC_DISPLAY_H

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
#define BRCM_SWARCH_VGC_DISP_SURFACE_TYPE     (0x00U)  /**< @brief #VGC_SurfaceType         */
#define BRCM_SWARCH_VGC_DISP_LVDSMODE_TYPE    (0x01U)  /**< @brief #VGC_LVDSModeType        */
#define BRCM_SWARCH_VGC_DISP_LVDSDE_TYPE      (0x02U)  /**< @brief #VGC_LVDSDEType          */
#define BRCM_SWARCH_VGC_DISP_TS_CNT_MACRO     (0x03U)  /**< @brief #VGC_DISP_TS_CNT         */
#define BRCM_SWARCH_VGC_DISP_PVVIDTIM_TYPE    (0x04U)  /**< @brief #VGC_PVVideoTimingType   */
#define BRCM_SWARCH_VGC_DISP_PVCFG_TYPE       (0x05U)  /**< @brief #VGC_PVCfgType           */
#define BRCM_SWARCH_VGC_DISP_LVDSCFG_TYPE     (0x06U)  /**< @brief #VGC_LVDSCfgType         */
#define BRCM_SWARCH_VGC_DISP_BT656CFG_TYPE    (0x07U)  /**< @brief #VGC_BT656CfgType        */
#define BRCM_SWARCH_VGC_DISP_DSICFG_TYPE      (0x08U)  /**< @brief #VGC_DSICfgType          */
#define BRCM_SWARCH_VGC_DISP_CFG_TYPE         (0x09U)  /**< @brief #VGC_DispCfgType         */
#define BRCM_SWARCH_VGC_DISP_STATS_TYPE       (0x0AU)  /**< @brief #VGC_DispStatsType       */
#define BRCM_SWARCH_VGC_DSI_CTRL_TYPE         (0x0BU)  /**< @brief #VGC_DsiCtrlType         */
#define BRCM_SWARCH_VGC_DISP_INIT_PROC        (0x0CU)  /**< @brief #VGC_DispInit            */
#define BRCM_SWARCH_VGC_START_PROC            (0x0DU)  /**< @brief #VGC_Start               */
#define BRCM_SWARCH_VGC_STOP_PROC             (0x0EU)  /**< @brief #VGC_Stop                */
#define BRCM_SWARCH_VGC_QUEUE_PROC            (0x0FU)  /**< @brief #VGC_QueueBuffer         */
#define BRCM_SWARCH_VGC_RELEASE_PROC          (0x10U)  /**< @brief #VGC_ReleaseBuffer       */
#define BRCM_SWARCH_VGC_DEQUEUE_PROC          (0x11U)  /**< @brief #VGC_DequeueBuffer       */
#define BRCM_SWARCH_VGC_ENSURF_PROC           (0x12U)  /**< @brief #VGC_EnableSurface       */
#define BRCM_SWARCH_VGC_DISSURF_PROC          (0x13U)  /**< @brief #VGC_DisableSurface      */
#define BRCM_SWARCH_VGC_DISP_GETSTATS_PROC    (0x14U)  /**< @brief #VGC_DispGetStats        */
/** @} */

/**
    @name VGC_SurfaceType Masks
    @{
    @brief Surface types

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef uint32_t VGC_SurfaceType;                           /**< @brief Surface types */
#define VGC_DISP_SURFACE_VID        (0x00000000UL)          /**< @brief Video Surface */
#define VGC_DISP_SURFACE_GFX        (0x00000001UL)          /**< @brief Graphics Surface */
#define VGC_DISP_SURFACE_MAX        (VGC_DISP_SURFACE_GFX)  /**< @brief Maximum value */
/** @} */

/**
    @brief LVDS Modes

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE

*/
typedef uint32_t VGC_LVDSModeType;                          /**< @brief LVDS Modes */
#define VGC_DISP_LVDSMODE_18        (0x00000000UL)          /**< @brief 18-bit mode */
#define VGC_DISP_LVDSMODE_24_1      (0x00000001UL)          /**< @brief 24-bit mode OpenLDI compatible */
#define VGC_DISP_LVDSMODE_24_0      (0x00000002UL)          /**< @brief 24-bit mode over 3 lanes */

/**
    @brief LVDS DE output Modes

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE

*/
typedef uint32_t VGC_LVDSDEType;                             /**< @brief LVDS DE output Modes */
#define VGC_DISP_LVDSDE_HACT_VACT    (0x00000000UL)          /**< @brief HACT and VACT */
#define VGC_DISP_LVDSDE_VACT         (0x00000001UL)          /**< @brief VACT */
#define VGC_DISP_LVDSDE_VACT_VBP_VFP (0x00000002UL)          /**< @brief VACT, VBP and VFP */
#define VGC_DISP_LVDSDE_OFF          (0x00000003UL)          /**< @brief OFF */

/**
    @brief DSI peripheral and Control

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE

*/
typedef uint32_t VGC_DsiCtrlType;                                 /**< @brief DSI control Flags as #VGC_DsiCtrlType*/
#define VGC_DSI_CTRL_HSDT_EOT_EN                (0x00000001UL)    /**< @brief End of High speed data transmission */
#define VGC_DSI_CTRL_LPDT_EOT_EN                (0x00000002UL)    /**< @brief End of Low power data transmission*/
#define VGC_DSI_CTRL_RX_LPDT_EOT_EN             (0x00000004UL)    /**< @brief End of Low power data reception */
#define VGC_DSI_CTRL_ST_END                     (0x00000008UL)    /**< @brief control transfer of HSYNC and SYNC events */
#define VGC_DSI_CTRL_HBP_CTRL                   (0x00000010UL)    /**< @brief behaviour of DSI1 during horizontal back porch */
#define VGC_DSI_CTRL_HFP_CTRL                   (0x00000020UL)    /**< @brief behaviour of DSI1 during horizontal front porch  */
#define VGC_DSI_CTRL_VBLP_CTRL                  (0x00000040UL)    /**< @brief behaviour of DSI1 during vertical blanking*/
#define VGC_DSI_CTRL_HACTIVE_NULL               (0x00000080UL)    /**< @brief behaviour of DSI1 during horizontal active  */
#define VGC_DSI_CTRL_LP_STOP_DISABLE            (0x00000100UL)    /**< @brief Stop State Disabled */
#define VGC_DSI_CTRL_LP_STOP_LINE               (0x00000200UL)    /**< @brief Enter LP_STOP once every line */
#define VGC_DSI_CTRL_LP_STOP_FRAME              (0x00000400UL)    /**< @brief Enter LP_STOP once every frame */
#define VGC_DSI_CTRL_VID_ST_VACTO               (0x00000800UL)    /**< @brief Hsync active mode set */
#define VGC_DSI_CTRL1_EN                        (0x00001000UL)    /**< @brief Enable Disp0 ctrl1 */
#define VGC_DSI_CTRL_CLANE_HS_EN                (0x00002000UL)    /**< @brief clock lane to enter HS mode  */
#define VGC_DSI_CTRL_VS_GAP_EN                  (0x00004000UL)    /**< @brief Null packets at the end of each line in VSTART */
#define VGC_DSI_CTRL_VS_LL_GAP_EN               (0x00008000UL)    /**< @brief Null packets at the end of the last line of VSTART */
#define VGC_DSI_CTRL_VS_GAP_NBLP                (0x00010000UL)    /**< @brief Null or blanking packet end during VSTART */
#define VGC_DSI_CTRL_VBLP_GAP_EN                (0x00020000UL)    /**< @brief Null/BLP packets at the end of VBLP  */
#define VGC_DSI_CTRL_VBLP_GAP_NBLP              (0x00040000UL)    /**< @brief Null or blanking packet to be sent at the end of VBLP  */
#define VGC_DSI_CTRL_VBLP_MAN_NBLP              (0x00080000UL)    /**< @brief Null or blanking packet to be sent to cover VBLP */
#define VGC_DSI_CTRL_HSA_MAN_NBLP               (0x00100000UL)    /**< @brief Null or blanking packet to be sent to cover HSA */
#define VGC_DSI_CTRL_HBP_GAP_EN                 (0x00200000UL)    /**< @brief Null/BLP packets at the end of HBP  */
#define VGC_DSI_CTRL_HBP_GAP_NBLP               (0x00400000UL)    /**< @brief Null or blanking packet to be sent at the end of HBP */
#define VGC_DSI_CTRL_HBP_MAN_NBLP               (0x00800000UL)    /**< @brief Null or blanking packet to be sent to cover HBP */
#define VGC_DSI_CTRL_HACT_GAP_EN                (0x01000000UL)    /**< @brief Sending the Null/BLP packets at the end of HACT */
#define VGC_DSI_CTRL_HACT_GAP_NBLP              (0x02000000UL)    /**< @brief Null or blanking packet to be sent at the end of HACT */
#define VGC_DSI_CTRL_HACT_MAN_NBLP              (0x04000000UL)    /**< @brief Null or blanking packet to be sent to cover HACT */
#define VGC_DSI_CTRL_HFP_GAP_EN                 (0x08000000UL)    /**< @brief Null/BLP packets at the end of HFP */
#define VGC_DSI_CTRL_HFP_GAP_NBLP               (0x10000000UL)    /**< @brief Null or blanking packet to be sent at the end of HFP  */
#define VGC_DSI_CTRL_HFP_MAN_NBLP               (0x20000000UL)    /**< @brief Null or blanking packet to be sent to cover HFP */
#define VGC_DSI_CTRL_DATA_TYPE_OV_EN            (0x40000000UL)    /**< @brief enables the overwrite the ID field in RGB888 */
#define VGC_DSI_CTRL_CSI2_MODE_EN               (0x80000000UL)    /**< @brief Enable CSI2 mode through DSI */

/**
    @brief Number of TimeStamp samples to collect for statistics information.
    First 20 samples are held

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
#define VGC_DISP_TS_CNT        (20UL)

/**
    @brief  Structure to hold timing parameters needed for a display.
            For horizontal timing, the unit is pixel
            For vertical timing, the unit is line

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_PVVideoTimingType {
    uint32_t sync;                  /**< @brief Sync period */
    uint32_t bp;                    /**< @brief Back porch period */
    uint32_t act;                   /**< @brief Period of actual video information */
    uint32_t fp;                    /**< @brief Front porch period */
} VGC_PVVideoTimingType;

/**
    @brief  Structure to hold parameters needed to initialize PixelValve

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_PVCfgType {
    uint8_t isCont;                 /**< @brief TRUE: Continuous mode, FALSE: SingleShot mode */
    VGC_PVVideoTimingType horz;        /**< @brief Horizontal video timing information in pixels as #PV_VideoTimingType*/
    VGC_PVVideoTimingType vert;        /**< @brief Vertical video timing information in lines as #PV_VideoTimingType*/
} VGC_PVCfgType;

/**
    @brief  Structure to hold LVDS configuration

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_LVDSCfgType {
    VGC_LVDSModeType mode;          /**< @brief Mode of operation as #VGC_LVDSModeType*/
    VGC_LVDSDEType de;              /**< @brief DE output mode as #VGC_LVDSDEType */
    uint8_t isSyncEnabled;          /**< @brief Enable Hsync and Vsync outputs */
} VGC_LVDSCfgType;

/**
    @brief  Structure to hold BT656 configuration

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_BT656CfgType {
    uint32_t lanes;                 /**< @brief Number of BT656 lanes to operate with */
} VGC_BT656CfgType;

/**
    @brief  Structure to hold DSI configuration

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_DSICfgType {
    VGC_DsiCtrlType ctrl;               /**< @brief DSI Control as #VGC_DsiCtrlType */
    uint32_t t0;                        /**< @brief Clock Lane Timing0 value */
    uint32_t t1;                        /**< @brief Clock Lane Timing1 value */
    uint32_t t2;                        /**< @brief Clock Lane Timing2 value */
    uint32_t t3;                        /**< @brief Data Lane Timing3 value */
    uint32_t t4;                        /**< @brief Data Lane Timing4 value */
    uint32_t t5;                        /**< @brief Data Lane Timing5 value */
    uint32_t t6;                        /**< @brief Data Lane Timing6 value */
    uint32_t t7;                        /**< @brief Data Lane Timing7 value */
    uint8_t lanes;                      /**< @brief Number of data lanes */
    uint32_t vChannId;                  /**< @brief Virtual channel ID of Video pkts */
    uint32_t nullBlkPkts;               /**< @brief Set minimum length Null/blanking packets */
    uint32_t rgb888Id;                  /**< @brief ID value in RGB888 mode for CSI-2 protocol*/
} VGC_DSICfgType;

/**
    @brief  All parameters needed to initialize VGC and its sub-components

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_DispCfgType {
    VGC_MiscCfgType misc;          /**< @brief Miscellaneous configuration as #VGC_MiscCfgType */
    VGC_CMPCfgType cmp;            /**< @brief Configuration for CMP as #VGC_CMPCfgType */
    VGC_GFDCfgType gfd;            /**< @brief Configuration for GFD as #VGC_GFDCfgType */
    VGC_PVCfgType pv;              /**< @brief Configuration for PV as #VGC_PVCfgType */
    VGC_LVDSCfgType lvds;          /**< @brief Configuration for LVDS as #VGC_LVDSCfgType */
    VGC_BT656CfgType bt656;        /**< @brief Configuration for BT656 as #VGC_BT656CfgType */
    VGC_DSICfgType dsi;            /**< @brief Configuration for DSI as #VGC_DSICfgType */
} VGC_DispCfgType;

/**
    @brief  VGC Statistics information

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_DispStatsType {
    BCM_TimeType start;                 /**< @brief Statistics Start time as #Time */
    BCM_TimeType end;                   /**< @brief Statistics End time as #Time */
    BCM_TimeType firstISR;              /**< @brief Time when VGC_DispDrvPVInterruptHandler was first triggered as #Time*/
    uint32_t peakISRTime;           /**< @brief Maximum time consumed by VGC_DispDrvPVInterruptHandler */
    uint32_t pwrDwnIRQCnt;          /**< @brief Number of PwrDwn interrupts */
    uint32_t bvbIRQCnt;             /**< @brief Number of BVB interrupts */
    uint32_t pvIRQCnt;              /**< @brief Number of PV interrupts */
    uint32_t pvErrors;              /**< @brief Number of errors generated by PixelVale */
    uint32_t framesDisplayed;       /**< @brief Number of frames displayed */
    uint32_t queueVid;              /**< @brief Number of video buffers queued by Client */
    uint32_t dequeueVid;            /**< @brief Number of video buffers dequeued by Client */
    uint32_t releaseVid;            /**< @brief Number of video buffers released by Client */
    uint32_t queueGfx;              /**< @brief Number of graphics buffers queued by Client */
    uint32_t dequeueGfx;            /**< @brief Number of graphics buffers dequeued by Client */
    uint32_t releaseGfx;            /**< @brief Number of graphics buffers released by Client */
    uint32_t repeat1CntVid;         /**< @brief Number of Video Buffers displayed once */
    uint32_t repeat2CntVid;         /**< @brief Number of Video Buffers displayed twice */
    uint32_t repeat3CntVid;         /**< @brief Number of Video Buffers displayed thrice */
    BCM_TimeType repeat3TsVid[VGC_DISP_TS_CNT];      /**< @brief TimeStamp of first #VGC_DISP_TS_CNT video buffers which are displayed 3 times as #Time*/
    uint32_t repeat4CntVid;         /**< @brief Number of Video Buffers displayed 4 times */
    BCM_TimeType repeat4TsVid[VGC_DISP_TS_CNT];      /**< @brief TimeStamp of first #VGC_DISP_TS_CNT video buffers which are displayed 4 times as #Time */
    uint32_t repeatNCntVid;         /**< @brief Number of Video Buffers displayed >4 times */
    uint32_t repeatNValVid[VGC_DISP_TS_CNT]; /**< @brief Count of first #VGC_DISP_TS_CNT video buffers which are displayed >4 times*/
    BCM_TimeType repeatNTsVid[VGC_DISP_TS_CNT];      /**< @brief TimeStamp of first #VGC_DISP_TS_CNT video buffers which are displayed >4 times as #Time */
    uint32_t repeatMaxVid;          /**< @brief Maximum number of times a Video Buffer was repeated */
    uint32_t repeat1CntGfx;         /**< @brief Number of Graphics Buffers displayed once */
    uint32_t repeat2CntGfx;         /**< @brief Number of Graphics Buffers displayed twice */
    uint32_t repeat3CntGfx;         /**< @brief Number of Graphics Buffers displayed thrice */
    BCM_TimeType repeat3TsGfx[VGC_DISP_TS_CNT];      /**< @brief TimeStamp of first #VGC_DISP_TS_CNT graphics buffers which are displayed 3 times as #Time */
    uint32_t repeat4CntGfx;         /**< @brief Number of Graphics Buffers displayed 4 times */
    BCM_TimeType repeat4TsGfx[VGC_DISP_TS_CNT];      /**< @brief TimeStamp of first #VGC_DISP_TS_CNT graphics buffers which are displayed 4 times as #Time */
    uint32_t repeatNCntGfx;         /**< @brief Number of Graphics Buffers displayed >4 times */
    uint32_t repeatNValGfx[VGC_DISP_TS_CNT]; /**< @brief Count of first #VGC_DISP_TS_CNT graphics buffers which are displayed >4 times*/
    BCM_TimeType repeatNTsGfx[VGC_DISP_TS_CNT];      /**< @brief TimeStamp of first #VGC_DISP_TS_CNT graphics buffers which are displayed >4 times as #Time */
    uint32_t repeatMaxGfx;          /**< @brief Maximum number of times a Graphics Buffer was repeated */
} VGC_DispStatsType;

/** @brief Initialize VGC

    This API intializes and configures VGC and all of its subcomponents.
    Memory pointed to by aCfg will not be dereferenced beyond this call and Client is at the liberty to free it up after the call

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aCfg                Pointer to configuration information

    Return values are documented in reverse-chronological order
    @return      void

    @post none

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern void VGC_DispInit(const VGC_DispCfgType *const aCfg);

/** @brief Start the VGC

    This API starts VGC and all of its subcomponents to compose and render
    one or more frames based on configuration provided through VGC_Init

    @behavior Sync, Non-reentrant

    @pre none

    @param       none

    <br>Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_UNINIT          VGC is not initialized
    @retval      #BCM_ERR_INVAL_STATE     Subcomponent is not in correct state

    @post none

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern int32_t VGC_Start(void);

/** @brief Stops the VGC

    This API requests the VGC to stop at the end of current frame.
    Valid only in continuous mode of operation.

    @behavior Sync, Non-reentrant

    @pre none

    @param       none

    <br>Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_INVAL_STATE     VGC is not composing

    @post none

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern int32_t VGC_Stop(void);

/** @brief Queue a buffer

    This API queues a buffer to VGC display. Attributes are copied internally and
    hence client is not mandated to retain aSurfAttr beyond the function call

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aVidSurfAttr        Pointer to Video Surface Attributes
    @param[in]   aVidBuffer          Pointer to Video buffer obtained from VGC
    @param[in]   aGfxSurfAttr        Pointer to Graphics Surface Attributes
    @param[in]   aGfxBuffer          Pointer to Graphics buffer obtained from VGC

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_UNINIT          Component is not initialized
    @retval      #BCM_ERR_INVAL_BUF_STATE Buffer was not dequeued prior to queuing
    @retval      #BCM_ERR_INVAL_PARAMS    Pixel format of buffer is invalid
                                        or aOutBuff is NULL in MWB mode

    @post none

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern int32_t VGC_QueueBuffer(const VGC_SurfAttrType * const aVidSurfAttr,
                               const VGC_BufferType * const aVidBuffer,
                               const VGC_SurfAttrType * const aGfxSurfAttr,
                               const VGC_BufferType * const aGfxBuffer);
/** @brief Release a buffer

    This API release a buffer acquired from VGC back to free pool.
    Intended to be called only during stop sequence.

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aSurfType           Indicates the type of surface
    @param[in]   aBuffer             Pointer to buffer to be released to VGC

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_UNINIT          Component is not initialized
    @retval      #BCM_ERR_BUSY            No free Surface available
    @retval      #BCM_ERR_INVAL_PARAMS    aSurfType is not valid
                                        or aBuffer is NULL
                                        or aBuffer was not dequeued earlier

    @post none


    @trace #BRCM_SWREQ_VGC_DISPLAY
    @limitations none
*/
extern int32_t VGC_ReleaseBuffer(VGC_SurfaceType aSurfType,
                                 const VGC_BufferType * const aBuffer);

/** @brief Dequeue a buffer

    This API dequeues a buffer from VGC

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aSurfType           Indicates the type of surface
    @param[in]   aSize               Size of the buffer requested
    @param[out]  aBuffer             Pointer to buffer to be filled by VGC

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_UNINIT          Component is not initialized
    @retval      #BCM_ERR_NOMEM           No free Surface available
    @retval      #BCM_ERR_INVAL_PARAMS    aSurfType is not valid
                                        or aSize is beyond limits
                                        or aBuffer is NULL

    @post none

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern int32_t VGC_DequeueBuffer(VGC_SurfaceType aSurfType,
                                 uint32_t aSize,
                                 VGC_BufferType *const aBuffer);

/** @brief Enable a surface

    This API enables a particular surface to be composed

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aSurfType           Indicates the type of surface

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_UNINIT          Component is not initialized
    @retval      #BCM_ERR_INVAL_PARAMS    aSurfType is invalid

    @post none

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern int32_t VGC_EnableSurface(VGC_SurfaceType aSurfType);

/** @brief Disable a surface

    This API disables a particular surface from being composed

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aSurfType           Indicates the type of surface

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_UNINIT          Component is not initialized
    @retval      #BCM_ERR_INVAL_PARAMS    aSurfType is invalid

    @post none

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern int32_t VGC_DisableSurface(VGC_SurfaceType aSurfType);

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

    @trace #BRCM_SWREQ_VGC_DISPLAY

    @limitations none
*/
extern int32_t VGC_DispGetStats(VGC_DispStatsType *const aStats);

#endif /* VGC_DISPLAY_H */

/** @} */
