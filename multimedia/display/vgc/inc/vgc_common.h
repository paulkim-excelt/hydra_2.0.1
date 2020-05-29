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

    @file vgc_common.h
    @brief VGC interface
    This header file contains the interface functions for VGC component.
    VGC exposes interfaces necessary to setup data pipeline and control composition by posting updated surface buffer attributes. Detailed interface along with necessary state machine and sequence diagrams is described in this header file.
    @version 0.53 Derived from AUTOSW_VGC_Driver_Interface_Specification_0.53.docx
*/

#ifndef VGC_COMMON_H
#define VGC_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <system.h>


/**
    @name Component API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_SWARCH_VGC_STATE_TYPE           (0x00U)   /**< @brief #VGC_StateType 			*/
#define BRCM_SWARCH_VGC_PIXFMT_TYPE          (0x01U)   /**< @brief #VGC_PixFmtType 			*/
#define BRCM_SWARCH_VGC_BLENDFACT_TYPE       (0x02U)   /**< @brief #VGC_BlendFactType 		*/
#define BRCM_SWARCH_VGC_OUTPUT_TYPE          (0x03U)   /**< @brief #VGC_OutputType 			*/
#define BRCM_SWARCH_VGC_PIXEL_TYPE           (0x04U)   /**< @brief #VGC_PixelType 			*/
#define BRCM_SWARCH_VGC_POINT_TYPE           (0x05U)   /**< @brief #VGC_PointType 			*/
#define BRCM_SWARCH_VGC_RECT_TYPE            (0x06U)   /**< @brief #VGC_RectType 			*/
#define BRCM_SWARCH_VGC_WIN_TYPE             (0x07U)   /**< @brief #VGC_WinType 			*/
#define BRCM_SWARCH_VGC_BLEND_TYPE           (0x08U)   /**< @brief #VGC_BlendType 			*/
#define BRCM_SWARCH_VGC_BUFFER_TYPE          (0x09U)   /**< @brief #VGC_BufferType 			*/
#define BRCM_SWARCH_VGC_SURFATTR_TYPE        (0x0AU)   /**< @brief #VGC_SurfAttrType		*/
#define BRCM_SWARCH_VGC_CMPCFG_TYPE          (0x0BU)   /**< @brief #VGC_CMPCfgType 			*/
#define BRCM_SWARCH_VGC_CSC_TYPE             (0x0CU)   /**< @brief #VGC_CSCType 			*/
#define BRCM_SWARCH_VGC_COLORKEY_TYPE        (0x0DU)   /**< @brief #VGC_ColorKeyType		*/
#define BRCM_SWARCH_VGC_GFDCFG_TYPE          (0x0EU)   /**< @brief #VGC_GFDCfgType 			*/
#define BRCM_SWARCH_VGC_DEINIT_PROC          (0x0FU)   /**< @brief #VGC_Deinit 				*/
#define BRCM_SWARCH_VGC_SETBG_PROC           (0x10U)   /**< @brief #VGC_SetBackgroundColor 	*/
#define BRCM_SWARCH_VGC_CSC_PROC             (0x11U)   /**< @brief #VGC_CSC 				*/
#define BRCM_SWARCH_VGC_COLORKEY_PROC        (0x12U)   /**< @brief #VGC_ColorKey 			*/
#define BRCM_SWARCH_VGC_LOADPALETTE_PROC     (0x13U)   /**< @brief #VGC_LoadPalette 		*/
#define BRCM_SWARCH_VGC_GETSTATUS_PROC       (0x14U)   /**< @brief #VGC_GetStatus 			*/
#define BRCM_SWARCH_VGC_MISC_CFG_TYPE        (0x15U)   /**< @brief #VGC_MiscCfgType     */
/** @} */

/**
    @name VGC_StateType
    @{
    @brief VGC State type

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef uint32_t VGC_StateType;                 /**< @brief VGC State type */
#define VGC_STATE_UNINIT       (0x00000001UL)   /**< @brief Uninitialized state */
#define VGC_STATE_IDLE         (0x00000002UL)   /**< @brief Idle state          */
#define VGC_STATE_COMPOSING    (0x00000003UL)   /**< @brief Composing state     */
/** @} */

