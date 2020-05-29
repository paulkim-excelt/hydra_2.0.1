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

#include <opticalflow_internal.h>

OpenVXLib_StorageInfo OptFlow_ImageInfo[OPTICALFLOW_MAX_DATA_ENTRIES];
uint32_t OptFlow_GraphCounter;

COMP_ALIGN(256)
uint8_t COMP_SECTION(".openvx.uncached.OptFlow_TempMem") OptFlow_TempMem[OPTICALFLOW_TEMP_STORAGE_SIZE];
COMP_ALIGN(256)
uint8_t COMP_SECTION(".openvx.uncached.OptFlow_PrevPyr") OptFlow_PrevPyr[OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE];
COMP_ALIGN(256)
uint8_t COMP_SECTION(".openvx.uncached.OptFlow_CurrPyr") OptFlow_CurrPyr[OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE];
COMP_ALIGN(256)
OpenVXLib_KeyPoint COMP_SECTION(".pp.cached.OptFlow_Corners")  OptFlow_Corners[OPTICALFLOW_CORNERS_MAX_COUNT];
COMP_ALIGN(256)
OpenVXLib_KeyPoint COMP_SECTION(".pp.cached.OptFlow_PrevPoints")  OptFlow_PrevPoints[OPTICALFLOW_PREVPOINTS_MAX_COUNT];
COMP_ALIGN(256)
OpenVXLib_KeyPoint COMP_SECTION(".pp.cached.OptFlow_CurrPoints")  OptFlow_CurrPoints[OPTICALFLOW_CURRPOINTS_MAX_COUNT];


#define OPTICALFLOW_GRAPH_INFO_SIZE (128UL*1024UL)
COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.OptFlow_GraphInfo") OptFlow_GraphInfo[OPTICALFLOW_GRAPH_INFO_SIZE];


int32_t OptFlow_Initialize()
{
    BCM_MemSet(OptFlow_TempMem, 0, OPTICALFLOW_TEMP_STORAGE_SIZE);
    BCM_MemSet(OptFlow_PrevPyr, 0, OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE);
    BCM_MemSet(OptFlow_CurrPyr, 0, OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE);
    BCM_MemSet(OptFlow_Corners, 0, sizeof(OptFlow_Corners));
    BCM_MemSet(OptFlow_PrevPoints, 0, sizeof(OptFlow_PrevPoints));
    BCM_MemSet(OptFlow_CurrPoints, 0, sizeof(OptFlow_CurrPoints));
    OptFlow_GraphCounter = 0UL;
    return BCM_ERR_OK;
}

int32_t OptFlow_UpdateConfig(const uint8_t *aData, uint32_t aSize)
{
    BCM_MemCpy(OptFlow_GraphInfo, aData, aSize);
    return BCM_ERR_OK;
}

uint32_t OptFlow_GetGraphSize()
{
    return OPTICALFLOW_GRAPH_INFO_SIZE;
}

uint8_t* OptFlow_GetGraph()
{
    return OptFlow_GraphInfo;
}

uint32_t OptFlow_GetNumStorages()
{
    return OPTICALFLOW_MAX_DATA_ENTRIES;
}

