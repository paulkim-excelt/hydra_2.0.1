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
    @file pp_isp_rdb.h
    @brief RDB File for PP_ISP

    @version 2018May25_rdb
*/

#ifndef PP_ISP_RDB_H
#define PP_ISP_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t PP_ISP_ISP0_CTRL_TYPE;
#define PP_ISP_ISP0_CTRL_EN_MASK (0x1UL)
#define PP_ISP_ISP0_CTRL_EN_SHIFT (0UL)




typedef uint8_t PP_ISP_RESERVED_TYPE;




typedef uint32_t PP_ISP_ISP0_BUFFADDR0_TYPE;
#define PP_ISP_ISP0_BUFFADDR0_ADDRESS_MASK (0xffffffffUL)
#define PP_ISP_ISP0_BUFFADDR0_ADDRESS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_BUFFADDR1_TYPE;
#define PP_ISP_ISP0_BUFFADDR1_ADDRESS_MASK (0xffffffffUL)
#define PP_ISP_ISP0_BUFFADDR1_ADDRESS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_BUFFADDR2_TYPE;
#define PP_ISP_ISP0_BUFFADDR2_ADDRESS_MASK (0xffffffffUL)
#define PP_ISP_ISP0_BUFFADDR2_ADDRESS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_STATUS_TYPE;
#define PP_ISP_ISP0_STATUS_IRQ_STATUS_MASK (0x1UL)
#define PP_ISP_ISP0_STATUS_IRQ_STATUS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_BA_TYPE;
#define PP_ISP_ISP0_BA_LAST_SLICE_MASK (0x2UL)
#define PP_ISP_ISP0_BA_LAST_SLICE_SHIFT (1UL)
#define PP_ISP_ISP0_BA_FIRST_SLICE_MASK (0x1UL)
#define PP_ISP_ISP0_BA_FIRST_SLICE_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_BUFFADDR0_S_TYPE;
#define PP_ISP_ISP0_BUFFADDR0_S_S_MASK (0xffffffffUL)
#define PP_ISP_ISP0_BUFFADDR0_S_S_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_BUFFADDR1_S_TYPE;
#define PP_ISP_ISP0_BUFFADDR1_S_S_MASK (0xffffffffUL)
#define PP_ISP_ISP0_BUFFADDR1_S_S_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_BUFFADDR2_S_TYPE;
#define PP_ISP_ISP0_BUFFADDR2_S_S_MASK (0xffffffffUL)
#define PP_ISP_ISP0_BUFFADDR2_S_S_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP0_COUNT_TYPE;
#define PP_ISP_ISP0_COUNT_FRAME_COUNT_MASK (0xff0000UL)
#define PP_ISP_ISP0_COUNT_FRAME_COUNT_SHIFT (16UL)
#define PP_ISP_ISP0_COUNT_SLICE_COUNT_MASK (0xffffUL)
#define PP_ISP_ISP0_COUNT_SLICE_COUNT_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_CTRL_TYPE;
#define PP_ISP_ISP1_CTRL_EN_MASK (0x1UL)
#define PP_ISP_ISP1_CTRL_EN_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_BUFFADDR0_TYPE;
#define PP_ISP_ISP1_BUFFADDR0_ADDRESS_MASK (0xffffffffUL)
#define PP_ISP_ISP1_BUFFADDR0_ADDRESS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_BUFFADDR1_TYPE;
#define PP_ISP_ISP1_BUFFADDR1_ADDRESS_MASK (0xffffffffUL)
#define PP_ISP_ISP1_BUFFADDR1_ADDRESS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_BUFFADDR2_TYPE;
#define PP_ISP_ISP1_BUFFADDR2_ADDRESS_MASK (0xffffffffUL)
#define PP_ISP_ISP1_BUFFADDR2_ADDRESS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_STATUS_TYPE;
#define PP_ISP_ISP1_STATUS_IRQ_STATUS_MASK (0x1UL)
#define PP_ISP_ISP1_STATUS_IRQ_STATUS_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_BA_TYPE;
#define PP_ISP_ISP1_BA_LAST_SLICE_MASK (0x2UL)
#define PP_ISP_ISP1_BA_LAST_SLICE_SHIFT (1UL)
#define PP_ISP_ISP1_BA_FIRST_SLICE_MASK (0x1UL)
#define PP_ISP_ISP1_BA_FIRST_SLICE_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_BUFFADDR0_S_TYPE;
#define PP_ISP_ISP1_BUFFADDR0_S_S_MASK (0xffffffffUL)
#define PP_ISP_ISP1_BUFFADDR0_S_S_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_BUFFADDR1_S_TYPE;
#define PP_ISP_ISP1_BUFFADDR1_S_S_MASK (0xffffffffUL)
#define PP_ISP_ISP1_BUFFADDR1_S_S_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_BUFFADDR2_S_TYPE;
#define PP_ISP_ISP1_BUFFADDR2_S_S_MASK (0xffffffffUL)
#define PP_ISP_ISP1_BUFFADDR2_S_S_SHIFT (0UL)




