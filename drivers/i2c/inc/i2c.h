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
    @defgroup grp_i2cdrv_ifc Interface
    @ingroup grp_i2cdrv

    @addtogroup grp_i2cdrv_ifc
    @{
    @section sec_i2c_overview Overview
    Broadcom IIC (BSC) is multi-master multi-slave serial bus. IIC bus is a two
    wire protocol, one wire for data (SDA) and another for the clock (SCL).
    Communication on the bus always starts by master and followed by slave
    response.
    @image html i2c_overview.jpg "IIC Driver Overview"

    IIC driver supports the following features:
    - Initialize IIC bus in master mode
    - Automatically send a start/stop condition on the bus during transaction
    - Works in interrupt mode
    - Capability to handle all IIC buses in the system

    Above figure describes driver architecture and its interactions with
    clients/application and the system.

    IIC driver manages all the buses in the chipset and initiates a transaction
    with IIC slave based on the i2c packet type.

    IIC driver has dedicated request queue for each IIC bus controller. Client
    requests are queued to the driver request queue; driver processes the
    requests and passes back the processed data to client in the same request
    packet.

    @section sec_i2c_pkt IIC Packets
    IIC driver manages all the buses and initiates transaction with an IIC
    slave based on the IIC packet type (see #IIC_PktType). IIC transfer packet
    defines how the transaction on the IIC bus shall happen. Each IIC packet
    defines the transaction type (Read/Write) and length of the transaction.
    Driver shall take care of generating START/RESTART and STOP condition at the
    start/restart/end of the each transaction. Client/application may need to
    form multiple packets in one transaction based upon the IIC slave protocol.

    @subsection subsec_i2c_single_read IIC Single Read
    Let’s take an example of IIC slave (address: 0x80) which is a 7-bit slave
    and supports an 8-bit addressable memory, and defines the IIC register read
    protocol as following:
    @image html i2c_single_read_8bit.jpg

    ST: START condition<BR>
    SA: Slave address<BR>
    RW: READ/WRITE bit<BR>
    REGADDR: Register Address<BR>
    A: ACK<BR>
    RS: Repeated start<BR>
    N: NACK<BR>
    DATA: Data<BR>
    SP: STOP condition<BR>

    To read register 0xAA (8-bit address) from this slave, client need to form
    following packet format:
    @code{.c}
    function i2c_reg_read()  {
        I2c_PktType I2cPkt[2];
        RegAddr = 0xAA;
        uint8_t value;

        I2cPkt[0].slaveAddr = 0x80;
        I2cPkt[0].Length = 1;
        I2cPkt[0].data = &RegAddr;
        I2cPkt[0].flags = IIC_FLG_OP_WR;

        I2cPkt[1].slaveAddr = 0x80;
        I2cPkt[1].length = 1;
        I2cPkt[1].data = &value;
        I2cPkt[1].flags = IIC_PKT_FLG_OP_RD;

        IIC_TransferRequest(0, I2cPkt, 2, MsgQInfo);
    }
    @endcode

    @note First packet is a write transaction which sends Register address to
    the device
    @note I2cPkt[0].flags is not set as IIC_PKT_FLG_OP_RD to denote it’s a write
    request
    @note Second packet is read transaction which reads the data from the slave

    Driver shall generate START condition on the bus for the very first packet
    (fresh transfer), followed by sending of the slave address and then it waits
    for an acknowledgement from the slave. During slave address transmission, if
    #IIC_PKT_FLG_OP_RD flag is not set, driver shall set RW flag (8th bit after
    start condition) to 0 otherwise it will set it to 1.

    After slave ACKs for slave address, driver reads the data from the "data"
    field of the packet and send it over bus and wait for ACK from slave again.
    Upon ACK from slave, driver sends the next data on the bus from the "data"
    field if "length" is greater than one.

    After completing the first packet, driver sends "RESTART" condition on the
    bus and processes the next packet in same manner as the previous packet.
    During the transfer sequence if slave does not ACK for any byte transmitted
    on the bus, driver stops the transfer and returns an error to the caller.
    For the last byte, driver automatically generates NACK condition followed by
    STOP condition.

    If IIC slave supports 16-bit addressable memory and defines the IIC register
    read protocol as following:
    @image html i2c_single_read_16bit.jpg

    To read register 0xAABB for this slave, client shall use the following
    packet format:

    @code{.c}
    function i2c_read() {
        I2c_PktType I2cPkt[2];
        RegAddr0 = 0xAA;
        RegAddr1 = 0xBB;
        uint8_t value;

        I2cPkt[0].slaveAddr = 0x80;
        I2cPkt[0].length = 2;
        I2cPkt[0].data[0] = &RegAddr1;
        I2cPkt[0].data[1] = &RegAddr0;

        I2cPkt[1].slaveAddr = 0x80;
        I2cPkt[1].length = 1;
        I2cPkt[1].data = &value;
        I2cPkt[1].flags = IIC_PKT_FLG_OP_RD;

        I2c_TransferRequest(0, I2cPkt, 2, MsgQInfo);
    }
    @endcode

    @note First packet is a write transaction which sends Register address to
    the device
    @note I2cPkt[0].flags is not set as IIC_PKT_FLG_OP_RD to denote it’s a write
    request
    @note Ssecond packet is read transaction which reads the data from the slave

    @subsection subsec_i2c_burst_read IIC Burst Read
    Consider the example of an IIC slave which supports burst read (automatic
    increment of memory address) and defines burst read protocol:
    @image html i2c_burst_read.jpg

    To read "N" bytes from this slave starting from address 0xAA, client shall
    form following packet format:

    @code{.c}
    function i2c_reg_burst()  {
        I2c_PktType I2cPkt[N];
        Addr= 0xAA;
        uint8_t value[N];

        I2cPkt[0].SlaveAddr = 0x80;
        I2cPkt[0].Length = 1;
        I2cPkt[0].data[0] = &Addr;

        I2cPkt[1].SlaveAddr = 0x80;
        I2cPkt[1].Length = N;
        I2cPkt[1].Data = value;
        I2cPkt[1].Flags = IIC_PKT_FLG_OP_RD;

        I2c_TransferRequest(0, I2cPkt, 2, MsgQInfo);
    }
    @endcode

    @note First packet is a write transaction which sends Register address to
    the device
    @note I2cPkt[0].flags is not set as IIC_PKT_FLG_OP_RD to denote it’s a write
    request
    @note Second packet is read transaction which reads the data from the slave

    After the IIC driver performs "N" byte read, it sets NACK condition followed
    by STOP.

    @subsection subsec_i2c_single_write IIC Single Write
    Consider the example of an IIC slave (address: 0x80) which is 7-bit slave
    and supports an 8-bit addressable memory, which defines the IIC single write
    protocol as following:
    @image html i2c_single_write_8bit.jpg

    To write the value 0xDD to a single register (address: 0xAA), client shall
    form following packet format:

    @code{.c}
    function i2c_write()  {
        I2c_PktType I2cPkt[2];
        Addr= 0xAA;
        uint8_t value = 0xDD;

        I2cPkt[0].slaveAddr = 0x80;
        I2cPkt[0].length = 2;
        I2cPkt[0].data[0] = &Addr;
        I2cPkt[0].data[1] = Value;

        I2c_TransferRequest(0, I2cPkt, 1, MsgQInfo);
    }
    @endcode

    @note Only 1 packet is needed, which sends slave address, followed by
    Register address and data to be written

    @subsection subsec_i2c_burst_write IIC Burst Write
    Consider the example of an IIC slave which is a 7-bit slave, supports an
    8-bit addressable memory, and defines the IIC burst write protocol as the
    following:
    @image html i2c_burst_write_8bit.jpg

    To write the value 0xDD to a single register (address: 0xAA), client needs
    to form following packet format:

    @code{.c}
    function i2c_write()  {
        I2c_PktType I2cPkt[2];
        Addr= 0xAA;
        uint8_t value1 = 1;
        uint8_t value1 = 2;
        uint8_t value1 = 2;

        I2cPkt[0].slaveAddr = 0x80;
        I2cPkt[0].length = 1;
        I2cPkt[0].data[0] = &Addr;
        I2cPkt[0].data[1] = Value1;
        I2cPkt[0].data[2] = Value2;
        I2cPkt[0].data[3] = Value3;

        I2c_Transfer(0, I2cPkt, 3, MsgQInfo);
    }
    @endcode

    @note Only 1 packet is needed, which sends slave address, followed by
    Register address and data to be written

    @section sec_i2c_fsm State Machine
    Below figure depicts software state machine for the IIC driver.
    @image html i2c_state_machine.jpg "IIC driver state machine"

    @section sec_i2c_seq Sequence Diagrams
    @todo image html i2c_seq_diagram_transmit.jpg "IIC Transmit Sequence"

    @section sec_i2c_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_i2c.dep

    @limitations IIC driver handles only master mode.
    @limitations Hardware transmit FIFO is currently not supported by driver.

    @file i2c.h
    @brief IIC driver interface
    This header file contains the interface for IIC driver.
    @version 0.62 Imported from docx
*/

#ifndef IIC_H
#define IIC_H

#include <stdint.h>

/**
    @name IIC driver API IDs
    @{
    @brief API IDs for IIC driver
*/
#define BRCM_SWARCH_IIC_HWID_TYPE             (0x00U) /**< @brief #IIC_HwIDType     */
#define BRCM_SWARCH_IIC_SPEED_TYPE            (0x01U) /**< @brief #IIC_SpeedType    */
#define BRCM_SWARCH_IIC_CONFIG_TYPE           (0x02U) /**< @brief #IIC_ConfigType   */
#define BRCM_SWARCH_IIC_PKT_FLG_MACRO         (0x03U) /**< @brief #IIC_PktFlagType  */
#define BRCM_SWARCH_IIC_PKT_TYPE              (0x04U) /**< @brief #IIC_PktType      */
#define BRCM_SWARCH_IIC_INIT_PROC             (0x05U) /**< @brief #IIC_Init         */
#define BRCM_SWARCH_IIC_TRANSFER_PROC         (0x06U) /**< @brief #IIC_Transfer     */
#define BRCM_SWARCH_IIC_GETSTATUS_PROC        (0x07U) /**< @brief #IIC_GetStatus    */
#define BRCM_SWARCH_IIC_DEINIT_PROC           (0x08U) /**< @brief #IIC_DeInit       */
/** @} */


/**
    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef uint32_t IIC_HwIDType;

/**
    @name IIC bus speed
    @{
    @brief IIC bus speed.

    Actual speed depends on the external pull up resistor and slave clock
    stretching. Actual bus speed will never exceed selected speed.

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef uint32_t IIC_SpeedType;
#define IIC_SPEED_100K          (1UL)   /**< @brief 100 KHz */
#define IIC_SPEED_400K          (2UL)   /**< @brief 400 KHz */
#define IIC_SPEED_MAX           (3UL)   /**< @brief Max speed supported by hardware */
/** @} */

/**
    @brief IIC Configuration

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef struct _IIC_ConfigType {
    IIC_HwIDType  id;              /**< @brief i2c hardware index */
    IIC_SpeedType speed;             /**< @brief Speed of the IIC bus */
    uint32_t      txFifoMode;        /**< @brief Disable/enable hardware transmit FIFO
                                     (In the current version FIFO mode is not
                                     supported):
                                     - 0UL: Disable hardware transmit FIFO
                                     - 1UL: Enable hardware transmit FIFO */
} IIC_ConfigType;

