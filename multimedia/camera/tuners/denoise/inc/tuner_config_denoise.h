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
#ifndef TUNER_CONFIG_DENOISE_H
#define TUNER_CONFIG_DENOISE_H

/**
    @defgroup grp_denoise Denoise
    @ingroup grp_tuners
    @defgroup grp_denoise_ifc Interface
    @ingroup grp_denoise
    @addtogroup grp_denoise_ifc
    @{
    This section contains configuration interface for Denoise tuner.

    @file tuner_config_denoise.h
    @brief Configuration structure for Denoise tuner.
    This file exposes Denoise tuner configuration structure.
*/

#include <pwl.h>
#include <isp_params.h>

/**
    @brief Magic for Denoise tuner ("DNST")
*/
#define TUNER_DENOISE_MAGIC   (0x444e5354UL)

/**
    @brief Version for Denoise tuner configuration structure
*/
#define TUNER_DENOISE_VERSION (1.0f)

/**
    @brief Maximum number of Denoise PWL points
*/
#define DENOISE_NUM_PTS       (8UL)

/**
    @brief Maximum number of curves
*/
#define DENOISE_MAX_HW_CURVES (4UL)

/**
    @brief Interpolation PWL for Denoise tuner
*/
typedef struct _Denoise_PWL {
   ISYS_PWLPointType pts[DENOISE_NUM_PTS]; /**< @brief PWL points */
   ISYS_PWLCurveType func; /**< @brief PWL function */
} Denoise_PWL;

/**
    @brief Configuration structure for Color Denoise tuner
*/
typedef struct _Denoise_ColourHW_Config {
   Denoise_PWL redStrength; /**< @brief Red strength PWL */
   Denoise_PWL blueStrength; /**< @brief Blue strength PWL */
   ISP_ColourDenoiseParamType cdnParams; /**< @brief ISP CDN parameters */
} Denoise_ColourHW_Config;

/**
    @brief Curve structure for Denoise tuner
*/
typedef struct _Denoise_Curve {
   uint16_t abscissae[12]; /**< @brief Abscissae points */
   uint16_t ordinates[12]; /**< @brief Ordinates points */
} Denoise_Curve;

/**
    @brief Configuration structure for Bayer Denoise tuner
*/
typedef struct _Denoise_BayerHW_Config {
   Denoise_Curve curve[DENOISE_MAX_HW_CURVES]; /**< @brief Bayer Denoise curve */
   Denoise_PWL interp; /**< @brief Interpolation PWL */

   uint8p8_t digGainReduction; /**< @brief Digital gain reduction */
   uint8_t pad2[2]; /**< @brief Padding */
   uint4p4_t grGain; /**< @brief Gain for GR */
   uint4p4_t gbGain; /**< @brief Gain for GB */
   uint4p4_t rGain; /**< @brief Gain for R */
   uint4p4_t bGain; /**< @brief Gain for B */
   Denoise_PWL grStr; /**< @brief GR strength */
   Denoise_PWL gbStr; /**< @brief GB strength */
   Denoise_PWL rStr; /**< @brief R strength */
   Denoise_PWL bStr; /**< @brief B strength */
   uint8_t mask[49]; /**< @brief Mask */
   uint8_t pad3[3]; /**< @brief Padding */
} Denoise_BayerHW_Config;

/**
    @brief Configuration structure for Denoise tuner
*/
typedef struct _Denoise_TunerConfig{
    uint32_t magic; /**< @brief Magic for Denoise tuner */
    uint32_t version; /**< @brief Version for Denoise tuner structure */
    uint32_t isEnabled; /**< @brief Enable status of Denoise tuner */
    uint32_t rate; /**< @brief Rate by which Denoise tuner runs */

    Denoise_BayerHW_Config bayerHW; /**< @brief Bayer Denoise tuner configuration */
    Denoise_ColourHW_Config colourHW; /**< @brief Color Denoise tuner configuration */
} Denoise_TunerConfig;

#endif /* TUNER_CONFIG_DENOISE_H */
/** @} */
