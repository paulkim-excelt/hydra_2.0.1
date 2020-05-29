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
    @defgroup grp_isys_ifc Interface
    @ingroup grp_isys

    @addtogroup grp_isys_ifc
    @{
    @section sec_isys_ifc_overview Overview
    Below figure depicts a block diagram representation of ISYS sub-system
    @image html isys_overview.jpg "ISYS library hierarchy"
    ISYS stands for “Imaging System” consisting of UNICAM, ISP and coordinating
    software (ISYS Framework).

    UNICAM is the Camera receiver hardware which receives Bayer image data from
    image sensor. UNICAM provides data to the ISP, which provides processed YUV
    output. ISYS framework coordinates the above mentioned sub-components to
    achieve YUV streaming functionality.

    ISYS library provides APIs for the following major functionalities along
    with other supporting APIs:
    -# Initialize
    -# Configure internal components
    -# Start streaming
    -# Stop streaming
    -# De-initialize
    -# Get group of lines (slices).

    Client of ISYS library should start by initializing the ISYS stack. Once
    ISYS is initialized, client should configure ISYS with the specific
    configuration generated using offline configuration generating utility (@ref grp_offline_utility).

    Following are the various configurations required by ISYS library:
    -# Mode image – General configuration.
    -# ISP Image – Configurations for ISP.
    -# UNICAM Image – Configurations for Camera sensor interface.

    ISYS offline utility supports generating configuration for Camera Sensor
    and Tuners as well. Layout of Camera Sensor image has been described in
    #CSN_ImageType.

    Once the configuration is complete, ISYS is ready to start streaming.

    Tearing down of streaming should be initiated by stopping of ISYS streaming
    followed by de-initialization.

    @subsection subsec_isys_isp_dual_pass ISP Dual Pass
    ISYS library supports operating ISP in dual-pass mode along with single
    pass mode. ISP dual Pass feature is aimed at having two differently tuned
    images for Computer Vision as well as Human Vision processing.

    @par Offiline Configurations
    Camera configuration would need HV as well as CV ISP configuration to be
    provided in a dual-Pass configuration.

    Camera offline utility [2] describes syntax for the configuration files.

    The offline utilities shall process these input files and create a combined
    ISP configuration containing common configuration, HV specific
    configuration and CV specific configuration.

    @par Firmware
    Library is capable of performing dual-pass operation in following schemes.

    @b Frame Mode:
    In frame mode, ISP process images once complete input frame is available.
    Here is the sequence of operation for Frame mode Dual-Pass,
    -# ISP driver waits for “Frame End” notification from UNICAM.
    -# Configure ISP for HV and kick-start ISP.
    -# Waits for “EOT” for HV.
    -# Configure ISP for CV and kick-start ISP.
    -# ISP finishes CV processing.
    -# Step 1

    Below figure represents timing diagram for Dual pass in frame mode. Time
    has been mentioned considering 720p @ 30fps streaming.
    @image html isys_time_frame_mode.jpg "Timing Diagram for Frame mode"

    @b Low Latency Mode:
    In low latency mode, HV will be processed in video mode during active frame
    time and CV gets processed in vertical blanking interval. This mode
    requires sufficient vertical blanking to process CV pass for the frame.
    Here is the sequence of operation for Dual-Pass:
    -# Configure ISP for HV.
    -# ISP process HV in synchronization with incoming frame (with latency of
       2-3 lines).
    -# ISP finishes HV Processing.
    -# Configure ISP for CV.
    -# ISP process CV with the image stored in memory.
    -# ISP finishes CV processing.
    -# Step 1.

    Below figure represents timing diagram for Dual pass. Time has been
    mentioned considering 720p @ 30fps streaming.
    @image html isys_time_low_latency_mode.jpg "Timing Diagram for Low latency mode"

    ISP dual-pass requires configuration as well firmware changes which work
    together.

    @section sec_isys_fsm State Machine
    Below figure depicts the state machine for ISYS library. It also depicts
    the APIs which triggers the state transition and which APIs are allowed to
    be called in different states of ISYS.
    @image html isys_state_machine.jpg
    ISYS library has the following states:
    - @b PowerDown: All sub-components are in un-initialized state.
    - @b Initialized: Individual components are initialized. State movement to
      @b Ready happens after configuring with all configurations mentioned in
      #ISYS_ConfigsType.
    - @b Ready: Individual components are configured. ISYS can be reconfigured
      in @b Ready state as well.
    - @b Streaming: Individual components are enabled and ISYS is streaming.
    - @b Error: One or more individual components went to error state.

    #ISYS_GetInfo can be called after the components moves to @b Ready State
    (i.e. this API can be called either @b Ready or in @b Streaming state).

    @section sec_isys_seq Sequence Diagrams
    This section depicts the sequence diagrams for benefit of application
    developers. Below sequence diagram depicts a typical flow of application to
    utilize ISYS.
    @image html isys_seq_diagram_typical_flow.jpg "Sequence diagram for ISYS Library Streaming Start-Stop"

    @section sec_isys_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   TBD                                             |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   One Pixel per Clock                             |

    @includedoc isys.dep

    @file isys.h
    @brief Data structures and API's of the ISYS subsystem
    This file exposes ISYS subsystem interfaces
    @version 1.82 Fixed TBDs
    @version 1.81 Imported from AUTOSW_ISYS_Interface_Spec_1.81.docx
