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
#ifndef TUNER_CONFIG_AGC_H
#define TUNER_CONFIG_AGC_H

/**
    @defgroup grp_agc AGC
    @ingroup grp_tuners
    @defgroup grp_agc_ifc Interface
    @ingroup grp_agc
    @addtogroup grp_agc_ifc
    @{
    This section contains configuration interface for Automatic Gain Control (AGC) tuner.

    @file tuner_config_agc.h
    @brief Configuration structure for AGC tuner.
    This file exposes AGC tuner configuration structure.
*/

#include <isys_types.h>
#include <pwl.h>

/**
    @brief Magic for AGC tuner ("AGCT")
*/
#define TUNER_AGC_MAGIC    (0x41474354UL)

/**
    @brief Version for AGC configuration structure
*/
#define TUNER_AGC_VERSION  (1.0f)

/**
    @brief Number of PWL points for AGC PWL
*/
#define ISP_TUNER_BRCM_AGC_PWL_NUM_PTS        (12UL)

/**
    @brief Maximum number of AGC exposure statges
*/
#define ISP_TUNER_BRCM_AGC_MAX_NUM_EXP_STAGES (10UL)

/**
    @name AGC Metering Mode
    @{
    @brief AGC Metering mode types
*/
typedef uint32_t AGC_MeteringMode; /**< @brief typedef for AGC Metering mode */
#define AGC_METER_NORMAL    (0UL)  /**< @brief Normal metering mode */
#define AGC_METER_HIGHLIGHT (1UL)  /**< @brief Highlight metering mode */
#define AGC_METER_HDR       (2UL)  /**< @brief HDR metering mode */
#define AGC_MAX_METER_MODE  (3UL)  /**< @brief Maximum number of metering modes */
/** @} */

/**
    @brief PWL data structure for AGC
*/
typedef struct _AGC_PWL {
   ISYS_PWLCurveType func; /**< @brief PWL function */
   ISYS_PWLPointType pts[ISP_TUNER_BRCM_AGC_PWL_NUM_PTS]; /**< @brief PWL Points */
} AGC_PWL;

/**
    @brief Calibration of exposure ranges for different modes
*/
typedef struct _AGC_ExposureCalc {
   int32_t mode; /**< @brief Not used by current algorithm */
   uint8_t numStages; /**< @brief Number of exposure stages */
   uint8_t pad1[3]; /**< @brief Padding */
   uint32_t shutterMin; /**< @brief Minimum shutter value */
   uint32_t shutterMax[ISP_TUNER_BRCM_AGC_MAX_NUM_EXP_STAGES]; /**< @brief Maximum shutter value */
   uint8p8_t agMin; /**< @brief The lowest possible analogue gain value */
   uint8p8_t agMax[ISP_TUNER_BRCM_AGC_MAX_NUM_EXP_STAGES]; /**< @brief The highest possible
                                                                       analogue gain value */
   uint8p8_t apertureMin; /**< @brief Minimum aperture value */
   uint8p8_t apertureMax[ISP_TUNER_BRCM_AGC_MAX_NUM_EXP_STAGES]; /**< @brief Maximum aperture value */
   uint8p8_t maxGainProduct; /**< @brief The product of analogue and digital gain may not exceed the
                                         max gain product */
   uint8_t pad2[2]; /**< @brief Padding */
   AGC_PWL yTarget; /**< @brief Not used by current algorithm */
} AGC_ExposureCalc;

/**
    @brief AGC HDR Metering configurations
*/
typedef struct _AGC_HDR_MeteringConfig {
    uint16_t   ldrBin; /**< @brief LDR bin in histogram  */
    uint8p8_t  ldrTgtVal; /**< @brief LDR target value  */
    uint0p16_t ldrPixPct; /**< @brief LDR pixel percentage  */
    uint16_t   ldrHistScale; /**< @brief LDR histogram scale  */
    uint16_t   hdrNumSatPix; /**< @brief HDR number of saturated pixels  */
    uint8p8_t  hdrFiltGain; /**< @brief HDR filter gain  */
} AGC_HDR_MeteringConfig;

