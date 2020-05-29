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

#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <stdlib.h>
#include <string.h>
#include "ee.h"
#include <openvx_lib.h>

#include <bcm_test.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <cache.h>
#include <imgl.h>
#include <osil/bcm_osil.h>
#include "test_main.h"

#define MAX_GRAPH_SIZE (6UL*16384UL)
uint8_t GraphInfo[MAX_GRAPH_SIZE];
int32_t GTestID = -1;

typedef OpenVXTestConfigType* (*OpenVXTest_GetTestConfigFuncPtr)();
const static OpenVXTest_GetTestConfigFuncPtr OpenVXTest_GetTestConfigFuncPtrTbl[] =
{
    ScaleTest_GetTestConfig,            /* 1 */
    ArithTest_GetTestConfig,            /* 2 */
    FilterTest_GetTestConfig,           /* 3 */
    HistTest_GetTestConfig,             /* 4 */
    ThresholdTest_GetTestConfig,        /* 5 */
    MorphTest_GetTestConfig,            /* 6 */
    ColorConvertTest_GetTestConfig,     /* 7 */
    BitdepthConvTest_GetTestConfig,     /* 8 */
    PedDetectTest_GetTestConfig,        /* 9 */
    OptFlowTest_GetTestConfig,          /* 10 */
    IntegralImgTest_GetTestConfig,      /* 11 */
    AccumulateImgTest_GetTestConfig,    /* 12 */
    GaussianPyramidTest_GetTestConfig,  /* 13 */
    HalfScaleGaussianTest_GetTestConfig,/* 14 */
    LaplacianPyramidTest_GetTestConfig, /* 15 */
    FaceDetectTest_GetTestConfig,       /* 16 */
    CropnScaleTest_GetTestConfig        /* 17 */
};

#define NUMTESTS (sizeof(OpenVXTest_GetTestConfigFuncPtrTbl) / sizeof(OpenVXTest_GetTestConfigFuncPtr))
static volatile int32_t openvxlib_qt_result[NUMTESTS];
#define MAX_ALLOC_MEMORY 0x1200000
COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.data") openVXPoolMem[MAX_ALLOC_MEMORY];

uint32_t UsedMemfrmAlloc = 0;

uint8_t* OpenVXTest_PoolMemAlloc(uint32_t aSize)
{
    uint8_t *ptr;
    uint32_t alignSize;

    if ((UsedMemfrmAlloc + aSize) > MAX_ALLOC_MEMORY){
        ULOG_ERR("Exceeds allcoated memory %s\n", __func__);
        return NULL;
    }

    ptr = openVXPoolMem+ UsedMemfrmAlloc;
    /* Aligining it to 256 */
    alignSize = ((aSize + 255) & ~(uint32_t)0xFF);
    UsedMemfrmAlloc += alignSize;

    return ptr;
}

void OpenVXTest_PoolFreeMem()
{
    UsedMemfrmAlloc = 0;
}

int32_t OpenVXTest_PTMLoadData(uint16_t id,uint8_t* aBuffer, uint32_t aMaxSize, uint32_t* aReadSize)
{
    BCM_EventMaskType mask;
    int32_t status;

    int32_t ret;
    IMGL_LoadStatusType imglStatus;

    ret = IMGL_LoadImg(id, aBuffer, 0UL, aMaxSize,
            SystemEvent0, &imglStatus);

    if (ret == BCM_ERR_OK) {
        BCM_WaitEvent(SystemEvent0);
        BCM_GetEvent(OpenVXTest, &mask);
        BCM_ClearEvent(SystemEvent0);
        status = IMGL_GetStatus(&imglStatus);
        if (status == BCM_ERR_OK) {
            *aReadSize = imglStatus.size;
            ret = BCM_ERR_OK;
        } else {
            ULOG_ERR("OpenVX TEST: IMGL_GetStatus Failed\n");
        }
    } else {
        ULOG_ERR("OpenVX TEST: IMGL_LoadImg Failed\n");
    }

    ULOG_ERR("OpenVX TEST: PTM Load: Image Size %d\n", *aReadSize);
    return ret;
}

int32_t Test_LoadGraph(uint16_t id)
{
    uint32_t imgSz;
    return OpenVXTest_PTMLoadData(id, GraphInfo, MAX_GRAPH_SIZE,&imgSz);
}

#define OPENVX_PP_BINARY_SIZE (200UL*1024UL)
uint8_t COMP_SECTION(".openvx.uncached.OpenVXTest_PPBinary") OpenVXTest_PPBinary[OPENVX_PP_BINARY_SIZE];
#define OPENVX_TABLE_SIZE (64UL*1024UL)//(20UL*1024UL)
uint8_t COMP_SECTION(".openvx.uncached.OpenVXTest_Table") OpenVXTest_Table[OPENVX_TABLE_SIZE];
#define OPENVX_NUM_PP_CORES 4UL

#define CHECK_ERR(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); openvxlib_qt_result[index] = ret;}

