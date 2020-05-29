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
/** @defgroup grp_isp_cfg_ifc Interface
    @ingroup grp_isp

    @addtogroup grp_isp_cfg_ifc
    @{

    @file isp_image_v2.h
    @brief ISP image layout definitions
    @version 0.1
*/

#ifndef ISP_IMAGE_V2_H
#define ISP_IMAGE_V2_H

#include <isys.h>
#include <bcm_err.h>
#include <compiler.h>

/**
    @name ISP image IDs
    @{
    @brief IDs for ISP image.
*/
#define BRCM_SWARCH_ISPIMGV2_BLOCK_ID_MACRO                             (0x81U) /**< @brief #ISPIMGV2_BLOCK_ID_IIFR                         */
#define BRCM_SWARCH_ISPIMGV2_BLKCFG_ID_MACRO                            (0x82U) /**< @brief #ISPIMGV2_BLKCFG_ID_0                           */
#define BRCM_SWARCH_ISPIMGV2_GET_BLOCK_ID_MACRO                         (0x83U) /**< @brief #ISPIMGV2_GET_BLOCK_ID                          */
#define BRCM_SWARCH_ISPIMGV2_GET_BLKCFG_ID_MACRO                        (0x84U) /**< @brief #ISPIMGV2_GET_BLKCFG_ID                         */
#define BRCM_SWARCH_ISPIMGV2_MAX_NUM_SECTIONS_MACRO                     (0x85U) /**< @brief #ISPIMGV2_MAX_NUM_SECTIONS                      */
#define BRCM_SWARCH_ISPIMGV2_SECTION_OFFSET_TABLE_ELEMENT_TYPE          (0x86U) /**< @brief #ISPIMGV2_SectionOffsetTableElementType         */
#define BRCM_SWARCH_ISPIMGV2_SECTION_OFFSET_TABLE_TYPE                  (0x87U) /**< @brief #ISPIMGV2_SectionOffsetTableType                */
#define BRCM_SWARCH_ISPIMGV2_MAX_NUM_PASSES_MACRO                       (0x88U) /**< @brief #ISPIMGV2_MAX_NUM_PASSES                        */
#define BRCM_SWARCH_ISPIMGV2_PASS_OFFSET_TABLE_ELEMENT_TYPE             (0x89U) /**< @brief #ISPIMGV2_PassOffsetTableElementType            */
#define BRCM_SWARCH_ISPIMGV2_PASS_OFFSET_TABLE_TYPE                     (0x8AU) /**< @brief #ISPIMGV2_PassOffsetTableType                   */
#define BRCM_SWARCH_ISPIMGV2_CONFIG_IMAGE_TYPE                          (0x8BU) /**< @brief #ISPIMGV2_ConfigImageType                       */
#define BRCM_SWARCH_ISPIMGV2_IIFR_BLKCFG0_TYPE                          (0x8CU) /**< @brief #ISPIMGV2_IIFR_BlkCfg0Type                      */
#define BRCM_SWARCH_ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_MODE_MACRO           (0x8DU) /**< @brief #ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_MODE            */
#define BRCM_SWARCH_ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_BLOCKLEN_MACRO       (0x8EU) /**< @brief #ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_BLOCKLEN        */
#define BRCM_SWARCH_ISPIMGV2_IIFR_BLKCFG0_GET_FORMAT_MACRO              (0x8FU) /**< @brief #ISPIMGV2_IIFR_BLKCFG0_GET_FORMAT               */
#define BRCM_SWARCH_ISPIMGV2_IIFR_BLKCFG0_GET_BAYER_ORDER_MACRO         (0x90U) /**< @brief #ISPIMGV2_IIFR_BLKCFG0_GET_BAYER_ORDER          */
#define BRCM_SWARCH_ISPIMGV2_IIFR_BLKCFG0_GET_TRANSPOSED_MACRO          (0x91U) /**< @brief #ISPIMGV2_IIFR_BLKCFG0_GET_TRANSPOSED           */
#define BRCM_SWARCH_ISPIMGV2_BL_PWLELEMENT_TYPE                         (0x92U) /**< @brief #ISPIMGV2_BL_PwlElementType                     */
#define BRCM_SWARCH_ISPIMGV2_BL_BLKCFG0_PWL_LEN_MACRO                   (0x93U) /**< @brief #ISPIMGV2_BL_BLKCFG0_PWL_LEN                    */
#define BRCM_SWARCH_ISPIMGV2_BL_BLKCFG0_CHANNEL_CONFIG_TYPE             (0x94U) /**< @brief #ISPIMGV2_BL_BlkCfg0ChannelConfigType           */
#define BRCM_SWARCH_ISPIMGV2_BL_BLKCFG0_TYPE                            (0x95U) /**< @brief #ISPIMGV2_BL_BlkCfg0Type                        */
#define BRCM_SWARCH_ISPIMGV2_BL_BLKCFG0_PWLOFFSETDESC_TYPE              (0x96U) /**< @brief #ISPIMGV2_BL_BlkCfg0PwlOffsetDescType           */
#define BRCM_SWARCH_ISPIMGV2_BL_BLKCFG0_COMPUTEPWL_OFFSETS_PROC         (0x97U) /**< @brief #ISPIMGV2_BL_BlkCfg0ComputePwlOffsets           */
#define BRCM_SWARCH_ISPIMGV2_LS_TABLE_ELEMENT_TYPE                      (0x98U) /**< @brief #ISPIMGV2_LS_TableElementType                   */
#define BRCM_SWARCH_ISPIMGV2_LS_BLKCFG0_TYPE                            (0x99U) /**< @brief #ISPIMGV2_LS_BlkCfg0Type                        */
#define BRCM_SWARCH_ISPIMGV2_LS_BLKCFG1_ID_MACRO                        (0x9AU) /**< @brief #ISPIMGV2_LS_BLKCFG1_ID                         */
#define BRCM_SWARCH_ISPIMGV2_LS_BLKCFG1_MAX_TABLE_ELTS_MACRO            (0x9BU) /**< @brief #ISPIMGV2_LS_BLKCFG1_MAX_TABLE_ELTS             */
#define BRCM_SWARCH_ISPIMGV2_LS_BLKCFG1_TYPE                            (0x9CU) /**< @brief #ISPIMGV2_LS_BlkCfg1Type                        */
#define BRCM_SWARCH_ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWX_MACRO           (0x9DU) /**< @brief #ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWX            */
#define BRCM_SWARCH_ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWY_MACRO           (0x9EU) /**< @brief #ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWY            */
#define BRCM_SWARCH_ISPIMGV2_HDR_PWLELEMENT_TYPE                        (0x9FU) /**< @brief #ISPIMGV2_HDR_PwlElementType                    */
#define BRCM_SWARCH_ISPIMGV2_HDR_BLKCFG0_PWL_LEN_MACRO                  (0xA0U) /**< @brief #ISPIMGV2_HDR_BLKCFG0_PWL_LEN                   */
#define BRCM_SWARCH_ISPIMGV2_HDR_BLKCFG0_TYPE                           (0xA1U) /**< @brief #ISPIMGV2_HDR_BlkCfg0Type                       */
#define BRCM_SWARCH_ISPIMGV2_HDR_BLKCFG0_GET_EXP_COMB_WT_MACRO          (0xA2U) /**< @brief #ISPIMGV2_HDR_BLKCFG0_GET_EXP_COMB_WT           */
#define BRCM_SWARCH_ISPIMGV2_HDR_BLKCFG0_GET_PWL_LEN_MACRO              (0xA3U) /**< @brief #ISPIMGV2_HDR_BLKCFG0_GET_PWL_LEN               */
#define BRCM_SWARCH_ISPIMGV2_HDR_BLKCFG0_GET_EXP_SWAP_MACRO             (0xA4U) /**< @brief #ISPIMGV2_HDR_BLKCFG0_GET_EXP_SWAP              */
#define BRCM_SWARCH_ISPIMGV2_HDR_BLKCFG0_GET_PWL_ABSISSA_AT_IDX_MACRO   (0xA5U) /**< @brief #ISPIMGV2_HDR_BLKCFG0_GET_PWL_ABSISSA_AT_IDX    */
#define BRCM_SWARCH_ISPIMGV2_HDR_BLKCFG0_GET_PWL_ORDINATE_AT_IDX_MACRO  (0xA6U) /**< @brief #ISPIMGV2_HDR_BLKCFG0_GET_PWL_ORDINATE_AT_IDX   */
#define BRCM_SWARCH_ISPIMGV2_BDN_PWLELEMENT_TYPE                        (0xA7U) /**< @brief #ISPIMGV2_BDN_PwlElementType                    */
#define BRCM_SWARCH_ISPIMGV2_BDN_BLKCFG0_PWL_LEN_MACRO                  (0xA8U) /**< @brief #ISPIMGV2_BDN_BLKCFG0_PWL_LEN                   */
#define BRCM_SWARCH_ISPIMGV2_BDN_BLKCFG0_TYPE                           (0xA9U) /**< @brief #ISPIMGV2_BDN_BlkCfg0Type                       */
#define BRCM_SWARCH_ISPIMGV2_BDN_BLKCFG0_GET_MASK_MACRO                 (0xAAU) /**< @brief #ISPIMGV2_BDN_BLKCFG0_GET_MASK                  */
#define BRCM_SWARCH_ISPIMGV2_BDN_BLKCFG0_GET_PWL_LEN_MACRO              (0xABU) /**< @brief #ISPIMGV2_BDN_BLKCFG0_GET_PWL_LEN               */
#define BRCM_SWARCH_ISPIMGV2_STATISTICS_BLKCFG0_MAX_NUM_REGIONS_MACRO   (0xACU) /**< @brief #ISPIMGV2_STATISTICS_BLKCFG0_MAX_NUM_REGIONS    */
#define BRCM_SWARCH_ISPIMGV2_STATISTICS_GROUP_THRESHOLD_TYPE            (0xADU) /**< @brief #ISPIMGV2_Statistics_GroupThresholdType         */
#define BRCM_SWARCH_ISPIMGV2_STATISTICS_REGION_GROUP_0_TYPE             (0xAEU) /**< @brief #ISPIMGV2_Statistics_Region_Group_0_Type        */
#define BRCM_SWARCH_ISPIMGV2_STATISTICS_REGION_GROUP_1_2_TYPE           (0xAFU) /**< @brief #ISPIMGV2_Statistics_Region_Group_1_2_Type      */
#define BRCM_SWARCH_ISPIMGV2_STATISTICS_BLKCFG0_TYPE                    (0xB0U) /**< @brief #ISPIMGV2_Statistics_BlkCfg0Type                */
#define BRCM_SWARCH_ISPIMGV2_WBGAIN_BLKCFG0_TYPE                        (0xB1U) /**< @brief #ISPIMGV2_WBGain_BlkCfg0Type                    */
#define BRCM_SWARCH_ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_R_MACRO          (0xB2U) /**< @brief #ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_R           */
#define BRCM_SWARCH_ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_G_MACRO          (0xB3U) /**< @brief #ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_G           */
#define BRCM_SWARCH_ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_B_MACRO          (0xB4U) /**< @brief #ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_B           */
#define BRCM_SWARCH_ISPIMGV2_DEFECTIVEPIXEL_BLKCFG0_TYPE                (0xB5U) /**< @brief #ISPIMGV2_DefectivePixel_BlkCfg0Type            */
#define BRCM_SWARCH_ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_LOW_MACRO      (0xB6U) /**< @brief #ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_LOW       */
#define BRCM_SWARCH_ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_HIGH_MACRO     (0xB7U) /**< @brief #ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_HIGH      */
#define BRCM_SWARCH_ISPIMGV2_FRINGING_BLKCFG0_TYPE                      (0xB8U) /**< @brief #ISPIMGV2_Fringing_BlkCfg0Type                  */
#define BRCM_SWARCH_ISPIMGV2_CROSSTALK_PWLELEMENT_TYPE                  (0xB9U) /**< @brief #ISPIMGV2_Crosstalk_PwlElementType              */
#define BRCM_SWARCH_ISPIMGV2_XC_BLKCFG0_PWL_LEN_MACRO                   (0xBAU) /**< @brief #ISPIMGV2_XC_BLKCFG0_PWL_LEN                    */
#define BRCM_SWARCH_ISPIMGV2_CROSSTALK_BLKCFG0_TYPE                     (0xBBU) /**< @brief #ISPIMGV2_Crosstalk_BlkCfg0Type                 */
#define BRCM_SWARCH_ISPIMGV2_DEMOSAIC_BLKCFG0_TYPE                      (0xBCU) /**< @brief #ISPIMGV2_Demosaic_BlkCfg0Type                  */
#define BRCM_SWARCH_ISPIMGV2_DM_BLKCFG0_GET_BAYER_TYPE_MACRO            (0xBDU) /**< @brief #ISPIMGV2_DM_BLKCFG0_GET_BAYER_TYPE             */
#define BRCM_SWARCH_ISPIMGV2_DM_BLKCFG0_GET_HV_BIAS_MACRO               (0xBEU) /**< @brief #ISPIMGV2_DM_BLKCFG0_GET_HV_BIAS                */
#define BRCM_SWARCH_ISPIMGV2_DM_BLKCFG0_GET_OFFSET_SHIFT_G_MACRO        (0xBFU) /**< @brief #ISPIMGV2_DM_BLKCFG0_GET_OFFSET_SHIFT_G         */
#define BRCM_SWARCH_ISPIMGV2_COLORDENOISE_PWLELEMENT_TYPE               (0xC0U) /**< @brief #ISPIMGV2_ColorDenoise_PwlElementType           */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0_PWL_LEN_MACRO                  (0xC1U) /**< @brief #ISPIMGV2_CDN_BLKCFG0_PWL_LEN                   */
#define BRCM_SWARCH_ISPIMGV2_COLORDENOISE_BLKCFG0_TYPE                  (0xC2U) /**< @brief #ISPIMGV2_ColorDenoise_BlkCfg0Type              */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0PWLOFFSETANDLENGTHDESC_TYPE     (0xC3U) /**< @brief #ISPIMGV2_CDN_BlkCfg0PwlOffsetAndLengthDescType */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0COMPUTEPWLOFFSETSANDLEN_MACRO   (0xC4U) /**< @brief #ISPIMGV2_CDN_BlkCfg0ComputePwlOffsetsAndLen    */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0_GET_FIR_SHIFT_MACRO            (0xC5U) /**< @brief #ISPIMGV2_CDN_BLKCFG0_GET_FIR_SHIFT             */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_LINES_MACRO       (0xC6U) /**< @brief #ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_LINES        */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_SHIFT_MACRO       (0xC7U) /**< @brief #ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_SHIFT        */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_G_MACRO             (0xC8U) /**< @brief #ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_G              */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_BG_MACRO            (0xC9U) /**< @brief #ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_BG             */
#define BRCM_SWARCH_ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_RG_MACRO            (0xCAU) /**< @brief #ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_RG             */
#define BRCM_SWARCH_ISPIMGV2_SHARPEN_PWLELEMENT_TYPE                    (0xCBU) /**< @brief #ISPIMGV2_Sharpen_PwlElementType                */
#define BRCM_SWARCH_ISPIMGV2_SHARPEN_BLKCFG0_TYPE                       (0xCCU) /**< @brief #ISPIMGV2_Sharpen_BlkCfg0Type                   */
#define BRCM_SWARCH_ISPIMGV2_SHARPEN_BLKCFG0PWLOFFSETANDLENGTHDESC_TYPE (0xCDU) /**< @brief #ISPIMGV2_Sharpen_BlkCfg0PwlOffsetAndLengthDescType */
#define BRCM_SWARCH_ISPIMGV2_SHARPEN_BLKCFG0COMPUTEPWLOFFSETSANDLEN_PROC (0xCEU)/**< @brief #ISPIMGV2_Sharpen_BlkCfg0ComputePwlOffsetsAndLen    */
#define BRCM_SWARCH_ISPIMGV2_YCYG_BLKCFG0_TYPE                          (0xCFU) /**< @brief #ISPIMGV2_YCYG_BlkCfg0Type                      */
#define BRCM_SWARCH_ISPIMGV2_YCYG_BLKCFG0_GET_SIGN_MASK_MACRO           (0xD0U) /**< @brief #ISPIMGV2_YCYG_BLKCFG0_GET_SIGN_MASK            */
#define BRCM_SWARCH_ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_0_PROC             (0xD1U) /**< @brief #ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_0             */
#define BRCM_SWARCH_ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_1_PROC             (0xD2U) /**< @brief #ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_1             */
#define BRCM_SWARCH_ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_2_PROC             (0xD3U) /**< @brief #ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_2             */
#define BRCM_SWARCH_ISPIMGV2_GAMMA_PWLELEMENT_TYPE                      (0xD4U) /**< @brief #ISPIMGV2_Gamma_PwlElementType                  */
#define BRCM_SWARCH_ISPIMGV2_GAMMA_BLKCFG0_TYPE                         (0xD5U) /**< @brief #ISPIMGV2_Gamma_BlkCfg0Type                     */
#define BRCM_SWARCH_ISPIMGV2_GAMMA_BLKCFG0PWLOFFSETANDLENGTHDESC_TYPE   (0xD6U) /**< @brief #ISPIMGV2_Gamma_BlkCfg0PwlOffsetAndLengthDescType   */
#define BRCM_SWARCH_ISPIMGV2_GAMMA_BLKCFG0COMPUTEPWLOFFSETSANDLEN_PROC  (0xD7U) /**< @brief #ISPIMGV2_Gamma_BlkCfg0ComputePwlOffsetsAndLen  */
#define BRCM_SWARCH_ISPIMGV2_FC_BLKCFG0_TYPE                            (0xD8U) /**< @brief #ISPIMGV2_FC_BlkCfg0Type                        */
#define BRCM_SWARCH_ISPIMGV2_LTM_PWLELEMENT_TYPE                        (0xD9U) /**< @brief #ISPIMGV2_LTM_PwlElementType                    */
#define BRCM_SWARCH_ISPIMGV2_LTM_BLKCFG0_TYPE                           (0xDAU) /**< @brief #ISPIMGV2_LTM_BlkCfg0Type                       */
#define BRCM_SWARCH_ISPIMGV2_LTM_BLKCFG0PWLOFFSETANDLENGTHDESC_TYPE     (0xDBU) /**< @brief #ISPIMGV2_LTM_BlkCfg0PwlOffsetAndLengthDescType */
#define BRCM_SWARCH_ISPIMGV2_LTM_BLKCFG0COMPUTEPWLOFFSETSANDLEN_PROC    (0xDCU) /**< @brief #ISPIMGV2_LTM_BlkCfg0ComputePwlOffsetsAndLen    */
#define BRCM_SWARCH_ISPIMGV2_TM_PWLELEMENT_TYPE                         (0xDDU) /**< @brief #ISPIMGV2_TM_PwlElementType                     */
#define BRCM_SWARCH_ISPIMGV2_TM_BLKCFG0_TYPE                            (0xDEU) /**< @brief #ISPIMGV2_TM_BlkCfg0Type                        */
#define BRCM_SWARCH_ISPIMGV2_CP_PWLELEMENT_TYPE                         (0xDFU) /**< @brief #ISPIMGV2_CP_PwlElementType                     */
#define BRCM_SWARCH_ISPIMGV2_CP_BLKCFG0_TYPE                            (0xE0U) /**< @brief #ISPIMGV2_CP_BlkCfg0Type                        */
#define BRCM_SWARCH_ISPIMGV2_CP_BLKCFG0PWLOFFSETANDLENGTHDESC_TYPE      (0xE1U) /**< @brief #ISPIMGV2_CP_BlkCfg0PwlOffsetAndLengthDescType  */
#define BRCM_SWARCH_ISPIMGV2_CP_BLKCFG0COMPUTEPWLOFFSETSANDLEN_PROC     (0xE2U) /**< @brief #ISPIMGV2_CP_BlkCfg0ComputePwlOffsetsAndLen     */
#define BRCM_SWARCH_ISPIMGV2_CP_BLKCFG0POPULATEPWL_PROC                 (0xE3U) /**< @brief #ISPIMGV2_CP_BlkCfg0PopulatePWL                 */
#define BRCM_SWARCH_ISPIMGV2_CC_BLKCFG0_TYPE                            (0xE4U) /**< @brief #ISPIMGV2_CC_BlkCfg0Type                        */
#define BRCM_SWARCH_ISPIMGV2_LR_BLKCFG0_TYPE                            (0xE5U) /**< @brief #ISPIMGV2_LR_BlkCfg0Type                        */
#define BRCM_SWARCH_ISPIMGV2_LR_BLKCFG0_GET_RB_SWAP_MACRO               (0xE6U) /**< @brief #ISPIMGV2_LR_BLKCFG0_GET_RB_SWAP                */
#define BRCM_SWARCH_ISPIMGV2_LR_BLKCFG0_GET_SHIFT_MACRO                 (0xE7U) /**< @brief #ISPIMGV2_LR_BLKCFG0_GET_SHIFT                  */
#define BRCM_SWARCH_ISPIMGV2_OUTPUT_BLKCFG0_TYPE                        (0xE8U) /**< @brief #ISPIMGV2_Output_BlkCfg0Type                    */
#define BRCM_SWARCH_ISPIMGV2_OUTPUT_BLKCFG0_GET_VFLIP_MACRO             (0xE9U) /**< @brief #ISPIMGV2_OUTPUT_BLKCFG0_GET_VFLIP              */
#define BRCM_SWARCH_ISPIMGV2_OUTPUT_BLKCFG0_GET_BIG_ENDIAN_MACRO        (0xEAU) /**< @brief #ISPIMGV2_OUTPUT_BLKCFG0_GET_BIG_ENDIAN         */
#define BRCM_SWARCH_ISPIMGV2_OUTPUT_BLKCFG0_GET_OUTPUT_WIDTH_MACRO      (0xEBU) /**< @brief #ISPIMGV2_OUTPUT_BLKCFG0_GET_OUTPUT_WIDTH       */
#define BRCM_SWARCH_ISPIMGV2_OUTPUT_BLKCFG0_GET_SAT_DEPTH_MACRO         (0xECU) /**< @brief #ISPIMGV2_OUTPUT_BLKCFG0_GET_SAT_DEPTH          */
#define BRCM_SWARCH_ISPIMGV2_OUTPUT_BLKCFG0_GET_SHIFT_MACRO             (0xEDU) /**< @brief #ISPIMGV2_OUTPUT_BLKCFG0_GET_SHIFT              */
#define BRCM_SWARCH_ISPIMGV2_OUTPUT_BLKCFG0_GET_DATA_FORMAT_MACRO       (0xEEU) /**< @brief #ISPIMGV2_OUTPUT_BLKCFG0_GET_DATA_FORMAT        */
#define BRCM_SWARCH_ISPIMGV2_MAGIC_V2_MACRO                             (0xEFU) /**< @brief #ISPIMGV2_MAGIC_V2                              */
#define BRCM_SWARCH_ISPIMGV2_CONFIGURE_PROC                             (0xF0U) /**< @brief #ISPIMGV2_Configure                             */
/** @} */

