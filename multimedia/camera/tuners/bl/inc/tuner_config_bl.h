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
#ifndef TUNER_CONFIG_BL_H
#define TUNER_CONFIG_BL_H

/**
    @defgroup grp_bl Black Level
    @ingroup grp_tuners
    @defgroup grp_bl_ifc Interface
    @ingroup grp_bl
    @addtogroup grp_bl_ifc
    @{
    This section contains configuration interface for Black Level tuner.

    @file tuner_config_bl.h
    @brief Configuration structure for Black Level tuner.
    This file exposes Black Level tuner configuration structure.
*/

#include <pwl.h>

/**
    @brief Magic for Black Level tuner ("BKLT")
*/
#define TUNER_BL_MAGIC   (0x424b4c54UL)

/**
    @brief Version for Black Level tuner configuration structure
*/
#define TUNER_BL_VERSION (1.0f)

/**
    @brief Maximum number of AG BL points
*/
#define BL_MAX_AG_BL_PTS (16UL)

/**
    @brief Black Level tuner configuration data structure
*/
typedef struct _BL_TunerConfig {
    uint32_t magic; /**< @brief Magic for Black Level tuner */
    uint32_t version; /**< @brief Version for Black Level tuner structure */
    uint32_t isEnabled; /**< @brief Enable status of Black Level tuner */
    uint32_t rate; /**< @brief Rate by which Black Level tuner runs */

    ISYS_PWLPointType agBL_pts[4][BL_MAX_AG_BL_PTS]; /**< @brief Analog Gain PWL points */
    ISYS_PWLCurveType agBL_rFunc; /**< @brief Analog Gain PWL function for “R” Pixels  */
    ISYS_PWLCurveType agBL_grFunc; /**< @brief Analog Gain PWL function for “GR” Pixels  */
    ISYS_PWLCurveType agBL_bFunc; /**< @brief Analog Gain PWL function for “B” Pixels  */
    ISYS_PWLCurveType agBL_gbFunc; /**< @brief Analog Gain PWL function for “GB” Pixels  */
} BL_TunerConfig;

#endif /* TUNER_CONFIG_BL_H */
/** @} */
