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
#include <pwm.h>
#include <osil/pwm_osil.h>
#include <bcm_err.h>
#include <ulog.h>
#include <string.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>
#include "shell.h"

#define GetModuleLogLevel() ULOG_LVL_ERROR

#define PWM_HW_ID                   (0UL)
#define PWM_IT1                     (1UL)
#define PWM_PERIOD_63MS             (0xFFFFU)
#define PWM_PERIOD_30MS             (0x79E7U)
#define PWM_DUTY_CYCLE_50_PERCENT(period)   ((period) >> 1U)
#define PWM_DUTY_CYCLE_25_PERCENT(period)   ((period) >> 2U)
#define PWM_DUTY_CYCLE_12_PERCENT(period)   ((period) >> 3U)

#define PWM_OUTPUT_ID0    (0UL)
#define PWM_OUTPUT_ID1    (1UL)
#define PWM_OUTPUT_ID2    (2UL)
#define PWM_OUTPUT_ID3    (3UL)

#define PWM_ALARM_EVENT   (PWM_Test_Alarm_Event_1)

static int32_t PWMIT1_Result = BCM_AT_NOT_STARTED;

void ApplicationInit(void)
{
    /* Dummy Init */
}

static PWM_ChanCfgType ChanCfg1[4UL] = {
    {
        .chanID = PWM_OUTPUT_ID0,
        .mode = PWM_CHANMODE_FULL_DRIVE,
        .class = PWM_CHANCLASS_VARIABLE,
        .polarity = PWM_CHANPOLARITY_ACTIVE_LOW,
        .prescaler = 0U,
        .period = PWM_PERIOD_63MS,
        .dutyCycle = PWM_DUTY_CYCLE_25_PERCENT(PWM_PERIOD_63MS),
    },
    {
        .chanID = PWM_OUTPUT_ID1,
        .mode = PWM_CHANMODE_FULL_DRIVE,
        .class = PWM_CHANCLASS_VARIABLE,
        .polarity = PWM_CHANPOLARITY_ACTIVE_HIGH,
        .prescaler = 0U,
        .period = PWM_PERIOD_63MS,
        .dutyCycle = PWM_DUTY_CYCLE_25_PERCENT(PWM_PERIOD_63MS),
    },
    {
        .chanID = PWM_OUTPUT_ID2,
        .mode = PWM_CHANMODE_FULL_DRIVE,
        .class = PWM_CHANCLASS_VARIABLE,
        .polarity = PWM_CHANPOLARITY_ACTIVE_LOW,
        .prescaler = 0U,
        .period = PWM_PERIOD_63MS,
        .dutyCycle = PWM_DUTY_CYCLE_12_PERCENT(PWM_PERIOD_63MS),
    },
    {
        .chanID = PWM_OUTPUT_ID3,
        .mode = PWM_CHANMODE_FULL_DRIVE,
        .class = PWM_CHANCLASS_VARIABLE,
        .polarity = PWM_CHANPOLARITY_ACTIVE_HIGH,
        .prescaler = 0U,
        .period = PWM_PERIOD_63MS,
        .dutyCycle = PWM_DUTY_CYCLE_12_PERCENT(PWM_PERIOD_63MS),
    },
};

static PWM_ConfigType Config1 = {
    .chanCfgTbl = ChanCfg1,
    .chanCfgTblSz = (sizeof(ChanCfg1)/sizeof(PWM_ChanCfgType)),
};

