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
    @file cmp_rdb.h
    @brief RDB File for CMP

    @version 2018May25_rdb
*/

#ifndef CMP_RDB_H
#define CMP_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t CMP_REVISION_TYPE;
#define CMP_REVISION_MAJOR_MASK (0xfff00UL)
#define CMP_REVISION_MAJOR_SHIFT (8UL)
#define CMP_REVISION_MINOR_MASK (0xffUL)
#define CMP_REVISION_MINOR_SHIFT (0UL)




typedef uint32_t CMP_CONFIGURATION_TYPE;
#define CMP_CONFIGURATION_V0_MA_CSC_PRESENT_MASK (0x20000UL)
#define CMP_CONFIGURATION_V0_MA_CSC_PRESENT_SHIFT (17UL)
#define CMP_CONFIGURATION_V0_NL_LUT_PRESENT_MASK (0x10000UL)
#define CMP_CONFIGURATION_V0_NL_LUT_PRESENT_SHIFT (16UL)
#define CMP_CONFIGURATION_SUPPORTS_4K_422_MASK (0x4000UL)
#define CMP_CONFIGURATION_SUPPORTS_4K_422_SHIFT (14UL)
#define CMP_CONFIGURATION_CORE_BVB_WIDTH_10_MASK (0x2000UL)
#define CMP_CONFIGURATION_CORE_BVB_WIDTH_10_SHIFT (13UL)
#define CMP_CONFIGURATION_PROC_CLK_IS_2X_MASK (0x1000UL)
#define CMP_CONFIGURATION_PROC_CLK_IS_2X_SHIFT (12UL)
#define CMP_CONFIGURATION_BOND_HD_ENABLE_MASK (0x400UL)
#define CMP_CONFIGURATION_BOND_HD_ENABLE_SHIFT (10UL)
#define CMP_CONFIGURATION_MOSAIC_PRESENT_MASK (0x200UL)
#define CMP_CONFIGURATION_MOSAIC_PRESENT_SHIFT (9UL)
#define CMP_CONFIGURATION_PEP_PRESENT_MASK (0x100UL)
#define CMP_CONFIGURATION_PEP_PRESENT_SHIFT (8UL)
#define CMP_CONFIGURATION_TN2TH_PRESENT_MASK (0x80UL)
#define CMP_CONFIGURATION_TN2TH_PRESENT_SHIFT (7UL)
#define CMP_CONFIGURATION_MASK_PRESENT_MASK (0x40UL)
#define CMP_CONFIGURATION_MASK_PRESENT_SHIFT (6UL)
#define CMP_CONFIGURATION_VIDEO_CONFIGURATION_MASK (0x38UL)
#define CMP_CONFIGURATION_VIDEO_CONFIGURATION_SHIFT (3UL)
#define CMP_CONFIGURATION_VIDEO_FORMAT_MASK (0x7UL)
#define CMP_CONFIGURATION_VIDEO_FORMAT_SHIFT (0UL)




typedef uint32_t CMP_CANVAS_CTRL_TYPE;
#define CMP_CANVAS_CTRL_ENABLE_CTRL_MASK (0x2UL)
#define CMP_CANVAS_CTRL_ENABLE_CTRL_SHIFT (1UL)
#define CMP_CANVAS_CTRL_ENABLE_MASK (0x1UL)
#define CMP_CANVAS_CTRL_ENABLE_SHIFT (0UL)




typedef uint32_t CMP_CANVAS_SIZE_TYPE;
#define CMP_CANVAS_SIZE_HSIZE_MASK (0x1fff0000UL)
#define CMP_CANVAS_SIZE_HSIZE_SHIFT (16UL)
#define CMP_CANVAS_SIZE_VSIZE_MASK (0xfffUL)
#define CMP_CANVAS_SIZE_VSIZE_SHIFT (0UL)




typedef uint32_t CMP_BG_COLOR_TYPE;
#define CMP_BG_COLOR_Y_MASK (0xff0000UL)
#define CMP_BG_COLOR_Y_SHIFT (16UL)
#define CMP_BG_COLOR_CB_MASK (0xff00UL)
#define CMP_BG_COLOR_CB_SHIFT (8UL)
#define CMP_BG_COLOR_CR_MASK (0xffUL)
#define CMP_BG_COLOR_CR_SHIFT (0UL)




