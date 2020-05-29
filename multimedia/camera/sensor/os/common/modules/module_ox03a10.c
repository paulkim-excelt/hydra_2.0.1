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
  File Name:  module_ox03a10.c
Description: This file exposes 0X03A10 camera module interfaces.
 ******************************************************************************/

/* Includes */
#include <string.h>
#include <system.h>
#include "camera_sensor_module.h"
//#define PRINT_EXP
//#define PRINT_GAIN
//#define PRINT_WB
//#define PRINT_WB_OFFSET
//#define PRINT_MD

#if defined PRINT_EXP || \
    defined PRINT_GAIN || \
    defined PRINT_WB || \
    defined PRINT_WB_OFFSET \
    || defined PRINT_MD
#include <ulog.h>
#define GetModuleLogLevel() ULOG_LVL_INFO
#endif

/* Enables for module specific functions */
// #define OX03A10_ENABLE_PARSE_METADATA
// #define OX03A10_ENABLE_GETOPS
// #define OX03A10_ENABLE_GET_CLOSEST_GAIN
// #define OX03A10_ENABLE_PARSE_STATISTICS
// #define OX03A10_ENABLE_CONFIGURE

/* Macros */
/* Register addresses */
#define COARSE_INT_TIME_REG_H          (0x3501UL)
#define COARSE_INT_TIME_REG_L          (0x3502UL)
#define ANALOG_GAIN_REG_H_HCG          (0x3508UL)
#define ANALOG_GAIN_REG_L_HCG          (0x3509UL)
#define DIGITAL_GAIN_REG_H_HCG         (0x350aUL)
#define DIGITAL_GAIN_REG_M_HCG         (0x350bUL)
#define DIGITAL_GAIN_REG_L_HCG         (0x350cUL)
#define ANALOG_GAIN_REG_H_LCG          (0x3548UL)
#define ANALOG_GAIN_REG_L_LCG          (0x3549UL)
#define DIGITAL_GAIN_REG_H_LCG         (0x354aUL)
#define DIGITAL_GAIN_REG_M_LCG         (0x354bUL)
#define DIGITAL_GAIN_REG_L_LCG         (0x354cUL)
#define COARSE_INT_TIME_REG_H_VS       (0x3581UL)
#define COARSE_INT_TIME_REG_L_VS       (0x3582UL)
#define ANALOG_GAIN_REG_H_VS           (0x3588UL)
#define ANALOG_GAIN_REG_L_VS           (0x3589UL)
#define DIGITAL_GAIN_REG_H_VS          (0x358aUL)
#define DIGITAL_GAIN_REG_M_VS          (0x358bUL)
#define DIGITAL_GAIN_REG_L_VS          (0x358cUL)
#define WB_GAIN_HCG_H_B                (0x5180UL)
#define WB_GAIN_HCG_L_B                (0x5181UL)
#define WB_GAIN_HCG_H_R                (0x5186UL)
#define WB_GAIN_HCG_L_R                (0x5187UL)
#define WB_GAIN_LCG_H_B                (0x51A0UL)
#define WB_GAIN_LCG_L_B                (0x51A1UL)
#define WB_GAIN_LCG_H_R                (0X51A6UL)
#define WB_GAIN_LCG_L_R                (0x51A7UL)
#define WB_GAIN_VS_H_B                 (0X51C0UL)
#define WB_GAIN_VS_L_B                 (0x51C1UL)
#define WB_GAIN_VS_H_R                 (0x51C6UL)
#define WB_GAIN_VS_L_R                 (0x51C7UL)
#define GROUP_PARAMETER_HOLD           (0x3208UL)
#define GROUP_MANUAL_LAUNCH_ENABLE     (0x320DUL)

/* Metadata index calculation */
#define RAW16_OFFSET_METADATA          (72U)
#define RAW16_METADATA_SKIP_BYTES      (32U)
#define RAW12_OFFSET_METADATA          (54U)
#define RAW12_METADATA_SKIP_BYTES      (24U)