/**
    @name VGC_PixFmtType
    @{
    @brief Pixel format types

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef uint32_t VGC_PixFmtType;                       /**< @brief Pixel format types */
#define VGC_PIXFMT_YUV422P     (0x00000001UL)          /**< @brief YUV422 Planar 8-bit per component */
#define VGC_PIXFMT_YUV444P     (0x00000002UL)          /**< @brief YUV444 Planar 8-bit per component */
#define VGC_PIXFMT_YUV422PEXT  (0x00000003UL)          /**< @brief YUV422 Planar 16-bit per component */
#define VGC_PIXFMT_YUV444PEXT  (0x00000004UL)          /**< @brief YUV444 Planar 16-bit per component */
#define VGC_PIXFMT_YUV420P     (0x00000005UL)          /**< @brief YUV420 Planar 8-bitper component */
#define VGC_PIXFMT_YUV420PEXT  (0x00000006UL)          /**< @brief YUV420 Planar 16-bit per component */
#define VGC_PIXFMT_MIN_VID     (VGC_PIXFMT_YUV422P)    /**< @brief Minimum value for Video input */
#define VGC_PIXFMT_MAX_VID     (VGC_PIXFMT_YUV420PEXT) /**< @brief Maximum value for Video input */
#define VGC_PIXFMT_IL          (0x00000007UL)          /**< @brief Interleaved YUV 8-bit per component */
#define VGC_PIXFMT_ILEXT       (0x00000008UL)          /**< @brief Interleaved YUV 16-bit per component */
#define VGC_PIXFMT_YUV420H     (0x00000009UL)          /**< @brief YUV420 Half-planar 8-bit per component */
#define VGC_PIXFMT_YUV420HEXT  (0x0000000AUL)          /**< @brief YUV420 Half-planar 16-bit per component */
#define VGC_PIXFMT_YUYV422     (0x0000000BUL)          /**< @brief YUYV422 Interleaved 8-bit per component */
#define VGC_PIXFMT_YVYU422     (0x0000000CUL)          /**< @brief YVYU422 Interleaved 8-bit per component */
#define VGC_PIXFMT_UYVY422     (0x0000000DUL)          /**< @brief UYVY422 Interleaved 8-bit per component */
#define VGC_PIXFMT_VYUY422     (0x0000000EUL)          /**< @brief VYUY422 Interleaved 8-bit per component */
#define VGC_PIXFMT_MIN_OUT     (VGC_PIXFMT_YUV422P)    /**< @brief Minimum value for Video output */
#define VGC_PIXFMT_MAX_OUT     (VGC_PIXFMT_VYUY422)    /**< @brief Maximum value for Video output */
#define VGC_PIXFMT_AYCRCB8888  (0x00010001UL)          /**< @brief AYCRCB8888 */
#define VGC_PIXFMT_YCRCBA8888  (0x00010002UL)          /**< @brief YCRCBA8888 */
#define VGC_PIXFMT_AP88_YUV    (0x00010003UL)          /**< @brief AP88 with Palette in RGB format */
#define VGC_PIXFMT_P0_YUV      (0x00010004UL)          /**< @brief 0-bit Palette mode with Palette in YUV format*/
#define VGC_PIXFMT_P1_YUV      (0x00010005UL)          /**< @brief 1-bit Palette mode with Palette in YUV format*/
#define VGC_PIXFMT_P2_YUV      (0x00010006UL)          /**< @brief 2-bit Palette mode with Palette in YUV format*/
#define VGC_PIXFMT_P4_YUV      (0x00010007UL)          /**< @brief 4-bit Palette mode with Palette in YUV format*/
#define VGC_PIXFMT_P8_YUV      (0x00010008UL)          /**< @brief 8-bit Palette mode with Palette in YUV format*/
#define VGC_PIXFMT_WRGB1555    (0x00010009UL)          /**< @brief WRGB1555 */
#define VGC_PIXFMT_RGBW5551    (0x0001000AUL)          /**< @brief RGBW5551 */
#define VGC_PIXFMT_ARGB4444    (0x0001000BUL)          /**< @brief ARGB4444 */
#define VGC_PIXFMT_RGBA4444    (0x0001000CUL)          /**< @brief RGBA4444 */
#define VGC_PIXFMT_ARGB8888    (0x0001000DUL)          /**< @brief ARGB8888 */
#define VGC_PIXFMT_RGBA8888    (0x0001000EUL)          /**< @brief RGBA8888 */
#define VGC_PIXFMT_AP88_RGB    (0x0001000FUL)          /**< @brief AP88 with Palette in YUV format*/
#define VGC_PIXFMT_P0_RGB      (0x00010010UL)          /**< @brief 0-bit Palette mode with Palette in RGB format*/
#define VGC_PIXFMT_P1_RGB      (0x00010011UL)          /**< @brief 1-bit Palette mode with Palette in RGB format*/
#define VGC_PIXFMT_P2_RGB      (0x00010012UL)          /**< @brief 2-bit Palette mode with Palette in RGB format*/
#define VGC_PIXFMT_P4_RGB      (0x00010013UL)          /**< @brief 4-bit Palette mode with Palette in RGB format*/
#define VGC_PIXFMT_P8_RGB      (0x00010014UL)          /**< @brief 8-bit Palette mode with Palette in RGB format*/
#define VGC_PIXFMT_MIN_GFX     (VGC_PIXFMT_AYCRCB8888) /**< @brief Minimum value for Graphics */
#define VGC_PIXFMT_MAX_GFX     (VGC_PIXFMT_P8_RGB)     /**< @brief Maximum value for Graphics */
/** @} */

