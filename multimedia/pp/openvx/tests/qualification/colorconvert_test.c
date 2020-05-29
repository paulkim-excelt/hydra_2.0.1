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
#include "colorconvert_test_internal.h"

static uint32_t ColorConvertTest_refCRC[8][4]={
{0xd9e55741, 0x4533e41, 0x49df164e, 0xe036542b},  /*RGB-GRAY, RGB-IYUV, RGB-RGBX, IYUV-RGB*/
{0, 0x4533e41, 0x7722b759, 0x21e78675},           /* 0, RGBX-IYUV, RGBX-RGB, IYUV-RGBX */
{0x3ffd8e49, 0x7cdc229, 0xb5800437,0},            /*BGR-GRAY, BGR-IYUV, BGR-RGBX,0*/
{0, 0x7cdc229, 0x217c602b,0},                     /*0, BGRX-IYUV, BGRX-RGB,0*/
{0xe036542b, 0x21e78675, 0,0},                    /* NV12 - RGB, NV12 - RGBX,0,0*/
{0x20d23d8c,0x725f162e, 0,0},                     /* NV21 - RGB, NV21 - RGBX,0,0*/
{0xe036542b, 0x21e78675,0,0},                     /* YUYV - RGB, UYVY - RGBX */
{0xc9352b15,0xa6245a7b,0,0}                       /* UYVY - RGB, YUYV - RGBX */
};

OpenVXLib_StorageInfo ColorConvertTestImageInfo[] = {
[COLORCONVERT_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_TEMP_STORAGE_IDX].storageSize = COLORCONVERT_TEST_TEMP_STORAGE_SIZE,

[COLORCONVERT_TEST_INRGB_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INRGB_IMG_IDX].storageSize = COLORCONVERT_TEST_INRGB_IMG_SIZE,

[COLORCONVERT_TEST_INRGBX_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INRGBX_IMG_IDX].storageSize = COLORCONVERT_TEST_INRGBX_IMG_SIZE,

[COLORCONVERT_TEST_INBGR_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INBGR_IMG_IDX].storageSize = COLORCONVERT_TEST_INBGR_IMG_SIZE,

[COLORCONVERT_TEST_INBGRX_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INBGRX_IMG_IDX].storageSize = COLORCONVERT_TEST_INBGRX_IMG_SIZE,

[COLORCONVERT_TEST_OUTGRAY_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_OUTGRAY_IMG_IDX].storageSize = COLORCONVERT_TEST_OUTGRAY_IMG_SIZE,

[COLORCONVERT_TEST_OUTIYUV_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_OUTIYUV_IMG_IDX].storageSize = COLORCONVERT_TEST_OUTIYUV_IMG_SIZE,

[COLORCONVERT_TEST_OUTRGBX_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_OUTRGBX_IMG_IDX].storageSize = COLORCONVERT_TEST_OUTRGBX_IMG_SIZE,

[COLORCONVERT_TEST_OUTRGB_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_OUTRGB_IMG_IDX].storageSize = COLORCONVERT_TEST_OUTRGB_IMG_SIZE,

[COLORCONVERT_TEST_INNV12_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INNV12_IMG_IDX].storageSize = COLORCONVERT_TEST_INNV12_IMG_SIZE,

[COLORCONVERT_TEST_INNV21_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INNV21_IMG_IDX].storageSize = COLORCONVERT_TEST_INNV21_IMG_SIZE,

[COLORCONVERT_TEST_INYUYV_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INYUYV_IMG_IDX].storageSize = COLORCONVERT_TEST_INYUYV_IMG_SIZE,

[COLORCONVERT_TEST_INUYVY_IMG_IDX].storageAddr = NULL,
[COLORCONVERT_TEST_INUYVY_IMG_IDX].storageSize = COLORCONVERT_TEST_INUYVY_IMG_SIZE,
};

int32_t ColorConvertTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;
    for (i=0;i<COLORCONVERT_TEST_MAX_DATA_ENTRIES;i++){
        tmpPtr = OpenVXTest_PoolMemAlloc(ColorConvertTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            ColorConvertTestImageInfo[i].storageAddr = tmpPtr;
        }
    }

    return BCM_ERR_OK;
}


void ColorConvertTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}
const OpenVXLib_StorageInfo* ColorConvertTest_GetImageInfo()
{
    /* initialize the inputs */
    return ColorConvertTestImageInfo;
}
uint32_t ColorConvertTest_GetNumImages()
{
    return COLORCONVERT_TEST_MAX_DATA_ENTRIES;
}

