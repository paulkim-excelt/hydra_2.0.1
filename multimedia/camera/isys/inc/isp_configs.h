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
    @defgroup grp_isp_ifc Interface
    @ingroup grp_isp
    @addtogroup grp_isp_ifc
    @{
    @file isp_configs.h
    @brief ISP Configuration interface
    This file exposes configurable static parameters of ISP blocks.
*/

#ifndef ISP_CONFIGS_H
#define ISP_CONFIGS_H

/**
    @name ISP Block configuration API IDs
    @{
    @brief API IDs for ISP Block configuration
*/
#define BRCM_SWARCH_ISP_IN_IDX_TYPE                     (0x0U)   /**< @brief #ISP_InIdxType                    */
#define BRCM_SWARCH_ISP_RAW_INPUT_FORMAT_TYPE           (0x1U)   /**< @brief #ISP_RawInputFormatType           */
#define BRCM_SWARCH_ISP_RAW_INPUT_DPCM_MODE_TYPE        (0x2U)   /**< @brief #ISP_RawInputDPCMModeType         */
#define BRCM_SWARCH_ISP_RAW_INPUT_CONFIG_TYPE           (0x3U)   /**< @brief #ISP_RawInputConfigType           */
#define BRCM_SWARCH_ISP_BLACK_LEVEL_CHANNEL_CONFIG_TYPE (0x4U)   /**< @brief #ISP_BlackLevelChannelConfigType  */
#define BRCM_SWARCH_ISP_BLACK_LEVEL_CONFIG_TYPE         (0x5U)   /**< @brief #ISP_BlackLevelConfigType         */
#define BRCM_SWARCH_ISP_LS_POINTS_MACRO                 (0x6U)   /**< @brief #ISP_LS_POINTS                    */
#define BRCM_SWARCH_ISP_LENS_SHADING_CONFIG_TYPE        (0x7U)   /**< @brief #ISP_LensShadingConfigType        */
#define BRCM_SWARCH_ISP_HDR_CONFIG_TYPE                 (0x8U)   /**< @brief #ISP_HDRConfigType                */
#define BRCM_SWARCH_ISP_BAYER_DENOISE_CONFIG_TYPE       (0x9U)   /**< @brief #ISP_BayerDenoiseConfigType       */
#define BRCM_SWARCH_ISP_STATISTICS_GROUP0_CONFIG_TYPE   (0xaU)   /**< @brief #ISP_StatisticsGroup0ConfigType   */
#define BRCM_SWARCH_ISP_STATISTICS_REGION_CONFIG_TYPE   (0xbU)   /**< @brief #ISP_StatisticsRegionConfigType   */
#define BRCM_SWARCH_ISP_STATISTICS_GROUP_CONFIG_TYPE    (0xcU)   /**< @brief #ISP_StatisticsGroupConfigType    */
#define BRCM_SWARCH_ISP_STATISTICS_CONFIG_TYPE          (0xdU)   /**< @brief #ISP_StatisticsConfigType         */
#define BRCM_SWARCH_ISP_WB_CONFIG_TYPE                  (0xeU)   /**< @brief #ISP_WBConfigType                 */
#define BRCM_SWARCH_ISP_GAIN_CONFIG_TYPE                (0xfU)   /**< @brief #ISP_GainConfigType               */
#define BRCM_SWARCH_ISP_DEFPIX_NUM_PTS_MACRO            (0x10U)  /**< @brief #ISP_DEFPIX_NUM_PTS               */
#define BRCM_SWARCH_ISP_DEFECTIVE_PIXEL_CONFIG_TYPE     (0x11U)  /**< @brief #ISP_DefectivePixelConfigType     */
#define BRCM_SWARCH_ISP_FRINGING_CONFIG_TYPE            (0x12U)  /**< @brief #ISP_FringingConfigType           */
#define BRCM_SWARCH_ISP_CROSS_TALK_CONFIG_TYPE          (0x13U)  /**< @brief #ISP_CrossTalkConfigType          */
#define BRCM_SWARCH_ISP_CDN_NUM_PTS_MACRO               (0x14U)  /**< @brief #ISP_CDN_NUM_PTS                  */
#define BRCM_SWARCH_ISP_COLOUR_DENOISE_CONFIG_TYPE      (0x15U)  /**< @brief #ISP_ColourDenoiseConfigType      */
#define BRCM_SWARCH_ISP_BAYER_TYPE                      (0x16U)  /**< @brief #ISP_BayerType                    */
#define BRCM_SWARCH_ISP_DEMOSAIC_CONFIG_TYPE            (0x17U)  /**< @brief #ISP_DemosaicConfigType           */
#define BRCM_SWARCH_ISP_SHARPEN_CONFIG_TYPE             (0x18U)  /**< @brief #ISP_SharpenConfigType            */
#define BRCM_SWARCH_ISP_COLOUR_CORRECTION_CONFIG_TYPE   (0x19U)  /**< @brief #ISP_ColourCorrectionConfigType   */
#define BRCM_SWARCH_ISP_GM_POINTS_MACRO                 (0x1aU)  /**< @brief #ISP_GM_POINTS                    */
#define BRCM_SWARCH_ISP_GAMMA_CONFIG_TYPE               (0x1bU)  /**< @brief #ISP_GammaConfigType              */
#define BRCM_SWARCH_ISP_RGB2YUV_CONVERSION_CONFIG_TYPE  (0x1cU)  /**< @brief #ISP_RGB2YUVConversionConfigType  */
#define BRCM_SWARCH_ISP_FALSE_COLOUR_CONFIG_TYPE        (0x1dU)  /**< @brief #ISP_FalseColourConfigType        */
#define BRCM_SWARCH_ISP_LTM_POINTS_MACRO                (0x1eU)  /**< @brief #ISP_LTM_POINTS                   */
#define BRCM_SWARCH_ISP_LTM_CONFIG_TYPE                 (0x1fU)  /**< @brief #ISP_LTMConfigType                */
#define BRCM_SWARCH_ISP_CHROMINANCE_STRETCH_CONFIG_TYPE (0x20U)  /**< @brief #ISP_ChrominanceStretchConfigType */
#define BRCM_SWARCH_ISP_TM_POINTS_MACRO                 (0x21U)  /**< @brief #ISP_TM_POINTS                    */
#define BRCM_SWARCH_ISP_TONEMAP_CONFIG_TYPE             (0x22U)  /**< @brief #ISP_ToneMapConfigType            */
#define BRCM_SWARCH_ISP_CONVERSION_CONFIG_TYPE          (0x23U)  /**< @brief #ISP_ConversionConfigType         */
#define BRCM_SWARCH_ISP_DATA_FORMAT_TYPE                (0x24U)  /**< @brief #ISP_DataFormat                   */
#define BRCM_SWARCH_ISP_OUTPUT_CONFIG_TYPE              (0x25U)  /**< @brief #ISP_OutputConfigType             */
#define BRCM_SWARCH_ISP_LORES_RESIZE_CONFIG_TYPE        (0x26U)  /**< @brief #ISP_LoresResizeConfigType        */
#define BRCM_SWARCH_ISP_BAYER_ORDER_TYPE                (0x27U)  /**< @brief #ISP_BayerOrderType               */
#define BRCM_SWARCH_ISP_BAYER_STAGE_TYPE                (0x28U)  /**< @brief #ISP_BayerStageType               */
#define BRCM_SWARCH_ISP_YCBCR_TYPE                      (0x29U)  /**< @brief #ISP_YCBCRStageType               */
#define BRCM_SWARCH_ISP_FRAME_CONFIG_TYPE               (0x2aU)  /**< @brief #ISP_FrameConfigType              */
/** @} */

/**
    @name ISP_InIdxType
    @{
    @brief ISP input index type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_InIdxType;
#define ISP_IN_IDX_LONG_EXP  (0UL)
#define ISP_IN_IDX_SHORT_EXP (1UL)
#define ISP_IN_IDX_MAX       (2UL)
/** @} */

/**
    @name ISP_RawInputFormatType
    @{
    @brief Raw input formats

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint8_t ISP_RawInputFormatType;
#define ISP_RAW_INPUT_FORMAT_RAW6             (0U)  /**< @brief RAW6 format    */
#define ISP_RAW_INPUT_FORMAT_RAW7             (1U)  /**< @brief RAW7 format    */
#define ISP_RAW_INPUT_FORMAT_RAW8             (2U)  /**< @brief RAW8 format    */
#define ISP_RAW_INPUT_FORMAT_RAW10            (3U)  /**< @brief RAW10 format   */
#define ISP_RAW_INPUT_FORMAT_RAW12            (4U)  /**< @brief RAW12 format   */
#define ISP_RAW_INPUT_FORMAT_RAW14            (5U)  /**< @brief RAW14 format   */
#define ISP_RAW_INPUT_FORMAT_RAW16            (6U)  /**< @brief RAW16 format   */
#define ISP_RAW_INPUT_FORMAT_RAW10_8          (7U)  /**< @brief RAW10_8 format */
#define ISP_RAW_INPUT_FORMAT_RAW12_8          (8U)  /**< @brief RAW12_8 format */
#define ISP_RAW_INPUT_FORMAT_RAW14_8          (9U)  /**< @brief RAW14_8 format */
#define ISP_RAW_INPUT_FORMAT_RAW10L           (11U) /**< @brief RAW10L format  */
#define ISP_RAW_INPUT_FORMAT_RAW12L           (12U) /**< @brief RAW12L format  */
#define ISP_RAW_INPUT_FORMAT_RAW14L           (13U) /**< @brief RAW14L format  */
#define ISP_RAW_INPUT_FORMAT_RAW4             (15U) /**< @brief RAW4 format    */
/** @} */

/**
    @name ISP_RawInputDPCMModeType
    @{
    @brief Raw input DPCM mode

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint8_t ISP_RawInputDPCMModeType;
#define ISP_RAW_INPUT_DPCM_MODE_NONE     (0U)
#define ISP_RAW_INPUT_DPCM_MODE_RAW10_8  (1U)
#define ISP_RAW_INPUT_DPCM_MODE_RAW12_8  (4U)
#define ISP_RAW_INPUT_DPCM_MODE_RAW14_8  (8U)
/** @} */

/**
    @brief Raw Input configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_RawInputConfigType {
    ISP_RawInputFormatType format;       /**< @brief Raw format as #ISP_RawInputFormatType  */
    ISP_RawInputDPCMModeType dpcmMode;   /**< @brief DPCM mode as #ISP_RawInputDPCMModeType */
    uint16_t stride;                     /**< @brief Input stride                           */
    uint16_t dpcmBlocklen;               /**< @brief DPCM block length                      */
} ISP_RawInputConfigType;

