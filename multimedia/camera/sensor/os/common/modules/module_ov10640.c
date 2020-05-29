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
// #define OV10640_ENABLE_PARSE_METADATA
// #define OV10640_ENABLE_GETOPS
// #define OV10640_ENABLE_GET_CLOSEST_GAIN
// #define OV10640_ENABLE_PARSE_STATISTICS
// #define OV10640_ENABLE_CONFIGURE

/******************************************************************************
  File Name:  module_ov10640.c
Description: This file exposes ov10640 camera module interfaces.
 ******************************************************************************/

/* Macros */
/* Register addresses */
#define METADATA_START_REG    (0x3000UL)
#define LONG_EXP_H_REG        (0x30e6UL)
#define LONG_EXP_L_REG        (0x30e7UL)
#define SHORT_EXP_H_REG       (0x30e8UL)
#define SHORT_EXP_L_REG       (0x30e9UL)
#define VS_EXP_REG            (0x30eaUL)
#define ANALOG_GAIN_REG       (0x30ebUL)
#define LONG_D_GAIN_H_REG     (0x30ecUL)
#define LONG_D_GAIN_L_REG     (0x30edUL)
#define SHORT_D_GAIN_H_REG    (0x30eeUL)
#define SHORT_D_GAIN_L_REG    (0x30efUL)
#define VS_D_GAIN_H_REG       (0x30f0UL)
#define VS_D_GAIN_L_REG       (0x30f1UL)
#define GH_LONG_EXP_H_REG     (0xB0e6UL)
#define GH_LONG_EXP_L_REG     (0xB0e7UL)
#define GH_SHORT_EXP_H_REG    (0xB0e8UL)
#define GH_SHORT_EXP_L_REG    (0xB0e9UL)
#define GH_VS_EXP_REG         (0xB0eaUL)
#define GH_ANALOG_GAIN_REG    (0xB0ebUL)
#define GH_LONG_D_GAIN_H_REG  (0xB0ecUL)
#define GH_LONG_D_GAIN_L_REG  (0xB0edUL)
#define GH_SHORT_D_GAIN_H_REG (0xB0eeUL)
#define GH_SHORT_D_GAIN_L_REG (0xB0efUL)
#define GH_VS_D_GAIN_H_REG    (0xB0f0UL)
#define GH_VS_D_GAIN_L_REG    (0xB0f1UL)
#define GH_GROUP_LENGTH_0     (0x3028UL)
#define GH_GROUP_CTRL         (0x302CUL)
#define GH_OPERATION_CTRL     (0x302FUL)
#define FRAME_COUNT_3_REG     (0x309cUL)
#define FRAME_COUNT_2_REG     (0x309dUL)
#define FRAME_COUNT_1_REG     (0x309eUL)
#define FRAME_COUNT_0_REG     (0x309fUL)
#define READ_MODE_REG         (0x3090UL)
#define R_GAIN_L_H_REG        (0x31c3UL)
#define R_GAIN_L_L_REG        (0x31c4UL)
#define GR_GAIN_L_H_REG       (0x31c5UL)
#define GR_GAIN_L_L_REG       (0x31c6UL)
#define GB_GAIN_L_H_REG       (0x31c7UL)
#define GB_GAIN_L_L_REG       (0x31c8UL)
#define B_GAIN_L_H_REG        (0x31c9UL)
#define B_GAIN_L_L_REG        (0x31caUL)
#define ISP_CTRL_0            (0x3127UL)
#define GH_R_GAIN_L_H_REG     (0xb1c3UL)
#define GH_R_GAIN_L_L_REG     (0xb1c4UL)
#define GH_GR_GAIN_L_H_REG    (0xb1c5UL)
#define GH_GR_GAIN_L_L_REG    (0xb1c6UL)
#define GH_GB_GAIN_L_H_REG    (0xb1c7UL)
#define GH_GB_GAIN_L_L_REG    (0xb1c8UL)
#define GH_B_GAIN_L_H_REG     (0xb1c9UL)
#define GH_B_GAIN_L_L_REG     (0xb1caUL)
#define GH_R_GAIN_S_H_REG     (0xb1cbUL)
#define GH_R_GAIN_S_L_REG     (0xb1ccUL)
#define GH_GR_GAIN_S_H_REG    (0xb1cdUL)
#define GH_GR_GAIN_S_L_REG    (0xb1ceUL)
#define GH_GB_GAIN_S_H_REG    (0xb1cfUL)
#define GH_GB_GAIN_S_L_REG    (0xb1d0UL)
#define GH_B_GAIN_S_H_REG     (0xb1d1UL)
#define GH_B_GAIN_S_L_REG     (0xb1d2UL)

