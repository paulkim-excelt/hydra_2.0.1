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
    @file gfd_rdb.h
    @brief RDB File for GFD

    @version 2018May25_rdb
*/

#ifndef GFD_RDB_H
#define GFD_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t GFD_REVISION_TYPE;
#define GFD_REVISION_MAJOR_MASK (0xff00UL)
#define GFD_REVISION_MAJOR_SHIFT (8UL)
#define GFD_REVISION_MINOR_MASK (0xffUL)
#define GFD_REVISION_MINOR_SHIFT (0UL)




typedef uint32_t GFD_CONFIGURATION_TYPE;
#define GFD_CONFIGURATION_BSTC_MASK (0x2000000UL)
#define GFD_CONFIGURATION_BSTC_SHIFT (25UL)
#define GFD_CONFIGURATION_NL_MA_CSC_PRESENT_MASK (0x1000000UL)
#define GFD_CONFIGURATION_NL_MA_CSC_PRESENT_SHIFT (24UL)
#define GFD_CONFIGURATION_NL_LUT_PRESENT_MASK (0x800000UL)
#define GFD_CONFIGURATION_NL_LUT_PRESENT_SHIFT (23UL)
#define GFD_CONFIGURATION_DCXG_MASK (0x400000UL)
#define GFD_CONFIGURATION_DCXG_SHIFT (22UL)
#define GFD_CONFIGURATION_SUPPORTS_4K_MASK (0x200000UL)
#define GFD_CONFIGURATION_SUPPORTS_4K_SHIFT (21UL)
#define GFD_CONFIGURATION_BVB_CLOCK_IS_2X_MASK (0x100000UL)
#define GFD_CONFIGURATION_BVB_CLOCK_IS_2X_SHIFT (20UL)
#define GFD_CONFIGURATION_VSCL_LSBF_SIZE_MASK (0x7ff80UL)
#define GFD_CONFIGURATION_VSCL_LSBF_SIZE_SHIFT (7UL)
#define GFD_CONFIGURATION_GC_MASK (0x4UL)
#define GFD_CONFIGURATION_GC_SHIFT (2UL)
#define GFD_CONFIGURATION_HSCL_MASK (0x2UL)
#define GFD_CONFIGURATION_HSCL_SHIFT (1UL)
#define GFD_CONFIGURATION_VSCL_MASK (0x1UL)
#define GFD_CONFIGURATION_VSCL_SHIFT (0UL)




typedef uint32_t GFD_ENABLE_TYPE;
#define GFD_ENABLE_CTRL_MASK (0x2UL)
#define GFD_ENABLE_CTRL_SHIFT (1UL)
#define GFD_ENABLE_ENABLE_MASK (0x1UL)
#define GFD_ENABLE_ENABLE_SHIFT (0UL)




typedef uint32_t GFD_LOAD_PALETTE_TYPE;
#define GFD_LOAD_PALETTE_PALETTE_MASK (0x1UL)
#define GFD_LOAD_PALETTE_PALETTE_SHIFT (0UL)




typedef uint32_t GFD_CTRL_TYPE;
#define GFD_CTRL_SCRATCH_BIT_1_MASK (0x2000UL)
#define GFD_CTRL_SCRATCH_BIT_1_SHIFT (13UL)
#define GFD_CTRL_SCRATCH_BIT_0_MASK (0x1000UL)
#define GFD_CTRL_SCRATCH_BIT_0_SHIFT (12UL)
#define GFD_CTRL_MEM_VIDEO_MASK (0xc00UL)
#define GFD_CTRL_MEM_VIDEO_SHIFT (10UL)
#define GFD_CTRL_BVB_VIDEO_MASK (0x300UL)
#define GFD_CTRL_BVB_VIDEO_SHIFT (8UL)
#define GFD_CTRL_MASK_HSCL_LONG_LINE_MASK (0x80UL)
#define GFD_CTRL_MASK_HSCL_LONG_LINE_SHIFT (7UL)
#define GFD_CTRL_MASK_HSCL_SHORT_LINE_MASK (0x40UL)
#define GFD_CTRL_MASK_HSCL_SHORT_LINE_SHIFT (6UL)
#define GFD_CTRL_CLUT_SCALE_MODE_MASK (0x20UL)
#define GFD_CTRL_CLUT_SCALE_MODE_SHIFT (5UL)
#define GFD_CTRL_CSC_ENABLE_MASK (0x10UL)
#define GFD_CTRL_CSC_ENABLE_SHIFT (4UL)
#define GFD_CTRL_GC_ENABLE_MASK (0x8UL)
#define GFD_CTRL_GC_ENABLE_SHIFT (3UL)
#define GFD_CTRL_HFIR_ENABLE_MASK (0x4UL)
#define GFD_CTRL_HFIR_ENABLE_SHIFT (2UL)
#define GFD_CTRL_ALPHA_PRE_MULT_MASK (0x2UL)
#define GFD_CTRL_ALPHA_PRE_MULT_SHIFT (1UL)
#define GFD_CTRL_COLOR_KEY_ENABLE_MASK (0x1UL)
#define GFD_CTRL_COLOR_KEY_ENABLE_SHIFT (0UL)




