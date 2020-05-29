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
/******************************************************************************
 File Name: bl_mdio.c
 Description: This file implements the MDIO driver.
******************************************************************************/
#include <inttypes.h>
#include <stdlib.h>
#include <bl_log.h>
#include <bl_chip_config.h>
#include <bl_mdio.h>
#include <bl_compiler.h>
#include "bl_mdio_drv.h"

#define BL_GetModuleLogLevel() (BL_LOG_LVL_INFO)

static BL_COMP_NEVER_INLINE void BL_MDIO_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_MIO_ID, aInstanceID, 0x0, aErr, 4UL, values);
}

void BL_MDIO_Init(void)
{
#ifdef __BCM8910X__
    BL_CFG_RDB_REGS->misc_ctrl = 0x00000001; // Diverge MDIO MDC/MDIO to top level MDC/MDIO lines (VREG and BRPHY)
    BL_CHIP_MISC_RDB_REGS->chip_test_mode = 0x00000100; // Configure CFG_MISC_CTRL module to drive BRPHY mdio_busy to value 1    91
    BL_MDIO_REGS->ctrl = 0x00000281; // Configure MDIO module with preamble, MDC = 25MHz
#endif
#ifdef __BCM89559G__
    BL_MDIO_REGS->ctrl = 0x00000283;
    BL_MDIO_Write(BL_MDIO_ACCESSMODE_CL45, 0x0U, 0x1, 0xA011, 0x1030); // Set MDIO to be 4mA driver in Denali
#endif
}

void BL_MDIO_DeInit(void)
{
#ifdef __BCM8910X__
    BL_CFG_RDB_REGS->misc_ctrl = 0x00000000; // Diverge MDIO MDC/MDIO to top level MDC/MDIO lines (VREG and BRPHY)
    BL_CHIP_MISC_RDB_REGS->chip_test_mode = 0x00000000; // Configure CFG_MISC_CTRL module to drive BRPHY mdio_busy to value 1 	91
#endif
#ifdef __BCM89559G__
    BL_MDIO_REGS->ctrl = 0;
#endif
}

void BL_MDIO_Write(BL_MDIO_AccessModeType aMode,
                  uint8_t aPhy,
                  uint8_t aDev,
                  uint16_t aReg,
                  uint16_t aValue)
{
    uint32_t regVal;
    if (BL_MDIO_ACCESSMODE_CL22 == aMode) {
        BL_LOG_VERB("MDIO wr: phyaddr: %02x raddr: %02x val: %04x\n", aPhy, aReg, aValue);
        BL_MDIO_REGS->cmd = ((1UL << MDIO_CMD_SB_SHIFT) & MDIO_CMD_SB_MASK)
            | ((MDIO_CMD_OPCODE_CL22_WRITE << MDIO_CMD_OP_SHIFT) & MDIO_CMD_OP_MASK)
            | ((aPhy << MDIO_CMD_PA_SHIFT) & MDIO_CMD_PA_MASK)
            | (((uint8_t)aReg << MDIO_CMD_RA_SHIFT) & MDIO_CMD_RA_MASK)
            | ((0x2UL << MDIO_CMD_TA_SHIFT) & MDIO_CMD_TA_MASK)
            | (aValue & MDIO_CMD_MDATA_MASK);
        while (0UL != (MDIO_CTRL_BUSY & BL_MDIO_REGS->ctrl)) {}
    } else if (BL_MDIO_ACCESSMODE_CL45 == aMode) {
        BL_LOG_VERB("MDIO wr: phyaddr: %02x devaddr: %02x raddr: %04x val: %04x\n", aPhy, aDev, aReg, aValue);
        regVal = ((0UL << MDIO_CMD_SB_SHIFT) & MDIO_CMD_SB_MASK)
            | ((MDIO_CMD_OPCODE_CL45_WRITE_ADDR << MDIO_CMD_OP_SHIFT) & MDIO_CMD_OP_MASK)
            | ((((uint32_t)aPhy & (MDIO_CMD_PA_MASK >> MDIO_CMD_PA_SHIFT)) << MDIO_CMD_PA_SHIFT))
            | ((((uint32_t)aDev & (MDIO_CMD_RA_MASK >> MDIO_CMD_RA_SHIFT)) << MDIO_CMD_RA_SHIFT))
            | ((0x2UL << MDIO_CMD_TA_SHIFT) & MDIO_CMD_TA_MASK)
            | (aReg & MDIO_CMD_MDATA_MASK);
        BL_MDIO_REGS->cmd = regVal;
        while (0UL != (MDIO_CTRL_BUSY & BL_MDIO_REGS->ctrl)) {}

        regVal = ((0UL << MDIO_CMD_SB_SHIFT) & MDIO_CMD_SB_MASK)
            | ((MDIO_CMD_OPCODE_CL45_WRITE_DATA << MDIO_CMD_OP_SHIFT) & MDIO_CMD_OP_MASK)
            | ((((uint32_t)aPhy & (MDIO_CMD_PA_MASK >> MDIO_CMD_PA_SHIFT)) << MDIO_CMD_PA_SHIFT))
            | ((((uint32_t)aDev & (MDIO_CMD_RA_MASK >> MDIO_CMD_RA_SHIFT)) << MDIO_CMD_RA_SHIFT))
            | ((0x2UL << MDIO_CMD_TA_SHIFT) & MDIO_CMD_TA_MASK)
            | (aValue & MDIO_CMD_MDATA_MASK);
        BL_MDIO_REGS->cmd = regVal;
        while (0UL != (MDIO_CTRL_BUSY & BL_MDIO_REGS->ctrl)) {}
    } else {
        BL_MDIO_ReportError(aMode, 0UL, 0UL, 0UL, 0UL, __LINE__);
    }
}

