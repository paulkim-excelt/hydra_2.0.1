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

/**
    @addtogroup grp_isp_ifc
    @{
    @file isp_params.h
    @brief ISP Parameter interface
    This file exposes configurable parameters of ISP.
*/

#ifndef ISP_PARAMS_H
#define ISP_PARAMS_H

#include <isp_configs.h>

/**
    @name ISP parameter interface IDs
    @{

    @brief Image interface IDs for ISP
*/
#define BRCM_SWARCH_ISP_PARAM_MACRO                (0x0U)  /**< @brief #ISP_PARAM_WB                */
#define BRCM_SWARCH_ISP_WB_PARAM_TYPE              (0x1U)  /**< @brief #ISP_WBParamType             */
#define BRCM_SWARCH_ISP_GAIN_PARAM_TYPE            (0x2U)  /**< @brief #ISP_GainParamType           */
#define BRCM_SWARCH_ISP_DENOISE_PARAM_TYPE         (0x3U)  /**< @brief #ISP_DenoiseParamType        */
#define BRCM_SWARCH_ISP_COLOUR_DENOISE_PARAM_TYPE  (0x4U)  /**< @brief #ISP_ColourDenoiseParamType  */
#define BRCM_SWARCH_ISP_CONVERSION_PARAM_TYPE      (0x5U)  /**< @brief #ISP_ConversionParamType     */
#define BRCM_SWARCH_ISP_DEFECTIVE_PIXEL_PARAM_TYPE (0x6U)  /**< @brief #ISP_DefectivePixelParamType */
#define BRCM_SWARCH_ISP_SHARPEN_PARAM_TYPE         (0x7U)  /**< @brief #ISP_SharpenParamType        */
#define BRCM_SWARCH_ISP_BLACK_LEVEL_PARAM_TYPE     (0x8U)  /**< @brief #ISP_BlackLevelParamType     */
#define BRCM_SWARCH_ISP_GAMMA_PARAM_TYPE           (0x9U)  /**< @brief #ISP_GammaParamType          */
#define BRCM_SWARCH_ISP_LTM_PARAM_TYPE             (0xaU)  /**< @brief #ISP_LTM_ParamType           */
#define BRCM_SWARCH_ISP_PARAM_TYPE                 (0xbU)  /**< @brief #ISP_ParamType               */
/** @} */

/**
    @name ISP Parameter type
    @{
    @brief ISP Parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_PARAM_WB            (0x49530401UL)
#define ISP_PARAM_GAIN          (0x49530402UL)
#define ISP_PARAM_DENOISE       (0x49530403UL)
#define ISP_PARAM_CDN           (0x49530404UL)
#define ISP_PARAM_YC            (0x49530405UL)
#define ISP_PARAM_DEFPIX        (0x49530406UL)
#define ISP_PARAM_SHARPEN       (0x49530407UL)
#define ISP_PARAM_BL            (0x49530408UL)
#define ISP_PARAM_GAMMA         (0x49530409UL)
#define ISP_PARAM_LTM           (0x4953040aUL)
/** @} */

