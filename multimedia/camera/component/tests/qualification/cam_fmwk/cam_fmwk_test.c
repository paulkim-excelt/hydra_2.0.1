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
  File Name:  cam_fmwk_test.c
Descritpion: This file implements qualification test application for camera fmwk.
 ******************************************************************************/
#define GetModuleLogLevel() GetCamFmwkTestLogLevel()

/* Includes */
#include <string.h>
#include <ee.h>
#include <bcm_err.h>
#include <ulog.h>
#include <bcm_time.h>
#include <bcm_test.h>

/* camera */
#include <cam_erika.h>
#include <isys.h>
#include <isys_osil.h>
#include <tuner.h>
#include <ctn_log.h>
#include <camera_sensor.h>
#include <camera_sensor_osil.h>
#include <osil/bcm_osil.h>

/* Macro for disabling Tuner */
#define DISABLE_TUNER

/* Globals */
static const uint32_t gFrameCount = 10UL;

/* Test status */
static volatile int32_t camfmwk_qt1_result = BCM_AT_NOT_STARTED;

/* Image - extern */
extern const uint8_t isp_image_array[];
extern const uint8_t unicam_image_array[];
extern const uint8_t cam_mode_image_array[];
extern const uint8_t sensor_common_image_array[];
extern const uint8_t sensor_mode_colorbar_image_array[];
#ifndef DISABLE_TUNER
extern const uint8_t tuner_image_array[];
#endif /* DISABLE_TUNER */

/* CamFmwk Test Log level */
static uint32_t gCamFmwkTestLogLevel = ULOG_LVL_VERBOSE;

/* Get CamFmwk Test Log Level */
COMP_INLINE uint32_t GetCamFmwkTestLogLevel()
{
    return gCamFmwkTestLogLevel;
}

/* Allocation for ISYS buffers */
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_YUVBuffer[MAX_VIDEO_WIDTH * 3UL * 16UL * 2UL] COMP_ALIGN(256);

/* Allocation for app buffers */
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufY[MAX_VIDEO_WIDTH * 2UL * 16UL]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufU[MAX_VIDEO_WIDTH * 16UL] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufV[MAX_VIDEO_WIDTH * 16UL] COMP_ALIGN(256);

static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufYSave[MAX_VIDEO_WIDTH * 2UL * 16UL]  COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufUSave[MAX_VIDEO_WIDTH * 16UL] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.yuv.uncached") Cam_AppBufVSave[MAX_VIDEO_WIDTH * 16UL] COMP_ALIGN(256);

#ifdef ENABLE_CAM_MODULE_OV10640
const uint32_t gCB_Y_CRC = 0xbb84366fUL;
const uint32_t gCB_U_CRC = 0xabe544daUL;
const uint32_t gCB_V_CRC = 0xe58431baUL;
#elif defined ENABLE_CAM_MODULE_OX3A10
const uint32_t gCB_Y_CRC = 0x6060113eUL;
const uint32_t gCB_U_CRC = 0x34a215a0UL;
const uint32_t gCB_V_CRC = 0xe66beda7UL;
#elif defined ENABLE_CAM_MODULE_AR0143
const uint32_t gCB_Y_CRC = 0x54cf44f2UL;
const uint32_t gCB_U_CRC = 0x14a9911bUL;
const uint32_t gCB_V_CRC = 0x6612ee05UL;
#endif

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

static int32_t gRegress = 0UL;

