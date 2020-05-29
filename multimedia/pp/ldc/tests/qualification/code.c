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

#include <stdlib.h>
#include <string.h>
#include "ee.h"
#include <pp.h>
#include <ldc_lib.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <crc.h>
#include <imgl.h>
#include <cache.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_INFO
#include <ulog.h>

#define MAX_LDC_BINARY_SIZE            (0x7000UL)

uint8_t COMP_SECTION(".pp.uncached.test") GLDCBinary[MAX_LDC_BINARY_SIZE];


#define MAX_LDC_MATRIX_SIZE            ((1920/16) * (1088/16) * 8)
uint8_t GMatrix[MAX_LDC_MATRIX_SIZE];


#define LDCLIB_TEST_CRC32_POLY         (0x04C11DB7UL)

static volatile int32_t LDCLib_qt1_result = BCM_AT_NOT_STARTED;

uint8_t COMP_SECTION(".pp.uncached.test.data") outImage[1920*16*3/2];
uint8_t COMP_SECTION(".pp.uncached.test.data") inImage[1920*16*3/2];

uint8_t COMP_ALIGN(256) COMP_SECTION(".pp.uncached.test.data") memPool[18*((1920*16)+(1024*8*2))];

#define CHECK_ERR(ret,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("Error: %d line %d\n",ret,line); LDCLib_qt1_result = ret;}

/*

    For each MBROW, we maintain NUM_REF_CRCS_PER_MBROW CRC's - that
    correspond to different build configurations of the LDC lib.

    CRC at Index 0 => expected to match when ldclib is built with the gcc toolchain gcc-arm-none-eabi-4_9-2015q3
    CRC at Index 1 => expected to match when ldclib is built with the rvct toolchain DS-5_5.26.2

*/
#define NUM_REF_CRCS_PER_MBROW 2

uint32_t CrcData_1080[68];
const uint32_t RefCrcData_1080[68][NUM_REF_CRCS_PER_MBROW] = {
{0x610a9dce, 0x610a9dce}, {0x7abda609, 0x7abda609}, {0x8486ac80, 0x8486ac80}, {0x826b94ac, 0x826b94ac},
{0x8b10fa1e, 0x8b10fa1e}, {0xf9984e27, 0xf9984e27}, {0x2f5c44c6, 0xfe4ef7f8}, {0x583fbffa, 0x583fbffa},
{0x87985fcc, 0x87985fcc}, {0xbc026961, 0xbc026961}, {0xe01c8b09, 0xe01c8b09}, {0x6352dede, 0x6352dede},
{0x4910e0cd, 0x4910e0cd}, {0x66278757, 0x66278757}, {0xada5e7e9, 0xada5e7e9}, {0x246bf9ba, 0x246bf9ba},
{0x2f902433, 0x2f902433}, {0x42640d36, 0x42640d36}, {0xae13a222, 0xae13a222}, {0x82eeb229, 0x82eeb229},
{0x94ede3a7, 0x94ede3a7}, {0x76350180, 0xd8fd7033}, {0xc8fc8ce4, 0xc8fc8ce4}, {0x76beece6, 0x76beece6},
{0xb8b02cd9, 0xb8b02cd9}, {0xd8310db4, 0xd8310db4}, {0x4a898cce, 0x4a898cce}, {0x67e06c3f, 0x67e06c3f},
{0x87c3ca78, 0x87c3ca78}, {0x81f3368f, 0x81f3368f}, {0xe7bfe3ba, 0xe7bfe3ba}, {0xc430e489, 0xc430e489},
{0xf949f2c4, 0xf949f2c4}, {0x9f8a13ad, 0x9f8a13ad}, {0x9f8f53a8, 0x0ec73409}, {0x557473bf, 0x557473bf},
{0x188f7ba6, 0x188f7ba6}, {0xb444e435, 0xb444e435}, {0xe02ee24d, 0xe02ee24d}, {0x327d73f0, 0x327d73f0},
{0xda288abe, 0xda288abe}, {0x0f5e4350, 0x0d6a7243}, {0xad35e6cb, 0xad35e6cb}, {0x316dbbfc, 0xacdc6ada},
{0xbcae4d4e, 0xbcae4d4e}, {0x8133df1d, 0x8133df1d}, {0x7ff85752, 0x7ff85752}, {0x4c55e423, 0x4c55e423},
{0x593f8f30, 0xf81d3166}, {0x9720a251, 0x9720a251}, {0x32d95594, 0x32d95594}, {0x40a283bf, 0x40a283bf},
{0x32f2024b, 0x32f2024b}, {0x283db587, 0x00ba3b3c}, {0x39b5ee75, 0x39b5ee75}, {0x75a0fad5, 0x75a0fad5},
{0xaf5fd0ed, 0x756ee753}, {0xb078410f, 0xec7c4e11}, {0x50c61774, 0x50c61774}, {0xcc4b3276, 0xcc4b3276},
{0x96c7375e, 0xc4d5e02e}, {0x4a779890, 0x4a779890}, {0x6f27eca3, 0x6f27eca3}, {0x2c4cb331, 0x2c4cb331},
{0x1609a21d, 0x1609a21d}, {0x0e225dad, 0x0e225dad}, {0x06742083, 0x06742083}, {0x8e42bbed, 0x8e42bbed},
};

