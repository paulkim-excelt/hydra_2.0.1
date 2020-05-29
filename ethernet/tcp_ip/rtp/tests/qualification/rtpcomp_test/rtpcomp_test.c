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

/******************************************************************************
 File Name: rtpcomp_test.c
 Description: This file implements the IT application for RTP Sender and Receiver.
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
#include <inet_cfg.h>
#include <osil/bcm_osil.h>

#include "inet_system.h"

#include "rtp_sender.h"
#include "rtp_receiver.h"

extern uint32_t GetAPPLogLevel();
#define GetModuleLogLevel   GetAPPLogLevel

#define CMD_STR_LEN         (5UL)
extern volatile int32_t rtpcomp_qt1_result;
volatile int32_t rtpcomp_qt1_result = BCM_AT_NOT_STARTED;

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

    .component[1].compID = MSYS_COMP_ID_RTPS,
    .component[1].numConfigs = 1UL,
    .component[1].config[0] = MSYS_CONFIG_RTP_SENDER,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
#if (1UL < RTP_MAX_NUM_INPUT_PORTS)
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 1U,
#endif
#if (2UL < RTP_MAX_NUM_INPUT_PORTS)
    .component[1].portMap[2].srcCompIdx = 0U,
    .component[1].portMap[2].srcPortIdx = 2U,
#endif
#if (3UL < RTP_MAX_NUM_INPUT_PORTS)
    .component[1].portMap[3].srcCompIdx = 0U,
    .component[1].portMap[3].srcPortIdx = 3U,
#endif

    .component[2].compID = MSYS_COMP_ID_RTPR,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = MSYS_CONFIG_RTP_RCVR,
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
#if (1UL < RTP_MAX_NUM_OUTPUT_PORTS)
    .component[3].portMap[1].srcCompIdx = 2U,
    .component[3].portMap[1].srcPortIdx = 1U,
#endif
#if (2UL < RTP_MAX_NUM_OUTPUT_PORTS)
    .component[3].portMap[2].srcCompIdx = 2U,
    .component[3].portMap[2].srcPortIdx = 2U,
#endif
#if (3UL < RTP_MAX_NUM_OUTPUT_PORTS)
    .component[3].portMap[3].srcCompIdx = 2U,
    .component[3].portMap[3].srcPortIdx = 3U,
#endif
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

void RTPTest_Start(void)
{
    int32_t ret;

    ret = MSYS_Init(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
    }

err:
    return;
}

void RTPTest_Stop(void)
{
    int32_t ret;

    /* Check that all ports have received data */
    if ((0UL == ExSnk_Port0DataSize)
#if (1UL < RTP_MAX_NUM_OUTPUT_PORTS)
        || (0UL == ExSnk_Port1DataSize)
#endif
#if (2UL < RTP_MAX_NUM_OUTPUT_PORTS)
        || (0UL == ExSnk_Port2DataSize)
#endif
#if (3UL < RTP_MAX_NUM_OUTPUT_PORTS)
        || (0UL == ExSnk_Port3DataSize)
#endif
    ){
        ULOG_ERR("Error >> Data size P0=%d P1=%d P2=%d P3=%d\n",
                            ExSnk_Port0DataSize,
                            ExSnk_Port1DataSize,
                            ExSnk_Port2DataSize,
                            ExSnk_Port3DataSize);
        rtpcomp_qt1_result = BCM_ERR_UNKNOWN;
    }

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Stop Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_DeInit Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
    }

    if (BCM_AT_EXECUTING == rtpcomp_qt1_result) {
        rtpcomp_qt1_result = BCM_ERR_OK;
    }
err:
    return;
}

void RTPTest_StopStart(void)
{
    int32_t ret;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Stop Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        rtpcomp_qt1_result = ret;
    }

err:
    return;
}

volatile uint32_t RTP_TestAlarmVal = 0UL;
uint32_t gRTPSTestCount = 0UL;
void Test_RTPCompAlarm_cb()
{
    if(1UL == RTP_TestAlarmVal)
    {
        if(++gRTPSTestCount == 2)
        {
            ATOMIC_Set(&RTP_TestAlarmVal, 0UL);
            SetEvent(Test_RTPComp,SystemEvent0);
        }
    }
}

TASK(Test_RTPComp)
{
    INET_SystemInterface *sysIntfObj = NULL;
    INET_ARPEntry* cache;
    int32_t ret;

    /* Get the interface structure for ARP cache access */
    ret = SysCfg_GetInterfaceByIndex(0UL, &sysIntfObj);
    if (BCM_ERR_OK != ret) {
        rtpcomp_qt1_result = ret;
    }
    cache = sysIntfObj->arpCache;

    /* Adding a dummy entry into ARP cache for loopback tests */
    cache->valid = 1UL;
    cache->ipAddr = (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)));
    cache->flags = INET_ARP_ENTRY_FLAG_STATIC;

    RTPTest_Start();
    gRTPSTestCount = 0UL;
    ATOMIC_Set(&RTP_TestAlarmVal, 1UL);
    BCM_WaitEvent(SystemEvent0);
    (void)BCM_ClearEvent(SystemEvent0);
    ULOG_ERR("Stop-Start\n");
    RTPTest_StopStart();
    gRTPSTestCount = 0UL;
    ATOMIC_Set(&RTP_TestAlarmVal, 1UL);
    BCM_WaitEvent(SystemEvent0);
    (void)BCM_ClearEvent(SystemEvent0);
    RTPTest_Stop();
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        rtpcomp_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test_RTPComp);
        ULOG_INFO("\n Test started");
        ret = BCM_ERR_OK;
    }
    return ret;
}
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = rtpcomp_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    ETHER_EnableLoopBack(ETHER_HW_ID_0);
    BCM_ActivateTask(ExSrc_CmdProcessTask);
    BCM_ActivateTask(ExSnk_CmdProcessTask);
}
