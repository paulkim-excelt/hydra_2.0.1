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

#include "test_main.h"
#include "accumulateImg_test_internal.h"

OpenVXLib_StorageInfo AccumulateTestImageInfo[] = {
[ACCUMULATEIMG_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[ACCUMULATEIMG_TEST_TEMP_STORAGE_IDX].storageSize = ACCUMULATEIMG_TEST_TEMP_STORAGE_SIZE,

[ACCUMULATEIMG_TEST_INU8_IMG_IDX].storageAddr = NULL,
[ACCUMULATEIMG_TEST_INU8_IMG_IDX].storageSize = ACCUMULATEIMG_TEST_INU8_IMG_SIZE,

[ACCUMULATEIMG_TEST_ACCUMS16_IMG_IDX].storageAddr = NULL,
[ACCUMULATEIMG_TEST_ACCUMS16_IMG_IDX].storageSize = ACCUMULATEIMG_TEST_ACCUMS16_IMG_SIZE,
};


int32_t AccumulateTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;
    for (i=0;i<(ACCUMULATEIMG_TEST_MAX_DATA_ENTRIES);i++){
            tmpPtr =  OpenVXTest_PoolMemAlloc(AccumulateTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            AccumulateTestImageInfo[i].storageAddr = tmpPtr;
        }
    }
    return BCM_ERR_OK;
}
void AccumulateTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}
const OpenVXLib_StorageInfo* AccumulateTest_GetImageInfo()
{
    return AccumulateTestImageInfo;
}
uint32_t AccumulateTest_GetNumImages()
{
    return ACCUMULATEIMG_TEST_MAX_DATA_ENTRIES;
}

int32_t AccumulateTest_PrepareInput(uint32_t aGraphID)
{
    int32_t ret;
    uint32_t imgSz = 0UL;
    uint8_t *accumulateTest_InImageU8 = AccumulateTestImageInfo[ACCUMULATEIMG_TEST_INU8_IMG_IDX].storageAddr;
    uint8_t *accumulateTest_OutImageS16 = AccumulateTestImageInfo[ACCUMULATEIMG_TEST_ACCUMS16_IMG_IDX].storageAddr;

    ret = OpenVXTest_PTMLoadData((uint16_t)0xFF0C, accumulateTest_InImageU8, ACCUMULATEIMG_TEST_INU8_IMG_SIZE, &imgSz);

    if (ACCUMULATEIMG_TEST_INU8_IMG_SIZE != imgSz) {
        ret = BCM_ERR_UNKNOWN;
        ULOG_ERR(" Accumulate Test Failed in  ptm load for Graph : \n");
    }

    if (0UL == aGraphID) {
        /* memset output images */
        memset(accumulateTest_OutImageS16, 0, ACCUMULATEIMG_TEST_ACCUMS16_IMG_SIZE);
        /* flush the cache */
    }
    if (0UL < aGraphID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t AccumulateTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t *accumulateTest_OutImageS16 = AccumulateTestImageInfo[ACCUMULATEIMG_TEST_ACCUMS16_IMG_IDX].storageAddr;

    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(accumulateTest_OutImageS16,ACCUMULATEIMG_TEST_ACCUMS16_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x453E80FFUL != crc) {
            ULOG_ERR(" Accumulate Test Failed in  accumulateTest_OutImageS16 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }
    /* check for CRC */
    return ret;
}

OpenVXTestConfigType AccumulateTestConfig = {AccumulateTest_GetImageInfo, AccumulateTest_GetNumImages, AccumulateTest_PrepareInput, AccumulateTest_ValidateOutput, AccumulateTest_SetMemory, AccumulateTest_FreeMemory, 0xFF1B, 1, ACCUMULATEIMG_TEST_UNIQUE_ID};

OpenVXTestConfigType* AccumulateImgTest_GetTestConfig()
{
    return &AccumulateTestConfig;
}

