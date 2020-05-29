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
    @addtogroup grp_isp_rt
    @{

    @file isp_qt.c
    @brief ISP Qualification Test Configs
    This source file contains configurations for ISP blocks.
    @version 0.1 Initial Version
*/

#include <string.h>
#include <isp.h>

static ISP_RawInputConfigType ISPQT_IIConfig;
static ISP_BlackLevelConfigType ISPQT_BLConfig;
static ISP_LensShadingConfigType ISPQT_LSConfig;
static ISP_HDRConfigType ISPQT_HDRConfig;
static ISP_BayerDenoiseConfigType ISPQT_DNConfig;
static ISP_StatisticsConfigType ISPQT_STConfig;
static ISP_WBConfigType ISPQT_WBConfig;
static ISP_GainConfigType ISPQT_GainConfig;
static ISP_DefectivePixelConfigType ISPQT_DPConfig;
static ISP_FringingConfigType ISPQT_FGConfig;
static ISP_CrossTalkConfigType ISPQT_XCConfig;
static ISP_ColourDenoiseConfigType ISPQT_CDNConfig;
static ISP_DemosaicConfigType ISPQT_DMConfig;
static ISP_SharpenConfigType ISPQT_SHConfig;
static ISP_ColourCorrectionConfigType ISPQT_YCConfig;
static ISP_GammaConfigType ISPQT_GMConfig;
static ISP_RGB2YUVConversionConfigType ISPQT_YGConfig;
static ISP_FalseColourConfigType ISPQT_FCConfig;
static ISP_LTMConfigType ISPQT_LTMConfig;
static ISP_ChrominanceStretchConfigType ISPQT_CPConfig;
static ISP_ToneMapConfigType ISPQT_TMConfig;
static ISP_ConversionConfigType ISPQT_CCConfig;
static ISP_OutputConfigType ISPQT_OutputConfig;
static ISP_LoresResizeConfigType ISPQT_LRConfig;
static ISP_FrameConfigType ISPQT_FRConfig;

extern ISP_RawInputConfigType * ISPQT_GetIIConfig(void)
{
    memset(&ISPQT_IIConfig, 0, sizeof(ISPQT_IIConfig));

    ISPQT_IIConfig.format = ISP_RAW_INPUT_FORMAT_RAW16;
    ISPQT_IIConfig.dpcmMode = ISP_RAW_INPUT_DPCM_MODE_NONE;
    ISPQT_IIConfig.stride = 2560U;
    /* Make the ISP aware of RAW16 */
    ISPQT_IIConfig.dpcmBlocklen = 2U;

    return &ISPQT_IIConfig;
}

static uint16_t ISPQT_blAbscissae[16] = {0U, 64U, 65535U, 65535U,
                                         65535U, 65535U, 65535U, 65535U,
                                         65535U, 65535U, 65535U, 65535U,
                                         65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_blOrdinates[16] = {0U, 0U, 65535U, 65535U,
                                       65535U, 65535U, 65535U, 65535U,
                                       65535U, 65535U, 65535U, 65535U,
                                       65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_blSlope[16] = {0U, 4100U, 0U, 0U,
                                     0U, 0U, 0U, 0U,
                                     0U, 0U, 0U, 0U,
                                     0U, 0U, 0U, 0U};
static uint16_t ISPQT_bl1Abscissae[16] = {0U, 4U, 4095U, 65535U,
                                         65535U, 65535U, 65535U, 65535U,
                                         65535U, 65535U, 65535U, 65535U,
                                         65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_bl1Ordinates[16] = {0U, 0U, 65535U, 65535U,
                                       65535U, 65535U, 65535U, 65535U,
                                       65535U, 65535U, 65535U, 65535U,
                                       65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_bl1Slope[16] = {0U, 4100U, 0U, 0U,
                                     0U, 0U, 0U, 0U,
                                     0U, 0U, 0U, 0U,
                                     0U, 0U, 0U, 0U};
extern ISP_BlackLevelConfigType * ISPQT_GetBLConfig(void)
{
    memset(&ISPQT_BLConfig, 0, sizeof(ISPQT_BLConfig));

    ISPQT_BLConfig.top = 0U;
    ISPQT_BLConfig.left = 0U;
    ISPQT_BLConfig.bottom = 720U;
    ISPQT_BLConfig.right = 1280U;
    ISPQT_BLConfig.darkFrameMultiplier = 0U;

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].abscissaeR[0],
           &ISPQT_blAbscissae[0],
           sizeof(ISPQT_blAbscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].ordinatesR[0],
           &ISPQT_blOrdinates[0],
           sizeof(ISPQT_blOrdinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].slopeR[0],
           &ISPQT_blSlope[0],
           sizeof(ISPQT_blSlope));

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].abscissaeGR[0],
           &ISPQT_blAbscissae[0],
           sizeof(ISPQT_blAbscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].ordinatesGR[0],
           &ISPQT_blOrdinates[0],
           sizeof(ISPQT_blOrdinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].slopeGR[0],
           &ISPQT_blSlope[0],
           sizeof(ISPQT_blSlope));

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].abscissaeB[0],
           &ISPQT_blAbscissae[0],
           sizeof(ISPQT_blAbscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].ordinatesB[0],
           &ISPQT_blOrdinates[0],
           sizeof(ISPQT_blOrdinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].slopeB[0],
           &ISPQT_blSlope[0],
           sizeof(ISPQT_blSlope));

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].abscissaeGB[0],
           &ISPQT_blAbscissae[0],
           sizeof(ISPQT_blAbscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].ordinatesGB[0],
           &ISPQT_blOrdinates[0],
           sizeof(ISPQT_blOrdinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].slopeGB[0],
           &ISPQT_blSlope[0],
           sizeof(ISPQT_blSlope));
    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].shiftR = 1U;
    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].shiftGR = 1U;
    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].shiftB = 1U;
    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].shiftGB = 1U;

    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].lvlR = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].lvlGR = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].lvlB = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].lvlGB = 0U;

    ISPQT_BLConfig.in[ISP_IN_IDX_LONG_EXP].threshold = 0U;

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].abscissaeR[0],
           &ISPQT_bl1Abscissae[0],
           sizeof(ISPQT_bl1Abscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].ordinatesR[0],
           &ISPQT_bl1Ordinates[0],
           sizeof(ISPQT_bl1Ordinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].slopeR[0],
           &ISPQT_bl1Slope[0],
           sizeof(ISPQT_bl1Slope));

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].abscissaeGR[0],
           &ISPQT_bl1Abscissae[0],
           sizeof(ISPQT_bl1Abscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].ordinatesGR[0],
           &ISPQT_bl1Ordinates[0],
           sizeof(ISPQT_bl1Ordinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].slopeGR[0],
           &ISPQT_bl1Slope[0],
           sizeof(ISPQT_bl1Slope));

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].abscissaeB[0],
           &ISPQT_bl1Abscissae[0],
           sizeof(ISPQT_bl1Abscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].ordinatesB[0],
           &ISPQT_bl1Ordinates[0],
           sizeof(ISPQT_bl1Ordinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].slopeB[0],
           &ISPQT_bl1Slope[0],
           sizeof(ISPQT_bl1Slope));

    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].abscissaeGB[0],
           &ISPQT_bl1Abscissae[0],
           sizeof(ISPQT_bl1Abscissae));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].ordinatesGB[0],
           &ISPQT_bl1Ordinates[0],
           sizeof(ISPQT_bl1Ordinates));
    memcpy(&ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].slopeGB[0],
           &ISPQT_bl1Slope[0],
           sizeof(ISPQT_bl1Slope));

    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].shiftR = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].shiftGR = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].shiftB = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].shiftGB = 0U;

    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].lvlR = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].lvlGR = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].lvlB = 0U;
    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].lvlGB = 0U;

    ISPQT_BLConfig.in[ISP_IN_IDX_SHORT_EXP].threshold = 0U;

    return &ISPQT_BLConfig;
}

static uint16_t ISPQT_cvR[] =
{
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
};