/**
    @{
    @brief Block ID's.
    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_BLOCK_ID_IIFR          (0x00UL)
#define ISPIMGV2_BLOCK_ID_BL            (0x01UL)
#define ISPIMGV2_BLOCK_ID_LS            (0x02UL)
#define ISPIMGV2_BLOCK_ID_HDR           (0x04UL)
#define ISPIMGV2_BLOCK_ID_BDN           (0x05UL)
#define ISPIMGV2_BLOCK_ID_ST            (0x06UL)
#define ISPIMGV2_BLOCK_ID_WBG           (0x07UL)
#define ISPIMGV2_BLOCK_ID_DPC           (0x08UL)
#define ISPIMGV2_BLOCK_ID_FG            (0x09UL)
#define ISPIMGV2_BLOCK_ID_XC            (0x0AUL)
#define ISPIMGV2_BLOCK_ID_CDN           (0x0BUL)
#define ISPIMGV2_BLOCK_ID_DM            (0x0CUL)
#define ISPIMGV2_BLOCK_ID_SH            (0x0DUL)
#define ISPIMGV2_BLOCK_ID_YC            (0x0EUL)
#define ISPIMGV2_BLOCK_ID_GM            (0x0FUL)
#define ISPIMGV2_BLOCK_ID_YG            (0x10UL)
#define ISPIMGV2_BLOCK_ID_FC            (0x11UL)
#define ISPIMGV2_BLOCK_ID_LTM           (0x12UL)
#define ISPIMGV2_BLOCK_ID_TM            (0x13UL)
#define ISPIMGV2_BLOCK_ID_CP            (0x14UL)
#define ISPIMGV2_BLOCK_ID_CC            (0x15UL)
#define ISPIMGV2_BLOCK_ID_LR            (0x16UL)
#define ISPIMGV2_BLOCK_ID_HO            (0x17UL)
#define ISPIMGV2_BLOCK_ID_LO            (0x18UL)
/** @} */

