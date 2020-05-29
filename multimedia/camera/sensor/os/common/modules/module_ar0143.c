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
  File Name:  module_AR0143.c
Description: This file exposes AR0143 camera module interfaces.
 ******************************************************************************/

/* Includes */
#include <string.h>
#include <system.h>
#include "camera_sensor_module.h"

/* Default is RAW16 */
#ifndef RAW12 /* RAW16 */
#define HIST_OFFSET (0x0014UL)
#else
#define HIST_OFFSET (0x000FUL)
#endif /* !RAW12 */

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
// #define AR0143_ENABLE_PARSE_METADATA
// #define AR0143_ENABLE_GETOPS
// #define AR0143_ENABLE_GET_CLOSEST_GAIN
#define AR0143_ENABLE_PARSE_STATISTICS
// #define AR0143_ENABLE_CONFIGURE

/* Macros */
/* Register addresses */
#define COARSE_INT_TIME_REG          (0x3012UL)
#define GROUP_PARAMETER_HOLD         (0x3022UL)
#define WB_GAIN_B_REG                (0x3058UL)
#define WB_GAIN_R_REG                (0x305AUL)
#define DIGITAL_GAIN_PDLO_GL_REG     (0x3308UL)
#define CONV_GAIN                    (0x3362UL)
#define ANALOG_GAIN_REG              (0x3366UL)

#ifdef RAW12
/* Metadata Offsets for RAW12 format */
#define FRAME_COUNT_3             (0x0009UL)
#define FRAME_COUNT_2             (0x000CUL)
#define FRAME_COUNT_1             (0x000FUL)
#define FRAME_COUNT_0             (0x0012UL)
#define COARSE_INT_TIME_H         (0x0471UL)
#define COARSE_INT_TIME_L         (0x0474UL)
#define ANALOG_GAIN_H             (0x0555UL)
#define ANALOG_GAIN_L             (0x0558UL)
#define WB_GAIN_R_H               (0x0543UL)
#define WB_GAIN_R_L               (0x0546UL)
#define WB_GAIN_B_H               (0x053DUL)
#define WB_GAIN_B_L               (0x0540UL)
#else /* RAW16 */
/* Metadata Offsets for RAW16 format */
#define FRAME_COUNT_3             (0x000CUL)
#define FRAME_COUNT_2             (0x0010UL)
#define FRAME_COUNT_1             (0x0014UL)
#define FRAME_COUNT_0             (0x0018UL)
#define COARSE_INT_TIME_H         (0x05ecUL)
#define COARSE_INT_TIME_L         (0x05f0UL)
#define ANALOG_GAIN_H             (0x071cUL)
#define ANALOG_GAIN_L             (0x0720UL)
#define WB_GAIN_R_H               (0x0704UL)
#define WB_GAIN_R_L               (0x0708UL)
#define WB_GAIN_B_H               (0x06fcUL)
#define WB_GAIN_B_L               (0x0700UL)
#define PRE_HDR_GAIN_E            (0x0930UL)
#endif

/* Conversion Gain Computed Value = 2.875 */
#define CONVERSION_GAIN              (0x02E0UL)

/* Conversion Gain Enable */
#define CONVERSION_GAIN_EN_3EXP      (0x07UL)
#define CONVERSION_GAIN_EN_2EXP      (0x03UL)
#define CONVERSION_GAIN_EN_1EXP      (0x01UL)

/* Digital Gain format: xx.yyyyyyyyy */
#define MAX_DIGITAL_GAIN_PDLO        (0x07FFUL)