typedef uint32_t CMP_BLEND_CTRL_TYPE;
#define CMP_BLEND_CTRL_BLEND_SOURCE_MASK (0xf0000UL)
#define CMP_BLEND_CTRL_BLEND_SOURCE_SHIFT (16UL)
#define CMP_BLEND_CTRL_CONSTANT_ALPHA_MASK (0xff00UL)
#define CMP_BLEND_CTRL_CONSTANT_ALPHA_SHIFT (8UL)
#define CMP_BLEND_CTRL_BACK_COLOR_BLEND_FACTOR_MASK (0x70UL)
#define CMP_BLEND_CTRL_BACK_COLOR_BLEND_FACTOR_SHIFT (4UL)
#define CMP_BLEND_CTRL_FRONT_COLOR_BLEND_FACTOR_MASK (0x7UL)
#define CMP_BLEND_CTRL_FRONT_COLOR_BLEND_FACTOR_SHIFT (0UL)




typedef uint8_t CMP_RESERVED_TYPE;




typedef uint32_t CMP_CTRL_TYPE;
#define CMP_CTRL_BVB_VIDEO_MASK (0x3UL)
#define CMP_CTRL_BVB_VIDEO_SHIFT (0UL)




typedef uint32_t CMP_OUT_CTRL_TYPE;
#define CMP_OUT_CTRL_OUT_TO_DS_CTRL_MASK (0x200UL)
#define CMP_OUT_CTRL_OUT_TO_DS_CTRL_SHIFT (9UL)
#define CMP_OUT_CTRL_OUT_TO_VEC_CTRL_MASK (0x100UL)
#define CMP_OUT_CTRL_OUT_TO_VEC_CTRL_SHIFT (8UL)
#define CMP_OUT_CTRL_CLIP_CTRL_MASK (0x1UL)
#define CMP_OUT_CTRL_CLIP_CTRL_SHIFT (0UL)




typedef uint32_t CMP_CRC_CONTROL_TYPE;
#define CMP_CRC_CONTROL_INIT_VALUE_MASK (0xffff0000UL)
#define CMP_CRC_CONTROL_INIT_VALUE_SHIFT (16UL)
#define CMP_CRC_CONTROL_MODE_8BIT_MASK (0x100UL)
#define CMP_CRC_CONTROL_MODE_8BIT_SHIFT (8UL)
#define CMP_CRC_CONTROL_PROBE_RATE_MASK (0xf0UL)
#define CMP_CRC_CONTROL_PROBE_RATE_SHIFT (4UL)
#define CMP_CRC_CONTROL_CLEAR_MASK (0x2UL)
#define CMP_CRC_CONTROL_CLEAR_SHIFT (1UL)
#define CMP_CRC_CONTROL_ENABLE_MASK (0x1UL)
#define CMP_CRC_CONTROL_ENABLE_SHIFT (0UL)




typedef uint32_t CMP_CRC_STATUS_TYPE;
#define CMP_CRC_STATUS_VALUE_MASK (0xffffUL)
#define CMP_CRC_STATUS_VALUE_SHIFT (0UL)




typedef uint32_t CMP_READBACK_POSITION_TYPE;
#define CMP_READBACK_POSITION_Y_POS_MASK (0xfff0000UL)
#define CMP_READBACK_POSITION_Y_POS_SHIFT (16UL)
#define CMP_READBACK_POSITION_X_POS_MASK (0x1fffUL)
#define CMP_READBACK_POSITION_X_POS_SHIFT (0UL)




typedef uint32_t CMP_READBACK_VALUE_TYPE;
#define CMP_READBACK_VALUE_VALUE_MASK (0x3fffffffUL)
#define CMP_READBACK_VALUE_VALUE_SHIFT (0UL)




typedef uint32_t CMP_SCRATCH_TYPE;
#define CMP_SCRATCH_VALUE_MASK (0xffffffffUL)
#define CMP_SCRATCH_VALUE_SHIFT (0UL)




typedef uint32_t CMP_MISC_TEST_TYPE;
#define CMP_MISC_TEST_DIS_CLK_GATE_MASK (0x80UL)
#define CMP_MISC_TEST_DIS_CLK_GATE_SHIFT (7UL)
#define CMP_MISC_TEST_SEG_RUL_DISABLE_MASK (0x40UL)
#define CMP_MISC_TEST_SEG_RUL_DISABLE_SHIFT (6UL)
#define CMP_MISC_TEST_CTRL_CLK_GATE_CST_MASK (0x20UL)
#define CMP_MISC_TEST_CTRL_CLK_GATE_CST_SHIFT (5UL)
#define CMP_MISC_TEST_CTRL_SURF_NO_DELAY_MASK (0x10UL)
#define CMP_MISC_TEST_CTRL_SURF_NO_DELAY_SHIFT (4UL)
#define CMP_MISC_TEST_CTRL_SURF_ON_DIS_MASK (0x8UL)
#define CMP_MISC_TEST_CTRL_SURF_ON_DIS_SHIFT (3UL)




