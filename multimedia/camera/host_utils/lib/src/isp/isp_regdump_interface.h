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
#ifndef ISP_REGDUMP_INTERFACE_H
#define ISP_REGDUMP_INTERFACE_H

/******************************************************************************
File Name:  isp_regdump_interface.h
Description: This defines interface structure between
             ispRegDump library and ISP offline utility.
******************************************************************************/
#include <isys_types.h>
#include "tuner_config_awb.h"
#include "tuner_config_agc.h"
#include "tuner_config_denoise.h"
#include "tuner_config_cc.h"
#include "tuner_config_defpix.h"
#include "tuner_config_sharpen.h"
#include "tuner_config_bl.h"
#include "tuner_config_tm.h"

/* Macros */
#define OFFSET_MASK      (0xFFFFUL)
#define ISP_MAX_REG      (5120UL)
#define TCB_MAX_DATA     (28UL)
#define GROUPXY_MAX_DATA (20UL)

/* AWB Defines */
#define AWB_MAX_STD_PTS  (16UL)

typedef enum {
   ISP_TUNER_AWB_METHOD_B = 0,
   ISP_TUNER_AWB_METHOD_RED_NORM_CT = 1,
   ISP_TUNER_AWB_METHOD_GREY_WORLD = 2,
   ISP_TUNER_AWB_METHOD_INITIAL = 3,
   ISP_TUNER_AWB_METHOD_FIXED_CAL_GAINS = 4
} AWB_METHOD;

/* ISP Reg Data */
typedef struct {
    uint32_t addr;
    uint32_t data;
} ISPRegData;

/* ISP Config */
/* Always align with 32bit */
typedef struct {
    ISPRegData IspBlockReg[ISP_MAX_REG];
    uint32_t TcbData[TCB_MAX_DATA];
    ISYS_OutPropsType outputProps[ISYS_OUT_IDX_MAX];
    uint32_t outputEnable;
    uint32_t ispVersion;
} ISPConfigData;

typedef struct _AWB_Bayes_T_Params {
    sint16p16_t qPathA;
    sint16p16_t qPathB;
    sint16p16_t qPathC;
    uint0p16_t rMin;
    uint0p16_t rMax; /* Maximum r_nrm of the colour temperature curve */
    uint0p16_t resPara; /* Resolution parallel to the colour temperature curve */
    uint0p16_t resPerp; /* Resolution perpendicular to the colour temperature curve */
    ISYS_PWLPointType ridgePosPts[AWB_MAX_STD_PTS];
    ISYS_PWLCurveType ridgePosFunc; /* Standard deviation of the colour temperature curve in
                              the positive direction (above the curve) in u0p16,
                              by r_nrm in u0p8 */
    ISYS_PWLPointType ridgeNegPts[AWB_MAX_STD_PTS];
    ISYS_PWLCurveType ridgeNegFunc; /* Standard deviation of the colour temperature curve in
                              the positive direction (below the curve) in u0p16,
                              by r_nrm in u0p8 */
    ISYS_PWLPointType stdPosPts[AWB_MAX_STD_PTS];
    ISYS_PWLCurveType stdPosFunc; /* Standard deviation of the colour temperature curve in
                            the positive direction (above the curve) in u0p16,
                            by r_nrm in u0p8 */
    ISYS_PWLPointType stdNegPts[AWB_MAX_STD_PTS];
    ISYS_PWLCurveType stdNegFunc; /* Standard deviation of the colour temperature curve in
                            the positive direction (below the curve) in u0p16,
                            by r_nrm in u0p8 */
    AWB_Ridge_PWL ridgePrior[AWB_MAX_RIDGE_PRIORS]; /* P(T) prior of the ridge - defined
                                                       for two different exposures */
    ISYS_PWLPointType ridgePriorInterpPts[AWB_NUM_EXP_P];
    ISYS_PWLCurveType ridgePriorInterpFunc; /* 0 to use ridge function 0, 256 to use ridge
                                      function 1 etc., by exposure */
} AWB_Bayes_T_Params;