/**
    @brief AGC Configuration
*/
typedef struct _AGC_TunerConfig{
    uint32_t magic;  /**< @brief Magic for AGC tuner */
    uint32_t version; /**< @brief Version for AGC tuner structure */
    uint32_t isEnabled; /**< @brief Enable status of AGC tuner */
    uint32_t rate; /**< @brief Rate by which AGC tuner runs */

    uint32_t ispBPP; /**< @brief ISP bits per pixel */
    uint32_t useAWB; /**< @brief use AWB gains in AGC */
    uint8p8_t gainForISO100; /**< @brief The gain that must be applied to the
                                         sensor output to achieve ISO 100
                                         sensitivity (analogue * digital gain).
                                         Calibration item, entirely dependent
                                         on the camera; should be specified by
                                         module manufacturer. */
    uint0p16_t subjectPropotion; /**< @brief Size of the central region of
                                             the image.*/
    uint8p8_t maxGain; /**< @brief  Maximum allowed change in gain from one
                                    frame to another.*/
    uint8p8_t minGain; /**< @brief  Minimum allowed change in gain from one
                                    frame to another.*/
    uint8p8_t digitalGainThreshold; /**< @brief  Not used by current algorithm.  */
    uint0p16_t bottomPixelProportion; /**< @brief  Not used by current algorithm. */
    uint0p16_t bottomHistogramProportion; /**< @brief  Not used by current algorithm. */
    uint8p8_t outerWeight; /**< @brief  Weight of the outer region histogram when
                                        added to the center region. */
    AGC_PWL yTarget; /**< @brief  (Pre-gamma) target for Y value against exposure. */
    uint8p8_t yTargetScale; /**< @brief  Scales the output of the PWL by a
                                         constant factor (for making quick
                                         adjustments to the brightness).*/
    uint0p16_t yCoeffs[3]; /**< @brief  Coefficients for obtaining Y from R, G and B. */
    uint0p16_t yCentreWeight; /**< @brief  Weight applied to the central Y value. */
    AGC_MeteringMode meteringMode; /**< @brief  Metering mode as defined in MeteringMode. */
    AGC_PWL hiDynamicRange; /**< @brief For highlight metering, the place in the dynamic range
                                        below which max_saturated_highlight pixels must lie. */
    uint0p16_t maxSaturatedHighlight; /**< @brief  For highlight metering, the proportion
                                                    of pixels that must lie below
                                                    hi_dynamic_range. */
    uint8p8_t expLoTh; /**< @brief  Only change the camera’s exposure if
                                    changes be less than this factor.*/
    uint8p8_t expHiTh; /**< @brief  Only change the camera’s exposure if
                                    changes be larger than this factor.*/
    AGC_ExposureCalc expMode; /**< @brief  Exposure modes specify the allowed
                                           permutations of exposure, gain and
                                           aperture in a multi-stage exposure
                                           system. */
    uint8_t frameQuantisation; /**< @brief  The AGC to update the camera every
                                            N Frames. */
    uint16_t fastStartupFrames; /**< @brief  Allows rapid change of AGC in the
                                             viewfinder at start-up. */
    uint8_t minGainUnity; /**< @brief  Minimum gain to avoid any risk that the
                                       algorithm decides it wants a digital gain
                                       less than 1.0*/
    uint8p8_t agcSpeedVideoUp; /**< @brief  The speed of change of exposure,
                                            when the exposure is increasing. */
    uint8p8_t agcSpeedVideoDown; /**< @brief  The speed of change of exposure,
                                              when the exposure is decreasing.*/
    uint16_t droppedStartupFrames; /**< @brief  Not used by current algorithm. */
    uint8_t minGainUnityStartupDisable; /**< @brief  Allow gain < 1x for startup frames */
    uint32_t group0NumRegion; /**< @brief  Number of regions in Group 0  */
    uint32_t flickerAvoidancePeriod; /**< @brief  Exposure will always be
                                                  arranged to be a multiple
                                                  of this time in microseconds
                                                  (if non-zero). */
    uint32_t useDigitalGain; /**< @brief  Whether to use ISP digital gain or not.*/
    AGC_HDR_MeteringConfig hdrM_Config; /**< @brief  HDR metering configuration.*/
} AGC_TunerConfig;

#endif /* TUNER_CONFIG_AGC_H */
/** @} */
