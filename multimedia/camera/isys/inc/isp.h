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
    @addtogroup grp_isp_ifc
    @{
    @file isp.h
    @brief Data structures and API's of ISP driver.
    This file exposes ISP driver interfaces.
*/

#ifndef ISP_H
#define ISP_H

#include <isys_types.h>
#include <isp_params.h>
#include <isp_configs.h>
#include <isp_image.h>
#include <isp_stats.h>

/**
    @name ISP API IDs
    @{
    @brief API IDs for ISP
*/
#define BRCM_SWARCH_ISP_INT_MACRO                          (0x00U)  /**< @brief #ISP_INT_ERROR                   */
#define BRCM_SWARCH_ISP_MAX_NUM_IN_BUFS_MACRO              (0x01U)  /**< @brief #ISP_MAX_NUM_IN_BUFS             */
#define BRCM_SWARCH_ISP_MAX_NUM_OUT_BUFS_MACRO             (0x02U)  /**< @brief #ISP_MAX_NUM_OUT_BUFS            */
#define BRCM_SWARCH_ISP_IMAGE_PLANE_IDX_TYPE               (0x03U)  /**< @brief #ISP_ImagePlaneIdxType           */
#define BRCM_SWARCH_ISP_OUT_ENABLE_TYPE                    (0x04U)  /**< @brief #ISP_OutEnableType               */
#define BRCM_SWARCH_ISP_OUT_IDX_TYPE                       (0x05U)  /**< @brief #ISP_OutIdxType                  */
#define BRCM_SWARCH_ISP_PASS_IDX_TYPE                      (0x06U)  /**< @brief #ISP_PassIdxType                 */
#define BRCM_SWARCH_ISP_IMAGE_PLANE_TYPE                   (0x07U)  /**< @brief #ISP_ImagePlaneType              */
#define BRCM_SWARCH_ISP_BUF_PARAM_TYPE                     (0x08U)  /**< @brief #ISP_BufParamType                */
#define BRCM_SWARCH_ISP_IN_BUF_PARAM_TYPE                  (0x09U)  /**< @brief #ISP_InBufParamType              */
#define BRCM_SWARCH_ISP_OUT_BUF_PARAM_TYPE                 (0x0aU)  /**< @brief #ISP_OutBufParamType             */
#define BRCM_SWARCH_ISP_HANDSHAKE_PARAM_TYPE               (0x0bU)  /**< @brief #ISP_HandshakeParamType          */
#define BRCM_SWARCH_ISP_DUAL_PASS_MODE_TYPE                (0x0cU)  /**< @brief #ISP_DualPassMode                */
#define BRCM_SWARCH_ISP_DUALPASS_PARAM_TYPE                (0x0dU)  /**< @brief #ISP_DualPassParamType           */
#define BRCM_SWARCH_ISP_TRIGGER_INFO_TYPE                  (0x0eU)  /**< @brief #ISP_TriggerInfoType             */
#define BRCM_SWARCH_ISP_INIT_PROC                          (0x0fU)  /**< @brief #ISP_Init                        */
#define BRCM_SWARCH_ISP_DEINIT_PROC                        (0x10U)  /**< @brief #ISP_Deinit                      */
#define BRCM_SWARCH_ISP_START_PROC                         (0x11U)  /**< @brief #ISP_Start                       */
#define BRCM_SWARCH_ISP_STOP_PROC                          (0x12U)  /**< @brief #ISP_Stop                        */
#define BRCM_SWARCH_ISP_CONFIGURE_PROC                     (0x13U)  /**< @brief #ISP_Configure                   */
#define BRCM_SWARCH_ISP_GET_PARAM_PROC                     (0x14U)  /**< @brief #ISP_GetParam                    */
#define BRCM_SWARCH_ISP_SET_PARAM_PROC                     (0x15U)  /**< @brief #ISP_SetParam                    */
#define BRCM_SWARCH_ISP_DISABLE_HW_SYNC_PROC               (0x16U)  /**< @brief #ISP_DisableHWSync               */
#define BRCM_SWARCH_ISP_GET_EVENT_PROC                     (0x17U)  /**< @brief #ISP_GetEvent                    */
#define BRCM_SWARCH_ISP_TRIGGER_PROC                       (0x18U)  /**< @brief #ISP_Trigger                     */
#define BRCM_SWARCH_ISP_CONFIGURE_RAW_INPUT_PROC           (0x19U)  /**< @brief #ISP_ConfigureRawInput           */
#define BRCM_SWARCH_ISP_CONFIGURE_BLACK_LEVEL_PROC         (0x1aU)  /**< @brief #ISP_ConfigureBlackLevel         */
#define BRCM_SWARCH_ISP_CONFIGURE_LENS_SHADING_PROC        (0x1bU)  /**< @brief #ISP_ConfigureLensShading        */
#define BRCM_SWARCH_ISP_CONFIGURE_HDR_PROC                 (0x1cU)  /**< @brief #ISP_ConfigureHDR                */
#define BRCM_SWARCH_ISP_CONFIGURE_BAYER_DENOISE_PROC       (0x1dU)  /**< @brief #ISP_ConfigureBayerDenoise       */
#define BRCM_SWARCH_ISP_CONFIGURE_STATISTICS_PROC          (0x1eU)  /**< @brief #ISP_ConfigureStatistics         */
#define BRCM_SWARCH_ISP_CONFIGURE_WB_PROC                  (0x1fU)  /**< @brief #ISP_ConfigureWB                 */
#define BRCM_SWARCH_ISP_CONFIGURE_GAIN_PROC                (0x20U)  /**< @brief #ISP_ConfigureGain               */
#define BRCM_SWARCH_ISP_CONFIGURE_DEFECTIVE_PIXEL_PROC     (0x21U)  /**< @brief #ISP_ConfigureDefectivePixel     */
#define BRCM_SWARCH_ISP_CONFIGURE_FRINGING_PROC            (0x22U)  /**< @brief #ISP_ConfigureFringing           */
#define BRCM_SWARCH_ISP_CONFIGURE_CROSS_TALK_PROC          (0x23U)  /**< @brief #ISP_ConfigureCrossTalk          */
#define BRCM_SWARCH_ISP_CONFIGURE_COLOUR_DENOISE_PROC      (0x24U)  /**< @brief #ISP_ConfigureColourDenoise      */
#define BRCM_SWARCH_ISP_CONFIGURE_DEMOSAIC_PROC            (0x25U)  /**< @brief #ISP_ConfigureDemosaic           */
#define BRCM_SWARCH_ISP_CONFIGURE_SHARPEN_PROC             (0x26U)  /**< @brief #ISP_ConfigureSharpen            */
#define BRCM_SWARCH_ISP_CONFIGURE_COLOUR_CORRECTION_PROC   (0x27U)  /**< @brief #ISP_ConfigureColourCorrection   */
#define BRCM_SWARCH_ISP_CONFIGURE_GAMMA_PROC               (0x28U)  /**< @brief #ISP_ConfigureGamma              */
#define BRCM_SWARCH_ISP_CONFIGURE_YG_COEFFS_PROC           (0x29U)  /**< @brief #ISP_ConfigureYGCoeffs           */
#define BRCM_SWARCH_ISP_CONFIGURE_FALSE_COLOUR_PROC        (0x2aU)  /**< @brief #ISP_ConfigureFalseColour        */
#define BRCM_SWARCH_ISP_CONFIGURE_LOCAL_TONEMAP_PROC       (0x2bU)  /**< @brief #ISP_ConfigureLocalToneMap       */
#define BRCM_SWARCH_ISP_CONFIGURE_CHROMINANCE_STRETCH_PROC (0x2cU)  /**< @brief #ISP_ConfigureChrominanceStretch */
#define BRCM_SWARCH_ISP_CONFIGURE_TONEMAP_PROC             (0x2dU)  /**< @brief #ISP_ConfigureToneMap            */
#define BRCM_SWARCH_ISP_CONFIGURE_COLOUR_CONVERSION_PROC   (0x2eU)  /**< @brief #ISP_ConfigureColourConversion   */
#define BRCM_SWARCH_ISP_CONFIGURE_HIRES_OUTPUT_PROC        (0x2fU)  /**< @brief #ISP_ConfigureHiresOutput        */
#define BRCM_SWARCH_ISP_CONFIGURE_LORES_RESIZE_PROC        (0x30U)  /**< @brief #ISP_ConfigureLoresResize        */
#define BRCM_SWARCH_ISP_CONFIGURE_LORES_OUTPUT_PROC        (0x31U)  /**< @brief #ISP_ConfigureLoresOutput        */
#define BRCM_SWARCH_ISP_CONFIGURE_FRAME_PROC               (0x32U)  /**< @brief #ISP_ConfigureFrame              */
#define BRCM_SWARCH_ISP_CONFIGURE_INPUT_BUFFER_PROC        (0x33U)  /**< @brief #ISP_ConfigureInputBuffer        */
#define BRCM_SWARCH_ISP_CONFIGURE_OUTPUT_BUFFER_PROC       (0x34U)  /**< @brief #ISP_ConfigureOuputBuffer        */
#define BRCM_SWARCH_ISP_CONFIGURE_HANDSHAKE_PROC           (0x35U)  /**< @brief #ISP_ConfigureHandshake          */
#define BRCM_SWARCH_ISP_CONFIGURE_DUALPASS_PROC            (0x36U)  /**< @brief #ISP_ConfigureDualPass           */
#define BRCM_SWARCH_ISP_CONFIGURE_SET_CV_BUFFER_PROC       (0x37U)  /**< @brief #ISP_SetCVBuffer                 */
#define BRCM_SWARCH_ISP_CONFIGURE_GET_CV_BUFFER_PROC       (0x38U)  /**< @brief #ISP_GetCVBuffer                 */
#define BRCM_SWARCH_ISP_CONFIGURE_GET_STATISTICS_PROC      (0x39U)  /**< @brief #ISP_GetStatistics               */
#define BRCM_SWARCH_ISP_UPDATE_FRAME_NUM_PROC              (0x3aU)  /**< @brief #ISP_UpdateFrameNum              */
#define BRCM_SWARCH_ISP_UPDATE_FRAME_TIME_STAMP_PROC       (0x3bU)  /**< @brief #ISP_UpdateFrameTimeStamp        */
/** @} */

/**
    @name Macro for ISP interrupt types
    @{
    @brief Macro for ISP interrupt types

    @trace  #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_INT_ERROR   (0x1UL)
#define ISP_INT_EOT_HV  (0x2UL)
#define ISP_INT_EOT_CV  (0x4UL)
#define ISP_INT_EOD     (0x8UL)
#define ISP_INT_STATS   (0x10UL)
/** @} */

/**
    @brief Maximum number of ISP input buffers

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_MAX_NUM_IN_BUFS     (2UL)

/**
    @brief Maximum number of ISP output buffers

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_MAX_NUM_OUT_BUFS    (2UL)

/**
    @name ISP_ImagePlaneIdxType
    @{
    @brief ISP image plane index type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_ImagePlaneIdxType;
#define ISP_IMAGE_PLANE_IDX_RY  (0UL)
#define ISP_IMAGE_PLANE_IDX_GU  (1UL)
#define ISP_IMAGE_PLANE_IDX_BV  (2UL)
#define ISP_IMAGE_PLANE_IDX_MAX (3UL)
/** @} */

/**
    @name ISP_OutEnableType
    @{
    @brief Output enable for ISP

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_OutEnableType;
#define ISP_OUT_ENABLE_LORES    (0x1UL)
#define ISP_OUT_ENABLE_HIRES    (0x2UL)
/** @} */

/**
    @name ISP_OutIdxType
    @{
    @brief Output index for ISP

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_OutIdxType;
#define ISP_OUT_IDX_LORES   (0UL)
#define ISP_OUT_IDX_HIRES   (1UL)
#define ISP_OUT_IDX_MAX     (2UL)
/** @} */

/**
    @name ISP_PassIdxType
    @{
    @brief Pass index for ISP

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_PassIdxType;      /**< @brief Index for pass type */
#define ISP_PASS_IDX_HV    (0UL)   /**< @brief Index for HV pass */
#define ISP_PASS_IDX_CV    (1UL)   /**< @brief Index for CV pass */
#define ISP_PASS_IDX_MAX   (2UL)   /**< @brief Maximum number of ISP passes */
/** @} */

/**
    @brief ISP plane type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ImagePlaneType {
    uint32_t startAddr;
    uint32_t endAddr;
    uint8_t  isEnabled;
    uint32_t stride;
    uint32_t allocLines;
    uint32_t widthInBytes;
} ISP_ImagePlaneType;

/**
    @brief ISP buffer parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_BufParamType {
    ISP_ImagePlaneType imagePlane[ISP_IMAGE_PLANE_IDX_MAX];
    uint32_t numMBrows;
    uint32_t numSlices;
} ISP_BufParamType;

/**
    @brief ISP input buffer parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_InBufParamType {
    ISP_BufParamType bufParam[ISP_MAX_NUM_IN_BUFS][ISP_IN_IDX_MAX];
    uint32_t numBufs;
} ISP_InBufParamType;

/**
    @brief ISP output buffer parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_OutBufParamType {
    ISP_BufParamType bufParam[ISP_MAX_NUM_OUT_BUFS][ISP_OUT_IDX_MAX];
    uint32_t sliceNum;
    uint32_t frameNum;
    uint64_t timestamp;
    uint32_t numBufs;
} ISP_OutBufParamType;

/**
    @brief ISP handshake parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_HandshakeParamType {
    /**< Threshold at which ISP would start stalling the pipeline for lores(multiple of lo_sync_lines) */
    uint32_t loResSyncThreshold;

    /**< Number of lines after which ISP would signal ready for lores */
    uint32_t loResSyncLines;

    /**< Threshold at which ISP would start stalling the pipeline for hires (multiple of hi_sync_lines) */
    uint32_t hiResSyncThreshold;

    /**< Number of lines after which ISP would signal ready for hires */
    uint32_t hiResSyncLines;
} ISP_HandshakeParamType;

