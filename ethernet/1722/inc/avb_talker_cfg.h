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
  @addtogroup grp_avbt_ifc
  @{
  @file avb_talker_cfg.h
  @brief Data structures of the AVB Talker (AVBT) Component.
  This file exposes the configuration structures of the AVBT Component
  which are used for configuration.
*/

#ifndef AVB_TALKER_CFG_H
#define AVB_TALKER_CFG_H

#include "avb_core.h"

/**
    @name AVBT Config Architecture IDs
    @{
    @brief Architecture IDs for configuration items for AVB Talker
*/
#define BRCM_SWARCH_AVBT_NUM_MAXPORTS_MACRO       (0x93U) /**< @brief #AVBT_MAX_NUM_PORTS */
#define BRCM_SWARCH_AVBT_COMMONIMAGEHDR_TYPE      (0x94U) /**< @brief #AVBT_CommonImgHdrType */
#define BRCM_SWARCH_AVBT_STREAM1722CFG_TYPE       (0x95U) /**< @brief #AVBT_Stream1722CfgType */
#define BRCM_SWARCH_AVBT_1722CONFIG_TYPE          (0x96U) /**< @brief #AVBT_1722ConfigType */
/** @} */

/**
    @brief Maximum number of ports supported by AVBT component

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_MAX_NUM_PORTS (0x4UL)  /**< @brief Maximum number of ports */

/**
    @brief AVB PT configuration image header

    Structure used to add the header information of the AVB
    configuration image

    @trace #BRCM_SWREQ_AVBT_INIT
*/
typedef struct _AVBT_CommonImgHdrType {
    uint32_t cfgMagic;   /**< @brief Magic ID for image */
    uint32_t AvbCfgType; /**< @brief Config ID */
} AVBT_CommonImgHdrType;

/**
    @brief AVB talker stream configuration structure

    Structure used to specify Stream parameters for
    AVB Talker component

    @trace #BRCM_SWREQ_AVBT_INIT
*/
typedef struct _AVBT_Stream1722CfgType {
    AVB_1722TlkrCfgType libCfg;            /**< @brief Library configuration */
    uint8_t             mcStreamAddr [8];  /**< @brief Stream Multicast Address */
    uint32_t            pcp;               /**< @brief Priority Code Point Value */
    uint32_t            vlanID;            /**< @brief VLAN ID */
    uint32_t            percentRsvdBW;     /**< @brief % reserved link bandwidth */
    uint32_t            isClassAtraffic;   /**< @brief Class A or Class B Traffic  */
    uint32_t            headerLenOffset;   /**< @brief Header length in bytes */
    uint32_t            rsv;               /**< @brief Reserved */
} AVBT_Stream1722CfgType;

/**
    @brief AVB talker configuration structure

    Configuration structure for AVB Talker component

    @trace #BRCM_SWREQ_AVBT_INIT
*/
typedef struct _AVBT_1722ConfigType {
    AVBT_CommonImgHdrType  imgHdr;      /**< @brief Common Header */
    uint32_t               numInpPort;  /**< @brief Num of Input ports (AVB Talker) */
    uint32_t               rsv;         /**< @brief Reserved */
    AVBT_Stream1722CfgType inpStreamCfg[AVBT_MAX_NUM_PORTS];
                                        /**< @brief 1722 Output Stream Configuration*/
} AVBT_1722ConfigType;

#endif /* AVB_TALKER_CFG_H */

/** @} */