*/

#ifndef ISYS_H
#define ISYS_H

#include <stdint.h>
#include <compiler.h>
#include <bcm_err.h>

/**
    @name ISYS API IDs
    @{
    @brief API IDs for ISYS
*/
#define BRCM_SWARCH_ISYS_MAX_IMAGE_PLANES_MACRO     (0x00U)    /**< @brief #ISYS_MAX_IMAGE_PLANES    */
#define BRCM_SWARCH_ISYS_MAX_NAME_CHAR_MACRO        (0x01U)    /**< @brief #ISYS_MAX_NAME_CHAR       */
#define BRCM_SWARCH_ISYS_UNICAM_STRIDE_ALIGN_MACRO  (0x02U)    /**< @brief #ISYS_UNICAM_STRIDE_ALIGN */
#define BRCM_SWARCH_ISYS_ISP_STRIDE1_ALIGN_MACRO    (0x03U)    /**< @brief #ISYS_ISP_STRIDE1_ALIGN   */
#define BRCM_SWARCH_ISYS_ISP_STRIDE2_ALIGN_MACRO    (0x04U)    /**< @brief #ISYS_ISP_STRIDE2_ALIGN   */
#define BRCM_SWARCH_ISYS_CONFIG_MACRO               (0x05U)    /**< @brief #ISYS_CONFIG_MODE         */
#define BRCM_SWARCH_ISYS_EVENT_TYPE                 (0x06U)    /**< @brief #ISYS_EventType           */
#define BRCM_SWARCH_ISYS_OUT_ENABLE_TYPE            (0x07U)    /**< @brief #ISYS_OutEnableType       */
#define BRCM_SWARCH_ISYS_OUT_IDX_TYPE               (0x08U)    /**< @brief #ISYS_OutIDXType          */
#define BRCM_SWARCH_ISYS_SENSOR_EXP_IDX_TYPE        (0x09U)    /**< @brief #ISYS_SensorExpIDXType    */
#define BRCM_SWARCH_ISYS_PASS_ENABLE_TYPE           (0x0AU)    /**< @brief #ISYS_PassEnableType      */
#define BRCM_SWARCH_ISYS_DUAL_PASS_MODE_TYPE        (0x0BU)    /**< @brief #ISYS_DualPassModeType    */
#define BRCM_SWARCH_ISYS_PASS_IDX_TYPE              (0x0CU)    /**< @brief #ISYS_PassIDXType         */
#define BRCM_SWARCH_ISYS_PLANE_TYPE                 (0x0DU)    /**< @brief #ISYS_PlaneType           */
#define BRCM_SWARCH_ISYS_FORMAT_TYPE                (0x0EU)    /**< @brief #ISYS_FormatType          */
#define BRCM_SWARCH_ISYS_STATE_TYPE                 (0x0FU)    /**< @brief #ISYS_StateType           */
#define BRCM_SWARCH_ISYS_MEMPOOL_IDX_TYPE           (0x10U)    /**< @brief #ISYS_MemPoolIDXType      */
#define BRCM_SWARCH_ISYS_COMMON_IMGHDR_TYPE         (0x11U)    /**< @brief #ISYS_CommonImgHdrType    */
#define BRCM_SWARCH_ISYS_IMAGE_TYPE                 (0x12U)    /**< @brief #ISYS_ImageType           */
#define BRCM_SWARCH_ISYS_CONFIGS_TYPE               (0x13U)    /**< @brief #ISYS_ConfigsType         */
#define BRCM_SWARCH_ISYS_IMAGE_PLANE_TYPE           (0x14U)    /**< @brief #ISYS_ImagePlaneType      */
#define BRCM_SWARCH_ISYS_BUFFER_TYPE                (0x15U)    /**< @brief #ISYS_BufferType          */
#define BRCM_SWARCH_ISYS_OUT_PROPS_TYPE             (0x16U)    /**< @brief #ISYS_OutPropsType        */
#define BRCM_SWARCH_ISYS_PASS_PROPS_TYPE            (0x17U)    /**< @brief #ISYS_PassPropsType       */
#define BRCM_SWARCH_ISYS_MODE_TYPE                  (0x18U)    /**< @brief #ISYS_ModeType            */
#define BRCM_SWARCH_ISYS_INFO_TYPE                  (0x19U)    /**< @brief #ISYS_InfoType            */
#define BRCM_SWARCH_ISYS_GET_STATE_PROC             (0x1AU)    /**< @brief #ISYS_GetState            */
#define BRCM_SWARCH_ISYS_INIT_PROC                  (0x1BU)    /**< @brief #ISYS_Init                */
#define BRCM_SWARCH_ISYS_DEINIT_PROC                (0x1CU)    /**< @brief #ISYS_Deinit              */
#define BRCM_SWARCH_ISYS_START_PROC                 (0x1DU)    /**< @brief #ISYS_Start               */
#define BRCM_SWARCH_ISYS_STOP_PROC                  (0x1EU)    /**< @brief #ISYS_Stop                */
#define BRCM_SWARCH_ISYS_CONFIGURE_PROC             (0x1FU)    /**< @brief #ISYS_Configure           */
#define BRCM_SWARCH_ISYS_PROCESS_EVENT_PROC         (0x20U)    /**< @brief #ISYS_ProcessEvent        */
#define BRCM_SWARCH_ISYS_FILL_BUFFER_PROC           (0x21U)    /**< @brief #ISYS_FillBuffer          */
#define BRCM_SWARCH_ISYS_GET_FILLED_BUFFER_PROC     (0x22U)    /**< @brief #ISYS_GetFilledBuffer     */
#define BRCM_SWARCH_ISYS_GET_INFO_PROC              (0x23U)    /**< @brief #ISYS_GetInfo             */
/** @} */

