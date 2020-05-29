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
#include "filter_test_internal.h"


OpenVXLib_StorageInfo FilterTestImageInfo[] = {
[FILTER_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[FILTER_TEST_TEMP_STORAGE_IDX].storageSize = FILTER_TEST_TEMP_STORAGE_SIZE,

[FILTER_TEST_IN_SOBEL_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_IN_SOBEL_IMG_IDX].storageSize = FILTER_TEST_IN_SOBEL_IMG_SIZE,

[FILTER_TEST_GRAD_X_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_GRAD_X_IMG_IDX].storageSize = FILTER_TEST_GRAD_X_IMG_SIZE,

[FILTER_TEST_GRAD_Y_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_GRAD_Y_IMG_IDX].storageSize = FILTER_TEST_GRAD_Y_IMG_SIZE,

[FILTER_TEST_INU8_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_INU8_IMG_IDX].storageSize = FILTER_TEST_INU8_IMG_SIZE,

[FILTER_TEST_CANNY_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_CANNY_IMG_IDX].storageSize = FILTER_TEST_CANNY_IMG_SIZE,

[FILTER_TEST_MEDIANU8_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_MEDIANU8_IMG_IDX].storageSize = FILTER_TEST_MEDIANU8_IMG_SIZE,

[FILTER_TEST_GAUSSIANU8_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_GAUSSIANU8_IMG_IDX].storageSize = FILTER_TEST_GAUSSIANU8_IMG_SIZE,

[FILTER_TEST_INS16_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_INS16_IMG_IDX].storageSize = FILTER_TEST_INS16_IMG_SIZE,

[FILTER_TEST_MEDIANS16_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_MEDIANS16_IMG_IDX].storageSize = FILTER_TEST_MEDIANS16_IMG_SIZE,

[FILTER_TEST_GAUSSIANS16_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_GAUSSIANS16_IMG_IDX].storageSize = FILTER_TEST_GAUSSIANS16_IMG_SIZE,

[FILTER_TEST_BLURU8_IMG_IDX].storageAddr = NULL,
[FILTER_TEST_BLURU8_IMG_IDX].storageSize = FILTER_TEST_BLURU8_IMG_SIZE,
};

int32_t FilterTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;

    for (i=0;i<FILTER_TEST_MAX_DATA_ENTRIES;i++){
        tmpPtr = OpenVXTest_PoolMemAlloc(FilterTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            FilterTestImageInfo[i].storageAddr = tmpPtr;
        }
    }
    return BCM_ERR_OK;

}

void FilterTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* FilterTest_GetImageInfo()
{
    /* initialize the inputs */
    return FilterTestImageInfo;
}
uint32_t FilterTest_GetNumImages()
{
    return FILTER_TEST_MAX_DATA_ENTRIES;
}

int32_t FilterTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t imgSz = 0UL;

    uint8_t *filterTest_InSobelImage = FilterTestImageInfo[FILTER_TEST_IN_SOBEL_IMG_IDX].storageAddr;
    uint8_t *filterTest_InU8Image = FilterTestImageInfo[FILTER_TEST_INU8_IMG_IDX].storageAddr;
    uint8_t *filterTest_InS16Image = FilterTestImageInfo[FILTER_TEST_INS16_IMG_IDX].storageAddr;
    uint8_t *filterTest_Canny = FilterTestImageInfo[FILTER_TEST_CANNY_IMG_IDX].storageAddr;
    uint8_t *filterTest_GradientX = FilterTestImageInfo[FILTER_TEST_GRAD_X_IMG_IDX].storageAddr;
    uint8_t *filterTest_GradientY = FilterTestImageInfo[FILTER_TEST_GRAD_Y_IMG_IDX].storageAddr;
    uint8_t *filterTest_MedianU8 = FilterTestImageInfo[FILTER_TEST_MEDIANU8_IMG_IDX].storageAddr;
    uint8_t *filterTest_GaussianU8 = FilterTestImageInfo[FILTER_TEST_GAUSSIANU8_IMG_IDX].storageAddr;
    uint8_t *filterTest_MedianS16 = FilterTestImageInfo[FILTER_TEST_MEDIANS16_IMG_IDX].storageAddr;
    uint8_t *filterTest_GaussianS16 = FilterTestImageInfo[FILTER_TEST_GAUSSIANS16_IMG_IDX].storageAddr;
    uint8_t *filterTest_BlurU8 = FilterTestImageInfo[FILTER_TEST_BLURU8_IMG_IDX].storageAddr;


    if (0UL == aGraphID) {

        /* BCM_MemSet the input image memory */
        BCM_MemSet(filterTest_InSobelImage, 0, FILTER_TEST_IN_SOBEL_IMG_SIZE);

        /* BCM_MemSet output images */
        BCM_MemSet(filterTest_Canny, 0, FILTER_TEST_CANNY_IMG_SIZE);
        BCM_MemSet(filterTest_GradientX, 0, FILTER_TEST_GRAD_X_IMG_SIZE);
        BCM_MemSet(filterTest_GradientY, 0, FILTER_TEST_GRAD_Y_IMG_SIZE);
        BCM_MemSet(filterTest_MedianU8, 0, FILTER_TEST_MEDIANU8_IMG_SIZE);
        BCM_MemSet(filterTest_GaussianU8, 0, FILTER_TEST_GAUSSIANU8_IMG_SIZE);
        BCM_MemSet(filterTest_MedianS16, 0, FILTER_TEST_MEDIANS16_IMG_SIZE);
        BCM_MemSet(filterTest_GaussianS16, 0, FILTER_TEST_GAUSSIANS16_IMG_SIZE);
        BCM_MemSet(filterTest_BlurU8, 0, FILTER_TEST_BLURU8_IMG_SIZE);

        /* set first image */
        for (i=350;i<360;i++) {
            for (j=400;j<480;j++) {
                filterTest_InSobelImage[i*1280+j] = 255;
            }
        }

        ret = OpenVXTest_PTMLoadData((uint16_t)0xFF06, filterTest_InU8Image, FILTER_TEST_INU8_IMG_SIZE, &imgSz);

        if (FILTER_TEST_INU8_IMG_SIZE != imgSz) {
            ret = BCM_ERR_UNKNOWN;
            goto exit;
        }

        ret = OpenVXTest_PTMLoadData((uint16_t)0xFF17, filterTest_InS16Image, FILTER_TEST_INS16_IMG_SIZE, &imgSz);

        if (FILTER_TEST_INS16_IMG_SIZE != imgSz) {
            ret = BCM_ERR_UNKNOWN;
            goto exit;
        }
    }