typedef uint32_t CMP_TEST_REG_TYPE;
#define CMP_TEST_REG_CMP_OUT_VCNT_MASK (0xfff0000UL)
#define CMP_TEST_REG_CMP_OUT_VCNT_SHIFT (16UL)
#define CMP_TEST_REG_CLK_STATE_MASK (0x700UL)
#define CMP_TEST_REG_CLK_STATE_SHIFT (8UL)
#define CMP_TEST_REG_CMP_I_ACCEPT_MASK (0x80UL)
#define CMP_TEST_REG_CMP_I_ACCEPT_SHIFT (7UL)
#define CMP_TEST_REG_CMP_O_READY_MASK (0x40UL)
#define CMP_TEST_REG_CMP_O_READY_SHIFT (6UL)
#define CMP_TEST_REG_G0_O_ACCEPT_MASK (0x20UL)
#define CMP_TEST_REG_G0_O_ACCEPT_SHIFT (5UL)
#define CMP_TEST_REG_G0_I_READY_MASK (0x10UL)
#define CMP_TEST_REG_G0_I_READY_SHIFT (4UL)
#define CMP_TEST_REG_V0_O_ACCEPT_MASK (0x2UL)
#define CMP_TEST_REG_V0_O_ACCEPT_SHIFT (1UL)
#define CMP_TEST_REG_V0_I_READY_MASK (0x1UL)
#define CMP_TEST_REG_V0_I_READY_SHIFT (0UL)




typedef uint32_t CMP_STATUS_CLEAR_TYPE;
#define CMP_STATUS_CLEAR_ENABLE_ERROR_MASK (0x1UL)
#define CMP_STATUS_CLEAR_ENABLE_ERROR_SHIFT (0UL)




typedef uint32_t CMP_STATUS_TYPE;
#define CMP_STATUS_ENABLE_ERROR_MASK (0x1UL)
#define CMP_STATUS_ENABLE_ERROR_SHIFT (0UL)




typedef uint32_t CMP_SUR_SIZE_TYPE;
#define CMP_SUR_SIZE_HSIZE_MASK (0x1fff0000UL)
#define CMP_SUR_SIZE_HSIZE_SHIFT (16UL)
#define CMP_SUR_SIZE_VSIZE_MASK (0xfffUL)
#define CMP_SUR_SIZE_VSIZE_SHIFT (0UL)




typedef uint32_t CMP_SUR_OFFSET_TYPE;
#define CMP_SUR_OFFSET_X_OFFSET_MASK (0x1fff0000UL)
#define CMP_SUR_OFFSET_X_OFFSET_SHIFT (16UL)
#define CMP_SUR_OFFSET_Y_OFFSET_MASK (0xfffUL)
#define CMP_SUR_OFFSET_Y_OFFSET_SHIFT (0UL)




typedef uint32_t CMP_SUR_DISPLAY_SIZE_TYPE;
#define CMP_SUR_DISPLAY_SIZE_HSIZE_MASK (0x1fff0000UL)
#define CMP_SUR_DISPLAY_SIZE_HSIZE_SHIFT (16UL)
#define CMP_SUR_DISPLAY_SIZE_VSIZE_MASK (0xfffUL)
#define CMP_SUR_DISPLAY_SIZE_VSIZE_SHIFT (0UL)




typedef uint32_t CMP_SUR_CANVAS_OFFSET_TYPE;
#define CMP_SUR_CANVAS_OFFSET_X_OFFSET_MASK (0x1fff0000UL)
#define CMP_SUR_CANVAS_OFFSET_X_OFFSET_SHIFT (16UL)
#define CMP_SUR_CANVAS_OFFSET_Y_OFFSET_MASK (0xfffUL)
#define CMP_SUR_CANVAS_OFFSET_Y_OFFSET_SHIFT (0UL)




typedef uint32_t CMP_SUR_CANVAS_X_OFFSET_R_TYPE;
#define CMP_SUR_CANVAS_X_OFFSET_R_OFFSET_MASK (0x1fffUL)
#define CMP_SUR_CANVAS_X_OFFSET_R_OFFSET_SHIFT (0UL)




