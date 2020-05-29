/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
  File Name:  sensor_test.c
Descritpion: This file implements unit test application for sensor.
 ******************************************************************************/

#define GetModuleLogLevel() GetSenorTestLogLevel()

/* Includes */
#include <string.h>
#include "cam_erika.h"
#include "camera_sensor.h"
#include "ee.h"
#include "bcm_err.h"
#include "camera_sensor_osil.h"
/* TODO: To Remove IIC */
#include <i2c.h>
#include "ulog.h"
#include "console.h"
#include <osil/bcm_osil.h>

/* defines */
/* Sensor Commands */
#define MSYS_COMP_CMD_INIT   (0UL)
#define MSYS_COMP_CMD_CONFIG (1UL)
#define MSYS_COMP_CMD_START  (2UL)
#define MSYS_COMP_CMD_STOP   (3UL)
#define MSYS_COMP_CMD_DEINIT (4UL)
#define COMP_QUIT       (5UL)

/* Test events mapping */
#define TEST_EVENT_START      SystemEvent0
#define TEST_EVENT_STOP       SystemEvent1
#define TEST_COMMAND_COMPLETE SystemEvent2
#define TEST_COMMAND_ERROR    SystemEvent3

/* IIC */
#define IIC_HW_ID          (3)
#define IIC_SPEED          (IIC_SPEED_100K)

/* Sensor Test Log level */
static uint32_t gSensorTestLogLevel = ULOG_LVL_VERBOSE;

/* Globals */
static uint32_t gCmd = 0UL;
static Cam_Configs gCamCfg;

/* Image - extern */
extern const uint8_t sensor_image_array[];
extern IIC_MsgQInfoType CSN_MsgQInfo;

/* Get Sensor Test Log Level */
COMP_INLINE uint32_t GetSenorTestLogLevel()
{
    return gSensorTestLogLevel;
}

/* ReadSensorChipID */
static int32_t ReadSensorChipID(void)
{
    int32_t ret = BCM_ERR_OK;
    /* IIC */
    uint8_t data[6] = {0};
    IIC_PktType pkt[2];
    int32_t i;
    BCM_EventMaskType mask;
    TaskType taskid;
    StatusType erikaRet;
    uint8_t reg_value[2];

    erikaRet = GetTaskID(&taskid);
    if (E_OK != erikaRet) {
        ULOG_ERR("%s: GetTaskID failed\n", __FUNCTION__);
    }

    /* IIC packet */
    pkt[0].flags = (0UL);
    pkt[0].slaveAddr = 0x30;
    reg_value[0] = (0x300a >> 8) & 0xff;
    reg_value[1] = (0x300a & 0xff);
    pkt[0].length = 2;
    pkt[0].data = reg_value;
    pkt[1].flags = (IIC_PKT_FLG_OP_RD | IIC_PKT_FLG_RESTART);
    pkt[1].slaveAddr = 0x30;
    pkt[1].length = 3;
    pkt[1].data = data;

    /* Chip ID read */
    ret = IIC_TransferRequest(IIC_HW_ID, pkt, 2, &CSN_MsgQInfo);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: IIC_TransferRequest failed\n", __FUNCTION__);
        goto exit;
    }

    BCM_WaitEvent(CSN_EVENT_IIC);
    BCM_GetEvent(taskid, &mask);

    if (mask & CSN_EVENT_IIC) {
        BCM_ClearEvent(CSN_EVENT_IIC);
    }

    ret = IIC_RecvResponse(IIC_HW_ID, &CSN_MsgQInfo);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("IIC_RecvResponse failed!!\n");
    }

    /* Print Chip ID */
    for (i = 0; i < 3; i++) {
        ULOG_ERR("%s: DATA %02x\n", __FUNCTION__, data[i]);
    }

exit:
    return ret;
}

/* SensorTest_Configure */
static int32_t SensorTest_Configure(const Cam_Configs aCfg)
{
    int32_t ret = BCM_ERR_OK;
    const CSN_ImageType *image = aCfg.pSensorImg;

    /* Configure CSN */
    ret = CSN_Configure(image);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: Configure failed with error = %d\n", __FUNCTION__, ret);
    } else {
        ULOG_VERB("%s: Configure Success \n", __FUNCTION__);
    }

    return ret;
}

