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

/**
    @addtogroup grp_vidxmtr
    @{
    @file vidxmtr.c
    @brief This file implements the video transmitter application with gPTP
            enabled

    @version 0.1 Initial version
*/

/* Includes */
#include <stddef.h>
#include <compiler.h>
#include <string.h>
#include <media_system.h>
#include <component.h>
#include <ee.h>
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <shell.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()   ULOG_LVL_INFO
#define CMD_STR_LEN         (5UL)

const MSYS_Type* GMSYS_Type;

#if ENABLE_1080P_CONFIG
static const MSYS_Type GMediaSys1080 = {
    .magicID = MSYS_MAGIC,
    .count = 3UL,

    .component[0].compID = MSYS_COMP_ID_CAM0,
    .component[0].numConfigs = 6UL,
    .component[0].config[0] = MSYS_CONFIG_CAM_MOD+1,
    .component[0].config[1] = MSYS_CONFIG_CAM_ISP+1,
    .component[0].config[2] = MSYS_CONFIG_CAM_UCM+1,
    .component[0].config[3] = MSYS_CONFIG_CAM_CSN+2,
    .component[0].config[4] = MSYS_CONFIG_CAM_CSN_MODE(3),
    .component[0].config[5] = MSYS_CONFIG_CAM_CTN+1,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_AVCE,
    .component[1].numConfigs = 2UL,
    .component[1].config[0] = MSYS_CONFIG_AVCE_MOD+1,
    .component[1].config[1] = MSYS_CONFIG_AVCE_FW0,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,

    .component[2].compID = MSYS_COMP_ID_AVBT,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = MSYS_CONFIG_AVB_TLKR,
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,
};
#endif
#if ENABLE_968P_CONFIG
static const MSYS_Type GMediaSys968 = {
    .magicID = MSYS_MAGIC,
    .count = 3UL,

    .component[0].compID = MSYS_COMP_ID_CAM0,
    .component[0].numConfigs = 6UL,
    .component[0].config[0] = MSYS_CONFIG_CAM_MOD+1,
    .component[0].config[1] = MSYS_CONFIG_CAM_ISP+1,
    .component[0].config[2] = MSYS_CONFIG_CAM_UCM+1,
    .component[0].config[3] = MSYS_CONFIG_CAM_CSN+2,
    .component[0].config[4] = MSYS_CONFIG_CAM_CSN_MODE(3),
    .component[0].config[5] = MSYS_CONFIG_CAM_CTN+1,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_AVCE,
    .component[1].numConfigs = 2UL,
    .component[1].config[0] = MSYS_CONFIG_AVCE_MOD+1,
    .component[1].config[1] = MSYS_CONFIG_AVCE_FW0,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,

    .component[2].compID = MSYS_COMP_ID_AVBT,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = MSYS_CONFIG_AVB_TLKR,
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,
};
#endif
#if ENABLE_1280P_CONFIG
static const MSYS_Type GMediaSys1280 = {
    .magicID = MSYS_MAGIC,
    .count = 3UL,

    .component[0].compID = MSYS_COMP_ID_CAM0,
    .component[0].numConfigs = 6UL,
    .component[0].config[0] = MSYS_CONFIG_CAM_MOD+2,
    .component[0].config[1] = MSYS_CONFIG_CAM_ISP+2,
    .component[0].config[2] = MSYS_CONFIG_CAM_UCM+2,
    .component[0].config[3] = MSYS_CONFIG_CAM_CSN+4,
    .component[0].config[4] = MSYS_CONFIG_CAM_CSN_MODE(5),
    .component[0].config[5] = MSYS_CONFIG_CAM_CTN+2,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_AVCE,
    .component[1].numConfigs = 2UL,
    .component[1].config[0] = MSYS_CONFIG_AVCE_MOD+2,
    .component[1].config[1] = MSYS_CONFIG_AVCE_FW0,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,

    .component[2].compID = MSYS_COMP_ID_AVBT,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = MSYS_CONFIG_AVB_TLKR,
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,
};
#endif
static const MSYS_Type GMediaSys720 = {
    .magicID = MSYS_MAGIC,
    .count = 4UL,

    .component[0].compID = MSYS_COMP_ID_CAM0,
    .component[0].numConfigs = 6UL,
    .component[0].config[0] = MSYS_CONFIG_CAM_MOD,
    .component[0].config[1] = MSYS_CONFIG_CAM_ISP,
    .component[0].config[2] = MSYS_CONFIG_CAM_UCM,
    .component[0].config[3] = MSYS_CONFIG_CAM_CSN,
    .component[0].config[4] = MSYS_CONFIG_CAM_CSN_MODE(1),
    .component[0].config[5] = MSYS_CONFIG_CAM_CTN,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_LDC,
    .component[1].numConfigs = 2UL,
    .component[1].config[0] = MSYS_CONFIG_LDC_MOD,
    .component[1].config[1] = MSYS_CONFIG_LDC_FW,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,

    .component[2].compID = MSYS_COMP_ID_AVCE,
    .component[2].numConfigs = 2UL,
    .component[2].config[0] = MSYS_CONFIG_AVCE_MOD,
    .component[2].config[1] = MSYS_CONFIG_AVCE_FW0,
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,

    .component[3].compID = MSYS_COMP_ID_AVBT,
    .component[3].numConfigs = 1UL,
    .component[3].config[0] = MSYS_CONFIG_AVB_TLKR,
    .component[3].portMap[0].srcCompIdx = 2U,
    .component[3].portMap[0].srcPortIdx = 0U,
};

/* Macros */
/**
    @brief Event for "app stop" command
*/
#define EVENT_APP_STOP (SystemEvent0)

/**
    @brief Event for printing statistics
*/
#define EVENT_PRINT_STATS (SystemEvent2)

