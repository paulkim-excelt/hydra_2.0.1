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
#include "integralImg_test_internal.h"


OpenVXLib_StorageInfo IntegralImgTestImageInfo[] = {
[INTEGRALIMG_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[INTEGRALIMG_TEST_TEMP_STORAGE_IDX].storageSize = INTEGRALIMG_TEST_TEMP_STORAGE_SIZE,

[INTEGRALIMG_TEST_INU8_IMG_IDX].storageAddr = NULL,
[INTEGRALIMG_TEST_INU8_IMG_IDX].storageSize = INTEGRALIMG_TEST_INU8_IMG_SIZE,

[INTEGRALIMG_TEST_OUTU32_IMG_IDX].storageAddr = NULL,
[INTEGRALIMG_TEST_OUTU32_IMG_IDX].storageSize = INTEGRALIMG_TEST_OUTU32_IMG_SIZE,

};


int32_t  IntegralImgTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;

    for (i=0;i<INTEGRALIMG_TEST_MAX_DATA_ENTRIES;i++){
        tmpPtr = OpenVXTest_PoolMemAlloc(IntegralImgTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            IntegralImgTestImageInfo[i].storageAddr = tmpPtr;
        }
    }

    return BCM_ERR_OK;
}

void IntegralImgTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* IntegralImgTest_GetImageInfo()
{
    return IntegralImgTestImageInfo;
}
uint32_t IntegralImgTest_GetNumImages()
{
    return INTEGRALIMG_TEST_MAX_DATA_ENTRIES;
}

int32_t IntegralImgTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;

    uint8_t* integralImgTest_InputImageU8 = IntegralImgTestImageInfo[INTEGRALIMG_TEST_INU8_IMG_IDX].storageAddr;
    if (0UL == aGraphID) {
        /* set first image */
        for (i=0;i<480;i++) {
            for (j=0;j<640;j++) {
                integralImgTest_InputImageU8[i*640+j] = i+j;
            }
        }
    }
    return BCM_ERR_OK;
}

int32_t IntegralImgTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t* integralImgTest_OutImageU32 = IntegralImgTestImageInfo[INTEGRALIMG_TEST_OUTU32_IMG_IDX].storageAddr;
    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(integralImgTest_OutImageU32,INTEGRALIMG_TEST_OUTU32_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x99253340UL != crc) {
            ULOG_ERR(" IntegralImg Test Failed in  integralImgTest_OutImageU32 for Graph : %x\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType IntegralImgTestConfig = {IntegralImgTest_GetImageInfo, IntegralImgTest_GetNumImages,
    IntegralImgTest_PrepareInput, IntegralImgTest_ValidateOutput, IntegralImgTest_SetMemory, IntegralImgTest_FreeMemory, 0xFF1A, 1, INTEGRALIMG_TEST_UNIQUE_ID};

OpenVXTestConfigType* IntegralImgTest_GetTestConfig()
{
    return &IntegralImgTestConfig;
}

