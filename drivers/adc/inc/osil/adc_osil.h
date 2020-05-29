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

/** @defgroup grp_adcdrv_il Integration Guide
    @ingroup grp_adcdrv

    @addtogroup grp_adcdrv_il
    @{
    @section sec_adc_integration Integration Guide
    ADC driver is implemented as a SysCmdReq (system command request
    [@ref ref_sads_spec]) and SysCmdHandler (system command handler [@ref ref_sads_spec])
    model. This section gives the necessary information on how to install
    command handler for ADC driver for privileged access as well as setting up
    the ADC driver’s interrupt handler

    @section sec_adc_il_seq Sequence Diagram

    @subsection subsec1_adc_il_seq SVC Layer Interaction
    @image html adc_svc_layer_seq_diagram.jpg
    All other ADC driver APIs follows the similar sequence as #ADC_Init

    @subsection subsec2_adc_il_seq ADC Interrupt and channel group notification
    @image html adc_interrupt_seq_diagram.jpg

    @file adc_osil.h
    @brief ADC Driver Integration Interfaces
    This header file contains the integration interface for ADC Driver
    @version 1.0 Changelist1
    @version 0.1 Changelist0
*/

#ifndef ADC_OSIL_H
#define ADC_OSIL_H

#include <adc.h>
#include <system.h>

/**
    @name ADC Driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for ADC Driver
*/
#define BRCM_SWARCH_ADC_CONFIG_GLOBAL           (0x40U) /**< @brief #ADC_Config             */
#define BRCM_SWARCH_ADC_CONFIGSZ_GLOBAL         (0x41U) /**< @brief #ADC_ConfigSz           */
#define BRCM_SWARCH_ADC_CMD_TYPE                (0x42U) /**< @brief #ADC_CmdType            */
#define BRCM_SWARCH_ADC_IOTYPE_TYPE             (0x43U) /**< @brief #ADC_IOType             */
#define BRCM_SWARCH_ADC_SYSCMDREQ_PROC          (0x44U) /**< @brief #ADC_SysCmdReq          */
#define BRCM_SWARCH_ADC_SYSCMDHANDLER_PROC      (0x45U) /**< @brief #ADC_SysCmdHandler      */
#define BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC (0x46U) /**< @brief #ADC_IRQHandlerInternal */
/** @} */

/**
    @brief Pointer to ADC driver configuration

    This pointer shall point to the ADC driver configuration

    @trace  #BRCM_SWREQ_ADC_INIT
*/
extern const ADC_ConfigType *ADC_Config;

/**
    @brief ADC Configuration array size

    Size of the ADC_Config (number of elements in the ADC_Config array)

    @trace  #BRCM_SWREQ_ADC_INIT
*/
extern const uint32_t ADC_ConfigSz;

/**
    @brief ADC driver system command request type

    @trace #BRCM_SWREQ_ADC_KERNEL_INTERFACE
*/
typedef uint32_t ADC_CmdType;
#define ADC_CMD_INIT                    (0UL)   /**< @brief ADC Command init                    */
#define ADC_CMD_DEINIT                  (1UL)   /**< @brief ADC Command deinit                  */
#define ADC_CMD_START_GRP_CONV          (2UL)   /**< @brief ADC Command start group conversion  */
#define ADC_CMD_STOP_GRP_CONV           (3UL)   /**< @brief ADC Command stop group conversion   */
#define ADC_CMD_READ_GRP_SAMPLE         (4UL)   /**< @brief ADC Command read group sample       */
#define ADC_CMD_ENABLE_GRP_NOTIF        (5UL)   /**< @brief ADC Command enable group notfn      */
#define ADC_CMD_DISABLE_GRP_NOTIF       (6UL)   /**< @brief ADC Command disable group notfn     */
#define ADC_CMD_GET_GRP_CONV_STATE      (7UL)   /**< @brief ADC Command group convert state     */

/**
    @brief IO structure for ADC driver

    @trace #BRCM_SWREQ_ADC_KERNEL_INTERFACE
 */
typedef struct _ADC_IOType {
    ADC_ChanGrpIDType           grpID;      /**< @brief Channel group ID                        */
    ADC_ChanGrpConvStateType    convState;  /**< @brief Conversion state                        */
    int32_t                     retVal;     /**< @brief Return value set by #ADC_SysCmdHandler  */
    uint32_t                    *sampleBuf; /**< @brief Sample buffer                           */
} ADC_IOType;

/** @brief Interface to raise a request to ADC driver

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd        Command requested by user
    @param[inout]   aADC_IOType      In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     Others                  As returned by #ADC_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ADC_KERNEL_INTERFACE

    @limitations None
*/
extern int32_t ADC_SysCmdReq(ADC_CmdType aCmd,
                             ADC_IOType *const aADCIO);

/** @brief Command handler interface

    This command handler interface shall handle all the commands requested from
    the system command handler. This interface shall be implemented by the
    integrator and shall be installed to SVC Layer

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aMagicID    ADC SVC Magic ID
    @param[in]      aCmd        Command requested from SysCmdHandler
    @param[inout]   aSysIO      uint8_t * to be interpreted as ADC_IOType

    @return     void

    @post None

    @note To be implemented.

    @trace  #BRCM_SWREQ_ADC_KERNEL_HANDLER

    @limitations None
*/
extern void ADC_SysCmdHandler(uint32_t aMagicID,
                              uint32_t aCmd,
                              uint8_t *aSysIO);

/** @brief ADC driver interrupt handlers

    This interface handles all the ADC interrupts. This interface shall be
    called from the interrupt service routine for ADC driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aHwID       ADC hardware ID

    @return     void

    @post None

    @note To be used.

    @trace  #BRCM_SWREQ_ADC_KERNEL_HANDLER

    @limitations None

    @snippet adc_osil.c Usage of ADC_IRQHandlerInternal
*/
extern void ADC_IRQHandlerInternal(ADC_HwIDType aHwID);

#endif  /* ADC_OSIL_H */
/** @} */
