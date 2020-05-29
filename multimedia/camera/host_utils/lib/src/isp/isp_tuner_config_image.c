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

/******************************************************************************
File Name:  isp_tuner_config_image.c
Description: This file implements isp tuner image dump.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "cam_mode_image.h"
#include "image_to_array.h"
#include "tuner_module.h"
#include "tuner_config_agc.h"
#include "tuner_config_awb.h"
#include "tuner_config_denoise.h"
#include "tuner_config_cc.h"
#include "tuner_config_defpix.h"
#include "tuner_config_sharpen.h"
#include "tuner_config_tm.h"
#include "isp_regdump_interface.h"

#define DEBUG_ERR printf

#define UINT8P8(x) ((uint8p8_t)((x) * 256.0f))

/* Externs */
/* ISP Tuner Config Data */
extern ISPTunerConfigDataIfc gTunerConfigData;
extern int32_t PreCalc_AWB(AWB_TunerConfig *aAwb);

/* Globals */
static Tuner_Image gTuner_ImageData;

/******************************************************************************
FUNCTION NAME: UpdateAGCData

ARGUMENTS: None

DESCRIPTION: This function updates AGC data.

RETURNS:
void
******************************************************************************/
static void UpdateAGCData(void)
{
    AGC_TunerConfig agc;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
            (gTuner_ImageData.cfgs[index-1].dataOffset +
             gTuner_ImageData.cfgs[index-1].count) : 0;

    /* AGC Data */
    memcpy(&agc, &gTunerConfigData.agc, sizeof(agc));

    /* Update Magic */
    agc.magic = TUNER_AGC_MAGIC;
    agc.version = (UINT8P8(TUNER_AGC_VERSION)) | (sizeof(AGC_TunerConfig) << 16);

    /* AGC Header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_AGC;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(AGC_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &agc, sizeof(AGC_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;
}

/******************************************************************************
FUNCTION NAME: UpdateAWBData

ARGUMENTS: None

DESCRIPTION: This function updates AWB data.

RETURNS:
BCM_ERR_*
******************************************************************************/
static int32_t UpdateAWBData(void)
{
    int32_t ret = BCM_ERR_OK;
    AWB_TunerConfig awb;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
        (gTuner_ImageData.cfgs[index-1].dataOffset +
         gTuner_ImageData.cfgs[index-1].count) : 0;
    int32_t i;

    memset(&awb, 0, sizeof(awb));

    /* AWB Data */
    awb.isEnabled = gTunerConfigData.awb.isEnabled;
    awb.method = gTunerConfigData.awb.method;
    if (0UL == gTunerConfigData.awb.rate) {
        awb.rate = 1UL;
    } else {
        awb.rate = gTunerConfigData.awb.rate;
    }
    awb.nRegionsX = gTunerConfigData.awb.nRegionsX;
    awb.nRegionsY = gTunerConfigData.awb.nRegionsY;

    /* Update useSensorAWB */
    awb.useSensorAWB = gTunerConfigData.awb.useSensorAWB;

    /* AWB Bayesian Configs */
    memcpy(&awb.awbSpeedModExposurePts[0],
            &gTunerConfigData.awb.awbSpeedModExposurePts[0],
            sizeof(awb.awbSpeedModExposurePts));
    memcpy(&awb.awbSpeedModExposureFunc,
            &gTunerConfigData.awb.awbSpeedModExposureFunc,
            sizeof(awb.awbSpeedModExposureFunc));

    /* Number of frames upon startup at which to run AWB at max speed */
    awb.fastStartupFrames =
        gTunerConfigData.awb.fastStartupFrames;

    /* Starting point on bayes curve to set gains on 1st frame */
    awb.initialRNrm = gTunerConfigData.awb.initialRNrm;

    /* Prefferred colour bias for initial CT curve point */
    memcpy(&awb.initialBias,
            &gTunerConfigData.awb.initialBias,
            sizeof(awb.initialBias));

    /* Number of AWB updates to keep the initial r_nrm */
    awb.initialStartupFrames =
        gTunerConfigData.awb.initialStartupFrames;

    /* Conversion of r/b gain ratio to CT */
    memcpy(&awb.ctPts[0],
            &gTunerConfigData.awb.ctPts[0],
            sizeof(awb.ctPts));
    memcpy(&awb.ctFunc,
            &gTunerConfigData.awb.ctFunc,
            sizeof(awb.ctFunc));

    /* Default CT vs exposure for when Bayes and NN both fail */
    memcpy(&awb.expToRedNormPts[0],
            &gTunerConfigData.awb.expToRedNormPts[0],
            sizeof(awb.expToRedNormPts));
    memcpy(&awb.expToRedNormFunc,
            &gTunerConfigData.awb.expToRedNormFunc,
            sizeof(awb.expToRedNormFunc));
    /* Bayes Parameters */
    awb.bayesCentreWeight =
        gTunerConfigData.awb.bayesCentreWeight;

    /* Colour Temperature curve */
    for (i = 0; i < AWB_MAX_T_CURVES; i++) {
        memcpy(&awb.bayesT[i].ridgePrior[0],
                &gTunerConfigData.awb.bayesT[i].ridgePrior[0],
                sizeof(awb.bayesT[i].ridgePrior));
        memcpy(&awb.bayesT[i].ridgePriorInterpPts[0],
                &gTunerConfigData.awb.bayesT[i].ridgePriorInterpPts[0],
                sizeof(awb.bayesT[i].ridgePriorInterpPts));
        memcpy(&awb.bayesT[i].ridgePriorInterpFunc,
                &gTunerConfigData.awb.bayesT[i].ridgePriorInterpFunc,
                sizeof(awb.bayesT[i].ridgePriorInterpFunc));
    }
    awb.bayesTCurveN =
        gTunerConfigData.awb.bayesTCurveN;
    awb.bayesInterCurveStrength =
        gTunerConfigData.awb.bayesInterCurveStrength;
    awb.bayesInterTPow =
        gTunerConfigData.awb.bayesInterTPow;

    /* Likelihood prior p.d.f by texture map */
    awb.bayes_S_T_PDFsTexN =
        gTunerConfigData.awb.bayes_S_T_PDFsTexN;
    for (i = 0; i < AWB_MAX_S_TEX_PDFS; i++) {
        memcpy(&awb.bayes_S_T_PDFsTex[i].wExpPts[0],
                &gTunerConfigData.awb.bayes_S_T_PDFsTex[i].wExpPts[0],
                sizeof(awb.bayes_S_T_PDFsTex[i].wExpPts));
        memcpy(&awb.bayes_S_T_PDFsTex[i].wExp,
                &gTunerConfigData.awb.bayes_S_T_PDFsTex[i].wExp,
                sizeof(awb.bayes_S_T_PDFsTex[i].wExp));
    }

    /* Update Magic */
    awb.magic = TUNER_AWB_MAGIC;
    awb.version = (UINT8P8(TUNER_AWB_VERSION)) | (sizeof(AWB_TunerConfig) << 16);

    /* Map AWB Method */
    switch(gTunerConfigData.awb.method) {
        case ISP_TUNER_AWB_METHOD_B:
            awb.method = AWB_METHOD_B;
            break;
        case ISP_TUNER_AWB_METHOD_RED_NORM_CT:
            awb.method = AWB_METHOD_RED_NORM_CT;
            break;
        case ISP_TUNER_AWB_METHOD_GREY_WORLD:
            awb.method = AWB_METHOD_GREY_WORLD;
            break;
        case ISP_TUNER_AWB_METHOD_INITIAL:
            awb.method = AWB_METHOD_INITIAL;
            break;
        case ISP_TUNER_AWB_METHOD_FIXED_CAL_GAINS:
            awb.method = AWB_METHOD_FIXED_CAL_GAINS;
            break;
        default:
            DEBUG_ERR("%s: AWB method not supported\n", __FUNCTION__);
            break;
    }

    /* PreClac */
    ret = PreCalc_AWB(&awb);
    if (BCM_ERR_OK != ret) {
        if (awb.isEnabled && (AWB_METHOD_B == awb.method)) {
            DEBUG_ERR("%s: PreCalc_AWB failed\n", __FUNCTION__);
            goto exit;
        } else {
            ret = BCM_ERR_OK;
        }
    }

    /* AWB Header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_AWB;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(AWB_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &awb, sizeof(AWB_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;

exit:
    return ret;
}

/******************************************************************************
FUNCTION NAME: UpdateDenoiseData

ARGUMENTS: None

DESCRIPTION: This function updates Denoise data.

RETURNS:
void
******************************************************************************/
static void UpdateDenoiseData(void)
{
    Denoise_TunerConfig denoise;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
            (gTuner_ImageData.cfgs[index-1].dataOffset +
             gTuner_ImageData.cfgs[index-1].count) : 0;

    /* Denoise Data */
    memcpy(&denoise, &gTunerConfigData.denoise, sizeof(denoise));
    /* Update Magic */
    denoise.magic = TUNER_DENOISE_MAGIC;
    denoise.version = (UINT8P8(TUNER_DENOISE_VERSION)) | (sizeof(Denoise_TunerConfig) << 16);

    /* Denooise Header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_DENOISE;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(Denoise_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &denoise, sizeof(Denoise_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;
}

/******************************************************************************
FUNCTION NAME: UpdateCC_Data

ARGUMENTS: None

DESCRIPTION: This function updates CC tuner data.

RETURNS:
void
******************************************************************************/
static void UpdateCC_Data(void)
{
    CC_TunerConfig cc;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
            (gTuner_ImageData.cfgs[index-1].dataOffset +
             gTuner_ImageData.cfgs[index-1].count) : 0;

    /* CC config Data */
    memcpy(&cc, &gTunerConfigData.cc, sizeof(cc));
    /* Update magic */
    cc.magic = TUNER_CC_MAGIC;
    cc.version = (UINT8P8(TUNER_CC_VERSION)) | (sizeof(CC_TunerConfig) << 16);

    /* CC tuner header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_CC;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(CC_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &cc, sizeof(CC_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;
}

/******************************************************************************
FUNCTION NAME: UpdateDefPix_Data

ARGUMENTS: None

DESCRIPTION: This function updates ADPC tuner data.

RETURNS:
void
******************************************************************************/
static void UpdateDefPix_Data(void)
{
    DefPix_TunerConfig defPix;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
            (gTuner_ImageData.cfgs[index-1].dataOffset +
             gTuner_ImageData.cfgs[index-1].count) : 0;

    /* ADPC Tuner config Data */
    memcpy(&defPix, &gTunerConfigData.defPix, sizeof(defPix));
    /* Update magic */
    defPix.magic = TUNER_DEFPIX_MAGIC;
    defPix.version = (UINT8P8(TUNER_DEFPIX_VERSION)) | (sizeof(DefPix_TunerConfig) << 16);

    /* ADPC tuner header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_DEFPIX;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(DefPix_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &defPix, sizeof(DefPix_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;
}

/******************************************************************************
FUNCTION NAME: UpdateSharpen_Data

ARGUMENTS: None

DESCRIPTION: This function updates Sharpening tuner data.

RETURNS:
void
******************************************************************************/
static void UpdateSharpen_Data(void)
{
    Sharpen_TunerConfig sharpen;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
            (gTuner_ImageData.cfgs[index-1].dataOffset +
             gTuner_ImageData.cfgs[index-1].count) : 0;

    /* Sharpen Tuner config Data */
    memcpy(&sharpen, &gTunerConfigData.sharpen, sizeof(sharpen));
    /* Update magic */
    sharpen.magic = TUNER_SHARPEN_MAGIC;
    sharpen.version = (UINT8P8(TUNER_SHARPEN_VERSION)) | (sizeof(Sharpen_TunerConfig) << 16);

    /* Sharpening tuner header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_SHARPEN;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(Sharpen_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &sharpen, sizeof(Sharpen_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;
}

/******************************************************************************
FUNCTION NAME: UpdateBL_Data

ARGUMENTS: None

DESCRIPTION: This function updates Black Level tuner data.

RETURNS:
void
******************************************************************************/
static void UpdateBL_Data(void)
{
    BL_TunerConfig bl;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
            (gTuner_ImageData.cfgs[index-1].dataOffset +
             gTuner_ImageData.cfgs[index-1].count) : 0;

    /* Black level Tuner config Data */
    memcpy(&bl, &gTunerConfigData.bl, sizeof(bl));
    /* Update magic */
    bl.magic = TUNER_BL_MAGIC;
    bl.version = (UINT8P8(TUNER_BL_VERSION)) | (sizeof(BL_TunerConfig) << 16);

    /* Black level tuner header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_BL;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(BL_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &bl, sizeof(BL_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;
}

/******************************************************************************
FUNCTION NAME: UpdateTM_Data

ARGUMENTS: None

DESCRIPTION: This function updates TM tuner data.

RETURNS:
void
******************************************************************************/
static void UpdateTM_Data(void)
{
    TM_TunerConfig tm;
    uint32_t index = gTuner_ImageData.imgHdr.numConfigs;
    uint32_t dataOffset = index ?
            (gTuner_ImageData.cfgs[index-1].dataOffset +
             gTuner_ImageData.cfgs[index-1].count) : 0;

    /* TM Tuner config Data */
    memcpy(&tm, &gTunerConfigData.tm, sizeof(tm));

    /* Update magic */
    tm.magic = TUNER_TM_MAGIC;
    tm.version = (UINT8P8(TUNER_BL_VERSION)) | (sizeof(TM_TunerConfig) << 16);

    /* Black level tuner header */
    gTuner_ImageData.cfgs[index].cfgType = TUNER_CONFIG_TM;
    gTuner_ImageData.cfgs[index].dataOffset = dataOffset;
    gTuner_ImageData.cfgs[index].count = sizeof(TM_TunerConfig);

    /* Copy data */
    memcpy(&gTuner_ImageData.tunerCfgData[dataOffset], &tm, sizeof(TM_TunerConfig));

    /* Update Num Configs */
    gTuner_ImageData.imgHdr.numConfigs++;
}

/******************************************************************************
FUNCTION NAME: ISPTunerConfigWriter

ARGUMENTS: None

DESCRIPTION: This function dumps ISP tuner config image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t ISPTunerConfigWriter(void)
{
    int32_t ret = BCM_ERR_OK;
    FILE *fp = NULL;

    memset(&gTuner_ImageData, 0, sizeof(Tuner_Image));

    fp = fopen("tuner.img", "w");
    if (NULL == fp) {
        DEBUG_ERR("%s: Failed to open file\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (TUNER_MAX_CONFIG_SIZE < sizeof(ISPTunerConfigData)) {
        DEBUG_ERR("%s: Insuf memory for Tuner Config\n", __FUNCTION__);
        DEBUG_ERR("%s: Reqd: %u, Available %d\n", __FUNCTION__,
                                                  sizeof(ISPTunerConfigData),
                                                  (int) TUNER_MAX_CONFIG_SIZE);
        ret = BCM_ERR_NOMEM;
        goto exit;
    }

    /* Common Header */
    gTuner_ImageData.imgHdr.commonHdr.cfgType = CAM_CONFIG_TUNER;
    gTuner_ImageData.imgHdr.commonHdr.cfgMagic = TUNER_MAGIC;

    /* AGC Data Update */
    UpdateAGCData();

    /* AWB Data Update */
    ret = UpdateAWBData();
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: UpdateAWBData failed\n", __FUNCTION__);
        goto exit;
    }

    /* Denoise Data Update */
    UpdateDenoiseData();

    /* CC tuner Data Update */
    UpdateCC_Data();

    /* ADPC tuner Data Update */
    UpdateDefPix_Data();

    /* Sharpening tuner Data Update */
    UpdateSharpen_Data();

    /* Black level tuner Data Update */
    UpdateBL_Data();

    /* TM tuner Data Update */
    UpdateTM_Data();

    /* Write to file */
    fwrite(&gTuner_ImageData, 1, sizeof(gTuner_ImageData), fp);

    /* Write to C file */
    ret = ImageToArray("tuner_image", (unsigned char *)&gTuner_ImageData, sizeof(gTuner_ImageData));
    if (0 !=  ret) {
        DEBUG_ERR("%s: Failed to write  image array\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
    }

exit:
    return ret;
}

/******************************************************************************
FUNCTION NAME: ISPTunerConfigReader

ARGUMENTS: None

DESCRIPTION: This function reads ISP tuner config image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t ISPTunerConfigReader(void *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    Tuner_Image *tunerImg = NULL;
    int32_t i;
    int32_t j;
    int32_t k;
    int32_t l;
    int32_t m;
    AGC_TunerConfig agc;
    AWB_TunerConfig awb;
    Denoise_TunerConfig denoise;
    CC_TunerConfig cc;
    DefPix_TunerConfig defPix;
    Sharpen_TunerConfig sharpen;
    BL_TunerConfig bl;
    TM_TunerConfig tm;
    FILE *fp = NULL;

    memset(&agc, 0, sizeof(AGC_TunerConfig));
    memset(&awb, 0, sizeof(AWB_TunerConfig));
    memset(&denoise, 0, sizeof(Denoise_TunerConfig));
    memset(&cc, 0, sizeof(CC_TunerConfig));
    memset(&defPix, 0, sizeof(DefPix_TunerConfig));
    memset(&sharpen, 0, sizeof(Sharpen_TunerConfig));
    memset(&bl, 0, sizeof(BL_TunerConfig));
    memset(&tm, 0, sizeof(TM_TunerConfig));

    if (NULL == aCfg) {
        DEBUG_ERR("%s: Null config\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    fp = fopen("tuner_config.txt", "w");
    if (NULL == fp) {
        DEBUG_ERR("%s: Failed to open file\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Tuner image */
    tunerImg = (Tuner_Image *)aCfg;

    /* Check Magic */
    if (TUNER_MAGIC != tunerImg->imgHdr.commonHdr.cfgMagic){
        DEBUG_ERR("%s: Wrong Magic in config\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Check number of configs */
    if (tunerImg->imgHdr.numConfigs > TUNER_MAX_CONFIGS){
        DEBUG_ERR("%s: More ISP tuner Configs! \n", __FUNCTION__);
        ret =  BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    for (i = 0UL; (i < tunerImg->imgHdr.numConfigs) && (BCM_ERR_OK == ret); i++) {
        switch(tunerImg->cfgs[i].cfgType){
            case TUNER_CONFIG_AGC:
                memcpy(&agc, &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                        tunerImg->cfgs[i].count);
                if (TUNER_AGC_MAGIC != agc.magic) {
                    DEBUG_ERR("%s: Wrong AGC magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### AGC Data ######\n");
                    fprintf(fp, "AGC.version = 0x%x\n", agc.version);
                    fprintf(fp, "AGC.isEnabled = %d\n", agc.isEnabled);
                    fprintf(fp, "AGC.rate = %d\n", agc.rate);
                    fprintf(fp, "AGC.gainForISO100 = %hu\n",
                                                    agc.gainForISO100);
                    fprintf(fp, "AGC.subjectPropotion = %hu\n",
                                                    agc.subjectPropotion);
                    fprintf(fp, "AGC.maxGain = %hu\n", agc.maxGain);
                    fprintf(fp, "AGC.minGain = %hu\n", agc.minGain);
                    fprintf(fp, "AGC.digitalGainThreshold = %hu\n",
                                                    agc.digitalGainThreshold);
                    fprintf(fp, "AGC.bottomPixelProportion = %hu\n",
                                                    agc.bottomPixelProportion);
                    fprintf(fp, "AGC.bottomHistogramProportion = %hu\n",
                                                    agc.bottomHistogramProportion);
                    fprintf(fp, "AGC.outerWeight = %hu\n", agc.outerWeight);
                    /* Print yTarget */
                    fprintf(fp, "AGC.yTarget.func.pts = %p\n",
                                                    agc.yTarget.func.pts);
                    fprintf(fp, "AGC.yTarget.func.maxNum = %d\n",
                                                    agc.yTarget.func.maxNum);
                    fprintf(fp, "AGC.yTarget.func.currentNum = %d\n",
                                                    agc.yTarget.func.currentNum);
                    for (j=0; j<ISP_TUNER_BRCM_AGC_PWL_NUM_PTS; j++) {
                        fprintf(fp, "AGC.yTarget.pts[%d].x = %d\n",
                                                    j, agc.yTarget.pts[j].x);
                        fprintf(fp, "AGC.yTarget.pts[%d].y = %d\n",
                                                    j, agc.yTarget.pts[j].y);
                    }
                    fprintf(fp, "AGC.yTargetScale = %hu\n", agc.yTargetScale);
                    for (j=0; j<3; j++) {
                        fprintf(fp, "AGC.yCoeffs[%d] = %hu\n", j,
                                                    agc.yCoeffs[j]);
                    }
                    fprintf(fp, "AGC.yCentreWeight = %hu\n",
                                                    agc.yCentreWeight);
                    fprintf(fp, "AGC.meteringMode = %d\n",
                                                    agc.meteringMode);
                    for (j=0; j<ISP_TUNER_BRCM_AGC_PWL_NUM_PTS; j++) {
                        fprintf(fp, "AGC.hiDynamicRange.pts[%d].x = %d\n", j,
                                                    agc.hiDynamicRange.pts[j].x);
                        fprintf(fp, "AGC.hiDynamicRange.pts[%d].y = %d\n", j,
                                                    agc.hiDynamicRange.pts[j].y);
                    }
                    fprintf(fp, "AGC.maxSaturatedHighlight = %hu\n",
                                                    agc.maxSaturatedHighlight);
                    fprintf(fp, "AGC.expLoTh = %hu\n", agc.expLoTh);
                    fprintf(fp, "AGC.expHiTh = %hu\n", agc.expHiTh);
                    /* Print exposure Mode */
                    fprintf(fp, "AGC.expMode.mode = %d\n", agc.expMode.mode);
                    fprintf(fp, "AGC.expMode.numStages = %u\n",
                                                    agc.expMode.numStages);
                    fprintf(fp, "AGC.expMode.shutterMin = %u\n",
                                                    agc.expMode.shutterMin);
                    for (j=0; j<ISP_TUNER_BRCM_AGC_MAX_NUM_EXP_STAGES; j++) {
                        fprintf(fp, "AGC.expMode.shutterMax[%d] = %u\n", j,
                                                    agc.expMode.shutterMax[j]);
                    }
                    fprintf(fp, "AGC.expMode.agMin = %hu\n", agc.expMode.agMin);
                    for (j=0; j<ISP_TUNER_BRCM_AGC_MAX_NUM_EXP_STAGES; j++) {
                        fprintf(fp, "AGC.expMode.agMax[%d] = %hu\n", j,
                                                    agc.expMode.agMax[j]);
                    }
                    fprintf(fp, "AGC.expMode.apertureMin = %hu\n",
                                                    agc.expMode.apertureMin);
                    for (j=0; j<ISP_TUNER_BRCM_AGC_MAX_NUM_EXP_STAGES; j++) {
                        fprintf(fp, "AGC.expMode.apertureMax[%d] = %hu\n", j,
                                                    agc.expMode.apertureMax[j]);
                    }
                    fprintf(fp, "AGC.expMode.maxGainProduct = %hu\n",
                                                    agc.expMode.maxGainProduct);
                    fprintf(fp, "AGC.expMode.yTarget.func.pts = %p\n",
                                                    agc.expMode.yTarget.func.pts);
                    fprintf(fp, "AGC.expMode.yTarget.func.maxNum = %d\n",
                                                    agc.expMode.yTarget.func.maxNum);
                    fprintf(fp, "AGC.expMode.yTarget.func.currentNum = %d\n",
                                                    agc.expMode.yTarget.func.currentNum);
                    for (j=0; j<ISP_TUNER_BRCM_AGC_PWL_NUM_PTS; j++) {
                        fprintf(fp, "AGC.expMode.yTarget.pts[%d].x = %d\n", j,
                                                    agc.expMode.yTarget.pts[j].x);
                        fprintf(fp, "AGC.expMode.yTarget.pts[%d].y = %d\n", j,
                                                    agc.expMode.yTarget.pts[j].y);
                    }
                    fprintf(fp, "AGC.frameQuantisation = %u\n",
                                                    agc.frameQuantisation);
                    fprintf(fp, "AGC.fastStartupFrames = %hu\n",
                                                    agc.fastStartupFrames);
                    fprintf(fp, "AGC.minGainUnity = %u\n",
                                                    agc.minGainUnity);
                    fprintf(fp, "AGC.agcSpeedVideoUp = %hu\n",
                                                    agc.agcSpeedVideoUp);
                    fprintf(fp, "AGC.agcSpeedVideoDown = %hu\n",
                                                    agc.agcSpeedVideoDown);
                    fprintf(fp, "AGC.droppedStartupFrames = %hu\n",
                                                    agc.droppedStartupFrames);
                    fprintf(fp, "AGC.minGainUnityStartupDisable = %u\n",
                                                    agc.minGainUnityStartupDisable);
                    fprintf(fp, "AGC.group0NumRegion = %u\n",
                                                    agc.group0NumRegion);
                    fprintf(fp, "AGC.flickerAvoidancePeriod = %u\n",
                                                    agc.flickerAvoidancePeriod);
                    fprintf(fp, "AGC.useDigitalGain = %u\n",
                                                    agc.useDigitalGain);
                    /* HDR Metering Configurations */
                    fprintf(fp, "AGC.hdrM_Config.ldrBin = %u\n",
                                                    agc.hdrM_Config.ldrBin);
                    fprintf(fp, "AGC.hdrM_Config.ldrTgtVal = %u\n",
                                                    agc.hdrM_Config.ldrTgtVal);
                    fprintf(fp, "AGC.hdrM_Config.ldrPixPct = %u\n",
                                                    agc.hdrM_Config.ldrPixPct);
                    fprintf(fp, "AGC.hdrM_Config.ldrHistScale = %u\n",
                                                    agc.hdrM_Config.ldrHistScale);
                    fprintf(fp, "AGC.hdrM_Config.hdrNumSatPix = %u\n",
                                                    agc.hdrM_Config.hdrNumSatPix);
                    fprintf(fp, "AGC.hdrM_Config.hdrFiltGain = %u\n",
                                                    agc.hdrM_Config.hdrFiltGain);
                }
                break;
            case TUNER_CONFIG_AWB:
                memcpy(&awb, &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                        tunerImg->cfgs[i].count);
                if (TUNER_AWB_MAGIC != awb.magic) {
                    DEBUG_ERR("%s: Wrong AWB magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### AWB Data ######\n");
                    fprintf(fp, "AWB.version = 0x%x\n", awb.version);
                    fprintf(fp, "AWB.isEnabled = %d\n", awb.isEnabled);
                    fprintf(fp, "AWB.rate = %d\n", awb.rate);
                    switch(awb.method) {
                        case AWB_METHOD_B:
                            fprintf(fp, "AWB.method = BAYESIAN\n");
                            break;
                        case AWB_METHOD_RED_NORM_CT:
                            fprintf(fp, "AWB.method = RED_NORM_CT\n");
                            break;
                        case AWB_METHOD_GREY_WORLD:
                            fprintf(fp, "AWB.method = GREY_WORLD\n");
                            break;
                        case AWB_METHOD_INITIAL:
                            fprintf(fp, "AWB.method = INITIAL\n");
                            break;
                        case AWB_METHOD_FIXED_CAL_GAINS:
                            fprintf(fp, "AWB.method = FIXED_CALC_GAINS\n");
                            break;
                        default:
                            fprintf(fp, "AWB.method = UNKNOWN\n");
                            break;
                    }
                    fprintf(fp, "AWB.nRegionsX = %d\n", awb.nRegionsX);
                    fprintf(fp, "AWB.nRegionsY = %d\n", awb.nRegionsY);
                    fprintf(fp, "AWB.useSensorAWB = %u\n", awb.useSensorAWB);

                    fprintf(fp, "AWB.awbSpeedModExposureFunc.pts = %p\n",
                            awb.awbSpeedModExposureFunc.pts);
                    fprintf(fp, "AWB.awbSpeedModExposureFunc.maxNum = %d\n",
                            awb.awbSpeedModExposureFunc.maxNum);
                    fprintf(fp, "AWB.awbSpeedModExposureFunc.currentNum = %d\n",
                            awb.awbSpeedModExposureFunc.currentNum);
                    for (j = 0; j < AWB_MAX_MOD_SPEED_PTS; j++) {
                        fprintf(fp, "AWB.awbSpeedModExposurePts[%d].x = %d\n",
                                j, awb.awbSpeedModExposurePts[j].x);
                        fprintf(fp, "AWB.awbSpeedModExposurePts[%d].y = %d\n",
                                j, awb.awbSpeedModExposurePts[j].y);
                    }
                    fprintf(fp, "AWB.fastStartupFrames = %u\n", awb.fastStartupFrames);
                    fprintf(fp, "AWB.initialRNrm = %u\n", awb.initialRNrm);
                    fprintf(fp, "AWB.initialBias.gainR = %u\n", awb.initialBias.gainR);
                    fprintf(fp, "AWB.initialBias.gainB = %u\n", awb.initialBias.gainB);
                    fprintf(fp, "AWB.initialStartupFrames = %u\n", awb.initialStartupFrames);

                    fprintf(fp, "AWB.expToRedNormFunc.maxNum = %d\n",
                            awb.expToRedNormFunc.maxNum);
                    fprintf(fp, "AWB.expToRedNormFunc.pts = %d\n",
                            awb.expToRedNormFunc.currentNum);
                    for (j = 0; j < AWB_MAX_CURVE_PTS; j++) {
                        fprintf(fp, "AWB.expToRedNormPts[%d].x = %d\n",
                                j, awb.expToRedNormPts[j].x);
                        fprintf(fp, "AWB.expToRedNormPts[%d].y = %d\n",
                                j, awb.expToRedNormPts[j].y);
                    }

                    fprintf(fp, "AWB.biasEnable = %u\n", awb.biasEnable);
                    fprintf(fp, "AWB.ctBias.r.func.pts = %p\n",
                            awb.ctBias.r.func.pts);
                    fprintf(fp, "AWB.ctBias.r.func.maxNum = %d\n",
                            awb.ctBias.r.func.maxNum);
                    fprintf(fp, "AWB.ctBias.r.func.currentNum = %d\n",
                            awb.ctBias.r.func.currentNum);
                    for (j = 0; j < AWB_MAX_CT_BIAS_PTS; j++) {
                         fprintf(fp, "AWB.ctBias.r.pts[%d].x = %d\n",
                                 j, awb.ctBias.r.pts[j].x);
                         fprintf(fp, "AWB.ctBias.r.pts[%d].y = %d\n",
                                 j, awb.ctBias.r.pts[j].y);
                    }
                    fprintf(fp, "AWB.ctBias.b.func.pts = %p\n",
                            awb.ctBias.b.func.pts);
                    fprintf(fp, "AWB.ctBias.b.func.maxNum = %d\n",
                            awb.ctBias.b.func.maxNum);
                    fprintf(fp, "AWB.ctBias.b.func.currentNum = %d\n",
                            awb.ctBias.b.func.currentNum);
                    for (j = 0; j < AWB_MAX_CT_BIAS_PTS; j++) {
                         fprintf(fp, "AWB.ctBias.b.pts[%d].x = %d\n",
                                 j, awb.ctBias.b.pts[j].x);
                         fprintf(fp, "AWB.ctBias.b.pts[%d].y = %d\n",
                                 j, awb.ctBias.b.pts[j].y);
                    }

                    fprintf(fp, "AWB.ctFunc.pts = %p\n", awb.ctFunc.pts);
                    fprintf(fp, "AWB.ctFunc.maxNum = %d\n", awb.ctFunc.maxNum);
                    fprintf(fp, "AWB.ctFunc.currentNum = %d\n", awb.ctFunc.currentNum);
                    for (j = 0; j < AWB_MAX_CURVE_PTS; j++) {
                        fprintf(fp, "AWB.ctPts[%d].x = %d\n", j, awb.ctPts[j].x);
                        fprintf(fp, "AWB.ctPts[%d].y = %d\n", j, awb.ctPts[j].y);
                    }

                    for (j = 0; j < AWB_MAX_T_CURVES; j++) {
                        for (k = 0; k < AWB_MAX_RIDGE_PRIORS; k++) {
                            fprintf(fp, "AWB.bayesT[%d].ridgePrior[%d].func.pts = %p\n",
                                    j, k, awb.bayesT[j].ridgePrior[k].func.pts);
                            fprintf(fp, "AWB.bayesT[%d].ridgePrior[%d].func.maxNum = %d\n",
                                    j, k, awb.bayesT[j].ridgePrior[k].func.maxNum);
                            fprintf(fp, "AWB.bayesT[%d].ridgePrior[%d].func.currentNum = %d\n",
                                    j, k, awb.bayesT[j].ridgePrior[k].func.currentNum);
                            for (l = 0; l < AWB_MAX_RIDGE_PRIOR_PTS; l++) {
                                fprintf(fp, "AWB.bayesT[%d].ridgePrior[%d].pts[%d].x = %d\n",
                                        j, k, l, awb.bayesT[j].ridgePrior[k].pts[l].x);
                                fprintf(fp, "AWB.bayesT[%d].ridgePrior[%d].pts[%d].y = %d\n",
                                        j, k, l, awb.bayesT[j].ridgePrior[k].pts[l].y);
                            }
                        }

                        fprintf(fp, "AWB.bayesT[%d].ridgePriorInterpFunc.pts = %p\n",
                                j, awb.bayesT[j].ridgePriorInterpFunc.pts);
                        fprintf(fp, "AWB.bayesT[%d].ridgePriorInterpFunc.maxNum = %d\n",
                                j, awb.bayesT[j].ridgePriorInterpFunc.maxNum);
                        fprintf(fp, "AWB.bayesT[%d].ridgePriorInterpFunc.currentNum = %d\n",
                                j, awb.bayesT[j].ridgePriorInterpFunc.currentNum);
                        for (k = 0; k < AWB_NUM_EXP_P; k++) {
                            fprintf(fp, "AWB.bayesT[%d].ridgePriorInterpPts[%d].x = %d\n",
                                    j, k, awb.bayesT[j].ridgePriorInterpPts[k].x);
                            fprintf(fp, "AWB.bayesT[%d].ridgePriorInterpPts[%d].y = %d\n",
                                    j, k, awb.bayesT[j].ridgePriorInterpPts[k].y);
                        }
                    }

                    fprintf(fp, "AWB.bayesCentreWeight = %u\n",
                            awb.bayesCentreWeight);
                    fprintf(fp, "AWB.bayesTCurveN = %u\n", awb.bayesTCurveN);
                    fprintf(fp, "AWB.bayesInterCurveStrength = %u\n",
                            awb.bayesInterCurveStrength);
                    fprintf(fp, "AWB.bayesInterTPow = %u\n", awb.bayesInterTPow);
                    fprintf(fp, "AWB.bayes_S_T_PDFsTexN = %u\n", awb.bayes_S_T_PDFsTexN);

                    for (j = 0; j < AWB_MAX_S_TEX_PDFS; j++) {
                        fprintf(fp, "AWB.bayes_S_T_PDFsTex[%d].wExp.pts = %p\n",
                                j, awb.bayes_S_T_PDFsTex[j].wExp.pts);
                        fprintf(fp, "AWB.bayes_S_T_PDFsTex[%d].wExp.maxNum = %d\n",
                                j, awb.bayes_S_T_PDFsTex[j].wExp.maxNum);
                        fprintf(fp, "AWB.bayes_S_T_PDFsTex[%d].wExp.currentNum = %d\n",
                                j, awb.bayes_S_T_PDFsTex[j].wExp.currentNum);
                        for (k = 0; k < AWB_NUM_TEX_P; k++) {
                            fprintf(fp, "AWB.bayes_S_T_PDFsTex[%d].wExpPts[%d].x = %d\n",
                                    j, k, awb.bayes_S_T_PDFsTex[j].wExpPts[k].x);
                            fprintf(fp, "AWB.bayes_S_T_PDFsTex[%d].wExpPts[%d].y = %d\n",
                                    j, k, awb.bayes_S_T_PDFsTex[j].wExpPts[k].y);

                        }
                        for (k = 0; k < AWB_TEX_DIM; k++) {
                            for ( l = 0; l < AWB_TEX_DIM; l++) {
                                int32_t index = (k * AWB_TEX_DIM + l);
                                fprintf(fp, "AWB.bayes_S_T_PDFsTex[%d].pdfTex[%d] = %u\n",
                                        j, index, awb.bayes_S_T_PDFsTex[j].pdfTex[index]);
                            }
                        }
                    }

                    /* Precalc Data */
                    for (j = 0; j < AWB_MAX_T_CURVES; j++) {
                        for (k = 0; k < MAX_T_POINTS_PER_CURVE; k++) {
                            fprintf(fp, "AWB.precalcData.t[%d].rNrm[%d] = %u\n",
                                    j, k, awb.precalcData.t[j].rNrm[k]);
                            fprintf(fp, "AWB.precalcData.t[%d].bNrm[%d] = %u\n",
                                    j, k, awb.precalcData.t[j].bNrm[k]);
                        }
                        fprintf(fp, "AWB.precalcData.t[%d].numTPoints = %u\n",
                                j, awb.precalcData.t[j].numTPoints);
                        for (k = 0; k < MAX_T_POINTS_PER_CURVE; k++) {
                            fprintf(fp, "AWB.precalcData.t[%d].pTPrior[%d] = %u\n",
                                    j, k, awb.precalcData.t[j].pTPrior[k]);
                        }
                        for (k = 0; k < MAX_T_POINTS_PER_CURVE; k++) {
                            for (l = 0; l < AWB_MAX_S_TEX_PDFS; l++) {
                                fprintf(fp, "AWB.precalcData.t[%d].pdfWeightsT[%d][%d] = %u\n",
                                        j, k, l, awb.precalcData.t[j].pdfWeightsT[k][l]);
                            }
                        }
                        for (k = 0; k < MAX_T_POINTS_PER_CURVE; k++) {
                            for (l = 0; l <  3; l++) {
                                for (m = 0; m <  3; m++) {
                                    fprintf(fp, "AWB.precalcData.t[%d].mhTCCM[%d][%d][%d] = %u\n",
                                            j, k, l, m, awb.precalcData.t[j].mhTCCM[k][l][m]);
                                }
                            }
                        }
                    }
                    fprintf(fp, "AWB.precalcData.partToPartGainR = %u\n",
                            awb.precalcData.partToPartGainR);
                    fprintf(fp, "AWB.precalcData.partToPartGainB = %u\n",
                            awb.precalcData.partToPartGainB);
                    fprintf(fp, "AWB.precalcData.canonSensR = %u\n",
                            awb.precalcData.canonSensR);
                    fprintf(fp, "AWB.precalcData.canonSensG = %u\n",
                            awb.precalcData.canonSensG);
                    fprintf(fp, "AWB.precalcData.canonSensB = %u\n",
                            awb.precalcData.canonSensB);
                    fprintf(fp, "AWB.precalcData.sensorSensR = %u\n",
                            awb.precalcData.sensorSensR);
                    fprintf(fp, "AWB.precalcData.sensorSensG = %u\n",
                            awb.precalcData.sensorSensG);
                    fprintf(fp, "AWB.precalcData.sensorSensB = %u\n",
                            awb.precalcData.sensorSensB);

                    for (j = 0; j < AWB_MAX_S_TEX_PDFS; j++) {
                        for (k = 0; k < 128; k++) {
                            fprintf(fp, "AWB.precalcData.pdfWeightsInt[%d][%d] = %u\n",
                                    j, k, awb.precalcData.pdfWeightsInt[j][k]);
                        }
                    }
                }
                break;
            case TUNER_CONFIG_DENOISE:
                memcpy(&denoise, &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                        tunerImg->cfgs[i].count);
                if (TUNER_DENOISE_MAGIC != denoise.magic) {
                    DEBUG_ERR("%s: Wrong Denoise magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### Denoise Data ######\n");
                    fprintf(fp, "Denoise.version = 0x%x\n", denoise.version);
                    fprintf(fp, "Denoise.isEnabled = %d\n", denoise.isEnabled);
                    fprintf(fp, "Denoise.rate = %d\n", denoise.rate);
                    for (j = 0; j < DENOISE_MAX_HW_CURVES; j++) {
                        for (k = 0; k < 12; k++) {
                            fprintf(fp, "Denoise.bayerHW.curve[%d].abscissae[%d] = %u\n",
                                                j, k, denoise.bayerHW.curve[j].abscissae[k]);
                            fprintf(fp, "Denoise.bayerHW.curve[%d].ordinates[%d] = %u\n",
                                                j, k, denoise.bayerHW.curve[j].ordinates[k]);
                        }
                    }
                    /* Print BayerHW interp */
                    fprintf(fp, "Denoise.bayerHW.interp.func.pts = %p\n",
                                                    denoise.bayerHW.interp.func.pts);
                    fprintf(fp, "Denoise.bayerHW.interp.func.maxNum = %d\n",
                                                    denoise.bayerHW.interp.func.maxNum);
                    fprintf(fp, "Denoise.bayerHW.interp.func.currentNum = %d\n",
                                                    denoise.bayerHW.interp.func.currentNum);
                    for (j=0; j<DENOISE_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.bayerHW.interp.pts[%d].x = %d\n",
                                                    j, denoise.bayerHW.interp.pts[j].x);
                        fprintf(fp, "Denoise.bayerHW.interp.pts[%d].y = %d\n",
                                                    j, denoise.bayerHW.interp.pts[j].y);
                    }

                    /* Print BayerHW params */
                    fprintf(fp, "Denoise.bayerHW.digGainReduction = %u\n",
                                                        denoise.bayerHW.digGainReduction);
                    fprintf(fp, "Denoise.bayerHW.grGain = %u\n", denoise.bayerHW.grGain);
                    fprintf(fp, "Denoise.bayerHW.gbGain = %u\n", denoise.bayerHW.gbGain);
                    fprintf(fp, "Denoise.bayerHW.rGain = %u\n", denoise.bayerHW.rGain);
                    fprintf(fp, "Denoise.bayerHW.bGain = %u\n", denoise.bayerHW.bGain);

                    /* Print BayerHW grStr PWL */
                    fprintf(fp, "Denoise.bayerHW.grStr.func.pts = %p\n",
                                                    denoise.bayerHW.grStr.func.pts);
                    fprintf(fp, "Denoise.bayerHW.grStr.func.maxNum = %d\n",
                                                    denoise.bayerHW.grStr.func.maxNum);
                    fprintf(fp, "Denoise.bayerHW.grStr.func.currentNum = %d\n",
                                                    denoise.bayerHW.grStr.func.currentNum);
                    for (j=0; j<DENOISE_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.bayerHW.grStr.pts[%d].x = %d\n",
                                                    j, denoise.bayerHW.grStr.pts[j].x);
                        fprintf(fp, "Denoise.bayerHW.grStr.pts[%d].y = %d\n",
                                                    j, denoise.bayerHW.grStr.pts[j].y);
                    }

                    /* Print BayerHW gbStr PWL */
                    fprintf(fp, "Denoise.bayerHW.gbStr.func.pts = %p\n",
                                                    denoise.bayerHW.gbStr.func.pts);
                    fprintf(fp, "Denoise.bayerHW.gbStr.func.maxNum = %d\n",
                                                    denoise.bayerHW.gbStr.func.maxNum);
                    fprintf(fp, "Denoise.bayerHW.gbStr.func.currentNum = %d\n",
                                                    denoise.bayerHW.gbStr.func.currentNum);
                    for (j=0; j<DENOISE_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.bayerHW.gbStr.pts[%d].x = %d\n",
                                                    j, denoise.bayerHW.gbStr.pts[j].x);
                        fprintf(fp, "Denoise.bayerHW.gbStr.pts[%d].y = %d\n",
                                                    j, denoise.bayerHW.gbStr.pts[j].y);
                    }

                    /* Print BayerHW rStr PWL */
                    fprintf(fp, "Denoise.bayerHW.rStr.func.pts = %p\n",
                                                    denoise.bayerHW.rStr.func.pts);
                    fprintf(fp, "Denoise.bayerHW.rStr.func.maxNum = %d\n",
                                                    denoise.bayerHW.rStr.func.maxNum);
                    fprintf(fp, "Denoise.bayerHW.rStr.func.currentNum = %d\n",
                                                    denoise.bayerHW.rStr.func.currentNum);
                    for (j=0; j<DENOISE_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.bayerHW.rStr.pts[%d].x = %d\n",
                                                    j, denoise.bayerHW.rStr.pts[j].x);
                        fprintf(fp, "Denoise.bayerHW.rStr.pts[%d].y = %d\n",
                                                    j, denoise.bayerHW.rStr.pts[j].y);
                    }

                    /* Print BayerHW bStr PWL */
                    fprintf(fp, "Denoise.bayerHW.bStr.func.pts = %p\n",
                                                    denoise.bayerHW.bStr.func.pts);
                    fprintf(fp, "Denoise.bayerHW.bStr.func.maxNum = %d\n",
                                                    denoise.bayerHW.bStr.func.maxNum);
                    fprintf(fp, "Denoise.bayerHW.bStr.func.currentNum = %d\n",
                                                    denoise.bayerHW.bStr.func.currentNum);
                    for (j=0; j<DENOISE_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.bayerHW.bStr.pts[%d].x = %d\n",
                                                    j, denoise.bayerHW.bStr.pts[j].x);
                        fprintf(fp, "Denoise.bayerHW.bStr.pts[%d].y = %d\n",
                                                    j, denoise.bayerHW.bStr.pts[j].y);
                    }

                    /* Print BayerHW Mask */
                    for (j=0; j<49; j++) {
                        fprintf(fp, "Denoise.bayerHW.mask[%d] = %u\n",
                                                    j, denoise.bayerHW.mask[j]);
                    }

                    /* Print colourHW redStrength PWL */
                    fprintf(fp, "Denoise.colourHW.redStrength.func.pts = %p\n",
                                                    denoise.colourHW.redStrength.func.pts);
                    fprintf(fp, "Denoise.colourHW.redStrength.func.maxNum = %d\n",
                                                    denoise.colourHW.redStrength.func.maxNum);
                    fprintf(fp, "Denoise.colourHW.redStrength.func.currentNum = %d\n",
                                                    denoise.colourHW.redStrength.func.currentNum);
                    for (j=0; j<DENOISE_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.colourHW.redStrength.pts[%d].x = %d\n",
                                                    j, denoise.colourHW.redStrength.pts[j].x);
                        fprintf(fp, "Denoise.colourHW.redStrength.pts[%d].y = %d\n",
                                                    j, denoise.colourHW.redStrength.pts[j].y);
                    }

                    /* Print colourHW blueStrength PWL */
                    fprintf(fp, "Denoise.colourHW.blueStrength.func.pts = %p\n",
                                                    denoise.colourHW.blueStrength.func.pts);
                    fprintf(fp, "Denoise.colourHW.blueStrength.func.maxNum = %d\n",
                                                    denoise.colourHW.blueStrength.func.maxNum);
                    fprintf(fp, "Denoise.colourHW.blueStrength.func.currentNum = %d\n",
                                                    denoise.colourHW.blueStrength.func.currentNum);
                    for (j=0; j<DENOISE_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.colourHW.blueStrength.pts[%d].x = %d\n",
                                                    j, denoise.colourHW.blueStrength.pts[j].x);
                        fprintf(fp, "Denoise.colourHW.blueStrength.pts[%d].y = %d\n",
                                                    j, denoise.colourHW.blueStrength.pts[j].y);
                    }
                    /* Print colourHW cdnParams */
                    for (j = 0UL; j < ISP_CDN_NUM_PTS; j++) {
                        fprintf(fp, "Denoise.colourHW.cdnParams.firBG_Abscissae[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.firBG_Abscissae[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.firBG_Ordinates[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.firBG_Ordinates[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.firBG_Slopes[%d] = %d\n",
                                            j, denoise.colourHW.cdnParams.firBG_Slopes[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.firRG_Abscissae[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.firRG_Abscissae[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.firRG_Ordinates[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.firRG_Ordinates[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.firRG_Slopes[%d] = %d\n",
                                            j, denoise.colourHW.cdnParams.firRG_Slopes[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.iirBG_Abscissae[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.iirBG_Abscissae[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.iirBG_Ordinates[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.iirBG_Ordinates[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.iirBG_Slopes[%d] = %d\n",
                                            j, denoise.colourHW.cdnParams.iirBG_Slopes[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.iirRG_Abscissae[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.iirRG_Abscissae[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.iirRG_Ordinates[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.iirRG_Ordinates[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.iirRG_Slopes[%d] = %d\n",
                                            j, denoise.colourHW.cdnParams.iirRG_Slopes[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.distAbscissae[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.distAbscissae[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.distOrdinates[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.distOrdinates[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.distSlopes[%d] = %d\n",
                                            j, denoise.colourHW.cdnParams.distSlopes[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.gAbscissae[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.gAbscissae[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.gOrdinates[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.gOrdinates[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.gSlopes[%d] = %d\n",
                                            j, denoise.colourHW.cdnParams.gSlopes[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.satAbscissae[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.satAbscissae[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.satOrdinates[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.satOrdinates[j]);
                        fprintf(fp, "Denoise.colourHW.cdnParams.satSlopes[%d] = %d\n",
                                            j, denoise.colourHW.cdnParams.satSlopes[j]);
                    }
                    for (j = 0UL; j < 4UL; j++) {
                        fprintf(fp, "Denoise.colourHW.cdnParams.weights[%d] = %u\n",
                                            j, denoise.colourHW.cdnParams.weights[j]);
                    }
                    fprintf(fp, "Denoise.colourHW.cdnParams.xCentre = %u\n",
                                            denoise.colourHW.cdnParams.xCentre);
                    fprintf(fp, "Denoise.colourHW.cdnParams.yCentre = %u\n",
                                            denoise.colourHW.cdnParams.yCentre);
                    fprintf(fp, "Denoise.colourHW.cdnParams.iirAttenuationLines = %u\n",
                                            denoise.colourHW.cdnParams.iirAttenuationLines);
                    fprintf(fp, "Denoise.colourHW.cdnParams.iirAttenuationShift = %u\n",
                                            denoise.colourHW.cdnParams.iirAttenuationShift);
                    fprintf(fp, "Denoise.colourHW.cdnParams.firShift = %u\n",
                                            denoise.colourHW.cdnParams.firShift);
                    fprintf(fp, "Denoise.colourHW.cdnParams._overlap = %u\n",
                                            denoise.colourHW.cdnParams._overlap);
                    fprintf(fp, "Denoise.colourHW.cdnParams.edgeThreshold = %u\n",
                                            denoise.colourHW.cdnParams.edgeThreshold);
                    fprintf(fp, "Denoise.colourHW.cdnParams.edgeStrength = %u\n",
                                            denoise.colourHW.cdnParams.edgeStrength);
                    fprintf(fp, "Denoise.colourHW.cdnParams.edgeMin = %u\n",
                                            denoise.colourHW.cdnParams.edgeMin);
                }
                break;
            case TUNER_CONFIG_CC:
                memcpy(&cc,
                       &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                       tunerImg->cfgs[i].count);
                if (TUNER_CC_MAGIC != cc.magic) {
                    DEBUG_ERR("%s: Wrong CC magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### CC Tuner Data ######\n");
                    fprintf(fp, "CC.version = 0x%x\n", cc.version);
                    fprintf(fp, "CC.isEnabled = %u\n", cc.isEnabled);
                    fprintf(fp, "CC.rate = %u\n", cc.rate);
                    fprintf(fp, "CC.ispBPP = %u\n", cc.ispBPP);

                    for (j=0; j<CC_MAX_NUM_TABLES; j++) {
                        for (k=0; k<3; k++) {
                            for (l=0; l<3; l++) {
                                fprintf(fp, "CC.table[%d].matrix[%d][%d] = %d\n",
                                        j, k, l, cc.table[j].matrix[k][l]);
                            }
                            fprintf(fp, "CC.table[%d].offsets[%d] = %d\n",
                                    j, k, cc.table[j].offsets[k]);
                        }
                        fprintf(fp, "CC.table[%d].baseGreenness = %d\n",
                                j, cc.table[j].baseGreenness);
                    }

                    fprintf(fp, "CC.interpFunc.maxNum = %d\n",
                            cc.interpFunc.maxNum);
                    fprintf(fp, "CC.interpFunc.currentNum = %d\n",
                            cc.interpFunc.currentNum);
                    for (j=0; j<CC_MAX_PWL_PTS; j++) {
                        fprintf(fp, "CC.interpPts[%d].x = %d\n",
                                j, cc.interpPts[j].x);
                        fprintf(fp, "CC.interpPts[%d].y = %d\n",
                                j, cc.interpPts[j].y);

                    }

                    fprintf(fp, "CC.saturationFunc.maxNum = %d\n",
                            cc.saturationFunc.maxNum);
                    fprintf(fp, "CC.saturationFunc.currentNum = %d\n",
                            cc.saturationFunc.currentNum);
                    for (j=0; j<CC_MAX_PWL_PTS; j++) {
                        fprintf(fp, "CC.saturationPts[%d].x = %d\n",
                                j, cc.saturationPts[j].x);
                        fprintf(fp, "CC.saturationPts[%d].y = %d\n",
                                j, cc.saturationPts[j].y);
                    }

                    for (j=0; j<3; j++) {
                        for (k=0; k<3; k++) {
                            fprintf(fp, "CC.greenMatrix[%d][%d] = %d\n",
                                    j, k, cc.greenMatrix[j][k]);
                        }
                    }
                }
                break;
            case TUNER_CONFIG_DEFPIX:
                memcpy(&defPix,
                       &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                       tunerImg->cfgs[i].count);
                if (TUNER_DEFPIX_MAGIC != defPix.magic) {
                    DEBUG_ERR("%s: Wrong ADPC magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### ADPC Tuner Data ######\n");
                    fprintf(fp, "defPix.version = 0x%x\n", defPix.version);
                    fprintf(fp, "defPix.isEnabled = %u\n", defPix.isEnabled);
                    fprintf(fp, "defPix.rate = %u\n", defPix.rate);

                    for (k=0; k<3; k++) {
                        fprintf(fp, "defPix.scaleHi[%d].func.maxNum = %d\n",
                                k, defPix.scaleHi[k].func.maxNum);
                        fprintf(fp, "defPix.scaleHi[%d].func.currentNum = %d\n",
                                k, defPix.scaleHi[k].func.currentNum);
                        for (j=0; j<DEFPIX_MAXPTS; j++) {
                            fprintf(fp, "defPix.scaleHi[%d].pts[%d].x = %d\n",
                                    k, j, defPix.scaleHi[k].pts[j].x);
                            fprintf(fp, "defPix.scaleHi[%d].pts[%d].y = %d\n",
                                    k, j, defPix.scaleHi[k].pts[j].y);
                        }
                    }
                    for (k=0; k<3; k++) {
                        fprintf(fp, "defPix.scaleLo[%d].func.maxNum = %d\n",
                                k, defPix.scaleLo[k].func.maxNum);
                        fprintf(fp, "defPix.scaleLo[%d].func.currentNum = %d\n",
                                k, defPix.scaleLo[k].func.currentNum);
                        for (j=0; j<DEFPIX_MAXPTS; j++) {
                            fprintf(fp, "defPix.scaleLo[%d].pts[%d].x = %d\n",
                                    k, j, defPix.scaleLo[k].pts[j].x);
                            fprintf(fp, "defPix.scaleLo[%d].pts[%d].y = %d\n",
                                    k, j, defPix.scaleLo[k].pts[j].y);
                        }
                    }
                    for (k=0; k<3; k++) {
                        fprintf(fp, "defPix.offsetHi[%d].func.maxNum = %d\n",
                                k, defPix.offsetHi[k].func.maxNum);
                        fprintf(fp, "defPix.offsetHi[%d].func.currentNum = %d\n",
                                k, defPix.offsetHi[k].func.currentNum);
                        for (j=0; j<DEFPIX_MAXPTS; j++) {
                            fprintf(fp, "defPix.offsetHi[%d].pts[%d].x = %d\n",
                                    k, j, defPix.offsetHi[k].pts[j].x);
                            fprintf(fp, "defPix.offsetHi[%d].pts[%d].y = %d\n",
                                    k, j, defPix.offsetHi[k].pts[j].y);
                        }
                    }
                    for (k=0; k<3; k++) {
                        fprintf(fp, "defPix.offsetLo[%d].func.maxNum = %d\n",
                                k, defPix.offsetLo[k].func.maxNum);
                        fprintf(fp, "defPix.offsetLo[%d].func.currentNum = %d\n",
                                k, defPix.offsetLo[k].func.currentNum);
                        for (j=0; j<DEFPIX_MAXPTS; j++) {
                            fprintf(fp, "defPix.offsetLo[%d].pts[%d].x = %d\n",
                                    k, j, defPix.offsetLo[k].pts[j].x);
                            fprintf(fp, "defPix.offsetLo[%d].pts[%d].y = %d\n",
                                    k, j, defPix.offsetLo[k].pts[j].y);
                        }
                    }

                    fprintf(fp, "defPix.replaceIdxLo.func.maxNum = %d\n",
                            defPix.replaceIdxLo.func.maxNum);
                    fprintf(fp, "defPix.replaceIdxLo.func.currentNum = %d\n",
                            defPix.replaceIdxLo.func.currentNum);
                    for (j=0; j<DEFPIX_MAXPTS; j++) {
                        fprintf(fp, "defPix.replaceIdxLo.pts[%d].x = %d\n",
                                j, defPix.replaceIdxLo.pts[j].x);
                        fprintf(fp, "defPix.replaceIdxLo.pts[%d].y = %d\n",
                                j, defPix.replaceIdxLo.pts[j].y);
                    }

                    fprintf(fp, "defPix.replaceIdxHi.func.maxNum = %d\n",
                            defPix.replaceIdxHi.func.maxNum);
                    fprintf(fp, "defPix.replaceIdxHi.func.currentNum = %d\n",
                            defPix.replaceIdxHi.func.currentNum);
                    for (j=0; j<DEFPIX_MAXPTS; j++) {
                        fprintf(fp, "defPix.replaceIdxHi.pts[%d].x = %d\n",
                                j, defPix.replaceIdxHi.pts[j].x);
                        fprintf(fp, "defPix.replaceIdxHi.pts[%d].y = %d\n",
                                j, defPix.replaceIdxHi.pts[j].y);
                    }

                    fprintf(fp, "defPix.replaceScaleLo.func.maxNum = %d\n",
                            defPix.replaceScaleLo.func.maxNum);
                    fprintf(fp, "defPix.replaceScaleLo.func.currentNum = %d\n",
                            defPix.replaceScaleLo.func.currentNum);
                    for (j=0; j<DEFPIX_MAXPTS; j++) {
                        fprintf(fp, "defPix.replaceScaleLo.pts[%d].x = %d\n",
                                j, defPix.replaceScaleLo.pts[j].x);
                        fprintf(fp, "defPix.replaceScaleLo.pts[%d].y = %d\n",
                                j, defPix.replaceScaleLo.pts[j].y);
                    }

                    fprintf(fp, "defPix.replaceScaleHi.func.maxNum = %d\n",
                            defPix.replaceScaleHi.func.maxNum);
                    fprintf(fp, "defPix.replaceScaleHi.func.currentNum = %d\n",
                            defPix.replaceScaleHi.func.currentNum);
                    for (j=0; j<DEFPIX_MAXPTS; j++) {
                        fprintf(fp, "defPix.replaceScaleHi.pts[%d].x = %d\n",
                                j, defPix.replaceScaleHi.pts[j].x);
                        fprintf(fp, "defPix.replaceScaleHi.pts[%d].y = %d\n",
                                j, defPix.replaceScaleHi.pts[j].y);
                    }
                }
                break;
            case TUNER_CONFIG_SHARPEN:
                memcpy(&sharpen,
                       &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                       tunerImg->cfgs[i].count);
                if (TUNER_SHARPEN_MAGIC != sharpen.magic) {
                    DEBUG_ERR("%s: Wrong Sharpen magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### Sharpening Tuner Data ######\n");
                    fprintf(fp, "sharpen.version = 0x%x\n", sharpen.version);
                    fprintf(fp, "sharpen.isEnabled = %u\n", sharpen.isEnabled);
                    fprintf(fp, "sharpen.rate = %u\n", sharpen.rate);

                    fprintf(fp, "sharpen.posGain.func.maxNum = %d\n",
                            sharpen.posGain.func.maxNum);
                    fprintf(fp, "sharpen.posGain.func.currentNum = %d\n",
                            sharpen.posGain.func.currentNum);
                    for (j=0; j<SHARPEN_MAXPTS; j++) {
                        fprintf(fp, "sharpen.posGain.pts[%d].x = %d\n",
                                j, sharpen.posGain.pts[j].x);
                        fprintf(fp, "sharpen.posGain.pts[%d].y = %d\n",
                                j, sharpen.posGain.pts[j].y);
                    }

                    fprintf(fp, "sharpen.relPosGain.func.maxNum = %d\n",
                            sharpen.relPosGain.func.maxNum);
                    fprintf(fp, "sharpen.relPosGain.func.currentNum = %d\n",
                            sharpen.relPosGain.func.currentNum);
                    for (j=0; j<SHARPEN_MAXPTS; j++) {
                        fprintf(fp, "sharpen.relPosGain.pts[%d].x = %d\n",
                                j, sharpen.relPosGain.pts[j].x);
                        fprintf(fp, "sharpen.relPosGain.pts[%d].y = %d\n",
                                j, sharpen.relPosGain.pts[j].y);
                    }

                    fprintf(fp, "sharpen.vGain.func.maxNum = %d\n",
                            sharpen.vGain.func.maxNum);
                    fprintf(fp, "sharpen.vGain.func.currentNum = %d\n",
                            sharpen.vGain.func.currentNum);
                    for (j=0; j<SHARPEN_MAXPTS; j++) {
                        fprintf(fp, "sharpen.vGain.pts[%d].x = %d\n",
                                j, sharpen.vGain.pts[j].x);
                        fprintf(fp, "sharpen.vGain.pts[%d].y = %d\n",
                                j, sharpen.vGain.pts[j].y);
                    }

                    fprintf(fp, "sharpen.respGain.func.maxNum = %d\n",
                            sharpen.respGain.func.maxNum);
                    fprintf(fp, "sharpen.respGain.func.currentNum = %d\n",
                            sharpen.respGain.func.currentNum);
                    for (j=0; j<SHARPEN_MAXPTS; j++) {
                        fprintf(fp, "sharpen.respGain.pts[%d].x = %d\n",
                                j, sharpen.respGain.pts[j].x);
                        fprintf(fp, "sharpen.respGain.pts[%d].y = %d\n",
                                j, sharpen.respGain.pts[j].y);
                    }

                    for (j=0; j<SHARPEN_MAXPTS; j++) {
                        fprintf(fp, "sharpen.response[%d].resp.func.maxNum = %d\n",
                                j, sharpen.response[j].resp.func.maxNum);
                        fprintf(fp, "sharpen.response[%d].resp.func.currentNum = %d\n",
                                j, sharpen.response[j].resp.func.currentNum);
                        for (k=0; k<SHARPEN_MAXPTS; k++) {
                            fprintf(fp, "sharpen.response[%d].resp.pts[%d].x = %d\n",
                                    j, k, sharpen.response[j].resp.pts[k].x);
                            fprintf(fp, "sharpen.response[%d].resp.pts[%d].y = %d\n",
                                    j, k, sharpen.response[j].resp.pts[k].y);
                        }

                        fprintf(fp, "sharpen.response[%d].resp1.func.maxNum = %d\n",
                                j, sharpen.response[j].resp1.func.maxNum);
                        fprintf(fp, "sharpen.response[%d].resp1.func.currentNum = %d\n",
                                j, sharpen.response[j].resp1.func.currentNum);
                        for (k=0; k<SHARPEN_MAXPTS; k++) {
                            fprintf(fp, "sharpen.response[%d].resp1.pts[%d].x = %d\n",
                                    j, k, sharpen.response[j].resp1.pts[k].x);
                            fprintf(fp, "sharpen.response[%d].resp1.pts[%d].y = %d\n",
                                    j, k, sharpen.response[j].resp1.pts[k].y);
                        }
                    }

                    fprintf(fp, "sharpen.responseInterp.func.maxNum = %d\n",
                            sharpen.responseInterp.func.maxNum);
                    fprintf(fp, "sharpen.responseInterp.func.currentNum = %d\n",
                            sharpen.responseInterp.func.currentNum);
                    for (j=0; j<SHARPEN_MAXPTS; j++) {
                        fprintf(fp, "sharpen.responseInterp.pts[%d].x = %d\n",
                                j, sharpen.responseInterp.pts[j].x);
                        fprintf(fp, "sharpen.responseInterp.pts[%d].y = %d\n",
                                j, sharpen.responseInterp.pts[j].y);
                    }

                    fprintf(fp, "sharpen.isotropicStrength.func.maxNum = %d\n",
                            sharpen.isotropicStrength.func.maxNum);
                    fprintf(fp, "sharpen.isotropicStrength.func.currentNum = %d\n",
                            sharpen.isotropicStrength.func.currentNum);
                    for (j=0; j<SHARPEN_MAXPTS; j++) {
                        fprintf(fp, "sharpen.isotropicStrength.pts[%d].x = %d\n",
                                j, sharpen.isotropicStrength.pts[j].x);
                        fprintf(fp, "sharpen.isotropicStrength.pts[%d].y = %d\n",
                                j, sharpen.isotropicStrength.pts[j].y);
                    }

                    fprintf(fp, "sharpen.respGainMin = %u\n", sharpen.respGainMin);
                    fprintf(fp, "sharpen.respGainMax = %u\n", sharpen.respGainMax);
                    fprintf(fp, "sharpen.avgG_shift = %u\n", sharpen.avgG_shift);
                }
                break;
            case TUNER_CONFIG_BL:
                memcpy(&bl,
                       &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                       tunerImg->cfgs[i].count);
                if (TUNER_BL_MAGIC != bl.magic) {
                    DEBUG_ERR("%s: Wrong bl magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### Black Level Tuner Data ######\n");
                    fprintf(fp, "bl.version = 0x%x\n",bl.version);
                    fprintf(fp, "bl.isEnabled = %u\n", bl.isEnabled);
                    fprintf(fp, "bl.rate = %u\n", bl.rate);

                    fprintf(fp, "bl.agBL_rFunc.maxNum = %d\n",
                            bl.agBL_rFunc.maxNum);
                    fprintf(fp, "bl.agBL_rFunc.currentNum = %d\n",
                            bl.agBL_rFunc.currentNum);
                    for (j=0; j<BL_MAX_AG_BL_PTS; j++) {
                        fprintf(fp, "bl.agBL_pts[0][%d].x = %d\n",
                                j, bl.agBL_pts[0][j].x);
                        fprintf(fp, "bl.agBL_pts[0][%d].y = %d\n",
                                j, bl.agBL_pts[0][j].y);
                    }

                    fprintf(fp, "bl.agBL_grFunc.maxNum = %d\n",
                            bl.agBL_grFunc.maxNum);
                    fprintf(fp, "bl.agBL_grFunc.currentNum = %d\n",
                            bl.agBL_grFunc.currentNum);
                    for (j=0; j<BL_MAX_AG_BL_PTS; j++) {
                        fprintf(fp, "bl.agBL_pts[0][%d].x = %d\n",
                                j, bl.agBL_pts[1][j].x);
                        fprintf(fp, "bl.agBL_pts[0][%d].y = %d\n",
                                j, bl.agBL_pts[1][j].y);
                    }

                    fprintf(fp, "bl.agBL_bFunc.maxNum = %d\n",
                            bl.agBL_bFunc.maxNum);
                    fprintf(fp, "bl.agBL_bFunc.currentNum = %d\n",
                            bl.agBL_bFunc.currentNum);
                    for (j=0; j<BL_MAX_AG_BL_PTS; j++) {
                        fprintf(fp, "bl.agBL_pts[0][%d].x = %d\n",
                                j, bl.agBL_pts[2][j].x);
                        fprintf(fp, "bl.agBL_pts[0][%d].y = %d\n",
                                j, bl.agBL_pts[2][j].y);
                    }

                    fprintf(fp, "bl.agBL_gbFunc.maxNum = %d\n",
                            bl.agBL_gbFunc.maxNum);
                    fprintf(fp, "bl.agBL_gbFunc.currentNum = %d\n",
                            bl.agBL_gbFunc.currentNum);
                    for (j=0; j<BL_MAX_AG_BL_PTS; j++) {
                        fprintf(fp, "bl.agBL_pts[0][%d].x = %d\n",
                                j, bl.agBL_pts[3][j].x);
                        fprintf(fp, "bl.agBL_pts[0][%d].y = %d\n",
                                j, bl.agBL_pts[3][j].y);
                    }
                }
                break;
            case TUNER_CONFIG_TM:
                memcpy(&tm,
                       &tunerImg->tunerCfgData[tunerImg->cfgs[i].dataOffset],
                       tunerImg->cfgs[i].count);
                if (TUNER_TM_MAGIC != tm.magic) {
                    DEBUG_ERR("%s: Wrong tm magic\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                    goto exit;
                } else {
                    fprintf(fp, "##### Tone Mapping Tuner Data ######\n");
                    fprintf(fp, "tm.version = 0x%x\n", tm.version);
                    fprintf(fp, "tm.isEnabled = %u\n", tm.isEnabled);
                    fprintf(fp, "tm.rate = %u\n", tm.rate);

                    fprintf(fp, "tm.useSensorStats = %u\n",
                            tm.useSensorStats);
                    fprintf(fp, "tm.useGamma = %u\n",
                            tm.useGamma);
                    fprintf(fp, "tm.tolPercent = %u\n",
                            tm.tolPercent);
                    fprintf(fp, "tm.dynRangeScale = %u\n",
                            tm.dynRangeScale);
                    fprintf(fp, "tm.ldrBin = %u\n",
                            tm.ldrBin);
                    fprintf(fp, "tm.ldrPoint = %u\n",
                            tm.ldrPoint);
                    for (j = 0; j < 32; j++) {
                        fprintf(fp, "tm.binMap[%d] = %u\n",
                                j, tm.binMap[j]);
                    }
                }
                break;
            default:
                DEBUG_ERR("%s: Wrong config type\n", __FUNCTION__);
                ret = BCM_ERR_INVAL_PARAMS;
                break;
        }
    }

exit:
    if (NULL != fp)
        fclose(fp);
    return ret;
}
