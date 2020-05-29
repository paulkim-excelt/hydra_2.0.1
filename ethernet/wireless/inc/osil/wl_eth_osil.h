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
    @defgroup grp_wleth_il Wireless Ethernet integration layer
    @ingroup grp_wireless

    @addtogroup grp_wleth_il
    @{

    @file wl_eth_osil.h
    @brief Wireless Ethernet driver integration layer
    This header file contains the interface functions for Wireless
    ethernet driver integration layer

    @version 1.26 Imported from docx
*/


#ifndef WL_ETH_OSIL_H
#define WL_ETH_OSIL_H
#include <inttypes.h>
#include <system.h>
#include <ethernet.h>
#include <eth_xcvr.h>
#include <wl_eth.h>
#include <wl_eth_xcvr.h>

/**
    @name Wireless Ethernet Driver integration Interface IDs
    @{
    @brief Interface IDs for Wireless Ethernet Driver integration layer
*/
#define BRCM_SWARCH_WLETH_REQID_TYPE            (0x40U) /**< @brief #WLETH_ReqIDType */
#define BRCM_SWARCH_WLETH_REQ_TYPE              (0x41U) /**< @brief #WLETH_ReqType */
#define BRCM_SWARCH_WLETH_MODECHANGEIND_PROC    (0x42U) /**< @brief #WLETH_ModeChangeInd */
#define BRCM_SWARCH_WLETH_RXIND_PROC            (0x43U) /**< @brief #WLETH_RxInd */
#define BRCM_SWARCH_WLETH_TXDONEIND_PROC        (0x44U) /**< @brief #WLETH_TxDoneInd */
#define BRCM_SWARCH_WLXCVR_MODECHANGEIND_PROC   (0x45U) /**< @brief #WLXCVR_ModeChangeInd */
#define BRCM_SWARCH_WLXCVR_GETPARAMDONEIND_PROC (0x46U) /**< @brief #WLXCVR_GetParamInd */
#define BRCM_SWARCH_WLETH_REQUEST_PROC          (0x47U) /**< @brief #WLETH_Request */
#define BRCM_SWARCH_WLETH_REQHANDLER_PROC       (0x48U) /**< @brief #WLETH_ReqHandler */
#define BRCM_SWARCH_WLETH_PROCESSREQUEST_PROC   (0x49U) /**< @brief #WLETH_ProcessRequest */
#define BRCM_SWARCH_WLETH_GETFIRMWAREINFO_PROC  (0x4AU) /**< @brief #WLETH_GetFirmwareInfo */
#define BRCM_SWARCH_WLETH_GETNVRAMINFO_PROC     (0x4BU) /**< @brief #WLETH_GetNvramInfo */
/** @} */

/**
    @{
    @brief Wireless ethernet driver request IDs

*/
typedef uint32_t WLETH_ReqIDType;
#define WLETH_REQID_CNTRL_INIT                (1UL) /**< @brief controller init req id */
#define WLETH_REQID_CNTRL_SET_MODE            (2UL) /**< @brief controller set mode req id */
#define WLETH_REQID_CNTRL_GET_MODE            (3UL) /**< @brief controller get mode req id */
#define WLETH_REQID_CNTRL_SET_MAC_ADDR        (4UL) /**< @brief controller set mac addr req id */
#define WLETH_REQID_CNTRL_GET_MAC_ADDR        (5UL) /**< @brief controller get mac addr req id */
#define WLETH_REQID_CNTRL_GET_TX_BUFFER       (6UL) /**< @brief controller get tx buffer req id */
#define WLETH_REQID_CNTRL_GET_STATS           (7UL) /**< @brief controller get stats req id */
#define WLETH_REQID_CNTRL_SEND_ETH_PACKET     (8UL) /**< @brief controller send packet req id */
#define WLETH_REQID_XCVR_INIT                 (9UL) /**< @brief xcvr init req id */
#define WLETH_REQID_XCVR_SET_MODE             (10UL) /**< @brief xcvr set mode req id */
#define WLETH_REQID_XCVR_GET_MODE             (11UL) /**< @brief xcvr get mode req id */
#define WLETH_REQID_XCVR_GET_LINK_STATE       (12UL) /**< @brief xcvr get link state req id */
#define WLETH_REQID_XCVR_GET_PARAM            (13UL) /**< @brief xcvr get radio param req id */
/** @} */

/**
    @brief Wlan driver request structure

    Request structure filled by the driver for specific API call
    Refer #WLETH_Request() function for details
 */
typedef struct {
    WLETH_ReqIDType         reqID;              /**< @brief request ID */
    uint32_t                hwID;               /**< @brief hardware ID */
    WLXCVR_RadioIDType      radioID;            /**< @brief Xcvr Radio ID */
    ETHER_ModeType          cntrlMode;          /**< @brief controller mode for set/get */
    uint8_t                 destMacAddr[ETHER_MAC_ADDR_SIZE];   /**< @brief mac address */
    ETHER_PrioType          priority;       /**< @brief ethernet priority */
    uint32_t                bufIdx;         /**< @brief ethernet buffer index */
    uint32_t                len;            /**< @brief ethernet buffer lenght */
    uint32_t                txConfirmation; /**< @brief TxConfirmation flag */
    uint32_t                *bufIdxOut;     /**< @brief ethernet buffer index out */
    uint8_t                 *buf;           /**< @brief ethernet Buffer out */
    uint32_t                *lenOut;        /**< @brief Pointer to length of buffer */
    ETHXCVR_ModeType        xcvrMode;       /**< @brief transceiver mode */
    ETHXCVR_LinkStateType   linkState;      /**< @brief xcvr link state */
    WLXCVR_RadioParamIDType radioParam;    /**< @brief Tx params list */
    const WLXCVR_CfgType    *xcvrConfig;    /**< @brief xcvr configuration */
    const WLETH_CfgType     *cntrlConfig;   /**< @brief controller configuration */
    uint8_t                 *macAddr;       /**< @brief pointer to mac address buffer */
} WLETH_ReqType;

/** @brief Controller mode change indication

    This callback is raised by the wireless ethernet controller driver
    when mode change request completes

    @behavior Sync, non-reentrant

    @pre None

    @param[in]       aID                     Index of the controller
    @param[out]      aMode                   current new mode

    @return          void

    @post None

    @limitations None
*/
extern void WLETH_ModeChangeInd(ETHER_HwIDType aID,
                                ETHER_ModeType aMode);

/** @brief Wireless Ethernet RX packet indication callback

    This interface is called by the driver for every RX packet received.
    Depending on the ethtype of the packet
    received, the corresponding RX call back to the respective client
    shall be raised from this interface.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]      aID             Ethernet controller ID
    @param[in]      aBuf            Pointer to packet buffer (payload)
    @param[in]      aLen            Length of the packet
    @param[in]      aSrcMacAddr     Pointer to source MAC address
    @param[in]      aDestMacAddr    Pointer to destination MAC address

    @return         void

    @post None

    @limitations None
*/
extern void WLETH_RxInd(ETHER_HwIDType aID, const uint8_t *const aBuf,
                        uint32_t aLen, const uint8_t *const aSrcMacAddr,
                        const uint8_t *const aDestMacAddr);

/** @brief Ethernet TX packet send confirmation callback

    This interface is called by the wireless ethernet Driver to indicate
    transmission completion of a packet.
    Depending on the ethype of the transmitted packet, the corresponding
    TX callback to the respective client shall be
    raised from this interface.

    This interface is not invoked by the Ethernet driver if the packet was sent with
    aTxConfirmation set as #FALSE (refer to #WLETH_Send).

    @behavior Sync, non-reentrant

    @pre None

    @param[in]      aID             Ethernet controller ID
    @param[in]      aBufIdx         Packet buffer index
    @param[in]      aResult         Status of packet transmission

    @return     void

    @post None

    @limitations None
*/
extern void WLETH_TxDoneInd(ETHER_HwIDType aID, uint32_t aBufIdx,
                            ETHER_ResultType aResult);

/** @brief Transceiver mode change indication

    This callback is raised by the wireless ethernet transceiver driver
    when xcvr mode change completes

    @behavior Sync, non-reentrant

    @pre None

    @param[in]       aID        Index of the transceiver
    @param[in]       aMode      New mode of the transceiver

    @return     void

    @post None

    @limitations None
*/
extern void WLXCVR_ModeChangeInd(ETHXCVR_IDType aID,
        ETHXCVR_ModeType aMode);

/**
    @brief Transceiver Get parameter done indication

    This callback is raised to client when radio
    parameter is read from the hardware (in response
    to #WLXCVR_GetRadioParamReq())

    @behavior Sync, non-reentrant

    @pre None

    @param[in]       aID       Index of the transceiver
    @param[in]       aResult   error code (paramete value
                               aValue is valid only when
                               aResult is set to #BCM_ERR_OK)
    @param[in]       aValue    Parameter value

    @return     void

    @post None

    @limitations None
*/

extern void WLXCVR_GetParamInd(ETHXCVR_IDType aID,
                                  int32_t aResult,
                                  WLXCVR_RadioParamIDType aParam,
                                  WLXCVR_RadioParamValType aValue);

/**
    @brief Stats available indication

    This callback is raised to client when wifi
    packet stats are available from hardware (in response
    to #WLETH_GetStats() request)

    @behavior Sync, non-reentrant

    @pre None

    @param[in]       aID       Index of the transceiver
    @param[in]       aResult   error code (aStats
                               is valid only when
                               aResult is set to #BCM_ERR_OK)
    @param[in]       aStats    Statistics

    @return     void

    @post None

    @limitations None
*/

extern void WLETH_GetStatsInd(ETHER_HwIDType aID,
                             int32_t aResult,
                             const WLETH_StatsType *const aStats);

/** @brief Get wlan Firmware information

    This API shall return the address of the location
    where firmware is loaded into memory. This function is called by
    the wlan driver during the initialization read the firmware and download
    it to wlan card

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID                     XCVR index
    @param[out]      aAddr                   Address where fw is loaded
    @param[out]      aScratch                Scratch area for read verification of FW/NVRAM data
    @param[out]      aSize                   Size of the firmware in bytes

    @retVal          BCM_ERR_OK              API call is successfull. Pointer returned
                                             in aAddr is valid
    @retVal          others                  Error

    @post None

    @limitations None
*/
extern int32_t WLETH_GetFirmwareInfo(ETHXCVR_IDType aID,
                                     uint8_t const ** aAddr,
                                     uint8_t const **aScratch,
                                     uint32_t *const aSize);

/** @brief Get wlan NVRAM information

    This API shall return the address of the location
    where NVRAM data is loaded. This is called by the driver
    during the initialiation of the card to load the NVRAM
    setting in the chip

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID                     XCVR index
    @param[out]      aAddr                   Address where nvram data is loaded
    @param[out]      aSize                   Size of the nvram in bytes

    @retVal          BCM_ERR_OK              API call is successfull. Pointer returned
                                             in aAddr is valid
    @retVal          others                  Error

    @post None

    @limitations None
*/
extern int32_t WLETH_GetNvramInfo(ETHXCVR_IDType aID,
                               uint8_t const **aAddr,
                               uint32_t *const aSize);


/** @brief Timer expiry handler

    This function is implemented by wlan driver. This function
    shall be called in wlan task context when timer expires

    @behavior Sync, non-rentrant

    @pre None

    @param[in]       aID                     SD slot ID index

    @return          void

    @post None

    @limitations None
*/
extern void WLETH_TimerExpiryHandler(uint32_t aBusID);

/** @brief Request to driver

    This API requests a service from the Wireless driver. This function
    internally calls driver request handler (#WLETH_ReqHandler())

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aID                     Index of the controller
    @param[inOut]    aReq                    Pointer to request structure

    @return          BCM_ERR_OK              Request was processes successfully
                                             and result is immediately available
                                             after the function returns
    @return         BCM_ERR_BUSY             Request queued successfully to the
                                             driver and it will be processed
                                             asynchronously by the driver.
                                             When request completes, respective
                                             callback will be raised to client
                                             (for example: when #WLXCVR_SetMode()
                                             is called, a request to change the xcvr
                                             mode is queued to the driver and when
                                             mode change completes, driver will
                                             raise #WLXCVR_ModeChangeInd())
    @return          BCM_ERR_UNINIT          driver is not initialized
    @return          BCM_ERR_INVAL_PARAMS    aID is invalid
    @return          BCM_ERR_INVAL_PARAMS    aReq is NULL
    @return          BCM_ERR_INVAL_PARAMS    some parameters in aReq is not valid
                                             (depending on the reqID)
    @post None

    @limitations None
*/
extern int32_t WLETH_Request(ETHER_HwIDType aID, WLETH_ReqType *const aReq);

/** @brief Driver internal request handler

    This is internal request handler of the driver.
    This is called by WLETH_Request() function

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aID            Index of the controller
    @param[inOut]    aReq           Pointer to request structure

    @return          BCM_ERR_OK              Request was processes successfully
                                             and result is immediately available
                                             after the function returns
    @return         BCM_ERR_BUSY             Request queued successfully to the
                                             driver. Integrator shall call
                                             #WLETH_ProcessRequest() in wireless
                                             thread context to process this queued
                                             request
    @return          BCM_ERR_UNINIT          driver is not initialized
    @return          BCM_ERR_INVAL_PARAMS    aID is invalid
    @return          BCM_ERR_INVAL_PARAMS    aReq is NULL
    @return          BCM_ERR_INVAL_PARAMS    some parameters in aReq is not valid
                                             (depending on the reqID)
    @post None

    @limitations None
 */
extern int32_t WLETH_ReqHandler(ETHER_HwIDType aID, WLETH_ReqType *const aReq);

/** @brief Driver's request processing function

    This API shall be called in the wlan task context to process the
    driver request queue

    @behavior Sync, Re-entrant

    @pre None

    @param[in]       aID                     Index of the controller

    @return          void

    @post None

    @limitations None
 */
extern void WLETH_ProcessRequest(ETHER_HwIDType aID);
#endif /* WL_ETH_OSIL_H */
/** @} */
