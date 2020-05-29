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
    @file vtmon_rdb.h
    @brief RDB File for VTMON

    @version 2018May25_rdb
*/

#ifndef VTMON_RDB_H
#define VTMON_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint8_t VTMON_RESERVED_TYPE;




typedef uint16_t VTMON_CNTRL_TYPE;
#define VTMON_CNTRL_ANA_OV_EN_MASK (0x8000U)
#define VTMON_CNTRL_ANA_OV_EN_SHIFT (15U)
#define VTMON_CNTRL_ANA_SEL_OV_MASK (0x7000U)
#define VTMON_CNTRL_ANA_SEL_OV_SHIFT (12U)
#define VTMON_CNTRL_ANA_RESET_OV_MASK (0x800U)
#define VTMON_CNTRL_ANA_RESET_OV_SHIFT (11U)
#define VTMON_CNTRL_ANA_BURN_IN_MASK (0x400U)
#define VTMON_CNTRL_ANA_BURN_IN_SHIFT (10U)
#define VTMON_CNTRL_ANA_1V_MODE_MASK (0x200U)
#define VTMON_CNTRL_ANA_1V_MODE_SHIFT (9U)
#define VTMON_CNTRL_PHY_CNTRL_MODE_MASK (0x180U)
#define VTMON_CNTRL_PHY_CNTRL_MODE_SHIFT (7U)
#define VTMON_CNTRL_PHY_VOLT_ENB_MASK (0x40U)
#define VTMON_CNTRL_PHY_VOLT_ENB_SHIFT (6U)
#define VTMON_CNTRL_PHY_TEMP_ENB_MASK (0x20U)
#define VTMON_CNTRL_PHY_TEMP_ENB_SHIFT (5U)
#define VTMON_CNTRL_TESTMODE_MASK (0x10U)
#define VTMON_CNTRL_TESTMODE_SHIFT (4U)
#define VTMON_CNTRL_SEL_MASK (0xeU)
#define VTMON_CNTRL_SEL_SHIFT (1U)
#define VTMON_CNTRL_PWRDN_MASK (0x1U)
#define VTMON_CNTRL_PWRDN_SHIFT (0U)




typedef uint16_t VTMON_TEMP_DATA_TYPE;
#define VTMON_TEMP_DATA_VTMON_PHY_PWRDN_STS_MASK (0x8000U)
#define VTMON_TEMP_DATA_VTMON_PHY_PWRDN_STS_SHIFT (15U)
#define VTMON_TEMP_DATA_TEMP_DATA_MASK (0x3ffU)
#define VTMON_TEMP_DATA_TEMP_DATA_SHIFT (0U)




typedef uint16_t VTMON_TEMP_THRESH_HIGH_TYPE;
#define VTMON_TEMP_THRESH_HIGH_HIGH_MASK (0x3ffU)
#define VTMON_TEMP_THRESH_HIGH_HIGH_SHIFT (0U)




typedef uint16_t VTMON_TEMP_THRESH_LOW_TYPE;
#define VTMON_TEMP_THRESH_LOW_LOW_MASK (0x3ffU)
#define VTMON_TEMP_THRESH_LOW_LOW_SHIFT (0U)




typedef uint16_t VTMON_V1P0_DATA_TYPE;
#define VTMON_V1P0_DATA_DATA_MASK (0x7ffU)
#define VTMON_V1P0_DATA_DATA_SHIFT (0U)




typedef uint16_t VTMON_V1P0_THRESH_HIGH_TYPE;
#define VTMON_V1P0_THRESH_HIGH_VTMON_VDDCMON_REFADJ_MAX_MASK (0x3800U)
#define VTMON_V1P0_THRESH_HIGH_VTMON_VDDCMON_REFADJ_MAX_SHIFT (11U)
#define VTMON_V1P0_THRESH_HIGH_V1P0_THRESH_HIGH_MASK (0x7ffU)
#define VTMON_V1P0_THRESH_HIGH_V1P0_THRESH_HIGH_SHIFT (0U)