/** @name ISP_DualPassMode
    @{
    @brief ISP dual pass mode type.

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_DualPassMode; /**< @brief typedef for dual pass mode */
#define ISP_DUAL_PASS_MODE_NONE  (0UL)    /**< @brief Dual-Pass mode is disabled */
#define ISP_DUAL_PASS_MODE_FRAME (1UL)    /**< @brief Frame based Dual-Pass mode */
#define ISP_DUAL_PASS_MODE_LL    (2UL)    /**< @brief Low Latency Dual-Pass mode */
/** @} */

/**
    @brief ISP Dual-Pass parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_DualPassParamType {
    ISP_DualPassMode dpMode;
    uint32_t hvIndex;
    uint32_t cvIndex;
    uint32_t outputEnable[ISP_PASS_IDX_MAX];
} ISP_DualPassParamType;

/**
    @brief ISP trigger info type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_TriggerInfoType {
    uint32_t inBufIdx;
    uint32_t outBufIdx;
    uint32_t frameNum;
    uint64_t timestamp;
} ISP_TriggerInfoType;

/** @brief Initialize ISP

    This API initializes ISP driver.

    @behavior Sync, Non-reentrant

    @pre This API shall only be called in the #ISP_STATE_POWEROFF state

    @param      void

    @retval     #BCM_ERR_OK             Initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully initialized, the ISP driver shall be in
          #ISP_STATE_INITIALISED state

    @trace  #BRCM_SWREQ_ISP_INIT

    @limitations None
*/
extern int32_t ISP_Init(void);

