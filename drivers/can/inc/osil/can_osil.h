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

/** @defgroup grp_candrv_il Integration
    @ingroup grp_candrv

    @addtogroup grp_candrv_il
    @{
    @section sec_can_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system. This section describes CAN driver Integration Layer (IL)
    in more detail.

    CAN driver requires a task for deferred CAN processing, from which the TX
    and RX client callbacks are called. This task shall be called as CommsTask
    further in this document. CommsTask shall be signaled from the CAN IRQ
    handler and shall have a higher priority than the client tasks.
    This section gives the different integration interfaces that shall be used
    to implement the CAN IRQ handler and the CommsTask.

    CAN driver is implemented with #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for CAN driver on
    to #SysCmdHandler for privileged access.

    @section sec_can_il_seq Sequence Diagram
    @subsection subsec_can_il_seq_tx CAN Transmit
    This sequence diagram describes the sequence of CAN Driver synchronous
    operation #CAN_DrvWrite. All the other CAN APIs follow a similar sequence as
    this sequence diagram .
    @image html can_seq_il_diagram_tx.jpg "CAN Driver Write Sequence"

    @subsection subsec_can_il_seq_rx CAN Receive
    This sequence diagram describes the sequence of CAN driver message reception
    and interaction with CommsTask.
    @image html can_seq_il_diagram_rx.jpg "CAN Driver Receive Sequence"

    @file can_osil.h
    @brief CAN drover  Integration Interfaces
    This header file contains the integration interfaces for CAN driver
    @version 0.96 Imported from docx
*/

#ifndef CAN_OSIL_H
#define CAN_OSIL_H

#include <can.h>
#include <system.h>

/**
    @name CAN driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for CAN driver
*/
#define BRCM_SWARCH_CAN_CMD_TYPE                        (0x40U) /**< @brief #CAN_CmdType                    */
#define BRCM_SWARCH_CAN_IO_TYPE                         (0x41U) /**< @brief #CAN_IOType                     */
#define BRCM_SWARCH_CAN_RXIND_PROC                      (0x42U) /**< @brief #CAN_RxInd                      */
#define BRCM_SWARCH_CAN_TXDONEIND_PROC                  (0x43U) /**< @brief #CAN_TxDoneInd                  */
#define BRCM_SWARCH_CAN_BUSOFFIND_PROC                  (0x44U) /**< @brief #CAN_BusOffInd                  */
#define BRCM_SWARCH_CAN_ERRORIND_PROC                   (0x45U) /**< @brief #CAN_ErrorInd                   */
#define BRCM_SWARCH_CAN_IRQLINE0HANDLERINTERNAL_PROC    (0x46U) /**< @brief #CAN_IRQLine0HandlerInternal    */
#define BRCM_SWARCH_CAN_IRQLINE1HANDLERINTERNAL_PROC    (0x47U) /**< @brief #CAN_IRQLine1HandlerInternal    */
#define BRCM_SWARCH_CAN_PROCESSPACKETS_PROC             (0x48U) /**< @brief #CAN_ProcessPkts                */
#define BRCM_SWARCH_CAN_SYSCMDREQ_PROC                  (0x49U) /**< @brief #CAN_SysCmdReq                  */
#define BRCM_SWARCH_CAN_SYSCMDHANDLER_PROC              (0x4AU) /**< @brief #CAN_SysCmdHandler              */
/** @} */

/**
    @name CAN IO Command type
    @{
    @brief CAN IO command values

    @trace  #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_CmdType;               /**< @brief Command type         */
