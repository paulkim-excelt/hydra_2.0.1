/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_pwmdrv_drv_impl PWM Driver Design
    @ingroup grp_pwmdrv

    @addtogroup grp_pwmdrv_drv_impl
    @{

    @file drivers/pwm/lib/pwm_drv.c
    @brief PWM Driver implementation
    @version 0.1 Initial version
*/

#include <string.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <system.h>
#include <chip_config.h>
#include <osil/pwm_osil.h>
#include "pwm.h"
#include "pwm_rdb.h"

/**
    @name PWM driver implementation IDs
    @{
    @brief API IDs for PWM driver implementation
*/
#define BRCM_SWDSGN_PWM_REGS_MACRO                     (0xA0U) /**< @brief #PWM_REGS */
#define BRCM_SWDSGN_PWM_RODEV_TYPE                     (0xA1U) /**< @brief #PWM_RODevType */
#define BRCM_SWDSGN_PWM_DRV_CHANDEV_TYPE               (0xA2U) /**< @brief #PWM_DrvChanDevType */
#define BRCM_SWDSGN_PWM_DRV_RWDEV_TYPE                 (0xA3U) /**< @brief #PWM_DrvRWDevType */
#define BRCM_SWDSGN_PWM_DRV_RWDEV_GLOBAL               (0xA4U) /**< @brief #PWM_DrvRWDev */
#define BRCM_SWDSGN_PWM_RODEV_GLOBAL                   (0xA5U) /**< @brief #PWM_RODev */
#define BRCM_SWDSGN_PWM_DRV_SET_PDC_REGS_PROC          (0xA7U) /**< @brief #PWM_DrvSetPDCRegs */
#define BRCM_SWDSGN_PWM_DRV_INIT_PROC                  (0xA8U) /**< @brief #PWM_DrvInit */
#define BRCM_SWDSGN_PWM_DRV_DEINIT_PROC                (0xA9U) /**< @brief #PWM_DrvDeinit */
#define BRCM_SWDSGN_PWM_DRV_START_CHANNEL_PROC         (0xAAU) /**< @brief #PWM_DrvStartChannel */
#define BRCM_SWDSGN_PWM_DRV_STOP_CHANNEL_PROC          (0xABU) /**< @brief #PWM_DrvStopChannel */
#define BRCM_SWDSGN_PWM_DRV_SET_DUTY_CYCLE_PROC        (0xACU) /**< @brief #PWM_DrvSetDutyCycle */
#define BRCM_SWDSGN_PWM_DRV_SET_PERIOD_DUTY_CYCLE_PROC (0xADU) /**< @brief #PWM_DrvSetPeriodDutyCycle */
#define BRCM_SWDSGN_PWM_SVCIO_TYPE                     (0xAEU) /**< @brief #PWM_SVCIOType */
#define BRCM_SWDSGN_PWM_SYSCMDHANDLER_PROC             (0xAFU) /**< @brief #PWM_SysCmdHandler */
#define BRCM_SWDSGN_PWM_DRV_STATUS_TYPE                (0xB0U) /**< @brief #PWM_DrvStatusType */
#define BRCM_SWDSGN_PWM_DRV_CHANSTATE_TYPE             (0xB1U) /**< @brief #PWM_DrvChanStateType */
/** @} */

/**
    @brief PWM status type
    @{
    Type to PWM driver state

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
typedef uint32_t PWM_DrvStatusType;
#define PWM_DRV_STATUS_UNINIT      (0UL)
#define PWM_DRV_STATUS_INIT        (1UL)
/** @} */

/**
    @brief PWM Channel status type
    @{
    Type to PWM driver channel state

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
typedef uint32_t PWM_DrvChanStateType;
#define PWM_DRV_CHANSTATE_UNINIT       (0UL)
#define PWM_DRV_CHANSTATE_IDLE         (1UL)
#define PWM_DRV_CHANSTATE_RUNNING      (2UL)
/** @} */

/**
    @brief PWM Regs macro

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
#define PWM_REGS(id)           (PWM_RODev[id].REGS)

/**
    @brief PWM RO DevType

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
typedef struct _PWM_RODevType {
    PWM_RDBType *REGS;
} PWM_RODevType;

/**
    @brief PWM Drv ChanDevType

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
typedef struct _PWM_DrvChanDevType {
    PWM_ChanClassType class;
    PWM_DrvChanStateType state;
} PWM_DrvChanDevType;

/**
    @brief PWM Drv RW DevType

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
typedef struct _PWM_DrvRWDevType {
    uint32_t pwmStatus;
    PWM_DrvChanDevType chan[PWM_MAX_CHAN_CNT];
} PWM_DrvRWDevType;

/**
    @brief PWM Drv RW Dev

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
static PWM_DrvRWDevType PWM_DrvRWDev[PWM_MAX_HW_ID] COMP_SECTION(".data.drivers") =
{
#if (PWM_MAX_HW_ID == 0)
    #error "PWM_MAX_HW_ID == 0"
#endif
    {
     .pwmStatus = PWM_DRV_STATUS_UNINIT,
#if (PWM_MAX_CHAN_CNT == 0)
#error "PWM_MAX_CHAN_CNT == 0"
#endif
     .chan = {
#if (PWM_MAX_CHAN_CNT > 0)
                {
                    .class = PWM_CHANCLASS_VARIABLE,     /* chan[0UL].class */
                    .state = PWM_DRV_CHANSTATE_UNINIT    /* chan[0UL].state */
                },