/* CamFwmkTestTask */
TASK(CamFwmkTestTask)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t yCRC, uCRC, vCRC;
    uint32_t frameCount;
    uint32_t fps = 0UL;
    uint32_t flags;
    ISYS_BufferType buf;
    BCM_EventMaskType mask = 0UL;
    uint32_t notMatched = 0UL;
    uint32_t outIdx = 0x0UL;
    ISYS_FormatType format;
    uint32_t width = 0x0UL;
    uint32_t widthInbytes = 0x0UL;
    ISYS_InfoType info;
    uint32_t ySize, uSize, vSize, yStride, uStride, vStride;
    uint32_t startTime = 0UL;
    uint32_t endTime = 0UL;

    ISYS_ConfigsType config = {
            .isysMemPool[ISYS_MEMPOOL_IDX_YUV] = Cam_YUVBuffer,
            .isysMemPool[ISYS_MEMPOOL_IDX_RAW] = NULL,
            .isysMemsize[ISYS_MEMPOOL_IDX_YUV] = sizeof(Cam_YUVBuffer),
            .isysMemsize[ISYS_MEMPOOL_IDX_RAW] = 0UL,
            .dpMode = ISYS_DUAL_PASS_MODE_NONE,
        };

    do {
        ret = Tuner_Init();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Tuner_Init failed (%d)\n",ret);
            goto exit;
        }

        ret = ISYS_Init();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_Init failed (%d)\n",ret);
            goto exit;
        }

        do {
            if (BCM_ERR_OK == ret) {
                ret = CSN_Init();
                if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                    ULOG_ERR("CSN_Init failed (%d)\n", ret);
                    goto exit;
                }
            }

            if (BCM_ERR_BUSY == ret) {
                uint32_t csnMask;

                BCM_WaitEvent(CAM_EVENT_CSN_RESET);
                BCM_GetEvent(CAMERA_TASK, &mask);
                BCM_ClearEvent(mask);
                csnMask = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA,
                                           CSN_EVENT_SOURCE_RESET);
                if (CSN_STATUS_ERROR == (csnMask & CSN_STATUS_ERROR)) {
                    ULOG_ERR("CSN_Init failed (%d)\n", ret);
                    goto exit;
                }

                if (CSN_STATE_INITIALISED == CSN_GetState()) {
                    ret = BCM_ERR_OK;
                } else {
                    ret = BCM_ERR_BUSY;
                }
            }
        } while (BCM_ERR_BUSY == ret);

        config.Cfg.pHdr = cam_mode_image_array;
        ret = ISYS_Configure(&config);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_Configure failed for Mode Image (%d)\n",ret);
            goto exit;
        }

        config.Cfg.pHdr = unicam_image_array;
        ret = ISYS_Configure(&config);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_Configure failed for UNICAM Image (%d)\n",ret);
            goto exit;
        }

        config.Cfg.pHdr = isp_image_array;
        ret = ISYS_Configure(&config);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_Configure failed for ISP Image (%d)\n",ret);
            goto exit;
        }

#ifndef DISABLE_TUNER
        config.Cfg.pHdr = tuner_image_array;
        ret = Tuner_Configure(config.Cfg.pTunerImg);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Tuner_Configure failed (%d)\n",ret);
            goto exit;
        }
#endif /* DISABLE_TUNER */

        ret = BCM_ERR_BUSY;
        while (ret == BCM_ERR_BUSY) {
            flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
            if (0UL != (flags & CSN_STATUS_ERROR)) {
                ret = BCM_ERR_UNKNOWN;
                ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
                goto exit;
            }
            if (0UL == (flags & CSN_STATUS_BUSY)) {
                ret = CSN_Configure((CSN_ImageType*)sensor_common_image_array);
                if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                    ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
                    goto exit;
                }
            }
            if (BCM_ERR_BUSY == ret) {
                BCM_WaitEvent(CSN_EVENT_IIC);
                BCM_GetEvent(CAMERA_TASK, &mask);
                BCM_ClearEvent(mask);
            }
        }
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CSN_Configure failed (%d)\n", ret);
            goto exit;
        }

        ret = BCM_ERR_BUSY;
        while (ret == BCM_ERR_BUSY) {
            flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
            if (0UL != (flags & CSN_STATUS_ERROR)) {
                ret = BCM_ERR_UNKNOWN;
                ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
                goto exit;
            }
            if (0UL == (flags & CSN_STATUS_BUSY)) {
                ret = CSN_Configure((CSN_ImageType*)sensor_mode_colorbar_image_array);
                if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                    ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
                    goto exit;
                }
            }
            if (BCM_ERR_BUSY == ret) {
                BCM_WaitEvent(CSN_EVENT_IIC);
                BCM_GetEvent(CAMERA_TASK, &mask);
                BCM_ClearEvent(mask);
            }
        }
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CSN_Configure failed (%d)\n", ret);
            goto exit;
        }

        /* Get Mode */
        ISYS_GetInfo(&info);
        outIdx = (0UL != (info.mode.passProps[ISYS_PASS_IDX_HV].outputEnable & ISYS_OUT_ENABLE_LORES)) ?
                               ISYS_OUT_IDX_LORES : ISYS_OUT_IDX_HIRES;
        format = info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].format;
        width = info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].width;
        widthInbytes = width * info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].outWidth;

        BCM_ActivateTask(CAMERA_TUNER_TASK);
#ifndef DISABLE_TUNER
        ret = Tuner_Start();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Tuner_Start failed (%d)\n",ret);
            goto exit;
        }
