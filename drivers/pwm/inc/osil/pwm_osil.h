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

/** @defgroup grp_pwmdrv_il Integration Guide
    @ingroup grp_pwmdrv

    @addtogroup grp_pwmdrv_il
    @{
    @section sec_pwm_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system. This section describes PWM driver Integration Layer in more
    detail.

    PWM driver is implemented with #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for PWM Driver onto
    #SysCmdHandler (system command handler).

    @section sec_pwm_il_seq Sequence Diagram
    The following sequence diagram depicts the sequence #PWM_Init as an example.
    All other PWM APIs shall follow the same sequence.
    @image html pwm_seq_il_diagram_init.jpg

    @file pwm_osil.h
    @brief PWM driver Integration Interfaces
    This header file contains the integration interfaces for PWM driver
    @version 0.70 Imported from docx
*/

#ifndef PWM_OSIL_H
#define PWM_OSIL_H

#include <system.h>
#include <pwm.h>

/**
    @name PWM driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for PWM driver
*/
#define BRCM_SWARCH_PWM_CMD_TYPE             (0x40U) /**< @brief #PWM_CmdType */
#define BRCM_SWARCH_PWM_IO_TYPE              (0x41U) /**< @brief #PWM_IOType */
#define BRCM_SWARCH_PWM_SYSCMDREQ_PROC       (0x42U) /**< @brief #PWM_SysCmdReq */
#define BRCM_SWARCH_PWM_SYS_CMD_HANDLER_PROC (0x43U) /**< @brief #PWM_SysCmdHandler */
/** @} */

/**
    @name PWM Commands
    @{
    @brief PWM system command request type

    @trace #BRCM_SWREQ_PWM_KERNEL_INTERFACE
*/
typedef uint32_t PWM_CmdType;   /**< @brief PWM command type */
#define PWM_CMD_INIT                   (1UL)
#define PWM_CMD_DEINIT                 (2UL)
#define PWM_CMD_START_CHANNEL          (3UL)
#define PWM_CMD_STOP_CHANNEL           (4UL)
#define PWM_CMD_SET_DUTY_CYCLE         (5UL)
#define PWM_CMD_SET_PERIOD_DUTY_CYCLE  (6UL)
/** @} */

/**
    @brief PWM IO structure

    PWM IO structure to be used for #PWM_CmdHandler.

    @trace #BRCM_SWREQ_PWM_KERNEL_INTERFACE
*/
typedef struct _PWM_IOType {
    int32_t retVal;                 /**< @brief Return value from the command
                                         internal function */
    PWM_HwIDType hwID;              /**< @brief PWM Hardware ID */
    PWM_ChanIDType chanID;          /**< @brief Channel ID */
    const PWM_ConfigType * config;  /**< @brief Pointer to PWM configuration */
    PWM_ValueType dutyCycle;        /**< @brief PWM Duty Cycle */
    PWM_ValueType period;           /**< @brief PWM Period */
} PWM_IOType;

/** @brief Raise PWM system command request

    Interface to raise PWM system command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aCmd        PWM system request command
    @param[in]      aIO         Pointer to #PWM_IOType

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request success
    @retval     Others                  As returned from #PWM_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid command
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PWM_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t PWM_SysCmdReq(PWM_CmdType aCmd, PWM_IOType *aPWMIO);


/** @brief Handle PWM system command request processing

    Interface to handle PWM system command request processing.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    PWM system request magic ID
    @param[in]      aCmd        PWM system request command
    @param[inout]   aSysIO      Pointer to system request IO for PWM_IOType


    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_PWM_KERNEL_HANDLER

    @limitations None
*/
extern void PWM_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);
#endif /* PWM_OSIL_H */
/** @} */