/* WaitForCmdComplete */
int32_t WaitForCmdComplete()
{
    BCM_EventMaskType mask = 0UL;
    int32_t retVal;
    int32_t ret = BCM_ERR_OK;

    retVal = BCM_WaitEvent(TEST_COMMAND_COMPLETE|TEST_COMMAND_ERROR);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: BCM_WaitEvent failed \n", __FUNCTION__);
        ret = BCM_ERR_UNKNOWN;
        goto exit;
    }

    retVal = BCM_GetEvent(SensorTestTask, &mask);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: BCM_GetEvent failed\n", __FUNCTION__);
        ret = BCM_ERR_UNKNOWN;
        goto exit;
    }

    if (0UL != (mask & TEST_COMMAND_COMPLETE)) {
        BCM_ClearEvent(TEST_COMMAND_COMPLETE);
    }

    if (0UL != (mask & TEST_COMMAND_ERROR)) {
        ULOG_ERR("%s:  TEST_COMMAND_ERROR \n", __FUNCTION__);
        BCM_ClearEvent(TEST_COMMAND_ERROR);
        ret = BCM_ERR_UNKNOWN;
    }

exit:
    return ret;
}

/* SendWaitSensorCmd */
int32_t SendWaitSensorCmd(uint32_t aCmd)
{
    BCM_EventMaskType mask = 0UL;
    int32_t retVal;
    int32_t ret = BCM_ERR_OK;

    gCmd = aCmd;
    mask = CAM_EVENT_CQUEUE;

    retVal = BCM_SetEvent(CAMERA_CMD_TASK, mask);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
        ret = BCM_ERR_UNKNOWN;
        goto exit;
    }

    ret = WaitForCmdComplete();

exit:
    return ret;
}

/* Camera_DataProcessTask */
TASK(Camera_DataProcessTask)
{
    int32_t status;

    status = BCM_TerminateTask();
    if(BCM_ERR_OK != status) {
        ULOG_ERR("BCM_TerminateTask Failed \n");
    }
}

