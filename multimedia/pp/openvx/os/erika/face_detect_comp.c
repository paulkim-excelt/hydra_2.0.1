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

#include <ovx_interface.h>

#include <face_detect_internal.h>

OpenVXLib_StorageInfo FaceDetect_ImageInfo[FACE_DETECT_MAX_DATA_ENTRIES];

COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.data") FaceDetect_TempMem[FACE_DETECT_TEMP_STORAGE_SIZE];
COMP_ALIGN(256)
FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE
 COMP_SECTION(".dram.test.data") FaceDetect_Rectangles[FACE_DETECT_OUTPUT_RECTANGLES_MAX_COUNT];
FACE_DETECT_NUM_RECTANGLES_SCALAR_TYPE FaceDetect_NumRectangles;

#define FACE_DETECT_GRAPH_INFO_SIZE (80UL*1024UL) /* scl-1.2, min=30 ,4 cores takes 56kbytes */
COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.FaceDetect_GraphInfo") FaceDetect_GraphInfo[FACE_DETECT_GRAPH_INFO_SIZE];


int32_t FaceDetect_Initialize()
{
    FaceDetect_NumRectangles = 0;
    return BCM_ERR_OK;
}

int32_t FaceDetect_UpdateConfig(const uint8_t *aData, uint32_t aSize)
{
    BCM_MemCpy(FaceDetect_GraphInfo, aData, aSize);
    return BCM_ERR_OK;
}

uint32_t FaceDetect_GetGraphSize()
{
    return FACE_DETECT_GRAPH_INFO_SIZE;
}

uint8_t* FaceDetect_GetGraph()
{
    return FaceDetect_GraphInfo;
}

uint32_t FaceDetect_GetNumStorages()
{
    return FACE_DETECT_MAX_DATA_ENTRIES;
}

OpenVXLib_StorageInfo* FaceDetect_GetStorageInfo(uint8_t *aInImage, uint32_t aJobIdx)
{
    FaceDetect_ImageInfo[FACE_DETECT_TEMP_STORAGE_IDX].storageAddr = FaceDetect_TempMem;
    FaceDetect_ImageInfo[FACE_DETECT_TEMP_STORAGE_IDX].storageSize = FACE_DETECT_TEMP_STORAGE_SIZE;
    FaceDetect_ImageInfo[FACE_DETECT_INPUT_IMG_IDX].storageAddr = aInImage;
    FaceDetect_ImageInfo[FACE_DETECT_INPUT_IMG_IDX].storageSize = FACE_DETECT_INPUT_IMG_SIZE;
    FaceDetect_ImageInfo[FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr = (uint8_t*)FaceDetect_Rectangles;
    FaceDetect_ImageInfo[FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageSize = sizeof(FaceDetect_Rectangles);
    FaceDetect_ImageInfo[FACE_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageAddr = (uint8_t*)&FaceDetect_NumRectangles;
    FaceDetect_ImageInfo[FACE_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageSize = sizeof(FaceDetect_NumRectangles);
    return FaceDetect_ImageInfo;
}

int32_t FaceDetect_Render(uint32_t aJobIdx)
{
    uint32_t i;
    int32_t num_rect = FaceDetect_NumRectangles;

    for (i = 0UL; i < num_rect; i++) {
        ULOG_INFO("[%u/%u]Face @ (%d, %d) (%d, %d)\n", i + 1UL, num_rect,
                        FaceDetect_Rectangles[i].startX,
                        FaceDetect_Rectangles[i].startY,
                        FaceDetect_Rectangles[i].endX,
                        FaceDetect_Rectangles[i].endY);
        drawline(1, FaceDetect_Rectangles[i].startX, FaceDetect_Rectangles[i].startY,
                        FaceDetect_Rectangles[i].endX, FaceDetect_Rectangles[i].startY);

        drawline(1, FaceDetect_Rectangles[i].startX, FaceDetect_Rectangles[i].startY,
                        FaceDetect_Rectangles[i].startX, FaceDetect_Rectangles[i].endY);

        drawline(1, FaceDetect_Rectangles[i].startX, FaceDetect_Rectangles[i].endY,
                        FaceDetect_Rectangles[i].endX, FaceDetect_Rectangles[i].endY);

        drawline(1, FaceDetect_Rectangles[i].endX, FaceDetect_Rectangles[i].startY,
                        FaceDetect_Rectangles[i].endX, FaceDetect_Rectangles[i].endY);
    }
    FaceDetect_NumRectangles = 0;
    return num_rect;
}

const OpenVX_ComponentType FaceDetect_Component = {
    .initialize = FaceDetect_Initialize,
    .updateConfig = FaceDetect_UpdateConfig,
    .getGraph = FaceDetect_GetGraph,
    .getGraphSize = FaceDetect_GetGraphSize,
    .getStorageInfo = FaceDetect_GetStorageInfo,
    .getNumStorages = FaceDetect_GetNumStorages,
    .render = FaceDetect_Render,
    .uniqueID = FACE_DETECT_UNIQUE_ID
};


const OpenVX_ComponentType *OVX0_GetComponent()
{
    return &FaceDetect_Component;
}
