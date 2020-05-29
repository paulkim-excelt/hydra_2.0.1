/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_wleth Wireless Ethernet Controller interface
    @ingroup grp_wireless

    @addtogroup grp_wleth
    @{
    @section sec_wleth_overview Overview
    @brief Wireless Ethernet interface

    Wireless ethernet controller interface provides APIs to initialize
    the controller, set mode (on/off) of the controller (#WLETH_SetMode()),
    Set the mac address of the wireless chip (#WLETH_SetMacAddr()) and
    send ethernet packet over the air (#WLETH_Send())

    @file wl_eth.h
    This header file contains the interface functions for Wireless Ethernet

    @version 1.26 Imported from docx
*/

#ifndef ETH_WL_H
#define ETH_WL_H

#include <stddef.h>
#include <inttypes.h>
#include <ethernet.h>
#include <eth_xcvr.h>

/**
    @name Wireless Ethernet  Interface IDs
    @{
    @brief Interface IDs for Wireless Ethernet
*/
#define BRCM_SWARCH_WLETH_CFG_TYPE                  (0x00U) /**< @brief #WLETH_CfgType */
#define BRCM_SWARCH_WLETH_STATS_TYPE                (0x01U) /**< @brief #WLETH_StatsType */
#define BRCM_SWARCH_WLETH_INIT_PROC                 (0x02U) /**< @brief #WLETH_Init */
#define BRCM_SWARCH_WLETH_SETMODE_PROC              (0x03U) /**< @brief #WLETH_SetMode */
#define BRCM_SWARCH_WLETH_GETMODE_PROC              (0x04U) /**< @brief #WLETH_GetMode */
#define BRCM_SWARCH_WLETH_SETMACADDR_PROC           (0x05U) /**< @brief #WLETH_SetMacAddr */
#define BRCM_SWARCH_WLETH_GETMACADDR_PROC           (0x06U) /**< @brief #WLETH_GetMacAddr */
#define BRCM_SWARCH_WLETH_GETTXBUFFER_PROC          (0x07U) /**< @brief #WLETH_GetTxBuffer */
#define BRCM_SWARCH_WLETH_SEND_PROC                 (0x08U) /**< @brief #WLETH_Send */
#define BRCM_SWARCH_WLETH_GET_STATS_PROC            (0x09U) /**< @brief #WLETH_GetStats */
/** @} */

/**
    @brief Wireless Ethernet configuration
*/
typedef struct {
    ETHER_HwIDType hwID;    /**< @brief Controller index */
    const uint8_t *macAddr; /**< @brief MAC address in network byte order */
} WLETH_CfgType;

/**
    @brief Wireless Ethernet RX packet statistics
 */
typedef struct {
    uint32_t    txFrag;         /**< @brief Transmitted fragment count */
    uint32_t    txMulti;        /**< @brief Multicast trasmitted fragment count */
    uint32_t    txFail;         /**< @brief Transmitted frame count */
    uint32_t    txRetry;        /**< @brief Retry count */
    uint32_t    txMultRetry;    /**< @brief Multiple retry count */
    uint32_t    rxDuplicate;    /**< @brief Frame duplicate count */
    uint32_t    rtsSuccess;     /**< @brief RTS success count */
    uint32_t    rtsFail;        /**< @brief RTS fail count */
    uint32_t    txNoACK;        /**< @brief ACK failure count */
    uint32_t    rxFrag;         /**< @brief Receive fragment count */
    uint32_t    rxMulti;        /**< @brief Multicast Receive fragment count */
    uint32_t    rxCRC;          /**< @brief FCS Errors */
} WLETH_StatsType;

/** @brief Initialize wireless controller

    This API initializes wireless ethernet
    On initialisation, controller will be set ETHER_MODE_DOWN. To
    make controller mode to ETHER_MODE_ACTIVE, explicit WLETH_SetMode()
    call is required with ETHER_MODE_ACTIVE mode.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]      aID         Index of the controller
    @param[in]      aConfig     Pointer to ethernet configurations

    @return     void

    @post None

    @limitations None
*/
extern void WLETH_Init(ETHER_HwIDType aID, const WLETH_CfgType *const aConfig);

/** @brief Set controller mode

    This API sets the controller mode. This is an asynchronous API.
    client will receive the callback notification (#WLETH_ModeChangeInd())
    when controller mode is changed

    @behavior Async, non-reentrant

    @pre None

    @param[in]       aID                     Wireless Ethernet controller ID
    @param[in]       aMode                   Mode to be set

    Return values are documented in reverse-chronological order
    @return          #BCM_ERR_OK              Request queued successfully
    @return          #BCM_ERR_INVAL_PARAMS    Invalid controller ID
    @return          #BCM_ERR_INVAL_PARAMS    Invalid controller mode
    @retval          #BCM_ERR_UNINIT           is not initialised.

    @post None

    @limitations None
*/
extern int32_t WLETH_SetMode(ETHER_HwIDType aID,
                             ETHER_ModeType aMode);

/** @brief Get controller mode

    This API gets the current controller mode.

    @behavior Async, non-reentrant

    @pre None

    @param[in]       aID                     Wireless Ethernet controller ID
    @param[out]      aMode                   Pointer to retrieve mode

    Return values are documented in reverse-chronological order
    @return          #BCM_ERR_OK              Mode retrieved successfully
    @return          #BCM_ERR_INVAL_PARAMS    Invalid controller ID
    @return          #BCM_ERR_INVAL_PARAMS    aMode is NULL
    @return          #BCM_ERR_UNINIT          Ethernet is not initialized

    @post None

    @limitations None
*/
extern int32_t WLETH_GetMode(ETHER_HwIDType aID,
                             ETHER_ModeType *const aMode);

/** @brief Set MAC address

    This API Sets the MAC address of the given Wireless Ethernet controller.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aID                     Wireless Ethernet controller ID
    @param[in]       aMacAddr                Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              MAC address is set successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid Wireless Ethernet controller ID
    @retval          #BCM_ERR_INVAL_PARAMS    aMacAddr is NULL
    @retval          #BCM_ERR_UNINIT          Ethernet driver is not initialised.

    @post None

    @limitations None
*/
extern int32_t WLETH_SetMacAddr(ETHER_HwIDType aID,
                            const uint8_t *const aMacAddr);

/** @brief Get MAC address

    This API get the MAC address of the wireless ethernet controller.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aID                     Wireless Ethernet controller ID
    @param[out]      aMacAddr                Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              MAC address is retrieved successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid Wireless Ethernet controller ID
    @retval          #BCM_ERR_INVAL_PARAMS    aMacAddr is NULL
    @retval          #BCM_ERR_UNINIT          Ethernet driver is not initialized

    @post None

    @limitations None
 */
extern int32_t WLETH_GetMacAddr(ETHER_HwIDType aID,
                                uint8_t *const aMacAddr);

/** @brief Get transmit buffer

    This API provides a Transmit buffer.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aID                     Wireless Ethernet controller ID
    @param[in]       aPriority               Priority of TX packet
    @param[out]      aBufIdx                 Buffer index
    @param[out]      aBuf                    Pointer to TX buffer
    @param[inout]    aLenInout               Length of the requested buffer (in),
                                             Length of the allocated buffer (out)

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              TX buffer successfully obtained
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid Wireless Ethernet controller ID
    @retval          #BCM_ERR_INVAL_PARAMS    aBufIdx is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aBuf is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aLenInOut is NULL
    @retval          #BCM_ERR_INVAL_PARAMS    aPriority > ETHER_PRIO_MAX
    @retval          #BCM_ERR_NOMEM           No TX buffers are available
    @retval          #BCM_ERR_UNINIT          Controller is not initialized

    @post None

    @limitations None
 */
extern int32_t WLETH_GetTxBuffer(ETHER_HwIDType aID,
                                 ETHER_PrioType aPriority,
                                 uint32_t *const aBufIdx,
                                 uint8_t **const aBuf,
                                 uint32_t *const aLenInOut);

/** @brief Queue an Ethernet packet for sending

    This API queues an Ethernet packet to driver for
    transmission over wireless

    @behavior Async, Re-entrant

    @pre None

    @param[in]       aID                     Wireless Ethernet controller ID
    @param[in]       aTxConfirmation         TX confirmation flag
    @param[in]       aBufIdx                 Buffer index (received by calling
                                             WLETH_GetTxBuffer())
    @param[in]       aLen                    Length of the data to be sent in bytes
    @param[in]       aDestMacAddr            Pointer to destination MAC address

    Return values are documented in reverse-chronological order
    @retval          #BCM_ERR_OK              Packet is queued successfully
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid Wireless Ethernet controller ID
    @retval          #BCM_ERR_INVAL_PARAMS    Invalid buffer index
    @retval          #BCM_ERR_INVAL_PARAMS    aLen is more than Ethernet frame size
    @retval          #BCM_ERR_INVAL_BUF_STATE Invalid packet buffer state
    @retval          #BCM_ERR_INVAL_STATE     Invalid controller state
    @retval          #BCM_ERR_UNINIT          driver is not initialized

    @post None

    @limitations None
*/
extern int32_t WLETH_Send(ETHER_HwIDType aID,
                          uint32_t aTxConfirmation,
                          uint32_t aBufIdx,
                          uint32_t aLen,
                          const uint8_t *const aDestMacAddr);

/** @brief Get Wireless Ethernet packet statistics

    This API retrieves the wirelss ethernet packet stats.
    This is asynchronous API. This API just queues the request
    to the driver. Client will receive callback notfication
    when stats are read from the hardware

    @behavior Async, Re-entrant

    @pre None

    @param[in]       aID                     Wireless Ethernet controller ID
    @param[out]      aStats                  Pointer to WLETH_StatsType structure

    @retval          #BCM_ERR_OK            Request Queued successfully
    @retval          #BCM_ERR_INVAL_PARAMS  Invalid Wireless Ethernet controller ID
    @retval          #BCM_ERR_UNINIT         is not initialized

    @post None

    @limitations None
 */
extern int32_t WLETH_GetStats(ETHER_HwIDType aID);

/** @brief Get Ethernet time

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[out]     aTS                 Current Ethernet time
    @param[out]     aTSQual             Quality of the retrieved time

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_GetTime(ETHER_HwIDType aID,
                             ETHER_TimestampType* const aTS,
                             ETHER_TimestampQualType* const aTSQual);

/** @brief Get packet Rx timestamp

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aPktBuf             Pointer to the received packet buffer
    @param[out]     aTS                 Timestamp captured at the instant the
                                        packet was received. This is derived
                                        from the Ethernet time base.
    @param[out]     aTSQual             Quality of the timestamp. Application
                                        may chose to ignore the timestamps which
                                        do not have quality value #ETHER_TIMESTAMPQUAL_VALID.

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_GetRxTimestamp(ETHER_HwIDType aID, const uint8_t* const aPktBuf,
                                    ETHER_TimestampType* const aTS,
                                    ETHER_TimestampQualType* const aTSQual);

/** @brief Get packet Tx timestamp

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aBufIdx             Buffer index (received in the transmit
                                        confirmation)
    @param[out]     aTS                 Timestamp captured at the instant the
                                        packet was transmitted. This is derived
                                        from the Ethernet time base.
    @param[out]     aTSQual             Quality of the timestamp. Application
                                        may chose to ignore the timestamps which
                                        do not have quality value #ETHER_TIMESTAMPQUAL_VALID.

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_GetTxTimestamp(ETHER_HwIDType aID, uint32_t aBufIdx,
                                    ETHER_TimestampType* const aTS,
                                    ETHER_TimestampQualType* const aTSQual);

/** @brief Get SQQI value time

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID         Index of the transceiver
    @param[out]     aSQIValue   SQI value of the link

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_XCVR_GetSQIValue(ETHXCVR_IDType aID,
                                      uint32_t *const aSQIValue);

/** @brief Enable Tx timestamping

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aBufIdx             Buffer index

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_EnableTxTimestamp(ETHER_HwIDType aID, uint32_t aBufIdx);

/** @brief Set Ethernet time

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTS                 New Ethernet time to be set

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_SetTime(ETHER_HwIDType aID,
                             const ETHER_TimestampType* const aTS);

/** @brief Set Ethernet correction time

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTD                 Time difference between time-master
                                        time and local Ethernet time
    @param[in]      aRR                 Rate ratio between the time master and
                                        local clock

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_SetCorrectionTime(ETHER_HwIDType aID,
                                       const ETHER_TimestampDiffType* const aTD,
                                       const ETHER_RateRatioType*     const aRR);

/** @brief Set GP timer

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aPeriod             Timer period in nanoseconds

    @retval          #BCM_ERR_NOSUPPORT      Not supported

    @post None

    @limitations None
 */
extern int32_t WLETH_SetGPTimer(ETHER_HwIDType aID, uint32_t aPeriod);

#endif /* ETH_WL_H */
/** @} */

