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
    @defgroup grp_csn_ifc Interface
    @ingroup grp_csn

    @addtogroup grp_csn_ifc
    @{
    @section sec_csn_overview Overview
    @todo Add overview

    @section sec_csn_fsm State Machine
    @todo Add state machine

    @section sec_csn_seq Sequence Diagrams
    @todo Add sequence diagram

    @section sec_csn_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   Update CPU load here                            |
    | Memory Bandwidth  |   Update Memory Bandwidth here                    |
    | HW Utilization    |   Update Hardware utilization here                |

    @limitations Add component limitations here

    @file camera_sensor.h
    @brief Camera Sensor Interface
    This header file contains the interface functions for CSN
    @version 0.1 Initial Version
*/
#ifndef CAMERA_SENSOR_H
#define CAMERA_SENSOR_H

#include "camera_sensor_image.h"
#include "camera_sensor_module.h"

/**
    @name CSN Architecture IDs
    @{
    @brief Architecture IDs for CSN
*/
#define BRCM_SWARCH_CSN_PARAM_MACRO           (0x00U) /**< @brief #CSN_PARAM_EXPOSURE_LONG */
#define BRCM_SWARCH_CSN_STATE_TYPE            (0x01U) /**< @brief #CSN_StateType           */
#define BRCM_SWARCH_CSN_EXP_PARAM_TYPE        (0x02U) /**< @brief #CSN_ExpParamType        */
#define BRCM_SWARCH_CSN_WB_PARAM_TYPE         (0x03U) /**< @brief #CSN_WB_ParamType        */
#define BRCM_SWARCH_CSN_PARAM_TYPE            (0x04U) /**< @brief #CSN_ParamsType          */
#define BRCM_SWARCH_CSN_EMB_OFFSETS_TYPE      (0x05U) /**< @brief #CSN_EmbOffsetsType      */
#define BRCM_SWARCH_CSN_GET_STATE_PROC        (0x06U) /**< @brief #CSN_GetState            */
#define BRCM_SWARCH_CSN_INIT_PROC             (0x07U) /**< @brief #CSN_Init                */
#define BRCM_SWARCH_CSN_DEINIT_PROC           (0x08U) /**< @brief #CSN_Deinit              */
#define BRCM_SWARCH_CSN_START_PROC            (0x09U) /**< @brief #CSN_Start               */
#define BRCM_SWARCH_CSN_STOP_PROC             (0x0aU) /**< @brief #CSN_Stop                */
#define BRCM_SWARCH_CSN_CONFIGURE_PROC        (0x0bU) /**< @brief #CSN_Configure           */
#define BRCM_SWARCH_CSN_GET_PARAM_PROC        (0x0cU) /**< @brief #CSN_GetParam            */
#define BRCM_SWARCH_CSN_SET_PARAM_PROC        (0x0dU) /**< @brief #CSN_SetParam            */
#define BRCM_SWARCH_CSN_TEST_PARAM_PROC       (0x0eU) /**< @brief #CSN_TestParam           */
#define BRCM_SWARCH_CSN_PARSE_METADATA_PROC   (0x0fU) /**< @brief #CSN_ParseMetadata       */
#define BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC (0x10U) /**< @brief #CSN_ParseStatistics     */
#define BRCM_SWARCH_CSN_GET_EMB_OFFSETS_PROC  (0x11U) /**< @brief #CSN_GetEmbOffsets       */
/** @} */

/**
    @name CSN Parameter types
    @{
    @brief Sensor parameter mask

    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE
*/
#define CSN_PARAM_EXPOSURE_LONG  (0x1UL)        /**< @brief Long exposure shutter  */
#define CSN_PARAM_EXPOSURE_SHORT (0x2UL)        /**< @brief Short exposure shutter */
#define CSN_PARAM_GAIN_LONG      (0x4UL)        /**< @brief Long exposure gain     */
#define CSN_PARAM_GAIN_SHORT     (0x8UL)        /**< @brief Short exposure gain    */
#define CSN_PARAM_WB             (0x10UL)       /**< @brief White Balance gain     */
/** @} */

/**
    @name CSN_StateType
    @{
    @brief Various states of CSN

    Refer @ref sec_csn_fsm for more details

    @trace #BRCM_SWREQ_CSN_INIT
*/
typedef uint32_t CSN_StateType;      /**< @brief typedef for CSN states  */
#define CSN_STATE_POWEROFF    (0UL)  /**< @brief CSN State Poweroff      */
#define CSN_STATE_INITIALISED (1UL)  /**< @brief CSN State Initialized   */
#define CSN_STATE_READY       (2UL)  /**< @brief CSN State Ready         */
#define CSN_STATE_STREAMING   (3UL)  /**< @brief CSN State Streaming     */
#define CSN_STATE_ERROR       (4UL)  /**< @brief CSN State Error         */
/** @} */

/**
    @brief CSN Exposure parameter structure

    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE
 */
typedef struct _CSN_ExpParamType {
    uint32_t shutter[CSN_MODULE_EXP_IDX_MAX];  /**< @brief eShutter in microseconds       */
    uint16_t gain[CSN_MODULE_EXP_IDX_MAX];     /**< @brief Analog gain in 8.8 formats     */
    uint32_t expRatio;                  /**< @brief EExposure ratio between long
                                             and short exposures                   */
    uint32_t flickerPeriod;             /**< @brief Flicker period in microseconds */
} CSN_ExpParamType;

/**
    @brief CSN WB parameter structure

    CSN WB parameter structure containing information about Red Gain and Blue
    gain applied.

    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE
*/
typedef struct _CSN_WB_ParamType {
    uint16_t gainR;         /**< @brief Gain for R channel (8.8 format) */
    uint16_t gainB;         /**< @brief Gain for B channel (8.8 format) */
} CSN_WB_ParamType;

/**
    @brief CSN parameter structure

    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE
 */
typedef struct _CSN_ParamsType {
    CSN_ExpParamType *expParam;    /**< @brief Pointer to exposure parameter      */
    CSN_WB_ParamType *wbParam;     /**< @brief Pointer to white balance parameter */
    uint32_t type;                 /**< @brief Mask for type of parameters to
                                        set/test as defined in #CSN_ParamType     */
} CSN_ParamsType;

/**
    @brief Get valid embedded buffer offsets

    @trace #BRCM_SWREQ_CSN_PARSE_ED
 */
typedef struct _CSN_EmbOffsetsType {
    uint16_t metaOffset[CSN_MODULE_EXP_IDX_MAX];  /**< @brief Metadata offset from start of buffer   */
    uint16_t metaSize[CSN_MODULE_EXP_IDX_MAX];    /**< @brief Metadata size                          */
    uint16_t statsOffset[CSN_MODULE_EXP_IDX_MAX]; /**< @brief Statistics offset from start of buffer */
    uint16_t statsSize[CSN_MODULE_EXP_IDX_MAX];   /**< @brief Statistics size                        */
} CSN_EmbOffsetsType;

/** @brief Get the Camera Sensor state

    This is an interface function to get Camera Sensor state

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #CSN_StateType     Present CSN state

    @post None

    @trace   #BRCM_SWREQ_CSN_INIT

    @limitations None
*/
extern CSN_StateType CSN_GetState(void);

/** @brief Initialize Camera Sensor

    This API initializes camera sensor

    @behavior Async, Non-reentrant

    @pre CSN shall be in #CSN_STATE_POWEROFF

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Initialize is successful
    @retval     #BCM_ERR_BUSY           Initialize is in progress
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_POWEROFF

    @post None

    @trace   #BRCM_SWREQ_CSN_INIT

    @limitations None
*/
extern int32_t CSN_Init(void);

/** @brief De-initialize Camera Sensor

    This API de-initializes camera sensor

    @behavior Sync, Non-reentrant

    @pre    None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             De-initialize is successful
    @retval     #BCM_ERR_INVAL_STATE    If state is already #CSN_STATE_POWEROFF

    @post None

    @trace   #BRCM_SWREQ_CSN_INIT

    @limitations None
*/
extern int32_t CSN_Deinit(void);

/** @brief Start Camera Sensor

    This API enable streaming from camera sensor

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_READY

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Start is successful
    @retval     #BCM_ERR_UNKNOWN        Failure in internal functions
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_READY

    @post None

    @trace   #BRCM_SWREQ_CSN

    @limitations None
*/
extern int32_t CSN_Start(void);

/** @brief Stop Camera Sensor

    This API disable streaming from camera sensor

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_STREAMING

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Stop is successful
    @retval     #BCM_ERR_UNKNOWN        Failure in internal functions
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_STREAMING

    @post None

    @trace   #BRCM_SWREQ_CSN

    @limitations None
*/
extern int32_t CSN_Stop(void);

/** @brief Configure Camera Sensor

    This API sets a static configuration to camera sensor.

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_INITIALISED or CSN_STATE_READY state
         while calling this API

    @param[inout]      aParam      Pointer to sensor params (input)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Configuration is successful
    @retval     #BCM_ERR_BUSY           Configuration is in progress
    @retval     #BCM_ERR_UNKNOWN        Failure in internal functions
    @retval     #BCM_ERR_INVAL_PARAMS   Any of configuration is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aCfg is NULL
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_INITIALISED or
                                        #CSN_STATE_READY

    @post None

    @trace   #BRCM_SWREQ_CSN_CONFIGURATION

    @limitations None
*/
extern int32_t CSN_Configure(const CSN_ImageType *aCfg);

/** @brief Get Camera Sensor parameters

    This API sets camera sensor params specified.

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_STREAMING state while calling this API

    @param[inout]      aParam      Pointer to sensor params (input)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Get parameter is successful
    @retval     #BCM_ERR_NOSUPPORT      Get parameter implementation is not
                                        available in CSN
    @retval     #BCM_ERR_INVAL_PARAMS   (aParam is NULL)
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_STREAMING

    @post None

    @trace   #BRCM_SWREQ_CSN_PARAM_UPDATE

    @limitations None
*/
extern int32_t CSN_GetParam(CSN_ParamsType *aParam);

 /** @brief Set Camera Sensor parameters

    This API sets camera sensor params specified.

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_STREAMING state while calling this API

    @param[in]      aParam      Pointer to sensor params (input)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Set parameter is successful
    @retval     #BCM_ERR_UNKNOWN        Set parameter has failed from CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aParam->wbParam is NULL when type is
                                        CSN_PARAM_WB)
    @retval     #BCM_ERR_INVAL_PARAMS   (aParam->expParam is NULL when type is
                                        CSN_PARAM_EXPOSURE_LONG or
                                        CSN_PARAM_EXPOSURE_SHORT or
                                        CSN_PARAM_GAIN_LONG or
                                        CSN_PARAM_GAIN_SHORT)
    @retval     #BCM_ERR_NOSUPPORT      Set parameter implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aParam is NULL) or
                                        (aParam->type is not valid)
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_STREAMING

    @post None

    @trace   #BRCM_SWREQ_CSN_PARAM_UPDATE

    @limitations None
*/
extern int32_t CSN_SetParam(const CSN_ParamsType *aParam);

/** @brief Test Camera Sensor parameters

    This API validates camera sensor params specified and updates the same if
    the passed parameters are not supported.

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_STREAMING state while calling this API

    @param[inout]      aParam      Pointer to sensor params (input/output)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Test parameter is successful
    @retval     #BCM_ERR_UNKNOWN        Test parameter has failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      Test parameter implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aParam is NULL) or
                                        (aParam->type is not valid)
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_STREAMING

    @post None

    @trace   #BRCM_SWREQ_CSN_PARAM_UPDATE

    @limitations None
*/
extern int32_t CSN_TestParam(CSN_ParamsType *aParam);

/** @brief Parse Camera Sensor metadata

    This API parses the metadata received from camera sensor and return
    the filled #CSN_MODULE_MetadataType structure.

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_STREAMING state while calling this API

    @param[in]      aBuffer      Metadata buffer for all exposures
    @param[in]      aSize        Metadata buffer sizes for all exposures
    @param[out]     aMetadata    #CSN_MODULE_MetadataType structure to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Metadata parsing is successful
    @retval     #BCM_ERR_UNKNOWN        Metadata parsing failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      Metadata parsing implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aBuffer is NULL) or
                                        (aStats is NULL)
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_STREAMING

    @post None

    @trace   #BRCM_SWREQ_CSN_PARSE_ED

    @limitations None
*/
extern int32_t CSN_ParseMetadata(const uint8_t * const (aBuffer) [CSN_MODULE_EXP_IDX_MAX],
                                 uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                 CSN_MODULE_MetadataType *aMetadata);

/** @brief Parse Camera Sensor statistics

    This API parses the statistics data received from camera sensor and return
    the filled #CSN_MODULE_StatsType structure.

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_STREAMING state while calling this API

    @param[in]      aBuffer      Statistics buffers for all exposures
    @param[in]      aSize        Statistics buffer sizes for all exposures
    @param[out]     aStats       #CSN_MODULE_StatsType structures to be filled
    @param[out]     aParsedMask  Status of parsed statistics parsed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Statistics parsing is successful
    @retval     #BCM_ERR_UNKNOWN        Statistics parsing failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      Statistics parsing implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aBuffer is NULL) or
                                        (aStats is NULL)
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_STREAMING

    @post None

    @trace   #BRCM_SWREQ_CSN_PARSE_ED

    @limitations None
*/
extern int32_t CSN_ParseStatistics(const uint8_t * const (aBuffer) [CSN_MODULE_EXP_IDX_MAX],
                                   uint32_t (aSize) [CSN_MODULE_EXP_IDX_MAX],
                                   CSN_MODULE_StatsType * const (aStats)[CSN_MODULE_EXP_IDX_MAX],
                                   uint32_t *aParsedMask);

/** @brief Get valid embedded boundary for buffer retrieval

    This API retrieves embedded buffer boundary for all exposures and return
    the filled #CSN_EmbOffsetsType structure.

    @behavior Sync, Reentrant

    @pre CSN shall be in #CSN_STATE_READY state while calling this API

    @param[out]      aEmbOffses      Embedded offset structure

    Return values are documented in reverse-chronological order
    @retval     void

    @post None

    @trace   #BRCM_SWREQ_CSN_PARSE_ED

    @limitations None
*/
extern void CSN_GetEmbOffsets(CSN_EmbOffsetsType *aEmbOffsets);

#endif /* CAMERA_SENSOR_H */

/** @} */
