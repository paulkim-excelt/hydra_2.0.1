/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_wdtdrv_impl Implementation
    @ingroup grp_wdtdrv

    @addtogroup grp_wdtdrv_impl
    @{
*/
#include <stdlib.h>
#include <wdt.h>
#include <wdt_osil.h>

#include <bcm_err.h>

/**
    @name Watchdog driver internal API IDs
    @{
    @brief API IDs for Watchdog driver
*/
#define BRCM_SWDSGN_WDT_INIT_PROC                 (0x80U) /**< @brief #WDT_Init */
#define BRCM_SWDSGN_WDT_SETMODE_PROC              (0x81U) /**< @brief #WDT_SetMode */
#define BRCM_SWDSGN_WDT_SETTRIGGERCONDITION_PROC  (0x82U) /**< @brief #WDT_SetTriggerCondition */
#define BRCM_SWDSGN_WDT_SERVICE_PROC              (0x83U) /**< @brief #WDT_Service */
#define BRCM_SWDSGN_WDT_CMDHANDLER_PROC           (0x84U) /**< @brief #WDT_CmdHandler */

/** @} */

/**
    @brief Init routine for WDT driver

    @trace #BRCM_SWARCH_WDT_INIT_PROC
    @trace #BRCM_SWREQ_WDT_INIT
*/
void WDT_Init(WDT_HwIDType aID, const WDT_CfgType *const aConfig)
{
    WDT_IOType wdtIO;

    wdtIO.hwID = aID;
    wdtIO.config = aConfig;
    (void)WDT_SysCmdReq(WDT_CMD_INIT, &wdtIO);
}

/**
    @brief Set Mode for WDT driver

    @trace #BRCM_SWARCH_WDT_SET_MODE_PROC
    @trace #BRCM_SWREQ_WDT_DYNAMIC_CONFIG
*/
int32_t WDT_SetMode(WDT_HwIDType aID, WDT_ModeType aMode)
{
    int32_t retVal;
    WDT_IOType wdtIO;

    wdtIO.hwID = aID;
    wdtIO.mode = aMode;
    retVal = WDT_SysCmdReq(WDT_CMD_SET_MODE, &wdtIO);

    return retVal;
}

/**
    @brief Set time for WDT driver

    @trace #BRCM_SWARCH_WDT_SET_TRIGGER_CONDITION_PROC
    @trace #BRCM_SWREQ_WDT_DYNAMIC_CONFIG
*/
int32_t WDT_SetTriggerCondition(WDT_HwIDType aID, uint32_t aTimeout_ms)
{
    int32_t retVal;
    WDT_IOType wdtIO;

    wdtIO.hwID = aID;
    wdtIO.timeout_ms = aTimeout_ms;
    retVal = WDT_SysCmdReq(WDT_CMD_SET_TRIG_COND, &wdtIO);

    return retVal;
}

/**
    @brief Service routine for WDT driver

    @trace #BRCM_SWARCH_WDT_SERVICE_PROC
    @trace #BRCM_SWREQ_WDT_SERVICE
*/
int32_t WDT_Service(WDT_HwIDType aID)
{
    int32_t retVal;
    WDT_IOType wdtIO;

    wdtIO.hwID = aID;
    retVal = WDT_SysCmdReq(WDT_CMD_SERVICE, &wdtIO);

    return retVal;
}

/** @} */