typedef uint32_t CMP_GFX_SUR_CTRL_TYPE;
#define CMP_GFX_SUR_CTRL_ENABLE_MASK (0x1UL)
#define CMP_GFX_SUR_CTRL_ENABLE_SHIFT (0UL)




typedef uint32_t CMP_BVB_STATUS_CLEAR_TYPE;
#define CMP_BVB_STATUS_CLEAR_MISSING_SYNC_MASK (0x10UL)
#define CMP_BVB_STATUS_CLEAR_MISSING_SYNC_SHIFT (4UL)
#define CMP_BVB_STATUS_CLEAR_LONG_SOURCE_MASK (0x8UL)
#define CMP_BVB_STATUS_CLEAR_LONG_SOURCE_SHIFT (3UL)
#define CMP_BVB_STATUS_CLEAR_SHORT_SOURCE_MASK (0x4UL)
#define CMP_BVB_STATUS_CLEAR_SHORT_SOURCE_SHIFT (2UL)
#define CMP_BVB_STATUS_CLEAR_LONG_LINE_MASK (0x2UL)
#define CMP_BVB_STATUS_CLEAR_LONG_LINE_SHIFT (1UL)
#define CMP_BVB_STATUS_CLEAR_SHORT_LINE_MASK (0x1UL)
#define CMP_BVB_STATUS_CLEAR_SHORT_LINE_SHIFT (0UL)




typedef uint32_t CMP_BVB_STATUS_TYPE;
#define CMP_BVB_STATUS_MISSING_SYNC_MASK (0x10UL)
#define CMP_BVB_STATUS_MISSING_SYNC_SHIFT (4UL)
#define CMP_BVB_STATUS_LONG_SOURCE_MASK (0x8UL)
#define CMP_BVB_STATUS_LONG_SOURCE_SHIFT (3UL)
#define CMP_BVB_STATUS_SHORT_SOURCE_MASK (0x4UL)
#define CMP_BVB_STATUS_SHORT_SOURCE_SHIFT (2UL)
#define CMP_BVB_STATUS_LONG_LINE_MASK (0x2UL)
#define CMP_BVB_STATUS_LONG_LINE_SHIFT (1UL)
#define CMP_BVB_STATUS_SHORT_LINE_MASK (0x1UL)
#define CMP_BVB_STATUS_SHORT_LINE_SHIFT (0UL)




typedef uint32_t CMP_V0_SUR_CTRL_NO_CSC_TYPE;
#define CMP_V0_SUR_CTRL_NO_CSC_CONST_COLOR_ENABLE_MASK (0x10000UL)
#define CMP_V0_SUR_CTRL_NO_CSC_CONST_COLOR_ENABLE_SHIFT (16UL)
#define CMP_V0_SUR_CTRL_NO_CSC_VWIN_ALPHA_MASK (0xff00UL)
#define CMP_V0_SUR_CTRL_NO_CSC_VWIN_ALPHA_SHIFT (8UL)
#define CMP_V0_SUR_CTRL_NO_CSC_DERING_EN_MASK (0x40UL)
#define CMP_V0_SUR_CTRL_NO_CSC_DERING_EN_SHIFT (6UL)
#define CMP_V0_SUR_CTRL_NO_CSC_UNBIASED_ROUND_ENABLE_MASK (0x20UL)
#define CMP_V0_SUR_CTRL_NO_CSC_UNBIASED_ROUND_ENABLE_SHIFT (5UL)
#define CMP_V0_SUR_CTRL_NO_CSC_FILT_CTRL_MASK (0x6UL)
#define CMP_V0_SUR_CTRL_NO_CSC_FILT_CTRL_SHIFT (1UL)
#define CMP_V0_SUR_CTRL_NO_CSC_SURFACE_ENABLE_MASK (0x1UL)
#define CMP_V0_SUR_CTRL_NO_CSC_SURFACE_ENABLE_SHIFT (0UL)




typedef uint32_t CMP_CONST_COLOR_TYPE;
#define CMP_CONST_COLOR_Y_MASK (0xff0000UL)
#define CMP_CONST_COLOR_Y_SHIFT (16UL)
#define CMP_CONST_COLOR_CB_MASK (0xff00UL)
#define CMP_CONST_COLOR_CB_SHIFT (8UL)
#define CMP_CONST_COLOR_CR_MASK (0xffUL)
#define CMP_CONST_COLOR_CR_SHIFT (0UL)




