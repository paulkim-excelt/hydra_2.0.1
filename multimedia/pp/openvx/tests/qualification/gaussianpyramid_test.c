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
#include "pyramid_test_internal.h"


OpenVXLib_StorageInfo GaussianPyramidTestImageInfo[] = {
[PYRAMID_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[PYRAMID_TEST_TEMP_STORAGE_IDX].storageSize = PYRAMID_TEST_TEMP_STORAGE_SIZE,

[PYRAMID_TEST_FULLIMAGE_IMG_IDX].storageAddr = NULL,
[PYRAMID_TEST_FULLIMAGE_IMG_IDX].storageSize = PYRAMID_TEST_FULLIMAGE_IMG_SIZE,

[PYRAMID_TEST_IMGPYRAMIDORB_PYRAMID_IDX].storageAddr = NULL,
[PYRAMID_TEST_IMGPYRAMIDORB_PYRAMID_IDX].storageSize = PYRAMID_TEST_IMGPYRAMIDORB_PYRAMID_SIZE,

[PYRAMID_TEST_IMGPYRAMIDHALFSCALE_PYRAMID_IDX].storageAddr = NULL,
[PYRAMID_TEST_IMGPYRAMIDHALFSCALE_PYRAMID_IDX].storageSize = PYRAMID_TEST_IMGPYRAMIDHALFSCALE_PYRAMID_SIZE,
};


int32_t  GaussianPyramidTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;

    for (i=0;i<PYRAMID_TEST_MAX_DATA_ENTRIES;i++){
        tmpPtr = OpenVXTest_PoolMemAlloc(GaussianPyramidTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            GaussianPyramidTestImageInfo[i].storageAddr = tmpPtr;
        }
    }

    return BCM_ERR_OK;
}

void GaussianPyramidTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* GaussianPyramidTest_GetImageInfo()
{
    return GaussianPyramidTestImageInfo;
}
uint32_t GaussianPyramidTest_GetNumImages()
{
    return PYRAMID_TEST_MAX_DATA_ENTRIES;
}

int32_t GaussianPyramidTest_PrepareInput(uint32_t aGraphID)
{
   /* int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t imgSz;*/
    int32_t ret = BCM_ERR_OK;
    uint32_t i,j;

    uint8_t* gaussianPyramidTest_fullImageU8 = GaussianPyramidTestImageInfo[PYRAMID_TEST_FULLIMAGE_IMG_IDX].storageAddr;
    uint8_t* gaussianPyramidTest_pyrImageU8 = GaussianPyramidTestImageInfo[PYRAMID_TEST_IMGPYRAMIDHALFSCALE_PYRAMID_IDX].storageAddr;
    uint8_t* gaussianPyramidTest_pyrORBImageU8 = GaussianPyramidTestImageInfo[PYRAMID_TEST_IMGPYRAMIDORB_PYRAMID_IDX].storageAddr;

    /* BCM_MemSet output images */
    BCM_MemSet(gaussianPyramidTest_pyrORBImageU8, 0, PYRAMID_TEST_IMGPYRAMIDORB_PYRAMID_SIZE);
    BCM_MemSet(gaussianPyramidTest_pyrImageU8, 0, PYRAMID_TEST_IMGPYRAMIDHALFSCALE_PYRAMID_SIZE);
    BCM_MemSet(gaussianPyramidTest_fullImageU8, 0, PYRAMID_TEST_FULLIMAGE_IMG_SIZE);

    /* set first image */
    for (i=0;i<720;i++) {
        for (j=0;j<1280;j++) {
            gaussianPyramidTest_fullImageU8[i*1280+j] = (i*1280)+j;
        }
    }
    /*ret = OpenVXTest_PTMLoadData((uint16_t)0xFF1D, gaussianPyramidTest_fullImageU8, PYRAMID_TEST_FULLIMAGE_IMG_SIZE, &imgSz);

    if (PYRAMID_TEST_FULLIMAGE_IMG_SIZE != imgSz) {
        ULOG_ERR("PTM load fails here %x", imgSz);
        ret = BCM_ERR_UNKNOWN;
    }*/

    return ret;
}

int32_t GaussianPyramidTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint32_t i,j;
    uint8_t* gaussianPyramidTest_pyrImageU8 = GaussianPyramidTestImageInfo[PYRAMID_TEST_IMGPYRAMIDHALFSCALE_PYRAMID_IDX].storageAddr;
    uint8_t* gaussianPyramidTest_pyrORBImageU8 = GaussianPyramidTestImageInfo[PYRAMID_TEST_IMGPYRAMIDORB_PYRAMID_IDX].storageAddr;
    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(gaussianPyramidTest_pyrImageU8,PYRAMID_TEST_IMGPYRAMIDHALFSCALE_PYRAMID_SIZE,OPENVX_TEST_CRC32_POLY);
        /* set the stride to zeros */
        for (i=0;i<605;i++) {
            for (j=1076;j<1152;j++) {
                gaussianPyramidTest_pyrORBImageU8[i*1152+j+PYRAMID_TEST_FULLIMAGE_IMG_SIZE] = 0;
            }
        }
        /* set the stride to zeros */
        for (i=0;i<509;i++) {
            for (j=905;j<1024;j++) {
                gaussianPyramidTest_pyrORBImageU8[i*1024+j+PYRAMID_TEST_FULLIMAGE_IMG_SIZE+0xAA280] = 0;
            }
        }
        if (0xCE4C7322UL != crc) {
            ULOG_ERR(" Gaussian Pyramid Test Failed in  gaussianPyramidTest HalfScale for Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
        crc = BCM_CRC32(gaussianPyramidTest_pyrORBImageU8,PYRAMID_TEST_IMGPYRAMIDORB_PYRAMID_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0xe4922673UL != crc) {
            ULOG_ERR(" Gaussian Pyramid Test Failed in  gaussianPyramidTest ORB for Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType GaussianPyramidTestConfig = {GaussianPyramidTest_GetImageInfo, GaussianPyramidTest_GetNumImages,
    GaussianPyramidTest_PrepareInput, GaussianPyramidTest_ValidateOutput, GaussianPyramidTest_SetMemory, GaussianPyramidTest_FreeMemory, 0xFF1C, 1, PYRAMID_TEST_UNIQUE_ID};

OpenVXTestConfigType* GaussianPyramidTest_GetTestConfig()
{
    return &GaussianPyramidTestConfig;
}

