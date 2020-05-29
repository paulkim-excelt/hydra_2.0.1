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

/**
    @defgroup grp_candrv_ifc Interface
    @ingroup grp_candrv

    @addtogroup grp_candrv_ifc
    @{
    @section sec_can_overview Overview
    @image html can_overview.jpg "Driver overview diagram"
    The above figure describes CAN driver architecture and its interaction with
    clients (application) and system.

    CAN driver supports CAN and CAN FD configuration, including the second baud
    rate and Bit-Rate Switch (BRS) for CAN FD. CAN Driver Core is based on the
    CAN subsystem, which is comprised of a CAN Hardware Controller. The CAN
    Controller performs communication according to ISO11898-1 (
    @ref sec_ref_can_spec) and to @ref sec_ref_can_fd_spec. The bit rate can be
    programmed to values up to 1 Mbit/s for standard CAN mode. High bit rates
    are possible in CAN FD mode. The CAN Driver also supports transmit
    cancellation.

    The message storage is a Message RAM outside of the module and has a width
    of 32 bits. Refer to @ref sec_ref_trm for the size of the Message RAM
    connected to the CAN Controller for specific chipset.

    CAN Driver Core consist of functions for RX Handling, TX Handling, and
    Interrupt handling along with Error and Wakeup handling. All functions
    concerning handling of messages are implemented by the RX Handler and the
    TX Handler components of the CAN Controller. The RX Handler manages message
    acceptance filtering, the transfer of received messages from the CAN Core
    to the Message RAM as well as providing receive message status information.
    The TX Handler is responsible for the transfer of transmit messages from
    the Message RAM to the CAN Core as well as providing transmit status
    information.

    CAN driver manages all the CAN buses in the hardware and initiates a
    transaction on the CAN bus using Message Object type (refer to
    #CAN_TxMsgObjType). Message Object is a basic software entity which defines
    how the transaction on CAN bus shall happen. Each Message Object defines
    parameters such as Message Type (Standard / Extended), Format (Standard
    CAN / CAN FD), and Data Length Code for messages.

    @section sec_can_fsm State Machine
    @todo Add CAN state machine informations

    @section sec_can_seq Sequence Diagrams
    @subsection subsec_can_seq_diagram_tx CAN Transmit Sequence
    This is a sample sequence diagram for CAN Driver operations. CAN Driver
    operation in below diagram is represented by #CAN_Write. Other operations
    such as #CAN_Sleep, #CAN_DrvDeInit etc., follow a similar sequence.
    @image html can_seq_diagram_tx.jpg "Transmit Sequence"

    @subsection subsec_cam_seq_diagram_rx CAN Receiver Sequence
    @image html can_seq_diagram_rx.jpg "Transmit Sequence"

    @section sec_can_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_can.dep

    @limitations None

    @file can.h
    @brief CAN driver application interface
    This header file contains the Application programmer interfaces for CAN
    driver
    @version 0.96 Imported from docx
*/

#ifndef CAN_H
#define CAN_H

#include <stdint.h>

