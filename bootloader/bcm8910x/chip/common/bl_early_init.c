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

#include <cortex_mx.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <bcm_memtest.h>

#define BL_SIZE_1KB                    (0x400UL)
#define BL_SIZE_512KB                  (0x80000UL)

/* MCU specific MPU region start addresses */
#define BL_DEV_GRP1_REGION_START       (0x40000000UL)
#define BL_DEV_GRP2_REGION_START       (0x40080000UL)
#define BL_DEV_GRP3_REGION_START       (0x410f0000UL)
#define BL_DEV_GRP4_REGION_START       (0x42800000UL)
#define BL_DEV_GRP5_REGION_START       (0xe0100000UL)
#define BL_DEV_GRP6_REGION_START       (0xe0200000UL)
#define BL_SRAM_REGION_START           (0x20000000UL)
#define BL_DRAM_REGION_START           (0x60000000UL)

typedef struct {
    uint32_t regionBaseAddr;
    uint32_t regionSz;
    uint32_t regionAttr;
} BL_MPU_TblType;

extern uint8_t __data_start__[];
extern uint8_t __data_end__[];
extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

#define BL_MPU_TABLE_SIZE_DEVICE       (6UL)

/* NOTE: Any change made here must be reflected in the ld script as well */
const BL_MPU_TblType BL_MPU_TblDevice[BL_MPU_TABLE_SIZE_DEVICE] = {
    /**********************************************************************/
    /*  regionBaseAddr         SIZE         regionAttr
     **********************************************************************/
    {BL_DEV_GRP1_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K, CORTEX_MX_MPU_ATTRIB_TEX_0 \
                                                              | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_PRIVILEGE \
                                                              | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC \
                                                              | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE},
    {BL_DEV_GRP2_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K, CORTEX_MX_MPU_ATTRIB_TEX_0 \
                                                              | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_PRIVILEGE \
                                                              | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC \
                                                              | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE},
    {BL_DEV_GRP3_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_64K, CORTEX_MX_MPU_ATTRIB_TEX_0 \
                                                              | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_PRIVILEGE \
                                                              | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC \
                                                              | CORTEX_MX_MPU_ATTRIB_SR0_DISABLE},
    {BL_DEV_GRP4_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_2K, CORTEX_MX_MPU_ATTRIB_TEX_0 \
                                                              | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_PRIVILEGE \
                                                              | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC},
    {BL_DEV_GRP5_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_16K, CORTEX_MX_MPU_ATTRIB_TEX_0 \
                                                              | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_PRIVILEGE \
                                                              | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC},
    {BL_DEV_GRP6_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K, CORTEX_MX_MPU_ATTRIB_TEX_0 \
                                                              | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                                              | CORTEX_MX_MPU_ATTRIB_PRIVILEGE \
                                                              | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC},
};

#if defined(BL_ENABLE_DDR)
#define BL_MPU_TABLE_SIZE_MEMORY  (2UL)
#else
#define BL_MPU_TABLE_SIZE_MEMORY  (1UL)
#endif

const BL_MPU_TblType BL_MPU_TblMemory[BL_MPU_TABLE_SIZE_MEMORY] =
{
    /**********************************************************************/
    /*  regionBaseAddr         SIZE         regionAttr
     **********************************************************************/
    /* SRAM Region also includes the ISYS SRAM (Refer to Section 6.1 in Software
     * Architecture Specification document v0.5)*/
    {BL_SRAM_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_2M, CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                                        | CORTEX_MX_MPU_ATTRIB_CACHEABLE \
                                                        | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                                        | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS},
#if defined(BL_ENABLE_DDR)
#if (BL_DDR_SIZE_MB == 64)
    {BL_DRAM_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_64M, CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                                         | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS \
                                                         | CORTEX_MX_MPU_ATTRIB_CACHEABLE \
                                                         | CORTEX_MX_MPU_ATTRIB_BUFFERABLE},
#elif (BL_DDR_SIZE_MB == 32)
    {BL_DRAM_REGION_START, CORTEX_MX_MPU_ATTRIB_SIZE_32M, CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                                         | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS \
                                                         | CORTEX_MX_MPU_ATTRIB_CACHEABLE \
                                                         | CORTEX_MX_MPU_ATTRIB_BUFFERABLE},
#else
#error "Unknown DDR Size"
#endif
#endif
};

static void BL_CPUInit(void)
{
    uint32_t i = 0UL;
    uint32_t reg_num = 0UL;

    CORTEX_MX_MPUDisable();

    for (i = 0; i < BL_MPU_TABLE_SIZE_DEVICE; i++) {
        CORTEX_MX_MPUConfigure(reg_num++, BL_MPU_TblDevice[i].regionBaseAddr, 
                BL_MPU_TblDevice[i].regionAttr | BL_MPU_TblDevice[i].regionSz);
    }

    for (i = 0UL; i < BL_MPU_TABLE_SIZE_MEMORY; i++) {
        CORTEX_MX_MPUConfigure(reg_num++, BL_MPU_TblMemory[i].regionBaseAddr, 
                BL_MPU_TblMemory[i].regionAttr | BL_MPU_TblMemory[i].regionSz);
    }

    CORTEX_MX_MPUEnable();


#ifdef BL_ENABLE_ICACHE
    /* Enable Instruction Cache */
    CORTEX_MX_ICacheEnable();
#endif

    /* CLEAR BSS section */
    BCM_MemSet(__bss_start__, 0U, (uint32_t)__bss_end__ - (uint32_t)__bss_start__);

#ifdef BL_ENABLE_DCACHE
    /* Enable Data Cache */
    CORTEX_MX_DCacheEnable();
#endif
}

static uint32_t BL_SRAMTest(void)
{
    uint32_t ret = 0;

    const uint32_t __bss_size__ = (uint32_t)__bss_end__ - ( uint32_t)__bss_start__;
    const uint32_t __data_size__ = (uint32_t)__data_end__ - (uint32_t)__data_start__;

    /* BSS Section Memory Test */
    ret = BCM_MemTest((void *const)__bss_start__, (const uint32_t)__bss_size__);
    if (ret != 0UL) {
        return ret;
    }

    /* DATA Section Memory Test */
    if (__bss_size__ > __data_size__) {
        BCM_MemCpy(__bss_start__, __data_start__, __data_size__);

        ret = BCM_MemTest((void *const)__data_start__, (const uint32_t)__data_size__);
        if (ret != 0UL) {
            return ret;
        }

        BCM_MemCpy(__data_start__, __bss_start__, __data_size__);
    }

    /*CLEAR BSS section */
    BCM_MemSet(__bss_start__, 0U, (uint32_t)__bss_end__ - (uint32_t)__bss_start__);

    /*SRAM Valid Section Memory Test*/
    ret = BCM_MemTest((void *const)(BL_SRAM_REGION_START + BL_SIZE_512KB), (const uint32_t)(BL_SIZE_512KB * 3UL));
    if (ret != 0) {
        return ret;
    }

    return ret;
}

void BL_EarlyInit(void)
{
    uint32_t err = 0;

    BL_CPUInit();
    err = BL_SRAMTest();
    if (err != 0UL) {
        while(1);
    }
}
