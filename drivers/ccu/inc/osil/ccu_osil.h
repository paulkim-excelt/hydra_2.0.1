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

/** @defgroup grp_ccudrv_il Integration Guide
    @ingroup grp_ccudrv

    @addtogroup grp_ccudrv_il
    @{
    @section sec_ccu_integration Integration Guide

    @section sec_ccu_il_seq Sequence Diagram
    @subsection subsec_ccu_il_seq_svc Driver SVC Layer Sequence Diagram
    The following sequence diagram depicts the sequence #CCU_Init as an example.
    All other CCU APIs shall follow the same sequence.
    @image html ccu_seq_il_diagram_svc.jpg "Driver SVC layer sequence"

    @subsection subsec_ccu_il_seq_cb Driver Callback Sequence Diagram
    The following sequence diagram depicts the sequence for callbacks from the
    CCU driver.
    @note The callbacks from CCU driver are invoked from interrupt context.

    @image html ccu_seq_il_diagram_cb.jpg "Driver callback sequence"

    @file ccu_osil.h
    @brief CCU Integration Interface
    This header file contains the integration interfaces for CCU driver.
    @version 0.22 Imported from docx
*/

#ifndef CCU_OSIL_H
#define CCU_OSIL_H

#include <inttypes.h>
#include <system.h>
#include <ccu.h>

/**
    @name CCU Integration Interface IDs
    @{
    @brief Integration Interface IDs for CCU driver
*/
#define BRCM_SWARCH_CCU_CMD_TYPE             (0x40U) /**< @brief #CCU_CmdType */
#define BRCM_SWARCH_CCU_IO_TYPE              (0x41U) /**< @brief #CCU_IOType */
#define BRCM_SWARCH_CCU_SYSCMDHANDLER_PROC   (0x42U) /**< @brief #CCU_SysCmdHandler */
#define BRCM_SWARCH_CCU_SYSCMDREQ_PROC       (0x43U) /**< @brief #CCU_SysCmdReq */
#define BRCM_SWARCH_CCU_IRQHANDLER0_PROC     (0x44U) /**< @brief #CCU_IRQHandler0 */
#define BRCM_SWARCH_CCU_IRQHANDLER1_PROC     (0x45U) /**< @brief #CCU_IRQHandler1 */
#define BRCM_SWARCH_CCU_IRQHANDLER2_PROC     (0x46U) /**< @brief #CCU_IRQHandler2 */
#define BRCM_SWARCH_CCU_IRQHANDLER3_PROC     (0x47U) /**< @brief #CCU_IRQHandler3 */
#define BRCM_SWARCH_CCU_UDCIRQHANDLER_PROC   (0x48U) /**< @brief #CCU_UDCIRQHandler */
#define BRCM_SWARCH_CCU_ETRIRQHANDLER_PROC   (0x49U) /**< @brief #CCU_ETRIRQHandler */
#define BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC  (0x4AU) /**< @brief #CCU_DrvIRQHandler */
/** @} */

/**
    @name CCU_CmdType
    @{
    @brief CCU system command request

    @trace #BRCM_SWREQ_CCU_KERNEL_INTERFACE
*/
typedef uint32_t CCU_CmdType;                            /**< @brief CCU system cmd request type  */
#define CCU_CMD_INIT                            (0x01UL) /**< @brief INIT                         */
#define CCU_CMD_DEINIT                          (0x02UL) /**< @brief DEINIT                       */
#define CCU_CMD_GET_CHAN_MODE                   (0x03UL) /**< @brief GET_CHAN_MODE                */
#define CCU_CMD_START_COMP_CHANNEL              (0x04UL) /**< @brief START_COMP_CHANNEL           */
#define CCU_CMD_STOP_COMP_CHANNEL               (0x05UL) /**< @brief STOP_COMP_CHANNEL            */
#define CCU_CMD_SET_COMP_PIN_PROPERTY           (0x06UL) /**< @brief SET_COMP_PIN_PROPERTY        */
#define CCU_CMD_SET_COMP_ABS_THRESHOLD          (0x07UL) /**< @brief SET_COMP_ABS_THRESHOLD       */
#define CCU_CMD_ENABLE_COMP_CALLBACK            (0x08UL) /**< @brief ENABLE_COMP_CALLBACK         */
#define CCU_CMD_DISABLE_COMP_CALLBACK           (0x09UL) /**< @brief DISABLE_COMP_CALLBACK        */
#define CCU_CMD_SET_CAP_SIGNAL_PROP             (0x0AUL) /**< @brief SET_CAP_SIGNAL_PROP          */
#define CCU_CMD_ENABLE_CAP_CALLBACK             (0x0BUL) /**< @brief ENABLE_CAP_CALLBACK          */
#define CCU_CMD_DISABLE_CAP_CALLBACK            (0x0CUL) /**< @brief DISABLE_CAP_CALLBACK         */
#define CCU_CMD_GET_CAP_INPUT_STATE             (0x0DUL) /**< @brief GET_CAP_INPUT_STATE          */
#define CCU_CMD_START_CAP_TIMESTAMP             (0x0EUL) /**< @brief START_CAP_TIMESTAMP          */
#define CCU_CMD_STOP_CAP_TIMESTAMP              (0x0FUL) /**< @brief STOP_CAP_TIMESTAMP           */
#define CCU_CMD_GET_CAP_TIMESTAMP_INDEX         (0x10UL) /**< @brief GET_CAP_TIMESTAMP_INDEX      */
#define CCU_CMD_RESET_CAP_EDGE_COUNT            (0x11UL) /**< @brief RESET_CAP_EDGE_COUNT         */
#define CCU_CMD_ENABLE_CAP_EDGE_COUNT           (0x12UL) /**< @brief ENABLE_CAP_EDGE_COUNT        */
#define CCU_CMD_DISABLE_CAP_EDGE_COUNT          (0x13UL) /**< @brief DISABLE_CAP_EDGE_COUNT       */
#define CCU_CMD_ENABLE_CAP_EDGE_DETECTION       (0x14UL) /**< @brief ENABLE_CAP_EDGE_DETECTION    */
#define CCU_CMD_DISABLE_CAP_EDGE_DETECTION      (0x15UL) /**< @brief DISABLE_CAP_EDGE_DETECTION   */
#define CCU_CMD_GET_CAP_EDGE_NUMBERS            (0x16UL) /**< @brief GET_CAP_EDGE_NUMBERS         */
#define CCU_CMD_START_CAP_SIGNAL_MEASUREMENT    (0x17UL) /**< @brief START_CAP_SIGNAL_MEASUREMENT */
#define CCU_CMD_STOP_CAP_SIGNAL_MEASUREMENT     (0x18UL) /**< @brief STOP_CAP_SIGNAL_MEASUREMENT  */
#define CCU_CMD_GET_CAP_TIME_ELAPSED            (0x19UL) /**< @brief GET_CAP_TIME_ELAPSED         */
#define CCU_CMD_GET_CAP_DUTY_CYCLE_VALUES       (0x1AUL) /**< @brief GET_CAP_DUTY_CYCLE_VALUES    */
/** @} */

