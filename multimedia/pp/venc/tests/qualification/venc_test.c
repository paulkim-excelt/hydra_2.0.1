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

/******************************************************************************
 File Name: venc_test.c
 Descritpion: This file implememnts test code for video encoder
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
#include <bcm_err.h>
#include <bcm_test.h>
#include <venc.h>
#include <atomic.h>

#include <bcm_time.h>
#include <crc.h>
#include <imgl.h>
#include <venc_lib.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>

extern uint32_t GetAPPLogLevel();
#define GetModuleLogLevel   GetAPPLogLevel
#define CMD_STR_LEN         (5UL)

extern volatile int32_t venc_qt1_result;
volatile int32_t venc_qt1_result = BCM_AT_NOT_STARTED;
extern volatile int32_t venc_qt2_result;
volatile int32_t venc_qt2_result = BCM_AT_NOT_STARTED;
extern int32_t ExSnk_CheckDataValidity();

uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}

#ifdef VENC_ENABLE_P_FRAME
#define BS_BUFFER_SIZE     (8UL*1024UL*1024UL)
#define NUM_TEST_MB_ROWS   (180UL)

uint8_t COMP_SECTION(".pp.slowmem.test") GTempMem[(1024) + (128*1024) + (((1280 * 720 * 3) / 2) * 2) + (1280*720*3)];
uint8_t COMP_SECTION(".pp.slowmem.test") bsBuffer[BS_BUFFER_SIZE];
uint8_t COMP_SECTION(".pp.slowmem.test") tempBsBuffer[BS_BUFFER_SIZE];
static uint32_t CRCRefVencLib = 0xfbd98f98UL;
#else
#define BS_BUFFER_SIZE     (8UL*1024UL)
#define NUM_TEST_MB_ROWS   (45UL)

uint8_t COMP_SECTION(".pp.uncached.test") GTempMem[(1024) + (128*1024) + (((1280 * 16 * 3) / 2) * 4)];
uint8_t COMP_SECTION(".pp.uncached.test") bsBuffer[BS_BUFFER_SIZE];
uint8_t COMP_SECTION(".pp.uncached.test") tempBsBuffer[BS_BUFFER_SIZE];
static uint32_t CRCRefVencLib = 0x2d8f4e03UL;
#endif

uint8_t COMP_SECTION(".pp.uncached.test") inImage[1280*16*3/2];

BCM_TimeType GStartTime[NUM_TEST_MB_ROWS];
BCM_TimeType GEndTime[NUM_TEST_MB_ROWS];
BCM_TimeType GTimeDiff[NUM_TEST_MB_ROWS];
uint32_t GStartCount;
uint32_t GEndCount;
uint32_t GTotalTime;

VENCLib_Context context;

uint32_t CRCValue;

#define CHECK_ERR(ret,file,line)  \
    if ((BCM_ERR_OK != ret) && (BCM_ERR_EAGAIN != ret)){ \
        ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line);\
        venc_qt2_result = ret;\
        }

#define CRC32_POLY             (0x04C11DB7UL)

int32_t ProcessedMBs = 0;
uint32_t totalInputMBs = 0;
void VENCLibTest()
{
    uint32_t i;
    int32_t ret;

    VENCLib_YUVBuffer yuvBuffer;
    VENCLib_Config  config;
    VENCLib_BitStream bsInfo;
    BCM_EventMaskType mask;
    uint32_t flags = 0;
    uint32_t totalSize;
    uint32_t binSize = 0;
    IMGL_LoadStatusType imglStatus;
    int32_t ptmStatus;
    uint8_t isContinue;
    uint8_t* tempBuffer;

    tempBuffer = GTempMem;

    totalSize = 0;

    GStartCount = 0;
    GEndCount = 0;

#ifdef VENC_ENABLE_LOW_COMPLEXITY
    loCmplxMode = 1;
#endif

    context.state = VENC_LIB_STATE_RESET;
    ret = VENCLib_Init(&context);
    CHECK_ERR(ret,__FILE__, __LINE__);

#ifdef VENC_ENABLE_P_FRAME
    config.profile = VENC_PROFILE_IP;
    config.intraPeriod = 12;
#else
    config.profile = VENC_PROFILE_I_ONLY;
    config.intraPeriod = 1;
#endif

    config.magic = 0UL;
    config.version = 0UL;
    config.width = 1280;
    config.height = 720;
    config.entropyCoding = VENC_ENTROPY_CABAC;
    config.fps = 30;
    config.bitrate = 0;
    config.quality = 25;
    config.hdrRefreshPeriod = 30;
    config.numPPCores = 1;

    bsInfo.bsBuffer = bsBuffer;
    ret = VENCLib_Configure(&context,&config,0UL,tempBuffer,bsBuffer,BS_BUFFER_SIZE);
    CHECK_ERR(ret,__FILE__, __LINE__);

#ifdef VENC_ENABLE_P_FRAME
    /* Load pp binary */
    ret = IMGL_LoadImg((uint16_t)MSYS_CONFIG_VENC_FW0 + 1, tempBuffer+1024, 0UL,
                        0x10000UL, SystemEvent1, &imglStatus);
