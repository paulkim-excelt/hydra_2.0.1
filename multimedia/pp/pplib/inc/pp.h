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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_pplib_ifc Interface
    @ingroup grp_pplib

    @addtogroup grp_pplib_ifc
    @{
    @section sec_comp_overview Overview
    @image html pplib_overview.jpg
    Pixel-Processor hardware subsystem consists of multicore v-MP cores.
    Pixel-Processor Low level library provides driver and driver interface for
    interaction with v-MP cores.
    <BR>Pixel-Processor Driver provides APIs for the following functionalities.
        -# Allocate a v-MP
        -# Register a Task to get notifications from v-MP
        -# Boot a v-MP
        -# Send a message to v-MP
        -# Receive a message from v-MP
        -# Read a GPDATA register of v-MP
        -# Write a GPDATA register of v-MP
        -# stop a v-MP
        -# free a v-MP
        -# accessing hardware semaphores
        .
    When a Core is allocated, a unique ID is returned to the caller. The Unique
    ID has to be passed for all the other functions to access the allocated core.
    PP driver provides APIs to send/receive messages to/from v-MP.
    <BR>A v-MP has 2x 32-bit GPDATA registers (0 & 1) and 2 x 4-bit registers
    (6 & 7) for application use. GPDATA registers -2, 3, 4, and 5 are reserved
    for mailbox system use [modifying these registers from applications is
    prohibited]. PP driver provides APIs to read and write to GPDATA registers
    of v-MP cores. Access to GPDATA registers is possible only when a v-MP core
    is booted.
    <BR>When a message is received from v-MP core to ARM, an interrupt is
    triggered from v-MP to ARM. Interrupt service routine then signals the Task
    which is registered by the user for the corresponding v-MP Core.
    <BR>PP driver also provides APIs for accessing hardware semaphores for
    synchronization between ARM and the v-MP firmware. Once a hardware semaphore
    is allocated (PP_AllocHWSem), PP_HWSemGetAddr API can be used to retrieve
    the semaphore address. This semaphore address shall be used in the v-MP
    firmware to access the semaphore. Note that the Hardware semaphores are
    independent of the v-MP cores. The number of hardware semaphores available
    is specific to the chipset.

    @section sec_comp_fsm State Machine
    @image html pplib_mp_state.jpg
    The state machine for each v-MP in the PP Driver is depicted above. It also
    depicts the APIs which cause the state transition and which APIs are allowed
    to be called in several states of each v-MP.
    @image html pplib_sem_state.jpg
    The state machine for each hardware semaphore is depicted above. It also
    depicts the APIs that cause the state transitions and which APIs are allowed
    to be called in each state of the hardware semaphore.

    @section sec_comp_seq Sequence Diagrams
    @image html pplib_mp_seq.jpg
    Above figure depicts a typical flow of application to interact with a v-MP core.
    @image html pplib_dma_seq.jpg
    Above figure depicts a typical flow of application to interact with pplib
    for DMA operation.
    @image html pplib_sem_seq.jpg
    Above figure depicts a typical flow of application to interact with Hardware
    Semaphores.

    @section sec_comp_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc pplib_all.dep

    @limitations None

    @todo add references
    Pixel Processor Core Architecture and Programmer’s guide, and replace [2]
    @file pp.h
    @brief Application interface for Pixel Processor Driver
    This header file contains the interface functions for PP Driver
    @version 1.4 Imported from MS Word document
*/

#ifndef PP_H
#define PP_H
#include <stdint.h>
#include <bcm_err.h>

/**
    @name PP Library API IDs
    @{
    @brief API IDs for PP Library
*/
#define BRCM_SWARCH_PP_MBOXSIZE_MACRO      (0x00U)    /**< @brief #PP_MSG_PAYLOAD_SIZE*/
#define BRCM_SWARCH_PP_DMACHAN_TYPE        (0x01U)    /**< @brief #PP_DMAChannelType  */
#define BRCM_SWARCH_PP_DMA_TYPE            (0x02U)    /**< @brief #PP_DMABufferType   */
#define BRCM_SWARCH_PP_INIT_PROC           (0x03U)    /**< @brief #PP_Init            */
#define BRCM_SWARCH_PP_ALLOC_VMP_PROC      (0x04U)    /**< @brief #PP_AllocCore       */
#define BRCM_SWARCH_PP_FREE_PROC           (0x05U)    /**< @brief #PP_FreeCore        */
#define BRCM_SWARCH_PP_BOOT_PROC           (0x06U)    /**< @brief #PP_BootCore        */
#define BRCM_SWARCH_PP_STOP_PROC           (0x07U)    /**< @brief #PP_StopCore        */
#define BRCM_SWARCH_PP_SENDMSG_PROC        (0x08U)    /**< @brief #PP_SendMsg         */
#define BRCM_SWARCH_PP_RECVMSG_PROC        (0x09U)    /**< @brief #PP_RecvMsg         */
#define BRCM_SWARCH_PP_GPDATAREAD_PROC     (0x0AU)    /**< @brief #PP_ReadGPData      */
#define BRCM_SWARCH_PP_GPDATAWRITE_PROC    (0x0BU)    /**< @brief #PP_WriteGPData     */
#define BRCM_SWARCH_PP_UPDATECLIENT_PROC   (0x0CU)    /**< @brief #PP_UpdateClient    */
#define BRCM_SWARCH_PP_ALLOCSEM_PROC       (0x0DU)    /**< @brief #PP_AllocHWSem      */
#define BRCM_SWARCH_PP_GETADDR_PROC        (0x0EU)    /**< @brief #PP_HWSemGetAddr    */
#define BRCM_SWARCH_PP_LOCK_PROC           (0x0FU)    /**< @brief #PP_TryLock         */
#define BRCM_SWARCH_PP_RELEASE_PROC        (0x10U)    /**< @brief #PP_ReleaseLock     */
#define BRCM_SWARCH_PP_FREE_SEM_PROC       (0x11U)    /**< @brief #PP_FreeHWSem       */
#define BRCM_SWARCH_PP_DMA_START_PROC      (0x12U)    /**< @brief #PP_DMAStart        */
#define BRCM_SWARCH_PP_DMA_STATUS_PROC     (0x13U)    /**< @brief #PP_DMAStatus       */
#define BRCM_SWARCH_PP_ENABLE_WD_PROC      (0x14U)    /**< @brief #PP_EnableWD        */
#define BRCM_SWARCH_PP_DISABLE_WD_PROC     (0x15U)    /**< @brief #PP_DisableWD       */
#define BRCM_SWARCH_PP_GET_ERR_STATUS_PROC (0x16U)    /**< @brief #PP_GetErrorStatus  */
#define BRCM_SWARCH_PP_CLIENT_TYPE         (0x17U)    /**< @brief #PP_ClientType      */
#define BRCM_SWARCH_PP_ERR_STATUS_TYPE     (0x18U)    /**< @brief #PP_ErrorStatusType */

/** @}*/

/**
    @brief Mailbox message size

    @trace #BRCM_SWREQ_PP_INIT
*/
#define PP_MSG_PAYLOAD_SIZE (56UL)

/**
    @name PP_ClientType Masks
    @{
    @brief Client type
    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
*/
typedef uint32_t PP_ClientType;
#define PP_CLIENT_GP    (0UL) /**< @brief GP handler client*/
#define PP_CLIENT_ECC   (1UL) /**< @brief Ecc handler client */
#define PP_CLIENT_WD    (2UL) /**< @brief Watch dog handler client */
#define PP_CLIENT_ALL   (3UL) /**< @brief all handler client */
/** @}*/

/**
    @brief PP DMA channel structure

    Structure for single-channel 2D DMA using PP

    @trace #BRCM_SWREQ_PP_DMA
*/
typedef struct _PP_DMAChannelType {
    uint8_t *addr;      /**< @brief Pointer to data    */
    uint32_t width;     /**< @brief Width in bytes     */
    uint32_t height;    /**< @brief Number of rows     */
    uint32_t stride;    /**< @brief Stride in bytes    */
} PP_DMAChannelType;

/**
    @brief PP DMA buffer structure

    Structure for multi-channel 2D DMA using PP

    @trace #BRCM_SWREQ_PP_DMA
*/
typedef struct _PP_DMABufferType {
    uint32_t numCH;             /**< @brief Number of channels */
    PP_DMAChannelType channel[3];   /**< @brief channel configuration */
} PP_DMABufferType;

/**
    @brief PP Error status structure

    Structure for error status of PP

    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
*/
typedef struct _PP_ErrorStatusType {
    uint32_t ncEcc;           /**< @brief Non correctable ECC error status. Core Would have been stopped if this count is non zero */
    uint32_t cEccCount;       /**< @brief correctable ECC error count. Core would be still running even if this count is non zero  */
    uint32_t bus;             /**< @brief Bus error status. Core would be still running even if this count is non zero             */
    uint32_t wdCount;         /**< @brief watch dog count. Core would be still running even if this count is non zero              */
} PP_ErrorStatusType;

/** @brief Initialize PP Driver

    Initialize PP Driver, DMA and CODEC engines

    @behavior Sync, Re-entrant/Non-reentrant

    @pre None

    @param[in]      aMode        Reserved for future
    @param[in]      aMPSafetyBin Safety firmware for MP
    @param[in]      aSPSafetyBin Safety firmware for SP
    @param[in]      aCodecBin    Codec/DMA firmware

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             PP Init successful
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   (aMPSafetyBin is NULL) or
                                        (aSPSafetyBin is NULL) or
                                        (aCodecBin is NULL)

    @post PP Driver is initialized successfully

    @trace #BRCM_SWREQ_PP_INIT

    @limitations None

    @note This takes approximately 4ms for completion
*/
int32_t PP_Init(uint32_t aMode,
                const uint8_t *aMPSafetyBin,
                const uint8_t *aSPSafetyBin,
                const uint8_t *aCodecBin);

/** @brief Allocates a core if available

    This API allocates a v-MP core to the caller.
    This API fills a unique number to the aCoreID field to identify the v-MP core
    allocated to the caller. All further APIs are required to pass the unique ID
    to access the v-MP core. This Core is marked to signal the calling Task when
    an interrupt occurs.The API tries to allocate a v-MP core and give a handle
    to the client for accessing the allocated v-MP core

    @behavior Sync, Re-entrant

    @pre PP_Init was successful

    @param[out]     aCoreID     Pointer to fill allocated Core ID and unique core
                                ID filled in aCoreID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Core successfully allocated
    @retval     #BCM_ERR_NODEV          No core available
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   (aCoreID is NULL)

    @post Upon success, v-MP is moved to Allocated state

    @trace #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_AllocCore(uint32_t *const aCoreID);

/** @brief Frees a core allocated previously

    This API frees an allocated a v-MP core.
    No further access to the core is possible after this API.
    Unique ID is no more valid. Caller would need to call PP_AllocCore
    to get a core again.  If the core is in running state when this API is
    called, the core is stopped and freed.

    @behavior Sync, Re-entrant

    @pre v-MP was allocated

    @param[in]      aCoreID     Core ID which needs to be freed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Core successfully freed
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid core ID
    @retval     #BCM_ERR_UNKNOWN        Integration Error

    @post aCoreID is invalid after this

    @trace  #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_FreeCore(const uint32_t aCoreID);

/** @brief Boot a core allocated previously

    This API boots the v-MP core with the provided boot binary.

    @behavior Sync, Re-entrant

    @pre v-MP is in allocated state

    @param[in]      aCoreID     Core ID to boot the v-MP core
    @param[inout]   aBinPtr     Memory pointer holding the bootable binary
                                firmware
    @param[out]     aBinSize    Valid Size of memory holding the bootable binary
                                firmware

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Core successfully booted
    @retval     #BCM_ERR_INVAL_PARAMS   aCoreID is invalid
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   (aBinPtr is NULL)

    @post v-MP moves to Booted state upon success

    @trace  #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_BootCore(const uint32_t aCoreID,
                    const uint8_t * const aBinPtr,
                    const uint32_t aBinSize);

/** @brief Stop a core

    This API stops the given v-MP (as given by aCoreID).

    @behavior Sync, Re-entrant

    @pre v-MP is in booted state

    @param[in]      aCoreID     Core ID of the core to be stopped

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Core is successfully stopped (or is
                                        already in the stopped state)
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Core ID

    @post v-MP moves to Stopped state upon success

    @trace  #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_StopCore(const uint32_t aCoreID);

/** @brief Send a message to v-MP core

    This API sends a message to the v-MP core.

    @behavior Sync, Re-entrant

    @pre v-MP is in booted state

    @param[in]      aCoreID     v_MP Core ID to which the message has to be sent
    @param[in]      aMsgType    Application specific 32-bit Message type
                                to identify payload content
    @param[in]      aPayload    Pointer to 56-bytes of message to be sent to v-MP
                                User shall provide the memory appropriately.
                                If aPayLoad is NULL, an empty message is sent.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message is successfully sent
    @retval     #BCM_ERR_BUSY           Mailbox is full and the message could
                                        not be posted
    @retval     #BCM_ERR_INVAL_STATE    (v-MP is not booted)
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   (aCoreID is NULL) or
                                        (Invalid core ID)

    @post None

    @trace  #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_SendMsg(const uint32_t aCoreID,
                    const uint32_t aMsgType,
                    const char* const aPayload);

/** @brief Read a message from mailbox

    This API receives a message from the v-MP core.
    Check mailbox for message and copies the same if available.

    @behavior Sync, Re-entrant

    @pre v-MP is in booted state

    @param[in]      aCoreID     ID of the v-MP core from which the message is
                                to be received
    @param[out]     aMsgType    Application specific 32-bit Message type to
                                identify payload content
    @param[out]     aPayload    Pointer to 56-bytes of message payload (filled
                                with payload sent from v-MP). User shall provide
                                sufficient memory to fill.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Received a message
    @retval     #BCM_ERR_NOMEM          No messages in mailbox
    @retval     #BCM_ERR_INVAL_STATE    v-MP is not booted
    @retval     #BCM_ERR_INVAL_PARAMS   aPayload is NULL or aCoreID is invalid
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   (aMsgType is NULL)

    @post Removes a mailbox message upon success

    @trace  #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_RecvMsg(const uint32_t aCoreID,
                    uint32_t * const aMsgType,
                    char* const aPayload);

/** @brief Read GPDATA register

    This API to get GPData register content of the v-MP core.

    @behavior Sync, Re-entrant

    @pre v-MP core is allocated and booted

    @param[in]      aCoreID     Core ID of v-MP
    @param[in]      aGPRegID    GPData Register ID [2] to read from
                                [valid values 0,1, 6 and 7]
    @param[out]     aValue      Pointer to fill the content of aGPRegID of v-MP

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             GPData read successful
    @retval     #BCM_ERR_INVAL_STATE    v-MP is not booted
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid core ID or
                                        Invalid GP Data Register ID
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   aValue is NULL

    @post None

    @trace #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_ReadGPData(const uint32_t aCoreID,
                        const uint32_t aGPRegID,
                        uint32_t *const aValue);

/** @brief Write GPDATA register

    This API to set GPData register content of the v-MP core.

    @behavior Sync, Re-entrant

    @pre v-MP core is allocated and booted

    @param[in]      aCoreID     Core ID of v-MP
    @param[in]      aGPRegID    GPData Register ID [2] to write to
                                [valid values 0,1, 6 and 7]
    @param[in]      aValue      Value to be written to aGPRegID of v-MP

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             GPData write successful
    @retval     #BCM_ERR_INVAL_STATE    v-MP is not booted
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid core ID or
                                        Invalid GP Data Register ID
    @retval     #BCM_ERR_UNKNOWN        Integration Error

    @post GPData register is updated on success

    @trace  #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_WriteGPData(const uint32_t aCoreID,
                        const uint32_t aGPRegID,
                        const uint32_t aValue);

/** @brief Update client task

    This API updates the task ID to v-MP core. The registered Task is triggered
    when an interrupt arrives from v-MP. It is application developer’s
    responsibility to synchronize between tasks while using this API.

    @behavior Sync, Re-entrant

    @pre v-MP core was allocated by the calling Task

    @param[in]      aCoreID         ID of the v-MP core to which aClient needs to
                                    be registered
    @param[in]      aClientID       ID of the client task to trigger when an
                                    interrupt arrives from v-MP
    @param[in]      aClientType     Type of the client to register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Client updated successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aCoreID is invalid
    @retval     #BCM_ERR_UNKNOWN        Integration Error

    @post Upon success, New client (aClient) will get triggered when interrupt
    arrives on the v-MP

    @trace #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_UpdateClient(const uint32_t aCoreID,
                        const int32_t aClientID,
                        const PP_ClientType aClientType);

/** @brief Allocates a HW Semaphore if available

    This API allocates a HW Semaphore to the callAer. This API fills a unique HW
    Semaphore ID after allocation (Note: ID is not the address of the
    Semaphore).

    @behavior Sync, Re-entrant

    @pre PP_Init was successful

    @param[out]     aHWSemID    Pointer to fill allocated HW Sem ID (valid only
                                when the return value is BCM_ERR_OK).

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             HW Semaphore allocated
    @retval     #BCM_ERR_NODEV          No HW Semaphore available
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   (aHWSemID is NULL)

    @post HW Semaphore is allocated upon Success. This can be used with other HW
    Semaphore related APIs.

    @trace #BRCM_SWREQ_PP_HWSEM

    @limitations None
*/
int32_t PP_AllocHWSem(uint32_t *const aHWSemID);

/** @brief Get address of allocated HW Semaphore

    This API retrieves address of the given HW Semaphore.

    @behavior Sync, Re-entrant

    @pre HW Semaphore is allocated successfully

    @param[in]      aHWSemID    HW Semaphore ID to get address for
    @param[out]     aHWSemAddr  Pointer to fill address of HW Semaphore (valid
                                only when the return value is BCM_ERR_OK).

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             HW Semaphore address is read
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Semaphore ID
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   (aHWSemAddr is NULL)

    @post None

    @trace  #BRCM_SWREQ_PP_HWSEM

    @limitations None
*/
int32_t PP_HWSemGetAddr(uint32_t aHWSemID, uint32_t * const aHWSemAddr);

/** @brief Try locking a HW Semaphore

    This API tries to lock a HW Semaphore and returns the status. This API fails
    if aHWSemID is not a vaild ID allocated or if it was freed already.

    @behavior Sync, Re-entrant

    @pre HW Semaphore is allocated successfully

    @param[in]      aHWSemID    HW Semaphore ID to be locked

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully locked
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_EAGAIN         HW Semaphore lock failed; try again
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Semaphore ID

    @post Upon success, HW semaphore is locked

    @trace #BRCM_SWREQ_PP_HWSEM

    @limitations None
*/
int32_t PP_TryLock(const uint32_t aHWSemID);

/** @brief Release lock on a HW Semaphore

    This API releases lock on a HW Semaphore. This API fails if aHWSemID is not
    a valid ID allocated or if it was freed already.

    @behavior Sync, Re-entrant

    @pre HW Semaphore was previously locked successfully

    @param[in]      aHWSemID    HW Semaphore ID to be released

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully released
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Semaphore ID

    @post Upon success, HW semaphore is released

    @trace #BRCM_SWREQ_PP_HWSEM

    @limitations None
*/
int32_t PP_ReleaseLock(const uint32_t aHWSemID);

/** @brief Frees a HW Semaphore allocated previously

    This API frees an allocated hardware semaphore. No further access to the
    hardware semaphore is possible after successfully calling this API. Called
    would need to call PP_AllocHWSem to get a hardware semaphore again. If the
    semaphore is in locked state when this API is called, the semaphore lock is
    released and freed.

    @behavior Sync, Re-entrant

    @pre aHWSemID is a valid Semaphore ID got with PP_AllocHWSem

    @param[in]      aHWSemID    HW Semaphore ID to be freed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully freed
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Semaphore ID

    @post Upon success, HW semaphore is freed.

    @trace #BRCM_SWREQ_PP_HWSEM

    @limitations None
*/
int32_t PP_FreeHWSem(const uint32_t aHWSemID);

/** @brief Starts DMA copy from inbuffer to outbuffer. Updates the job index

    This API starts copying the data from input buffer to output buffer through
    DMA. It updates the job index. It is the responsibility of the Caller to
    take care of the necessary cache maintenance operations and synchronization
    with other software.

    @behavior Sync, Re-entrant

    @pre PP_Init is successful

    @param[in]      aIn         Input Buffer ‘from’ where data has to be copied
    @param[out]     aOut        Output Buffer ‘to’ where data has to be copied
    @param[out]     aJobIdx     Pointer to Job Index which gets updated

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Job posted successfully
    @retval     #BCM_ERR_UNKNOWN        Integration Error or Error in initialization
    @retval     #BCM_ERR_INVAL_PARAMS   (aIn is NULL) or
                                        (aOut is NULL) or
                                        (aJobIdx is NULL)

    @post Upon success, DMA job is queued.

    @trace #BRCM_SWREQ_PP_DMA

    @limitations None
*/
int32_t PP_DMAStart(PP_DMABufferType *aIn, PP_DMABufferType *aOut, uint32_t *aJobIdx);

/** @brief Returns the job status of the jobindex

    This API updates the status of the job requested. It is tracked through the
    job index.

    @behavior Sync, Re-entrant

    @pre valid aJobIdx is got through PP_DMAStart API

    @param[in]      aJobIdx     JobIndex for which status is needed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             DMA operation complete
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_BUSY           DMA is still in progress
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid job index

    @post aJobIdx is marked invalid if the returned value is BCM_ERR_OK

    @trace #BRCM_SWREQ_PP_DMA

    @limitations None
*/
int32_t PP_DMAStatus(uint32_t aJobIdx);

/** @brief Enable the watch dog

    This API enables the watchdog of the PP core.

    @behavior Sync, Re-entrant

    @pre valid aCoreID is got through PP_AllocCore API

    @param[in]      aCoreID     Core ID for which watch dog needs to be
                                enabled
    @param[in]      aTimeUS     Number of micro seconds after which watch dog
                                needs to raise interrupt

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Watch dog enabled
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid core ID
    @retval     #BCM_ERR_INVAL_PARAMS   aTimeUS is out of range

    @post upon success watch dog starts counting the ticks

    @trace #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_EnableWD(const uint32_t aCoreID,
                    const uint32_t aTimeUS);

/** @brief Disable the watch dog

    This API disables the watchdog of the PP core.

    @behavior Sync, Re-entrant

    @pre valid aCoreID is got through PP_AllocCore API

    @param[in]      aCoreID     Core ID for which watch dog needs to be
                                disabled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Watch dog disabled
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid core ID

    @post upon success watch dog get disabled

    @trace #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_DisableWD(const uint32_t aCoreID);

/** @brief Get error status

    This API accumulates and returns the error status of the core

    @behavior Sync, Re-entrant

    @pre valid aCoreID is got through PP_AllocCore API

    @param[in]      aCoreID      Core ID for which watch dog needs to be
                                 disabled
    @param[out]     aErrorStatus Pointer to the structure to store the error
                                 status and counts

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid core ID
    @retval     #BCM_ERR_INVAL_PARAMS   aErrorStatus is  NULL

    @post no chnage happens.

    @trace #BRCM_SWREQ_PP_CORE

    @limitations None
*/
int32_t PP_GetErrorStatus(const uint32_t aCoreID, PP_ErrorStatusType *aErrorStatus);

#endif /* PP_H */
/** @} */
