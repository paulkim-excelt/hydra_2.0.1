/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
  File Name:  module_ov9716.c
Description: This file exposes OV9716 camera module interfaces.
 ******************************************************************************/

/* Includes */
#include <string.h>
#include <system.h>
#include <camera_sensor_module.h>
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
// #define OV9716_ENABLE_PARSE_METADATA
// #define OV9716_ENABLE_GETOPS
// #define OV9716_ENABLE_GET_CLOSEST_GAIN
// #define OV9716_ENABLE_PARSE_STATISTICS
// #define OV9716_ENABLE_CONFIGURE

/* Macros */
/* Register addresses */
#define COARSE_INT_TIME_REG_H          (0xB0B6UL)
#define COARSE_INT_TIME_REG_L          (0xB0B7UL)
#define COARSE_INT_TIME_REG_H_VS       (0xB0B8UL)
#define COARSE_INT_TIME_REG_L_VS       (0xB0B9UL)
#define FINE_INTEGRATION_TIME_REG_VS   (0XB0BAUL)
#define ANALOG_GAIN_REG                (0xB0BBUL)
#define DIGITAL_GAIN_REG_H_HCG         (0xB162UL)
#define DIGITAL_GAIN_REG_L_HCG         (0xB163UL)
#define DIGITAL_GAIN_REG_H_LCG         (0xB164UL)
#define DIGITAL_GAIN_REG_L_LCG         (0xB165UL)
#define DIGITAL_GAIN_REG_H_VS          (0xB166UL)
#define DIGITAL_GAIN_REG_L_VS          (0xB167UL)
#define WB_GAIN_HCG_H_R                (0xB360UL)
#define WB_GAIN_HCG_L_R                (0xB361UL)
#define WB_GAIN_HCG_H_B                (0xB366UL)
#define WB_GAIN_HCG_L_B                (0xB367UL)
#define WB_GAIN_LCG_H_R                (0xB368UL)
#define WB_GAIN_LCG_L_R                (0xB369UL)
#define WB_GAIN_LCG_H_B                (0XB36EUL)
#define WB_GAIN_LCG_L_B                (0xB36FUL)
#define WB_GAIN_VS_H_R                 (0XB370UL)
#define WB_GAIN_VS_L_R                 (0xB371UL)
#define WB_GAIN_VS_H_B                 (0xB376UL)
#define WB_GAIN_VS_L_B                 (0xB377UL)

#define OFFSET_HCG_H_R                 (0xB378UL)
#define OFFSET_HCG_M_R                 (0xB379UL)
#define OFFSET_HCG_L_R                 (0xB37AUL)
#define OFFSET_HCG_H_B                 (0xB381UL)
#define OFFSET_HCG_M_B                 (0xB382UL)
#define OFFSET_HCG_L_B                 (0xB383UL)

#define OFFSET_HCG_H_GR                (0xB37BUL)
#define OFFSET_HCG_M_GR                (0xB37CUL)
#define OFFSET_HCG_L_GR                (0xB37DUL)
#define OFFSET_HCG_H_GB                (0xB37EUL)
#define OFFSET_HCG_M_GB                (0xB37FUL)
#define OFFSET_HCG_L_GB                (0xB380UL)

#define OFFSET_LCG_H_R                 (0xB384UL)
#define OFFSET_LCG_M_R                 (0xB385UL)
#define OFFSET_LCG_L_R                 (0xB386UL)
#define OFFSET_LCG_H_B                 (0xB38DUL)
#define OFFSET_LCG_M_B                 (0xB38EUL)
#define OFFSET_LCG_L_B                 (0xB38FUL)

#define OFFSET_LCG_H_GR                (0xB387UL)
#define OFFSET_LCG_M_GR                (0xB388UL)
#define OFFSET_LCG_L_GR                (0xB389UL)
#define OFFSET_LCG_H_GB                (0xB38AUL)
#define OFFSET_LCG_M_GB                (0xB38BUL)
#define OFFSET_LCG_L_GB                (0xB38CUL)

#define OFFSET_VS_H_R                  (0xB390UL)
#define OFFSET_VS_M_R                  (0xB391UL)
#define OFFSET_VS_L_R                  (0xB392UL)
#define OFFSET_VS_H_B                  (0xB399UL)
#define OFFSET_VS_M_B                  (0xB39AUL)
#define OFFSET_VS_L_B                  (0xB39BUL)