/* Camera Sensor API IDs for error reporting */
/* Range:  0x40 - 0x4f */
#define OX03A10_API_PARSE_METADATA    (0x40U)
#define OX03A10_API_PARSE_STATISTICS  (0x41U)
#define OX03A10_API_GET_OPS           (0x42U)
#define OX03A10_API_GET_CLOSEST_GAIN  (0x43U)
#define OX03A10_API_CONFIGURE         (0x44U)

#if defined OX03A10_ENABLE_CONFIGURE || \
    defined OX03A10_ENABLE_GETOPS
/* Static for sensor WB */
static uint16_t gRGain = 1024U;
static uint16_t gBGain = 1024U;
#endif

#if defined OX03A10_ENABLE_CONFIGURE || \
    defined OX03A10_ENABLE_PARSE_METADATA
static uint32_t gBPP = 0UL;
#endif

#ifdef OX03A10_ENABLE_PARSE_METADATA
/* OX03A10_getMetaByte */
static uint8_t OX03A10_getMetaByte(const uint8_t * const aBuffer,
                                   uint32_t aIdx)
{
    uint8_t byte = 0U;
    uint32_t idx;

    if (16U == gBPP) {
        idx = RAW16_OFFSET_METADATA + aIdx * RAW16_METADATA_SKIP_BYTES;
        byte = aBuffer[idx];

    } else { /* gBPP == 12 */
        idx = RAW12_OFFSET_METADATA + aIdx * RAW12_METADATA_SKIP_BYTES;
        byte = aBuffer[idx];
    }

    return byte;
}

/* OX03A10_ParseMetadata */
static int32_t OX03A10_ParseMetadata(const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                     uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                     CSN_MODULE_MetadataType * const aMetadata)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t exposureH = 0U;
    uint8_t exposureL = 0U;
    uint8_t aGainLCG_H = 0U;
    uint8_t aGainLCG_L = 0U;
    uint8_t dGainLCG_H = 0U;
    uint8_t dGainLCG_M = 0U;
    uint8_t dGainLCG_L = 0U;
    uint8_t frameCountH = 0U;
    uint8_t frameCountL = 0U;
    uint16_t aGain = 0U;
    uint16_t dGain = 0U;
    uint16_t rGain = 0U;
    uint16_t bGain = 0U;

    if ((NULL == aBuffer) || (NULL == aBuffer[0]) || (NULL == aMetadata)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OX03A10_API_PARSE_METADATA,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }
    /* TODO: Check size of the buffer */

    /* Sequence of Metadata  ini file
    0x3501-0x3502 -->Exposure Lines
    0x3548-0x354C --> LCG Analog/Digital Gain
    0x483E-0x483F --> FRAME NUMBER
    0x5186/0x5180 --> BLUE RED SENSOR AWB GAINS
    */
    exposureH = OX03A10_getMetaByte(aBuffer[0], 0UL);
    exposureL = OX03A10_getMetaByte(aBuffer[0], 1UL);
    aGainLCG_H = OX03A10_getMetaByte(aBuffer[0], 2UL);
    aGainLCG_L = OX03A10_getMetaByte(aBuffer[0], 3UL);
    dGainLCG_H = OX03A10_getMetaByte(aBuffer[0], 4UL);
    dGainLCG_M = OX03A10_getMetaByte(aBuffer[0], 5UL);
    dGainLCG_L = OX03A10_getMetaByte(aBuffer[0], 6UL);
    frameCountH = OX03A10_getMetaByte(aBuffer[0], 7UL);
    frameCountL = OX03A10_getMetaByte(aBuffer[0], 8UL);
    rGain = (((uint16_t) OX03A10_getMetaByte(aBuffer[0], 9UL)) << 8UL) |
              (uint16_t) OX03A10_getMetaByte(aBuffer[0], 10UL);
    bGain = (((uint16_t) OX03A10_getMetaByte(aBuffer[0], 11UL)) <<8UL) |
              (uint16_t) OX03A10_getMetaByte(aBuffer[0], 12UL);

    /* Exposure */
    aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] =
                ((uint32_t)((exposureH & 0xffUL) << 8UL))| ((exposureL));

    /* aGain 5P6 */
    aGain = (((uint16_t) aGainLCG_H & 0x1fU) << 6U) |
            (((uint16_t) aGainLCG_L >> 2U) & 0x3fU);

    /* dGain 4p10 */
    dGain =  (((uint16_t) dGainLCG_H & 0x0fU) << 10U) |
             (((uint16_t) dGainLCG_M & 0xffU) << 2U) |
             (((uint16_t) dGainLCG_L >> 6U) & 0x3U);
    aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] =
             (uint16_t)((((dGain * 256U) / 1024U) * ((aGain * 256U) / 64U)) / 256U);
    aMetadata->frameNum = (((uint32_t) frameCountH & 0xffUL) << 8UL) |
                           ((uint32_t) frameCountL & 0xffUL);

    aMetadata->gainR = rGain >> 2UL;
    aMetadata->gainB = bGain >> 2UL;
    aMetadata->wbEnabled = (0x81U == (OX03A10_getMetaByte(aBuffer[0], 13UL) &
                                                            0x81U)) ? 1U : 0U;