/**
  @name CAN Driver architecture IDs
  @{
  @brief Architecture IDs for CAN Driver
*/
#define BRCM_SWARCH_CAN_MAXDATASIZE_MACRO           (0x00U) /**< @brief #CAN_MAX_DATA_SIZE          */
#define BRCM_SWARCH_CAN_FDMAXDATASIZE_MACRO         (0x00U) /**< @brief #CAN_FD_MAX_DATA_SIZE       */
#define BRCM_SWARCH_CAN_HWID_TYPE                   (0x00U) /**< @brief #CAN_HwIDType               */
#define BRCM_SWARCH_CAN_MSGID_TYPE                  (0x01U) /**< @brief #CAN_MsgIDType              */
#define BRCM_SWARCH_CAN_MSGFORMAT_TYPE              (0x02U) /**< @brief #CAN_MsgFormatType          */
#define BRCM_SWARCH_CAN_ERROR_TYPE                  (0x03U) /**< @brief #CAN_ErrorType              */
#define BRCM_SWARCH_CAN_TXBUFCFG_TYPE               (0x04U) /**< @brief #CAN_TxBufCfgType           */
#define BRCM_SWARCH_CAN_TXESICFG_TYPE               (0x05U) /**< @brief #CAN_TxESICfgType           */
#define BRCM_SWARCH_CAN_RXMSGFLAGS_TYPE             (0x06U) /**< @brief #CAN_RxMsgFlagsType         */
#define BRCM_SWARCH_CAN_RXFILTERBUFCFG_TYPE         (0x07U) /**< @brief #CAN_RxFilterBufCfgType     */
#define BRCM_SWARCH_CAN_MSGFILTER_TYPE              (0x08U) /**< @brief #CAN_MsgFilterType          */
#define BRCM_SWARCH_CAN_RXFLTRNOMATCHCFG_TYPE       (0x09U) /**< @brief #CAN_RxFltrNoMatchCfgType   */
#define BRCM_SWARCH_CAN_TXBUFMODE_TYPE              (0x0AU) /**< @brief #CAN_TxBufModeType          */
#define BRCM_SWARCH_CAN_RXFIFOOPMODE_TYPE           (0x0BU) /**< @brief #CAN_RxFifoOpModeType       */
#define BRCM_SWARCH_CAN_MSGRAMDATASZ_TYPE           (0x0CU) /**< @brief #CAN_MsgRAMDataSzType       */
#define BRCM_SWARCH_CAN_ERRCNT_TYPE                 (0x0DU) /**< @brief #CAN_ErrCntType             */
#define BRCM_SWARCH_CAN_MSGINFO_TYPE                (0x0EU) /**< @brief #CAN_MsgInfoType            */
#define BRCM_SWARCH_CAN_RXMSGOBJ_TYPE               (0x0FU) /**< @brief #CAN_RxMsgObjType           */
#define BRCM_SWARCH_CAN_TXMSGOBJ_TYPE               (0x10U) /**< @brief #CAN_TxMsgObjType           */
#define BRCM_SWARCH_CAN_RXFILTEROBJ_TYPE            (0x11U) /**< @brief #CAN_RxFilterObjType        */
#define BRCM_SWARCH_CAN_FILTERCFG_TYPE              (0x12U) /**< @brief #CAN_FilterCfgType          */
#define BRCM_SWARCH_CAN_BAUDRATECFG_TYPE            (0x13U) /**< @brief #CAN_BaudRateCfgType        */
#define BRCM_SWARCH_CAN_FDBAUDRATECFG_TYPE          (0x14U) /**< @brief #CAN_FdBaudRateCfgType      */
#define BRCM_SWARCH_CAN_FDCFG_TYPE                  (0x15U) /**< @brief #CAN_FdCfgType              */
#define BRCM_SWARCH_CAN_MSGRAMFLTRCFG_TYPE          (0x16U) /**< @brief #CAN_MsgRAMFltrCfgType      */
#define BRCM_SWARCH_CAN_MSGRAMRXFIFOCFG_TYPE        (0x17U) /**< @brief #CAN_MsgRAMRxFifoCfgType    */
#define BRCM_SWARCH_CAN_MSGRAMRXBUFCFG_TYPE         (0x18U) /**< @brief #CAN_MsgRAMRxBufCfgType     */
#define BRCM_SWARCH_CAN_MSGRAMTXBUFCFG_TYPE         (0x19U) /**< @brief #CAN_MsgRAMTxBufCfgType     */
#define BRCM_SWARCH_CAN_MSGRAMTXEVTCFG_TYPE         (0x1AU) /**< @brief #CAN_MsgRAMTxEvtCfgType     */
#define BRCM_SWARCH_CAN_MSGRAMCFG_TYPE              (0x1BU) /**< @brief #CAN_MsgRAMCfgType          */
#define BRCM_SWARCH_CAN_CONFIG_TYPE                 (0x1CU) /**< @brief #CAN_ConfigType             */
#define BRCM_SWARCH_CAN_INIT_PROC                   (0x1DU) /**< @brief #CAN_Init                   */
#define BRCM_SWARCH_CAN_DEINIT_PROC                 (0x1EU) /**< @brief #CAN_DeInit                 */
#define BRCM_SWARCH_CAN_WRITE_PROC                  (0x1FU) /**< @brief #CAN_Write                  */
#define BRCM_SWARCH_CAN_SLEEP_PROC                  (0x20U) /**< @brief #CAN_Sleep                  */
#define BRCM_SWARCH_CAN_WAKEUP_PROC                 (0x21U) /**< @brief #CAN_WakeUp                 */
#define BRCM_SWARCH_CAN_GETERRCNTS_PROC             (0x22U) /**< @brief #CAN_GetErrCnts             */
/** @} */