typedef uint32_t GFD_FORMAT_DEF_1_TYPE;
#define GFD_FORMAT_DEF_1_FORMAT_TYPE_MASK (0x70000UL)
#define GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT (16UL)
#define GFD_FORMAT_DEF_1_CH3_NUM_BITS_MASK (0xf000UL)
#define GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT (12UL)
#define GFD_FORMAT_DEF_1_CH2_NUM_BITS_MASK (0xf00UL)
#define GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT (8UL)
#define GFD_FORMAT_DEF_1_CH1_NUM_BITS_MASK (0xf0UL)
#define GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT (4UL)
#define GFD_FORMAT_DEF_1_CH0_NUM_BITS_MASK (0xfUL)
#define GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT (0UL)




typedef uint32_t GFD_FORMAT_DEF_2_TYPE;
#define GFD_FORMAT_DEF_2_CH3_LSB_POS_MASK (0x1f000000UL)
#define GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT (24UL)
#define GFD_FORMAT_DEF_2_CH2_LSB_POS_MASK (0x1f0000UL)
#define GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT (16UL)
#define GFD_FORMAT_DEF_2_CH1_LSB_POS_MASK (0x1f00UL)
#define GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT (8UL)
#define GFD_FORMAT_DEF_2_CH0_LSB_POS_MASK (0x1fUL)
#define GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT (0UL)




typedef uint32_t GFD_HORIZ_STEP_TYPE;
#define GFD_HORIZ_STEP_INT_MASK (0xe00000UL)
#define GFD_HORIZ_STEP_INT_SHIFT (21UL)
#define GFD_HORIZ_STEP_STEP_MASK (0x1ffff8UL)
#define GFD_HORIZ_STEP_STEP_SHIFT (3UL)




typedef uint32_t GFD_HORIZ_INIT_PHASE_TYPE;
#define GFD_HORIZ_INIT_PHASE_PHASE_MASK (0xffe000UL)
#define GFD_HORIZ_INIT_PHASE_PHASE_SHIFT (13UL)




typedef uint32_t GFD_HORIZ_INIT_PHASE_R_TYPE;
#define GFD_HORIZ_INIT_PHASE_R_PHASE_MASK (0xffe000UL)
#define GFD_HORIZ_INIT_PHASE_R_PHASE_SHIFT (13UL)




typedef uint32_t GFD_WIN_COLOR_TYPE;
#define GFD_WIN_COLOR_COLOR_MASK (0xffffffUL)
#define GFD_WIN_COLOR_COLOR_SHIFT (0UL)




typedef uint32_t GFD_W_ALPHA_TYPE;
#define GFD_W_ALPHA_W1_ALPHA_MASK (0xff0000UL)
#define GFD_W_ALPHA_W1_ALPHA_SHIFT (16UL)
#define GFD_W_ALPHA_W0_ALPHA_MASK (0xffUL)
#define GFD_W_ALPHA_W0_ALPHA_SHIFT (0UL)




