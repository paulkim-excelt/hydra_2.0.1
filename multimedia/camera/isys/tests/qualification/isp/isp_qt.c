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
    @defgroup grp_isp_rt Qualification Tests
    @ingroup grp_isp

    @addtogroup grp_isp_rt
    @{

    @file isp_qt.c
    @brief ISP Qualification Test
    This source file contains the requirement tests for ISP
    @version 0.1 Initial Version
*/

#define GetModuleLogLevel() ISPQT_GetTestLogLevel()

/* Includes */
#include <string.h>
#include <ee.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <mcu_ext.h>
#include <cam_mode_image.h>
#include <cam_erika.h>
#include <isp.h>
#include <isp_osil.h>
#include <osil/bcm_osil.h>
#include <isp_image_v2.h>
#include <ov10640_csi_isp_image.h>

/* Plane sizes */
#define ISPQT_PLANE_SIZE  (MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT)

#define ISPQT_IN_BUF_SIZE  (MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT * 2UL)

/* Space for Input Image */
#define UNICAM_IMG_SIZE      (0x43000UL)

/* Globals */
static volatile int32_t ISPQT_qt1_result = BCM_AT_NOT_STARTED;

/* Image - extern */
extern const uint8_t isp_image_array[];
extern const uint8_t cam_mode_image_array[];

/* ISP Test Log level */
static uint32_t ISPQT_TestLogLevel = ULOG_LVL_VERBOSE;

/* Get ISP Test Log Level */
COMP_INLINE uint32_t ISPQT_GetTestLogLevel()
{
    return ISPQT_TestLogLevel;
}

/* Allocation for ISP out buffers */
static uint8_t COMP_SECTION(".cam.raw.slowmem") ISPQT_OutBuf[ISP_IMAGE_PLANE_IDX_MAX][ISPQT_PLANE_SIZE]  COMP_ALIGN(256);
/* ISP input buffer */
static uint8_t COMP_SECTION(".cam.raw.slowmem") ISPQT_InBuf[ISP_IN_IDX_MAX][ISPQT_IN_BUF_SIZE] COMP_ALIGN(256);

/* Reference CRCs */
const uint32_t ISPQT_CRC[ISP_IMAGE_PLANE_IDX_MAX] = { 0x47d0cf5bUL,
                                                      0xfa95ddaaUL,
                                                      0xc2230e04UL};
const uint32_t ISPQT_StatsCRC = 0x4a415e49UL;

static uint8_t ISYS_UnicamImgMemory[UNICAM_IMG_SIZE] COMP_SECTION(".image.input");

/* CRC calculation APIs */
#define ISPQT_CRC32_POLY (0x04C11DB7UL)
static uint32_t ISPQT_Reverse(uint32_t aValue)
{
    uint32_t val = aValue;

    val = ((val & 0x55555555UL) << 1UL) | ((val >> 1UL) & 0x55555555UL);
    val = ((val & 0x33333333UL) << 2UL) | ((val >> 2UL) & 0x33333333UL);
    val = ((val & 0x0F0F0F0FUL) << 4UL) | ((val >> 4UL) & 0x0F0F0F0FUL);
    val = (val << 24UL) | ((val & 0xFF00UL) << 8UL) |
        ((val >> 8UL) & 0xFF00UL) | (val >> 24UL);

    return val;
}

static uint32_t ISPQT_CalculateCRC(const uint8_t *const aAddr,
                                   uint32_t aLen)
{
    uint32_t i, j;
    uint32_t byte;
    uint32_t crc;

    i = 0UL;
    crc = 0UL;
    while (aLen != 0UL) {
        byte = aAddr[i];
        byte = ISPQT_Reverse(byte);
        for (j = 0UL; j <= 7UL; j++) {
            if (((int32_t)(crc ^ byte)) < 0L) {
                crc = (crc << 1UL) ^ ISPQT_CRC32_POLY;
            } else {
                crc = crc << 1UL;
            }
            byte = byte << 1UL;
        }
        i++;
        aLen--;
    }

    return ISPQT_Reverse(~crc);
}

#ifndef ISPQT_DISABLE_BLOCK_CONFIGURATION
extern ISP_RawInputConfigType * ISPQT_GetIIConfig(void);
extern ISP_BlackLevelConfigType * ISPQT_GetBLConfig(void);
extern ISP_LensShadingConfigType * ISPQT_GetLSConfig(void);
extern ISP_HDRConfigType * ISPQT_GetHDRConfig(void);
extern ISP_BayerDenoiseConfigType * ISPQT_GetDNConfig(void);
extern ISP_StatisticsConfigType * ISPQT_GetStatisticsConfig(void);
extern ISP_WBConfigType * ISPQT_GetWBConfig(void);
extern ISP_GainConfigType * ISPQT_GetGainConfig(void);
extern ISP_DefectivePixelConfigType * ISPQT_GetDPConfig(void);
extern ISP_FringingConfigType * ISPQT_GetFGConfig(void);
extern ISP_CrossTalkConfigType * ISPQT_GetXCConfig(void);
extern ISP_ColourDenoiseConfigType * ISPQT_GetCDNConfig(void);
extern ISP_DemosaicConfigType * ISPQT_GetDMConfig(void);
extern ISP_SharpenConfigType * ISPQT_GetSHConfig(void);
extern ISP_ColourCorrectionConfigType * ISPQT_GetYCConfig(void);
extern ISP_GammaConfigType * ISPQT_GetGMConfig(void);
extern ISP_RGB2YUVConversionConfigType * ISPQT_GetYGConfig(void);
extern ISP_FalseColourConfigType * ISPQT_GetFCConfig(void);
extern ISP_LTMConfigType * ISPQT_GetLTMConfig(void);
extern ISP_ChrominanceStretchConfigType * ISPQT_GetCPConfig(void);
extern ISP_ToneMapConfigType * ISPQT_GetTMConfig(void);
extern ISP_ConversionConfigType * ISPQT_GetCCConfig(void);
extern ISP_OutputConfigType * ISPQT_GetHOConfig(void);
extern ISP_LoresResizeConfigType * ISPQT_GetLRConfig(void);
extern ISP_OutputConfigType * ISPQT_GetLOConfig(void);
extern ISP_FrameConfigType * ISPQT_GetFRConfig(void);
#endif /* ISPQT_DISABLE_BLOCK_CONFIGURATION */

static ISP_StatsOutputType ISPQT_Stats;

static uint8_t ISYS_UnicamImgMemory[UNICAM_IMG_SIZE] COMP_SECTION(".image.input");

/* ISPQT_Task */
TASK(ISPQT_Task)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t crc[ISP_IMAGE_PLANE_IDX_MAX];
    uint32_t notMatched = 0UL;
#ifndef ISPQT_DISABLE_ISPIMGV2_CONFIGURATION
    const ISPIMGV2_ConfigImageType *ispV2Img = (const ISPIMGV2_ConfigImageType *) ov10640_csi_isp_image_array;
#endif
    const ISP_ImageType *ispCfg = (const ISP_ImageType *) &isp_image_array;
    const ISYS_ModeImageType *modeCfg = (const ISYS_ModeImageType *) &cam_mode_image_array;
    uint32_t numExposures = 1UL;
    uint32_t inImgStart[ISP_IN_IDX_MAX];
    uint32_t inImgEnd[ISP_IN_IDX_MAX];
    BCM_EventMaskType mask = 0UL;
    ISP_DualPassParamType dualPassParam;
    ISP_InBufParamType ispInBufParam;
    ISP_OutBufParamType ispOutBufParam;
    ISP_HandshakeParamType ispHandshakeParam;
    const ISYS_PassPropsType *hvPassProps = &modeCfg->mode.passProps[ISP_PASS_IDX_HV];
    uint32_t hvIdx;
    uint32_t i;
    uint32_t statsCRC;
    ISP_StatsOutputType *stats;
#ifndef ISPQT_DISABLE_BLOCK_CONFIGURATION
    ISP_RawInputConfigType *iiConfig;
    ISP_BlackLevelConfigType *blConfig;
    ISP_LensShadingConfigType *lsConfig;
    ISP_HDRConfigType *hdrConfig;
    ISP_BayerDenoiseConfigType *dnConfig;
    ISP_StatisticsConfigType *stConfig;
    ISP_WBConfigType *wbConfig;
    ISP_GainConfigType *gainConfig;
    ISP_DefectivePixelConfigType *dpConfig;
    ISP_FringingConfigType *fgConfig;
    ISP_CrossTalkConfigType *xcConfig;
    ISP_ColourDenoiseConfigType *cdnConfig;
    ISP_DemosaicConfigType *dmConfig;
    ISP_SharpenConfigType *shConfig;
    ISP_ColourCorrectionConfigType *ycConfig;
    ISP_GammaConfigType *gmConfig;
    ISP_RGB2YUVConversionConfigType *ygConfig;
    ISP_FalseColourConfigType *fcConfig;
    ISP_LTMConfigType *ltmConfig;
    ISP_ChrominanceStretchConfigType *cpConfig;
    ISP_ToneMapConfigType *tmConfig;
    ISP_ConversionConfigType *ccConfig;
    ISP_OutputConfigType *hoConfig;
    ISP_LoresResizeConfigType *lrConfig;
    ISP_OutputConfigType *loConfig;
    ISP_FrameConfigType *frConfig;
#endif /* ISPQT_DISABLE_BLOCK_CONFIGURATION */

    /* Initialize buffer params */
    memset(&ispInBufParam, 0, sizeof(ispInBufParam));
    memset(&ispOutBufParam, 0, sizeof(ispInBufParam));
    memset(&ispHandshakeParam, 0, sizeof(ispInBufParam));

    /* Memset input buffer */
    memset(&ISPQT_InBuf, 0xa, sizeof(ISPQT_InBuf));

    /* Memset output buffer */
    memset(&ISPQT_OutBuf[0][0], 0, sizeof(ISPQT_OutBuf));

    /* Memset stats buffer */
    memset(&ISPQT_Stats, 0, sizeof(ISPQT_Stats));

    if (0UL != modeCfg->mode.hdr) {
        numExposures = 2UL;
    }
    hvIdx = (0UL !=
              (hvPassProps->outputEnable &
              ISP_OUT_ENABLE_HIRES)) ? ISP_OUT_IDX_HIRES : ISP_OUT_IDX_LORES;

    ret = ISP_Init();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_Init failed (%d)\n", ret);
        goto func_exit;
    }

    /* Disable HW sync */
    ret = ISP_DisableHWSync();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_DisableHWSync failed (%d)\n", ret);
        goto func_exit;
    }

    /* Enable HW handshake */
    if (ISP_OUT_IDX_HIRES == hvIdx) {
        MCU_SetDispHsSrc(MCU_CPUDHSCH_0, MCU_CPUDHSID_ISP);
    } else {
        MCU_SetDispHsSrc(MCU_CPUDHSCH_1, MCU_CPUDHSID_ISP);
    }

    /* Set ISYS dual pass mode */
    dualPassParam.dpMode = ISYS_DUAL_PASS_MODE_NONE;
    dualPassParam.hvIndex = hvIdx;
    for (i = 0UL; i < ISYS_PASS_IDX_MAX; i++) {
        dualPassParam.outputEnable[i] = modeCfg->mode.passProps[i].outputEnable;
    }
    ret = ISP_ConfigureDualPass(&dualPassParam);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureDualPass failed (%d)\n", ret);
        goto func_exit;
    }

    /* Calculate Input image Start-End addresses */
    for (i = 0UL; i < numExposures; i++) {
        inImgStart[i] = (uint32_t) &ISPQT_InBuf[i][0];
        inImgEnd[i] = (uint32_t) &ISPQT_InBuf[i][ISPQT_IN_BUF_SIZE];
    }
    for (i = 0UL; i < numExposures; i++) {
        /* ISP Input */
        ispInBufParam.bufParam[0][i].imagePlane[ISYS_PLANE_RY].startAddr = inImgStart[i];
        ispInBufParam.bufParam[0][i].imagePlane[ISYS_PLANE_RY].endAddr = inImgEnd[i];
        ispInBufParam.bufParam[0][i].imagePlane[ISYS_PLANE_RY].isEnabled = 1U;
    }
    ret = ISP_ConfigureInputBuffer(&ispInBufParam);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureInputBuffer failed (%d)\n", ret);
        goto func_exit;
    }

    /* Set Output Buffer params */
    ispOutBufParam.numBufs = 1UL;
    ispOutBufParam.bufParam[0][hvIdx].numMBrows =
               (((hvPassProps->outputProps[hvIdx].height + 15UL) & ~15UL)/16UL);
    ispOutBufParam.bufParam[0][hvIdx ].numSlices =
               (((hvPassProps->outputProps[hvIdx].height + 15UL) & ~15UL)/16UL);
    for (i = 0UL; i < ISP_IMAGE_PLANE_IDX_MAX; i++) {
        ispOutBufParam.bufParam[0][hvIdx ].imagePlane[i].startAddr =
                                                 (uint32_t) &ISPQT_OutBuf[i][0];
        ispOutBufParam.bufParam[0][hvIdx ].imagePlane[i].endAddr =
                                  (uint32_t) &ISPQT_OutBuf[i][ISPQT_PLANE_SIZE];
        ispOutBufParam.bufParam[0][hvIdx ].imagePlane[i].isEnabled = 1U;
        ispOutBufParam.bufParam[0][hvIdx ].imagePlane[i].stride =
                                   hvPassProps->outputProps[hvIdx].stride[i];
        ispOutBufParam.bufParam[0][hvIdx ].imagePlane[i].allocLines =
                                   hvPassProps->outputProps[hvIdx].height;
        ispOutBufParam.bufParam[0][hvIdx ].imagePlane[i].widthInBytes =
                                   hvPassProps->outputProps[hvIdx].stride[i];
    }
    ret = ISP_ConfigureOuputBuffer(&ispOutBufParam);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureOuputBuffer failed (%d)\n", ret);
        goto func_exit;
    }

    /* Set Handshake params */
    ispHandshakeParam.loResSyncLines = 16U;
    ispHandshakeParam.loResSyncThreshold = 48U;
    ispHandshakeParam.hiResSyncLines = 16U;
    ispHandshakeParam.hiResSyncThreshold = 48U;
    ret = ISP_ConfigureHandshake(&ispHandshakeParam);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureHandshake failed (%d)\n", ret);
        goto func_exit;
    }

    /* Configure ISP */
    ret = ISP_Configure(ispCfg);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_Configure failed for ISP Image (%d)\n", ret);
        goto func_exit;
    }