#else
    ret = IMGL_LoadImg((uint16_t)MSYS_CONFIG_VENC_FW0, tempBuffer+1024, 0UL,
                        0x10000UL, SystemEvent1, &imglStatus);
#endif
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        binSize = imglStatus.size;
    }
    ret = VENCLib_Boot(&context,
                       VENC_LIB_FW0_ID,
                       tempBuffer+1024,
                       binSize,
                       tempBuffer);
    CHECK_ERR(ret,__FILE__, __LINE__);


#ifdef VENC_ENABLE_P_FRAME
    /* Load pp binary */
    ret = IMGL_LoadImg((uint16_t)MSYS_CONFIG_VENC_FW1 + 1, tempBuffer+1024, 0UL,
                        0x10000UL, SystemEvent1, &imglStatus);
#else
    ret = IMGL_LoadImg((uint16_t)MSYS_CONFIG_VENC_FW1, tempBuffer+1024, 0UL,
                        0x10000UL, SystemEvent1, &imglStatus);
#endif
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        binSize = imglStatus.size;
    }
    ret = VENCLib_Boot(&context,
                       VENC_LIB_FW1_ID,
                       tempBuffer+1024,
                       binSize,
                       tempBuffer);
    CHECK_ERR(ret,__FILE__, __LINE__);

    yuvBuffer.lumaStride = 1280;
    yuvBuffer.chromaStride = 640;
    yuvBuffer.frameNo = 0;
    yuvBuffer.mbRow = 0;

    memset(inImage,0,1280*16*3/2);

#ifndef VENC_ENABLE_P_FRAME
    BCM_CpuNDelay(2000*1000);
#endif

    isContinue = 1;
    ProcessedMBs = 0;
    totalInputMBs = 0;
    while(1 == isContinue) {
        yuvBuffer.lumaAddr = &inImage[0];
        yuvBuffer.cbAddr = &inImage[1280*16];
        yuvBuffer.crAddr = &inImage[(1280*16) + (640*8)];
        if(totalInputMBs < NUM_TEST_MB_ROWS) {
            ret = VENCLib_Encode(&context, &yuvBuffer, tempBuffer);
            if (BCM_ERR_OK == ret) {
                BCM_GetTime(&GStartTime[GStartCount++]);
                yuvBuffer.mbRow++;
                if (45UL == yuvBuffer.mbRow) {
                    yuvBuffer.mbRow  = 0;
                }
                totalInputMBs++;
            } else if (BCM_ERR_EAGAIN != ret) {
                CHECK_ERR(ret,__FILE__, __LINE__);
            }
        }

        BCM_WaitEvent(PP_GPEvent);
        BCM_GetEvent(Test_VENCComp,&mask);
        BCM_ClearEvent(mask);

        do {
            flags = VENCLib_Process(&context, tempBuffer, &bsInfo);
            if (0UL != (flags & VENC_LIB_MBROW_PROCESSED)) {
                BCM_GetTime(&GEndTime[GEndCount++]);
            }
            if (0UL != (flags & VENC_LIB_BITSTREAM_FILLED)) {
                /* get filled size */
                ULOG_ERR("Bitstream size %d\n",bsInfo.size);
                BCM_MemCpy(tempBsBuffer + totalSize,bsBuffer+bsInfo.offset , bsInfo.size);
                totalSize += bsInfo.size;
                if (bsInfo.nalType == VENC_NALTYPE_VCL) {
#ifdef VENC_ENABLE_P_FRAME
                    ProcessedMBs += 45UL;
#else
                    ProcessedMBs += 2UL;
#endif
                }

                if (NUM_TEST_MB_ROWS <= ProcessedMBs) {
                    isContinue = 0UL;
                }
            }
        } while (flags != 0);
    }

    for (i=0;i<GEndCount;i++)
    {
        GTimeDiff[i] = BCM_GetTimeAbsDiff(GEndTime[i],GStartTime[i]);
    }
    GTotalTime = 0;
    for (i=0;i<GEndCount;i++)
    {
        GTotalTime += GTimeDiff[i].ns;
    }
    ULOG_ERR("Avg per MB row : %dns\n",GTotalTime/GEndCount);

    VENCLib_DeInit(&context);
    CRCValue = BCM_CRC32(tempBsBuffer,totalSize,CRC32_POLY);
    if (CRCValue != CRCRefVencLib) {
        ULOG_ERR("Output CRC Check failed %08x\n",CRCValue);
        venc_qt2_result = BCM_ERR_DATA_INTEG;
    }
    ULOG_INFO("Filled Size %x\n",totalSize);

    ULOG_INFO("**********%s completed ***********\n",__func__);

}


