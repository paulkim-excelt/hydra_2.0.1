/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
File Name:  isp_tuner_precalac_awb.c
Description: This file implements precal for AWB-Bayesian.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <isys.h>
#include <isys_types.h>
#include <isp_params.h>
#include <tuner_config_awb.h>
#include <tuner_config_cc.h>
#include "isp_regdump_interface.h"

#define DEBUG_ERR  printf
#define DEBUG_WARN printf

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

/* Externs */
/* ISP Tuner Config Data */
extern ISPTunerConfigDataIfc gTunerConfigData;

/* CorrectionInterp */
static int32_t CorrectionInterp(ISP_ConversionParamType *aDest,
        CC_TunerConfig *aConfig,
        uint32_t aCt)
{
    int32_t ret = BCM_ERR_OK;
    int32_t table0Num, table1Num;
    int32_t interp, i, j;
    CC_Table *table0, *table1;
    ISYS_PWLPointType pt;
    CC_TunerConfig ccm = *aConfig;

    /* Evaluate the interpolation function at the x value given by the red gain */
    pt.x = aCt;
    ccm.interpFunc.pts = &ccm.interpPts[0];
    ret = ISYS_PWLEvaluate(&ccm.interpFunc, &pt);
    if (BCM_ERR_OK != ret) {
        pt.y = 0;
    }
    interp = pt.y;

    if (interp <= 0L && interp >= (CC_MAX_NUM_TABLES - 1UL) * 256UL) {
        DEBUG_WARN("%s:Out-of-range ordinates in interpFunc\n", __FUNCTION__);
    }

    interp = min(max(interp, 0),
            (CC_MAX_NUM_TABLES - 1) * 256);
    table0Num = max(min(interp >> 8,
                CC_MAX_NUM_TABLES - 1), 0);
    table1Num = min(table0Num + 1,
            CC_MAX_NUM_TABLES - 1);
    table0 = &ccm.table[table0Num];
    table1 = &ccm.table[table1Num];
    interp -= table0Num << 8;

    /* Do the red gain interpolation */
    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            aDest->matrix[j][i] = (short)((table0->matrix[j][i] * (256 - interp) +
                        table1->matrix[j][i] * interp) >> 8);
            aDest->offsets[j] = (int32_t)((table0->offsets[j] * (256 - interp) +
                        table1->offsets[j] * interp) >> 8);
        }
    }
    return ret;
}

/* AWBEstimateCt */
static uint32_t AWBEstimateCt(AWB_DPF_Config *aAWBCfgData, uint0p16_t aRNrm)
{
    int32_t retval = BCM_ERR_OK;
    uint32_t ct = 0UL;
    ISYS_PWLPointType pt = {0};
    pt.x = aRNrm / 256L;

    if (aAWBCfgData->ctFunc.currentNum) {
        aAWBCfgData->ctFunc.pts = &aAWBCfgData->ctPts[0];
        retval = ISYS_PWLEvaluateExt(&(aAWBCfgData->ctFunc), &pt);

        if (BCM_ERR_OK != retval) {
            pt.y = (6500L + 3000L) / 2L;
        }

        ct = pt.y;
    } else {
        DEBUG_ERR("%s: No ct to aRNrm configured in DPF\n", __FUNCTION__);
    }

    return ct;
}