#ifdef PRINT_MD
    ULOG_INFO("FN: %d, SH: %d , rGain: %d, bGain: %d, wbEnabled: %d, aGain: %d (Ref)\n", aMetadata->frameNum,
                                                         aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG],
                                                         aMetadata->gainR,
                                                         aMetadata->gainB,
                                                         aMetadata->wbEnabled,
                                                         aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG]);
#endif

func_exit:
    return ret;
}
#endif /* OX03A10_ENABLE_PARSE_METADATA */

#ifdef OX03A10_ENABLE_PARSE_STATISTICS
/* OX03A10_ParseStatistics */
static int32_t OX03A10_ParseStatistics (const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                        CSN_MODULE_StatsType * const (aSensorStats)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t *aParsedMask)
{
    int32_t ret = BCM_ERR_NOSUPPORT;

    return ret;
}
#endif /* OX03A10_ENABLE_PARSE_STATISTICS */

#ifdef OX03A10_ENABLE_GETOPS
/* OX03A10_GetOps */
static int32_t OX03A10_GetOps(uint32_t aOpsType, CSN_MODULE_OpsParamType * const aOpsParam, CSN_MODULE_SetOpsType * const aOps)
{
    int32_t ret = BCM_ERR_OK;
    uint16_t analogGain = 0U;
    uint16_t analogGainHCG = 0U;
    uint16_t analogGainVS = 0U;
    uint16_t VS_Gain_Res = 256U;
    uint16_t digitalGain = 0U;
    uint32_t expLines = 0UL;
    uint16_t expLinesVS = 0U;
    CSN_MODULE_ExpParamType *expP = NULL;
    uint8_t isHDR = 0U;
    uint32_t i;
#ifdef PRINT_EXP
    uint16_t exposure[CSN_MODULE_EXP_TIME_IDX_MAX];
#endif
#ifdef PRINT_GAIN
    uint16_t gain[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_MAX_GAIN_COMPS];
#endif
#ifdef PRINT_WB
    uint16_t wbGain[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_WG_GAIN_IDX_MAX];
    uint16_t wbGainOffset[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_WG_GAIN_IDX_MAX];
#endif
#if defined PRINT_GAIN || defined PRINT_WB
    uint32_t j;
#endif

#ifdef PRINT_EXP
    for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
        exposure[i] = 0U;
    }
#endif
#ifdef PRINT_GAIN
    /* Initialize gain */
    for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
        for (j = 0UL; j < CSN_MODULE_MAX_GAIN_COMPS; j++) {
            gain[i][j] = 256U;
        }
    }
