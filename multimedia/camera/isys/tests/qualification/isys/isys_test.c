/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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

/******************************************************************************
  File Name:  isys_test.c
Descritpion: This file implements qualification test application for isys fmwk.
 ******************************************************************************/
#define GetModuleLogLevel() GetISYS_TestLogLevel()

/* Includes */
#include <string.h>
#include "ee.h"
#include <bcm_err.h>
#include "ulog.h"
#include <bcm_test.h>
#include <osil/bcm_osil.h>

/* camera */
#include "cam_erika.h"
#include <isys.h>
#include "isys_osil.h"


/* Plane sizes */
#define APP_Y_SIZE  (MAX_VIDEO_WIDTH * 16UL)
#define APP_UV_SIZE ((MAX_VIDEO_WIDTH >> 1) * 8UL)

/* Globals */
static volatile int32_t isyslib_qt1_result = BCM_AT_NOT_STARTED;
static const uint32_t gFrameCount = 10UL;

/* Image - extern */
extern const uint8_t isp_image_array[];
extern const uint8_t unicam_image_array[];
extern const uint8_t cam_mode_image_array[];

/* ISYS Test Log level */
static uint32_t gISYS_TestLogLevel = ULOG_LVL_VERBOSE;

/* Get ISYS Test Log Level */
COMP_INLINE uint32_t GetISYS_TestLogLevel()
{
    return gISYS_TestLogLevel;
}

/* Allocation for ISYS buffers */
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_YUVBuffer[MAX_VIDEO_WIDTH*16UL*3UL*2UL] COMP_ALIGN(256);

/* Allocation for app buffers */
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufY[APP_Y_SIZE]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufU[APP_UV_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufV[APP_UV_SIZE] COMP_ALIGN(256);

static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufYSave[APP_Y_SIZE]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufUSave[APP_UV_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufVSave[APP_UV_SIZE] COMP_ALIGN(256);

const uint32_t gTG_Y_CRC = 0x544f2345UL;
const uint32_t gTG_U_CRC = 0xe0251041UL;
const uint32_t gTG_V_CRC = 0xef21ac57UL;

#define CRC32_POLY (0x04C11DB7UL)
static uint32_t Reverse(uint32_t aValue)
{
    uint32_t val = aValue;

    val = ((val & 0x55555555UL) << 1UL) | ((val >> 1UL) & 0x55555555UL);
    val = ((val & 0x33333333UL) << 2UL) | ((val >> 2UL) & 0x33333333UL);
    val = ((val & 0x0F0F0F0FUL) << 4UL) | ((val >> 4UL) & 0x0F0F0F0FUL);
    val = (val << 24UL) | ((val & 0xFF00UL) << 8UL) |
        ((val >> 8UL) & 0xFF00UL) | (val >> 24UL);

    return val;
}

static uint32_t CalculateCRC(const uint8_t *const aAddr,
                             uint32_t aLen)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;

    i = 0UL;
    crc = 0UL;
    while (aLen != 0UL) {
        byte = aAddr[i];
        byte = Reverse(byte);
        for (j = 0UL; j <= 7UL; j++) {
            if (((int32_t)(crc ^ byte)) < 0L) {
                crc = (crc << 1UL) ^ CRC32_POLY;
            } else {
                crc = crc << 1UL;
            }
            byte = byte << 1UL;
        }
        i++;
        aLen--;
    }

    return Reverse(~crc);
}

/* ISYS_ParseFrameNum */
int32_t ISYS_ParseFrameNum(const uint8_t *aBufferLong,
                           uint32_t aSizeLong,
                           const uint8_t *aBufferShort,
                           uint32_t aSizeShort,
                           uint32_t *aFrameNum)
{
    return BCM_ERR_NOSUPPORT;
}

/* ISYS_TestTask */
TASK(ISYS_TestTask)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t yCRC, uCRC, vCRC;
    uint32_t frameCount = 0UL;
    uint32_t flags;
    ISYS_BufferType buf;
    BCM_EventMaskType mask = 0UL;
    uint32_t notMatched = 0UL;
    ISYS_ConfigsType config = {
            .isysMemPool[ISYS_MEMPOOL_IDX_YUV] = Cam_YUVBuffer,
            .isysMemPool[ISYS_MEMPOOL_IDX_RAW] = NULL,
            .isysMemsize[ISYS_MEMPOOL_IDX_YUV] = sizeof(Cam_YUVBuffer),
            .isysMemsize[ISYS_MEMPOOL_IDX_RAW] = 0UL,
            .dpMode = ISYS_DUAL_PASS_MODE_NONE,
        };

    ret = ISYS_Init();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Init failed (%d)\n", ret);
        goto exit;
    }

    config.Cfg.pHdr = cam_mode_image_array;
    ret = ISYS_Configure(&config);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Configure failed for Mode Image (%d)\n", ret);
        goto exit;
    }
    config.Cfg.pHdr = unicam_image_array;
    ret = ISYS_Configure(&config);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Configure failed for UNICAM Image (%d)\n", ret);
        goto exit;
    }
    config.Cfg.pHdr = isp_image_array;
    ret = ISYS_Configure(&config);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Configure failed for ISP Image (%d)\n", ret);
        goto exit;
    }

    ret = ISYS_Start();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Start failed (%d)\n", ret);
        goto exit;
    }
    frameCount = 0UL;

    buf.plane[0].addr = Cam_AppBufY;
    buf.plane[0].size = APP_Y_SIZE;
    buf.plane[0].stride = MAX_VIDEO_WIDTH;
    buf.plane[0].isEnabled = 1UL;
    buf.plane[1].addr = Cam_AppBufU;
    buf.plane[1].size = APP_UV_SIZE;
    buf.plane[1].stride = MAX_VIDEO_WIDTH/2UL;
    buf.plane[1].isEnabled = 1UL;
    buf.plane[2].addr = Cam_AppBufV;
    buf.plane[2].size = APP_UV_SIZE;
    buf.plane[2].stride = MAX_VIDEO_WIDTH/2UL;
    buf.plane[2].isEnabled = 1UL;

    ret = ISYS_FillBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
        goto exit;
    }

    while (frameCount < gFrameCount) {
        BCM_WaitEvent(ISP_EVENT | UNICAM_EVENT | PP_GPEvent);
        BCM_GetEvent(ISYS_TestTask, &mask);
        BCM_ClearEvent(mask);
        if (0UL != (mask & ISP_EVENT) || 0UL != (mask & UNICAM_EVENT)) {
            BCM_ClearEvent(ISP_EVENT | UNICAM_EVENT);
            flags = ISYS_ProcessEvent();
            if (flags & ISYS_EVENT_ERROR) {
                break;
            }
        }
        ret = ISYS_GetFilledBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
        if (BCM_ERR_OK != ret) {
            continue;
        }
        if (0UL == frameCount) {
            memcpy(Cam_AppBufYSave, buf.plane[0].addr, buf.plane[0].size);
            memcpy(Cam_AppBufUSave, buf.plane[1].addr, buf.plane[1].size);
            memcpy(Cam_AppBufVSave, buf.plane[2].addr, buf.plane[2].size);
        }
        frameCount = buf.frameNum;
        if (frameCount < gFrameCount) {
            ret = ISYS_FillBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
                break;
            }
        }
    }
    ULOG_INFO("Received (%u) frames\n", frameCount);