/* Metadata index calculation */
#define METADATA_IDX(REG) ((3UL * ((REG) - METADATA_START_REG)) + 1UL)

/* Camera Sensor API IDs for error reporting */
/* Range:  0x20 - 0x2f */
#define OV10640_API_PARSE_METADATA    (0x20U)
#define OV10640_API_GET_OPS           (0x21U)
#define OV10640_API_GET_CLOSEST_GAIN  (0x22U)

#if defined OV10640_ENABLE_GETOPS || \
    defined OV10640_ENABLE_PARSE_METADATA
/* AG Register backup used for update */
static uint8_t gAnalogGainReg = 0U;
static uint8_t gOv10640_ReadMode = 0U;
#endif

#ifdef OV10640_ENABLE_PARSE_METADATA
/* OV10640_ParseMetadata */
static int32_t OV10640_ParseMetadata(const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                     uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                     CSN_MODULE_MetadataType * const aMetadata)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t GainLRH_Addr, GainLRL_Addr;
    uint32_t GainLBH_Addr, GainLBL_Addr;

    if ((NULL == aBuffer) || (NULL == aBuffer[0]) || (NULL == aMetadata)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV10640_API_PARSE_METADATA,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    /* Parse metadata */
    /* Update frame count */
    aMetadata->frameNum = (((uint32_t)(aBuffer[0][METADATA_IDX(FRAME_COUNT_3_REG)] & 0xffUL) << 24UL)|
                           ((uint32_t)(aBuffer[0][METADATA_IDX(FRAME_COUNT_2_REG)] & 0xffUL) << 16UL)|
                           ((uint32_t)(aBuffer[0][METADATA_IDX(FRAME_COUNT_1_REG)] & 0xffUL) << 8UL) |
                           ((uint32_t)(aBuffer[0][METADATA_IDX(FRAME_COUNT_0_REG)] & 0xffUL)));
    /* Exposure */
    aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] =
                       ((((uint32_t)aBuffer[0][METADATA_IDX(LONG_EXP_H_REG)] & 0xffUL) << 8UL)|
                       ((uint32_t)aBuffer[0][METADATA_IDX(LONG_EXP_L_REG)] & 0xffUL));
    /* Analog gain */
    switch((aBuffer[0][METADATA_IDX(ANALOG_GAIN_REG)] & 0x3UL)) {
        case 0UL:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = 1U;
            break;
        case 1UL:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = 2U;
            break;
        case 2UL:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = 4U;
            break;
        default:
            aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = 8U;
            break;
    }

    /* Update analog gain */
    aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] *=
                        ((uint16_t)(aBuffer[0][METADATA_IDX(LONG_D_GAIN_H_REG)] & 0x3fUL) << 8UL)
                        |(uint16_t)(aBuffer[0][METADATA_IDX(LONG_D_GAIN_L_REG)]);

    /* Save AG register for updates */
    gAnalogGainReg = aBuffer[0][METADATA_IDX(ANALOG_GAIN_REG)];

    gOv10640_ReadMode = aBuffer[0][METADATA_IDX(READ_MODE_REG)];
    switch ((gOv10640_ReadMode & 0xcU) >> 2U) {
        case 1U:
            GainLRH_Addr = GR_GAIN_L_H_REG;
            GainLRL_Addr = GR_GAIN_L_L_REG;
            GainLBH_Addr = GB_GAIN_L_H_REG;
            GainLBL_Addr = GB_GAIN_L_L_REG;
            break;
        case 2U:
            GainLRH_Addr = GB_GAIN_L_H_REG;
            GainLRL_Addr = GB_GAIN_L_L_REG;
            GainLBH_Addr = GR_GAIN_L_H_REG;
            GainLBL_Addr = GR_GAIN_L_L_REG;
            break;
        case 3U:
            GainLRH_Addr = B_GAIN_L_H_REG;
            GainLRL_Addr = B_GAIN_L_L_REG;
            GainLBH_Addr = R_GAIN_L_H_REG;
            GainLBL_Addr = R_GAIN_L_L_REG;
            break;
        default:
            GainLRH_Addr = R_GAIN_L_H_REG;
            GainLRL_Addr = R_GAIN_L_L_REG;
            GainLBH_Addr = B_GAIN_L_H_REG;
            GainLBL_Addr = B_GAIN_L_L_REG;
            break;
    }

    aMetadata->gainR =
                   ((((uint16_t)aBuffer[0][METADATA_IDX(GainLRH_Addr)] & 0xffU) << 8U)|
               ((uint16_t)aBuffer[0][METADATA_IDX(GainLRL_Addr)] & 0xffU));
    aMetadata->gainB =
                   ((((uint16_t)aBuffer[0][METADATA_IDX(GainLBH_Addr)] & 0xffU) << 8U)|
               ((uint16_t)aBuffer[0][METADATA_IDX(GainLBL_Addr)] & 0xffU));
    aMetadata->wbEnabled = (0x3U == (aBuffer[0][METADATA_IDX(ISP_CTRL_0)] & 0x3U)) ? 1U : 0U;

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
#endif /* OV10640_ENABLE_PARSE_METADATA */