/**
    @brief CCU IO structure

    CCU IO structure to be used for #CCU_Drv_CmdHandler.

    @trace #BRCM_SWREQ_CCU_KERNEL_INTERFACE
*/
typedef struct _CCU_IOType {
    int32_t retVal;                 /**< @brief Return value from the command
                                         internal function*/
    const CCU_ConfigType *config;   /**< @brief Pointer to CCU configuration */
    CCU_ChanType chanNum;           /**< @brief Channel number */
    CCU_ChanModeType *chanMode;     /**< @brief Pointer to retrieve channel
                                         mode */
    CCU_PinActionType pinAction;    /**< @brief Output pin action on compare
                                         match */
    CCU_OutPinSelType outPin;       /**< @brief Output pin selected */
    CCU_InPinSelType inPin;         /**< @brief Input pin selected */
    CCU_ValueType absValue;         /**< @brief Absolute threshold value */
    CCU_EdgeType edge;              /**< @brief Edge selected for compare output
                                         or capture input */
    CCU_InputStateType *inputState; /**< @brief Pointer to retrieve capture
                                         channel input state */
    CCU_ValueType *tsBuf;           /**< @brief Pointer to timestamp buffer */
    uint32_t tsBufSize;             /**< @brief Size of timestamp buffer */
    uint32_t notifyInterval;        /**< @brief Notify interval for capture
                                         timestamp mode */
    CCU_TSIndexType *tsIndex;       /**< @brief Timestamp buffer index */
    CCU_EdgeNumberType *edgeNum;    /**< @brief Pointer to retrieve edge
                                         number */
    CCU_ValueType *timeElapsed;     /**< @brief Pointer to retrieve time
                                         elapsed */
    CCU_DutyCycleType *dutyCycle;   /**< @brief Pointer to retrieve duty
                                         cycle */
} CCU_IOType;



/** @brief Handle CCU system command request processing

    Interface to handle CCU system command request processing.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    CCU system request magic ID
    @param[in]      aCmd        CCU system request command
    @param[inout]   aSysIO      Pointer to system request IO for #CCU_IOType


    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER

    @limitations None
*/
extern void CCU_SysCmdHandler(uint32_t aMagicID,
                              uint32_t aCmd,
                              uint8_t *aSysIO);

/** @brief Raise CCU driver system command request

    Interface to raise CCU driver system command request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aCmd        CCU system request command
    @param[inout]   aIO         Pointer to #CCU_IOType

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request raised successfully
    @retval     Others                  As returned from #CCU_Drv_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_CCU_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t CCU_SysCmdReq(CCU_CmdType aCmd,
                             CCU_IOType * const aIO);

/**
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
extern void CCU_IRQHandler0(void);

/**
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
extern void CCU_IRQHandler1(void);

/**
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
extern void CCU_IRQHandler2(void);

/**
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
extern void CCU_IRQHandler3(void);

/**
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
extern void CCU_UDCIRQHandler(void);

/**
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
extern void CCU_ETRIRQHandler(void);

/**
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
extern void CCU_DrvIRQHandler(void);

#endif /* CCU_OSIL_H */

/** @} */