/** @brief De-initialize ISP

    This API de-initializes ISP driver.

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval     #BCM_ERR_OK             De-initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong  state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully de-initialized, the ISP driver shall be in
          #ISP_STATE_POWEROFF state

    @trace  #BRCM_SWREQ_ISP_INIT

    @limitations None
*/
extern int32_t ISP_Deinit(void);

/** @brief Enable ISP driver

    This enables all ISP driver in the appropriate order

    @behavior Sync, Non-reentrant

    @pre ISP shall be in #ISP_STATE_READY while calling this API

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Start is successful
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Camera peripheral mode
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully started, ISP shall be in #ISP_STATE_POWEROFF state

    @trace  #BRCM_SWREQ_ISP

    @limitations None
*/
extern int32_t ISP_Start(void);

/** @brief Disable ISP driver

    This API disables all driver in the appropriate order

    @behavior Sync, Non-reentrant

    @pre ISP shall be in #ISP_STATE_ENABLED state while calling this API

    @param      void

    @retval     #BCM_ERR_OK             Stop is successful
    @retval     #BCM_ERR_TIME_OUT       ISP stop failed
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully started, ISP shall be in #ISP_STATE_READY state

    @trace  #BRCM_SWREQ_ISP

    @limitations None
*/
extern int32_t ISP_Stop(void);

