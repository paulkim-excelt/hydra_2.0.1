/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

/**
    @defgroup grp_vdec_qt Qualification Tests
    @ingroup grp_vdec

    @addtogroup grp_vdec_qt
    @{

    @file vdec_qt.c
    @brief VDEC Qualification Test

    @version 0.1 Initial Version
*/
#include <stddef.h>
#include <compiler.h>
#include <string.h>
#include <ee.h>
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <atomic.h>
#include <vdec_lib.h>
#include <imgl.h>
#include <crc.h>
#include <cache.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>
#include <media_system.h>
#include <component.h>
#include "vdec_test.h"

#define GetModuleLogLevel() ULOG_LVL_ERROR

extern volatile int32_t vdec_qt1_result;
volatile int32_t vdec_qt1_result = BCM_AT_NOT_STARTED;

static int32_t vdec_qt2_result = BCM_AT_NOT_STARTED;

extern uint8_t BsBuffer[];
uint8_t  COMP_SECTION("..noload.data")  BsBuffer[0x50000UL];

extern uint32_t BsBufSize;
uint32_t BsBufSize = 0UL;

extern uint32_t slice_offset[];
uint32_t slice_offset[15UL];
extern uint32_t maxNumSlices;
uint32_t maxNumSlices = 15UL;


uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}


#define NUM_FRAMES         (5UL)
#define WIDTH              (1280UL)
#define HEIGHT             (800UL)
#define NUM_REF_BUFFERS    (1UL)

#define NUM_MBROWS         ((HEIGHT + 15UL) >> 4UL)
#define NUM_MBCOLS         ((WIDTH + 15UL) >> 4UL)
#define CRC32_POLY         (0x04C11DB7UL)
#define REF_MEM_DATA_SIZE  ((2 * (((WIDTH * HEIGHT * 3) / 2)) + (2 *(NUM_MBROWS * NUM_MBCOLS * 120))) + ((NUM_MBROWS + 1) * NUM_MBCOLS * 16 * 8))
#define INT_MEM_DATA_SIZE  (13 * 1024)

static volatile int32_t vdec_qt_result = BCM_ERR_OK;


#define CHECK_ERR(ret,file,line)  if (BCM_ERR_OK != ret) { ULOG_ERR("Error: %d \t line %d\n",ret,line); vdec_qt_result = ret;}


BCM_TimeType COMP_SECTION(".dram.test.data") GTimeDecode[NUM_FRAMES];
BCM_TimeType COMP_SECTION(".dram.test.data") GTimeCopy[NUM_FRAMES];

uint8_t COMP_SECTION(".pp.uncached.int") GIntDataBuf[INT_MEM_DATA_SIZE];
uint8_t COMP_SECTION(".dram.test.data") GRefDataBuf[NUM_REF_BUFFERS][REF_MEM_DATA_SIZE];

uint8_t COMP_SECTION(".dram.test.data") GMPBinary8[0x6000UL];
uint8_t COMP_SECTION(".dram.test.data") GMPBinaryN[0x6000UL];
uint8_t COMP_SECTION(".dram.test.data") GMPCopyBinary[0x6000UL];
uint8_t COMP_SECTION(".dram.test.data") GSPBinaryCabac[0x10000UL];
uint8_t COMP_SECTION(".dram.test.data") GSPBinaryCavlc[0x10000UL];

uint8_t  COMP_SECTION(".dram.test.data") GBsBufferLib[0x100000UL];

uint8_t COMP_SECTION(".dram.test.data") GYUVFrame[((WIDTH * HEIGHT * 3) / 2)];

VDEC_ContextType GContext;

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


uint32_t VDEC_LoadTestImg(uint16_t aImgID, uint8_t* aBuffer, uint32_t aBufSize)
{
    int32_t ret;
    IMGL_LoadStatusType imglStatus;

    /* load the memories */
    ret = IMGL_LoadImg(aImgID,
                       aBuffer,
                       0UL,
                       aBufSize,
                       SystemEvent1,
                       &imglStatus);
    CHECK_ERR(ret,__FILE__, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ret = IMGL_GetStatus(&imglStatus);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("IMGL Error: %d  \t line %d \n",ret,__LINE__);
    }
    return imglStatus.size;
}

