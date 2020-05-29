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
    @file adc_rdb.h
    @brief RDB File for ADC

    @version 2018May25_rdb
*/

#ifndef ADC_RDB_H
#define ADC_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t ADC_CTRL_TYPE;
#define ADC_CTRL_ADC_IRQ_MASK_MASK (0x800000UL)
#define ADC_CTRL_ADC_IRQ_MASK_SHIFT (23UL)
#define ADC_CTRL_UNUSED_MASK (0x3f0000UL)
#define ADC_CTRL_UNUSED_SHIFT (16UL)
#define ADC_CTRL_ENABLE_MASK (0x8000UL)
#define ADC_CTRL_ENABLE_SHIFT (15UL)
#define ADC_CTRL_AMUX_MASK (0x7000UL)
#define ADC_CTRL_AMUX_SHIFT (12UL)
#define ADC_CTRL_REFOUT_EN_MASK (0x800UL)
#define ADC_CTRL_REFOUT_EN_SHIFT (11UL)
#define ADC_CTRL_CKQEXT_EN_MASK (0x400UL)
#define ADC_CTRL_CKQEXT_EN_SHIFT (10UL)
#define ADC_CTRL_TESTMODE_MASK (0x200UL)
#define ADC_CTRL_TESTMODE_SHIFT (9UL)
#define ADC_CTRL_BG_CTL_MASK (0x70UL)
#define ADC_CTRL_BG_CTL_SHIFT (4UL)
#define ADC_CTRL_BG_FASTSTART_DIS_MASK (0x4UL)
#define ADC_CTRL_BG_FASTSTART_DIS_SHIFT (2UL)
#define ADC_CTRL_BG_ALTREF_MASK (0x2UL)
#define ADC_CTRL_BG_ALTREF_SHIFT (1UL)
#define ADC_CTRL_ADCPWRUP_MASK (0x1UL)
#define ADC_CTRL_ADCPWRUP_SHIFT (0UL)




typedef uint32_t ADC_STAT_TYPE;
#define ADC_STAT_RDY_MASK (0x10000UL)
#define ADC_STAT_RDY_SHIFT (16UL)
#define ADC_STAT_OUT_MASK (0x3ffUL)
#define ADC_STAT_OUT_SHIFT (0UL)




typedef uint32_t ADC_IRQ_STAT_TYPE;
#define ADC_IRQ_STAT_IRQ_MASK (0x1UL)
#define ADC_IRQ_STAT_IRQ_SHIFT (0UL)




typedef uint8_t ADC_RESERVED_TYPE;




typedef volatile struct COMP_PACKED _ADC_RDBType {
    ADC_CTRL_TYPE ctrl; /* OFFSET: 0x0 */
    ADC_STAT_TYPE stat; /* OFFSET: 0x4 */
    ADC_IRQ_STAT_TYPE irq_stat; /* OFFSET: 0x8 */
    ADC_RESERVED_TYPE rsvd0[4068]; /* OFFSET: 0xc */
    ADC_RESERVED_TYPE reserved[4]; /* OFFSET: 0xff0 */
} ADC_RDBType;


#define ADC_BASE                        (0x4002A000UL)



#define ADC_MAX_HW_ID                   (1UL)

#endif /* ADC_RDB_H */
