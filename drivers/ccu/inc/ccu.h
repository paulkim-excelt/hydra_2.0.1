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
    @defgroup grp_ccudrv_ifc Interface
    @ingroup grp_ccudrv

    @addtogroup grp_ccudrv_ifc
    @{
    @section sec_ccu_overview Overview
    @image html ccu_overview.jpg "Driver overview diagram"
    CCU driver manages compare output and capture input for CCT block. Each
    channel of CCU can be configured in capture or compare mode.

    In compare mode, CCU channel is configured for a compare threshold value. If
    pin action chosen is HIGH level on compare match, in this case compare
    output is set to LOW level from start of CCU counter value till compare
    threshold. On compare match, compare output is set to HIGH level till CCU
    counter increments to maximum configured count. In this way, different duty
    cycle signals can be generated on compare output pin.

    In capture mode, CCU channel captures different signal information of input
    signal on capture pin. CCU channel can be configure for edge count, edge
    detect, timestamp capture and signal measurement capture modes. In edge
    count mode, CCU driver can be queried for number of expected edge counted of
    the input signal. In edge detect mode, CCU driver invoke callback function
    on detection of expected edge of the input signal. In timestamp mode, CCU
    driver captures timestamp on reception of expected edge of the input signal
    and invokes callback based on the notify interval configured. In signal
    measurement mode, CCU driver can be used to retrieve HIGH time, LOW time,
    period or duty cycle of the input signal.

    @note There shall be only one client for each CCU channel.

    @section sec_ccu_fsm State Machine
    This section describes state machines for driver, compare channel and
    capture channel in different modes.

    @subsection subsec_ccu_fsm_drv Driver State Machine
    @image html ccu_state_machine_drv.jpg "Driver state machine"

    @subsection subsec_ccu_fsm_comp Compare Channel State Machine
    @image html ccu_state_machine_comp.jpg "Compare channel state machine"

    @subsection subsec_ccu_fsm_cap Capture Channel State Machines

    @par Capture Channel – “Edge Count Mode” State Machine
    @image html ccu_state_machine_cap_ed_cnt.jpg "Edge Count Mode state machine"

    @par Capture Channel - “Edge Detect Mode” State Machine
    @image html ccu_state_machine_cap_ed_det.jpg "Edge Detect Mode state machine"

    @par Capture Channel - “Timestamp Mode” State Machine
    @image html ccu_state_machine_cap_ts.jpg "Timestamp Mode state machine"

    @par Capture Channel - “Signal Measurement Mode” State Machine
    @todo image html ccu_state_machine_cap_sig.jpg "Signal Measurement Mode state machine"

    @section sec_ccu_seq Sequence Diagrams
    This section describes use-case sequence diagrams for CCU driver.

    @subsection subsec_ccu_seq_init Driver Init - DeInit
    The following sequence diagram depicts initialization and de-initialization
    sequence for CCU driver.
    @image html ccu_seq_diagram_init.jpg "Driver init – deinit sequence"

    @subsection subsec_ccu_seq_comp Compare Channel Use-Case
    The following sequence diagram depicts use-case sequence for compare
    channel.
    @image html ccu_seq_diagram_comp.jpg "Compare channel use-case sequence"

    @subsection subsec_ccu_seq_ed_cnt Capture Channel – “Edge Count Mode”
    The following sequence diagram depicts use-case sequence diagram for capture
    channel in “edge count mode”.
    @image html ccu_seq_diagram_ed_cnt.jpg "Edge count mode use-case sequence"

    @subsection subsec_ccu_seq_ed_det Capture Channel – “Edge Detect Mode”
    The following sequence diagram depicts use-case sequence diagram for capture
    channel in “edge detect mode”.
    @image html ccu_seq_diagram_ed_det.jpg "Edge detect mode use-case sequence"

    @subsection subsec_ccu_seq_ts Capture Channel – “Timestamp Mode”
    The following sequence diagram depicts use-case sequence diagram for capture
    channel in “edge timestamp mode”.
    @image html ccu_seq_diagram_ts.jpg "Timestamp mode use-case sequence"

    @subsection subsec_ccu_seq_sig Capture Channel – “Signal Measurement Mode”
    The below sequence diagram depicts use-case sequence diagram for capture
    channel in “signal measurement mode”. Before retrieving signal measurement
    values, client shall check for Capture input state by calling
    #CCU_GetCapInputState. If input state is #CCU_INPUTSTATE_ACTIVE, this
    indicates signal measurement is complete and signal measure values can be
    retrieved using #CCU_GetCapTimeElapsed or #CCU_GetCapDutyCycleValues.
    Otherwise, signal measurement values shall not be retrieved.
    @image html ccu_seq_diagram_sig.jpg "Signal measurement mode use-case sequence"

    @section sec_ccu_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @limitations None

    @file ccu.h
    @brief CCU driver application interface
    This header file contains the APIs for CCU driver.
    @version 0.22 Imported from docx
*/

#ifndef CCU_H
#define CCU_H

#include <stddef.h>
#include <inttypes.h>

/**
    @name CCU API IDs
    @{
    @brief API IDs for CCU driver
*/
#define BRCM_SWARCH_CCU_CHAN_TYPE                       (0x00U) /**< @brief #CCU_ChanType */
#define BRCM_SWARCH_CCU_CHANMODE_TYPE                   (0x01U) /**< @brief #CCU_ChanModeType */
#define BRCM_SWARCH_CCU_OUTPINSEL_TYPE                  (0x02U) /**< @brief #CCU_OutPinSelType */
#define BRCM_SWARCH_CCU_PINACTION_TYPE                  (0x03U) /**< @brief #CCU_PinActionType */
#define BRCM_SWARCH_CCU_INPINSEL_TYPE                   (0x04U) /**< @brief #CCU_InPinSelType */
#define BRCM_SWARCH_CCU_INPUTSTATE_TYPE                 (0x05U) /**< @brief #CCU_InputStateType */
#define BRCM_SWARCH_CCU_EDGE_TYPE                       (0x06U) /**< @brief #CCU_EdgeType */
#define BRCM_SWARCH_CCU_VALUE_TYPE                      (0x07U) /**< @brief #CCU_ValueType */
#define BRCM_SWARCH_CCU_TSINDEX_TYPE                    (0x08U) /**< @brief #CCU_TSIndexType */
#define BRCM_SWARCH_CCU_EDGENUMBER_TYPE                 (0x09U) /**< @brief #CCU_EdgeNumberType */
#define BRCM_SWARCH_CCU_CAPMODE_TYPE                    (0x0AU) /**< @brief #CCU_CapModeType */
#define BRCM_SWARCH_CCU_SIGNALPROPERTY_TYPE             (0x0BU) /**< @brief #CCU_SignalPropertyType */
#define BRCM_SWARCH_CCU_TIMESTAMPBUF_TYPE               (0x0CU) /**< @brief #CCU_TimeStampBufType */
#define BRCM_SWARCH_CCU_CALLBACK_TYPE                   (0x0DU) /**< @brief #CCU_CallbackType */
#define BRCM_SWARCH_CCU_CHANCAPCFG_TYPE                 (0x0EU) /**< @brief #CCU_ChanCapCfgType */
#define BRCM_SWARCH_CCU_CHANCOMPCFG_TYPE                (0x0FU) /**< @brief #CCU_ChanCompCfgType */
#define BRCM_SWARCH_CCU_CHANCFGTBL_TYPE                 (0x10U) /**< @brief #CCU_ChanCfgTblType */
#define BRCM_SWARCH_CCU_CONFIG_TYPE                     (0x11U) /**< @brief #CCU_ConfigType */
#define BRCM_SWARCH_CCU_DUTYCYCLE_TYPE                  (0x12U) /**< @brief #CCU_DutyCycleType */
#define BRCM_SWARCH_CCU_INIT_PROC                       (0x13U) /**< @brief #CCU_Init */
#define BRCM_SWARCH_CCU_DEINIT_PROC                     (0x14U) /**< @brief #CCU_DeInit */
#define BRCM_SWARCH_CCU_GETCHANMODE_PROC                (0x15U) /**< @brief #CCU_GetChanMode */
#define BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC           (0x16U) /**< @brief #CCU_StartCompChannel */
#define BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC            (0x17U) /**< @brief #CCU_StopCompChannel */
#define BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC         (0x18U) /**< @brief #CCU_SetCompPinProperty */
#define BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC        (0x19U) /**< @brief #CCU_SetCompAbsThreshold */
#define BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC         (0x1AU) /**< @brief #CCU_EnableCompCallback */
#define BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC        (0x1BU) /**< @brief #CCU_DisableCompCallback */
#define BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC           (0x1CU) /**< @brief #CCU_SetCapSignalProp */
#define BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC          (0x1DU) /**< @brief #CCU_EnableCapCallback */
#define BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC         (0x1EU) /**< @brief #CCU_DisableCapCallback */
#define BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC           (0x1FU) /**< @brief #CCU_GetCapInputState */
#define BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC          (0x20U) /**< @brief #CCU_StartCapTimestamp */
#define BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC           (0x21U) /**< @brief #CCU_StopCapTimestamp */
#define BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC       (0x22U) /**< @brief #CCU_GetCapTimestampIndex */
#define BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC          (0x23U) /**< @brief #CCU_ResetCapEdgeCount */
#define BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC         (0x24U) /**< @brief #CCU_EnableCapEdgeCount */
#define BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC        (0x25U) /**< @brief #CCU_DisableCapEdgeCount */
#define BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC     (0x26U) /**< @brief #CCU_EnableCapEdgeDetection */
#define BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC    (0x27U) /**< @brief #CCU_DisableCapEdgeDetection */
#define BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC          (0x28U) /**< @brief #CCU_GetCapEdgeNumbers */
#define BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC  (0x29U) /**< @brief #CCU_StartCapSignalMeasurement */
#define BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC   (0x2AU) /**< @brief #CCU_StopCapSignalMeasurement */
#define BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC          (0x2BU) /**< @brief #CCU_GetCapTimeElapsed */
#define BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC      (0x2CU) /**< @brief #CCU_GetCapDutyCycleValues */
/** @} */

/**
    @name CCU_ChanType
    @{
    @brief CCU channel type

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_ChanType;           /**< @brief CCU channel type */
#define CCU_CHAN_0       (0UL)           /**< @brief CCU channel 0 ID */
#define CCU_CHAN_1       (1UL)           /**< @brief CCU channel 1 ID */
#define CCU_CHAN_2       (2UL)           /**< @brief CCU channel 2 ID */
#define CCU_CHAN_3       (3UL)           /**< @brief CCU channel 3 ID */
#define CCU_CHAN_MAX     (CCU_CHAN_3)    /**< @brief CCU channel ID max */
/** @} */

/**
    @name CCU_ChanModeType
    @{
    @brief CCU channel mode type

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_ChanModeType;      /**< @brief CCU channel mode type */
#define CCU_CHANMODE_CAPTURE   (1UL)    /**< @brief CCU channel mode: capture */
#define CCU_CHANMODE_COMPARE   (2UL)    /**< @brief CCU channel mode: compare */
#define CCU_CHANMODE_NONE      (3UL)    /**< @brief CCU channel mode: none */
/** @} */

/**
    @name CCU_OutPinSelType
    @{
    @brief CCU output pin select

    This type is used to select the compare out pin.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_OutPinSelType;   /**< @brief CCU output pin select type */
#define CCU_OUTPINSEL_OUT     (1UL)   /**< @brief CCU out pin select OUTn */
#define CCU_OUTPINSEL_OUT_N   (2UL)   /**< @brief CCU out pin select OUTn_n */
/** @} */

/**
    @name CCU_PinActionType
    @{
    @brief CCU pin action

    This type is used to define the action to be taken on compare out pin on
    compare match.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_PinActionType;    /**< @brief CCU pin action type */
#define CCU_PINACTION_SET_LOW  (1UL)   /**< @brief Set out pin to LOW level */
#define CCU_PINACTION_SET_HIGH (2UL)   /**< @brief Set out pin to HIGH level */
#define CCU_PINACTION_TOGGLE   (3UL)   /**< @brief Toggle out pin level. <b>
                                             This pin action is not supported.
                                             </b> */
#define CCU_PINACTION_DISABLE  (4UL)   /**< @brief Disable change on out pin
                                             level */
/** @} */

/**
    @name CCU_InPinSelType
    @{
    @brief CCU input pin select type

    This type is used to select the capture input pin.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_InPinSelType;    /**< @brief CCU input pin select type */
#define CCU_INPINSEL_IN0      (0UL)   /**< @brief CCU input pin select IN0 */
#define CCU_INPINSEL_IN1      (1UL)   /**< @brief CCU input pin select IN1 */
#define CCU_INPINSEL_IN2      (2UL)   /**< @brief CCU input pin select IN2 */
#define CCU_INPINSEL_IN3      (3UL)   /**< @brief CCU input pin select IN3 */
/** @} */

/**
    @name CCU_InputStateType
    @{
    @brief CCU input state type

    This type is used to describe capture channel input state.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_InputStateType;   /**< @brief CCU input state type */
#define CCU_INPUTSTATE_ACTIVE  (1UL)   /**< @brief CCU input state ACTIVE: an
                                            activation edge is detected */
#define CCU_INPUTSTATE_IDLE    (2UL)   /**< @brief CCU input state IDLE: no
                                            activation edge detected since last
                                            call of #CCU_GetCapInputState or
                                            #CCU_Init */
/** @} */

/**
    @name CCU_EdgeType
    @{
    @brief CCU edge type

    This type is used to describe signal edges.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_EdgeType;      /**< @brief CCU edge type */
#define CCU_EDGE_RISING     (1UL)   /**< @brief CCU signal rising edge */
#define CCU_EDGE_FALLING    (2UL)   /**< @brief CCU signal falling edge */
#define CCU_EDGE_BOTH       (3UL)   /**< @brief CCU signal both edge */
/** @} */

/**
    @name CCU_ValueType
    @{
    @brief CCU value type

    This type is used to describe capture timestamp ticks, absolute threshold
    for compare etc.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint16_t CCU_ValueType;     /**< @brief CCU value type */
/** @} */

/**
    @name CCU_TSIndexType
    @{
    @brief CCU timestamp index type

    This type is used to describe timestamp buffer index.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_TSIndexType;   /**< @brief CCU timestamp index type */
/** @} */

/**
    @name CCU_EdgeNumberType
    @{
    @brief CCU edge number type

    This type is used to describe number of capture signal edge detected in case
    of capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_EdgeNumberType;    /**< @brief CCU edge number type */
/** @} */

/**
    @name CCU_CapModeType
    @{
    @brief CCU capture mode

    This type is used to describe mode of capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_CapModeType;          /**< @brief CCU capture mode
                                                 type */
#define CCU_CAPMODE_NONE           (0UL)   /**< @brief No capture mode */
#define CCU_CAPMODE_TIMESTAMP      (1UL)   /**< @brief Timestamp capture
                                                 mode */
#define CCU_CAPMODE_EDGE_COUNT     (2UL)   /**< @brief Edge count capture
                                                 mode */
#define CCU_CAPMODE_EDGE_DETECT    (3UL)   /**< @brief Edge detect capture
                                                 mode */
#define CCU_CAPMODE_SIGNAL_MEASURE (4UL)   /**< @brief Signal measure capture
                                                 mode */
/** @} */

/**
    @name CCU_SignalPropertyType
    @{
    @brief CCU signal property type

    This type is used to describe signal property in case of
    #CCU_CAPMODE_SIGNAL_MEASURE mode of capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_SignalPropertyType;       /**< @brief CCU signal property
                                                    type */
#define CCU_SIGNALPROPERTY_LOW_TIME    (1UL)   /**< @brief Signal property LOW
                                                    time */
#define CCU_SIGNALPROPERTY_HIGH_TIME   (2UL)   /**< @brief Signal property HIGH
                                                    time */
#define CCU_SIGNALPROPERTY_PERIOD_TIME (3UL)   /**< @brief Signal property period
                                                    time */
#define CCU_SIGNALPROPERTY_DUTY_CYCLE  (4UL)   /**< @brief Signal property duty
                                                    cycle */
/** @} */

/**
    @name CCU_TimeStampBufType
    @{
    @brief CCU timestamp buffer type

    This type is used to describe type of timestamp buffer i.e. linear,
    circular.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_TimeStampBufType;        /**< @brief CCU timestamp buffer type */
#define CCU_TIMESTAMPBUF_LINEAR       (1UL)   /**< @brief Timestamp buffer linear,
                                                   shall be filled once */
#define CCU_TIMESTAMPBUF_CIRCULAR     (2UL)   /**< @brief Timestamp buffer circular,
                                                   shall be filled in circular
                                                   order */
/** @} */

/** @brief CCU callback type

    This type is used to describe function pointers for callback functions.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU_CONFIGURATION

    @limitations Callback functions are invoked from interrupt context. Callback
    functions shall be implemented to have minimum execution time.
*/
typedef void (*CCU_CallbackType) (void);

/**
    @brief Configuration for capture channel

    Type to describe configuration for capture channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct _CCU_ChanCapCfgType {
    CCU_CallbackType const timestampCB; /**< @brief Function pointer for
                                             timestamp callback. This callback
                                             function is invoked by driver in
                                             #CCU_CAPMODE_TIMESTAMP mode based
                                             on the notify interval mentioned
                                             during #CCU_StartCapTimestamp and
                                             when callback is enabled by calling
                                             #CCU_EnableCapCallback. */
    CCU_CallbackType const edgeDetectCB;/**< @brief Function pointer for
                                             edge-detect callback. This callback
                                             function is invoked by driver on
                                             detection of expected edge when
                                             edge-detect mode and capture
                                             callback are enabled by calling
                                             #CCU_EnableCapEdgeDetection and
                                             #CCU_EnableCapCallback
                                             respectively. */
    CCU_InPinSelType pin;               /**< @brief Default input pin for the
                                             channel */
    CCU_EdgeType edge;                  /**< @brief Default capture edge for the
                                             channel */
    CCU_CapModeType mode;               /**< @brief Capture mode for the
                                             channel */
    CCU_TimeStampBufType TSBufProp;     /**< @brief Type of timestamp buffer for
                                             the channel. This is used in case
                                             of #CCU_CAPMODE_TIMESTAMP capture
                                             mode. */
    CCU_SignalPropertyType sigMeasProp; /**< @brief Signal measurement property
                                             for the channel. This parameter is
                                             to be used in case of
                                             #CCU_CAPMODE_SIGNAL_MEASURE
                                             capture mode. */
} CCU_ChanCapCfgType;

