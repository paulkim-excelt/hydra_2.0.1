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
#include <imgl.h>

#define GetModuleLogLevel()   ULOG_LVL_INFO
#define CMD_STR_LEN         (5UL)

static const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 8UL,

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

    .component[5].compID = MSYS_COMP_ID_AVCD,
    .component[5].numConfigs = 2UL,
    .component[5].config[0] = MSYS_CONFIG_AVCD_MOD,
    .component[5].config[1] = MSYS_CONFIG_AVCD_FW0,
    .component[5].portMap[0].srcCompIdx = 4U,
    .component[5].portMap[0].srcPortIdx = 0U,

    .component[6].compID = MSYS_COMP_ID_GFXSRC,
    .component[6].numConfigs = 1UL,
    .component[6].config[0] = 0U,
    .component[6].portMap[0].srcCompIdx = 0U,
    .component[6].portMap[0].srcPortIdx = 0U,

    .component[7].compID = MSYS_COMP_ID_VGCC,
    .component[7].numConfigs = 1UL,
    .component[7].config[0] = 0UL,
    .component[7].portMap[0].srcCompIdx = 5U,
    .component[7].portMap[0].srcPortIdx = 0U,
    .component[7].portMap[1].srcCompIdx = 6U,
    .component[7].portMap[1].srcPortIdx = 0U,

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

uint8_t COMP_SECTION(".dram.test.data") YUVBuffer[WIDTH*HEIGHT*3/2];
uint32_t err_count = 0;
int32_t Measure_SNR(uint8_t *aSrc)
{
    int32_t i;
    int32_t j;
    int32_t ret;
    uint32_t err = 0UL;
    uint8_t *src = aSrc;
    uint8_t *ref = YUVBuffer;

    DCACHE_InvalRange((uint32_t)aSrc, (WIDTH*HEIGHT*3/2));
    for (i=0; i<HEIGHT; i++) {
        for (j=0; j<WIDTH; j++) {
            err += abs(ref[WIDTH*i + j] - src[WIDTH*i + j]);
        }
    }
    src = &aSrc[WIDTH*HEIGHT];
    ref = &YUVBuffer[WIDTH*HEIGHT];
    for (i=0; i<(HEIGHT/2); i++) {
        for (j=0; j<(WIDTH/2); j++) {
            err += abs(ref[(WIDTH/2)*i + j] - src[(WIDTH/2)*i + j]);
        }
    }
    src = &aSrc[(WIDTH*HEIGHT) + ((WIDTH/2)*(HEIGHT/2))];
    ref = &YUVBuffer[(WIDTH*HEIGHT) + ((WIDTH/2)*(HEIGHT/2))];
    for (i=0; i<(HEIGHT/2); i++) {
        for (j=0; j<(WIDTH/2); j++) {
            err += abs(ref[(WIDTH/2)*i + j] - src[(WIDTH/2)*i + j]);
        }
    }
    err_count = err;
    ULOG_ERR("err count = %d\n",err);
    if(err < 4*WIDTH*HEIGHT*3/2) {
        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_UNKNOWN;
    }
    return ret;
}

static volatile int32_t vidxcvr_qt_result = BCM_AT_NOT_STARTED;
volatile uint32_t TestAlarmVal = 0UL;
uint32_t VidXcvrATTestCount = 0UL;
void Test_VidXcvrAlarm_cb()
{
    if(1UL == TestAlarmVal)
    {
        if(++VidXcvrATTestCount == 2)
        {
            ATOMIC_Set(&TestAlarmVal, 0UL);
            SetEvent(VidXcvr_AT,SystemEvent0);
        }
    }
}

extern uint16_t rle_buffer[512*1024];
extern void RLE_Decode(unsigned char * buffer, uint32_t aSize);

TASK(VidXcvr_AT)
{
    int32_t retVal;
    int32_t ret;
    BCM_TimeType t1 = {0UL, 0UL};
    uint32_t lSize = 0UL;
    VGC_DispStatsType stats;
    IMGL_LoadStatusType imglStatus;
    BCM_MemSet(&stats, 0x0, sizeof(VGC_DispStatsType));
    memset(VGC_VidMem, 0, VGC_VidSz*3UL);

    int32_t ptmStatus;
    ret = IMGL_LoadImg((uint16_t)0xFF00, (uint8_t*)rle_buffer, 0UL, 512*1024, SystemEvent1, &imglStatus);
    if (BCM_ERR_OK == ret) {
        (void)BCM_WaitEvent(SystemEvent1);
        (void)BCM_ClearEvent(SystemEvent1);
        ptmStatus = IMGL_GetStatus(&imglStatus);
        if (ptmStatus == BCM_ERR_OK) {
            lSize = imglStatus.size;
            ULOG_INFO("lsize= %d\n", lSize);
            RLE_Decode(YUVBuffer, lSize);
            ULOG_INFO("VidXcvr_AT_test PTM load for test data successful\n");
        } else {
            ULOG_ERR("VidXcvr_AT_test PTM load for test data failed (%d)\n", ptmStatus);
        }
    }

    VidXcvrATTestCount = 0UL;

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
    VGC_DispGetStats(&stats);
    if (stats.framesDisplayed > 25UL) {
        if (BCM_ERR_OK == Measure_SNR(&VGC_VidMem[0UL])) {
            vidxcvr_qt_result = BCM_ERR_OK;
        } else if (BCM_ERR_OK == Measure_SNR(&VGC_VidMem[VGC_VidSz])) {
            vidxcvr_qt_result = BCM_ERR_OK;
        } else if (BCM_ERR_OK == Measure_SNR(&VGC_VidMem[2UL * VGC_VidSz])) {
            vidxcvr_qt_result = BCM_ERR_OK;
        } else {
            vidxcvr_qt_result = BCM_ERR_UNKNOWN;
        }
    } else {
        vidxcvr_qt_result = BCM_ERR_UNKNOWN;
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
        vidxcvr_qt_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(VidXcvr_AT);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = vidxcvr_qt_result;
    }
    return ret;
}

void Shell_App(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    int32_t ret;
    if (NULL != aArgString) {
        if (0 == strncmp(aArgString, "start", CMD_STR_LEN)) {
            ret = BCM_ActivateTask(VidXcvr_AT);
            if (BCM_ERR_OK != ret) {
                CONSOLE_Write(aConsoleID, "Failed to send start signal\n");
            }
        } else if (0 == strncmp(aArgString, "stop", CMD_STR_LEN)) {
            ret = BCM_SetEvent(VidXcvr_AT, SystemEvent0);
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