#ifdef OV10640_ENABLE_GETOPS
/* OV10640_GetOps */
static int32_t OV10640_GetOps(uint32_t aOpsType, CSN_MODULE_OpsParamType * const aOpsParam, CSN_MODULE_SetOpsType * const aOps)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t analogGain = 0U;
    uint16_t digitalGain = 0U;
    uint8_t analogGainS = 0U;
    uint16_t digitalGainS = 0U;
    uint16_t integerVal = 0U;
    uint16_t integerValS = 0U;
    uint8_t agVal = gAnalogGainReg;
    uint32_t isHDR = 0UL;
    uint32_t linesShort = 0U;
    uint32_t gainShort;
    uint32_t gainRatio;
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

    if ((NULL == aOps) || (NULL == aOpsParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV10640_API_GET_OPS,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    if ((NULL == aOpsParam->expParam) || (NULL == aOpsParam->wbParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV10640_API_GET_OPS,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    /* Assign exposure pointer */
    expP = aOpsParam->expParam;

    /* Check if HDR is enabled */
    isHDR = (1U == expP->expRatio) ? 0UL : 1UL;

    /* Initialize count */
    aOps->count = 0UL;

    /* Init GH Hold */
    aOps->op[aOps->count].reg = GH_GROUP_LENGTH_0;
    aOps->op[aOps->count].val = 0x40UL;
    aOps->count++;
    aOps->op[aOps->count].reg = GH_OPERATION_CTRL;
    aOps->op[aOps->count].val = 0x0UL;
    aOps->count++;
    aOps->op[aOps->count].reg = GH_GROUP_CTRL;
    aOps->op[aOps->count].val = 0x0UL;
    aOps->count++;

    if (0UL != (aOpsType & CSN_MODULE_OP_EXPOSURE_LONG)) {
        uint32_t expLines = (uint32_t) expP->expLines[CSN_MODULE_EXP_IDX_LONG];
        uint32_t gain = (uint32_t) expP->gain[CSN_MODULE_EXP_IDX_LONG];

#ifdef PRINT_EXP
        exposure[0] = expLines;
#endif
        aOps->op[aOps->count].reg = GH_LONG_EXP_H_REG;
        aOps->op[aOps->count].val = ((expLines >> 8UL) & 0xffUL);
        aOps->count++;
        aOps->op[aOps->count].reg = GH_LONG_EXP_L_REG;
        aOps->op[aOps->count].val = (expLines & 0xffUL);
        aOps->count++;
        /* Update SHORT for HDR */
        if (0UL != isHDR) {
            if (gain < 768UL) {
                gainRatio = (gain * 256UL)/768UL;
            } else {
                gainRatio = 256UL;
            }
            linesShort = (expLines * gainRatio)/256UL;
#ifdef PRINT_EXP
            exposure[2] = linesShort;
#endif
            /* Program SHORT */
            aOps->op[aOps->count].reg = GH_SHORT_EXP_H_REG;
            aOps->op[aOps->count].val = ((linesShort >> 8UL) & 0xffUL);
            aOps->count++;
            aOps->op[aOps->count].reg = GH_SHORT_EXP_L_REG;
            aOps->op[aOps->count].val = (linesShort & 0xffUL);
            aOps->count++;
        }
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_GAIN_LONG)) {
        uint32_t totalGain = (uint32_t) expP->gain[CSN_MODULE_EXP_IDX_LONG];

        /* Calc gain for LONG */
        integerVal = (uint16_t) (((totalGain * 256UL)/346UL) >> 8U);
        switch (integerVal) {
            case 0:
            case 1:
                analogGain = 0U;
                digitalGain = (uint16_t) totalGain;
                break;
            case 2:
            case 3:
                analogGain = 1U;
                digitalGain = ((uint16_t) totalGain >> 1U);
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                analogGain = 2U;
                digitalGain = ((uint16_t) totalGain >> 2U);
                break;
            default:
                analogGain = 3U;
                digitalGain = ((uint16_t) totalGain >> 3U);
                break;
        }

        /* Calculate Gain for HDR */
        if (0UL != isHDR) {
            if (totalGain < 768UL) {
                gainRatio = (totalGain * 256UL)/768UL;
            } else {
                gainRatio = 256UL;
            }

            if (0UL == gainRatio) {
                gainRatio = 256UL;
            }
            gainShort = (totalGain * 256UL)/gainRatio;

            /* Calc gain for S */
            integerValS = (uint16_t) (gainShort >> 8U);
            switch (integerValS) {
                case 0:
                case 1:
                    analogGainS = 0U;
                    digitalGainS = (uint16_t) gainShort;
                    break;
                case 2:
                case 3:
                    analogGainS = 1U;
                    digitalGainS = (uint16_t) (gainShort >> 1U);
                    break;
                case 4:
                case 5:
                case 6:
                case 7:
                    analogGainS = 2U;
                    digitalGainS = (uint16_t) (gainShort >> 2U);
                    break;
                default:
                    analogGainS = 3U;
                    digitalGainS = (uint16_t) (gainShort >> 3U);
                    break;
            }
        }

        /* Program AG for LONG */
        agVal &= (0xfcU);
        agVal |= (analogGain & 0x03U);
        /* Program AG for SHORT (HDR case) */
        if (0UL != isHDR) {
            agVal &= (0xf3U);
            agVal |= ((analogGainS << 2UL) & 0x0cU);
        }
        aOps->op[aOps->count].reg = GH_ANALOG_GAIN_REG;
        aOps->op[aOps->count].val = (agVal & 0xffUL);
        aOps->count++;

        /* Digital gain - LONG */
        aOps->op[aOps->count].reg = GH_LONG_D_GAIN_H_REG;
        aOps->op[aOps->count].val = ((digitalGain & 0x3f00UL) >> 8UL);
        aOps->count++;
        aOps->op[aOps->count].reg = GH_LONG_D_GAIN_L_REG;
        aOps->op[aOps->count].val = (digitalGain & 0xffUL);
        aOps->count++;

#ifdef PRINT_GAIN
        gain[0][0] = analogGain;
        gain[0][1] = digitalGain;
#endif
        /* Program DG for HDR */
        if (0UL != isHDR) {
            /* Digital gain - SHORT */
            aOps->op[aOps->count].reg = GH_SHORT_D_GAIN_H_REG;
            aOps->op[aOps->count].val = ((digitalGainS & 0x3f00UL) >> 8UL);
            aOps->count++;
            aOps->op[aOps->count].reg = GH_SHORT_D_GAIN_L_REG;
            aOps->op[aOps->count].val = (digitalGainS & 0xffUL);
            aOps->count++;
#ifdef PRINT_GAIN
        gain[1][0] = analogGainS;
        gain[1][1] = digitalGainS;
#endif
        }
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_WB)) {
        uint8_t gainRH = (uint8_t) ((aOpsParam->wbParam->gainR >> 8UL) & 0xffU);
        uint8_t gainRL = (uint8_t) ((aOpsParam->wbParam->gainR) & 0xffU);
        uint8_t gainBH = (uint8_t) ((aOpsParam->wbParam->gainB >> 8UL) & 0xffU);
        uint8_t gainBL = (uint8_t) ((aOpsParam->wbParam->gainB) & 0xffU);
        uint32_t GainLRH_Addr = 0UL, GainLRL_Addr = 0UL;
        uint32_t GainLBH_Addr = 0UL, GainLBL_Addr = 0UL;
        uint32_t GainSRH_Addr = 0UL, GainSRL_Addr = 0UL;
        uint32_t GainSBH_Addr = 0UL, GainSBL_Addr = 0UL;

#if defined PRINT_WB || defined PRINT_WB_OFFSET
        /* Copy wb gain */
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                if (CSN_MODULE_WG_GAIN_IDX_R == j) {
#ifdef PRINT_WB
                    wbGain[i][j] = aOpsParam->wbParam->gainR;
#endif
                } else if (CSN_MODULE_WG_GAIN_IDX_B == j) {
#ifdef PRINT_WB
                    wbGain[i][j] = aOpsParam->wbParam->gainB;
#endif
                } else {
#ifdef PRINT_WB
                    wbGain[i][j] = 256U;
#endif
                }
            }
        }
#endif
        switch ((gOv10640_ReadMode & 0xcU) >> 2U) {
            case 1U:
                GainLRH_Addr = GH_GR_GAIN_L_H_REG;
                GainLRL_Addr = GH_GR_GAIN_L_L_REG;
                GainLBH_Addr = GH_GB_GAIN_L_H_REG;
                GainLBL_Addr = GH_GB_GAIN_L_L_REG;
                if (0UL != isHDR) {
                    GainSRH_Addr = GH_GR_GAIN_S_H_REG;
                    GainSRL_Addr = GH_GR_GAIN_S_L_REG;
                    GainSBH_Addr = GH_GB_GAIN_S_H_REG;
                    GainSBL_Addr = GH_GB_GAIN_S_L_REG;
                }
                break;
            case 2U:
                GainLRH_Addr = GH_GB_GAIN_L_H_REG;
                GainLRL_Addr = GH_GB_GAIN_L_L_REG;
                GainLBH_Addr = GH_GR_GAIN_L_H_REG;
                GainLBL_Addr = GH_GR_GAIN_L_L_REG;
                if (0UL != isHDR) {
                    GainSRH_Addr = GH_GB_GAIN_S_H_REG;
                    GainSRL_Addr = GH_GB_GAIN_S_L_REG;
                    GainSBH_Addr = GH_GR_GAIN_S_H_REG;
                    GainSBL_Addr = GH_GR_GAIN_S_L_REG;
                }
                break;
            case 3U:
                GainLRH_Addr = GH_B_GAIN_L_H_REG;
                GainLRL_Addr = GH_B_GAIN_L_L_REG;
                GainLBH_Addr = GH_R_GAIN_L_H_REG;
                GainLBL_Addr = GH_R_GAIN_L_L_REG;
                if (0UL != isHDR) {
                    GainSRH_Addr = GH_B_GAIN_S_H_REG;
                    GainSRL_Addr = GH_B_GAIN_S_L_REG;
                    GainSBH_Addr = GH_R_GAIN_S_H_REG;
                    GainSBL_Addr = GH_R_GAIN_S_L_REG;
                }
                break;
            default:
                GainLRH_Addr = GH_R_GAIN_L_H_REG;
                GainLRL_Addr = GH_R_GAIN_L_L_REG;
                GainLBH_Addr = GH_B_GAIN_L_H_REG;
                GainLBL_Addr = GH_B_GAIN_L_L_REG;
                if (0UL != isHDR) {
                    GainSRH_Addr = GH_R_GAIN_S_H_REG;
                    GainSRL_Addr = GH_R_GAIN_S_L_REG;
                    GainSBH_Addr = GH_B_GAIN_S_H_REG;
                    GainSBL_Addr = GH_B_GAIN_S_L_REG;
                }
                break;
        }
        /* R gain - LONG */
        aOps->op[aOps->count].reg = GainLRH_Addr;
        aOps->op[aOps->count].val = (uint32_t) gainRH;
        aOps->count++;
        aOps->op[aOps->count].reg = GainLRL_Addr;
        aOps->op[aOps->count].val = (uint32_t) gainRL;
        aOps->count++;

        /* B Gain - Long */
        aOps->op[aOps->count].reg = GainLBH_Addr;
        aOps->op[aOps->count].val = (uint32_t) gainBH;
        aOps->count++;
        aOps->op[aOps->count].reg = GainLBL_Addr;
        aOps->op[aOps->count].val = (uint32_t) gainBL;
        aOps->count++;

        if (0UL != isHDR) {
            /* R gain - LONG */
            aOps->op[aOps->count].reg = GainSRH_Addr;
            aOps->op[aOps->count].val = (uint32_t) gainRH;
            aOps->count++;
            aOps->op[aOps->count].reg = GainSRL_Addr;
            aOps->op[aOps->count].val = (uint32_t) gainRL;
            aOps->count++;

            /* B Gain - Long */
            aOps->op[aOps->count].reg = GainSBH_Addr;
            aOps->op[aOps->count].val = (uint32_t) gainBH;
            aOps->count++;
            aOps->op[aOps->count].reg = GainSBL_Addr;
            aOps->op[aOps->count].val = (uint32_t) gainBL;
            aOps->count++;
        }
    }

    /* Launch GH */
    aOps->op[aOps->count].reg = GH_GROUP_CTRL;
    aOps->op[aOps->count].val = 0x20UL;
    aOps->count++;
    aOps->op[aOps->count].reg = GH_OPERATION_CTRL;
    aOps->op[aOps->count].val = 0x1UL;
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
#endif /* OV10640_ENABLE_GETOPS */