/**
    @brief ISP White-Balance parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_WBParamType {
    uint8p8_t gainR;
    uint8p8_t gainB;
} ISP_WBParamType;

/**
    @brief ISP Gain parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_GainParamType {
    uint8p8_t gain;
    uint8_t pad1[2];
    uint32_t offsetR;
    uint32_t offsetG;
    uint32_t offsetB;
    uint16_t threshold;
    uint8_t pad2[2];
} ISP_GainParamType;

/**
    @brief ISP Bayer Denoise parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_DenoiseParamType {
    uint16_t abscissae[12];
    uint16_t ordinates[12];
    sint4p12_t slope[12];
    uint4p4_t grGain, gbGain, rGain, bGain;
    uint0p8_t grStr, gbStr, rStr, bStr;
    uint8_t mask[49];
    uint8_t pad[3];
} ISP_DenoiseParamType;

/**
    @brief ISP Colour Denoise parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ColourDenoiseParamType {
    uint16_t firBG_Abscissae[ISP_CDN_NUM_PTS]; /**< @brief B-G FIR Filter PWL Abscissae */
    uint16_t firBG_Ordinates[ISP_CDN_NUM_PTS]; /**< @brief B-G FIR Filter PWL Ordinates */
    sint8p8_t firBG_Slopes[ISP_CDN_NUM_PTS]; /**< @brief B-G FIR Filter PWL Slopes */
    uint16_t firRG_Abscissae[ISP_CDN_NUM_PTS]; /**< @brief R-G FIR Filter PWL Abscissae */
    uint16_t firRG_Ordinates[ISP_CDN_NUM_PTS]; /**< @brief R-G FIR Filter PWL Ordinates */
    sint8p8_t firRG_Slopes[ISP_CDN_NUM_PTS]; /**< @brief R-G FIR Filter PWL Slopes */
    uint16_t iirBG_Abscissae[ISP_CDN_NUM_PTS]; /**< @brief B-G IIR Filter PWL Abscissae */
    uint16_t iirBG_Ordinates[ISP_CDN_NUM_PTS]; /**< @brief B-G IIR Filter PWL Ordinates */
    sint8p8_t iirBG_Slopes[ISP_CDN_NUM_PTS]; /**< @brief B-G IIR Filter PWL Slopes */
    uint16_t iirRG_Abscissae[ISP_CDN_NUM_PTS]; /**< @brief R-G IIR Filter PWL Abscissae */
    uint16_t iirRG_Ordinates[ISP_CDN_NUM_PTS]; /**< @brief R-G IIR Filter PWL Ordinates */
    sint8p8_t iirRG_Slopes[ISP_CDN_NUM_PTS]; /**< @brief R-G IIR Filter PWL Slopes */
    uint8_t weights[4]; /**< @brief CDN Weights */
    uint16_t distAbscissae[ISP_CDN_NUM_PTS]; /**< @brief Dist PWL Abscissae */
    uint16_t distOrdinates[ISP_CDN_NUM_PTS]; /**< @brief Dist PWL Ordinates */
    sint4p12_t distSlopes[ISP_CDN_NUM_PTS]; /**< @brief Dist PWL Slopes */
    uint16_t gAbscissae[ISP_CDN_NUM_PTS]; /**< @brief G PWL Abscissae */
    uint16_t gOrdinates[ISP_CDN_NUM_PTS]; /**< @brief G PWL Ordinates */
    sint4p12_t gSlopes[ISP_CDN_NUM_PTS]; /**< @brief G PWL Slopes */
    uint16_t satAbscissae[ISP_CDN_NUM_PTS]; /**< @brief  SAT PWL Abscissae */
    uint16_t satOrdinates[ISP_CDN_NUM_PTS]; /**< @brief  SAT PWL Ordinates*/
    sint4p12_t satSlopes[ISP_CDN_NUM_PTS]; /**< @brief  SAT PWL Slopes */
    uint16_t xCentre; /**< @brief X Centre */
    uint16_t yCentre; /**< @brief Y Centre */
    uint8_t iirAttenuationLines; /**< @brief IIR Attenuate Lines */
    uint8_t iirAttenuationShift; /**< @brief IIR Attenuate Shift */
    uint8_t firShift; /**< @brief FIR Shift */
    uint8_t _overlap; /**< @brief Not present in hardware */
    uint16_t edgeThreshold; /**< @brief Edge Threshold */
    uint8p8_t edgeStrength; /**< @brief CDN Edge Strength */
    uint8p8_t edgeMin; /**< @brief CDN Edge Min */
    uint8_t pad[2]; /**< @brief Padding */
} ISP_ColourDenoiseParamType;