void Test_IT(int32_t index)
{
    int32_t ret;
    uint32_t graphID;
    BCM_EventMaskType mask;
    OpenVXLib_Context context;
    uint32_t imgSz = 0UL;
    uint32_t flags = 0;
    ULOG_INFO("**********%s Started ***********\n",__func__);
    OpenVXTestConfigType* testConfig;

    context.state = OPENVX_LIB_STATE_RESET;
    ret = OpenVXTest_PTMLoadData(0xAC10, OpenVXTest_PPBinary, OPENVX_PP_BINARY_SIZE, &imgSz);
    CHECK_ERR(ret,__FILE__,__LINE__);
    ret = OpenVXLib_Init(&context);
    CHECK_ERR(ret,__FILE__,__LINE__);
    if  (BCM_ERR_OK == ret) {
        ret = OpenVXLib_Boot(&context, OPENVX_NUM_PP_CORES, OpenVXTest_PPBinary, imgSz);
        CHECK_ERR(ret,__FILE__,__LINE__);
    }

    ret = OpenVXTest_PTMLoadData(0xAC11, OpenVXTest_Table, OPENVX_TABLE_SIZE, &imgSz);
    CHECK_ERR(ret,__FILE__,__LINE__);

    testConfig = OpenVXTest_GetTestConfigFuncPtrTbl[index]();
    ret = testConfig->setMemory();
    if (ret == BCM_ERR_NOMEM){
        ULOG_ERR("********* Memory cannot be allocated %s***********\n",__func__);
        goto FREE_MEM;
    }
    for(graphID=0;graphID<testConfig->numGraphs;graphID++) {
        ret = Test_LoadGraph(testConfig->startGraphID + graphID);
        CHECK_ERR(ret,__FILE__,__LINE__);

        /* prepare the input images */
        ret = testConfig->PrepareInput(graphID);
        CHECK_ERR(ret,__FILE__,__LINE__);
        /* post the message to server */
        ret = OpenVXLib_Start(&context, GraphInfo, testConfig->GetImageInfo(), OpenVXTest_Table, testConfig->uniqueID);
        CHECK_ERR(ret,__FILE__,__LINE__);
        if (BCM_ERR_OK == ret) {
            do {
                BCM_WaitEvent(PP_GPEvent);
                BCM_GetEvent(OpenVXTest,&mask);
                BCM_ClearEvent(mask);
                flags = OpenVXLib_Process(&context, GraphInfo, testConfig->GetImageInfo(), OpenVXTest_Table, testConfig->uniqueID);
                if (0UL != (flags & OPENVX_LIB_ERROR)) {
                    CHECK_ERR(BCM_ERR_UNKNOWN,__FILE__,__LINE__);
                }
            } while (0UL == (flags & OPENVX_LIB_DONE));
            ret = testConfig->ValidateOutput(graphID);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("Graph %d Failed \n",graphID);
                openvxlib_qt_result[index] = BCM_ERR_DATA_INTEG;
            }
        }
    }

    ret = OpenVXLib_DeInit(&context);
    CHECK_ERR(ret,__FILE__,__LINE__);
    if (BCM_AT_EXECUTING == openvxlib_qt_result[index]) {
        openvxlib_qt_result[index] = BCM_ERR_OK;
    }
    ULOG_INFO("**********%s completed ***********\n",__func__);
FREE_MEM:
    testConfig->freeMemory();

}
TASK(OpenVXTest)
{

    if ((GTestID != -1) && (GTestID < NUMTESTS)) {
        Test_IT(GTestID);
    }
    GTestID = -1;
    BCM_TerminateTask();
}
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if ((aIndex <= NUMTESTS) && (aIndex > 0)){
        if (-1 == GTestID) {
            GTestID = aIndex-1;
            openvxlib_qt_result[aIndex-1] = BCM_AT_EXECUTING;
            BCM_ActivateTask(OpenVXTest);
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
    if ((aIndex <= NUMTESTS) && (aIndex > 0)){
        ret = openvxlib_qt_result[aIndex-1];
    }
    return ret;
}

void ApplicationInit()
{
    int32_t i;

    for (i=0;i<NUMTESTS;i++)
    {
        openvxlib_qt_result[i] = BCM_AT_NOT_STARTED;
    }
    GTestID = -1;
}

/*
void Test_IT1()
{
    int32_t ret;
    OpenVXLib_Context context;
    OpenVXLib_Flags flags;
    BCM_EventMaskType mask;

    context.state = OPENVX_LIB_STATE_RESET;

    ULOG_INFO("**********%s Started ***********\n",__func__);
    ret = OpenVXLib_Init(&context, 4UL);
    CHECK_ERR(ret,__FILE__,__LINE__);
    ret = OpenVXLib_Boot(&context,vid_vmp_cv,sizeof(vid_vmp_cv));
    CHECK_ERR(ret,__FILE__,__LINE__);
    SetupTestImage();
    ret = OpenVXLib_Start(&context, graph_start, (uint32_t *)GImages);
    CHECK_ERR(ret,__FILE__,__LINE__);
    do {
        BCM_WaitEvent(PP_GPEvent);
        BCM_GetEvent(OpenVXTest,&mask);
        BCM_ClearEvent(mask);
        flags = OpenVXLib_Process(&context,graph_start, (uint32_t*)GImages);
        if(0UL != (flags & OPENVX_LIB_DONE)) {
            break;
        }
    } while (0UL != flags);

    ret = OpenVXLib_DeInit(&context);
    CHECK_ERR(ret,__FILE__,__LINE__);

    if (BCM_AT_EXECUTING == openvxlib_qt_result) {
        openvxlib_qt_result = BCM_ERR_OK;
    }
    ULOG_INFO("**********%s completed ***********\n",__func__);
}
*/