/**
    @name CAN_HwIDType
    @{
    @brief CAN hardware/controller identifier

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_HwIDType;          /**< @brief CAN controller ID type  */
#define CAN_HWID_0             (0UL)    /**< @brief CAN contoller 0 ID      */
/** @} */

/**
    @name CAN_MsgIDType
    @{
    @brief CAN Message Identifier

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_MsgIDType;        /**< @brief CAN Message Identifier type  */
#define CAN_MSGID_STANDARD     (0UL)   /**< @brief Standard (11-bit) CAN Message
                                                    Identifier                  */
#define CAN_MSGID_EXTENDED     (1UL)   /**< @brief Extended (29-bit) CAN Message
                                                    Identifier                  */
/** @} */

/**
    @name CAN_MsgFormatType
    @{
    @brief CAN Message Format

    Detailed description of COMP_State.

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_MsgFormatType;    /**< @brief CAN Message Format type      */
#define CAN_MSGFORMAT_STANDARD (0UL)   /**< @brief Standard CAN Message Format  */
#define CAN_MSGFORMAT_CANFD    (1UL)   /**< @brief CAN FD Message Format        */
/** @} */

/**
    @name CAN_ErrorType
    @{
    @brief CAN Driver Errors

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_ErrorType;         /**< @brief CAM error type          */
#define CAN_ERROR_STUFF         (1UL)   /**< @brief CAN stuff error         */
#define CAN_ERROR_FORM          (2UL)   /**< @brief CAN form error          */
#define CAN_ERROR_ACK           (3UL)   /**< @brief CAN acknowledge error   */
#define CAN_ERROR_BIT1          (4UL)   /**< @brief CAN bit 1 error         */
#define CAN_ERROR_BIT0          (5UL)   /**< @brief CAN bit 0 error         */
#define CAN_ERROR_CRC           (6UL)   /**< @brief CAN CRC error           */
/** @} */

/**
    @name CAN_TxBufCfgType
    @{
    @brief CAN TX buffer/FIFO/queue configuration

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_TxBufCfgType;        /**< @brief CAN TX buffer configuration
                                                        type                    */
#define CAN_TXBUFCFG_BUFFER       (0UL)   /**< @brief Use transmit buffer       */
#define CAN_TXBUFCFG_FIFO_QUEUE   (1UL)   /**< @brief Use transmit FIFO/queue   */
/** @} */

/**
    @name CAN_TxESICfgType
    @{
    @brief CAN Error State Indicator configuration of message

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_TxESICfgType;          /**< @brief CAN Error State Indicator
                                                        configuration               */
#define CAN_TXESICFG_ACTIVE        (0UL)    /**< @brief message error state indicator
                                                        set                         */
#define CAN_TXESICFG_PASSIVE       (1UL)    /**< @brief message error state indicator
                                                        transmitted recessive       */
/** @} */

/**
    @name CAN_RxMsgFlagsType
    @{
    @brief CAN Rx Message flags

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_RxMsgFlagsType;                     /**< @brief Rx Message Flags type              */
#define CAN_RXMSGFLAGS_RTR            (0x00000001UL)     /**< @brief Remote Transmission Request Flag   */
#define CAN_RXMSGFLAGS_XTD            (0x00000002UL)     /**< @brief Extended Identifier Flag           */
#define CAN_RXMSGFLAGS_ESI            (0x00000004UL)     /**< @brief Error State Indicator Flag         */
#define CAN_RXMSGFLAGS_BRS            (0x00000008UL)     /**< @brief Bit Rate Switch Flag               */
#define CAN_RXMSGFLAGS_FDF            (0x00000010UL)     /**< @brief FD Frame Flag                      */
#define CAN_RXMSGFLAGS_DLC            (0x00000F00UL)     /**< @brief Data Length Code                   */
/** @} */

/**
    @name CAN_RxFilterBufCfgType
    @{
    @brief CAN RX Buffer Configuration

    @note Refer @ref sec_ref_trm for RX FIFO and buffer support

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_RxFilterBufCfgType;    /**< @brief CAN RX Buffer configuration */
#define CAN_RXFILTERBUFCFG_FIFO_0   (1UL)   /**< @brief use rx fifo 0 for message
                                                        reception                   */