/**
    @brief Black Level channel configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_BlackLevelChannelConfigType {
    uint16_t abscissaeR[16];     /**< @brief Abscissae for R PWL         */
    uint16_t ordinatesR[16];     /**< @brief Ordinates for R PWL         */
    uint4p12_t slopeR[16];       /**< @brief Slopes for R PWL            */
    uint16_t abscissaeGR[16];    /**< @brief Abscissae for GR PWL        */
    uint16_t ordinatesGR[16];    /**< @brief Ordinates for GR PWL        */
    uint4p12_t slopeGR[16];      /**< @brief Slopes for GR PWL           */
    uint16_t abscissaeB[16];     /**< @brief Abscissae for B PWL         */
    uint16_t ordinatesB[16];     /**< @brief Ordinates for B PWL         */
    uint4p12_t slopeB[16];       /**< @brief Slopes for B PWL            */
    uint16_t abscissaeGB[16];    /**< @brief Abscissae for GB PWL        */
    uint16_t ordinatesGB[16];    /**< @brief Ordinates for GB PWL        */
    uint4p12_t slopeGB[16];      /**< @brief Slopes for GB PWL           */
    uint8_t shiftR;              /**< @brief Set if slopeR is u4.12      */
    uint8_t shiftGR;             /**< @brief Set if slopeGR is u4.12     */
    uint8_t shiftB;              /**< @brief Set if slopeB is u4.12      */
    uint8_t shiftGB;             /**< @brief Set if slopeGB is u4.12     */
    uint16_t lvlR;               /**< @brief Black level R               */
    uint16_t lvlGR;              /**< @brief Black level GR              */
    uint16_t lvlB;               /**< @brief Black level B               */
    uint16_t lvlGB;              /**< @brief Black level GB              */
    uint16_t threshold;          /**< @brief Black level threshold (u16) */
} ISP_BlackLevelChannelConfigType;

