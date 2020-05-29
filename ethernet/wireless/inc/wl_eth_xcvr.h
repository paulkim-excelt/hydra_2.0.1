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
    @defgroup grp_wleth_xcvr Wireless Ethernet Transceiver interface
    @ingroup grp_wireless

    @addtogroup grp_wleth_xcvr
    @{

    @file wl_eth_xcvr.h
    @brief Wireless Ethernet Transceiver interface
    Wireless ethernet transceiver interface provides APIs
    to initialize the wireless transceiver (downloading of the
    firmware, initializing the wireless chip).It provides APIs
    to Turn On/Off the wireless transceiver (#WLXCVR_SetMode()),
    and get/set the radio parameters of the chip (#WLXCVR_GetRadioParamReq())
    Since the wireless transceivers are mostly outside of the MCU,
    most of the APIs are asynchronous (it only queues the request
    to stack which is processed asynchronously by the stack) and when
    request completes, callback indication is raised to client

    @version 1.26 Imported from docx
*/

#ifndef WL_ETH_XCVR_H
#define WL_ETH_XCVR_H

#include <stddef.h>
#include <inttypes.h>
#include <eth_xcvr.h>
#include <wl_country_codes.h>

/**
    @name Wireless Ethernet Transceiver Interface IDs
    @{
    @brief Interface IDs for Wireless Ethernet Transceiver Driver
*/
#define BRCM_SWARCH_WLXCVR_SSIDNAMELENMAX_MACRO     (0x00U) /**< @brief #WLXCVR_SSID_NAME_LEN_MAX */
#define BRCM_SWARCH_WLXCVR_SECURITYKEYLENMAX_MACRO  (0x01U) /**< @brief #WLXCVR_SECURITY_KEY_LEN_MAX */
#define BRCM_SWARCH_WLXCVR_SECURITYKEYLENMIN_MACRO  (0x02U) /**< @brief #WLXCVR_SECURITY_KEY_LEN_MIN */
#define BRCM_SWARCH_WLXCVR_RADIOID_TYPE             (0x03U) /**< @brief #WLXCVR_RadioIDType */
#define BRCM_SWARCH_WLXCVR_BAND_TYPE                (0x04U) /**< @brief #WLXCVR_BandType */
#define BRCM_SWARCH_WLXCVR_RADIOPARAMSID_TYPE       (0x05U) /**< @brief #WLXCVR_RadioParamIDType */
#define BRCM_SWARCH_WLXCVR_INF_TYPE                 (0x06U) /**< @brief #WLXCVR_InfType */
#define BRCM_SWARCH_WLXCVR_SECURITY_TYPE            (0x07U) /**< @brief #WLXCVR_SecurityType */
#define BRCM_SWARCH_WLXCVR_APCFG_TYPE               (0x08U) /**< @brief #WLXCVR_APCfgType */
#define BRCM_SWARCH_WLXCVR_WLANCFG_TYPE             (0x09U) /**< @brief #WLXCVR_WLANCfgType */
#define BRCM_SWARCH_WLXCVR_CFG_TYPE                 (0x0AU) /**< @brief #WLXCVR_CfgType */
#define BRCM_SWARCH_WLXCVR_INIT_PROC                (0x0BU) /**< @brief #WLXCVR_Init */
#define BRCM_SWARCH_WLXCVR_SETMODE_PROC             (0x0CU) /**< @brief #WLXCVR_SetMode */
#define BRCM_SWARCH_WLXCVR_GETMODE_PROC             (0x0DU) /**< @brief #WLXCVR_GetMode */
#define BRCM_SWARCH_WLXCVR_GETLINKSTATE_PROC        (0x0EU) /**< @brief #WLXCVR_GetLinkState */
#define BRCM_SWARCH_WLXCVR_GETRADIOPARAMREQ_PROC    (0x0FU) /**< @brief #WLXCVR_GetRadioParamReq */
/** @} */

/**
    @brief AP SSID max lenght
*/
#define WLXCVR_SSID_NAME_LEN_MAX           (32UL)

/**
    @brief WPA/WPA2 security key max lenght
*/
#define WLXCVR_SECURITY_KEY_LEN_MAX        (64UL)

/**
    @brief WPA/WPA2 security key min lenght
*/
#define WLXCVR_SECURITY_KEY_LEN_MIN        (8UL)

/**
    @brief Wireless transceiver radio index

    Index of radio within the context of transceiver
    Bits[7;0] is for index of the radio
    Bits[31:8]: is for the channel withing the context of the radio
*/
typedef uint32_t WLXCVR_RadioIDType;

/**
    @name WLXCVR_BandType
    @{
    @brief 802.11 bands
*/
typedef uint32_t WLXCVR_BandType;
#define WLXCVR_BAND_AUTO            (0UL) /**< @brief Auto selection */
#define WLXCVR_BAND_2G              (1UL) /**< @brief 2G Band */
#define WLXCVR_BAND_5G              (2UL) /**< @brief 5G Band */
/** @} */

/**
    @name WLXCVR_RadioParamIDType
    @{
    @brief Radio parameter types
*/
typedef uint32_t WLXCVR_RadioParamIDType;
#define WLXCVR_RADIOPARAMS_BAND        (1UL)   /**< @brief Radio Band. Refer #WLXCVR_BandType */
#define WLXCVR_RADIOPARAMS_TXPWR       (2UL)   /**< @brief Transmit power in dbm */
#define WLXCVR_RADIOPARAMS_BANDWIDTH   (3UL)   /**< @brief Bandwidth */
#define WLXCVR_RADIOPARAMS_BITRATE     (4UL)   /**< @brief trasmit rate (in kbps) */
#define WLXCVR_RADIOPARAMS_RSSI        (5UL)   /**< @brief RSSI */
/** @} */

/**
    @brief Radio Get/Set parameter value type
*/
typedef uint32_t WLXCVR_RadioParamValType;

/**
    @name WLXCVR_InfType
    @{
    @brief Wireless interface type
*/
typedef uint32_t WLXCVR_InfType;
#define WLXCVR_INF_STA                       (0UL)   /**< @brief 802.11 Station or client interface */
#define WLXCVR_INF_AP                        (1UL)   /**< @brief 802.11 Access point interface */
/** @} */

/**
    @name WLXCVR_SecurityType
    @{
    @brief Wireless security type
*/
typedef uint32_t WLXCVR_SecurityType;
#define WLXCVR_SECURITY_OPEN                 (0UL)  /**< @brief open network. No security */
#define WLXCVR_SECURITY_WPA                  (2UL)  /**< @brief WPA Personal (AES + preshared key) */
#define WLXCVR_SECURITY_WPA2                 (3UL)  /**< @brief WPA2 Personal (AES + preshared key) */
/** @} */

/**
    @brief Wireless access point configuration type

    For AP mode (WLXCVR_WLANCfgType::interface set to WLXCVR_INF_AP), this
    configuration parameter will be used to initialize access point with given
    ssid and security.
    For Station mode (WLXCVR_WLANCfgType::interface set to WLXCVR_INF_STA),
    this configuration parameter will be used to connect to a remote access
    point with given SSID and security key
*/
typedef struct {
    uint8_t ssid[WLXCVR_SSID_NAME_LEN_MAX]; /**< @brief AP SSID. if WLAN interface is configured in AP mode
                                                        (#WLXCVR_WLANCfgType::interface is set to WLXCVR_INF_AP),
                                                        AP will be initialized with this name.
                                                        if WLAN interface is STA mode (#WLXCVR_WLANCfgType::interface
                                                        is set to WLXCVR_INF_STA), the station will try
                                                        to connect to this SSID during init */
    uint32_t                    ssidLen;    /**< @brief Lenght of the ssid */
    WLXCVR_SecurityType         security;   /**< @brief Security mode */
    uint8_t                     key[WLXCVR_SECURITY_KEY_LEN_MAX];  /**< @brief security key */
    uint32_t                    keyLen;                     /**< @brief length of the key */
    uint32_t                    channel;                    /**< @brief Channel (AP mode) */
} WLXCVR_APCfgType;

/**
    @brief 802.11 WLAN configuration structure
 */
typedef struct {
    WLXCVR_InfType      interface;  /**< @brief 802.11 interface type */
    WLXCVR_CountryCode   country;    /**< @brief Country code */
    WLXCVR_APCfgType    apCfg;      /**< @brief AP configuration */
} WLXCVR_WLANCfgType;

/**
    @brief WLAN transceiver configuration structure
 */
typedef struct {
    ETHXCVR_IDType      xcvrID;     /**< @brief Transceiver hw id */
    uint32_t            busID;      /**< @brief Transceiver bus ID */
    WLXCVR_WLANCfgType  wlanCfg;    /**< @brief Transceiver WLAN configuration */
} WLXCVR_CfgType;

/** @brief Initialize wireless transceiver

    This API initializes wireless ethernet transceiver.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]      aID         Index of the Transceiver
    @param[in]      aConfig     Pointer to transceiver configuration

    @return     void

    @post None

    @limitations None
*/
void WLXCVR_Init(ETHXCVR_IDType aID,
                    const WLXCVR_CfgType *const aConfig);

/** @brief Sets wireless transceiver mode

    This is asynchronous function. This API just queues
    the request to change the mode of the trasceiver. When mode
    change is completed, client will be indicated by callback
    notification WLXCVR_ModeChangeInd(). This API shall be
    called after xcvr is initialized (WLXCVR_Init())
    if device is configured in AP mode (WLXCVR_WLANCfgType.interface is
    set to WLXCVR_INF_AP), this request will initialize the card in
    Access point mode and then raise the WLXCVR_ModeChangeInd()
    callback.
    if device is configured in STA mode (WLXCVR_WLANCfgType.interface
    is set to WLXCVR_INF_STA), this request will initialize the card
    in station mode (infra) and will try automatically join to
    AP which is set in #WLXCVR_APCfgType.ssid field. When join is
    successful, WLXCVR_ModeChangeInd() will be raised

    @behavior Async, non-reentrant

    @pre None

    @param[in]       aID                     Index of the Transceiver
    @param[in]       aMode                   Mode to be set

    @return          #BCM_ERR_OK              Transceiver mode set successfully
    @return          #BCM_ERR_INVAL_PARAMS    Invalid transceiver ID
    @return          #BCM_ERR_INVAL_PARAMS    Invalid transceiver mode
    @return          #BCM_ERR_UNINIT          Transceiver driver is not
                                             initialized

    @post None

    @limitations None
*/
extern int32_t WLXCVR_SetMode(ETHXCVR_IDType aID,
                                 ETHXCVR_ModeType aMode);

/** @brief Get current mode of wireless transceiver

    This API gets the current mode of ethernet transceiver.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]      aID         Index of the Transceiver
    @param[in]      aMode       Pointer to mode variable

    @return          #BCM_ERR_OK              Transceiver mode set successfully
    @return          #BCM_ERR_INVAL_PARAMS    Invalid transceiver ID
    @return          #BCM_ERR_INVAL_PARAMS    aMode in NULL
    @return          #BCM_ERR_UNINIT          Transceiver driver is not initialized

    @post None

    @limitations None
*/
extern int32_t WLXCVR_GetMode(ETHXCVR_IDType aID,
                                 ETHXCVR_ModeType *const aMode);

/** @brief Get transceiver link state

    This API retrieves the transceiver link state. This API
    will return #ETHXCVR_LINKSTATE_ACTIVE if it join was successful during
    #WLXCVR_SetMode(), else this will return #ETHXCVR_LINKSTATE_DOWN

    @behavior Sync, non-reentrant

    @pre None

    @param[in]       aID                     Index of the transceiver
    @param[out]      aLinkState              Pointer to link state

    @return          #BCM_ERR_OK              Successfully retrieved transceiver
                                             link state
    @return          #BCM_ERR_INVAL_PARAMS    Invalid transceiver index
    @return          #BCM_ERR_INVAL_PARAMS    aLinkState is NULL
    @return          #BCM_ERR_UNINIT          Transceiver driver is not
                                             initialized

    @post None

    @limitations None
*/
extern int32_t WLXCVR_GetLinkState(ETHXCVR_IDType aID,
                                   ETHXCVR_LinkStateType *const aLinkState);

/** @brief Gets Radio parameters

    Request to retrieves the given radio parameter from transceiver.
    This is an asynchronous API. This API just queues the request.
    When parameter is read from the transceiver, a callback indication
    #WLXCVR_GetParamInd() will be raised with result

    @behavior Async, non-reentrant

    @pre None

    @param[in]       aID                     Index of the transceiver
    @param[out]      aParams                 Radio Parameter to get

    @return          #BCM_ERR_OK              Request queued successfully
    @return          #BCM_ERR_INVAL_PARAMS    Invalid transceiver index
    @return          #BCM_ERR_INVAL_PARAMS    Invalid paramter ID
    @return          #BCM_ERR_UNINIT          Transceiver driver is not
                                             initialized

    @post None

    @limitations None
*/
extern int32_t WLXCVR_GetRadioParamReq(ETHXCVR_IDType aID,
                                       WLXCVR_RadioIDType aRadioID,
                                       WLXCVR_RadioParamIDType aParams);

#endif /* WL_ETH_XCVR_H */
/** @} */