/* Analog Gain code and corresponding value in 8p8 format*/
#define ANALOG_GAIN_CODE_1x          (0x0007UL)
#define ANALOG_GAIN_VAL_1x           (0x0100UL)
#define ANALOG_GAIN_CODE_1p25x       (0x0008UL)
#define ANALOG_GAIN_VAL_1p25x        (0x0140UL)
#define ANALOG_GAIN_CODE_1p5x        (0x0009UL)
#define ANALOG_GAIN_VAL_1p5x         (0x0180UL)
#define ANALOG_GAIN_CODE_2x          (0x000AUL)
#define ANALOG_GAIN_VAL_2x           (0x0200UL)
#define ANALOG_GAIN_CODE_2p33x       (0x000BUL)
#define ANALOG_GAIN_VAL_2p33x        (0x0255UL)
#define ANALOG_GAIN_CODE_3p5x        (0x000CUL)
#define ANALOG_GAIN_VAL_3p5x         (0x0380UL)
#define ANALOG_GAIN_CODE_4x          (0x000DUL)
#define ANALOG_GAIN_VAL_4x           (0x0400UL)
#define ANALOG_GAIN_CODE_8x          (0x000EUL)
#define ANALOG_GAIN_VAL_8x           (0x0800UL)

/* Camera Sensor API IDs for error reporting */
/* Range:  0x30 - 0x3f */
#define AR0143_API_PARSE_METADATA    (0x30U)
#define AR0143_API_PARSE_STATISTICS  (0x31U)
#define AR0143_API_GET_OPS           (0x32U)
#define AR0143_API_GET_CLOSEST_GAIN  (0x33U)
#define AR0143_API_CONFIGURE         (0x34U)

/* Histogram offsets */
#define AR0143_HISTOGRAM_BINS        (0X0F4U)

/* Globals */
/* AG Register backup used for update */
#if defined AR0143_ENABLE_GETOPS || \
    defined AR0143_ENABLE_CONFIGURE
static uint8_t  gAr0143_ReadMode = 0U;
static uint16_t gRGain = 256U;
static uint16_t gBGain = 256U;
#endif

/* Conversion Gain Enable backup used for update */
#if defined AR0143_ENABLE_GETOPS || \
    defined AR0143_ENABLE_CONFIGURE || \
    defined AR0143_ENABLE_PARSE_METADATA
static uint8_t  gConverGainEn = 0U;
static uint16_t gDgain = 256U;
static uint16_t gFrameQunat = 0U;
#endif

#ifdef AR0143_ENABLE_PARSE_METADATA
/* AR0143_ParseMetadata */
static int32_t AR0143_ParseMetadata(const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                    uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                    CSN_MODULE_MetadataType * const aMetadata)
{
    static uint16_t digitalGainReg = 256U;
    uint16_t wbGainReg;
    uint16_t agVal;
    int32_t ret = BCM_ERR_OK;

    if ((NULL == aBuffer) || (NULL == aBuffer[0]) || (NULL == aMetadata)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            AR0143_API_PARSE_METADATA,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    /* Parse metadata */
    /* Update frame count */
    aMetadata->frameNum = (((uint32_t)(aBuffer[0][FRAME_COUNT_3] & 0xffUL) << 24UL) |
                           ((uint32_t)(aBuffer[0][FRAME_COUNT_2] & 0xffUL) << 16UL) |
                           ((uint32_t)(aBuffer[0][FRAME_COUNT_1] & 0xffUL) << 8UL) |
                           ((uint32_t)(aBuffer[0][FRAME_COUNT_0] & 0xffUL)));
    /* Exposure */
    aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] =
                       ((((uint32_t)aBuffer[0][COARSE_INT_TIME_H] & 0xffUL) << 8UL) |
                        ((uint32_t)aBuffer[0][COARSE_INT_TIME_L] & 0xffUL));

    agVal =   (uint16_t) ((((uint32_t)aBuffer[0][ANALOG_GAIN_H] & 0xffUL) << 8UL)|
                         ((uint32_t)aBuffer[0][ANALOG_GAIN_L] & 0xffUL));

    gFrameQunat++;

    if (gFrameQunat == 5UL) {
        digitalGainReg = gDgain;
    }

    /* Analog gain */
    switch (agVal & 0x000fU) {
        case ANALOG_GAIN_CODE_1x:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_1x;
            break;
        case ANALOG_GAIN_CODE_1p25x:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_1p25x;
            break;
        case ANALOG_GAIN_CODE_1p5x:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_1p5x;
            break;
        case ANALOG_GAIN_CODE_2x:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_2x;
            break;
        case ANALOG_GAIN_CODE_2p33x:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_2p33x;
            break;
        case ANALOG_GAIN_CODE_3p5x:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_3p5x;
            break;
        case ANALOG_GAIN_CODE_4x:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_4x;
            break;
        default:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = ANALOG_GAIN_VAL_8x;
            break;
    }

    if (0UL != gConverGainEn) {
        aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] =
            ((uint32_t) aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] * CONVERSION_GAIN) >> 8UL;
    }

    aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] =
            ((uint32_t)aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG]*digitalGainReg) >> 8UL;

    wbGainReg = (uint16_t)
                ((((uint32_t)aBuffer[0][WB_GAIN_R_H] & 0xffUL) << 8UL)|
                ((uint32_t)aBuffer[0][WB_GAIN_R_L] & 0xffUL));

    wbGainReg = (uint16_t)(((uint32_t)wbGainReg << 8UL) >> 7UL);
    aMetadata->gainR = wbGainReg;

    wbGainReg = (uint16_t)
                ((((uint32_t)aBuffer[0][WB_GAIN_B_H] & 0xffUL) << 8UL)|
                ((uint32_t)aBuffer[0][WB_GAIN_B_L] & 0xffUL));
    wbGainReg = (uint16_t)(((uint32_t)wbGainReg << 8UL) >> 7UL);
    aMetadata->gainB = wbGainReg;

    aMetadata->wbEnabled = (0UL != (aBuffer[0][PRE_HDR_GAIN_E] & 0x10U)) ? 1U : 0U;

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
#endif /* AR0143_ENABLE_PARSE_METADATA */

