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
    @file unicam_csi.c
    @brief Test for UNICAM CSI streaming
    This tests unicam with CSI
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
#include <osil/bcm_osil.h>
#include <pp.h>

/* camera */
#include <cam_mode_image.h>
#include <cam_erika.h>
#include <unicam.h>
#include <unicam_osil.h>
#include <camera_sensor.h>
#include <camera_sensor_osil.h>

/* Globals */
static const uint32_t UCMCSIQT_FrameCount = 100UL;

/* Test status */
static volatile int32_t UCMCSIQT_qt1_result = BCM_AT_NOT_STARTED;

/* Image - extern */
extern const uint8_t unicam_image_array[];
extern const uint8_t cam_mode_image_array[];
extern const uint8_t sensor_common_image_array[];
extern const uint8_t sensor_mode_colorbar_image_array[];

#define UCMCSIQT_ALIGN(x, y)                 \
    ((((uint32_t)(x)) + (((uint32_t)(y)) - 1UL)) & (~(uint32_t)((y) - 1UL)))

/* Allocation for UNICAM IMG buffers */
static uint8_t UCMCSIQT_RawBufferGlobal[UCMCSIQT_ALIGN(((MAX_VIDEO_WIDTH * RAW_OUT_BPP) / 8UL), 32UL) * MAX_VIDEO_HEIGHT * MAX_NUM_RAW_CHANNELS * 2UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");
static uint8_t UCMCSIQT_DataBufferGlobal[UCMCSIQT_ALIGN(((MAX_VIDEO_WIDTH * RAW_OUT_BPP) / 8UL), 32UL) * 8UL * MAX_NUM_RAW_CHANNELS * 2UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");

/* Allocation for UNICAM IMG buffers */
static uint8_t UCMCSIQT_AppRawBufferGlobal[MAX_NUM_RAW_CHANNELS][UCMCSIQT_ALIGN(((MAX_VIDEO_WIDTH * RAW_OUT_BPP) / 8UL), 32UL) * MAX_VIDEO_HEIGHT] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");

const uint32_t UCMCSIQT_CB_CRC_Raw = 0x4d235923UL;

#define UCMCSIQT_CRC32_POLY (0x04C11DB7UL)
static uint32_t UCMCSIQT_Reverse(uint32_t aValue)
{
    uint32_t val = aValue;

    val = ((val & 0x55555555UL) << 1UL) | ((val >> 1UL) & 0x55555555UL);
    val = ((val & 0x33333333UL) << 2UL) | ((val >> 2UL) & 0x33333333UL);
    val = ((val & 0x0F0F0F0FUL) << 4UL) | ((val >> 4UL) & 0x0F0F0F0FUL);
    val = (val << 24UL) | ((val & 0xFF00UL) << 8UL) |
        ((val >> 8UL) & 0xFF00UL) | (val >> 24UL);

    return val;
}

static uint32_t UCMCSIQT_CalculateCRC(const uint8_t *const aAddr,
                             uint32_t aLen)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;

    i = 0UL;
    crc = 0UL;
    while (aLen != 0UL) {
        byte = aAddr[i];
        byte = UCMCSIQT_Reverse(byte);
        for (j = 0UL; j <= 7UL; j++) {
            if (((int32_t)(crc ^ byte)) < 0L) {
                crc = (crc << 1UL) ^ UCMCSIQT_CRC32_POLY;
            } else {
                crc = crc << 1UL;
            }
            byte = byte << 1UL;
        }
        i++;
        aLen--;
    }

    return UCMCSIQT_Reverse(~crc);
}

/* UCMCSIQT_CalcInputStride */
static int32_t UCMCSIQT_CalcInputStride(const ISYS_ModeType *aMode, uint32_t *aStride)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t bpp = 0UL;

    /* To get image id's and bpp */
    switch (aMode->inputFormat) {
        case UNICAM_FORMAT_RAW6:
            bpp = 6UL;
            break;
        case UNICAM_FORMAT_RAW7:
            bpp = 7UL;
            break;
        case UNICAM_FORMAT_RAW8:
            bpp = 8UL;
            break;
        case UNICAM_FORMAT_RAW10:
            bpp = 10UL;
            break;
        case UNICAM_FORMAT_RAW12:
            bpp = 12UL;
            break;
        case UNICAM_FORMAT_RAW14:
            bpp = 14UL;
            break;
        case UNICAM_FORMAT_RAW16:
            bpp = 16UL;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (BCM_ERR_OK == ret) {
        if (UNICAM_MODE_TG == aMode->unicamMode) {
            *aStride = UCMCSIQT_ALIGN(((UCMCSIQT_ALIGN(bpp, 8UL) * aMode->inputWidth) >>
                                                3UL), 32UL);
        } else {
            *aStride = UCMCSIQT_ALIGN(((bpp * aMode->inputWidth) >> 3UL),
                                  32UL);
        }
    }

    return ret;
}

/* ISYS_ED_START_LINES */
#define UCMCSIQT_ED_START_LINES(x) (((uint32_t) modeCfg->mode.metaLines[(x)] & \
                            UNICAM_MODECFG_META_LINES_START_MASK) >> \
                            UNICAM_MODECFG_META_LINES_START_SHIFT)

