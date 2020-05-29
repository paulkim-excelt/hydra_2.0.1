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
  File Name:  tuner.c
Description: This file implements tuner software.
 ******************************************************************************/

/* Includes */
#include <string.h>
#include <tuner.h>
#include <tuner_func_table.h>
#include <isys.h>
#include <isys_osil.h>
#include <camera_sensor.h>
#include <system.h>

static void Tuner_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                                uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CTN_ID, 0U, aApiID, aErr, 4UL, values);
}

/* Defines */
#define MAX_INT                 (0xffffffffUL)

/* Tuner module function access */
#define TUNER_MODULE_FUNC(x, y) (TunerGetFuncTable()[x]->y)
/* Number of tuners */
#define NUM_TUNERS              (TunerGetNumEntries())

/* Tuner API IDs for error reporting */
/* Range:  0x1 - 0x1f */
#define TUNER_API_INIT                (0x1U)
#define TUNER_API_DEINIT              (0x2U)
#define TUNER_API_START               (0x3U)
#define TUNER_API_STOP                (0x4U)
#define TUNER_API_CONFIGURE           (0x5U)
#define TUNER_API_METADATA_UPDATE     (0x6U)
#define TUNER_API_ISP_STATS_UPDATE    (0x7U)
#define TUNER_API_SENSOR_STATS_UPDATE (0x8U)
#define TUNER_API_PROCESS_EVENT       (0x9U)

/* Tuner internal errors - used for error reporting */
#define TUNER_ERROR_CFG_NULL_PTR    (0x544e0001UL)
#define TUNER_ERROR_CFG_TYPE        (0x544e0002UL)
#define TUNER_ERROR_CFG_MAGIC       (0x544e0003UL)
#define TUNER_ERROR_CFG_COUNT       (0x544e0004UL)

/* Tuner State */
typedef struct {
    uint32_t state;
    uint32_t prevState;
    Tuner_CommonConfig config;
    uint32_t tunerCfgType[MAX_TUNERS];
    uint8_t *edBuf;
    uint32_t edBufSize;
    CSN_MODULE_MetadataType metadata;
    ISYS_StatisticsType stats;
    CSN_MODULE_StatsType sensorStats[CSN_MODULE_EXP_IDX_MAX];
    uint8_t tunerDisable[MAX_TUNERS];
    CSN_EmbOffsetsType embOffsets;
} TunerState;

/* Global Tuner gTunerState */
static TunerState gTunerState =
{
    .state = TUNER_STATE_RESET,
    .prevState = TUNER_STATE_RESET,
};

/******************************************************************************
  FUNCTION NAME: Tuner_Init

ARGUMENTS: None

DESCRIPTION: This interface initializes Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_VERSION
BCM_ERR_UNKNOWN

 ******************************************************************************/
int32_t Tuner_Init(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;

    if (TUNER_STATE_RESET != gTunerState.state) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_INIT,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        for (i=0UL; i < NUM_TUNERS; i++) {
            gTunerState.tunerDisable[i] = 0U;
            if (NULL != TUNER_MODULE_FUNC(i, getCfgType)) {
                uint32_t cfgType = TUNER_MODULE_FUNC(i, getCfgType)();
                gTunerState.tunerCfgType[i] = cfgType;
            }

            if (NULL != TUNER_MODULE_FUNC(i, init)) {
                ret = TUNER_MODULE_FUNC(i, init)();
                if (BCM_ERR_OK != ret) {
                    gTunerState.tunerDisable[i] = 1U;
                    Tuner_ReportError(TUNER_API_INIT,
                            ret, i, 0UL, 0UL, 0UL);
                    ret = BCM_ERR_OK;
                }
            }
        }
    }

    gTunerState.prevState = gTunerState.state;
    gTunerState.state = TUNER_STATE_INITIALISED;

func_exit:
    return ret;
}

/******************************************************************************
  FUNCTION NAME: Tuner_Deinit

ARGUMENTS: None

DESCRIPTION: This interface de-initializes Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_UNKNOWN

 ******************************************************************************/