typedef uint32_t GFD_KEY_MAX_TYPE;
#define GFD_KEY_MAX_Y_R_MASK (0xff0000UL)
#define GFD_KEY_MAX_Y_R_SHIFT (16UL)
#define GFD_KEY_MAX_CB_G_MASK (0xff00UL)
#define GFD_KEY_MAX_CB_G_SHIFT (8UL)
#define GFD_KEY_MAX_CR_B_MASK (0xffUL)
#define GFD_KEY_MAX_CR_B_SHIFT (0UL)




typedef uint32_t GFD_KEY_MIN_TYPE;
#define GFD_KEY_MIN_Y_R_MASK (0xff0000UL)
#define GFD_KEY_MIN_Y_R_SHIFT (16UL)
#define GFD_KEY_MIN_CB_G_MASK (0xff00UL)
#define GFD_KEY_MIN_CB_G_SHIFT (8UL)
#define GFD_KEY_MIN_CR_B_MASK (0xffUL)
#define GFD_KEY_MIN_CR_B_SHIFT (0UL)




typedef uint32_t GFD_KEY_MASK_TYPE;
#define GFD_KEY_MASK_Y_R_MASK (0xff0000UL)
#define GFD_KEY_MASK_Y_R_SHIFT (16UL)
#define GFD_KEY_MASK_CB_G_MASK (0xff00UL)
#define GFD_KEY_MASK_CB_G_SHIFT (8UL)
#define GFD_KEY_MASK_CR_B_MASK (0xffUL)
#define GFD_KEY_MASK_CR_B_SHIFT (0UL)




typedef uint32_t GFD_KEY_ALPHA_TYPE;
#define GFD_KEY_ALPHA_DEFAULT_ALPHA_MASK (0xff00UL)
#define GFD_KEY_ALPHA_DEFAULT_ALPHA_SHIFT (8UL)
#define GFD_KEY_ALPHA_KEY_ALPHA_MASK (0xffUL)
#define GFD_KEY_ALPHA_KEY_ALPHA_SHIFT (0UL)




typedef uint32_t GFD_SRC_OFFSET_TYPE;
#define GFD_SRC_OFFSET_BLANK_PIXEL_COUNT_MASK (0x7UL)
#define GFD_SRC_OFFSET_BLANK_PIXEL_COUNT_SHIFT (0UL)




typedef uint32_t GFD_SRC_HSIZE_TYPE;
#define GFD_SRC_HSIZE_HSIZE_MASK (0x1fffUL)
#define GFD_SRC_HSIZE_HSIZE_SHIFT (0UL)




typedef uint32_t GFD_SRC_START_TYPE;
#define GFD_SRC_START_ADDR_MASK (0xffffffffUL)
#define GFD_SRC_START_ADDR_SHIFT (0UL)




typedef uint32_t GFD_SRC_START_R_TYPE;
#define GFD_SRC_START_R_ADDR_MASK (0xffffffffUL)
#define GFD_SRC_START_R_ADDR_SHIFT (0UL)




typedef uint32_t GFD_SRC_PITCH_TYPE;
#define GFD_SRC_PITCH_PITCH_MASK (0xffffUL)
#define GFD_SRC_PITCH_PITCH_SHIFT (0UL)




typedef uint32_t GFD_PALETTE_START_TYPE;
#define GFD_PALETTE_START_ADDR_MASK (0xffffffffUL)
#define GFD_PALETTE_START_ADDR_SHIFT (0UL)




typedef uint32_t GFD_PALETTE_SIZE_TYPE;
#define GFD_PALETTE_SIZE_SIZE_MASK (0x1ffUL)
#define GFD_PALETTE_SIZE_SIZE_SHIFT (0UL)




typedef uint32_t GFD_DISP_PIC_SIZE_TYPE;
#define GFD_DISP_PIC_SIZE_HSIZE_MASK (0x1fff0000UL)
#define GFD_DISP_PIC_SIZE_HSIZE_SHIFT (16UL)
#define GFD_DISP_PIC_SIZE_VSIZE_MASK (0xfffUL)
#define GFD_DISP_PIC_SIZE_VSIZE_SHIFT (0UL)




typedef uint8_t GFD_RESERVED_TYPE;




typedef uint32_t GFD_CSC_COEFF_C0_TYPE;
#define GFD_CSC_COEFF_C0_C0_MASK (0xffffUL)
#define GFD_CSC_COEFF_C0_C0_SHIFT (0UL)




