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
#include <string.h>
#include <ulog.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <chip_config.h>
#include <console.h>
#include <wl_eth.h>
#include <wl_eth_xcvr.h>
#include <wl_eth_osil.h>
#include <board.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     (ULOG_LVL_INFO)
#define WL_SHELL_EVENT          (SystemEvent4)

static BCM_TaskType taskID;
static WLXCVR_RadioParamIDType radioParamID;
static WLXCVR_RadioParamValType radioParamValue;
static WLETH_StatsType wlStats;
static int32_t result = BCM_ERR_UNKNOWN;

static void WL_ShellWaitAndClrEvent(EventMaskType aEvent)
{
    int32_t status = BCM_ERR_OK;
    BCM_EventMaskType mask;
    StatusType retval;

    retval = GetTaskID((TaskRefType)&taskID);
    if (E_OK == retval) {
        status = BCM_ERR_OK;
    } else {
        status = BCM_ERR_UNKNOWN;
    }
    if (BCM_ERR_OK == status) {
        status = BCM_WaitEvent(aEvent);
        if (status == BCM_ERR_OK) {
            status = BCM_GetEvent(taskID, &mask);
            if ((BCM_ERR_OK == status) && ((mask & aEvent) != 0UL)) {
                (void)BCM_ClearEvent(aEvent);
            }
        }
    }
}

void WLXCVR_GetParamInd(ETHXCVR_IDType aID,
        int32_t aResult,
        WLXCVR_RadioParamIDType aParam,
        WLXCVR_RadioParamValType aValue)
{
    result = aResult;
    if (BCM_ERR_OK == aResult) {
        radioParamValue = aValue;
    } else {
        ULOG_ERR("WLXCVR_GetParamInd() error: %d\n", aResult);
    }
    BCM_SetEvent(taskID, WL_SHELL_EVENT);
}

void WLETH_GetStatsInd(ETHER_HwIDType aID,
        int32_t aResult,
        const WLETH_StatsType *const aStats)
{
    result = aResult;
    if (BCM_ERR_OK == aResult) {
        BCM_MemCpy(&wlStats, aStats, sizeof(WLETH_StatsType));
    } else {
        ULOG_ERR("WLETH_GetStatsInd: error: %d\n", aResult);
    }
    BCM_SetEvent(taskID, WL_SHELL_EVENT);
}

static void WL_ShellPrintCounters(void)
{
    int32_t ret;

    ret = WLETH_GetStats(0UL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("WLETH_GetStats returned err: %d\n", ret);
    }
    WL_ShellWaitAndClrEvent(WL_SHELL_EVENT);
    if (BCM_ERR_OK == result) {
        ULOG_ERR("#### WL counters ####\n");
        ULOG_ERR("\ttxFrag: %d\n", wlStats.txFrag);
        ULOG_ERR("\ttxMulti: %d\n", wlStats.txMulti);
        ULOG_ERR("\ttxFail: %d\n", wlStats.txFail);
        ULOG_ERR("\ttxMultRetry: %d\n", wlStats.txMultRetry);
        ULOG_ERR("\trxDuplicate: %d\n", wlStats.rxDuplicate);
        ULOG_ERR("\trtsSuccess: %d\n", wlStats.rtsSuccess);
        ULOG_ERR("\trtsFail: %d\n", wlStats.rtsFail);
        ULOG_ERR("\ttxNoACK: %d\n", wlStats.txNoACK);
        ULOG_ERR("\trxFrag: %d\n", wlStats.rxFrag);
        ULOG_ERR("\trxMulti: %d\n", wlStats.rxMulti);
        ULOG_ERR("\trxCRC: %d\n", wlStats.rxCRC);
    }
}

static void WL_ShellGetRadioParams(WLXCVR_RadioParamIDType aParam,
        WLXCVR_RadioParamIDType *const aVal)
{
    radioParamID = aParam;
    result = WLXCVR_GetRadioParamReq(0UL, 0UL, radioParamID);
    if (BCM_ERR_OK == result) {
        WL_ShellWaitAndClrEvent(WL_SHELL_EVENT);
        *aVal = radioParamValue;
    }
}

static void WL_ShellRadioParamsCmdHandler(uint32_t aConsoleID, char* aArgString,
        uint32_t aArgStringLen)
{
    char *remain = NULL;
    uint32_t remLen = 0UL;
    uint32_t val = 0UL;

    CONSOLE_SplitLine(aArgString, aArgStringLen, &remain, &remLen);
    if (remLen == 0UL) {
        CONSOLE_Write(aConsoleID, "radio get <paramter> ==> Get radio parameter \n");
        goto err_exit;
    }
    if (0UL == strncmp(remain, "get", 3UL)) {
        CONSOLE_SplitLine(remain, remLen, &remain, &remLen);
        if (0UL == strncmp(remain, "pwr", 3UL)) {
            WL_ShellGetRadioParams(WLXCVR_RADIOPARAMS_TXPWR, &val);
            if (BCM_ERR_OK == result) {
                ULOG_ERR("TX channel power: %d(dbm)\n", val);
            }
        }
        if (0UL == strncmp(remain, "bitrate", 7UL)) {
            WL_ShellGetRadioParams(WLXCVR_RADIOPARAMS_BITRATE, &val);
            if (BCM_ERR_OK == result) {
                ULOG_ERR("Bit rate: %d\n", val);
            }
        }
    }
err_exit:
    return;
}

static void WL_ShellPrintHelp(uint32_t aConsoleID)
{
    CONSOLE_Write(aConsoleID, "wlan Commands:\n");
    CONSOLE_Write(aConsoleID, "counter  ==> Dump WL stats\n");
    CONSOLE_Write(aConsoleID, "radio get <paramter> ==> Get/Set the radio parameter \n");
}

void Shell_WlCmdHandler(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    if (NULL != aArgString) {
        if (0UL == strncmp(aArgString, "counter", 7UL)) {
            WL_ShellPrintCounters();
        } else if (0UL == strncmp(aArgString, "radio", 5UL)) {
            WL_ShellRadioParamsCmdHandler(aConsoleID, aArgString, aArgStringLen);
        }
    } else {
        WL_ShellPrintHelp(aConsoleID);
    }
}
