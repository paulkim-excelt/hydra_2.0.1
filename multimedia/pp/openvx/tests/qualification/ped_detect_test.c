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

#include "test_main.h"
#include "pedestrian_detect_internal.h"

PEDESTRIAN_DETECT_NUM_RECTANGLES_SCALAR_TYPE PedDetectTest_NumRect;

OpenVXLib_StorageInfo PedDetectTestImageInfo[] = {
[PEDESTRIAN_DETECT_TEMP_STORAGE_IDX].storageAddr = NULL,
[PEDESTRIAN_DETECT_TEMP_STORAGE_IDX].storageSize = PEDESTRIAN_DETECT_TEMP_STORAGE_SIZE,

[PEDESTRIAN_DETECT_INPUT_IMG_IDX].storageAddr = NULL,
[PEDESTRIAN_DETECT_INPUT_IMG_IDX].storageSize = PEDESTRIAN_DETECT_INPUT_IMG_SIZE,

[PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr = NULL,
[PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageSize = sizeof(PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE) * PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_MAX_COUNT,

[PEDESTRIAN_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageAddr = (uint8_t*)&PedDetectTest_NumRect,
[PEDESTRIAN_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageSize = sizeof(PedDetectTest_NumRect),
};

int32_t PedDetectTest_SetMemory()
{
    uint8_t *tmpPtr;
    tmpPtr =  OpenVXTest_PoolMemAlloc(PEDESTRIAN_DETECT_TEMP_STORAGE_SIZE);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        PedDetectTestImageInfo[PEDESTRIAN_DETECT_TEMP_STORAGE_IDX].storageAddr = tmpPtr;
    }
    tmpPtr = OpenVXTest_PoolMemAlloc(PEDESTRIAN_DETECT_INPUT_IMG_SIZE);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        PedDetectTestImageInfo[PEDESTRIAN_DETECT_INPUT_IMG_IDX].storageAddr = tmpPtr;
    }
    tmpPtr = OpenVXTest_PoolMemAlloc(sizeof(PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE) * PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_MAX_COUNT);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        PedDetectTestImageInfo[PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr = tmpPtr;
    }
    return BCM_ERR_OK;
}
void PedDetectTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* PedDetectTest_GetImageInfo()
{
    return PedDetectTestImageInfo;
}
uint32_t PedDetectTest_GetNumImages()
{
    return PEDESTRIAN_DETECT_MAX_DATA_ENTRIES;
}

int32_t PedDetectTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;
    uint8_t *pedDetectTest_InImage = PedDetectTestImageInfo[PEDESTRIAN_DETECT_INPUT_IMG_IDX].storageAddr;

    /* set first image */
    for (i=0;i<720;i++) {
        for (j=0;j<1280;j++) {
            pedDetectTest_InImage[i*1280+j] = 0;
        }
    }
    PedDetectTest_NumRect = 0;
    return BCM_ERR_OK;
}

int32_t PedDetectTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_OK;
    int32_t i;
    PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE *pedDetectTest_Rectangles;

    pedDetectTest_Rectangles = (PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE *)PedDetectTestImageInfo[PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr;
    ULOG_ERR(" Pedestrian Detect Test completed : %d\n",PedDetectTest_NumRect);
    for (i=0; i< PedDetectTest_NumRect; i++) {
        ULOG_INFO("Found Pedestrian @ (%d, %d), size (%d, %d) \n",
                        pedDetectTest_Rectangles[i].startX,
                        pedDetectTest_Rectangles[i].startY,
                        pedDetectTest_Rectangles[i].endX - pedDetectTest_Rectangles[i].startX,
                        pedDetectTest_Rectangles[i].endY - pedDetectTest_Rectangles[i].startY);
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType PedDetectTestConfig = {PedDetectTest_GetImageInfo, PedDetectTest_GetNumImages, PedDetectTest_PrepareInput, PedDetectTest_ValidateOutput, PedDetectTest_SetMemory, PedDetectTest_FreeMemory, 0xFF16, 1, PEDESTRIAN_DETECT_UNIQUE_ID};

OpenVXTestConfigType* PedDetectTest_GetTestConfig()
{
    return &PedDetectTestConfig;
}