typedef uint32_t PP_ISP_ISP1_COUNT_TYPE;
#define PP_ISP_ISP1_COUNT_FRAME_COUNT_MASK (0xff0000UL)
#define PP_ISP_ISP1_COUNT_FRAME_COUNT_SHIFT (16UL)
#define PP_ISP_ISP1_COUNT_SLICE_COUNT_MASK (0xffffUL)
#define PP_ISP_ISP1_COUNT_SLICE_COUNT_SHIFT (0UL)




typedef volatile struct COMP_PACKED _PP_ISP_RDBType {
    PP_ISP_ISP0_CTRL_TYPE isp0_ctrl; /* OFFSET: 0x0 */
    PP_ISP_RESERVED_TYPE rsvd0[4]; /* OFFSET: 0x4 */
    PP_ISP_ISP0_BUFFADDR0_TYPE isp0_buffaddr0; /* OFFSET: 0x8 */
    PP_ISP_RESERVED_TYPE rsvd1[4]; /* OFFSET: 0xc */
    PP_ISP_ISP0_BUFFADDR1_TYPE isp0_buffaddr1; /* OFFSET: 0x10 */
    PP_ISP_RESERVED_TYPE rsvd2[4]; /* OFFSET: 0x14 */
    PP_ISP_ISP0_BUFFADDR2_TYPE isp0_buffaddr2; /* OFFSET: 0x18 */
    PP_ISP_RESERVED_TYPE rsvd3[4]; /* OFFSET: 0x1c */
    PP_ISP_ISP0_STATUS_TYPE isp0_status; /* OFFSET: 0x20 */
    PP_ISP_RESERVED_TYPE rsvd4[4]; /* OFFSET: 0x24 */
    PP_ISP_ISP0_BA_TYPE isp0_ba; /* OFFSET: 0x28 */
    PP_ISP_RESERVED_TYPE rsvd5[4]; /* OFFSET: 0x2c */
    PP_ISP_ISP0_BUFFADDR0_S_TYPE isp0_buffaddr0_s; /* OFFSET: 0x30 */
    PP_ISP_RESERVED_TYPE rsvd6[4]; /* OFFSET: 0x34 */
    PP_ISP_ISP0_BUFFADDR1_S_TYPE isp0_buffaddr1_s; /* OFFSET: 0x38 */
    PP_ISP_RESERVED_TYPE rsvd7[4]; /* OFFSET: 0x3c */
    PP_ISP_ISP0_BUFFADDR2_S_TYPE isp0_buffaddr2_s; /* OFFSET: 0x40 */
    PP_ISP_RESERVED_TYPE rsvd8[4]; /* OFFSET: 0x44 */
    PP_ISP_ISP0_COUNT_TYPE isp0_count; /* OFFSET: 0x48 */
    PP_ISP_RESERVED_TYPE rsvd9[4]; /* OFFSET: 0x4c */
    PP_ISP_ISP1_CTRL_TYPE isp1_ctrl; /* OFFSET: 0x50 */
    PP_ISP_RESERVED_TYPE rsvd10[4]; /* OFFSET: 0x54 */
    PP_ISP_ISP1_BUFFADDR0_TYPE isp1_buffaddr0; /* OFFSET: 0x58 */
    PP_ISP_RESERVED_TYPE rsvd11[4]; /* OFFSET: 0x5c */
    PP_ISP_ISP1_BUFFADDR1_TYPE isp1_buffaddr1; /* OFFSET: 0x60 */
    PP_ISP_RESERVED_TYPE rsvd12[4]; /* OFFSET: 0x64 */
    PP_ISP_ISP1_BUFFADDR2_TYPE isp1_buffaddr2; /* OFFSET: 0x68 */
    PP_ISP_RESERVED_TYPE rsvd13[4]; /* OFFSET: 0x6c */
    PP_ISP_ISP1_STATUS_TYPE isp1_status; /* OFFSET: 0x70 */
    PP_ISP_RESERVED_TYPE rsvd14[4]; /* OFFSET: 0x74 */
    PP_ISP_ISP1_BA_TYPE isp1_ba; /* OFFSET: 0x78 */
    PP_ISP_RESERVED_TYPE rsvd15[4]; /* OFFSET: 0x7c */
    PP_ISP_ISP1_BUFFADDR0_S_TYPE isp1_buffaddr0_s; /* OFFSET: 0x80 */
    PP_ISP_RESERVED_TYPE rsvd16[4]; /* OFFSET: 0x84 */
    PP_ISP_ISP1_BUFFADDR1_S_TYPE isp1_buffaddr1_s; /* OFFSET: 0x88 */
    PP_ISP_RESERVED_TYPE rsvd17[4]; /* OFFSET: 0x8c */
    PP_ISP_ISP1_BUFFADDR2_S_TYPE isp1_buffaddr2_s; /* OFFSET: 0x90 */
    PP_ISP_RESERVED_TYPE rsvd18[4]; /* OFFSET: 0x94 */
    PP_ISP_ISP1_COUNT_TYPE isp1_count; /* OFFSET: 0x98 */
} PP_ISP_RDBType;


#define PP_ISP_BASE                     (0xE023D000UL)



#define PP_ISP_MAX_HW_ID                (1UL)

#endif /* PP_ISP_RDB_H */
