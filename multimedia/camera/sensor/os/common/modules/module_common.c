/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
#include <crc.h>
#include <bcm_utils.h>
#include <camera_sensor_module.h>
//#define PRINT_EXP
//#define PRINT_GAIN
//#define PRINT_WB
//#define PRINT_WB_OFFSET
//#define PRINT_MD
//#define PRINT_ED_CRC

#if defined PRINT_EXP || \
    defined PRINT_GAIN || \
    defined PRINT_WB || \
    defined PRINT_WB_OFFSET ||\
    defined PRINT_ED_CRC ||\
    defined PRINT_MD
#include <ulog.h>
#define GetModuleLogLevel() ULOG_LVL_INFO
#endif

/******************************************************************************
  File Name:  module_common.c
Description: This file exposes common camera module interfaces.
 ******************************************************************************/

/* CSN_MODULE_GET_BYTE */
#define CSN_MODULE_GET_BYTE(buf, off, mask, shift) \
        ((0U != ((shift) & CSN_MODULE_SHIFT_DIR_MASK)) ? \
        (buf[(off)] & (mask)) >> ((shift) & CSN_MODULE_SHIFT_COUNT_MASK) : \
        (buf[(off)] & (mask)) << ((shift) & CSN_MODULE_SHIFT_COUNT_MASK))

/* CSN_MODULE_MAKE_BYTE */
#define CSN_MODULE_MAKE_BYTE(val, mask, shift) \
        ((0U != ((shift) & CSN_MODULE_SHIFT_DIR_MASK)) ? \
        (((val) << ((shift) & CSN_MODULE_SHIFT_COUNT_MASK)) & (mask))  : \
        (((val) >> ((shift) & CSN_MODULE_SHIFT_COUNT_MASK)) & (mask)))

/* CSN_MODULE_SHIFT_BYTE */
#define CSN_MODULE_SHIFT_BYTE(val, shift) \
        ((0U != ((shift) & CSN_MODULE_SHIFT_DIR_MASK)) ? \
         ((val) >> ((shift) & CSN_MODULE_SHIFT_COUNT_MASK)) : \
         ((val) << ((shift) & CSN_MODULE_SHIFT_COUNT_MASK)))

/* CSN_MODULE_REV_SHIFT_BYTE */
#define CSN_MODULE_REV_SHIFT_BYTE(val, shift) \
        ((0U != ((shift) & CSN_MODULE_SHIFT_DIR_MASK)) ? \
         ((val) << ((shift) & CSN_MODULE_SHIFT_COUNT_MASK)) : \
         ((val) >> ((shift) & CSN_MODULE_SHIFT_COUNT_MASK)))

/* CSN_MODULE_WRITE_BYTES */
#define CSN_MODULE_WRITE_BYTES(cfg, value, addr, ops) \
{ \
    if (0U != (cfg)->numBytes) { \
        int32_t byteIdx; \
        for (byteIdx = ((int32_t)(cfg)->numBytes - 1L); byteIdx >= 0L; byteIdx--) { \
            CSN_MODULE_ParseByteCfgType *bytes = &(cfg)->bytes[byteIdx]; \
            uint32_t byte; \
            byte = (uint32_t) CSN_MODULE_MAKE_BYTE(CSN_MODULE_REV_SHIFT_BYTE((value), \
                                                   (cfg)->shift), \
                                                   bytes->mask, \
                                                   bytes->shift); \
            if (CSN_MODULE_INVALID_USHORT != (addr)[byteIdx]) { \
                (ops)->op[(ops)->count].reg = (addr)[byteIdx]; \
                (ops)->op[(ops)->count].val = byte; \
                (ops)->count++; \
            } \
        } \
    } \
}

/* This needs to go to OTP/flash */
static CSN_MODULE_TunerCfgType gCSN_MODULE_TunerCfg;