/**
    @brief Black Level configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_BlackLevelConfigType {
    uint8_t  top;                    /**< @brief Black rows in top           */
    uint8_t  left;                   /**< @brief Black rows in left          */
    uint16_t bottom;                 /**< @brief Height of the image in pixels
                                          excluding any bottom edge black
                                                               border        */
    uint16_t right;                  /**< @brief Width of the image in pixels
                                          excluding any right hand edge black
                                                                      border */
    uint8p8_t darkFrameMultiplier;   /**< @brief Multiplier applied to dark
                                                         frame pixels (u8.8) */
    ISP_BlackLevelChannelConfigType in[ISP_IN_IDX_MAX]; /**< @brief Channel
                                        specific parameters                  */

} ISP_BlackLevelConfigType;

/**
    @brief ISP Lens shading gain grid Points

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_LS_POINTS         (1089U)

/**
    @brief Lens Shading configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_LensShadingConfigType {
    uint8_t cellSzPowerX;           /**< @brief Cell size "x" in power of 2           */
    uint8_t cellSzPowerY;           /**< @brief Cell size "y" in power of 2           */
    uint16_t pitch;                 /**< @brief Pitch of the image in grids           */
    uint16_t xOffset;               /**< @brief Offset "x" for the grid               */
    uint16_t yOffset;               /**< @brief Offset "y" for the grid               */
    uint16_t cvR[ISP_LS_POINTS];    /**< @brief LS gain array for "R" channel (4.8p)  */
    uint16_t cvGR[ISP_LS_POINTS];   /**< @brief LS gain array for "GR" channel (4.8p) */
    uint16_t cvB[ISP_LS_POINTS];    /**< @brief LS gain array for "B" channel (4.8p)  */
    uint16_t cvGB[ISP_LS_POINTS];   /**< @brief LS gain array for "GB" channel (4.8p) */
} ISP_LensShadingConfigType;

/**
    @brief HDR configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_HDRConfigType {
    uint5p11_t expRatio;    /**< @brief Program exposure ratio (8p8)                          */
    uint14p2_t thrHi;       /**< @brief High threshold for HDR fusion                         */
    uint14p2_t thrLo;       /**< @brief Low threshold for HDR fusion                          */
    uint32_t wtSC;          /**< @brief Weighting factor for HDR fusion, 0p16 format          */
    uint8_t swapExp;        /**< @brief Swap long and short exposure streams                  */
    uint8_t pwlShift;       /**< @brief 0: Slope precision of 8p8, 1: Slope precision of 4p12 */
    uint8p8_t wbGainR;      /**< @brief White balance red gain on fused output                */
    uint8p8_t wbGainB;      /**< @brief White balance blue gain on fused output               */
    uint32_t wbThreshold;   /**< @brief Saturation threshold, 23p0 format                     */
    uint32_t abscissae[16]; /**< @brief Abscissa of the PWL for fused pixels                  */
    uint16_t ordinates[16]; /**< @brief Ordinate of the PWL for fused pixels.                 */
    uint4p12_t slope[16];   /**< @brief Slope of the PWL for fused pixels.                    */
} ISP_HDRConfigType;

/**
    @brief Bayer Denoise configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_BayerDenoiseConfigType {
    uint16_t abscissae[12]; /**< @brief Threshold function abscissae        */
    uint16_t ordinates[12]; /**< @brief Threshold function ordinates        */
    sint4p12_t slope[12];   /**< @brief Threshold function slope values     */
    uint4p4_t grGain;       /**< @brief GR Gain in u2.4 format              */
    uint4p4_t gbGain;       /**< @brief GB Gain in u2.4 format              */
    uint4p4_t rGain;        /**< @brief Red Gain in u2.4 format             */
    uint4p4_t bGain;        /**< @brief Blue Gain in u2.4 format            */
    uint0p8_t grStr;        /**< @brief Strength of denoising for GR with
                                        corresponding central pixel in u.8
                                        (2 bits are ignored, so u.6) format */
    uint0p8_t gbStr;        /**< @brief Strength of denoising for GB with
                                        corresponding central pixel in u.8
                                        (2 bits are ignored, so u.6) format */
    uint0p8_t rStr;         /**< @brief Strength of denoising for Red with
                                        corresponding central pixel in u.8
                                        (2 bits are ignored, so u.6) format */
    uint0p8_t bStr;         /**< @brief Strength of denoising for Blue with
                                        corresponding central pixel in u.8
                                        (2 bits are ignored, so u.6) format */
    uint64_t mask;          /**< @brief Mask of included pixel clusters     */
} ISP_BayerDenoiseConfigType;

/**
    @brief Statistics Group0 configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_StatisticsGroup0ConfigType {
    uint16_t x[18]; /**< @brief X cordinates for Group0 region */
    uint16_t y[18]; /**< @brief Y cordinates for Group0 region */
    uint32_t pitch; /**< @brief Pitch value used in accesses to
                         internal group 0 memory               */
} ISP_StatisticsGroup0ConfigType;

/**
    @brief Statistics region configuration

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_StatisticsRegionConfigType {
    uint16_t xOffset[32]; /**< @brief X offset of group 1 or 2 region */
    uint16_t yOffset[32]; /**< @brief Y offset of group 1 or 2 region */
    uint16_t width[32];   /**< @brief Width of group 1 or 2 region    */
    uint16_t height[32];  /**< @brief Height of group 1 or 2 region   */
} ISP_StatisticsRegionConfigType;

