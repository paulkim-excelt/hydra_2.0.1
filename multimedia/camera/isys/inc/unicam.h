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
    @defgroup grp_unicam_ifc Interface
    @ingroup grp_unicam

    @addtogroup grp_unicam_ifc
    @{
    @section sec_unicam_overview Overview
    @todo Add overview

    @section sec_unicam_fsm State Machine
    @todo Add state machine

    @section sec_unicam_seq Sequence Diagrams
    @todo Add sequence diagram

    @section sec_unicam_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   TBD                                             |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   TBD                                             |

    @todo Update performance numbers
    @includedoc unicam.dep

    @limitations Add component limitations here

    @file unicam.h
    @brief UNICAM driver interface
    This header file contains the interface functions for UNICAM
    @version 0.1 Initial Version
*/

#ifndef UNICAM_H
#define UNICAM_H

#include <unicam_osil.h>

/**
    @name UNICAM API IDs
    @{
    @brief API IDs for UNICAM
*/
#define BRCM_SWARCH_UNICAM_DEFAULT_ALLOC_LINES_MACRO (0x0U)  /**< @brief #UNICAM_DEFAULT_ALLOC_LINES */
#define BRCM_SWARCH_UNICAM_INIT_PROC                 (0x1U)  /**< @brief #UNICAM_Init                */
#define BRCM_SWARCH_UNICAM_DEINIT_PROC               (0x2U)  /**< @brief #UNICAM_Deinit              */
#define BRCM_SWARCH_UNICAM_START_PROC                (0x3U)  /**< @brief #UNICAM_Start               */
#define BRCM_SWARCH_UNICAM_STOP_PROC                 (0x4U)  /**< @brief #UNICAM_Stop                */
#define BRCM_SWARCH_UNICAM_CONFIGURE_PROC            (0x5U)  /**< @brief #UNICAM_Configure           */
#define BRCM_SWARCH_UNICAM_GET_EVENT_PROC            (0x6U)  /**< @brief #UNICAM_GetEvent            */
#define BRCM_SWARCH_UNICAM_GET_CUR_BUF_IDX_PROC      (0x7U)  /**< @brief #UNICAM_GetCurBufIdx        */
#define BRCM_SWARCH_UNICAM_GET_EMB_BUFFER_PROC       (0x8U)  /**< @brief #UNICAM_GetEmbBuffer        */
#define BRCM_SWARCH_UNICAM_GET_EMB_BUF_PTR_PROC      (0x9U)  /**< @brief #UNICAM_GetEmbBufPtr        */
/** @} */

/**
    @brief Default number of lines to be allocated for UNICAM image

    @trace #BRCM_SWREQ_UNICAM_CONFIGURATION
*/
#define UNICAM_DEFAULT_ALLOC_LINES (5UL)

/** @brief Initialize UNICAM

    This API initializes UNICAM driver.

    @behavior Sync, Non-reentrant

    @pre This API shall only be called in the #UNICAM_STATE_POWEROFF state

    @param      void

    @retval     #BCM_ERR_OK             Initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully initialized, the UNICAM driver shall be in
          #UNICAM_STATE_INITIALISED state

    @trace  #BRCM_SWREQ_UNICAM_INIT

    @limitations None
*/
extern int32_t UNICAM_Init(void);

/** @brief De-initialize UNICAM

    This API de-initializes UNICAM driver.

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval     #BCM_ERR_OK             De-initialization is successful
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong  state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully de-initialized, the UNICAM driver shall be in
          #UNICAM_STATE_POWEROFF state

    @trace  #BRCM_SWREQ_UNICAM_INIT

    @limitations None
*/
extern int32_t UNICAM_Deinit(void);

/** @brief Enable UNICAM driver

    This enables all UNICAM driver in the appropriate order

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_STATE_READY while calling this API

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Start is successful
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Camera peripheral mode
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully started, UNICAM shall be in #UNICAM_STATE_POWEROFF state

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_Start(void);

/** @brief Disable UNICAM driver

    This API disables all driver in the appropriate order

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_STATE_STREAMING state while calling this API

    @param      void

    @retval     #BCM_ERR_OK             Stop is successful
    @retval     #BCM_ERR_TIME_OUT       UNICAM stop failed
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully started, UNICAM shall be in #UNICAM_STATE_READY state

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern int32_t UNICAM_Stop(void);

/** @brief Configure UNICAM driver

    This API configures UNICAM sub-component based on configuration type

    @behavior Sync, Non-reentrant

    @pre UNICAM shall be in #UNICAM_STATE_INITIALISED or #UNICAM_STATE_READY
         state while calling this API

    @param[in]      aP_Cfg       UNICAM sub-component configuration
    @param[in]      aIfcConfig   UNICAM common interface configuration
    @param[in]      aBuf         UNICAM buffer configuration


    @retval     #BCM_ERR_OK             Configuration is successful
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong data1 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong image1 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong data0 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong image0 address configured
    @retval     #BCM_ERR_INVAL_PARAMS   Insufficient configurations
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->blkCfg[i].offset
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->blkCfgCount
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->numDataLanes
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong aCfg->mode
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong cfgMagic
    @retval     #BCM_ERR_INVAL_PARAMS   Wrong cfgType
    @retval     #BCM_ERR_INVAL_PARAMS   aCfg is NULL
    @retval     #BCM_ERR_INVAL_STATE    UNICAM driver is in wrong state
    @retval     #BCM_ERR_UNKNOWN        SVC send command has failed

    @post Once successfully configured, UNICAM shall be in #UNICAM_STATE_READY state

    @trace  #BRCM_SWREQ_UNICAM_CONFIGURATION

    @limitations None
*/
extern int32_t UNICAM_Configure(const UNICAM_ImageType *aCfg,
                                UNICAM_CommonIfcConfigType *aIfcConfig,
                                UNICAM_BufParamType *aBuf);

/** @brief Get outstanding events in UNICAM driver

    This API retrieves outstanding events in UNICAM driver

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  void

    @retval uint32_t Mask of event's occurred

    @post None

    @trace  #BRCM_SWREQ_UNICAM

    @limitations None
*/
extern uint32_t UNICAM_GetEvent(void);

/** @brief Retrieve current buffer index

    @trace #BRCM_SWREQ_UNICAM
*/
extern uint32_t UNICAM_GetCurBufIdx(void);

/** @brief Retrieve latest embedded buffer

    @trace #BRCM_SWREQ_UNICAM
*/
extern int32_t UNICAM_GetEmbBuffer(UNICAM_CurED_ParamType *aEDParam);

/** @brief Retrieve latest embedded buffer pointer

    @trace #BRCM_SWREQ_UNICAM
*/
extern int32_t UNICAM_GetEmbBufPtr(UNICAM_CurED_ParamType *aEDParam);

#endif /* UNICAM_H */
/** @} */

