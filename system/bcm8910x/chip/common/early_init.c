/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
#include <bcm_err.h>
#include <bcm_utils.h>
#include <avt.h>
#include <pinmux.h>
#include <ulog.h>
#include <clk.h>
#include <mcu.h>
#include <mcu_ext.h>
#include <wdt.h>
#include <osil/uart_osil.h>
#include <osil/flsmgr_osil.h>
#include <osil/vtmon_osil.h>
#if defined(ENABLE_ETH)
#include <osil/eth_osil.h>
#endif
#include <build_info.h>
#include <board.h>
#include <init.h>
#include <interrupt.h>
#include <chip_config.h>
#include <compiler.h>
#include "ee.h"
#if defined(ENABLE_OTP)
#include <otp.h>
#endif
#if defined(ENABLE_ISYS_SRAM_TEST)
#include <bcm_memtest.h>
#endif
#if defined(ENABLE_THREAD_PROTECTION)
#include <thread_safety.h>
#endif

#define GetModuleLogLevel()     ULOG_LVL_ERROR

static volatile uint32_t MCU_InitErr = FALSE;
static volatile uint32_t MCU_InitDone = FALSE;

uint32_t EarlyInitTime;

/**
 * Externs
 */

/* MCU specific MPU region start addresses */
#define BCM8910X_DEVICE_GRP1_START       (0x40000000UL)
#define BCM8910X_DEVICE_GRP2_START       (0x40080000UL)
#define BCM8910X_DEVICE_GRP3_START       (0x410f0000UL)
#define BCM8910X_DEVICE_GRP4_START       (0x42800000UL)
#define BCM8910X_DEVICE_GRP5_START       (0xe0100000UL)
#define BCM8910X_DEVICE_GRP6_START       (0xe0200000UL)

#define BCM8910X_DEVICE_MEMORY_ATTRIB (CORTEX_MX_MPU_ATTRIB_TEX_0\
                            | CORTEX_MX_MPU_ATTRIB_BUFFERABLE\
                            | CORTEX_MX_MPU_ATTRIB_PRIVILEGE\
                            | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC)

#define BCM8910X_TASK_ATTRIB    (CORTEX_MX_MPU_ATTRIB_TEX_1 \
                                | CORTEX_MX_MPU_ATTRIB_CACHEABLE \
                                | CORTEX_MX_MPU_ATTRIB_BUFFERABLE \
                                | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS \
                                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC)

typedef struct _BCM8910X_MPUTableType {
    uint32_t baseAddress;
    CORTEX_MX_MPUAttribType attrib;
} BCM8910X_MPUTableType;

#define BCM8910X_DEFINE_MPU_TABLE(aVar, aSize) \
    const BCM8910X_MPUTableType aVar[aSize]

#define BCM8910X_DEVICE_MPU_TABLE_SIZE  (6UL)
static BCM8910X_DEFINE_MPU_TABLE(BCM8910X_DeviceMPUTable, BCM8910X_DEVICE_MPU_TABLE_SIZE) = {
    {BCM8910X_DEVICE_GRP1_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K
                                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE
                                | BCM8910X_DEVICE_MEMORY_ATTRIB },
    {BCM8910X_DEVICE_GRP2_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K
                                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE
                                | BCM8910X_DEVICE_MEMORY_ATTRIB },
    {BCM8910X_DEVICE_GRP3_START, CORTEX_MX_MPU_ATTRIB_SIZE_64K
                                | CORTEX_MX_MPU_ATTRIB_SR0_DISABLE
                                | BCM8910X_DEVICE_MEMORY_ATTRIB },
    {BCM8910X_DEVICE_GRP4_START, CORTEX_MX_MPU_ATTRIB_SIZE_2K
                                | BCM8910X_DEVICE_MEMORY_ATTRIB },
    {BCM8910X_DEVICE_GRP5_START, CORTEX_MX_MPU_ATTRIB_SIZE_16K
                                | BCM8910X_DEVICE_MEMORY_ATTRIB },
    {BCM8910X_DEVICE_GRP6_START, CORTEX_MX_MPU_ATTRIB_SIZE_256K
                                | BCM8910X_DEVICE_MEMORY_ATTRIB },
};

