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
    @defgroup grp_pwmdrv_ifc Interface
    @ingroup grp_pwmdrv

    @addtogroup grp_pwmdrv_ifc
    @{
    @section sec_pwm_overview Overview
    @image html pwm_overview.jpg "Drvier Overview"
    PWM driver provides functions for initialization, control, and generation of
    pulses with variable pulse width on specific output pins. It provides
    interfaces for selecting the period and duty cycle of the output signal.

    Above figure describes the driver architecture. The PWM Driver Interface
    utilizes the underlying PWM Block to generate the output pulses. Refer to
    @ref sec_ref_trm for description of the PWM peripheral.

    @section sec_pwm_fsm State Machine
    This section describes state machines for driver and PWM channel.

    @subsection subsec_pwm_fsm_chann PWM Channel State Machine
    @image html pwm_state_machine_channel.jpg "Channel state machine"

    @section sec_pwm_seq Sequence Diagrams
    This section describes use-case sequence diagrams for PWM driver.

    @subsection subsec_pwm_seq_init Driver Init - Deinit Sequence Diagram
    The following sequence diagram depicts initialization and de-initialization
    sequence for PWM driver:
    @image html pwm_seq_diagram_init_deint.jpg "Driver init - deinit sequence"

    @subsection subsec_pwm_seq_usecase Set Period and Duty Cycle Usecase
    The following sequence diagram depicts sequence for changing PWM period and
    duty cycle via PWM driver.
    @image html pwm_seq_diagram_use_case.jpg "Set PWM period \& duty cycle sequence"

    @section sec_pwm_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @limitations Since PWM Period and Duty cycle values are based on number of
    input clock cycles, not all combinations of period and duty cycle may be
    exactly achievable.

    @file pwm.h
    @brief PWM Application Interface
    This header file contains the application interfaces for PWM driver.
    @version 0.70 Imported from docx
*/

#ifndef PWM_H
#define PWM_H

#include <stddef.h>
#include <inttypes.h>

/**
    @name PWM driver API IDs
    @{
    @brief API IDs for PWM driver
*/
#define BRCM_SWARCH_PWM_PRESCALE_MAX_MACRO          (0x00U) /**< @brief #PWM_PRESCALE_MAX */
#define BRCM_SWARCH_PWM_HW_ID_TYPE                  (0x01U) /**< @brief #PWM_HwIDType */
#define BRCM_SWARCH_PWM_PWM_CHAN_ID_TYPE            (0x02U) /**< @brief #PWM_ChanIDType */
#define BRCM_SWARCH_PWM_VALUE_TYPE                  (0x03U) /**< @brief #PWM_ValueType */
#define BRCM_SWARCH_PWM_CHANMODE_TYPE               (0x04U) /**< @brief #PWM_ChanModeType */
#define BRCM_SWARCH_PWM_CHANPOLARITY_TYPE           (0x05U) /**< @brief #PWM_ChanPolarityType */
#define BRCM_SWARCH_PWM_CHANCLASS_TYPE              (0x06U) /**< @brief #PWM_ChanClassType */
#define BRCM_SWARCH_PWM_CHAN_CFG_TYPE               (0x07U) /**< @brief #PWM_ChanCfgType */
#define BRCM_SWARCH_PWM_CONFIG_TYPE                 (0x08U) /**< @brief #PWM_ConfigType */
#define BRCM_SWARCH_PWM_INIT_PROC                   (0x09U) /**< @brief #PWM_Init */
#define BRCM_SWARCH_PWM_DEINIT_PROC                 (0x0AU) /**< @brief #PWM_DeInit */
#define BRCM_SWARCH_PWM_START_CHANNEL_PROC          (0x0BU) /**< @brief #PWM_StartChannel */
#define BRCM_SWARCH_PWM_STOP_CHANNEL_PROC           (0x0CU) /**< @brief #PWM_StopChannel */
#define BRCM_SWARCH_PWM_SET_DUTY_CYCLE_PROC         (0x0DU) /**< @brief #PWM_SetDutyCycle */
#define BRCM_SWARCH_PWM_SET_PERIOD_DUTY_CYCLE_PROC  (0x0EU) /**< @brief #PWM_SetPeriodDutyCycle */
/** @} */

/**
    @brief Macro brief description

    Macro Detailed description.

    @trace #BRCM_SWREQ_PWM_DATA_TYPES
 */
#define PWM_PRESCALE_MAX       (63U)

/**
    @name PWM_HwIDType
    @{
    @brief Type for PWM Hardware/Controller Identifier

    @trace #BRCM_SWREQ_PWM_DATA_TYPES

*/
typedef uint32_t PWM_HwIDType;  /**< @brief Type for PWM Hardware ID */
#define PWM_HW_ID_0       (0UL) /**< @brief PWM Controller 0 ID */
/** @} */

/**
    @name PWM_ChanIDType
    @{
    @brief PWM channel type

    Refer @ref sec_ref_trm for number of channels available on the chipset

    @trace #BRCM_SWREQ_PWM_DATA_TYPES

*/
typedef uint32_t PWM_ChanIDType;    /**< @brief PWM channel type */
/** @} */

/**
    @name PWM_ValueType
    @{
    @brief PWM value type

    This type is used to describe PWM values for Duty Cycle and Period.

    @trace #BRCM_SWREQ_PWM_DATA_TYPES
*/
typedef uint16_t PWM_ValueType; /**< @brief PWM value type */
/** @} */

/**
    @name PWM_ChanModeType
    @{
    @brief PWM Channel Mode Type

    This type is used to describe PWM Channel mode.

    @trace #BRCM_SWREQ_PWM_DATA_TYPES
*/
typedef uint32_t PWM_ChanModeType;          /**< @brief PWM Channel Mode type */
#define PWM_CHANMODE_FULL_DRIVE    (0UL)    /**< @brief PWM Channel mode for Full
                                             Drive */
#define PWM_CHANMODE_OPEN_DRAIN    (1UL)    /**< @brief PWM Channel mode for Open
                                             Drain */
/** @} */

/**
    @name PWM_ChanPolarityType
    @{
    @brief PWM Channel ploarity type

    This type is used to describe PWM polrity.

    @trace #BRCM_SWREQ_PWM_DATA_TYPES
*/
typedef uint32_t PWM_ChanPolarityType;          /**< @brief PWM Channel ploarity
                                                 type */
#define PWM_CHANPOLARITY_ACTIVE_LOW    (0UL)    /**< @brief Output Polarity for PWM
                                                 channel shall be active low */
#define PWM_CHANPOLARITY_ACTIVE_HIGH   (1UL)    /**< @brief Output Polarity for PWM
                                                 channel shall be active high */
/** @} */

/**
    @name PWM_ChanClassType
    @{
    @brief Type to describe PWM channel class

    @trace #BRCM_SWREQ_PWM_DATA_TYPES

*/
typedef uint32_t PWM_ChanClassType;         /**< @brief PWM channel class type */
#define PWM_CHANCLASS_VARIABLE    (0UL)     /**< @brief PWM channel has variable period.
                                             The duty cycle and period can be
                                             changed. */
#define PWM_CHANCLASS_FIXED       (1UL)     /**< @brief PWM channel has fixed period.
                                             Only the duty cycle can be changed. */
/** @} */

/**
    @brief PWM channel configuration

    Type to describe PWM channel configuration parameters.

    @trace #BRCM_SWREQ_PWM_DATA_TYPES

*/
typedef struct _PWM_ChanCfgType {
    PWM_ChanIDType chanID;          /**< @brief Channel ID */
    PWM_ChanClassType class;        /**< @brief Class of the output channel */
    PWM_ChanModeType mode;          /**< @brief Mode of the output channel */
    PWM_ChanPolarityType polarity;  /**< @brief Polarity of the output channel */
    PWM_ValueType prescaler;        /**< @brief Prescaler value to be used for
                                         the channel. Refer to @ref sec_ref_trm
                                         for values */
    PWM_ValueType period;           /**< @brief Default period used for the
                                         output channel */
    PWM_ValueType dutyCycle;        /**< @brief Default duty cycle used for the
                                         output channel */
} PWM_ChanCfgType;

/**
    @brief Output channel configuration table

    Type to describe output channel configuration table.

    @trace #BRCM_SWREQ_PWM_DATA_TYPES

*/
typedef struct _PWM_ConfigType {
    PWM_ChanCfgType * const chanCfgTbl; /**< @brief Pointer to output channel
                                             configuration table */
    uint32_t chanCfgTblSz;              /**< @brief Size of the output channel
                                             configuration table */
} PWM_ConfigType;

/** @brief Initialize PWM

    API to initialize PWM hardware

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         PWM Hardware ID
    @param[in]      aConfig     Pointer to PWM driver configuration

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_PWM_INIT

    @limitations None
*/
extern void PWM_Init(PWM_HwIDType aID, const PWM_ConfigType *const aConfig);

/** @brief De-initialize PWM

    API to de-initialize PWM hardware. This API shall stop all the configured
    channels and then de-initialize the PWM hardware.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         PWM Hardware ID

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_PWM_INIT

    @limitations None
*/
extern void PWM_DeInit(PWM_HwIDType aID);

/** @brief Start PWM channel

    API to start PWM channel.

    @behavior Sync, Re-entrant (for different @c aChanNum)

    @pre TBD

    @param[in]      aID         PWM Hardware ID
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Channel started successfully
    @retval     #BCM_ERR_INVAL_STATE    Channel not in IDLE state
    @retval     #BCM_ERR_INVAL_PARAMS   @c aChanNum is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   @c aID is invalid
    @retval     #BCM_ERR_UNINIT         PWM Driver not initialized for specified
                                        hardware ID

    @post TBD

    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

    @limitations None
*/
extern int32_t PWM_StartChannel(PWM_HwIDType aID, PWM_ChanIDType aChanNum);

/** @brief Stop channel

    API to stop PWM channel.

    @behavior Sync, Re-entrant (for different @c aChanNum)

    @pre TBD

    @param[in]      aID         PWM Hardware ID
    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Channel stopped successfully
    @retval     #BCM_ERR_INVAL_STATE    Channel not in Running state
    @retval     #BCM_ERR_INVAL_PARAMS   @c aChanNum is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   @c aID is invalid
    @retval     #BCM_ERR_UNINIT         PWM Driver not initialized for specified
                                        hardware ID

    @post TBD

    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

    @limitations None
*/
extern int32_t PWM_StopChannel(PWM_HwIDType aID, PWM_ChanIDType aChanNum);

/** @brief Set duty cycle

    API to set Duty Cycle value of a specific output channel in number of input
    clock cycles. The specified channel, if in running state, must be moved to
    Idle State by using #PWM_StopChannel before calling this API.

    @behavior Sync, Re-entrant (for different @c aChanNum)

    @pre TBD

    @param[in]      aID         PWM Hardware ID
    @param[in]      aChanNum    Channel number
    @param[in]      aDutyCycle  Value for Duty Cycle to set for the specific
                                channel (in number of PWM input clock cycles)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Duty cycle set successfully
    @retval     #BCM_ERR_INVAL_STATE    Channel not in IDLE state
    @retval     #BCM_ERR_INVAL_PARAMS   Channel number is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   @c aID is invalid
    @retval     #BCM_ERR_UNINIT         PWM Driver not initialized for specified
                                        Hardware ID

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

    @limitations None
*/
extern int32_t PWM_SetDutyCycle(PWM_HwIDType aID,
                                PWM_ChanIDType aChanNum,
                                PWM_ValueType aDutyCycle);

/** @brief Set period duty cycle

    API to set Period and Duty Cycle values of a specific channel in number of
    input clock cycles. The specified channel, if in running state, must be
    moved to Idle State by using #PWM_StopChannel before calling this API.

    @behavior Sync, Re-entrant (for different @c aChanNum)

    @pre TBD

    @param[in]      aID         PWM Hardware ID
    @param[in]      aChanNum    Channel number
    @param[in]      aPeriod     Value for Period to set for the specific channel
                                (in number of PWM input clock cycles)
    @param[in]      aDutyCycle  Value for Duty Cycle to set for the specific
                                channel (in number of PWM input clock cycles)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_STATE    Channel not in IDLE state
    @retval     #BCM_ERR_INVAL_STATE    Channel class set to #PWM_FIXED
    @retval     #BCM_ERR_INVAL_PARAMS   Channel number is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   @c aID is invalid
    @retval     #BCM_ERR_UNINIT         PWM Driver not initialized for specified
                                        hardware ID

    @post TBD

    @trace #BRCM_SWREQ_PWM_CHANNEL_OPERATIONS

    @limitations None
*/
extern int32_t PWM_SetPeriodDutyCycle(PWM_HwIDType aID,
                                      PWM_ChanIDType aChanNum,
                                      PWM_ValueType aPeriod,
                                      PWM_ValueType aDutyCycle);
#endif /* PWM_H */
/** @} */