/**
    @{
    @brief Possible values of the Block Config ID's.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_BLKCFG_ID_0             (0x0UL)
#define ISPIMGV2_BLKCFG_ID_1             (0x1UL)
#define ISPIMGV2_BLKCFG_ID_2             (0x2UL)
#define ISPIMGV2_BLKCFG_ID_3             (0x3UL)
#define ISPIMGV2_BLKCFG_ID_4             (0x4UL)
#define ISPIMGV2_BLKCFG_ID_5             (0x5UL)
#define ISPIMGV2_BLKCFG_ID_6             (0x6UL)
#define ISPIMGV2_BLKCFG_ID_7             (0x7UL)

/** @} */

/**
    @brief Macro to extract block id from the section ID.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_GET_BLOCK_ID(sectionId) ((((uint32_t)sectionId) & 0xF8UL) >> 3UL)

/**
    @brief Macro to extract block config id from the section ID.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_GET_BLKCFG_ID(sectionId) (((uint32_t)sectionId) & 0x07UL)

/**
    @brief Type describing the layout of each element in the section table.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_SectionOffsetTableElementType {
    uint8_t sectionId;
    uint16_t sectionOffset;
} ISPIMGV2_SectionOffsetTableElementType;

/**
    @brief Maximum number of allowed sections.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_MAX_NUM_SECTIONS           (32U)

/**
    @brief Type describing the layout of a section table.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_SectionOffsetTableType {
    uint16_t numSections;
    ISPIMGV2_SectionOffsetTableElementType sections[ISPIMGV2_MAX_NUM_SECTIONS];
} ISPIMGV2_SectionOffsetTableType;

/**
    @brief Type describing the layout of each element in the pass table.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_PassOffsetTableElementType {
    uint8_t passId;
    uint16_t passSectionTableOffset;
} ISPIMGV2_PassOffsetTableElementType;

/**
    @brief Maximum number of allowed passes.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_MAX_NUM_PASSES             (3U)

/**
    @brief Type describing the layout of a pass table.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_PassOffsetTableType {
    uint8_t numPasses;
    ISPIMGV2_PassOffsetTableElementType passes[ISPIMGV2_MAX_NUM_PASSES];
} ISPIMGV2_PassOffsetTableType;

/**
    @brief Type describing the ISP configuration image header.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_ConfigImageType {
    ISYS_CommonImgHdrType imgHdr;
    ISPIMGV2_PassOffsetTableType passOffsetTable;
} ISPIMGV2_ConfigImageType;

/**
    @brief Frame and Input image configuration.

    This configuration programmes both II and FR
    blocks on the ISP HW.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED  _ISPIMGV2_IIFR_BlkCfg0Type {
    uint16_t magic;
    uint16_t width;
    uint16_t height;
    uint16_t stride;
    uint32_t bayerEn;
    uint32_t yCbCrEn;
    uint16_t dpcmCfg;
    uint8_t formatBayerOrderAndTranspose;
} ISPIMGV2_IIFR_BlkCfg0Type;

/**
    @brief Macro to extract the dpcm mode of the frame

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/

#define ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_MODE(iifrBlkCfg0)      \
    ((uint8_t)(((uint32_t)((iifrBlkCfg0)->dpcmCfg) >> 9UL) & 0x7UL))

/**
    @brief Macro to extract the dpcm block length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_BLOCKLEN(iifrBlkCfg0)  \
    ((uint16_t)(((uint32_t)((iifrBlkCfg0)->dpcmCfg)) & 0x1FFUL))

/**
    @brief Macro to extract image format

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_IIFR_BLKCFG0_GET_FORMAT(iifrBlkCfg0)         \
    ((uint8_t)(((uint32_t)((iifrBlkCfg0)->formatBayerOrderAndTranspose)) & 0xFUL))

/**
    @brief Macro to extract the bayer order of the frame

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_IIFR_BLKCFG0_GET_BAYER_ORDER(iifrBlkCfg0)    \
    ((uint8_t)((((uint32_t)((iifrBlkCfg0)->formatBayerOrderAndTranspose)) >> 4UL) & 0x3UL))

/**
    @brief Macro to extract the transposed setting of the frame

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_IIFR_BLKCFG0_GET_TRANSPOSED(iifrBlkCfg0)     \
    ((uint8_t)((((uint32_t)((iifrBlkCfg0)->formatBayerOrderAndTranspose)) >> 6UL) & 0x1UL))

/**
    @brief Black level PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_BL_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_BL_PwlElementType;

/**
    @brief Length of BL PWL's

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_BL_BLKCFG0_PWL_LEN     (16U)

/**
    @brief Black level config specific to a particular exposure

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_BL_BlkCfg0ChannelConfigType {
    uint16_t lvlR;
    uint16_t lvlB;
    uint16_t lvlGR;
    uint16_t lvlGB;
    uint8_t  pwlLenR;
    uint8_t  pwlLenB;
    uint8_t  pwlLenGR;
    uint8_t  pwlLenGB;
} ISPIMGV2_BL_BlkCfg0ChannelConfigType;

/**
    @brief Uncompressed black level configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_BL_BlkCfg0Type {
    uint16_t magic;
    uint16_t dfBorderLeft;
    uint16_t dfBorderRight;
    uint16_t dfBorderTop;
    uint16_t dfBorderBottom;
    uint16_t dfMult;
    uint16_t threshold;
    ISPIMGV2_BL_BlkCfg0ChannelConfigType channelConfig[ISP_IN_IDX_MAX];
    ISPIMGV2_BL_PwlElementType pwlElements[4U * ISP_IN_IDX_MAX * ISPIMGV2_BL_BLKCFG0_PWL_LEN];
} ISPIMGV2_BL_BlkCfg0Type;

/**
    @brief Helper type for PWL offset computations corresponding to the
           uncompressed black level configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_BL_BlkCfg0PwlOffsetDescType {
    uint32_t pwlOffsetR;
    uint32_t pwlOffsetB;
    uint32_t pwlOffsetGR;
    uint32_t pwlOffsetGB;
} ISPIMGV2_BL_BlkCfg0PwlOffsetDescType;

/** @brief Helper function to compute the PWL offsets corresponding to the
           uncompressed black level configuration

    @behavior Sync, Re-entrant

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Offset computation successful.
    @retval     #BCM_ERR_INVAL_PARAMS    Offset computation went bad. The config
                                         passed in is invalid.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_BL_BlkCfg0ComputePwlOffsets(const ISPIMGV2_BL_BlkCfg0Type *const aBlBlkCfg0, ISPIMGV2_BL_BlkCfg0PwlOffsetDescType *const aPwlOffsetDesc)
{
    int32_t ret;
    uint32_t i;

    uint32_t invalidPwlLength = 0U;

    /* Validate PWL lengths */
    for (i = 0U; i < ISP_IN_IDX_MAX; i++) {

        if (aBlBlkCfg0->channelConfig[i].pwlLenR > ISPIMGV2_BL_BLKCFG0_PWL_LEN) {
            invalidPwlLength |= 0x1U;
        }

        if (aBlBlkCfg0->channelConfig[i].pwlLenB > ISPIMGV2_BL_BLKCFG0_PWL_LEN) {
            invalidPwlLength |= 0x1U;
        }

        if (aBlBlkCfg0->channelConfig[i].pwlLenGB > ISPIMGV2_BL_BLKCFG0_PWL_LEN) {
            invalidPwlLength |= 0x1U;
        }

        if (aBlBlkCfg0->channelConfig[i].pwlLenGR > ISPIMGV2_BL_BLKCFG0_PWL_LEN) {
            invalidPwlLength |= 0x1U;
        }
    }

    if (0U == invalidPwlLength) {

        uint32_t offset = 0U;

        for (i = 0U; i < ISP_IN_IDX_MAX; i++) {
            aPwlOffsetDesc[i].pwlOffsetR = offset;
            offset += aBlBlkCfg0->channelConfig[i].pwlLenR;

            aPwlOffsetDesc[i].pwlOffsetB = offset;
            offset += aBlBlkCfg0->channelConfig[i].pwlLenB;

            aPwlOffsetDesc[i].pwlOffsetGR = offset;
            offset += aBlBlkCfg0->channelConfig[i].pwlLenGR;

            aPwlOffsetDesc[i].pwlOffsetGB = offset;
            offset += aBlBlkCfg0->channelConfig[i].pwlLenGB;
        }

        ret = BCM_ERR_OK;

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/**
    @brief Default lens shading configuration.

    The configuration image reader, on seeing this config in
    the image uses "default" settings for the lens shading.

    This boils down to programming the unity lens shading tables
    on the HW.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_LS_BlkCfg0Type {
} ISPIMGV2_LS_BlkCfg0Type;

/**
    @brief Type describing an element in the uncompressed lens shading table.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_LS_TableElementType {
    uint16_t cvR;
    uint16_t cvB;
    uint16_t cvGR;
    uint16_t cvGB;
} ISPIMGV2_LS_TableElementType;

/**
    @brief Maximum uncompressed lens shading table size.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_LS_BLKCFG1_MAX_TABLE_ELTS  (1089U)

/**
    @brief Uncompressed lens shading configuration.

    Lens shading configuration is uncompressed. That is, the
    lens shading tables (which tend to be quite large) are explicitly
    set by the user.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_LS_BlkCfg1Type {
    uint16_t magic;
    uint8_t pitch;
    uint8_t vpitch;
    uint8_t cellSizes;
    uint16_t tableEltCount;
    ISPIMGV2_LS_TableElementType lensShadingTable[ISPIMGV2_LS_BLKCFG1_MAX_TABLE_ELTS];
} ISPIMGV2_LS_BlkCfg1Type;

/**
    @brief Macro to extract cell_size_power_x.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWX(lsBlkCfg1) \
    ((uint8_t)(((lsBlkCfg1)->cellSizes) & 0x0FUL))

/**
    @brief Macro to extract cell_size_power_y.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWY(lsBlkCfg1) \
    ((uint8_t)((((lsBlkCfg1)->cellSizes) & 0xF0UL) >> 4UL))

/**
    @brief HDR fusion PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_HDR_PwlElementType {
    uint8_t  absLo;
    uint16_t absHi;
    uint16_t ord;
} ISPIMGV2_HDR_PwlElementType;

/**
    @brief HDR fusion PWL length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_HDR_BLKCFG0_PWL_LEN    (16U)

/**
    @brief HDR fusion configuration.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_HDR_BlkCfg0Type {
    uint16_t magic;
    uint16_t gainR;
    uint16_t gainB;
    uint16_t expRatio;
    uint32_t wbThres;
    uint16_t expCombLoThres;
    uint16_t expCombHiThres;
    uint32_t expCombWtPwlLenAndExpSwap;
    ISPIMGV2_HDR_PwlElementType pwlElements[ISPIMGV2_HDR_BLKCFG0_PWL_LEN];
} ISPIMGV2_HDR_BlkCfg0Type;

/**
    @brief Macro to extract the exposure combining weight

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_HDR_BLKCFG0_GET_EXP_COMB_WT(hdrBlkCfg0)                \
    ((uint32_t)((hdrBlkCfg0)->expCombWtPwlLenAndExpSwap & 0xFFFFFUL))

/**
    @brief Macro to extract the HDR PWL length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_HDR_BLKCFG0_GET_PWL_LEN(hdrBlkCfg0)                    \
    ((uint32_t)(((hdrBlkCfg0)->expCombWtPwlLenAndExpSwap & 0x1F00000UL) >> 20UL))

/**
    @brief Macro to check if the exposures are to be swapped

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_HDR_BLKCFG0_GET_EXP_SWAP(hdrBlkCfg0)                   \
    ((uint8_t)(((hdrBlkCfg0)->expCombWtPwlLenAndExpSwap & 0x2000000UL) >> 25UL))

/**
    @brief Macro to extract a HDR PWL absissa at a specified index

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_HDR_BLKCFG0_GET_PWL_ABSISSA_AT_IDX(blkCfgStr0, idx)    \
    (((uint32_t)((blkCfgStr0)->pwlElements[(idx)].absLo)) |             \
     (((uint32_t)((blkCfgStr0)->pwlElements[(idx)].absHi)) << 8UL))

/**
    @brief Macro to extract a HDR PWL ordinate at a specified index

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_HDR_BLKCFG0_GET_PWL_ORDINATE_AT_IDX(blkCfgStr0, idx)   \
    ((uint16_t)((blkCfgStr0)->pwlElements[(idx)].ord))

/**
    @brief Bayer denoise PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_BDN_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_BDN_PwlElementType;

/**
    @brief Bayer denoise PWL length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_BDN_BLKCFG0_PWL_LEN    (12U)

/**
    @brief Bayer denoise configuration.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_BDN_BlkCfg0Type {
    uint16_t magic;
    uint8_t gainR;
    uint8_t gainB;
    uint8_t gainGR;
    uint8_t gainGB;
    uint8_t strengthR;
    uint8_t strengthB;
    uint8_t strengthGR;
    uint8_t strengthGB;
    uint64_t maskAndPwlLen;
    ISPIMGV2_BDN_PwlElementType pwlElements[ISPIMGV2_BDN_BLKCFG0_PWL_LEN];
} ISPIMGV2_BDN_BlkCfg0Type;

/**
    @brief Macro to extract the pixel cluster mask

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_BDN_BLKCFG0_GET_MASK(bdnBlkCfg0)       \
    (((bdnBlkCfg0)->maskAndPwlLen) & 0x1FFFFFFFFFFFFULL)

/**
    @brief Macro to extract the bayer denoise PWL length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_BDN_BLKCFG0_GET_PWL_LEN(bdnBlkCfg0)    \
    ((uint32_t)(((bdnBlkCfg0)->maskAndPwlLen) >> 56UL))

/**
    @brief Statistics region array length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_STATISTICS_BLKCFG0_MAX_NUM_REGIONS (32UL)

/**
    @brief Statistics group threshold configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Statistics_GroupThresholdType {
    uint16_t rLo;
    uint16_t rHi;
    uint16_t gLo;
    uint16_t gHi;
    uint16_t bLo;
    uint16_t bHi;
    int16_t rgLo;
    int16_t rgHi;
    int16_t bgLo;
    int16_t bgHi;
} ISPIMGV2_Statistics_GroupThresholdType;

/**
    @brief Statistics group 0 region description

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Statistics_Region_Group_0_Type {
    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t width;
    uint16_t height;
    uint16_t hNum;
    uint16_t vNum;
} ISPIMGV2_Statistics_Region_Group_0_Type;

/**
    @brief Statistics group 1 and 2 region description

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Statistics_Region_Group_1_2_Type {
    uint16_t offsetX;
    uint16_t offsetY;
    uint16_t width;
    uint16_t height;
} ISPIMGV2_Statistics_Region_Group_1_2_Type;

/**
    @brief Statistics configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Statistics_BlkCfg0Type {
    uint16_t magic;
    uint16_t frameWidth;
    uint16_t frameHeight;
    uint16_t hMask0;
    uint16_t hMask1;
    uint16_t rowFactor;
    uint8_t  pixelShift;
    uint8_t  shift;
    uint8_t  columns;
    uint8_t  numGroup1And2Regions;
    ISPIMGV2_Statistics_GroupThresholdType groupThresholds[3UL];
    ISPIMGV2_Statistics_Region_Group_0_Type   group0Regions;
    ISPIMGV2_Statistics_Region_Group_1_2_Type group1And2Regions[ISPIMGV2_STATISTICS_BLKCFG0_MAX_NUM_REGIONS];
} ISPIMGV2_Statistics_BlkCfg0Type;

/**
    @brief White balance and gain configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_WBGain_BlkCfg0Type {
    uint16_t magic;
    uint16_t gainR;
    uint16_t gainB;
    uint16_t gainAll;
    uint64_t offsets;
    uint16_t threshold;
} ISPIMGV2_WBGain_BlkCfg0Type;

/**
    @brief Macro to extract the red gain offset

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/

#define ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_R(wbGainBlkCfg0) \
    ((uint32_t)(((wbGainBlkCfg0)->offsets) & 0x1FFFFULL))

/**
    @brief Macro to extract the green gain offset

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/

#define ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_G(wbGainBlkCfg0) \
    ((uint32_t)((((wbGainBlkCfg0)->offsets) >> 20UL) & 0x1FFFFULL))

/**
    @brief Macro to extract the blue gain offset

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_B(wbGainBlkCfg0) \
    ((uint32_t)((((wbGainBlkCfg0)->offsets) >> 40UL) & 0x1FFFFULL))

/**
    @brief Defective pixel correction configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_DefectivePixel_BlkCfg0Type {
    uint16_t magic;
    int8_t  scaleLo[3U];
    int16_t offsetLo[3U];
    int8_t  scaleHi[3U];
    int16_t offsetHi[3U];
    uint8_t replaceIdxs;
    uint8_t replaceScaleLo;
    uint8_t replaceScaleHi;
} ISPIMGV2_DefectivePixel_BlkCfg0Type;

/**
    @brief Macro to extract the low replace index

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_LOW(dpcBlkCfg0)    \
    ((uint8_t)((dpcBlkCfg0)->replaceIdxs & 0xFU))

/**
    @brief Macro to extract the high replace index

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_HIGH(dpcBlkCfg0)   \
    ((uint8_t)(((dpcBlkCfg0)->replaceIdxs >> 4U) & 0xFU))

/**
    @brief Fringing correction configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Fringing_BlkCfg0Type {
    uint16_t magic;
    int16_t rPos;
    int16_t rNeg;
    int16_t bPos;
    int16_t bNeg;
    int16_t grPos;
    int16_t grNeg;
    int16_t gbPos;
    int16_t gbNeg;
    uint16_t rPosLim;
    uint16_t rNegLim;
    uint16_t bPosLim;
    uint16_t bNegLim;
    uint16_t grPosLim;
    uint16_t grNegLim;
    uint16_t gbPosLim;
    uint16_t gbNegLim;
} ISPIMGV2_Fringing_BlkCfg0Type;

/**
    @brief Crosstalk PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Crosstalk_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_Crosstalk_PwlElementType;

/**
    @brief Crosstalk PWL length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_XC_BLKCFG0_PWL_LEN    (8U)

/**
    @brief Crosstalk configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Crosstalk_BlkCfg0Type {
    uint16_t magic;
    uint8_t pwlLength;
    ISPIMGV2_Crosstalk_PwlElementType pwlElements[ISPIMGV2_XC_BLKCFG0_PWL_LEN];
} ISPIMGV2_Crosstalk_BlkCfg0Type;

/**
    @brief Demosaic configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Demosaic_BlkCfg0Type {
    uint16_t magic;
    uint16_t threshold;
    uint16_t bayerTypeHVBiasAndOffShG;
} ISPIMGV2_Demosaic_BlkCfg0Type;

/**
    @brief Macro to extract the bayer type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_DM_BLKCFG0_GET_BAYER_TYPE(dmBlkCfg0)       \
    ((uint8_t)((dmBlkCfg0)->bayerTypeHVBiasAndOffShG & 0x3U))

/**
    @brief Macro to extract the HV bias

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_DM_BLKCFG0_GET_HV_BIAS(dmBlkCfg0)          \
    ((uint16_t)(((dmBlkCfg0)->bayerTypeHVBiasAndOffShG >> 2U) & 0x1FFU))

/**
    @brief Macro to extract the G offset and shift

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_DM_BLKCFG0_GET_OFFSET_SHIFT_G(dmBlkCfg0)   \
    ((uint8_t)(((dmBlkCfg0)->bayerTypeHVBiasAndOffShG >> 11U) & 0xFU))

/**
    @brief Color denoise PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_ColorDenoise_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_ColorDenoise_PwlElementType;

/**
    @brief Color denoise PWL length

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_CDN_BLKCFG0_PWL_LEN    (6UL)

/**
    @brief Color denoise configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_ColorDenoise_BlkCfg0Type {
    uint16_t magic;
    uint16_t misc;
    uint16_t xCenter;
    uint16_t yCenter;
    uint16_t edgeThreshold;
    uint16_t edgeStrength;
    uint16_t edgeMin;
    uint16_t weights;
    uint32_t pwlLengths;
    ISPIMGV2_ColorDenoise_PwlElementType pwlElements[7UL * ISPIMGV2_CDN_BLKCFG0_PWL_LEN];
} ISPIMGV2_ColorDenoise_BlkCfg0Type;

/**
    @brief Helper type for PWL offset computations corresponding to the
           color denoise configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_CDN_BlkCfg0PwlOffsetAndLengthDescType {
    uint32_t pwlOffsetFirBg;
    uint32_t pwlLengthFirBg;
    uint32_t pwlOffsetFirRg;
    uint32_t pwlLengthFirRg;
    uint32_t pwlOffsetIirBG;
    uint32_t pwlLengthIirBG;
    uint32_t pwlOffsetIirRg;
    uint32_t pwlLengthIirRg;
    uint32_t pwlOffsetDist;
    uint32_t pwlLengthDist;
    uint32_t pwlOffsetG;
    uint32_t pwlLengthG;
    uint32_t pwlOffsetSat;
    uint32_t pwlLengthSat;
} ISPIMGV2_CDN_BlkCfg0PwlOffsetAndLengthDescType;

/** @brief Helper function to compute the PWL offsets corresponding to the
           color denoise configuration

    @behavior Sync, Re-entrant

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Offset computation successful.
    @retval     #BCM_ERR_INVAL_PARAMS    Offset computation went bad. The config
                                         passed in is invalid.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_CDN_BlkCfg0ComputePwlOffsetsAndLen(const ISPIMGV2_ColorDenoise_BlkCfg0Type *const aCdnBlkCfg0, ISPIMGV2_CDN_BlkCfg0PwlOffsetAndLengthDescType *const aPwlOffsetAndLenDesc)
{
    uint32_t i;
    int32_t  ret = BCM_ERR_OK;
    const uint32_t pwlLengths = aCdnBlkCfg0->pwlLengths;

    /* All PWL's have to be less than ISPIMGV2_CDN_BLKCFG0_PWL_LEN long */
    for (i = 0UL; i < 7UL; i++) {
        const uint32_t shift = i * 3UL;
        const uint32_t length = (pwlLengths >> shift) & 0x7UL;
        if (ISPIMGV2_CDN_BLKCFG0_PWL_LEN < length) {
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
    }

    if (BCM_ERR_OK == ret) {
        uint32_t offset = 0UL;

        aPwlOffsetAndLenDesc->pwlOffsetFirBg = offset;
        aPwlOffsetAndLenDesc->pwlLengthFirBg = ((pwlLengths) & 0x7UL);
        offset += aPwlOffsetAndLenDesc->pwlLengthFirBg;

        aPwlOffsetAndLenDesc->pwlOffsetFirRg = offset;
        aPwlOffsetAndLenDesc->pwlLengthFirRg = ((pwlLengths >> 3UL) & 0x7UL);
        offset += aPwlOffsetAndLenDesc->pwlLengthFirRg;

        aPwlOffsetAndLenDesc->pwlOffsetIirBG = offset;
        aPwlOffsetAndLenDesc->pwlLengthIirBG = ((pwlLengths >> 6UL) & 0x7UL);
        offset += aPwlOffsetAndLenDesc->pwlLengthIirBG;

        aPwlOffsetAndLenDesc->pwlOffsetIirRg = offset;
        aPwlOffsetAndLenDesc->pwlLengthIirRg = ((pwlLengths >> 9UL) & 0x7UL);
        offset += aPwlOffsetAndLenDesc->pwlLengthIirRg;

        aPwlOffsetAndLenDesc->pwlOffsetDist = offset;
        aPwlOffsetAndLenDesc->pwlLengthDist = ((pwlLengths >> 12UL) & 0x7UL);
        offset += aPwlOffsetAndLenDesc->pwlLengthDist;

        aPwlOffsetAndLenDesc->pwlOffsetG = offset;
        aPwlOffsetAndLenDesc->pwlLengthG = ((pwlLengths >> 15UL) & 0x7UL);
        offset += aPwlOffsetAndLenDesc->pwlLengthG;

        aPwlOffsetAndLenDesc->pwlOffsetSat = offset;
        aPwlOffsetAndLenDesc->pwlLengthSat = ((pwlLengths >> 18UL) & 0x7UL);
    }

    return ret;
}

/**
    @brief Macro to extract the FIR shift

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_CDN_BLKCFG0_GET_FIR_SHIFT(cdnBlkCfg0)      \
    ((uint8_t)((cdnBlkCfg0)->misc & 0x3U))

/**
    @brief Macro to extract the IIR attenuation lines

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_LINES(cdnBlkCfg0) \
    ((uint8_t)(((cdnBlkCfg0)->misc >> 2U) & 0xFU))

/**
    @brief Macro to extract the IIR attenuation shift

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_SHIFT(cdnBlkCfg0) \
    ((uint8_t)(((cdnBlkCfg0)->misc >> 6U) & 0x7U))

/**
    @brief Macro to extract the G weight

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_G(cdnBlkCfg0)       \
    ((uint8_t)(((cdnBlkCfg0)->weights) & 0xFU))

/**
    @brief Macro to extract the BG weight

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_BG(cdnBlkCfg0)      \
    ((uint8_t)(((cdnBlkCfg0)->weights >> 4U) & 0xFU))

/**
    @brief Macro to extract the RG weight

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_RG(cdnBlkCfg0)      \
    ((uint8_t)(((cdnBlkCfg0)->weights >> 8U) & 0xFU))

/**
    @brief Sharpen PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Sharpen_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_Sharpen_PwlElementType;

/**
    @brief Sharpen configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Sharpen_BlkCfg0Type {
    uint16_t magic;
    uint8_t  avgShiftG;
    uint16_t isotropicStrength;
    uint16_t xCenter;
    uint16_t yCenter;
    uint32_t pwlLengths;
    ISPIMGV2_Sharpen_PwlElementType pwlElements[56U];
} ISPIMGV2_Sharpen_BlkCfg0Type;

/**
    @brief Helper type for PWL offset computations corresponding to the
           sharpen configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Sharpen_BlkCfg0PwlOffsetAndLengthDescType {
    uint32_t pwlOffsetPosGain;
    uint32_t pwlLengthPosGain;
    uint32_t pwlOffsetVGain;
    uint32_t pwlLengthVGain;
    uint32_t pwlOffsetResp;
    uint32_t pwlLengthResp;
    uint32_t pwlOffsetResp1;
    uint32_t pwlLengthResp1;
    uint32_t pwlOffsetRespGain;
    uint32_t pwlLengthRespGain;
} ISPIMGV2_Sharpen_BlkCfg0PwlOffsetAndLengthDescType;

/** @brief Helper function to compute the PWL offsets corresponding to the
           sharpen configuration

    @behavior Sync, Re-entrant

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Offset computation successful.
    @retval     #BCM_ERR_INVAL_PARAMS    Offset computation went bad. The config
                                         passed in is invalid.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_Sharpen_BlkCfg0ComputePwlOffsetsAndLen(const ISPIMGV2_Sharpen_BlkCfg0Type *const aShBlkCfg0, ISPIMGV2_Sharpen_BlkCfg0PwlOffsetAndLengthDescType *const aPwlOffsetAndLenDesc)
{
    int32_t ret;
    const uint32_t pwlLengths = aShBlkCfg0->pwlLengths;

    aPwlOffsetAndLenDesc->pwlLengthPosGain  = (pwlLengths & 0xFUL);
    aPwlOffsetAndLenDesc->pwlLengthVGain    = ((pwlLengths >> 4UL) & 0xFUL);
    aPwlOffsetAndLenDesc->pwlLengthRespGain = ((pwlLengths >> 8UL) & 0x1FUL);
    aPwlOffsetAndLenDesc->pwlLengthResp     = ((pwlLengths >> 13UL) & 0xFUL);
    aPwlOffsetAndLenDesc->pwlLengthResp1    = ((pwlLengths >> 17UL) & 0xFUL);

    if ((aPwlOffsetAndLenDesc->pwlLengthPosGain <= 8UL) &&
        (aPwlOffsetAndLenDesc->pwlLengthVGain <= 8UL) &&
        (aPwlOffsetAndLenDesc->pwlLengthResp <= 12UL) &&
        (aPwlOffsetAndLenDesc->pwlLengthResp1 <= 12UL) &&
        (aPwlOffsetAndLenDesc->pwlLengthRespGain <= 16UL)) {

        uint32_t offset = 0UL;

        aPwlOffsetAndLenDesc->pwlOffsetPosGain = offset;
        offset += aPwlOffsetAndLenDesc->pwlLengthPosGain;

        aPwlOffsetAndLenDesc->pwlOffsetVGain = offset;
        offset += aPwlOffsetAndLenDesc->pwlLengthVGain;

        aPwlOffsetAndLenDesc->pwlOffsetRespGain = offset;
        offset += aPwlOffsetAndLenDesc->pwlLengthRespGain;

        aPwlOffsetAndLenDesc->pwlOffsetResp = offset;
        offset += aPwlOffsetAndLenDesc->pwlLengthResp;

        aPwlOffsetAndLenDesc->pwlOffsetResp1 = offset;

        ret = BCM_ERR_OK;

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/**
    @brief Pre and post gamma color conversion configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_YCYG_BlkCfg0Type {
    uint16_t magic;
    int16_t matrix[9U];
    uint8_t offsetSignBits;
    uint16_t offset0Bits16;
    uint16_t offset1Bits16;
    uint16_t offset2Bits16;
} ISPIMGV2_YCYG_BlkCfg0Type;

/**
    @brief Helper macro used by the ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_* functions

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_YCYG_BLKCFG0_GET_SIGN_MASK(blkCfg0, mask)    \
    ((0U == ((blkCfg0)->offsetSignBits & (mask)))  ? 0UL : 0xFFFF0000UL)

/** @brief Helper function to compute the offset 0

    @behavior Sync, Re-entrant

    @retval int32_t Computed Offset 0 value

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_0(const ISPIMGV2_YCYG_BlkCfg0Type *blkCfg0)
{
    const uint32_t val =
        (((uint32_t)((blkCfg0)->offset0Bits16)) | ISPIMGV2_YCYG_BLKCFG0_GET_SIGN_MASK(blkCfg0, 0x1U));
    return (int32_t)val;
}

/** @brief Helper function to compute the offset 1

    @behavior Sync, Re-entrant

    @retval int32_t Computed Offset 1 value

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_1(const ISPIMGV2_YCYG_BlkCfg0Type *blkCfg0)
{
    const uint32_t val =
        (((uint32_t)((blkCfg0)->offset1Bits16)) | ISPIMGV2_YCYG_BLKCFG0_GET_SIGN_MASK(blkCfg0, 0x2U));
    return (int32_t)val;
}

/** @brief Helper function to compute the offset 2

    @behavior Sync, Re-entrant

    @retval int32_t Computed Offset 2 value

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_2(const ISPIMGV2_YCYG_BlkCfg0Type *blkCfg0)
{
    const uint32_t val =
        (((uint32_t)((blkCfg0)->offset2Bits16)) | ISPIMGV2_YCYG_BLKCFG0_GET_SIGN_MASK(blkCfg0, 0x4U));
    return (int32_t)val;
}

/**
    @brief Gamma PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Gamma_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_Gamma_PwlElementType;

/**
    @brief Gamma configuraiton

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Gamma_BlkCfg0Type {
    uint16_t magic;
    uint32_t pwlLengths;
    ISPIMGV2_Gamma_PwlElementType pwlElements[3UL * 32UL];
} ISPIMGV2_Gamma_BlkCfg0Type;

/**
    @brief Helper type for PWL offset computations corresponding to the
           gamma configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Gamma_BlkCfg0PwlOffsetAndLengthDescType {
    uint32_t pwlOffsetR;
    uint32_t pwlLengthR;
    uint32_t pwlOffsetG;
    uint32_t pwlLengthG;
    uint32_t pwlOffsetB;
    uint32_t pwlLengthB;
} ISPIMGV2_Gamma_BlkCfg0PwlOffsetAndLengthDescType;

/** @brief Helper function to compute the PWL offsets corresponding to the
           gamma configuration 0

    @behavior Sync, Re-entrant

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Offset computation successful.
    @retval     #BCM_ERR_INVAL_PARAMS    Offset computation went bad. The config
                                         passed in is invalid.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_Gamma_BlkCfg0ComputePwlOffsetsAndLen(const ISPIMGV2_Gamma_BlkCfg0Type *const aGmBlkCfg0, ISPIMGV2_Gamma_BlkCfg0PwlOffsetAndLengthDescType *const aPwlOffsetAndLenDesc)
{
    int32_t ret;
    const uint32_t pwlLengths = aGmBlkCfg0->pwlLengths;

    const uint32_t lenR = (pwlLengths & 0x2FUL);
    const uint32_t lenG = ((pwlLengths >> 6UL) & 0x2FUL);
    const uint32_t lenB = ((pwlLengths >> 12UL) & 0x2FUL);

    if ((lenR <= 32UL) && (lenG <= 32UL) && (lenB <= 32UL)) {
        uint32_t offset = 0U;

        aPwlOffsetAndLenDesc->pwlOffsetR = offset;
        aPwlOffsetAndLenDesc->pwlLengthR = lenR;
        offset += lenR;

        aPwlOffsetAndLenDesc->pwlOffsetG = offset;
        aPwlOffsetAndLenDesc->pwlLengthG = lenG;
        offset += lenG;

        aPwlOffsetAndLenDesc->pwlOffsetB = offset;
        aPwlOffsetAndLenDesc->pwlLengthB = lenB;

        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/**
    @brief False color supression configuraiton

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_FC_BlkCfg0Type {
    uint16_t magic;
    uint8_t strength1;
    uint8_t strength2;
    uint8_t filterMixWt;
    uint8_t filterDesatWt;
    uint16_t filterMixLo;
    uint16_t filterMixHi;
    uint16_t filterDesatLo;
    uint16_t filterDesatHi;
} ISPIMGV2_FC_BlkCfg0Type;

/**
    @brief Local tone mapping PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_LTM_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_LTM_PwlElementType;

/**
    @brief Local tone mapping configuraiton

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_LTM_BlkCfg0Type {
    uint16_t magic;
    uint8_t  khp;
    uint8_t  kcolor;
    uint16_t epsilonVal;
    uint32_t divSat;
    uint16_t pwlLengthsAndGfEn;
    uint32_t gaussianKernel[81UL];
    ISPIMGV2_LTM_PwlElementType pwlElements[64UL];
} ISPIMGV2_LTM_BlkCfg0Type;

/**
    @brief Helper type for PWL offset computations corresponding to the
           LTM configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_LTM_BlkCfg0PwlOffsetAndLengthDescType {
    uint8_t  gfEn;
    uint32_t pwlOffsetBgm;
    uint32_t pwlLengthBgm;
    uint32_t pwlOffsetDgm;
    uint32_t pwlLengthDgm;
} ISPIMGV2_LTM_BlkCfg0PwlOffsetAndLengthDescType;

/** @brief Helper function to compute the PWL offsets corresponding to the
           LTM configuration 0

    @behavior Sync, Re-entrant

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Offset computation successful.
    @retval     #BCM_ERR_INVAL_PARAMS    Offset computation went bad. The config
                                         passed in is invalid.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_LTM_BlkCfg0ComputePwlOffsetsAndLen(const ISPIMGV2_LTM_BlkCfg0Type *ltmBlkCfg0, ISPIMGV2_LTM_BlkCfg0PwlOffsetAndLengthDescType *const aPwlOffsetAndLenDesc)
{
    int32_t ret;
    const uint32_t pwlLengthsAndGfEn = ltmBlkCfg0->pwlLengthsAndGfEn;
    const uint32_t gfEn = ((pwlLengthsAndGfEn >> 13UL) & 0x1UL);
    const uint16_t pwlLengthBgm = ((ltmBlkCfg0->pwlLengthsAndGfEn & 0x3FU));
    const uint16_t pwlLengthDgm = ((ltmBlkCfg0->pwlLengthsAndGfEn >> 6U) & 0x1FU);

    aPwlOffsetAndLenDesc->gfEn = (uint8_t)gfEn;
    aPwlOffsetAndLenDesc->pwlLengthBgm = (uint32_t)pwlLengthBgm;
    aPwlOffsetAndLenDesc->pwlLengthDgm = (uint32_t)pwlLengthDgm;

    if ((aPwlOffsetAndLenDesc->pwlLengthBgm <= 32UL) && (aPwlOffsetAndLenDesc->pwlLengthDgm <= 32UL)) {

        aPwlOffsetAndLenDesc->pwlOffsetBgm = 0UL;
        aPwlOffsetAndLenDesc->pwlOffsetDgm = aPwlOffsetAndLenDesc->pwlLengthBgm;
        ret = BCM_ERR_OK;

    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/**
    @brief Tone mapping PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_TM_PwlElementType {
    uint16_t abs;
    uint16_t ord;
} ISPIMGV2_TM_PwlElementType;

/**
    @brief Tone mapping configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_TM_BlkCfg0Type {
    uint16_t magic;
    uint8_t pwlLength;
    ISPIMGV2_TM_PwlElementType pwlElements[32UL];
} ISPIMGV2_TM_BlkCfg0Type;

/**
    @brief Color processing PWL element type

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_CP_PwlElementType {
    int16_t  abs;
    uint16_t ord;
} ISPIMGV2_CP_PwlElementType;

/**
    @brief Color processing configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_CP_BlkCfg0Type {
    uint16_t magic;
    uint8_t pwlLengths;
    ISPIMGV2_CP_PwlElementType pwlElements[16UL];
} ISPIMGV2_CP_BlkCfg0Type;

/**
    @brief Helper type for PWL offset computations corresponding to the
           CP configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_CP_BlkCfg0PwlOffsetAndLengthDescType {
    uint32_t pwlOffsetU;
    uint32_t pwlLengthU;
    uint32_t pwlOffsetV;
    uint32_t pwlLengthV;
} ISPIMGV2_CP_BlkCfg0PwlOffsetAndLengthDescType;

/** @brief Helper function to compute the PWL offsets corresponding to the
           CP configuration 0

    @behavior Sync, Re-entrant

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK              Offset computation successful.
    @retval     #BCM_ERR_INVAL_PARAMS    Offset computation went bad. The config
                                         passed in is invalid.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
static inline int32_t ISPIMGV2_CP_BlkCfg0ComputePwlOffsetsAndLen(const ISPIMGV2_CP_BlkCfg0Type *const aCpBlkCfg0, ISPIMGV2_CP_BlkCfg0PwlOffsetAndLengthDescType *const aPwlOffsetAndLenDesc)
{
    int32_t ret;
    const uint32_t pwlLengths = aCpBlkCfg0->pwlLengths;

    const uint32_t lenU = (pwlLengths & 0xFUL);
    const uint32_t lenV = ((pwlLengths >> 4UL) & 0xFUL);

    if ((lenU <= 8UL) && (lenV <= 8UL)) {
        uint32_t offset = 0U;

        aPwlOffsetAndLenDesc->pwlOffsetU = offset;
        aPwlOffsetAndLenDesc->pwlLengthU = lenU;
        offset += lenU;

        aPwlOffsetAndLenDesc->pwlOffsetV = offset;
        aPwlOffsetAndLenDesc->pwlLengthV = lenV;

        ret = BCM_ERR_OK;
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

/**
    @brief Color conversion configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_CC_BlkCfg0Type {
    uint16_t magic;
    int16_t matrix[9U];
    int32_t offsets[3U];
} ISPIMGV2_CC_BlkCfg0Type;

/**
    @brief Low resolution resize configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_LR_BlkCfg0Type {
    uint16_t magic;
    uint16_t frameWidth;
    uint16_t frameHeight;
    uint32_t outputWidth;
    uint32_t outputHeight;
    uint8_t  shiftAndRbSwap;
} ISPIMGV2_LR_BlkCfg0Type;

/**
    @brief Macro to extract the RB swap

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_LR_BLKCFG0_GET_RB_SWAP(lrBlkCfg0)  \
    (uint8_t)(((lrBlkCfg0)->shiftAndRbSwap & 0x10UL) >> 4UL)

/**
    @brief Macro to extract shift from LR config

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_LR_BLKCFG0_GET_SHIFT(lrBlkCfg0)    \
    (uint8_t)(((lrBlkCfg0)->shiftAndRbSwap & 0x0FUL))

/**
    @brief Output configuration

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
typedef struct COMP_PACKED _ISPIMGV2_Output_BlkCfg0Type {
    uint16_t magic;
    uint16_t misc;
    int32_t  stride1;
    int32_t  stride2;
} ISPIMGV2_Output_BlkCfg0Type;

/**
    @brief Macro to extract the VFLIP config

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_OUTPUT_BLKCFG0_GET_VFLIP(blkCfg0)          \
    ((uint8_t)((blkCfg0)->misc & 0x1U))

/**
    @brief Macro to extract the big endian config

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_OUTPUT_BLKCFG0_GET_BIG_ENDIAN(blkCfg0)     \
    ((uint8_t)(((blkCfg0)->misc >> 1U) & 0x1U))

/**
    @brief Macro to extract the output width configuratin

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/

#define ISPIMGV2_OUTPUT_BLKCFG0_GET_OUTPUT_WIDTH(blkCfg0)   \
    ((uint8_t)(((blkCfg0)->misc >> 2U) & 0x1U))


/**
    @brief Macro to extract the saturation depth

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_OUTPUT_BLKCFG0_GET_SAT_DEPTH(blkCfg0)      \
    ((uint8_t)(((blkCfg0)->misc >> 3U) & 0x7U))

/**
    @brief Macro to extract the output shift

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_OUTPUT_BLKCFG0_GET_SHIFT(blkCfg0)          \
    ((uint8_t)(((blkCfg0)->misc >> 6U) & 0xFU))

/**
    @brief Macro to extract the output data format

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_OUTPUT_BLKCFG0_GET_DATA_FORMAT(blkCfg0)    \
    ((uint8_t)(((blkCfg0)->misc >> 10U) & 0x7U))

/**
    @brief Macro describing the magic that is embedded in the ISP configuration image.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
#define ISPIMGV2_MAGIC_V2                   (0xABCE1234UL)

/**
    @brief API to parse the incoming ISP image and configure the ISP HW.

    @trace #BRCM_SWREQ_ISPIMGV2_CONFIGURATION
*/
int32_t ISPIMGV2_Configure(const struct _ISPIMGV2_ConfigImageType *const aIspImg,
                           uint8_t* aScratchMem, uint32_t aScratchMemSize);

#endif /* ISP_IMAGE_V2_H */
/** @} */
