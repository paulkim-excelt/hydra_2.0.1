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

/** @defgroup grp_pplib_il Integration Guide
    @ingroup grp_pplib

    @addtogroup grp_pplib_il
    @{
    @section sec_pplib_integration Integration Guide
    This page describes functions needed by an integrator. Some of the
    interfaces are implemented as part of the PP library. Some of the interfaces
    need to be implemented by the integrator.
    <BR>.drivers.pp shall be mapped to cached memory
    <BR>.pp.mbox shall be mapped to cached memory

    @section sec_comp_il_seq Sequence Diagram
    Sequence diagrams in this section have SVC layer lifelines. If SVC
    exceptions are enabled in the system, SVC layer raises an SVC as specified
    by Architecture. If SVC exceptions are not used for drivers, SVC layer could
    be understood as a direct function call.
    <BR>Below figure depicts execution sequence for allocating a v-MP.
    (PP_FreeCore, PP_BootCore, PP_StopCore, PP_SendMsg, PP_ReadGPData,
    PP_WriteGPData, PP_AllocHWSem, PP_HWSemGetAddr, PP_TryLock, PP_ReleaseLock
    and PP_FreeHWSem all follow similar sequence as PP_AllocCore)
    @image html pplib_integ_alloc.jpg
    Below figure depics execution sequence of PP_RecvMsg API.
    @image html pplib_integ_recvmsg.jpg

    @file pp_osil.h
    @brief Pixel processor library integration interfaces.
    This header file defines the integration interfaces for PixelProcessor library.
    @version 1.4 Imported from MS Word document
*/

#ifndef PP_OSIL_H
#define PP_OSIL_H

#include <bcm_err.h>
#include <stdint.h>
#include <pp.h>
#include <system.h>

/**
    @name PP Library Integration IDs
    @{
    @brief Integration IDs for PP Library
*/
#define BRCM_SWARCH_PP_INVALID_IDX_MACRO           (0x81U) /**< @brief #PP_INVALID_IDX        */
#define BRCM_SWARCH_PP_CMD_MACRO                   (0x82U) /**< @brief #PP_DrvCmdType         */
#define BRCM_SWARCH_PP_REQUEST_TYPE                (0x83U) /**< @brief #PP_IOType             */
#define BRCM_SWARCH_PP_REQUEST_PROC                (0x84U) /**< @brief #PP_SysCmdReq          */
#define BRCM_SWARCH_PP_DRV_CLIENT_TYPE             (0x85U) /**< @brief #PP_DrvClientType      */
#define BRCM_SWARCH_PP_DRV_GETCLIENT_PROC          (0x86U) /**< @brief #PP_DrvGetClient       */
#define BRCM_SWARCH_PP_DRV_GP_HANDLER_PROC         (0x87U) /**< @brief #PP_DrvGPHandler       */
#define BRCM_SWARCH_PP_DRV_ECC_HANDLER_PROC        (0x88U) /**< @brief #PP_DrvEccHandler      */
#define BRCM_SWARCH_PP_DRV_WD_HANDLER_PROC         (0x89U) /**< @brief #PP_DrvWdHandler       */
#define BRCM_SWARCH_PP_DRV_CMD_HANDLER_PROC        (0x8AU) /**< @brief #PP_DrvCmdHandler      */
#define BRCM_SWARCH_PP_GP_HANDLER_PROC             (0x8BU) /**< @brief #PP_GPHandler          */
#define BRCM_SWARCH_PP_ECC_HANDLER_PROC            (0x8CU) /**< @brief #PP_EccHandler         */
#define BRCM_SWARCH_PP_WD_HANDLER_PROC             (0x8DU) /**< @brief #PP_WdHandler          */
#define BRCM_SWARCH_PP_SERVER_NOTIFY_MASK_MACRO    (0x8EU) /**< @brief #PP_SERVER_NOTIFY_MASK */
/** @}*/


/**
    @brief Macro to identify server tasks

    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
*/
#define PP_SERVER_NOTIFY_MASK   (0xFF000000UL)

/**
    @brief Macro to identify invalid core index

    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
*/
#define PP_INVALID_IDX       (0xFFFFFFFFUL)