/* Camera_CmdProcessTask */
TASK(Camera_CmdProcessTask)
{
    BCM_EventMaskType mask;
    int32_t retVal;
    uint32_t quit = 0;
    int32_t ret = BCM_ERR_OK;

    retVal = BCM_SetEvent(SensorTestTask, TEST_COMMAND_COMPLETE);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
        goto exit;
    }

    while(0UL == quit) {
        retVal = BCM_WaitEvent(CSN_EVENT_IIC|SENSOR_EVENT_CMD_COMPLETE|SENSOR_EVENT_ERROR|CAM_EVENT_CQUEUE);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: BCM_WaitEvent failed \n", __FUNCTION__);
        }

        if (0UL != (mask & CSN_EVENT_IIC)) {
            ULOG_VERB("%s: CSN_EVENT_IIC from CSN\n", __FUNCTION__);
            BCM_ClearEvent(CSN_EVENT_IIC);
            CSN_ProcessEvent_OSIL();
        }

        retVal = BCM_GetEvent(CAMERA_CMD_TASK, &mask);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: BCM_GetEvent failed\n", __FUNCTION__);
        }

        if (0UL != (mask & SENSOR_EVENT_CMD_COMPLETE)) {
            BCM_ClearEvent(SENSOR_EVENT_CMD_COMPLETE);
            retVal = BCM_SetEvent(SensorTestTask, TEST_COMMAND_COMPLETE);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
            }
        }

        if (0UL != (mask & SENSOR_EVENT_ERROR)) {
            BCM_ClearEvent(SENSOR_EVENT_ERROR);
            ULOG_ERR("%s: SENSOR_EVENT_ERROR \n", __FUNCTION__);
            retVal = BCM_SetEvent(SensorTestTask, TEST_COMMAND_ERROR);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
            }
        }

        if (0UL != (mask & CAM_EVENT_CQUEUE)) {
            BCM_ClearEvent(CAM_EVENT_CQUEUE);
            switch(gCmd){
                case MSYS_COMP_CMD_INIT:
                    /* Initialize Sensor */
                    ret = CSN_Init();
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("%s: CSN_Init failed with error = %d\n", __FUNCTION__, ret);
                    } else {
                        ULOG_VERB("%s: CSN_Init Success \n", __FUNCTION__);
                    }
                    break;
                case MSYS_COMP_CMD_CONFIG:
                    /* Configure */
                    ret = SensorTest_Configure(gCamCfg);
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("%s: SensorTest_Configure failed with error = %d\n", __FUNCTION__, ret);
                    } else {
                        ULOG_VERB("%s: SensorTest_Configure Success \n", __FUNCTION__);
                    }
                    break;
                case MSYS_COMP_CMD_START:
                    /* Start */
                    ret = CSN_Start();
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("%s: CSN_Start failed with error = %d\n", __FUNCTION__, ret);
                    } else {
                        ULOG_VERB("%s: CSN_Start Success \n", __FUNCTION__);
                    }
                    break;
                case MSYS_COMP_CMD_STOP:
                    /* Stop */
                    ret = CSN_Stop();
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("%s: CSN_Stop failed with error = %d\n", __FUNCTION__, ret);
                    } else {
                        ULOG_VERB("%s: CSN_Stop Success \n", __FUNCTION__);
                    }
                    break;
                case MSYS_COMP_CMD_DEINIT:
                    /* Deinit */
                    ret = CSN_Deinit();
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("%s: CSN_Deinit failed with error = %d\n", __FUNCTION__, ret);
                    } else {
                        ULOG_VERB("%s: CSN_Deinit Success \n", __FUNCTION__);
                    }
                    break;
                case COMP_QUIT:
                    if (COMP_QUIT == gCmd) {
                        retVal = BCM_SetEvent(SensorTestTask, TEST_COMMAND_COMPLETE);
                        quit = 1;
                        if (BCM_ERR_OK != retVal) {
                            ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
                            ret = BCM_ERR_UNKNOWN;
                        }
                    }
                    break;
                default:
                   ULOG_ERR("%s: Unknown command \n", __FUNCTION__);
                   break;
            }
            if (BCM_ERR_OK != ret) {
                retVal = BCM_SetEvent(SensorTestTask, TEST_COMMAND_ERROR);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
                }
            }
        }
    }

exit:
    ULOG_INFO("%s: COMP_QUIT \n", __FUNCTION__);
    retVal = BCM_TerminateTask();
    if(BCM_ERR_OK != retVal) {
        ULOG_ERR("BCM_TerminateTask Failed \n");
    }
}

/* SensorTest_Start */
int32_t SensorTest_Start(void)
{
    int32_t ret = BCM_ERR_OK;

    ret = SendWaitSensorCmd(MSYS_COMP_CMD_INIT);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: MSYS_COMP_CMD_INIT failed \n", __FUNCTION__);
        goto exit;
    }
    ULOG_VERB("%s: MSYS_COMP_CMD_INIT Success \n", __FUNCTION__);

    /* Read Sensor Images */
    gCamCfg.pSensorImg = (struct _CSN_ImageType *) &sensor_image_array[0];
    ret = SendWaitSensorCmd(MSYS_COMP_CMD_CONFIG);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: MSYS_COMP_CMD_CONFIG Sensor failed \n", __FUNCTION__);
        goto exit;
    }
    ULOG_VERB("%s: MSYS_COMP_CMD_CONFIG Sensor Success \n", __FUNCTION__);

    /* Read Chip ID */
    ret = ReadSensorChipID();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: Read Chip ID failed\n", __FUNCTION__);
    }

    ret = SendWaitSensorCmd(MSYS_COMP_CMD_START);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: MSYS_COMP_CMD_START failed \n", __FUNCTION__);
        goto exit;
    }
    ULOG_VERB("%s: MSYS_COMP_CMD_START Success \n", __FUNCTION__);

