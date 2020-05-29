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

/**
    @defgroup grp_pwmdrv_impl Implementation
    @ingroup grp_pwmdrv

    @addtogroup grp_pwmdrv_impl
    @{

    @file drivers/pwm/lib/pwm.c
    @brief PWM Driver implementation
    @version 0.1 Initial version
*/

#include <stdint.h>
#include <osil/pwm_osil.h>

/**
    @name PWM implementation IDs
    @{
    @brief API IDs for PWM driver implementation
*/
#define BRCM_SWDSGN_PWM_INIT_PROC                  (0x92U) /**< @brief #PWM_Init */
#define BRCM_SWDSGN_PWM_DEINIT_PROC                (0x93U) /**< @brief #PWM_DeInit */
#define BRCM_SWDSGN_PWM_START_CHANNEL_PROC         (0x94U) /**< @brief #PWM_StartChannel */
#define BRCM_SWDSGN_PWM_STOP_CHANNEL_PROC          (0x95U) /**< @brief #PWM_StopChannel */
#define BRCM_SWDSGN_PWM_SET_DUTY_CYCLE_PROC        (0x96U) /**< @brief #PWM_SetDutyCycle */
#define BRCM_SWDSGN_PWM_SET_PERIOD_DUTY_CYCLE_PROC (0x97U) /**< @brief #PWM_SetPeriodDutyCycle */
#define BRCM_SWDSGN_PWM_CMDHANDLER_PROC            (0x98U) /**< @brief #PWM_CmdHandler */
/** @} */


/**
    @brief PWM driver init function

    @trace #BRCM_SWARCH_PWM_INIT_PROC
    @trace #BRCM_SWREQ_PWM_INIT

*/
void PWM_Init(PWM_HwIDType aID, const PWM_ConfigType *const aConfig)
{
    int32_t retVal;
    PWM_IOType pwmIO;

    pwmIO.retVal = BCM_ERR_UNKNOWN;
    pwmIO.hwID = aID;
    pwmIO.config = aConfig;
    retVal = PWM_SysCmdReq(PWM_CMD_INIT, &pwmIO);
    (void)retVal;
}

/**
    @brief PWM driver de-init function

    @trace #BRCM_SWARCH_PWM_DEINIT_PROC
    @trace #BRCM_SWREQ_PWM_INIT
*/
void PWM_DeInit(PWM_HwIDType aID)
{
    int32_t retVal;
    PWM_IOType pwmIO;

    pwmIO.retVal = BCM_ERR_UNKNOWN;
    pwmIO.hwID = aID;
    retVal = PWM_SysCmdReq(PWM_CMD_DEINIT, &pwmIO);
    (void)retVal;
}

/**
    @brief PWM driver Start Channel function

    @trace #BRCM_SWARCH_PWM_START_CHANNEL_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

*/
int32_t PWM_StartChannel(PWM_HwIDType aID, PWM_ChanIDType aChanNum)
{
    int32_t retVal;
    PWM_IOType pwmIO;

    pwmIO.retVal = BCM_ERR_UNKNOWN;
    pwmIO.hwID = aID;
    pwmIO.chanID = aChanNum;
    retVal = PWM_SysCmdReq(PWM_CMD_START_CHANNEL, &pwmIO);

    return retVal;
}

/**
    @brief PWM driver Stop Channel function

    @trace #BRCM_SWARCH_PWM_STOP_CHANNEL_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

*/
int32_t PWM_StopChannel(PWM_HwIDType aID, PWM_ChanIDType aChanNum)
{
    int32_t retVal;
    PWM_IOType pwmIO;

    pwmIO.retVal = BCM_ERR_UNKNOWN;
    pwmIO.hwID = aID;
    pwmIO.chanID = aChanNum;
    retVal = PWM_SysCmdReq(PWM_CMD_STOP_CHANNEL, &pwmIO);

    return retVal;
}

/**
    @brief PWM driver Set Duty Cycle function

    @trace #BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

*/
int32_t PWM_SetDutyCycle(PWM_HwIDType aID, PWM_ChanIDType aChanNum, PWM_ValueType aDutyCycle)
{
    int32_t retVal;
    PWM_IOType pwmIO;

    pwmIO.retVal = BCM_ERR_UNKNOWN;
    pwmIO.hwID = aID;
    pwmIO.chanID = aChanNum;
    pwmIO.dutyCycle = aDutyCycle;
    retVal = PWM_SysCmdReq(PWM_CMD_SET_DUTY_CYCLE, &pwmIO);

    return retVal;
}

/**
    @brief PWM driver Set Period Duty Cycle function

    @trace #BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC
    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

*/
int32_t PWM_SetPeriodDutyCycle(PWM_HwIDType aID, PWM_ChanIDType aChanNum, PWM_ValueType aPeriod, PWM_ValueType aDutyCycle)
{
    int32_t retVal;
    PWM_IOType pwmIO;

    pwmIO.retVal = BCM_ERR_UNKNOWN;
    pwmIO.hwID = aID;
    pwmIO.chanID = aChanNum;
    pwmIO.period = aPeriod;
    pwmIO.dutyCycle = aDutyCycle;
    retVal = PWM_SysCmdReq(PWM_CMD_SET_PERIOD_DUTY_CYCLE, &pwmIO);

    return retVal;
}

/** @} */