/**
    @name PP Driver commands
    @{
    @brief List of PP Driver commands

    PP Driver specific commands handled by PPDrv_CmdHandler

    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
*/
typedef uint32_t PP_DrvCmdType;
#define PP_CMD_INIT              (1UL)   /**< @brief Init */
#define PP_CMD_ALLOC             (2UL)   /**< @brief Allocate a core */
#define PP_CMD_FREE              (3UL)   /**< @brief Free a core */
#define PP_CMD_BOOT              (4UL)   /**< @brief Boot a core */
#define PP_CMD_STOP              (5UL)   /**< @brief Stop a core */
#define PP_CMD_CUSTOM            (6UL)   /**< @brief Custom command */
#define PP_CMD_UPDATE_CLIENT     (7UL)   /**< @brief Update Client Task */
#define PP_CMD_ENABLE_WD         (8UL)   /**< @brief Enable Watchdog for v-MP */
#define PP_CMD_DISABLE_WD        (9UL)   /**< @brief Disable Watchdog for v-MP */

#define PP_CMD_SEND_MSG          (10UL)  /**< @brief Send a message to v-MP */
#define PP_CMD_RECV_MSG          (11UL)  /**< @brief Receive a message from v-MP */
#define PP_CMD_READ_GPDATA       (12UL)  /**< @brief Read a GPDATA register of v-MP */
#define PP_CMD_WRITE_GPDATA      (13UL)  /**< @brief Write a GPDATA register of v-MP */
#define PP_CMD_GET_ERR_STATUS    (14UL)  /**< @brief Get error status of a core */

#define PP_CMD_ALLOC_HWSEM       (16UL)  /**< @brief Allocate HW semaphore */
#define PP_CMD_GET_ADDR_HWSEM    (17UL)  /**< @brief Get address of hardware semaphore */
#define PP_CMD_TRY_LOCK_HWSEM    (18UL)  /**< @brief Lock a HW semaphore */
#define PP_CMD_REL_LOCK_HWSEM    (19UL)  /**< @brief Release lock on a HW semaphore */
#define PP_CMD_FREE_HWSEM        (20UL)  /**< @brief Free an allocated HW semaphone */

#define PP_CMD_INIT_CODEC        (24UL)  /**< @brief Init Codec */
#define PP_CMD_POST_JOB          (25UL)  /**< @brief Post Job */
#define PP_CMD_GET_JOB_STATUS    (26UL)  /**< @brief Get Job Status*/

#define PP_CMD_MAX               (32UL)  /**< @brief Maximum number of commands */
/** @}*/

/**
    @brief PPIO structure passed from SysCmdReq to SysCmdHandler (for privileged
    access)

    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
 */
typedef struct _PP_IOType {
    int32_t            retVal;             /**< @brief Return value from PPDrv_CmdHandler */
    uint32_t           coreID;             /**< @brief Inout parameter of core ID of v-MP */
    const uint8_t      *bootBinary;        /**< @brief Binary to be booted on v-MP */
    char               *payLoad;           /**< @brief Message payload to be received*/
    const char         *inPayLoad;         /**< @brief Message payload to be sent*/
    uint32_t           msgType;            /**< @brief 32-bit message type for send/recv message */
    uint32_t           gpVal;              /**< @brief Inout for holding GPData Value */
    uint32_t           gpReg;              /**< @brief gpData register ID */
    int32_t            clientID;           /**< @brief Client ID (Task ID) to be updated */
    PP_ClientType      clientType;         /**< @brief Client type */
    uint32_t           time;               /**< @brief Time in ms for watchdog of v-MP */
    uint32_t           hwSemID;            /**< @brief HW Semaphore ID */
    uint32_t           hwSemAddr;          /**< @brief Address of allocated HW semaphore */
    PP_ErrorStatusType *errorStatus;       /**< @brief Error status to be filled */
    uint32_t           reserved[8];        /**< @brief Reserved fields */
} PP_IOType;


/**
    @brief Client ID

    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
 */
typedef struct _PP_DrvClientType {
    uint32_t valid;         /**< validity of client 0/1 */
    int32_t id;             /**< 32-bit client ID       */
} PP_DrvClientType;

