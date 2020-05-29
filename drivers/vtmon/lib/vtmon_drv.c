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

/**
    @defgroup grp_vtmondrv_drv_impl Driver Implementation
    @ingroup grp_vtmondrv

    @addtogroup grp_vtmondrv_drv_impl
    @{
*/

#include <stddef.h>

#include <bcm_err.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <vtmon_osil.h>
#include <vtmon_rdb.h>

/**
    @name VTMON driver Implementation IDs
    @{
    @brief Implementation IDs for VTMON driver
*/
#define BRCM_SWDSGN_VTMON_TEMP_MACRO            (0x90U) /**< @brief #VTMON_TEMP_MIN         */
#define BRCM_SWDSGN_VTMON_VOLT_MACRO            (0x91U) /**< @brief #VTMON_VOLT_V1P0_MAX    */
#define BRCM_SWDSGN_VTMON_DRV_GET_VOLTAGE_PROC  (0x92U) /**< @brief #VTMON_DrvGetVoltage    */
#define BRCM_SWDSGN_VTMON_DRV_SET_VOLTAGE_PROC  (0x93U) /**< @brief #VTMON_DrvSetVoltage    */
#define BRCM_SWDSGN_VTMON_DRV_GET_TEMP_PROC     (0x94U) /**< @brief #VTMON_DrvGetTemperature*/
#define BRCM_SWDSGN_VTMON_DRV_SET_TEMP_PROC     (0x95U) /**< @brief #VTMON_DrvSetTemperature*/
#define BRCM_SWDSGN_VTMON_DRV_POWER_UP_PROC     (0x96U) /**< @brief #VTMON_DrvPowerUp       */
#define BRCM_SWDSGN_VTMON_DRV_POWER_DOWN_PROC   (0x97U) /**< @brief #VTMON_DrvPowerDown     */
#define BRCM_SWDSGN_VTMON_SVCIO_TYPE            (0x98U) /**< @brief #VTMON_SVCIOType        */
#define BRCM_SWDSGN_VTMON_SYSCMDHANDLER_PROC    (0x99U) /**< @brief #VTMON_SysCmdHandler    */
#define BRCM_SWDSGN_VTMON_DRV_IRQHANDLER_PROC   (0x9AU) /**< @brief #VTMON_DrvIRQHandler    */
/** @} */

/**
    @trace #BRCM_SWARCH_VTMON_SET_TEMP_MONITOR_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_INIT
*/
#define VTMON_TEMP_MIN    VTMON_TEMP(VTMON_TEMP_THRESH_LOW_LOW_MASK)
#define VTMON_TEMP_MAX    VTMON_TEMP(0)

/**
    @trace #BRCM_SWARCH_VTMON_SET_VOLT_MONITOR_PROC
    @trace #BRCM_SWARCH_VTMON_GET_VOLT_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_INIT
*/
#define VTMON_VOLT_V1P0_MAX  VTMON_VOLT_V1P0(VTMON_V1P0_THRESH_HIGH_V1P0_THRESH_HIGH_MASK)
#define VTMON_VOLT_V1P8_MAX  VTMON_VOLT_V1P8(VTMON_V1P8_THRESH_HIGH_V1P0_HIGH_MASK)
#define VTMON_VOLT_V3P3_2P5_MAX  VTMON_VOLT_V3P3_2P5(VTMON_V3P3_2P5_THRESH_HIGH_V1P0_HIGH_MASK)

