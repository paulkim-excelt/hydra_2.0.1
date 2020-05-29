/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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

#include <compiler.h>
#include <bcm_err.h>
#include <openvx_lib.h>
#include <stdlib.h>
#include <string.h>
#include <ulog.h>
#include <cache.h>
#include <crc.h>
#include <bcm_utils.h>

#include "test_main.h"
#include "opticalflow_internal.h"

uint8_t *OptFlowTest_PrevPyr;
uint8_t *OptFlowTest_CurrPyr;
COMP_ALIGN(256)
OpenVXLib_KeyPoint COMP_SECTION(".dram.test.data")  OptFlowTest_Corners[OPTICALFLOW_CORNERS_MAX_COUNT];
COMP_ALIGN(256)
OpenVXLib_KeyPoint COMP_SECTION(".dram.test.data")  OptFlowTest_PrevPoints[OPTICALFLOW_PREVPOINTS_MAX_COUNT];
COMP_ALIGN(256)
OpenVXLib_KeyPoint COMP_SECTION(".dram.test.data")  OptFlowTest_CurrPoints[OPTICALFLOW_CURRPOINTS_MAX_COUNT];


OpenVXLib_StorageInfo OptFlowTestImageInfo[OPTICALFLOW_MAX_DATA_ENTRIES];


int32_t  OptFlowTest_SetMemory()
{
    uint8_t *tmpPtr;

    tmpPtr = OpenVXTest_PoolMemAlloc(OPTICALFLOW_TEMP_STORAGE_SIZE);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        OptFlowTestImageInfo[OPTICALFLOW_TEMP_STORAGE_IDX].storageAddr = tmpPtr;
    }
    OptFlowTestImageInfo[OPTICALFLOW_TEMP_STORAGE_IDX].storageSize = OPTICALFLOW_TEMP_STORAGE_SIZE;
    tmpPtr = OpenVXTest_PoolMemAlloc(OPTICALFLOW_CURRIMAGE_IMG_SIZE);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        OptFlowTestImageInfo[OPTICALFLOW_CURRIMAGE_IMG_IDX].storageAddr = tmpPtr;
    }
    OptFlowTestImageInfo[OPTICALFLOW_CURRIMAGE_IMG_IDX].storageSize = OPTICALFLOW_CURRIMAGE_IMG_SIZE;
    OptFlowTest_PrevPyr = OpenVXTest_PoolMemAlloc(OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE);
    OptFlowTest_CurrPyr = OpenVXTest_PoolMemAlloc(OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE);
    return BCM_ERR_OK;
}

void OptFlowTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* OptFlowTest_GetImageInfo()
{
    return OptFlowTestImageInfo;
}
uint32_t OptFlowTest_GetNumImages()
{
    return OPTICALFLOW_MAX_DATA_ENTRIES;
}