/** @brief Configure ISP driver

    This API configures ISP sub-component based on configuration type

    @behavior Sync, Non-reentrant

    @pre ISP shall be in #ISP_STATE_INITIALISED or #ISP_STATE_READY
         state while calling this API

    @param[in]      aP_Cfg       ISP sub-component configuration
    @param[in]      aIfcConfig   ISP common interface configuration
    @param[in]      aBuf         ISP buffer configuration


    @retval     #BCM_ERR_OK             Configuration is successful
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong data1 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong image1 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong data0 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong image0 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Insufficient configurations
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->blkCfg[i].offset
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->blkCfgCount
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->numDataLanes
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->mode
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong cfgMagic
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong cfgType
    @retval     #BCM_ERR_INVAL_PARAMS   aCfg is NULL
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully configured, ISP shall be in #ISP_STATE_READY state

    @trace  #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_Configure(const ISP_ImageType *aCfg);

/** @brief Get ISP dynamic parameters

    This API retrieves ISP parameters.

    @behavior Sync, Non-reentrant

    @pre ISP shall be in #ISP_STATE_ENABLED state while calling this API

    @param[in]      aType       Parameter type
    @param[in]      aParam      Union of supported params
    @param[out]     aIsEnabled  Is the specific block enabled or not

    @retval     #BCM_ERR_OK             Retrieval is successful
    @retval     #BCM_ERR_INVAL_PARAMS   aType is Invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aParam is Invalid
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post None

    @trace  #BRCM_SWREQ_ISP_PARAM_UPDATE

    @limitations None
*/
extern int32_t ISP_GetParam(const uint32_t aType,
                            ISP_ParamType aParam,
                            uint8_t *aIsEnabled);

