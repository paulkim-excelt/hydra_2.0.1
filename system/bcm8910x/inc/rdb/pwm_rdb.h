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
    @file pwm_rdb.h
    @brief RDB File for PWM

    @version 2018May25_rdb
*/

#ifndef PWM_RDB_H
#define PWM_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t PWM_CTL_TYPE;
#define PWM_CTL_OPENDRAIN_MASK (0x78000UL)
#define PWM_CTL_OPENDRAIN_SHIFT (15UL)
#define PWM_CTL_OUT_POLARITY_MASK (0xf00UL)
#define PWM_CTL_OUT_POLARITY_SHIFT (8UL)
#define PWM_CTL_ENABLE_MASK (0xfUL)
#define PWM_CTL_ENABLE_SHIFT (0UL)




typedef uint32_t PWM_PERIOD_CNT_0_TYPE;
#define PWM_PERIOD_CNT_0_0_MASK (0xffffUL)
#define PWM_PERIOD_CNT_0_0_SHIFT (0UL)




typedef uint32_t PWM_DUTY_CNT_0_TYPE;
#define PWM_DUTY_CNT_0_0_MASK (0xffffUL)
#define PWM_DUTY_CNT_0_0_SHIFT (0UL)




typedef uint32_t PWM_PERIOD_CNT_1_TYPE;
#define PWM_PERIOD_CNT_1_1_MASK (0xffffUL)
#define PWM_PERIOD_CNT_1_1_SHIFT (0UL)




typedef uint32_t PWM_DUTY_CNT_1_TYPE;
#define PWM_DUTY_CNT_1_1_MASK (0xffffUL)
#define PWM_DUTY_CNT_1_1_SHIFT (0UL)




typedef uint32_t PWM_PERIOD_CNT_2_TYPE;
#define PWM_PERIOD_CNT_2_2_MASK (0xffffUL)
#define PWM_PERIOD_CNT_2_2_SHIFT (0UL)




typedef uint32_t PWM_DUTY_CNT_2_TYPE;
#define PWM_DUTY_CNT_2_2_MASK (0xffffUL)
#define PWM_DUTY_CNT_2_2_SHIFT (0UL)




typedef uint32_t PWM_PERIOD_CNT_3_TYPE;
#define PWM_PERIOD_CNT_3_3_MASK (0xffffUL)
#define PWM_PERIOD_CNT_3_3_SHIFT (0UL)




typedef uint32_t PWM_DUTY_CNT_3_TYPE;
#define PWM_DUTY_CNT_3_3_MASK (0xffffUL)
#define PWM_DUTY_CNT_3_3_SHIFT (0UL)




typedef uint32_t PWM_PRESCALE_TYPE;
#define PWM_PRESCALE_PWM0_PRESCALE_MASK (0xfc0000UL)
#define PWM_PRESCALE_PWM0_PRESCALE_SHIFT (18UL)
#define PWM_PRESCALE_PWM1_PRESCALE_MASK (0x3f000UL)
#define PWM_PRESCALE_PWM1_PRESCALE_SHIFT (12UL)
#define PWM_PRESCALE_PWM2_PRESCALE_MASK (0xfc0UL)
#define PWM_PRESCALE_PWM2_PRESCALE_SHIFT (6UL)
#define PWM_PRESCALE_PWM3_PRESCALE_MASK (0x3fUL)
#define PWM_PRESCALE_PWM3_PRESCALE_SHIFT (0UL)




typedef volatile struct COMP_PACKED _PWM_RDBType {
    PWM_CTL_TYPE pwmctl; /* OFFSET: 0x0 */
    PWM_PERIOD_CNT_0_TYPE period_cnt_0; /* OFFSET: 0x4 */
    PWM_DUTY_CNT_0_TYPE duty_cnt_0; /* OFFSET: 0x8 */
    PWM_PERIOD_CNT_1_TYPE period_cnt_1; /* OFFSET: 0xc */
    PWM_DUTY_CNT_1_TYPE duty_cnt_1; /* OFFSET: 0x10 */
    PWM_PERIOD_CNT_2_TYPE period_cnt_2; /* OFFSET: 0x14 */
    PWM_DUTY_CNT_2_TYPE duty_cnt_2; /* OFFSET: 0x18 */
    PWM_PERIOD_CNT_3_TYPE period_cnt_3; /* OFFSET: 0x1c */
    PWM_DUTY_CNT_3_TYPE duty_cnt_3; /* OFFSET: 0x20 */
    PWM_PRESCALE_TYPE prescale; /* OFFSET: 0x24 */
} PWM_RDBType;


#define PWM_BASE                        (0x40021000UL)



#define PWM_MAX_HW_ID                   (1UL)

#endif /* PWM_RDB_H */