/* UCMCSIQT_ED_END_LINES */
#define UCMCSIQT_ED_END_LINES(x)   (((uint32_t) modeCfg->mode.metaLines[(x)] & \
                            UNICAM_MODECFG_META_LINES_END_MASK) >> \
                            UNICAM_MODECFG_META_LINES_END_SHIFT)

/* Invalid index for SP DMA index */
#define UCMCSIQT_DMA_INVALID_INDEX          (0xffffffffUL)

/* UNICAM_CSITestTask */
TASK(UNICAM_CSITestTask)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t rawCRC;
    uint32_t frameCount = 0UL;
    uint32_t fps = 0UL;
    uint32_t flags;
    BCM_EventMaskType mask = 0UL;
    uint32_t notMatched = 0UL;
    uint32_t startTime = 0UL;
    uint32_t endTime = 0UL;
    UNICAM_CommonIfcConfigType ucmIfcConfig;
    uint32_t inputStride;
    uint32_t inImgStart[UNICAM_MAX_NUM_IMG_BUFS][UNICAM_OUTPUT_IDX_MAX];
    uint32_t inImgEnd[UNICAM_MAX_NUM_IMG_BUFS][UNICAM_OUTPUT_IDX_MAX];
    uint32_t inDataStart[UNICAM_MAX_NUM_DATA_BUFS][UNICAM_OUTPUT_IDX_MAX];
    uint32_t inDataEnd[UNICAM_MAX_NUM_DATA_BUFS][UNICAM_OUTPUT_IDX_MAX];
    uint32_t i, j;
    UNICAM_BufParamType unicamBufParam;
    PP_DMABufferType dmaInBuf, dmaOutBuf;
    uint32_t dmaJobIdx = UCMCSIQT_DMA_INVALID_INDEX;
    const UNICAM_ImageType *ucmCfg = (const UNICAM_ImageType *) &unicam_image_array;
    const ISYS_ModeImageType *modeCfg = (const ISYS_ModeImageType *) &cam_mode_image_array;
    uint32_t rawMemPool = (uint32_t) &UCMCSIQT_RawBufferGlobal[0];
    uint32_t unicamLines = modeCfg->mode.inputHeight;
    uint32_t rawMemSize = sizeof(UCMCSIQT_RawBufferGlobal);
    uint32_t numRawBufs = 2UL;
    uint32_t numExposures = 1UL;
    uint32_t dataPtr = (uint32_t) &UCMCSIQT_DataBufferGlobal[0];

    /* Init Buffers */
    BCM_MemSet(&UCMCSIQT_RawBufferGlobal[0], 0, rawMemSize);
    BCM_MemSet(&UCMCSIQT_DataBufferGlobal[0], 0, sizeof(UCMCSIQT_DataBufferGlobal));
    BCM_MemSet(&UCMCSIQT_AppRawBufferGlobal[0][0], 0, sizeof(UCMCSIQT_AppRawBufferGlobal));

    ret = UNICAM_Init();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("UNICAM_Init failed (%d)\n",ret);
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

    /* Calculate input stride */
    ret = UCMCSIQT_CalcInputStride(&modeCfg->mode, &inputStride);
    if (BCM_ERR_OK != ret) {
        goto func_exit;
    }

    if (0UL != modeCfg->mode.hdr) {
        numExposures = 2UL;
    }

    /* Calculate UNICAM image Start-End addresses */
    for (i = 0UL; i < numExposures; i++) {
        for (j = 0UL; j < numRawBufs; j++) {
            inImgStart[j][i] = (rawMemPool +
                               (i * numRawBufs * inputStride * unicamLines) +
                               (j * inputStride * unicamLines));
            inImgEnd[j][i] = inImgStart[j][i];
            inImgEnd[j][i] += (inputStride * unicamLines);

            /* Check for memory */
            if (inImgEnd[j][i] > (rawMemPool + rawMemSize)) {
                ret = BCM_ERR_NOMEM;
                goto func_exit;
            }
        }
    }

    /* Calculate UNICAM data Start-End addresses */
    for (i = 0UL; i < numExposures; i++) {
        for (j = 0UL; j < UNICAM_MAX_NUM_DATA_BUFS; j++) {
            inDataStart[j][i] = dataPtr;
            dataPtr += (inputStride *
                       ((uint32_t) UCMCSIQT_ED_START_LINES(i) +
                        (uint32_t) UCMCSIQT_ED_END_LINES(i)));
            inDataEnd[j][i] = dataPtr;
        }
    }

    /* Configure Unicam image buffer pointers */
    for (i = 0UL; i < numExposures; i++) {
        for (j = 0UL; j < numRawBufs; j++) {
            unicamBufParam.imgBuf[j][i].startAddr =
                inImgStart[j][i];
            unicamBufParam.imgBuf[j][i].endAddr =
                inImgEnd[j][i];
        }
    }
    unicamBufParam.numImgBufs = numRawBufs;
    unicamBufParam.numExposures = numExposures;

    /* Configure Unicam data buffer pointers */
    for (i = 0UL; i < numExposures; i++) {
        for (j = 0UL; j < UNICAM_MAX_NUM_DATA_BUFS; j++) {
            unicamBufParam.dataBuf[j][i].startAddr = inDataStart[j][i];
            unicamBufParam.dataBuf[j][i].endAddr = inDataEnd[j][i];
        }
    }

    /* Prepare UNICAM interface configuration */
    ucmIfcConfig.hdr = modeCfg->mode.hdr;
    ucmIfcConfig.fps = modeCfg->mode.fps;
    ucmIfcConfig.metaLines[0UL] = modeCfg->mode.metaLines[0UL];
    ucmIfcConfig.metaLines[1UL] = modeCfg->mode.metaLines[1UL];
    ucmIfcConfig.shortLinesOffset = modeCfg->mode.shortLinesOffset;
    ucmIfcConfig.width = modeCfg->mode.inputWidth;
    ucmIfcConfig.height = modeCfg->mode.inputHeight;
    ucmIfcConfig.stride = (uint16_t) inputStride;

    ret = UNICAM_Configure(ucmCfg,
                           &ucmIfcConfig,
                           &unicamBufParam);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("UNICAM_Configure failed for UNICAM Image (%d)\n",ret);
        goto func_exit;
    }

    /* Configure Camera Sensor for common image */
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

    /* Configure Camera Sensor for mode image */
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

    ret = UNICAM_Start();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("UNICAM_Start failed (%d)\n",ret);
        goto func_exit;
    }

    /* Start camera sensor */
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

    /* Wait for EOT */
    while(1) {
        uint32_t inBufIdx = 0UL;
        uint32_t flags;
        BCM_TimeType timestamp = {0UL, 0UL};

        BCM_WaitEvent(UNICAM_EVENT | PP_GPEvent);
        BCM_GetEvent(UNICAM_CSITestTask, &mask);
        BCM_ClearEvent(mask);

        if (0UL != (mask & PP_GPEvent)) {
            if (UCMCSIQT_DMA_INVALID_INDEX != dmaJobIdx) {
                ret = PP_DMAStatus(dmaJobIdx);
                if ((BCM_ERR_OK == ret) || (BCM_ERR_BUSY == ret)) {
                    if (BCM_ERR_OK == ret) {
                        dmaJobIdx = UCMCSIQT_DMA_INVALID_INDEX;
                    }
                } else {
                    ULOG_ERR("PP_DMAStatus failed (%d)\n", ret);
                }
            } else {
                ULOG_ERR("Unexpected event (%d)\n", __LINE__);
            }
        }

        flags = UNICAM_GetEvent();
        if (0UL != (flags & UNICAM_INT_FS)) {
            BCM_GetTime(&timestamp);
            if (0UL == startTime) {
                startTime = timestamp.s;
            }
        }

        if (0UL != (flags & UNICAM_INT_FE)) {
            /* got a frame */
            BCM_GetTime(&timestamp);
            endTime = timestamp.s;
            frameCount++;
            fps++;

            if (UCMCSIQT_DMA_INVALID_INDEX == dmaJobIdx) {
                inBufIdx = UNICAM_GetCurBufIdx();

                /* Initialize channel number */
                dmaInBuf.numCH = 0UL;
                dmaOutBuf.numCH = 0UL;

                for (i = 0UL; i < numExposures; i++) {
                    /* Initialize DMA-In configurations */
                    dmaInBuf.channel[i].addr = (uint8_t *) unicamBufParam.imgBuf[inBufIdx][i].startAddr;
                    dmaInBuf.channel[i].width = inputStride;
                    dmaInBuf.channel[i].height = unicamLines;
                    dmaInBuf.channel[i].stride = inputStride;
                    dmaInBuf.numCH++;

                    /* Initialize DMA-out configurations */
                    dmaOutBuf.channel[i].addr = &UCMCSIQT_AppRawBufferGlobal[i][0];
                    dmaOutBuf.channel[i].width = inputStride;
                    dmaOutBuf.channel[i].height = unicamLines;
                    dmaOutBuf.channel[i].stride = inputStride;
                    dmaOutBuf.numCH++;
                }

                ret = PP_DMAStart(&dmaInBuf,
                                  &dmaOutBuf,
                                  &dmaJobIdx);
                if (BCM_ERR_OK != ret) {
                    ULOG_ERR("PP_DMAStart failed (%d)\n", ret);
                }
            }

            if ((0UL != startTime) && (0UL != endTime) && (endTime != startTime)) {
                ULOG_INFO("FPS: %d \n", fps);
                fps = 0UL;
                startTime = 0UL;
            }

            if (frameCount >= UCMCSIQT_FrameCount) {
                break;
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

    ret = UNICAM_Stop();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("UNICAM_Stop failed (%d)\n",ret);
        goto func_exit;
    }

    ret = CSN_Deinit();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("CSN_Deinit failed (%d)\n",ret);
        goto func_exit;
    }

    ret = UNICAM_Deinit();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("UNICAM_Deinit failed (%d)\n",ret);
        goto func_exit;
    }

    rawCRC = UCMCSIQT_CalculateCRC(&UCMCSIQT_AppRawBufferGlobal[0][0], sizeof(UCMCSIQT_AppRawBufferGlobal));
    if (rawCRC != UCMCSIQT_CB_CRC_Raw) {
        notMatched = 1UL;
        ULOG_ERR("rawCRC = 0x%x \n", rawCRC);
    }

    if ((UCMCSIQT_FrameCount == frameCount) && (0UL == notMatched)) {
        ULOG_INFO("Test Passed!\n");
        if (BCM_AT_EXECUTING == UCMCSIQT_qt1_result) {
            UCMCSIQT_qt1_result = BCM_ERR_OK;
        }
    } else {
        ULOG_INFO("Test Failed!\n");
        if (BCM_AT_EXECUTING == UCMCSIQT_qt1_result) {
            UCMCSIQT_qt1_result = BCM_ERR_UNKNOWN;
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
        UCMCSIQT_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(UNICAM_CSITestTask);
        ret = BCM_ERR_OK;
    }

    return ret;
}

/* BCM_GetResultAT */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        ret = UCMCSIQT_qt1_result;
    }

    return ret;
}

void ApplicationInit()
{
}