OpenVXLib_StorageInfo* OptFlow_GetStorageInfo(uint8_t *aInImage, uint32_t aJobIdx)
{
    OptFlow_ImageInfo[OPTICALFLOW_TEMP_STORAGE_IDX].storageAddr = OptFlow_TempMem;
    OptFlow_ImageInfo[OPTICALFLOW_TEMP_STORAGE_IDX].storageSize = OPTICALFLOW_TEMP_STORAGE_SIZE;
    OptFlow_ImageInfo[OPTICALFLOW_CURRIMAGE_IMG_IDX].storageAddr = aInImage;
    OptFlow_ImageInfo[OPTICALFLOW_CURRIMAGE_IMG_IDX].storageSize = OPTICALFLOW_CURRIMAGE_IMG_SIZE;
    OptFlow_ImageInfo[OPTICALFLOW_CORNERS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlow_Corners;
    OptFlow_ImageInfo[OPTICALFLOW_CORNERS_ARRAY_IDX].storageSize = sizeof(OptFlow_Corners);
    if (0UL == (aJobIdx & 1UL)) {
        BCM_MemSet(OptFlow_CurrPoints, 0, sizeof(OptFlow_CurrPoints));
        OptFlow_ImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageAddr = OptFlow_PrevPyr;
        OptFlow_ImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE;
        OptFlow_ImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageAddr = OptFlow_CurrPyr;
        OptFlow_ImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE;
        OptFlow_ImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlow_PrevPoints;
        OptFlow_ImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlow_PrevPoints);
        OptFlow_ImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)&OptFlow_CurrPoints;
        OptFlow_ImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlow_CurrPoints);
    } else {
        BCM_MemSet(OptFlow_PrevPoints, 0, sizeof(OptFlow_PrevPoints));
        OptFlow_ImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageAddr = OptFlow_PrevPyr;
        OptFlow_ImageInfo[OPTICALFLOW_CURRPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_PREVPYRAMID_PYRAMID_SIZE;
        OptFlow_ImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageAddr = OptFlow_CurrPyr;
        OptFlow_ImageInfo[OPTICALFLOW_PREVPYRAMID_PYRAMID_IDX].storageSize = OPTICALFLOW_CURRPYRAMID_PYRAMID_SIZE;
        OptFlow_ImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)OptFlow_PrevPoints;
        OptFlow_ImageInfo[OPTICALFLOW_CURRPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlow_PrevPoints);
        OptFlow_ImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageAddr = (uint8_t*)&OptFlow_CurrPoints;
        OptFlow_ImageInfo[OPTICALFLOW_PREVPOINTS_ARRAY_IDX].storageSize = sizeof(OptFlow_CurrPoints);
    }
    return OptFlow_ImageInfo;
}

int32_t OptFlow_Render(uint32_t aIdx)
{
    uint32_t i;
    int32_t numDetect = 0;
    int32_t numTrack = 0;
    OpenVXLib_KeyPoint *prevPoints = NULL;
    OpenVXLib_KeyPoint *currPoints = NULL;

    if (0UL == (aIdx & 1UL)) {
        prevPoints = OptFlow_PrevPoints;
        currPoints = OptFlow_CurrPoints;
    } else {
        currPoints = OptFlow_PrevPoints;
        prevPoints = OptFlow_CurrPoints;
    }

    for (i = 0UL; i < OPTICALFLOW_CORNERS_MAX_COUNT; i++) {
        if (OptFlow_Corners[i].trackingStatus == OPENVX_LIB_KEY_POINT_DETECTED) {
            numDetect++;
        }
    }

    for (i = 0UL; i < OPTICALFLOW_PREVPOINTS_MAX_COUNT; i++) {
        if (currPoints[i].trackingStatus == OPENVX_LIB_KEY_POINT_TRACKED) {
            int32_t x = prevPoints[i].x;
            int32_t y = prevPoints[i].y;
            int32_t p = currPoints[i].x;
            int32_t q = currPoints[i].y;
            drawline(2, x, y, p, q);
            drawline(1, p-2, q-2, p+2, q-2);
            drawline(1, p-2, q+2, p+2, q+2);
            drawline(1, p-2, q-2, p-2, q+2);
            drawline(1, p+2, q-2, p+2, q+2);
            numTrack++;
        }
    }
    ULOG_INFO("numDetect(%d) + numTrack(%d)\n", numDetect, numTrack);
    return numDetect + numTrack;
}

const OpenVX_ComponentType OptFlow_Component = {
    .initialize = OptFlow_Initialize,
    .updateConfig = OptFlow_UpdateConfig,
    .getGraph = OptFlow_GetGraph,
    .getGraphSize = OptFlow_GetGraphSize,
    .getStorageInfo = OptFlow_GetStorageInfo,
    .getNumStorages = OptFlow_GetNumStorages,
    .render = OptFlow_Render,
    .uniqueID = OPTICALFLOW_UNIQUE_ID
};


const OpenVX_ComponentType *OVX0_GetComponent()
{
    return &OptFlow_Component;
}