#define CAN_CMD_INIT               (0UL)    /**< @brief Init command         */
#define CAN_CMD_DEINIT             (1UL)    /**< @brief Deinit command       */
#define CAN_CMD_WRITE              (2UL)    /**< @brief Write command        */
#define CAN_CMD_SLEEP              (3UL)    /**< @brief Sleep command        */
#define CAN_CMD_WAKEUP             (4UL)    /**< @brief Wakeup command       */
#define CAN_CMD_GET_ERR_CNTS       (5UL)    /**< @brief GetErrCnts command   */
#define CAN_CMD_GET_RX_PKT         (6UL)    /**< @brief GetRxPkt command     */
#define CAN_CMD_DEQUEUE_RX_PKT     (7UL)    /**< @brief DeQueRxPkt command   */
#define CAN_CMD_GET_TX_PKT         (8UL)    /**< @brief GetTxPkt command     */
#define CAN_CMD_GET_ERR_CODE       (10UL)   /**< @brief GetErrCode command   */
#define CAN_CMD_BUS_OFF            (12UL)   /**< @brief GetErrCode command   */
#define CAN_CMD_BUS_OFF_RECOVERY   (13UL)   /**< @brief DeQueErrCode command */
/** @} */

/**
    @brief CAN IO structure

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_IOType {
    int32_t                 retVal;         /**< @brief SVC Response                            */
    CAN_HwIDType            hwID;           /**< @brief Index of CAN controller                 */
    const CAN_ConfigType    *configData;    /**< @brief Pointer to CAN configuration data       */
    CAN_TxMsgObjType        *txObj;         /**< @brief Pointer to TX message object            */
    CAN_RxMsgObjType        *rxObj;         /**< @brief Pointer to RX message object            */
    CAN_ErrCntType          *errCntDesc;    /**< @brief Pointer to Error Counter Info object    */
    CAN_ErrorType           *errCode;       /**< @brief Pointer to Error Code                   */
    uint32_t                *raiseCb;       /**< @brief Pointer to callback flag                */
} CAN_IOType;

/** @brief Notify received CAN message

    Callback invoked by CAN driver to notify client about a received message.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN Controller
    @param[in]      aMsgInfo    Pointer to received message

    @return     void

    @post TBD

    @note This interface shall be implemented by the client to handle the
    reception of Standard and Extended CAN messages.

    @trace  #BRCM_SWREQ_CAN_NOTIFICATION

    @limitations None

    Below is a reference implementation. For example, client task (CLIENT_TASK)
    waits for CAN_RX_MSG_EVENT and CAN_ERR_EVENT when waiting for CAN data.
    After successful reception of CAN message client task shall be woken up by
    setting CAN_RX_MSG_EVENT in the #CAN_RxInd callback function.

    @code{.c}
    BCM_SetEvent(CLIENT_TASK, CAN_RX_MSG_EVENT)
    @endcode
*/
extern void CAN_RxInd(CAN_HwIDType aID, const CAN_RxMsgObjType * const aMsgInfo);

/** @brief Notify about transmitted message

    Callback invoked by CAN driver to notify client that a message is
    transmitted over CAN via the TX FIFO / queue or dedicated TX buffer.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller
    @param[in]      aXmtdObj    Pointer to object containing transmitted info

    @return     void

    @post TBD

    @note This interface shall be implemented by the client.

    @trace  #BRCM_SWREQ_CAN_NOTIFICATION

    @limitations None

    Below is a reference implementation of #CAN_TxDoneInd. For example,
    Client task (CLIENT_TASK) waits for CAN_TX_DONE_EVENT & CAN_ERR_EVENT when
    waiting for CAN data. After successful reception of CAN message client task
    shall be woken up by setting CAN_TX_DONE_EVENT in the #CAN_TxDoneInd
    callback function.

    @code{.c}
    BCM_SetEvent(CLIENT_TASK, CAN_TX_DONE_EVENT)
    @endcode
*/
extern void CAN_TxDoneInd(CAN_HwIDType aID, const CAN_TxMsgObjType * const aXmtdObj);