int32_t Tuner_Deinit(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;

    if (TUNER_STATE_RESET == gTunerState.state) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_DEINIT,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        for (i=0UL; i < NUM_TUNERS; i++) {
            if (NULL != TUNER_MODULE_FUNC(i, deinit)) {
                ret = TUNER_MODULE_FUNC(i, deinit)();
                if (BCM_ERR_OK != ret) {
                    Tuner_ReportError(TUNER_API_DEINIT,
                            ret, i, 0UL, 0UL, 0UL);
                    ret = BCM_ERR_OK;
                }
            }
        }
    }

    gTunerState.prevState = gTunerState.state;
    gTunerState.state = TUNER_STATE_RESET;

func_exit:
    return ret;
}


/******************************************************************************
  FUNCTION NAME: Tuner_Start

ARGUMENTS: None

DESCRIPTION: This interface starts Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE

 ******************************************************************************/
int32_t Tuner_Start(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;

    if (TUNER_STATE_READY != gTunerState.state) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_START,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        CSN_GetEmbOffsets(&gTunerState.embOffsets);
        for (i=0UL; i < NUM_TUNERS; i++) {
            if ((NULL != TUNER_MODULE_FUNC(i, start)) &&
                (0U == gTunerState.tunerDisable[i])) {
                ret = TUNER_MODULE_FUNC(i, start());
                if (BCM_ERR_OK != ret) {
                    gTunerState.tunerDisable[i] = 1U;
                    Tuner_ReportError(TUNER_API_START,
                            ret, i, 0UL, 0UL, 0UL);
                    ret = BCM_ERR_OK;
                }
            }
        }
    }

    gTunerState.prevState = gTunerState.state;
    gTunerState.state = TUNER_STATE_RUNNING;

func_exit:
    return ret;
}


/******************************************************************************
  FUNCTION NAME: Tuner_Stop
ARGUMENTS: None

DESCRIPTION: This interface stops Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_UNKNOWN

 ******************************************************************************/
int32_t Tuner_Stop(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;

    if (TUNER_STATE_RUNNING != gTunerState.state) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_STOP,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        for (i=0UL; i < NUM_TUNERS; i++) {
            if (NULL != TUNER_MODULE_FUNC(i, stop)) {
                ret = TUNER_MODULE_FUNC(i, stop)();
                if (BCM_ERR_OK != ret) {
                    Tuner_ReportError(TUNER_API_STOP,
                            ret, i, 0UL, 0UL, 0UL);
                    ret = BCM_ERR_OK;
                }
            }
        }
    }

    gTunerState.prevState = gTunerState.state;
    gTunerState.state = TUNER_STATE_READY;

func_exit:
    return ret;
}


/******************************************************************************
  FUNCTION NAME: Tuner_Configure

ARGUMENTS:
Tuner_Image* -> Pointer to config struct input

DESCRIPTION: This interface sets a static configuration to Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_INVAL_PARAMS

 ******************************************************************************/