#define BCM8910X_MEMORY_SRAM_START           (0x20000000UL)
#define BCM8910X_MEMORY_FAST_DMA_START       (0x20000000UL)
#define BCM8910X_MEMORY_CODE_RO_START        (0x20180000UL)
#define BCM8910X_MEMORY_SRAM_DATA_START      (0x201C0000UL)
#define BCM8910X_MEMORY_DRAM_DATA_START      (0x60000000UL)
#define BCM8910X_MEMORY_ISYS_START           (0x20200000UL)

#if defined(ENABLE_DDR)
#define BCM8910X_MEMORY_MPU_TABLE_SIZE  (6UL)
#else
#define BCM8910X_MEMORY_MPU_TABLE_SIZE  (5UL)
#endif

static BCM8910X_DEFINE_MPU_TABLE(BCM8910X_MemoryMPUTable, BCM8910X_MEMORY_MPU_TABLE_SIZE) = {
    {BCM8910X_MEMORY_SRAM_START, CORTEX_MX_MPU_ATTRIB_SIZE_4M
                                | CORTEX_MX_MPU_ATTRIB_TEX_1
                                | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                                | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                | CORTEX_MX_MPU_ATTRIB_USER_READ
                                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC
                                | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE },
    {BCM8910X_MEMORY_ISYS_START, CORTEX_MX_MPU_ATTRIB_SIZE_512K
                                | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                                | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC
                                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE },
    {BCM8910X_MEMORY_FAST_DMA_START, CORTEX_MX_MPU_ATTRIB_SIZE_2M
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                    | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC
                                    | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE },
#if defined(ENABLE_DDR)
    {BCM8910X_MEMORY_CODE_RO_START, CORTEX_MX_MPU_ATTRIB_SIZE_512K
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                                    | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                    | CORTEX_MX_MPU_ATTRIB_READ_ONLY
                                    | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE },
#if (DDR_SIZE_MB == 64)
    {BCM8910X_MEMORY_DRAM_DATA_START, CORTEX_MX_MPU_ATTRIB_SIZE_64M
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                    | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                    | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC },
    {BCM8910X_MEMORY_DRAM_DATA_START, CORTEX_MX_MPU_ATTRIB_SIZE_16M
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                                    | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                    | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                    | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC },
#elif (DDR_SIZE_MB == 32)
    {BCM8910X_MEMORY_DRAM_DATA_START, CORTEX_MX_MPU_ATTRIB_SIZE_32M
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                    | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                    | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC },
    {BCM8910X_MEMORY_DRAM_DATA_START, CORTEX_MX_MPU_ATTRIB_SIZE_8M
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                                    | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                    | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                    | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC },
#else
#error "Unknown DDR Size"
#endif
#else
    {0x20100000UL, CORTEX_MX_MPU_ATTRIB_SIZE_1M
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                                    | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                    | CORTEX_MX_MPU_ATTRIB_READ_ONLY
                                    | CORTEX_MX_MPU_ATTRIB_SR0_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR1_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE },
    {0x20180000UL, CORTEX_MX_MPU_ATTRIB_SIZE_512K
                                    | CORTEX_MX_MPU_ATTRIB_TEX_1
                                    | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                                    | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                                    | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                                    | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC
                                    | CORTEX_MX_MPU_ATTRIB_SR0_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR1_DISABLE
                                    | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE },
#endif
};

#if defined(ENABLE_THREAD_PROTECTION)
typedef struct _BCM_SectionInfo {
    uint32_t start;
    uint32_t end;
} BCM_SectionInfo;

typedef struct _BCM_GroupMemoryAttributes {
    BCM_GroupIDType group;
    BCM_SectionInfo data;
    BCM_SectionInfo bss;
} BCM_GroupMemoryAttributes;

/**
    @note: MISRA C-2012 Rule 8.6 errors will be generated for all below linker
    variables for non-availability of definition in C file as these are from
    linker
*/
extern uint8_t __init_thread_stack__[];
extern uint8_t init_thread_stack_size[];
extern uint8_t __sys_data_start__[];
extern uint8_t __sys_data_end__[];
extern uint8_t __sys_bss_start__[];
extern uint8_t __sys_bss_end__[];
extern uint8_t __ldc_data_start__[];
extern uint8_t __ldc_data_end__[];
extern uint8_t __ldc_bss_start__[];
extern uint8_t __ldc_bss_end__[];

