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
    @defgroup grp_framework_ifc Media Framework Interface
    @ingroup grp_framework

    @addtogroup grp_framework_ifc
    @{
    @file media_system.h
    @brief Framework interface for applications.
    This file provides framework interfaces for the applications to interact
    with the different underlying components.
    @version 0.93 Added Media Port Statistics
    @version 0.92 Import from document
*/


#ifndef MEDIA_SYSTEM_H
#define MEDIA_SYSTEM_H

#include <stdint.h>
#include <media_port.h>
#include <wds.h>

/**
    @name Media System Architecture IDs
    @{
    @brief Architecture IDs for Media System
*/
#define BRCM_SWARCH_MSYS_MACRO            (0x00U)   /**< @brief #MSYS_MAGIC         */
#define BRCM_SWARCH_MSYS_TYPE             (0x01U)   /**< @brief #MSYS_Type          */
#define BRCM_SWARCH_MSYS_GET_CONFIG_PROC  (0x02U)   /**< @brief #MSYS_Get           */
#define BRCM_SWARCH_MSYS_INIT_PROC        (0x03U)   /**< @brief #MSYS_Init          */
#define BRCM_SWARCH_MSYS_CONFIGURE_PROC   (0x04U)   /**< @brief #MSYS_Configure     */
#define BRCM_SWARCH_MSYS_START_PROC       (0x05U)   /**< @brief #MSYS_Start         */
#define BRCM_SWARCH_MSYS_STOP_PROC        (0x06U)   /**< @brief #MSYS_Stop          */
#define BRCM_SWARCH_MSYS_DEINIT_PROC      (0x07U)   /**< @brief #MSYS_DeInit        */
#define BRCM_SWARCH_MSYS_GET_STATS_PROC   (0x08U)   /**< @brief #MSYS_GetInStats    */
#define BRCM_SWARCH_MSYS_WDS_ERR_CB_PROC  (0x09U)   /**< @brief #MSYS_WdsErrorCb    */
/** @} */

/**
    @name Media System Definition macros
    @{
    @brief Macros for Media System definition

    @trace #BRCM_SWREQ_MSYS_DEFINITION
*/
#define MSYS_MAGIC           (0x5359534DUL)  /**< Media system magic ID: MSYS             */
#define MSYS_MAX_LENGTH      (16UL)   /**< Maximum number of components in a media system */
#define MSYS_MAX_NUM_CONFIGS (16UL)  /**< Maximum number of component configurations */
/** @} */


/**
    @brief Media System Configuration Structure

    @trace #BRCM_SWREQ_MSYS_DEFINITION
 */
typedef struct _MSYS_Type {
    uint32_t magicID;            /**< Magic ID containing "MSYS"   */
    uint32_t count;              /**< Number of valid components in
                                                the media system */
    struct {
        uint64_t compID;         /**< Component ID as specified in component.h  */
        uint32_t numConfigs;     /**< Number of valid configurations for compID */
        uint32_t config[MSYS_MAX_NUM_CONFIGS];/**< Config IDs for the component  */
        struct {
            uint16_t srcCompIdx;   /**< Source component index to which input port
                                        is connected       */
            uint16_t srcPortIdx;   /**< Source component port index to which input
                                        port is connected    */
        } portMap[MSYS_IN_PORT_IDX_MAX];  /**< Port map for all input ports to which
                                        source component they are connected */
    } component[MSYS_MAX_LENGTH]; /**< Component array */
} MSYS_Type;

/** @brief Callback from framework to retrieve media system information

    This API is invoked by the framework to retrieve the media system
    connection information.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIdx    Unused

    @return Currently active MSYS_Type pointer

    @post None

    @trace #BRCM_SWREQ_MSYS_DEFINITION

    @limitations None
*/
const MSYS_Type* MSYS_Get(uint32_t aIdx);

/** @brief API to initialize a set of connected components logically

    This API is invoked by the application to initialize a set of connected
    components logically denoting as a media system

    @behavior Sync, Non-reentrant

    @pre Media System is in Reset State

    @param[in]      aSystem       Pointer to Media System definition

    Return values are documented in reverse-chronological order
    @return  #BCM_ERR_OK              Media System initialized successfully
    @return  #BCM_ERR_UNKNOWN         Initialization of any of the components of
                                      the media system failed
    @return  #BCM_ERR_INVAL_STATE     If any of the components of the Media
                                      System are not in Reset state
    @return  #BCM_ERR_INVAL_PARAMS    aSystem has invalid component
    @return  #BCM_ERR_INVAL_PARAMS    aSystem's count is invalid (0 or more
                                      than MAX_MEDIA_SYSTEM_LENGTH)
    @return  #BCM_ERR_INVAL_PARAMS    aSystem has invalid magic
    @return  #BCM_ERR_INVAL_PARAMS    aSystem is NULL

    @post Media System is moved to Init State upon success

    @trace #BRCM_SWREQ_MSYS

    @limitations None
*/
int32_t MSYS_Init(const MSYS_Type* const aSystem);

/** @brief API to configure a set of connected components logically

    This API is invoked by the application to configure a media system.
    <BR>The components of the media system should be in either the initialized
    or stopped state.

    @behavior Sync, Non-reentrant

    @pre Media System is in Init State

    @param[in]      aSystem       Pointer to Media System definition

    Return values are documented in reverse-chronological order
    @return  #BCM_ERR_OK           Pointer to configuration list
    @return  #BCM_ERR_UNKNOWN      Configuration application on one of the
                                   components of the media system failed
    @return  #BCM_ERR_INVAL_STATE  If any of the components of the Media
                                   System are in deinitialized, or started
                                   state
    @return  #BCM_ERR_INVAL_PARAMS aConfigs has invalid component or config ID
    @return  #BCM_ERR_INVAL_PARAMS aConfigs has invalid count (0 or greater
                                   than MAX_NUM_MEDIA_SYSTEM_CONFIGS)
    @return  #BCM_ERR_INVAL_PARAMS Configs has invalid magic
    @return  #BCM_ERR_INVAL_PARAMS aConfigs is NULL

    @post Media System is moved to Ready state

    @trace #BRCM_SWREQ_MSYS

    @limitations None
*/
int32_t MSYS_Configure(const MSYS_Type* const aSystem);

/** @brief API to start a set of connected components logically

    This API is invoked by the application to start media system
    streaming/processing

    @behavior Sync, Non-reentrant

    @pre Media System is in Ready State

    @param[in]      aSystem       Pointer to Media System definition

    Return values are documented in reverse-chronological order
    @return  #BCM_ERR_OK           Media System started media
                                   processing/streaming successfully
    @return  #BCM_ERR_UNKNOWN      Media system failed to start
                                   streaming/processing
    @return  #BCM_ERR_INVAL_STATE  If any of the components of the Media
                                   System are not in either configured or
                                   stopped states
    @return  #BCM_ERR_INVAL_PARAMS aSystem has invalid component
    @return  #BCM_ERR_INVAL_PARAMS aSystem's count is invalid (0 or more than
                                   MAX_MEDIA_SYSTEM_LENGTH)
    @return  #BCM_ERR_INVAL_PARAMS aSystem has invalid magic
    @return  #BCM_ERR_INVAL_PARAMS aSystem is NULL

    @post Media System is moved to Running State and is processing data
        (depending on source component is getting data from appropriate
         external source)

    @trace #BRCM_SWREQ_MSYS

    @limitations None
*/
int32_t MSYS_Start(const MSYS_Type* const aSystem);

/** @brief API to stop media system processing/streaming

    This API is invoked by the application to stop media system
    streaming/processing

    @behavior Sync, Non-reentrant

    @pre Media System is in streaming/running state

    @param[in]      aSystem       Pointer to Media System definition

    Return values are documented in reverse-chronological order
    @return #BCM_ERR_OK           Media System stopped media
                                  processing/streaming successfully
    @return #BCM_ERR_UNKNOWN      Media system failed to stop
                                  streaming/processing
    @return #BCM_ERR_INVAL_STATE  If any of the components of the Media
                                  System are not in started state
    @return #BCM_ERR_INVAL_PARAMS aSystem has invalid component
    @return #BCM_ERR_INVAL_PARAMS aSystem's count is invalid (0 or more than
                                  MAX_MEDIA_SYSTEM_LENGTH)
    @return #BCM_ERR_INVAL_PARAMS aSystem has invalid magic
    @return #BCM_ERR_INVAL_PARAMS aSystem is NULL

    @post Media System is moved to Ready State

    @trace #BRCM_SWREQ_MSYS

    @limitations None
*/
int32_t MSYS_Stop(const MSYS_Type* const aSystem);

/** @brief API to De-initialize a set of connected components logically

    This API is invoked by the application to De-initialize a set of connected
    components logically denoting as a media system

    @behavior Sync, Non-reentrant

    @pre Media System is in Ready/Init State

    @param[in]      aSystem       Pointer to Media System definition

    Return values are documented in reverse-chronological order
    @return #BCM_ERR_OK           Media System Deinitialized successfully
    @return  #BCM_ERR_UNKNOWN     De-initialization of any of the components
                                  of the media system failed
    @return #BCM_ERR_INVAL_STATE  If any of the components of Media System
                                  are in Started or De-initialized state
    @return #BCM_ERR_INVAL_PARAMS aSystem has invalid component
    @return #BCM_ERR_INVAL_PARAMS aSystem's count is invalid (0 or more than
                                  MAX_MEDIA_SYSTEM_LENGTH)
    @return #BCM_ERR_INVAL_PARAMS aSystem has invalid magic
    @return #BCM_ERR_INVAL_PARAMS aSystem is NULL

    @post Media System is in Reset State

    @trace #BRCM_SWREQ_MSYS

    @limitations None
*/
int32_t MSYS_DeInit(const MSYS_Type* const aSystem);

/** @brief API to get #MSYS_PortStatsType for input port

    This API is invoked by the application to retrieve statistics for
    component's input port

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre Media System is in Init State

    @param[in]   aCompId             Component ID
    @param[in]   aInPortIdx          Input port index
    @param[out]  aStats              Pointer to stats to be filled

    Return values are documented in reverse-chronological order
    @retval      #BCM_ERR_OK            Statistics retrieved successfully
    @retval      #BCM_ERR_UNKNOWN       Statistics retrieval failed
    @retval      #BCM_ERR_INVAL_PARAMS  aInPortIdx is invalid
    @retval      #BCM_ERR_INVAL_PARAMS  aStats is NULL

    @post None

    @trace #BRCM_SWREQ_MSYS_STATISTICS

    @limitations None
*/
int32_t MSYS_GetInStats(const uint64_t aCompId,
                        const uint32_t aInPortIdx,
                        MSYS_PortStatsType* const aStats);

/** @brief API called by #WDS_Monitor to report errors from Media components

    When Media components enable participation in WDS monitoring,
    errors detected by @ref WDS_Monitor will be notified to application
    through this API.
    Media components should install this function as the callback in their
    supervision entities (WDS_SEType structures defined by media component).
    Application shall implement this API to handle errors reported by media
    components. Handling could include re-initializing the media pipeline, or further
    notifying to the System for a reset request (again through WDS SE created by application)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aName              @ref WDS_CbType
    @param[in]   aError             @ref WDS_CbType
    @param[in]   aMonitor           @ref WDS_CbType
    @param[in]   aMaxAllowedErrors  @ref WDS_CbType

    @return void

    @post None

    @trace #BRCM_SWREQ_MSYS_WDS_CALLBACK

    @limitations None
*/
void MSYS_WdsErrorCb(const char* aName, int32_t aError,
                            const WDS_MonitorType *aMonitor, uint32_t aMaxAllowedErrors);

#endif /* MEDIA_SYSTEM_H */

/** @} */
