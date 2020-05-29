/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
#include <bcm_time.h>
#include <osil/bcm_osil.h>

extern uint32_t GetAPPLogLevel(void);
int32_t DisplayIT_Stop(void);

#define GetModuleLogLevel   GetAPPLogLevel

#define CMD_START_STR_LEN         (6UL)
#define CMD_STOP_STR_LEN          (5UL)

uint32_t GetAPPLogLevel(void)
{
    return ULOG_LVL_INFO;
}

static const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 2UL,

    .component = {
        {
            .compID = MSYS_COMP_ID_EX_SRC,
            .numConfigs = 1UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = MSYS_COMP_ID_VGCC,
            .numConfigs = 1UL,
            .config = {0UL},
            .portMap = {
                {
                    .srcCompIdx = 0U,
                    .srcPortIdx = 0U,
                },
                {
                    .srcCompIdx = 0U,
                    .srcPortIdx = 1U,
                },
                {
                    .srcCompIdx = 0U,
                    .srcPortIdx = 0U,
                },
                {
                    .srcCompIdx = 0U,
                    .srcPortIdx = 0U,
                }
            }
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        },
        {
            .compID = 0ULL,
            .numConfigs = 0UL,
            .config = {0UL},
            .portMap = {{0U,0U},{0U,0U},{0U,0U},{0U,0U}},
        }
    }
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
typedef uint32_t DisplayIT_Type;
#define DISPLAY_IT0     (0UL)               /**< Verify display starts */
#define DISPLAY_ITMAX   (DISPLAY_IT0)   /**< Max test cases*/

/** System Event Mapping */
#define DISPLAY_IT_EVENT1  (SystemEvent1)      /**< stop */

static uint32_t alarmCount;

static volatile int32_t DisplayITRes[DISPLAY_ITMAX + 1UL] =
{
BCM_AT_NOT_AVAILABLE,
};

static int32_t DisplayIT_Start(void)
{
    int32_t ret;

    ULOG_ERR("%s\n", __FUNCTION__);
    (void)BCM_ActivateTask(ExSrc_CmdProcessTask);

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

    ret = BCM_SetRelAlarm(DisplayIT_Alarm, 1000, 0);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("BCM_SetRelAlarm Failed %d\n",ret);
        goto err;
    }

err:
    return ret;
}

int32_t DisplayIT_Stop(void)
{
    int32_t ret;

    ULOG_ERR("%s started\n", __FUNCTION__);

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Stop Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_DeInit Failed %d\n",ret);
    }

err:
    return ret;
}

void DisplayIT_AlarmCB(void)
{
    (void)SetEvent(DisplayIT_Task, DISPLAY_IT_EVENT1);
}

TASK(DisplayIT_Task)
{
    int32_t ret;
    BCM_EventMaskType mask = 0U;

    ret = DisplayIT_Start();
    if (BCM_ERR_OK != ret) {
        DisplayITRes[DISPLAY_IT0] = ret;
        ULOG_ERR("DisplayIT_Start failed %d\n",ret);
    } else {
        (void)BCM_WaitEvent(DISPLAY_IT_EVENT1);
        (void)BCM_GetEvent(DisplayIT_Task, &mask);
        if (0U != (mask & DISPLAY_IT_EVENT1)) {
            (void)BCM_ClearEvent(DISPLAY_IT_EVENT1);
            DisplayITRes[DISPLAY_IT0] = DisplayIT_Stop();
        }
    }

    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t arrayIndex = aIndex - 1UL;

    switch (arrayIndex) {
    case DISPLAY_IT0:
        if (BCM_AT_EXECUTING != DisplayITRes[arrayIndex]) {
            alarmCount = 0;
            DisplayITRes[arrayIndex] = BCM_AT_EXECUTING;
            ret = BCM_ActivateTask(DisplayIT_Task);
            if(BCM_ERR_OK != ret) {
                DisplayITRes[arrayIndex] = ret;
                ULOG_ERR("BCM_ActivateTask failed %d\n",ret);
            }
        } else {
            ret = BCM_ERR_BUSY;
        }
        break;
    default:
        ret = BCM_AT_NOT_AVAILABLE;
        break;
    }

    ULOG_ERR("DisplayIT Execute IT%lu stat=%d\n", aIndex, ret);
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret;
    uint32_t arrayIndex = aIndex - 1UL;

    if (arrayIndex > DISPLAY_ITMAX) {
        ret = BCM_AT_NOT_AVAILABLE;
    } else {
        ret = DisplayITRes[arrayIndex];
    }

    ULOG_ERR("DisplayIT getResult IT%lu stat=%d\n", aIndex, ret);
    return ret;
}

void ApplicationInit(void)
{

}