#define OFFSET_VS_H_GR                 (0xB393UL)
#define OFFSET_VS_M_GR                 (0xB394UL)
#define OFFSET_VS_L_GR                 (0xB395UL)
#define OFFSET_VS_H_GB                 (0xB396UL)
#define OFFSET_VS_M_GB                 (0xB397UL)
#define OFFSET_VS_L_GB                 (0xB398UL)

#define GROUP_PARAMETER_SET            (0x3467UL)
#define GROUP_MANUAL_LAUNCH_ENABLE     (0x320DUL)

/* Metadata index calculation */
#define RAW16_OFFSET_METADATA          (2UL)
#define RAW16_METADATA_SKIP_BYTES      (4UL)
#define METADATA_START_ADDRESS         (0X30C9UL)

/* Camera Sensor API IDs for error reporting */
/* Range:  0x40 - 0x4f */
#define OV9716_API_PARSE_METADATA    (0x40U)
#define OV9716_API_PARSE_STATISTICS  (0x41U)
#define OV9716_API_GET_OPS           (0x42U)
#define OV9716_API_GET_CLOSEST_GAIN  (0x43U)
#define OV9716_API_CONFIGURE         (0x44U)

#if defined OV9716_ENABLE_GETOPS || \
    defined OV9716_ENABLE_CONFIGURE
/* Static for sensor WB */
static uint16_t gRGain = 256U;
static uint16_t gBGain = 256U;
#endif

#ifdef OV9716_ENABLE_PARSE_METADATA
/* OV9716_getMetaByte */
static uint8_t OV9716_getMetaByte(const uint8_t * const aBuffer,
                                   uint32_t address)
{
    uint8_t byte = 0U;
    uint32_t idx;

    if (address >= METADATA_START_ADDRESS) {
        idx = RAW16_OFFSET_METADATA +
              (address - METADATA_START_ADDRESS) * RAW16_METADATA_SKIP_BYTES;
        byte = aBuffer[idx] ;
    }

    return byte;
}

/* OV9716_ParseMetadata */
static int32_t OV9716_ParseMetadata(const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                     uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                     CSN_MODULE_MetadataType * const aMetadata)
{
    int32_t ret = BCM_ERR_OK;
    uint8_t exposureH = 0U;
    uint8_t exposureL = 0U;
    uint8_t dGainLCG_H = 0U;
    uint8_t dGainLCG_L = 0U;
    uint8_t frameCountH = 0U;
    uint8_t frameCountL = 0U;
    uint8_t aGainCode = 0U;
    uint16_t dGainLCG, aGainVal;
    uint16_t rGain = 0U;
    uint16_t bGain = 0U;

    if ((NULL == aBuffer) || (NULL == aBuffer[0]) || (NULL == aMetadata)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV9716_API_PARSE_METADATA,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    /* Metadata Dump Sequence start 30c9 end 339b
    0x30b6-0x30b7 -->Exposure Lines LCG
    0x30bb --> All Analog/Conversion Gains
    0x3164-0x3165 --> Digital Gains LCG
    0x3368-0x3369 --> AWB Gains LCG
    0x3384-0x3386 --> Offsets LCG
    0x3203-0x3204 --> Frame Counter
    0x3250 --> White Balance Gain Enable
    */
    exposureH = OV9716_getMetaByte(aBuffer[0], 0X30c9UL);
    exposureL = OV9716_getMetaByte(aBuffer[0], 0X30caUL);
    aGainCode = OV9716_getMetaByte(aBuffer[0], 0x30d0UL);
    dGainLCG_H = OV9716_getMetaByte(aBuffer[0], 0x3170UL);
    dGainLCG_L = OV9716_getMetaByte(aBuffer[0], 0x3171UL);
    frameCountH = OV9716_getMetaByte(aBuffer[0], 0x30d7UL);
    frameCountL = OV9716_getMetaByte(aBuffer[0], 0x30d8UL);
    rGain = (((uint16_t) OV9716_getMetaByte(aBuffer[0], 0x3360UL)) << 8UL) |
              (uint16_t) OV9716_getMetaByte(aBuffer[0], 0x3361UL);
    bGain = (((uint16_t) OV9716_getMetaByte(aBuffer[0], 0x3366UL)) << 8UL) |
              (uint16_t) OV9716_getMetaByte(aBuffer[0], 0x3367UL);

    /* Exposure */
    aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] =
                ((uint32_t)((exposureH & 0xffUL) << 8UL))| ((exposureL));

    /* aGain takes 1,2,4,8 */
    aGainCode &=  0x03U;
    if (aGainCode == 0U) {
       aGainVal = 256U;
    } else if(aGainCode == 1U) {
       aGainVal = 512U;
    } else if(aGainCode == 2U) {
       aGainVal = 1024U;
    } else {
       aGainVal = 2048U;
    }

    /* dGain 6p8 */
    dGainLCG =  (uint16_t)((dGainLCG_H & 0x3fUL) << 8UL | dGainLCG_L);
    aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] =
                (uint16_t)((aGainVal * dGainLCG) / 256U);
    aMetadata->frameNum = (((uint32_t) frameCountH & 0xffUL) << 8UL) |
                           ((uint32_t) frameCountL & 0xffUL);
    aMetadata->gainR = rGain;
    aMetadata->gainB = bGain;
    aMetadata->wbEnabled = (0x33U == (OV9716_getMetaByte(aBuffer[0], 0x3250UL) &
                            0x33U)) ? 1UL : 0UL;
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
#endif /* OV9716_ENABLE_PARSE_METADATA */