#ifndef ISPQT_DISABLE_ISPIMGV2_CONFIGURATION
    ret = ISPIMGV2_Configure(ispV2Img,
                            (void *) (&ISYS_UnicamImgMemory[0]),
                             sizeof(ISYS_UnicamImgMemory));
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISPIMGV2_Configure failed with error (%d)\n", ret);
        goto func_exit;
    }
#endif

#ifndef ISPQT_DISABLE_BLOCK_CONFIGURATION
    /* Override Block configurations */
    /* Override II configurations */
    iiConfig = ISPQT_GetIIConfig();
    ret = ISP_ConfigureRawInput(ISP_CFG_PASS_HV, iiConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureRawInput failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override BL configurations */
    blConfig = ISPQT_GetBLConfig();
    ret = ISP_ConfigureBlackLevel(ISP_CFG_PASS_HV, blConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureBlackLevel failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override LS configurations */
    lsConfig = ISPQT_GetLSConfig();
    ret = ISP_ConfigureLensShading(ISP_CFG_PASS_HV, lsConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureLensShading failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override HDR configurations */
    hdrConfig = ISPQT_GetHDRConfig();
    ret = ISP_ConfigureHDR(ISP_CFG_PASS_HV, hdrConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureHDR failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override DN configurations */
    dnConfig = ISPQT_GetDNConfig();
    ret = ISP_ConfigureBayerDenoise(ISP_CFG_PASS_HV, dnConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureBayerDenoise failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Statistics configurations */
    stConfig = ISPQT_GetStatisticsConfig();
    ret = ISP_ConfigureStatistics(ISP_CFG_PASS_HV, stConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureStatistics failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override WB configurations */
    wbConfig = ISPQT_GetWBConfig();
    ret = ISP_ConfigureWB(ISP_CFG_PASS_HV, wbConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureWB failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Gain configurations */
    gainConfig = ISPQT_GetGainConfig();
    ret = ISP_ConfigureGain(ISP_CFG_PASS_HV, gainConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureGain failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override DP configurations */
    dpConfig = ISPQT_GetDPConfig();
    ret = ISP_ConfigureDefectivePixel(ISP_CFG_PASS_HV, dpConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureDefectivePixel failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override FG configurations */
    fgConfig = ISPQT_GetFGConfig();
    ret = ISP_ConfigureFringing(ISP_CFG_PASS_HV, fgConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureFringing failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override CrossTalk configurations */
    xcConfig = ISPQT_GetXCConfig();
    ret = ISP_ConfigureCrossTalk(ISP_CFG_PASS_HV, xcConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureCrossTalk failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Colour Denoise configurations */
    cdnConfig = ISPQT_GetCDNConfig();
    ret = ISP_ConfigureColourDenoise(ISP_CFG_PASS_HV, cdnConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureColourDenoise failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Demosaic configurations */
    dmConfig = ISPQT_GetDMConfig();
    ret = ISP_ConfigureDemosaic(ISP_CFG_PASS_HV, dmConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureDemosaic failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Sharpen configurations */
    shConfig = ISPQT_GetSHConfig();
    ret = ISP_ConfigureSharpen(ISP_CFG_PASS_HV, shConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureSharpen failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Colour Correction configurations */
    ycConfig = ISPQT_GetYCConfig();
    ret = ISP_ConfigureColourCorrection(ISP_CFG_PASS_HV, ycConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureColourCorrection failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Gamma configurations */
    gmConfig = ISPQT_GetGMConfig();
    ret = ISP_ConfigureGamma(ISP_CFG_PASS_HV, gmConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureGamma failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override RGB to YUV configurations */
    ygConfig = ISPQT_GetYGConfig();
    ret = ISP_ConfigureYGCoeffs(ISP_CFG_PASS_HV, ygConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureYGCoeffs failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override False Colour configurations */
    fcConfig = ISPQT_GetFCConfig();
    ret = ISP_ConfigureFalseColour(ISP_CFG_PASS_HV, fcConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureFalseColour failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Local ToneMap configurations */
    ltmConfig = ISPQT_GetLTMConfig();
    ret = ISP_ConfigureLocalToneMap(ISP_CFG_PASS_HV, ltmConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureLocalToneMap failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Chrominance Stretch configurations */
    cpConfig = ISPQT_GetCPConfig();
    ret = ISP_ConfigureChrominanceStretch(ISP_CFG_PASS_HV, cpConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureChrominanceStretch failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override Tone Mapping configurations */
    tmConfig = ISPQT_GetTMConfig();
    ret = ISP_ConfigureToneMap(ISP_CFG_PASS_HV, tmConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureToneMap failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override CC configurations */
    ccConfig = ISPQT_GetCCConfig();
    ret = ISP_ConfigureColourConversion(ISP_CFG_PASS_HV, ccConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureColourConversion failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override HO configurations */
    hoConfig = ISPQT_GetHOConfig();
    ret = ISP_ConfigureHiresOutput(ISP_CFG_PASS_HV, hoConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureHiresOutput failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override LR configurations */
    lrConfig = ISPQT_GetLRConfig();
    ret = ISP_ConfigureLoresResize(ISP_CFG_PASS_HV, lrConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureLoresResize failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override LO configurations */
    loConfig = ISPQT_GetLOConfig();
    ret = ISP_ConfigureLoresOutput(ISP_CFG_PASS_HV, loConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureLoresOutput failed (%d)\n", ret);
        goto func_exit;
    }

    /* Override FR configurations */
    frConfig = ISPQT_GetFRConfig();
    ret = ISP_ConfigureFrame(ISP_CFG_PASS_HV, frConfig);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_ConfigureFrame failed (%d)\n", ret);
        goto func_exit;
    }
#endif /* ISPQT_DISABLE_BLOCK_CONFIGURATION */

    ret = ISP_Start();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_Start failed (%d)\n", ret);
        goto func_exit;
    }

    /* Wait for EOT */
    while(1) {
        uint32_t flags;

        BCM_WaitEvent(ISP_EVENT);
        BCM_GetEvent(ISPQT_Task, &mask);
        BCM_ClearEvent(mask);
        flags = ISP_GetEvent();
        if (0UL != (flags & ISP_INT_EOT_HV)) {
            break;
        }
    }

    /* Get statistics */
    ret = ISP_GetStatistics(&stats, NULL, NULL);
    if (BCM_ERR_OK != ret) {
        goto func_exit;
    }
    BCM_MemCpy(&ISPQT_Stats, stats, sizeof(ISPQT_Stats));

func_exit:
    ret = ISP_Stop();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_Stop failed (%d)\n",ret);
    }

    ret = ISP_Deinit();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("ISP_Deinit failed (%d)\n",ret);
    }

    for (i = 0UL; i < ISP_IMAGE_PLANE_IDX_MAX; i++) {
        crc[i] = ISPQT_CalculateCRC(&ISPQT_OutBuf[i][0], ISPQT_PLANE_SIZE);
        if (ISPQT_CRC[i] != crc[i]) {
            notMatched = 1UL;
        }
    }

    /* Check CRC for statistics */
    statsCRC = ISPQT_CalculateCRC((uint8_t *)&ISPQT_Stats, sizeof(ISPQT_Stats));
    if (ISPQT_StatsCRC != statsCRC) {
        notMatched = 1UL;
    }

    if (0UL == notMatched) {
        ULOG_INFO("Test Passed!\n");
        if (BCM_AT_EXECUTING == ISPQT_qt1_result) {
            ISPQT_qt1_result = BCM_ERR_OK;
        }
    } else {
        ULOG_INFO("Test Failed!\n");
        ULOG_ERR("yCRC = 0x%x \n", crc[0]);
        ULOG_ERR("uCRC = 0x%x \n", crc[1]);
        ULOG_ERR("vCRC = 0x%x \n", crc[2]);
        ULOG_ERR("statsCRC = 0x%x \n", statsCRC);
        if (BCM_AT_EXECUTING == ISPQT_qt1_result) {
            ISPQT_qt1_result = BCM_ERR_UNKNOWN;
        }
    }

    BCM_TerminateTask();
}

/* BCM_ExecuteAT */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ISPQT_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(ISPQT_Task);
        ret = BCM_ERR_OK;
    }
    return ret;
}

/* BCM_GetResultAT */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = ISPQT_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
}

/** @} */
