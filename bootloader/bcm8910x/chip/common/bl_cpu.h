/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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

/******************************************************************************
 File Name: bl_cpu.h
 Description: This file describes the CPU block interface.
******************************************************************************/

#ifndef BL_BCM8910X_CPU_H
#define BL_BCM8910X_CPU_H

/* Includes */
#include <stdint.h>

typedef volatile struct {
    uint32_t CPU_DCACHEERR;         /**< Data Cache Error Status Register */
#define CPU_DCACHEERR_DCERR_MASK        (0x03fffff0UL)
#define CPU_DCACHEERR_DCERR_SHIFT       (4UL)
#define CPU_DCACHEERR_DCDET_MASK        (0x0000000fUL)
#define CPU_DCACHEERR_DCDET_SHIFT       (0UL)
    uint32_t CPU_ICACHEERR;         /**< Instruction Cache Error Status */
#define CPU_ICACHEERR_ICERR_MASK        (0x03fffff0UL)
#define CPU_ICACHEERR_ICERR_SHIFT       (4UL)
#define CPU_ICACHEERR_ICDET_MASK        (0x0000000fUL)
#define CPU_ICACHEERR_ICDET_SHIFT       (0UL)
    uint32_t MEMCTL;            /**< Memory Test and Debug Cntrl Register */
#define CPU_MEMCTL_ROMTM_MASK           (0x0001f800UL)
#define CPU_MEMCTL_ROMTM_SHIFT          (11UL)
#define CPU_MEMCTL_MEMPOWERONIN_MASK    (0x00000400UL)
#define CPU_MEMCTL_MEMPOWERONIN_SHIFT   (10UL)
#define CPU_MEMCTL_MEMPOWEROKIN_MASK    (0x00000200UL)
#define CPU_MEMCTL_MEMPOWEROKIN_SHIFT   (9UL)
#define CPU_MEMCTL_MEMISO_MASK          (0x00000100UL)
#define CPU_MEMCTL_MEMISO_SHIFT         (8UL)
#define CPU_MEMCTL_MEMPDA_MASK          (0x00000080UL)
#define CPU_MEMCTL_MEMPDA_SHIFT         (7UL)
#define CPU_MEMCTL_MEMTM_MASK           (0x0000007fUL)
#define CPU_MEMCTL_MEMTM_SHIFT          (0UL)
    uint32_t CPU_DBGSCRATCH;        /**< Debug Scratch Register */
#define CPU_DBGSCRATCH_MASK             (0xffffffffUL)
#define CPU_DBGSCRATCH_SHIFT            (0UL)
    uint32_t CPU_MBISTCTL;          /**< MBIST Control Register */
#define CPU_MBISTCTL_WRENABLE_MASK      (0x00000002UL)
#define CPU_MBISTCTL_WRENABLE_SHIFT     (1UL)
#define CPU_MBISTCTL_ENABLE_MASK        (0x00000001UL)
#define CPU_MBISTCTL_ENABLE_SHIFT       (0UL)
    uint32_t CPU_MBISTWDATA;        /**< MBIST Write Data Register */
#define CPU_MBISTWDATA_MASK             (0xffffffffUL)
#define CPU_MBISTWDATA_SHIFT            (0UL)
    uint32_t CPU_MBISTRDATA;        /**< MBIST Read Data Register */
#define CPU_MBISTRDATA_MASK             (0xffffffffUL)
#define CPU_MBISTRDATA_SHIFT            (0UL)
    uint32_t CPU_IDCACHE_SECCNT;    /**< I/D Cache Single-bit Err Cnt Reg */
#define CPU_IDCACHE_SECCNT_MASK         (0x0000000fUL)
#define CPU_IDCACHE_SECCNT_SHIFT        (0UL)
    uint32_t CPU_MISCCTL;           /**< Miscellaneous Control Register */
#define CPU_MISCCTL_TIMSTMPSTRT_MASK    (0x80000000UL)
#define CPU_MISCCTL_TIMSTMPSTRT_SHIFT   (31UL)
#define CPU_MISCCTL_TIMSTMPSTOP_MASK    (0x40000000UL)
#define CPU_MISCCTL_TIMSTMPSTOP_SHIFT   (30UL)
#define CPU_MISCCTL_CPUCLKENOVR_MASK    (0x00000010UL)
#define CPU_MISCCTL_CPUCLKENOVR_SHIFT   (4UL)
#define CPU_MISCCTL_ROMCLKENOVR_MASK    (0x00000008UL)
#define CPU_MISCCTL_ROMCLKENOVR_SHIFT   (3UL)
#define CPU_MISCCTL_ATBCLKENOVR_MASK    (0x00000004UL)
#define CPU_MISCCTL_ATBCLKENOVR_SHIFT   (2UL)
#define CPU_MISCCTL_TRACESFTRST_MASK    (0x00000002UL)
#define CPU_MISCCTL_TRACESFTRST_SHIFT   (1UL)
#define CPU_MISCCTL_TRACECLKSEL_MASK    (0x00000001UL)
#define CPU_MISCCTL_TRACECLKSEL_SHIFT   (0UL)
    uint32_t CPU_MSPICTL;           /**< MSPI Control Register */
#define CPU_MSPICTL_WRDISABLE_MASK      (0x00000003UL)
#define CPU_MSPICTL_WRDISABLE_SHIFT     (0UL)
    uint32_t CPU_DHSCTL;            /**< Display Handshake Control Register */
#define CPU_DHSCTL_DHS1ACK_MASK         (0x00000800UL)
#define CPU_DHSCTL_DHS1ACK_SHIFT        (11UL)
#define CPU_DHSCTL_DHS1READY_MASK       (0x00000400UL)
#define CPU_DHSCTL_DHS1READY_SHIFT      (10UL)
#define CPU_DHSCTL_DHS1BA_MASK          (0x00000300UL)
#define CPU_DHSCTL_DHS1BA_SHIFT         (8UL)
#define CPU_DHSCTL_DHS1SEL_MASK         (0x000000c0UL)
#define CPU_DHSCTL_DHS1SELCPU_MASK      (0x00000000UL)
#define CPU_DHSCTL_DHS1SELDISPLAY_MASK  (0x00000040UL)
#define CPU_DHSCTL_DHS1SELIMG_MASK      (0x00000080UL)
#define CPU_DHSCTL_DHS1SEL_SHIFT        (6UL)
#define CPU_DHSCTL_DHS0ACK_MASK         (0x00000020UL)
#define CPU_DHSCTL_DHS0ACK_SHIFT        (5UL)
#define CPU_DHSCTL_DHS0READY_MASK       (0x00000010UL)
#define CPU_DHSCTL_DHS0READY_SHIFT      (4UL)
#define CPU_DHSCTL_DHS0BA_MASK          (0x0000000cUL)
#define CPU_DHSCTL_DHS0BA_SHIFT         (2UL)
#define CPU_DHSCTL_DHS0SEL_MASK         (0x00000003UL)
#define CPU_DHSCTL_DHS0SELCPU_MASK      (0x00000000UL)
#define CPU_DHSCTL_DHS0SELDISPLAY_MASK  (0x00000001UL)
#define CPU_DHSCTL_DHS0SELIMG_MASK      (0x00000002UL)
#define CPU_DHSCTL_DHS0SEL_SHIFT        (0UL)
    uint32_t RESERVED1[29];
    uint32_t CPU_NMISTSCTL;         /**< Non-maskable Interrupt Status and Control Register */
#define CPU_NMISTSCTL_CLEAR_MASK        (0x80000000UL)
#define CPU_NMISTSCTL_CLEAR_SHIFT       (31UL)
#define CPU_NMISTSCTL_WDOG_MASK         (0x00000004UL)
#define CPU_NMISTSCTL_WDOG_SHIFT        (2UL)
#define CPU_NMISTSCTL_IDCACHEERR_MASK   (0x00000002UL)
#define CPU_NMISTSCTL_IDCACHEERR_SHIFT  (1UL)
#define CPU_NMISTSCTL_SRAMECCUNCOR_MASK (0x00000001UL)
#define CPU_NMISTSCTL_SRAMECCUNCOR_SHIFT (0UL)
    uint32_t RESERVED2[895];
    uint32_t CPU_RESETOUT;          /**< CPU Reset Output Register */
#define CPU_RESETOUT_VAL_MASK           (0x80000000UL)
#define CPU_RESETOUT_VAL_SHIFT          (31UL)
#define CPU_RESETOUT_KEY_MASK           (0x7fffffffUL)
#define CPU_RESETOUT_KEY_SHIFT          (0UL)
} BL_CPU_RegsType;

#endif /* BL_BCM8910X_CPU_H */