/**
    @trace #BRCM_SWARCH_VTMON_GET_VOLT_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_TEMP_VOLTAGE
*/
static int32_t VTMON_DrvGetVoltage(VTMON_VoltConfigType *aVoltConfig,
                                VTMON_VoltType *const aVolt)
{
    int32_t err = BCM_ERR_OK;
    VTMON_RDBType *const vtmonRegs = (VTMON_RDBType *)VTMON_BASE;
    uint16_t control = (vtmonRegs->vtmon_cntrl & VTMON_CNTRL_PWRDN_MASK);

    if (VTMON_CNTRL_PWRDN_MASK == control) {
        err = BCM_ERR_INVAL_STATE;
    } else {
        switch (aVoltConfig->ID) {
        case VTMON_VOLT_ID_1V0:
            aVoltConfig->threshLow = VTMON_VOLT_V1P0(vtmonRegs->v1p0_thresh_low);
            aVoltConfig->threshHigh = VTMON_VOLT_V1P0(vtmonRegs->v1p0_thresh_high);
            *aVolt = VTMON_VOLT_V1P0(vtmonRegs->v1p0_data);
        break;
        case VTMON_VOLT_ID_1V8:
            aVoltConfig->threshLow = VTMON_VOLT_V1P8(vtmonRegs->v1p8_thresh_low);
            aVoltConfig->threshHigh = VTMON_VOLT_V1P8(vtmonRegs->v1p8_thresh_high);
            *aVolt = VTMON_VOLT_V1P8(vtmonRegs->v1p8_data);
        break;
        case VTMON_VOLT_ID_3V3:
            aVoltConfig->threshLow = VTMON_VOLT_V3P3_2P5(vtmonRegs->v3p3_2p5_thresh_low);
            aVoltConfig->threshHigh = VTMON_VOLT_V3P3_2P5(vtmonRegs->v3p3_2p5_thresh_high);
            *aVolt = VTMON_VOLT_V3P3_2P5(vtmonRegs->v3p3_2p5_data);
        break;
        default:
            err = BCM_ERR_INVAL_PARAMS;
            break;
        }
    }

    return err;
}

/**
    @trace #BRCM_SWARCH_VTMON_SET_VOLT_MONITOR_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_INIT
*/
static int32_t VTMON_DrvSetVoltage(VTMON_VoltConfigType *aVoltConfig)
{
    int32_t err = BCM_ERR_OK;
    VTMON_RDBType *const vtmonRegs = (VTMON_RDBType *)VTMON_BASE;

    switch (aVoltConfig->ID) {
    case VTMON_VOLT_ID_1V0:
        {
            VTMON_V1P0_THRESH_LOW_TYPE low = VTMON_VOLT_V1P0_TOREG(aVoltConfig->threshLow);
            VTMON_V1P0_THRESH_HIGH_TYPE high = VTMON_VOLT_V1P0_TOREG(aVoltConfig->threshHigh);
            if ((aVoltConfig->threshLow < aVoltConfig->threshHigh)
                && (aVoltConfig->threshHigh <= VTMON_VOLT_V1P0_MAX)){
                vtmonRegs->v1p0_thresh_low = low;
                vtmonRegs->v1p0_thresh_high = high;
                vtmonRegs->vtmon_intr &= (uint16_t)(~(VTMON_INTR_V1P0_MASK_MASK));
            } else {
                err = BCM_ERR_INVAL_PARAMS;
            }
        }
    break;
    case VTMON_VOLT_ID_1V8:
        {
            VTMON_V1P8_THRESH_LOW_TYPE low = VTMON_VOLT_V1P8_TOREG(aVoltConfig->threshLow);
            VTMON_V1P8_THRESH_HIGH_TYPE high = VTMON_VOLT_V1P8_TOREG(aVoltConfig->threshHigh);
            if ((aVoltConfig->threshLow < aVoltConfig->threshHigh)
                && (aVoltConfig->threshHigh <= VTMON_VOLT_V1P8_MAX)){
                vtmonRegs->v1p8_thresh_low = low;
                vtmonRegs->v1p8_thresh_high = high;
                vtmonRegs->vtmon_intr &= (uint16_t)(~(VTMON_INTR_V1P8_MASK_MASK));
            } else {
                err = BCM_ERR_INVAL_PARAMS;
            }
        }
    break;
    case VTMON_VOLT_ID_3V3:
        {
            VTMON_V3P3_2P5_THRESH_LOW_TYPE low = VTMON_VOLT_V3P3_2P5_TOREG(aVoltConfig->threshLow);
            VTMON_V3P3_2P5_THRESH_HIGH_TYPE high = VTMON_VOLT_V3P3_2P5_TOREG(aVoltConfig->threshHigh);
            if ((aVoltConfig->threshLow < aVoltConfig->threshHigh)
                && (aVoltConfig->threshHigh <= VTMON_VOLT_V3P3_2P5_MAX)){
                vtmonRegs->v3p3_2p5_thresh_low = low;
                vtmonRegs->v3p3_2p5_thresh_high = high;
                vtmonRegs->vtmon_intr &= (uint16_t)(~(VTMON_INTR_V3P3_2P5_MASK_MASK));
            } else {
                err = BCM_ERR_INVAL_PARAMS;
            }
        }
    break;
    default:
        err = BCM_ERR_INVAL_PARAMS;
        break;
    }

    if (BCM_ERR_OK != err) {
        const uint32_t values[4] = {aVoltConfig->ID, aVoltConfig->threshLow, aVoltConfig->threshHigh, 0UL};
        BCM_ReportError(BCM_VTM_ID, 0U, BRCM_SWDSGN_VTMON_DRV_SET_VOLTAGE_PROC, err,
                4UL, values);
    }
    return err;
}