/* CSN_MODULE_CommonParseMetadata */
static int32_t CSN_MODULE_CommonParseMetadata(const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                              uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                              CSN_MODULE_MetadataType * const aMetadata)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i, j;
    CSN_MODULE_ParseCfgType *parseCfg = &gCSN_MODULE_TunerCfg.parseCfg;
    uint16_t wbGain[CSN_MODULE_WG_GAIN_IDX_MAX];
    uint16_t gain[CSN_MODULE_MAX_GAIN_COMPS];

    if ((NULL == aBuffer) || (NULL == aMetadata)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto func_exit;
    }

    for (i = 0UL; i < parseCfg->parseEECRCCfg.numCRCs; i++) {
        uint8_t data[32U];
        uint32_t refCRC, crc;
        CSN_MODULE_ParseEDCRCType *parseEECRCCfg = &parseCfg->parseEECRCCfg;
        CSN_MODULE_EDLayoutType *edLayout = &parseEECRCCfg->edCrcLayout[i].edLayout;
        CSN_MODULE_CRCLayoutType *crcLayout = &parseEECRCCfg->edCrcLayout[i].crcLayout;
        uint32_t channelIdx = parseEECRCCfg->edCrcLayout[i].channelIdx;

        /* Validate configurations and input */
        if (NULL == aBuffer[channelIdx]) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto func_exit;
        }
        if (CSN_MODULE_ED_FORMAT_TAGGED != parseEECRCCfg->edFormat) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto func_exit;
        }
        if (CSN_MODULE_ED_CRC_FORMAT_32C != parseEECRCCfg->crcType) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto func_exit;
        }
        if (CSN_MODULE_CRC_ORDER_LE != crcLayout->order) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto func_exit;
        }
        if (32U < edLayout->numBytes) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto func_exit;
        }

        /* Retreive data */
        BCM_MemSet(&data, 0x0U, sizeof(data));
        for (j = 0UL; j < edLayout->numBytes; j++) {
            data[j] = aBuffer[channelIdx][edLayout->startOffset +
                              (edLayout->jumpOffset * j)];
        }
        /* Retreive CRC */
        refCRC = 0UL;
        for (j = 0UL; j < crcLayout->numBytes; j++) {
            uint8_t byte;
            byte = aBuffer[channelIdx][crcLayout->startOffset +
                                       (crcLayout->jumpOffset * j)];
            refCRC |= (uint32_t) (((uint32_t)byte) << (j * 8U));
        }
        /* Calculate CRC */
        crc = BCM_CRC32Custom(&data[0],
                              edLayout->numBytes,
                              crcLayout->poly,
                              crcLayout->crcInitValue);

        /* Compare CRC */
        if (refCRC != crc) {
            ret = BCM_ERR_INVAL_PARAMS;
#ifdef PRINT_ED_CRC
            ULOG_INFO("ED CRC Check failed, refCRC: 0x%x, CRC: 0x%x \n", refCRC, crc);
#endif /* PRINT_ED_CRC */
            goto func_exit;
        } else {
#ifdef PRINT_ED_CRC
            ULOG_INFO("ED CRC Check Success, CRC: 0x%x\n", crc);
#endif /* PRINT_ED_CRC */
        }
    }

    /* Parse Frame Number */
    aMetadata->frameNum = 0UL;
    if (NULL == aBuffer[parseCfg->parseFnCfg.bufIdx]) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto func_exit;
    }
    for (i = 0UL; i < parseCfg->parseFnCfg.numBytes; i++) {
        CSN_MODULE_ParseByteCfgType *bytes = &parseCfg->parseFnCfg.bytes[i];

        aMetadata->frameNum |= CSN_MODULE_GET_BYTE((uint32_t) aBuffer[parseCfg->parseFnCfg.bufIdx],
                                                               bytes->offset,
                                                               bytes->mask,
                                                               bytes->shift);
    }
    aMetadata->frameNum = CSN_MODULE_SHIFT_BYTE(aMetadata->frameNum,
                                             parseCfg-> parseFnCfg.shift);

    /* Parse Exposure Lines */
    aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] = 0UL;
    if (NULL == aBuffer[parseCfg->parseExpCfg.bufIdx]) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto func_exit;
    }
    for (i = 0UL; i < parseCfg->parseExpCfg.numBytes; i++) {
        CSN_MODULE_ParseByteCfgType *bytes = &parseCfg->parseExpCfg.bytes[i];

        aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] |= CSN_MODULE_GET_BYTE((uint32_t) aBuffer[parseCfg->parseExpCfg.bufIdx],
                                                               bytes->offset,
                                                               bytes->mask,
                                                               bytes->shift);
    }
    aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] = CSN_MODULE_SHIFT_BYTE(
                        aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG],
                        parseCfg->parseExpCfg.shift);

    /* Parse RGain/GGain/BGain */
    for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
        CSN_MODULE_ParseWBGainCfgType *wbGainCfg = &parseCfg->parseWBCfg.parseWBGainCfg[j];

        if (NULL == aBuffer[wbGainCfg->bufIdx]) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto func_exit;
        }
        wbGain[j] = 0U;
        for (i = 0UL; i < wbGainCfg->numBytes; i++) {
            CSN_MODULE_ParseByteCfgType *bytes = & wbGainCfg->bytes[i];

            wbGain[j] |= CSN_MODULE_GET_BYTE((uint16_t) aBuffer[wbGainCfg->bufIdx],
                                                         bytes->offset,
                                                         bytes->mask,
                                                         bytes->shift);
        }
        if (0U == wbGainCfg->numBytes) {
            wbGain[j] = CSN_MODULE_UP8_UNITY;
        } else {
            wbGain[j] = CSN_MODULE_SHIFT_BYTE(wbGain[j],
                                              wbGainCfg->shift);
        }
    }
    aMetadata->gainR = wbGain[CSN_MODULE_WG_GAIN_IDX_R];
    aMetadata->gainB = wbGain[CSN_MODULE_WG_GAIN_IDX_B];

    /* Normalize WB gains */
    if (CSN_MODULE_UP8_UNITY != wbGain[CSN_MODULE_WG_GAIN_IDX_GR]) {
        aMetadata->gainR = (aMetadata->gainR * CSN_MODULE_UP8_UNITY /wbGain[CSN_MODULE_WG_GAIN_IDX_GR]);
        aMetadata->gainB = (aMetadata->gainB * CSN_MODULE_UP8_UNITY /wbGain[CSN_MODULE_WG_GAIN_IDX_GR]);
    }

    /* Parse WBE */
    if (NULL == aBuffer[parseCfg->parseWBCfg.enByteBufIdx]) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto func_exit;
    }
    aMetadata->wbEnabled = CSN_MODULE_GET_BYTE((uint32_t) aBuffer[parseCfg->parseWBCfg.enByteBufIdx],
                                               parseCfg->parseWBCfg.enByte.offset,
                                               parseCfg->parseWBCfg.enByte.mask,
                                               parseCfg->parseWBCfg.enByte.shift);
    aMetadata->wbEnabled = (parseCfg->parseWBCfg.enByte.mask == aMetadata->wbEnabled) ?
                            1U : 0U;

    /* Parse Gain */
    for (j = 0UL; j < CSN_MODULE_MAX_GAIN_COMPS; j++) {
        gain[j] = 0U;

        if (NULL == aBuffer[parseCfg->parseGainCfg[j].bufIdx]) {
            ret = BCM_ERR_INVAL_PARAMS;
            goto func_exit;
        }
        for (i = 0UL; i < parseCfg->parseGainCfg[j].numBytes; i++) {
            CSN_MODULE_ParseByteCfgType *bytes = &parseCfg->parseGainCfg[j].bytes[i];

            gain[j] |= CSN_MODULE_GET_BYTE((uint16_t) aBuffer[parseCfg->parseGainCfg[j].bufIdx],
                                                         bytes->offset,
                                                         bytes->mask,
                                                         bytes->shift);
        }
        gain[j] = CSN_MODULE_SHIFT_BYTE(gain[j], parseCfg->parseGainCfg[j].shift);

        if (0U != parseCfg->parseGainCfg[j].useTable) {
            uint8_t idx = parseCfg->parseGainCfg[j].useTable - 1U;

            for (i = 0U; i < parseCfg->gainTable[idx].numVals; i++) {
                if ((parseCfg->gainTable[idx].code[i] & parseCfg->gainTable[idx].mask) ==
                            (gain[j] &
                            parseCfg->gainTable[idx].mask)) {
                    gain[j] = parseCfg->gainTable[idx].val[i];
                    break;
                }
            }
        }

        if (0U == parseCfg->parseGainCfg[j].numBytes) {
            gain[j] = CSN_MODULE_UP8_UNITY;
        }
    }

    /* TODO: Add WB GR/GB gain to AG */
    aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] = CSN_MODULE_UP8_UNITY;
    for (j = 0UL; j < CSN_MODULE_MAX_GAIN_COMPS; j++) {
        aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] =
             (aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG] * gain[j] / CSN_MODULE_UP8_UNITY);
    }