int32_t ColorConvertTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t size,sizeX;
    uint32_t i,j;
    uint8_t *inRGBptr;
    uint8_t *inRGBXptr;
    uint8_t *inNVptr;
    uint8_t *inYUYVptr;
    uint8_t *colorConvertTest_InputImageRGB = ColorConvertTestImageInfo[COLORCONVERT_TEST_INRGB_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_InputImageRGBX = ColorConvertTestImageInfo[COLORCONVERT_TEST_INRGBX_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_InputImageNV12 = ColorConvertTestImageInfo[COLORCONVERT_TEST_INNV12_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_InputImageYUYV = ColorConvertTestImageInfo[COLORCONVERT_TEST_INYUYV_IMG_IDX].storageAddr;

    uint8_t *colorConvertTest_InputImageBGR = ColorConvertTestImageInfo[COLORCONVERT_TEST_INBGR_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_InputImageBGRX = ColorConvertTestImageInfo[COLORCONVERT_TEST_INBGRX_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_InputImageNV21 = ColorConvertTestImageInfo[COLORCONVERT_TEST_INNV21_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_InputImageUYVY = ColorConvertTestImageInfo[COLORCONVERT_TEST_INUYVY_IMG_IDX].storageAddr;

    uint8_t *colorConvertTest_OutputImageGray = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTGRAY_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_OutputImageIYUV = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTIYUV_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_OutputImageRGBX = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTRGBX_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_OutputImageRGB = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTRGB_IMG_IDX].storageAddr;

    uint32_t width = 640;//640*480 //256*256
    uint32_t blkHt = 60; //480/8 //256/8

    inRGBptr = colorConvertTest_InputImageRGB;
    inRGBXptr = colorConvertTest_InputImageRGBX;
    inNVptr = colorConvertTest_InputImageNV12;
    inYUYVptr = colorConvertTest_InputImageYUYV;
    /****************************************RGB/RGBX **********************************/
    for (i=0;i<blkHt;i++){ //red
        for (j=0;j<width;j++){
            *inRGBptr++ = 0xFF;*inRGBptr++ = 0;*inRGBptr++ = 0;
            *inRGBXptr++ = 0xFF;*inRGBXptr++ = 0;*inRGBXptr++ = 0;*inRGBXptr++ = 0xFF;
        }
    }
    size =  (width*blkHt*3);BCM_MemSet(inRGBptr, 0, size);  inRGBptr +=size; //blk
    sizeX =  (width*blkHt*4);

    for (i=0;i<blkHt;i++) {//RGBX black
        for (j=0;j<width;j++){
            *inRGBXptr++ = 0;*inRGBXptr++ = 0;*inRGBXptr++ = 0;*inRGBXptr++ = 0xFF;
        }
    }
    for (i=0;i<blkHt;i++) {//green
        for (j=0;j<width;j++){
            *inRGBptr++ = 0;*inRGBptr++ = 0xff;*inRGBptr++ = 0;
            *inRGBXptr++ = 0;*inRGBXptr++ = 0xff;*inRGBXptr++ = 0;*inRGBXptr++ = 0xFF;
        }
    }
    BCM_MemSet(inRGBptr, 0xFF, size);inRGBptr += size; //white
    BCM_MemSet(inRGBXptr, 0xFF, sizeX);inRGBXptr += sizeX;
    for (i=0;i<blkHt;i++) {//blue
        for (j=0;j<width;j++){
            *inRGBptr++ = 0;*inRGBptr++ = 0;*inRGBptr++ = 0xff;
            *inRGBXptr++ = 0;*inRGBXptr++ = 0;*inRGBXptr++ = 0xff;*inRGBXptr++ = 0xFF;
        }
    }
    BCM_MemSet(inRGBptr, 0x80, size);inRGBptr += size; //gray
    for (i=0;i<blkHt;i++) {//RGBX black
        for (j=0;j<width;j++){
            *inRGBXptr++ = 0x80;*inRGBXptr++ = 0x80;*inRGBXptr++ = 0x80;*inRGBXptr++ = 0xFF;
        }
    }
    for (i=0;i<blkHt;i++){ //yello
        for (j=0;j<width;j++){
            *inRGBptr++ = 0xff;*inRGBptr++ = 0xff;*inRGBptr++ = 0;
            *inRGBXptr++ = 0xff;*inRGBXptr++ = 0xff;*inRGBXptr++ = 0;*inRGBXptr++ = 0xFF;
        }
    }
    for (i=0;i<blkHt;i++) { //purple
        for (j=0;j<width;j++){
            *inRGBptr++ = 0x80;*inRGBptr++ = 0;*inRGBptr++ = 0x80;
            *inRGBXptr++ = 0x80;*inRGBXptr++ = 0;*inRGBXptr++ = 0x80;*inRGBXptr++ = 0xFF;
        }
    }

    /*********************************** NV12/NV21 ***************************************/
    size = width*blkHt;
    BCM_MemSet(inNVptr, 0x52, size);    inNVptr += size;
    BCM_MemSet(inNVptr, 0x10, size);    inNVptr += size;
    BCM_MemSet(inNVptr, 0x90, size);    inNVptr += size;
    BCM_MemSet(inNVptr, 0xeb, size);    inNVptr += size;
    BCM_MemSet(inNVptr, 0x29, size);    inNVptr += size;
    BCM_MemSet(inNVptr, 0x7e, size);    inNVptr += size;
    BCM_MemSet(inNVptr, 0xd2, size);    inNVptr += size;
    BCM_MemSet(inNVptr, 0x3e, size);    inNVptr += size;

    for (j=0;j<(size>>1);j+=2)
    {
        *inNVptr++ = 0x5a;  *inNVptr++ = 0xF0;
    }
    BCM_MemSet(inNVptr,0x80,(size>>1));     inNVptr += (size>>1);
    for (j=0;j<(size>>1);j+=2)
    {
        *inNVptr++ = 0x36;  *inNVptr++ = 0x22;
    }
    BCM_MemSet(inNVptr,0x80,(size>>1));     inNVptr += (size>>1);
    for (j=0;j<(size>>1);j+=2)
    {
        *inNVptr++ = 0xf0;  *inNVptr++ = 0x6e;
    }
    BCM_MemSet(inNVptr,0x80,(size>>1));     inNVptr += (size>>1);
    for (j=0;j<(size>>1);j+=2)
    {
        *inNVptr++ = 0x10;  *inNVptr++ = 0x92;
    }
    for (j=0;j<(size>>1);j+=2)
    {
        *inNVptr++ = 0xa5;  *inNVptr++ = 0xaf;
    }

    /***************************************YUYV/UYVY ***********************/
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0x52; *inYUYVptr++ = 0x5a; *inYUYVptr++ = 0x52; *inYUYVptr++ = 0xf0;
    }
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0x10; *inYUYVptr++ = 0x80; *inYUYVptr++ = 0x10; *inYUYVptr++ = 0x80;
    }
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0x90; *inYUYVptr++ = 0x36; *inYUYVptr++ = 0x90; *inYUYVptr++ = 0x22;
    }
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0xeb; *inYUYVptr++ = 0x80; *inYUYVptr++ = 0xeb; *inYUYVptr++ = 0x80;
    }
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0x29; *inYUYVptr++ = 0xf0; *inYUYVptr++ = 0x29; *inYUYVptr++ = 0x6e;
    }
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0x7e; *inYUYVptr++ = 0x80; *inYUYVptr++ = 0x7e; *inYUYVptr++ = 0x80;
    }
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0xd2; *inYUYVptr++ = 0x10; *inYUYVptr++ = 0xd2; *inYUYVptr++ = 0x92;
    }
    for (j=0;j<(width*blkHt*2);j+=4)
    {
        *inYUYVptr++ = 0x3e; *inYUYVptr++ = 0xa5; *inYUYVptr++ = 0x3e; *inYUYVptr++ = 0xaf;
    }


    BCM_MemCpy(colorConvertTest_InputImageBGR,colorConvertTest_InputImageRGB,COLORCONVERT_TEST_INRGB_IMG_SIZE);
    BCM_MemCpy(colorConvertTest_InputImageBGRX,colorConvertTest_InputImageRGBX,COLORCONVERT_TEST_INRGBX_IMG_SIZE);
    BCM_MemCpy(colorConvertTest_InputImageNV21,colorConvertTest_InputImageNV12,COLORCONVERT_TEST_INNV12_IMG_SIZE);
    BCM_MemCpy(colorConvertTest_InputImageUYVY,colorConvertTest_InputImageYUYV,COLORCONVERT_TEST_INYUYV_IMG_SIZE);

    /* BCM_MemSet output images */
    BCM_MemSet(colorConvertTest_OutputImageGray, 0, COLORCONVERT_TEST_OUTGRAY_IMG_SIZE);
    BCM_MemSet(colorConvertTest_OutputImageIYUV, 0, COLORCONVERT_TEST_OUTIYUV_IMG_SIZE);
    BCM_MemSet(colorConvertTest_OutputImageRGBX, 0, COLORCONVERT_TEST_OUTRGBX_IMG_SIZE);
    BCM_MemSet(colorConvertTest_OutputImageRGB, 0, COLORCONVERT_TEST_OUTRGB_IMG_SIZE);

    return BCM_ERR_OK;
}

