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

/** @defgroup grp_i2cdrv_il Integration Guide
    @ingroup grp_i2cdrv

    @addtogroup grp_i2cdrv_il
    @{
    @section sec_i2c_integration Integration Guide
    IIC driver uses Message Queue to queue the different IIC requests. All
    client requests are queued on to this request queue. Driver processes these
    requests asynchronously, and provides processed data back to the
    client/application in the same IIC request packet.

    In order to process the queued IIC requests asynchronously, IIC driver
    requires a task (a server task from Message Queue context). This task shall
    be called SerialIO task further in this document. SerialIO task is also
    responsible for processing IIC driver state machine.

    Integration Guide describes interfaces required for integrating IIC driver
    to a system. Some of the interfaces are implemented as part of IIC driver
    itself. Some of the interfaces shall be implemented by the integrator.

    @subsection subsec_i2c_serialio SerialIO Task
    SerialIO task has two roles:
    -# As the Message Queue server task: To process the IIC request queued onto
       the IIC driver queues, when Message Queue signals it
    -# To do deferred processing of the IIC slave device responses, when ISR
       handler for different IIC controller signals the SerialIO task.

    It is the integrator’s responsibility to implement this SerialIO task,
    for which a reference implementation is given below:

    @code{.c}
    #define IIC_EVENTS      (IIC_IRQ_EVENT0 | IIC_IRQ_EVENT1 \
                             IIC_IRQ_EVENT2 | IIC_IRQ_EVENT3 \
                             IIC_MSG_EVENT)

    void SerialIOTaskFunc(void)
    {
        BCM_WaitEvent(IIC_EVENTS)
        BCM_GetEvent(TaskSerialIO, &mask)
        if ((mask & IIC_MSG_EVENT) != 0UL) {
            BCM_ClearEvent(IIC_MSG_EVENT)
            IIC_MsgQEventHandler()
        }
        if ((mask & IIC_IRQ_EVENT0) != 0UL) {
            BCM_ClearEvent(IIC_IRQ_EVENT0);
            IIC_IRQEventHandler(0UL);
        }
        if ((mask & IIC_IRQ_EVENT1) != 0UL) {
            BCM_ClearEvent(IIC_IRQ_EVENT1);
            IIC_IRQEventHandler(1UL);
        }
        if ((mask & IIC_IRQ_EVENT2) != 0UL) {
            BCM_ClearEvent(IIC_IRQ_EVENT2);
            IIC_IRQEventHandler(2UL);
        }
        if ((mask & IIC_IRQ_EVENT3) != 0UL) {
            BCM_ClearEvent(IIC_IRQ_EVENT3);
            IIC_IRQEventHandler(3UL);
        }
    }
    @endcode
    @note IIC_IRQ_EVENT0, IIC_IRQ_EVENT1 etc are Event masksfor different IIC
    bus controllers.
    @note IIC_MSG_EVENT shall be the event mask with which Message Queue shall
    signal SerialIO task.

    @section sec_i2c_il_seq Sequence Diagram
    @subsection subsec_i2c_il_seq_init IIC Init Request
    @image html i2c_seq_il_diagram_init.jpg "Init request sequence"
    A similar sequence is followed by #IIC_DeInitRequest, and
    #IIC_TransferRequest APIs.

    @subsection subsec_i2c_il_seq_transmit IIC Transmit
    @image html i2c_seq_il_diagram_transmit.jpg "Transmit sequence (integration)"

    @file i2c_osil.h
    @brief IIC driver Integration Interfaces
    This header file contains the integration interfaces for IIC driver
    @version 0.62 Imported from docx
*/

#ifndef IIC_OSIL_H
#define IIC_OSIL_H

#include <system.h>
#include <i2c.h>

/**
    @name IIC driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for IIC driver
*/
#define BRCM_SWARCH_IIC_CMD_TYPE            (0x20U) /**< @brief #IIC_CmdType            */
#define BRCM_SWARCH_IIC_IO_TYPE             (0x21U) /**< @brief #IIC_IOType             */
#define BRCM_SWARCH_IIC_CLIENT_SIGNAL_TYPE  (0x22U) /**< @brief #IIC_ClientSignalType   */
#define BRCM_SWARCH_IIC_SYSCMDREQ_PROC      (0x23U) /**< @brief #IIC_SysCmdReq          */
#define BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC  (0x24U) /**< @brief #IIC_SysCmdHandler      */
#define BRCM_SWARCH_IIC_IRQHANDLER_PROC     (0x25U) /**< @brief #IIC_IRQHandler         */
#define BRCM_SWARCH_IIC_IRQHANDLER0_PROC    (0x26U) /**< @brief #IIC_IRQHandler0        */
#define BRCM_SWARCH_IIC_IRQHANDLER1_PROC    (0x27U) /**< @brief #IIC_IRQHandler1        */
#define BRCM_SWARCH_IIC_IRQHANDLER2_PROC    (0x28U) /**< @brief #IIC_IRQHandler2        */
#define BRCM_SWARCH_IIC_IRQHANDLER3_PROC    (0x29U) /**< @brief #IIC_IRQHandler3        */
/** @} */