/**
    @name IIC packet flags
    @{
    @brief IIC packet flags

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef uint32_t IIC_PktFlagType;
#define IIC_PKT_FLG_SLAVE_10BIT     (0x1UL) /**< @brief 10bit slave device */
#define IIC_PKT_FLG_OP_RD           (0x2UL) /**< @brief IIC read. If this flag is
                                             not used, it’s assumed as an IIC
                                             write. */
#define IIC_PKT_FLG_RESTART         (0x4UL) /**< @brief Slave needs restart */
/** @} */

/**
    @brief IIC transfer packet

    IIC transfer packet structure.

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef struct _IIC_PktType {
    IIC_PktFlagType flags;     /**< @brief Packet flags (combination of
                                     one/more of the following):
                                            - #IIC_PKT_FLG_SLAVE_10BIT
                                            - #IIC_PKT_FLG_OP_RD
                                            - #IIC_PKT_FLG_RESTART */
    uint32_t slaveAddr;         /**< @brief Device address of the IIC slave.
                                     This slaveAddr is 7bit/10bit address
                                     (without any shift). */
    uint32_t length;            /**< @brief Length of the transfer in bytes */
    uint8_t *data;              /**< @brief Pointer to memory address where data
                                     is to be read from (for IIC write) or to
                                     be written to (for IIC read). */
} IIC_PktType;