#ifdef PRINT_MD
    ULOG_INFO("FN: %d, SH: %d , rGain: %d, bGain: %d, wbEnabled: %d, aGain: %d"
               " size of parse_config: %d, getOps config: %d \n", aMetadata->frameNum,
                                 aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG],
                                 aMetadata->gainR,
                                 aMetadata->gainB,
                                 aMetadata->wbEnabled,
                                 aMetadata->analogGain[CSN_MODULE_EXP_IDX_LONG],
                                 sizeof(gCSN_MODULE_TunerCfg.parseCfg),
                                 sizeof(gCSN_MODULE_TunerCfg.getOpsCfg));
#endif
func_exit:
    return ret;
}

/* CSN_MODULE_FindGainBoundary */
static void CSN_MODULE_FindGainBoundary(CSN_MODULE_ParseCfgType *aParseCfg,
                                       CSN_MODULE_GetOpsCfgType *aGetOpsCfg,
                                       uint32_t aStartGainIdx,
                                       uint32_t aStageIdx,
                                       uint32_t aExpIdx,
                                       uint16_t *aMinGain,
                                       uint16_t *aMaxGain)
{
    uint32_t i;
    uint64_t minGain = CSN_MODULE_UP8_UNITY;
    uint64_t maxGain = CSN_MODULE_UP8_UNITY;

    /* Not doing any NULL check as this is an internal function */

    for (i = aStartGainIdx; i < CSN_MODULE_MAX_GAIN_COMPS; i++) {
        /* Check the gain comp is valid */
        if (0U == aGetOpsCfg->gainParamCfg[aExpIdx][i].max[aStageIdx]) {
            if (0UL == i) {
                /* Invalid chain */
                maxGain = 0ULL;
            }
            break;
        }
        /* Update min and max gain in the gain chain */
        minGain = (minGain *
                   aGetOpsCfg->gainParamCfg[aExpIdx][i].min[aStageIdx] /
                   CSN_MODULE_UP8_UNITY);
        maxGain = (maxGain *
                   aGetOpsCfg->gainParamCfg[aExpIdx][i].max[aStageIdx] /
                   CSN_MODULE_UP8_UNITY);
    }

    /* Ceiling to max short */
    if (minGain > CSN_MODULE_INVALID_USHORT) {
        minGain = CSN_MODULE_INVALID_USHORT;
    }

    if (maxGain > CSN_MODULE_INVALID_USHORT) {
        maxGain = CSN_MODULE_INVALID_USHORT;
    }

    *aMinGain = (uint16_t) minGain;
    *aMaxGain = (uint16_t) maxGain;
}