/**
    @name IIC_CmdType
    @{
    @brief IIC SVC command types
    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE

*/
typedef uint32_t IIC_CmdType;
#define IIC_CMD_INIT                (1UL) /**< @brief Bus init command              */
#define IIC_CMD_TRANSFER_SUBMIT     (2UL) /**< @brief Bus submit transfer command   */
#define IIC_CMD_TRANSFER_STATUS     (3UL) /**< @brief Bus transfer status command   */
#define IIC_CMD_DEINIT              (4UL) /**< @brief Bus deinit command            */
/** @}*/

/**
    @brief IIC IO structure

    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE

 */
typedef struct _IIC_IOType {
    int32_t                 retVal;     /**< @brief Return value                        */
    IIC_HwIDType            hwID;       /**< @brief IIC bus index                       */
    uint32_t                jobID;      /**< @brief IIC bus job index                   */
    uint32_t                numPkts;    /**< @brief Number of IIC packets               */
    IIC_PktType*            pkts;       /**< @brief Array of IIC packets                */
    int32_t                 clientID;   /**< @brief Client ID to signal completion      */
    uint32_t                clientMask; /**< @brief Client mask to signal completion    */
    const IIC_ConfigType    *config;    /**< @brief Pointer to configurations           */
} IIC_IOType;


/**
    @trace #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
typedef struct _IIC_ClientSignalType {
    uint32_t    done;
    int32_t     clientID;
    uint32_t    clientMask;
} IIC_ClientSignalType;

/** @brief Raise a command request

    This interface raises a command request to the IIC driver.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         Pointer to in-Out structure containing request
                                parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_UNKNOWN        Integration Error
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     Others                  As returned by #IIC_CmdHandler

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_IIC_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t IIC_SysCmdReq(IIC_CmdType aCmd, IIC_IOType *const aIO);

/** @brief Handle IIC driver commands from System command request

    This interface handles all the IIC driver commands requested from System
    command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    IIC driver SVC magic ID
    @param[in]      aCmd        Command requested from system command handler
    @param[inout]   aSysIO      uint8_t * to be interpreted as #IIC_IOType

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER

    @limitations None
*/
extern void IIC_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO);

/** @brief IIC IRQ handler

    Interface to handle interrupt triggered for the give IIC bus controller.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         Index of IIC bus controller

    @retval         0UL         Transfer in progress, Do not wake up client
    @retval         1UL         Transfer is complete, Notify client

    @post TBD

    @note Integrator shall call this interface from the ISR for the
    corresponding IIC bus controller. The ISR shall then signal the SerialIOTask
    to process IIC slave responses.

    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER

    @limitations None

    @snippet i2c_osil.c Usage of IIC_IRQHandler
*/
extern IIC_ClientSignalType IIC_IRQHandler(IIC_HwIDType aID);

/** @brief IIC IRQ handler for instance 0

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      void

    @retval         void

    @post TBD

    @note Integrator shall implement this interface and install in interrupt vector

    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER

    @limitations None
*/
extern void IIC_IRQHandler0(void);

/** @brief IIC IRQ handler for instance 1

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      void

    @retval         void

    @post TBD

    @note Integrator shall implement this interface and install in interrupt vector

    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER

    @limitations None
*/
extern void IIC_IRQHandler1(void);

/** @brief IIC IRQ handler for instance 2

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      void

    @retval         void

    @post TBD

    @note Integrator shall implement this interface and install in interrupt vector

    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER

    @limitations None
*/
extern void IIC_IRQHandler2(void);

/** @brief IIC IRQ handler for instance 3

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      void

    @retval         void

    @post TBD

    @note Integrator shall implement this interface and install in interrupt vector

    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER

    @limitations None
*/
extern void IIC_IRQHandler3(void);

#endif /* IIC_OSIL_H */
/** @} */