static int32_t PWMOutputTest(const PWM_ConfigType * const aConfig)
{
    uint32_t chanId;
    int32_t retVal = BCM_ERR_OK;
    BCM_EventMaskType mask;
    ULOG_ERR("PWMOutputTest\n");

    for (chanId = 0UL; chanId <= aConfig->chanCfgTbl[3].chanID; chanId++) {
        ULOG_ERR("\n Starting PWM Channel %d", chanId);
        retVal = PWM_StartChannel(PWM_HW_ID, chanId);
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError starting PWM Channel %d", chanId);
            goto err;
        }

        (void)BCM_SetRelAlarm(PWM_Test_Alarm_1, 250, 0);

        (void)BCM_WaitEvent(PWM_ALARM_EVENT);
        (void)BCM_GetEvent(PWMIT1_Task, &mask);
        if (0UL != (mask & PWM_ALARM_EVENT)) {
            (void)BCM_ClearEvent(PWM_ALARM_EVENT);
        }

        /* Change Duty Cycle for PWM Output */
        retVal = PWM_StopChannel(PWM_HW_ID, chanId);
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError stoping PWM Channel %d", chanId);
            goto err;
        }

        retVal = PWM_SetDutyCycle(PWM_HW_ID, chanId, PWM_DUTY_CYCLE_50_PERCENT(Config1.chanCfgTbl[chanId].period));
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError Setting Duty Cycle for Channel %d", chanId);
            goto err;
        }

        retVal = PWM_StartChannel(PWM_HW_ID, chanId);
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError starting PWM Channel %d", chanId);
            goto err;
        }

        (void)BCM_SetRelAlarm(PWM_Test_Alarm_1, 250, 0);

        (void)BCM_WaitEvent(PWM_ALARM_EVENT);
        (void)BCM_GetEvent(PWMIT1_Task, &mask);
        if (0UL != (mask & PWM_ALARM_EVENT)) {
            (void)BCM_ClearEvent(PWM_ALARM_EVENT);
        }

        /* Stop PWM Output */
        retVal = PWM_StopChannel(PWM_HW_ID, chanId);
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError stoping PWM Channel %d", chanId);
            goto err;
        }

        /* Change Period and Duty Cycle for Output */
        retVal = PWM_SetPeriodDutyCycle(PWM_HW_ID, chanId, PWM_PERIOD_30MS,
                                        PWM_DUTY_CYCLE_25_PERCENT(PWM_PERIOD_30MS));
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError Setting Period and Duty Cycle for Channel %d", chanId);
            goto err;
        }

        retVal = PWM_StartChannel(PWM_HW_ID, chanId);
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError starting PWM Channel %d", chanId);
            goto err;
        }

        (void)BCM_SetRelAlarm(PWM_Test_Alarm_1, 250, 0);

        (void)BCM_WaitEvent(PWM_ALARM_EVENT);
        (void)BCM_GetEvent(PWMIT1_Task, &mask);
        if (0UL != (mask & PWM_ALARM_EVENT)) {
            (void)BCM_ClearEvent(PWM_ALARM_EVENT);
        }

        /* Stop PWM Output */
        retVal = PWM_StopChannel(PWM_HW_ID, chanId);
        if (retVal != BCM_ERR_OK) {
            ULOG_ERR("\nError stoping PWM Channel %d", chanId);
            goto err;
        }
    }

err:
    return retVal;
}

void PWM_TestAlarm1_Cb(void)
{
    (void)SetEvent(PWMIT1_Task, PWM_ALARM_EVENT);
}

TASK(PWMIT1_Task)
{
    int32_t retVal = BCM_ERR_OK;

    PWM_Init(PWM_HW_ID, &Config1);

    retVal = PWMOutputTest(&Config1);
    if (retVal != BCM_ERR_OK){
        ULOG_ERR("\nError: PWM Output Test fail");
        goto err;
    }

    PWMIT1_Result = BCM_ERR_OK;

err:
    if (BCM_AT_EXECUTING == PWMIT1_Result) {
        PWMIT1_Result= BCM_ERR_UNKNOWN;
    }

    PWM_DeInit(PWM_HW_ID);

    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex)
    {
    case PWM_IT1:
        PWMIT1_Result = BCM_AT_EXECUTING;
        (void)BCM_ActivateTask(PWMIT1_Task);
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;

    switch(aIndex)
    {
    case PWM_IT1:
        retVal = PWMIT1_Result;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}