/** @brief Initialize IIC bus

    This API queues a request to initialize the given IIC bus controller with
    the given configuration. This request is processed asynchronously and on
    completion client is signaled using clientMask (in aMsgQInfo).

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the IIC bus
    @param[in]      aConfig     Pointer to IIC configuration structure

    @return     void

    @post TBD

    @trace  #BRCM_SWREQ_IIC_INIT

    @limitations This API shall be called only from task context.
*/
extern void IIC_Init(IIC_HwIDType aID,
                    const IIC_ConfigType * const aConfig);

/** @brief IIC transfer request

    This API queues a transfer request to IIC driver. When this request
    processing is complete, client shall be signaled using clientMask (part of
    aMsgQInfo). Refer @ref sec_i2c_pkt for more details.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the IIC bus controller
    @param[in]      aPkts       Pointer to array of IIC transfer packets
    @param[in]      aNumPkts    Number of IIC transfer packets
    @param[in]      aClientMask Mask to notify client (as used by @ref MSGQ_Type)
    @param[out]     aJobID      Handle to query status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Transfer request is successfully queued
                                        to IIC driver
    @retval     Others                  As returned by #IIC_SendMsg
    @retval     #BCM_ERR_INVAL_PARAMS   aMsgQInfo is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aNumPkts is zero
    @retval     #BCM_ERR_INVAL_PARAMS   aPkts is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aID

    @post TBD

    @trace  #BRCM_SWREQ_IIC_TRANSFER

    @limitations This API shall be called only from task context.
*/
extern int32_t IIC_Transfer(IIC_HwIDType aID,
                           IIC_PktType * const aPkts,
                           uint32_t aNumPkts,
                           uint32_t aClientMask,
                           uint32_t *aJobID);