#define CAN_RXFILTERBUFCFG_FIFO_1   (2UL)   /**< @brief use rx fifo 1 for message
                                                        reception                   */
#define CAN_RXFILTERBUFCFG_BUFFER   (7UL)   /**< @brief use rx buffer for message
                                                        reception                   */
/** @} */

/**
    @name CAN_MsgFilterType
    @{
    @brief Type of filter element

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_MsgFilterType;        /**< @brief Type of filter element           */
#define CAN_MSGFILTER_RANGE        (0UL)   /**< @brief Range filter for message
                                                        reception from filter id 1 to
                                                        filter id 2                     */
#define CAN_MSGFILTER_DUAL         (1UL)   /**< @brief Use dual filter for specific
                                                        message reception for filter id 1
                                                        or filter id 2                  */
#define CAN_MSGFILTER_CLASSIC      (2UL)   /**< @brief Use classic bit mask filter
                                                        for message reception where filter
                                                        id 1 = filter and filter id 2 =
                                                        mask                            */
#define CAN_MSGFILTER_DISABLED     (3UL)   /**< @brief Filter element disabled          */
/** @} */

/**
    @name CAN_RxFltrNoMatchCfgType
    @{
    @brief CAN RX non matching frames configuration

    Detailed description of COMP_State.

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_RxFltrNoMatchCfgType;          /**< @brief  RX non matching frames configuration type  */
#define CAN_RXFLTRNOMATCHCFG_ACCEPT_FIFO0    (0UL)  /**< @brief Accept non-matching frames in RX FIFO 0     */
#define CAN_RXFLTRNOMATCHCFG_ACCEPT_FIFO1    (1UL)  /**< @brief Accept non-matching frames in RX FIFO 1     */
#define CAN_RXFLTRNOMATCHCFG_REJECT          (2UL)  /**< @brief Reject non-matching frames                  */
/** @} */

/**
    @name CAN_TxBufModeType
    @{
    @brief CAN FIFO/Queue configuration

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_TxBufModeType;        /**< @brief CAN FIFO/Queue configuration type        */
#define CAN_TXBUFMODE_FIFO         (0UL)   /**< @brief Use FIFO for CAN message transmission    */
#define CAN_TXBUFMODE_QUEUE        (1UL)   /**< @brief Use Queue for CAN message transmission   */
/** @} */

/**
    @name CAN_RxFifoOpModeType
    @{
    @brief RX FIFO operation mode configuration

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_RxFifoOpModeType;      /**< @brief RX FIFO operation mode
                                                        configuration type      */
#define CAN_RXFIFOOPMODE_BLOCKING   (0UL)   /**< @brief FIFO blocking mode      */
#define CAN_RXFIFOOPMODE_OVERWRITE  (1UL)   /**< @brief FIFO overwrite mode     */
/** @} */

/**
    @name CAN_MsgRAMDataSzType
    @{
    @brief CAN MSGRAM data elemements size

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef uint32_t CAN_MsgRAMDataSzType;     /**< @brief Data field size type */
#define CAN_MSGRAMDATASZ_8         (0UL)   /**< @brief 8 byte data field    */
#define CAN_MSGRAMDATASZ_12        (1UL)   /**< @brief 12 byte data field   */
#define CAN_MSGRAMDATASZ_16        (2UL)   /**< @brief 16 byte data field   */
#define CAN_MSGRAMDATASZ_20        (3UL)   /**< @brief 20 byte data field   */
#define CAN_MSGRAMDATASZ_24        (4UL)   /**< @brief 24 byte data field   */
#define CAN_MSGRAMDATASZ_32        (5UL)   /**< @brief 32 byte data field   */
#define CAN_MSGRAMDATASZ_48        (6UL)   /**< @brief 48 byte data field   */
#define CAN_MSGRAMDATASZ_64        (7UL)   /**< @brief 64 byte data field   */
/** @} */

/**
    @brief CAN error counter information

    Structure containing CAN error information

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_ErrCntType {
    uint32_t tec;    /**< @brief Transmit Error Counter */
    uint32_t rec;    /**< @brief Receive Error Counter  */
    uint32_t rp;     /**< @brief Receive Error Passive  */
    uint32_t cel;    /**< @brief CAN Error Logging      */
} CAN_ErrCntType;