/**
   @brief ISYS maximum number of image planes

   @trace #BRCM_SWREQ_ISYS_INIT
*/
#define ISYS_MAX_IMAGE_PLANES (3UL)

/**
    @brief Maximum number of characters in sensor name

    @trace #BRCM_SWREQ_ISYS_INIT
*/
#define ISYS_MAX_NAME_CHAR    (64UL)

/**
    @brief ISP stride requirement UNICAM output

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
#define ISYS_UNICAM_STRIDE_ALIGN  (32UL)

/**
    @brief ISP stride requirement for first channel

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
#define ISYS_ISP_STRIDE1_ALIGN (32UL)

/**
    @brief ISP stride requirement for second channel

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
#define ISYS_ISP_STRIDE2_ALIGN (16UL)

/**
    @name ISYS Configuration type
    @{
    @trace #BRCM_SWREQ_ISYS_COMMON_IMAGE_HEADER
*/
#define ISYS_CONFIG_MODE     (1UL) /**< @brief Mode Configuration   */
#define ISYS_CONFIG_ISP      (2UL) /**< @brief ISP Configuration    */
#define ISYS_CONFIG_UNICAM   (3UL) /**< @brief UNICAM Configuration */
/** @} */

/**
    @name ISYS_EventType
    @{
    @brief Macro for ISYS Event types (Masks)

    @trace #BRCM_SWREQ_ISYS
*/
typedef uint32_t ISYS_EventType;            /**< @brief typedef for ISYS Events  */
#define ISYS_EVENT_ERROR       (0x10000UL)  /**< @brief Notifies Error from ISYS */
/** @} */