typedef uint32_t GFD_CSC_COEFF_C1_TYPE;
#define GFD_CSC_COEFF_C1_C1_MASK (0xffffUL)
#define GFD_CSC_COEFF_C1_C1_SHIFT (0UL)




typedef uint32_t GFD_CSC_COEFF_C2_TYPE;
#define GFD_CSC_COEFF_C2_C2_MASK (0xffffUL)
#define GFD_CSC_COEFF_C2_C2_SHIFT (0UL)




typedef uint32_t GFD_CSC_COEFF_C3_TYPE;
#define GFD_CSC_COEFF_C3_C3_MASK (0xffffUL)
#define GFD_CSC_COEFF_C3_C3_SHIFT (0UL)




typedef uint32_t GFD_CSC_COEFF_C4_TYPE;
#define GFD_CSC_COEFF_C4_C4_MASK (0xffffUL)
#define GFD_CSC_COEFF_C4_C4_SHIFT (0UL)




typedef uint32_t GFD_BVB_STATUS_CLEAR_TYPE;
#define GFD_BVB_STATUS_CLEAR_ENABLE_ERROR_MASK (0x80UL)
#define GFD_BVB_STATUS_CLEAR_ENABLE_ERROR_SHIFT (7UL)
#define GFD_BVB_STATUS_CLEAR_HSCL_LONG_LINE_MASK (0x40UL)
#define GFD_BVB_STATUS_CLEAR_HSCL_LONG_LINE_SHIFT (6UL)
#define GFD_BVB_STATUS_CLEAR_HSCL_SHORT_LINE_MASK (0x20UL)
#define GFD_BVB_STATUS_CLEAR_HSCL_SHORT_LINE_SHIFT (5UL)
#define GFD_BVB_STATUS_CLEAR_MISSING_SYNC_MASK (0x10UL)
#define GFD_BVB_STATUS_CLEAR_MISSING_SYNC_SHIFT (4UL)
#define GFD_BVB_STATUS_CLEAR_LONG_SOURCE_MASK (0x8UL)
#define GFD_BVB_STATUS_CLEAR_LONG_SOURCE_SHIFT (3UL)
#define GFD_BVB_STATUS_CLEAR_SHORT_SOURCE_MASK (0x4UL)
#define GFD_BVB_STATUS_CLEAR_SHORT_SOURCE_SHIFT (2UL)
#define GFD_BVB_STATUS_CLEAR_LONG_LINE_MASK (0x2UL)
#define GFD_BVB_STATUS_CLEAR_LONG_LINE_SHIFT (1UL)
#define GFD_BVB_STATUS_CLEAR_SHORT_LINE_MASK (0x1UL)
#define GFD_BVB_STATUS_CLEAR_SHORT_LINE_SHIFT (0UL)




typedef uint32_t GFD_BVB_STATUS_TYPE;
#define GFD_BVB_STATUS_ENABLE_ERROR_MASK (0x80UL)
#define GFD_BVB_STATUS_ENABLE_ERROR_SHIFT (7UL)
#define GFD_BVB_STATUS_HSCL_LONG_LINE_MASK (0x40UL)
#define GFD_BVB_STATUS_HSCL_LONG_LINE_SHIFT (6UL)
#define GFD_BVB_STATUS_HSCL_SHORT_LINE_MASK (0x20UL)
#define GFD_BVB_STATUS_HSCL_SHORT_LINE_SHIFT (5UL)
#define GFD_BVB_STATUS_MISSING_SYNC_MASK (0x10UL)
#define GFD_BVB_STATUS_MISSING_SYNC_SHIFT (4UL)
#define GFD_BVB_STATUS_LONG_SOURCE_MASK (0x8UL)
#define GFD_BVB_STATUS_LONG_SOURCE_SHIFT (3UL)
#define GFD_BVB_STATUS_SHORT_SOURCE_MASK (0x4UL)
#define GFD_BVB_STATUS_SHORT_SOURCE_SHIFT (2UL)
#define GFD_BVB_STATUS_LONG_LINE_MASK (0x2UL)
#define GFD_BVB_STATUS_LONG_LINE_SHIFT (1UL)
#define GFD_BVB_STATUS_SHORT_LINE_MASK (0x1UL)
#define GFD_BVB_STATUS_SHORT_LINE_SHIFT (0UL)




