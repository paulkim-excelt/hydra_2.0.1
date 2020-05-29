/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
  @addtogroup grp_inetlib
  @{
  @file inet.h
  @brief Common definitions for the Internet Stack

  This file provides the common definitions for the Internet Stack

  @version 0.86 Imported from MS Word document
*/
#ifndef INET_H
#define INET_H

#include <stdint.h>

/**
  @name INET_IPAddress
  @{
  @brief IP Address

  @trace #BRCM_ARCH_INETLIB_IPADDRESS_TYPE   #BRCM_REQ_INET_IPADDRESS
*/
typedef uint32_t INET_IPAddress;       /**< @brief typedef for IP Address */
#define INET_IPADDRESS_INVALID (0x0UL) /**< @brief Invalid IP Address */
/** @} */

/**
  @name INET_Port
  @{
  @brief L4 port number

  @trace #BRCM_ARCH_INETLIB_PORT_TYPE   #BRCM_REQ_INET_PORT
*/
typedef uint16_t INET_Port;    /**< @brief typedef for port number */
#define INET_PORT_INVALID (0U) /**< @brief Invalid port number */
/** @} */

/** @struct INET_EndPointIPPort
    @brief IP address and Port for an endpoint

    @trace #TBD
*/
typedef struct _INET_EndPointIPPort {
    INET_Port             localPort;  /**< @brief Local port number */
    INET_Port             remotePort; /**< @brief Remote port number */
    INET_IPAddress        remoteAddr; /**< @brief Remote IP address. Provide a valid
                                           unicast/multicast address here */
} INET_EndPointIPPort;

#endif /* INET_H */
/** @} */