/**
    @name VGC_BlendFactType
    @{
    @brief Blend Factor types

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef uint32_t VGC_BlendFactType;                                     /**< @brief Blend Factor types */
#define VGC_BLENDFACT_ZERO             (0x00000000UL)                   /**< @brief Transparent */
#define VGC_BLENDFACT_ONE              (0x00000001UL)                   /**< @brief Opaque */
#define VGC_BLENDFACT_FRONT            (0x00000002UL)                   /**< @brief Front alpha per pixel */
#define VGC_BLENDFACT_ONE_MINUS_FRONT  (0x00000003UL)                   /**< @brief (1 - Front alpha per pixel) */
#define VGC_BLENDFACT_CONST            (0x00000004UL)                   /**< @brief Constant alpha */
#define VGC_BLENDFACT_ONE_MINUS_CONST  (0x00000005UL)                   /**< @brief (1 - Constant alpha) */
#define VGC_BLENDFACT_MAX              (VGC_BLENDFACT_ONE_MINUS_CONST)  /**< @brief Maximum value */
/** @} */

/**
    @name VGC_OutputType
    @{
    @brief Output types

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef uint32_t VGC_OutputType;                       /**< @brief Output types */
#define VGC_OUTPUT_MWB         (0x00000000UL)          /**< @brief Memory Write Back */
#define VGC_OUTPUT_LVDS        (0x00000001UL)          /**< @brief LVDS */
#define VGC_OUTPUT_BT656       (0x00000002UL)          /**< @brief BT656 */
#define VGC_OUTPUT_DSI         (0x00000004UL)          /**< @brief DSI */
/** @} */

/**
    @brief  Structure to define C2 and Cb Swap configuration

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_MiscCfgType {
    VGC_OutputType output;               /**< @brief Output path as #VGC_OutputType */
    uint8_t yuvToRgbBypass;              /**< @brief Bypass YUV to RGB conversion */
    uint8_t lvdsCrCbSwap;                    /**< @brief swap Cr and Cb to the YUV2RGB convertor for LVDS */
    uint8_t iifCrCbSwap ;                     /**< @brief  swap IIF output Cr and Cb*/
} VGC_MiscCfgType;

/**
    @brief  Structure to define Color information of a pixel

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_PixelType {
    uint8_t alpha;                  /**< @brief Alpha component of a pixel  */
    uint8_t y_r;                    /**< @brief Y or R component based on format */
    uint8_t cb_g;                   /**< @brief Cb or G component based on format */
    uint8_t cr_b;                   /**< @brief Cr or B component based on format */
    VGC_PixFmtType fmt;             /**< @brief format as #VGC_PixFmtType */
} VGC_PixelType;

