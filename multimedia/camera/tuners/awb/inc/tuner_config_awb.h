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
#ifndef TUNER_CONFIG_AWB_H
#define TUNER_CONFIG_AWB_H

/**
    @defgroup grp_awb AWB
    @ingroup grp_tuners
    @defgroup grp_awb_ifc Interface
    @ingroup grp_awb
    @addtogroup grp_awb_ifc
    @{
    This section contains configuration interface for Automatic White Balance (AWB) tuner.

    @file tuner_config_awb.h
    @brief Configuration structure for AWB tuner.
    This file exposes AWB tuner configuration structure.
*/

#include <pwl.h>
#include <isp_params.h>

/**
    @brief Magic for AWB tuner ("AWBT")
*/
#define TUNER_AWB_MAGIC   (0x41574254UL)

/**
    @brief Version for AWB configuration structure
*/
#define TUNER_AWB_VERSION (1.0f)

/**
    @brief Maximum number of AWB speed points
*/
#define AWB_MAX_MOD_SPEED_PTS       (15UL)

/**
    @brief Maximum number of curve points
*/
#define AWB_MAX_CURVE_PTS           (15UL)

/**
    @brief Maximum number of CT bias points
*/
#define AWB_MAX_CT_BIAS_PTS         (20UL)

/**
    @brief Maximum number of curves supported
*/
#define AWB_MAX_T_CURVES            (1UL)

/**
    @brief Maximum number of PDFs supported
*/
#define AWB_MAX_S_TEX_PDFS          (3UL)

/**
    @brief Maximum number of Texture points
*/
#define AWB_NUM_TEX_P               (8UL)

/**
    @brief Maximum number of exposure points
*/
#define AWB_NUM_EXP_P               (16UL)

/**
    @brief Maximum number of ridge prior points
*/
#define AWB_MAX_RIDGE_PRIOR_PTS     (16UL)

/**
    @brief Maximum number of ridge priors
*/
#define AWB_MAX_RIDGE_PRIORS        (6UL)

/**
    @brief AWB texture dimension
*/
#define AWB_TEX_DIM                 (64UL)

/**
    @brief Maximum number of points per curve
*/
#define MAX_T_POINTS_PER_CURVE      (512UL)

/**
    @brief Maximum number of CCMs supported
*/
#define AWB_MAKESUM_MAX_CCMS        (MAX_T_POINTS_PER_CURVE)

/**
    @name AWB method
    @{
    @brief AWB method types
*/
#define AWB_METHOD_B                 (0x4d42594eUL) /**< @brief Bayesian        */
#define AWB_METHOD_RED_NORM_CT       (0x4d524e43UL) /**< @brief Red norm CT     */
#define AWB_METHOD_GREY_WORLD        (0x4d475257UL) /**< @brief Grey-World      */
#define AWB_METHOD_INITIAL           (0x4d494e49UL) /**< @brief Initial Gains   */
#define AWB_METHOD_FIXED_CAL_GAINS   (0x4d464347UL) /**< @brief Fixed cal gains */
/** @} */

/**
    @brief CT Bias channel
 */
typedef struct _CT_Bias_Channel {
    ISYS_PWLCurveType  func; /**< @brief Bias: x is color temp  y is bias */
    ISYS_PWLPointType    pts[AWB_MAX_CT_BIAS_PTS]; /**< @brief PWL points */
} CT_Bias_Channel;

/**
    @brief CT Bias
 */
typedef struct _CT_Bias {
   CT_Bias_Channel r; /**< @brief Red channel bias */
   CT_Bias_Channel b; /**< @brief Blue channel bias */
} CT_Bias;

/**
    @brief AWB Ridge PWL
 */
typedef struct _AWB_RIDGE_PWL {
   ISYS_PWLCurveType func; /**< @brief PWL function */
   ISYS_PWLPointType pts[AWB_MAX_RIDGE_PRIOR_PTS]; /**< @brief PWL points */
} AWB_Ridge_PWL;

/**
    @brief AWB Bayesian Temperature curve configuration
 */
typedef struct _AWB_Bayes_T_Config {
    AWB_Ridge_PWL ridgePrior[AWB_MAX_RIDGE_PRIORS]; /**< Prior of the ridge defined
                                                         for two different exposures */
    ISYS_PWLPointType ridgePriorInterpPts[AWB_NUM_EXP_P]; /**< @brief PWL points for ridge prior */
    ISYS_PWLCurveType ridgePriorInterpFunc; /**< @brief PWL Function for ridge prior */
} AWB_Bayes_T_Config;

/**
    @brief AWB PDF texture image
 */
typedef struct _AWB_2D_TexPDFImage {
    ISYS_PWLPointType wExpPts[AWB_NUM_TEX_P]; /**< @brief Exposure weight points */
    ISYS_PWLCurveType wExp; /**< @brief PWL function for exposure (variation of PDF
                        weight by exposure). */
    uint8_t pdfTex[AWB_TEX_DIM*AWB_TEX_DIM]; /**< @brief Pointer to 256x256 T-format PDF
                                                texture, s-axis as rNrm,
                                                t-axis as bNrm */
} AWB_2D_TexPDFImage;

/**
    @brief AWB pre-calculated Temperature curve configuration
 */
