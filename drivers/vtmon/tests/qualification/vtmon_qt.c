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
#include "ee.h"
#include <vtmon.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <shell.h>
#include <osil/vtmon_osil.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()             (ULOG_LVL_ERROR)

#define VTMON_IT1                           (1UL)

/* VTMON wait time before first valid sample is available
 *
 * time = vtmon sampling time for each montors x no of monintors)
 *      = ~650us * 4 = 2600us
 */
#define VTMON_SAMPLING_TIME_ALL_MONITORS_NS    (2400000UL)

static int32_t VTMONIT1_Status = BCM_AT_NOT_STARTED;
static const VTMON_ConfigType VTMON_Config = {
    .tempConfig = {
        .threshLow = 0,
        .threshHigh = 15,
    },
    .voltConfig = {
        {
            .ID = VTMON_VOLT_ID_1V0,
            .threshLow = 900,
            .threshHigh = 1100,
        },
        {
            .ID = VTMON_VOLT_ID_1V8,
            .threshLow = 1700,
            .threshHigh = 1900,
        },
        {
            .ID = VTMON_VOLT_ID_3V3,
            .threshLow = 3200,
            .threshHigh = 3400,
        },
    },
};

TASK(VTMONIT_Task)
{
    int32_t retVal;
    VTMON_TempType currTemp;
    VTMON_TempConfigType tempConfig;
    VTMON_VoltConfigType voltConfig;
    VTMON_VoltType voltage;
    uint32_t i;

    ULOG_ERR("VTMON TEST: START\n");
    VTMON_Init(&VTMON_Config);
    BCM_WaitEvent(VTMON_TempThrshdEvent);
    BCM_ClearEvent(VTMON_TempThrshdEvent);
    retVal = VTMON_GetTemperature(&tempConfig, &currTemp);
    if (BCM_ERR_OK == retVal) {
        ULOG_ERR("GetTemperature: threshLow: %d threshHigh: %d curr: %d\n",
                tempConfig.threshLow, tempConfig.threshHigh, currTemp);
        BCM_CpuNDelay(VTMON_SAMPLING_TIME_ALL_MONITORS_NS);
        for (i = 0UL; i < VTMON_VOLT_ID_MAX; i++) {
            BCM_MemSet(&voltConfig, 0x0, sizeof(VTMON_VoltConfigType));
            voltConfig.ID = VTMON_Config.voltConfig[i].ID;
            retVal = VTMON_GetVoltage(&voltConfig, &voltage);
            ULOG_ERR("voltage id: %d curr voltage: %d\n", VTMON_Config.voltConfig[i].ID,
                    voltage);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("VTMON_GetVoltage() failed. err: %d\n", retVal);
                break;
            } else {
                if (0UL == voltage) {
                    ULOG_ERR("VTMON_GetVoltage() retured voltage as 0\n");
                    retVal = BCM_ERR_UNKNOWN;
                    break;
                }
            }
        }
    } else {
        ULOG_ERR("VTMON_GetTemperature() failed. err: %d\n", retVal);
    }
    VTMONIT1_Status = retVal;

    VTMON_Deinit();
    ULOG_ERR("VTMON TEST: END\n");
    BCM_TerminateTask();
}

void VTMON_TempNotification(VTMON_TempType aCurrent,
                             VTMON_TempType aLowThreshold,
                             VTMON_TempType aHighThreshold)
{
    if (BCM_AT_EXECUTING == VTMONIT1_Status) {
        ULOG_ERR("VTMON_TempNotification curr temp:%d\n", aCurrent);
        (void)SetEvent(VTMONIT_Task, VTMON_TempThrshdEvent);
    } else {
        ULOG_ERR("Extra Temp Notification\n");
        /* TODO: Debug extra interrupt */
    }
}

void VTMON_VoltNotification(VTMON_VoltIDType aVoltID,
                             VTMON_VoltType aCurrent,
                             VTMON_VoltType aLowThreshold,
                             VTMON_VoltType aHighThreshold)
{
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    if(BCM_AT_EXECUTING != VTMONIT1_Status) {
        switch(aIndex) {
        case VTMON_IT1:
            VTMONIT1_Status = BCM_AT_EXECUTING;
            (void)BCM_ActivateTask(VTMONIT_Task);
            break;
        default:
            retVal = BCM_AT_NOT_AVAILABLE;
            break;
        }
    } else {
        ULOG_ERR("VTMON TEST: EXECUTING\n");
    }

    return retVal;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;

    switch(aIndex) {
    case VTMON_IT1:
        retVal = VTMONIT1_Status;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }

    return retVal;
}

void ApplicationInit(void)
{
}