#ifdef AR0143_ENABLE_PARSE_STATISTICS
/* AR0143_ParseStatistics */
static int32_t AR0143_ParseStatistics (const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                        CSN_MODULE_StatsType * const (aSensorStats)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t *aParsedMask)
{
    static uint8_t isAbscFilled = 0UL;
    uint32_t i = 0UL;
    int32_t ret = BCM_ERR_OK;
    uint8_t *histogramPtr;
    uint32_t temp;

    if ((NULL == aBuffer) || (NULL == aBuffer[0]) || (NULL == aSensorStats) ||
        (NULL == aSensorStats[0])) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            AR0143_API_PARSE_STATISTICS,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

#ifdef RAW12
    /* Parse Histogram */
    histogramPtr = (uint8_t *) aBuffer[0] + HIST_OFFSET;
    for (i = 0UL; i < (AR0143_HISTOGRAM_BINS * 3UL); i += 3UL){
        aSensorStats[0]->histOrdinates[i/3] = (((*(histogramPtr + i)<<2) |
                                             ((*(histogramPtr + i +2)>>2) & 3))<<10) |
                                             ((*(histogramPtr + i + 1)<<2) |
                                             ((*(histogramPtr + i +2)>>6) & 3));
    }

#else /* RAW16 */
    /* Parse Histogram */
    histogramPtr = (uint8_t *) aBuffer[0] + HIST_OFFSET;
    for (i = 0UL; i < (AR0143_HISTOGRAM_BINS * 4UL); i += 4UL){
        aSensorStats[0]->histOrdinates[i/4] = (((*(histogramPtr + i)<<2) |
                                             (( *(histogramPtr + i + 1)>>6) & 3))<<10) |
                                             ((*(histogramPtr + i + 2)<<2) |
                                             (( *(histogramPtr+ i + 3)>>6) & 3));
    }
#endif

    if (0U == isAbscFilled) {
        /* Fill In Histogram Abcissae Needs to be done once per program execution */
        for (i = 0; i < 17; i++) {
            aSensorStats[0]->histAbcissae[i] = i * 8 ;
        }
        for (i = 17; i < 25; i++) {
            aSensorStats[0]->histAbcissae[i] = aSensorStats[0]->histAbcissae[i-1]+16 ;
        }
        for (i = 25; i < 33; i++) {
            aSensorStats[0]->histAbcissae[i] = aSensorStats[0]->histAbcissae[i-1]+32 ;
        }
        for (i = 33; i < 41; i++) {
            aSensorStats[0]->histAbcissae[i] = aSensorStats[0]->histAbcissae[i-1]+64;
        }
        for (i = 41; i < 65; i++) {
            aSensorStats[0]->histAbcissae[i] = aSensorStats[0]->histAbcissae[i-1]+128;
        }
        for (i = 65; i < 185; i++) {
            aSensorStats[0]->histAbcissae[i] = aSensorStats[0]->histAbcissae[i-1]+512;
        }
        for (i = 185; i < 244; i++) {
            aSensorStats[0]->histAbcissae[i] = aSensorStats[0]->histAbcissae[i-1]+16384;
        }

        /*Compress Abcissae Using Black Level Block */
        for (i = 0; i<244 ; i++) {
            temp=aSensorStats[0]->histAbcissae[i];
            if (temp>=16384UL && temp<32768UL) {
                temp = ((((temp - 16384UL)*((8192UL<<8UL)/16384UL))>>8UL)+16384UL);
            } else if (temp>=32768UL && temp<65536UL) {
                temp = ((((temp - 32768UL)*((8192UL<<8UL)/32768UL))>>8UL)+24576UL);
            } else if (temp>=65536UL && temp<131702UL) {
                temp = ((((temp - 65536UL)*((8192UL<<8UL)/65536UL))>>8UL)+32768UL);
            } else if (temp>=131702UL && temp<262144UL) {
                temp = ((((temp - 131702UL)*((8192UL<<8UL)/131702UL))>>8UL)+40960UL);
            } else if (temp>=262144UL && temp<524288UL) {
                temp = ((((temp - 262144UL)*((8192UL<<8UL)/262144UL))>>8UL)+49152UL);
            } else if (temp>=524288UL && temp<1048577UL) {
                temp = ((((temp - 524288UL)*((8192UL<<8UL)/524288UL))>>8UL)+57344UL);
            }
            aSensorStats[0]->histAbcissae[i]= temp;
        }
        isAbscFilled = 1U;
    }

    aSensorStats[0]->numValidBins = 244UL;
    aSensorStats[0]->blBin = 19UL;

    if (NULL != aParsedMask) {
        *aParsedMask = CSN_MODULE_STATS_PARSED_LONG;
    }
func_exit:
    return ret;
}
#endif /* AR0143_ENABLE_PARSE_STATISTICS */

