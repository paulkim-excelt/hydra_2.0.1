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

/** @defgroup grp_lindrv_il Integration Guide
    @ingroup grp_lindrv

    @addtogroup grp_lindrv_il
    @{
    @section sec_lin_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system. This section describes LIN driver integration layer in detail.

    LIN driver is implemented in #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for LIN driver on to
    #SysCmdHandler for privileged access.

    @section sec_lin_il_seq Sequence Diagram
    Below sequence diagram describes the sequence of LIN Driver synchronous
    operation #LIN_TransmitFrame. The other LIN APIs follow a similar sequence
    as this sequence diagram.
    @image html lin_seq_il_diagram_tx_frame.jpg

    @file lin_osil.h
    @brief LIN driver Integration Interfaces
    This header file contains the integration interfaces for LIN driver.
    @version 0.50 Imported from docx
*/

#ifndef LIN_OSIL_H
#define LIN_OSIL_H

#include <system.h>
#include <lin.h>

/**
    @name LIN Integration Interface IDs
    @{
    @brief Integration Interface IDs for LIN driver.
*/
#define BRCM_SWARCH_LIN_IL_CMD_TYPE             (0x40U) /**< @brief #LIN_CmdType */
#define BRCM_SWARCH_LIN_IL_IO_TYPE              (0x41U) /**< @brief #LINIO */
#define BRCM_SWARCH_LIN_IL_SYS_CMD_REQ_PROC     (0x42U) /**< @brief #LIN_SysCmdReq */
#define BRCM_SWARCH_LIN_IL_CMD_HANDLER_PROC     (0x43U) /**< @brief #LIN_CmdHandler */
#define BRCM_SWARCH_LIN_IL_SYS_CMD_HANDLER_PROC (0x44U) /**< @brief #LIN_SysCmdHandler */
#define BRCM_SWARCH_LIN_IL_TX_CB_PROC           (0x45U) /**< @brief #LIN_TxCb */
#define BRCM_SWARCH_LIN_IL_RX_CB_PROC           (0x46U) /**< @brief #LIN_RxCb */
#define BRCM_SWARCH_LIN_IL_ERR_CB_PROC          (0x47U) /**< @brief #LIN_ErrCb */
#define BRCM_SWARCH_LIN_IL_DRV_TX_CB_PROC       (0x48U) /**< @brief #LINDrv_TxCb */
#define BRCM_SWARCH_LIN_IL_DRV_RX_CB_PROC       (0x49U) /**< @brief #LINDrv_RxCb */
#define BRCM_SWARCH_LIN_IL_DRV_ERR_CB_PROC      (0x4AU) /**< @brief #LINDrv_ErrCb */
#define BRCM_SWARCH_LIN_IL_WAKEUP_NOTIF_PROC    (0x4BU) /**< @brief #LIN_WakeUpNotification */
/** @} */

/**
    @name LIN_CmdType
    @{
    @brief LIN driver command


    @trace #BRCM_SWREQ_LIN_IL_CMD
*/
typedef uint32_t LIN_CmdType;   /**< @brief LIN command type */
/** @} */

/**
    @brief LIN IO Structure

    @trace #BRCM_SWREQ_LIN_IL_IO
*/
typedef struct _LINIO {
    int32_t retVal;                 /**< @brief SVC Response */
    LIN_HwIDType hwID;              /**< @brief Index of LIN
                                         hardware/controller */
    const LIN_ConfigType * config;  /**< @brief Pointer to LIN configuration
                                         data */
    const LIN_FrmObjType * frmObj;  /**< @brief Pointer to LIN Frame Object */
    uint8_t * dataPtr;              /**< @brief Pointer to LIN TX data */
} LINIO;

/** @brief Raise system command request

    Interface to raise system command request for LIN driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        LIN system request command
    @param[inout]   aLINIO      Pointer to #LINIO

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request raised successfully
    @retval     Others                  As returned from #LIN_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_LIN_IL_SYS_CMD_REQ

    @limitations None
*/
extern int32_t LIN_SysCmdReq(LIN_CmdType aCmd, LINIO *const aLINIO);

/** @brief Internal command handler

    This internal LIN driver internal interface handles all LIN driver system
    command requests.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aCmd        LIN command requested
    @param[inout]   aIO         LIN IO (#LINIO)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request processed successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid command
    @retval     Others                  Error values specific to the command
                                        raised
    @retval     #BCM_ERR_UNKNOWN        @a aIO is NULL
    @return     void

    @post TBD

    @note This interface shall be called from #LIN_SysCmdHandler.

    @trace #BRCM_SWREQ_LIN_IL_CMD_HANDLER

    @limitations None
*/
extern int32_t LIN_CmdHandler(LIN_CmdType aCmd, LINIO * const aIO);