/* Precalc1Lamp */
static int32_t Precalc1Lamp(
        AWB_Bayes_T_Precalc *aPrecalc,
        AWB_DPF_Config *aAWBCfgData,
        uint16_t aTInv2var,uint0p16_t aRNrmMid,
        sint0p16_t aDistFromMid, sint0p16_t aDistAlongExp,
        sint16p16_t aPerpGrad, sint8p8_t aCosArctanPerpGrad,
        sint16p16_t aPerpOffset,int aCurT)
{
    uint0p16_t rNrm, bNrm;
    uint8p8_t gainR, gainB;
    uint8_t ipdf;
    ISYS_PWLPointType pt;
    int16_t (*tCCM)[MAX_T_POINTS_PER_CURVE][3][3];
    sint8p8_t tmpPTPrior;
    int32_t ret = BCM_ERR_OK;
    CC_TunerConfig *ccmCfg = NULL;

    rNrm = min(max((aDistFromMid * aCosArctanPerpGrad) / 256 + aRNrmMid, 0), 65536);
    bNrm = min(max((aPerpGrad / 256 * rNrm / 256) + aPerpOffset, 0), 65536);
    gainR = ((65536 - rNrm - bNrm) * 256) / (rNrm);
    gainB = ((65536 - rNrm - bNrm) * 256) / (bNrm);

    tCCM = (void *)aPrecalc->mhTCCM;

    if ((0U != aAWBCfgData->useLocalCcm)) {
        ccmCfg = &aAWBCfgData->cc;
    } else if (0UL != gTunerConfigData.cc.isEnabled) {
        ccmCfg = &gTunerConfigData.cc;
    }

    if (NULL != ccmCfg) {
        ISP_ConversionParamType ccm;
        int32_t k;
        uint32_t ct;

        ct = AWBEstimateCt(aAWBCfgData, rNrm);
        ret = CorrectionInterp(&ccm, ccmCfg, ct);
        if (BCM_ERR_OK != ret) {
            goto exit;
        }

        for(k = 0; k < 3; k++) {
            /* Don't need to scale as it's all normalised in the calc anyway */
            (*tCCM)[aCurT][k][0] = (sint4p12_t)((ccm.matrix[k][0] * gainR) / 256);
            (*tCCM)[aCurT][k][1] = (sint4p12_t)(ccm.matrix[k][1]);
            (*tCCM)[aCurT][k][2] = (sint4p12_t)((ccm.matrix[k][2] * gainB) / 256);
        }
    } else {
        memset((*tCCM)[aCurT], 0, 9 * sizeof(uint16_t));
        (*tCCM)[aCurT][0][0] = (sint4p12_t)((gainR * 4096) / 256);
        (*tCCM)[aCurT][1][1] = (sint4p12_t)4096;
        (*tCCM)[aCurT][2][2] = (sint4p12_t)((gainB * 4096) / 256);
    }
    aPrecalc->rNrm[aCurT] = rNrm;
    aPrecalc->bNrm[aCurT] = bNrm;

    tmpPTPrior = -((((((aDistAlongExp * aDistAlongExp) / 255) * aTInv2var) / 255)) / 255);
    aPrecalc->pTPrior[aCurT] = 255 + tmpPTPrior + ((tmpPTPrior * tmpPTPrior) / (2 * 255)) +
        ((tmpPTPrior * tmpPTPrior * tmpPTPrior) / (6 * 65535)) + (((tmpPTPrior * tmpPTPrior) /
            255 * (tmpPTPrior * tmpPTPrior) / 255) / (24 * 255));

    /* Calculate the PDF weights as determined by lamp colour */
    pt.x = rNrm / 256;
    for(ipdf = 0; ipdf < aAWBCfgData->bayes_S_T_PDFsTexN; ipdf++) {
    aAWBCfgData->bayes_S_T_PDFsTex[ipdf].wRNrm.pts =
        &aAWBCfgData->bayes_S_T_PDFsTex[ipdf].wRNrmPts[0];
        int32_t pwl_retval = ISYS_PWLEvaluate(&(aAWBCfgData->bayes_S_T_PDFsTex[ipdf].wRNrm), &pt);
        if (0 != pwl_retval) {
            DEBUG_WARN("ERROR: AWB.bayes_S_T_PDFsTex[%d].wRNrm does not \
                    cover the point %d\n", ipdf, pt.x);
            pt.y = 0;
        }
        aPrecalc->pdfWeightsT[aCurT][ipdf]= pt.y;
    }

exit:
    return ret;
}