/** @brief VDEC Library test

    VDEC library test

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     Others                  Failure

    @post None

    @trace #BRCM_SWQTSEQ_VDEC_SEQ1

    @limitations None

    @code{.unparsed}
    Load binary images
    Load bit stream

    ret = VDEC_Init()
    if ret is not ERR_OK
        Report and log error

    ret = VDEC_Configure()
    if ret is not ERR_OK
        Report and log error

    loop for all frames
        loop for all slices
            read stream data as slice
            ret = VDEC_GetStreamInfo(slice)
            if ret is not ERR_OK
                Report and log error
            if slice is video and has complete frame
                exit slice loop
            else
                get next slice

        ret = VDEC_Decode(slice)
        if ret is not ERR_OK
            Report and log error

        wait for PP event

        flags = VDEC_Process()
        if flags has #VDEC_FLAGS_DECODE_COMPLETED not set
            Report and log error as ERR_UNKNOWN

        if flags has #VDEC_FLAGS_FRAME_AVAILABLE not set
            Report and log error as ERR_UNKNOWN

        ret = VDEC_CopyFrameData()
        if ret is not ERR_OK
            Report and log error

        wait for PP event

        flags = VDEC_Process()
        if flags has #VDEC_FLAGS_FRAME_DATA_COPY_DONE not set
            Report and log error as ERR_UNKNOWN

    ret = VDEC_DeInit()
    if ret is not ERR_OK
            Report and log error

    @endcode

*/
int32_t VDEC_QTSequence1(void)
{
    uint32_t i;
    uint32_t framesCompleted;
    int32_t ret;
    uint32_t bsBufSizeLib;
    uint32_t remainingSize;
    uint32_t currOffset;
    uint32_t currSize;
    BCM_EventMaskType mask;
    VDEC_ConfigType  config;
    VDEC_FlagsType flags;
    VDEC_StreamInfoType streamInfo;
    VDEC_SliceInfoType sliceInfo;
    VDEC_YUVBufferType yuvBuffer;
    BCM_TimeType t1,t2;

    vdec_qt_result = BCM_ERR_OK;

    streamInfo.streamID = VDEC_STREAMID_0;
    streamInfo.intMemory = GIntDataBuf;
    streamInfo.intMemorySize = INT_MEM_DATA_SIZE;

    streamInfo.mpBinary8 = GMPBinary8;
    streamInfo.mpBinaryN = GMPBinaryN;
    streamInfo.spBinaryCabac = GSPBinaryCabac;
    streamInfo.spBinaryCavlc = GSPBinaryCavlc;
    streamInfo.mpBinaryCopy = GMPCopyBinary;

    streamInfo.mpBinary8Size = VDEC_LoadTestImg(0xDE90U, GMPBinary8, 0x6000UL);
    streamInfo.mpBinaryNSize = VDEC_LoadTestImg(0xDE91U, GMPBinaryN, 0x6000UL);
    streamInfo.mpBinaryCopySize = VDEC_LoadTestImg(0xDE92U, GMPCopyBinary, 0x6000UL);
    streamInfo.spBinaryCavlcSize = VDEC_LoadTestImg(0xDEA0U, GSPBinaryCavlc, 0x10000UL);
    streamInfo.spBinaryCabacSize = VDEC_LoadTestImg(0xDEA1U, GSPBinaryCabac, 0x10000UL);

    bsBufSizeLib = VDEC_LoadTestImg(0XFF00U, GBsBufferLib, 0x100000);

    streamInfo.configData = GBsBufferLib;
    currSize = FindResync(&GBsBufferLib[0UL],bsBufSizeLib);
    currSize += FindResync(&GBsBufferLib[currSize],bsBufSizeLib - currSize);

    streamInfo.configDataSize = currSize;

    /* To store the out frame */
    yuvBuffer.yAddr = GYUVFrame;
    yuvBuffer.uAddr = &GYUVFrame[(WIDTH * HEIGHT)];
    yuvBuffer.vAddr = &GYUVFrame[(WIDTH * HEIGHT) + ((WIDTH * HEIGHT) >> 2UL)];
    yuvBuffer.yStride = WIDTH;
    yuvBuffer.cStride = WIDTH >> 1UL;
    yuvBuffer.width = WIDTH;
    yuvBuffer.height = HEIGHT;
    yuvBuffer.bitDepth = VDEC_BITDEPTH_8;
    yuvBuffer.format = VDEC_YUVFORMAT_420;

    GContext.state = VDEC_STATE_RESET;

    ret = VDEC_Init(&GContext);
    CHECK_ERR(ret,__FILE__, __LINE__);

    config.width = WIDTH;
    config.height = HEIGHT;
    config.bytesPerPixel = 1UL;
    config.totalMPCores = 2UL;
    config.streamMPCores[VDEC_STREAMID_0] = 1UL;
    config.streamMPCores[VDEC_STREAMID_1] = 0UL;
    config.streamMPCores[VDEC_STREAMID_2] = 0UL;
    config.streamMPCores[VDEC_STREAMID_3] = 0UL;
    config.numSPCores = 1UL;
    config.frameRate[VDEC_STREAMID_0] = 30UL;
    config.frameRate[VDEC_STREAMID_1] = 0UL;
    config.frameRate[VDEC_STREAMID_2] = 0UL;
    config.frameRate[VDEC_STREAMID_3] = 0UL;

    /* Note: Library test uses single stream and only one reference buffer */
    ret = VDEC_Configure(&GContext,&config,1UL, VDEC_NUM_REF_BUFFERS * VDEC_REF_MEM_DATA_SIZE);
    CHECK_ERR(ret,__FILE__, __LINE__);

    framesCompleted = 0UL;

    remainingSize = bsBufSizeLib;
    currOffset = 0UL;

    while(framesCompleted < NUM_FRAMES) {
        while (0UL < remainingSize) {
            /* send the SPS and PPS */
            currSize = FindResync(&GBsBufferLib[currOffset],
                                  remainingSize);
            if (currSize ==  -1) {
                currSize = remainingSize;
            }
            ret = VDEC_GetStreamInfo(&GContext,
                                     &streamInfo,
                                     &GBsBufferLib[currOffset],
                                     currSize,
                                     &sliceInfo);
            CHECK_ERR(ret,__FILE__, __LINE__);

            if (VDEC_SLICE_VIDEO == sliceInfo.sliceType) {
                BCM_GetTime(&t1);
                ret = VDEC_Decode(&GContext,
                                  &streamInfo,
                                  &GBsBufferLib[currOffset],
                                  currSize);
                CHECK_ERR(ret,__FILE__, __LINE__);

                remainingSize -= currSize;
                currOffset += currSize;
                break;
            }

            remainingSize -= currSize;
            currOffset += currSize;
        }

        while (1UL)  {
            BCM_WaitEvent(PP_GPEvent);
            BCM_GetEvent(Test_VDECComp, &mask);
            BCM_ClearEvent(mask);
            flags = VDEC_Process(&GContext);
            if (0UL != (flags & (VDEC_FLAGS_DECODE_COMPLETED << streamInfo.streamID))) {
                BCM_GetTime(&t2);
                GTimeDecode[framesCompleted] = BCM_GetTimeAbsDiff(t1,t2);
            }
            if (0UL != (flags & (VDEC_FLAGS_FRAME_AVAILABLE << streamInfo.streamID))) {
                BCM_GetTime(&t1);
                /* Send fill buffer info */
                ret = VDEC_CopyFrameData(&GContext,
                                      &streamInfo,
                                      VDEC_BUFMODE_FRAME,
                                      &yuvBuffer);
                CHECK_ERR(ret,__FILE__, __LINE__);
            }
            if (0UL != (flags & (VDEC_FLAGS_FRAME_DATA_COPY_DONE << streamInfo.streamID))) {
                BCM_GetTime(&t2);
                GTimeCopy[framesCompleted] = BCM_GetTimeAbsDiff(t1,t2);
                /* fill Buffer done*/
                framesCompleted++;
                break;
            }
            if (0UL != (flags & (VDEC_FLAGS_ERROR_DETECTED << streamInfo.streamID))) {
                CHECK_ERR(BCM_ERR_UNKNOWN,__FILE__, __LINE__);
                framesCompleted = NUM_FRAMES + 1;
                break;
            }
            if (0UL != (flags & (VDEC_FLAGS_LIBRARY_ERROR << streamInfo.streamID))) {
                CHECK_ERR(BCM_ERR_UNKNOWN,__FILE__, __LINE__);
                framesCompleted = NUM_FRAMES + 1;
                break;
            }
        }
    }

    if (framesCompleted == NUM_FRAMES) {
        for (i = 0UL; i < framesCompleted; i++) {
            ULOG_ERR("%d: Decode %d , copy %d \n",i,GTimeDecode[i].ns,GTimeCopy[i].ns);
        }
    } else {
        ULOG_ERR(" Test failed \n");
    }

    VDEC_DeInit(&GContext);
    ULOG_INFO("**********%s completed ***********\n",__func__);
    return vdec_qt_result;
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

    .component[1].compID = MSYS_COMP_ID_VDEC,
    .component[1].numConfigs = 1UL,
    .component[1].config[0] = MSYS_CONFIG_VDEC_MOD,
#if (0UL < VDEC_COMP_NUM_STREAMS)
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
#endif
#if (1UL < VDEC_COMP_NUM_STREAMS)
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 1U,
#endif
#if (2UL < VDEC_COMP_NUM_STREAMS)
    .component[1].portMap[2].srcCompIdx = 0U,
    .component[1].portMap[2].srcPortIdx = 2U,
#endif
#if (3UL < VDEC_COMP_NUM_STREAMS)
    .component[1].portMap[3].srcCompIdx = 0U,
    .component[1].portMap[3].srcPortIdx = 3U,
#endif

    .component[2].compID = MSYS_COMP_ID_EX_SNK,
    .component[2].numConfigs = 1UL,
    .component[2].config[0] = 0UL,
#if (0UL < VDEC_COMP_NUM_STREAMS)
    .component[2].portMap[0].srcCompIdx = 1U,
    .component[2].portMap[0].srcPortIdx = 0U,
#endif
#if (1UL < VDEC_COMP_NUM_STREAMS)
    .component[2].portMap[1].srcCompIdx = 1U,
    .component[2].portMap[1].srcPortIdx = 1U,
#endif
#if (2UL < VDEC_COMP_NUM_STREAMS)
    .component[2].portMap[2].srcCompIdx = 1U,
    .component[2].portMap[2].srcPortIdx = 2U,
#endif
#if (3UL < VDEC_COMP_NUM_STREAMS)
    .component[2].portMap[3].srcCompIdx = 1U,
    .component[2].portMap[3].srcPortIdx = 3U,
#endif
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


int32_t VDECTest_Start(void)
{
    int32_t ret;

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

err:
    return ret;
}

int32_t VDECTest_Stop(void)
{
    int32_t ret;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        goto err;
    }

err:
    return ret;
}