#ifdef OV9716_ENABLE_PARSE_STATISTICS
/* OV9716_ParseStatistics */
static int32_t OV9716_ParseStatistics (const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                        CSN_MODULE_StatsType * const (aSensorStats)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t *aParsedMask)
{
    int32_t ret = BCM_ERR_NOSUPPORT;

    return ret;
}
#endif /* OV9716_ENABLE_PARSE_STATISTICS */

#ifdef OV9716_ENABLE_GETOPS
/* OV9716_GetOps */
static int32_t OV9716_GetOps(uint32_t aOpsType,
                             CSN_MODULE_OpsParamType * const aOpsParam,
                             CSN_MODULE_SetOpsType * const aOps)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t expLines = 0UL;
    uint16_t expLinesVS = 0U;
    CSN_MODULE_ExpParamType *expP = NULL;
    uint8_t isHDR = 0U;
    uint8_t gAnalogGainReg = 0U;
    uint8_t analogGain =  0U;
    uint16_t vsGainRes = 256U;
    uint16_t digitalGain = 256U; uint16_t digitalGain_HCG = 256U;
    uint16_t digitalGain_VS = 256U;
    uint32_t i;
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
                        OV9716_API_GET_OPS,
                        ret,
                        (uint32_t) (__LINE__), NULL);
        goto func_exit;
    }

    /* Initialize count */
    aOps->count = 0UL;

    if ((NULL == aOpsParam->expParam) || (NULL == aOpsParam->wbParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV9716_API_GET_OPS,
                            ret,
                            0UL, NULL);
        goto func_exit;
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_WB)) {
        if (aOpsParam->wbParam->gainR == 0U) {
            gRGain = 256U;
        } else {
            gRGain = (aOpsParam->wbParam->gainR);
        }

        if (aOpsParam->wbParam->gainB == 0U) {
            gBGain = 256U;
        } else {
            gBGain = (aOpsParam->wbParam->gainB);
        }
#if defined PRINT_WB || defined PRINT_WB_OFFSET
        /* Copy wb gain */
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                if (CSN_MODULE_WG_GAIN_IDX_R == j) {
#ifdef PRINT_WB
                    wbGain[i][j] = gRGain;
#endif
#ifdef PRINT_WB_OFFSET
                    wbGainOffset[i][j] = (((uint32_t) gRGain -256UL) * 64UL);
#endif
                } else if (CSN_MODULE_WG_GAIN_IDX_B == j) {
#ifdef PRINT_WB
                    wbGain[i][j] = gBGain;
#endif
#ifdef PRINT_WB_OFFSET
                    wbGainOffset[i][j] = (((uint32_t) gBGain -256UL) * 64UL);
#endif
                } else {
#ifdef PRINT_WB
                    wbGain[i][j] = 256U;
#endif
#ifdef PRINT_WB_OFFSET
                    wbGainOffset[i][j] = 0U;
#endif
                }
            }
        }