#endif /* DISABLE_TUNER */

        ret = ISYS_Start();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_Start failed (%d)\n",ret);
            goto exit;
        }

        ret = BCM_ERR_BUSY;
        while (ret == BCM_ERR_BUSY) {
            flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
            if (0UL != (flags & CSN_STATUS_ERROR)) {
                ret = BCM_ERR_UNKNOWN;
                ULOG_ERR("CSN_Start failed for Sensor Image (%d)\n",ret);
                goto exit;
            }
            if (0UL == (flags & CSN_STATUS_BUSY)) {
                ret = CSN_Start();
                if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                    ULOG_ERR("CSN_Start failed for Sensor Image (%d)\n",ret);
                    goto exit;
                }
            }
            if (BCM_ERR_BUSY == ret) {
                BCM_WaitEvent(CSN_EVENT_IIC);
                BCM_GetEvent(CAMERA_TASK, &mask);
                BCM_ClearEvent(mask);
            }
        }
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CSN_Start failed (%d)\n",ret);
            goto exit;
        }

        frameCount = 0UL;

        switch(format) {
            case ISYS_FORMAT_444_PLANAR:
                ySize = widthInbytes * 16UL;
                yStride = widthInbytes;
                uSize = vSize = widthInbytes * 16UL;
                uStride = vStride = widthInbytes;
                break;
            case ISYS_FORMAT_422_PLANAR:
                ySize = widthInbytes * 16UL;
                yStride = widthInbytes;
                uSize = vSize = (widthInbytes/2UL) * 16UL;
                uStride = vStride = (widthInbytes/2UL);
                break;
            case ISYS_FORMAT_420_PLANAR:
                ySize = widthInbytes * 16UL;
                yStride = widthInbytes;
                uSize = vSize = (widthInbytes >> 1UL) * 8UL;
                uStride = vStride = widthInbytes/2UL;
                break;
            case ISYS_FORMAT_420_SEMI_PLANAR_UV:
                ySize = widthInbytes * 16UL;
                yStride = widthInbytes;
                uSize = widthInbytes * 8UL;
                uStride = widthInbytes;
                vSize = 0UL;
                vStride = 0UL;
                break;
            case ISYS_FORMAT_422_INTERLEAVED_YUYV:
            case ISYS_FORMAT_422_INTERLEAVED_YVYU:
            case ISYS_FORMAT_422_INTERLEAVED_UYVY:
            case ISYS_FORMAT_422_INTERLEAVED_VYUY:
                ySize = widthInbytes * 2UL * 16UL;
                yStride = widthInbytes * 2UL;
                uSize = vSize = 0UL;
                uStride = vStride = 0UL;
                break;
            default:
                ULOG_ERR("%s: Unsupported format: %u \n", __FUNCTION__, format);
                goto exit;
        }

        buf.plane[0].addr = Cam_AppBufY;
        buf.plane[0].size = ySize;
        buf.plane[0].stride = yStride;
        buf.plane[0].isEnabled = 1UL;

        buf.plane[1].addr = Cam_AppBufU;
        buf.plane[1].size = uSize;
        buf.plane[1].stride = uStride;
        buf.plane[1].isEnabled = 1UL;

        buf.plane[2].addr = Cam_AppBufV;
        buf.plane[2].size = vSize;
        buf.plane[2].stride = vStride;
        buf.plane[2].isEnabled = 1UL;

        ret = ISYS_FillBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
            goto exit;
        }

        while (frameCount < gFrameCount) {
            BCM_WaitEvent(ISP_EVENT | UNICAM_EVENT | PP_GPEvent);
            BCM_GetEvent(CamFwmkTestTask, &mask);
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
            /* Compare buffer */
            if ((ISYS_FORMAT_420_PLANAR == format) && (1280UL == width)) {
                if (0UL == frameCount) {
                    memcpy(Cam_AppBufYSave, buf.plane[0].addr, buf.plane[0].size);
                    memcpy(Cam_AppBufUSave, buf.plane[1].addr, buf.plane[1].size);
                    memcpy(Cam_AppBufVSave, buf.plane[2].addr, buf.plane[2].size);
                }
            }
            if (0UL == frameCount) {
                startTime = (buf.timestamp)/BCM_NS_PER_SEC;
            }
            endTime = (buf.timestamp)/BCM_NS_PER_SEC;

            if (frameCount != buf.frameNum) {
                /* CTN Test Logging structure */
                typedef struct _CTN_LogTest {
                    uint32_t val1;
                    uint32_t val2;
                } CTN_LogTest;
                CTN_LogTest testLog;
                int32_t ret;

                testLog.val1 = 0xabcd0123UL;
                testLog.val2 = 0x3456789aUL;
                /* Send test logging through UDP */
                ret = CTN_Log(CTN_LOG_ID_AGC,
                              0U,
                              buf.frameNum,
                              (uint8_t *) &testLog,
                              sizeof(testLog));
                if (BCM_ERR_OK != ret) {
                    ULOG_ERR("CTN_Log failed (%d)\n",ret);
                }
                frameCount = buf.frameNum;
                fps++;
            }

            if ((0UL != startTime) && (0UL != endTime) && (endTime != startTime)) {
                ULOG_INFO("FPS: %d \n", fps);
                fps = 0UL;
                startTime = endTime;
            }

            if (frameCount < gFrameCount) {
                ret = ISYS_FillBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
                if (BCM_ERR_OK != ret) {
                    ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
                    break;
                }
            }
        }
        ULOG_INFO("Received (%u) frames\n", frameCount);

        /* Wait for Tuner Task termination */
        BCM_ClearEvent(TUNER_EVENT_CMD_COMPLETE);
        BCM_SetEvent(CAMERA_TUNER_TASK, TUNER_EVENT_QUIT);
        BCM_WaitEvent(TUNER_EVENT_CMD_COMPLETE);
        BCM_ClearEvent(TUNER_EVENT_CMD_COMPLETE);

        ret = BCM_ERR_BUSY;
        while (ret == BCM_ERR_BUSY) {
            flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
            if (0UL != (flags & CSN_STATUS_ERROR)) {
                ret = BCM_ERR_UNKNOWN;
                ULOG_ERR("CSN_Stop failed for Sensor Image (%d)\n",ret);
                goto exit;
            }
            if (0UL == (flags & CSN_STATUS_BUSY)) {
                ret = CSN_Stop();
                if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                    ULOG_ERR("CSN_Stop failed for Sensor Image (%d)\n",ret);
                    goto exit;
                }
            }
            if (BCM_ERR_BUSY == ret) {
                BCM_WaitEvent(CSN_EVENT_IIC);
                BCM_GetEvent(CAMERA_TASK, &mask);
                BCM_ClearEvent(mask);
            }
        }
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CSN_Stop failed (%d)\n",ret);
            goto exit;
        }

        ret = ISYS_Stop();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_Stop failed (%d)\n",ret);
            goto exit;
        }