/**
    @trace #BRCM_SWARCH_VTMON_GET_TEMP_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_TEMP_VOLTAGE
*/
static int32_t VTMON_DrvGetTemperature(VTMON_TempConfigType *aTempConfig, VTMON_TempType *const aTemp)
{
    int32_t err = BCM_ERR_OK;
    VTMON_RDBType *const vtmonRegs = (VTMON_RDBType *)VTMON_BASE;
    uint16_t control = (vtmonRegs->vtmon_cntrl & VTMON_CNTRL_PWRDN_MASK);

    if (VTMON_CNTRL_PWRDN_MASK == control) {
        err = BCM_ERR_INVAL_STATE;
    } else {
        aTempConfig->threshLow = VTMON_TEMP(vtmonRegs->temp_thresh_low);
        aTempConfig->threshHigh = VTMON_TEMP(vtmonRegs->temp_thresh_high);
        *aTemp = VTMON_TEMP((uint16_t)(vtmonRegs->temp_data & VTMON_TEMP_DATA_TEMP_DATA_MASK));
    }

    return err;
}

/**
    @trace #BRCM_SWARCH_VTMON_SET_TEMP_MONITOR_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_INIT
*/
static int32_t VTMON_DrvSetTemperature(VTMON_TempConfigType *aTempConfig)
{
    int32_t err = BCM_ERR_OK;
    VTMON_RDBType *const vtmonRegs = (VTMON_RDBType *)VTMON_BASE;

    uint16_t tempLow = VTMON_TEMP_TOREG(aTempConfig->threshLow);
    uint16_t tempHigh = VTMON_TEMP_TOREG(aTempConfig->threshHigh);
    if ((VTMON_TEMP_MIN <= aTempConfig->threshLow)
        && (aTempConfig->threshLow < aTempConfig->threshHigh)
        && (aTempConfig->threshHigh <= VTMON_TEMP_MAX)) {
        vtmonRegs->temp_thresh_low = tempLow;
        vtmonRegs->temp_thresh_high = tempHigh;
        vtmonRegs->vtmon_intr &= (uint16_t)(~(VTMON_INTR_TEMP_MASK_MASK));
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != err) {
        const uint32_t values[4] = {(uint32_t)aTempConfig->threshLow,
                                    (uint32_t)aTempConfig->threshHigh, 0UL, 0UL};
        BCM_ReportError(BCM_VTM_ID, 0U, BRCM_SWDSGN_VTMON_DRV_SET_TEMP_PROC, err, 4UL, values);
    }
    return err;
}

