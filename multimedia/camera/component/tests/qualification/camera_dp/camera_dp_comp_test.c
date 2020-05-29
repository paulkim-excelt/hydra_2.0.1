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
 File Name: camera_dp_comp_test.c
 Descritpion: This file implememnts test application for camera component in
 Dual-pass mode.
******************************************************************************/

#define GetModuleLogLevel() ULOG_LVL_INFO

/* Includes */
#include <stddef.h>
#include <compiler.h>
#include <string.h>
#include <media_system.h>
#include <camera.h>
#include <ee.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

/* Test status */
static volatile int32_t camera_qt1_result = BCM_AT_NOT_STARTED;

const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 2UL,

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

    .component[1].compID = MSYS_COMP_ID_EX_SNK,
    .component[1].numConfigs = 1UL,
    .component[1].config[0] = 0UL,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 1U,
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

/* CameraCompTest_Start */
int32_t CameraCompTest_Start(void)
{
    int32_t ret = BCM_ERR_OK;

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

err:
    return ret;
}

/* CameraCompTest_Stop */
int32_t CameraCompTest_Stop(void)
{
    int32_t ret = BCM_ERR_OK;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Stop Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_DeInit Failed %d\n",ret);
    }

err:
    return ret;
}

/* CameraDP_TestTask */
TASK(CameraDP_TestTask)
{
    BCM_EventMaskType mask = 0UL;
    int32_t ret = BCM_ERR_OK;

    /* Start Camera */
    ret = CameraCompTest_Start();
    if (BCM_ERR_OK != ret) {
        goto exit;
    }

    /* Wait for result */
    BCM_WaitEvent(CAMERA_TEST_EVENT_PASS | CAMERA_TEST_EVENT_FAIL);
    BCM_GetEvent(CameraDP_TestTask, &mask);

    if (0UL != (mask & CAMERA_TEST_EVENT_FAIL)) {
        BCM_ClearEvent(CAMERA_TEST_EVENT_FAIL);
        camera_qt1_result = BCM_ERR_UNKNOWN;
        ULOG_INFO("Test Failed\n");

    } else if (0UL != (mask & CAMERA_TEST_EVENT_PASS)) {
        BCM_ClearEvent(CAMERA_TEST_EVENT_PASS);
        camera_qt1_result = BCM_ERR_OK;
        ULOG_INFO("Test Passed\n");
    }

    /* Stop Camera */
    ret = CameraCompTest_Stop();

exit:
    if (BCM_ERR_OK != ret) {
        camera_qt1_result = ret;
    }

    BCM_TerminateTask();
}

/* BCM_ExecuteAT */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        camera_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(CameraDP_TestTask);
        ret = BCM_ERR_OK;
    }

    return ret;
}

/* BCM_GetResultAT */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        ret = camera_qt1_result;
    }

    return ret;
}

void ApplicationInit()
{
    BCM_ActivateTask(ExSnk_CmdProcessTask);
}