typedef uint32_t CMP_SUR_CHROMA_KEYING_TYPE;
#define CMP_SUR_CHROMA_KEYING_ENABLE_MASK (0x80000000UL)
#define CMP_SUR_CHROMA_KEYING_ENABLE_SHIFT (31UL)
#define CMP_SUR_CHROMA_KEYING_C_MASK_MASK (0xff0000UL)
#define CMP_SUR_CHROMA_KEYING_C_MASK_SHIFT (16UL)
#define CMP_SUR_CHROMA_KEYING_C_HIGH_MASK (0xff00UL)
#define CMP_SUR_CHROMA_KEYING_C_HIGH_SHIFT (8UL)
#define CMP_SUR_CHROMA_KEYING_C_LOW_MASK (0xffUL)
#define CMP_SUR_CHROMA_KEYING_C_LOW_SHIFT (0UL)




typedef uint32_t CMP_SUR_LUMA_KEYING_TYPE;
#define CMP_SUR_LUMA_KEYING_ENABLE_MASK (0x80000000UL)
#define CMP_SUR_LUMA_KEYING_ENABLE_SHIFT (31UL)
#define CMP_SUR_LUMA_KEYING_Y_MASK_MASK (0xff0000UL)
#define CMP_SUR_LUMA_KEYING_Y_MASK_SHIFT (16UL)
#define CMP_SUR_LUMA_KEYING_Y_HIGH_MASK (0xff00UL)
#define CMP_SUR_LUMA_KEYING_Y_HIGH_SHIFT (8UL)
#define CMP_SUR_LUMA_KEYING_Y_LOW_MASK (0xffUL)
#define CMP_SUR_LUMA_KEYING_Y_LOW_SHIFT (0UL)




typedef volatile struct COMP_PACKED _CMP_RDBType {
    CMP_REVISION_TYPE revision; /* OFFSET: 0x0 */
    CMP_CONFIGURATION_TYPE hw_configuration; /* OFFSET: 0x4 */
    CMP_CANVAS_CTRL_TYPE canvas_ctrl; /* OFFSET: 0x8 */
    CMP_CANVAS_SIZE_TYPE canvas_size; /* OFFSET: 0xc */
    CMP_BG_COLOR_TYPE bg_color; /* OFFSET: 0x10 */
    CMP_BLEND_CTRL_TYPE blend_0_ctrl; /* OFFSET: 0x14 */
    CMP_BLEND_CTRL_TYPE blend_1_ctrl; /* OFFSET: 0x18 */
    CMP_RESERVED_TYPE rsvd0[12]; /* OFFSET: 0x1c */
    CMP_CTRL_TYPE cmp_ctrl; /* OFFSET: 0x28 */
    CMP_RESERVED_TYPE rsvd1[20]; /* OFFSET: 0x2c */
    CMP_OUT_CTRL_TYPE cmp_out_ctrl; /* OFFSET: 0x40 */
    CMP_CRC_CONTROL_TYPE crc_ctrl; /* OFFSET: 0x44 */
    CMP_CRC_STATUS_TYPE crc_y_status; /* OFFSET: 0x48 */
    CMP_CRC_STATUS_TYPE crc_cb_status; /* OFFSET: 0x4c */
    CMP_CRC_STATUS_TYPE crc_cr_status; /* OFFSET: 0x50 */
    CMP_READBACK_POSITION_TYPE readback_position; /* OFFSET: 0x54 */
    CMP_READBACK_VALUE_TYPE readback_value; /* OFFSET: 0x58 */
    CMP_RESERVED_TYPE rsvd2[4]; /* OFFSET: 0x5c */
    CMP_SCRATCH_TYPE scratch_register; /* OFFSET: 0x60 */
    CMP_RESERVED_TYPE rsvd3[4]; /* OFFSET: 0x64 */
    CMP_MISC_TEST_TYPE test_register; /* OFFSET: 0x68 */
    CMP_TEST_REG_TYPE test_register1; /* OFFSET: 0x6c */
    CMP_RESERVED_TYPE rsvd4[12]; /* OFFSET: 0x70 */
    CMP_STATUS_CLEAR_TYPE cmp_status_clear; /* OFFSET: 0x7c */
    CMP_STATUS_TYPE cmp_status; /* OFFSET: 0x80 */
    CMP_RESERVED_TYPE rsvd5[252]; /* OFFSET: 0x84 */
    CMP_SUR_SIZE_TYPE g0_surface_size; /* OFFSET: 0x180 */
    CMP_SUR_OFFSET_TYPE g0_surface_offset; /* OFFSET: 0x184 */
    CMP_SUR_DISPLAY_SIZE_TYPE g0_display_size; /* OFFSET: 0x188 */
    CMP_SUR_CANVAS_OFFSET_TYPE g0_canvas_offset; /* OFFSET: 0x18c */
    CMP_SUR_CANVAS_X_OFFSET_R_TYPE g0_canvas_x_offset_r; /* OFFSET: 0x190 */
    CMP_GFX_SUR_CTRL_TYPE g0_surface_ctrl; /* OFFSET: 0x194 */
    CMP_BVB_STATUS_CLEAR_TYPE g0_bvb_in_status_clear; /* OFFSET: 0x198 */
    CMP_BVB_STATUS_TYPE g0_bvb_in_status; /* OFFSET: 0x19c */
    CMP_RESERVED_TYPE rsvd6[96]; /* OFFSET: 0x1a0 */
    CMP_SUR_SIZE_TYPE v0_surface_size; /* OFFSET: 0x200 */
    CMP_SUR_OFFSET_TYPE v0_surface_offset; /* OFFSET: 0x204 */
    CMP_SUR_DISPLAY_SIZE_TYPE v0_display_size; /* OFFSET: 0x208 */
    CMP_SUR_CANVAS_OFFSET_TYPE v0_canvas_offset; /* OFFSET: 0x20c */
    CMP_SUR_CANVAS_X_OFFSET_R_TYPE v0_canvas_x_offset_r; /* OFFSET: 0x210 */
    CMP_V0_SUR_CTRL_NO_CSC_TYPE v0_surface_ctrl; /* OFFSET: 0x214 */
    CMP_BVB_STATUS_CLEAR_TYPE v0_bvb_in_status_clear; /* OFFSET: 0x218 */
    CMP_BVB_STATUS_TYPE v0_bvb_in_status; /* OFFSET: 0x21c */
    CMP_CONST_COLOR_TYPE v0_const_color; /* OFFSET: 0x220 */
    CMP_SUR_CHROMA_KEYING_TYPE v0_cb_keying; /* OFFSET: 0x224 */
    CMP_SUR_CHROMA_KEYING_TYPE v0_cr_keying; /* OFFSET: 0x228 */
    CMP_SUR_LUMA_KEYING_TYPE v0_luma_keying; /* OFFSET: 0x22c */
} CMP_RDBType;