/**
    @brief Statistics group configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_StatisticsGroupConfigType {
    uint16_t rLo[3]; /**< @brief Low R threshold for inclusion of a pixel for group    */
    uint16_t rHi[3]; /**< @brief High R threshold for inclusion of a pixel for group   */
    uint16_t gLo[3]; /**< @brief Low G threshold for inclusion of a pixel for group    */
    uint16_t gHi[3]; /**< @brief High G threshold for inclusion of a pixel for group   */
    uint16_t bLo[3]; /**< @brief Low B threshold for inclusion of a pixel for group    */
    uint16_t bHi[3]; /**< @brief High B threshold for inclusion of a pixel for group   */
    int16_t rgLo[3]; /**< @brief Low R-G threshold for inclusion of a pixel for group  */
    int16_t rgHi[3]; /**< @brief High R-G threshold for inclusion of a pixel for group */
    int16_t bgLo[3]; /**< @brief Low B-G threshold for inclusion of a pixel for group  */
    int16_t bgHi[3]; /**< @brief High B-G threshold for inclusion of a pixel for group */
} ISP_StatisticsGroupConfigType;

/**
    @brief ISP Statistics configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_StatisticsConfigType {
    uint16_t   hMask0;                     /**< @brief Bitmask indicating which Group 2 regions
                                                   contribute to the first set of R/G/B histograms */
    uint16_t   hMask1;                     /**< @brief Bitmask indicating which Group 2 regions
                                                  contribute to the second set of R/G/B histograms */
    uint8_t    pixelShift;                 /**< @brief Right-shift applied to R, G and B values
                                                                   before gathering all statistics */
    uint8_t    shift;                      /**< @brief Right-shift applied to R, G and B values
                                                                              before histogramming */
    uint16_t   rowFactor;                  /**< @brief Defines the number of rows/columns to group
                                                      for generating row/column sums as 2ROW_NUM+1 */
    uint8_t    columns;                    /**< @brief Set to 1 to accumulate columns, not rows    */
    ISP_StatisticsGroup0ConfigType group0; /**< @brief Group0 configuration                        */
    ISP_StatisticsRegionConfigType region; /**< @brief Region configuration                        */
    ISP_StatisticsGroupConfigType group;   /**< @brief Group configuration                         */
} ISP_StatisticsConfigType;

/**
    @brief ISP WB configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_WBConfigType {
    uint8p8_t gainR; /**< @brief Red pixel gain (u8.8 format)  */
    uint8p8_t gainB; /**< @brief Blue pixel gain (u8.8 format) */
} ISP_WBConfigType;

/**
    @brief ISP Gain configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef struct _ISP_GainConfigType {
    uint8p8_t gain;     /**< @brief Gain applied to all pixels (u8.8 format)   */
    int32_t offsetR;    /**< @brief Red pixel offset (u17 format)              */
    int32_t offsetG;    /**< @brief Green pixel offset (u17 format)            */
    int32_t offsetB;    /**< @brief Blue pixel offset (u17 format)             */
    uint16_t threshold; /**< @brief Threshold applied after colour gains (u16) */
} ISP_GainConfigType;

/**
    @brief ISP Defective Pixel points

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_DEFPIX_NUM_PTS (6UL)

/**
    @brief ISP Defective Pixel configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_DefectivePixelConfigType {
    sint4p12_t scaleHi[3];    /**< @brief Scale factor for correcting high
                                                       outliers (u2.4 format) */
    int32_t offsetHi[3];      /**< @brief offset for correcting high outliers
                                                          (s16 format)        */
    sint4p12_t scaleLo[3];    /**< @brief Scale factor for correcting low
                                                      outliers (u2.4 format)  */
    int32_t offsetLo[3];      /**< @brief Offset for correcting low outliers
                                                         (s16 format)         */
    uint8_t replaceIdxLo;     /**< @brief Rank index of lowest-valued pixel to
                                          be used in the replacement decision */
    uint8_t replaceIdxHi;     /**< @brief Rank index of highest-valued pixel to
                                          be used in the replacement decision */
    uint8p8_t replaceScaleLo; /**< @brief Scale used in interpolation of the
                                   replacement for low-valued outliers (u4.4) */
    uint8p8_t replaceScaleHi; /**< @brief Scale used in interpolation of the
                                  replacement for high-valued outliers (u4.4) */
} ISP_DefectivePixelConfigType;

/**
    @brief ISP Fringing configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_FringingConfigType {
    sint4p12_t rPos;   /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    sint4p12_t rNeg;   /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    sint4p12_t grPos;  /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    sint4p12_t grNeg;  /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    sint4p12_t bPos;   /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    sint4p12_t bNeg;   /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    sint4p12_t gbPos;  /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    sint4p12_t gbNeg;  /**< @brief The strength of sharpening to
                            apply in s2.8 format (top 2 bits and bottom 4 bits
                            ignored, i.e. access as s4.12).                   */
    uint16_t rPosLim;  /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
    uint16_t rNegLim;  /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
    uint16_t grPosLim; /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
    uint16_t grNegLim; /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
    uint16_t bPosLim;  /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
    uint16_t bNegLim;  /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
    uint16_t gbPosLim; /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
    uint16_t gbNegLim; /**< @brief The maximum change in pixel value applied by
                            the fringing block to these pixels                */
} ISP_FringingConfigType;

/**
    @brief ISP Crosstalk configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_CrossTalkConfigType {
    uint16_t abscissaeLim[8]; /**< @brief Abscissa of the PWL limit function  */
    uint16_t ordinatesLim[8]; /**< @brief Ordinate  of the PWL limit function */
    uint4p12_t slopeLim[8];   /**< @brief Slope of the PWL limit function     */
} ISP_CrossTalkConfigType;