const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 3UL,

    .component[0].compID = MSYS_COMP_ID_EX_SRC,
    .component[0].numConfigs = 1UL,
    .component[0].config[0] = 0UL,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_VENC,
    .component[1].numConfigs = 3UL,
#ifdef VENC_ENABLE_P_FRAME
    .component[1].config[0] = MSYS_CONFIG_VENC_MOD + 1,
    .component[1].config[1] = MSYS_CONFIG_VENC_FW0 + 1,
    .component[1].config[2] = MSYS_CONFIG_VENC_FW1 + 1,
#else
    .component[1].config[0] = MSYS_CONFIG_VENC_MOD,
    .component[1].config[1] = MSYS_CONFIG_VENC_FW0,
    .component[1].config[2] = MSYS_CONFIG_VENC_FW1,
#endif
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,

    .component[2].compID = MSYS_COMP_ID_EX_SNK,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = 0UL,
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,
};

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

void VENCTest_Start(void)
{
    int32_t ret;

    ret = MSYS_Init(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        venc_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        venc_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        venc_qt1_result = ret;
    }

err:
    return;
}

void VENCTest_Stop(void)
{
    int32_t ret;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        venc_qt1_result = ret;
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        venc_qt1_result = ret;
    }

err:
    return;
}
volatile uint32_t TestAlarmVal = 0UL;
uint32_t gVENCTestCount = 0UL;
void Test_VENCCompAlarm_cb()
{
    if(1UL == TestAlarmVal)
    {
        if(++gVENCTestCount == 2)
        {
            ATOMIC_Set(&TestAlarmVal, 0UL);
            SetEvent(Test_VENCComp,SystemEvent0);
        }
    }
}
uint32_t GTestID = 0UL;
TASK(Test_VENCComp)
{
    if (1UL == GTestID) {
        VENCTest_Start();
        gVENCTestCount = 0UL;
        ATOMIC_Set(&TestAlarmVal, 1UL);
        BCM_WaitEvent(SystemEvent0);
        VENCTest_Stop();
        if (BCM_AT_EXECUTING == venc_qt1_result) {
            venc_qt1_result = ExSnk_CheckDataValidity();
        }
    }
    if (2UL == GTestID) {
        VENCLibTest();
        if (BCM_AT_EXECUTING == venc_qt2_result) {
            venc_qt2_result = BCM_ERR_OK;
        }
    }
    GTestID = 0UL;
    BCM_TerminateTask();

}
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if ((0UL != GTestID) && (2UL >= aIndex) && (0UL != aIndex)) {
        return BCM_AT_EXECUTING;
    }

    if (1UL == aIndex) {
        GTestID = 1UL;
        venc_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test_VENCComp);
        ret = BCM_ERR_OK;
    }
    if (2UL == aIndex) {
        GTestID = 2UL;
        venc_qt2_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test_VENCComp);
        ret = BCM_ERR_OK;
    }
    return ret;
}
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = venc_qt1_result;
    }
    if (2UL == aIndex) {
        ret = venc_qt2_result;
    }
    return ret;
}

void ApplicationInit()
{
    BCM_ActivateTask(ExSrc_CmdProcessTask);
    BCM_ActivateTask(ExSnk_CmdProcessTask);
}