/* CSN_MODULE_GetStageIdx */
static uint8_t CSN_MODULE_GetStageIdx(CSN_MODULE_ParseCfgType *aParseCfg,
                                       CSN_MODULE_GetOpsCfgType *aGetOpsCfg,
                                       uint32_t aExpIdx,
                                       uint16_t aGain)
{
    uint8_t stageIdx = CSN_MODULE_MAX_NUM_GAIN_STAGES;
    uint16_t minGain = CSN_MODULE_UP8_UNITY;
    uint16_t maxGain = CSN_MODULE_UP8_UNITY;
    uint8_t j;

    /* Not doing any NULL check as this is an internal function */

    for (j = 0U; j < CSN_MODULE_MAX_NUM_GAIN_STAGES; j++) {
        CSN_MODULE_FindGainBoundary(aParseCfg,
                                   aGetOpsCfg,
                                   0UL,
                                   j,
                                   aExpIdx,
                                   &minGain,
                                   &maxGain);
        /* Not a valid stage */
        if (0U == maxGain) {
            break;
        }

        /* Not a valid stage(not expected as well!), break */
        if (aGain < minGain) {
            break;
        }

        if (aGain <= maxGain) {
            /* Found the stage */
            stageIdx = j;
            break;
        } else {
            /* save this stage and try for next one as well */
            stageIdx = j;
        }
    }

    return stageIdx;
}
/* CSN_MODULE_GetGainFromTable */
static void CSN_MODULE_GetGainFromTable(CSN_MODULE_ParseCfgType *aParseCfg,
                                        uint32_t aCompGainIdx,
                                        uint16_t aInGain,
                                        uint16_t *aOutCode,
                                        uint16_t *aOutGain)
{
    /* Not doing any NULL check as this is an internal function */
    uint32_t i;
    uint8_t tableIdx = aParseCfg->parseGainCfg[aCompGainIdx].useTable - 1U;
    uint16_t gainVal = aParseCfg->gainTable[tableIdx].val[0];
    uint16_t gainCode = aParseCfg->gainTable[tableIdx].code[0];

    /* TODO: tableIdx has not been checked run-time, do we need? */
    for (i = 0U; i < aParseCfg->gainTable[tableIdx].numVals; i++) {
        if (aInGain >= aParseCfg->gainTable[tableIdx].val[i]) {
            gainCode = aParseCfg->gainTable[tableIdx].code[i];
            gainVal = aParseCfg->gainTable[tableIdx].val[i];
        } else {
            break;
        }
    }

    *aOutCode = gainCode;
    *aOutGain = gainVal;
}