/**
    @brief ISP Number of CDN PWL points

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_CDN_NUM_PTS (6UL)

/**
    @brief ISP Color Denoise configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ColourDenoiseConfigType {
    uint16_t firBGAbscissae[ISP_CDN_NUM_PTS];  /**< @brief B-G FIR Filter PWL
                                                                    Abscissae  */
    uint16_t firBGOrdinates[ISP_CDN_NUM_PTS];  /**< @brief B-G FIR Filter PWL
                                                                    Ordinates  */
    sint8p8_t firBGSlopes[ISP_CDN_NUM_PTS];    /**< @brief B-G FIR Filter PWL
                                                                    Slopes     */
    uint16_t firRGAbscissae[ISP_CDN_NUM_PTS];  /**< @brief R-G FIR Filter PWL
                                                                    Abscissae  */
    uint16_t firRGOrdinates[ISP_CDN_NUM_PTS];  /**< @brief R-G FIR Filter PWL
                                                                    Ordinates  */
    sint8p8_t firRGSlopes[ISP_CDN_NUM_PTS];    /**< @brief R-G FIR Filter PWL
                                                                    Slopes     */
    uint16_t iirBGAbscissae[ISP_CDN_NUM_PTS];  /**< @brief B-G IIR Filter PWL
                                                                    Abscissae  */
    uint16_t iirBGOrdinates[ISP_CDN_NUM_PTS];  /**< @brief B-G IIR Filter PWL
                                                                    Ordinates  */
    sint8p8_t iirBGSlopes[ISP_CDN_NUM_PTS];    /**< @brief B-G IIR Filter PWL
                                                                    Slopes     */
    uint16_t iirRGAbscissae[ISP_CDN_NUM_PTS];  /**< @brief R-G IIR Filter PWL
                                                                    Abscissae  */
    uint16_t iirRGOrdinates[ISP_CDN_NUM_PTS];  /**< @brief R-G IIR Filter PWL
                                                                    Ordinates  */
    sint8p8_t iirRGSlopes[ISP_CDN_NUM_PTS];    /**< @brief R-G IIR Filter PWL
                                                                    Slopes     */
    uint8_t weights[4];                        /**< @brief CDN Weights         */
    uint16_t distAbscissae[ISP_CDN_NUM_PTS];   /**< @brief Dist PWL Abscissae  */
    uint16_t distOrdinates[ISP_CDN_NUM_PTS];   /**< @brief Dist PWL Ordinates  */
    sint4p12_t distSlopes[ISP_CDN_NUM_PTS];    /**< @brief Dist PWL Slopes     */
    uint16_t gAbscissae[ISP_CDN_NUM_PTS];      /**< @brief G PWL Abscissae     */
    uint16_t gOrdinates[ISP_CDN_NUM_PTS];      /**< @brief G PWL Ordinates     */
    sint4p12_t gSlopes[ISP_CDN_NUM_PTS];       /**< @brief G PWL Slopes        */
    uint16_t satAbscissae[ISP_CDN_NUM_PTS];    /**< @brief  SAT PWL Abscissae  */
    uint16_t satOrdinates[ISP_CDN_NUM_PTS];    /**< @brief  SAT PWL Ordinates  */
    sint4p12_t satSlopes[ISP_CDN_NUM_PTS];     /**< @brief  SAT PWL Slopes     */
    uint16_t xCentre;                          /**< @brief X Centre            */
    uint16_t yCentre;                          /**< @brief Y Centre            */
    uint8_t iirAttenuationLines;               /**< @brief IIR Attenuate Lines */
    uint8_t iirAttenuationShift;               /**< @brief IIR Attenuate Shift */
    uint8_t firShift;                          /**< @brief FIR Shift           */
    uint16_t edgeThreshold;                    /**< @brief Edge Threshold      */
    uint8p8_t edgeStrength;                    /**< @brief CDN Edge Strength   */
    uint8p8_t edgeMin;                         /**< @brief CDN Edge Min        */
} ISP_ColourDenoiseConfigType;

/**
    @name ISP_BayerType
    @{
    @brief Bayer type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint8_t ISP_BayerType;
#define ISP_BAYER_TYPE_TRUE       (0U) /**< @brief Data is Bayer pattern         */
#define ISP_BAYER_TYPE_H_COSITED  (1U) /**< @brief Data is horizontally co-sited */
#define ISP_BAYER_TYPE_V_COSITED  (2U) /**< @brief Data is vertically co-sited   */
#define ISP_BAYER_TYPE_HV_COSITED (3U) /**< @brief Data is both horizontally and
                                                             vertically co-sited */
/** @} */

/**
    @brief ISP Demosaic configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_DemosaicConfigType {
    ISP_BayerType bayerType; /**< @brief Bayer Type                             */
    uint16_t thresholdN;     /**< @brief Demosaicing Threshold                  */
    uint8p8_t hvBias;        /**< @brief Factor that can be increased to prefer
                                   horizontal or vertical, rather than
                                   diagonal, sharpening when performing co-sited
                                   demosaicing                                  */
    uint8_t gOffsetShift;    /**< @brief G offset shift parameter that can be
                                  increased to cause less soft interpolation of
                                  red/blue channels                             */
} ISP_DemosaicConfigType;

/**
    @brief ISP Sharpening configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_SharpenConfigType {
    uint16_t xCentre;                    /**< @brief Horizontal coordinate of the
                                                            centre of sharpness */
    uint16_t yCentre;                    /**< @brief Vertical coordinate of the
                                                           centre of sharpness  */
    uint16_t posGainAbsc[8];             /**< @brief Abscissa of the gain
                                                                       function */
    uint4p12_t posGainOrd[8];            /**< @brief Ordinate of the gain
                                                       function in u4.12 format */
    sint4p12_t posGainSlope[8];          /**< @brief Slope of the distance gain
                                                         function, s4p12 format */
    uint16_t vGainAbsc[8];               /**< @brief Abscissa of the brightness
                                                                  gain function */
    uint16_t vGainOrd[8];                /**< @brief Ordinate of the brightness
                                              gain function  u2.12 format (top 2
                                              bits and bottom 2 bits ignored)   */
    sint4p12_t vGainSlope[8];            /**< @brief Slope of the brightness
                                                                  gain function */
    int16_t respAbsc[12];                /**< @brief Abscissa of the mapping
                                                           function, s16 format */
    int16_t respOrd[12];                 /**< @brief Ordinate of the mapping
                                                           function, s16 format */
    uint8p8_t respSlope[12];             /**< @brief Slope of the mapping
                                                          function, u8p8 format */
    int16_t resp1Absc[12];               /**< @brief Abscissa of the mapping
                                                           function, s16 format */
    int16_t resp1Ord[12];                /**< @brief Ordinate of the mapping
                                                           function, s16 format */
    uint8p8_t resp1Slope[12];            /**< @brief Slope of the mapping
                                                          function, u8p8 format */
    uint16_t respGainAbsc[16];           /**< @brief Abscissa of the gain
                                                                       function */
    uint16_t respGainOrd[16];            /**< @brief Ordinate of the gain
                                                       function in u4.12 format */
    sint4p12_t respGainSlope[16];        /**< @brief Slope of the distance gain
                                                         function, s4p12 format */
    uint8p8_t isotropicStrength;         /**< @brief Isotropic filter gain, u4p4
                                              format (top and bottom 4 bits
                                                                       ignored) */
    uint8_t avgG_shift;                  /**< @brief Average G shift for
                                              determining sharpening threshold  */
} ISP_SharpenConfigType;

