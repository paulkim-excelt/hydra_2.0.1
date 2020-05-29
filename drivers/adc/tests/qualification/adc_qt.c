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
#include <ulog.h>
#include <adc.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <chip_config.h>
#include <shell.h>
#include <osil/adc_osil.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel() ULOG_LVL_INFO

#define ADC_IT1             (1UL)
#define ADC_IT2             (2UL)

#define ADCIT1_GRP_ID       (0UL)
#define ADCIT2_GRP_ID       (9UL)

static int32_t ADCIT_Status = BCM_AT_NOT_STARTED;
static uint32_t ADCIT_TestIdx;

static uint32_t ADCIT_ResultArray[ADC_MAX_NUM_CHANN * 2UL];
static void ADCIT_GrpConvCb(ADC_ChanGrpIDType aGrpID);
static uint32_t ADCIT_CbCnt = 0UL;

static const ADC_ChannCfgType ADCIT_ChanCfgTbl[] = {
#if ADC_MAX_NUM_CHANN > 0UL
    {
        .channID = ADC_CHANNELID_0,
        .limitCheck = ADC_LIMITCHECK_ALWAYS,
        .limitLow = 0UL,
        .limitHigh = 0UL,
    },
#endif
#if ADC_MAX_NUM_CHANN > 1UL
    {
        .channID = ADC_CHANNELID_1,
        .limitCheck = ADC_LIMITCHECK_ALWAYS,
        .limitLow = 0UL,
        .limitHigh = 0UL,
    },
#endif
#if ADC_MAX_NUM_CHANN > 2UL
    {
        .channID = ADC_CHANNELID_2,
        .limitCheck = ADC_LIMITCHECK_ALWAYS,
        .limitLow = 0UL,
        .limitHigh = 0UL,
    },
#endif
#if ADC_MAX_NUM_CHANN > 3UL
#error "Test does not support more than 3 channels"
#endif
};

static const ADC_GrpCfgType ADCIT1_GrpCfgTbl[] = {
    {
        .grpID = ADCIT1_GRP_ID,
        .channCfgIdxList = {
#if ADC_MAX_NUM_CHANN > 0UL
            0UL,
#endif
#if ADC_MAX_NUM_CHANN > 1UL
            1UL,
#endif
#if ADC_MAX_NUM_CHANN > 2UL
            2UL,
#endif
        },
#if ADC_MAX_NUM_CHANN == 1UL
        .channCfgIdxListSz = 1UL,
#endif
#if ADC_MAX_NUM_CHANN == 2UL
        .channCfgIdxListSz = 2UL,
#endif
#if ADC_MAX_NUM_CHANN == 3UL
        .channCfgIdxListSz = 3UL,
#endif
        .callback = ADCIT_GrpConvCb,
    },
};

static const ADC_ConfigType ADCIT1_Cfg[ADC_MAX_HW_ID] = {
    {
        .hwID = 0UL,
        .channCfgList = ADCIT_ChanCfgTbl,
        .channCfgListSz = (sizeof(ADCIT_ChanCfgTbl)/sizeof(ADC_ChannCfgType)),
        .grpCfgList = ADCIT1_GrpCfgTbl,
        .grpCfgListSz = (sizeof(ADCIT1_GrpCfgTbl)/sizeof(ADC_GrpCfgType)),
    },
#if (ADC_MAX_HW_ID > 1)
#error "ADC_MAX_HW_ID > 1"
#endif
};


static const ADC_GrpCfgType ADCIT2_GrpCfgTbl[] = {
    {
        .grpID = ADCIT2_GRP_ID,
        .channCfgIdxList = {
#if ADC_MAX_NUM_CHANN > 0UL
            0UL,
#endif
#if ADC_MAX_NUM_CHANN > 1UL
            1UL,
#endif
#if ADC_MAX_NUM_CHANN > 2UL
            2UL,
#endif
        },
#if ADC_MAX_NUM_CHANN == 1UL
        .channCfgIdxListSz = 1UL,
#endif
#if ADC_MAX_NUM_CHANN == 2UL
        .channCfgIdxListSz = 2UL,
#endif
#if ADC_MAX_NUM_CHANN == 3UL
        .channCfgIdxListSz = 3UL,
#endif
        .callback = ADCIT_GrpConvCb,
    },
};

static const ADC_ConfigType ADCIT2_Cfg[ADC_MAX_HW_ID] = {
    {
        .hwID = 0UL,
        .channCfgList = ADCIT_ChanCfgTbl,
        .channCfgListSz = (sizeof(ADCIT_ChanCfgTbl)/sizeof(ADC_ChannCfgType)),
        .grpCfgList = ADCIT2_GrpCfgTbl,
        .grpCfgListSz = (sizeof(ADCIT2_GrpCfgTbl)/sizeof(ADC_GrpCfgType)),
    },
#if (ADC_MAX_HW_ID > 1)
#error "ADC_MAX_HW_ID > 1"
#endif
};

const ADC_ConfigType *ADC_Config;
const uint32_t ADC_ConfigSz = 1UL;

static void ADCIT_GrpConvCb(ADC_ChanGrpIDType aGrpID)
{
    if (ADCIT2_GRP_ID == aGrpID) {
        ADCIT_CbCnt++;
        (void)SetEvent(ADCIT_Task, ADCIT_Event);
    }
}

