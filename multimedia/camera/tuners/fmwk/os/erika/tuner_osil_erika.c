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
  File Name:  tuner_osil_erika.c
Descritpion: This file implements tuner OSI for Erika.
 ******************************************************************************/

/* Includes */
#include "cam_erika.h"
#include <isys.h>
#include <isys_tuner.h>
#include <i2c.h>
#include "isys_osil.h"
#include "camera_sensor.h"
#include "camera_sensor_osil.h"
#include <tuner.h>
#include <ctn_log.h>
#include <compiler.h>
#include "ee.h"
#include <system.h>
#include <string.h>
#include <osil/bcm_osil.h>

static void TunerOsil_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CTN_ID, 0U, aApiID, aErr, 4UL, values);
}

/* Tuner OSIL API IDs for error reporting */
/* Range:  0xe0 - 0xff */
#define TUNER_OSIL_API_TASK                  (0xe0U)
#define TUNER_OSIL_API_CS_PARSE_MD           (0xe1U)
#define TUNER_OSIL_API_CS_SET_PARAM          (0xe2U)
#define TUNER_OSIL_API_CS_TEST_PARAM         (0xe3U)
#define TUNER_OSIL_API_CS_PARSE_SENSOR_STATS (0xe4U)

/* Tuner Task state */
#define TUNER_TASK_STATE_RUNNING      (0x0UL)
#define TUNER_TASK_STATE_WAIT_CLEANUP (0x1UL)
#define TUNER_TASK_STATE_STOPPED      (0x2UL)

/* Tuner Task */
TASK(Camera_TunerTask)
{
    uint32_t taskState = TUNER_TASK_STATE_RUNNING;
    int32_t ret = BCM_ERR_OK;
    int32_t retVal;
    BCM_EventMaskType mask = 0x0UL;
    uint32_t event = 0x0UL;

    /* Initialize CTN Logging */
    ret = CTN_InitLog();
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                ret, __LINE__, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    do {
        retVal = BCM_WaitEvent(ISP_STATS_EVENT|
                SENSOR_META_EVENT|
                SENSOR_STATS_EVENT|
                TUNER_EVENT_QUIT|
                CSN_EVENT_IIC |
                UDPTxEvent
                );
        if (BCM_ERR_OK != retVal) {
            TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                    retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        retVal = BCM_GetEvent(CAMERA_TUNER_TASK, &mask);
        if (BCM_ERR_OK != retVal) {
            TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                    retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        retVal = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != retVal) {
            TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                    retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        if ((TUNER_STATE_RUNNING == Tuner_GetState())) {
#if ENABLE_CAMERA_SENSOR
            if (0UL != (mask & CSN_EVENT_IIC)) {
                event = CSN_ProcessEvent(CSN_CLIENT_IDX_TUNER, CSN_EVENT_SOURCE_IIC);
                if (0UL != (event & CSN_STATUS_ERROR)) {
                    retVal = BCM_SetEvent(CAMERA_TASK, TUNER_EVENT_ERROR);
                    if (BCM_ERR_OK != retVal) {
                        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                                retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                    }
                }
            }
#endif /* ENABLE_CAMERA_SENSOR */
            if (0UL != (mask & SENSOR_META_EVENT)) {
                ret = Tuner_ProcessEvent(TUNER_EVENT_SENSOR_META_UPDATE, 0UL);
                if(BCM_ERR_OK != ret) {
                    retVal = BCM_SetEvent(CAMERA_TASK, TUNER_EVENT_ERROR);
                    if (BCM_ERR_OK != retVal) {
                        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                                retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                    }
                }
            }

            if (0UL != (mask & ISP_STATS_EVENT)) {
                ret = Tuner_ProcessEvent(TUNER_EVENT_ISP_STATS_UPDATE, 0UL);
                if(BCM_ERR_OK != ret) {
                    retVal = BCM_SetEvent(CAMERA_TASK, TUNER_EVENT_ERROR);
                    if (BCM_ERR_OK != retVal) {
                        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                                retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                    }
                }
            }

            if (0UL != (mask & SENSOR_STATS_EVENT)) {
                ret = Tuner_ProcessEvent(TUNER_EVENT_SENSOR_STATS_UPDATE, 0UL);
                if(BCM_ERR_OK != ret) {
                    retVal = BCM_SetEvent(CAMERA_TASK, TUNER_EVENT_ERROR);
                    if (BCM_ERR_OK != retVal) {
                        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                                retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                    }
                }
            }
        }

        if (0UL != (mask & TUNER_EVENT_QUIT)) {
            taskState = TUNER_TASK_STATE_WAIT_CLEANUP;
#if ENABLE_CAMERA_SENSOR
            event = CSN_ProcessEvent(CSN_CLIENT_IDX_TUNER, CSN_EVENT_SOURCE_IIC);
#else
            event = 0UL;
#endif /* ENABLE_CAMERA_SENSOR */
            if (0UL == (event & CSN_STATUS_BUSY)) {
                taskState = TUNER_TASK_STATE_STOPPED;
            }
        }

        /* Push logs through UDP */
        ret = CTN_ProcessLogs();
        if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
            TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                    retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

    } while (TUNER_TASK_STATE_STOPPED != taskState);

    /* De-Initialize CTN Logging */
    ret = CTN_DeinitLog();
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOSUPPORT != ret)) {
        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    retVal = BCM_SetEvent(CAMERA_TASK, TUNER_EVENT_CMD_COMPLETE);
    if (BCM_ERR_OK != retVal) {
        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }

func_exit:
    retVal = BCM_TerminateTask();
    if(BCM_ERR_OK != retVal) {
        TunerOsil_ReportError(TUNER_OSIL_API_TASK,
                retVal, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }
}

int32_t ISYS_ParseFrameNum(const uint8_t *aBufferLong,
                           uint32_t aSizeLong,
                           const uint8_t *aBufferShort,
                           uint32_t aSizeShort,
                           uint32_t *aFrameNum)
{
#if ENABLE_CAMERA_SENSOR
    int32_t ret;
    CSN_MODULE_MetadataType metadata;
    CSN_EmbOffsetsType embOffsets;

    /* TODO: Need not call for every frame */
    CSN_GetEmbOffsets(&embOffsets);

    metadata.frameNum = 0UL;
    {
        const uint8_t * const edBufP[ISYS_SENSOR_EXP_IDX_MAX] =
                          {(aBufferLong +
                            embOffsets.metaOffset[CSN_MODULE_EXP_IDX_LONG]),
                           (aBufferShort +
                            embOffsets.metaOffset[CSN_MODULE_EXP_IDX_SHORT])};
        uint32_t edSize[ISYS_SENSOR_EXP_IDX_MAX] = {
                            embOffsets.metaSize[CSN_MODULE_EXP_IDX_LONG],
                            embOffsets.metaSize[CSN_MODULE_EXP_IDX_SHORT]};
        ret = CSN_ParseMetadata(edBufP,
                                edSize,
                                &metadata);
        if (NULL != aFrameNum) {
            *aFrameNum = metadata.frameNum;
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }
    return ret;
#else
    return BCM_ERR_OK;
#endif /* ENABLE_CAMERA_SENSOR */
}