/**
    @brief CAN message information

    Structure containing CAN message information and data pointer

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_MsgInfoType {
     uint32_t       msgID;           /**< @brief CAN message ID                 */
     CAN_MsgIDType  msgIDType;       /**< @brief Type of CAN message ID         */
     uint32_t       length;          /**< @brief Data length of the CAN
                                                message                         */
     uint8_t       *dataPtr;         /**< @brief pointer to data buffer of CAN
                                                message                         */
     CAN_RxMsgFlagsType msgFlags;    /**< @brief CAN Message Flags, used for RX
                                                messages only                      */
} CAN_MsgInfoType;

/**
    @brief RX message objects

    Structure for specifying RX message objects

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_RxMsgObjType {
    CAN_MsgInfoType     msgInfo;            /**< @brief Contains CAN message
                                                        information and pointer to received data    */
    CAN_RxMsgFlagsType msgFlags;            /**< @brief CAN Message Flags, used for RX
                                                        messages only                               */
} CAN_RxMsgObjType;

/**
    @brief TX message objects

    Structure for specifying TX message objects

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_TxMsgObjType {
    CAN_MsgFormatType   msgFormat;          /**< @brief CAN message format                  */
    uint32_t            bitRateSwitchEn;    /**< @brief Enable bit rate switch during TX    */
    CAN_TxBufCfgType    txBufCfg;           /**< @brief Configuration of CAN TX
                                                        message object to use CAN TX
                                                        buffer/FIFO/queue                   */
    CAN_MsgInfoType     msgInfo;            /**< @brief Contains CAN message
                                                        information and pointer to data
                                                        for transmission                    */
    CAN_TxESICfgType    errStateIndCfg;     /**< @brief Enable Error State Indicator        */
    uint32_t            remoteTxRequestEn;  /**< @brief Enable Remote Transmission Request  */
} CAN_TxMsgObjType;

/**
    @brief RX message filter object type

    Structure specifying receive messages and filter elements

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_RxFilterObjType {
    CAN_MsgIDType             msgIDType;       /**< @brief CAN message identifier       */
    CAN_RxFilterBufCfgType    bufCfg;          /**< @brief filtered packet destination  */
    CAN_MsgFilterType         filterType;      /**< @brief Filter type to use for
                                                            CAN message reception       */
    uint32_t                  filterID1;       /**< @brief Value to use for filter
                                                            ID 1                        */
    uint32_t                  filterID2;       /**< @brief Value to use for filter
                                                            ID 2                        */
} CAN_RxFilterObjType;

/**
    @brief RX message filter configuration structure type

    Structure specifying RX filter configuration

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_FilterCfgType {
    const CAN_RxFilterObjType   *filterObjPtr;
    uint32_t                    filterObjSz;
    CAN_RxFltrNoMatchCfgType    rxNoMatchStd; /**< @brief CAN RX non-matching               */
    CAN_RxFltrNoMatchCfgType    rxNoMatchExt; /**< @brief CAN RX non-matching extended ID   */
} CAN_FilterCfgType;

/**
    @brief CAN baud rate configuration structure type

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_BaudRateCfgType {
    uint8_t  propSeg;       /**< @brief propagation delay in time quanta                */
    uint8_t  seg1;          /**< @brief phase segment 1 in time quantas                 */
    uint8_t  seg2;          /**< @brief phase segment 2 in time quantas                 */
    uint8_t  syncJumpWidth; /**< @brief Synchronization jump width in time quatas       */
    uint16_t prescale;      /**< @brief The value by which the oscillator frequency is
                                        divided for generating the bit time quanta      */
} CAN_BaudRateCfgType;

/**
    @brief CAN FD baud rate configuration structure type

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_FdBaudRateCfgType {
    uint8_t  propSeg;           /**< @brief propagation delay in time quanta                */
    uint8_t  seg1;              /**< @brief phase segment 1 in time quantas                 */
    uint8_t  seg2;              /**< @brief phase segment 2 in time quantas                 */
    uint8_t  syncJumpWidth;     /**< @brief Synchronization jump width in time quatas       */
    uint8_t  delayCompOffset;   /**< @brief Transmit delay coompensation offset             */
    uint8_t  delayCompFilter;   /**< @brief Transmit delay coompensation filter             */
    uint16_t prescale;          /**< @brief The value by which the oscillator frequency is
                                            divided for generating the bit time quanta      */
} CAN_FdBaudRateCfgType;

