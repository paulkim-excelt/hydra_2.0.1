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
#include <avcd.h>
#include <bcm_test.h>
#include <atomic.h>
#include <avcd_lib.h>
#include <imgl.h>
#include <crc.h>
#include <cache.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>

extern uint32_t GetAPPLogLevel();
#define GetModuleLogLevel   GetAPPLogLevel
extern volatile int32_t avcd_qt1_result;
volatile int32_t avcd_qt1_result = BCM_AT_NOT_STARTED;

uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}

#define WIDTH              (1280UL)
#define HEIGHT             (720UL)
#define NUM_MBROWS         ((HEIGHT + 15UL) >> 4UL)
#define CRC32_POLY         (0x04C11DB7UL)
#define TEMP_MEM_DATA_SIZE (800*1024)

BCM_TimeType t1,t2,GStartTime,GEndTime;
static volatile int32_t avcd_qt2_result = BCM_AT_NOT_STARTED;
static volatile int32_t avcd_qt3_result = BCM_AT_NOT_STARTED;
static volatile int32_t avcdlib_qt_result = BCM_ERR_OK;

uint8_t COMP_SECTION(".dram.test.data") tempDataBuf[TEMP_MEM_DATA_SIZE];
uint8_t  COMP_SECTION(".dram.test.data") BsBufferLib[0x22000];
uint32_t COMP_SECTION(".dram.test.data") RefCRCLib[0x400];
uint32_t BsBufSizeLib;

uint8_t COMP_SECTION(".dram.test.data") BsBuffer[0x12000];
uint32_t BsBufSize;
uint32_t COMP_SECTION(".dram.test.data") slice_offset[45];
uint32_t maxNumSlices = 0;

uint32_t COMP_SECTION(".dram.test.data") RefCRC[0x400];

static uint8_t COMP_SECTION(".dram.test.data") outImage[WIDTH*HEIGHT*3];
#define CHECK_ERR(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("Error: %d \tfile: %s \t line %d\n",ret,file,line); avcdlib_qt_result = ret;}

int32_t FindResync(unsigned char* bs,uint32_t size)
{
    int32_t i;
    int32_t offset = -1;
    uint32_t zeroCount = 0;

    for(i = 4; i < size; i++) {
        if(zeroCount == 3 && bs[i] == 1) {
            offset = i-3;
            break;
        }
        if(bs[i] == 0) {
            zeroCount++;
        } else {
            zeroCount = 0;
        }
    }
    return offset;
}
//uint32_t slice_offset[11] = {13,8,25849,20576,8889,8199,16172,13965,21820,14425,6895};

#define MAX_NUM_STREAMS 4
BCM_TimeType GTimeDiff[MAX_NUM_STREAMS*NUM_MBROWS];
uint32_t GTotalTime;

uint32_t GNumStreams;
uint32_t GGetProfile;
volatile int32_t* GErr;
AVCDLib_Context context;
int32_t offset;
uint32_t currSize[4];
uint32_t currOffset[4];
AVCDLib_StreamInfo sliceInfo;
uint32_t mbRowNum[4];
uint32_t totalMB;

uint8_t COMP_SECTION(".dram.test.data") GPPBinary[0x7000UL];