#endif
#if (PWM_MAX_CHAN_CNT > 1)
                {
                    .class = PWM_CHANCLASS_VARIABLE,     /* chan[1UL].class */
                    .state = PWM_DRV_CHANSTATE_UNINIT    /* chan[1UL].state */
                },
#endif
#if (PWM_MAX_CHAN_CNT > 2)
                {
                    .class = PWM_CHANCLASS_VARIABLE,     /* chan[2UL].class */
                    .state = PWM_DRV_CHANSTATE_UNINIT    /* chan[2UL].state */
                },
#endif
#if (PWM_MAX_CHAN_CNT > 3)
                {
                    .class = PWM_CHANCLASS_VARIABLE,     /* chan[3UL].class */
                    .state = PWM_DRV_CHANSTATE_UNINIT    /* chan[3UL].state */
                },
#endif
            }
#if (PWM_MAX_CHAN_CNT > 4)
#error "PWM_MAX_CHAN_CNT > 4"
#endif
    }
#if (PWM_MAX_HW_ID > 1)
    #error "PWM_MAX_HW_ID > 1 is not supported by the driver"
#endif
};

/**
    @brief PWM Drv RO Dev

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
static const PWM_RODevType PWM_RODev[PWM_MAX_HW_ID] =
{
#if (PWM_MAX_HW_ID == 0)
    #error "PWM_MAX_HW_ID == 0"
#endif
    {.REGS = (PWM_RDBType *)PWM_BASE},
#if (PWM_MAX_HW_ID > 1)
    #error "PWM_MAX_HW_ID > 1 is not supported by the driver"
#endif
};

/**
    @brief PWM Drv Set Registers

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
static int32_t PWM_DrvSetPDCRegs(PWM_HwIDType aID, PWM_ChanIDType aChanID,
                                   PWM_ValueType aPeriod, PWM_ValueType aDutyCycle)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aChanID) {
        case 0UL:
            PWM_REGS(aID)->period_cnt_0 = (uint32_t)aPeriod;
            PWM_REGS(aID)->duty_cnt_0 = (uint32_t)aDutyCycle;
            break;
        case 1UL:
            PWM_REGS(aID)->period_cnt_1 = (uint32_t)aPeriod;
            PWM_REGS(aID)->duty_cnt_1 = (uint32_t)aDutyCycle;
            break;
        case 2UL:
            PWM_REGS(aID)->period_cnt_2 = (uint32_t)aPeriod;
            PWM_REGS(aID)->duty_cnt_2 = (uint32_t)aDutyCycle;
            break;
        case 3UL:
            PWM_REGS(aID)->period_cnt_3 = (uint32_t)aPeriod;
            PWM_REGS(aID)->duty_cnt_3 = (uint32_t)aDutyCycle;
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
    }
    return retVal;
}

/**
    @brief PWM Drv Init

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
static int32_t PWM_DrvInit(PWM_HwIDType aID, const PWM_ConfigType *const aConfig)
{
    uint32_t i;
    PWM_ChanCfgType * chanCfgTbl;
    uint32_t chanNum = 0UL;
    int32_t retVal = BCM_ERR_OK;

    if (PWM_DRV_STATUS_INIT == PWM_DrvRWDev[aID].pwmStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if ((PWM_MAX_HW_ID <= aID) || (NULL == aConfig)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    chanCfgTbl = aConfig->chanCfgTbl;

    if (NULL != chanCfgTbl) {
        for (i = 0UL; i < aConfig->chanCfgTblSz; i++) {
            if ((chanCfgTbl[i].chanID >= PWM_MAX_CHAN_CNT) ||
                (chanCfgTbl[i].mode > PWM_CHANMODE_OPEN_DRAIN) ||
                (chanCfgTbl[i].class > PWM_CHANCLASS_FIXED) ||
                (chanCfgTbl[i].polarity > PWM_CHANPOLARITY_ACTIVE_HIGH) ||
                (chanCfgTbl[i].prescaler > PWM_PRESCALE_MAX))
            {
                retVal = BCM_ERR_INVAL_PARAMS;
                goto err;
            }
        }

        for (i = 0UL; i < aConfig->chanCfgTblSz; i++) {
            chanNum = chanCfgTbl[i].chanID;

            PWM_DrvRWDev[aID].chan[chanNum].class = chanCfgTbl[i].class;

            PWM_REGS(aID)->pwmctl |= (PWM_CTL_OPENDRAIN_MASK & (chanCfgTbl[i].mode <<
                                     (PWM_CTL_OPENDRAIN_SHIFT + chanNum))) | (PWM_CTL_OUT_POLARITY_MASK &
                                     (chanCfgTbl[i].polarity << (PWM_CTL_OUT_POLARITY_SHIFT + chanNum)));

            PWM_REGS(aID)->prescale |= ((uint32_t)chanCfgTbl[i].prescaler << (PWM_PRESCALE_PWM0_PRESCALE_SHIFT -
                                       (chanNum * 6UL)));

            retVal = PWM_DrvSetPDCRegs(aID, chanNum, chanCfgTbl[i].period, chanCfgTbl[i].dutyCycle);
            if (BCM_ERR_OK != retVal) {
                break;
            }
            PWM_DrvRWDev[aID].chan[chanNum].state = PWM_DRV_CHANSTATE_IDLE;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK == retVal) {
        PWM_DrvRWDev[aID].pwmStatus = PWM_DRV_STATUS_INIT;
    }
err:
    return retVal;
}

/**
    @brief PWM Drv DeInit

    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
static int32_t PWM_DrvDeinit(PWM_HwIDType aID)
{
    int32_t retVal = BCM_ERR_OK;
    PWM_ChanIDType chanNum = 0UL;

    if (PWM_DRV_STATUS_INIT != PWM_DrvRWDev[aID].pwmStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (PWM_MAX_HW_ID <= aID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    for (chanNum = 0UL; chanNum < PWM_MAX_CHAN_CNT; chanNum++) {
        PWM_DrvRWDev[aID].chan[chanNum].state = PWM_DRV_CHANSTATE_UNINIT;
    }

    /* Disable PWM */
    PWM_REGS(aID)->pwmctl &= ~PWM_CTL_ENABLE_MASK;
    PWM_DrvRWDev[aID].pwmStatus = PWM_DRV_STATUS_UNINIT;

