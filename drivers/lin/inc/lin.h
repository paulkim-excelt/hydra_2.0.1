/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_lindrv_ifc Interface
    @ingroup grp_lindrv

    @addtogroup grp_lindrv_ifc
    @{
    @section sec_lin_overview Overview
    @image html lin_overview.jpg "LIN Driver"
    Above figure describes LIN driver architecture and its interaction with
    clients (application) and system.

    LIN Driver uses the underlying UART Driver Core which consists of functions
    for UART-based Rx Handling, Tx Handling, and Interrupt Handling. All
    functions concerning the LIN protocol handling is implemented in the LIN
    driver core. The UART driver core handles the received messages from the
    UART as well as the transfer of transmit messages from the UART onto the LIN
    bus via the LIN transceiver.

    LIN driver manages all the LIN buses in the hardware and initiates a
    transaction on the LIN bus using LIN Frame Object type (refer to
    #LIN_FrmObjType). Frame Object is a basic software entity which defines how
    the transaction on LIN bus shall happen.  Each Frame Object defines
    parameters such as Frame Identifier, Response type, Data length, etc.

    @section sec_lin_fsm State Machine
    This section describes state machines for driver and LIN communication.
    @image html lin_state_machine.jpg "Component state machine"

    @section sec_lin_seq Sequence Diagrams
    @subsection subsec_lin_seq_master LIN Transmission with Master Response
    The following sequence diagram depicts an example of LIN Frame Transmission
    sequence with a Master response for LIN driver.
    @image html lin_seq_diagram_master.jpg

    @subsection subsec_lin_seq_slave LIN Transmission with Slave Response
    The following sequence diagram depicts an example of LIN Frame Transmission
    sequence with a Slave response for LIN driver.
    @image html lin_seq_diagram_slave.jpg

    @subsection subsec_lin_seq_monitor LIN Transmission with Monitor Response
    The following sequence diagram depicts an example of LIN Frame Transmission
    sequence with Monitor response for LIN driver.
    @image html lin_seq_diagram_monitor.jpg

    @section sec_lin_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @limitations None

    @file lin.h
    @brief LIN driver application interface
    This header file contains the APIs for LIN driver
    @version 0.50 Imported from docx
*/

#ifndef LIN_H
#define LIN_H

#include <stddef.h>
#include <inttypes.h>

/**
    @name LIN API IDs
    @{
    @brief API IDs for LIN driver.
*/
#define BRCM_SWARCH_LIN_HW_ID_TYPE          (0x00U) /**< @brief #LIN_HwIDType */
#define BRCM_SWARCH_LIN_BAUD_RATE_TYPE      (0x01U) /**< @brief #LIN_BaudrateType */
#define BRCM_SWARCH_LIN_FRM_CHECKSUM_TYPE   (0x02U) /**< @brief #LIN_FrmChecksumType */
#define BRCM_SWARCH_LIN_STATUS_TYPE         (0x03U) /**< @brief #LIN_StatusType */
#define BRCM_SWARCH_LIN_WAKEUP_DETECT_TYPE  (0x04U) /**< @brief #LIN_WakeupDetectType */
#define BRCM_SWARCH_LIN_FRM_RESP_TYPE       (0x05U) /**< @brief #LIN_FrmResponseType */
#define BRCM_SWARCH_LIN_LOOP_BACK_MODE_TYPE (0x06U) /**< @brief #LIN_LoopbackModeType */
#define BRCM_SWARCH_LIN_ERROR_TYPE          (0x07U) /**< @brief #LIN_ErrorType */
#define BRCM_SWARCH_LIN_FRM_OBJ_TYPE        (0x08U) /**< @brief #LIN_FrmObjType */
#define BRCM_SWARCH_LIN_CONFIG_TYPE         (0x09U) /**< @brief #LIN_ConfigType */
#define BRCM_SWARCH_LIN_INIT_PROC           (0x0AU) /**< @brief #LIN_Init */
#define BRCM_SWARCH_LIN_DEINIT_PROC         (0x0BU) /**< @brief #LIN_DeInit */
#define BRCM_SWARCH_LIN_TRANSMIT_FRAME_PROC (0x0CU) /**< @brief #LIN_TransmitFrame */
#define BRCM_SWARCH_LIN_SEND_SLEEP_PROC     (0x0DU) /**< @brief #LIN_SendSleep */
#define BRCM_SWARCH_LIN_SEND_WAKEUP_PROC    (0x0EU) /**< @brief #LIN_SendWakeup */
#define BRCM_SWARCH_LIN_READ_STATUS_PROC    (0x0FU) /**< @brief #LIN_ReadStatus */
#define BRCM_SWARCH_LIN_GET_DATA_PROC       (0x01U) /**< @brief #LIN_GetData */
/** @} */

/**
    @name LIN_HwIDType
    @{
    @brief Type for LIN Hardware/Controller Identifier.

    @trace #BRCM_SWREQ_LIN_HW_ID
*/
typedef uint32_t LIN_HwIDType;  /**< @brief LIN controller ID type */
/** @} */

/**
    @name LIN_BaudrateType
    @{
    @brief LIN baud rate type

    @trace #BRCM_SWREQ_LIN_BAUD_RATE
*/
typedef uint32_t LIN_BaudrateType;  /**< @brief LIN baud rate type */
#define LIN_BAUD_9600       (0UL)   /**< @brief LIN Baudrate of 9600bps */
#define LIN_BAUD_19200      (1UL)   /**< @brief LIN Baudrate of 19200bps */
/** @} */

/**
    @name LIN_FrmChecksumType
    @{
    @brief Type for LIN Frame Checksum Configuration

    @trace #BRCM_SWREQ_LIN_FRM_CHECKSUM
*/
typedef uint32_t LIN_FrmChecksumType;   /**< @brief LIN Frame Checksum type */
#define LIN_CHECKSUM_CLASSIC    (0UL)   /**< @brief LIN Classic Checksum */
#define LIN_CHECKSUM_ENHANCED   (1UL)   /**< @brief LIN Enhanced Checksum */
/** @} */

/**
    @name LIN_StatusType
    @{
    @brief LIN driver status

    @trace #BRCM_SWREQ_LIN_STATUS
*/
typedef int32_t LIN_StatusType;         /**< @brief LIN driver status type */
#define LIN_STATUS_UNINIT       (0x00)  /**< @brief LIN driver uninitialized */
#define LIN_TX_BUSY             (0x01)  /**< @brief LIN transmition in progress */
#define LIN_RX_BUSY             (0x02)  /**< @brief LIN reception in progress */
#define LIN_STATUS_SLEEP        (0x03)  /**< @brief LIN in sleep state */
#define LIN_NORMAL_OPERATION    (0x04)  /**< @brief LIN in normal operation */
#define LIN_RX_OK               (0x05)  /**< @brief LIN data reception ok */
#define LIN_TX_OK               (0x06)  /**< @brief LIN frame transmition ok */
#define LIN_TX_HEADER_ERR       (0x07)  /**< @brief LIN header transmit error */
#define LIN_TX_RESPONSE_ERR     (0x08)  /**< @brief LIN response tramsit error */
#define LIN_RX_ERROR            (0x09)  /**< @brief LIN data reception error */
#define LIN_RX_NO_RESPONSE      (0x0A)  /**< @brief No LIN response received */
#define LIN_ERR_UNKNOWN         (0x0B)  /**< @brief Unknown LIN error */
/** @} */

/**
    @name LIN_WakeupDetectType
    @{
    @brief Wake up detection configuration

    @trace #BRCM_SWREQ_LIN_WAKEUP_DETECT
*/
typedef uint32_t LIN_WakeupDetectType;      /**< @brief Wake up detection type */
#define LIN_WAKEUP_DETECT_DISABLED  (0UL)   /**< @brief LIN wakeup detection
                                                 from bus activity disabled */
#define LIN_WAKEUP_DETECT_ENABLED   (1UL)   /**< @brief LIN wakeup detection
                                                 from bus activity enabled */
/** @} */

/**
    @name LIN_FrmResponseType
    @{
    @brief Type for LIN frame response

    @trace #BRCM_SWREQ_LIN_FRM_RESP
*/
typedef uint32_t LIN_FrmResponseType;   /**< @brief LIN frame response type */
#define LIN_FRM_TX          (0UL)       /**< @brief Response data bytes are
                                             transmitted by the master */
#define LIN_FRM_RX          (1UL)       /**< @brief Response data bytes are
                                             received by the master */
#define LIN_FRM_MON         (2UL)       /**< @brief Response data bytes are
                                             monitored by master (sent from one
                                             slave to another slave) */
/** @} */

/**
    @name LIN_LoopbackModeType
    @{
    @brief LIN loopback mode

    @trace #BRCM_SWREQ_LIN_LOOP_BACK_MODE
*/
typedef uint32_t LIN_LoopbackModeType;      /**< @brief LIN loopback mode
                                                 type */
#define LIN_UART_LOOPBACK_DISABLED  (0UL)   /**< @brief LIN/UART loopback mode
                                                 disabled */
#define LIN_UART_LOOPBACK_ENABLED   (1UL)   /**< @brief LIN/UART loopback mode
                                                 enabled */
/** @} */

/**
    @name LIN_ErrorType
    @{
    @brief LIN errors

    @trace #BRCM_SWREQ_LIN_ERROR
*/
typedef uint32_t LIN_ErrorType;             /**< @brief LIN error type */
#define LIN_FRAMING_ERR_MASK    (0x01UL)    /**< @brief Framing error */
#define LIN_PARITY_ERR_MASK     (0x02UL)    /**< @brief Parity error */
#define LIN_BREAK_ERR_MASK      (0x04UL)    /**< @brief Break error */
#define LIN_OVERRUN_ERR_MASK    (0x08UL)    /**< @brief Overrun error */
#define LIN_TIMEOUT_ERR_MASK    (0x10UL)    /**< @brief Timeout error */
/** @} */

/**
    @brief LIN frame object

    Type for specifying LIN frame information and pointer to data.

    @trace #BRCM_SWREQ_LIN_FRM_OBJ
*/
typedef struct {
    uint8_t frmId;                          /**< @brief Protected Identifier of
                                                 the LIN frame */
    LIN_FrmResponseType frmResponse;        /**< @brief Type of LIN frame
                                                 response */
    uint32_t frmDataLen;                    /**< @brief Data length of the LIN
                                                 frame */
    uint8_t * frmDataPtr;                   /**< @brief Pointer to data buffer
                                                 of LIN frame */
    LIN_FrmChecksumType frmChecksumModel;   /**< @brief LIN frame checksum
                                                 model */
} LIN_FrmObjType;

/**
    @brief LIN configuration structure

    @trace #BRCM_SWREQ_LIN_CONFIG
*/
typedef struct {
    LIN_BaudrateType baudrate;          /**< @brief LIN bus baudrate */
    LIN_WakeupDetectType wakeUpCfg;     /**< @brief LIN configuration for wakeup
                                             via bus activity */
    LIN_LoopbackModeType loopbackMode;  /**< @brief LIN loopback mode
                                             configuration */
} LIN_ConfigType;

/** @brief Initialize LIN controller

    This API initializes the UART Controller and internal data structures with
    given parameters.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       Index of the LIN (UART) Controller
    @param[in]      aConfig     Pointer to Driver Configuration

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_LIN_INIT

    @limitations None
*/
extern void LIN_Init(LIN_HwIDType aHwID, const LIN_ConfigType *const aConfig);

/** @brief De-initialize LIN controller

    This API de-initializes the UART and puts it back to its original state.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       Index of the LIN (UART) Controller

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_LIN_DEINIT

    @limitations None
*/
extern void LIN_DeInit(LIN_HwIDType aHwID);

/** @brief Issue transmit header request to LIN driver

    This API issues the transmit header request to LIN driver. Transmit request
    shall be processed by the driver. Depending on the type of the frame as set
    in application layer, response data bytes shall be sent from master,
    received from slave, or monitored between slaves.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       Index of the LIN Controller
    @param[in]      aFrmObj     Frame object for transmission

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transmit request successful
    @retval     #BCM_ERR_INVAL_STATE    LIN Controller is in invalid state
                                        (e.g. Power-down/Sleep mode)
    @retval     #BCM_ERR_UNINIT         LIN driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @a aHwID
    @retval     #BCM_ERR_INVAL_PARAMS   @a aFrmObj in NULL

    @post TBD

    @trace #BRCM_SWREQ_LIN_TRANSMIT_FRAME

    @limitations None
*/
extern int32_t LIN_TransmitFrame(LIN_HwIDType aHwID,
                                 const LIN_FrmObjType * const aFrmObj);

/** @brief Send sleep broadcast message

    This API sends the sleep broadcast message to all slaves on the LIN bus.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       Index of the LIN Controller

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             LIN hardware transition to low-power
                                        mode successfull
    @retval     #BCM_ERR_INVAL_STATE    LIN Driver in invalid State (e.g.,
                                        already be in sleep/low-power mode)
    @retval     #BCM_ERR_UNINIT         LIN Driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid ID for LIN controller

    @post TBD

    @trace #BRCM_SWREQ_LIN_SEND_SLEEP

    @limitations None
*/
extern int32_t LIN_SendSleep(LIN_HwIDType aHwID);

/** @brief Send wakeup pulse

    This API sends wakeup pulse to all slaves on the LIN bus.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       Index of the LIN Controller

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             LIN wakeup pulse transmitted
                                        successfully
    @retval     #BCM_ERR_INVAL_STATE    LIN driver is not in low-power/sleep
                                        mode
    @retval     #BCM_ERR_UNINIT         LIN driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid ID for LIN Controller

    @post TBD

    @trace #BRCM_SWREQ_LIN_SEND_WAKEUP

    @limitations None
*/
extern int32_t LIN_SendWakeup(LIN_HwIDType aHwID);

/** @brief Read LIN driver status

    This API retrieves the status of the LIN driver.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       Index of the LIN Controller

    Return values are documented in reverse-chronological order
    @retval     #LIN_StatusType             LIN driver status

    @post TBD

    @trace #BRCM_SWREQ_LIN_READ_STATUS

    @limitations None
*/
extern LIN_StatusType LIN_ReadStatus(LIN_HwIDType aHwID);

/** @brief Retrieve RX data

    This API retrieves RX data for a subscribed frame from LIN driver.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aHwID       Index of the LIN Controller
    @param[out]     aDataPtr    Pointer to RX data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved RX data
    @retval     #BCM_ERR_INVAL_STATE    LIN driver in invalid state (not in
                                        #LIN_RX_OK)
    @retval     #BCM_ERR_UNINIT         LIN driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   @a aDataPtr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aHwID is invalid

    @post TBD

    @trace #BRCM_SWREQ_LIN_GET_DATA

    @limitations None
*/
extern int32_t LIN_GetData(LIN_HwIDType aHwID, uint8_t *const aDataPtr);

#endif /* LIN_H */
/** @} */
