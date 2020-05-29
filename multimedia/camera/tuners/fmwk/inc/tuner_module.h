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
#ifndef TUNER_MODULE_H
#define TUNER_MODULE_H

/**
 * @file tuner_module.h
 *
 * @brief Data structures and API's of Tuner Module.
 *
 * This file exposes Tuner Module interfaces.
*/

#include <isys_osil.h>
#include <isys_tuner.h>
#include <camera_sensor.h>

/** \def CAM_CONFIG_TUNER
    \brief A macro that defines the config for tuner.
*/
#define CAM_CONFIG_TUNER    (4U)

/** \def TUNER_MAGIC
    \brief A macro that defines Tuner magic Identifier.
*/
#define TUNER_MAGIC           (0x4954554eUL) /* "ITUN" */

/** \def MAX_TUNERS
    \brief A macro that defines Maximum number of tuners available.
*/
#define MAX_TUNERS            (8UL)

/** \def TUNER_MAX_CONFIGS
    \brief A macro that defines maximum number of configs.
*/
#define TUNER_MAX_CONFIGS     (MAX_TUNERS+1UL)

/** \def TUNER_MAX_CONFIG_SIZE
    \brief A macro that defines maximum reserved configuration size.
*/
#define TUNER_MAX_CONFIG_SIZE (51200UL)

/** \def Tuner_CfgType
    \brief Tuner configuration type.
*/
typedef uint32_t Tuner_CfgType;
#define TUNER_CONFIG_COMMON   (0x54434f4dUL) /* "TCOM" */
#define TUNER_CONFIG_AGC      (0x54414743UL) /* "TAGC" */
#define TUNER_CONFIG_AWB      (0x54415742UL) /* "TAWB" */
#define TUNER_CONFIG_DENOISE  (0x5442444eUL) /* "TBDN" */
#define TUNER_CONFIG_CC       (0x5443434dUL) /* "TCCM" */
#define TUNER_CONFIG_DEFPIX   (0x54445043UL) /* "TDPC" */
#define TUNER_CONFIG_SHARPEN  (0x54534841UL) /* "TSHA" */
#define TUNER_CONFIG_BL       (0x54424b4cUL) /* "TBKL" */
#define TUNER_CONFIG_TM       (0x5441544dUL) /* "TATM" */

struct _AWB_TunerConfig;
struct _AGC_TunerConfig;
struct _Denoise_TunerConfig;
struct _CC_TunerConfig;
struct _DefPix_TunerConfig;
struct _Sharpen_TunerConfig;
struct _BL_TunerConfig;
struct _LTM_TunerConfig;

/** \struct  TunerCfg
    \brief Structure containing configuration information.
*/
typedef struct _TunerCfg {
    union {
       const uint8_t *ptr;                            /**<  Generic ptr */
       const struct _Tuner_CommonConfig *commonCfg;   /**<  Ptr to common config */
       const struct _AGC_TunerConfig *agcCfg;         /**<  Ptr to AGC config */
       const struct _AWB_TunerConfig *awbCfg;         /**<  Ptr to AWB config */
       const struct _Denoise_TunerConfig *denoiseCfg; /**<  Ptr to denoise config */
       const struct _CC_TunerConfig *ccCfg;           /**<  Ptr to CC config */
       const struct _DefPix_TunerConfig *defPixCfg;   /**<  Ptr to DefPix config */
       const struct _Sharpen_TunerConfig *sharpenCfg; /**<  Ptr to Sharpen config */
       const struct _BL_TunerConfig *blCfg;           /**<  Ptr to BL config */
       const struct _TM_TunerConfig *tmCfg;           /**<  Ptr to TM config */
    } cfgData;
    uint32_t size;                                    /**<  Size of configuration */
} TunerCfg;

/** \struct  Tuner_CommonConfig
    \brief Structure containing common configuration.
*/
typedef struct _Tuner_CommonConfig {
    uint32_t dummy; /**<  Dummy member */
} Tuner_CommonConfig;