/**
    @brief Colour Correction configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ColourCorrectionConfigType {
    int16_t matrix[3][3];   /**< @brief Conversion Matrix (s4.12 format (bottom
                                                               4 bits ignored)) */
    uint8_t pad[2];
    int32_t offsets[3];     /**< @brief Offsets(s17 format)                     */
} ISP_ColourCorrectionConfigType;

/**
    @brief ISP Number of Gamma PWL points

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_GM_POINTS (32UL)

/**
    @brief Gamma configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_GammaConfigType {
    uint16_t abscissaeR[ISP_GM_POINTS]; /**< @brief Abscissa of the PWL gamma
                                                    function for red pixels     */
    uint16_t ordinatesR[ISP_GM_POINTS]; /**< @brief Ordinates of the PWL gamma
                                                    function for red pixels     */
    int32_t  slopeR[ISP_GM_POINTS];     /**< @brief Slope of the PWL gamma
                                                    function for red pixels     */
    uint16_t abscissaeG[ISP_GM_POINTS]; /**< @brief Abscissa of the PWL gamma
                                                    function for green pixels   */
    uint16_t ordinatesG[ISP_GM_POINTS]; /**< @brief Ordinates of the PWL gamma
                                                    function for green pixels   */
    int32_t  slopeG[ISP_GM_POINTS];     /**< @brief Slope of the PWL gamma
                                                    function for green pixels   */
    uint16_t abscissaeB[ISP_GM_POINTS]; /**< @brief Abscissa of the PWL gamma
                                                    function for blue pixels    */
    uint16_t ordinatesB[ISP_GM_POINTS]; /**< @brief Ordinates of the PWL gamma
                                                    function for blue pixels    */
    int32_t  slopeB[ISP_GM_POINTS];     /**< @brief Slope of the PWL gamma
                                                    function for blue pixels    */
    uint8_t  shift;                     /**< @brief 0: 8p8 precision for slope
                                                    1: 4p12 precision for slope */
} ISP_GammaConfigType;

/**
    @brief RGB to YUV conversion configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_RGB2YUVConversionConfigType {
    int16_t matrix[3][3];   /**< @brief Conversion Matrix (4p10 format) */
    uint8_t pad[2];
    int32_t offsets[3];     /**< @brief Offsets(17p0)                   */
} ISP_RGB2YUVConversionConfigType;

/**
    @brief False colour correction configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_FalseColourConfigType {
    uint4p4_t  strength1;        /**< @brief Strength of mean Cb/Cr filter
                                                              response (u4.4)       */
    uint4p4_t  strength2;        /**< @brief Strength of most neutral Cb/Cr
                                                       filter response (u4.4)       */
    uint8p8_t  filterMixCw;      /**< @brief Weighting given to the central
                                      cluster input to the mixing Filter
                                      response. (u5.3) (Only used if ABSOLUTE is 0) */
    uint16_t   filterMixLo;      /**< @brief Lower limit to the mixing filter
                                                            response. In u16 format */
    uint16_t   filterMixHi;      /**< @brief Upper limit to the mixing filter
                                                            response. In u16 format */
    uint8p8_t  filterDesatCw;    /**< @brief Weighting given to the central cluster
                                      input to the mixing filter response. (u5.3)
                                      (Only used if ABSOLUTE is 0)                  */
    uint16_t   filterDesatLo;    /**< @brief Lower limit to the desaturation
                                                     filter response. In u16 format */
    uint16_t   filterDesatHi;    /**< @brief Upper limit to the desaturation
                                                     filter response. In u16 format */
    /* Derived parameters (not part of config, reqd by hardware) */
    uint8_t    filterMixAbs;     /**< @brief Assign absolute weighting to the
                                                        central cluster input       */
    uint8_t    filterDesatAbs;   /**< @brief Assign absolute weighting to the
                                      central cluster input                         */
    uint16_t   filterMixNorm;    /**< @brief Normalisation value used after
                                       weighting central input cluster (Used only
                                                                   if ABSOLUTE = 0) */
    uint16_t   filterMixNorm2;   /**< @brief Normalisation value used to map the
                                      response range specified in ISPFC_FMIX_THRESH
                                                               to the range [0,256] */
    uint16_t   filterDesatNorm;  /**< @brief Normalisation value used after
                                      weighting central input cluster (Used only
                                                    if ABSOLUTE = 0)                */
    uint16_t   filterDesatNorm2; /**< @brief Normalisation value used to map the
                                      response range specified in
                                      ISPFC_FDESAT_THRESH to the range [0,256]      */
} ISP_FalseColourConfigType;