volatile uint32_t TestAlarmVal = 0UL;

uint32_t gVDECTestCount = 0UL;

void Test_VDECCompAlarm_cb()
{
    if(1UL == TestAlarmVal)
    {
        if(++gVDECTestCount == 10)
        {
            ATOMIC_Set(&TestAlarmVal, 0UL);
            SetEvent(Test_VDECComp,SystemEvent1);
        }
    }
}

void VDECTest_Setup()
{
    uint32_t remainingSize;
    uint32_t currOffset;
    uint32_t currSize;

    BsBufSize = VDEC_LoadTestImg(0XFF01U, BsBuffer, 0x50000);
    /* calculate the slice offsets */
    maxNumSlices = 0UL;


    remainingSize = BsBufSize;
    currOffset = 0;
    while ((0UL < remainingSize) &&
           (15UL > maxNumSlices)) {
        /* send the SPS and PPS */
        currSize = FindResync(&BsBuffer[currOffset],
                              remainingSize);
        if (currSize == -1) {
            slice_offset[maxNumSlices++] = remainingSize;
            break;
        } else {
            slice_offset[maxNumSlices++] = currSize;
            currOffset += currSize;
            remainingSize -= currSize;
        }
    }
}

/** @brief VDEC component test

    VDEC component test

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     Others                  Failure

    @post None

    @trace #BRCM_SWQTSEQ_VDEC_SEQ0

    @limitations None

    @code{.unparsed}

    Load test files
    ret = MSYS_Init()
    if ret is not ERR_OK
        Report and log error

    ret = MSYS_Configure()
    if ret is not ERR_OK
        Report and log error

    ret = MSYS_Start()
    if ret is not ERR_OK
        Report and log error

    Wait for alarm

    ret = MSYS_Stop()
    if ret is not ERR_OK
        Report and log error

    ret = MSYS_DeInit()
    if ret is not ERR_OK
        Report and log error

    @endcode

*/
int32_t VDEC_QTSequence0(void)
{
    int32_t ret;
    BCM_EventMaskType mask;

    vdec_qt_result = BCM_AT_EXECUTING;
    VDECTest_Setup();

    ret = VDECTest_Start();
    CHECK_ERR(ret,__FILE__, __LINE__);
    gVDECTestCount = 0UL;
    ATOMIC_Set(&TestAlarmVal, 1UL);
    BCM_WaitEvent(SystemEvent1);
    BCM_GetEvent(Test_VDECComp, &mask);
    BCM_ClearEvent(mask);
    ret = VDECTest_Stop();
    CHECK_ERR(ret,__FILE__, __LINE__);

    if (BCM_AT_EXECUTING == vdec_qt_result) {
        vdec_qt_result = BCM_ERR_OK;
    }
    return vdec_qt_result;
}