/**
    @brief  Structure to define co-ordinates of a point in 2-D plane

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_PointType {
    uint32_t x;                     /**< @brief Horizontal position from origin */
    uint32_t y;                     /**< @brief Vertical position from origin */
} VGC_PointType;

/**
    @brief  Structure to define a rectangle

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_RectType {
    uint32_t width;                 /**< @brief Width of the rectangle */
    uint32_t height;                /**< @brief Height of the rectangle */
} VGC_RectType;

/**
    @brief  Structure to define a window

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_WinType {
    VGC_RectType buffRect;          /**< @brief Resolution of input buffer in pixels as #VGC_RectType */
    VGC_PointType surfaceOffset;    /**< @brief Offset co-ordinates to overlay input buffer at an offset on the surface as #VGC_PointType */
    VGC_RectType dispRect;          /**< @brief Resolution of cropped display in pixels as #VGC_RectType */
    VGC_PointType canvasOffset;     /**< @brief Offset co-ordinates to overlay input buffer at an offset on the canvas as #VGC_PointType */
} VGC_WinType;

/**
    @brief  Structure to define blend attributes

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_BlendType {
    VGC_BlendFactType back;         /**< @brief Blend factor of back surface as #VGC_BlendFactType */
    VGC_BlendFactType front;        /**< @brief Blend factor of front surface as #VGC_BlendFactType */
    uint32_t alpha;                 /**< @brief Constant Alpha */
} VGC_BlendType;

/**
    @brief  Structure to define buffer attributes

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_BufferType {
    uint8_t *addr;                  /**< @brief Address of the buffer */
    uint32_t idx;                   /**< @brief Index of the buffer */
} VGC_BufferType;

/**
    @brief  Structure to define attributes of a composed Surface buffer

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_SurfAttrType {
    VGC_WinType window;             /**< @brief Window information as #VGC_WinType */
    VGC_PixFmtType fmt;             /**< @brief Color format as #VGC_PixFmtType */
    VGC_BlendType blend;            /**< @brief Blend factor as #VGC_BlendType */
} VGC_SurfAttrType;

/**
    @brief  Structure to hold parameters needed to initialize Compositor

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_CMPCfgType {
    VGC_PixelType bg;               /**< @brief Background pixel color as #VGC_PixelType */
    VGC_RectType panel;             /**< @brief Display panel resolution in pixels as #VGC_RectType */
} VGC_CMPCfgType;

/**
    @brief  Structure to hold Color Space Conversion Matrix Coefficients

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_CSCType {
    uint16_t c00;                   /**< @brief CSC Matrix Coefficients C00 */
    uint16_t c01;                   /**< @brief CSC Matrix Coefficients C01 */
    uint16_t c02;                   /**< @brief CSC Matrix Coefficients C02 */
    uint16_t c03;                   /**< @brief CSC Matrix Coefficients C03 */
    uint16_t c04;                   /**< @brief CSC Matrix Coefficients C04 */
    uint16_t c10;                   /**< @brief CSC Matrix Coefficients C10 */
    uint16_t c11;                   /**< @brief CSC Matrix Coefficients C11 */
    uint16_t c12;                   /**< @brief CSC Matrix Coefficients C12 */
    uint16_t c13;                   /**< @brief CSC Matrix Coefficients C13 */
    uint16_t c14;                   /**< @brief CSC Matrix Coefficients C14 */
    uint16_t c20;                   /**< @brief CSC Matrix Coefficients C20 */
    uint16_t c21;                   /**< @brief CSC Matrix Coefficients C21 */
    uint16_t c22;                   /**< @brief CSC Matrix Coefficients C22 */
    uint16_t c23;                   /**< @brief CSC Matrix Coefficients C23 */
    uint16_t c24;                   /**< @brief CSC Matrix Coefficients C24 */
} VGC_CSCType;

