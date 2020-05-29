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

/** @defgroup grp_csn_il Integration Guide
    @ingroup grp_csn

    @addtogroup grp_csn_il
    @{
    @section sec_comp_integration Integration Guide
    @todo Add overview

    @section sec_csn_il_seq Sequence Diagram
    @todo Add sequence diagram

    @file camera_sensor_osil.h
    @brief CSN Integration Interfaces
    This header file contains the integration interface functions for CSN

    @version 0.1 Initial version
*/

#ifndef CAMERA_SENSOR_OSIL_H
#define CAMERA_SENSOR_OSIL_H

#include <stdint.h>
#include <i2c.h>

/**
    @name CSN Integration Interface IDs
    @{
    @brief Integration Interface IDs for CSN
*/
#define BRCM_SWARCH_CSN_CLIENT_IDX_MACRO     (0x81U) /**< @brief #CSN_ClientIDX       */
#define BRCM_SWARCH_CSN_STATUS_MACRO         (0x82U) /**< @brief #CSN_STATUS_NONE     */
#define BRCM_SWARCH_CSN_PROCESS_EVENT_PROC   (0x83U) /**< @brief #CSN_ProcessEvent    */
#define BRCM_SWARCH_CSN_SAFE_STATE_CB_PROC   (0x84U) /**< @brief #CSN_SafeStateCb     */
#define BRCM_SWARCH_CSN_EVENT_SOURCE_TYPE    (0x85U) /**< @brief #CSN_EventSourceType */
/** @} */

/**
    @name CS_ClientIDX
    @{
    @brief Client index for CSN

    @trace #BRCM_SWREQ_CSN_INIT
*/
typedef uint32_t CSN_ClientIDX;
#define CSN_CLIENT_IDX_CAMERA    (0UL) /**< @brief Index for Camera client                        */
#define CSN_CLIENT_IDX_TUNER     (1UL) /**< @brief Index for Tuner client                         */
#define CSN_CLIENT_IDX_MAX       (2UL) /**< @brief Maximum number of camera sensor driver clients */
/** @} */

/**
    @name CSN_EventSourceType
    @{
    @brief Event type

    @trace #BRCM_SWREQ_CSN_INIT
*/
typedef uint32_t CSN_EventSourceType;
#define CSN_EVENT_SOURCE_IIC    (0UL) /**< @brief Event from I2C            */
#define CSN_EVENT_SOURCE_SAFETY (1UL) /**< @brief Event from Safety handler */
#define CSN_EVENT_SOURCE_RESET  (2UL) /**< @brief Event from RESET module   */
/** @} */

/**
    @name CSN process event status
    @{
    @brief CSN Process event status

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
#define CSN_STATUS_NONE           (0x000UL) /**< @brief None             */
#define CSN_STATUS_CMD_COMPLETE   (0x100UL) /**< @brief Command complete */
#define CSN_STATUS_ERROR          (0x200UL) /**< @brief Error            */
#define CSN_STATUS_BUSY           (0x400UL) /**< @brief CSN is busy      */
/** @} */

/** @brief Process CSN events

    Process CSN events to be called from client.

    @behavior Sync

    @pre None

    @param[in]      aClient    Client ID as mentioned in #CS_ClientIDX
    @param[in]      aEventType Event type as mentioned in #CSN_EventType

    @retval uint32_t Masks as in #CSN_STATUS_NONE
                                 #CSN_STATUS_CMD_COMPLETE
                                 #CSN_STATUS_ERROR
                                 #CSN_STATUS_BUSY
    @post None

    @note To be used

    @trace #BRCM_SWREQ_CSN_CONFIGURATION

    @limitations None
*/
extern uint32_t CSN_ProcessEvent(uint32_t aClient,
                                 CSN_EventSourceType aEventType);

/** @brief Safe-state callback

    Safe-state movement notification from Camera sensor. This API shall be
    invoked by the board specific interrupt handler.

    @behavior Sync

    @pre None

    @param      void

    @return     void

    @post None

    @note To be implemented

    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY

    @limitations None
*/
extern void CSN_SafeStateCb(void);

#endif /* CAMERA_SENSOR_OSIL_H */
/** @} */