#endif
        goto func_exit;
    }

    /* Assign exposure pointer */
    expP = aOpsParam->expParam;
    isHDR = (expP->expRatio == 16UL)? 1U : 0U;

    aOps->op[aOps->count].reg = 0X3467UL;
    aOps->op[aOps->count].val = 0UL;
    aOps->count++;

    aOps->op[aOps->count].reg = 0X3464UL;
    aOps->op[aOps->count].val = 0UL;
    aOps->count++;

    if (0UL != (aOpsType & CSN_MODULE_OP_EXPOSURE_LONG)) {
        expLines = (uint32_t) expP->expLines[CSN_MODULE_EXP_IDX_LONG];
#ifdef PRINT_EXP
        exposure[0] = expLines;
#endif

        aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_H;
        aOps->op[aOps->count].val = ((expLines >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_L;
        aOps->op[aOps->count].val = expLines & 0xffUL;
        aOps->count++;

        /* Update VS for HDR */
        if (0U != isHDR) {
            /* Program Very Short */
            /*Divide by Exposure Ratio HCG to VS*/
            expLinesVS = (uint16_t) (expLines >> 4UL);
            if (expLinesVS < 1U) {
                expLinesVS = 1U;
            }

            if(expLinesVS > 20U) {
                vsGainRes = (256U * expLinesVS) / 20U;
                expLinesVS = 20U;
            }

#ifdef PRINT_EXP
            exposure[2] = expLinesVS;
#endif
            aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_H_VS;
            aOps->op[aOps->count].val = (((uint32_t) expLinesVS) >> 8UL) & 0xffUL;
            aOps->count++;

            aOps->op[aOps->count].reg = COARSE_INT_TIME_REG_L_VS;
            aOps->op[aOps->count].val = ((uint32_t)expLinesVS)  & 0xffUL;
            aOps->count++;
        }

        aOps->op[aOps->count].reg = WB_GAIN_HCG_H_R;
        aOps->op[aOps->count].val = ((((uint32_t) gRGain) >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_HCG_L_R;
        aOps->op[aOps->count].val = ((uint32_t) gRGain) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_HCG_H_B;
        aOps->op[aOps->count].val = ((((uint32_t) gBGain) >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_HCG_L_B;
        aOps->op[aOps->count].val = ((uint32_t) gBGain) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_H_R;
        aOps->op[aOps->count].val = ((((uint32_t) gRGain) >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_L_R;
        aOps->op[aOps->count].val = ((uint32_t) gRGain) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_H_B;
        aOps->op[aOps->count].val = ((((uint32_t) gBGain) >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_LCG_L_B;
        aOps->op[aOps->count].val = ((uint32_t) gBGain) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_H_R;
        aOps->op[aOps->count].val = ((((uint32_t) gRGain) >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_L_R;
        aOps->op[aOps->count].val = ((uint32_t) gRGain) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_H_B;
        aOps->op[aOps->count].val = ((((uint32_t) gBGain) >> 8UL) & 0xffUL);
        aOps->count++;

        aOps->op[aOps->count].reg = WB_GAIN_VS_L_B;
        aOps->op[aOps->count].val = ((uint32_t) gBGain)  & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_HCG_H_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain - 256UL) * 64UL)) >>
                                     16UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_HCG_M_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                     8UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_HCG_L_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                     0UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_HCG_H_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    16UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_HCG_M_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    8UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_HCG_L_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    0UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_LCG_H_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                    16UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_LCG_M_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                    8UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_LCG_L_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                    0UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_LCG_H_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    16UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_LCG_M_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    8UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_LCG_L_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    0UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_VS_H_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                    16UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_VS_M_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                    8UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_VS_L_R ;
        aOps->op[aOps->count].val = (((((uint32_t) gRGain -256UL) * 64UL)) >>
                                    0UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_VS_H_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    16UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_VS_M_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    8UL) & 0xffUL;
        aOps->count++;

        aOps->op[aOps->count].reg = OFFSET_VS_L_B ;
        aOps->op[aOps->count].val = (((((uint32_t) gBGain -256UL) * 64UL)) >>
                                    0UL) & 0xffUL;
        aOps->count++;
    }

     if (0UL != (aOpsType & CSN_MODULE_OP_GAIN_LONG)) {
        uint16_t totalGain = (uint16_t) expP->gain[CSN_MODULE_EXP_IDX_LONG];

        /* Analog Gain only Support Discrete Values of 0/1/2/4 for 1X 2X 4X 8X*/
        if (totalGain >=256U && totalGain < 512U){
            analogGain = 0U; //1X
            digitalGain = (uint16_t) totalGain;
        } else if (totalGain >= 512U && totalGain < 1024U){
            analogGain = 1U;   //2X
            digitalGain = (uint16_t) (totalGain >> 1U);
        } else if (totalGain >= 1024U && totalGain < 2048U){
            analogGain = 2U;  //4X
            digitalGain = (uint16_t) (totalGain >> 2U);
        } else {
            analogGain = 3U;  //8X
            digitalGain = (uint16_t) (totalGain >> 3U);
        }

        gAnalogGainReg = analogGain; // Set LCG Gain
        gAnalogGainReg |= analogGain << 2; //Set HCG Gain
        gAnalogGainReg |= analogGain << 4; //Set VS Gain
#ifdef PRINT_GAIN
        gain[0][0] = gAnalogGainReg;
        gain[1][0] = gAnalogGainReg;
        gain[2][0] = gAnalogGainReg;
#endif

        aOps->op[aOps->count].reg = ANALOG_GAIN_REG;
        aOps->op[aOps->count].val = (uint32_t) gAnalogGainReg;
        aOps->count++;

        /* Digital Gains HCG LCG */
        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_H_LCG;
        aOps->op[aOps->count].val = ((((uint32_t) digitalGain) >> 8UL) & 0x3fUL);
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_L_LCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain) & 0xffUL);
        aOps->count++;

#ifdef PRINT_GAIN
        gain[0][1] = digitalGain;
#endif
        /* Increase Digital Gain 1.45X to Reach 16X HCG/LCG ratio CG is only 11X */
        digitalGain_HCG = (digitalGain * 372U) / 256U;

        if(digitalGain_HCG > 2047U) {
            digitalGain_HCG = 2047U;
        }
#ifdef PRINT_GAIN
        gain[1][1] = digitalGain_HCG;
#endif

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_H_HCG;
        aOps->op[aOps->count].val = ((((uint32_t) digitalGain_HCG) >> 8UL) &
                                       0x3fUL);
        aOps->count++;

        aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_L_HCG;
        aOps->op[aOps->count].val = (((uint32_t) digitalGain_HCG) & 0xffUL);
        aOps->count++;

        if (0U != isHDR) {
            digitalGain_VS = (vsGainRes * digitalGain) / 256U;

            if (digitalGain_VS > 2047U) {
                digitalGain_VS = 2047U;
            }
#ifdef PRINT_GAIN
        gain[2][1] = digitalGain_VS;
#endif
            aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_H_VS;
            aOps->op[aOps->count].val = ((((uint32_t) digitalGain_VS) >> 8UL) &
                                            0x3fUL);
            aOps->count++;

            aOps->op[aOps->count].reg = DIGITAL_GAIN_REG_L_VS;
            aOps->op[aOps->count].val = (((uint32_t) digitalGain_VS) & 0xffUL);
            aOps->count++;
        }
    }
    aOps->op[aOps->count].reg = 0x3464UL;
    aOps->op[aOps->count].val = 64UL;
    aOps->count++;
    aOps->op[aOps->count].reg = 0x3467UL;
    aOps->op[aOps->count].val = 1UL;
    aOps->count++;

func_exit:
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
#endif /* OV9716_ENABLE_GETOPS */

#ifdef OV9716_ENABLE_GET_CLOSEST_GAIN
/* OV9716_GetClosestGain */
static int32_t OV9716_GetClosestGain(uint32_t aOpsType, uint16_t * const aGain)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aGain) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV9716_API_GET_CLOSEST_GAIN,
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
#endif /* OV9716_ENABLE_GET_CLOSEST_GAIN */

#ifdef OV9716_ENABLE_CONFIGURE
/* OV9716_Configure */
static int32_t OV9716_Configure(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL == aConfig) {
        ret = BCM_ERR_INVAL_PARAMS;
        BCM_ReportError(BCM_CSN_ID,
                            0U,
                            OV9716_API_CONFIGURE,
                            ret,
                            (uint32_t)(__LINE__),
                            NULL);
        goto func_exit;
    }
    gRGain = 256U;
    gBGain = 256U;

func_exit:
    return ret;
}
#endif /* OV9716_ENABLE_CONFIGURE */

/* gOV9716_Module */
static const CSN_MODULE_Type gOV9716_Module =
{
#ifdef OV9716_ENABLE_PARSE_METADATA
    .parseMetadata = OV9716_ParseMetadata,
#else
    .parseMetadata = NULL,
#endif
#ifdef OV9716_ENABLE_GETOPS
    .getOps = OV9716_GetOps,
#else
    .getOps = NULL,
#endif
#ifdef OV9716_ENABLE_GET_CLOSEST_GAIN
    .getClosestGain = OV9716_GetClosestGain,
#else
    .getClosestGain = NULL,
#endif
#ifdef OV9716_ENABLE_PARSE_STATISTICS
    .parseStatistics = OV9716_ParseStatistics,
#else
    .parseStatistics = NULL,
#endif
#ifdef OV9716_ENABLE_CONFIGURE
    .sensorConfigure = OV9716_Configure,
#else
    .sensorConfigure = NULL,
#endif
};

/* CSN_MODULE_GetModule */
const CSN_MODULE_Type * CSN_MODULE_GetModule(void)
{
    return &gOV9716_Module;
}