/* CSN_MODULE_CommonGetOps */
static int32_t CSN_MODULE_CommonGetOps(uint32_t aOpsType,
                                       CSN_MODULE_OpsParamType * const aOpsParam,
                                       CSN_MODULE_SetOpsType * const aOps)
{
    int32_t ret = BCM_ERR_OK;
    CSN_MODULE_GetOpsCfgType *getOpsCfg = &gCSN_MODULE_TunerCfg.getOpsCfg;
    CSN_MODULE_ParseCfgType *parseCfg = &gCSN_MODULE_TunerCfg.parseCfg;
    CSN_MODULE_GroupHoldCfgType *grHold;
    uint32_t i, j, k;
    uint16_t exposure[CSN_MODULE_EXP_TIME_IDX_MAX];
    uint16_t gain[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_MAX_GAIN_COMPS];
    uint16_t residualGain[CSN_MODULE_EXP_TIME_IDX_MAX];
    uint16_t wbGain[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_WG_GAIN_IDX_MAX];
    uint16_t wbGainOffset[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_WG_GAIN_IDX_MAX];

    if ((NULL == aOps) || (NULL == aOpsParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto func_exit;
    }

    if ((NULL == aOpsParam->expParam) || (NULL == aOpsParam->wbParam)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto func_exit;
    }

    /* Initialize count */
    aOps->count = 0UL;

    /* Add Group Hold */
    grHold = &getOpsCfg->ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START];
    for (i = 0UL; i < CSN_MODULE_MAX_NUM_GR_HLD_REGS; i++) {
        if (CSN_MODULE_INVALID_USHORT != grHold->addr[i]) {
            aOps->op[aOps->count].reg = grHold->addr[i];
            aOps->op[aOps->count].val = grHold->val[i];
            aOps->count++;
        }
    }

    /* Initialize gain */
    if (0UL != (aOpsType & CSN_MODULE_OP_GAIN_LONG)) {
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            residualGain[i] = CSN_MODULE_UP8_UNITY;
            for (j = 0UL; j < CSN_MODULE_MAX_GAIN_COMPS; j++) {
                gain[i][j] = CSN_MODULE_UP8_UNITY;
            }
        }
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_EXPOSURE_LONG)) {
        CSN_MODULE_ExpParamType *expP = aOpsParam->expParam;

        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            if (CSN_MODULE_INVALID_USHORT != getOpsCfg->expCfg[i].addr[0]) {
                uint16_t ratio = ((0UL == i) || (0UL == expP->expRatio)) ?
                                  1U : expP->expRatio;

                exposure[i] = (uint16_t) (expP->expLines[CSN_MODULE_EXP_IDX_LONG] / ratio);

                if (exposure[i] < getOpsCfg->expCfg[i].linesMin) {
                    exposure[i] = getOpsCfg->expCfg[i].linesMin;
                }

                if (exposure[i] >
                        getOpsCfg->expCfg[i].linesMax) {
                    residualGain[i] =
                            (CSN_MODULE_UP8_UNITY * exposure[i]) /
                             getOpsCfg->expCfg[i].linesMax;
                    exposure[i] =
                        getOpsCfg->expCfg[i].linesMax;
                }
            } else {
                exposure[i] = 0U;
            }
        }

        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            CSN_MODULE_WRITE_BYTES(&parseCfg->parseExpCfg,
                                   (uint32_t) exposure[i],
                                   getOpsCfg->expCfg[i].addr,
                                   aOps);
        }
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_GAIN_LONG)) {
        CSN_MODULE_ExpParamType *expP = aOpsParam->expParam;

        /* Decide on the gains */
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            uint16_t totalGain =  expP->gain[CSN_MODULE_EXP_IDX_LONG];
            uint16_t minGain = CSN_MODULE_UP8_UNITY;
            uint16_t maxGain = CSN_MODULE_UP8_UNITY;
            uint16_t preGain = CSN_MODULE_UP8_UNITY;
            uint8_t stageIdx = 0U;

            /* Multiply with residual gain */
            totalGain = (totalGain * residualGain[i] / CSN_MODULE_UP8_UNITY);

            /* Find pre-gain product for finding stage */
            for (k = 0UL; k < CSN_MODULE_MAX_GAIN_COMPS; k++) {
                if (0U != (getOpsCfg->gainParamCfg[i][k].ratio & CSN_MODULE_RATIO_PRE_MASK)) {
                    preGain = (preGain *
                        (getOpsCfg->gainParamCfg[i][k].ratio & CSN_MODULE_RATIO_VAL_MASK) /
                        CSN_MODULE_UP12_UNITY);
                }
            }

            /* Get stage index */
            stageIdx = CSN_MODULE_GetStageIdx(parseCfg,
                                              getOpsCfg,
                                              i,
                                             (totalGain *
                                              preGain / CSN_MODULE_UP8_UNITY));
            if (CSN_MODULE_MAX_NUM_GAIN_STAGES != stageIdx) {
                /* Stage is decided at this point */
                for (k = 0UL; k < CSN_MODULE_MAX_GAIN_COMPS; k++) {
                    uint16_t compGain = totalGain;

                    /* Find pre-gain for the gain component */
                    if (0U != (getOpsCfg->gainParamCfg[i][k].ratio & CSN_MODULE_RATIO_PRE_MASK)) {
                        compGain = (totalGain *
                                    (getOpsCfg->gainParamCfg[i][k].ratio & CSN_MODULE_RATIO_VAL_MASK) /
                                    CSN_MODULE_UP12_UNITY);
                    }

                    /* Find the minimum gain to be applied for other gains components in the stage */
                    CSN_MODULE_FindGainBoundary(parseCfg,
                                               getOpsCfg,
                                               (k+1U),
                                               stageIdx,
                                               i,
                                               &minGain,
                                               &maxGain);
                    compGain = (compGain  * CSN_MODULE_UP8_UNITY / minGain);

                    /* Keep min gain */
                    if (compGain < getOpsCfg->gainParamCfg[i][k].min[stageIdx]) {
                        compGain = getOpsCfg->gainParamCfg[i][k].min[stageIdx];
                    }

                    /* Ceil max gain */
                    if (compGain > getOpsCfg->gainParamCfg[i][k].max[stageIdx]) {
                        compGain = getOpsCfg->gainParamCfg[i][k].max[stageIdx];
                    }

                    /* Map the gain if there is a table */
                    if (0U != parseCfg->parseGainCfg[k].useTable) {
                        /* Find if there a code value exist for this */
                        CSN_MODULE_GetGainFromTable(parseCfg,
                                                    k,
                                                    compGain,
                                                    &gain[i][k],
                                                    &compGain);
                    } else {
                        /* Handle fixed point precision only for
                                                     the one without table */
                        if (0U != (parseCfg->parseGainCfg[k].shift &
                                   CSN_MODULE_SHIFT_DIR_MASK)) {
                            uint8_t shiftBits = (parseCfg->parseGainCfg[k].shift &
                                                 CSN_MODULE_SHIFT_COUNT_MASK);
                            compGain = (compGain >> shiftBits) << shiftBits;
                        }
                        gain[i][k] = (uint16_t) compGain;
                    }
                    totalGain = (totalGain * CSN_MODULE_UP8_UNITY / compGain);

                    /* Apply post gain */
                    if (0U == (getOpsCfg->gainParamCfg[i][k].ratio & CSN_MODULE_RATIO_PRE_MASK)) {
                        if (0U != parseCfg->parseGainCfg[k].useTable) {
                            compGain = (compGain *
                                        getOpsCfg->gainParamCfg[i][k].ratio /
                                        CSN_MODULE_UP12_UNITY);
                            CSN_MODULE_GetGainFromTable(parseCfg,
                                                        k,
                                                        compGain,
                                                        &gain[i][k],
                                                        &compGain);
                        } else {
                            gain[i][k] = (gain[i][k] *
                                        getOpsCfg->gainParamCfg[i][k].ratio /
                                        CSN_MODULE_UP12_UNITY);
                            if (gain[i][k] > getOpsCfg->gainParamCfg[i][k].max[stageIdx]) {
                                gain[i][k] = getOpsCfg->gainParamCfg[i][k].max[stageIdx];
                            }
                        }
                    }
                }
            }
        }

        /* Write Gains */
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            for (j = 0UL; j < CSN_MODULE_MAX_GAIN_COMPS; j++) {
                CSN_MODULE_WRITE_BYTES(&parseCfg->parseGainCfg[j],
                                       (uint32_t) gain[i][j],
                                       getOpsCfg->gainRegCfg[i][j].addr,
                                       aOps);
            }
        }
    }

    if (0UL != (aOpsType & CSN_MODULE_OP_WB)) {
        /* TODO: Handle GGain , min, max */
        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                if (CSN_MODULE_WG_GAIN_IDX_R == j) {
                    wbGain[i][j] = aOpsParam->wbParam->gainR;
                } else if (CSN_MODULE_WG_GAIN_IDX_B == j) {
                    wbGain[i][j] = aOpsParam->wbParam->gainB;
                } else {
                    wbGain[i][j] = CSN_MODULE_UP8_UNITY;
                }
                CSN_MODULE_WRITE_BYTES(&parseCfg->parseWBCfg.parseWBGainCfg[j],
                                       (uint32_t) wbGain[i][j],
                                       getOpsCfg->wbCfg[i].wbGainCfg[j].addr,
                                       aOps);
            }
        }

        for (i = 0UL; i < CSN_MODULE_EXP_TIME_IDX_MAX; i++) {
            for (j = 0UL; j < CSN_MODULE_WG_GAIN_IDX_MAX; j++) {
                wbGainOffset[i][j] = (wbGain[i][j] - getOpsCfg->wbCfg[i].wbOffsetCfg[j].sub) *
                                             getOpsCfg->wbCfg[i].wbOffsetCfg[j].mul;
                CSN_MODULE_WRITE_BYTES(&parseCfg->parseWBCfg.parseWBOffsetCfg,
                                       (uint32_t) wbGainOffset[i][j],
                                       getOpsCfg->wbCfg[i].wbOffsetCfg[j].addr,
                                       aOps);
            }
        }
    }

    /* Add Group Hold - End */
    grHold = &getOpsCfg->ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END];
    for (i = 0UL; i < CSN_MODULE_MAX_NUM_GR_HLD_REGS; i++) {
        if (CSN_MODULE_INVALID_USHORT != grHold->addr[i]) {
            aOps->op[aOps->count].reg = grHold->addr[i];
            aOps->op[aOps->count].val = grHold->val[i];
            aOps->count++;
        }
    }

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
            ULOG_INFO("\n");
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
            ULOG_INFO("\n");