/** @brief Get Client ID to signal

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIndex           Core Index for which the Client ID is needed
    @param[in]      aClientType      Client type

    @retval     PP_ClientType        Client ID of core index (aIndex)

    @post None

    @note To be used by Integrator

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None
*/
PP_DrvClientType PP_DrvGetClient(uint32_t aIndex, PP_ClientType aClientType);

/** @brief Handle PP GP Interrupt

    This interface acknowledges PP GP interrupts and returns back a bit mask of all
    v-MPs whose registered Tasks need to be triggered.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param[in]      void

    @retval     uint32_t    Bit mask with bits enabled for which
                            Tasks needs to be triggered. Bit0 (LSB)
                            for v-MP0 and Bit7 for v-MP7

    @post Signal the client tasks whose v-MP is bit-marked for signaling

    @note To be used by Integrator.

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None

*/
uint32_t PP_DrvGPHandler(void);

/**
    @brief Handle PP ECC Error Interrupt

    This interface acknowledges PP ECC error interrupts and returns back a bit mask
    of all v-MPs whose registered Tasks need to be triggered.

    @behavior Sync, Non-reentrant

    @pre Execute in Interrupt handler context

    @param[in]      void

    @retval     uint32_t    Bit mask with bits enabled for which
                            Tasks needs to be triggered. Bit0 (LSB)
                            for v-MP0 and Bit7 for v-MP7

    @post Signal the client tasks whose v-MP is bit-marked for signaling

    @note To be used by Integrator

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None

*/
uint32_t PP_DrvEccHandler(void);

/**
    @brief Handle PP Watchdog Error Interrupt

    This interface acknowledges PP Watchdog error interrupts and returns back a bit mask
    of all v-MPs whose registered Tasks need to be triggered.

    @behavior Sync, Non-reentrant

    @pre Execute in Interrupt handler context

    @param[in]      void

    @retval     uint32_t    Bit mask with bits enabled for which
                            Tasks needs to be triggered. Bit0 (LSB)
                            for v-MP0 and Bit7 for v-MP7

    @post Signal the client tasks whose v-MP is bit-marked for signaling

    @note To be used by Integrator

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None
*/
uint32_t PP_DrvWdHandler(void);

/**
    @brief PP Driver command handler

    This interface handles all the PP driver commands requested from system
    command request. This interface shall be implemented by the integrator and
    shall hook it to SVC layer as per Architecture.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aMagic      PP Driver magic number
    @param[inout]   aCmd        Command requested by SysCmdReq
    @param[out]     aPPSysIO    uint8_t * request to be interpreted as PPSysIO

    @return void (for void functions)

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None
*/
void PP_DrvCmdHandler(uint32_t aMagic,
                        uint32_t aCmd,
                        uint8_t *aPPSysIO);

/**
    @brief PP Driver command requestor

    Interface to raise System Command Request for PP driver.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aPPIO       In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             System command request completed
                                        successfully
    @retval     #BCM_ERR_UNKNOWN        Integration error
    @retval     Others                  Error values specific to the command passed

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE

    @limitations None
*/
int32_t PP_SysCmdReq(const PP_DrvCmdType aCmd, PP_IOType *aPPIO);

/** @brief PP GP Interrupt Handler to be installed in Vector table

    This interface calls @ref PP_DrvGPHandler and signal all clients
    whose GP Interrupt got triggered.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param[in]      void

    @retval     void

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None

*/
void PP_GPHandler(void);

/**
    @brief PP ECC Error Interrupt Handler to be installed in Vector table

    This interface calls @ref PP_DrvEccHandler and signal all clients
    whose v-MP cores raised ECC Interrupt.

    @behavior Sync, Non-reentrant

    @pre Execute in Interrupt handler context

    @param[in]      void

    @retval     void

    @post None

    @note To be implemented by Integrator

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None

*/
void PP_EccHandler(void);

/**
    @brief PP Watchdog Error Interrupt Handler to be installed in Vector table

    This interface calls @ref PP_DrvWdHandler and signal all clients
    whose v-MP cores raised Watchdog Interrupt.

    @behavior Sync, Non-reentrant

    @pre Execute in Interrupt handler context

    @param[in]      void

    @retval     void

    @post None

    @note To be implemented by Integrator

    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER

    @limitations None
*/
void PP_WdHandler(void);

#endif /* PP_OSIL_H */
/** @} */