/** @brief Get response of a queued IIC request

    This API shall be called by the client after it is signaled about the
    completion of the IIC request.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the IIC bus controller ID
    @param[in]      aJobID     Handle as provided by IIC_Transfer

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             IIC driver response is successfully
                                        retrieved
    @retval     Others                  As returned by #IIC_RecvMsg
    @retval     #BCM_ERR_INVAL_PARAMS   aMsgQInfo is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aID

    @post TBD

    @trace  #BRCM_SWREQ_IIC_TRANSFER

    @limitations any limitations for this api
*/
extern int32_t IIC_GetStatus(IIC_HwIDType aID,
                               uint32_t aJobID);

/** @brief De-initialize IIC bus controller

    This API queues a request to de-initialize the given IIC bus controller. On
    successful de-initialization, bus controller shall be in uninitialized
    state. Once the request is successfully processed by the IIC driver, the
    client is signaled using clientMask (part of aMsgQInfo).

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the IIC bus controller
    @param[out]     aMsgQInfo   Pointer to retrieve Message Queue info

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             De-Init request is successfully queued
                                        to IIC driver queue
    @retval     Others                  As returned by #IIC_SendMsg
    @retval     #BCM_ERR_INVAL_PARAMS   aMsgQInfo is NULL or
    @retval     #BCM_ERR_INVAL_PARAMS   aID is invalid

    @post TBD

    @trace  #BRCM_SWREQ_IIC_INIT

    @limitations This API shall be called only from task context.
*/
extern int32_t IIC_DeInit(IIC_HwIDType aID);

#endif /* IIC_H */
/** @} */