int32_t Tuner_Configure(const Tuner_Image *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;
    uint32_t j = 0UL;
    ISYS_InfoType info;

    if ((TUNER_STATE_INITIALISED != gTunerState.state) &&
        (TUNER_STATE_READY != gTunerState.state)) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_CONFIGURE,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        /* Check Parameter */
        if (NULL == aCfg) {
            ret =  BCM_ERR_INVAL_PARAMS;
            Tuner_ReportError(TUNER_API_CONFIGURE,
                    ret, TUNER_ERROR_CFG_NULL_PTR, 0UL, 0UL, 0UL);
            goto func_exit;
        }

        /* Check Config type */
        if (CAM_CONFIG_TUNER != aCfg->imgHdr.commonHdr.cfgType) {
            ret  = BCM_ERR_INVAL_PARAMS;
            Tuner_ReportError(TUNER_API_CONFIGURE,
                    ret, TUNER_ERROR_CFG_TYPE, aCfg->imgHdr.commonHdr.cfgType, 0UL, 0UL);
            goto func_exit;
        }

        /* Check Magic */
        if (TUNER_MAGIC != aCfg->imgHdr.commonHdr.cfgMagic) {
            ret = BCM_ERR_INVAL_PARAMS;
            Tuner_ReportError(TUNER_API_CONFIGURE,
                    ret, TUNER_ERROR_CFG_MAGIC, aCfg->imgHdr.commonHdr.cfgMagic, 0UL, 0UL);
            goto func_exit;
        }

        /* Check number of configs */
        if (aCfg->imgHdr.numConfigs > TUNER_MAX_CONFIGS) {
            ret =  BCM_ERR_INVAL_PARAMS;
            Tuner_ReportError(TUNER_API_CONFIGURE,
                    ret, TUNER_ERROR_CFG_COUNT, aCfg->imgHdr.numConfigs, 0UL, 0UL);
            goto func_exit;
        }

        for (i=0UL; i < aCfg->imgHdr.numConfigs; i++) {
            const TunerCfg cfg = {
                .cfgData.ptr = &aCfg->tunerCfgData[aCfg->cfgs[i].dataOffset],
                .size = aCfg->cfgs[i].count
            };

            if (TUNER_CONFIG_COMMON == aCfg->cfgs[i].cfgType) {
                gTunerState.config = *(cfg.cfgData.commonCfg);
            } else {
                uint32_t tunerIdx = MAX_INT;
                for (j=0UL; j < NUM_TUNERS; j++) {
                    if (gTunerState.tunerCfgType[j] == aCfg->cfgs[i].cfgType) {
                        tunerIdx = j;
                        break;
                    }
                }

                if (MAX_INT != tunerIdx) {
                    if ((NULL != TUNER_MODULE_FUNC(tunerIdx, configure)) &&
                        (0U == gTunerState.tunerDisable[tunerIdx])){
                        ret = TUNER_MODULE_FUNC(tunerIdx, configure)(
                                &cfg);
                        if (BCM_ERR_OK != ret) {
                            gTunerState.tunerDisable[tunerIdx] = 1U;
                            Tuner_ReportError(TUNER_API_CONFIGURE,
                                    ret, i, tunerIdx, 0UL, 0UL);
                            ret = BCM_ERR_OK;
                        }
                    }
                }
            }
        }
        /* Update ED mem */
        ISYS_GetInfo(&info);
        gTunerState.edBuf = info.tunerMem;
        gTunerState.edBufSize = info.tunerMemSize;
        if ((NULL == gTunerState.edBuf) || (0UL ==  gTunerState.edBufSize)) {
            ret =  BCM_ERR_NOMEM;
            Tuner_ReportError(TUNER_API_CONFIGURE,
                    ret, __LINE__, 0UL, 0UL, 0UL);
            goto func_exit;
        }
    }

    gTunerState.prevState = gTunerState.state;
    gTunerState.state = TUNER_STATE_READY;

func_exit:
    return ret;
}


/******************************************************************************
  FUNCTION NAME: Tuner_MetadataUpdate

ARGUMENTS:

DESCRIPTION: This interface submits metadata update to tuners.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_INVAL_PARAMS

 ******************************************************************************/