err:
    return retVal;
}

/**
    @brief PWM Drv StartChannel

    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

*/
static int32_t PWM_DrvStartChannel(PWM_HwIDType aID, PWM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;

    if ((PWM_MAX_HW_ID <= aID) || (PWM_MAX_CHAN_CNT <= aChanID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (PWM_DRV_STATUS_INIT != PWM_DrvRWDev[aID].pwmStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (PWM_DRV_CHANSTATE_IDLE != PWM_DrvRWDev[aID].chan[aChanID].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    PWM_DrvRWDev[aID].chan[aChanID].state = PWM_DRV_CHANSTATE_RUNNING;
    PWM_REGS(aID)->pwmctl |= PWM_CTL_ENABLE_MASK & (0x01UL << aChanID);

err:
    return retVal;
}

/**
    @brief PWM Drv StopChannel

    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS
*/
static int32_t PWM_DrvStopChannel(PWM_HwIDType aID, PWM_ChanIDType aChanID)
{
    int32_t retVal = BCM_ERR_OK;

    if ((PWM_MAX_HW_ID <= aID) || (PWM_MAX_CHAN_CNT <= aChanID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (PWM_DRV_STATUS_INIT != PWM_DrvRWDev[aID].pwmStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (PWM_DRV_CHANSTATE_RUNNING != PWM_DrvRWDev[aID].chan[aChanID].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    PWM_DrvRWDev[aID].chan[aChanID].state = PWM_DRV_CHANSTATE_IDLE;
    PWM_REGS(aID)->pwmctl &= ~(PWM_CTL_ENABLE_MASK & (0x01UL << aChanID));

err:
    return retVal;
}

/**
    @brief PWM Drv SetDutyCycle

    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

*/
static int32_t PWM_DrvSetDutyCycle(PWM_HwIDType aID, PWM_ChanIDType aChanID, PWM_ValueType aDutyCycle)
{
    int32_t retVal = BCM_ERR_OK;

    if ((PWM_MAX_HW_ID <= aID) || (PWM_MAX_CHAN_CNT <= aChanID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (PWM_DRV_STATUS_INIT != PWM_DrvRWDev[aID].pwmStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (PWM_DRV_CHANSTATE_IDLE != PWM_DrvRWDev[aID].chan[aChanID].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    switch(aChanID){
        case 0UL:
            PWM_REGS(aID)->duty_cnt_0 = (uint32_t)aDutyCycle;
            break;
        case 1UL:
            PWM_REGS(aID)->duty_cnt_1 = (uint32_t)aDutyCycle;
            break;
        case 2UL:
            PWM_REGS(aID)->duty_cnt_2 = (uint32_t)aDutyCycle;
            break;
        case 3UL:
            PWM_REGS(aID)->duty_cnt_3 = (uint32_t)aDutyCycle;
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
    }

err:
    return retVal;
}

/**
    @brief PWM Drv SetPeriodDutyCycle

    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS
*/
static int32_t PWM_DrvSetPeriodDutyCycle(PWM_HwIDType aID, PWM_ChanIDType aChanID,
                                         PWM_ValueType aPeriod, PWM_ValueType aDutyCycle)
{
    int32_t retVal = BCM_ERR_OK;

    if ((PWM_MAX_HW_ID <= aID) || (PWM_MAX_CHAN_CNT <= aChanID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (PWM_DRV_STATUS_INIT != PWM_DrvRWDev[aID].pwmStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if ((PWM_CHANCLASS_FIXED == PWM_DrvRWDev[aID].chan[aChanID].class) ||
        (PWM_DRV_CHANSTATE_IDLE != PWM_DrvRWDev[aID].chan[aChanID].state)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    retVal = PWM_DrvSetPDCRegs(aID, aChanID, aPeriod, aDutyCycle);

err:
    return retVal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_PWM_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PWM_KERNEL_INTERFACE
*/
typedef union _PWM_SVCIOType {
    uint8_t *data;
    PWM_IOType *io;
} PWM_SVCIOType;


/**
    @code{.c}
    if pwmIO is NOT NULL {
        if aMagicID is SVC_MAGIC_PWM_ID
            aSysIO.pwmIO->retVal = PWM_CmdHandler(aCmd, aSysIO.pwmIO)
        else
            aSysIO.pwmIO->retVal = BCM_ERR_INVAL_MAGIC
    }
    @endcode

    @trace #BRCM_SWARCH_PWM_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_PWM_KERNEL_HANDLER
*/
void PWM_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    PWM_SVCIOType pwm;
    pwm.data =aSysIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_PWM_ID == aMagicID) {
            switch (aCmd) {
                case PWM_CMD_INIT:
                    retVal = PWM_DrvInit(pwm.io->hwID, pwm.io->config);
                    break;
                case PWM_CMD_DEINIT:
                    retVal = PWM_DrvDeinit(pwm.io->hwID);
                    break;
                case PWM_CMD_START_CHANNEL:
                    retVal = PWM_DrvStartChannel(pwm.io->hwID, pwm.io->chanID);
                    break;
                case PWM_CMD_STOP_CHANNEL:
                    retVal = PWM_DrvStopChannel(pwm.io->hwID, pwm.io->chanID);
                    break;
                case PWM_CMD_SET_DUTY_CYCLE:
                    retVal = PWM_DrvSetDutyCycle(pwm.io->hwID, pwm.io->chanID, pwm.io->dutyCycle);
                    break;
                case PWM_CMD_SET_PERIOD_DUTY_CYCLE:
                    retVal = PWM_DrvSetPeriodDutyCycle(pwm.io->hwID, pwm.io->chanID, pwm.io->period,
                                                       pwm.io->dutyCycle);
                    break;
               default:
                    retVal = BCM_ERR_INVAL_PARAMS;
                    break;
                }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        pwm.io->retVal = retVal;
    }
    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, (uint32_t)aSysIO, 0UL, 0UL};
        BCM_ReportError(BCM_PWM_ID, 0U, (uint8_t)aCmd, retVal, 4UL, values);
    }
}

/** @} */
