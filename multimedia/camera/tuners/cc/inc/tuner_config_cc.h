/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
#ifndef TUNER_CONFIG_CC_H
#define TUNER_CONFIG_CC_H

/**
    @defgroup grp_cc Color Correction
    @ingroup grp_tuners
    @defgroup grp_cc_ifc Interface
    @ingroup grp_cc
    @addtogroup grp_cc_ifc
    @{
    This section contains configuration interface for Color Correction tuner.

    @file tuner_config_cc.h
    @brief Configuration structure for Color Correction tuner.
    This file exposes Color Correction tuner config structure.
*/

#include <pwl.h>

/**
    @brief Magic for Color Correction tuner ("CCMT")
*/
#define TUNER_CC_MAGIC   (0x43434d54UL)

/**
    @brief Version for Color Correction tuner configuration structure
*/
#define TUNER_CC_VERSION (1.0f)

/**
    @brief Maximum number CC tables
*/
#define CC_MAX_NUM_TABLES (7UL)
/**
    @brief Maximum number of CC PWL points
*/
#define CC_MAX_PWL_PTS    (16UL)

/**
    @brief Color Correction table
*/
typedef struct _CC_Table {
   int16_t matrix[3][3]; /**< @brief CC Matrix */
   uint8_t pad[2]; /**< @brief Padding */
   int32_t offsets[3]; /**< @brief Offsets  */
   int32_t baseGreenness; /**< @brief Base greenness  */
} CC_Table;

/**
    @brief Color Correction tuner configuration data structure
*/
typedef struct _CC_TunerConfig {
    uint32_t magic; /**< @brief Magic for Color Correction tuner */
    uint32_t version; /**< @brief Version for Color Correction tuner structure */
    uint32_t isEnabled; /**< @brief Enable status of Color Correction tuner */
    uint32_t rate; /**< @brief Rate by which Color Correction tuner runs */

    uint32_t ispBPP; /**< @brief ISP bits per pixel */
    CC_Table table[CC_MAX_NUM_TABLES]; /**< @brief CC Table  */
    ISYS_PWLPointType interpPts[CC_MAX_PWL_PTS]; /**< @brief Interpolation PWL points  */
    ISYS_PWLCurveType interpFunc; /**< @brief Interpolation PWL function */
    ISYS_PWLPointType saturationPts[CC_MAX_PWL_PTS]; /**< @brief Saturation PWL points */
    ISYS_PWLCurveType saturationFunc; /**< @brief Saturation PWL function */
    int16_t greenMatrix[3][3]; /**< @brief Green Matrix */
    uint8_t pad[2]; /**< @brief Padding */
} CC_TunerConfig;

#endif /* TUNER_CONFIG_CC_H */
/** @} */
