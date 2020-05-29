/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

/**
    @file bayer_stream.c
    @brief Bayer streaming using ISP bypass
    This tests Bayer streaming using ISP
    @version 0.1 Initial version
*/

#define GetModuleLogLevel() ULOG_LVL_VERBOSE

/* Includes */
#include <string.h>
#include <ee.h>
#include <bcm_err.h>
#include <ulog.h>
#include <bcm_time.h>
#include <bcm_test.h>
#include <bcm_utils.h>

/* camera */
#include <cam_erika.h>
#include <isys.h>
#include <isys_osil.h>
#include <camera_sensor.h>
#include <camera_sensor_osil.h>
#include <osil/bcm_osil.h>

/* Globals */
static const uint32_t BS_FrameCount = 2UL;

/* Test status */
static volatile int32_t BS_qt1_result = BCM_AT_NOT_STARTED;

/* Image - extern */
extern const uint8_t isp_image_array[];
extern const uint8_t unicam_image_array[];
extern const uint8_t cam_mode_image_array[];
extern const uint8_t sensor_common_image_array[];
extern const uint8_t sensor_mode_colorbar_image_array[];

#define BS_ALIGN(x, y)                 \
    ((((uint32_t)(x)) + (((uint32_t)(y)) - 1UL)) & (~(uint32_t)((y) - 1UL)))

/* YUV Frame size */
#define BS_ISP_HV_COMP_SIZE  (MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT * 2UL)
#define BS_ISP_HV_FRAME_SIZE (BS_ISP_HV_COMP_SIZE * 3UL)
#define BS_ISP_HV_SLICE_SIZE (MAX_VIDEO_WIDTH * 2UL * 16UL)

#define BS_ISP_CV_FRAME_SIZE (MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT * 2UL)

