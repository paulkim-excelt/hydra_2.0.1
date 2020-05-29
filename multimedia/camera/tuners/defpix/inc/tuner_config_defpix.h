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
#ifndef TUNER_CONFIG_DEFPIX_H
#define TUNER_CONFIG_DEFPIX_H

/**
    @defgroup grp_defpix Defective Pixel Correction
    @ingroup grp_tuners
    @defgroup grp_defpix_ifc Interface
    @ingroup grp_defpix
    @addtogroup grp_defpix_ifc
    @{
    This section contains configuration interface for Defective Pixel Correction tuner.

    @file tuner_config_defpix.h
    @brief Configuration structure for Defective Pixel Correction tuner.
    This file exposes Defective Pixel Correction tuner configuration structure.
*/

#include <pwl.h>

/**
    @brief Magic for Defective Pixel Correction tuner ("ADPT")
*/
#define TUNER_DEFPIX_MAGIC   (0x41445054UL)

/**
    @brief Version for Defective Pixel Correction tuner configuration structure
*/
#define TUNER_DEFPIX_VERSION (1.0f)

/**
    @brief Maximum number of Defective Pixel Correction PWL points
*/
#define DEFPIX_MAXPTS (12UL)

/**
    @brief PWL structure for Defective Pixel Correction tuner.
*/
typedef struct _DefPix_PWL {
   ISYS_PWLCurveType func; /**< @brief PWL Function */
   ISYS_PWLPointType pts[DEFPIX_MAXPTS]; /**< @brief PWL points */
} DefPix_PWL;

/**
    @brief Defective Pixel Correction tuner configuration data structure
*/
typedef struct _DefPix_TunerConfig {
    uint32_t magic; /**< @brief Magic for Defective Pixel Correction tuner */
    uint32_t version; /**< @brief Version for Defective Pixel Correction tuner structure */
    uint32_t isEnabled; /**< @brief Enable status of Defective Pixel Correction tuner */
    uint32_t rate; /**< @brief Rate by which Defective Pixel Correction tuner runs */

    DefPix_PWL scaleHi[3]; /**< @brief Scale-Hi PWL function */
    DefPix_PWL scaleLo[3]; /**< @brief Scale-Lo PWL function */
    DefPix_PWL offsetHi[3]; /**< @brief Offset-Hi PWL function */
    DefPix_PWL offsetLo[3]; /**< @brief Offset-Lo PWL function */

    DefPix_PWL replaceIdxLo; /**< @brief Replace Index-Lo PWL function */
    DefPix_PWL replaceIdxHi; /**< @brief Replace Index-Hi PWL function */
    DefPix_PWL replaceScaleLo; /**< @brief Replace Scale-Lo PWL function */
    DefPix_PWL replaceScaleHi; /**< @brief Replace Scale-Hi PWL function */
} DefPix_TunerConfig;

#endif /* TUNER_CONFIG_DEFPIX_H */
/** @} */