#ifdef AR0143_ENABLE_GETOPS
/* AR0143_GetOps */
static int32_t AR0143_GetOps(uint32_t aOpsType,
                             CSN_MODULE_OpsParamType * const aOpsParam,
                             CSN_MODULE_SetOpsType * const aOps)
{
    int32_t ret = BCM_ERR_OK;
    uint16_t analogGain = 0U;
    uint16_t digitalGain = 0U;
    uint32_t expLines = 0UL;
    uint16_t agVal = 0U;
    CSN_MODULE_ExpParamType *expP = NULL;
#ifdef PRINT_EXP
    uint16_t exposure[CSN_MODULE_EXP_TIME_IDX_MAX];
#endif
#ifdef PRINT_GAIN
    uint16_t gain[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_MAX_GAIN_COMPS];
#endif
#ifdef PRINT_WB
    uint16_t wbGain[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_WG_GAIN_IDX_MAX];
#endif
#ifdef PRINT_WB_OFFSET
    uint16_t wbGainOffset[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_WG_GAIN_IDX_MAX];
#endif

#if defined PRINT_GAIN || defined PRINT_WB || defined PRINT_WB_OFFSET || defined PRINT_EXP
    uint32_t i;
#endif

#if defined PRINT_GAIN || defined PRINT_WB || defined PRINT_WB_OFFSET
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
#if defined PRINT_WB || defined PRINT_WB_OFFSET
    /* Initialize gain */
    for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
        for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
#ifdef PRINT_WB
            wbGain[i][j] = 256U;
#endif
#ifdef PRINT_WB_OFFSET
            wbGainOffset[i][j] = 0U;
#endif
        }
    }