static int32_t Tuner_MetadataUpdate(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;
    uint32_t edSize[ISYS_SENSOR_EXP_IDX_MAX] = {0UL, 0UL};
    uint8_t * edBuf[ISYS_SENSOR_EXP_IDX_MAX] = {NULL, NULL};

    if (TUNER_STATE_RUNNING != gTunerState.state) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_METADATA_UPDATE,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        uint32_t usedSize = 0UL;

        /* Fill offsets and sizes */
        for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
            edBuf[i] = gTunerState.edBuf + usedSize;
            edSize[i] = (uint32_t) gTunerState.embOffsets.metaSize[i];
            usedSize += edSize[i];
        }

        /* Check sizes */
        if (usedSize > gTunerState.edBufSize) {
            ret = BCM_ERR_NOMEM;
            Tuner_ReportError(TUNER_API_METADATA_UPDATE,
                    ret, __LINE__, usedSize, 0UL, 0UL);
            goto func_exit;
        }

        for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
            if (0U != gTunerState.embOffsets.metaSize[i]) {
                ret = ISYS_GetMetadataBuf(i,
                                         edBuf[i],
                                         gTunerState.embOffsets.metaOffset[i],
                                         gTunerState.embOffsets.metaSize[i]);
                if (BCM_ERR_OK != ret) {
                    goto func_exit;
                }
            }
        }

        /* Initialize params */
        gTunerState.metadata.gainR = 256U;
        gTunerState.metadata.gainB = 256U;
        gTunerState.metadata.wbEnabled = 0U;

        {
            const uint8_t * const edBufP[ISYS_SENSOR_EXP_IDX_MAX] =
                                               {edBuf[CSN_MODULE_EXP_IDX_LONG],
                                                edBuf[CSN_MODULE_EXP_IDX_SHORT]};

            ret = CSN_ParseMetadata(edBufP,
                                    edSize,
                                    &gTunerState.metadata);
            if (BCM_ERR_OK != ret) {
                goto func_exit;
            }
        }

        /* Now that all params are validated, run the actual tuner loop.
           Error reported from this is not fatal and need not be propagated to
           higher layers.
           */
        for (i=0UL; i < NUM_TUNERS; i++) {
            if ((NULL != TUNER_MODULE_FUNC(i, metadataUpdate)) &&
                (0U == gTunerState.tunerDisable[i])) {
                ret = TUNER_MODULE_FUNC(i, metadataUpdate)(&gTunerState.metadata);
                if (BCM_ERR_OK != ret) {
                    ret = BCM_ERR_OK;
                }
            }
        }
    }

func_exit:
    return ret;
}


/******************************************************************************
  FUNCTION NAME: Tuner_ISP_StatsUpdate

ARGUMENTS:

DESCRIPTION: This interface submits ISP statistics update to tuners.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_INVAL_PARAMS

 ******************************************************************************/
static int32_t Tuner_ISP_StatsUpdate(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;

    if (TUNER_STATE_RUNNING != gTunerState.state) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_ISP_STATS_UPDATE,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        /* Copy Statistics */
        ret = ISYS_GetStatistics(&gTunerState.stats);
        if (BCM_ERR_OK != ret) {
            goto func_exit;
        }

        /* Now that all params are validated, run the actual tuner loop.
           Error reported from this is not fatal and need not be propagated to
           higher layers.
           */
        for (i = 0UL; i < NUM_TUNERS; i++) {
            if ((NULL != TUNER_MODULE_FUNC(i, ispStatsUpdate)) &&
                (0U == gTunerState.tunerDisable[i])) {
                ret = TUNER_MODULE_FUNC(i, ispStatsUpdate)(&gTunerState.stats);
                if (BCM_ERR_OK != ret) {
                    ret = BCM_ERR_OK;
                }
            }
        }
    }
func_exit:
    return ret;
}


/******************************************************************************
  FUNCTION NAME: Tuner_SensorStatsUpdate

ARGUMENTS:

DESCRIPTION: This interface submits sensor statistics update to tuners.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_INVAL_PARAMS

 ******************************************************************************/