/*
 * This API is invoked by the framework to retrieve the media system connection
 * information.
 */
const MSYS_Type* MSYS_Get(uint32_t aIdx)
{
    const MSYS_Type* mSys = GMSYS_Type;
    (void)aIdx;
    return mSys;
}


void MSYS_WdsErrorCb(const char* aName,
	             int32_t aError,
                     const WDS_MonitorType *aMonitor,
		     uint32_t aMaxAllowedErrors)
{
}

static void Vidxmtr_CompStats(uint64_t aCompID, uint32_t aPortIdx)
{
    MSYS_PortStatsType sinkInStats;
    int32_t ret;
    uint32_t i;
    char compIdStr[9];

    ret = MSYS_GetInStats(aCompID, aPortIdx, &sinkInStats);
    if (BCM_ERR_OK == ret) {
            /* Make component ID string */
            for (i = 0UL; i < 8UL; i++) {
                compIdStr[i] = (char) ((aCompID >> (i * 8UL)) & 0xffU);
            }
            compIdStr[8] = '\0';

            ULOG_INFO("\nMedia Port Statistics for comp:[%s] Port:[%u] \n", compIdStr, aPortIdx);
            ULOG_INFO("--------------------------------------------------\n");
            ULOG_INFO("nRx       : %u\n", sinkInStats.nRx);
            ULOG_INFO("nRxFailed : %u\n", sinkInStats.nRxFailed);
            ULOG_INFO("nTx       : %u\n", sinkInStats.nTx);
            ULOG_INFO("nTxFailed : %u\n", sinkInStats.nTxFailed);
            ULOG_INFO("avgLatency: %u ns\n", sinkInStats.avgLatency);
            ULOG_INFO("frameCount: %u\n", sinkInStats.frameCount);
    } else {
        ULOG_ERR("MSYS_GetInStats Failed %d\n", ret);
    }
}

TASK(VidXmtr)
{
    int32_t ret;
    BCM_TimeType t1 = {0UL, 0UL};
    uint32_t stopCmd = 0UL;
    BCM_EventMaskType mask = 0UL;

    ret = MSYS_Init(GMSYS_Type);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_Configure(GMSYS_Type);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_Start(GMSYS_Type);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        goto err;
    }

    BCM_GetTime(&t1);
    ULOG_ERR("Video Transmission started at %u sec %u msec from start\n", t1.s, t1.ns/1000000UL);

    do {
        ret = BCM_WaitEvent(EVENT_APP_STOP | EVENT_PRINT_STATS);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("BCM_WaitEvent Failed %d\n",ret);
            goto err;
        }

        ret = BCM_GetEvent(VidXmtr, &mask);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("BCM_GetEvent Failed %d\n",ret);
            goto err;
        }

        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("BCM_ClearEvent Failed %d\n",ret);
            goto err;
        }

        if (0UL != (mask & EVENT_PRINT_STATS)) {
            uint32_t i;

            for (i = 1UL; i<GMSYS_Type->count; i++) {
                Vidxmtr_CompStats(GMSYS_Type->component[i].compID, 0UL);
            }
        }

        if (0UL != (mask & EVENT_APP_STOP)) {
            stopCmd = 1UL;
        }

    } while (0UL == stopCmd);

    ret = MSYS_Stop(GMSYS_Type);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Stop Failed %d\n",ret);
        goto err;
    }

err:
    ret = MSYS_DeInit(GMSYS_Type);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_DeInit Failed %d\n",ret);
    }

    ret = BCM_TerminateTask();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("BCM_TerminateTask Failed %d\n",ret);
    }
}

void ApplicationInit()
{
    int32_t ret;
    GMSYS_Type = &GMediaSys720;

    ret = BCM_ActivateTask(VidXmtr);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("BCM_ActivateTask Failed %d\n",ret);
    }
}

void Shell_App(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    int32_t ret;
    if (NULL != aArgString) {
        if (0 == strncmp(aArgString, "start", CMD_STR_LEN)) {
            ret = BCM_ActivateTask(VidXmtr);
            if (BCM_ERR_OK != ret) {
                CONSOLE_Write(aConsoleID, "Failed to send start signal\n");
            } else {
                if (0) {
#if ENABLE_1080P_CONFIG
                } else if (0 == strncmp(aArgString, "start 1080", CMD_STR_LEN+5)) {
                    GMSYS_Type = &GMediaSys1080;
#endif
#if ENABLE_968P_CONFIG
                } else if (0 == strncmp(aArgString, "start 968", CMD_STR_LEN+5)) {
                    GMSYS_Type = &GMediaSys968;
#endif
#if ENABLE_1280P_CONFIG
                } else if (0 == strncmp(aArgString, "start 1280", CMD_STR_LEN+5)) {
                    GMSYS_Type = &GMediaSys1280;
#endif
                } else {
                    GMSYS_Type = &GMediaSys720;
                }
            }
        } else if (0 == strncmp(aArgString, "stop", CMD_STR_LEN)) {
            ret = BCM_SetEvent(VidXmtr, EVENT_APP_STOP);
            if (BCM_ERR_OK != ret) {
                CONSOLE_Write(aConsoleID, "Failed to send stop signal\n");
            }
        } else if (0 == strncmp(aArgString, "stats", CMD_STR_LEN)) {
            ret = BCM_SetEvent(VidXmtr, EVENT_PRINT_STATS);
            if (BCM_ERR_OK != ret) {
                CONSOLE_Write(aConsoleID, "Failed to send stats signal\n");
            }
        } else {
            CONSOLE_Write(aConsoleID, "Invalid argument. Usage: app start/stop/stats\n");
        }
    } else {
        CONSOLE_Write(aConsoleID, "No argument mentioned. Usage: app start/stop/stats\n");
    }
}

/** @}*/