/* AWBMakesumCTformatRasterTranslation */
#define LOGICAL_XOR(a, b) (((a) && !(b)) || (!(a) && (b)))
static int32_t AWBMakesumCTformatRasterTranslation(
        uint8_t *aTformat,
        uint8_t *aRaster,
        int32_t aTToRaster)
{
    int32_t ret = BCM_ERR_OK;
    int32_t r, b;

    if (NULL == aTformat && NULL == aRaster) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    for (b=0; b < AWB_TEX_DIM; b++) {
        for (r=0; r < AWB_TEX_DIM; r++) {
            int32_t tileOddRow = b & AWB_TEX_DIM;
            int32_t tileCol = tileOddRow ? 3 - r / AWB_TEX_DIM : r / AWB_TEX_DIM;
            int32_t tileRow = b / AWB_TEX_DIM;
            int32_t tileN = tileRow * 4 + tileCol;
            int32_t subtileCol = LOGICAL_XOR(tileOddRow, (r & 32)) ? 1 : 0;
            int32_t subtileRow = LOGICAL_XOR((r & 32), (b & 32)) ? 1 : 0;
            int32_t subtileN = subtileRow + subtileCol * 2;
            int32_t microtileCol = ((r & 31) / 8);
            int32_t microtileRow = ((b & 31) / 8);
            int32_t microtile_n = microtileRow * 4 + microtileCol;
            int32_t pixelCol = r & 7;
            int32_t pixelRow = b & 7;
            int32_t pixelN = pixelRow * 8 + pixelCol;

            uint8_t *rasterp = &aRaster[b * AWB_TEX_DIM + r];
            uint8_t *tformatp = &aTformat[tileN * 0x1000 + subtileN * 0x400 +
                microtile_n * 0x40 + pixelN];

            if (aTToRaster) {
                *rasterp=*tformatp;
            } else {
                *tformatp=*rasterp;
            }
        }
    }
exit:
    return ret;
}

/* AWBMakesumSubPDFTranslate */
static int32_t AWBMakesumSubPDFTranslate(
        uint8_t *aDestPDF,
        uint8_t *aDestPDFBitfield,
        uint8_t *aSourcePDF)
{
    int32_t ret = BCM_ERR_OK;
    int32_t j, k;

    /* Copy and rasterise the texture */
    ret = AWBMakesumCTformatRasterTranslation(aSourcePDF, aDestPDF, 1);
    if (BCM_ERR_OK != ret) {
        goto exit;
    }

    /* Create a 1-bit version of the texture. Well, sort of.
     *  Each texture lookup will do memory load
     *  from a 2x2 region of the texture so we must look
     *  at all four texture values */
    if (NULL != aDestPDFBitfield) {
        if (AWB_TEX_DIM != 64) {
            ret = BCM_ERR_INVAL_PARAMS;
            DEBUG_ERR("%s: Invalid AWB_TEX_DIM\n", __FUNCTION__);
            goto exit;
        }

        memset(aDestPDFBitfield, 0, 256);

        for (j = 0; j < 64; j++) {
            for (k = 0; k < 64; k++) {
                int32_t j2 = min(63, j + 1);
                int32_t k2 = min(63, k + 1);

                if (aDestPDF[j * 64 + k] ||
                        aDestPDF[j2 * 64 + k] ||
                        aDestPDF[j * 64 + k2] ||
                        aDestPDF[j2 * 64 + k2]) {
                    int32_t byte = j * 4 + (k / 2) / 8;
                    int32_t bit = (k / 2) % 8;
                    aDestPDFBitfield[byte] |= (1 << bit);
                }
            }
        }
    }

exit:
    return ret;
}

