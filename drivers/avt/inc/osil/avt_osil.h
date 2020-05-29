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

/** @defgroup grp_avtdrv_il Integration Guide
    @ingroup grp_avtdrv

    @addtogroup grp_avtdrv_il
    @{
    @section sec_avt_integration Integration Guide
    AVT driver is implemented as a #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for AVT driver for
    privileged access.

    @section sec_avt_il_seq Sequence Diagram
    @subsection subsec_avt_il_seq_svc SVC Layer Interactions
    @image html avt_seq_il_diagram_svc.jpg "SVC Layer Interaction"
    A similar sequence is followed by all other AVT APIs except
    #AVT_GetSystemTime. For AVT_GetSystemTime sequence, refer to @refer
    subsec_avt_il_seq_get_sys_time

    @subsection subsec_avt_il_seq_get_sys_time AVT_GetSystemTime sequence
    @image html avt_seq_il_diagram_get_sys_time.jpg "AVT_GetSystemTime"

    @file avt_osil.h
    @brief AVT Integration Interfaces
    This header file contains the integration interfaces for AVT driver.
    @version 0.50 Imported from docx
*/

#ifndef AVT_OSIL_H
#define AVT_OSIL_H

#include <system.h>
#include <avt.h>

/**
    @name AVT Integration Interface IDs
    @{
    @brief Architecture IDs for AVT driver
*/
#define BRCM_SWARCH_AVT_CMD_TYPE                    (0x20U) /**< @brief #AVT_CmdType                */
#define BRCM_SWARCH_AVT_IO_TYPE                     (0x21U) /**< @brief #AVT_IOType                 */
#define BRCM_SWARCH_AVT_SYSCMDREQ_PROC              (0x22U) /**< @brief #AVT_SysCmdReq              */
#define BRCM_SWARCH_AVT_DRVIRQHANDLER_PROC          (0x23U) /**< @brief #AVT_DrvIRQHandler          */
#define BRCM_SWARCH_AVT_SNAPSHOT_IRQHANDLER_PROC    (0x24U) /**< @brief #AVT_SysSnapShotIRQHandler  */
#define BRCM_SWARCH_AVT_PERIODIC_IRQHANDLER_PROC    (0x25U) /**< @brief #AVT_SysPeriodicIRQHandler  */
#define BRCM_SWARCH_AVT_SYS_CMD_HANDLER_PROC        (0x26U) /**< @brief #AVT_SysCmdHandler          */
/** @} */

/**
    @name AVT_CmdType
    @{
    @brief AVT driver system request command type

    @trace #BRCM_SWREQ_AVT_KERNEL_INTERFACE
*/
typedef uint32_t AVT_CmdType;                     /**< @brief AVT driver system command type    */
#define AVT_CMD_ENABLE_MTS                  (0UL) /**< @brief Enable MTS                        */
#define AVT_CMD_DISABLE_MTS                 (1UL) /**< @brief Disable MTS                       */
#define AVT_CMD_GET_MTS_VAL                 (2UL) /**< @brief Get current MTS value             */
#define AVT_CMD_SET_PERIODIC_TIMER          (3UL) /**< @brief Set periodic timer                */
#define AVT_CMD_GET_TIME                    (4UL) /**< @brief Get current time                  */
#define AVT_CMD_RESET_1588                  (5UL) /**< @brief Reset 1588                        */
/** @} */

/**
    @brief IO structure for AVT driver

    @trace #BRCM_SWREQ_AVT_KERNEL_INTERFACE
 */
typedef struct _AVT_IOType {
    int32_t                 retVal;                     /**< @brief Return value set by
                                                                #AVT_SysCmdHandler                  */
    AVT_MTSChanIDType       channel;                    /**< @brief MTS channel ID                  */
    uint64_t                mtsSysTime;                 /**< @brief time for MTS and System time    */
    const AVT_MTSConfigType *mtsCfg;                    /**< @brief Pointer to MTS configuration    */
} AVT_IOType;

/** @brief Raise a command request to AVT driver

    This interface raises a command request to the AVT driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         Pointer to in-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request raised successfully
    @retval     Others                  As returned from #AVT_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_AVT_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t AVT_SysCmdReq(AVT_CmdType aCmd, AVT_IOType *const aIO);

/** @brief Handle AVT driver interrupts

    Interface to handle AVT driver interrupts.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param      void

    @return     void

    @post TBD

    @note  This interface shall be called from interrupt handlers for AVT
    driver.Integrator shall install the interrupt handlers for AVT driver in the
    vector table and enable interrupt (refer to @ref sec_ref_trm for interrupt
    number mappings).

    @trace #BRCM_SWREQ_AVT_KERNEL_HANDLER

    @limitations None

    @snippet avt_osil.c Usage of AVT_DrvIRQHandler
*/
extern void AVT_DrvIRQHandler(void);

/** @brief AVT Snapshot interrupt handler to be installed in vector table

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @return     void

    @post None

    @note This interface shall be implemented by the integrator and shall
    install it to interrupt vector table.

    @trace #BRCM_SWREQ_AVT_KERNEL_HANDLER

    @limitations None
*/
extern void AVT_SysSnapShotIRQHandler(void);

/** @brief AVT Periodic timer interrupt handler to be installed in vector table

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @return     void

    @post None

    @note This interface shall be implemented by the integrator and shall
    install it to interrupt vector table.

    @trace #BRCM_SWREQ_AVT_KERNEL_HANDLER

    @limitations None
*/
extern void AVT_SysPeriodicIRQHandler(void);

/** @brief Handle all the AVT driver commands

    This interface handles all the AVT driver commands requested from System
    command request.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre TBD

    @param[in]      aMagicID    AVT driver SVC Magic number
    @param[in]      aCmd        Command requested by system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as #AVT_IOType

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_AVT_KERNEL_HANDLER

    @limitations None
*/
extern void AVT_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO);

#endif /* AVT_OSIL_H */
/** @} */
