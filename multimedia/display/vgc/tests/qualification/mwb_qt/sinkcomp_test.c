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
 File Name: sinkcomp_test.c
 Descritpion: This file implements application interface for media framework.
******************************************************************************/

/* Includes */
#include <stddef.h>
#include <compiler.h>
#include <string.h>

#include "media_system.h"
#include "component.h"
#include "ee.h"
#include "ulog.h"
#include "console.h"
#include <bcm_err.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

extern uint32_t GetAPPLogLevel();
int32_t MwbIT_Stop(void);
#define GetModuleLogLevel   GetAPPLogLevel

#define CMD_START_STR_LEN         (6UL)
#define CMD_STOP_STR_LEN          (5UL)

uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}

const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 3UL,

    .component[0].compID = MSYS_COMP_ID_EX_SRC,
    .component[0].numConfigs = 1UL,
    .component[0].config[0] = 0UL,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_VGCC,
    .component[1].numConfigs = 1UL,
    .component[1].config[0] = 0UL,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 1U,

    .component[2].compID = MSYS_COMP_ID_EX_SNK,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = 0UL,
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,
};

/*
 * This API is invoked by the framework to retrieve the media system connection
 * information.
 */
const MSYS_Type* MSYS_Get(uint32_t aIdx)
{
    const MSYS_Type* mSys = NULL;
    if(0UL == aIdx) {
        mSys = &mediaSys;
    }
    return mSys;
}


void MSYS_WdsErrorCb(const char* aName,
	             int32_t aError,
                     const WDS_MonitorType *aMonitor,
		     uint32_t aMaxAllowedErrors)
{
}

/** Integration Test Number Enumeration */
typedef uint32_t MWB_IT_Type;
#define MWB_IT0     (0UL)               /**< Verify display starts */
#define MWB_ITMAX   (MWB_IT0)   /**< Max test cases*/

/** System Event Mapping */
#define MWB_IT_EVENT0  (SystemEvent0)      /**< Verify display starts */
#define MWB_IT_EVENT1  (SystemEvent1)      /**< Display stop */

#define MWB_IT_ALL_EVENTS  (SystemEvent0   \
                                | SystemEvent1)

static uint32_t alarmCount;

static volatile int32_t MwbITRes[MWB_ITMAX + 1UL] =
{
BCM_AT_NOT_AVAILABLE,
};

int32_t MwbIT_Start(void)
{
    int32_t ret;
    StatusType status;

    ULOG_ERR("%s\n", __FUNCTION__);

    BCM_ActivateTask(ExSrc_CmdProcessTask);

    ret = MSYS_Init(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        goto err;
    }

    status = SetRelAlarm(MwbIT_Alarm, 1000, 0);
    if (E_OK != status) {
        ret = BCM_ERR_UNKNOWN;
        goto err;
    }

err:
    return ret;
}

int32_t MwbIT_Stop(void)
{
    int32_t ret;

    ULOG_ERR("%s\n", __FUNCTION__);
    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
    }

err:
    return ret;
}

void MwbIT_AlarmCB()
{
    if (BCM_AT_EXECUTING == MwbITRes[0]) {
        alarmCount++;
        if (10UL == alarmCount) {
            SetEvent(MwbIT_Task, MWB_IT_EVENT1);
        }
    }
}

TASK(MwbIT_Task)
{
    BCM_EventMaskType mask = 0U;

    while (1) {
        BCM_WaitEvent(MWB_IT_ALL_EVENTS);
        BCM_GetEvent(MwbIT_Task, &mask);
        if (0U != (mask & MWB_IT_EVENT0)) {
            BCM_ClearEvent(MWB_IT_EVENT0);
            MwbIT_Start();
        }

        if (0U != (mask & MWB_IT_EVENT1)) {
            BCM_ClearEvent(MWB_IT_EVENT1);
            MwbITRes[MWB_IT0] = MwbIT_Stop();
        }
    }
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t testID = aIndex - 1UL;

    if (BCM_AT_EXECUTING == MwbITRes[testID])
        return BCM_AT_EXECUTING;

    switch (testID) {
    case MWB_IT0:
        alarmCount = 0;
        MwbITRes[testID] = BCM_AT_EXECUTING;
        BCM_SetEvent(MwbIT_Task, MWB_IT_EVENT0);
        break;
    default:
        ret = BCM_AT_NOT_AVAILABLE;
        break;
    }

    ULOG_ERR("MwbIT Execute IT%lu stat=%d\n", aIndex, ret);
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret;
    uint32_t testID = aIndex - 1UL;

    if (BCM_AT_EXECUTING == MwbITRes[testID])
        return BCM_AT_EXECUTING;

    if (testID > MWB_ITMAX) {
        ret = BCM_AT_NOT_AVAILABLE;
    } else {
        ret = MwbITRes[testID];
    }

    ULOG_ERR("MwbIT getResult IT%lu stat=%d\n", aIndex, ret);
    return ret;
}

void ApplicationInit()
{
    BCM_ActivateTask(MwbIT_Task);
}