/** @brief Set ISP dynamic parameters

    This API sets ISP dynamic parameters

    @behavior Sync, Non-reentrant

    @pre ISP shall be in #ISP_STATE_ENABLED state while calling this API

    @param[in]      aType       Parameter type
    @param[in]      aParam      Union of supported params

    @retval     #BCM_ERR_OK             Retrieval is successful
    @retval     #BCM_ERR_INVAL_PARAMS   aType is Invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aParam is Invalid
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post None

    @trace  #BRCM_SWREQ_ISP_PARAM_UPDATE

    @limitations None
*/
extern int32_t ISP_SetParam(const uint32_t aType, ISP_ParamType aParam);

/** @brief Disable UNICAM-ISP hardware synchronization

    This API disables UNICAM-ISP hardware synchronization

    @behavior Sync, Non-reentrant

    @pre ISP shall be in #ISP_STATE_INITIALISED state while calling this API

    @param      void

    @retval     #BCM_ERR_OK             Disable is successful
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state

    @post None

    @trace  #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_DisableHWSync(void);

/** @brief Gets ISP hardware event

    This API gets ISP hardware event occurred

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval uint32_t Mask of event's occurred

    @post None

    @trace  #BRCM_SWREQ_ISP

    @limitations None
*/
extern uint32_t ISP_GetEvent(void);

/** @brief Trigger ISP for processing

    This API trigger ISP for processing

    @behavior Sync, Non-reentrant

    @pre ISP shall be in #ISP_STATE_ENABLED state while calling this API

    @param[in]      aTriggerInfo #ISP_TriggerInfoType input

    @retval     #BCM_ERR_OK             Trigger is successful
    @retval     #BCM_ERR_INVAL_PARAMS   aTriggerInfo is NULL
    @retval     #BCM_ERR_INVAL_STATE    ISP driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post None

    @trace  #BRCM_SWREQ_ISP_DP

    @limitations None
*/
extern int32_t ISP_Trigger(ISP_TriggerInfoType *aTriggerInfo);

