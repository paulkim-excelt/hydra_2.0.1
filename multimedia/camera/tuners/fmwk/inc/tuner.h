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
#ifndef TUNER_H
#define TUNER_H

/******************************************************************************
  File Name:  tuner.h
Description: This file exposes Tuner interfaces.
 ******************************************************************************/

/* Includes */
#include <isys.h>
#include <tuner_module.h>

/* Tuner State */
typedef uint32_t Tuner_State;
#define TUNER_STATE_RESET         (0UL)
#define TUNER_STATE_INITIALISED   (1UL)
#define TUNER_STATE_READY         (2UL)
#define TUNER_STATE_RUNNING       (3UL)
#define TUNER_STATE_ERROR         (4UL)

/**
    @name Tuner_EventType
    @{
    @brief Tuner Event types
*/
typedef uint32_t Tuner_EventType;                  /**< @brief Tuner Event type */
#define TUNER_EVENT_SENSOR_META_UPDATE  (0x1000UL) /**< @brief Sensor metadata update for Tuner */
#define TUNER_EVENT_SENSOR_STATS_UPDATE (0x2000UL) /**< @brief Sensor statistics update for Tuner */
#define TUNER_EVENT_ISP_STATS_UPDATE    (0x4000UL) /**< @brief ISP statistics update for Tuner */
/** @} */

/******************************************************************************
  FUNCTION NAME: Tuner_Init

ARGUMENTS: None

DESCRIPTION: This interface initialises Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_VERSION
BCM_ERR_UNKNOWN

 ******************************************************************************/
extern int32_t Tuner_Init(void);


/******************************************************************************
  FUNCTION NAME: Tuner_Deinit

ARGUMENTS: None

DESCRIPTION: This interface deinitialises Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_UNKNOWN

 ******************************************************************************/
extern int32_t Tuner_Deinit(void);


/******************************************************************************
  FUNCTION NAME: Tuner_Start

ARGUMENTS: None

DESCRIPTION: This interface starts Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE

 ******************************************************************************/
extern int32_t Tuner_Start(void);


/******************************************************************************
  FUNCTION NAME: Tuner_Stop
ARGUMENTS: None

DESCRIPTION: This interface stops Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_UNKNOWN

 ******************************************************************************/
extern int32_t Tuner_Stop(void);


/******************************************************************************
  FUNCTION NAME: Tuner_Configure

ARGUMENTS:
Tuner_Image* -> Pointer to config struct input

DESCRIPTION: This interface sets a static configuration to Tuner.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_INVAL_PARAMS

 ******************************************************************************/
extern int32_t Tuner_Configure(const Tuner_Image *aCfg);


/******************************************************************************
  FUNCTION NAME: Tuner_ProcessEvent

ARGUMENTS:
Tuner_EventType -> event type.
uint32_t        -> Index in case of PROCESS event.

DESCRIPTION: This interface process tuner Events.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_STATE
BCM_ERR_INVAL_PARAMS
BCM_ERR_UNKNOWN

 ******************************************************************************/
extern int32_t Tuner_ProcessEvent(Tuner_EventType aEvent, uint32_t aIndex);


/******************************************************************************
  FUNCTION NAME: Tuner_GetState

ARGUMENTS: None

DESCRIPTION: This interface gets present tuner State.

RETURNS:
Tuner State

 ******************************************************************************/
extern Tuner_State Tuner_GetState(void);


/******************************************************************************
  FUNCTION NAME: Tuner_GetIDX

ARGUMENTS:
aConfigType -> Config type for tuner.

DESCRIPTION: This interface gets tuner index.

RETURNS:
Tuner Index.

 ******************************************************************************/
uint32_t Tuner_GetIDX(uint32_t aConfigType);

#endif /* TUNER_H */