typedef struct {
    uint0p16_t rNrm[MAX_T_POINTS_PER_CURVE]; /**< @brief r/(r+g+b) for each T */
    uint0p16_t bNrm[MAX_T_POINTS_PER_CURVE]; /**< @brief b/(r+g+b) for each T */
    uint32_t numTPoints; /**< @brief Number of temperature points */
    uint8_t pTPrior[MAX_T_POINTS_PER_CURVE]; /**< @brief P(T) prior perpendicular to the
                                                color temperature curve */
    uint8_t pdfWeightsT[MAX_T_POINTS_PER_CURVE][AWB_MAX_S_TEX_PDFS]; /**< @brief 2D PDF weights */
    sint4p12_t mhTCCM[MAX_T_POINTS_PER_CURVE][3][3]; /**< @brief Color correction
                                                                 matrix for temperature. */
} AWB_Bayes_T_Precalc;

/**
    @brief AWB pre-calculated data structure for Bayesian method
 */
typedef struct _AWB_Precalc {
    AWB_Bayes_T_Precalc t[AWB_MAX_T_CURVES]; /**< @brief Pre-calculated data for
                                                         the given temperature curve */
    uint8p8_t partToPartGainR; /**< @brief Part to part gain for Red */
    uint8p8_t partToPartGainB; /**< @brief Part to part gain for Blue */
    uint16_t canonSensR; /**< @brief Canonical color sensitivity for Red */
    uint16_t canonSensG; /**< @brief Canonical color sensitivity for Green */
    uint16_t canonSensB; /**< @brief Canonical color sensitivity for Blue */
    uint16_t sensorSensR; /**< @brief Sensor color sensitivity for Red */
    uint16_t sensorSensG; /**< @brief Sensor color sensitivity for Green */
    uint16_t sensorSensB; /**< @brief Sensor color sensitivity for Blue */
    uint8_t pdfWeightsInt[AWB_MAX_S_TEX_PDFS][128]; /**< @brief Weight due to patch Brightness */
} AWB_Precalc;

/**
    @brief AWB Tuner configuration structure
 */
typedef struct _AWB_TunerConfig {
    uint32_t magic; /**< @brief Magic number */
    uint32_t version; /**< @brief Version for AWB configuration structure */
    uint32_t isEnabled; /**< @brief Enable status of AWB tuner */
    uint32_t method; /**< @brief The method by which AWB should be performed */
    uint32_t rate; /**< @brief Rate by which AWB tuner runs */
    uint32_t nRegionsX; /**< @brief Number of regions to use along X axis */
    uint32_t nRegionsY; /**< @brief Number of regions to use along Y axis */
    uint32_t useSensorAWB; /**< @brief Whether to use sensor WB or not */
    ISYS_PWLPointType awbSpeedModExposurePts[AWB_MAX_MOD_SPEED_PTS]; /**< @brief Adjust the speed of
                               adaptation based on the change in exposure between frames.
                               X measures the change in exposure (bigger is a greater change)
                               and y multiplies the AWB speed supplied by the application */
    ISYS_PWLCurveType awbSpeedModExposureFunc; /**< @brief PWL function for speed-mod-exposure points */
    uint16_t fastStartupFrames; /**< @brief Number of frames upon startup
                                   at which to run AWB at max speed */
    uint0p8_t initialRNrm; /**< @brief Starting point on Bayesian curve to
                               set gains on 1st frame */
    ISP_WBParamType initialBias; /**< @brief Preferred color bias for initial
                                CT curve point */
    int32_t initialStartupFrames; /**< @brief Number of AWB updates to keep
                                     the initial r_nrm for */
    ISYS_PWLPointType   expToRedNormPts[AWB_MAX_CURVE_PTS]; /**< @brief PWL points to exposure to red
                                                             norms */
    ISYS_PWLCurveType expToRedNormFunc; /**< @brief PWL function for
                                           exposure to red norms */
    uint32_t  biasEnable; /**< @brief Bias of AWB color according to color temperature
                             of result */
    CT_Bias ctBias; /**< @brief Color temperature bias */
    ISYS_PWLPointType  ctPts[AWB_MAX_CURVE_PTS]; /**< @brief Conversion of r/b gain ratio to CT */
    ISYS_PWLCurveType ctFunc; /**< @brief Curve to calculate CT from r_nrm */
    AWB_Bayes_T_Config bayesT[AWB_MAX_T_CURVES]; /**< @brief Color temperature curve */
    uint8p8_t bayesCentreWeight; /**< @brief Additional weight to add to the center of
                                             the image */
    uint8p8_t bayesInterCurveStrength; /**< @brief Strength with which to weight the most
                    probable curve. The higher this number, the more heavily weighted that
                    the apparently most probable color temperature curve is given. Larger
                    values therefore make a more definite and less stable choice. */
    uint8_t bayesTCurveN; /**< @brief Number of color temperature curves to use */
    uint8_t bayesInterTPow; /**< @brief The integer power n to which the posterior
                            probability for each lamp is raised before means averaging */
    uint8_t bayes_S_T_PDFsTexN; /**< @brief Likelihood prior p.d.f by texture map */
    uint8_t pad; /**< @brief padding */
    AWB_2D_TexPDFImage bayes_S_T_PDFsTex[AWB_MAX_S_TEX_PDFS]; /**< @brief Two dimension
                                                                       texture for PDF */
    AWB_Precalc precalcData; /**< @brief Pre-Calculated data */
} AWB_TunerConfig;

#endif /* TUNER_CONFIG_AWB_H */
/** @} */
