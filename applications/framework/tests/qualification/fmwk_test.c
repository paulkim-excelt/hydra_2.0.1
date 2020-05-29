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
 File Name: media_system.c
 Descritpion: This file implememnts application interface for media framework.
******************************************************************************/

/* Includes */
#include <stddef.h>
#include <compiler.h>
#include <string.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include "media_system.h"
#include "component.h"
#include "ee.h"
#include "ulog.h"
#include "bcm_err.h"
#include "console.h"
#include "shell.h"
#include <osil/bcm_osil.h>

uint8_t txData[1000],rxData[1000];
uint32_t txPktCount, rxPktCount;
extern uint32_t GetAPPLogLevel(void);
#define GetModuleLogLevel   GetAPPLogLevel
#define CMD_STR_LEN         (5UL)
#define MEDIA_SYS_COMP(COMP_ID, NUM_CONFIGS, CONFIG_ID, SCR_COMP_IDX, SRC_PORT_IDX)             \
{                                                                                               \
        COMP_ID,                                                                                \
        NUM_CONFIGS,                                                                            \
        {CONFIG_ID, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL}, \
        {                                                                                       \
            {SCR_COMP_IDX, SRC_PORT_IDX}, {0U, 0U}, {0U, 0U}, {0U, 0U}                          \
        }                                                                                       \
}

uint32_t GetAPPLogLevel(void)
{
    return ULOG_LVL_INFO;
}

static const MSYS_Type mediaSys = {
    MSYS_MAGIC,
    3UL,
    {
        MEDIA_SYS_COMP(MSYS_COMP_ID_EX_SRC, 1UL, 0UL, 0U, 0U),  /* component [0] */
        MEDIA_SYS_COMP(MSYS_COMP_ID_EX_INT, 1UL, 0UL, 0U, 0U),  /* component [1] */
        MEDIA_SYS_COMP(MSYS_COMP_ID_EX_SNK, 1UL, 0UL, 1U, 0U),  /* component [2] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [3] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [4] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [5] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [6] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [7] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [8] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [9] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [10] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [11] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [12] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [13] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U),  /* component [14] */
        MEDIA_SYS_COMP(               0ULL, 0UL, 0UL, 0U, 0U)   /* component [15] */
    }
};

/* Macros */

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

/** Test ID */
#define FMWK_IT0     (0UL)           /**< Test ID 1  */
#define FMWK_ITMAX   (FMWK_IT0)   /**< Max test cases*/

/** System Event Mapping */
#define FMWK_IT_STOP_EVENT   (SystemEvent0)      /**< stop Event  */

static volatile int32_t FmwkITRes[FMWK_ITMAX + 1UL] =
{
BCM_AT_NOT_AVAILABLE,
};

static void FmwkTest_Start(void)
{
    int32_t ret;

    txPktCount = 0;
    rxPktCount = 0;
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
    }
    ret = BCM_SetRelAlarm(FmwkIT_Alarm, 1000, 0);
    if (BCM_ERR_OK != ret) {
         ULOG_ERR("SetRelAlarm Failed %d\n",ret);
        goto err;
    }

err:
    return;
}

static void FmwkTest_Stop(void)
{
    int32_t ret;

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
    return;
}

void FmwkIT_Alarm_CB(void)
{
	(void)SetEvent(FmwkIT_Task,FMWK_IT_STOP_EVENT);
}

static uint32_t GTestIDidx = 0UL;
TASK(FmwkIT_Task)
{
	FmwkTest_Start();
	(void)BCM_WaitEvent(FMWK_IT_STOP_EVENT);
	FmwkTest_Stop();
	FmwkITRes[GTestIDidx] = BCM_ERR_OK;
	(void)BCM_TerminateTask();
}
int32_t BCM_ExecuteAT(uint32_t aIndex)
{

	int32_t ret = BCM_ERR_OK;
	uint32_t TestID = aIndex - 1UL;

	if (BCM_AT_EXECUTING == FmwkITRes[TestID]) {
		return BCM_AT_EXECUTING;
    }

	switch (TestID) {
	case FMWK_IT0:
        FmwkITRes[TestID] = BCM_AT_EXECUTING;
		GTestIDidx = TestID;
		(void)BCM_ActivateTask(FmwkIT_Task);
        break;
    default:
        ret = BCM_AT_NOT_AVAILABLE;
        break;
    }

    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret;
    uint32_t TestID = aIndex - 1UL;
    MSYS_PortStatsType sinkInStats;

	if (TestID > FMWK_ITMAX) {
        return BCM_AT_NOT_AVAILABLE;
    }

	if (BCM_AT_EXECUTING == FmwkITRes[TestID]) {
		return BCM_AT_EXECUTING;
    }

	/* Verify the Src and Sink data */
    if(0L != memcmp(txData, rxData, txPktCount)) {
        ULOG_INFO("Data Verification failed\n");
		return BCM_ERR_CUSTOM;
    }
    /* Print Stats for Sink's input Port */
    ret = MSYS_GetInStats(mediaSys.component[2].compID,
                                0UL,
                                &sinkInStats);
    if (BCM_ERR_OK == ret) {
        ULOG_INFO("\n*********Media System Port Stats*************\n");
        ULOG_INFO("EX_SINK:  nRx: %u\n", sinkInStats.nRx);
        ULOG_INFO("EX_SINK:  nRxFailed: %u\n", sinkInStats.nRxFailed);
        ULOG_INFO("EX_SINK:  nRxCancelled: %u\n", sinkInStats.nRxCancelled);
        ULOG_INFO("EX_SINK:  nTx: %u\n", sinkInStats.nTx);
        ULOG_INFO("EX_SINK:  nTxFailed: %u\n", sinkInStats.nTxFailed);
        ULOG_INFO("EX_SINK:  nTxCritFailed: %u\n", sinkInStats.nTxCritFailed);
        ULOG_INFO("EX_SINK:  avgLatency: %u ns\n", sinkInStats.avgLatency);
        ULOG_INFO("EX_SINK:  frameCount: %u Bytes\n", sinkInStats.frameCount);
        ULOG_INFO("*******************************************\n");
    } else {
        ULOG_ERR("MSYS_GetInStats Failed %d\n", ret);
    }


    return ret;
}

void ApplicationInit(void)
{
}
