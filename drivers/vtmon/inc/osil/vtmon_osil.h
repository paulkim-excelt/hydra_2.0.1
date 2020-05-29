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

/** @defgroup grp_vtmon_il Integration Guide
    @ingroup grp_vtmondrv

    @addtogroup grp_vtmon_il
    @{
    @section sec_vtmon_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system.

    This driver requires a task for deferred interrupt processing. This task
    shall be referred as SerialIO task. This task shall be signaled from VTMON
    interrupt handler after calling #VTMON_Drv_IRQHandler.

    VTMON driver is implemented with #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for VTMON Driver
    onto #SysCmdHandler(system command handler).

    @section sec_vtmon_il_seq Sequence Diagram
    @image html vtmon_seq_il_diagram_svc.jpg "vtmon_seq_il_diagram_svc.jpg"

    @file vtmon_osil.h
    @brief VTMON Driver Integration Interfaces
    This header file contains the integration interface for VTMON driver.
    @version 0.50 Imported from docx
*/

#ifndef VTMON_OSIL_H
#define VTMON_OSIL_H

#include <system.h>
#include <vtmon.h>

/**
    @name VTMON driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for VTMON driver
*/
#define BRCM_SWARCH_VTMON_CMD_TYPE                   (0x40U) /**< @brief #VTMON_CmdType         */
#define BRCM_SWARCH_VTMON_IO_TYPE                    (0x41U) /**< @brief #VTMON_IOType          */
#define BRCM_SWARCH_VTMON_SYSCMDHANDLER_PROC         (0x42U) /**< @brief #VTMON_SysCmdHandler   */
#define BRCM_SWARCH_VTMON_SYSCMDREQ_PROC             (0x43U) /**< @brief #VTMON_SysCmdReq       */
#define BRCM_SWARCH_VTMON_DRV_IRQHANDLER_PROC        (0x44U) /**< @brief #VTMON_DrvIRQHandler   */
#define BRCM_SWARCH_VTMON_IRQHANDLER_PROC            (0x45U) /**< @brief #VTMON_IRQHandler      */
#define BRCM_SWARCH_VTMON_TEMPNOTIFICATIONCB_PROC    (0x46U) /**< @brief #VTMON_TempNotification*/
#define BRCM_SWARCH_VTMON_VOLTNOTIFICATIONCB_PROC    (0x47U) /**< @brief #VTMON_VoltNotification*/
/** @} */

/**
    @name VTMON_CmdType
    @{
    @brief VTMON command type

    @trace #BRCM_SWREQ_VTMON_KERNEL_INTERFACE
*/
typedef uint32_t VTMON_CmdType;                 /**< @brief VTMON command type */
#define VTMON_CMD_POWER_UP              (0x0UL) /**< @brief VTMON command for
                                                     powering up VTMON */
#define VTMON_CMD_POWER_DOWN            (0x1UL) /**< @brief VTMON command for
                                                     powering down VTMON */
#define VTMON_CMD_GET_CURR_VOLT         (0x2UL) /**< @brief VTMON command to get
                                                     the current voltage */
#define VTMON_CMD_SET_VOLT_CONFIG       (0x3UL) /**< @brief VTMON command to set
                                                     the voltage configuration */
#define VTMON_CMD_GET_CURR_TEMP         (0x4UL) /**< @brief VTMON command to get
                                                     the current temperature */
#define VTMON_CMD_SET_TEMP_CONFIG       (0x5UL) /**< @brief VTMON command to set
                                                     the temperature configuration */
/** @} */

/**
    @brief VTMON IO structure

    VTMON IO structure to be used for #VTMON_CmdHandler

    @trace #BRCM_SWREQ_VTMON_KERNEL_INTERFACE
*/
typedef struct _VTMON_IOType {
    int32_t retVal;                 /**< @brief Return value from the command
                                         operation function */
    VTMON_TempType currTemp;        /**< @brief current temperature */
    VTMON_VoltType currVolt;        /**< @brief current voltage */
    VTMON_TempConfigType tempConfig;
    VTMON_VoltConfigType voltConfig;
} VTMON_IOType;

/** @brief VTMON system command handler

    This interface handles all the VTMON driver commands requested from
    #SysCmdReq.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMagicID    SVC Magic ID for VTMON
    @param[in]      aCmd        System command request
    @param[inout]   aSysIO      Pointer to system request IO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall hook
    it to SVC layer.

    @trace  #BRCM_SWREQ_VTMON_KERNEL_HANDLER

    @limitations None
*/
extern void VTMON_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief VTMON system command request

    Interface for VTMON system command request.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        VTMON command
    @param[inout]   aIO         Pointer to VTMON IO

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             VTMON command operation success
    @retval     Others                  Error values specific to the command
                                        requested
    @retval     #BCM_ERR_INVAL_PARAMS   aIO is NULL

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_VTMON_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t VTMON_SysCmdReq(VTMON_CmdType aCmd, VTMON_IOType *const aIO);

/** @brief VTMON Interrupt request handler

    This interface handles VTMON interrupts.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @note This interface shall be called from ISR handler for VTMON driver.

    @trace  #BRCM_SWREQ_VTMON_KERNEL_HANDLER

    @limitations None

    @snippet vtmon_osil.c Usage of VTMON_Drv_IRQHandler
*/
extern void VTMON_DrvIRQHandler(void);

/** @brief VTMON Interrupt request handler to be installed in vector table

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @note This interface shall be implemented by integrator and install it
    to the vector table.

    @trace  #BRCM_SWREQ_VTMON_KERNEL_HANDLER

    @limitations None
*/
extern void VTMON_IRQHandler(void);

/** @brief Temperature notification callback

    Callback interface to notify that the temperature has crossed the threshold
    set. It is mandatory for clients to implement this function.

    @behavior Sync, Re-entrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator. The integrator
    shall not set new threshold value in the callback.

    @trace  #BRCM_SWREQ_VTMON_NOTIFICATIONS

    @limitations None
*/
extern void VTMON_TempNotification(VTMON_TempType aCurrent,
                                    VTMON_TempType aLowThreshold,
                                    VTMON_TempType aHighThreshold);

/** @brief Voltage notification callback

    Callback interface to notify that the voltage has crossed the threshold set.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aVoltID     Voltage ID

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator. The integrator
    shall not set new threshold value in the callback.

    @trace  #BRCM_SWREQ_VTMON_NOTIFICATIONS

    @limitations None
*/
extern void VTMON_VoltNotification(VTMON_VoltIDType aVoltID,
                                        VTMON_VoltType aCurrent,
                                        VTMON_VoltType aLowThreshold,
                                        VTMON_VoltType aHighThreshold);

#endif /* VTMON_OSIL_H */
/** @} */
