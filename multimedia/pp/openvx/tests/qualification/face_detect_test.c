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
#include "face_detect_internal.h"

FACE_DETECT_NUM_RECTANGLES_SCALAR_TYPE FaceDetectTest_NumRect;

OpenVXLib_StorageInfo FaceDetectTestImageInfo[] = {
[FACE_DETECT_TEMP_STORAGE_IDX].storageAddr = NULL,
[FACE_DETECT_TEMP_STORAGE_IDX].storageSize = FACE_DETECT_TEMP_STORAGE_SIZE,

[FACE_DETECT_INPUT_IMG_IDX].storageAddr = NULL,
[FACE_DETECT_INPUT_IMG_IDX].storageSize = FACE_DETECT_INPUT_IMG_SIZE,

[FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr = NULL,
[FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageSize = sizeof(FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE) * FACE_DETECT_OUTPUT_RECTANGLES_MAX_COUNT,

[FACE_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageAddr = (uint8_t*)&FaceDetectTest_NumRect,
[FACE_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageSize = sizeof(FaceDetectTest_NumRect),
};

int32_t FaceDetectTest_SetMemory()
{
    uint8_t *tmpPtr;
    tmpPtr =  OpenVXTest_PoolMemAlloc(FACE_DETECT_TEMP_STORAGE_SIZE);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        FaceDetectTestImageInfo[FACE_DETECT_TEMP_STORAGE_IDX].storageAddr = tmpPtr;
    }
    tmpPtr = OpenVXTest_PoolMemAlloc(FACE_DETECT_INPUT_IMG_SIZE);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        FaceDetectTestImageInfo[FACE_DETECT_INPUT_IMG_IDX].storageAddr = tmpPtr;
    }
    tmpPtr = OpenVXTest_PoolMemAlloc(sizeof(FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE) * FACE_DETECT_OUTPUT_RECTANGLES_MAX_COUNT);
    if (tmpPtr == NULL){
        return BCM_ERR_NOMEM;
    } else {
        FaceDetectTestImageInfo[FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr = tmpPtr;
    }
    return BCM_ERR_OK;
}
void FaceDetectTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* FaceDetectTest_GetImageInfo()
{
    return FaceDetectTestImageInfo;
}
uint32_t FaceDetectTest_GetNumImages()
{
    return FACE_DETECT_MAX_DATA_ENTRIES;
}

int32_t FaceDetectTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t imgSz = 0UL;
    int32_t ret = BCM_ERR_OK;
    uint8_t *faceDetectTest_InImage = FaceDetectTestImageInfo[FACE_DETECT_INPUT_IMG_IDX].storageAddr;
    FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE *faceDetTest_Rect;

    faceDetTest_Rect = (FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE *)FaceDetectTestImageInfo[ FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr;
    memset(faceDetTest_Rect,0,FaceDetectTestImageInfo[FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageSize);
    ret = OpenVXTest_PTMLoadData((uint16_t)0xFF20, faceDetectTest_InImage, FACE_DETECT_INPUT_IMG_SIZE, &imgSz);

    if (FACE_DETECT_INPUT_IMG_SIZE != imgSz) {
        ret = BCM_ERR_UNKNOWN;
    }

    FaceDetectTest_NumRect = 0;
    return ret;
}

int32_t FaceDetectTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_OK;
    int32_t i;
    FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE *faceDetectTest_Rectangles;

    faceDetectTest_Rectangles = (FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE *)FaceDetectTestImageInfo[FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr;
    ULOG_ERR(" face Detect Test completed : %d\n",FaceDetectTest_NumRect);
    for (i=0; i< FaceDetectTest_NumRect; i++) {
        ULOG_INFO("Found face @ (%d, %d), size (%d, %d) \n",
                        faceDetectTest_Rectangles[i].startX,
                        faceDetectTest_Rectangles[i].startY,
                        faceDetectTest_Rectangles[i].endX - faceDetectTest_Rectangles[i].startX,
                        faceDetectTest_Rectangles[i].endY - faceDetectTest_Rectangles[i].startY);
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType FaceDetectTestConfig = {FaceDetectTest_GetImageInfo, FaceDetectTest_GetNumImages, FaceDetectTest_PrepareInput, FaceDetectTest_ValidateOutput, FaceDetectTest_SetMemory, FaceDetectTest_FreeMemory, 0xFF1F, 1, FACE_DETECT_UNIQUE_ID};

OpenVXTestConfigType* FaceDetectTest_GetTestConfig()
{
    return &FaceDetectTestConfig;
}

