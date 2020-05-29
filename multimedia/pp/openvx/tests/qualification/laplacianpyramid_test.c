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
#include "lp_recon_test_internal.h"


OpenVXLib_StorageInfo LaplacianPyramidTestImageInfo[] = {
[LP_RECON_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[LP_RECON_TEST_TEMP_STORAGE_IDX].storageSize = LP_RECON_TEST_TEMP_STORAGE_SIZE,

[LP_RECON_TEST_FULLIMG_IMG_IDX].storageAddr = NULL,
[LP_RECON_TEST_FULLIMG_IMG_IDX].storageSize = LP_RECON_TEST_FULLIMG_IMG_SIZE,

[LP_RECON_TEST_OUTIMG_IMG_IDX].storageAddr = NULL,
[LP_RECON_TEST_OUTIMG_IMG_IDX].storageSize = LP_RECON_TEST_OUTIMG_IMG_SIZE,

[LP_RECON_TEST_OUTRECONIMG_IMG_IDX].storageAddr = NULL,
[LP_RECON_TEST_OUTRECONIMG_IMG_IDX].storageSize = LP_RECON_TEST_OUTRECONIMG_IMG_SIZE,

[LP_RECON_TEST_LAPLACIAN_PYRAMID_IDX].storageAddr = NULL,
[LP_RECON_TEST_LAPLACIAN_PYRAMID_IDX].storageSize = LP_RECON_TEST_LAPLACIAN_PYRAMID_SIZE,
};


int32_t  LaplacianPyramidTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;

    for (i=0;i<LP_RECON_TEST_MAX_DATA_ENTRIES;i++){
        tmpPtr = OpenVXTest_PoolMemAlloc(LaplacianPyramidTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            LaplacianPyramidTestImageInfo[i].storageAddr = tmpPtr;
        }
    }

    return BCM_ERR_OK;
}

void LaplacianPyramidTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* LaplacianPyramidTest_GetImageInfo()
{
    return LaplacianPyramidTestImageInfo;
}
uint32_t LaplacianPyramidTest_GetNumImages()
{
    return LP_RECON_TEST_MAX_DATA_ENTRIES;
}

int32_t LaplacianPyramidTest_PrepareInput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t imgSz = 0UL;
    //int32_t ret = BCM_ERR_OK;
    //uint32_t i,j;

    uint8_t* laplacePyrTest_fullImageU8 = LaplacianPyramidTestImageInfo[LP_RECON_TEST_FULLIMG_IMG_IDX].storageAddr;
    uint8_t* laplacePyrTest_pyrImageS16 = LaplacianPyramidTestImageInfo[LP_RECON_TEST_LAPLACIAN_PYRAMID_IDX].storageAddr;
    uint8_t* laplacePyrTest_outImgS16 = LaplacianPyramidTestImageInfo[LP_RECON_TEST_OUTIMG_IMG_IDX].storageAddr;
    uint8_t* laplaceReconTest_outImgU8 = LaplacianPyramidTestImageInfo[LP_RECON_TEST_OUTRECONIMG_IMG_IDX].storageAddr;

    /* BCM_MemSet output images */
    BCM_MemSet(laplacePyrTest_fullImageU8, 0, LP_RECON_TEST_FULLIMG_IMG_SIZE);
    BCM_MemSet(laplacePyrTest_pyrImageS16, 0, LP_RECON_TEST_LAPLACIAN_PYRAMID_SIZE);
    BCM_MemSet(laplacePyrTest_outImgS16, 0, LP_RECON_TEST_OUTIMG_IMG_SIZE);
    BCM_MemSet(laplaceReconTest_outImgU8, 0, LP_RECON_TEST_OUTRECONIMG_IMG_SIZE);

    /* set first image */
    /*for (i=0;i<720;i++) {
        for (j=0;j<1280;j++) {
            laplacePyrTest_fullImageU8[i*1280+j] = (i*1280)+j;
        }
    }*/

    ret = OpenVXTest_PTMLoadData((uint16_t)0xFF20, laplacePyrTest_fullImageU8, LP_RECON_TEST_FULLIMG_IMG_SIZE, &imgSz);

    if (LP_RECON_TEST_FULLIMG_IMG_SIZE != imgSz) {
        ULOG_ERR("PTM load fails here %x", imgSz);
        ret = BCM_ERR_UNKNOWN;
    }

    return ret;
}

int32_t LaplacianPyramidTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t* laplacePyrTest_pyrImageS16 = LaplacianPyramidTestImageInfo[LP_RECON_TEST_LAPLACIAN_PYRAMID_IDX].storageAddr;
    uint8_t* laplacePyrTest_outImgS16 = LaplacianPyramidTestImageInfo[LP_RECON_TEST_OUTIMG_IMG_IDX].storageAddr;
    uint8_t* laplaceReconTest_outImgU8 = LaplacianPyramidTestImageInfo[LP_RECON_TEST_OUTRECONIMG_IMG_IDX].storageAddr;
    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(laplacePyrTest_pyrImageS16,LP_RECON_TEST_LAPLACIAN_PYRAMID_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0xb238b707UL != crc) {
            ULOG_ERR(" Laplacian Pyramid Test Failed in  for Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
        crc = BCM_CRC32(laplacePyrTest_outImgS16,LP_RECON_TEST_OUTIMG_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x988dd758UL != crc) {
            ULOG_ERR(" Laplacian Pyramid Test Failed in  for Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
        crc = BCM_CRC32(laplaceReconTest_outImgU8,LP_RECON_TEST_OUTRECONIMG_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x4544b87bUL != crc) {
            ULOG_ERR(" Laplacian recon Test Failed in  for Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
    }
    /* check for CRC */
    return ret;
}

OpenVXTestConfigType LaplacianPyramidTestConfig = {LaplacianPyramidTest_GetImageInfo, LaplacianPyramidTest_GetNumImages,
    LaplacianPyramidTest_PrepareInput, LaplacianPyramidTest_ValidateOutput, LaplacianPyramidTest_SetMemory, LaplacianPyramidTest_FreeMemory, 0xFF1E, 1, LP_RECON_TEST_UNIQUE_ID};

OpenVXTestConfigType* LaplacianPyramidTest_GetTestConfig()
{
    return &LaplacianPyramidTestConfig;
}