/**
    @name ISYS_OutEnableType
    @{
    @brief ISYS output enable mask type

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_OutEnableType;     /**< @brief typedef for output enable mask    */
#define ISYS_OUT_ENABLE_LORES    (0x1UL) /**< @brief Enable low-resolution ISP output  */
#define ISYS_OUT_ENABLE_HIRES    (0x2UL) /**< @brief Enable high-resolution ISP output */
/** @} */

/** @name ISYS_OutIDXType
    @{
    @brief ISYS output index.

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_OutIDXType; /**< @brief typedef for ISYS output index    */
#define ISYS_OUT_IDX_LORES (0UL)  /**< @brief Index for low-resolution output  */
#define ISYS_OUT_IDX_HIRES (1UL)  /**< @brief Index for high-resolution output */
#define ISYS_OUT_IDX_MAX   (2UL)  /**< @brief Maximum number of output streams */
/** @} */

/** @name ISYS_SensorExpIDXType
    @{
    @brief Camera sensor exposure index

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_SensorExpIDXType;  /**< @brief typedef for Camera sensor exposure index */
#define ISYS_SENSOR_EXP_IDX_LONG  (0UL)  /**< @brief Long exposure             */
#define ISYS_SENSOR_EXP_IDX_SHORT (1UL)  /**< @brief Short exposre             */
#define ISYS_SENSOR_EXP_IDX_MAX   (2UL)  /**< @brief Maximum number of exposre */
/** @} */

/** @name ISYS_PassEnableType
    @{
    @brief ISYS pass enable mask type.

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_PassEnableType; /**< @brief typedef for ISYS pass enable mask */
#define ISYS_PASS_ENABLE_HV   (0x1UL)     /**< @brief Enable ISP HV pass            */
#define ISYS_PASS_ENABLE_CV   (0x2UL)     /**< @brief Enable ISP CV pass            */
/** @} */

/** @name ISYS_DualPassModeType
    @{
    @brief ISYS dual pass mode type.

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_DualPassModeType; /**< @brief typedef for dual pass mode */
#define ISYS_DUAL_PASS_MODE_NONE  (0UL) /**< @brief Dual-Pass mode is disabled */
#define ISYS_DUAL_PASS_MODE_FRAME (1UL) /**< @brief Frame based Dual-Pass mode */
#define ISYS_DUAL_PASS_MODE_LL    (2UL) /**< @brief Low Latency Dual-Pass mode */
/** @} */

/** @name ISYS_PassIDXType
    @{
    @brief ISYS pass index

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_PassIDXType;  /**< @brief Index for pass type */
#define ISYS_PASS_IDX_HV    (0UL)   /**< @brief Index for HV pass */
#define ISYS_PASS_IDX_CV    (1UL)   /**< @brief Index for CV pass */
#define ISYS_PASS_IDX_MAX (2UL)     /**< @brief Maximum number of ISP passes */
/** @} */

/** @name ISYS_PlaneType
    @{
    @brief ISYS Image plane type

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_PlaneType; /**< @brief ISYS Image plane type */
#define ISYS_PLANE_RY  (0UL)     /**< @brief Y Image plane */
#define ISYS_PLANE_GU  (1UL)     /**< @brief G/U Image plane */
#define ISYS_PLANE_BV  (2UL)     /**< @brief B/V Image plane */
/** @} */