/**
    @brief Number of LTM PWL points

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_LTM_POINTS (32UL)

/**
    @brief Local Tone Mapping configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_LTMConfigType {
    uint8_t  khp;                            /**< @brief Khp 4p4 format               */
    uint8_t  kcolor;                         /**< @brief Kcolor 4p4 format            */
    uint16_t epsilonVal;                     /**< @brief Epsilon value in 0p16 format */
    uint8_t  enGf;                           /**< @brief Enable GF                    */
    uint32_t divSat;                         /**< @brief Div out saturation value     */
    uint32_t gkerCoeff[81UL];                /**< @brief Gaussian kernel coefficient  */
    uint8_t  shiftBgm;                       /**< @brief 0: 8p8 precision for slope
                                                         1: 4p12 precision for slope  */
    uint8_t  shiftDgmG;                      /**< @brief 0: 8p8 precision for slope
                                                         1: 4p12 precision for slope  */
    uint16_t bgmAbscissae[ISP_LTM_POINTS];   /**< @brief Low pass (base) Gamma PWL
                                                                            Abscissae */
    uint16_t bgmOrdinates[ISP_LTM_POINTS];   /**< @brief Low pass (base) Gamma PWL
                                                                            Ordinates */
    uint16_t bgmSlope[ISP_LTM_POINTS];       /**< @brief Low pass (base) Gamma PWL
                                                                                Slope */
    uint16_t dgmG_Abscissae[ISP_LTM_POINTS]; /**< @brief High pass (detail) Gamma gain
                                                                        PWL Abscissae */
    uint16_t dgmG_Ordinates[ISP_LTM_POINTS]; /**< @brief High pass (detail) Gamma gain
                                                                        PWL Ordinates */
    int16_t dgmG_Slope[ISP_LTM_POINTS];      /**< @brief High pass (detail) Gamma gain
                                                                            PWL Slope */
} ISP_LTMConfigType;

/**
    @brief Chrominance Stretch configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ChrominanceStretchConfigType {
    int32_t    uAbscissae[8]; /**< @brief PWL function for CB values in s16 format */
    uint16_t   uOrdinates[8]; /**< @brief Ordinates for the PWL transfer function
                                                       for CB values in u16 format */
    uint4p12_t uSlope[8];     /**< @brief Slopes for the PWL transfer function for
                                                         CB values in u4.12 format */
    int32_t    vAbscissae[8]; /**< @brief PWL function for CR values in s16 format */
    uint16_t   vOrdinates[8]; /**< @brief Ordinates for the PWL transfer function
                                                       for CR values in u16 format */
    uint4p12_t vSlope[8];     /**< @brief Slopes for the PWL transfer function for
                                                         CR values in u4.12 format */
    uint8_t    uvShift;       /**< @brief 0: 8p8 precision for slope
                                          1: 4p12 precision for slope              */
} ISP_ChrominanceStretchConfigType;

/**
    @brief Number of Tone Mapping PWL points

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_TM_POINTS (32UL)

/**
    @brief Tone Mapping configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ToneMapConfigType {
    uint16_t yAbscissae[ISP_TM_POINTS]; /**< @brief Abscissa for the PWL transfer
                                             function for Y values. In u16 format   */
    uint16_t yOrdinates[ISP_TM_POINTS]; /**< @brief Ordinate for the PWL transfer
                                             function for Y values. In u16 format   */
    uint4p12_t ySlope[ISP_TM_POINTS];   /**< @brief Slope for the PWL transfer
                                             function for Y values. In u4.12 format */
    uint8_t  yShift;                    /**< @brief 0: 8p8 precision for slope
                                                    1: 4p12 precision for slope     */
} ISP_ToneMapConfigType;

/**
    @brief Colour conversion configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ConversionConfigType {
    int16_t matrix[3][3];   /**< @brief Conversion Matrix (s6p10) */
    uint8_t pad[2];
    int32_t offsets[3];     /**< @brief Offsets(s18)              */
} ISP_ConversionConfigType;

/**
    @name ISP_DataFormat
    @{
    @brief ISP data format type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint8_t ISP_DataFormat;                    /**< @brief ISP image format type            */
#define ISP_DATA_FORMAT_INTERLEAVED          (0U)  /**< @brief Interleaved format               */
#define ISP_DATA_FORMAT_RGB565               (1U)  /**< @brief RGB565                           */
#define ISP_DATA_FORMAT_RGBA32_0             (2U)  /**< @brief RGBA32 with "A" filled with 0    */
#define ISP_DATA_FORMAT_RGBA32_255           (3U)  /**< @brief RGBA32 with "A" filled with 0xff */
#define ISP_DATA_FORMAT_444_PLANAR           (4U)  /**< @brief 444 Planar                       */
#define ISP_DATA_FORMAT_422_PLANAR           (5U)  /**< @brief 422 planar                       */
#define ISP_DATA_FORMAT_420_PLANAR           (6U)  /**< @brief 420 Planar                       */
#define ISP_DATA_FORMAT_420_SEMI_PLANAR_UV   (7U)  /**< @brief 420 SP(UV)                       */
#define ISP_DATA_FORMAT_422_INTERLEAVED_YUYV (8U)  /**< @brief 422-I-YUYV                       */
#define ISP_DATA_FORMAT_422_INTERLEAVED_YVYU (9U)  /**< @brief 422-I-YVYU                       */
#define ISP_DATA_FORMAT_422_INTERLEAVED_UYVY (10U) /**< @brief 422-I-UYVY                       */
#define ISP_DATA_FORMAT_422_INTERLEAVED_VYUY (11U) /**< @brief 422-I-VYUY                       */
/** @} */

/**
    @brief Output configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_OutputConfigType {
   ISP_DataFormat dataFormat; /**< @brief ISP data format type             */
   uint8_t outputWidth;       /**< @brief Output width                     */
   uint8_t vFlip;             /**< @brief Enable Vertical Flip             */
   uint8_t shift;             /**< @brief Number of bits to shift          */
   uint8_t bigEndian;         /**< @brief Enable BE                        */
   uint8_t satDepth;          /**< @brief Bit depth for saturation         */
   uint8_t cbufEn;            /**< @brief Enable circular buffer operation */
   int32_t stride1;           /**< @brief Stride for 1st component         */
   int32_t stride2;           /**< @brief Stride for 2nd component         */
   int16_t syncLines;         /**< @brief Number of lines after which ISP
                                                        would signal ready */
   int16_t syncThresh;        /**< @brief Threshold at which ISP would start
                                    stalling the pipeline -
                                    should be a multiple of SYNC_LINES     */
} ISP_OutputConfigType;