/**
    @brief CAN FD baud configuration structure type

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_FdCfgType {
    uint32_t                enable;               /**< @brief Enable/Disable CAN FD operation   */
    uint32_t                bitRateSwitchEn;      /**< @brief Enable/Disable Tx bit rate switch */
    uint32_t                delayCompEn;          /**< @brief Enable/Disable Delay compensation */
    CAN_FdBaudRateCfgType   rateCfg;              /**< @brief FD rate configuration             */
} CAN_FdCfgType;

/**
    @brief CAN MSGRAM Filter configuration structure type

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_MsgRAMFltrCfgType {
    uint32_t    startAddr;     /**< @brief start address of the Filter elements */
    uint32_t    numElements;   /**< @brief Number of filter elements            */
} CAN_MsgRAMFltrCfgType;

/**
    @brief CAN MSGRAM RX FIFO configuration

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_MsgRAMRxFifoCfgType {
    uint32_t             startAddr;   /**< @brief start address of the RX FIFO      */
    uint32_t             numElements; /**< @brief number of elements in RX FIFO     */
    uint32_t             elementSz;   /**< @brief size of each element              */
    CAN_RxFifoOpModeType mode;        /**< @brief operation mode of the FIFO        */
    uint32_t             waterMark;   /**< @brief FIFO watermark (for interrupt)    */
} CAN_MsgRAMRxFifoCfgType;

/**
    @brief CAN MSGRAM RX Buffer configuration

    CAN message RAM RX Buffer configuration structure

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_MsgRAMRxBufCfgType {
    uint32_t startAddr; /**< @brief start address of the RX Buffer      */
    uint32_t elementSz; /**< @brief size of each element in Rx buffer   */
} CAN_MsgRAMRxBufCfgType;

/**
    @brief CAN MSGRAM TX Event configuration

    CAN message RAM TX event configuration structure

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_MsgRAMTxEvtCfgType {
    uint32_t startAddr;     /**< @brief start address of the Tx event elements  */
    uint32_t numElements;   /**< @brief number of Tx event elements             */
    uint32_t waterMark;     /**< @brief watermark level (for interrupt)         */
} CAN_MsgRAMTxEvtCfgType;

/**
    @brief CAN MSGRAM TX Buffer configuration

    CAN message RAM TX buffer configuration structure

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_MsgRAMTxBufCfgType {
    uint32_t             startAddr;         /**< @brief start address of Tx buffer elements */
    uint32_t             fifoQueueSize;     /**< @brief number of Tx buffer elements        */
    CAN_MsgRAMDataSzType elementSz;         /**< @brief size of each element                */
    CAN_TxBufModeType    mode;              /**< @brief buffer mode type                    */
    uint32_t             numDedicatedBufs;  /**< @brief number of dedicated TX buffers      */
} CAN_MsgRAMTxBufCfgType;

/**
    @brief CAN MSGRAM configuration

    CAN message RAM configuration structure

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_MsgRAMCfgType {
    CAN_MsgRAMFltrCfgType   stdFltrCfg; /**< @brief standard filter configuration   */
    CAN_MsgRAMFltrCfgType   extFltrCfg; /**< @brief extended filter configuration   */
    CAN_MsgRAMRxFifoCfgType fifo0Cfg;   /**< @brief RX FIFO0 configuration          */
    CAN_MsgRAMRxFifoCfgType fifo1Cfg;   /**< @brief RX FIFO1 configuration          */
    CAN_MsgRAMRxBufCfgType  rxBufCfg;   /**< @brief RX buffer configuration         */
    CAN_MsgRAMTxEvtCfgType  txEventCfg; /**< @brief TX event configuration          */
    CAN_MsgRAMTxBufCfgType  txBufCfg;   /**< @brief Tx buffer configuration         */
} CAN_MsgRAMCfgType;