uint32_t CrcData_720[45];
const uint32_t RefCrcData_720[45][NUM_REF_CRCS_PER_MBROW] = {
{0xbaf7118c, 0xbaf7118c}, {0x66a0e510, 0x66a0e510}, {0x7787ced4, 0x7787ced4}, {0x584e6534, 0xb196933e},
{0x2402cab4, 0x2402cab4}, {0x276fc086, 0x276fc086}, {0xe5dcff90, 0x83500a0d}, {0xc6ebf63a, 0xc6ebf63a},
{0x37712ca6, 0x37712ca6}, {0xc48d3e89, 0xc48d3e89}, {0x0e8c8dc7, 0x0e8c8dc7}, {0x1b71c1b1, 0x1b71c1b1},
{0x08d30940, 0x08d30940}, {0xae2b68bc, 0xae2b68bc}, {0x1fe0291d, 0x1fe0291d}, {0x739d8f6d, 0x739d8f6d},
{0x026b29c2, 0x026b29c2}, {0xf195e674, 0xf195e674}, {0x693eef81, 0x693eef81}, {0x091c5000, 0x091c5000},
{0x61be7cc4, 0xc7879d0d}, {0xbc98d6ad, 0xbc98d6ad}, {0x28c3fd61, 0x28c3fd61}, {0xa5c83a5c, 0xa5c83a5c},
{0x203ce9f1, 0x203ce9f1}, {0xd3e26c1c, 0xd3e26c1c}, {0xcffd7d07, 0xcffd7d07}, {0x349cea49, 0x349cea49},
{0x7fb5d991, 0x7fb5d991}, {0xddf84d57, 0xddf84d57}, {0x4655568a, 0x4655568a}, {0xc03e426f, 0xc03e426f},
{0x18856aea, 0x18856aea}, {0x2fbb5eeb, 0x2fbb5eeb}, {0x40185e5c, 0x40185e5c}, {0x2ab795b2, 0xcb95fa9e},
{0x59897bbc, 0x59897bbc}, {0x4dd875ed, 0x4dd875ed}, {0x9a7c3a11, 0x9a7c3a11}, {0xbb9db778, 0xbb9db778},
{0x33512598, 0x33512598}, {0xb522d403, 0xb522d403}, {0x325f3c9e, 0x377577f7}, {0x78e3d32a, 0xc7976cf7},
{0xf8a652d7, 0xf8a652d7},
};

LDCLib_Context context;
uint32_t outMBRow;
uint32_t inMBRow;
uint32_t inSubmitMBRow;
uint32_t outSubmitMBRow;