#define BCM_GROUPMEMORYATTRIBUTES_SZ    (2UL)
static const BCM_GroupMemoryAttributes MemoryAttributes[BCM_GROUPMEMORYATTRIBUTES_SZ] = {
    {BCM_GROUPID_SYS, {(uint32_t)__sys_data_start__, (uint32_t)__sys_data_end__}, {(uint32_t)__sys_bss_start__, (uint32_t)__sys_bss_end__}},
    {BCM_GROUPID_LDC, {(uint32_t)__ldc_data_start__, (uint32_t)__ldc_data_end__}, {(uint32_t)__ldc_bss_start__, (uint32_t)__ldc_bss_end__}},
};

extern void BCM_AddTaskMPUEntry(uint32_t aTaskID);
extern void BCM8910X_MPUAddInitThreadStack(void);
#endif

#if defined(ENABLE_ISYS_SRAM_TEST)
#define BCM8910X_MEMORY_ISYS_SIZE            (0x60000UL)

#define BCM8910X_MEMORYTEST_NUM_PATTERNS  (6UL)

const uint32_t BCM8910X_MemoryTestPatterns[BCM8910X_MEMORYTEST_NUM_PATTERNS] = {
    0x00000000UL,
    0x55555555UL,
    0x33333333UL,
    0x0F0F0F0FUL,
    0x00FF00FFUL,
    0x0000FFFFUL
};

#endif

static void BCM8910X_CPUInit(void)
{
    extern uint8_t __bss_start__[];
    extern uint8_t __bss_end__[];
    uint32_t i = 0UL;

    CORTEX_MX_MPUDisable();

    for (i = 0UL; i < BCM8910X_DEVICE_MPU_TABLE_SIZE; i++) {
        CORTEX_MX_MPUConfigure(i, BCM8910X_DeviceMPUTable[i].baseAddress,
                                    BCM8910X_DeviceMPUTable[i].attrib);
    }
    for (i = 0UL; i < BCM8910X_MEMORY_MPU_TABLE_SIZE; i++) {
        CORTEX_MX_MPUConfigure(BCM8910X_DEVICE_MPU_TABLE_SIZE + i,
                                    BCM8910X_MemoryMPUTable[i].baseAddress,
                                    BCM8910X_MemoryMPUTable[i].attrib);
    }
    CORTEX_MX_MPUEnable();

    /* Enable Instruction Cache */
    CORTEX_MX_ICacheEnable();

    /* CLEAR BSS section */
    BCM_MemSet(__bss_start__, 0U, (uint32_t)__bss_end__ - (uint32_t)__bss_start__);

    /* Enable Instruction Cache */
    CORTEX_MX_DCacheEnable();

    /*Enable Faults and Disable Interrupts */
    CORTEX_MX_FAULT_ENABLE();
    CORTEX_MX_INTR_DISABLE();

    /* CPU to expose the maximum counter value as a macro that can be loaded? */
    CORTEX_MX_SysTickInit(0xFFFFFF);

#if defined(ENABLE_ISYS_SRAM_TEST)
    /* memory walkthough for ISYS memmory */
    for (i = 0UL; i < BCM8910X_MEMORYTEST_NUM_PATTERNS; i++) {
        int32_t retVal;
        retVal = BCM_MemTestMarchC((volatile uint32_t *)BCM8910X_MEMORY_ISYS_START, BCM8910X_MEMORY_ISYS_SIZE >> 2UL, BCM8910X_MemoryTestPatterns[i]);
        if (BCM_ERR_OK != retVal) {
            while(1UL);
        }

        retVal = BCM_MemTestHammer((volatile uint32_t *)BCM8910X_MEMORY_ISYS_START, BCM8910X_MEMORY_ISYS_SIZE >> 2UL, BCM8910X_MemoryTestPatterns[i]);
        if (BCM_ERR_OK != retVal) {
            while(1UL);
        }

    }
#endif /*  ENABLE_ISYS_SRAM_TEST */
}

