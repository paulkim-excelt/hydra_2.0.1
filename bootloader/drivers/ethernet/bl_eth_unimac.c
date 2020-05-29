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

/**
    @defgroup grp_bl_eth_xcvr_impl Implementation
    @ingroup grp_bl_eth_xcvr

    @addtogroup grp_bl_eth_xcvr_impl
    @{
    @file bl_eth_unimac.c
    @brief This file implements UNIMAC layer of Ethernet driver

    @version 0.30 Imported from docx
*/

#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include <bl_chip_config.h>
#include <bl_utils.h>
#include "bl_eth_amac.h"
#include "bl_eth_unimac.h"
#include <amac_rdb.h>

/**
    @name Ethernet Transceiver Driver API IDs
    @{
    @brief API IDs for Ethernet Transceiver
*/
#define BRCM_DSGN_BL_ETHER_XCVR_DEV_TYPE            (0x80U) /**< @brief #BL_ETHER_XcvrDevType */
#define BRCM_DSGN_BL_ETHER_UNIMAC_ENABLE_RX_RESET   (0x81U) /**< @brief #BL_UNIMAC_EnableRxReset */
/** @} */

static AMAC_RDBType * const BL_UNIMAC_REGS = (AMAC_RDBType * const)AMAC_BASE;

/**
    @brief Structure for Xcvr device

    @trace #BRCM_DSGN_BL_ETHER_XCVR_DEV_TYPE #BRCM_REQ_ETHER_XCVR_DEV
 */
typedef struct {
    uint32_t    init;         /**< @brief initialized? */
    uint32_t    enabled;      /**< @brief Xcvr enabled? */
    uint8_t     macAddr[6UL]; /**< @brief MAC Addresss configured */
} BL_ETHER_XcvrDevType;

static BL_ETHER_XcvrDevType BL_XcvrDev = {
    .init = BL_FALSE,
    .enabled = BL_FALSE,
    .macAddr = {0UL},
};

static int32_t BL_UNIMAC_CheckConfigParams(const BL_ETHER_XcvrCfgType *const aCfg)
{

    uint8_t macAddrZero[6UL] = {(uint8_t)0};
    int32_t ret = BL_BCM_ERR_INVAL_PARAMS;

    /* check supported speeds */
    if ((aCfg->speed != BL_ETHER_XCVR_SPEED_10MBPS) &&
            (aCfg->speed != BL_ETHER_XCVR_SPEED_100MBPS)) {
        goto err;
    }

    if ((aCfg->mode != BL_ETHER_XCVR_DUPLEX_MODE_FULL) &&
            (aCfg->mode != BL_ETHER_XCVR_DUPLEX_MODE_HALF)) {
        goto err;
    }

    if (aCfg->maxRxFrmLen > BL_ETH_FRAME_SIZE) {
        goto err;
    }

    if ((aCfg->macAddr != NULL) &&
            (memcmp(aCfg->macAddr, macAddrZero, 6UL) == 0)) {
        goto err;
    }
    ret = BL_BCM_ERR_OK;

err:
    return ret;
}

/** @brief This API enables/disables UniMAC Rx reset logic

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      enable      BL_FALSE(0): disable/ BL_TRUE(1): enable

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_ETHER_UNIMAC_ENABLE_RX_RESET  #BRCM_ARCH_BL_ETHER_UNIMAC_ENABLE_RX_RESET

    @limitations None

    @code{.c}
    if enable equals to BL_TRUE:
		assert UniMAC logic reset;
		disable UniMAC rx logic;
    else:
		de-assert UniMAC logic reset;
		enable UniMAC rx logic;
    @endcode
*/
void BL_UNIMAC_EnableRxReset(uint32_t enable)
{
    uint32_t regVal = BL_UNIMAC_REGS->unimac0_command_config;

    if (enable == BL_TRUE) {
        regVal |= UMAC_CMDCFG_SW_RESET_MASK;
        regVal &= ~UMAC_CMDCFG_RX_ENA_MASK;
    } else {
        regVal &= ~UMAC_CMDCFG_SW_RESET_MASK;
        regVal |= UMAC_CMDCFG_RX_ENA_MASK;
    }
    BL_UNIMAC_REGS->unimac0_command_config = regVal;
}