uint32_t GTestID;

TASK(Test_VDECComp)
{
    vdec_qt_result = BCM_ERR_OK;
    if (1UL == GTestID) {
        vdec_qt1_result = BCM_AT_EXECUTING;
        vdec_qt1_result = VDEC_QTSequence0();
    }
    if (2UL == GTestID) {
        vdec_qt2_result = BCM_AT_EXECUTING;
        vdec_qt2_result = VDEC_QTSequence1();
    }
    GTestID = 0UL;
    BCM_TerminateTask();
}

void VDECQT1_Start(void)
{
    BCM_ActivateTask(Test_VDECComp);
}

void VDECQT2_Start(void)
{
    BCM_ActivateTask(Test_VDECComp);
}

int32_t VDECQT1_GetResult(void)
{
    return vdec_qt1_result;
}

int32_t VDECQT2_GetResult(void)
{
    return vdec_qt2_result;
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_ERR_OK;
    if (0UL == GTestID) {
        switch(aIndex)
        {
        case VDEC_QT1:
            GTestID = VDEC_QT1;
            VDECQT1_Start();
            break;
        case VDEC_QT2:
            GTestID = VDEC_QT2;
            VDECQT2_Start();
            break;
        default:
            ret = BCM_AT_NOT_AVAILABLE;
            break;
        }
    } else {
        ret = BCM_AT_NOT_STARTED;
    }

    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret;

    switch(aIndex)
    {
    case VDEC_QT1:
        ret = VDECQT1_GetResult();
        break;
    case VDEC_QT2:
        ret = VDECQT2_GetResult();
        break;
    default:
        ret = BCM_AT_NOT_AVAILABLE;
        break;
    }

    return ret;
}

void ApplicationInit()
{
    (void)BCM_ActivateTask(ExSrc_CmdProcessTask);
    (void)BCM_ActivateTask(ExSnk_CmdProcessTask);
}

/** @} */