/** @name ISYS_FormatType
    @{
    @brief ISYS Image format

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_FormatType;               /**< @brief ISYS image format type */
#define ISYS_FORMAT_INTERLEAVED          (0UL)  /**< @brief Interleaved format */
#define ISYS_FORMAT_RGB565               (1UL)  /**< @brief RGB565 */
#define ISYS_FORMAT_RGBA32_0             (2UL)  /**< @brief RGBA32 with "A" filled with 0 */
#define ISYS_FORMAT_RGBA32_255           (3UL)  /**< @brief RGBA32 with "A" filled with 0xff */
#define ISYS_FORMAT_444_PLANAR           (4UL)  /**< @brief 444 Planar */
#define ISYS_FORMAT_422_PLANAR           (5UL)  /**< @brief 422 planar */
#define ISYS_FORMAT_420_PLANAR           (6UL)  /**< @brief 420 Planar */
#define ISYS_FORMAT_420_SEMI_PLANAR_UV   (7UL)  /**< @brief 420 SP(UV) */
#define ISYS_FORMAT_422_INTERLEAVED_YUYV (8UL)  /**< @brief 422-I-YUYV */
#define ISYS_FORMAT_422_INTERLEAVED_YVYU (9UL)  /**< @brief 422-I-YVYU */
#define ISYS_FORMAT_422_INTERLEAVED_UYVY (10UL) /**< @brief 422-I-UYVY */
#define ISYS_FORMAT_422_INTERLEAVED_VYUY (11UL) /**< @brief 422-I-VYUY */
/** @} */

/**
    @name ISYS State
    @{
    @brief ISYS state type
    Refer @ref sec_isys_fsm for more details

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef uint32_t ISYS_StateType;        /**< @brief typedef for ISYS states */
#define ISYS_STATE_POWEROFF      (0UL)  /**< @brief ISYS State Poweroff */
#define ISYS_STATE_INITIALISED   (1UL)  /**< @brief ISYS State Initialized */
#define ISYS_STATE_READY         (2UL)  /**< @brief ISYS State Ready */
#define ISYS_STATE_STREAMING     (3UL)  /**< @brief ISYS State Streaming */
#define ISYS_STATE_ERROR         (4UL)  /**< @brief ISYS State Error */
/** @} */

/** @name ISYS_MemPoolIDXType
    @{
    @brief ISYS memory pool index

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
typedef uint32_t ISYS_MemPoolIDXType;   /**< @brief typedef for ISYS memory pool */
#define ISYS_MEMPOOL_IDX_YUV      (0UL) /**< @brief Index for YUV memory pool */
#define ISYS_MEMPOOL_IDX_RAW      (1UL) /**< @brief Index for RAW memory */
#define ISYS_MEMPOOL_IDX_MAX      (2UL) /**< @brief Maximum number of pool memory */
/** @} */

/**
    @brief ISYS common image header

    @trace #BRCM_SWREQ_ISYS_COMMON_IMAGE_HEADER
*/
typedef struct _ISYS_CommonImgHdrType {
    uint32_t cfgType;   /**< @brief Image UID */
    uint32_t cfgMagic;  /**< @brief Magic for component use */
} ISYS_CommonImgHdrType;

/**
    @brief ISYS images union

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
typedef union _ISYS_ImageType {
    const uint8_t *pHdr;                                    /**< @brief Pointer to header for component usage */
    const ISYS_CommonImgHdrType *pCommonHdr;                /**< @brief Pointer to Common image header */
    const struct _ISYS_ModeImageType *pModeImg;                 /**< @brief Pointer to Mode image */
    const struct _ISP_ImageType *pISP_Img;                  /**< @brief Pointer to ISP image */
    const struct _ISPIMGV2_ConfigImageType *pISPIMGV2_Img;  /**< @brief Pointer to ISPV2 image */
    const struct _Tuner_Image *pTunerImg;                   /**< @brief Pointer to Tuner image */
    const struct _UNICAM_ImageType *pUNICAMImg;             /**< @brief Pointer to UNICAM image */
} ISYS_ImageType;