int32_t ColorConvertTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t crcCalc[4];
    uint8_t *colorConvertTest_OutputImageGray = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTGRAY_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_OutputImageIYUV = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTIYUV_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_OutputImageRGBX = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTRGBX_IMG_IDX].storageAddr;
    uint8_t *colorConvertTest_OutputImageRGB = ColorConvertTestImageInfo[COLORCONVERT_TEST_OUTRGB_IMG_IDX].storageAddr;


    switch(aGraphID){
        case 0UL:   /*from /to RGB*/
            crcCalc[0] = BCM_CRC32(colorConvertTest_OutputImageGray, COLORCONVERT_TEST_OUTGRAY_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[1] = BCM_CRC32(colorConvertTest_OutputImageIYUV, COLORCONVERT_TEST_OUTIYUV_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[2] = BCM_CRC32(colorConvertTest_OutputImageRGBX, COLORCONVERT_TEST_OUTRGBX_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[3] = BCM_CRC32(colorConvertTest_OutputImageRGB, COLORCONVERT_TEST_INRGB_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            break;
        case 1UL: /*from/to RGBX */
            crcCalc[0] = 0;
            crcCalc[1] = BCM_CRC32(colorConvertTest_OutputImageIYUV, COLORCONVERT_TEST_OUTIYUV_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[2] = BCM_CRC32(colorConvertTest_OutputImageRGB, COLORCONVERT_TEST_OUTRGB_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[3] = BCM_CRC32(colorConvertTest_OutputImageRGBX, COLORCONVERT_TEST_OUTRGBX_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            break;
        case 2UL: /*from/to BGR/RGB */
            crcCalc[0] = BCM_CRC32(colorConvertTest_OutputImageGray, COLORCONVERT_TEST_OUTGRAY_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[1] = BCM_CRC32(colorConvertTest_OutputImageIYUV, COLORCONVERT_TEST_OUTIYUV_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[2] = BCM_CRC32(colorConvertTest_OutputImageRGBX, COLORCONVERT_TEST_OUTRGBX_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[3] = 0;
            break;
        case 3UL: /*from/to BGRX/RGBX */
            crcCalc[0] = 0;
            crcCalc[1] = BCM_CRC32(colorConvertTest_OutputImageIYUV, COLORCONVERT_TEST_OUTIYUV_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[2] = BCM_CRC32(colorConvertTest_OutputImageRGB, COLORCONVERT_TEST_OUTRGB_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[3] = 0;
            break;
        case 4UL:   /* NV12*/
        case 5UL:   /* NV21*/
        case 6UL:   /* YUYV*/
        case 7UL:   /* UYVY*/
            crcCalc[0] = BCM_CRC32(colorConvertTest_OutputImageRGB, COLORCONVERT_TEST_OUTRGB_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[1] = BCM_CRC32(colorConvertTest_OutputImageRGBX, COLORCONVERT_TEST_OUTRGBX_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
            crcCalc[2] = 0;
            crcCalc[3] = 0;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }
    if (BCM_ERR_OK == ret) {
        for (i=0;i<4;i++){
            if (ColorConvertTest_refCRC[aGraphID][i] != crcCalc[i]){
                ULOG_ERR("CRC check failed for graph %d test %d\n",aGraphID,i);
                ret = BCM_ERR_DATA_INTEG;
            }
        }
    }
    return ret;
}

OpenVXTestConfigType ColorConvertTestConfig = {ColorConvertTest_GetImageInfo, ColorConvertTest_GetNumImages, ColorConvertTest_PrepareInput, ColorConvertTest_ValidateOutput, ColorConvertTest_SetMemory, ColorConvertTest_FreeMemory, 0xFF0D, 8, COLORCONVERT_TEST_UNIQUE_ID};

OpenVXTestConfigType* ColorConvertTest_GetTestConfig()
{
    return &ColorConvertTestConfig;
}
