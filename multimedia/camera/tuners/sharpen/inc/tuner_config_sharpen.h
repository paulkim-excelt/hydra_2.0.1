/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
#ifndef TUNER_CONFIG_SHARPEN_H
#define TUNER_CONFIG_SHARPEN_H

/**
    @defgroup grp_sharpen Sharpening
    @ingroup grp_tuners
    @defgroup grp_sharpen_ifc Interface
    @ingroup grp_sharpen
    @addtogroup grp_sharpen_ifc
    @{
    This section contains configuration interface for Sharpening tuner.

    @file tuner_config_sharpen.h
    @brief Configuration structure for Sharpening tuner.
    This file exposes Sharpening tuner configuration structure.
*/

#include <pwl.h>

/**
    @brief Magic for Sharpening tuner ("SHAT")
*/
#define TUNER_SHARPEN_MAGIC   (0x53484154UL)

/**
    @brief Version for Sharpening tuner configuration structure
*/
#define TUNER_SHARPEN_VERSION (1.0f)

/**
    @brief Maximum number of Sharpening tuner PWL points
*/
#define SHARPEN_MAXPTS        (12UL)

/**
    @brief Interpolation PWL for Sharpening tuner
*/
typedef struct _Sharpen_PWL {
    ISYS_PWLCurveType func; /**< @brief PWL function */
    ISYS_PWLPointType pts[SHARPEN_MAXPTS]; /**< @brief PWL points */
} Sharpen_PWL;

/**
    @brief Response structure for Sharpening tuner
*/
typedef struct _Sharpen_Responses {
   Sharpen_PWL resp; /**< @brief Response PWL function*/
   Sharpen_PWL resp1; /**< @brief Second response PWL function*/
} Sharpen_Responses;

/**
    @brief Configuration data structure for Sharpening tuner
*/
typedef struct _Sharpen_TunerConfig {
    uint32_t magic; /**< @brief Magic for Sharpening tuner */
    uint32_t version; /**< @brief Version for Sharpening tuner structure */
    uint32_t isEnabled; /**< @brief Enable status of Sharpening tuner */
    uint32_t rate; /**< @brief Rate by which Sharpening tuner runs */

    Sharpen_PWL posGain; /**< @brief Pos-Gain PWL function */
    Sharpen_PWL relPosGain; /**< @brief Rel-Pos-Gain PWL function */
    Sharpen_PWL vGain; /**< @brief v-Gain PWL function */
    Sharpen_PWL respGain; /**< @brief Response-Gain PWL function */
    Sharpen_Responses response[SHARPEN_MAXPTS]; /**< @brief Sharpening Response configurations */
    Sharpen_PWL responseInterp; /**< @brief Response interpolation function  */
    Sharpen_PWL isotropicStrength; /**< @brief Isotropic strength */

    uint8p8_t respGainMin; /**< @brief Minimum response gain */
    uint8p8_t respGainMax; /**< @brief Maximum response gain */

    uint8_t avgG_shift; /**< @brief Average G shift */
    uint8_t pad[3]; /**< @brief Padding*/
} Sharpen_TunerConfig;

#endif /* TUNER_CONFIG_SHARPEN_H */
/** @} */
