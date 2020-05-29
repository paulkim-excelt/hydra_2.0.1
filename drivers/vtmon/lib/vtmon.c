/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_vtmondrv_impl Implementation
    @ingroup grp_vtmondrv

    @addtogroup grp_vtmondrv_impl
    @{
*/

#include <stddef.h>
#include <bcm_err.h>
#include <system.h>
#include <vtmon_osil.h>
#include <vtmon.h>

/**
    @name VTMON driver Implementation IDs
    @{
    @brief Implementation IDs for VTMON driver
*/
#define BRCM_SWDSGN_VTMON_INIT_PROC             (0x10U) /**< @brief #VTMON_Init */
#define BRCM_SWDSGN_VTMON_DEINIT_PROC           (0x11U) /**< @brief #VTMON_Deinit */
#define BRCM_SWDSGN_VTMON_SET_TEMP_MONITOR_PROC (0x12U) /**< @brief #VTMON_SetTemperatureMonitor */
#define BRCM_SWDSGN_VTMON_GET_TEMP_PROC         (0x13U) /**< @brief #VTMON_GetTemperature */
#define BRCM_SWDSGN_VTMON_SET_VOLT_MONITOR_PROC (0x14U) /**< @brief #VTMON_SetVoltageMonitor */
#define BRCM_SWDSGN_VTMON_GET_VOLT_PROC         (0x15U) /**< @brief #VTMON_GetVoltage */
/** @} */

/**
    @trace  #BRCM_SWARCH_VTMON_GET_VOLT_PROC
    @trace  #BRCM_SWREQ_VTMON_TEMP_VOLTAGE
*/
int32_t VTMON_GetVoltage(VTMON_VoltConfigType *aVoltConfig,
                                 VTMON_VoltType *const aVolt)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    VTMON_IOType vtmonIO;

    if ((NULL != aVolt)
        && (NULL != aVoltConfig)) {
        vtmonIO.retVal = BCM_ERR_UNKNOWN;
        vtmonIO.voltConfig.ID = aVoltConfig->ID;
        retVal = VTMON_SysCmdReq(VTMON_CMD_GET_CURR_VOLT, &vtmonIO);
        if (BCM_ERR_OK == retVal) {
            *aVolt = vtmonIO.currVolt;
            aVoltConfig->threshLow = vtmonIO.voltConfig.threshLow;
            aVoltConfig->threshHigh = vtmonIO.voltConfig.threshHigh;
        }
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_VTMON_SET_VOLT_MONITOR_PROC
    @trace  #BRCM_SWREQ_VTMON_INIT
*/
int32_t VTMON_SetVoltageMonitor(const VTMON_VoltConfigType *aVoltConfig)
{
    int32_t retVal =  BCM_ERR_INVAL_PARAMS;
    VTMON_IOType vtmonIO;

    if (NULL != aVoltConfig) {
        vtmonIO.retVal = BCM_ERR_UNKNOWN;
        vtmonIO.voltConfig = *aVoltConfig;
        retVal = VTMON_SysCmdReq(VTMON_CMD_SET_VOLT_CONFIG, &vtmonIO);
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_VTMON_GET_TEMP_PROC
    @trace  #BRCM_SWREQ_VTMON_TEMP_VOLTAGE
*/
int32_t VTMON_GetTemperature(VTMON_TempConfigType *aTempConfig, VTMON_TempType *const aTemp)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    VTMON_IOType vtmonIO;

    if ((NULL != aTemp)
        && (NULL != aTempConfig)) {
        vtmonIO.retVal = BCM_ERR_UNKNOWN;
        retVal = VTMON_SysCmdReq(VTMON_CMD_GET_CURR_TEMP, &vtmonIO);
        if (BCM_ERR_OK == retVal) {
            *aTemp = vtmonIO.currTemp;
            *aTempConfig = vtmonIO.tempConfig;
        }
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_VTMON_SET_TEMP_MONITOR_PROC
    @trace  #BRCM_SWREQ_VTMON_INIT
*/
int32_t VTMON_SetTemperatureMonitor(const VTMON_TempConfigType *aTempConfig)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    VTMON_IOType vtmonIO;

    if (NULL != aTempConfig) {
        vtmonIO.retVal = BCM_ERR_UNKNOWN;
        vtmonIO.tempConfig = *aTempConfig;
        retVal = VTMON_SysCmdReq(VTMON_CMD_SET_TEMP_CONFIG, &vtmonIO);
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_VTMON_DEINIT_PROC
    @trace  #BRCM_SWREQ_VTMON_INIT
*/
void VTMON_Deinit(void)
{
    int32_t retVal;
    VTMON_IOType vtmonIO;

    vtmonIO.retVal = BCM_ERR_UNKNOWN;
    retVal = VTMON_SysCmdReq(VTMON_CMD_POWER_DOWN, &vtmonIO);
    (void) retVal;
}

/**
    @trace  #BRCM_SWARCH_VTMON_INIT_PROC
    @trace  #BRCM_SWREQ_VTMON_INIT
*/
void VTMON_Init(const VTMON_ConfigType *const aConfig)
{
    int32_t retVal;
    uint32_t i;
    VTMON_IOType vtmonIO;

    if (NULL != aConfig) {
        retVal = VTMON_SetTemperatureMonitor(&aConfig->tempConfig);
        if (BCM_ERR_OK == retVal) {
            for (i = 0UL; i < VTMON_VOLT_ID_MAX; i++) {
                retVal = VTMON_SetVoltageMonitor(&aConfig->voltConfig[i]);
                (void) retVal;
            }
        }
        vtmonIO.retVal = BCM_ERR_UNKNOWN;
        retVal = VTMON_SysCmdReq(VTMON_CMD_POWER_UP, &vtmonIO);
    }
    (void) retVal;
}

/** @} */