static uint16_t ISPQT_cvGR[] =
{
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
};

static uint16_t ISPQT_cvB[] =
{
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
};

static uint16_t ISPQT_cvGB[] =
{
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
};

extern ISP_LensShadingConfigType * ISPQT_GetLSConfig(void)
{
    uint32_t i;

    memset(&ISPQT_LSConfig, 0, sizeof(ISPQT_LSConfig));
    ISPQT_LSConfig.cellSzPowerX = 7U;
    ISPQT_LSConfig.cellSzPowerY = 7U;
    ISPQT_LSConfig.pitch = 15U;
    ISPQT_LSConfig.xOffset = 0U;
    ISPQT_LSConfig.yOffset = 0U;
    uint32_t vPitch = 9UL;

    for(i=0UL; i < (ISPQT_LSConfig.pitch * vPitch); i++) {
        ISPQT_LSConfig.cvR[i] = ISPQT_cvR[i] >> 4U;
        ISPQT_LSConfig.cvGR[i] = ISPQT_cvGR[i] >> 4U;
        ISPQT_LSConfig.cvB[i] = ISPQT_cvB[i] >> 4U;
        ISPQT_LSConfig.cvGB[i] = ISPQT_cvGB[i] >> 4U;
    }

    return &ISPQT_LSConfig;
}

static uint32_t ISPQT_HDRAbscissae[16] = {0UL,       16384UL,   65535UL,   524288UL,
                                          1048575UL, 4194303UL, 4194303UL, 4194303UL,
                                          4194303UL, 4194303UL, 4194303UL, 4194303UL,
                                          4194303UL, 4194303UL, 4194303UL, 4194303UL};