static int32_t Tuner_SensorStatsUpdate(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i = 0UL;
    uint32_t frameNum = 0UL;
    uint32_t parsedMask = 0UL;
    uint32_t edSize[ISYS_SENSOR_EXP_IDX_MAX] = {0UL, 0UL};
    uint8_t * edBuf[ISYS_SENSOR_EXP_IDX_MAX] = {NULL, NULL};
    CSN_MODULE_StatsType * const sensorStats[CSN_MODULE_EXP_IDX_MAX] = {
                            &gTunerState.sensorStats[CSN_MODULE_EXP_IDX_LONG],
                            &gTunerState.sensorStats[CSN_MODULE_EXP_IDX_SHORT]};

    if (TUNER_STATE_RUNNING != gTunerState.state) {
        ret = BCM_ERR_INVAL_STATE;
        Tuner_ReportError(TUNER_API_ISP_STATS_UPDATE,
                ret, gTunerState.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        uint32_t usedSize = 0UL;

        /* Fill offsets and sizes */
        for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
            edBuf[i] = gTunerState.edBuf + usedSize;
            edSize[i] = (uint32_t) gTunerState.embOffsets.statsSize[i];
            usedSize += edSize[i];
        }

        /* Check sizes */
        if (usedSize > gTunerState.edBufSize) {
            ret = BCM_ERR_NOMEM;
            Tuner_ReportError(TUNER_API_SENSOR_STATS_UPDATE,
                    ret, __LINE__, usedSize, 0UL, 0UL);
            goto func_exit;
        }

        for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
            if (0U != gTunerState.embOffsets.statsSize[i]) {
                ret = ISYS_GetSensorStatsBuf(i,
                                         edBuf[i],
                                         gTunerState.embOffsets.statsOffset[i],
                                         gTunerState.embOffsets.statsSize[i],
                                         &frameNum);
                if (BCM_ERR_OK != ret) {
                    goto func_exit;
                }
            }
        }
        {
            const uint8_t * const edBufP[ISYS_SENSOR_EXP_IDX_MAX] =
                                              {edBuf[CSN_MODULE_EXP_IDX_LONG],
                                               edBuf[CSN_MODULE_EXP_IDX_SHORT]};

            ret = CSN_ParseStatistics(edBufP,
                                      edSize,
                                      sensorStats,
                                      &parsedMask);
            if (BCM_ERR_OK != ret) {
                goto func_exit;
            }
            /* Tuner framework can choose to combine the stats based on
               parsedMask */
        }

        for (i=0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
            gTunerState.sensorStats[i].frameNum = frameNum;
        }
        /* Now that all params are validated, run the actual tuner loop.
           Error reported from this is not fatal and need not be propagated to
           higher layers.
           */
        for (i = 0UL; i < NUM_TUNERS; i++) {
            if ((NULL != TUNER_MODULE_FUNC(i, sensorStatsUpdate)) &&
                (0U == gTunerState.tunerDisable[i])) {
                ret = TUNER_MODULE_FUNC(i, sensorStatsUpdate)
                            (sensorStats, parsedMask);
                if (BCM_ERR_OK != ret) {
                    ret = BCM_ERR_OK;
                }
            }
        }
    }

func_exit:
    return ret;
}


/******************************************************************************
  FUNCTION NAME: Tuner_ProcessEvent

ARGUMENTS:
Tuner_EventType -> event type.
uint32_t        -> Index in case of PROCESS event.

DESCRIPTION: This interface process tuner Events.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_INVAL_PARAMS
BCM_ERR_UNKNOWN

 ******************************************************************************/
int32_t Tuner_ProcessEvent(Tuner_EventType aEvent, uint32_t aIndex)
{
    int32_t ret = BCM_ERR_OK;

    if (TUNER_STATE_RUNNING == gTunerState.state) {
        switch(aEvent) {
            case TUNER_EVENT_SENSOR_META_UPDATE:
                ret = Tuner_MetadataUpdate();
                break;
            case TUNER_EVENT_SENSOR_STATS_UPDATE:
                ret = Tuner_SensorStatsUpdate();
                break;
            case TUNER_EVENT_ISP_STATS_UPDATE:
                ret = Tuner_ISP_StatsUpdate();
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                Tuner_ReportError(TUNER_API_PROCESS_EVENT,
                        ret, 0UL, 0UL, 0UL, 0UL);
                break;
        }
    }
    return ret;
}

/******************************************************************************
  FUNCTION NAME: Tuner_GetState

ARGUMENTS: None

DESCRIPTION: This interface gets present tuner State.

RETURNS:
Tuner State

 ******************************************************************************/
Tuner_State Tuner_GetState(void)
{
    return gTunerState.state;
}

/******************************************************************************
  FUNCTION NAME: Tuner_GetIDX

ARGUMENTS:
aConfigType -> Config type for tuner.

DESCRIPTION: This interface gets tuner index.

RETURNS:
Tuner Index.

 ******************************************************************************/
uint32_t Tuner_GetIDX(uint32_t aConfigType)
{
    uint32_t tunerIdx = MAX_INT;
    uint32_t i;

    for (i=0UL; i < NUM_TUNERS; i++) {
        if (gTunerState.tunerCfgType[i] == aConfigType) {
            tunerIdx = i;
            break;
        }
    }

    return tunerIdx;
}