/** @brief Notify bus-off event

    This callback is invoked by CAN driver to notify client when a Bus Off event
    has occurred.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    @return     void

    @post TBD

    @note This interface shall be implemented by the client.

    @trace  #BRCM_SWREQ_CAN_NOTIFICATION

    @limitations None

    Below is a reference implementation of #CAN_BusOffInd. For example,
    client task (CLIENT_TASK) waits for CAN_BUS_OFF_EVENT when waiting for Bus
    Off event. After a Bus Off event occurs, task shall be woken up by setting
    CAN_BUS_OFF_EVENT in the #CAN_BusOffInd callback function.

    @code{.c}
    BCM_SetEvent(CLIENT_TASK, CAN_BUS_OFF_EVENT)
    @endcode
*/
extern void CAN_BusOffInd(CAN_HwIDType aID);

/** @brief Notify Bus error

    This interface is invoked by CAN driver to notify client when CAN controller
    meets with a Bus error condition.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller
    @param[in]      errorCode   Code for error that occurred on CAN bus

    @return     void

    @post TBD

    @note This interface shall be implemented by the client.

    @trace  #BRCM_SWREQ_CAN_NOTIFICATION

    @limitations None

    Below is reference implementation of #CAN_ErrorInd. For example,
    client task (CLIENT_TASK) waits for CAN_RX_MSG_EVENT & CAN_ERR_EVENT when
    waiting for CAN data. After a Bus error is detected client task shall be
    woken up by setting CAN_ERR_EVENT in the #CAN_ErrorInd callback
    function.

    @code{.c}
    BCM_SetEvent(CLIENT_TASK, CAN_ERR_EVENT)
    @endcode
*/
extern void CAN_ErrorInd(CAN_HwIDType aID, CAN_ErrorType errorCode);

/** @brief Handler for CAN IRQ Line-0

    Interface to handle interrupts on CAN interrupt Line 0.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    @return     void

    @post TBD

    @note This interface shall be called from the interrupt handler for CAN
    Line 0. Integrator shall install the interrupt handler for CAN in the vector
    table. Refer to @ref sec_ref_trm for interrupt mapping.

    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER

    @limitations None

    @snippet can_osil.c Usage of CAN_IRQLine0HandlerInternal
*/
extern void CAN_IRQLine0HandlerInternal(CAN_HwIDType aID);

/** @brief Handler for CAN IRQ Line-1

    Interface to handle interrupts on CAN interrupt Line 0.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    @return     void

    @post TBD

    @note This interface shall be called from the interrupt handler for CAN
    Line 1. Integrator shall install the interrupt handler for CAN in the vector
    table. Refer to @ref sec_ref_trm for interrupt mapping.

    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER

    @limitations None

    @snippet can_osil.c Usage of CAN_IRQLine1HandlerInternal
*/
extern void CAN_IRQLine1HandlerInternal(CAN_HwIDType aID);

/** @brief Handle deferred CAN processing

    This interface handles deferred CAN processing. This interface shall be
    called to process the CAN Packets (TX/RX), error and bus-off from
    CommsTask.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    @return     void

    @post TBD

    @note This interface shall be called from CommsTask, whenever CommsTask is
    signalled from the CAN driver.

    @trace  #BRCM_SWREQ_CAN_COMMUNICATION

    @limitations None

    @snippet can_osil.c Usage of CAN_ProcessPkts
*/
extern void CAN_ProcessPkts(CAN_HwIDType aID);

/** @brief Raise system command request

    Interface to raise System Command Request for CAN driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        CAN system request command
    @param[inout]   aIO         Pointer to #CAN_IOType

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request success
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_CAN_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t CAN_SysCmdReq(CAN_CmdType aCmd, CAN_IOType * const aIO);

/** @brief Handle CAN driver commands

    This interface handles all the CAN driver commands requested from
    #SysCmdReq.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    CAN system request magic ID
    @param[in]      aCmd        CAN system request command
    @param[inout]   aSysIO      Pointer to system request IO to be interpreted
                                as #CAN_IOType

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER

    @limitations None
*/
extern void CAN_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

#endif /* CAN_OSIL_H */
/** @} */