typedef uint16_t VTMON_V1P0_THRESH_LOW_TYPE;
#define VTMON_V1P0_THRESH_LOW_LOW_MASK (0x7ffU)
#define VTMON_V1P0_THRESH_LOW_LOW_SHIFT (0U)




typedef uint16_t VTMON_V3P3_2P5_DATA_TYPE;
#define VTMON_V3P3_2P5_DATA_DATA_MASK (0xfffU)
#define VTMON_V3P3_2P5_DATA_DATA_SHIFT (0U)




typedef uint16_t VTMON_V3P3_2P5_THRESH_HIGH_TYPE;
#define VTMON_V3P3_2P5_THRESH_HIGH_V1P0_HIGH_MASK (0xfffU)
#define VTMON_V3P3_2P5_THRESH_HIGH_V1P0_HIGH_SHIFT (0U)




typedef uint16_t VTMON_V3P3_2P5_THRESH_LOW_TYPE;
#define VTMON_V3P3_2P5_THRESH_LOW_V1P0_LOW_MASK (0xfffU)
#define VTMON_V3P3_2P5_THRESH_LOW_V1P0_LOW_SHIFT (0U)




typedef uint16_t VTMON_INTR_TYPE;
#define VTMON_INTR_V3P3_2P5_MASK_MASK (0x800U)
#define VTMON_INTR_V3P3_2P5_MASK_SHIFT (11U)
#define VTMON_INTR_V1P8_MASK_MASK (0x400U)
#define VTMON_INTR_V1P8_MASK_SHIFT (10U)
#define VTMON_INTR_V1P0_MASK_MASK (0x200U)
#define VTMON_INTR_V1P0_MASK_SHIFT (9U)
#define VTMON_INTR_TEMP_MASK_MASK (0x100U)
#define VTMON_INTR_TEMP_MASK_SHIFT (8U)
#define VTMON_INTR_V3P3_2P5_INTR_MASK (0x8U)
#define VTMON_INTR_V3P3_2P5_INTR_SHIFT (3U)
#define VTMON_INTR_V1P8_INTR_MASK (0x4U)
#define VTMON_INTR_V1P8_INTR_SHIFT (2U)
#define VTMON_INTR_V1P0_INTR_MASK (0x2U)
#define VTMON_INTR_V1P0_INTR_SHIFT (1U)
#define VTMON_INTR_TEMP_INTR_MASK (0x1U)
#define VTMON_INTR_TEMP_INTR_SHIFT (0U)




typedef uint16_t VTMON_V1P8_DATA_TYPE;
#define VTMON_V1P8_DATA_DATA_MASK (0xfffU)
#define VTMON_V1P8_DATA_DATA_SHIFT (0U)




typedef uint16_t VTMON_V1P8_THRESH_HIGH_TYPE;
#define VTMON_V1P8_THRESH_HIGH_V1P0_HIGH_MASK (0xfffU)
#define VTMON_V1P8_THRESH_HIGH_V1P0_HIGH_SHIFT (0U)




typedef uint16_t VTMON_V1P8_THRESH_LOW_TYPE;
#define VTMON_V1P8_THRESH_LOW_V1P0_LOW_MASK (0xfffU)
#define VTMON_V1P8_THRESH_LOW_V1P0_LOW_SHIFT (0U)




typedef uint16_t VTMON_SPARE_REG_2_TYPE;
#define VTMON_SPARE_REG_2_REG_MASK (0xffffU)
#define VTMON_SPARE_REG_2_REG_SHIFT (0U)