/**
    @brief ISYS Configuration

    This shall be filled by the ISYS client and invoke #ISYS_Configure API to
    get an internal component configured.

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
typedef struct _ISYS_ConfigsType {
    ISYS_ImageType Cfg;                     /**< @brief Configurations union */
    uint8_t *isysMemPool[ISYS_MEMPOOL_IDX_MAX]; /**< @brief ISYS memory pool
                                                 ISYS memory pool for:
                                                 -# #ISYS_MEMPOOL_IDX_YUV: Minimum 3 Macro Block rows of memory for ISP
                                                    output in single-pass mode. Memory to hold 2 YUV frames
                                                    would be required for “Frame mode” dual-pass.
                                                 -# #ISYS_MEMPOOL_IDX_RAW: Frame memory for ISP input in case of “low
                                                    latency” dual-ISP, and 2 frame memory for “frame mode” dual ISP.
                                            */
    uint32_t isysMemsize[ISYS_MEMPOOL_IDX_MAX]; /**< @brief Memory pool sizes */
    ISYS_DualPassModeType dpMode;           /**< @brief Dual-Pass mode selection */
} ISYS_ConfigsType;

/**
    @brief Image plane

    Structure to hold Image plane details

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
typedef struct _ISYS_ImagePlaneType {
    uint8_t  *addr;             /**< @brief Start address of image plane */
    uint32_t size;              /**< @brief Size of Image plane memory */
    uint32_t stride;            /**< @brief Stride for the image plane */
    uint32_t isEnabled;         /**< @brief Enable flag for the plane */
    uint32_t height;            /**< @brief Height for the image plane */
    uint32_t widthInBytes;      /**< @brief Width in bytes for the image plane */
} ISYS_ImagePlaneType;

/**
    @brief ISYS buffer

    ISYS buffer structure

    @trace #BRCM_SWREQ_ISYS_CONFIGURATION
*/
typedef struct _ISYS_BufferType {
    uint64_t timestamp;                               /**< @brief Frame Timestamp */
    ISYS_ImagePlaneType plane[ISYS_MAX_IMAGE_PLANES]; /**< @brief Image plane
                                                         details */
    uint32_t sliceNum;                                /**< @brief Slice number    */
    uint32_t frameNum;                                /**< @brief Frame number    */
} ISYS_BufferType;

/**
    @brief ISYS output stream properties

    Structure to hold output stream properties

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef struct _ISYS_OutPropsType {
    uint32_t width;                         /**< @brief Width */
    uint32_t height;                        /**< @brief Height */
    uint32_t stride[ISYS_MAX_IMAGE_PLANES]; /**< @brief Stride for each plane */
    ISYS_FormatType format;                 /**< @brief Format */
    uint32_t outWidth;                      /**< @brief Output (per channel)
                                                 width in bytes */
} ISYS_OutPropsType;

/**
    @brief Pass properties for ISP

    Structure to hold ISP pass properties

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef struct _ISYS_PassPropsType {
    ISYS_OutPropsType outputProps[ISYS_OUT_IDX_MAX]; /**< @brief Output properties */
    ISYS_OutEnableType outputEnable;                 /**< @brief Output enable     */
} ISYS_PassPropsType;

/**
    @brief ISYS Mode

    ISYS Mode structure containing information about active configuration

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef struct _ISYS_ModeType {
    uint8_t hdr;                                            /**< @brief Is HDR Enabled?   */
    uint8_t fps;                                            /**< @brief FPS of the stream */
    uint8_t metaLines[ISYS_SENSOR_EXP_IDX_MAX];             /**< @brief Metadata lines for UNICAM */
    uint16_t shortLinesOffset;                              /**< @brief Line offset between long and short exposures */
    uint16_t inputWidth;                                    /**< @brief Width for RAW data from sensor  */
    uint16_t inputHeight;                                   /**< @brief Height for RAW data from sensor */
    uint8_t inputFormat;                                    /**< @brief Stride for RAW data */
    uint8_t unicamMode;                                     /**< @brief Peripheral mode for UNICAM */
    uint32_t ispVersion;                                    /**< @brief ISP Version */
    char sensorName[ISYS_MAX_NAME_CHAR];                    /**< @brief Name of the sensor */
    ISYS_PassPropsType passProps[ISYS_PASS_IDX_MAX];      /**< @brief Configurations for Passes */
    ISYS_PassEnableType passEnable;                         /**< @brief Enable mask for Passes */
} ISYS_ModeType;