/** \struct  Tuner_ConfigInfo
    \brief Structure containing configuration header.
*/
typedef struct _Tuner_ConfigInfo {
    Tuner_CfgType cfgType; /**<  Configuration type */
    uint32_t dataOffset;   /**<  Data offset for configuration */
    uint32_t count;        /**<  Size of configuration */
} Tuner_ConfigInfo;

/** \struct  Tuner_ImageHdr
    \brief Structure containing Tuner image header.
*/
typedef struct _Tuner_ImageHdr {
    ISYS_CommonImgHdrType commonHdr; /**<  Common ISYS image header   */
    uint32_t numConfigs;             /**<  Number of configs in image */
} Tuner_ImageHdr;

/** \struct  Tuner_Image
    \brief Structure containing Tuner image.
*/
typedef struct _Tuner_Image {
    Tuner_ImageHdr imgHdr;                       /**<  Tuner Image header */
    Tuner_ConfigInfo cfgs[TUNER_MAX_CONFIGS];    /**<  Configs header */
    uint8_t tunerCfgData[TUNER_MAX_CONFIG_SIZE]; /**<  Configuration data */
} Tuner_Image;

/** @brief Tuner module information retrieval function
 *
 * API to Get Tuner module information.
 *
 * @param[in]  None
 * @param[out] None
 *
 * @return      ConfigType
 */
typedef uint32_t (* const TunerGetCfgType )(void);

/** @brief Tuner module initialisation function
 *
 * API to initialize tuner module.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnInit )(void);

/** @brief Tuner module de-initialisation function
 *
 * API to de-initialize tuner module.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnDeinit )(void);

/** @brief Tuner module start function
 *
 * API to start tuner module.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnStart )(void);

/** @brief Tuner module stop function
 *
 * API to stop tuner module.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnStop )(void);

/** @brief Tuner module configure function
 *
 * API to configure tuner module.
 *
 * @param[in]   aCfg        Tuner configuration.
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnConfigure )(const TunerCfg *aCfg);

/** @brief Tuner module metadata update function
 *
 * API to update metadata for tuner module.
 *
 * @param[in]   aMetadata           Metadata information
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnMetadataUpdate )(CSN_MODULE_MetadataType *aMetadata);

/** @brief Tuner module statistics update(ISP) function
 *
 * API to update ISP statistics for tuner module.
 *
 * @param[in]   aStats           Statistics information
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnISP_StatsUpdate )(ISYS_StatisticsType *aStats);

/** @brief Tuner module statistics update (Sensor) function
 *
 * API to update sensor statistics for tuner module.
 *
 * @param[in]   aSensorStats      Sensor Statistics information
*  @param[in]   aParsedMask       Status of parsed statistics parsed
 * @param[out]  None
 *
 * @return   int32_t Error status BCM_ERR_OK if success
 */
typedef int32_t (* const TunerFnSensorStatsUpdate )(
            CSN_MODULE_StatsType * const (aSensorStats)[CSN_MODULE_EXP_IDX_MAX],
            uint32_t aParsedMask);

/** \struct  TunerModule
    \brief Structure Tuner Modules.
*/
typedef struct _TunerModule {
    TunerGetCfgType getCfgType;                 /**< Get Config Type API          */
    TunerFnInit init;                           /**< Init API                     */
    TunerFnDeinit deinit;                       /**< De-init API                  */
    TunerFnStart start;                         /**< Start API                    */
    TunerFnStop stop;                           /**< Stop API                     */
    TunerFnConfigure configure;                 /**< Configure API                */
    TunerFnMetadataUpdate metadataUpdate;       /**< Metadata update API          */
    TunerFnISP_StatsUpdate ispStatsUpdate;      /**< ISP Statistics update API    */
    TunerFnSensorStatsUpdate sensorStatsUpdate; /**< Sensor Statistics update API */
} TunerModule;

extern const TunerModule gAGC_Module;
extern const TunerModule gAWB_Module;
extern const TunerModule gDenoise_Module;
extern const TunerModule gCC_Module;
extern const TunerModule gDefPix_Module;
extern const TunerModule gSharpen_Module;
extern const TunerModule gBL_Module;
extern const TunerModule gTM_Module;

#endif /* TUNER_MODULE_H */
