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
    @defgroup grp_adcdrv_ifc Interface
    @ingroup grp_adcdrv

    @addtogroup grp_adcdrv_ifc
    @{
    @section sec_adc_overview Overview
    @image html adc_overview.jpg
    The Analog-to-Digital Converter (ADC) driver initializes and controls the
    internal ADC hardware units. The ADC driver provides APIs to start and stop
    conversion. It also provides APIs to enable and disable a notification
    mechanism as wells ways to query the status and result of a conversion.

    The ADC driver works on ADC channel groups, which is nothing but a logical
    grouping of the various ADC analog input pins (ADC channels) within a given
    ADC hardware unit, accessed and controlled as a single entity. The same
    channel can be part of multiple channel groups as well.

    ADC driver provides API to enable a notification mechanism.
    #ADC_EnableGroupNotif that notifies the client when one conversion round is
    completed. If required, client may disable the notification
    #ADC_DisableGroupNotif as well. ADC driver also provides the option to
    query the status of a channel group.

    A group conversion when client calls ADC_StartGroupConv.
    Conversion of a group is done in rounds. One conversion round consists of
    one sample per channel. Only after the completion of the one round, next
    conversion round is started. If group notification is enabled by client,
    it will receieve the callback when a round of covertion completes.
    Client can also query the status of the group by calling #ADC_GetGroupConvState
    and if the conversion state is #ADC_CHANGRPCONVSTATE_AVAILABLE, client
    can read the latest conversion data using #ADC_ReadGroupSample

    ADC driver also provides an API #ADC_StopGroupConv, to stop conversion of a
    channel group. This API stops any ongoing conversion of the given channel.

    ADC driver provides allows only one group conversion at time. While a
    group conversion is going on, if client tries to start conversion of
    another group, ADC driver returns error #BCM_ERR_BUSY

    @section sec_adc_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_adc.dep

    @limitations None

    |   References                                                          |
    |:---------------------------------------------------------------------:|
    |   @anchor ref_uml_spec UML 2.5 specification                          |
    |   @anchor ref_sads_spec Software Architecture and Design Specification|
    |   @anchor ref_trm_spec Technical Reference Manual                     |
    |   @anchor ref_utilities_spec Utilities Interface Specification        |

    @file adc.h
    @brief ADC Driver interface
    This header file contains the interface functions for ADC Driver
    @version 1.0 Changelist1
    @version 0.1 Changelist0
*/

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/**
    @name ADC Driver Architecture IDs
    @{
    @brief Architecture IDs for ADC Driver
*/
#define BRCM_SWARCH_ADC_HWID_TYPE                    (0x00U) /**< @brief #ADC_HwIDType              */
#define BRCM_SWARCH_ADC_CHANNELID_TYPE               (0x01U) /**< @brief #ADC_ChannelIDType         */
#define BRCM_SWARCH_ADC_VAL_TYPE                     (0x02U) /**< @brief #ADC_ValType               */
#define BRCM_SWARCH_ADC_CHANGRPID_TYPE               (0x03U) /**< @brief #ADC_ChanGrpIDType         */
#define BRCM_SWARCH_ADC_CHANGRPCONVSTATE_TYPE        (0x04U) /**< @brief #ADC_ChanGrpConvStateType  */
#define BRCM_SWARCH_ADC_LIMITCHECK_TYPE              (0x05U) /**< @brief #ADC_LimitCheckType        */
#define BRCM_SWARCH_ADC_CHANNCFG_TYPE                (0x06U) /**< @brief #ADC_ChannCfgType          */
#define BRCM_SWARCH_ADC_GRPCONVCB_PROC               (0x07U) /**< @brief #ADC_GrpConvCbType         */
#define BRCM_SWARCH_ADC_GRPCFG_TYPE                  (0x07U) /**< @brief #ADC_GrpCfgType            */
#define BRCM_SWARCH_ADC_CONFIG_TYPE                  (0x09U) /**< @brief #ADC_ConfigType            */
#define BRCM_SWARCH_ADC_INIT_PROC                    (0x0AU) /**< @brief #ADC_Init                  */
#define BRCM_SWARCH_ADC_DEINIT_PROC                  (0x0BU) /**< @brief #ADC_DeInit                */
#define BRCM_SWARCH_ADC_STARTGROUPCONV_PROC          (0x0CU) /**< @brief #ADC_StartGroupConv        */
#define BRCM_SWARCH_ADC_STOPGROUPCONV_PROC           (0x0DU) /**< @brief #ADC_StopGroupConv         */
#define BRCM_SWARCH_ADC_READGROUPSAMPLE_PROC         (0x0EU) /**< @brief #ADC_ReadGroupSample       */
#define BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC        (0x0FU) /**< @brief #ADC_EnableGroupNotif      */
#define BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC       (0x10U) /**< @brief #ADC_DisableGroupNotif     */
#define BRCM_SWARCH_ADC_GETGROUPCONVSTATE_PROC       (0x11U) /**< @brief #ADC_GetGroupConvState     */
/** @} */

/**
    @brief ADC Hardware ID type

    @trace #BRCM_SWREQ_ADC_INIT
*/
typedef uint32_t ADC_HwIDType;

/**
    @name ADC_ChannelIDType
    @{
    @brief ADC Channel ID

    @trace #BRCM_SWREQ_ADC_INIT
*/
typedef uint32_t ADC_ChannelIDType;   /**< @brief ADC_ChannelID typedef */
#define ADC_CHANNELID_0   (0UL)       /**< @brief ADC Channel ID 0      */
#define ADC_CHANNELID_1   (1UL)       /**< @brief ADC Channel ID 1      */
#define ADC_CHANNELID_2   (2UL)       /**< @brief ADC Channel ID 2      */
/** @} */

/**
    @brief ADC value type
    Type for the converted values of a channel group

    @trace #BRCM_SWREQ_ADC_INIT
*/
typedef uint32_t ADC_ValType;

/**
    @brief ADC channel group ID type

    @trace #BRCM_SWREQ_ADC_KERNEL_INTERFACE
*/
typedef uint32_t ADC_ChanGrpIDType;

/**
    @name ADC_ChanGrpConvStateType
    @{
    @brief ADC channel group conversion state

    @trace #BRCM_SWREQ_ADC_KERNEL_INTERFACE
*/
typedef uint32_t ADC_ChanGrpConvStateType;      /**< @brief ADC_ChanGrpConvState typedef    */
#define ADC_CHANGRPCONVSTATE_IDLE         (0UL) /**< @brief Channel group is idle           */
#define ADC_CHANGRPCONVSTATE_BUSY         (1UL) /**< @brief Channel group conversion in
                                                            progress and no conversion
                                                            results are available yet       */
#define ADC_CHANGRPCONVSTATE_AVAILABLE    (2UL) /**< @brief Channel group conversion is
                                                            in progress and atleast one
                                                            conversion result is available  */
/** @} */

/**
    @name ADC_LimitCheckType
    @{
    @brief ADC channel range check type
    This type defines which samples are taken into the sample buffer if limit
    check is enabled for the channel.

    @trace #BRCM_SWREQ_ADC_INIT
*/
typedef uint32_t ADC_LimitCheckType;            /**< @brief ADC_LimitCheck typedef              */
#define ADC_LIMITCHECK_ALWAYS            (0UL)  /**< @brief Complete range
                                                           (no limit check for low or high)     */
#define ADC_LIMITCHECK_BETWEEN           (1UL)  /**< @brief Sample value between low and high
                                                           (both included)                      */
#define ADC_LIMITCHECK_NOT_BETWEEN       (2UL)  /**< @brief Sample value below high limit or
                                                           above low limit (low limit included) */
#define ADC_LIMITCHECK_NOT_OVER_HIGH     (3UL)  /**< @brief Sample value below high limit
                                                           (high limit included)                */
#define ADC_LIMITCHECK_NOT_UNDER_LOW     (4UL)  /**< @brief Sample value above low limit        */
#define ADC_LIMITCHECK_OVER_HIGH         (5UL)  /**< @brief Sample value above high limit       */
#define ADC_LIMITCHECK_UNDER_LOW         (6UL)  /**< @brief Sample value below low limit
                                                           (low limit value included)           */
/** @} */

/**
    @brief ADC channel configuration

    @trace #BRCM_SWREQ_ADC_INIT
 */
typedef struct _ADC_ChannCfgType {
    ADC_ChannelIDType   channID;        /**< @brief ADC channel ID                  */
    ADC_LimitCheckType  limitCheck;     /**< @brief ADC channel range check type    */
    uint32_t            limitLow;       /**< @brief ADC channel range lower limit   */
    uint32_t            limitHigh;      /**< @brief ADC channel range maximum limit */
} ADC_ChannCfgType;

/** @brief ADC channel group notification callback type

    Type for channel group notification callback. Once notification is enabled,
    this callback notification is raised from the ADC driver whenever a group
    conversion round is completed.

    @note This callback notification is raised from the interrupt context; hence
    client shall keep these callbacks reasonably short

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @trace #BRCM_SWREQ_ADC_INIT

    @limitations None
*/
typedef void (*ADC_GrpConvCbType)(ADC_ChanGrpIDType aGrpID);

/**
    @brief ADC channel group configuration

    @trace #BRCM_SWREQ_ADC_INIT
 */
typedef struct _ADC_GrpCfgType {
    ADC_ChanGrpIDType   grpID;                              /**< @brief Channel group ID                */
    ADC_GrpConvCbType   const callback;                     /**< @brief Callback function for this
                                                                        channel group                   */
    uint32_t            channCfgIdxList[ADC_MAX_NUM_CHANN]; /**< @brief List of ADC channels belonging to
                                                                        this group. The entries are
                                                                        indices of the channels in the
                                                                        channel configuration list      */
    uint32_t            channCfgIdxListSz;                  /**< @brief Size of the channel index list */

} ADC_GrpCfgType;

/**
    @brief ADC hardware configuration

    @trace #BRCM_SWREQ_ADC_INIT
 */
typedef struct _ADC_ConfigType {
    ADC_HwIDType            hwID;                   /**< @brief ADC hardware ID                         */
    const ADC_ChannCfgType  *const channCfgList;    /**< @brief ADC channel configuration array         */
    uint32_t                channCfgListSz;         /**< @brief Size of the channel configuration array */
    const ADC_GrpCfgType    *const grpCfgList;      /**< @brief ADC channel group configuration array   */
    uint32_t                grpCfgListSz;           /**< @brief Size of the channel group
                                                                configuration array                     */
} ADC_ConfigType;

/** @brief Initialize the ADC driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @trace #BRCM_SWREQ_ADC_INIT

    @limitations None
*/
extern void ADC_Init(void);

/** @brief De-initialize the ADC driver

    @note This API de-initializes the ADC driver only if all the channel groups
     are in either #ADC_CHANGRPCONVSTATE_IDLE while no conversion is going on.
     If not, this API returns without taking any action

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @trace #BRCM_SWREQ_ADC_INIT

    @limitations None
*/
extern void ADC_DeInit(void);

/** @brief This API tries to start conversion of the given channel group

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aGrpID      Channel group ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Channel group conversion started
                                        successfully or channel group
    @retval     #BCM_ERR_BUSY           Channel group is not in
                                        #ADC_CHANGRPCONVSTATE_IDLE (if channel
                                        group is implicitly stoppable) or
                                        #ADC_CHANGRPCONVSTATE_COMPLETED (if
                                        channel group is not implicitly
                                        stoppable)
    @retval     #BCM_ERR_BUSY           Channel group is already queued
    @retval     #BCM_ERR_NOMEM          Sample buffer not setup
    @retval     #BCM_ERR_INVAL_PARAMS   aGrpID is invalid
    @retval     #BCM_ERR_UNINIT         ADC driver is not initialized

    @post None

    @trace #BRCM_SWREQ_ADC_SERVICES

    @limitations None
*/
extern int32_t ADC_StartGroupConv(ADC_ChanGrpIDType aGrpID);

/** @brief Stop channel group conversion

    This API stops conversion of the given channel group

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aGrpID      Channel group ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Channel group conversion stopped
    @retval     #BCM_ERR_INVAL_STATE    Channel group is already in
                                        #ADC_CHANGRPCONVSTATE_IDLE
    @retval     #BCM_ERR_INVAL_PARAMS   aGrpID is invalid
    @retval     #BCM_ERR_UNINIT         ADC driver is not initialized

    @post None

    @trace #BRCM_SWREQ_ADC_SERVICES

    @limitations None
*/
extern int32_t ADC_StopGroupConv(ADC_ChanGrpIDType aGrpID);

/** @brief Read result of the last completed round

    This API reads the result of the last completed conversion round and
    copies it to the given buffer

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aGrpID      Channel group ID
    @param[out]     aBuf        Pointer to data buffer

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Conversion results are available and
                                        read into the given data buffer
    @retval     #BCM_ERR_BUSY           Channel group is in
                                        #ADC_CHANGRPCONVSTATE_BUSY
    @retval     #BCM_ERR_INVAL_STATE    Channel group is in
                                        #ADC_CHANGRPCONVSTATE_IDLE
    @retval     #BCM_ERR_INVAL_PARAMS   aGrpID is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   aBuf is NULL
    @retval     #BCM_ERR_UNINIT         ADC driver is not initialized

    @post None

    @trace #BRCM_SWREQ_ADC_SERVICES

    @limitations None
*/
extern int32_t ADC_ReadGroupSample(ADC_ChanGrpIDType aGrpID,
                                   uint32_t *aBuf);

/** @brief Enable channel group notification

    This API enables the given channel group notification

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aGrpID      Channel group ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Channel group notification enabled
                                        successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Callback function pointer is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aGrpID is invalid
    @retval     #BCM_ERR_UNINIT         ADC driver is not initialized

    @post None

    @trace #BRCM_SWREQ_ADC_SERVICES

    @limitations None
*/
extern int32_t ADC_EnableGroupNotif(ADC_ChanGrpIDType aGrpID);

/** @brief Disable channel group notification

    This API disables the given channel group notification

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aGrpID      Channel group ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Channel group notification disabled
                                        successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Callback function pointer is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aGrpID is invalid
    @retval     #BCM_ERR_UNINIT         ADC driver is not initialized

    @post None

    @trace #BRCM_SWREQ_ADC_SERVICES

    @limitations None
*/
extern int32_t ADC_DisableGroupNotif(ADC_ChanGrpIDType aGrpID);

/** @brief Get the channel group conversion state

    This API gets the conversion state of the given channel group

    @note If aGrpID is invalid or ADC driver is not in initialized state this
    API shall return #ADC_CHANGRPCONVSTATE_IDLE

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aGrpID      Channel group ID

    @retval     #ADC_ChanGrpConvStateType   Conversion state of the channel
                                            group.

    @post None

    @trace #BRCM_SWREQ_ADC_SERVICES

    @limitations None
*/
extern ADC_ChanGrpConvStateType ADC_GetGroupConvState(ADC_ChanGrpIDType aGrpID);
#endif  /* ADC_H */
/** @} */