typedef uint32_t GFD_SCRATCH_TYPE;
#define GFD_SCRATCH_VALUE_MASK (0xffffffffUL)
#define GFD_SCRATCH_VALUE_SHIFT (0UL)




typedef uint32_t GFD_DITHER_CTRL_TYPE;
#define GFD_DITHER_CTRL_MODE_MASK (0xc0000000UL)
#define GFD_DITHER_CTRL_MODE_SHIFT (30UL)
#define GFD_DITHER_CTRL_OFFSET_CH2_MASK (0x3e000000UL)
#define GFD_DITHER_CTRL_OFFSET_CH2_SHIFT (25UL)
#define GFD_DITHER_CTRL_SCALE_CH2_MASK (0x1f00000UL)
#define GFD_DITHER_CTRL_SCALE_CH2_SHIFT (20UL)
#define GFD_DITHER_CTRL_OFFSET_CH1_MASK (0xf8000UL)
#define GFD_DITHER_CTRL_OFFSET_CH1_SHIFT (15UL)
#define GFD_DITHER_CTRL_SCALE_CH1_MASK (0x7c00UL)
#define GFD_DITHER_CTRL_SCALE_CH1_SHIFT (10UL)
#define GFD_DITHER_CTRL_OFFSET_CH0_MASK (0x3e0UL)
#define GFD_DITHER_CTRL_OFFSET_CH0_SHIFT (5UL)
#define GFD_DITHER_CTRL_SCALE_CH0_MASK (0x1fUL)
#define GFD_DITHER_CTRL_SCALE_CH0_SHIFT (0UL)




typedef uint32_t GFD_DITHER_LFSR_INIT_TYPE;
#define GFD_DITHER_LFSR_INIT_SEQ_MASK (0x300000UL)
#define GFD_DITHER_LFSR_INIT_SEQ_SHIFT (20UL)
#define GFD_DITHER_LFSR_INIT_VALUE_MASK (0xfffffUL)
#define GFD_DITHER_LFSR_INIT_VALUE_SHIFT (0UL)




typedef uint32_t GFD_DITHER_LFSR_CTRL_TYPE;
#define GFD_DITHER_LFSR_CTRL_T2_MASK (0x700UL)
#define GFD_DITHER_LFSR_CTRL_T2_SHIFT (8UL)
#define GFD_DITHER_LFSR_CTRL_T1_MASK (0x70UL)
#define GFD_DITHER_LFSR_CTRL_T1_SHIFT (4UL)
#define GFD_DITHER_LFSR_CTRL_T0_MASK (0x7UL)
#define GFD_DITHER_LFSR_CTRL_T0_SHIFT (0UL)




typedef uint32_t GFD_SCB_STATUS_TYPE;
#define GFD_SCB_STATUS_2_CLNT_INIT_STATUS_MASK (0x4UL)
#define GFD_SCB_STATUS_2_CLNT_INIT_STATUS_SHIFT (2UL)
#define GFD_SCB_STATUS_1_CLNT_INIT_STATUS_MASK (0x2UL)
#define GFD_SCB_STATUS_1_CLNT_INIT_STATUS_SHIFT (1UL)
#define GFD_SCB_STATUS_0_CLNT_INIT_STATUS_MASK (0x1UL)
#define GFD_SCB_STATUS_0_CLNT_INIT_STATUS_SHIFT (0UL)




