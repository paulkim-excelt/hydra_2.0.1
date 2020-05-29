/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
 File Name: avbcomp_test.c
 Description: This file implements the comp test application for AVB component.
******************************************************************************/

/* Includes */
#include <stddef.h>
#include <compiler.h>
#include <string.h>

#include <media_system.h>
#include <component.h>
#include "ee.h"
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <atomic.h>
#include <eth_osil.h>
#include <osil/bcm_osil.h>

#include "avb_talker.h"
#include "avb_listener.h"

extern uint32_t GetAPPLogLevel();
#define GetModuleLogLevel   GetAPPLogLevel

#define CMD_STR_LEN         (5UL)
extern volatile int32_t avbcomp_qt1_result;
volatile int32_t avbcomp_qt1_result = BCM_AT_NOT_STARTED;

/* Variables to check the total data received by test sink */
extern uint32_t ExSnk_Port0DataSize;
extern uint32_t ExSnk_Port1DataSize;
extern uint32_t ExSnk_Port2DataSize;
extern uint32_t ExSnk_Port3DataSize;

uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}

const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 4UL,

    .component[0].compID = MSYS_COMP_ID_EX_SRC,
    .component[0].numConfigs = 1UL,
    .component[0].config[0] = 0UL,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,
    .component[0].portMap[1].srcCompIdx = 0U,
    .component[0].portMap[1].srcPortIdx = 0U,
    .component[0].portMap[2].srcCompIdx = 0U,
    .component[0].portMap[2].srcPortIdx = 0U,
    .component[0].portMap[3].srcCompIdx = 0U,
    .component[0].portMap[3].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_AVBT,
    .component[1].numConfigs = 1UL,
    .component[1].config[0] = MSYS_CONFIG_AVB_TLKR,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 1U,
    .component[1].portMap[2].srcCompIdx = 0U,
    .component[1].portMap[2].srcPortIdx = 2U,
    .component[1].portMap[3].srcCompIdx = 0U,
    .component[1].portMap[3].srcPortIdx = 3U,

    .component[2].compID = MSYS_COMP_ID_AVBL,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = MSYS_CONFIG_AVB_LSTR,
    .component[2].portMap[0].srcCompIdx = 0U,
    .component[2].portMap[0].srcPortIdx = 0U,
    .component[2].portMap[1].srcCompIdx = 0U,
    .component[2].portMap[1].srcPortIdx = 0U,
    .component[2].portMap[2].srcCompIdx = 0U,
    .component[2].portMap[2].srcPortIdx = 0U,
    .component[2].portMap[3].srcCompIdx = 0U,
    .component[2].portMap[3].srcPortIdx = 0U,

    .component[3].compID = MSYS_COMP_ID_EX_SNK,
    .component[3].numConfigs = 1UL,
    .component[3].config[0] = 0UL,
    .component[3].portMap[0].srcCompIdx = 2U,
    .component[3].portMap[0].srcPortIdx = 0U,
    .component[3].portMap[1].srcCompIdx = 2U,
    .component[3].portMap[1].srcPortIdx = 1U,
    .component[3].portMap[2].srcCompIdx = 2U,
    .component[3].portMap[2].srcPortIdx = 2U,
    .component[3].portMap[3].srcCompIdx = 2U,
    .component[3].portMap[3].srcPortIdx = 3U,
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

void AVBTest_Start(void)
{
    int32_t ret;

    ret = MSYS_Init(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avbcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        avbcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        avbcomp_qt1_result = ret;
    }

err:
    return;
}

void AVBTest_StopStart(void)
{
    int32_t ret;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avbcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        avbcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        avbcomp_qt1_result = ret;
    }

err:
    return;
}

void AVBTest_Stop(void)
{
    int32_t ret;

    /* Check that all ports have received data */
    if ((0UL == ExSnk_Port0DataSize) ||
        (0UL == ExSnk_Port1DataSize) ||
        (0UL == ExSnk_Port2DataSize) ||
        (0UL == ExSnk_Port3DataSize)){
        ULOG_ERR("Error >> Data size P0=%d P1=%d P2=%d P3=%d\n",
                            ExSnk_Port0DataSize,
                            ExSnk_Port1DataSize,
                            ExSnk_Port2DataSize,
                            ExSnk_Port3DataSize);
        avbcomp_qt1_result = BCM_ERR_UNKNOWN;
    }

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avbcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avbcomp_qt1_result = ret;
    }

    if (BCM_AT_EXECUTING == avbcomp_qt1_result) {
        avbcomp_qt1_result = BCM_ERR_OK;
    }
err:
    return;
}
volatile uint32_t AVB_TestAlarmVal = 0UL;
uint32_t gAVBTestCount = 0UL;
void Test_AVBCompAlarm_cb()
{
    if(1UL == AVB_TestAlarmVal)
    {
        if(++gAVBTestCount == 2)
        {
            ATOMIC_Set(&AVB_TestAlarmVal, 0UL);
            SetEvent(Test_AVBComp,SystemEvent0);
        }
    }
}
TASK(Test_AVBComp)
{
    AVBTest_Start();
    gAVBTestCount = 0UL;
    ATOMIC_Set(&AVB_TestAlarmVal, 1UL);
    BCM_WaitEvent(SystemEvent0);
    (void)BCM_ClearEvent(SystemEvent0);
    ULOG_INFO("Stop-Start Test \n");
    gAVBTestCount = 0UL;
    ATOMIC_Set(&AVB_TestAlarmVal, 1UL);
    AVBTest_StopStart();
    (void)BCM_WaitEvent(SystemEvent0);
    (void)BCM_ClearEvent(SystemEvent0);
    AVBTest_Stop();
    ULOG_INFO("Test ended\n");
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        avbcomp_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test_AVBComp);
        ULOG_INFO("Test started\n");
        ret = BCM_ERR_OK;
    }
    return ret;
}
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = avbcomp_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    ETHER_EnableLoopBack(ETHER_HW_ID_0);
    BCM_ActivateTask(ExSrc_CmdProcessTask);
    BCM_ActivateTask(ExSnk_CmdProcessTask);
}