/** @brief Configure RAW Input Formatter block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aIIConfig       RAW Input configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureRawInput(const ISP_CfgPassType aPass,
                                     const ISP_RawInputConfigType *aIIConfig);

/** @brief Configure Black Level Compensation block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aBLConfig       Black Level configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureBlackLevel(const ISP_CfgPassType aPass,
                                     const ISP_BlackLevelConfigType *aBLConfig);

/** @brief Configure Lens shading compensation block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aLSConfig       Lens Shading configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureLensShading(const ISP_CfgPassType aPass,
                                   const ISP_LensShadingConfigType *aLSConfig);

/** @brief Configure HDR block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aHDRConfig      HDR configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureHDR(const ISP_CfgPassType aPass,
                                   const ISP_HDRConfigType *aHDRConfig);

/** @brief Configure Bayer Denoise block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aDNConfig       Bayer Denoise configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureBayerDenoise(const ISP_CfgPassType aPass,
                                   const ISP_BayerDenoiseConfigType *aDNConfig);

/** @brief Configure Statistics Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aStatsConfig    Statistics configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureStatistics(const ISP_CfgPassType aPass,
                                  const ISP_StatisticsConfigType *aStatsConfig);

/** @brief Configure White Balance Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aWBConfig       White Balance configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureWB(const ISP_CfgPassType aPass,
                                  const ISP_WBConfigType *aWBConfig);

/** @brief Configure Gain Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aGainConfig     Gain configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureGain(const ISP_CfgPassType aPass,
                                  const ISP_GainConfigType *aGainConfig);

/** @brief Configure Defective Pixel Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aDP             Defective Pixel configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureDefectivePixel(const ISP_CfgPassType aPass,
                                  const ISP_DefectivePixelConfigType *aDP);

/** @brief Configure Fringing Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aFGConfig       Fringing configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureFringing(const ISP_CfgPassType aPass,
                                  const ISP_FringingConfigType *aFGConfig);

/** @brief Configure Cross-Talk Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aXCConfig       Cross-Talk configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureCrossTalk(const ISP_CfgPassType aPass,
                                  const ISP_CrossTalkConfigType *aXCConfig);

/** @brief Configure Colour Denoise Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aCDNConfig      Colour Denoise configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureColourDenoise(const ISP_CfgPassType aPass,
                                  const ISP_ColourDenoiseConfigType *aCDNConfig);

/** @brief Configure Demosaic Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aDMConfig       Demosaic configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureDemosaic(const ISP_CfgPassType aPass,
                                  const ISP_DemosaicConfigType *aDMConfig);

/** @brief Configure Sharpening Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aSHConfig       Sharpening configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureSharpen(const ISP_CfgPassType aPass,
                                  const ISP_SharpenConfigType *aSHConfig);

/** @brief Configure Colour Correction Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aYCConfig       Colour Correction configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureColourCorrection(const ISP_CfgPassType aPass,
                              const ISP_ColourCorrectionConfigType *aYCConfig);

/** @brief Configure Gamma Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aGMConfig       Gamma configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureGamma(const ISP_CfgPassType aPass,
                                  const ISP_GammaConfigType *aGMConfig);

/** @brief Configure RGB to YUV Conversion Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aYGConfig       RGB to YUV configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureYGCoeffs(const ISP_CfgPassType aPass,
                              const ISP_RGB2YUVConversionConfigType *aYGConfig);

/** @brief Configure False Colour Correction Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aFCConfig       False Colour Correction configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureFalseColour(const ISP_CfgPassType aPass,
                              const ISP_FalseColourConfigType *aFCConfig);

/** @brief Configure Local Tone Mapping Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aLTMConfig      Local Tone Mapping configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureLocalToneMap(const ISP_CfgPassType aPass,
                              const ISP_LTMConfigType *aLTMConfig);

/** @brief Configure Chrominance Stretch Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aCPConfig       Chrominance Stretch configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureChrominanceStretch(const ISP_CfgPassType aPass,
                              const ISP_ChrominanceStretchConfigType *aCPConfig);

/** @brief Configure Tone Mapping Block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aTMConfig       Tone Mapping configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureToneMap(const ISP_CfgPassType aPass,
                              const ISP_ToneMapConfigType *aTMConfig);

/** @brief Configure Color Conversion block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aCCConfig       Color Conversion configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureColourConversion(const ISP_CfgPassType aPass,
                                   const ISP_ConversionConfigType *aCCConfig);

/** @brief Configure Hires Output formatter block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aHOConfig       Hires Output configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureHiresOutput(const ISP_CfgPassType aPass,
                                   const ISP_OutputConfigType *aHOConfig);

/** @brief Configure Low Resolution Resize block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aLRConfig       Lo-res Resize configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureLoresResize(const ISP_CfgPassType aPass,
                                    ISP_LoresResizeConfigType *aLRConfig);

/** @brief Configure Lores Output formatter block

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aLOConfig       Lores Output configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureLoresOutput(const ISP_CfgPassType aPass,
                                   const ISP_OutputConfigType *aLOConfig);

/** @brief Configure Frame parameters

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aPass           Pass  type as in #ISP_CfgPassType
    @param[in]      aFRConfig       Frame configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureFrame(const ISP_CfgPassType aPass,
                                   const ISP_FrameConfigType *aFRConfig);

/** @brief Configure Input buffer addresses

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aInBufParam       Input buffer configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureInputBuffer(ISP_InBufParamType *aInBufParam);

/** @brief Configure Output buffer addresses

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aOutBufParam       Output buffer configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureOuputBuffer(ISP_OutBufParamType *aOutBufParam);

/** @brief Configure Handshake Parameters

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aHSParam       Handshake configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureHandshake(ISP_HandshakeParamType *aHSParam);

/** @brief Configure Dual-Pass parameters

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aDPParam       Dual-Pass configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_CONFIGURATION

    @limitations None
*/
extern int32_t ISP_ConfigureDualPass(ISP_DualPassParamType *aDPParam);