int32_t avcd_lib_test(uint32_t aNumStreams,uint32_t aGetProfile)
{
    int32_t i;
    int32_t ret;

    AVCDLib_YUVBuffer yuvBuffer;
    AVCDLib_Config  config;
    AVCDLib_Flags flags;

    uint32_t numErr=0;

    uint32_t binSize = 0UL;
    IMGL_LoadStatusType imglStatus;
    int32_t ptmStatus;

    avcdlib_qt_result = BCM_ERR_OK;
    context.state = AVCD_LIB_STATE_RESET;

    ret = AVCDLib_Init(&context);
    CHECK_ERR(ret,__FILE__, __LINE__);

    config.width = WIDTH;
    config.height = HEIGHT;
    config.level = 51;
    config.profile = 66;
    config.numPPCores = 2;
    for(i=0;i<aNumStreams;i++) {
        ret = AVCDLib_Configure(&context,i,&config,TEMP_MEM_DATA_SIZE,aNumStreams);
    }
    CHECK_ERR(ret,__FILE__, __LINE__);


    /* Load pp binary */
     ret = IMGL_LoadImg((uint16_t)0xDE10, GPPBinary, 0UL,
                        0x7000, SystemEvent1, &imglStatus);
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else{
        binSize = imglStatus.size;
    }

    ret = AVCDLib_Boot(&context,
                    0x10,
                    GPPBinary,
                    binSize);
    CHECK_ERR(ret,__FILE__, __LINE__);

    for(i=0;i<aNumStreams;i++) {
        offset = FindResync(&BsBufferLib[0UL], BsBufSizeLib);
        currOffset[i] = 0UL;
        if (offset ==  -1) {
            currSize[i] = BsBufSizeLib;
        } else {
            currSize[i] = offset;
        }
        mbRowNum[i] = 0;
    }

    totalMB = 0;

    BCM_GetTime(&t1);
    while (totalMB < (aNumStreams*NUM_MBROWS))  {
        flags = AVCDLib_Process(&context,tempDataBuf);
        if(flags != 0) {
            for(i=0;i<aNumStreams;i++) {
                if(flags&(AVCD_LIB_ERROR_DETECTED<<i)) {
                    numErr++;
                }
                if(flags&(AVCD_LIB_SLICE_PROCESSED<<i)) {
                    /* Slice Processed */
                }
                if(flags&(AVCD_LIB_FILL_BUFFER_DONE<<i)) {
                    BCM_GetTime(&GEndTime);
                    GTimeDiff[(i*NUM_MBROWS)+mbRowNum[i]] = BCM_GetTimeAbsDiff(GStartTime,GEndTime);
                    mbRowNum[i]++;
                    totalMB++;
                    if(mbRowNum[i] == NUM_MBROWS) {
                        mbRowNum[i] = 0;
                    }
                }
            }
        }
        for(i = 0; i < aNumStreams; i++) {
            sliceInfo.sliceType = AVCD_UNKNOWN_SLICE;
            AVCDLib_GetStreamInfo(&context, i, &BsBufferLib[currOffset[i]], currSize[i], &sliceInfo);
            if (sliceInfo.sliceType == AVCD_VIDEO_SLICE) {
                ret = AVCDLib_DecodeSlice(&context, i, &BsBufferLib[currOffset[i]], currSize[i],
                                            &sliceInfo, tempDataBuf);
                if(BCM_ERR_OK == ret) {
                    currOffset[i] += currSize[i];
                    offset = FindResync(&BsBufferLib[currOffset[i]], BsBufSizeLib - currOffset[i]);
                    if (offset ==  -1) {
                        currSize[i] = BsBufSizeLib - currOffset[i];
                    } else {
                        currSize[i] = offset;
                    }
                }
            } else {
                currOffset[i] += currSize[i];
                offset = FindResync(&BsBufferLib[currOffset[i]], BsBufSizeLib - currOffset[i]);
                if (offset ==  -1) {
                    currSize[i] = BsBufSizeLib - currOffset[i];
                } else {
                    currSize[i] = offset;
                }

            }
            if(1UL == context.streamContext[i].fillBufReq) {
                yuvBuffer.yStride = WIDTH;
                yuvBuffer.cStride = (WIDTH >> 1);
                yuvBuffer.format = 0;
                yuvBuffer.width = WIDTH;
                yuvBuffer.height = HEIGHT;
                yuvBuffer.yAddr = outImage + (mbRowNum[i]*16*WIDTH);
                yuvBuffer.uAddr = outImage + (WIDTH*HEIGHT) + (mbRowNum[i]*8*(WIDTH >> 1));
                yuvBuffer.vAddr = outImage + (WIDTH*HEIGHT)+((WIDTH >> 1)*(HEIGHT >> 1)) + (mbRowNum[i]*8*(WIDTH >> 1));
                ret = AVCDLib_FillBuffer(&context, i, tempDataBuf, &yuvBuffer);
                if(ret == BCM_ERR_OK) {
                    BCM_GetTime(&GStartTime);
                }
            }
        }
    }

    BCM_GetTime(&t2);

    t1 = BCM_GetTimeAbsDiff(t1,t2);

    ULOG_ERR("Error Count %d\n",numErr);
    ULOG_ERR("total mbRows %d Time (%ds, %dns) \n",totalMB,t1.s,t1.ns);
    if(0UL != aGetProfile) {
        GTotalTime = 0;
        for(i=0;i<(aNumStreams*NUM_MBROWS);i++){
            ULOG_ERR("%d MB Row Time (%ds, %dns)\n",i,GTimeDiff[i].s,GTimeDiff[i].ns);
            GTotalTime += GTimeDiff[i].ns;
        }
        ULOG_ERR("Average time per MB row %dns\n",GTotalTime/(aNumStreams*NUM_MBROWS));
    }
    if(BCM_ERR_OK == avcdlib_qt_result) {
        DCACHE_InvalRange((uint32_t)outImage,(WIDTH*HEIGHT*3/2));
        if (RefCRCLib[0UL] != BCM_CRC32(outImage,(WIDTH*HEIGHT*3)/2,CRC32_POLY)) {
            ULOG_ERR("Output Image CRC Check failed \n");
            avcdlib_qt_result = BCM_ERR_DATA_INTEG;
        }
    }

    AVCDLib_DeInit(&context);
    ULOG_INFO("**********%s completed ***********\n",__func__);
    return avcdlib_qt_result;
}

/* Macros */