static uint16_t ISPQT_HDROrdinates[16] = {0U,       16384U,   40960U,   55296U,
                                          63487U, 65535U, 65535U, 65535U,
                                          65535U, 65535U, 65535U, 65535U,
                                          65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_HDRSlope[16] = {4096U, 2048U, 127U, 63U,
                                     2U, 0U, 0U, 0U,
                                     0U, 0U, 0U, 0U,
                                     0U, 0U, 0U, 0U};

extern ISP_HDRConfigType * ISPQT_GetHDRConfig(void)
{
    memset(&ISPQT_HDRConfig, 0, sizeof(ISPQT_HDRConfig));

    ISPQT_HDRConfig.expRatio = (uint5p11_t) (16.0f *  256.0f);
    ISPQT_HDRConfig.thrHi = 65000U;
    ISPQT_HDRConfig.thrLo = 62500U;
    ISPQT_HDRConfig.wtSC = 26844U;
    ISPQT_HDRConfig.swapExp = 0U;
    ISPQT_HDRConfig.pwlShift = 1U;
    ISPQT_HDRConfig.wbGainR = (uint8p8_t) (1.0f * 256.0f);
    ISPQT_HDRConfig.wbGainB = (uint8p8_t) (1.0f * 256.0f);
    ISPQT_HDRConfig.wbThreshold = 1048575U;

    memcpy(&ISPQT_HDRConfig.abscissae[0],
           &ISPQT_HDRAbscissae[0],
           sizeof(ISPQT_HDRAbscissae));
    memcpy(&ISPQT_HDRConfig.ordinates[0],
           &ISPQT_HDROrdinates[0],
           sizeof(ISPQT_HDROrdinates));
    memcpy(&ISPQT_HDRConfig.slope[0],
           &ISPQT_HDRSlope[0],
           sizeof(ISPQT_HDRSlope));

    return &ISPQT_HDRConfig;
}

static uint16_t ISPQT_DNAbscissae[12] = { 0U,     1500U,  3015U,  4000U,
                                          4800U,  7360U,  12000U, 14950U,
                                          26750U, 29450U, 35150U, 65535U};
static uint16_t ISPQT_DNOrdinates[12] = { 0U,    45U,   2000U, 3000U,
                                          3500U, 4000U, 5000U, 5000U,
                                          2000U, 2000U, 2000U, 2000U};
static sint4p12_t ISPQT_DNSlope[12] = {122, 5285, 4158, 2560,
                                       800, 882, 0, 64495,
                                       0, 0, 0, 0};

static uint8_t ISPQT_DNmask[49] = {
    1U, 0U, 0U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 1U, 0U, 0U, 0U, 0U, 0U, 1U
};

extern ISP_BayerDenoiseConfigType * ISPQT_GetDNConfig(void)
{
    uint32_t i;
    uint64_t mask = 0ULL;

    for (i = 0U; i < 49U; i++) {
        uint64_t bit = (uint64_t)ISPQT_DNmask[i];
        mask |= (bit << i);
    }

    memset(&ISPQT_DNConfig, 0, sizeof(ISPQT_DNConfig));

    memcpy(&ISPQT_DNConfig.abscissae[0],
           &ISPQT_DNAbscissae[0],
           sizeof(ISPQT_DNAbscissae));
    memcpy(&ISPQT_DNConfig.ordinates[0],
           &ISPQT_DNOrdinates[0],
           sizeof(ISPQT_DNOrdinates));
    memcpy(&ISPQT_DNConfig.slope[0],
           &ISPQT_DNSlope[0],
           sizeof(ISPQT_DNSlope));
    ISPQT_DNConfig.grGain = (uint4p4_t) ((2.625f * 16.0f) + 0.5f);
    ISPQT_DNConfig.gbGain = (uint4p4_t) ((2.625f * 16.0f) + 0.5f);
    ISPQT_DNConfig.rGain = (uint4p4_t) ((2.625f * 16.0f) + 0.5f);
    ISPQT_DNConfig.bGain = (uint4p4_t) ((2.625f * 16.0f) + 0.5f);
    ISPQT_DNConfig.grStr = (uint0p8_t) ((0.539f * 256.0f) + 0.5f);
    ISPQT_DNConfig.gbStr = (uint0p8_t) ((0.539f * 256.0f) + 0.5f);
    ISPQT_DNConfig.rStr = (uint0p8_t) ((0.539f * 256.0f) + 0.5f);
    ISPQT_DNConfig.bStr = (uint0p8_t) ((0.539f * 256.0f) + 0.5f);
    ISPQT_DNConfig.mask = mask;

    return &ISPQT_DNConfig;
}

/* Group0 params */
static uint32_t ISPQT_STGROUP0xOffset = 0UL;
static uint32_t ISPQT_STGROUP0yOffset = 0UL;
static uint32_t ISPQT_STGROUP0width = 80UL;
static uint32_t ISPQT_STGROUP0height = 90UL;
static uint32_t ISPQT_STGROUP0hNum = 16UL;

/* Region params */
static uint16_t ISPQT_STREGIONxOffset[32] = {0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U, 320U, 640U, 960U,
                                             0U, 320U, 640U, 960U,
                                             0U, 320U, 640U, 960U,
                                             0U, 320U, 640U, 960U};
static uint16_t ISPQT_STREGIONyOffset[32] = {0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             180U, 180U, 180U, 180U,
                                             360U, 360U, 360U, 360U,
                                             540U, 540U, 540U, 540U};
static uint16_t ISPQT_STREGIONwidth[32]   = {0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             320U, 320U, 320U, 320U,
                                             320U, 320U, 320U, 320U,
                                             320U, 320U, 320U, 320U,
                                             320U, 320U, 320U, 320U};
static uint16_t ISPQT_STREGIONheight[32]  = {0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             0U,   0U,   0U,   0U,
                                             180U, 180U, 180U, 180U,
                                             180U, 180U, 180U, 180U,
                                             180U, 180U, 180U, 180U,
                                             180U, 180U, 180U, 180U};
/* Group params */
static uint16_t ISPQT_GROUPrLo[3] = {64U,    0U, 0U    };
static uint16_t ISPQT_GROUPrHi[3] = {32768U, 0U, 65535U};
static uint16_t ISPQT_GROUPgLo[3] = {64U,    0U, 0U    };
static uint16_t ISPQT_GROUPgHi[3] = {32768U, 0U, 65535U};
static uint16_t ISPQT_GROUPbLo[3] = {64U,    0U, 0U    };
static uint16_t ISPQT_GROUPbHi[3] = {32768U, 0U, 65535U};
static int16_t ISPQT_GROUPrgLo[3] = {-32768, 0,  -32768};
static int16_t ISPQT_GROUPrgHi[3] = {32767,  0,  32767 };
static int16_t ISPQT_GROUPbgLo[3] = {-32768, 0,  -32768};
static int16_t ISPQT_GROUPbgHi[3] = {32767,  0,  32767 };

extern ISP_StatisticsConfigType * ISPQT_GetStatisticsConfig(void)
{
    uint32_t i;

    memset(&ISPQT_STConfig, 0, sizeof(ISPQT_STConfig));

    ISPQT_STConfig.hMask0 = 0x00ffU;
    ISPQT_STConfig.hMask1 = 0xff00U;
    ISPQT_STConfig.pixelShift = 2U;
    ISPQT_STConfig.shift = 9U;
    ISPQT_STConfig.rowFactor = 0U;
    ISPQT_STConfig.columns = 0U;

    /* Fill Group0 configurations */
    for (i = 0UL; i < 18UL; i++) {
        ISPQT_STConfig.group0.x[i] = ISPQT_STGROUP0xOffset + (ISPQT_STGROUP0width * i);
        if (1280U == ISPQT_STConfig.group0.x[i]) {
            break;
        }
    }

    for (i = 0UL; i < 18UL; i++) {
        ISPQT_STConfig.group0.y[i] = ISPQT_STGROUP0yOffset + (ISPQT_STGROUP0height * i);
        if (720U == ISPQT_STConfig.group0.y[i]) {
            break;
        }
    }
    ISPQT_STConfig.group0.pitch = ISPQT_STGROUP0hNum;

    /* Fill region configurations */
    memcpy(&ISPQT_STConfig.region.xOffset[0], &ISPQT_STREGIONxOffset[0], sizeof(ISPQT_STREGIONxOffset));
    memcpy(&ISPQT_STConfig.region.yOffset[0], &ISPQT_STREGIONyOffset[0], sizeof(ISPQT_STREGIONyOffset));
    memcpy(&ISPQT_STConfig.region.width[0], &ISPQT_STREGIONwidth[0], sizeof(ISPQT_STREGIONwidth));
    memcpy(&ISPQT_STConfig.region.height[0], &ISPQT_STREGIONheight[0], sizeof(ISPQT_STREGIONheight));

    /* Fill group configurations */
    for (i = 0UL; i < 3UL; i++) {
        ISPQT_STConfig.group.rLo[i] = ISPQT_GROUPrLo[i];
        ISPQT_STConfig.group.rHi[i] = ISPQT_GROUPrHi[i];
        ISPQT_STConfig.group.gLo[i] = ISPQT_GROUPgLo[i];
        ISPQT_STConfig.group.gHi[i] = ISPQT_GROUPgHi[i];
        ISPQT_STConfig.group.bLo[i] = ISPQT_GROUPbLo[i];
        ISPQT_STConfig.group.bHi[i] = ISPQT_GROUPbHi[i];
        ISPQT_STConfig.group.rgLo[i] = ISPQT_GROUPrgLo[i];
        ISPQT_STConfig.group.rgHi[i] = ISPQT_GROUPrgHi[i];
        ISPQT_STConfig.group.bgLo[i] = ISPQT_GROUPbgLo[i];
        ISPQT_STConfig.group.bgHi[i] = ISPQT_GROUPbgHi[i];
    }

    return &ISPQT_STConfig;
}

extern ISP_WBConfigType * ISPQT_GetWBConfig(void)
{
    memset(&ISPQT_WBConfig, 0, sizeof(ISPQT_WBConfig));

    ISPQT_WBConfig.gainR = 256U;
    ISPQT_WBConfig.gainB = 256U;

    return &ISPQT_WBConfig;
}

extern ISP_GainConfigType * ISPQT_GetGainConfig(void)
{
    memset(&ISPQT_GainConfig, 0, sizeof(ISPQT_GainConfig));

    ISPQT_GainConfig.gain = 256U;
    ISPQT_GainConfig.offsetR = 0U;
    ISPQT_GainConfig.offsetG = 0U;
    ISPQT_GainConfig.offsetB = 0U;
    ISPQT_GainConfig.threshold = 65535U;

    return &ISPQT_GainConfig;
}

static sint4p12_t ISPQT_DPscaleHi[3] = {16, 16, 16};
static int32_t ISPQT_DPoffsetHi[3] = {5000L, 5000L, 5000L};
static sint4p12_t ISPQT_DPscaleLo[3] = {16, 16, 16};
static int32_t ISPQT_DPoffsetLo[3] = {5000L, 5000L, 5000L};

extern ISP_DefectivePixelConfigType * ISPQT_GetDPConfig(void)
{
    memset(&ISPQT_DPConfig, 0, sizeof(ISPQT_DPConfig));

    memcpy(&ISPQT_DPConfig.scaleHi[0],
           ISPQT_DPscaleHi, sizeof(ISPQT_DPConfig.scaleHi));
    memcpy(&ISPQT_DPConfig.offsetHi[0],
            ISPQT_DPoffsetHi, sizeof(ISPQT_DPConfig.offsetHi));
    memcpy(&ISPQT_DPConfig.scaleLo[0],
            ISPQT_DPscaleLo, sizeof(ISPQT_DPConfig.scaleLo));
    memcpy(&ISPQT_DPConfig.offsetLo[0],
            ISPQT_DPoffsetLo, sizeof(ISPQT_DPConfig.offsetLo));
    ISPQT_DPConfig.replaceIdxLo = 1U;
    ISPQT_DPConfig.replaceIdxHi = 9U;
    ISPQT_DPConfig.replaceScaleLo = 16U;
    ISPQT_DPConfig.replaceScaleHi = 16U;

    return &ISPQT_DPConfig;
}

extern ISP_FringingConfigType * ISPQT_GetFGConfig(void)
{
    memset(&ISPQT_FGConfig, 0, sizeof(ISPQT_FGConfig));

    ISPQT_FGConfig.rPos = (sint4p12_t) (0.20f * 4096.0f + 0.5f);
    ISPQT_FGConfig.rNeg = (sint4p12_t) (0.16f * 4096.0f + 0.5f);
    ISPQT_FGConfig.grPos = (sint4p12_t) (-0.02f * 4096.0f - 0.5f);
    ISPQT_FGConfig.grNeg = (sint4p12_t) (0.0f * 4096.0f + 0.5f);
    ISPQT_FGConfig.bPos = (sint4p12_t) (0.12f * 4096.0f + 0.5f);
    ISPQT_FGConfig.bNeg = (sint4p12_t) (0.08f * 4096.0f + 0.5f);
    ISPQT_FGConfig.gbPos = (sint4p12_t) (-0.02f * 4096.0f - 0.5f);
    ISPQT_FGConfig.gbNeg = (sint4p12_t) (0.0f * 4096.0f + 0.5f);
    ISPQT_FGConfig.rPosLim = 4000U;
    ISPQT_FGConfig.rNegLim = 4000U;
    ISPQT_FGConfig.grPosLim = 3000U;
    ISPQT_FGConfig.grNegLim = 2000U;
    ISPQT_FGConfig.bPosLim = 4000U;
    ISPQT_FGConfig.bNegLim = 4000U;
    ISPQT_FGConfig.gbPosLim = 3000U;
    ISPQT_FGConfig.gbNegLim = 2000U;

    return &ISPQT_FGConfig;
}

static uint16_t ISPQT_XCAbscissae[8] = { 0U,     1024U,  8192U,  65535U,
                                         65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_XCOrdinates[8] = { 200U, 600U, 900U, 900U,
                                         900U, 900U, 900U, 900U};
static uint4p12_t ISPQT_XCSlope[8] = {1728U, 299U, 128U, 128U,
                                      128U, 128U, 128U, 0U};

extern ISP_CrossTalkConfigType * ISPQT_GetXCConfig(void)
{
    memset(&ISPQT_XCConfig, 0, sizeof(ISPQT_XCConfig));

    memcpy(&ISPQT_XCConfig.abscissaeLim[0],
           &ISPQT_XCAbscissae[0], sizeof(ISPQT_XCConfig.abscissaeLim));
    memcpy(&ISPQT_XCConfig.ordinatesLim[0],
           &ISPQT_XCOrdinates[0], sizeof(ISPQT_XCConfig.ordinatesLim));
    memcpy(&ISPQT_XCConfig.slopeLim[0],
           &ISPQT_XCSlope[0], sizeof(ISPQT_XCConfig.slopeLim));

    return &ISPQT_XCConfig;
}

static uint16_t ISPQT_CDNFirBGAbscissae[6] = { 0U,     60U,    108U,
                                               65535U, 65535U, 65535U};
static uint16_t ISPQT_CDNFirBGOrdinates[6] = { 32U, 32U, 0U,
                                               0U,   0U, 0U};
static sint8p8_t ISPQT_CDNFirBGSlope[6] = {0, -170, 0,
                                           0, 0,    0};

static uint16_t ISPQT_CDNFirRGAbscissae[6] = { 0U,     60U,    108U,
                                               65535U, 65535U, 65535U};
static uint16_t ISPQT_CDNFirRGOrdinates[6] = { 32U, 32U, 0U,
                                               0U,   0U, 0U};
static sint8p8_t ISPQT_CDNFirRGSlope[6] = {0, -170, 0,
                                           0, 0,    0};

static uint16_t ISPQT_CDNiirBGAbscissae[6] = { 0U,     40U,    72U,
                                               65535U, 65535U, 65535U};
static uint16_t ISPQT_CDNiirBGOrdinates[6] = { 128U, 128U, 0U,
                                               0U,   0U, 0U};
static sint8p8_t ISPQT_CDNiirBGSlope[6] = {0, 0xFC00, 0,
                                           0, 0,    0};

static uint16_t ISPQT_CDNiirRGAbscissae[6] = { 0U,     40U,    72U,
                                               65535U, 65535U, 65535U};
static uint16_t ISPQT_CDNiirRGOrdinates[6] = { 128U, 128U, 0U,
                                               0U,   0U, 0U};
static sint8p8_t ISPQT_CDNiirRGSlope[6] = {0, 0xFC00, 0,
                                           0, 0,    0};

uint8_t ISPQT_CDNweights[4] = {6U, 5U, 5U, 0U};

static uint16_t ISPQT_CDNdistAbscissae[6] = { 0U,     1512U,    2016U,
                                              3528U, 65535U, 65535U};
static uint16_t ISPQT_CDNdistOrdinates[6] = { 127U, 127U, 95U,
                                              15U,  15U, 15U};
static sint4p12_t ISPQT_CDNdistSlope[6] = {0, 0xFEFC, 0xFF28,
                                           0, 0,      0};

static uint16_t ISPQT_CDNgAbscissae[6] = { 0U,     300U,   65535U,
                                           65535U, 65535U, 65535U};
static uint16_t ISPQT_CDNgOrdinates[6] = { 48U, 16U, 16U,
                                           16U, 16U, 16U};
static sint4p12_t ISPQT_CDNgSlope[6] = {0xFE4C, 0, 0,
                                        0,      0, 0};

static uint16_t ISPQT_CDNsatAbscissae[6] = { 0U,     200U,   400U,
                                             65535U, 65535U, 65535U};
static uint16_t ISPQT_CDNsatOrdinates[6] = { 32U, 32U, 64U,
                                             64U, 64U, 64U};
static sint4p12_t ISPQT_CDNsatSlope[6] = {0, 0x028F, 0,
                                          0, 0,      0};

extern ISP_ColourDenoiseConfigType * ISPQT_GetCDNConfig(void)
{
    memset(&ISPQT_CDNConfig, 0, sizeof(ISPQT_CDNConfig));

    memcpy(&ISPQT_CDNConfig.firBGAbscissae[0],
           &ISPQT_CDNFirBGAbscissae[0], sizeof(ISPQT_CDNConfig.firBGAbscissae));
    memcpy(&ISPQT_CDNConfig.firBGOrdinates[0],
           &ISPQT_CDNFirBGOrdinates[0], sizeof(ISPQT_CDNConfig.firBGOrdinates));
    memcpy(&ISPQT_CDNConfig.firBGSlopes[0],
           &ISPQT_CDNFirBGSlope[0], sizeof(ISPQT_CDNConfig.firBGSlopes));

    memcpy(&ISPQT_CDNConfig.firRGAbscissae[0],
           &ISPQT_CDNFirRGAbscissae[0], sizeof(ISPQT_CDNConfig.firRGAbscissae));
    memcpy(&ISPQT_CDNConfig.firRGOrdinates[0],
           &ISPQT_CDNFirRGOrdinates[0], sizeof(ISPQT_CDNConfig.firRGOrdinates));
    memcpy(&ISPQT_CDNConfig.firRGSlopes[0],
           &ISPQT_CDNFirRGSlope[0], sizeof(ISPQT_CDNConfig.firRGSlopes));

    memcpy(&ISPQT_CDNConfig.iirBGAbscissae[0],
           &ISPQT_CDNiirBGAbscissae[0], sizeof(ISPQT_CDNConfig.iirBGAbscissae));
    memcpy(&ISPQT_CDNConfig.iirBGOrdinates[0],
           &ISPQT_CDNiirBGOrdinates[0], sizeof(ISPQT_CDNConfig.iirBGOrdinates));
    memcpy(&ISPQT_CDNConfig.iirBGSlopes[0],
           &ISPQT_CDNiirBGSlope[0], sizeof(ISPQT_CDNConfig.iirBGSlopes));

    memcpy(&ISPQT_CDNConfig.iirRGAbscissae[0],
           &ISPQT_CDNiirRGAbscissae[0], sizeof(ISPQT_CDNConfig.iirRGAbscissae));
    memcpy(&ISPQT_CDNConfig.iirRGOrdinates[0],
           &ISPQT_CDNiirRGOrdinates[0], sizeof(ISPQT_CDNConfig.iirRGOrdinates));
    memcpy(&ISPQT_CDNConfig.iirRGSlopes[0],
           &ISPQT_CDNiirRGSlope[0], sizeof(ISPQT_CDNConfig.iirRGSlopes));

    memcpy(&ISPQT_CDNConfig.weights[0],
           &ISPQT_CDNweights[0], sizeof(ISPQT_CDNConfig.weights));

    memcpy(&ISPQT_CDNConfig.distAbscissae[0],
           &ISPQT_CDNdistAbscissae[0], sizeof(ISPQT_CDNConfig.distAbscissae));
    memcpy(&ISPQT_CDNConfig.distOrdinates[0],
           &ISPQT_CDNdistOrdinates[0], sizeof(ISPQT_CDNConfig.distOrdinates));
    memcpy(&ISPQT_CDNConfig.distSlopes[0],
           &ISPQT_CDNdistSlope[0], sizeof(ISPQT_CDNConfig.distSlopes));

    memcpy(&ISPQT_CDNConfig.gAbscissae[0],
           &ISPQT_CDNgAbscissae[0], sizeof(ISPQT_CDNConfig.gAbscissae));
    memcpy(&ISPQT_CDNConfig.gOrdinates[0],
           &ISPQT_CDNgOrdinates[0], sizeof(ISPQT_CDNConfig.gOrdinates));
    memcpy(&ISPQT_CDNConfig.gSlopes[0],
           &ISPQT_CDNgSlope[0], sizeof(ISPQT_CDNConfig.gSlopes));

    memcpy(&ISPQT_CDNConfig.satAbscissae[0],
           &ISPQT_CDNsatAbscissae[0], sizeof(ISPQT_CDNConfig.satAbscissae));
    memcpy(&ISPQT_CDNConfig.satOrdinates[0],
           &ISPQT_CDNsatOrdinates[0], sizeof(ISPQT_CDNConfig.satOrdinates));
    memcpy(&ISPQT_CDNConfig.satSlopes[0],
           &ISPQT_CDNsatSlope[0], sizeof(ISPQT_CDNConfig.satSlopes));

    ISPQT_CDNConfig.xCentre = 0U;
    ISPQT_CDNConfig.yCentre = 0U;

    ISPQT_CDNConfig.iirAttenuationLines = 6U;
    ISPQT_CDNConfig.iirAttenuationShift = 2U;

    ISPQT_CDNConfig.firShift = 1U;

    ISPQT_CDNConfig.edgeThreshold = 5U;
    ISPQT_CDNConfig.edgeStrength = (uint8p8_t) (4.0 * 256.0 + 0.5);
    ISPQT_CDNConfig.edgeMin = (uint8p8_t) (0.15 * 256.0 + 0.5);

    return &ISPQT_CDNConfig;
}

extern ISP_DemosaicConfigType * ISPQT_GetDMConfig(void)
{
    memset(&ISPQT_DMConfig, 0, sizeof(ISPQT_DMConfig));

    ISPQT_DMConfig.bayerType = ISP_BAYER_TYPE_TRUE;
    ISPQT_DMConfig.thresholdN = 20U;
    ISPQT_DMConfig.hvBias = 0U;
    ISPQT_DMConfig.gOffsetShift = 0U;

    return &ISPQT_DMConfig;
}

static uint16_t ISPQT_SHposGainAbsc[8] = {0U,    750U,  1500U,  2000U,
                                          2500U, 3000U, 65535U, 65535U};
static float ISPQT_SHposGainOrd[8] = {1.000, 1.000, 0.916, 0.818,
                                      0.659, 0.464, 0.464, 0.464};
static sint4p12_t ISPQT_SHposGainSlope[8] = {0,      0xF8AA, 0xF32C, 0xEB23,
                                             0xE677, 0,      0,      0};

static uint16_t ISPQT_SHvGainAbsc[8] = {0U,    1600U,  3000U,  5000U,
                                        7000U, 12000U, 65535U, 65535U};
static float ISPQT_SHvGainOrd[8] = {0.240, 0.240, 0.240, 0.240,
                                    0.200, 0.160, 0.160, 0.160};
static sint4p12_t ISPQT_SHvGainSlope[8] = {0, 0, 0, 0xFEB1,
                                           0xFF7A, 0, 0, 0};

static int16_t ISPQT_SHrespAbsc[12] = { -32768, -300,  -200,  -100,
                                        -64,  -10,  10, 64,
                                        100, 200, 300, 32767};
static int16_t ISPQT_SHrespOrd[12] = { -600, -600, -536, -382,
                                       -213, 0,    0,    162,
                                       275,  466,  550,  550};
static uint8p8_t ISPQT_SHrespSlope[12] = {0U,      0x00A3U, 0x018AU, 0x04B1U,
                                          0x03F1U, 0U,      0x0300U, 0x0323U,
                                          0x01E8U, 0x00D7U, 0U,      0U};

static int16_t ISPQT_SHresp1Absc[12] = { -32768, -300,  -100,  -64,
                                         -16,  12,  16, 64,
                                         100, 200, 300, 32767};
static int16_t ISPQT_SHresp1Ord[12] = { -120, -120, -40, -20,
                                        0, 0,    0,    20,
                                        40,  80,  120,  120};
static uint8p8_t ISPQT_SHresp1Slope[12] ={0U,      0x0066U, 0x008EU, 0x006AU,
                                          0U,      0U,      0x006AU, 0x008EU,
                                          0x0066U, 0x0066U, 0U,      0U};

static uint16_t ISPQT_SHrespGainAbsc[16] = {0U, 1126U, 1438U, 2124U,
                                            2498U, 4032U, 4734U, 6768U,
                                            7470U, 9910U, 10612U, 11450U,
                                            12896U, 13850U, 14956U, 65535U};
static uint16_t ISPQT_SHrespGainOrd[16] = {50U, 96U, 147U, 146U,
                                           192U, 245U, 326U, 370U,
                                           431U, 477U, 549U, 548U,
                                           664U, 661U, 420U, 420U};
static sint4p12_t ISPQT_SHrespGainSlope[16] = {0x00A7, 0x029D, 0xFFFB, 0x01F7,
                                               0x008D, 0x01D8, 0x0058, 0x0163,
                                               0x004D, 0x01A4, 0xFFFC, 0x0148,
                                               0xFFF4, 0xFC84, 0,      0};

extern ISP_SharpenConfigType * ISPQT_GetSHConfig(void)
{
    uint32_t i;

    memset(&ISPQT_SHConfig, 0, sizeof(ISPQT_SHConfig));

    ISPQT_SHConfig.xCentre = 640U;
    ISPQT_SHConfig.yCentre = 360U;

    memcpy(&ISPQT_SHConfig.posGainAbsc[0],
           &ISPQT_SHposGainAbsc[0], sizeof(ISPQT_SHConfig.posGainAbsc));
    for (i = 0UL; i < 8UL; i++) {
        ISPQT_SHConfig.posGainOrd[i] = (uint4p12_t) (ISPQT_SHposGainOrd[i] *
                                                     4096.0 + 0.5);
    }
    memcpy(&ISPQT_SHConfig.posGainSlope[0],
           &ISPQT_SHposGainSlope[0], sizeof(ISPQT_SHConfig.posGainSlope));

    memcpy(&ISPQT_SHConfig.vGainAbsc[0],
           &ISPQT_SHvGainAbsc[0], sizeof(ISPQT_SHConfig.vGainAbsc));
    for (i = 0UL; i < 8UL; i++) {
        ISPQT_SHConfig.vGainOrd[i] = (uint4p12_t) (ISPQT_SHvGainOrd[i] *
                                                     4096.0 + 0.5);
    }
    memcpy(&ISPQT_SHConfig.vGainSlope[0],
           &ISPQT_SHvGainSlope[0], sizeof(ISPQT_SHConfig.vGainSlope));

    memcpy(&ISPQT_SHConfig.respAbsc[0],
           &ISPQT_SHrespAbsc[0], sizeof(ISPQT_SHConfig.respAbsc));
    memcpy(&ISPQT_SHConfig.respOrd[0],
           &ISPQT_SHrespOrd[0], sizeof(ISPQT_SHConfig.respOrd));
    memcpy(&ISPQT_SHConfig.respSlope[0],
           &ISPQT_SHrespSlope[0], sizeof(ISPQT_SHConfig.respSlope));

    memcpy(&ISPQT_SHConfig.resp1Absc[0],
           &ISPQT_SHresp1Absc[0], sizeof(ISPQT_SHConfig.resp1Absc));
    memcpy(&ISPQT_SHConfig.resp1Ord[0],
           &ISPQT_SHresp1Ord[0], sizeof(ISPQT_SHConfig.resp1Ord));
    memcpy(&ISPQT_SHConfig.resp1Slope[0],
           &ISPQT_SHresp1Slope[0], sizeof(ISPQT_SHConfig.resp1Slope));

    memcpy(&ISPQT_SHConfig.respGainAbsc[0],
           &ISPQT_SHrespGainAbsc[0], sizeof(ISPQT_SHConfig.respGainAbsc));
    memcpy(&ISPQT_SHConfig.respGainOrd[0],
           &ISPQT_SHrespGainOrd[0], sizeof(ISPQT_SHConfig.respGainOrd));
    memcpy(&ISPQT_SHConfig.respGainSlope[0],
           &ISPQT_SHrespGainSlope[0], sizeof(ISPQT_SHConfig.respGainSlope));

    ISPQT_SHConfig.isotropicStrength = (uint8p8_t) (0.500 * 256.0 + 0.5);
    ISPQT_SHConfig.avgG_shift = 0U;

    return &ISPQT_SHConfig;
}

float ISPQT_YCMatrix[3][3] = {
    {1.4873f,  -0.4097f, -0.0776f},
    {-0.1875f, 1.5227f,  -0.3352f},
    {-0.0393f, -0.4031f, 1.4421f}
    };

int32_t ISPQT_YCOffsets[3] = {0L, 0L, 0L};

extern ISP_ColourCorrectionConfigType * ISPQT_GetYCConfig(void)
{
    uint32_t i, j;

    memset(&ISPQT_YCConfig, 0, sizeof(ISPQT_YCConfig));

    for (i = 0UL; i < 3UL; i++) {
        for (j = 0UL; j < 3UL; j++) {
            ISPQT_YCConfig.matrix[i][j] = (int16_t) ((ISPQT_YCMatrix[i][j] * 4096.0f +
                                          ((0.0f < ISPQT_YCMatrix[i][j]) ? 0.5f : -0.5f)));
        }
    }
    memcpy(&ISPQT_YCConfig.offsets[0], &ISPQT_YCOffsets[0], sizeof(ISPQT_YCOffsets));

    return &ISPQT_YCConfig;
}

static uint16_t ISPQT_GMabscissae[32] =   {0U,     128U,   256U,   512U,
                                            1024U,  1536U,  2048U,  4096U,
                                            8192U,  16384U, 32768U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_GMordinates[32] =   {0U,     13561U, 18586U, 25467U,
                                            34900U, 41964U, 45013U, 52934U,
                                            57510U, 61687U, 64031U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U};
static int32_t ISPQT_GMslope[32] = {0x69F2L, 0x2742L, 0x1AE1L, 0x126CL,
                                     0xDCCL,  0x5F4L,  0x3DEL,  0x11EL,
                                     0x82L,   0x24L,   0xBL,    0L,
                                     0L,      0L,      0L,      0L,
                                     0L,      0L,      0L,      0L,
                                     0L,      0L,      0L,      0L,
                                     0L,      0L,      0L,      0L,
                                     0L,      0L,      0L,      0L};

extern ISP_GammaConfigType * ISPQT_GetGMConfig(void)
{
    memset(&ISPQT_GMConfig, 0, sizeof(ISPQT_GMConfig));

    memcpy(&ISPQT_GMConfig.abscissaeR[0],
           &ISPQT_GMabscissae[0], sizeof(ISPQT_GMConfig.abscissaeR));
    memcpy(&ISPQT_GMConfig.ordinatesR[0],
           &ISPQT_GMordinates[0], sizeof(ISPQT_GMConfig.ordinatesR));
    memcpy(&ISPQT_GMConfig.slopeR[0],
           &ISPQT_GMslope[0], sizeof(ISPQT_GMConfig.slopeR));

    memcpy(&ISPQT_GMConfig.abscissaeG[0],
           &ISPQT_GMabscissae[0], sizeof(ISPQT_GMConfig.abscissaeG));
    memcpy(&ISPQT_GMConfig.ordinatesG[0],
           &ISPQT_GMordinates[0], sizeof(ISPQT_GMConfig.ordinatesG));
    memcpy(&ISPQT_GMConfig.slopeG[0],
           &ISPQT_GMslope[0], sizeof(ISPQT_GMConfig.slopeG));

    memcpy(&ISPQT_GMConfig.abscissaeB[0],
           &ISPQT_GMabscissae[0], sizeof(ISPQT_GMConfig.abscissaeB));
    memcpy(&ISPQT_GMConfig.ordinatesB[0],
           &ISPQT_GMordinates[0], sizeof(ISPQT_GMConfig.ordinatesB));
    memcpy(&ISPQT_GMConfig.slopeB[0],
           &ISPQT_GMslope[0], sizeof(ISPQT_GMConfig.slopeB));

    ISPQT_GMConfig.shift = 0U;

    return &ISPQT_GMConfig;
}

float ISPQT_YGMatrix[3][3] = {
    {0.2566f,  0.5064f,  0.0978},
    {-0.1482f, -0.2914f, 0.4396},
    {0.4389f,  -0.3677f, -0.0713}
    };

int32_t ISPQT_YGOffsets[3] = {0L, 0L, 0L};

extern ISP_RGB2YUVConversionConfigType * ISPQT_GetYGConfig(void)
{
    uint32_t i, j;

    memset(&ISPQT_YGConfig, 0, sizeof(ISPQT_YGConfig));

    for (i = 0UL; i < 3UL; i++) {
        for (j = 0UL; j < 3UL; j++) {
            ISPQT_YGConfig.matrix[i][j] = (int16_t) ((ISPQT_YGMatrix[i][j] * 1024.0f +
                                          ((0.0f < ISPQT_YGMatrix[i][j]) ? 0.5f : -0.5f)));
        }
    }
    memcpy(&ISPQT_YGConfig.offsets[0], &ISPQT_YGOffsets[0], sizeof(ISPQT_YGOffsets));

    return &ISPQT_YGConfig;
}

extern ISP_FalseColourConfigType * ISPQT_GetFCConfig(void)
{
    const uint32_t normOffset = 0x40UL;

    memset(&ISPQT_FCConfig, 0, sizeof(ISPQT_FCConfig));

    ISPQT_FCConfig.strength1 = (uint4p4_t) (0.0 * 16.0 + 0.5);
    ISPQT_FCConfig.strength2 = (uint4p4_t) (1.0 * 16.0 + 0.5);
    ISPQT_FCConfig.filterMixCw = (uint8p8_t) (1.0 * 8.0 + 0.5);
    ISPQT_FCConfig.filterMixLo = 2000U;
    ISPQT_FCConfig.filterMixHi = 3000U;
    ISPQT_FCConfig.filterDesatCw = (uint8p8_t) (8.0 * 8.0 + 0.5);
    ISPQT_FCConfig.filterDesatLo = 65535U;
    ISPQT_FCConfig.filterDesatHi = 65535U;
    ISPQT_FCConfig.filterMixAbs = (ISPQT_FCConfig.filterMixCw >= 0x2000U) ? 1U : 0U;
    ISPQT_FCConfig.filterDesatAbs = (ISPQT_FCConfig.filterDesatCw >= 0x2000U) ? 1U : 0U;

    ISPQT_FCConfig.filterMixNorm  = (uint16_t)(0x8000U /
                              (ISPQT_FCConfig.filterMixCw + normOffset));
    if (ISPQT_FCConfig.filterMixHi > ISPQT_FCConfig.filterMixLo) {
        ISPQT_FCConfig.filterMixNorm2 = (256U << 12U) /
                      (ISPQT_FCConfig.filterMixHi - ISPQT_FCConfig.filterMixLo);
    } else if (ISPQT_FCConfig.filterMixHi < ISPQT_FCConfig.filterMixLo) {
      ISPQT_FCConfig.filterMixNorm = 0;
    }

    ISPQT_FCConfig.filterDesatNorm  = (uint16_t)(0x8000U /
                           (ISPQT_FCConfig.filterDesatCw + normOffset));

    if (ISPQT_FCConfig.filterDesatHi > ISPQT_FCConfig.filterDesatLo) {
        ISPQT_FCConfig.filterDesatNorm2 = (256U << 12U) /
                    (ISPQT_FCConfig.filterDesatHi-ISPQT_FCConfig.filterDesatLo);
    } else if (ISPQT_FCConfig.filterDesatHi < ISPQT_FCConfig.filterDesatLo) {
        ISPQT_FCConfig.filterDesatNorm = 0;
    }

    return &ISPQT_FCConfig;
}

int32_t ISPQT_LTMgkerCoeff[81] =  {0, 0, 2, 8, 14, 8, 2, 0, 0,
                            0, 5, 63, 281, 463, 281, 63, 5, 0,
                            2, 63, 764, 3425, 5646, 3425, 764, 63, 2,
                            8, 281, 3425, 15349, 25306, 15349, 3425, 281, 8,
                            14, 463, 5646, 25306, 41722, 25306, 5646, 463, 14,
                            8, 281, 3425, 15349, 25306, 15349, 3425, 281, 8,
                            2, 63, 764, 3425, 5646, 3425, 764, 63, 2,
                            0, 5, 63, 281, 463, 281, 63, 5, 0,
                            0, 0, 2, 8, 14, 8, 2, 0, 0};

static uint16_t ISPQT_LTMbgmAbscissae[32] =   {0U,     128U,   256U,   512U,
                                               1024U,  1536U,  2048U,  4096U,
                                               8192U,  16384U, 32768U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_LTMbgmOrdinates[32] =   {0U,     1656U,  3312U,  6616U,
                                               13232U, 19848U, 26464U, 52928U,
                                               57504U, 61680U, 64024U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_LTMbgmSlope[32] =   {0x0CF0U, 0x0CF0U, 0x0CE8U, 0x0CECU,
                                           0x0CECU, 0x0CECU, 0x0CECU, 0x011EU,
                                           0x0082U, 0x0024U, 0x000BU, 0U,
                                           0U,      0U,      0U,      0U,
                                           0U,      0U,      0U,      0U,
                                           0U,      0U,      0U,      0U,
                                           0U,      0U,      0U,      0U,
                                           0U,      0U,      0U,      0U};

static uint16_t ISPQT_LTMdgmG_Abscissae[32] = {0U,     128U,   256U,   512U,
                                               1024U,  1536U,  2048U,  4096U,
                                               8192U,  16384U, 32768U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U,
                                               65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_LTMdgmG_Ordinates[32] = {1656U,  1656U,  3304U,  6616U,
                                               6616U,  6616U,  6616U,  4576U,
                                               4176U,  2344U,  1512U,  1512U,
                                               1512U,  1512U,  1512U,  1512U,
                                               1512U,  1512U,  1512U,  1512U,
                                               1512U,  1512U,  1512U,  1512U,
                                               1512U,  1512U,  1512U,  1512U,
                                               1512U,  1512U,  1512U,  1512U};
static uint16_t ISPQT_LTMdgmG_Slope[32]  =   {0U,      0x0CE0U, 0x0CF0U, 0U,
                                              0U,      0U,      0xFF01U, 0xFFE7U,
                                              0xFFC7U, 0xFFF3U, 0U,      0U,
                                              0U,      0U,      0U,      0U,
                                              0U,      0U,      0U,      0U,
                                              0U,      0U,      0U,      0U,
                                              0U,      0U,      0U,      0U,
                                              0U,      0U,      0U,      0U};

extern ISP_LTMConfigType * ISPQT_GetLTMConfig(void)
{
    memset(&ISPQT_LTMConfig, 0, sizeof(ISPQT_LTMConfig));

    ISPQT_LTMConfig.khp = (uint8_t) (6.0 * 16.0 + 0.5);
    ISPQT_LTMConfig.kcolor = (uint8_t) (1.0 * 16.0 + 0.5);
    ISPQT_LTMConfig.epsilonVal = 10485U;
    ISPQT_LTMConfig.enGf = 0U;
    ISPQT_LTMConfig.divSat = 1835008UL;
    memcpy(&ISPQT_LTMConfig.gkerCoeff[0],
           &ISPQT_LTMgkerCoeff[0], sizeof(ISPQT_LTMConfig.gkerCoeff));
    ISPQT_LTMConfig.shiftBgm = 0U;
    ISPQT_LTMConfig.shiftDgmG = 0U;

    memcpy(&ISPQT_LTMConfig.bgmAbscissae[0],
           &ISPQT_LTMbgmAbscissae[0], sizeof(ISPQT_LTMConfig.bgmAbscissae));
    memcpy(&ISPQT_LTMConfig.bgmOrdinates[0],
           &ISPQT_LTMbgmOrdinates[0], sizeof(ISPQT_LTMConfig.bgmOrdinates));
    memcpy(&ISPQT_LTMConfig.bgmSlope[0],
           &ISPQT_LTMbgmSlope[0], sizeof(ISPQT_LTMConfig.bgmSlope));

    memcpy(&ISPQT_LTMConfig.dgmG_Abscissae[0],
           &ISPQT_LTMdgmG_Abscissae[0], sizeof(ISPQT_LTMConfig.dgmG_Abscissae));
    memcpy(&ISPQT_LTMConfig.dgmG_Ordinates[0],
           &ISPQT_LTMdgmG_Ordinates[0], sizeof(ISPQT_LTMConfig.dgmG_Ordinates));
    memcpy(&ISPQT_LTMConfig.dgmG_Slope[0],
           &ISPQT_LTMdgmG_Slope[0], sizeof(ISPQT_LTMConfig.dgmG_Slope));

    return &ISPQT_LTMConfig;
}

static int32_t ISPQT_CPuAbscissae[8] = { -32768L, -16384L, 16384L, 32767L,
                                         32767L, 32767L,  32767L, 32767L};
static uint16_t ISPQT_CPuOrdinates[8] = { 0U,     16384U, 49152U, 65535U,
                                          65535U, 65535U, 65535U, 65535U};
static uint4p12_t ISPQT_CPuSlope[8] = {0x1000U, 0x1000U, 0x1000U, 0U,
                                       0U,      0U,      0U,      0U};

static int32_t ISPQT_CPvAbscissae[8] = { -32768L, -16384L, 16384L, 32767L,
                                         32767L, 32767L,  32767L, 32767L};
static uint16_t ISPQT_CPvOrdinates[8] = { 0U,     16384U, 49152U, 65535U,
                                          65535U, 65535U, 65535U, 65535U};
static uint4p12_t ISPQT_CPvSlope[8] = {0x1000U, 0x1000U, 0x1000U, 0U,
                                       0U,      0U,      0U,      0U};

extern ISP_ChrominanceStretchConfigType * ISPQT_GetCPConfig(void)
{
    memset(&ISPQT_CPConfig, 0, sizeof(ISPQT_CPConfig));

    memcpy(&ISPQT_CPConfig.uAbscissae[0],
           &ISPQT_CPuAbscissae[0], sizeof(ISPQT_CPConfig.uAbscissae));
    memcpy(&ISPQT_CPConfig.uOrdinates[0],
           &ISPQT_CPuOrdinates[0], sizeof(ISPQT_CPConfig.uOrdinates));
    memcpy(&ISPQT_CPConfig.uSlope[0],
           &ISPQT_CPuSlope[0], sizeof(ISPQT_CPConfig.uSlope));

    memcpy(&ISPQT_CPConfig.vAbscissae[0],
           &ISPQT_CPvAbscissae[0], sizeof(ISPQT_CPConfig.vAbscissae));
    memcpy(&ISPQT_CPConfig.vOrdinates[0],
           &ISPQT_CPvOrdinates[0], sizeof(ISPQT_CPConfig.vOrdinates));
    memcpy(&ISPQT_CPConfig.vSlope[0],
           &ISPQT_CPvSlope[0], sizeof(ISPQT_CPConfig.vSlope));

    ISPQT_CPConfig.uvShift = 1U;

    return &ISPQT_CPConfig;
}

static uint16_t ISPQT_TMyAbscissae[32] =   {0U,     65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U};
static uint16_t ISPQT_TMyOrdinates[32] =   {0U,     65535U, 65535U,  65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U,
                                            65535U, 65535U, 65535U, 65535U};
static uint4p12_t ISPQT_TMySlope[32] =   {0x1000U, 0U,      0U,      0U,
                                          0U,      0U,      0U,      0U,
                                          0U,      0U,      0U,      0U,
                                          0U,      0U,      0U,      0U,
                                          0U,      0U,      0U,      0U,
                                          0U,      0U,      0U,      0U,
                                          0U,      0U,      0U,      0U,
                                          0U,      0U,      0U,      0U};

extern ISP_ToneMapConfigType * ISPQT_GetTMConfig(void)
{
    memset(&ISPQT_TMConfig, 0, sizeof(ISPQT_TMConfig));

    memcpy(&ISPQT_TMConfig.yAbscissae[0],
           &ISPQT_TMyAbscissae[0], sizeof(ISPQT_TMConfig.yAbscissae));
    memcpy(&ISPQT_TMConfig.yOrdinates[0],
           &ISPQT_TMyOrdinates[0], sizeof(ISPQT_TMConfig.yOrdinates));
    memcpy(&ISPQT_TMConfig.ySlope[0],
           &ISPQT_TMySlope[0], sizeof(ISPQT_TMConfig.ySlope));

    ISPQT_TMConfig.yShift = 1U;

    return &ISPQT_TMConfig;
}

float ISPQT_CCMatrix[3][3] = {
    {1.1619f,  0.0009f, 1.5994f},
    {1.1616f, -0.3899f, -0.8108f},
    {1.1617f,  2.0166f, 0.0018f}
    };

int32_t ISPQT_CCOffsets[3] = {-52441L, 39346L, -66138L};

extern ISP_ConversionConfigType * ISPQT_GetCCConfig(void)
{
    uint32_t i, j;

    memset(&ISPQT_CCConfig, 0, sizeof(ISPQT_CCConfig));
    for (i = 0UL; i < 3UL; i++) {
        for (j = 0UL; j < 3UL; j++) {
            ISPQT_CCConfig.matrix[i][j] = (int16_t) ((ISPQT_CCMatrix[i][j] * 1024.0f +
                                          ((0.0f < ISPQT_CCMatrix[i][j]) ? 0.5f : -0.5f)));
        }
    }
    memcpy(&ISPQT_CCConfig.offsets[0], &ISPQT_CCOffsets[0], sizeof(ISPQT_CCOffsets));

    return &ISPQT_CCConfig;
}

extern ISP_OutputConfigType * ISPQT_GetHOConfig(void)
{
    memset(&ISPQT_OutputConfig, 0, sizeof(ISPQT_OutputConfig));

    ISPQT_OutputConfig.dataFormat = ISP_DATA_FORMAT_420_PLANAR;
    ISPQT_OutputConfig.outputWidth = 0U;
    ISPQT_OutputConfig.vFlip = 0U;
    ISPQT_OutputConfig.shift = 8U;
    ISPQT_OutputConfig.bigEndian = 0U;
    ISPQT_OutputConfig.satDepth = 0U;
    ISPQT_OutputConfig.cbufEn = 1U;
    ISPQT_OutputConfig.stride1 = 1280U;
    ISPQT_OutputConfig.stride2 = 640U;
    ISPQT_OutputConfig.syncLines = 16U;
    ISPQT_OutputConfig.syncThresh = 48U;

    return &ISPQT_OutputConfig;
}

extern ISP_LoresResizeConfigType * ISPQT_GetLRConfig(void)
{
    uint16_t scaleXInt, scaleYInt;

    memset(&ISPQT_LRConfig, 0, sizeof(ISPQT_LRConfig));

    ISPQT_LRConfig.tscaleX = ((1280U << 16U)/640U);
    if ((1280U << 16U)  - (640U * ISPQT_LRConfig.tscaleX) >= ISPQT_LRConfig.tscaleX) {
        ISPQT_LRConfig.tscaleX++;
    }
    ISPQT_LRConfig.tscaleY = ((720U << 16U)/480U);
    if ((720U << 16U) - (480U * ISPQT_LRConfig.tscaleY) >= ISPQT_LRConfig.tscaleY) {
        ISPQT_LRConfig.tscaleY++;
    }
    scaleXInt = (uint16_t)(ISPQT_LRConfig.tscaleX >> 16U);
    scaleYInt = (uint16_t)(ISPQT_LRConfig.tscaleY >> 16U);

    if ((1U == scaleXInt) && (1U == scaleYInt)) {
        ISPQT_LRConfig.tnorm0 = 0xffffU;
    } else {
        ISPQT_LRConfig.tnorm0 = (uint16_t)(0x10000U / (scaleXInt * scaleYInt));
    }
    ISPQT_LRConfig.tnorm1 = (uint16_t)(0x10000U / ((scaleXInt + 1U) * scaleYInt));
    ISPQT_LRConfig.tnorm2 = (uint16_t)(0x10000U / (scaleXInt * (scaleYInt + 1U)));
    ISPQT_LRConfig.tnorm3 = (uint16_t)(0x10000U / ((scaleXInt + 1U) * (scaleYInt + 1U)));
    ISPQT_LRConfig.shift = 0U;
    ISPQT_LRConfig.rbSwap = 0U;

    return &ISPQT_LRConfig;
}

extern ISP_OutputConfigType * ISPQT_GetLOConfig(void)
{
    memset(&ISPQT_OutputConfig, 0, sizeof(ISPQT_OutputConfig));

    ISPQT_OutputConfig.dataFormat = ISP_DATA_FORMAT_420_PLANAR;
    ISPQT_OutputConfig.outputWidth = 0U;
    ISPQT_OutputConfig.vFlip = 0U;
    ISPQT_OutputConfig.shift = 8U;
    ISPQT_OutputConfig.bigEndian = 0U;
    ISPQT_OutputConfig.satDepth = 0U;
    ISPQT_OutputConfig.cbufEn = 1U;
    ISPQT_OutputConfig.stride1 = 640U;
    ISPQT_OutputConfig.stride2 = 320U;
    ISPQT_OutputConfig.syncLines = 16U;
    ISPQT_OutputConfig.syncThresh = 48U;

    return &ISPQT_OutputConfig;
}

static const uint32_t mosaicPatterns[] = {
    0xDD22DD22UL, 0x22DD22DDUL, 0x88778877UL, 0x77887788UL
};

extern ISP_FrameConfigType * ISPQT_GetFRConfig(void)
{
    memset(&ISPQT_FRConfig, 0, sizeof(ISPQT_FRConfig));

    ISPQT_FRConfig.width = 1280U;
    ISPQT_FRConfig.height = 720U;
    ISPQT_FRConfig.bayerOrder = ISP_BAYER_ORDER_BGGR;
    ISPQT_FRConfig.bayerEn = (ISP_BAYER_STAGE_IMAGE_INPUT |
                              ISP_BAYER_STAGE_SECOND_FRAME_INPUT |
                              ISP_BAYER_STAGE_BLACK_LEVEL |
                              ISP_BAYER_STAGE_LENS_SHADING |
                              ISP_BAYER_STAGE_HIGH_DYNAMIC_RANGE |
                              ISP_BAYER_STAGE_DENOISE |
                              ISP_BAYER_STAGE_STATISTICS |
                              ISP_BAYER_STAGE_WHITE_BALANCE_GAIN |
                              ISP_BAYER_STAGE_DEFECTIVE_PIXEL_AUTO |
                              ISP_BAYER_STAGE_FRINGING |
                              ISP_BAYER_STAGE_CROSSTALK |
                              ISP_BAYER_STAGE_COLOUR_DENOISE |
                              ISP_BAYER_STAGE_DEMOSAIC);
    ISPQT_FRConfig.ycbcrEn = (ISP_YCBCR_STAGE_YUV_CONVERSION |
                              ISP_YCBCR_STAGE_SHARPEN |
                              ISP_YCBCR_STAGE_GAMMA |
                              ISP_YCBCR_STAGE_YCBCR_CONVERSION |
                              ISP_YCBCR_STAGE_FALSE_COLOUR |
                              ISP_YCBCR_STAGE_LTM |
                              ISP_YCBCR_STAGE_CHROMINANCE_STRETCH |
                              ISP_YCBCR_STAGE_TONE_MAPPING |
                              ISP_YCBCR_STAGE_CONVERSION |
                              ISP_YCBCR_STAGE_LRESIZE |
                              ISP_YCBCR_STAGE_LO_RES_OUTPUT);

    ISPQT_FRConfig.transposed = 0U;
    ISPQT_FRConfig.mosaic = mosaicPatterns[ISP_BAYER_ORDER_BGGR];

    return &ISPQT_FRConfig;
}