/** @brief Set CV Output buffer addresses

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aCVBufParam       CV buffer configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_DP

    @limitations None
*/
extern int32_t ISP_SetCVBuffer(ISP_OutBufParamType *aCVBufParam);

/** @brief Get filled CV buffer

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aCVBufParam       CV buffer configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_DP

    @limitations None
*/
extern int32_t ISP_GetCVBuffer(ISP_OutBufParamType *aCVBufParam);

/** @brief Get ISP Statistics (latest)

    @behavior Sync, Non-reentrant

    @pre ISP is initialized

    @param[in]      aCVBufParam       CV buffer configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      Configuration failed
    @retval     #BCM_ERR_INVAL_PARAMS Invalid params
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP_STATISTICS

    @limitations None
*/
extern int32_t ISP_GetStatistics(ISP_StatsOutputType **aStatsP,
                                 uint8_t *aIsEnabled,
                                 uint32_t *aFN);

/** @brief Update source frame number
    @behavior Sync, Non-reentrant

    @pre ISP is enabled

    @param[in]      aFrameNum       Frame number

    Return values are documented in reverse-chronological order

    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      SVC failed
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP

    @limitations None
*/
extern int32_t ISP_UpdateFrameNum(uint32_t aFrameNum);

/** @brief Update source frame time-stamp
    @behavior Sync, Non-reentrant

    @pre ISP is enabled

    @param[in]      aFrameTimeStamp       Frame time-stamp

    Return values are documented in reverse-chronological order

    @retval     #BCM_ERR_OK           Success
    @retval     #BCM_ERR_UNKNOWN      SVC failed
    @retval     #BCM_ERR_INVAL_STATE  ISP is in invalid state for this API

    @post None

    @trace #BRCM_SWREQ_ISP

    @limitations None
*/
extern int32_t ISP_UpdateFrameTimeStamp(uint64_t aFrameTimeStamp);

#endif /* ISP_H */
/** @} */
