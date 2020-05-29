/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_csn_board Board specific sensor Interfaces
    @ingroup grp_csn

    @addtogroup grp_csn_board
    @{
    @section sec_csn_board_overview Overview
    @todo Add overview

    @file camera_sensor_board.h
    @brief CSN Board specific sensor interfaces
    This source file contains the interfaces for Board specific CSN APIs.
    @version 0.1 Initial version
*/

#ifndef CAMERA_SENSOR_BOARD_H
#define CAMERA_SENSOR_BOARD_H

#include <stdint.h>
#include <bcm_err.h>

/**
    @name CSN Board Specific Architecture IDs
    @{
    @brief Architecture IDs for CSN-Board
*/
#define BRCM_SWARCH_CSN_BOARD_RESET_REQ_PROC             (0x00U)   /**< @brief #CSN_BoardResetReq           */
#define BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC         (0x01U)   /**< @brief #CSN_BoardResetHandler       */
#define BRCM_SWARCH_CSN_BOARD_ENABLE_SAFE_STATE_CB_PROC  (0x02U)   /**< @brief #CSN_BoardEnableSafeStateCb  */
#define BRCM_SWARCH_CSN_BOARD_DISABLE_SAFE_STATE_CB_PROC (0x03U)   /**< @brief #CSN_BoardDisableSafeStateCb */
/** @} */

/** @brief Request for Camera Sensor reset

    Reset camera sensor using hardware signal, so that sensor recovers from
    any bad/error state.

    @behavior Async

    @pre None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Reset is successful
    @retval     #BCM_ERR_UNKNOWN        Reset failed
    @retval     #BCM_ERR_BUSY           Reset is in Progress

    @post Camera sensor is reset/initialised.

    @note To be implemented as part of Board specific module

    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY

    @limitations None
*/
extern int32_t CSN_BoardResetReq(void);

/** @brief Handler for Camera Sensor Reset

    Main handler for any event once the camera sensor reset request is
    successfully submitted.

    @behavior Sync

    @pre CSN_BoardResetReq is submitted

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Reset is successful
    @retval     #BCM_ERR_UNKNOWN        Reset failed
    @retval     #BCM_ERR_BUSY           Reset is in Progress

    @post None

    @note To be implemented as part of Board specific module

    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY

    @limitations None
*/
extern int32_t CSN_BoardResetHandler(void);

/** @brief Enable safe-state handler

    This API enables safe-state handler.

    @behavior Sync

    @pre None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Enable successful
    @retval     #BCM_ERR_UNKNOWN        Enable failed

    @post None

    @note To be implemented as part of Board specific module

    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY

    @limitations None
*/
extern int32_t CSN_BoardEnableSafeStateCb(void);

/** @brief Disable Safe State Handler

    This API disables safe-state handler.

    @behavior Sync

    @pre None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Disable successful
    @retval     #BCM_ERR_UNKNOWN        Disable failed

    @post None

    @note To be implemented as part of Board specific module

    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY

    @limitations None
*/
extern int32_t CSN_BoardDisableSafeStateCb(void);

#endif /* CAMERA_SENSOR_BOARD_H */
/** @} */