#endif

    if (NULL == aOps || NULL == aOpsParam) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            AR0143_API_GET_OPS,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    aOps->count = 0UL;
    if ((NULL == aOpsParam->expParam) || (NULL == aOpsParam->wbParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            AR0143_API_GET_OPS,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_WB)) {
        if (aOpsParam->wbParam->gainR == 0UL) {
            gRGain = 256UL;
        } else {
            gRGain = aOpsParam->wbParam->gainR;
        }

        if (aOpsParam->wbParam->gainB == 0UL) {
            gBGain = 256UL;
        } else {
            gBGain = aOpsParam->wbParam->gainB;
        }
#if defined PRINT_WB || defined PRINT_WB_OFFSET
        /* Copy wb gain */
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                if (CSN_MODULE_WG_GAIN_IDX_R == j) {
#ifdef PRINT_WB
                    wbGain[i][j] = gRGain;
#endif
                } else if (CSN_MODULE_WG_GAIN_IDX_B == j) {
#ifdef PRINT_WB
                    wbGain[i][j] = gBGain;
#endif
                } else {
#ifdef PRINT_WB
                    wbGain[i][j] = 256U;
#endif
                }
            }
        }
#endif
        goto func_exit;
    }
    /* Assign exposure pointer */
    expP = aOpsParam->expParam;

    /*Group Parameter hold: set 0x301A[15]: 1 to inhibit register update*/
    aOps->op[aOps->count].reg = GROUP_PARAMETER_HOLD;
    aOps->op[aOps->count].val = 0xffffU;
    aOps->count++;

    if (0UL != (aOpsType & CSN_MODULE_OP_EXPOSURE_LONG)) {
        expLines = (uint32_t) expP->expLines[CSN_MODULE_EXP_IDX_LONG];

#ifdef PRINT_EXP
        exposure[0] = expLines;
#endif

        aOps->op[aOps->count].reg = COARSE_INT_TIME_REG;
        aOps->op[aOps->count].val = expLines;
        aOps->count++;

        uint16_t gainR = gRGain >> 1UL;
        uint16_t gainB = gBGain >> 1UL;
        uint32_t GainLR_Addr = 0UL;
        uint32_t GainLB_Addr = 0UL;

        switch ((gAr0143_ReadMode & 0xcU) >> 2U) {
            case 1U:
                GainLR_Addr = WB_GAIN_R_REG;
                GainLB_Addr = WB_GAIN_B_REG;
                break;
            case 2U:
                GainLR_Addr = WB_GAIN_R_REG;
                GainLB_Addr = WB_GAIN_B_REG;
                break;
            case 3U:
                GainLR_Addr = WB_GAIN_R_REG;
                GainLB_Addr = WB_GAIN_B_REG;
                break;
            default:
                GainLR_Addr = WB_GAIN_R_REG;
                GainLB_Addr = WB_GAIN_B_REG;
                break;
        }

        /* R gain - LONG */
        aOps->op[aOps->count].reg = GainLR_Addr;
        aOps->op[aOps->count].val = (uint32_t) gainR;
        aOps->count++;

        /* B Gain - Long */
        aOps->op[aOps->count].reg = GainLB_Addr;
        aOps->op[aOps->count].val = (uint32_t) gainB;
        aOps->count++;
    }
    if (0UL != (aOpsType & CSN_MODULE_OP_GAIN_LONG)) {
        uint32_t totalGain = (uint32_t) expP->gain[CSN_MODULE_EXP_IDX_LONG];
        uint32_t reCalcGain = 0UL;

        if (totalGain >= ANALOG_GAIN_VAL_1x && totalGain < ANALOG_GAIN_VAL_1p25x){
            analogGain = ANALOG_GAIN_CODE_1x;
            digitalGain = (uint16_t)((totalGain * 512)/256);
        }
        else if (totalGain >= ANALOG_GAIN_VAL_1p25x && totalGain < ANALOG_GAIN_VAL_1p5x){
            analogGain = ANALOG_GAIN_CODE_1p25x;
            digitalGain = (uint16_t)((totalGain * 512U)/320U);
        }
        else if (totalGain >= ANALOG_GAIN_VAL_1p5x && totalGain < ANALOG_GAIN_VAL_2x){
            analogGain = ANALOG_GAIN_CODE_1p5x;
            digitalGain = (uint16_t)((totalGain * 512U)/384U);
        }
        else if (totalGain >= ANALOG_GAIN_VAL_2x && totalGain < ANALOG_GAIN_VAL_2p33x){
            analogGain = ANALOG_GAIN_CODE_2x;
            digitalGain = (uint16_t)((totalGain * 512U)/512U);
        }
        else if (totalGain >= ANALOG_GAIN_VAL_2p33x && totalGain < ANALOG_GAIN_VAL_3p5x){
            analogGain = ANALOG_GAIN_CODE_2p33x;
            digitalGain = (uint16_t)((totalGain * 512U)/597U);
        }
        else if (totalGain >= ANALOG_GAIN_VAL_3p5x && totalGain < ANALOG_GAIN_VAL_4x){
            analogGain = ANALOG_GAIN_CODE_3p5x;
            digitalGain = (uint16_t)((totalGain * 512U)/896U);
        }
        else if (totalGain >= ANALOG_GAIN_VAL_4x && totalGain < ANALOG_GAIN_VAL_8x){
            analogGain = ANALOG_GAIN_CODE_4x;
            digitalGain = (uint16_t)((totalGain * 512U)/1024U);
        }
        else{
            analogGain = ANALOG_GAIN_CODE_8x;
            digitalGain = (uint16_t)((totalGain * 512U)/2048U);
        }
        /* Default DCG is disabled */
        gConverGainEn = 0x0U;
        if (totalGain > ANALOG_GAIN_VAL_8x) {
            /* For three Exposures set to 7 */
            gConverGainEn = CONVERSION_GAIN_EN_3EXP;
            /* DCG gain after experimentation was found to be (1/2.877) = 356/1024 */
            reCalcGain = (uint16_t)((totalGain * 356UL)/1024UL);
            if (reCalcGain >= ANALOG_GAIN_VAL_2p33x && reCalcGain < ANALOG_GAIN_VAL_3p5x) {
                analogGain = ANALOG_GAIN_CODE_2p33x;
                /* Scale = 1/2.33 */
                digitalGain = (uint16_t)((reCalcGain * 2U * 439U)/1024U);
            }
            else if (reCalcGain >= ANALOG_GAIN_VAL_3p5x && reCalcGain < ANALOG_GAIN_VAL_4x) {
                analogGain = ANALOG_GAIN_CODE_3p5x;
                /* Scale = 1/3.50 */
                digitalGain = (uint16_t)((reCalcGain * 2U * 292U)/1024U);
            }
            else if (reCalcGain >= ANALOG_GAIN_VAL_4x && reCalcGain < ANALOG_GAIN_VAL_8x) {
                analogGain = ANALOG_GAIN_CODE_4x;
                /* Scale = 1/4 */
                digitalGain = (uint16_t)((reCalcGain * 2U * 256U)/1024U);
            }
            else{
                analogGain = ANALOG_GAIN_CODE_4x;
                /* Scale = 1/4 */
                digitalGain = (uint16_t)((reCalcGain * 2U * 256U)/1024U);
            }
        }
        if (digitalGain > MAX_DIGITAL_GAIN_PDLO) {
            digitalGain = MAX_DIGITAL_GAIN_PDLO;
        }

        analogGain = (analogGain & 0x000fU);
        agVal |= (analogGain << 8UL) | (analogGain << 4UL) | (analogGain << 0UL);

        aOps->op[aOps->count].reg = ANALOG_GAIN_REG;
        aOps->op[aOps->count].val = agVal;
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_PDLO_GL_REG;
        aOps->op[aOps->count].val = digitalGain;
        aOps->count++;

        aOps->op[aOps->count].reg = CONV_GAIN;
        aOps->op[aOps->count].val = gConverGainEn;
        aOps->count++;

#ifdef PRINT_GAIN
        gain[0][0] = gConverGainEn;
        gain[0][1] = agVal;
        gain[0][2] = digitalGain >> 1UL;
#endif
        gFrameQunat = 1UL;
        gDgain = (digitalGain >> 1UL);
    }

    /* Group Parameter hold: set 0x301A[15]: 0 to
       apply pending register update on next frame */
    aOps->op[aOps->count].reg = GROUP_PARAMETER_HOLD;
    aOps->op[aOps->count].val = 0U;
    aOps->count++;