/**
    @brief ISYS Information structure

    ISYS Information structure containing information about active configuration
    and library statistics.

    @trace #BRCM_SWREQ_ISYS_INIT
*/
typedef struct _ISYS_InfoType {
    ISYS_ModeType mode;         /**< @brief ISYS Mode structure             */
    uint64_t startTimestamp;    /**< @brief Time-stamp of first frame       */
    uint64_t curTimestamp;      /**< @brief Time-stamp of latest frame      */
    uint32_t curFrameNum;       /**< @brief Frame number for latest frame   */
    uint8_t *scratchMem;        /**< @brief ISYS scratch memory address     */
    uint32_t scratchMemSize;    /**< @brief ISYS scratch memory size        */
    uint8_t *tunerMem;          /**< @brief Tuner meta/stats memory address */
    uint32_t tunerMemSize;      /**< @brief Tuner meta/stats memory size    */
} ISYS_InfoType;

/** @brief Retrieve the present state of the ISYS

    This API lets the client retrieve the present state of the ISYS sub-system.

    @behavior Sync, Re-entrant

    @pre None

    @param      void

    @retval     #ISYS_STATE_POWEROFF        ISYS sub-system is in Power-OFF
                                            state
    @retval     #ISYS_STATE_INITIALISED     ISYS sub-system has been initialized
    @retval     #ISYS_STATE_READY           ISYS sub-system is in READY state
    @retval     #ISYS_STATE_STREAMING       ISYS sub-system is streaming the
                                            data (Sensor/TG)
    @retval     #ISYS_STATE_ERROR           ISYS sub-system has gone to an error
                                            sate

    @post       None

    @trace  #BRCM_SWREQ_ISYS_INIT

    @limitations None
*/
ISYS_StateType ISYS_GetState(void);

/** @brief Initialize ISYS

    This API internally initializes all ISYS sub-components.

    @behavior Sync, Non-reentrant

    @pre This API shall only be called in the #ISYS_STATE_POWEROFF state

    @param      void

    @retval     #BCM_ERR_OK             Initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    Any of the sub-components is in wrong
                                        state
    @retval     #BCM_ERR_VERSION        Underlying hardware mismatches with
                                        driver software

    @post Once successfully initialized, the ISYS sub-system shall be in
          #ISYS_STATE_INITIALISED state

    @trace  #BRCM_SWREQ_ISYS_INIT

    @limitations None
*/
int32_t ISYS_Init(void);

/** @brief De-initialize ISYS

    This API internally de-initializes all ISYS sub-components.

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval     #BCM_ERR_OK             De-initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    Any of the sub-components is in wrong
                                        state

    @post Once successfully de-initialized, the ISYS sub-system shall be in
          #ISYS_STATE_POWEROFF state

    @trace  #BRCM_SWREQ_ISYS_INIT

    @limitations None
*/
int32_t ISYS_Deinit(void);

/** @brief Enable ISYS sub-components

    This enables all ISYS sub-components in the appropriate order

    @behavior Sync, Non-reentrant

    @pre ISYS shall be in #ISYS_STATE_READY while calling this API

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Start is successful
    @retval     #BCM_ERR_INVAL_STATE    Any of the sub-components is in wrong
                                        state

    @post Once successfully started, ISYS shall be in #ISYS_STATE_POWEROFF state

    @trace  #BRCM_SWREQ_ISYS

    @limitations None
*/
int32_t ISYS_Start(void);