#ifdef OV10640_ENABLE_GET_CLOSEST_GAIN
/* OV10640_GetClosestGain */
static int32_t OV10640_GetClosestGain(uint32_t aOpsType, uint16_t * const aGain)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aGain) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV10640_API_GET_CLOSEST_GAIN,
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
#endif /* OV10640_ENABLE_GET_CLOSEST_GAIN */

/* gOV10640_Module */
static const CSN_MODULE_Type gOV10640_Module =
{
#ifdef OV10640_ENABLE_PARSE_METADATA
    .parseMetadata = OV10640_ParseMetadata,
#else
    .parseMetadata = NULL,
#endif
#ifdef OV10640_ENABLE_GETOPS
    .getOps = OV10640_GetOps,
#else
    .getOps = NULL,
#endif
#ifdef OV10640_ENABLE_GET_CLOSEST_GAIN
    .getClosestGain = OV10640_GetClosestGain,
#else
    .getClosestGain = NULL,
#endif
#ifdef OV10640_ENABLE_PARSE_STATISTICS
    .parseStatistics = NULL,
#else
    .parseStatistics = NULL,
#endif
#ifdef OV10640_ENABLE_CONFIGURE
    .sensorConfigure = NULL,
#else
    .sensorConfigure = NULL,
#endif
};
/* CSN_MODULE_GetModule */
const CSN_MODULE_Type * CSN_MODULE_GetModule(void)
{
    return &gOV10640_Module;
}
