/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_avce_il Integration Guide
    @ingroup grp_avce

    @addtogroup grp_avce_il
    @{
    @section sec_avce_integration Integration Guide
    This page describes functions needed by an integrator. The interfaces
    need to be implemented by the integrator.

    @file avce_osil.h
    @brief AVC Encoder library integration interfaces.
    This header file defines the integration interfaces for AVC Encoder library.
    @version 1.4 Imported from MS Word document
*/

#ifndef AVCE_OSIL_H
#define AVCE_OSIL_H

#include <bcm_err.h>
#include <stdint.h>
#include <system.h>

/**
    @name AVCE Integration IDs
    @{
    @brief Integration IDs for AVCE Library
*/
#define BRCM_ARCH_AVCELIB_IL_GETCOREBUF_PROC   (0x80UL)    /**< @brief #AVCELib_GetCoreContextBuf    */
#define BRCM_ARCH_AVCELIB_IL_GETBSBUF_PROC     (0x81UL)    /**< @brief #AVCELib_GetTempBsBuf   */
/** @}*/

/**
    @brief Bit stream buffer

    @trace #BRCM_ARCH_AVCE_IL_BS_BUFFER  #BRCM_REQ_AVCE_MEMORY
 */
typedef struct _AVCELib_TempBsBuffer {
    uint32_t idx;         /**< Buffer Index */
    uint32_t size;        /**< Buffer Size */
    uint8_t* buffer;      /**< Buffer Address */
} AVCELib_TempBsBuffer;

/** @brief Get Client ID to signal

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIndex      Core Index for which the Client ID is needed

    @retval     PPDrv_Client        Client ID of core index (aIndex)

    @post None

    @note To be used by Integrator

    @trace  #BRCM_ARCH_PPLIB_IL_GETCLIENT_PROC  #BRCM_REQ_PPLIB_PRIVILEGED

    @limitations None

    @snippet pplib_osil.c Usage of PPDrv_SetEvents
*/
uint8_t* AVCELib_GetCoreContextBuf(const uint32_t aCoreIndex);

/** @brief Get index of core

    This API gets the index of the core to which aCoreID belongs to.
    This index can be used to set/update a Task ID in TaskArray

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCoreID     Core ID of v-MP for which Index needs to be found

    @retval     uint32_t    PPDRV_INVALID_IDX if aCoreID is invalid
                            0-31 (index of the core corresponding to the given code ID)

    @post None

    @note To be used by Integrator

    @trace  #BRCM_ARCH_PPLIB_IL_GETCORE_PROC  #BRCM_REQ_PPLIB_PRIVILEGED

    @limitations None

    @snippet pplib_osil.c Usage of PPDrv_SetEvents
*/
void AVCELib_GetTempBsBuf(const uint32_t aBufIndex,AVCELib_TempBsBuffer* const aTempBsBuf);

#endif /* AVCE_OSIL_H */
/** @} */