/* ADC test with notification */
static void ADC_TestIT2(void)
{
    int32_t retVal;
    BCM_EventMaskType mask;
    ADC_ChanGrpConvStateType state;
    uint32_t i;

    ADC_Config = ADCIT2_Cfg;

    ULOG_ERR("ADC_TestIT2++\n");
    ADC_Init();

    retVal = ADC_EnableGroupNotif(ADCIT2_GRP_ID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ADC_EnableGroupNotif Failed. retVal: %d\n", retVal);
        goto err;
    }

    retVal = ADC_StartGroupConv(ADCIT2_GRP_ID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ADC_StartGroupConv Failed. retVal: %d\n", retVal);
        goto err;
    }

    state = ADC_GetGroupConvState(ADCIT2_GRP_ID);
    if ((ADC_CHANGRPCONVSTATE_BUSY != state)
            && (ADC_CHANGRPCONVSTATE_AVAILABLE != state)) {
        ULOG_ERR("ADC_GetGroupConvState Failed. state[%lu]\n", state);
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    for (i = 0UL; i < 2UL; i++) {
        (void)BCM_WaitEvent(ADCIT_Event);
        (void)BCM_GetEvent(ADCIT_Task, &mask);
        if ((mask & ADCIT_Event) == ADCIT_Event) {
            (void)BCM_ClearEvent(ADCIT_Event);
        }
        retVal = ADC_ReadGroupSample(ADCIT2_GRP_ID, ADCIT_ResultArray);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("ADC_ReadGroupSample Failed\n");
            goto err;
        }
    }

    retVal = ADC_DisableGroupNotif(ADCIT2_GRP_ID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ADC_DisableGroupNotif Failed. retVal: %d\n", retVal);
        goto err;
    }

    retVal = ADC_StopGroupConv(ADCIT2_GRP_ID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ADC_StopGroupConv() Failed\n");
        goto err;
    }


err:
    ADC_DeInit();
    if (BCM_AT_EXECUTING == ADCIT_Status) {
        ADCIT_Status = retVal;
    }
    ULOG_ERR("ADC_TestIT2--\n");
}

/* ADC Test with polling method */
static void ADC_TestIT1(void)
{
    int32_t retVal;
    ADC_ChanGrpConvStateType state;

    ADC_Config = ADCIT1_Cfg;

    ULOG_ERR("ADC_TestIT1++\n");
    ADC_Init();

    retVal = ADC_StartGroupConv(ADCIT1_GRP_ID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ADC_StartGroupConv Failed\n");
        goto err;
    }

    state = ADC_GetGroupConvState(ADCIT1_GRP_ID);
    if ((ADC_CHANGRPCONVSTATE_BUSY != state)
            && (ADC_CHANGRPCONVSTATE_AVAILABLE != state)) {
        ULOG_ERR("ADC_GetGroupConvState Failed. state[%lu]\n", state);
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    do {
        state = ADC_GetGroupConvState(ADCIT1_GRP_ID);
    } while (ADC_CHANGRPCONVSTATE_AVAILABLE != state);

    retVal = ADC_ReadGroupSample(ADCIT1_GRP_ID, ADCIT_ResultArray);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ADC_ReadGroupSample Failed\n");
        ADC_DeInit();
        goto err;
    }

    retVal = ADC_StopGroupConv(ADCIT1_GRP_ID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("ADC_StopGroupConv() Failed\n");
        ADC_DeInit();
        goto err;
    }

    ADC_DeInit();

err:
    if (BCM_AT_EXECUTING == ADCIT_Status) {
        ADCIT_Status = retVal;
    }
    ULOG_ERR("ADC_TestIT1--\n");
}

TASK(ADCIT_Task)
{
    if (ADCIT_TestIdx == ADC_IT1) {
        ADC_TestIT1();
    } else if (ADCIT_TestIdx == ADC_IT2) {
        ADC_TestIT2();
    } else {
        ADCIT_Status = BCM_ERR_UNKNOWN;
    }
    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;
    StatusType retErika = E_OK;
    TaskStateType taskState = SUSPENDED;

    switch (aIndex) {
    case ADC_IT1:
    case ADC_IT2:
        retErika = GetTaskState(ADCIT_Task, &taskState);
        if (E_OK == retErika) {
            if (SUSPENDED == taskState) {
                ADCIT_Status = BCM_AT_EXECUTING;
                ADCIT_TestIdx = aIndex;
                retVal = BCM_ActivateTask(ADCIT_Task);
                if (BCM_ERR_OK != retVal) {
                    retVal = BCM_AT_NOT_STARTED;
                    ADCIT_Status = BCM_AT_NOT_STARTED;
                    ULOG_ERR("BCM_ActivateTask() failed!! status: %d\n\r", retVal);
                }
            } else {
                retVal = BCM_AT_EXECUTING;
                ULOG_ERR("ADCIT_Task is already running \n\r");
            }
        } else {
            retVal = BCM_ERR_UNKNOWN;
            ULOG_ERR("GetTaskState() failed status: %d\n\r", retErika);
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;

    switch (aIndex) {
    case ADC_IT1:
    case ADC_IT2:
        if (ADCIT_TestIdx == aIndex) {
            retVal = ADCIT_Status;
        } else {
            retVal = BCM_AT_NOT_AVAILABLE;
        }
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