/**
    @brief  Structure to hold Color Keying information

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_ColorKeyType {
    VGC_PixelType min;              /**< @brief Color Key Min value as #VGC_PixelType*/
    VGC_PixelType max;              /**< @brief Color Key Max value as #VGC_PixelType*/
    VGC_PixelType mask;             /**< @brief Color Key Mask as #VGC_PixelType*/
    uint8_t defaultAlpha;           /**< @brief Default alpha value */
    uint8_t keyAlpha;               /**< @brief Keyed alpha value */
} VGC_ColorKeyType;

/**
    @brief  Structure to hold parameters needed to initialize Graphics Feeder

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_GFDCfgType {
    VGC_CSCType csc;                /**< @brief CSC Matrix Coefficients as #VGC_CSCType */
} VGC_GFDCfgType;

/** @brief Deinitalize VGC

    This API deinitializes VGC and all of its subcomponents.
    This may need to be retried in case component/subcomponent is busy

    @behavior Sync, Non-reentrant

    @pre none

    @param       none

    <br>Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              success
    @retval      #BCM_ERR_BUSY            In the process of shutting down the
                                        component and/or sub-components
    @retval      #BCM_ERR_INVAL_STATE     Component is not in idle state

    @post none

    @trace #BRCM_SWREQ_VGC_COMMON

    @limitations none
*/
extern int32_t VGC_Deinit(void);

/** @brief Set background color

    This API sets the background color of the canvas

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aPixel              Pointer to pixel color information

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK              Background color set successfully
    @retval      #BCM_ERR_UNINIT          Component is not initialised
    @retval      #BCM_ERR_INVAL_PARAMS    aPixel is NULL
    @retval      #BCM_ERR_INVAL_PARAMS    aPixel->fmt != VGC_PIXFMT_YUV444P

    @post none

    @trace #BRCM_SWREQ_VGC_COMMON

    @limitations none
*/
extern int32_t VGC_SetBackgroundColor(const VGC_PixelType * const aPixel);

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

    @trace #BRCM_SWREQ_VGC_COMMON

    @limitations none
*/

extern int32_t VGC_CSC(uint32_t aEnable, const VGC_CSCType *const aCsc);

/** @brief Enable/Disable Color Keying

    This API is used to enable/disable Color Keying.

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aEnable            Boolean to indicate whether to enable/disable
    @param[in]   aColorKey          Pointer to memory where Colour Keying
                                   information is available

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK             Color Key enabled/disabled as requested
    @retval      #BCM_ERR_UNINIT         Component is not initialised
    @retval      #BCM_ERR_INVAL_PARAMS   aColorKey is NULL

    @post none

    @trace #BRCM_SWREQ_VGC_COMMON

    @limitations none
*/
extern int32_t VGC_ColorKey(uint32_t aEnable,
                            const VGC_ColorKeyType *const aColorKey);

/** @brief Request loading a Palette

    This API requests VGC to load a Palette from a particular memory
    location.

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]   aPalette           Pointer to memory where Palette is available
                                   Should be aligned to 256bytes
    @param[in]   aSize              Palette size in number of 32-bits palette entries
                                   Maximum size is 256

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK             Palette load request successful
    @retval      #BCM_ERR_UNINIT         Component is not initialised
    @retval      #BCM_ERR_INVAL_PARAMS   aPalette is NULL
    @retval      #BCM_ERR_INVAL_PARAMS   aSize is not in the range: 1-256

    @post none

    @trace #BRCM_SWREQ_VGC_COMMON

    @limitations none
*/
extern int32_t VGC_LoadPalette(const uint32_t *const aPalette,
                               const uint32_t aSize);


/** @brief Get the status of VGC

    This API gets the status of VGC

    @behavior Sync, Non-reentrant

    @pre none

    @param[out]   aState            Pointer to State variable to be filled by VGC
    @param[out]   aLineNumber       Pointer to LineNumber variable to be filled by VGC

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK         Status and LineNumber obtained successfully

    @post none

    @trace #BRCM_SWREQ_VGC_COMMON

    @limitations none
*/
extern int32_t VGC_GetStatus(VGC_StateType *const aState,
                             uint32_t *const aLineNumber);

#endif /* VGC_COMMON_H */

/** @} */