/**
    @trace #BRCM_SWARCH_VTMON_DEINIT_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_INIT
*/
static int32_t VTMON_DrvPowerDown(void)
{
    /* Monitor power down */
    VTMON_RDBType *const vtmonRegs = (VTMON_RDBType *)VTMON_BASE;
    vtmonRegs->vtmon_intr |= (VTMON_INTR_V3P3_2P5_MASK_MASK
                                    | VTMON_INTR_V1P8_MASK_MASK
                                    | VTMON_INTR_V1P0_MASK_MASK
                                    | VTMON_INTR_TEMP_MASK_MASK);
    vtmonRegs->vtmon_cntrl = (VTMON_CNTRL_PWRDN_MASK <<
            VTMON_CNTRL_PWRDN_SHIFT);
    return BCM_ERR_OK;
}

/**
    @trace #BRCM_SWARCH_VTMON_INIT_PROC
    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_INIT
*/
static int32_t VTMON_DrvPowerUp(void)
{
    uint16_t cntrl;
    VTMON_RDBType *const vtmonRegs = (VTMON_RDBType *)VTMON_BASE;
    uint16_t mask = vtmonRegs->vtmon_intr & (VTMON_INTR_TEMP_MASK_MASK
                                                | VTMON_INTR_V1P0_MASK_MASK
                                                | VTMON_INTR_V1P8_MASK_MASK
                                                | VTMON_INTR_V3P3_2P5_MASK_MASK);
    /* Disable interrupts */
    vtmonRegs->vtmon_intr |= (VTMON_INTR_TEMP_MASK_MASK
                                    | VTMON_INTR_V1P0_MASK_MASK
                                    | VTMON_INTR_V1P8_MASK_MASK
                                    | VTMON_INTR_V3P3_2P5_MASK_MASK);
    cntrl = vtmonRegs->vtmon_cntrl;
    cntrl &= (uint16_t)(~(VTMON_CNTRL_SEL_MASK));
    cntrl |= VTMON_ALL_MON << VTMON_CNTRL_SEL_SHIFT;
    cntrl &= (uint16_t)(~VTMON_CNTRL_PWRDN_MASK);
    vtmonRegs->vtmon_cntrl = cntrl;

    /* Enable interrupts */
    vtmonRegs->vtmon_intr &= (mask | (uint16_t)(~((VTMON_INTR_TEMP_MASK_MASK
                                            | VTMON_INTR_V1P0_MASK_MASK
                                            | VTMON_INTR_V1P8_MASK_MASK
                                            | VTMON_INTR_V3P3_2P5_MASK_MASK))));

    return BCM_ERR_OK;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_VTMON_KERNEL_INTERFACE
*/
typedef union _VTMON_SVCIOType {
    uint8_t *data;
    VTMON_IOType *io;
} VTMON_SVCIOType;

/**
    @code{.c}
    if aSysIO.vtmonIO is not NULL
        if aMagicID is SVC_MAGIC_VTM_ID
            vtmonIO->retVal = VTMON_CmdHandler(aCmd, vtmonIO)
        else
            vtmonIO->retVal = BCM_ERR_INVAL_MAGIC;
    @endcode

    @trace #BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_KERNEL_HANDLER
*/

void VTMON_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    VTMON_SVCIOType vtmon;
    vtmon.data = aSysIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_VTM_ID) {
            switch (aCmd) {
            case VTMON_CMD_POWER_UP:
                retVal = VTMON_DrvPowerUp();
                break;
            case VTMON_CMD_POWER_DOWN:
                retVal = VTMON_DrvPowerDown();
                break;
            case VTMON_CMD_GET_CURR_VOLT:
                retVal = VTMON_DrvGetVoltage(&vtmon.io->voltConfig, &vtmon.io->currVolt);
                break;
            case VTMON_CMD_SET_VOLT_CONFIG:
                retVal = VTMON_DrvSetVoltage(&vtmon.io->voltConfig);
                break;
            case VTMON_CMD_GET_CURR_TEMP:
                retVal = VTMON_DrvGetTemperature(&vtmon.io->tempConfig, &vtmon.io->currTemp);
                break;
            case VTMON_CMD_SET_TEMP_CONFIG:
                retVal = VTMON_DrvSetTemperature(&vtmon.io->tempConfig);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
            vtmon.io->retVal = retVal;
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
    }
    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_VTM_ID, 0U, BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC, retVal,
                4UL, values);
    }
}