/**
    @brief Configuration for compare channel

    Type to describe configuration for compare channel.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct _CCU_ChanCompCfgType {
    CCU_CallbackType const compCB;  /**< @brief Function pointer for compare
                                         callback. This function is invoked by
                                         driver on compare match, if callback is
                                         enabled by calling
                                         #CCU_EnableCompCallback */
    CCU_ValueType compThreshold;    /**< @brief Default compare threshold for
                                         the channel */
} CCU_ChanCompCfgType;

/**
    @brief Channel configuration table

    Type to describe channel configuration table.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct _CCU_ChanCfgTblType {
    CCU_ChanType chanNum;       /**< @brief Channel number */
    CCU_ChanModeType chanMode;  /**< @brief Mode of the channel */
    CCU_ChanCapCfgType capCfg;  /**< @brief Capture channel configuration. This
                                     field shall be updated appropriately in
                                     case of chanMode is
                                     #CCU_CHANMODE_CAPTURE. */
    CCU_ChanCompCfgType compCfg;/**< @brief Compare channel configuration. This
                                     field shall be updated appropriately in
                                     case of chanMode is
                                     #CCU_CHANMODE_COMPARE. */
} CCU_ChanCfgTblType;

/**
    @brief CCU driver configuration

    Type to describe CCU driver configuration.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct _CCU_ConfigType {
    uint16_t cntrMaxValue;              /**< @brief CCU counter’s maximum value.
                                             CCU counter starts from 0 and
                                             increment till maximum value before
                                             wrapping back to 0. Functionality
                                             of all the capture and compare
                                             channel is based on single CCU
                                             counter. */
    uint16_t preScaleRatio;             /**< @brief Pre scale ratio for clock
                                            used to increment CCU counter. This
                                            ratio is used to decrease frequency
                                            of clock feed to CCU counter. i.e.
                                            - 0: Clock frequency division by 1
                                            - 1: Clock frequency division by 2
                                            - 2: Clock frequency division by 3
                                            - And so on... */
    CCU_ChanCfgTblType * const chanCfgTbl;  /**< @brief Pointer to channel
                                                 configuration table */
    uint32_t chanCfgTblSz;                  /**< @brief Size of the channel
                                                 configuration table */
} CCU_ConfigType;

