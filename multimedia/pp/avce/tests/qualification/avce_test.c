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
#include <bcm_err.h>
#include <bcm_test.h>
#include <avce.h>
#include <atomic.h>

#include <bcm_time.h>
#include <crc.h>
#include <imgl.h>
#include <avce_lib.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>

extern uint32_t GetAPPLogLevel();
#define GetModuleLogLevel   GetAPPLogLevel
#define CMD_STR_LEN         (5UL)

extern volatile int32_t avce_qt1_result;
volatile int32_t avce_qt1_result = BCM_AT_NOT_STARTED;
extern volatile int32_t avce_qt2_result;
volatile int32_t avce_qt2_result = BCM_AT_NOT_STARTED;
extern int32_t ExSnk_CheckDataValidity();

uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}

uint8_t COMP_SECTION(".pp.uncached.test") inImage[1280*16*3/2];
uint8_t COMP_SECTION(".pp.uncached.test") bsBuffer[16*1024];
uint8_t COMP_SECTION(".pp.uncached.test") tempBsBuffer[16*1024] COMP_ALIGN(32);

uint8_t COMP_SECTION(".pp.uncached.test") libIntBuf[AVCE_LIB_INTERNAL_USE_BUF_SIZE] COMP_ALIGN(32);

uint8_t COMP_SECTION(".pp.uncached.test") GPPBinary[0x6000UL];
#define CHECK_ERR(ret,file,line)  \
    if ((BCM_ERR_OK != ret) && (BCM_ERR_EAGAIN != ret)){ \
        ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line);\
        avce_qt2_result = ret;\
        }

uint32_t CRCValue;
#ifdef AVCE_ENABLE_LOW_COMPLEXITY
static uint32_t CRCRefAvceLib= 0xba0d4ca1UL;
#else
static uint32_t CRCRefAvceLib= 0x72d3d1f7UL;
#endif

#define CRC32_POLY             (0x04C11DB7UL)

BCM_TimeType GStartTime[45];
BCM_TimeType GEndTime[45];
BCM_TimeType GTimeDiff[45];
uint32_t GStartCount;
uint32_t GEndCount;
uint32_t GTotalTime;

AVCELib_Context context;
void AVCELibTest()
{
    uint32_t i;
    int32_t ret;

    AVCELib_YUVBuffer yuvBuffer;
    AVCELib_Config  config;
    BCM_EventMaskType mask;
    uint32_t flags = 0;
    uint32_t filledSize;
    uint32_t totalSize;
    uint32_t sliceNum;
    uint8_t loCmplxMode=0;
    uint32_t binSize = 0;
    IMGL_LoadStatusType imglStatus;
    int32_t ptmStatus;


    totalSize = 0;

    GStartCount = 0;
    GEndCount = 0;

#ifdef AVCE_ENABLE_LOW_COMPLEXITY
    loCmplxMode = 1;
#endif

    context.state = AVCE_LIB_STATE_RESET;
    ret = AVCELib_Init(&context);
    CHECK_ERR(ret,__FILE__, __LINE__);

    config.width = 1280;
    config.height = 720;
    config.fps = 30;
    config.bitrate = 30000000;
    config.level = 51;
    config.profile = 66;
    config.cropLeft = 0;
    config.cropRight = 0;
    config.cropTop = 0;
    config.cropBottom = 0;
    config.sliceCfg[0] = 0;
    config.sliceCfg[1] = 5;
    config.sliceCfg[2] = 10;
    config.sliceCfg[3] = 15;
    config.sliceCfg[4] = 20;
    config.sliceCfg[5] = 25;
    config.sliceCfg[6] = 30;
    config.sliceCfg[7] = 35;
    config.sliceCfg[8] = 40;
    config.sliceCfg[9] = 0;
    for (i=10;i<128;i++) {
        config.sliceCfg[i] = 0;
    }
    config.numPPCores = 1;
    config.minQP = 12;
    config.maxQP = 44;

    /* load the binary */
    ret = AVCELib_Configure(&context,&config,loCmplxMode,2UL);
    CHECK_ERR(ret,__FILE__, __LINE__);

    /* Load pp binary */
     ret = IMGL_LoadImg((uint16_t)0xEE10, GPPBinary, 0UL,
                        0x6000UL, SystemEvent1, &imglStatus);
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        binSize = imglStatus.size;
    }
    ret = AVCELib_Boot(&context,
                       0xEE10,
                       GPPBinary,
                       binSize);
    CHECK_ERR(ret,__FILE__, __LINE__);

    yuvBuffer.lumaStride = 1280;
    yuvBuffer.chromaStride = 640;
    yuvBuffer.format = 0;
    yuvBuffer.frameNo = 0;
    yuvBuffer.mbRow = 0;
#if AVCE_YUV_SEMIPLANAR_FORMAT
    yuvBuffer.format = 1;
#else
    yuvBuffer.format = 0;