typedef volatile struct COMP_PACKED _VTMON_RDBType {
    VTMON_RESERVED_TYPE rsvd0[1026]; /* OFFSET: 0x0 */
    VTMON_CNTRL_TYPE vtmon_cntrl; /* OFFSET: 0x402 */
    VTMON_TEMP_DATA_TYPE temp_data; /* OFFSET: 0x404 */
    VTMON_TEMP_THRESH_HIGH_TYPE temp_thresh_high; /* OFFSET: 0x406 */
    VTMON_TEMP_THRESH_LOW_TYPE temp_thresh_low; /* OFFSET: 0x408 */
    VTMON_V1P0_DATA_TYPE v1p0_data; /* OFFSET: 0x40a */
    VTMON_V1P0_THRESH_HIGH_TYPE v1p0_thresh_high; /* OFFSET: 0x40c */
    VTMON_V1P0_THRESH_LOW_TYPE v1p0_thresh_low; /* OFFSET: 0x40e */
    VTMON_RESERVED_TYPE rsvd1[256]; /* OFFSET: 0x410 */
    VTMON_V3P3_2P5_DATA_TYPE v3p3_2p5_data; /* OFFSET: 0x510 */
    VTMON_V3P3_2P5_THRESH_HIGH_TYPE v3p3_2p5_thresh_high; /* OFFSET: 0x512 */
    VTMON_V3P3_2P5_THRESH_LOW_TYPE v3p3_2p5_thresh_low; /* OFFSET: 0x514 */
    VTMON_INTR_TYPE vtmon_intr; /* OFFSET: 0x516 */
    VTMON_V1P8_DATA_TYPE v1p8_data; /* OFFSET: 0x518 */
    VTMON_V1P8_THRESH_HIGH_TYPE v1p8_thresh_high; /* OFFSET: 0x51a */
    VTMON_V1P8_THRESH_LOW_TYPE v1p8_thresh_low; /* OFFSET: 0x51c */
    VTMON_RESERVED_TYPE rsvd2[736]; /* OFFSET: 0x51e */
    VTMON_SPARE_REG_2_TYPE spare_reg_2; /* OFFSET: 0x7fe */
} VTMON_RDBType;


#define VTMON_BASE                      (0x42800000UL)



#define VTMON_MAX_HW_ID                 (1UL)


#define VTMON_TEMP_BIAS                 (41335000L)


#define VTMON_TEMP_PRESCALE             (49055L)


#define VTMON_TEMP_DIV                  (100000L)


#define VTMON_VOLTAGE_SHIFT             (10U)


#define VTMON_V1P0_SCALE                (1257U)


#define VTMON_V1P8_SCALE                (550U)


#define VTMON_V3P3_2P5_SCALE            (1100U)


#define VTMON_TEMP_MON                  (0x0U)


#define VTMON_V1P0_MON                  (0x1U)


#define VTMON_V1P8_MON                  (0x2U)


#define VTMON_V3P3_2P5_MON              (0x3U)


#define VTMON_ALL_MON                   (0x4U)


#define VTMON_TEMP(aVal)                (((int16_t)((VTMON_TEMP_BIAS - (((int32_t)(aVal)) * VTMON_TEMP_PRESCALE)) / VTMON_TEMP_DIV)))


#define VTMON_TEMP_TOREG(aVal)          (((uint16_t)((int16_t)((VTMON_TEMP_BIAS - (((int32_t)aVal) * VTMON_TEMP_DIV)) / VTMON_TEMP_PRESCALE))))


#define VTMON_VOLT_V1P0(aVal)           (((VTMON_V1P0_SCALE * (aVal)) >> VTMON_VOLTAGE_SHIFT))


#define VTMON_VOLT_V1P0_TOREG(aVal)     (((uint16_t)((((uint32_t)aVal) << VTMON_VOLTAGE_SHIFT)/ VTMON_V1P0_SCALE)))


#define VTMON_VOLT_V1P8(aVal)           (((VTMON_V1P8_SCALE * (aVal) >> VTMON_VOLTAGE_SHIFT)))


#define VTMON_VOLT_V1P8_TOREG(aVal)     (((uint16_t)((((uint32_t)aVal) << VTMON_VOLTAGE_SHIFT)/ VTMON_V1P8_SCALE)))


#define VTMON_VOLT_V3P3_2P5(aVal)       (((VTMON_V3P3_2P5_SCALE * (aVal) >> VTMON_VOLTAGE_SHIFT)))


#define VTMON_VOLT_V3P3_2P5_TOREG(aVal)  (((uint16_t)((((uint32_t)aVal) << VTMON_VOLTAGE_SHIFT)/ VTMON_V3P3_2P5_SCALE)))

#endif /* VTMON_RDB_H */