/**
    @brief ISP Conversion parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ConversionParamType {
    int16_t matrix[3][3];
    uint8_t pad[2];
    int32_t offsets[3];
} ISP_ConversionParamType;

/**
    @brief ISP Defective Pixel parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_DefectivePixelParamType {
    sint4p12_t scaleHi[3];
    int32_t offsetHi[3];
    sint4p12_t scaleLo[3];
    int32_t offsetLo[3];
    uint8_t replaceIdxLo;
    uint8_t replaceIdxHi;
    uint8_t pad2[2];
    uint8p8_t replaceScaleLo;
    uint8p8_t replaceScaleHi;
} ISP_DefectivePixelParamType;

/**
    @brief ISP Sharpening parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_SharpenParamType {
    uint16_t xCentre;
    uint16_t yCentre;
    uint16_t posGainAbsc[8];
    uint4p12_t posGainOrd[8];
    sint4p12_t posGainSlope[8];
    uint16_t vGainAbsc[8];
    uint16_t vGainOrd[8];
    sint4p12_t vGainSlope[8];
    int16_t respAbsc[12], respOrd[12];
    uint8p8_t respSlope[12];
    int16_t resp1Absc[12], resp1Ord[12];
    uint8p8_t resp1Slope[12];
    uint16_t respGainAbsc[16];
    uint16_t respGainOrd[16];
    sint4p12_t respGainSlope[16];
    uint4p4_t isotropicStrength;
    uint8_t avgG_shift;
} ISP_SharpenParamType;

/**
    @brief ISP Black Level parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_BlackLevelParamType {
    struct {
        uint16_t blLvlR;
        uint16_t blLvlGR;
        uint16_t blLvlB;
        uint16_t blLvlGB;
    } in[ISP_IN_IDX_MAX];
} ISP_BlackLevelParamType;

/**
    @brief ISP Gamma parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_GammaParamType {
    uint16_t abscissaeR[ISP_GM_POINTS];
    uint16_t ordinatesR[ISP_GM_POINTS];
    int32_t  slopeR[ISP_GM_POINTS];
    uint16_t abscissaeG[ISP_GM_POINTS];
    uint16_t ordinatesG[ISP_GM_POINTS];
    int32_t  slopeG[ISP_GM_POINTS];
    uint16_t abscissaeB[ISP_GM_POINTS];
    uint16_t ordinatesB[ISP_GM_POINTS];
    int32_t  slopeB[ISP_GM_POINTS];
    uint8_t  shift;
} ISP_GammaParamType;

/**
    @brief ISP Local Tone Mapping parameter type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_LTM_ParamType {
    uint8_t  khp;
    uint8_t  kcolor;
    uint16_t epsilonVal;
    uint8_t  enGf;
    uint32_t divSat;
    uint32_t gkerCoeff[81UL];
    uint8_t  shiftBgm;
    uint8_t  shiftDgmG;
    uint16_t bgmAbscissae[ISP_LTM_POINTS];
    uint16_t bgmOrdinates[ISP_LTM_POINTS];
    uint16_t bgmSlope[ISP_LTM_POINTS];
    uint16_t dgmG_Abscissae[ISP_LTM_POINTS];
    uint16_t dgmG_Ordinates[ISP_LTM_POINTS];
    uint16_t dgmG_Slope[ISP_LTM_POINTS];
} ISP_LTM_ParamType;

/**
    @brief ISP Parameter union type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef union _ISP_ParamType {
    uint8_t *ptr;
    ISP_WBParamType *wbParam;
    ISP_GainParamType *gainParam;
    ISP_DenoiseParamType *denoiseParam;
    ISP_ColourDenoiseParamType *cdnParam;
    ISP_ConversionParamType *convParam;
    ISP_DefectivePixelParamType *defPixParam;
    ISP_SharpenParamType *sharpenParam;
    ISP_BlackLevelParamType *blParam;
    ISP_GammaParamType *gammaParam;
    ISP_LTM_ParamType *ltmParam;
} ISP_ParamType;

#endif /* ISP_PARAMS_H */
/** @} */