#ifndef DISABLE_TUNER
        ret = Tuner_Stop();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Tuner_Stop failed (%d)\n",ret);
            goto exit;
        }
#endif /* DISABLE_TUNER */

        ret = CSN_Deinit();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CSN_Deinit failed (%d)\n",ret);
            goto exit;
        }

        ret = ISYS_Deinit();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("ISYS_Deinit failed (%d)\n",ret);
            goto exit;
        }

        ret = Tuner_Deinit();
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Tuner_Deinit failed (%d)\n",ret);
            goto exit;
        }
    } while (0UL != gRegress);

    yCRC = CalculateCRC(Cam_AppBufYSave, buf.plane[0].size);
    uCRC = CalculateCRC(Cam_AppBufUSave, buf.plane[1].size);
    vCRC = CalculateCRC(Cam_AppBufVSave, buf.plane[2].size);
    if ((yCRC != gCB_Y_CRC) ||
            (uCRC != gCB_U_CRC) ||
            (vCRC != gCB_V_CRC)) {
        notMatched = 1UL;
        ULOG_ERR("yCRC = 0x%x \n", yCRC);
        ULOG_ERR("uCRC = 0x%x \n", uCRC);
        ULOG_ERR("vCRC = 0x%x \n", vCRC);
    }

    if ((gFrameCount == frameCount) && (0UL == notMatched)) {
        ULOG_INFO("Test Passed!\n");
        if (BCM_AT_EXECUTING == camfmwk_qt1_result) {
            camfmwk_qt1_result = BCM_ERR_OK;
        }
    } else {
        ULOG_INFO("Test Failed!\n");
        if (BCM_AT_EXECUTING == camfmwk_qt1_result) {
            camfmwk_qt1_result = BCM_ERR_UNKNOWN;
        }
    }


exit:
    BCM_TerminateTask();
}

/* BCM_ExecuteAT */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        camfmwk_qt1_result = BCM_AT_EXECUTING;
        gRegress = 0UL;
        BCM_ActivateTask(CamFwmkTestTask);
        ret = BCM_ERR_OK;
    }

    return ret;
}

/* BCM_GetResultAT */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        ret = camfmwk_qt1_result;
    }

    return ret;
}

void ApplicationInit()
{
}