exit:
    return ret;
}

uint32_t FilterTest_RefCRCGradX = 0x57602fe9;
uint32_t FilterTest_RefCRCGradY = 0xa657d521;
uint32_t FilterTest_RefCRCCanny = 0x85058235;
uint32_t FilterTest_RefCRCMedianU8 = 0xbe0f7a56;
uint32_t FilterTest_RefCRCGaussianU8 = 0xe99b72d2;
uint32_t FilterTest_RefCRCMedianS16 = 0x325eef59;
uint32_t FilterTest_RefCRCGaussianS16 = 0xa0311dd3;
uint32_t FilterTest_RefCRCBlurU8 = 0xED31303E;



int32_t FilterTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t *filterTest_Canny = FilterTestImageInfo[FILTER_TEST_CANNY_IMG_IDX].storageAddr;
    uint8_t *filterTest_GradientX = FilterTestImageInfo[FILTER_TEST_GRAD_X_IMG_IDX].storageAddr;
    uint8_t *filterTest_GradientY = FilterTestImageInfo[FILTER_TEST_GRAD_Y_IMG_IDX].storageAddr;
    uint8_t *filterTest_MedianU8 = FilterTestImageInfo[FILTER_TEST_MEDIANU8_IMG_IDX].storageAddr;
    uint8_t *filterTest_GaussianU8 = FilterTestImageInfo[FILTER_TEST_GAUSSIANU8_IMG_IDX].storageAddr;
    uint8_t *filterTest_MedianS16 = FilterTestImageInfo[FILTER_TEST_MEDIANS16_IMG_IDX].storageAddr;
    uint8_t *filterTest_GaussianS16 = FilterTestImageInfo[FILTER_TEST_GAUSSIANS16_IMG_IDX].storageAddr;
    uint8_t *filterTest_BlurU8 = FilterTestImageInfo[FILTER_TEST_BLURU8_IMG_IDX].storageAddr;

    if (0UL == aGraphID) {

        ret = BCM_ERR_OK;
        /* check for CRC */
        crc = BCM_CRC32(filterTest_Canny,FILTER_TEST_CANNY_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCCanny != crc) {
            ULOG_ERR(" Filter Test Failed in Canny for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(filterTest_GradientX,FILTER_TEST_GRAD_X_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCGradX != crc) {
            ULOG_ERR(" Filter Test Failed in Grad X for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(filterTest_GradientY,FILTER_TEST_GRAD_Y_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCGradY != crc) {
            ULOG_ERR(" Filter Test Failed in Grad Y for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(filterTest_MedianU8,FILTER_TEST_MEDIANU8_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCMedianU8 != crc) {
            ULOG_ERR(" Filter Test Failed in MedianU8 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(filterTest_GaussianU8,FILTER_TEST_GAUSSIANU8_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCGaussianU8 != crc) {
            ULOG_ERR(" Filter Test Failed in GuassianU8 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(filterTest_MedianS16,FILTER_TEST_MEDIANS16_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCMedianS16 != crc) {
            ULOG_ERR(" Filter Test Failed in MedianS16 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(filterTest_GaussianS16,FILTER_TEST_GAUSSIANS16_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCGaussianS16 != crc) {
            ULOG_ERR(" Filter Test Failed in GaussianS16 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(filterTest_BlurU8,FILTER_TEST_BLURU8_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (FilterTest_RefCRCBlurU8 != crc) {
            ULOG_ERR(" Filter Test Failed in BlurU8 for Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    return ret;
}

OpenVXTestConfigType FilterTestConfig = {FilterTest_GetImageInfo, FilterTest_GetNumImages, FilterTest_PrepareInput, FilterTest_ValidateOutput, FilterTest_SetMemory, FilterTest_FreeMemory, 0xFF05, 1, FILTER_TEST_UNIQUE_ID};

OpenVXTestConfigType* FilterTest_GetTestConfig()
{
    return &FilterTestConfig;
}
