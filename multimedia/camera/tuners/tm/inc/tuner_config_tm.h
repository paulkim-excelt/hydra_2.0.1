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
#ifndef TUNER_CONFIG_TM_H
#define TUNER_CONFIG_TM_H

/**
    @defgroup grp_tm Tone Mapping
    @ingroup grp_tuners
    @defgroup grp_tm_ifc Interface
    @ingroup grp_tm
    @addtogroup grp_tm_ifc
    @{
    This section contains configuration interface for Tone Mapping tuner.

    @file tuner_config_tm.h
    @brief Configuration structure for Tone Mapping tuner.
    This file exposes Tone Mapping tuner configuration structure.
*/

#include <isys_types.h>

/**
    @brief Magic for Tone Mapping tuner ("ATMT")
*/
#define TUNER_TM_MAGIC    (0x41544d54UL)

/**
    @brief Version for Tone Mapping tuner configuration structure
*/
#define TUNER_TM_VERSION  (1.0f)

/**
    @brief Configuration structure for Tone Mapping tuner
*/
typedef struct _TM_TunerConfig{
    uint32_t magic; /**< @brief Magic for Tone Mapping tuner */
    uint32_t version; /**< @brief Version for Tone Mapping tuner structure */
    uint32_t isEnabled; /**< @brief Enable status of Tone Mapping tuner */
    uint32_t rate; /**< @brief Rate by which Tone Mapping tuner runs */

    uint32_t useSensorStats; /**< @brief Whether to use sensor statistics or not */
    uint32_t useGamma; /**< @brief Whether to use ISP gamma block for TM */
    uint8p8_t tolPercent; /**< @brief Tolerance percentage */
    uint8p8_t dynRangeScale; /**< @brief Dynamic range scale */
    uint16_t ldrBin; /**< @brief LDR bin in histogram */
    uint16_t ldrPoint; /**< @brief LDR point */
    uint16_t binMap[32]; /**< @brief Bin map */
} TM_TunerConfig;

#endif /* TUNER_CONFIG_TM_H */
/** @} */