void TestLDCLib(uint32_t aConfigID, uint32_t *aCrcData, const uint32_t aRefCrcData[][NUM_REF_CRCS_PER_MBROW])
{
    uint32_t i, j;
    int32_t ret;

    LDCLib_Config config;
    LDCLib_YUVBuffer yuvBuffer;
    IMGL_LoadStatusType imglStatus;

    BCM_EventMaskType mask;
    uint32_t flags;

    uint32_t binSize = 0;
    BCM_TimeType t1,t2,t3;
    int32_t ptmStatus;

    uint32_t copyInProgress;
    uint32_t correctInProgress;

    context.state = LDC_LIB_STATE_RESET;
    ret = LDCLib_Init(&context);
    CHECK_ERR(ret, __LINE__);

    /* Load config */
    ret = IMGL_LoadImg(aConfigID, (uint8_t*)&config, 0UL, sizeof(LDCLib_Config), SystemEvent1, &imglStatus);

    CHECK_ERR(ret, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    }

    BCM_GetTime(&t1);
    ret = LDCLib_Configure(&context, &config, GMatrix, sizeof(memPool));
    BCM_GetTime(&t2);
    CHECK_ERR(ret, __LINE__);
    ULOG_INFO("Configure time=%d\n", (t2.ns - t1.ns));
    DCACHE_CleanRange((uint32_t)GMatrix, MAX_LDC_MATRIX_SIZE);

    /* Load pp binary */
    ret = IMGL_LoadImg((uint16_t)0xDC10, GLDCBinary, 0UL,
                        MAX_LDC_BINARY_SIZE, SystemEvent1, &imglStatus);
    CHECK_ERR(ret, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);\
    } else {
        binSize = imglStatus.size;
    }

    ret = LDCLib_Boot(&context,
                    0UL,
                    GLDCBinary,
                    binSize);
    CHECK_ERR(ret, __LINE__);

    yuvBuffer.lumaStride = (config.input_width);
    yuvBuffer.chromaStride = (config.input_width / 2UL);
    yuvBuffer.format = 0;
    yuvBuffer.frameNo = 0;
    yuvBuffer.mbRow = 0;

    outMBRow = 0;
    inMBRow = 0;
    inSubmitMBRow = 0;
    outSubmitMBRow = 0;
    copyInProgress = 0U;
    correctInProgress = 0U;

    while (outMBRow < (config.output_height >> 4)) {
        if((inMBRow == inSubmitMBRow) && (inSubmitMBRow < (config.input_height >> 4))) {
            yuvBuffer.lumaAddr = &inImage[0UL];
            yuvBuffer.cbAddr = &inImage[config.input_width*16];
            yuvBuffer.crAddr = &inImage[config.input_width*16 + config.input_width*4];
            yuvBuffer.mbRow = inSubmitMBRow;
            yuvBuffer.lumaStride = 0;
            yuvBuffer.chromaStride = 0;

            if (inSubmitMBRow & 1) {
                BCM_MemSet(yuvBuffer.lumaAddr, 0x00, config.input_width*16);
                BCM_MemSet(yuvBuffer.cbAddr, 0x80, config.input_width*4);
                BCM_MemSet(yuvBuffer.crAddr, 0x80, config.input_width*4);
            } else {
                BCM_MemSet(yuvBuffer.lumaAddr, 0xFF, config.input_width*16);
                BCM_MemSet(yuvBuffer.cbAddr, 0x80, config.input_width*4);
                BCM_MemSet(yuvBuffer.crAddr, 0x80, config.input_width*4);
            }

            ret = LDCLib_CopyBuffer(&context, memPool,&yuvBuffer);
            if (BCM_ERR_BUSY == ret) {
                inSubmitMBRow++;
                BCM_GetTime(&t1);
                copyInProgress = 1U;
            } else if (BCM_ERR_OK == ret) {
                inMBRow++;
                inSubmitMBRow++;
                ULOG_INFO("d %d time=0\n",inMBRow);
            }
        }
        yuvBuffer.lumaAddr = &outImage[0UL];
        yuvBuffer.cbAddr = &outImage[config.output_width*16];
        yuvBuffer.crAddr = &outImage[config.output_width*16 + config.output_width*4];
        yuvBuffer.mbRow = outSubmitMBRow;
        yuvBuffer.lumaStride = config.output_width;
        yuvBuffer.chromaStride = config.output_width / 2;
        ret = LDCLib_Correct(&context,
                                memPool,
                                GMatrix,
                                &yuvBuffer);
        if(ret == BCM_ERR_BUSY) {
            BCM_GetTime(&t2);
            correctInProgress = 1U;
            outSubmitMBRow++;
        }

        flags = 0U;
        if ((copyInProgress == 1U) || (correctInProgress == 1U)) {
            BCM_WaitEvent(PP_GPEvent);
            BCM_GetEvent(LDCLibTest,&mask);
            BCM_ClearEvent(mask);
            flags = LDCLib_Process(&context);
        }

        if (flags & LDC_LIB_COPY_DONE) {
            BCM_GetTime(&t3);
            t3 = BCM_GetTimeAbsDiff(t1,t3);
            ULOG_INFO("d %d time=%d\n",inMBRow,t3.ns);
            copyInProgress = 0U;
            inMBRow++;
        }
        if (flags & LDC_LIB_CORRECT_DONE) {
            BCM_GetTime(&t3);
            t3 = BCM_GetTimeAbsDiff(t2,t3);
            ULOG_INFO("c %d time=%d\n",outMBRow,t3.ns);
            aCrcData[outMBRow] = BCM_CRC32(outImage,(config.output_width*16*3/2),LDCLIB_TEST_CRC32_POLY);
            correctInProgress = 0U;
            outMBRow++;
        }

    }

    LDCLib_DeInit(&context);

    for (i = 0UL; i < (config.output_height >> 4); i++) {
        uint32_t matchedCrc = 0U;

        for (j = 0UL; j < NUM_REF_CRCS_PER_MBROW; j++) {
            if (aCrcData[i] == aRefCrcData[i][j]) {
                matchedCrc = 1U;
                break;
            }
        }

        if (matchedCrc != 1U) {
            ULOG_ERR("%d: 0x%08x\n", i, aCrcData[i]);
            if (LDCLib_qt1_result != BCM_ERR_DATA_INTEG) {
                LDCLib_qt1_result = BCM_ERR_DATA_INTEG;
            }
        }
    }

    if (BCM_AT_EXECUTING == LDCLib_qt1_result) {
        LDCLib_qt1_result = BCM_ERR_OK;
    }
    ULOG_INFO("**********%s completed ***********\n",__func__);
}
TASK(LDCLibTest)
{
    /* Test 720p config */
    TestLDCLib(0xDC20,CrcData_720,RefCrcData_720);

    /* Test 1080p config */
    TestLDCLib(0xDC21,CrcData_1080,RefCrcData_1080);

    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        LDCLib_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(LDCLibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = LDCLib_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
}