#if defined(ENABLE_THREAD_PROTECTION)
void BCM_AddTaskMPUEntry(uint32_t aTaskID)
{
    uint32_t sizeAttr;
    uint32_t dataAddr = 0UL;
    uint32_t bssAddr = 0UL;
    uint32_t dataAttr = 0UL;
    uint32_t bssAttr = 0UL;
    uint32_t i;
    uint32_t groupID = BRCM_TaskInfo[aTaskID].group;
    uint32_t actualSize, regionSize;

    CORTEX_MX_MPUDisable();
    sizeAttr = CORTEX_MX_MPUGetSizeAttrib(BRCM_TaskInfo[aTaskID].size << 2UL, BRCM_TaskInfo[aTaskID].size << 2UL);

    if (sizeAttr > 0UL) {
        CORTEX_MX_MPUConfigure(BCM8910X_DEVICE_MPU_TABLE_SIZE + BCM8910X_MEMORY_MPU_TABLE_SIZE,
                (uint32_t)BRCM_TaskInfo[aTaskID].base, (sizeAttr | BCM8910X_TASK_ATTRIB));
    }


    for (i = 0; i < BCM_GROUPMEMORYATTRIBUTES_SZ; i++) {
        if (groupID == MemoryAttributes[i].group) {
            actualSize = MemoryAttributes[i].data.end - MemoryAttributes[i].data.start;
            regionSize = BCM_RoundUpPowerTwo(actualSize);
            if (0UL != regionSize) {
                /* minimum size for regionSize */
                if (regionSize < 32UL) {
                    regionSize = 32UL;
                }

                if (0UL == (MemoryAttributes[i].data.start % regionSize)) {
                    dataAttr = CORTEX_MX_MPUGetSizeAttrib(regionSize, actualSize);

                    dataAddr = MemoryAttributes[i].data.start;
                }
            }

            actualSize = MemoryAttributes[i].bss.end - MemoryAttributes[i].bss.start;
            regionSize = BCM_RoundUpPowerTwo(actualSize);
            if (0UL != regionSize) {
                /* minimum size for regionSize */
                if (regionSize < 32UL) {
                    regionSize = 32UL;
                }

                if (0UL == (MemoryAttributes[i].bss.start % regionSize)) {
                    bssAttr = CORTEX_MX_MPUGetSizeAttrib(regionSize, actualSize);

                    bssAddr = MemoryAttributes[i].bss.start;
                }
            }
            break;
        }
    }

    if (dataAttr > 0UL) {
        CORTEX_MX_MPUConfigure(BCM8910X_DEVICE_MPU_TABLE_SIZE + BCM8910X_MEMORY_MPU_TABLE_SIZE + 1UL,
                dataAddr,
                dataAttr
                | CORTEX_MX_MPU_ATTRIB_TEX_1
                | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC);
    } else {
        /* disable region */
        CORTEX_MX_MPUConfigure(BCM8910X_DEVICE_MPU_TABLE_SIZE + BCM8910X_MEMORY_MPU_TABLE_SIZE + 1UL,
                dataAddr,
                CORTEX_MX_MPU_ATTRIB_SR0_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR1_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
    }

    if (bssAttr > 0UL) {
        CORTEX_MX_MPUConfigure(BCM8910X_DEVICE_MPU_TABLE_SIZE + BCM8910X_MEMORY_MPU_TABLE_SIZE + 2UL,
                bssAddr,
                bssAttr
                | CORTEX_MX_MPU_ATTRIB_TEX_1
                | CORTEX_MX_MPU_ATTRIB_CACHEABLE
                | CORTEX_MX_MPU_ATTRIB_BUFFERABLE
                | CORTEX_MX_MPU_ATTRIB_FULL_ACCESS
                | CORTEX_MX_MPU_ATTRIB_NEVER_EXEC);
    } else {
        /* disable region */
        CORTEX_MX_MPUConfigure(BCM8910X_DEVICE_MPU_TABLE_SIZE + BCM8910X_MEMORY_MPU_TABLE_SIZE + 2UL,
                bssAddr,
                CORTEX_MX_MPU_ATTRIB_SR0_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR1_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR2_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR3_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR4_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR5_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR6_DISABLE
                | CORTEX_MX_MPU_ATTRIB_SR7_DISABLE);
    }

    CORTEX_MX_MPUEnable();
}

void BCM_MPUAddInitThreadStack(void)
{
    uint32_t sizeAttr;
    CORTEX_MX_MPUDisable();
    sizeAttr = CORTEX_MX_MPUGetSizeAttrib((uint32_t)init_thread_stack_size, (uint32_t)init_thread_stack_size);
    CORTEX_MX_MPUConfigure(BCM8910X_DEVICE_MPU_TABLE_SIZE + BCM8910X_MEMORY_MPU_TABLE_SIZE,
            (uint32_t)__init_thread_stack__ - (uint32_t)init_thread_stack_size,
            (sizeAttr | BCM8910X_TASK_ATTRIB));
    CORTEX_MX_MPUEnable();
}
#endif


#if defined(ENABLE_VTMON) && !defined(ENABLE_VTMON_TEST)
void VTMON_TempNotification(VTMON_TempType aCurrent,
                              VTMON_TempType aLowThreshold,
                              VTMON_TempType aHighThreshold)
{
    /* TODO: Update this function */
}

void VTMON_VoltNotification(VTMON_VoltIDType aVoltID,
                             VTMON_VoltType aCurrent,
                             VTMON_VoltType aLowThreshold,
                             VTMON_VoltType aHighThreshold)
{
    /* TODO: Update this function */
}
#endif

static void BCM8910X_MCUInit(void)
{
    uint32_t i;
    int32_t err = BCM_ERR_OK;
    MCU_PllStatusType pllStatus = MCU_PLLSTATUS_UNLOCKED;

    MCU_Init(mcu_cfg_table);

    /* Initialize all the clocks and RAM sections */

    /* Initialize the PLLs  */
#if defined(ENABLE_CAMERA_SENSOR) || defined(ENABLE_IIC_TEST) || defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST)
    err = MCU_ClkInit(MCU_CLK_CFG_ID_CAM);
    if (BCM_ERR_OK != err) {
        goto err;
    }
#endif  /* defined(ENABLE_CAMERA_SENSOR) || defined(ENABLE_IIC_TEST) || defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST) */

#if defined(ENABLE_VGCC_DISPLAY) || defined(ENABLE_VGC_TEST)
    err = MCU_ClkInit(MCU_CLK_CFG_ID_DISP);
    if (BCM_ERR_OK != err) {
        goto err;
    }
#endif  /* ENABLE_VGCC_DISPLAY */

#if defined(ENABLE_I2S)
    err = MCU_ClkInit(MCU_CLK_CFG_ID_AUDIO_MCLK_0);
    if (BCM_ERR_OK != err) {
        goto err;
    }
#endif  /* ENABLE_I2S */

    /* Wait for all the PLLs to be locked */
    while (pllStatus != MCU_PLLSTATUS_LOCKED) {
        pllStatus = MCU_GetPllStatus();
    }

    /* Initialize QSPI clock */
#ifdef ENABLE_FLASH
    if (flash_clk_table != NULL) {
        for (i = 0UL; i < flash_clk_table_sz; i++) {
            err = MCU_ClkInit(flash_clk_table[i]);
            if (BCM_ERR_OK != err) {
                break;
            }
        }
        if (BCM_ERR_OK != err) {
            goto err;
        }
    }
#endif /* ENABLE_FLASH */

    /* Initialize all the RAM sections */
    if (mcu_cfg_table->ramCfgTbl != NULL) {
        for (i = 0UL; i < mcu_cfg_table->ramCfgTblSize; i++) {
            MCU_RamSectionInit(i);
        }
    }
err:
    if (err != BCM_ERR_OK) {
        MCU_InitErr = TRUE;
        while (1UL) { }
    } else {
        MCU_InitDone = TRUE;
    }
}

void BCM8910X_EarlyInit(void)
{
    BCM8910X_CPUInit();

    EarlyInitTime = TIM_GetTimeRaw(0UL) / 1000000UL;

    /* initialize MCU */
    BCM8910X_MCUInit();

#if defined(ENABLE_SD)
    MCU_SDPadCfg();
#endif
#if defined(ENABLE_VGC_BT656)
    MCU_BT656PadCfg();
#endif

    /* There is no periperhal reset bit for AMAC in CFG block, so we need to
        reset the AMAC by disabling and re-enabling AMAC DMA */
#if defined (ENABLE_ETH_AMAC)
    ETHER_DisableAllChannels(0x0UL);
#endif

    /* Enable the FPU */
    CORTEX_MX_FPUEnable();
}
