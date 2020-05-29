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

#include "ee.h"
#include <ulog.h>
#include <avt.h>
#include <avt_osil.h>
#include <bcm_time.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <shell.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()    ULOG_LVL_INFO

#define AVT_IT1                (1UL)

static int32_t AVTIT1_Status = BCM_AT_NOT_STARTED;
static uint32_t avtTestIdx = 0UL;

void ApplicationInit(void)
{
    /* Dummy Init */
}

static uint64_t time1 = 0ULL;
static uint64_t time2 = 0ULL;

static void WaitGetClearEvent(TaskType aTaskID)
{
    BCM_EventMaskType mask;
    uint8_t i = 0U;
    do {
        (void)BCM_WaitEvent(AVTIT1_TimerEvent);
        (void)BCM_GetEvent(aTaskID, &mask);
        if (0UL != (mask & AVTIT1_TimerEvent)){
            (void)BCM_ClearEvent(AVTIT1_TimerEvent);
            break;
        }
        i++;
    } while(i > 0U);
}

void AVTIT1_AlarmCb(void)
{
    time2 = AVT_GetSystemTime();
    (void)SetEvent(AVTIT1_Task, AVTIT1_TimerEvent);
}

TASK(AVTIT1_Task)
{
    StatusType ret;
    int32_t retVal = BCM_ERR_OK;

    time1 = AVT_GetTime();
    if (0ULL == time1) {
        ULOG_ERR("\nGetTime Error: \t %ld", __LINE__);
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }
    ret = BCM_SetRelAlarm(AVTIT1_Alarm, 250, 0);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("\nAlarm Error:%ld \t %ld", ret, __LINE__);
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }
    WaitGetClearEvent(AVTIT1_Task);
    if (0ULL == time2) {
        ULOG_ERR("\nGetSystemTime Error: \t %ld", __LINE__);
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }
    if (time1 == time2) {
        ULOG_ERR("\nGetTime Error: \t %ld", __LINE__);
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }

err:
    if (BCM_AT_EXECUTING == AVTIT1_Status){
        AVTIT1_Status = retVal;
    }
    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;
    StatusType retErika = E_OK;
    TaskStateType taskState = SUSPENDED;
    TaskStateRefType taskStatePtr;

    taskStatePtr = &taskState;

    switch (aIndex) {
    case AVT_IT1:
        retErika = GetTaskState(AVTIT1_Task, taskStatePtr);
        if (E_OK == retErika) {
            if (SUSPENDED == taskState) {
                AVTIT1_Status = BCM_AT_EXECUTING;
                avtTestIdx = aIndex;
                retVal = BCM_ActivateTask(AVTIT1_Task);
                if (BCM_ERR_OK != retVal) {
                    retVal = BCM_AT_NOT_STARTED;
                    AVTIT1_Status = BCM_AT_NOT_STARTED;
                    ULOG_ERR("AVTIT1_Task activation failed, error: %d\n\r", retVal);
                }
            } else {
                retVal = BCM_AT_EXECUTING;
                ULOG_ERR("AVTIT1_Task is already running \n\r");
            }
        } else {
            retVal = BCM_ERR_UNKNOWN;
            ULOG_ERR("GetTaskState AVTIT1_Task failed Error: %d\n\r", retErika);
        }
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

    switch (aIndex) {
    case AVT_IT1:
        if (aIndex == avtTestIdx) {
            retVal = AVTIT1_Status;
        } else {
            retVal = BCM_AT_NOT_AVAILABLE;
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}
