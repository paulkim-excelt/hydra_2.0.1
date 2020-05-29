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
/**
    @defgroup grp_wleth_sdio_il Wireless ethernet SDIO integration layer
    @ingroup grp_wireless

    @addtogroup grp_wleth_sdio_il
    @{

    @file wl_sdio_osil.h
    @brief SDIO integration layer for wireless ethernet
    This header file contains the integration function for SDIO

    @version 1.26 Imported from docx
*/

#ifndef WL_SDIO_OSLI_H
#define WL_SDIO_OSLI_H

#include <inttypes.h>
#include <wl_eth.h>
#include <wl_eth_xcvr.h>
#include <sd.h>

/**
    @name Wireless Ethernet Driver integration Interface IDs
    @{
    @brief Interface IDs for Wireless Ethernet Driver integration layer
*/
#define BRCM_SWARCH_WLETH_SDIOOPDONEIND_PROC            (0x50U) /**< @brief #WLETH_SdioOpDoneInd */
#define BRCM_SWARCH_WLETH_SDIOCARDINTRIND_PROC          (0x51U) /**< @brief #WLETH_SdioCardIntrInd */
#define BRCM_SWARCH_WLETH_SDIOGETCLIENTID_PROC          (0x52U) /**< @brief #WLETH_SdioGetClientID */
#define BRCM_SWARCH_WLETH_SDIOXFERCMPLTHANDLER_PROC     (0x53U) /**< @brief #WLETH_SdioXferCmpltHandler */
#define BRCM_SWARCH_WLETH_SDIOCARDINTRHANDLER_PROC      (0x54U) /**< @brief #WLETH_SdioCardIntrHandler */

/** @brief SDIO transfer complete callback

    This is callback function from the SDIO layer when a requested transfer
    is completed. It is expected that this function will signal wlan task
    and will call WLETH_SdioXferCmpltHandler() function in wlan task context
    for the processing of the wlan driver state machine

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID                     SD slot index
    @param[in]       aClientID               SD Client ID
    @param[in]       aResult                 operation result

    @return     void

    @post None

    @limitations None
*/
extern void WLETH_SdioOpDoneInd(SD_SlotIDType aID,
                                SD_ClientIDType aClientID,
                                SD_OpResultType aResult);

/** @brief SDIO Card interrupt indication

    This is callback function from the SDIO layer when card interrupt
    is received. It is expected that this function will signal wlan task
    and will call WLETH_SdioCardIntrHandler() function in wlan task context for the
    processing the wlan driver state machine

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID                     SD slot ID index
    @param[in]       aClientID               SD Client ID

    @return          void

    @post None

    @limitations None
*/
extern void WLETH_SdioCardIntrInd(SD_SlotIDType aID, SD_ClientIDType aClientID);

/** @brief Get SD client ID for wireless card

    This API shall return the SD client ID registered with SD driver
    for wlan card

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID                     SD slot ID index
    @param[out]      aClientID               SD Client ID

    @return          void

    @post None

    @limitations None
*/
extern int32_t WLETH_SdioGetClientID(SD_SlotIDType aID,
                                     SD_ClientIDType *const aClientID);

/** @brief SDIO transfer completion handler

    This function is implemented by wireless ethernet driver
    and shall be called in wlan task context to process the driver
    state machine

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID            SD slot ID
    @param[in]       aResult        Result of the last transfer operation

    @return          void

    @post None

    @limitations None
 */
extern void WLETH_SdioXferCmpltHandler(uint32_t aBusID, SD_OpResultType aResult);

/** @brief SDIO card interrupt indication

    This function is implemented by wlan driver. This function
    shall be called in task context when wlan card raises an
    card interrupt

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID                     SD slot ID

    @return          void

    @post None

    @limitations None
*/

extern void WLETH_SdioCardIntrHandler(uint32_t aBusID);
#endif /* WL_SDIO_OSLI_H */
/** @} */