#endif
#ifdef PRINT_WB
    /* Initialize gain */
    for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
        for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
            wbGain[i][j] = 256U;
            wbGainOffset[i][j] = 0U;
        }
    }
#endif

    if ((NULL == aOps) || (NULL == aOpsParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                        0U,
                        OX03A10_API_GET_OPS,
                        ret,
                        (uint32_t) (__LINE__), NULL);
        goto func_exit;
    }

    if ((NULL == aOpsParam->expParam) || (NULL == aOpsParam->wbParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OX03A10_API_GET_OPS,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }


    if (0UL != (aOpsType & CSN_MODULE_OP_WB)) {
        if (aOpsParam->wbParam->gainR == 0UL) {
            gRGain = 1024U;
        } else {
            gRGain = (aOpsParam->wbParam->gainR) * 4U;
        }

        if (aOpsParam->wbParam->gainB == 0UL) {
            gBGain = 1024U;
        } else {
            gBGain = (aOpsParam->wbParam->gainB) * 4U;
        }
        goto func_exit;
    }

    /* Initialize count */
    aOps->count = 0UL;

    /* Assign exposure pointer */
    expP = aOpsParam->expParam;
    isHDR = (expP->expRatio == 16UL)? 1U : 0U;

    /* Add Group Hold */
    aOps->op[aOps->count].reg = GROUP_PARAMETER_HOLD;
    aOps->op[aOps->count].val = 0x00UL;
    aOps->count++;

    if (0UL != (aOpsType & CSN_MODULE_OP_EXPOSURE_LONG)) {
        expLines = (uint32_t) expP->expLines[CSN_MODULE_EXP_IDX_LONG];
        aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_H;
        aOps->op[aOps->count].val = ((expLines >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_L;
        aOps->op[aOps->count].val = expLines & 0xffUL;
        aOps->count++;
#ifdef PRINT_EXP
        exposure[0] = expLines;
#endif
        /* Update VS for HDR */
        if (0UL != isHDR) {
            /* Program Very Short */
            /* Divide by Exposure Ratio HCG to VS */
            expLinesVS = (uint16_t) (expLines >> 4UL);
            if (expLinesVS < 3U) {
                expLinesVS = 3U;
            }

            if(expLinesVS > 16U) {
                VS_Gain_Res = (256U * expLinesVS)/16U;
                expLinesVS = 16U;
            }
            aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_H_VS;
            aOps->op[aOps->count].val = 0UL;
            aOps->count++;

            aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_L_VS;
            aOps->op[aOps->count].val = expLinesVS  & 0xffUL;
            aOps->count++;
#ifdef PRINT_EXP
            exposure[2] = expLinesVS;
#endif
        }
#ifdef PRINT_WB
        wbGain[0][0] = gRGain;
        wbGain[0][3] = gBGain;
#endif

        aOps->op[aOps->count].reg = WB_GAIN_HCG_H_R;
        aOps->op[aOps->count].val = (((uint32_t) gRGain >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_HCG_L_R;
        aOps->op[aOps->count].val = ((uint32_t) gRGain & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_HCG_H_B;
        aOps->op[aOps->count].val = (((uint32_t) gBGain >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_HCG_L_B;
        aOps->op[aOps->count].val = ((uint32_t) gBGain & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_H_R;
        aOps->op[aOps->count].val = (((uint32_t) gRGain >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_L_R;
        aOps->op[aOps->count].val = ((uint32_t) gRGain & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_H_B;
        aOps->op[aOps->count].val = (((uint32_t) gBGain >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_L_B;
        aOps->op[aOps->count].val = ((uint32_t) gBGain & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_H_R;
        aOps->op[aOps->count].val = (((uint32_t) gRGain >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_L_R;
        aOps->op[aOps->count].val = ((uint32_t) gRGain & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_H_B;
        aOps->op[aOps->count].val = (((uint32_t) gBGain >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_L_B;
        aOps->op[aOps->count].val = ((uint32_t) gBGain  & 0xffUL);
        aOps->count++;
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_GAIN_LONG)) {
        uint16_t totalGain = (uint16_t) expP->gain[CSN_MODULE_EXP_IDX_LONG];
        /* Min Analog/Digital Gain LCG is 1.5 Use AG for Noise */
        if (totalGain <= 4096U) {
            analogGain  =  totalGain;
            digitalGain =  1024U;
        } else {
           analogGain = 4096U;
           digitalGain = (totalGain * 1024U)/4096U;
        }
        /* Analog Gain HCG to LCG need to have ratio 1.0625 as per settings */
        aOps->op[aOps->count].reg = ANALOG_GAIN_REG_H_LCG;
        aOps->op[aOps->count].val = ((uint32_t) analogGain) / 256UL;
        aOps->count++;

        aOps->op[aOps->count].reg = ANALOG_GAIN_REG_L_LCG;
        aOps->op[aOps->count].val = (((uint32_t) analogGain >> 2UL) & 0x3fUL) << 2UL;
        aOps->count++;

#ifdef PRINT_GAIN
        gain[0][0] = analogGain;
#endif
        /* HCG to LCG total Gain Ratio not to be same */
        analogGainHCG = (analogGain * 256U)/272U;
        aOps->op[aOps->count].reg = ANALOG_GAIN_REG_H_HCG;
        aOps->op[aOps->count].val = ((uint32_t) analogGainHCG) / 256UL;
        aOps->count++;
        aOps->op[aOps->count].reg = ANALOG_GAIN_REG_L_HCG;
        aOps->op[aOps->count].val = (((uint32_t) analogGainHCG >> 2UL) & 0x3fUL) << 2UL;
        aOps->count++;

#ifdef PRINT_GAIN
        gain[1][0] = analogGainHCG;
#endif
        /* Digital Gains HCG LCG */
        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_H_HCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain >> 0x0aUL) & 0x03UL);
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_M_HCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain >> 0x02UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_L_HCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain & 0x03UL) << 0x06UL);
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_H_LCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain >> 0x0aUL) & 0x03UL);
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_M_LCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain >> 0x02UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_L_LCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain & 0x03UL) << 0x06UL);
        aOps->count++;
#ifdef PRINT_GAIN
        gain[0][1] = digitalGain >> 2;
        gain[1][1] = digitalGain >> 2;
#endif

        /* Gains for VS == Gains for HCG */
        if (0U != isHDR) {
            analogGainVS = (VS_Gain_Res * analogGainHCG) / 256U;
            if (analogGainVS <= 4096U){
                digitalGain =  1024U;
            } else {
                digitalGain = (analogGainVS * 1024U)/4096U;
                analogGainVS = 4096U;
            }
            aOps->op[aOps->count].reg = ANALOG_GAIN_REG_H_VS;
            aOps->op[aOps->count].val = (uint32_t)analogGainVS / 256UL;
            aOps->count++;

            aOps->op[aOps->count].reg = ANALOG_GAIN_REG_L_VS;
            aOps->op[aOps->count].val = (((uint32_t) analogGainVS >> 2UL) & 0x3fUL) << 2UL;
            aOps->count++;

            aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_H_VS;
            aOps->op[aOps->count].val = (((uint32_t) digitalGain >> 0x0aUL) & 0x03UL);
            aOps->count++;

            aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_M_VS;
            aOps->op[aOps->count].val = (((uint32_t) digitalGain >> 0x02UL) & 0xffUL);
            aOps->count++;

            aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_L_VS;
            aOps->op[aOps->count].val = (((uint32_t) digitalGain & 0x03UL) << 0x06UL);
            aOps->count++;
#ifdef PRINT_GAIN
            gain[2][0] = analogGainVS;
            gain[2][1] = digitalGain >> 2;
#endif
        }
    }
    /* Group Parameter Hold End */
    aOps->op[aOps->count].reg = GROUP_PARAMETER_HOLD;
    aOps->op[aOps->count].val = 0x10UL;
    aOps->count++;

    /* Manual Launch Enable */
    aOps->op[aOps->count].reg = GROUP_MANUAL_LAUNCH_ENABLE;
    aOps->op[aOps->count].val = 0x00UL;
    aOps->count++;

    /* Manual Delay Launch */
    aOps->op[aOps->count].reg = GROUP_PARAMETER_HOLD;
    aOps->op[aOps->count].val = 0xe0UL;
    aOps->count++;


    if (0UL == (aOpsType & CSN_MODULE_OP_WB)) {
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
#ifdef PRINT_EXP
            ULOG_INFO("exp_%d: %d ", i, exposure[i]);
#endif
#ifdef PRINT_GAIN
            for (j = 0UL; j < CSN_MODULE_MAX_GAIN_COMPS; j++) {
                ULOG_INFO("G%d_%d: %d ", j, i, gain[i][j]);
            }
#endif
#if defined PRINT_GAIN ||  defined PRINT_EXP
            ULOG_INFO(" <ref>\n");
#endif
        }
    }

#ifdef PRINT_WB
    if (0UL != (aOpsType & CSN_MODULE_OP_WB)) {
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                ULOG_INFO("wbG%d_%d: %d ", j, i, wbGain[i][j]);
            }
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                ULOG_INFO("wbO%d_%d: %d ", j, i, wbGainOffset[i][j]);
            }
            ULOG_INFO(" <ref>\n");
        }
    }
#endif

func_exit:
    return ret;
}
#endif /* OX03A10_ENABLE_GETOPS */

#ifdef OX03A10_ENABLE_GET_CLOSEST_GAIN
/* AR0143_GetClosestGain */
static int32_t OX03A10_GetClosestGain(uint32_t aOpsType, uint16_t * const aGain)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aGain) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OX03A10_API_GET_CLOSEST_GAIN,
                            ret,
                            (uint32_t)(__LINE__),
                            NULL);
        goto func_exit;
    }

    switch(aOpsType) {
        case CSN_MODULE_OP_GAIN_LONG:
             break;
        case CSN_MODULE_OP_GAIN_SHORT:
             break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }

func_exit:
    return ret;
}
#endif /* OX03A10_ENABLE_GET_CLOSEST_GAIN */

#ifdef OX03A10_ENABLE_CONFIGURE
/* OX03A10_Configure */
static int32_t OX03A10_Configure(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OX03A10_API_CONFIGURE,
                            ret,
                            (uint32_t)(__LINE__),
                            NULL);
        goto func_exit;
    }
    gRGain = 1024U;
    gBGain = 1024U;
    gBPP = aConfig->bpp;

func_exit:
    return ret;
}
#endif /* OX03A10_ENABLE_CONFIGURE */

/* gOX03A10_Module */
static const CSN_MODULE_Type gOX03A10_Module =
{
#ifdef OX03A10_ENABLE_PARSE_METADATA
    .parseMetadata = OX03A10_ParseMetadata,
#else
    .parseMetadata = NULL,
#endif
#ifdef OX03A10_ENABLE_GETOPS
    .getOps = OX03A10_GetOps,
#else
    .getOps = NULL,
#endif
#ifdef OX03A10_ENABLE_GET_CLOSEST_GAIN
    .getClosestGain = OX03A10_GetClosestGain,
#else
    .getClosestGain = NULL,
#endif
#ifdef OX03A10_ENABLE_PARSE_STATISTICS
    .parseStatistics = OX03A10_ParseStatistics,
#else
    .parseStatistics = NULL,
#endif
#ifdef OX03A10_ENABLE_CONFIGURE
    .sensorConfigure = OX03A10_Configure,
#else
    .sensorConfigure = NULL,
#endif
};

/* CSN_MODULE_GetModule */
const CSN_MODULE_Type * CSN_MODULE_GetModule(void)
{
    return &gOX03A10_Module;
}
