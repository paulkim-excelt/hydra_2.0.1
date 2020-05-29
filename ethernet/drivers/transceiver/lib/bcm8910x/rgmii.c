/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
    @defgroup grp_eth_xcvrdrv_rgmii_impl IO block based RGMII/RVMII/RMII/MII driver for bcm8953x
    @ingroup grp_eth_xcvrdrv

    @addtogroup grp_eth_xcvrdrv_rgmii_impl
    @{

    @file rgmii.c
    @brief This file implements RGMII driver of Ethernet driver.

    @version 0.1 Initial version
*/
#include <string.h>
#include <compiler.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <eth_xcvr.h>
#include <mcu_rdb.h>
#include <brphy_mii_rdb.h>
#include <cfg_rdb.h>

/**
    @name RGMII driver Design IDs
    @{
    @brief Design IDs for RGMII driver
*/
#define BRCM_SWDSGN_RGMII_INIT_PROC                   (0xA1U) /**< @brief #RGMII_Init */
#define BRCM_SWDSGN_RGMII_RESET_PROC                  (0xA2U) /**< @brief #RGMII_Command */
#define BRCM_SWDSGN_RGMII_GETSQIVALUE_PROC            (0xA3U) /**< @brief #RGMII_GetSQIValue */
#define BRCM_SWDSGN_RGMII_GETDUPLEXMODE_PROC          (0xA4U) /**< @brief #RGMII_GetDuplexMode */
#define BRCM_SWDSGN_RGMII_GETSPEED_PROC               (0xA5U) /**< @brief #RGMII_GetSpeed */
#define BRCM_SWDSGN_RGMII_GETLINKSTATE_PROC           (0xA6U) /**< @brief #RGMII_GetLinkState */
#define BRCM_SWDSGN_RGMII_GETPARAM_PROC               (0xA7U) /**< @brief #RGMII_GetParam */
#define BRCM_SWDSGN_RGMII_SETPARAM_PROC               (0xA8U) /**< @brief #RGMII_SetParam */
#define BRCM_SWDSGN_RGMII_GETMODE_PROC                (0xA9U) /**< @brief #RGMII_GetMode */
#define BRCM_SWDSGN_RGMII_SETMODE_PROC                (0xAAU) /**< @brief #RGMII_SetMode */
#define BRCM_SWDSGN_RGMII_SETSPEED_PROC               (0xABU) /**< @brief #RGMII_SetSpeed */
#define BRCM_SWDSGN_RGMII_GETAUTONEGSTATUS_PROC       (0xACU) /**< @brief #RGMII_GetAutoNegStatus */
#define BRCM_SWDSGN_RGMII_SETDUPLEXMODE_PROC          (0xADU) /**< @brief #RGMII_SetDuplexMode */
#define BRCM_SWDSGN_RGMII_SETFLOWCONTROL_PROC         (0xAEU) /**< @brief #RGMII_SetFlowControl */
#define BRCM_SWDSGN_RGMII_GETFLOWCONTROL_PROC         (0xAFU) /**< @brief #RGMII_GetFlowControl */
#define BRCM_SWDSGN_RGMII_LINKCHANGEINDHANDLER_PROC   (0xB1U) /**< @brief #RGMII_LinkChangeIndHandler */
#define BRCM_SWDSGN_RGMII_DEINIT_PROC                 (0xB2U) /**< @brief #RGMII_DeInit */
#define BRCM_SWDSGN_RGMII_GETSTATS_PROC               (0xB3U) /**< @brief #RGMII_GetStats */
#define BRCM_SWDSGN_RGMII_RWDATA_TYPE                 (0xB4U) /**< @brief #RGMII_RWDataType */
#define BRCM_SWDSGN_RGMII_MCU_REGS_GLOBAL             (0xB5U) /**< @brief #RGMII_MCU_REGS */
#define BRCM_SWDSGN_RGMII_BRPHY_REGS_GLOBAL           (0xB6U) /**< @brief #RGMII_BRPHY_REGS */
#define BRCM_SWDSGN_RGMII_CFG_REGS_GLOBAL             (0xB7U) /**< @brief #RGMII_CFG_REGS */
#define BRCM_SWDSGN_RGMII_RWDATA_GLOBAL               (0xB8U) /**< @brief #RGMII_RWData */
#define BRCM_SWDSGN_RGMII_FUNCTBL_GLOBAL              (0xB9U) /**< @brief #RGMII_FuncTbl */
/** @} */

/**
    @brief CHIP MISC (MCU) block base address

    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static MCU_RDBType* const RGMII_MCU_REGS = (MCU_RDBType *const) MCU_BASE;

/**
    @brief BRPHY CL22 block base address

    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static BRPHY_MII_RDBType * const RGMII_BRPHY_REGS = (BRPHY_MII_RDBType *const)BRPHY0_CL22_IEEE_BASE;

/**
    @brief CFG block base address

    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static CFG_RDBType * const RGMII_CFG_REGS = (CFG_RDBType *const)CFG_BASE;

/**
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
*/
typedef struct _RGMII_RWDataType {
    ETHXCVR_StateType state;
} RGMII_RWDataType;

/**
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
*/
static RGMII_RWDataType RGMII_RWData COMP_SECTION(".data.drivers") =
{
    .state = ETHXCVR_STATE_UNINIT,
};

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_Init(uint8_t aBusIdx,
                          const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;


    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHXCVR_STATE_UNINIT != RGMII_RWData.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    RGMII_RWData.state = ETHXCVR_STATE_INIT;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_DeInit(uint8_t aBusIdx,
                            const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    RGMII_RWData.state = ETHXCVR_STATE_UNINIT;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetMode(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType aMode)
{
    int32_t  retVal = BCM_ERR_OK;
    uint32_t speed;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aConfig->speed) {
            case ETHXCVR_SPEED_100MBPS:
                speed = 1UL;
                break;
            case ETHXCVR_SPEED_1000MBPS:
                speed = 2UL;
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
        }
        if (BCM_ERR_OK == retVal) {
            if (ETHXCVR_MODE_ACTIVE == aMode) {
                RGMII_MCU_REGS->pad_cfg &= ~(MCU_PAD_CFG_RGMII_MODEHV_MASK);
                RGMII_BRPHY_REGS->mii_ctrl |= BRPHY_MII_MII_CTRL_POWER_DOWN_MASK;
                RGMII_CFG_REGS->rgmii_ctrl = (CFG_RGMII_CTRL_EN_MASK |
                        CFG_RGMII_CTRL_BYPASS_IMP_2NS_DEL_MASK |
                        (speed << CFG_RGMII_CTRL_SPEED_SHIFT));
            } else {
                RGMII_CFG_REGS->rgmii_ctrl = 0UL;
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetMode(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig,
                             ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aMode)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (0U == (RGMII_CFG_REGS->rgmii_ctrl & CFG_RGMII_CTRL_EN_MASK)) {
            *aMode = ETHXCVR_MODE_DOWN;
        } else {
            *aMode = ETHXCVR_MODE_ACTIVE;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetLinkState(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  ETHXCVR_LinkStateType *const aLinkState)
{
    int32_t retVal;
    ETHXCVR_ModeType mode;

    retVal = RGMII_GetMode(aBusIdx, aConfig, &mode);

    if ((NULL != aLinkState)
        && (BCM_ERR_OK == retVal)) {
        if (ETHXCVR_MODE_ACTIVE == mode) {
            *aLinkState = ETHXCVR_LINKSTATE_ACTIVE;
        } else {
            *aLinkState = ETHXCVR_LINKSTATE_DOWN;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_Command(uint8_t aBusIdx,
                             const ETHXCVR_PortConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetSpeed(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType *const aSpeed)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t speed;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aSpeed)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        speed = (RGMII_CFG_REGS->rgmii_ctrl & CFG_RGMII_CTRL_SPEED_MASK)
                >> CFG_RGMII_CTRL_SPEED_SHIFT;
        if (speed == 1UL) {
            *aSpeed = ETHXCVR_SPEED_100MBPS;
        } else if (speed == 2UL) {
            *aSpeed = ETHXCVR_SPEED_1000MBPS;
        } else {
            retVal = BCM_ERR_UNKNOWN;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetSpeed(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_SpeedType aSpeed)
{
    int32_t  retVal = BCM_ERR_OK;
    uint32_t speed;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
         || (NULL == aConfig)
         || (ETHXCVR_SPEED_1000MBPS < aSpeed)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        switch (aConfig->speed) {
            case ETHXCVR_SPEED_100MBPS:
                speed = 1UL;
                break;
            case ETHXCVR_SPEED_1000MBPS:
                speed = 2UL;
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
        }
        if (BCM_ERR_OK == retVal) {
            RGMII_CFG_REGS->rgmii_ctrl &= ~(CFG_RGMII_CTRL_SPEED_MASK);
            RGMII_CFG_REGS->rgmii_ctrl |= (speed << CFG_RGMII_CTRL_SPEED_SHIFT);
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetDuplexMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType *const aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aDuplexMode)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        *aDuplexMode = ETHXCVR_DUPLEXMODE_FULL;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetDuplexMode(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_DuplexModeType aDuplexMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
        retVal = BCM_ERR_UNINIT;
    } else {
        if (aDuplexMode == ETHXCVR_DUPLEXMODE_HALF) {
            retVal = BCM_ERR_NOSUPPORT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetFlowControl(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType *const aFlowControl)
{
    int32_t  retVal  = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)
        || (NULL == aFlowControl)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetFlowControl(uint8_t aBusIdx,
                                    const ETHXCVR_PortConfigType *const aConfig,
                                    ETHXCVR_FlowControlType aFlowControl)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetSQIValue(uint8_t aBusIdx,
                                 const ETHXCVR_PortConfigType *const aConfig,
                                 uint32_t *const aSQIValue)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aSQIValue)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_SetParam(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_BooleanType aMode)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
         || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetParam(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_BooleanType *const aMode)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aMode)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetAutoNegStatus(uint8_t aBusIdx,
                                      const ETHXCVR_PortConfigType *const aConfig,
                                      ETHXCVR_AutoNegStatusType *const aStatus)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aStatus)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_GetStats(uint8_t aBusIdx,
                              const ETHXCVR_PortConfigType *const aConfig,
                              ETHXCVR_StatsType *const aStats)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aStats)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_RGMII_FUNCTBL_GLOBAL
    @trace #BRCM_SWREQ_RGMII_FUNCTBL
*/
static int32_t RGMII_StateHandler(uint8_t aBusIdx,
                                  const ETHXCVR_PortConfigType *const aConfig,
                                  uint32_t *const aIsModeChanged,
                                  ETHXCVR_ModeType *const aMode)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aConfig)
        || (NULL == aIsModeChanged)
        || (NULL == aMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

int32_t RGMII_LinkChangeIndHandler(uint8_t aBusIdx,
                                   const ETHXCVR_PortConfigType *const aConfig,
                                   ETHXCVR_LinkStateType *const aLinkState,
                                   uint32_t *const aIsLinkStateChanged)
{
    int32_t retVal = BCM_ERR_NOSUPPORT;

    if ((ETHXCVR_HW_ID_MAX <= aBusIdx)
        || (NULL == aLinkState)
        || (NULL == aIsLinkStateChanged)
        || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ETHXCVR_STATE_INIT != RGMII_RWData.state) {
            retVal = BCM_ERR_UNINIT;
        }
    }

    return retVal;
}

const ETHXCVR_FuncTblType RGMII_FuncTbl = {
    .init = RGMII_Init,
    .deinit = RGMII_DeInit,
    .reset = RGMII_Command,
    .setMode = RGMII_SetMode,
    .getMode = RGMII_GetMode,
    .setMasterMode = RGMII_SetParam,
    .getMasterMode = RGMII_GetParam,
    .getSpeed = RGMII_GetSpeed,
    .setSpeed = RGMII_SetSpeed,
    .getDuplexMode = RGMII_GetDuplexMode,
    .setDuplexMode = RGMII_SetDuplexMode,
    .setFlowControl = RGMII_SetFlowControl,
    .getFlowControl = RGMII_GetFlowControl,
    .getLinkState = RGMII_GetLinkState,
    .getSQIValue = RGMII_GetSQIValue,
    .setLoopbackMode = RGMII_SetParam,
    .getLoopbackMode = RGMII_GetParam,
    .setJumboMode = RGMII_SetParam,
    .getJumboMode = RGMII_GetParam,
    .setAutoNegMode = RGMII_SetParam,
    .getAutoNegStatus = RGMII_GetAutoNegStatus,
    .restartAutoNeg = RGMII_Command,
    .getStats = RGMII_GetStats,
    .stateHandler = RGMII_StateHandler,
    .updateHWStatus = RGMII_Command,
    .linkChangeIndHandler = RGMII_LinkChangeIndHandler,
    .linkIRQHandler  = RGMII_Command,
};

/* Nothing past this line */