exit:
    ret = ISYS_Stop();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Stop failed (%d)\n",ret);
    }

    ret = ISYS_Deinit();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Deinit failed (%d)\n",ret);
    }

    yCRC = CalculateCRC(Cam_AppBufYSave, buf.plane[0].size);
    uCRC = CalculateCRC(Cam_AppBufUSave, buf.plane[1].size);
    vCRC = CalculateCRC(Cam_AppBufVSave, buf.plane[2].size);
    if ((yCRC != gTG_Y_CRC) ||
            (uCRC != gTG_U_CRC) ||
            (vCRC != gTG_V_CRC)) {
        notMatched = 1UL;
        ULOG_ERR("yCRC = 0x%x \n", yCRC);
        ULOG_ERR("uCRC = 0x%x \n", uCRC);
        ULOG_ERR("vCRC = 0x%x \n", vCRC);
    }

    if ((gFrameCount == frameCount) && (0UL == notMatched)) {
        ULOG_INFO("Test Passed!\n");
        if (BCM_AT_EXECUTING == isyslib_qt1_result) {
            isyslib_qt1_result = BCM_ERR_OK;
        }
    } else {
        ULOG_INFO("Test Failed!\n");
        if (BCM_AT_EXECUTING == isyslib_qt1_result) {
            isyslib_qt1_result = BCM_ERR_UNKNOWN;
        }
    }

    BCM_TerminateTask();
}

/* BCM_ExecuteAT */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        isyslib_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(ISYS_TestTask);
        ret = BCM_ERR_OK;
    }
    return ret;
}

/* BCM_GetResultAT */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = isyslib_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
}
