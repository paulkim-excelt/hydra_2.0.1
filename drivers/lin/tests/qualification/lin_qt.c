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
#include "ee.h"
#include <lin.h>
#include <osil/lin_osil.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <ulog.h>
#include <string.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()         ULOG_LVL_ERROR
#define LIN_IT1                        (1UL)

#define LIN_HW_ID_0                    (1UL)

#define LIN_SCHEDULE_TBL_SZ            (6UL)
#define LIN_SCHEDULE_TBL_CYCLE_COUNT   (10UL)

#define LIN_TEST_WAIT_ALARM_EVENT      (LIN_Test_Wait_Alarm_Event)
#define LIN_SCHEDULE_TBL_ALARM_EVENT   (LIN_Schedule_Tbl_Alarm_Event)
#define LIN_STOP_SCHEDULE_TBL_EVENT    (LIN_Stop_Schedule_Tbl_Event)

typedef struct {
    LIN_FrmObjType * frmObj;
    uint32_t slotDelay;
}LIN_FrmSlotType;

static volatile int32_t LINIT1_Result = BCM_AT_NOT_STARTED;

static uint8_t testData1[] = { 0x11UL, 0x22UL, 0x33UL, 0x44UL, 0x55UL, 0x66UL, 0x77UL, 0x88UL };
static uint8_t testData2[] = { 0xDEUL, 0xADUL, 0xBEUL, 0xEFUL, 0xFEUL, 0xEDUL, 0xBEUL, 0xEBUL };
static uint8_t testData3[] = { 0x01UL, 0x10UL, 0x02UL, 0x20UL, 0x03UL, 0x30UL, 0x04UL, 0x40UL };
static uint8_t rxDataBuf[8];

static LIN_ConfigType Config1 = {
    .baudrate = LIN_BAUD_19200,
    .wakeUpCfg = LIN_WAKEUP_DETECT_ENABLED,
    .loopbackMode = LIN_UART_LOOPBACK_ENABLED,
};

static LIN_FrmObjType frm1 = {
    .frmId = 0xC1U,
    .frmResponse = LIN_FRM_TX,
    .frmDataLen = sizeof(testData1),
    .frmDataPtr = (uint8_t *)&testData1,
    .frmChecksumModel = LIN_CHECKSUM_CLASSIC,
};

static LIN_FrmObjType frm2 = {
    .frmId = 0x42U,
    .frmResponse = LIN_FRM_RX,
    .frmDataLen = 8UL,
    .frmDataPtr = NULL,
    .frmChecksumModel = LIN_CHECKSUM_ENHANCED,
};

static LIN_FrmObjType frm3 = {
    .frmId = 0x03U,
    .frmResponse = LIN_FRM_MON,
    .frmDataLen = 8UL,
    .frmDataPtr = NULL,
    .frmChecksumModel = LIN_CHECKSUM_CLASSIC,
};

static LIN_FrmObjType frm4 = {
    .frmId = 0xC4U,
    .frmResponse = LIN_FRM_TX,
    .frmDataLen = sizeof(testData2),
    .frmDataPtr = (uint8_t *)&testData2,
    .frmChecksumModel = LIN_CHECKSUM_CLASSIC,
};

static LIN_FrmObjType frm5 = {
    .frmId = 0x85U,
    .frmResponse = LIN_FRM_RX,
    .frmDataLen = 8UL,
    .frmDataPtr = NULL,
    .frmChecksumModel = LIN_CHECKSUM_CLASSIC,
};

static LIN_FrmObjType frm6 = {
    .frmId = 0x06U,
    .frmResponse = LIN_FRM_TX,
    .frmDataLen = sizeof(testData3),
    .frmDataPtr = (uint8_t *)&testData3,
    .frmChecksumModel = LIN_CHECKSUM_CLASSIC,
};

static LIN_FrmSlotType LIN_ScheduleTbl[LIN_SCHEDULE_TBL_SZ] =
{
    {
        .frmObj = &frm1,
        .slotDelay = 1000UL,
    },
    {
        .frmObj = &frm2,
        .slotDelay = 1500UL,
    },
    {
        .frmObj = &frm3,
        .slotDelay = 1500UL,
    },
    {
        .frmObj = &frm4,
        .slotDelay = 2000UL,
    },
    {
        .frmObj = &frm5,
        .slotDelay = 2000UL,
    },
    {
        .frmObj = &frm6,
        .slotDelay = 3000UL,
    }
};

TASK(LINIT1_Task)
{
    BCM_EventMaskType mask;
    uint32_t frmCounter = 0UL;
    int32_t status;

    ULOG_ERR("Starting LIN Schedule Table\n");
    LIN_Init(LIN_HW_ID_0, &Config1);

    while(frmCounter < LIN_SCHEDULE_TBL_SZ) {
        /* If loopback mode enabled, only send LIN Tx Frames */
        if ((LIN_UART_LOOPBACK_DISABLED == Config1.loopbackMode) ||
            ((LIN_UART_LOOPBACK_ENABLED == Config1.loopbackMode) &&
            (LIN_FRM_TX == LIN_ScheduleTbl[frmCounter].frmObj->frmResponse))) {

            LIN_TransmitFrame(LIN_HW_ID_0,
                    (const LIN_FrmObjType * const)LIN_ScheduleTbl[frmCounter].frmObj);

            do {
                status = LIN_ReadStatus(LIN_HW_ID_0);
            } while ((status == LIN_TX_BUSY) || (status == LIN_RX_BUSY));

            if ((LIN_TX_OK != status) && (LIN_RX_OK != status)) {
                ULOG_ERR("\nError in LIN Frame Tx/Rx for Frame Object %d", frmCounter);
                LINIT1_Result = BCM_ERR_UNKNOWN;
                goto err;
            }

            if (LIN_RX_OK == status) {
                LIN_GetData(LIN_HW_ID_0, rxDataBuf);
            }

            SetRelAlarm(LIN_Schedule_Tbl_Alarm, LIN_ScheduleTbl[frmCounter].slotDelay, 0);
            BCM_WaitEvent(LIN_SCHEDULE_TBL_ALARM_EVENT);
            BCM_GetEvent(LINIT1_Task, &mask);
            if ((mask & LIN_SCHEDULE_TBL_ALARM_EVENT) != 0UL) {
                BCM_ClearEvent(LIN_SCHEDULE_TBL_ALARM_EVENT);
            }
        }

        frmCounter++;
    }

err:
    LIN_DeInit(LIN_HW_ID_0);

    if (BCM_AT_EXECUTING == LINIT1_Result) {
        LINIT1_Result = BCM_ERR_OK;
    }

    BCM_TerminateTask();
}

void LIN_WakeUpNotification(LIN_HwIDType aId)
{
    /* Stub function */
}

void LIN_ScheduleTblAlarm_Cb()
{
    SetEvent(LINIT1_Task, LIN_SCHEDULE_TBL_ALARM_EVENT);
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;
    switch (aIndex) {
    case LIN_IT1:
        LINIT1_Result = BCM_AT_EXECUTING;
        BCM_ActivateTask(LINIT1_Task);
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

void ApplicationInit(void)
{
    /* Dummy Init */
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;
    switch(aIndex) {
    case LIN_IT1:
        retVal = LINIT1_Result;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}