#endif
        }
    }
#endif

func_exit:
    return ret;
}

/* CSN_MODULE_CommonGetClosestGain */
static int32_t CSN_MODULE_CommonGetClosestGain(uint32_t aOpsType,
                                     uint16_t * const aGain)
{
    int32_t ret = BCM_ERR_OK;

    return ret;
}

extern CSN_MODULE_TunerCfgType * CSN_MODULE_GetStaticConfig(CSN_MODULE_ConfigType *aConfig);

/* CSN_MODULE_CommonConfigure */
static int32_t CSN_MODULE_CommonConfigure(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    if (NULL != aConfig) {
        CSN_MODULE_TunerCfgType *cfg = CSN_MODULE_GetStaticConfig(aConfig);

        if (NULL != cfg) {
            if (NULL ==
                memcpy(&gCSN_MODULE_TunerCfg, cfg, sizeof(gCSN_MODULE_TunerCfg))) {
                    ret = BCM_ERR_UNKNOWN;
                }
        } else {
            ret = BCM_ERR_UNKNOWN;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/* CSN_MODULE_CommonParseStatistics */
static int32_t CSN_MODULE_CommonParseStatistics (const uint8_t * const (aBuffer)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                        CSN_MODULE_StatsType * const (aSensorStats)[CSN_MODULE_EXP_IDX_MAX],
                                        uint32_t *aParsedMask)
{
    int32_t ret = BCM_ERR_OK;

    return ret;
}

/* gCSN_MODULE_Common */
static const CSN_MODULE_Type gCSN_MODULE_Common =
{
    .parseMetadata = CSN_MODULE_CommonParseMetadata,
    .getOps = CSN_MODULE_CommonGetOps,
    .getClosestGain = CSN_MODULE_CommonGetClosestGain,
    .parseStatistics = CSN_MODULE_CommonParseStatistics,
    .sensorConfigure = CSN_MODULE_CommonConfigure,
};

/* CSN_MODULE_GetModule */
const CSN_MODULE_Type * CSN_MODULE_GetCommonModule(void)
{
    return &gCSN_MODULE_Common;
}