/* UpdatePDFs */
static int32_t UpdatePDFs(AWB_DPF_Config *aAWBCfgData, AWB_TunerConfig *aAwb)
{
    int32_t ret = BCM_ERR_OK;
    int32_t i;

    for (i = 0; i < AWB_MAX_S_TEX_PDFS; i++) {
        if (aAWBCfgData && aAwb) {
            const uint32_t *src = (i < AWB_MAX_S_TEX_PDFS) ?
                aAWBCfgData->bayes_S_T_PDFsTex[i].pdfTex :
                aAWBCfgData->bayes_S_T_PDFsTex[i].pdfTex;
            uint8_t *src8 = (uint8_t*)src;

            ret = AWBMakesumSubPDFTranslate(
                    &aAwb->bayes_S_T_PDFsTex[i].pdfTex[0],
                    NULL,
                    src8);
            if (BCM_ERR_OK != ret) {
                ret = BCM_ERR_INVAL_PARAMS;
                goto exit;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }
exit:
    return ret;
}

/******************************************************************************
FUNCTION NAME: PreCalc_AWB

ARGUMENTS: aAwb

DESCRIPTION: This function Pre-calculate all T grid information.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t PreCalc_AWB(AWB_TunerConfig *aAwb)
{
    int32_t ret = BCM_ERR_OK;
    int32_t ti, ipdf, curT;
    uint0p16_t rMin, rMax;
    AWB_DPF_Config *CfgData = &gTunerConfigData.awb;
    int32_t canonicalValid = ((0 != CfgData->canonSensR) &&
            (0 != CfgData->canonSensG) &&
            (0 != CfgData->canonSensB)) ? 1 : 0;
    int32_t sensorValid = ((0 != CfgData->sensorSensR) &&
            (0 != CfgData->sensorSensG) &&
            (0 != CfgData->sensorSensB)) ? 1 : 0;

    /* Check we have valid number of curves and pdf's */
    if ((0U == CfgData->bayesTCurveN) ||
        (AWB_MAX_T_CURVES < CfgData->bayesTCurveN) ||
        (0U == CfgData->bayes_S_T_PDFsTexN) ||
        (AWB_MAX_S_TEX_PDFS < CfgData->bayes_S_T_PDFsTexN)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (canonicalValid && sensorValid) {
        aAwb->precalcData.partToPartGainR = ((CfgData->canonSensR *
                    CfgData->sensorSensG) * 256) / (CfgData->canonSensG *
                    CfgData->sensorSensR);
        aAwb->precalcData.partToPartGainB = ((CfgData->canonSensB *
                    CfgData->sensorSensG) * 256) / (CfgData->canonSensG *
                    CfgData->sensorSensB);
        aAwb->precalcData.canonSensR = CfgData->canonSensR;
        aAwb->precalcData.canonSensG = CfgData->canonSensG;
        aAwb->precalcData.canonSensB = CfgData->canonSensB;
        aAwb->precalcData.sensorSensR = CfgData->sensorSensR;
        aAwb->precalcData.sensorSensG = CfgData->sensorSensG;
        aAwb->precalcData.sensorSensB = CfgData->sensorSensB;
    } else {
        aAwb->precalcData.partToPartGainR = 256;
        aAwb->precalcData.partToPartGainB = 256;
        aAwb->precalcData.canonSensR = 256;
        aAwb->precalcData.canonSensG = 256;
        aAwb->precalcData.canonSensB = 256;
        aAwb->precalcData.sensorSensR = 256;
        aAwb->precalcData.sensorSensG = 256;
        aAwb->precalcData.sensorSensB = 256;

        if (canonicalValid) {
            aAwb->precalcData.canonSensR = CfgData->canonSensR;
            aAwb->precalcData.canonSensG = CfgData->canonSensG;
            aAwb->precalcData.canonSensB = CfgData->canonSensB;
            aAwb->precalcData.sensorSensR = CfgData->canonSensR;
            aAwb->precalcData.sensorSensG = CfgData->canonSensG;
            aAwb->precalcData.sensorSensB = CfgData->canonSensB;
        } else {
            DEBUG_WARN("AWB canonical gains not set \n");
        }

        if (sensorValid) {
            aAwb->precalcData.canonSensR = CfgData->sensorSensR;
            aAwb->precalcData.canonSensG = CfgData->sensorSensG;
            aAwb->precalcData.canonSensB = CfgData->sensorSensB;
            aAwb->precalcData.sensorSensR = CfgData->sensorSensR;
            aAwb->precalcData.sensorSensG = CfgData->sensorSensG;
            aAwb->precalcData.sensorSensB = CfgData->sensorSensB;
        } else {
            DEBUG_WARN("AWB sensor gains not set \n");
        }
    }

    for(ti = 0; ti < CfgData->bayesTCurveN; ti++) {
        AWB_Bayes_T_Params *params = (AWB_Bayes_T_Params *)&CfgData->bayesT[ti];
        AWB_Bayes_T_Precalc *precalc = (AWB_Bayes_T_Precalc *)&aAwb->precalcData.t[ti];
        uint0p16_t rNrmMid,  bNrmMid;
        sint16p16_t gradHere, perpGradHere, perpOffsetHere;
        sint8p8_t arctanPerpGrad, cosArctanPerpGrad, perpGradHere4Trig;
        int32_t t;
        int32_t qpa, qpb, qpc;
        int32_t  temp1, temp2;

        rMin = max(params->rMin, 0);
        rMax = min(params->rMax, 65536);
        curT = 0L;

        if (params->resPara < 0) {
            params->resPara = 6553;
        }
        if (params->resPerp < 0) {
            params->resPara = 6553;
        }

        for (t = 0,rNrmMid = rMin; rNrmMid < rMax; rNrmMid += params->resPara,t++) {
            qpa = (params->qPathA)/16;
            qpb = (params->qPathB)/16;
            qpc = (params->qPathC)/16;
            temp1 = ((qpa * rNrmMid)/65536)   + qpb;
            temp2 = ((temp1 * rNrmMid)/65536) + qpc;
            bNrmMid = 16 * temp2;
            temp1 = ((2 * qpa * rNrmMid)/65536) + qpb;
            gradHere = 16 * temp1;
            perpGradHere = -((65536 * 256) / (gradHere)) * 256;
            perpOffsetHere = bNrmMid - ((perpGradHere / 256 * rNrmMid / 256));
            perpGradHere4Trig = perpGradHere / 256;

            if (perpGradHere4Trig <= 256 && perpGradHere4Trig >= 0) {
                arctanPerpGrad = perpGradHere4Trig - ((perpGradHere4Trig *
                            perpGradHere4Trig * perpGradHere4Trig) / (3 * 256 * 256))
                    + (((perpGradHere4Trig * perpGradHere4Trig) /
                                256 * (perpGradHere4Trig * perpGradHere4Trig) /
                                256 * perpGradHere4Trig) / (5 * 65536));
            } else if (perpGradHere4Trig <0 && perpGradHere4Trig >= -256) {
                arctanPerpGrad = -(perpGradHere4Trig - ((perpGradHere4Trig *
                                perpGradHere4Trig * perpGradHere4Trig) / (3 * 256 * 256))
                        + (((perpGradHere4Trig * perpGradHere4Trig) /
                                256 * (perpGradHere4Trig * perpGradHere4Trig) /
                                256 * perpGradHere4Trig) / (5 *65536)));
            } else if (perpGradHere4Trig > 256) {
                perpGradHere4Trig = 65536 / perpGradHere4Trig;
                arctanPerpGrad = ((804) / 2) - (perpGradHere4Trig - ((perpGradHere4Trig *
                                perpGradHere4Trig * perpGradHere4Trig) / (3 * 256 * 256))
                        + (((perpGradHere4Trig * perpGradHere4Trig) /
                                256 * (perpGradHere4Trig * perpGradHere4Trig) /
                                256 * perpGradHere4Trig) / (5 * 65536)));
            } else if (perpGradHere4Trig < -256) {
                perpGradHere4Trig = 65536 / perpGradHere4Trig;
                arctanPerpGrad = ((-804) / 2) - (perpGradHere4Trig - ((perpGradHere4Trig *
                                perpGradHere4Trig * perpGradHere4Trig) / (3 * 256 * 256))
                        + (((perpGradHere4Trig * perpGradHere4Trig) /
                                256 * (perpGradHere4Trig * perpGradHere4Trig) /
                                256 * perpGradHere4Trig) / (5 * 65536)));
            }

            cosArctanPerpGrad = (256 - ((arctanPerpGrad * arctanPerpGrad) /
                        (2 * 1 * 256)) + (((arctanPerpGrad * arctanPerpGrad) /
                                256 * (arctanPerpGrad * arctanPerpGrad) / 256) /
                            (4 * 3 * 2 * 1 *  256)));
            sint0p16_t distFromMid;
            uint0p16_t stdNeg, stdPos;
            uint16_t   tInv2varPos, tInv2varNeg;
            ISYS_PWLPointType pt;

            pt.x = max(min(rNrmMid / 256, 255), 0);
            pt.y = 0;
            params->stdPosFunc.pts = &params->stdPosPts[0];
            params->stdNegFunc.pts = &params->stdNegPts[0];
            if (0 != ISYS_PWLEvaluate(&params->stdPosFunc, &pt)) {
                DEBUG_ERR("AWB: AWB.BAYES_T_%d.stdPosFunc not complete (no \
                           output for %d).\n", ti, pt.x);
                pt.y = 6553;
            }
            stdPos = pt.y;

            if (0 != ISYS_PWLEvaluate(&params->stdNegFunc, &pt)) {
                DEBUG_ERR("ERROR: AWB: AWB.BAYES_T_%d.stdNegFunc not complete \
                           (no output for %d).\n", ti, pt.x);
                pt.y = 6553;
            }
            stdNeg = pt.y;

            tInv2varPos = (((65536 * 256) / (stdPos * stdPos)) / 2) * 256;
            tInv2varNeg = (((65536 * 256) / (stdNeg * stdNeg)) / 2) * 256;

            for (distFromMid = 0; distFromMid < stdPos * 2  &&
                    curT < MAX_T_POINTS_PER_CURVE; distFromMid += params->resPerp, curT++) {
                ret = Precalc1Lamp(precalc, CfgData, tInv2varPos,rNrmMid, distFromMid,
                        distFromMid, perpGradHere, cosArctanPerpGrad,
                        perpOffsetHere, curT);
                if (BCM_ERR_OK != ret) {
                    goto exit;
                }
            }

            for (distFromMid = -params->resPerp; distFromMid > -stdNeg * 2 &&
                    curT < MAX_T_POINTS_PER_CURVE; distFromMid -= params->resPerp, curT++) {
                ret = Precalc1Lamp(precalc, CfgData, tInv2varNeg, rNrmMid, distFromMid,
                        distFromMid, perpGradHere, cosArctanPerpGrad,
                        perpOffsetHere, curT);
                if (BCM_ERR_OK != ret) {
                    goto exit;
                }
            }
        }

        precalc->numTPoints = curT;

        if(curT > MAX_T_POINTS_PER_CURVE) {
            DEBUG_ERR("AWB Error: Exceeded number of valid points in a curve; truncating.");
            ret = BCM_ERR_UNKNOWN;
            goto exit;
        }
    }

    /* Precalculate lookup tables for PDF intensity dependencies from the PWL
     * functions supplied */
    for (ipdf = 0; ipdf < CfgData->bayes_S_T_PDFsTexN; ipdf++) {
        int32_t i;

        CfgData->bayes_S_T_PDFsTex[ipdf].wInt.pts = &CfgData->bayes_S_T_PDFsTex[ipdf].wIntPts[0];
        for(i = 0; i < 128; i++)
        {
            ISYS_PWLPointType pt;
            int32_t pwl_retval;

            pt.x = i * 4 - 256;
            pwl_retval = ISYS_PWLEvaluate(&(CfgData->bayes_S_T_PDFsTex[ipdf].wInt), &pt);
            if(pwl_retval != 0)
                pt.y = 0;

            aAwb->precalcData.pdfWeightsInt[ipdf][i] = pt.y;
        }
    }

    /* Convert to raster format, and calculate bitfield */
    ret = UpdatePDFs(CfgData, aAwb);
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: UpdatePDFs failed\n", __FUNCTION__);
    }

exit:
    return ret;
}
