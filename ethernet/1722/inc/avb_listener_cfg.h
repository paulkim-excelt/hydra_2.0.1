/*****************************************************************************
Copyright 2018-2019 Broadcom Limited.  All rights reserved.

This program is the proprietary software of Broadcom Corporation and/or its
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
  @addtogroup grp_avbl_ifc
  @{
  @file avb_listener_cfg.h
  @brief Data structures of the AVB Listener (AVBL) Component.
  This file exposes the configuration structures of the AVBL Component
  which are used for library and component configuration.
*/

#ifndef AVB_LISTENER_CFG_H
#define AVB_LISTENER_CFG_H

/**
    @name AVBL Architecture IDs
    @{
    @brief Architecture IDs for configuration items for AVB Listener
*/
#define BRCM_SWARCH_AVBL_MAX_NUM_PORTS_MACRO     (20U) /**< @brief #AVBL_MAX_NUM_PORTS */
#define BRCM_SWARCH_AVBL_COMMONIMGHDR_TYPE       (21U) /**< @brief #AVBL_CommonImgHdrType */
#define BRCM_SWARCH_AVBL_STREAM1722CFG_TYPE      (22U) /**< @brief #AVBL_Stream1722CfgType */
#define BRCM_SWARCH_AVBL_1722CONFIG_TYPE         (23U) /**< @brief #AVBL_1722ConfigType */
#define BRCM_SWARCH_AVBL_PAYLFORMAT_TYPE         (24U) /**< @brief #AVBL_PaylFormatType */
/** @} */

/**
    @brief Maximum number of ports supported by AVBL component

    @trace #BRCM_SWREQ_AVBL_TODO
*/
#define AVBL_MAX_NUM_PORTS (0x4UL)         /**< @brief Maximum number of ports */

/**
    @name AVBL_PaylFormatType
    @{
    @brief List of 1722 payload formats

    @trace #BRCM_SWREQ_AVBL_TODO
*/
typedef uint32_t AVBL_PaylFormatType;      /**< @brief Payload Format */
#define AVBL_PAYLFORMAT_H264       (0x0UL) /**< @brief H264 payload type */
#define AVBL_PAYLFORMAT_PCM        (0x1UL) /**< @brief PCM payload type */
/** @} */

/**
    @brief AVB PT configuration image header

    Structure used to add the header information of the AVBL
    component configuration image

    @trace #BRCM_SWREQ_AVBL_TODO
*/
typedef struct _AVBL_CommonImgHdrType {
    uint32_t cfgMagic;   /**< @brief Magic ID for image */
    uint32_t AvbCfgType; /**< @brief Config ID */
} AVBL_CommonImgHdrType;

/**
    @brief AVB listener stream configuration structure

    Structure used to specify Stream parameters for
    AVB Listener component

    @trace #BRCM_SWREQ_AVBL_TODO
*/
typedef struct _AVBL_Stream1722CfgType {
    uint64_t             streamID;         /**< @brief Stream ID */
    AVBL_PaylFormatType  payloadFormat;    /**< @brief Payload format */
    uint8_t              mcStreamAddr [8]; /**< @brief Stream Multicast Address */
    uint32_t             pcp;              /**< @brief Priority Code Point Value */
    uint32_t             vlanID;           /**< @brief VLAN ID */
    uint32_t             isClassAtraffic;  /**< @brief Class A or Class B Traffic */
} AVBL_Stream1722CfgType;

/**
    @brief Configuration structure for AVB Listener component

    Configuration structure for AVB Listener component

    @trace #BRCM_SWREQ_AVBL_TODO
*/
typedef struct _AVBL_1722ConfigType {
    AVBL_CommonImgHdrType  imgHdr;      /**< @brief Common Header */
    uint32_t               numOutPort;  /**< @brief Num of Output ports (AVB Listener) */
    uint32_t               rsv;         /**< @brief Reserved */
    AVBL_Stream1722CfgType outStreamCfg[AVBL_MAX_NUM_PORTS];
                                        /**< @brief 1722 Incoming Stream Configuration */

} AVBL_1722ConfigType;

#endif /* AVB_LISTENER_CFG_H */

/** @} */