uint16_t BL_MDIO_Read(BL_MDIO_AccessModeType aMode,
                     uint8_t aPhy,
                     uint8_t aDev,
                     uint16_t aReg)
{
    uint32_t regVal;

    if (BL_MDIO_ACCESSMODE_CL22 == aMode) {
        BL_MDIO_REGS->cmd = ((1UL << MDIO_CMD_SB_SHIFT) & MDIO_CMD_SB_MASK)
            | ((MDIO_CMD_OPCODE_CL22_READ << MDIO_CMD_OP_SHIFT) & MDIO_CMD_OP_MASK)
            | ((aPhy << MDIO_CMD_PA_SHIFT) & MDIO_CMD_PA_MASK)
            | (((uint8_t)aReg << MDIO_CMD_RA_SHIFT) & MDIO_CMD_RA_MASK)
            | ((0x2UL << MDIO_CMD_TA_SHIFT) & MDIO_CMD_TA_MASK);
        while (0UL != (MDIO_CTRL_BUSY & BL_MDIO_REGS->ctrl)) {}
    } else if (BL_MDIO_ACCESSMODE_CL45 == aMode) {
        regVal = ((0UL << MDIO_CMD_SB_SHIFT) & MDIO_CMD_SB_MASK)
            | ((MDIO_CMD_OPCODE_CL45_WRITE_ADDR << MDIO_CMD_OP_SHIFT) & MDIO_CMD_OP_MASK)
            | ((((uint32_t)aPhy & (MDIO_CMD_PA_MASK >> MDIO_CMD_PA_SHIFT)) << MDIO_CMD_PA_SHIFT))
            | ((((uint32_t)aDev & (MDIO_CMD_RA_MASK >> MDIO_CMD_RA_SHIFT)) << MDIO_CMD_RA_SHIFT))
            | ((0x2UL << MDIO_CMD_TA_SHIFT) & MDIO_CMD_TA_MASK)
            | (aReg & MDIO_CMD_MDATA_MASK);
        BL_MDIO_REGS->cmd = regVal;
        while (0UL != (MDIO_CTRL_BUSY & BL_MDIO_REGS->ctrl)) {}

        regVal = ((0UL << MDIO_CMD_SB_SHIFT) & MDIO_CMD_SB_MASK)
            | ((MDIO_CMD_OPCODE_CL45_READ << MDIO_CMD_OP_SHIFT) & MDIO_CMD_OP_MASK)
            | ((((uint32_t)aPhy & (MDIO_CMD_PA_MASK >> MDIO_CMD_PA_SHIFT)) << MDIO_CMD_PA_SHIFT))
            | ((((uint32_t)aDev & (MDIO_CMD_RA_MASK >> MDIO_CMD_RA_SHIFT)) << MDIO_CMD_RA_SHIFT))
            | ((0x2UL << MDIO_CMD_TA_SHIFT) & MDIO_CMD_TA_MASK);
        BL_MDIO_REGS->cmd = regVal;
        while (0UL != (MDIO_CTRL_BUSY & BL_MDIO_REGS->ctrl)) {}
    } else {
        BL_MDIO_ReportError(aMode, 0UL, 0UL, 0UL, 0UL, __LINE__);
    }

    return ((uint16_t)(BL_MDIO_REGS->cmd & MDIO_CMD_MDATA_MASK));
}