#endif

    memset(inImage,0,1280*16*3/2);

    for (sliceNum = 0; sliceNum < 1; sliceNum++) {
        yuvBuffer.lumaAddr = &inImage[0];
        yuvBuffer.cbAddr = &inImage[1280*16];
        yuvBuffer.crAddr = &inImage[(1280*16) + (640*8)];

        ULOG_INFO("Slice-%d\n",sliceNum);
        yuvBuffer.mbRow = sliceNum*5;
        ret = AVCELib_Encode(&context, &yuvBuffer, 1UL,libIntBuf);
        CHECK_ERR(ret,__FILE__, __LINE__);
        if (BCM_ERR_OK == ret) {
            BCM_GetTime(&GStartTime[GStartCount++]);
            yuvBuffer.mbRow++;
        }
        while ((context.state == AVCE_LIB_STATE_RUNNING) || (yuvBuffer.mbRow < 5)) {
            if (yuvBuffer.mbRow < 5) {
                ret = AVCELib_Encode(&context, &yuvBuffer, 1UL,libIntBuf);
                CHECK_ERR(ret,__FILE__, __LINE__);
                if (BCM_ERR_OK == ret) {
                    BCM_GetTime(&GStartTime[GStartCount++]);
                    yuvBuffer.mbRow++;
                }
            }
            BCM_WaitEvent(PP_GPEvent);
            BCM_GetEvent(Test_AVCEComp,&mask);
            BCM_ClearEvent(mask);
            flags = AVCELib_Process(&context);
            ULOG_INFO("AVCELib_Process: %x\n", flags);
            if (flags & AVCE_LIB_MBROW_PROCESSED) {
                BCM_GetTime(&GEndTime[GEndCount++]);

            }
            if(flags&AVCE_LIB_BITSTREAM_FILLED) {
                /* get filled size */
                filledSize = 0UL;
                ret = AVCELib_GetBitStreamSize(&context, &filledSize);
                BCM_MemCpy(bsBuffer + totalSize, tempBsBuffer, filledSize);
                ULOG_INFO("Bitstream %d\n",filledSize);
                totalSize += filledSize;
            }
            if (flags & AVCE_LIB_BITSTREAM_BUFFER) {
                ret = AVCELib_FillBitStream(&context, tempBsBuffer, 16*1024);
                CHECK_ERR(ret,__FILE__, __LINE__);
            }
        }

    }

    for (i=0;i<GStartCount;i++)
    {
        ULOG_ERR("GStartTime: (%ds, %dns)\n",GStartTime[i].s,GStartTime[i].ns);
    }

    for (i=0;i<GEndCount;i++)
    {
        ULOG_ERR("GEndTime: (%ds, %dns)\n",GEndTime[i].s,GEndTime[i].ns);
    }
    for (i=0;i<GEndCount;i++)
    {
        GTimeDiff[i] = BCM_GetTimeAbsDiff(GEndTime[i],GStartTime[i]);
    }
    GTotalTime = 0;
    for (i=0;i<GEndCount;i++)
    {
        ULOG_ERR("%d MBrow Time: (%ds, %dns)\n",i,GTimeDiff[i].s,GTimeDiff[i].ns);
        GTotalTime += GTimeDiff[i].ns;
    }
    ULOG_ERR("Avg per MB row : %dns\n",GTotalTime/GEndCount);


    ULOG_INFO("Slice is Completed\n");
    AVCELib_DeInit(&context);
    CRCValue = BCM_CRC32(bsBuffer,totalSize,CRC32_POLY);
    if (CRCValue != CRCRefAvceLib) {
        ULOG_ERR("Output CRC Check failed %08x\n",CRCValue);
        avce_qt2_result = BCM_ERR_DATA_INTEG;
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

    .component[1].compID = MSYS_COMP_ID_AVCE,
    .component[1].numConfigs = 2UL,
    .component[1].config[0] = MSYS_CONFIG_AVCE_MOD,
    .component[1].config[1] = MSYS_CONFIG_AVCE_FW0,
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

void AVCETest_Start(void)
{
    int32_t ret;

    ret = MSYS_Init(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avce_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        avce_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        avce_qt1_result = ret;
    }

err:
    return;
}

void AVCETest_Stop(void)
{
    int32_t ret;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avce_qt1_result = ret;
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avce_qt1_result = ret;
    }

err:
    return;
}
volatile uint32_t TestAlarmVal = 0UL;
uint32_t gAVCETestCount = 0UL;
void Test_AVCECompAlarm_cb()
{
    if(1UL == TestAlarmVal)
    {
        if(++gAVCETestCount == 2)
        {
            ATOMIC_Set(&TestAlarmVal, 0UL);
            SetEvent(Test_AVCEComp,SystemEvent0);
        }
    }
}
uint32_t GTestID = 0UL;
TASK(Test_AVCEComp)
{
    if (1UL == GTestID) {
        AVCETest_Start();
        gAVCETestCount = 0UL;
        ATOMIC_Set(&TestAlarmVal, 1UL);
        BCM_WaitEvent(SystemEvent0);
        AVCETest_Stop();
        if (BCM_AT_EXECUTING == avce_qt1_result) {
            avce_qt1_result = ExSnk_CheckDataValidity();
        }
    }
    if (2UL == GTestID) {
        AVCELibTest();
        if (BCM_AT_EXECUTING == avce_qt2_result) {
            avce_qt2_result = BCM_ERR_OK;
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
        avce_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test_AVCEComp);
        ret = BCM_ERR_OK;
    }
    if (2UL == aIndex) {
        GTestID = 2UL;
        avce_qt2_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test_AVCEComp);
        ret = BCM_ERR_OK;
    }
    return ret;
}
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = avce_qt1_result;
    }
    if (2UL == aIndex) {
        ret = avce_qt2_result;
    }
    return ret;
}

void ApplicationInit()
{
    BCM_ActivateTask(ExSrc_CmdProcessTask);
    BCM_ActivateTask(ExSnk_CmdProcessTask);
}
