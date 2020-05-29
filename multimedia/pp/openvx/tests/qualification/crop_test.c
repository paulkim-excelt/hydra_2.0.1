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
#include "cropnscale_test_internal.h"


OpenVXLib_StorageInfo CropnScaleTestImageInfo[] = {
[CROPNSCALE_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[CROPNSCALE_TEST_TEMP_STORAGE_IDX].storageSize = CROPNSCALE_TEST_TEMP_STORAGE_SIZE,

[CROPNSCALE_TEST_INU8_IMG_IDX].storageAddr = NULL,
[CROPNSCALE_TEST_INU8_IMG_IDX].storageSize = CROPNSCALE_TEST_INU8_IMG_SIZE,

[CROPNSCALE_TEST_OUTU8_IMG_IDX].storageAddr = NULL,
[CROPNSCALE_TEST_OUTU8_IMG_IDX].storageSize = CROPNSCALE_TEST_OUTU8_IMG_SIZE,

};
const OpenVXLib_StorageInfo* CropnScaleTest_GetImageInfo()
{
    return CropnScaleTestImageInfo;
}
uint32_t CropnScaleTest_GetNumImages()
{
    return CROPNSCALE_TEST_MAX_DATA_ENTRIES;
}

int32_t  CropnScaleTest_SetMemory()
{
    uint8_t *tmpPtr;
    uint32_t i;

    for (i=0;i<CROPNSCALE_TEST_MAX_DATA_ENTRIES;i++){ /*SCALARs are allocated out*/
        tmpPtr = OpenVXTest_PoolMemAlloc(CropnScaleTestImageInfo[i].storageSize );
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            CropnScaleTestImageInfo[i].storageAddr = tmpPtr;
        }
    }
    return BCM_ERR_OK;
}

void CropnScaleTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

int32_t CropnScaleTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t imgSz = 0UL;
    int32_t ret = BCM_ERR_OK;
    uint8_t* CropnScaleTest_InputImage = CropnScaleTestImageInfo[CROPNSCALE_TEST_INU8_IMG_IDX].storageAddr;

    ret = OpenVXTest_PTMLoadData((uint16_t)0xFF20, CropnScaleTest_InputImage, CROPNSCALE_TEST_INU8_IMG_SIZE, &imgSz);
    if (CROPNSCALE_TEST_INU8_IMG_SIZE != imgSz) {
        ULOG_ERR(" loading image fails : %x\n",imgSz);
        ret = BCM_ERR_UNKNOWN;
    }
    return ret;
}

int32_t CropnScaleTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t* CropnScaleTest_OutImage = CropnScaleTestImageInfo[CROPNSCALE_TEST_OUTU8_IMG_IDX].storageAddr;

    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(CropnScaleTest_OutImage,CROPNSCALE_TEST_OUTU8_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0xe16080c1 != crc) {
            ULOG_ERR(" Crop Test Failed for Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType CropnScaleTestConfig = {CropnScaleTest_GetImageInfo, CropnScaleTest_GetNumImages, CropnScaleTest_PrepareInput, CropnScaleTest_ValidateOutput, CropnScaleTest_SetMemory, CropnScaleTest_FreeMemory, 0xFF21, 1, CROPNSCALE_TEST_UNIQUE_ID};

OpenVXTestConfigType* CropnScaleTest_GetTestConfig()
{
    return &CropnScaleTestConfig;
}
