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


#include <stdlib.h>
#include <string.h>
#include <system.h>
#include "ee.h"
#include <board.h>
#include <init.h>
#include <idle.h>
#include <build_info.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <osil/bcm_osil.h>
#include <osil/wds_osil.h>
#include <console.h>

#define GetModuleLogLevel()         (ULOG_LVL_INFO)

#if defined(ENABLE_AVT)
#include <avt.h>
#endif

extern void ApplicationInit(void);

/* system monitor event */
#define SYSTEM_MONITOR_TASK         (BCM8910X_SystemTask)
#define SYSTEM_MONITOR_EVENTS       (SYSTEM_MONITOR_ALARM_EVENT)

#ifdef ENABLE_VTMON
#define SYS_TIME_CNT_MAX            (600UL)
static uint32_t SYS_TimeCnt;
#endif

#define NS_TO_MS(ns)                (ns / 1000000UL)

extern uint32_t EarlyInitTime;
uint32_t SYS_CommsReadyTime;
uint32_t SYS_BootTime;

/**
    @code{.unparsed}
    @endcode
*/
static void SYS_ProcessEvents(uint32_t aEventMask)
{
    int32_t    retVal = BCM_ERR_OK;

    /* Case when the system alarm has expired */
    if(aEventMask & SYSTEM_MONITOR_ALARM_EVENT){
#ifdef ENABLE_VTMON
        VTMON_TempConfigType tempConfig;
        VTMON_TempType temp;

        /* System notification alarm is fired every one second and */
        /* VTMON temperature is printed on the console every 1 min */
        SYS_TimeCnt++;
        if (SYS_TIME_CNT_MAX == SYS_TimeCnt) {
            retVal = VTMON_GetTemperature(&tempConfig, &temp);
            if (BCM_ERR_OK == retVal) {
                ULOG_ERR("[VTMON] temp: %d C\n", temp);
            }
            SYS_TimeCnt = 0UL;
        }
#endif
    }
    return;
}

#ifdef ENABLE_SYSTEM_MONITOR
void SYS_NotfnAlarmCb(void)
{
    (void)SetEvent(SYSTEM_MONITOR_TASK, SYSTEM_MONITOR_ALARM_EVENT);
}
#endif

int32_t RPC_AsyncEvent(BCM_GroupIDType aGrpID, BCM_CompIDType aCompID, uint8_t aCmdID,
                       const uint8_t *const aParams, uint32_t aSize)
{
    /* This function to be extended */
    return BCM_ERR_OK;
}

TASK(BCM8910X_SystemTask)
{
    int32_t retVal;
    BCM_EventMaskType mask;
    MCU_BootTimeInfoType bootTime;
    char consoleBuf[128UL];
    uint8_t blBootTime;
    uint8_t blXcvrInitTime;
    uint32_t entryTime;

#if defined(ENABLE_WATCHDOG_SP805)
    /* Initialize watchdog */
    WDT_Init(0UL, &WDT_Config[0UL]);
#endif
    WDS_Init(WDS_SupervisionEntities, WDS_SUPERVISION_ENTITY_CNT);

    MCU_GetBootTimeInfo(&bootTime);
    blBootTime = bootTime.blBootTime;
    blXcvrInitTime = bootTime.xcvrInitTime;
    entryTime = blXcvrInitTime + EarlyInitTime;

#ifdef ENABLE_WFI
    (void)IDLE_Init();
#endif


#if defined(ENABLE_GPIO_GIO_V2)
    PINMUX_Init();
#endif

#if defined(ENABLE_AVT)
    AVT_SetPeriodicTimer((uint32_t)SYS_TICK_US);
#endif  /* ENABLE_AVT */

    (void)BCM_ActivateTask(TaskShell);

#if defined(ENABLE_FLSMGR)
    /* This will internally initialize the flash, flash manager and PTM and
    wait for Acknowledgement from FLSMGR to be ready. Currently, not checking for
    error state. To be added in Future */
    FLSMGR_ServerInit();
    do {
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    } while (SYS_SUBSYSTEM_STATE_UNINIT == SYS_GetState(SYS_SUBSYSTEM_NVM));
#endif

    (void)BCM_ActivateTask(CommsTask);
    do {
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    } while (SYS_SUBSYSTEM_STATE_UNINIT == SYS_GetState(SYS_SUBSYSTEM_COMMS));
    SYS_CommsReadyTime = NS_TO_MS(BCM_GetTimeNs()) + entryTime;

    (void)BCM_ActivateTask(TaskSerialIO);

    retVal = BCM8910X_LateInit();

    if (BCM_ERR_OK == retVal) {
        ApplicationInit();
    } else {
        ULOG_ERR("BCM8910X_LateInit Failed (%d) \n", retVal);
    }

    SYS_BootTime = NS_TO_MS(BCM_GetTimeNs()) + entryTime;
    CONSOLE_PrintToBuffer(consoleBuf, sizeof(consoleBuf),
            "BootTime(ms): BL:%d XcvrInit:%d\n", blBootTime, blXcvrInitTime);
    CONSOLE_Write(CONSOLE_UART, consoleBuf);

    CONSOLE_PrintToBuffer(consoleBuf, sizeof(consoleBuf),
            "BootTime(ms): CommsReady:%d SysReady:%d\n",
            SYS_CommsReadyTime, SYS_BootTime);
    CONSOLE_Write(CONSOLE_UART, consoleBuf);
    while (1UL) {
        /* All System Processing, Error and Safety handling here */
        (void)BCM_WaitEvent(SYSTEM_MONITOR_ALARM_EVENT);
        (void)BCM_GetEvent(BCM8910X_SystemTask, &mask);
        (void)BCM_ClearEvent(mask);
        if (mask & SYSTEM_MONITOR_ALARM_EVENT) {
            SYS_ProcessEvents(mask & SYSTEM_MONITOR_ALARM_EVENT);

            retVal = WDS_Monitor(WDS_SupervisionEntities, WDS_SUPERVISION_ENTITY_CNT);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("WDS_Monitor returned %d\n", retVal);
            }
        }
    }
    while (SYS_SUBSYSTEM_STATE_UNINIT != SYS_GetState(SYS_SUBSYSTEM_COMMS)) {
        (void)BCM_SetEvent(CommsTask, ShutdownEvent);
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    }
#if defined(ENABLE_FLSMGR)
    while (SYS_SUBSYSTEM_STATE_UNINIT != SYS_GetState(SYS_SUBSYSTEM_NVM)) {
        (void)BCM_SetEvent(FLSMGR_Server, ShutdownEvent);
        (void)BCM_WaitEvent(SYS_NOTIF_EVENT);
        (void)BCM_ClearEvent(SYS_NOTIF_EVENT);
    }
#endif

}

void BCM8910X_StartOS(void)
{
    (void)BCM_StartOS(0UL);
}

int main()
{
    /* Do Nothing here*/
    while(1UL) {}
    return BCM_ERR_OK;
}