/**
    @brief Lores Resize configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_LoresResizeConfigType {
   uint32_t tscaleX;  /**< @brief Number of pixels horizontally that are
                           combined into one output pixel (u6.16 format) */

   uint32_t tscaleY;  /**< @brief Number of pixels vertically that are
                           combined into one output pixel (u6.16 format) */

   uint16_t tnorm0;   /**< @brief Normalisation factor (u0.12 format)    */
   uint16_t tnorm1;   /**< @brief Normalisation factor (u0.12 format)    */
   uint16_t tnorm2;   /**< @brief Normalisation factor (u0.12 format)    */
   uint16_t tnorm3;   /**< @brief Normalisation factor (u0.12 format)    */
   uint8_t shift;     /**< @brief Swap R & B*/
   uint8_t rbSwap;    /**< @brief Pixels are left-shifted by this amount
                           (in the range 0 to 8) and saturated as 16-bit
                           unsigned values                               */
} ISP_LoresResizeConfigType;

/**
    @name ISP_BayerOrderType
    @{
    @brief Bayer Order type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint8_t ISP_BayerOrderType;
#define ISP_BAYER_ORDER_RGGB        (0U) /**< @brief RGGB                          */
#define ISP_BAYER_ORDER_GBRG        (1U) /**< @brief GBRG                          */
#define ISP_BAYER_ORDER_BGGR        (2U) /**< @brief BGGR                          */
#define ISP_BAYER_ORDER_GRBG        (3U) /**< @brief GRBG                          */
#define ISP_BAYER_ORDER_G_FIRST     (1U) /**< @brief mask to tell if G pixel first */
#define ISP_BAYER_ORDER_RB_REVERSED (2U) /**< @brief mask to tell if BGGR or GRBG  */
/** @} */

/**
    @name ISP_BayerStageType
    @{
    @brief Bayer Stage type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_BayerStageType;
#define ISP_BAYER_STAGE_IMAGE_INPUT                (1UL << 0U)  /**< @brief Image input                 */
#define ISP_BAYER_STAGE_SECOND_FRAME_INPUT         (1UL << 1U)  /**< @brief Second frame input          */
#define ISP_BAYER_STAGE_BLACK_LEVEL                (1UL << 2U)  /**< @brief Black Level                 */
#define ISP_BAYER_STAGE_LENS_SHADING               (1UL << 3U)  /**< @brief Lens Shading                */
#define ISP_BAYER_STAGE_STATISTICS                 (1UL << 4U)  /**< @brief Statistics                  */
#define ISP_BAYER_STAGE_WHITE_BALANCE_GAIN         (1UL << 5U)  /**< @brief White Balance and Gain      */
#define ISP_BAYER_STAGE_DEFECTIVE_PIXEL_PROGRAMMED (1UL << 6U)  /**< @brief Defective Pixel Programmed  */
#define ISP_BAYER_STAGE_DEFECTIVE_PIXEL_AUTO       (1UL << 7U)  /**< @brief Defective Pixel Auto        */
#define ISP_BAYER_STAGE_RESAMPLE                   (1UL << 8U)  /**< @brief Resampling                  */
#define ISP_BAYER_STAGE_CROSSTALK                  (1UL << 9U)  /**< @brief Cross-Talk                  */
#define ISP_BAYER_STAGE_DENOISE                    (1UL << 10U) /**< @brief Denoise                     */
#define ISP_BAYER_STAGE_DEMOSAIC                   (1UL << 11U) /**< @brief Demosaic                    */
#define ISP_BAYER_STAGE_FRINGING                   (1UL << 13U) /**< @brief Fringing                    */
#define ISP_BAYER_STAGE_COLOUR_DENOISE             (1UL << 15U) /**< @brief Color Denoise               */
#define ISP_BAYER_STAGE_HIGH_DYNAMIC_RANGE         (1UL << 16U) /**< @brief HDR                         */
/** @} */

/**
    @name ISP_YCBCRStageType
    @{
    @brief YCBCR Stage type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_YCBCRStageType;
#define ISP_YCBCR_STAGE_YUV_INPUT           (1UL << 0U)    /**< @brief YUV Input                 */
#define ISP_YCBCR_STAGE_YUV_CONVERSION      (1UL << 1U)    /**< @brief YUV Conversion            */
#define ISP_YCBCR_STAGE_GAMMA               (1UL << 2U)    /**< @brief Gamma                     */
#define ISP_YCBCR_STAGE_YCBCR_CONVERSION    (1UL << 3U)    /**< @brief YCBCR Coversion           */
#define ISP_YCBCR_STAGE_FALSE_COLOUR        (1UL << 5U)    /**< @brief False Color               */
#define ISP_YCBCR_STAGE_SHARPEN             (1UL << 6U)    /**< @brief Sharpen                   */
#define ISP_YCBCR_STAGE_DISTORTION          (1UL << 7U)    /**< @brief Distortion                */
#define ISP_YCBCR_STAGE_CHROMINANCE_STRETCH (1UL << 8U)    /**< @brief Chrominance Stretch       */
#define ISP_YCBCR_STAGE_CONVERSION          (1UL << 10U)   /**< @brief Conversion                */
#define ISP_YCBCR_STAGE_LRESIZE             (1UL << 11U)   /**< @brief Low-Res Resizer           */
#define ISP_YCBCR_STAGE_HI_RES_OUTPUT       (1UL << 12U)   /**< @brief Hi-Res Output             */
#define ISP_YCBCR_STAGE_LO_RES_OUTPUT       (1UL << 13U)   /**< @brief Lo-Res Output             */
#define ISP_YCBCR_STAGE_TONE_MAPPING        (1UL << 14U)   /**< @brief Tone Mapping              */
#define ISP_YCBCR_STAGE_LTM                 (1UL << 25U)   /**< @brief Local Tone Mapping        */
/** @} */

/**
    @brief Frame configuration structure

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_FrameConfigType {
   uint16_t width;                /**< @brief Width of Image          */
   uint16_t height;               /**< @brief Height of image         */
   ISP_BayerOrderType bayerOrder; /**< @brief Bayer Order             */
   uint32_t bayerEn;              /**< @brief Bayer pipe enables      */
   uint32_t ycbcrEn;              /**< @brief YCbCr pipe enables      */
   uint8_t transposed;            /**< @brief Is the image transposed */
   uint32_t mosaic;               /**< @brief Mosaic pattern          */
} ISP_FrameConfigType;

#endif /* ISP_CONFIGS_H */
/** @} */