#if defined(BL_ENABLE_ETH_TEST)
static void BL_UNIMAC_SetLoopbackMode(uint32_t enable,
                                      uint32_t local)
{
    uint32_t regVal = BL_UNIMAC_REGS->unimac0_command_config;

    if ((enable == BL_TRUE) && (local == BL_TRUE)) {
        regVal |= UMAC_CMDCFG_LOOP_ENA_MASK;
    } else if ((enable == BL_TRUE) && (local == BL_FALSE)) {
        regVal |= UMAC_CMDCFG_LINE_LOOPBACK_MASK;
    } else if ((enable == BL_FALSE) && (local == BL_TRUE)) {
        regVal &= ~UMAC_CMDCFG_LOOP_ENA_MASK;
    } else {
        regVal &= ~UMAC_CMDCFG_LINE_LOOPBACK_MASK;
    }

    BL_UNIMAC_REGS->unimac0_command_config = regVal;
}

void BL_UNIMAC_EnableLoopback(uint32_t aLocal)
{
    BL_UNIMAC_SetLoopbackMode(BL_TRUE, aLocal);
}
#endif

static int32_t BL_UNIMAC_GetMacAddr(uint8_t *const aMacAddr)
{
    int32_t retVal;

    if (NULL == aMacAddr) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        if (BL_TRUE == BL_XcvrDev.init) {
            BL_BCM_MemCpy((void *)aMacAddr, (void *)BL_XcvrDev.macAddr,
                    (6UL * sizeof(uint8_t)));
            retVal = BL_BCM_ERR_OK;
        } else {
            retVal = BL_BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

static void BL_UNIMAC_SetMacAddr(const uint8_t *const aMacAddr)
{
    uint32_t macAddr0;
    uint32_t macAddr1;

    /* Set MacAddr */
    macAddr0 = ((((uint32_t)aMacAddr[1UL]) << 8UL)
            | (((uint32_t)aMacAddr[0UL]) << 0UL));

    macAddr1 = ((((uint32_t)aMacAddr[5UL]) << 24UL)
            | (((uint32_t)aMacAddr[4UL]) << 16UL)
            | (((uint32_t)aMacAddr[3UL]) << 8UL)
            | (((uint32_t)aMacAddr[2UL]) << 0UL));

    /* Careful: Register is other way around that it usually looks like */
    BL_UNIMAC_REGS->unimac0_mac_0 = macAddr0;
    BL_UNIMAC_REGS->unimac0_mac_1 = macAddr1;
    BL_BCM_MemCpy((void *)BL_XcvrDev.macAddr, (void *)aMacAddr,
            (6UL * sizeof(uint8_t)));
}

static void BL_UNIMAC_Init(const BL_ETHER_XcvrCfgType *const aConfig)
{
    int32_t retVal;
    uint32_t regVal;
    uint32_t speedMask;
    uint32_t modeMask;

    if ((aConfig == NULL) || (BL_TRUE == BL_XcvrDev.init)) {
        goto err;
    }

    retVal = BL_UNIMAC_CheckConfigParams(aConfig);
    if (retVal != BL_BCM_ERR_OK) {
        goto err;
    }

    /* reset Unimac TX/RX */
    regVal = BL_UNIMAC_REGS->unimac0_command_config;
    regVal |= UMAC_CMDCFG_SW_RESET_MASK;
    regVal &= ~UMAC_CMDCFG_TX_ENA_MASK;
    regVal &= ~ UMAC_CMDCFG_RX_ENA_MASK;

    BL_UNIMAC_REGS->unimac0_command_config = regVal;

    /* Set max frame length. */
    BL_UNIMAC_REGS->unimac0_frm_length = aConfig->maxRxFrmLen;

    regVal = BL_UNIMAC_REGS->unimac0_command_config;
    if (aConfig->speed == BL_ETHER_XCVR_SPEED_10MBPS) {
        speedMask = UMAC_CMDCFG_ETH_SPEED_10MBPS;
    } else {
        speedMask = UMAC_CMDCFG_ETH_SPEED_100MBPS;
    }

    if (aConfig->mode == BL_ETHER_XCVR_DUPLEX_MODE_FULL) {
        modeMask = UMAC_CMDCFG_HD_FULL_DUPLEX;
    } else {
        modeMask = UMAC_CMDCFG_HD_HALF_DUPLEX;
    }

    /* Set speed */
    regVal &= ~UMAC_CMDCFG_ETH_SPEED_MASK;
    regVal |= (speedMask << UMAC_CMDCFG_ETH_SPEED_SHIFT)
        & UMAC_CMDCFG_ETH_SPEED_MASK;
    /* Enable mode */
    regVal &= ~UMAC_CMDCFG_HD_ENA_MASK;
    regVal |= (modeMask << UMAC_CMDCFG_HD_ENA_SHIFT)
        & UMAC_CMDCFG_HD_ENA_MASK;

    /* Enable RX FIFO overflow logic */
    regVal |= UMAC_CMDCFG_OVERFLOW_EN_MASK;

    /* Ignore Rx Pause Frames */
    regVal |= UMAC_CMDCFG_PAUSE_IGNORE_MASK;
    /* Disable Tx Pause Frames */
    regVal |= UMAC_CMDCFG_IGNORE_TX_PAUSE_MASK;

    BL_UNIMAC_REGS->unimac0_command_config = regVal;

    if (NULL != aConfig->macAddr) {
        BL_UNIMAC_SetMacAddr(aConfig->macAddr);
    }

    /* De-assert reset and Enable TX/RX */
    regVal = BL_UNIMAC_REGS->unimac0_command_config;
    regVal &= ~UMAC_CMDCFG_SW_RESET_MASK;
    regVal |= UMAC_CMDCFG_TX_ENA_MASK;
    regVal |= UMAC_CMDCFG_RX_ENA_MASK;
    BL_UNIMAC_REGS->unimac0_command_config = regVal;

    BL_XcvrDev.init = BL_TRUE;
err:
    return ;
}

/**
    @code{.c}
    if aMacAddr equal to NULL:
        set retVal to BL_BCM_ERR_INVAL_PARAMS;
    else:
        if BL_XcvrDev.init to BL_TRUE:
            copy MAC address to aMacAddr from BL_XcvrDev.macAddr;
            set retVal to BL_BCM_ERR_OK;
        else:
            set retVal to BL_BCM_ERR_UNINIT;

    return retVal;
    @endcode
*/
int32_t BL_ETHER_XcvrGetMacAddr(BL_ETHER_HwIDType aID,
                                uint8_t *const aMacAddr)
{
    return BL_UNIMAC_GetMacAddr(aMacAddr);
}

/**
    @code{.c}
    set retVal to BL_BCM_ERR_OK;

    if aMacAddr equal to NULL:
        set retVal to BL_BCM_ERR_INVAL_PARAMS;
    else:
        if BL_XcvrDev.init equal to BL_FALSE:
            set retVal to BL_BCM_ERR_UNINIT;
        else:
            set MAC address to UniMAC register;
            keep a copy of MAC address to BL_XcvrDev.macAddr;

    return retVal;
    @endcode
*/
int32_t BL_ETHER_XcvrSetMacAddr(BL_ETHER_HwIDType aID,
                                const uint8_t *const aMacAddr)
{
    int32_t retVal = BL_BCM_ERR_OK;

    if (aMacAddr == NULL) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else if (0UL == BL_XcvrDev.init) {
        retVal = BL_BCM_ERR_UNINIT;
    } else {
        BL_UNIMAC_SetMacAddr(aMacAddr);
    }
    return retVal;
}

/**
    @code{.c}
    if aConfig equal to  NULL
        or if BL_XcvrDev.init is BL_TRUE:
        goto exit;

    assert reset UniMAC logic;
    disable UniMAC tx logic;
    disable UniMAC rx logic;

    set max frame length;
    set MAC speed;
    set MAC duplex mode;
    enable rx fifo overflow logic;
    ignore rx pause frames;
    disable tx pause frames;
    set MAC address;

    de-assert reset UniMAC logic;
    enable UniMAC tx logic;
    enable UniMAC rx logic;

    set BL_XcvrDev.init to BL_TRUE;
exit:
    @endcode
*/
void BL_ETHER_XcvrInit(BL_ETHER_XcvrIDType aID,
                       const BL_ETHER_XcvrCfgType *const aConfig)
{
    BL_UNIMAC_Init(aConfig);
}

/* Nothing past this line */

/** @} */