/**
    @trace #BRCM_SWARCH_VTMON_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_VTMON_KERNEL_HANDLER
*/
void VTMON_DrvIRQHandler(void)
{
    VTMON_RDBType *const vtmonRegs = (VTMON_RDBType *)VTMON_BASE;
    VTMON_INTR_TYPE intrStatus = vtmonRegs->vtmon_intr;

    /* Mask interrupt and Notify client */
    if (VTMON_INTR_TEMP_INTR_MASK == (intrStatus & VTMON_INTR_TEMP_INTR_MASK)) {
        VTMON_TempType cur, low, high;
        cur = VTMON_TEMP((uint16_t)(vtmonRegs->temp_data & VTMON_TEMP_DATA_TEMP_DATA_MASK));
        low = VTMON_TEMP(vtmonRegs->temp_thresh_low);
        high = VTMON_TEMP(vtmonRegs->temp_thresh_high);
        VTMON_TempNotification(cur, low, high);
        vtmonRegs->vtmon_intr |= VTMON_INTR_TEMP_MASK_MASK;
    }
    if (VTMON_INTR_V1P0_INTR_MASK == (intrStatus & VTMON_INTR_V1P0_INTR_MASK)) {
        VTMON_VoltType cur, low, high;
        cur = VTMON_VOLT_V1P0(vtmonRegs->v1p0_data);
        low = VTMON_VOLT_V1P0(vtmonRegs->v1p0_thresh_low);
        high = VTMON_VOLT_V1P0(vtmonRegs->v1p0_thresh_high);
        VTMON_VoltNotification(VTMON_VOLT_ID_1V0, cur, low, high);
        vtmonRegs->vtmon_intr |= VTMON_INTR_V1P0_MASK_MASK;
    }
    if (VTMON_INTR_V1P8_INTR_MASK == (intrStatus & VTMON_INTR_V1P8_INTR_MASK)) {
        VTMON_VoltType cur, low, high;
        cur = VTMON_VOLT_V1P8(vtmonRegs->v1p8_data);
        low = VTMON_VOLT_V1P8(vtmonRegs->v1p8_thresh_low);
        high = VTMON_VOLT_V1P8(vtmonRegs->v1p8_thresh_high);
        VTMON_VoltNotification(VTMON_VOLT_ID_1V8, cur, low, high);
        vtmonRegs->vtmon_intr |= VTMON_INTR_V1P8_MASK_MASK;
    }
    if (VTMON_INTR_V3P3_2P5_INTR_MASK == (intrStatus & VTMON_INTR_V3P3_2P5_INTR_MASK)) {
        VTMON_VoltType cur, low, high;
        cur = VTMON_VOLT_V3P3_2P5(vtmonRegs->v3p3_2p5_data);
        low = VTMON_VOLT_V3P3_2P5(vtmonRegs->v3p3_2p5_thresh_low);
        high = VTMON_VOLT_V3P3_2P5(vtmonRegs->v3p3_2p5_thresh_high);
        VTMON_VoltNotification(VTMON_VOLT_ID_3V3, cur, low, high);
        vtmonRegs->vtmon_intr |= VTMON_INTR_V3P3_2P5_MASK_MASK;
    }
}

/** @} */
