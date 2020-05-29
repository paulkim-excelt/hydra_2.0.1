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
#include <media_system.h>
#include <component.h>
#include <ee.h>
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <shell.h>
#include <bcm_time.h>
#include <eth_osil.h>
#include <atomic.h>
#include <bcm_test.h>
#include <vgc.h>
#include <vgc_osil.h>
#include <cache.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()   ULOG_LVL_INFO
#define CMD_STR_LEN         (5UL)

static const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 6UL,

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

    .component[4].compID = MSYS_COMP_ID_AVBL,
    .component[4].numConfigs = 1UL,
    .component[4].config[0] = MSYS_CONFIG_AVB_LSTR,
    .component[4].portMap[0].srcCompIdx = 0U,
    .component[4].portMap[0].srcPortIdx = 0U,

    .component[5].compID = MSYS_COMP_ID_EX_SNK,
    .component[5].numConfigs = 1UL,
    .component[5].config[0] = 0U,
    .component[5].portMap[0].srcCompIdx = 4U,
    .component[5].portMap[0].srcPortIdx = 0U,

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

#define WIDTH   (1280UL)
#define HEIGHT  (720UL)
static uint32_t Reverse(uint32_t aData)
{
    uint32_t val;
#if defined(__GNU__)
    COMP_ASM( "rbit %0, %1"
            :"=r"(val)
            :"r"(aData)
            :"cc");
#elif defined(__ARMCC__)   /* !__GCC__ */
    COMP_ASM( "rbit val, aData");
#else   /* !__ARMCC__ */
#error Unsupported compiler
#endif /* __ARMCC__ */

    return val;
}

static uint32_t CalculateCRC(const uint8_t *const aAddr,
                             uint32_t aLen)
{
#define CRC32_POLY             (0x04C11DB7UL)
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;

    i = 0UL;
    crc = 0UL;
    while (aLen != 0UL) {
        byte = aAddr[i];
        byte = Reverse(byte);
        for (j = 0UL; j <= 7UL; j++) {
            if (((int32_t)(crc ^ byte)) < 0L) {
                crc = (crc << 1UL) ^ CRC32_POLY;
            } else {
                crc = crc << 1UL;
            }
            byte = byte << 1UL;
        }
        i++;
        aLen--;
    }

    return Reverse(~crc);
}
extern uint8_t ExSnk_BsBuffer[];
extern uint32_t ExSnk_BsBufferSize;

/*
    For each configuration of interest, we maintain multiple CRC's.
    The count is given by the NUM_REF_CRCS macro, and the array has the name -
    vidxmtr_qt_crcs.

    CRC at Index 0 => expected to match when ldclib is built with the gcc toolchain gcc-arm-none-eabi-4_9-2015q3
    CRC at Index 1 => expected to match when ldclib is built with the rvct toolchain DS-5_5.26.2
*/

#define NUM_REF_CRCS 2U

const uint32_t vidxmtr_qt_crcs[NUM_REF_CRCS] = {
#ifdef AVCE_ENABLE_LOW_COMPLEXITY
#if defined ENABLE_CAM_MODULE_OX3A10
    0xec814d1aUL,
    0x6935b43eUL,
#elif defined ENABLE_CAM_MODULE_AR0143
    0x559396bbUL,
    0x93633cb3UL,
#endif
#else
#ifdef ENABLE_CAM_MODULE_OV10640
    0x34119f1fUL,
    0x34119f1fUL,
#elif defined ENABLE_CAM_MODULE_OX3A10
    0x890eb002UL,
    0x56263d25UL,
#endif
#endif
};

volatile int32_t vidxmtr_qt_result = BCM_AT_NOT_STARTED;
volatile uint32_t TestAlarmVal = 0UL;
uint32_t VidXmtrATTestCount = 0UL;
void Test_VidXmtrAlarm_cb()
{
    if(1UL == TestAlarmVal)
    {
        if(++VidXmtrATTestCount == 2)
        {
            ATOMIC_Set(&TestAlarmVal, 0UL);
            SetEvent(VidXmtr_AT,SystemEvent0);
        }
    }
}
extern uint32_t ExSnk_SliceCount;
TASK(VidXmtr_AT)
{
    int32_t retVal;
    int32_t ret;
    uint32_t i, crc;
    BCM_TimeType t1 = {0UL, 0UL};
    VidXmtrATTestCount = 0UL;

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
    ULOG_ERR("Video Transceiver started at %u sec %u msec from start\n", t1.s, t1.ns/1000000UL);
    ATOMIC_Set(&TestAlarmVal, 1UL);

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
    crc = CalculateCRC(ExSnk_BsBuffer, ExSnk_BsBufferSize);
    ULOG_INFO("CRC:0x%08x, %d\n", crc, ExSnk_BsBufferSize);

    vidxmtr_qt_result = BCM_ERR_UNKNOWN;
    for (i = 0U; i < NUM_REF_CRCS; i++) {
        if (vidxmtr_qt_crcs[i] == crc) {
            vidxmtr_qt_result = BCM_ERR_OK;
            break;
        }
    }

    retVal = BCM_TerminateTask();
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("BCM_TerminateTask Failed %d\n",retVal);
    }
}

void ApplicationInit(void)
{
    ETHER_EnableLoopBack(ETHER_HW_ID_0);
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        vidxmtr_qt_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(VidXmtr_AT);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = vidxmtr_qt_result;
    }
    return ret;
}

void Shell_App(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    int32_t ret;
    if (NULL != aArgString) {
        if (0 == strncmp(aArgString, "start", CMD_STR_LEN)) {
            ret = BCM_ActivateTask(VidXmtr_AT);
            if (BCM_ERR_OK != ret) {
                CONSOLE_Write(aConsoleID, "Failed to send start signal\n");
            }
        } else if (0 == strncmp(aArgString, "stop", CMD_STR_LEN)) {
            ret = BCM_SetEvent(VidXmtr_AT, SystemEvent0);
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