/**
    @brief Signal duty cycle

    Type to describe capture signal duty-cycle for capture channel in signal
    measurement mode.

    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct _CCU_DutyCycleType {
    CCU_ValueType activeTime;   /**< @brief Active time of the signal in terms
                                     of CCU counter ticks. Active time means
                                     time duration in ticks for which input
                                     signal is at level HIGH within a period. */
    CCU_ValueType periodTime;   /**< @brief Period of the signal in terms of
                                     CCU counter ticks. Period is the time
                                     duration in ticks between two consecutive
                                     rising edges of input signal. */
} CCU_DutyCycleType;

/** @brief Initialize CCU driver

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aConfig     Pointer to CCU driver configuration

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_Init(const CCU_ConfigType *const aConfig);

/** @brief De-initialize CCU driver

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_DeInit(void);

/** @brief Retrieve mode of the channel

    API to retrieve mode of the channel.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aChanNum    Channel number for which mode is queried

    Return values are documented in reverse-chronological order
    @retval     #CCU_ChanModeType       Mode of the channel

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern CCU_ChanModeType CCU_GetChanMode(CCU_ChanType aChanNum);

/** @brief Start compare channel

    API to start compare channel. This API changes channel state from stopped to
    running. This API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_StartCompChannel(CCU_ChanType aChanNum);

/** @brief Stop compate channel

    API to stop compare channel. This API changes channel state from running to
    stopped. This API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_StopCompChannel(CCU_ChanType aChanNum);

/** @brief Set compare out pin properties

    API to set compare out pin properties. This API shall be called when channel
    is configured in #CCU_CHANMODE_COMPARE mode and channel is not in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number
    @param[in]      aPinAction  Action on compare output pin on compare match
    @param[in]      aOutPin     Compare output pin selected

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_SetCompPinProperty(CCU_ChanType aChanNum,
                                   CCU_PinActionType aPinAction,
                                   CCU_OutPinSelType aOutPin);

/** @brief Set absolute threshold for compare match

    API to set absolute threshold for compare match. This API shall be called
    when channel is configured in #CCU_CHANMODE_COMPARE mode and channel is not
    in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number
    @param[in]      aAbsValue   Absolute threshold value for compare match

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_SetCompAbsThreshold(CCU_ChanType aChanNum,
                                    CCU_ValueType aAbsValue);

/** @brief Enable compare callback

    API to enable compare callback. If compare callback is enabled,
    CCU_ChanCompCfgType::compCB is non-null in compare channel config and
    compare channel is started by #CCU_StartCompChannel, then
    CCU_ChanCompCfgType::compCB shall be invoked for each compare match. This
    API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode and channel is not in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_EnableCompCallback(CCU_ChanType aChanNum);

/** @brief Disable compare callback

    API to disable compare callback. If compare callback is disabled then
    CCU_ChanCompCfgType::compCB shall not be invoked for each compare match on
    the channel. This API shall be called when channel is configured in
    #CCU_CHANMODE_COMPARE mode and channel is not in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_DisableCompCallback(CCU_ChanType aChanNum);

/** @brief Set capture signal properties

    API to set capture signal properties. This API shall be called when channel
    is configured in #CCU_CHANMODE_CAPTURE mode and channel is not in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number
    @param[in]      aInPin      Input pin selected for the capture channel
    @param[in]      aEdge       Capture edge for the channel

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_SetCapSignalProp(CCU_ChanType aChanNum,
                                 CCU_InPinSelType aInPin,
                                 CCU_EdgeType aEdge);

/** @brief Enable capture callback

    API to enable capture callback. If the capture mode selected is
    #CCU_CAPMODE_EDGE_DETECT, callback is enabled and
    CCU_ChanCapCfgType::edgeDetectCB is non-null, then
    CCU_ChanCapCfgType::edgeDetectCB shall be invoked on each edge.

    If the capture mode selected is #CCU_CAPMODE_TIMESTAMP, callback is enabled
    and CCU_ChanCapCfgType::timestampCB is non-null, then
    CCU_ChanCapCfgType::timestampCB shall be invoked after each notify interval.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode and channel is not in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_EnableCapCallback(CCU_ChanType aChanNum);

/** @brief Disable capture callback

    API to disable capture callback. This API shall be called when channel is
    configured in #CCU_CHANMODE_CAPTURE mode and channel is not in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_DisableCapCallback(CCU_ChanType aChanNum);

/** @brief Retrieve input state of the capture channel

    API to retrieve input state of the capture channel. This API shall return
    input state as #CCU_INPUTSTATE_IDLE, if capture edge not detected after
    starting of capture or after last call to #CCU_GetCapInputState. This API
    shall return input state as #CCU_INPUTSTATE_ACTIVE, if capture edge is
    detected after starting of capture or after last call to
    #CCU_GetCapInputState.

    In case of signal measurement, this API shall return input state as
    #CCU_INPUTSTATE_IDLE if signal measure is not complete and this API shall
    return input state as #CCU_INPUTSTATE_ACTIVE if signal measurement is
    complete.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum    Channel number

    Return values are documented in reverse-chronological order
    @retlva     #CCU_INPUTSTATE_ACTIVE Capture edge detected after starting of
                                        the capture or after last call to
                                        #CCU_GetCapInputState
    @retlva     #CCU_INPUTSTATE_IDLE   Capture edge not detected after starting
                                        of the capture or after last call to
                                        #CCU_GetCapInputState

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern CCU_InputStateType CCU_GetCapInputState(CCU_ChanType aChanNum);

/** @brief Start capture channel time stamping

    API to start capture channel time stamping.

    On starting of capture channel for time stamping, channel shall be moved
    from stopped to running state and time stamp shall be captured on detection
    of each configured edges.

    Time stamping can be stopped by explicitly calling #CCU_StopCapTimestamp or
    when @a aTSBuf is of linear type and it is completely filled.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode and channel capture mode is
    #CCU_CAPMODE_TIMESTAMP.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number
    @param[out]     aTSBuf          Pointer to timestamp buffer
    @param[in]      aBufSize        Size of timestamp buffer in terms of
                                    #CCU_ValueType
    @param[in]      aNotifyInterval Interval after which
                                    CCU_ChanCapCfgType::timestampCB shall be
                                    invoked (if callback is enabled and @a
                                    aNotifyInterval is greater than 0). @a
                                    aNotifyInterval is in terms of number of
                                    timestamp captured.

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_StartCapTimestamp(CCU_ChanType aChanNum,
                                  CCU_ValueType *const aTSBuf,
                                  uint32_t aBufSize,
                                  uint32_t aNotifyInterval);

/** @brief Stop capture channel time stamping

    API to stop capture channel time stamping.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode and channel
    capture mode is #CCU_CAPMODE_TIMESTAMP.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_StopCapTimestamp(CCU_ChanType aChanNum);

/** @brief Retrieve capture channel timestamp index

    API to retrieve capture channel timestamp index.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is #CCU_CAPMODE_TIMESTAMP
    and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @retval     #CCU_IndexType          Index of next timestamp to be written in
                                        the timestamp buffer.

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern CCU_TSIndexType CCU_GetCapTimestampIndex(CCU_ChanType aChanNum);

/** @brief Reset the capture edge counter

    API to reset the capture edge counter.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_EDGE_COUNT and channel is in stopped state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_ResetCapEdgeCount(CCU_ChanType aChanNum);

/** @brief Enable capture edge counter

    API to enable capture edge counter.

    This API changes channel from stopped to running state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_COUNT and channel is in stopped state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_EnableCapEdgeCount(CCU_ChanType aChanNum);

/** @brief Disable capture edge counter

    API to disable capture edge counter.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_COUNT and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_DisableCapEdgeCount(CCU_ChanType aChanNum);

/** @brief Enable capture edge detection

    API to enable capture edge detection.

    This API changes channel from stopped to running state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_DETECT and channel is in stopped state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_EnableCapEdgeDetection(CCU_ChanType aChanNum);

/** @brief Disable capture edge detection

    API to disable capture edge detection.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_EDGE_DETECT and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_DisableCapEdgeDetection(CCU_ChanType aChanNum);

/** @brief Retrieve capture edge number

    API to retrieve capture edge number.
    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_EDGE_COUNT and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @retval     #CCU_EdgeNumberType     Number of edges detected on the channel

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern CCU_EdgeNumberType CCU_GetCapEdgeNumbers(CCU_ChanType aChanNum);

/** @brief Start capture signal measurement

    API to start capture signal measurement.
    This API changes channel from stopped to running state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_SIGNAL_MEASURE and channel is in stopped
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_StartCapSignalMeasurement(CCU_ChanType aChanNum);

/** @brief Stop capture signal measurement

    API to stop capture signal measurement.

    This API changes channel from running to stopped state. This API shall be
    called when channel is configured in #CCU_CHANMODE_CAPTURE mode, channel
    capture mode is #CCU_CAPMODE_SIGNAL_MEASURE and channel is in running
    state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_StopCapSignalMeasurement(CCU_ChanType aChanNum);

/** @brief Retrieve capture time elapsed

    API to retrieve capture time elapsed.

    Elapsed time shall be 0 in case signal measurement is in-progress or channel
    input state is #CCU_INPUTSTATE_IDLE. This API shall be called when channel
    is configured in #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_SIGNAL_MEASURE, signal measure property is
    #CCU_SIGNALPROPERTY_LOW_TIME / #CCU_SIGNALPROPERTY_HIGH_TIME /
    #CCU_SIGNALPROPERTY_PERIOD_TIME and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    Return values are documented in reverse-chronological order
    @retval     #CCU_ValueType          Elapsed time for channel after
                                        #CCU_StartCapSignalMeasurementor after
                                        last #CCU_GetCapTimeElapsed. Elapsed
                                        time refers to:
                                        - Low time, if signal property is
                                          #CCU_SIGNALPROPERTY_LOW_TIME
                                        - High time, if signal property is
                                          #CCU_SIGNALPROPERTY_HIGH_TIME
                                        - Period time, if signal property is
                                          #CCU_SIGNALPROPERTY_PERIOD_TIME

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern CCU_ValueType CCU_GetCapTimeElapsed(CCU_ChanType aChanNum);

/** @brief Retrieve capture channel duty cycle values

    API to retrieve capture channel duty cycle values.

    This API provides CCU_DutyCycleType::activeTime and
    CCU_DutyCycleType::periodTime for concurrent interval for single duty
    cycle value. Duty cycle, CCU_DutyCycleType::activeTime and
    CCU_DutyCycleType::periodTime are returned as 0 in case these are not
    captured for concurrent interval.

    This API shall be called when channel is configured in
    #CCU_CHANMODE_CAPTURE mode, channel capture mode is
    #CCU_CAPMODE_SIGNAL_MEASURE, signal measure property is
    #CCU_SIGNALPROPERTY_DUTY_CYCLE and channel is in running state.

    @behavior Sync, Re-entrant for different aChanNum

    @pre TBD

    @param[in]      aChanNum        Channel number

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_CCU

    @limitations None
*/
extern void CCU_GetCapDutyCycleValues(CCU_ChanType aChanNum,
                                      CCU_DutyCycleType *const aDutyCycle);

#endif /* CCU_H */
/** @} */