typedef volatile struct COMP_PACKED _GFD_RDBType {
    GFD_REVISION_TYPE revision; /* OFFSET: 0x0 */
    GFD_CONFIGURATION_TYPE hw_configuration; /* OFFSET: 0x4 */
    GFD_ENABLE_TYPE enable; /* OFFSET: 0x8 */
    GFD_LOAD_PALETTE_TYPE load_palette; /* OFFSET: 0xc */
    GFD_CTRL_TYPE ctrl; /* OFFSET: 0x10 */
    GFD_FORMAT_DEF_1_TYPE format_def_1; /* OFFSET: 0x14 */
    GFD_FORMAT_DEF_2_TYPE format_def_2; /* OFFSET: 0x18 */
    GFD_HORIZ_STEP_TYPE horiz_fir_src_step; /* OFFSET: 0x1c */
    GFD_HORIZ_INIT_PHASE_TYPE horiz_fir_init_phase; /* OFFSET: 0x20 */
    GFD_HORIZ_INIT_PHASE_R_TYPE horiz_fir_init_phase_r; /* OFFSET: 0x24 */
    GFD_WIN_COLOR_TYPE win_color; /* OFFSET: 0x28 */
    GFD_W_ALPHA_TYPE w_alpha; /* OFFSET: 0x2c */
    GFD_KEY_MAX_TYPE key_max; /* OFFSET: 0x30 */
    GFD_KEY_MIN_TYPE key_min; /* OFFSET: 0x34 */
    GFD_KEY_MASK_TYPE key_mask; /* OFFSET: 0x38 */
    GFD_KEY_ALPHA_TYPE key_alpha; /* OFFSET: 0x3c */
    GFD_SRC_OFFSET_TYPE src_offset; /* OFFSET: 0x40 */
    GFD_SRC_HSIZE_TYPE src_hsize; /* OFFSET: 0x44 */
    GFD_SRC_START_TYPE src_start; /* OFFSET: 0x48 */
    GFD_SRC_START_R_TYPE src_start_r; /* OFFSET: 0x4c */
    GFD_SRC_PITCH_TYPE src_pitch; /* OFFSET: 0x50 */
    GFD_PALETTE_START_TYPE palette_start; /* OFFSET: 0x54 */
    GFD_PALETTE_SIZE_TYPE palette_size; /* OFFSET: 0x58 */
    GFD_DISP_PIC_SIZE_TYPE disp_pic_size; /* OFFSET: 0x5c */
    GFD_RESERVED_TYPE rsvd0[160]; /* OFFSET: 0x60 */
    GFD_CSC_COEFF_C0_TYPE csc_coeff_c00; /* OFFSET: 0x100 */
    GFD_CSC_COEFF_C1_TYPE csc_coeff_c01; /* OFFSET: 0x104 */
    GFD_CSC_COEFF_C2_TYPE csc_coeff_c02; /* OFFSET: 0x108 */
    GFD_CSC_COEFF_C3_TYPE csc_coeff_c03; /* OFFSET: 0x10c */
    GFD_CSC_COEFF_C4_TYPE csc_coeff_c04; /* OFFSET: 0x110 */
    GFD_CSC_COEFF_C0_TYPE csc_coeff_c10; /* OFFSET: 0x114 */
    GFD_CSC_COEFF_C1_TYPE csc_coeff_c11; /* OFFSET: 0x118 */
    GFD_CSC_COEFF_C2_TYPE csc_coeff_c12; /* OFFSET: 0x11c */
    GFD_CSC_COEFF_C3_TYPE csc_coeff_c13; /* OFFSET: 0x120 */
    GFD_CSC_COEFF_C4_TYPE csc_coeff_c14; /* OFFSET: 0x124 */
    GFD_CSC_COEFF_C0_TYPE csc_coeff_c20; /* OFFSET: 0x128 */
    GFD_CSC_COEFF_C1_TYPE csc_coeff_c21; /* OFFSET: 0x12c */
    GFD_CSC_COEFF_C2_TYPE csc_coeff_c22; /* OFFSET: 0x130 */
    GFD_CSC_COEFF_C3_TYPE csc_coeff_c23; /* OFFSET: 0x134 */
    GFD_CSC_COEFF_C4_TYPE csc_coeff_c24; /* OFFSET: 0x138 */
    GFD_RESERVED_TYPE rsvd1[8]; /* OFFSET: 0x13c */
    GFD_BVB_STATUS_CLEAR_TYPE status_clear; /* OFFSET: 0x144 */
    GFD_BVB_STATUS_TYPE status; /* OFFSET: 0x148 */
    GFD_RESERVED_TYPE rsvd2[4]; /* OFFSET: 0x14c */
    GFD_SCRATCH_TYPE scratch0; /* OFFSET: 0x150 */
    GFD_SCRATCH_TYPE scratch1; /* OFFSET: 0x154 */
    GFD_DITHER_CTRL_TYPE csc_dither_ctrl; /* OFFSET: 0x158 */
    GFD_DITHER_LFSR_INIT_TYPE csc_dither_lfsr_init; /* OFFSET: 0x15c */
    GFD_DITHER_LFSR_CTRL_TYPE csc_dither_lfsr_ctrl; /* OFFSET: 0x160 */
    GFD_RESERVED_TYPE rsvd3[200]; /* OFFSET: 0x164 */
    GFD_SCB_STATUS_TYPE scb_clnt_status; /* OFFSET: 0x22c */
} GFD_RDBType;