func_exit:
#if defined PRINT_GAIN || defined PRINT_WB || defined PRINT_WB_OFFSET || defined PRINT_EXP
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
#endif

#if defined PRINT_WB || defined PRINT_WB_OFFSET
    if (0UL != (aOpsType & CSN_MODULE_OP_WB)) {
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
#ifdef PRINT_WB
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                ULOG_INFO("wbG%d_%d: %d ", j, i, wbGain[i][j]);
            }
#endif
#ifdef PRINT_WB_OFFSET
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                ULOG_INFO("wbO%d_%d: %d ", j, i, wbGainOffset[i][j]);
            }
#endif
#if defined PRINT_WB ||  defined PRINT_WB_OFFSET
            ULOG_INFO(" <ref>\n");
#endif
        }
    }
#endif
    return ret;
}
#endif /* AR0143_ENABLE_GETOPS */

#ifdef AR0143_ENABLE_GET_CLOSEST_GAIN
/* AR0143_GetClosestGain */
static int32_t AR0143_GetClosestGain(uint32_t aOpsType, uint16_t * const aGain)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aGain) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            AR0143_API_GET_CLOSEST_GAIN,
                            ret,
                            0UL, NULL);
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
#endif /* AR0143_ENABLE_GET_CLOSEST_GAIN */

