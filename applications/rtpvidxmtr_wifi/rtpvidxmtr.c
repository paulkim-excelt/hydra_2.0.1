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

/**

    @addtogroup grp_rtpvidxmtr_wifi
    @{
    @file rtpvidxmtr.c
    @brief This file implements the Wifi RTP video transmitter application

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
#include <eth_osil.h>
#include <inet_cfg.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()   ULOG_LVL_INFO
#define CMD_STR_LEN         (5UL)

static const MSYS_Type mediaSys = {
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

    .component[3].compID = MSYS_COMP_ID_RTPS,
    .component[3].numConfigs = 1UL,
    .component[3].config[0] = MSYS_CONFIG_RTP_SENDER,
    .component[3].portMap[0].srcCompIdx = 2U,
    .component[3].portMap[0].srcPortIdx = 0U,
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


TASK(RtpVidXmtr)
{
    int32_t ret;
    BCM_TimeType t1 = {0UL, 0UL};

#ifdef ENABLE_STATIC_ARP
    INET_SystemInterface *sysIntfObj = NULL;
    INET_ARPEntry* cache;

    /* Get the interface structure for ARP cache access */
    ret = SysCfg_GetInterfaceByIndex(0UL, &sysIntfObj);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("SysCfg_GetInterfaceByIndex() returned err: %d\n", ret);
        goto err;
    }
    cache = sysIntfObj->arpCache;

    /* Adding a dummy entry into ARP cache for loopback tests */
    cache->valid = 1UL;
    cache->ipAddr = (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)));
    cache->flags = INET_ARP_ENTRY_FLAG_STATIC;
#endif

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

    BCM_GetTime(&t1);
    ULOG_ERR("Video Transmission started at %u sec %u msec from start\n", t1.s, t1.ns/1000000UL);

    ret = BCM_WaitEvent(SystemEvent0);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("BCM_WaitEvent Failed %d\n",ret);
        goto err;
    }
    ret = BCM_ClearEvent(SystemEvent0);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("BCM_ClearEvent Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Stop Failed %d\n",ret);
        goto err;
    }

err:
    ret = MSYS_DeInit(&mediaSys);
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
    ret = BCM_ActivateTask(RtpVidXmtr);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("BCM_ActivateTask Failed %d\n",ret);
    }
}

void Shell_App(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    int32_t ret;
    if (NULL != aArgString) {
        if (0 == strncmp(aArgString, "start", CMD_STR_LEN)) {
            ret = BCM_ActivateTask(RtpVidXmtr);
            if (BCM_ERR_OK != ret) {
                CONSOLE_Write(aConsoleID, "Failed to send start signal\n");
            }
        } else if (0 == strncmp(aArgString, "stop", CMD_STR_LEN)) {
            ret = BCM_SetEvent(RtpVidXmtr, SystemEvent0);
            if (BCM_ERR_OK != ret) {
                CONSOLE_Write(aConsoleID, "Failed to send stop signal\n");
            }
        } else {
            CONSOLE_Write(aConsoleID, "Invalid argument. Usage: app start/stop\n");
        }
    } else {
        CONSOLE_Write(aConsoleID, "No argument mentioned. Usage: app start/stop\n");
    }
}

/** @} */