#define GFD_0_BASE                      (0x400A0200UL)



#define GFD_MAX_HW_ID                   (1UL)


#define GFD_STATUS_RSVD0_MASK           (0xffffff00UL)


#define GFD_EN_CTRL_STOP_ON_FIELD_END   (0UL)


#define GFD_EN_ENABLE_ENABLE            (1UL)


#define GFD_FMT_1_TYPE_OTHER_FORMATS    (0UL)


#define GFD_FMT_1_TYPE_PALETTE_FORMAT   (3UL)


#define GFD_FMT_1_TYPE_WRGB1555_RGBW5551  (1UL)


#define GFD_FMT_1_VAL_GEN_8888          (((GFD_FMT_1_TYPE_OTHER_FORMATS << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)    \
                                          | (8UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (8UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (8UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (8UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_GEN_4444          (((GFD_FMT_1_TYPE_OTHER_FORMATS << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)    \
                                          | (4UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (4UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (4UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (4UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_GEN_1555          (((GFD_FMT_1_TYPE_WRGB1555_RGBW5551 << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)\
                                          | (1UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (5UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (5UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (5UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_AP88              (((GFD_FMT_1_TYPE_PALETTE_FORMAT << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)   \
                                          | (8UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (8UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_P0                (((GFD_FMT_1_TYPE_PALETTE_FORMAT << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)   \
                                          | (0UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_P1                (((GFD_FMT_1_TYPE_PALETTE_FORMAT << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)   \
                                          | (0UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (1UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_P2                (((GFD_FMT_1_TYPE_PALETTE_FORMAT << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)   \
                                          | (0UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (2UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_P4                (((GFD_FMT_1_TYPE_PALETTE_FORMAT << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)   \
                                          | (0UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (4UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_1_VAL_P8                (((GFD_FMT_1_TYPE_PALETTE_FORMAT << GFD_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)   \
                                          | (0UL << GFD_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT)                          \
                                          | (0UL << GFD_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT)                          \
                                          | (8UL << GFD_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT)))


#define GFD_FMT_2_VAL_AYCRCB8888        (((24UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | (16UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 8UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_YCRCBA8888        ((( 0UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | (24UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 8UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | (16UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_ARGB8888          (((24UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | (16UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 8UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_RGBA8888          ((( 0UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | (24UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | (16UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 8UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_WRGB1555          (((15UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | (10UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 5UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_RGBW5551          ((( 0UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | (11UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 6UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 1UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_ARGB4444          (((12UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | ( 8UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 4UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_RGBA4444          ((( 0UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | (12UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 8UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 4UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_AP88              ((( 8UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_Px                ((( 0UL << GFD_FORMAT_DEF_2_CH3_LSB_POS_SHIFT)               \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH2_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH1_LSB_POS_SHIFT)             \
                                          | ( 0UL << GFD_FORMAT_DEF_2_CH0_LSB_POS_SHIFT)))


#define GFD_FMT_2_VAL_P0                (GFD_FMT_2_VAL_Px)


#define GFD_FMT_2_VAL_P1                (GFD_FMT_2_VAL_Px)


#define GFD_FMT_2_VAL_P2                (GFD_FMT_2_VAL_Px)


#define GFD_FMT_2_VAL_P4                (GFD_FMT_2_VAL_Px)


#define GFD_FMT_2_VAL_P8                (GFD_FMT_2_VAL_Px)

#endif /* GFD_RDB_H */