#ifdef AR0143_ENABLE_CONFIGURE
/* AR0143_Configure */
static int32_t AR0143_Configure(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            AR0143_API_CONFIGURE,
                            ret,
                            (uint32_t)(__LINE__),
                            NULL);
        goto func_exit;
    }
    gAr0143_ReadMode = 0U;
    gConverGainEn = 0U;
    gRGain = 256U;
    gBGain = 256U;
    gDgain = 256U;
    gFrameQunat = 0U;

func_exit:
    return ret;
}
#endif /* AR0143_ENABLE_CONFIGURE */

/* gAR0143_Module */
static const CSN_MODULE_Type gAR0143_Module =
{
#ifdef AR0143_ENABLE_PARSE_METADATA
    .parseMetadata = AR0143_ParseMetadata,
#else
    .parseMetadata = NULL,
#endif
#ifdef AR0143_ENABLE_GETOPS
    .getOps = AR0143_GetOps,
#else
    .getOps = NULL,
#endif
#ifdef AR0143_ENABLE_GET_CLOSEST_GAIN
    .getClosestGain = AR0143_GetClosestGain,
#else
    .getClosestGain = NULL,
#endif
#ifdef AR0143_ENABLE_PARSE_STATISTICS
    .parseStatistics = AR0143_ParseStatistics,
#else
    .parseStatistics = NULL,
#endif
#ifdef AR0143_ENABLE_CONFIGURE
    .sensorConfigure = AR0143_Configure,
#else
    .sensorConfigure = NULL,
#endif
};

/* CSN_MODULE_GetModule */
const CSN_MODULE_Type * CSN_MODULE_GetModule(void)
{
    return &gAR0143_Module;
}