/**
    @brief CAN configuration structure

    @trace #BRCM_SWREQ_CAN_KERNEL_INTERFACE
*/
typedef struct _CAN_ConfigType {
    const CAN_BaudRateCfgType baudRateCfg;          /**< @brief Baud rate configuration                 */
    const CAN_FdCfgType       fdCfg;                /**< @brief Baud rate configuration                 */
    const CAN_FilterCfgType  *filterCfgPtr;         /**< @brief CAN RX filter configuration             */
    uint32_t                  loopbackModeEn;       /**< @brief Enable Loopback mode                    */
    uint32_t                  autoBusOffRecoveryEn; /**< @brief Enable Auto bus off Recovery            */
    CAN_MsgRAMCfgType         msgRAMCfg;            /**< @brief message RAM configuration               */
    uint32_t                  pxhdEn;               /**< @brief Enable CAN Protocol Exception Handling  */
    uint32_t                  rmtFrmEn;             /**< @brief Enable CAN Remote Frame                 */
} CAN_ConfigType;

/** @brief Intialize CAN driver

    This API initializes CAN controller, necessary software variables, data
    structures, and the Message RAM.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the CAN controller
    @param[in]      aConfig     Pointer to CAN driver configuration

    @return     void

    @post None

    @trace  #BRCM_SWREQ_CAN_INIT

    @limitations None
*/
extern void CAN_Init(CAN_HwIDType aID, const CAN_ConfigType * const aConfig);

/** @brief De-initialize CAN driver

    This API initializes CAN controller, necessary software variables, data
    structures, and the Message RAM.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Index of the CAN controller

    @return     void

    @post None

    @trace  #BRCM_SWREQ_CAN_INIT

    @limitations None
*/
extern void CAN_DeInit(CAN_HwIDType aID);

/** @brief CAN write request

    This API issues the Write request to CAN driver. Write request is processed
    by the driver and when completed, client is notified via TX complete
    callback.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller
    @param[inout]   aTxObj      Message object for transmission

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Write request successful
    @retval     #BCM_ERR_INVAL_PARAMS   @p aTxObj is NULL and/or<BR>
                                        @p aID is invalid
    @retval     #BCM_ERR_BUSY           TX Queue is full or <BR>
                                        TX buffers are full
    @retval     #BCM_ERR_INVAL_PARAMS   @p aTxObj->msgInfo.length is
                                        greater than 15
    @retval     #BCM_ERR_INVAL_STATE    CAN controller is in invalid state
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized

    @post TBD

    @trace  #BRCM_SWREQ_CAN_COMMUNICATION

    @limitations None
*/
extern int32_t CAN_Write(CAN_HwIDType aID, CAN_TxMsgObjType * const aTxObj);

/** @brief Low-Power/Sleep mode

    This API moves the CAN controller to Low-Power/Sleep mode.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             CAN Controller successful transition to
                                        Low-Power mode
    @retval     #BCM_ERR_TIME_OUT       Time out while waiting for CAN
                                        controller to transition to Low-Power
                                        mode
    @retval     #BCM_ERR_INVAL_STATE    CAN driver in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS   @p aID is invalid
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized

    @post TBD

    @trace  #BRCM_SWREQ_CAN_POWER

    @limitations None
*/
extern int32_t CAN_Sleep(CAN_HwIDType aID);

/** @brief Wakeup controller from Low-Power/Sleep mode

    This API brings the CAN controller out of Low-Power/Sleep mode.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of the CAN controller

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             CAN controller successful woken up from
                                        Low-Power mode
    @retval     #BCM_ERR_TIME_OUT       Time out while waiting for CAN
                                        controller to wake up from Low-Power
                                        mode
    @retval     #BCM_ERR_INVAL_STATE    CAN driver in invalid state
    @retval     #BCM_ERR_INVAL_PARAMS   @p aID is invalid
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized

    @post TBD

    @trace  #BRCM_SWREQ_CAN_POWER

    @limitations None
*/
extern int32_t CAN_WakeUp(CAN_HwIDType aID);

/** @brief Get Error Counter information

    This API provides Error Counter information.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID            Index of the CAN controller
    @param[out]     aErrCntDesc    Pointer to Error Counter Description Structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Error Counter info retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   @p aErrCntDesc is NULL and/or<BR>
                                        @p aID is invalid
    @retval     #BCM_ERR_INVAL_STATE    CAN driver in invalid state
    @retval     #BCM_ERR_UNINIT         CAN driver is not initialized


    @post TBD

    @trace  #BRCM_SWREQ_CAN_COMMUNICATION

    @limitations None
*/

extern int32_t CAN_GetErrCnts(CAN_HwIDType aID, CAN_ErrCntType * const aErrCntDesc);

#endif /* CAN_H */
/** @} */