exit:
    return ret;;
}

/* SensorTest_Stop */
int32_t SensorTest_Stop(void)
{
    int32_t ret = BCM_ERR_OK;

    ret = SendWaitSensorCmd(MSYS_COMP_CMD_STOP);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: MSYS_COMP_CMD_STOP failed \n", __FUNCTION__);
    }
    ULOG_VERB("%s: MSYS_COMP_CMD_STOP Success \n", __FUNCTION__);

    ret |= SendWaitSensorCmd(MSYS_COMP_CMD_DEINIT);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: MSYS_COMP_CMD_DEINIT failed \n", __FUNCTION__);
    }
    ULOG_VERB("%s: MSYS_COMP_CMD_DEINIT Success \n", __FUNCTION__);

    return ret;
}

/* SensorTestTask */
TASK(SensorTestTask)
{
    BCM_EventMaskType mask;
    int32_t retVal;
    uint32_t quit = 0UL;
    int32_t ret = BCM_ERR_OK;

    retVal = BCM_ActivateTask(CAMERA_CMD_TASK);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Command process start failed (%d)\n", __FUNCTION__, retVal);
        goto exit;
    }
    ret = WaitForCmdComplete();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: Failed on wait after activate (%d)\n", __FUNCTION__, retVal);
        goto exit;
    }
    ULOG_INFO("%s: Command process start Success \n", __FUNCTION__);

    while(0UL == quit) {
        retVal = BCM_WaitEvent(TEST_EVENT_START|TEST_EVENT_STOP);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: BCM_WaitEvent failed \n", __FUNCTION__);
        }

        retVal = BCM_GetEvent(SensorTestTask, &mask);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: BCM_GetEvent failed\n", __FUNCTION__);
        }

        if (0UL != (mask & TEST_EVENT_START)) {
            BCM_ClearEvent(TEST_EVENT_START);
            ret = SensorTest_Start();
            if (BCM_ERR_OK == ret){
                ULOG_INFO("%s: SensorTest_Start passed\n", __FUNCTION__);
            } else {
                ULOG_ERR("%s: SensorTest_Start failed\n", __FUNCTION__);
            }
        }

        if (0UL != (mask & TEST_EVENT_STOP)) {
            BCM_ClearEvent(TEST_EVENT_STOP);
            ret = SensorTest_Stop();
            if (BCM_ERR_OK == ret){
                ULOG_INFO("%s: SensorTest_Stop passed\n", __FUNCTION__);
            } else {
                ULOG_ERR("%s: SensorTest_Stop failed\n", __FUNCTION__);
            }
        }
    }

exit:
    /* Terminate CmdTask */
    ret |= SendWaitSensorCmd(COMP_QUIT);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("%s: COMP_QUIT failed \n", __FUNCTION__);
    }
    ULOG_INFO("%s: Terminate CmdTask Success \n", __FUNCTION__);

    retVal = BCM_TerminateTask();
    if(BCM_ERR_OK != retVal) {
        ULOG_ERR("BCM_TerminateTask Failed \n");
    }
}

/* Shell_TestFunc */
void Shell_TestFunc(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    BCM_EventMaskType mask = 0x0UL;
    int32_t ret;

    if (NULL != aArgString) {
        if (0 == strncmp(aArgString, "start", aArgStringLen)) {
            mask = TEST_EVENT_START;
            ret = BCM_SetEvent(SensorTestTask, mask);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
            }
        } else if (0 == strncmp(aArgString, "stop", aArgStringLen)) {
            mask = TEST_EVENT_STOP;
            ret = BCM_SetEvent(SensorTestTask, mask);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("%s: Failed to BCM_SetEvent \n", __FUNCTION__);
            }
        } else {
            CONSOLE_Write(aConsoleID, "Invalid argument. Usage: test start/stop\n");
        }
    } else {
        CONSOLE_Write(aConsoleID, "No argument mentioned. Usage: test start/stop\n");
    }
}

/* ApplicationInit */
void ApplicationInit()
{
    BCM_ActivateTask(SensorTestTask);
}
