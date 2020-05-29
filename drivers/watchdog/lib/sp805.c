/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_wdtdrv_drv_impl Driver Implementation
    @ingroup grp_wdtdrv

    @addtogroup grp_wdtdrv_drv_impl
    @{
*/

#include <stdlib.h>
#include <wdt.h>
#include <wdt_osil.h>
#include <bcm_err.h>
#include <compiler.h>
#include <bcm_utils.h>

#include "wdt_rdb.h"

/**
    @name Watchdog driver implementation IDs
    @{
    @brief Implementation IDs for Watchdog driver
*/
#define BRCM_SWDSGN_WDT_DRV_MS_MACRO                  (0x90U) /**< @brief #WDT_DRV_MS_PER_SEC           */
#define BRCM_SWDSGN_WDT_DRV_DEV_TYPE                  (0x91U) /**< @brief #WDT_DrvDevType               */
#define BRCM_SWDSGN_WDT_DRV_REGS_GLOBAL               (0x92U) /**< @brief #WDT_DrvRegs                  */
#define BRCM_SWDSGN_WDT_DRV_DEV_GLOBAL                (0x93U) /**< @brief #WDT_DrvDev                   */
#define BRCM_SWDSGN_WDT_DRV_SETMODEINTERNAL_PROC      (0x94U) /**< @brief #WDT_DrvSetModeInternal       */
#define BRCM_SWDSGN_WDT_DRV_INIT_PROC                 (0x95U) /**< @brief #WDT_DrvInit                  */
#define BRCM_SWDSGN_WDT_DRV_SETMODE_PROC              (0x96U) /**< @brief #WDT_DrvSetMode               */
#define BRCM_SWDSGN_WDT_DRV_SETTRIGGERCONDITION_PROC  (0x97U) /**< @brief #WDT_DrvSetTriggerCondition   */
#define BRCM_SWDSGN_WDT_DRV_SERVICE_PROC              (0x98U) /**< @brief #WDT_DrvService               */
#define BRCM_SWDSGN_WDT_SYSCMDHANDLER_PROC            (0x99U) /**< @brief #WDT_SysCmdHandler            */
#define BRCM_SWDSGN_WDT_SVCIO_TYPE                    (0x9AU) /**< @brief #WDT_SVCIOType                */
/** @} */

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
#define WDT_DRV_MS_PER_SEC              (1000UL)
#define WDT_DRV_MS_TO_TICK(time_ms)     (((time_ms) * (WDT_CLOCK /WDT_DRV_MS_PER_SEC)) - 1UL)
#define WDT_DRV_MS_FROM_TICK(ticks)     (((uint32_t)((((ticks) + 1ULL) * (uint64_t)WDT_DRV_MS_PER_SEC) / (uint64_t) WDT_CLOCK)))
#define WDT_DRV_MS_TIMEOUT_MAX          WDT_DRV_MS_FROM_TICK(0xFFFFFFFFUL)

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
typedef struct _WDT_DrvDevType {
    WDT_ModeType    currentMode;      /**< Current Mode of driver             */
    WDT_StateType   state;            /**< State of the driver                */
    WDT_CfgType     config;           /**< Configuration of the SP805 device  */
} WDT_DrvDevType;

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
static WDT_RDBType * const WDT_DrvRegs[WDT_NUM_HW_ID] =
{
#if (WDT_NUM_HW_ID == 0)
#error "WDT_NUM_HW_ID = 0"
#endif
    (WDT_RDBType *) WDT0_BASE,
#if (WDT_NUM_HW_ID > 1)
#error "WDT_NUM_HW_ID > 1"
#endif
};

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
static WDT_DrvDevType WDT_DrvDev[WDT_NUM_HW_ID] COMP_SECTION(".data.drivers") =
{
#if (WDT_NUM_HW_ID == 0)
#error "WDT_NUM_HW_ID = 0"
#endif
    {
        .currentMode = WDT_MODE_OFF,
        .state = WDT_STATE_UNINIT,
        .config = {0},
    },
#if (WDT_NUM_HW_ID > 1)
#error "watchdog driver do not supports more than one watchdog hardware"
#endif
};

/**
    @brief Internal function to set the watchdog mode

    Helper function to set the mode of SP805 driver.
    Note that this function do not change the state of the
    driver, and assume that the SP805 registers are unlocked.

    NOTE: This internal function makes the following assumptions:
          1) aID, driver state are already validated by the caller.
          2) SP805 registers are already unlocked by the caller.
          3) Caller shall lock SP805 registers after calling this fucntion.

    @param[in]   aID     SP805 hardware ID
    @param[in]   aMode   mode to be set

    @retval      BCM_ERR_OK              Mode set successfully
    @retval      BCM_ERR_INVAL_PARAMS    Invalid mode

    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
static int32_t WDT_DrvSetModeInternal(WDT_HwIDType aID, WDT_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    switch (aMode) {
    case WDT_MODE_OFF:
        /* Check if runtime disabling is allowed by configuration */
        if (WDT_DISABLE_ALLOWED == WDT_DrvDev[aID].config.runtimeDisable) {
            /* Disable the counter and interrupt */
            WDT_DrvRegs[aID]->wdogcontrol &= (~WDT_WDOGCONTROL_INTEN_MASK);

            /* Clear the interrupt */
            WDT_DrvRegs[aID]->wdogintclr = WDT_WDOGINTCLR_WDOGINTCLR_MASK;

            WDT_DrvDev[aID].currentMode = WDT_MODE_OFF;
        } else {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
        break;
    case WDT_MODE_ACTIVE:
        /* Enable back the counter and interrupt */
        WDT_DrvRegs[aID]->wdogcontrol &= (~WDT_WDOGCONTROL_INTEN_MASK);
        WDT_DrvRegs[aID]->wdogcontrol |= WDT_WDOGCONTROL_INTEN_MASK;
        WDT_DrvRegs[aID]->wdogcontrol |= WDT_WDOGCONTROL_RESEN_MASK;

        WDT_DrvDev[aID].currentMode =  WDT_MODE_ACTIVE;
        break;
    default:
        retVal = BCM_ERR_INVAL_PARAMS;
        break;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
static void WDT_DrvInit(WDT_HwIDType aID, const WDT_CfgType *const aConfig)
{
    int32_t retVal;
    uint32_t ticks;

    if (NULL == aConfig) {
        goto err;
    }

    if ((WDT_DISABLE_ALLOWED != aConfig->runtimeDisable)
        && (WDT_DISABLE_NOT_ALLOWED != aConfig->runtimeDisable)) {
        goto err;
    }

    if ((WDT_MODE_OFF != aConfig->defaultMode)
        && (WDT_MODE_ACTIVE != aConfig->defaultMode)) {
        goto err;
    }

    if (WDT_DRV_MS_TIMEOUT_MAX < aConfig->defaultTimeout_ms) {
        goto err;
    }

    if (WDT_NUM_HW_ID > aID) {
        if (WDT_STATE_UNINIT == WDT_DrvDev[aID].state) {
            BCM_MemCpy(&WDT_DrvDev[aID].config, aConfig, sizeof(WDT_CfgType));

            if (0UL != WDT_DrvDev[aID].config.defaultTimeout_ms) {
                ticks = WDT_DRV_MS_TO_TICK(WDT_DrvDev[aID].config.defaultTimeout_ms);
            } else {
                ticks = 1UL;
            }

            /* Unlock the sp805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_UNLOCK;

            /* Set the default timeout */
            WDT_DrvRegs[aID]->wdogload = ticks;
            /* Set default mode */
            retVal = WDT_DrvSetModeInternal(aID,
                                           WDT_DrvDev[aID].config.defaultMode);

            /* Lock the SP805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_LOCK;

            if (BCM_ERR_OK == retVal) {
                /* Move driver to IDLE state */
                WDT_DrvDev[aID].state = WDT_STATE_IDLE;
            }
        }
    }

err:
        return;
}

/**
    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWREQ_WDT_DYNAMIC_CONFIG
*/
static int32_t WDT_DrvSetMode(WDT_HwIDType aID, WDT_ModeType aMode)
{
    int32_t retVal;
    if (WDT_NUM_HW_ID > aID) {
        if (WDT_STATE_IDLE == WDT_DrvDev[aID].state) {
            /* Move to BUSY state */
            WDT_DrvDev[aID].state = WDT_STATE_BUSY;

            /* Unlock the SP805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_UNLOCK;

            /* Call the internal function to actually set the mode */
            retVal = WDT_DrvSetModeInternal(aID, aMode);

            /* Lock the SP805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_LOCK;

            /* Move back to IDLE state */
            WDT_DrvDev[aID].state = WDT_STATE_IDLE;
        } else if (WDT_STATE_UNINIT == WDT_DrvDev[aID].state) {
            retVal = BCM_ERR_UNINIT;
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWREQ_WDT_DYNAMIC_CONFIG
*/
static int32_t WDT_DrvSetTriggerCondition(WDT_HwIDType aID, uint32_t aTimeout_ms)
{
    int32_t retVal;
    uint32_t ticks;

    if ((WDT_NUM_HW_ID > aID)
        && (WDT_DRV_MS_TIMEOUT_MAX >= aTimeout_ms)) {
        if (WDT_STATE_IDLE == WDT_DrvDev[aID].state) {
            /* Move to BUSY State */
            WDT_DrvDev[aID].state = WDT_STATE_BUSY;

            /* Unlock the SP805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_UNLOCK;

            /*
             * Update the load register only if the current load register value
             * is non-zero.
             */
            if (0UL != WDT_DrvRegs[aID]->wdogload) {
                if (0UL != aTimeout_ms) {
                    ticks = WDT_DRV_MS_TO_TICK(aTimeout_ms);
                } else {
                    ticks = 1UL;
                }
                WDT_DrvRegs[aID]->wdogload = ticks;
            }

            /* Lock the SP805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_LOCK;

            /* Move back to IDLE State */
            WDT_DrvDev[aID].state = WDT_STATE_IDLE;

            retVal = BCM_ERR_OK;
        } else if (WDT_STATE_UNINIT == WDT_DrvDev[aID].state) {
            retVal = BCM_ERR_UNINIT;
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_SERVICE
*/
static int32_t WDT_DrvService(WDT_HwIDType aID)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (WDT_NUM_HW_ID > aID) {
        if ((WDT_STATE_IDLE == WDT_DrvDev[aID].state)
            && (WDT_MODE_ACTIVE == WDT_DrvDev[aID].currentMode)){
            /* Move to busy state */
            WDT_DrvDev[aID].state = WDT_STATE_BUSY;

            /* Unlock the SP805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_UNLOCK;

            /* Clear the SP805 interrupt */
            WDT_DrvRegs[aID]->wdogintclr = WDT_WDOGINTCLR_WDOGINTCLR_MASK;

            /* Lock the SP805 registers */
            WDT_DrvRegs[aID]->wdoglock = WDT_REG_LOCK;

            /* Move driver back to idle state */
            WDT_DrvDev[aID].state = WDT_STATE_IDLE;
            retVal = BCM_ERR_OK;
        } else {
            retVal = BCM_ERR_UNINIT;
        }
    }
    return retVal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion
    @trace #BRCM_SWARCH_WDT_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_WDT_KERNEL_HANDLER
*/
typedef union _WDT_SVCIOType {
    uint8_t     *data;
    WDT_IOType  *io;
} WDT_SVCIOType;

/**
    @code{.c}
    if aSysIO.wdtIO is not NULL
        if aMagicID is not BCM_WDT_SVC_MAGIC_ID
             wdtIO->retVal = WDT_CmdHandler(aCmd, wdtIO)
        else
            wdtIO->retVal = BCM_ERR_INVAL_MAGIC
    @endcode
    @trace #BRCM_SWARCH_WDT_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_WDT_KERNEL_HANDLER
*/
void WDT_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    WDT_SVCIOType wdt;
    wdt.data = aSysIO;
    int32_t retVal = BCM_ERR_OK;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_WDT_ID == aMagicID) {
            switch (aCmd) {
            case WDT_CMD_INIT:
                WDT_DrvInit(wdt.io->hwID, wdt.io->config);
                break;
            case WDT_CMD_SET_MODE:
                retVal = WDT_DrvSetMode(wdt.io->hwID, wdt.io->mode);
                break;
            case WDT_CMD_SET_TRIG_COND:
                retVal = WDT_DrvSetTriggerCondition(wdt.io->hwID, wdt.io->timeout_ms);
                break;
            case WDT_CMD_SERVICE:
                retVal = WDT_DrvService(wdt.io->hwID);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        wdt.io->retVal = retVal;
    }

    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_WDG_ID, 0U, BRCM_SWARCH_WDT_SYSCMDHANDLER_PROC, retVal,
                4UL, values);
    }
}

/** @} */
