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
#include "bitdepthConv_test_internal.h"


BITDEPTHCONV_TEST_SCALAR_VAL_SCALAR_TYPE BitdepthConvTest_ScalarInt32;

OpenVXLib_StorageInfo BitdepthConvTestImageInfo[] = {
[BITDEPTHCONV_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[BITDEPTHCONV_TEST_TEMP_STORAGE_IDX].storageSize = BITDEPTHCONV_TEST_TEMP_STORAGE_SIZE,

[BITDEPTHCONV_TEST_INU8_IMG_IDX].storageAddr = NULL,
[BITDEPTHCONV_TEST_INU8_IMG_IDX].storageSize = BITDEPTHCONV_TEST_INU8_IMG_SIZE,

[BITDEPTHCONV_TEST_OUTU8_IMG_IDX].storageAddr = NULL,
[BITDEPTHCONV_TEST_OUTU8_IMG_IDX].storageSize = BITDEPTHCONV_TEST_OUTU8_IMG_SIZE,

[BITDEPTHCONV_TEST_INS16_IMG_IDX].storageAddr = NULL,
[BITDEPTHCONV_TEST_INS16_IMG_IDX].storageSize = BITDEPTHCONV_TEST_INS16_IMG_SIZE,

[BITDEPTHCONV_TEST_OUTS16_IMG_IDX].storageAddr = NULL,
[BITDEPTHCONV_TEST_OUTS16_IMG_IDX].storageSize = BITDEPTHCONV_TEST_OUTS16_IMG_SIZE,

[BITDEPTHCONV_TEST_SCALAR_VAL_SCALAR_IDX].storageAddr = (uint8_t*)&BitdepthConvTest_ScalarInt32,
[BITDEPTHCONV_TEST_SCALAR_VAL_SCALAR_IDX].storageSize = sizeof(BitdepthConvTest_ScalarInt32),

};
const OpenVXLib_StorageInfo* BitdepthConvTest_GetImageInfo()
{
    return BitdepthConvTestImageInfo;
}
uint32_t BitdepthConvTest_GetNumImages()
{
    return BITDEPTHCONV_TEST_MAX_DATA_ENTRIES;
}

int32_t BitdepthConvTest_SetMemory()
{
    uint8_t *tmpPtr;
    uint32_t i;

    for (i=0;i<(BITDEPTHCONV_TEST_MAX_DATA_ENTRIES-1);i++){ /*Scalars are allocated outside */
        tmpPtr = OpenVXTest_PoolMemAlloc(BitdepthConvTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            BitdepthConvTestImageInfo[i].storageAddr = tmpPtr;
        }
    }
    return BCM_ERR_OK;
}

void BitdepthConvTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}




int32_t BitdepthConvTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;
    int16_t *s16In;
    uint8_t *bitdepthConvTest_InputImageU8 = BitdepthConvTestImageInfo[BITDEPTHCONV_TEST_INU8_IMG_IDX].storageAddr;
    uint8_t *bitdepthConvTest_InputImageS16 = BitdepthConvTestImageInfo[BITDEPTHCONV_TEST_INS16_IMG_IDX].storageAddr;
    uint8_t *bitdepthConvTest_OutImageU8 = BitdepthConvTestImageInfo[BITDEPTHCONV_TEST_OUTU8_IMG_IDX].storageAddr;
    uint8_t *bitdepthConvTest_OutImageS16 = BitdepthConvTestImageInfo[BITDEPTHCONV_TEST_OUTS16_IMG_IDX].storageAddr;

    s16In = (int16_t*)bitdepthConvTest_InputImageS16;

    if (0UL == aGraphID) {
        /* BCM_MemSet the output images */
        BCM_MemSet(bitdepthConvTest_OutImageU8, 0, BITDEPTHCONV_TEST_OUTU8_IMG_SIZE);
        BCM_MemSet(bitdepthConvTest_OutImageS16, 0, BITDEPTHCONV_TEST_OUTS16_IMG_SIZE);

        /* set first image */
        for (i=0;i<480;i++) {
            for (j=0;j<640;j++) {
                bitdepthConvTest_InputImageU8[i*640+j] = i+j;
                s16In[i*640+j] = (i*640) + j;
            }
        }
    }
    return BCM_ERR_OK;
}

int32_t BitdepthConvTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t *bitdepthConvTest_OutImageU8 = BitdepthConvTestImageInfo[BITDEPTHCONV_TEST_OUTU8_IMG_IDX].storageAddr;
    uint8_t *bitdepthConvTest_OutImageS16 = BitdepthConvTestImageInfo[BITDEPTHCONV_TEST_OUTS16_IMG_IDX].storageAddr;


    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(bitdepthConvTest_OutImageU8,BITDEPTHCONV_TEST_OUTU8_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0xb8d8f070UL != crc) {
            ULOG_ERR(" BitdepthConv Test Failed in  bitdepthConvTest_OutImageU8 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(bitdepthConvTest_OutImageS16,BITDEPTHCONV_TEST_OUTS16_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x149C6AEBUL != crc) {
            ULOG_ERR(" BitdepthConv Test Failed in bitdepthConvTest_OutImageS16 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }
    /* check for CRC */
    return ret;
}

OpenVXTestConfigType BitdepthConvTestConfig = {BitdepthConvTest_GetImageInfo, BitdepthConvTest_GetNumImages, BitdepthConvTest_PrepareInput, BitdepthConvTest_ValidateOutput, BitdepthConvTest_SetMemory, BitdepthConvTest_FreeMemory,0xFF15, 1, BITDEPTHCONV_TEST_UNIQUE_ID};

OpenVXTestConfigType* BitdepthConvTest_GetTestConfig()
{
    return &BitdepthConvTestConfig;
}