/* Allocation for ISYS buffers */
static uint8_t BS_YUVBufferGlobal[BS_ISP_HV_FRAME_SIZE * 2UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");
static uint8_t BS_RawBufferGlobal[BS_ALIGN(((MAX_VIDEO_WIDTH * RAW_OUT_BPP) / 8UL), 32UL) * MAX_VIDEO_HEIGHT * MAX_NUM_RAW_CHANNELS * 2UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");

/* Allocation for app buffers for DCG */
static uint8_t COMP_SECTION(".cam.raw.slowmem") BS_AppRawBufLongY[BS_ISP_HV_COMP_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.raw.slowmem") BS_AppRawBufLongU[BS_ISP_HV_COMP_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.raw.slowmem") BS_AppRawBufLongV[BS_ISP_HV_COMP_SIZE] COMP_ALIGN(256);

/* Allocation for app buffers for VS */
static uint8_t COMP_SECTION(".cam.raw.slowmem") BS_AppRawBufShortY[BS_ISP_HV_COMP_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.raw.slowmem") BS_AppRawBufShortU[BS_ISP_HV_COMP_SIZE] COMP_ALIGN(256);
static uint8_t COMP_SECTION(".cam.raw.slowmem") BS_AppRawBufShortV[BS_ISP_HV_COMP_SIZE] COMP_ALIGN(256);

const uint32_t BS_CB_Y_CRC_Long = 0xfe6346afUL;
const uint32_t BS_CB_U_CRC_Long = 0xfe6346afUL;
const uint32_t BS_CB_V_CRC_Long = 0xfe6346afUL;

const uint32_t BS_CB_Y_CRC_Short = 0xd5766312UL;
const uint32_t BS_CB_U_CRC_Short = 0xd5766312UL;
const uint32_t BS_CB_V_CRC_Short = 0xd5766312UL;

#define BS_CRC32_POLY (0x04C11DB7UL)
static uint32_t BS_Reverse(uint32_t aValue)
{
    uint32_t val = aValue;

    val = ((val & 0x55555555UL) << 1UL) | ((val >> 1UL) & 0x55555555UL);
    val = ((val & 0x33333333UL) << 2UL) | ((val >> 2UL) & 0x33333333UL);
    val = ((val & 0x0F0F0F0FUL) << 4UL) | ((val >> 4UL) & 0x0F0F0F0FUL);
    val = (val << 24UL) | ((val & 0xFF00UL) << 8UL) |
        ((val >> 8UL) & 0xFF00UL) | (val >> 24UL);

    return val;
}

static uint32_t BS_CalculateCRC(const uint8_t *const aAddr,
                             uint32_t aLen)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;

    i = 0UL;
    crc = 0UL;
    while (aLen != 0UL) {
        byte = aAddr[i];
        byte = BS_Reverse(byte);
        for (j = 0UL; j <= 7UL; j++) {
            if (((int32_t)(crc ^ byte)) < 0L) {
                crc = (crc << 1UL) ^ BS_CRC32_POLY;
            } else {
                crc = crc << 1UL;
            }
            byte = byte << 1UL;
        }
        i++;
        aLen--;
    }

    return BS_Reverse(~crc);
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

/* BayerStreamTask */
TASK(BayerStreamTask)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t yCRC, uCRC, vCRC;
    uint32_t frameCount;
    uint32_t fps = 0UL;
    uint32_t flags;
    ISYS_BufferType buf;
    ISYS_BufferType bufS;
    BCM_EventMaskType mask = 0UL;
    uint32_t notMatched = 0UL;
    uint32_t outIdx = 0x0UL;
    ISYS_FormatType format;
    uint32_t width = 0UL;
    uint32_t height = 0UL;
    uint32_t widthInbytes = 0x0UL;
    ISYS_InfoType info;
    uint32_t ySize, uSize, vSize, yStride, uStride, vStride;
    uint32_t startTime = 0UL;
    uint32_t endTime = 0UL;
    uint8_t hvDone = 0U;
    uint8_t cvDone = 0U;
    uint32_t lastSliceNum = 0UL;

    ISYS_ConfigsType config = {
            .isysMemPool[ISYS_MEMPOOL_IDX_YUV] = BS_YUVBufferGlobal,
            .isysMemPool[ISYS_MEMPOOL_IDX_RAW] = BS_RawBufferGlobal,
            .isysMemsize[ISYS_MEMPOOL_IDX_YUV] = sizeof(BS_YUVBufferGlobal),
            .isysMemsize[ISYS_MEMPOOL_IDX_RAW] = sizeof(BS_RawBufferGlobal),
            .dpMode = ISYS_DUAL_PASS_MODE_FRAME,
        };

    /* Init Buffers */
    BCM_MemSet(&BS_YUVBufferGlobal[0], 0, sizeof(BS_YUVBufferGlobal));
    BCM_MemSet(&BS_RawBufferGlobal[0], 0, sizeof(BS_RawBufferGlobal));

    BCM_MemSet(&BS_AppRawBufLongY[0], 0, sizeof(BS_AppRawBufLongY));
    BCM_MemSet(&BS_AppRawBufLongU[0], 0, sizeof(BS_AppRawBufLongU));
    BCM_MemSet(&BS_AppRawBufLongV[0], 0, sizeof(BS_AppRawBufLongV));

    BCM_MemSet(&BS_AppRawBufShortY[0], 0, sizeof(BS_AppRawBufShortY));
    BCM_MemSet(&BS_AppRawBufShortU[0], 0, sizeof(BS_AppRawBufShortU));
    BCM_MemSet(&BS_AppRawBufShortV[0], 0, sizeof(BS_AppRawBufShortV));

    ret = ISYS_Init();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Init failed (%d)\n",ret);
        goto func_exit;
    }

    do {
        if (BCM_ERR_OK == ret) {
            ret = CSN_Init();
            if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                ULOG_ERR("CSN_Init failed (%d)\n", ret);
                goto func_exit;
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
                goto func_exit;
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
        goto func_exit;
    }

    config.Cfg.pHdr = unicam_image_array;
    ret = ISYS_Configure(&config);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Configure failed for UNICAM Image (%d)\n",ret);
        goto func_exit;
    }

    config.Cfg.pHdr = isp_image_array;
    ret = ISYS_Configure(&config);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Configure failed for ISP Image (%d)\n",ret);
        goto func_exit;
    }

    ret = BCM_ERR_BUSY;
    while (ret == BCM_ERR_BUSY) {
        flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
        if (0UL != (flags & CSN_STATUS_ERROR)) {
            ret = BCM_ERR_UNKNOWN;
            ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
            goto func_exit;
        }
        if (0UL == (flags & CSN_STATUS_BUSY)) {
            ret = CSN_Configure((CSN_ImageType*)sensor_common_image_array);
            if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
                goto func_exit;
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
        goto func_exit;
    }

    ret = BCM_ERR_BUSY;
    while (ret == BCM_ERR_BUSY) {
        flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
        if (0UL != (flags & CSN_STATUS_ERROR)) {
            ret = BCM_ERR_UNKNOWN;
            ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
            goto func_exit;
        }
        if (0UL == (flags & CSN_STATUS_BUSY)) {
            ret = CSN_Configure((CSN_ImageType*)sensor_mode_colorbar_image_array);
            if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                ULOG_ERR("CSN_Configure failed for Sensor Image (%d)\n",ret);
                goto func_exit;
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
        goto func_exit;
    }

    /* Get Mode */
    ISYS_GetInfo(&info);
    outIdx = (0UL != (info.mode.passProps[ISYS_PASS_IDX_HV].outputEnable & ISYS_OUT_ENABLE_LORES)) ?
                           ISYS_OUT_IDX_LORES : ISYS_OUT_IDX_HIRES;
    format = info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].format;
    width = info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].width;
    widthInbytes = width * info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].outWidth;
    height = info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].height;
    lastSliceNum = (((height + 15UL) & ~15UL)/16UL) - 1UL;

    ret = ISYS_Start();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Start failed (%d)\n",ret);
        goto func_exit;
    }

    ret = BCM_ERR_BUSY;
    while (ret == BCM_ERR_BUSY) {
        flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
        if (0UL != (flags & CSN_STATUS_ERROR)) {
            ret = BCM_ERR_UNKNOWN;
            ULOG_ERR("CSN_Start failed for Sensor Image (%d)\n",ret);
            goto func_exit;
        }
        if (0UL == (flags & CSN_STATUS_BUSY)) {
            ret = CSN_Start();
            if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                ULOG_ERR("CSN_Start failed for Sensor Image (%d)\n",ret);
                goto func_exit;
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
        goto func_exit;
    }

    frameCount = 0UL;

    switch(format) {
        case ISYS_FORMAT_444_PLANAR:
            ySize = widthInbytes * 16UL;
            yStride = widthInbytes;
            uSize = vSize = widthInbytes * 16UL;
            uStride = vStride = widthInbytes;
            break;
        default:
            ULOG_ERR("%s: Unsupported format: %u \n", __FUNCTION__, format);
            goto func_exit;
    }

    buf.plane[0].addr = BS_AppRawBufLongY;
    buf.plane[0].size = ySize;
    buf.plane[0].stride = yStride;
    buf.plane[0].isEnabled = 1UL;

    buf.plane[1].addr = BS_AppRawBufLongU;
    buf.plane[1].size = uSize;
    buf.plane[1].stride = uStride;
    buf.plane[1].isEnabled = 1UL;

    buf.plane[2].addr = BS_AppRawBufLongV;
    buf.plane[2].size = vSize;
    buf.plane[2].stride = vStride;
    buf.plane[2].isEnabled = 1UL;

    ret = ISYS_FillBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
        goto func_exit;
    }

    bufS.plane[0].addr = BS_AppRawBufShortY;
    bufS.plane[0].size = BS_ISP_CV_FRAME_SIZE;
    bufS.plane[0].stride = yStride;
    bufS.plane[0].isEnabled = 1UL;

    bufS.plane[1].addr = BS_AppRawBufShortU;
    bufS.plane[1].size = BS_ISP_CV_FRAME_SIZE;
    bufS.plane[1].stride = uStride;
    bufS.plane[1].isEnabled = 1UL;

    bufS.plane[2].addr = BS_AppRawBufShortV;
    bufS.plane[2].size = BS_ISP_CV_FRAME_SIZE;
    bufS.plane[2].stride = vStride;
    bufS.plane[2].isEnabled = 1UL;

    ret = ISYS_FillBuffer(ISYS_PASS_IDX_CV, ISYS_OUT_IDX_MAX, &bufS);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
        goto func_exit;
    }

    while ((0U == hvDone) || (0U == cvDone)) {
        BCM_WaitEvent(ISP_EVENT | UNICAM_EVENT | PP_GPEvent);
        BCM_GetEvent(BayerStreamTask, &mask);
        BCM_ClearEvent(mask);
        if (0UL != (mask & ISP_EVENT) || 0UL != (mask & UNICAM_EVENT)) {
            BCM_ClearEvent(ISP_EVENT | UNICAM_EVENT);
            flags = ISYS_ProcessEvent();
            if (flags & ISYS_EVENT_ERROR) {
                break;
            }
        }

        if (0U == hvDone) {
            ret = ISYS_GetFilledBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
            if (BCM_ERR_OK == ret) {
                if (0UL == frameCount) {
                    startTime = (buf.timestamp)/BCM_NS_PER_SEC;
                }
                endTime = (buf.timestamp)/BCM_NS_PER_SEC;

                if (lastSliceNum == buf.sliceNum) {
                    frameCount++;
                    fps++;
                }

                if ((0UL != startTime) && (0UL != endTime) && (endTime != startTime)) {
                    ULOG_INFO("FPS: %d \n", fps);
                    fps = 0UL;
                    startTime = endTime;
                }

                if (frameCount < BS_FrameCount) {
                    buf.plane[0].addr = BS_AppRawBufLongY + ((buf.sliceNum + 1U) * yStride * 16U);
                    buf.plane[1].addr = BS_AppRawBufLongU + ((buf.sliceNum + 1U) * uStride * 16U);
                    buf.plane[2].addr = BS_AppRawBufLongV + ((buf.sliceNum + 1U) * vStride * 16U);

                    ret = ISYS_FillBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
                        break;
                    }
                } else {
                    hvDone = 1U;
                }
            }
        }

        if (0U == cvDone) {
            ret = ISYS_GetFilledBuffer(ISYS_PASS_IDX_CV, ISYS_OUT_IDX_MAX, &bufS);
            if (BCM_ERR_OK == ret) {
                if (frameCount < BS_FrameCount) {
                    ret = ISYS_FillBuffer(ISYS_PASS_IDX_CV, ISYS_OUT_IDX_MAX, &bufS);
                    if (BCM_ERR_OK != ret) {
                        ULOG_ERR("ISYS_FillBuffer failed (%d)\n", ret);
                        goto func_exit;
                    }
                } else {
                    cvDone = 1U;
                }
            }
        }
    }
    ULOG_INFO("Received (%u) frames\n", frameCount);

    ret = BCM_ERR_BUSY;
    while (ret == BCM_ERR_BUSY) {
        flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
        if (0UL != (flags & CSN_STATUS_ERROR)) {
            ret = BCM_ERR_UNKNOWN;
            ULOG_ERR("CSN_Stop failed for Sensor Image (%d)\n",ret);
            goto func_exit;
        }
        if (0UL == (flags & CSN_STATUS_BUSY)) {
            ret = CSN_Stop();
            if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                ULOG_ERR("CSN_Stop failed for Sensor Image (%d)\n",ret);
                goto func_exit;
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
        goto func_exit;
    }

    ret = ISYS_Stop();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Stop failed (%d)\n",ret);
        goto func_exit;
    }

    ret = CSN_Deinit();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CSN_Deinit failed (%d)\n",ret);
        goto func_exit;
    }

    ret = ISYS_Deinit();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISYS_Deinit failed (%d)\n",ret);
        goto func_exit;
    }

    yCRC = BS_CalculateCRC(BS_AppRawBufLongY, BS_ISP_HV_COMP_SIZE);
    uCRC = BS_CalculateCRC(BS_AppRawBufLongU, BS_ISP_HV_COMP_SIZE);
    vCRC = BS_CalculateCRC(BS_AppRawBufLongV, BS_ISP_HV_COMP_SIZE);
    if ((yCRC != BS_CB_Y_CRC_Long) ||
            (uCRC != BS_CB_U_CRC_Long) ||
            (vCRC != BS_CB_V_CRC_Long)) {
        notMatched = 1UL;
        ULOG_ERR("Long: yCRC = 0x%x \n", yCRC);
        ULOG_ERR("Long: uCRC = 0x%x \n", uCRC);
        ULOG_ERR("Long: vCRC = 0x%x \n", vCRC);
    }

    yCRC = BS_CalculateCRC(BS_AppRawBufShortY, BS_ISP_HV_COMP_SIZE);
    uCRC = BS_CalculateCRC(BS_AppRawBufShortU, BS_ISP_HV_COMP_SIZE);
    vCRC = BS_CalculateCRC(BS_AppRawBufShortV, BS_ISP_HV_COMP_SIZE);
    if ((yCRC != BS_CB_Y_CRC_Short) ||
            (uCRC != BS_CB_U_CRC_Short) ||
            (vCRC != BS_CB_V_CRC_Short)) {
        notMatched = 1UL;
        ULOG_ERR("Short: yCRC = 0x%x \n", yCRC);
        ULOG_ERR("Short: uCRC = 0x%x \n", uCRC);
        ULOG_ERR("Short: vCRC = 0x%x \n", vCRC);
    }

    if ((BS_FrameCount == frameCount) && (0UL == notMatched)) {
        ULOG_INFO("Test Passed!\n");
        if (BCM_AT_EXECUTING == BS_qt1_result) {
            BS_qt1_result = BCM_ERR_OK;
        }
    } else {
        ULOG_INFO("Test Failed!\n");
        if (BCM_AT_EXECUTING == BS_qt1_result) {
            BS_qt1_result = BCM_ERR_UNKNOWN;
        }
    }

func_exit:
    BCM_TerminateTask();
}

/* BCM_ExecuteAT */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        BS_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(BayerStreamTask);
        ret = BCM_ERR_OK;
    }

    return ret;
}

/* BCM_GetResultAT */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        ret = BS_qt1_result;
    }

    return ret;
}

void ApplicationInit()
{
}