/** @brief Disable ISYS sub-components

    This API disables all sub-components in the appropriate order

    @behavior Sync, Non-reentrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param      void

    @retval     #BCM_ERR_OK             Stop is successful
    @retval     #BCM_ERR_BUSY           Stop is being processed
    @retval     #BCM_ERR_INVAL_STATE    Any of the sub-components is in wrong
                                        state

    @post Once successfully started, ISYS shall be in #ISYS_STATE_READY state

    @trace  #BRCM_SWREQ_ISYS

    @limitations None
*/
int32_t ISYS_Stop(void);

/** @brief Configure ISYS sub-components

    This API configures ISYS sub-component based on configuration type

    @behavior Sync, Non-reentrant

    @pre ISYS shall be in #ISYS_STATE_INITIALISED or #ISYS_STATE_READY
         state while calling this API

    @param[in]      aP_Cfg      ISYS sub-component configuration


    @retval     #BCM_ERR_OK             Configuration is successful
    @retval     #BCM_ERR_INVAL_STATE    Any of the sub-components is in wrong
                                        state
    @retval     #BCM_ERR_INVAL_PARAMS   Configuration is invalid

    @post Once successfully configured, ISYS shall be in #ISYS_STATE_READY state

    @trace  #BRCM_SWREQ_ISYS_CONFIGURATION

    @limitations None
*/
int32_t ISYS_Configure(const ISYS_ConfigsType *aP_Cfg);

/** @brief Process ISYS events

    This API shall be invoked when ISP/UNICAM interrupt occurs. This should not
    be invoked from ISR context.

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval     #ISYS_EventType

    @post None

    @trace  #BRCM_SWREQ_ISYS

    @limitations None
*/
ISYS_EventType ISYS_ProcessEvent(void);

/** @brief Queue fill buffer request

    This queues fill Buffer request to the ISYS library

    @behavior Async, Non-reentrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[in]      aPassIdx        Pass index
    @param[in]      aOutIdx         ISYS Output index
    @param[in]      aBufParam       ISYS buffer to be filled

    @retval     #BCM_ERR_OK             Request has been posted successfully
    @retval     #BCM_ERR_BUSY           Queue is full, client shall retry
    @retval     #BCM_ERR_INVAL_PARAMS   If parameters passed is not valid
    @retval     #BCM_ERR_INVAL_STATE    If state is not #ISYS_STATE_STREAMING

    @post None

    @trace  #BRCM_SWREQ_ISYS

    @limitations None
*/
int32_t ISYS_FillBuffer(ISYS_PassIDXType aPassIdx,
                        ISYS_OutIDXType aOutIdx,
                        const ISYS_BufferType * const aBufParam);

/** @brief Get queued buffer

    This API shall be called to get queued buffer. Buffer shall be ready
    after DMA initiated from ISP is done.

    @behavior Sync, Non-reentrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[in]      aPassIdx        Pass index
    @param[in]      aOutIdx         ISYS Output index
    @param[out]     aBufParam       ISYS buffer

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Buffer has been filled successfully
    @retval     #BCM_ERR_UNKNOWN        If there was no corresponding #ISYS_FillBuffer request
    @retval     #BCM_ERR_BUSY           If ISYS is not done with filling the buffer
    @retval     #BCM_ERR_INVAL_PARAMS   If parameters passed is not valid
    @retval     #BCM_ERR_INVAL_STATE    If state is not #ISYS_STATE_STREAMING

    @post None

    @trace  #BRCM_SWREQ_ISYS

    @limitations None
*/
int32_t ISYS_GetFilledBuffer(ISYS_PassIDXType aPassIdx,
                             ISYS_OutIDXType aOutIdx,
                             ISYS_BufferType * const aBufParam);

/** @brief Brief description of COMP_Function

    This API lets the client retrieve the present state of the ISYS sub-system

    @behavior Sync, Re-entrant

    @pre None

    @param      aP_Info   Pointer to ISYS information structure

    @return     void

    @post None

    @trace  #BRCM_SWREQ_ISYS

    @limitations None
*/
void ISYS_GetInfo(ISYS_InfoType *aP_Info);

#endif /* ISYS_H */
/** @} */