const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 3UL,

    .component[0].compID = MSYS_COMP_ID_EX_SRC,
    .component[0].numConfigs = 1UL,
    .component[0].config[0] = 0UL,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_AVCD,
    .component[1].numConfigs = 2UL,
    .component[1].config[0] = MSYS_CONFIG_AVCD_MOD,
    .component[1].config[1] = MSYS_CONFIG_AVCD_FW0,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 1U,
    .component[1].portMap[2].srcCompIdx = 0U,
    .component[1].portMap[2].srcPortIdx = 2U,
    .component[1].portMap[3].srcCompIdx = 0U,
    .component[1].portMap[3].srcPortIdx = 3U,

    .component[2].compID = MSYS_COMP_ID_EX_SNK,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = 0UL,
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,
    .component[2].portMap[1].srcCompIdx = 1U,
    .component[2].portMap[1].srcPortIdx = 1U,
    .component[2].portMap[2].srcCompIdx = 1U,
    .component[2].portMap[2].srcPortIdx = 2U,
    .component[2].portMap[3].srcCompIdx = 1U,
    .component[2].portMap[3].srcPortIdx = 3U,
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

void AVCDTest_Start(void)
{
    int32_t ret;

    ret = MSYS_Init(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avcd_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        avcd_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        avcd_qt1_result = ret;
    }

err:
    return;
}

void AVCDTest_Stop(void)
{
    int32_t ret;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avcd_qt1_result = ret;
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        avcd_qt1_result = ret;
    }

    if (BCM_AT_EXECUTING == avcd_qt1_result) {
        avcd_qt1_result = BCM_ERR_OK;
    }
err:
    return;
}
volatile uint32_t TestAlarmVal = 0UL;
uint32_t gAVCDTestCount = 0UL;
void Test_AVCDCompAlarm_cb()
{
    if(1UL == TestAlarmVal)
    {
        if(++gAVCDTestCount == 2)
        {
            ATOMIC_Set(&TestAlarmVal, 0UL);
            SetEvent(Test_AVCDComp,SystemEvent0);
        }
    }
}
uint32_t GTestID = 0;
TASK(Test_AVCDComp)
{
    if (1UL == GTestID) {
        avcd_qt1_result = BCM_AT_EXECUTING;
        AVCDTest_Start();
        gAVCDTestCount = 0UL;
        ATOMIC_Set(&TestAlarmVal, 1UL);
        BCM_WaitEvent(SystemEvent0);
        AVCDTest_Stop();
    }
    if (2UL == GTestID) {
        /* single instance */
        avcd_qt2_result = BCM_AT_EXECUTING;
        avcd_qt2_result = avcd_lib_test(1UL,1UL);

    }
    if (3UL == GTestID) {
        /* multi instance */
        avcd_qt3_result = BCM_AT_EXECUTING;
        avcd_qt3_result = avcd_lib_test(4UL,0UL);
    }
    GTestID = 0UL;
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if ((4UL > aIndex) && (0UL != aIndex)) {
        if (0UL == GTestID) {
            GTestID = aIndex;
            BCM_ActivateTask(Test_AVCDComp);
            ULOG_INFO("\n Test started");
            ret = BCM_ERR_OK;
        } else {
            ret = BCM_AT_NOT_STARTED;
        }
    }
    return ret;
}
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = avcd_qt1_result;
    }
    if (2UL == aIndex) {
        ret = avcd_qt2_result;
    }
    if (3UL == aIndex) {
        ret = avcd_qt3_result;
    }
    return ret;
}

void ApplicationInit()
{
    int32_t ret;
    uint32_t temp = 0UL;
    IMGL_LoadStatusType imglStatus;
    int32_t ptmStatus;

    /* load the memories */
    /* load lib input */
    ret = IMGL_LoadImg((uint16_t)0XFF03, BsBufferLib, 0UL,
                        0x22000, SystemEvent1, &imglStatus);
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        BsBufSizeLib = imglStatus.size;
    }

    /* load lib sha reference */
    ret = IMGL_LoadImg((uint16_t)0XFF01, (uint8_t*)RefCRCLib, 0UL,
                        0x1000, SystemEvent1, &imglStatus);
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        temp = imglStatus.size;
    }

    if (4UL != temp) {
        ULOG_ERR("\n PTM Load Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    }
    /* load comp input */
    ret = IMGL_LoadImg((uint16_t)0XFF02, BsBuffer, 0UL,
                        0x12000, SystemEvent1, &imglStatus);
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        BsBufSize = imglStatus.size;
    }

    /* load comp sha reference */
    ret = IMGL_LoadImg((uint16_t)0XFF00, (uint8_t*)RefCRC, 0UL,
                        0x1000, SystemEvent1, &imglStatus);
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        temp = imglStatus.size;
    }

    if ((45UL*4UL) != temp) {
        ULOG_ERR("\n PTM Load Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    }

    /* update the sliceoffsets */
    {
        uint8_t* bs;
        uint32_t offset;
        uint32_t size;

        bs = BsBuffer;
        size = BsBufSize;
        maxNumSlices = 0;
        do{
            offset = FindResync(bs,size);
            slice_offset[maxNumSlices++] = offset;
            bs += offset;
            size -= offset;
        }while(offset != -1);
        /* compensate the last offset */
        slice_offset[maxNumSlices - 1] = size + offset;
    }

    BCM_ActivateTask(ExSrc_CmdProcessTask);
    BCM_ActivateTask(ExSnk_CmdProcessTask);
}