int32_t OptFlowTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;
    uint8_t *optFlowTest_CurrImage = OptFlowTestImageInfo[OPTICALFLOW_CURRIMAGE_IMG_IDX].storageAddr;
    uint8_t *OptFlowTest_TempMem = OptFlowTestImageInfo[OPTICALFLOW_TEMP_STORAGE_IDX].storageAddr;

    if (0UL == aGraphID) {
        OptFlowTestImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageAddr = OptFlowTest_PrevPyr;
        OptFlowTestImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE;
        OptFlowTestImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageAddr = OptFlowTest_CurrPyr;
        OptFlowTestImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE;
        OptFlowTestImageInfo[OPTICALFLOW_CORNERS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlowTest_Corners;
        OptFlowTestImageInfo[OPTICALFLOW_CORNERS_ARRAY_IDX].storageSize = sizeof(OptFlowTest_Corners);
        OptFlowTestImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlowTest_PrevPoints;
        OptFlowTestImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlowTest_PrevPoints);
        OptFlowTestImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlowTest_CurrPoints;
        OptFlowTestImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlowTest_CurrPoints);

        BCM_MemSet(OptFlowTest_TempMem,0, OPTICALFLOW_TEMP_STORAGE_SIZE);
        BCM_MemSet(optFlowTest_CurrImage,0, OPTICALFLOW_CURRIMAGE_IMG_SIZE);
        BCM_MemSet(OptFlowTest_PrevPyr,0, OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE);
        BCM_MemSet(OptFlowTest_CurrPyr,0, OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE);

        BCM_MemSet(OptFlowTest_Corners,0,sizeof(OptFlowTest_Corners));
        BCM_MemSet(OptFlowTest_PrevPoints,0,sizeof(OptFlowTest_PrevPoints));
        BCM_MemSet(OptFlowTest_CurrPoints,0,sizeof(OptFlowTest_CurrPoints));

        for(i=290;i<322;i++) {
            for(j=402;j<450;j++) {
                optFlowTest_CurrImage[i*1280+j] = 128;
            }
        }
        for(i=496;i<544;i++) {
            for(j=192;j<224;j++) {
                optFlowTest_CurrImage[i*1280+j] = 128;
            }
        }
    } else {

        OptFlowTestImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageAddr = OptFlowTest_PrevPyr;
        OptFlowTestImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE;
        OptFlowTestImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageAddr = OptFlowTest_CurrPyr;
        OptFlowTestImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE;
        OptFlowTestImageInfo[OPTICALFLOW_CORNERS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlowTest_Corners;
        OptFlowTestImageInfo[OPTICALFLOW_CORNERS_ARRAY_IDX].storageSize = sizeof(OptFlowTest_Corners);
        OptFlowTestImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlowTest_PrevPoints;
        OptFlowTestImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlowTest_PrevPoints);
        OptFlowTestImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlowTest_CurrPoints;
        OptFlowTestImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlowTest_CurrPoints);

        BCM_MemSet(optFlowTest_CurrImage,0, OPTICALFLOW_CURRIMAGE_IMG_SIZE);
        for(i=290;i<322;i++) {
            for(j=402;j<450;j++) {
                optFlowTest_CurrImage[(i+4)*1280+j+7] = 128;
            }
        }
        for(i=496;i<544;i++) {
            for(j=192;j<224;j++) {
                optFlowTest_CurrImage[(i+4)*1280+j+7] = 128;
            }
        }
    }

    return BCM_ERR_OK;
}

int32_t OptFlowTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t i;//,j;
//    uint32_t crc;

    ret = BCM_ERR_OK;
     for (i=0;i<OPTICALFLOW_CORNERS_MAX_COUNT;i++) {
         if (OptFlowTest_Corners[i].trackingStatus == OPENVX_LIB_KEY_POINT_DETECTED){
            ULOG_ERR("Corners : %d %d\n",OptFlowTest_Corners[i].x, OptFlowTest_Corners[i].y);
         }
    }
     for (i=0;i<OPTICALFLOW_CORNERS_MAX_COUNT;i++) {
         if (OptFlowTest_CurrPoints[i].trackingStatus != OPENVX_LIB_KEY_POINT_LOST){
            ULOG_ERR("0-TrackedCorners[%d] : %d %d\n", OptFlowTest_CurrPoints[i].trackingStatus,
                            OptFlowTest_CurrPoints[i].x, OptFlowTest_CurrPoints[i].y);
         }
    }
     for (i=0;i<OPTICALFLOW_CORNERS_MAX_COUNT;i++) {
         if (OptFlowTest_PrevPoints[i].trackingStatus != OPENVX_LIB_KEY_POINT_LOST){
            ULOG_ERR("1-TrackedCorners[%d] : %d %d\n", OptFlowTest_PrevPoints[i].trackingStatus,
                            OptFlowTest_PrevPoints[i].x, OptFlowTest_PrevPoints[i].y);
         }
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType OptFlowTestConfig = {OptFlowTest_GetImageInfo, OptFlowTest_GetNumImages, OptFlowTest_PrepareInput, OptFlowTest_ValidateOutput, OptFlowTest_SetMemory, OptFlowTest_FreeMemory, 0xFF18, 2, OPTICALFLOW_UNIQUE_ID};

OpenVXTestConfigType* OptFlowTest_GetTestConfig()
{
    return &OptFlowTestConfig;
}

