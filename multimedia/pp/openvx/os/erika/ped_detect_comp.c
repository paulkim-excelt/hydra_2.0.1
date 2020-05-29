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

#include <pedestrian_detect_internal.h>

OpenVXLib_StorageInfo PedDetect_ImageInfo[PEDESTRIAN_DETECT_MAX_DATA_ENTRIES];

COMP_ALIGN(256)
uint8_t COMP_SECTION(".dram.test.data") PedDetect_TempMem[PEDESTRIAN_DETECT_TEMP_STORAGE_SIZE];
COMP_ALIGN(256)
PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE
 COMP_SECTION(".dram.test.data") PedDetect_Rectangles[PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_MAX_COUNT];
PEDESTRIAN_DETECT_NUM_RECTANGLES_SCALAR_TYPE PedDetect_NumRectangles;

#define PED_DETECT_GRAPH_INFO_SIZE (192UL*1024UL)
COMP_ALIGN(256)
uint8_t COMP_SECTION(".pp.uncached.PedDetect_GraphInfo") PedDetect_GraphInfo[PED_DETECT_GRAPH_INFO_SIZE];


int32_t PedDetect_Initialize()
{
    PedDetect_NumRectangles = 0;
    return BCM_ERR_OK;
}

int32_t PedDetect_UpdateConfig(const uint8_t *aData, uint32_t aSize)
{
    BCM_MemCpy(PedDetect_GraphInfo, aData, aSize);
    return BCM_ERR_OK;
}

uint32_t PedDetect_GetGraphSize()
{
    return PED_DETECT_GRAPH_INFO_SIZE;
}

uint8_t* PedDetect_GetGraph()
{
    return PedDetect_GraphInfo;
}

uint32_t PedDetect_GetNumStorages()
{
    return PEDESTRIAN_DETECT_MAX_DATA_ENTRIES;
}

OpenVXLib_StorageInfo* PedDetect_GetStorageInfo(uint8_t *aInImage, uint32_t aJobIdx)
{
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_TEMP_STORAGE_IDX].storageAddr = PedDetect_TempMem;
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_TEMP_STORAGE_IDX].storageSize = PEDESTRIAN_DETECT_TEMP_STORAGE_SIZE;
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_INPUT_IMG_IDX].storageAddr = aInImage;
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_INPUT_IMG_IDX].storageSize = PEDESTRIAN_DETECT_INPUT_IMG_SIZE;
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageAddr = (uint8_t*)PedDetect_Rectangles;
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX].storageSize = sizeof(PedDetect_Rectangles);
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageAddr = (uint8_t*)&PedDetect_NumRectangles;
    PedDetect_ImageInfo[PEDESTRIAN_DETECT_NUM_RECTANGLES_SCALAR_IDX].storageSize = sizeof(PedDetect_NumRectangles);
    return PedDetect_ImageInfo;
}

#define SCALE_FACTOR (2UL)
int32_t PedDetect_Render(uint32_t aJobIdx)
{
    uint32_t i;
    int32_t num_rect = PedDetect_NumRectangles;

    for (i = 0UL; i < num_rect; i++) {
        ULOG_INFO("[%u/%u]Pedestrian @ (%d, %d) (%d, %d)\n", i + 1UL, num_rect,
                        SCALE_FACTOR*PedDetect_Rectangles[i].startX,
                        SCALE_FACTOR*PedDetect_Rectangles[i].startY,
                        SCALE_FACTOR*PedDetect_Rectangles[i].endX,
                        SCALE_FACTOR*PedDetect_Rectangles[i].endY);
        drawline(1, SCALE_FACTOR*PedDetect_Rectangles[i].startX, SCALE_FACTOR*PedDetect_Rectangles[i].startY,
                        SCALE_FACTOR*PedDetect_Rectangles[i].endX, SCALE_FACTOR*PedDetect_Rectangles[i].startY);

        drawline(1, SCALE_FACTOR*PedDetect_Rectangles[i].startX, SCALE_FACTOR*PedDetect_Rectangles[i].startY,
                        SCALE_FACTOR*PedDetect_Rectangles[i].startX, SCALE_FACTOR*PedDetect_Rectangles[i].endY);

        drawline(1, SCALE_FACTOR*PedDetect_Rectangles[i].startX, SCALE_FACTOR*PedDetect_Rectangles[i].endY,
                        SCALE_FACTOR*PedDetect_Rectangles[i].endX, SCALE_FACTOR*PedDetect_Rectangles[i].endY);

        drawline(1, SCALE_FACTOR*PedDetect_Rectangles[i].endX, SCALE_FACTOR*PedDetect_Rectangles[i].startY,
                        SCALE_FACTOR*PedDetect_Rectangles[i].endX, SCALE_FACTOR*PedDetect_Rectangles[i].endY);
    }
    PedDetect_NumRectangles = 0;
    return num_rect;
}

const OpenVX_ComponentType PedDetect_Component = {
    .initialize = PedDetect_Initialize,
    .updateConfig = PedDetect_UpdateConfig,
    .getGraph = PedDetect_GetGraph,
    .getGraphSize = PedDetect_GetGraphSize,
    .getStorageInfo = PedDetect_GetStorageInfo,
    .getNumStorages = PedDetect_GetNumStorages,
    .render = PedDetect_Render,
    .uniqueID = PEDESTRIAN_DETECT_UNIQUE_ID
};


const OpenVX_ComponentType *OVX0_GetComponent()
{
    return &PedDetect_Component;
}