/** @brief Handle all LIN driver commands

    This interface handles all the LIN driver commands requested from
    #SysCmdReq.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    LIN system request magic ID
    @param[in]      aCmd        LIN system request command
    @param[inout]   aSysIO      Pointer to system request IO to be imterpreted
                                as #LINIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator, andh shall
    install it to SVC layer.

    @trace #BRCM_SWREQ_LIN_IL_SYS_CMD_HANDLER

    @limitations None
*/
extern void LIN_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief UART TX Call back

    This interface shall fill the buffer (@a aData) with TX data (upto a maximum
    of @a aSize number of bytes).

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[out]     aData       Buffer to be filled
    @param[in]      aSize       Size upto which the buffer needs to be filled

    @return     Actual size filled

    @post TBD

    @note This interface shall be implemented by the integrator and shall be
    called from the UART driver whenever UART TX FIFO is available. (Refer to
    #UART_TxCb).

    @trace #BRCM_SWREQ_LIN_IL_TX_CB

    @limitations None
*/
extern uint32_t LIN_TxCb(char * const aData, uint32_t aSize);

/** @brief LIN RX callback

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[out]     aData       Data received on the UART driver
    @param[in]      aSize       Size of the data received (in bytes)

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall be
    called from the UART driver, when LIN RX data is available. (Refer to
    #UART_RxCb).

    @trace #BRCM_SWREQ_LIN_IL_RX_CB

    @limitations None
*/
extern void LIN_RxCb(char * const aData, uint32_t aSize);

/** @brief Handle LIN errors

    This interface shall handle any errors from the LIN controller.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre TBD

    @param[in]      aErr        Error on the LIN controller
    @param[in]      aData       Data received

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    be called from UART driver, whenever UART Controller encounters an error.
    (Refer to #UART_ErrCb).

    @trace #BRCM_SWREQ_LIN_IL_ERR_CB

    @limitations any limitations for this api
*/
extern void LIN_ErrCb(LIN_ErrorType aErr, char aData);

/** @brief Internal interface to handle LIN TX callback

    This internal interface handles all the LIN TX callbacks.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       LIN Hardware ID
    @param[in]      aData       Pointer to input buffer that shall be pushed to
                                UART driver
    @param[in]      aSize       Size of the buffer

    @return     Actual size filled by the client

    @post TBD

    @note This interface shall be called from #LIN_TxCb.

    @trace #BRCM_SWREQ_LIN_IL_DRV_TX_CB

    @limitations None
*/
extern uint32_t LINDrv_TxCb(LIN_HwIDType aHwID,
                            char * const aData,
                            uint32_t aSize);

/** @brief Internal interface to handle LIN RX callback

    This internal interface handles all the LIN RX callbacks.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       LIN Hardware ID
    @param[in]      aData       Pointer to data buffer
    @param[in]      aSize       Size of the buffer

    @return     void

    @post TBD

    @note This interface shall be called from #LIN_RxCb.

    @trace #BRCM_SWREQ_LIN_IL_DRV_RX_CB

    @limitations None
*/
extern void LINDrv_RxCb(LIN_HwIDType aHwID, char * const aData, uint32_t aSize);

/** @brief Internal interface to handle LIN error callbacks

    This internal interface handles all the LIN error callbacks.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre TBD

    @param[in]      aHwID       LIN Hardware ID
    @param[in]      aErr        Error on Controller
    @param[in]      aData       Data received on error

    @return     void

    @post TBD

    @note This interface shall be called from #LIN_ErrCb.

    @trace #BRCM_SWREQ_LIN_IL_DRV_ERR_CB

    @limitations None
*/
extern void LINDrv_ErrCb(LIN_HwIDType aHwID, LIN_ErrorType aErr, char aData);

/** @brief LIN wakeup notification

    Callback function invoked by LIN driver to notify ithe client application of
    a received wakeup pulse.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aId         Index of the LIN Hardware/Controller

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace #BRCM_SWREQ_LIN_IL_WAKEUP_NOTIF

    @limitations None

    @code{.c}
    BCM_SetEvent(CLIENT_TASK, LIN_WAKEUP_EVENT)
    @endcode
*/
void LIN_WakeUpNotification(LIN_HwIDType aId);

#endif /* LIN_OSIL_H */
/** @} */