typedef struct _AWB_2D_TexPDF {
    ISYS_PWLPointType wIntPts[AWB_NUM_TEX_P];
    ISYS_PWLCurveType wInt; /* Variation of pdf weight by intensity */
    ISYS_PWLPointType wRNrmPts[AWB_NUM_TEX_P];
    ISYS_PWLCurveType wRNrm; /* variation of pdf weight by r_nrm */
    ISYS_PWLPointType wExpPts[AWB_NUM_TEX_P];
    ISYS_PWLCurveType wExp; /* Variation of pdf weight by exposure */
    uint32_t pdfTex[AWB_TEX_DIM*AWB_TEX_DIM/4]; /* pointer to 256x256 T-format PDF
                                                   texture, s-axis as r_nrm,
                                                   t-axis as b_nrm */
} AWB_2D_TexPDF;

/* AWB tuner configuration */
typedef struct _AWB_DPF_Config {
    uint32_t magic;
    uint32_t version;
    uint32_t isEnabled;
    uint32_t method;
    uint32_t rate;
    uint32_t nRegionsX; /* Number of regions to use along X axis */
    uint32_t nRegionsY; /* Number of regions to use along Y axis */
    uint32_t useSensorAWB;
    ISYS_PWLPointType awbSpeedModExposurePts[AWB_MAX_MOD_SPEED_PTS];
    ISYS_PWLCurveType awbSpeedModExposureFunc;
    uint16_t fastStartupFrames; /* Number of frames upon startup at which to
                                   run AWB at max speed */
    uint0p8_t initialRNrm;  /* Starting point on bayes curve to set gains
                               on 1st frame */
    uint8_t pad1[1];
    ISP_WBParamType initialBias; /* Preferred colour bias for initial CT curve point */
    int32_t initialStartupFrames; /* number of awb updates to keep the initial rNrm for */
    ISYS_PWLPointType   expToRedNormPts[AWB_MAX_CURVE_PTS];
    ISYS_PWLCurveType expToRedNormFunc;
    uint32_t  biasEnable; /* Bias of AWB colour according to colour temperature
                             of result */
    CT_Bias ctBias;
    ISYS_PWLPointType  ctPts[AWB_MAX_CURVE_PTS];   /* Conversion of r/b gain ratio to CT */
    ISYS_PWLCurveType ctFunc; /* Curve to calculate CT from r_nrm */
    uint8p8_t canonSensR, canonSensG, canonSensB;
    uint8p8_t sensorSensR, sensorSensG, sensorSensB;
    AWB_Bayes_T_Params bayesT[AWB_MAX_T_CURVES]; /* Colour temperature curve */
    uint8p8_t bayesCentreWeight;
    uint8p8_t bayesInterCurveStrength;
    uint8_t bayesTCurveN;
    uint8_t bayesInterTPow;
    uint8_t bayes_S_T_PDFsTexN; /* Likelihood prior p.d.f by texture map */
    uint8_t pad3;
    AWB_2D_TexPDF bayes_S_T_PDFsTex[AWB_MAX_S_TEX_PDFS];
    CC_TunerConfig cc;
    uint8_t useLocalCcm;
    uint8_t pad4[3];
} AWB_DPF_Config;

typedef struct {
    AGC_TunerConfig agc;
    AWB_DPF_Config awb;
    Denoise_TunerConfig denoise;
    CC_TunerConfig cc;
    DefPix_TunerConfig defPix;
    Sharpen_TunerConfig sharpen;
    BL_TunerConfig bl;
    TM_TunerConfig tm;
} ISPTunerConfigDataIfc;

typedef struct {
    AGC_TunerConfig agc;
    AWB_TunerConfig awb;
    Denoise_TunerConfig denoise;
    CC_TunerConfig cc;
    DefPix_TunerConfig defPix;
    Sharpen_TunerConfig sharpen;
    BL_TunerConfig bl;
    TM_TunerConfig tm;
} ISPTunerConfigData;

/* ISP DPF Parser API to parse DPF file */
int IDP_Parse(const char *filename);

#endif /* ISP_REGDUMP_INTERFACE_H */