#define CMP_0_BASE                      (0x400A0600UL)



#define CMP_MAX_HW_ID                   (1UL)


#define CMP_BLEND_0_CTRL_FRONT_ZERO     (0UL)


#define CMP_BLEND_0_CTRL_FRONT_ONE      (1UL)


#define CMP_BLEND_0_CTRL_FRONT_FRONT_ALPHA  (2UL)


#define CMP_BLEND_0_CTRL_FRONT_ONE_MINUS_FRONT_ALPHA  (3UL)


#define CMP_BLEND_0_CTRL_FRONT_CONSTANT_ALPHA  (4UL)


#define CMP_BLEND_0_CTRL_FRONT_ONE_MINUS_CONSTANT_ALPHA  (5UL)


#define CMP_BLEND_0_CTRL_SOURCE_SURFACE_V0  (0UL)


#define CMP_BLEND_0_CTRL_SOURCE_BACKGROUND_BYPASS  (15UL)


#define CMP_BLEND_0_CTRL_BACK_ONE       (1UL)


#define CMP_BLEND_1_CTRL_SOURCE_SURFACE_G0  (2UL)


#define CMP_BLEND_1_CTRL_SOURCE_BACKGROUND_BYPASS  (15UL)


#define CMP_BLEND_1_CTRL_BACK_ONE       (1UL)


#define CMP_BLEND_1_CTRL_FRONT_ZERO     (0UL)


#define CMP_BVB_STATUS_RSVD0_MASK       (0xffffffe0UL)


#define CMP_OUT_CTRL_OUT_TO_DS_CTRL_ENABLE  (1UL)


#define CMP_OUT_CTRL_OUT_TO_VEC_CTRL_ENABLE  (1UL)


#define CMP_OUT_CTRL_CLIP_CTRL_ENABLE   (1UL)

#endif /* CMP_RDB_H */