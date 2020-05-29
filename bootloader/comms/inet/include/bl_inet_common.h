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

#ifndef BL_INET_COMMON_H
#define BL_INET_COMMON_H
#include <bcm_comp.h>
#include <bcm_err.h>
/** @file bl_inet_common.h
    @brief Common internet stack definitions

    This file provides the interface/APIs which can be used to communicate
    with a remote peer using the Internet stack
*/

/* Includes */
/**********************************************************************
                     Data Structures
**********************************************************************/

typedef uint32_t BL_INET_IPAddressType;
#define BL_INET_IPADDR_BCAST     (1UL)
#define BL_INET_IPADDR_MCAST     (2UL)
#define BL_INET_IPADDR_UCAST     (3UL)

#define BL_INET_IP_HDR_LEN       (20U)

#define BL_INET_IP_ETHTYPE       (0x800U)

#define BL_INET_UDP_HDR_LEN      (8U)

int32_t BL_INET_UDPParseHdr(const uint8_t* const aBuf,
                            const uint16_t aBufLen,
                            const uint16_t aTotalLen,
                            const BL_INET_IPAddress aSrcAddr,
                            const BL_INET_IPAddress aDestAddr,
                            const BL_INET_L4Protocol aL4Proto,
                            BL_INET_Port* const aSrcPort,
                            BL_INET_Port* const aDestPort);

uint16_t BL_INET_UDPCalcChksum(const uint8_t* const aBuf,
                               const uint16_t aTotalLen,
                               const uint16_t aChksumCoverage,
                               const BL_INET_L4Protocol aL4Proto,
                               const BL_INET_IPAddress aSrcAddr,
                               const BL_INET_IPAddress aDestAddr,
                               const BL_INET_Port aSrcPort,
                               const BL_INET_Port aDestPort);

void BL_INET_UDPAddHdr(uint8_t* const aBuf,
                       const BL_INET_Port aSrcPort,
                       const BL_INET_Port aDestPort,
                       const uint16_t aLength,
                       const uint16_t aChksum);

uint16_t BL_INET_IPGetHdrLen(const BL_INET_EndPointId aEndPointId);

void BL_INET_IPAddHdr(uint8_t* const aBuf,
                      const uint16_t aLen,
                      const uint16_t aId,
                      const uint16_t aMoreFrags,
                      const uint16_t aOffset,
                      uint16_t aIPHdrLen,
                      const BL_INET_L4Protocol aProto,
                      const BL_INET_IPAddress aSrcAddr,
                      const BL_INET_IPAddress aDestAddr,
                      const BL_INET_DSCP aDSCP);

void BL_INET_IPGetNextHop(const BL_INET_IPAddress aDestAddr,
                          const BL_INET_IPAddress aDefaultRoute,
                          const BL_INET_IPAddress aSrcAddr,
                          const BL_INET_Netmask aNetmask,
                          BL_INET_IPAddress* const aNextHop);

int32_t BL_INET_IPDetectAddressType(const BL_INET_IPAddress aRemoteIPAddr,
                                    const BL_INET_IPAddress aLocalIPAddr,
                                    const BL_INET_Netmask aMask,
                                    BL_INET_IPAddressType* const aIPAddrType);

int32_t BL_INET_IPParseHdr(const uint8_t* const aBuf,
                           uint16_t* const aHdrLen,
                           BL_INET_IPAddress* const aSrcIPAddr,
                           BL_INET_IPAddress* const aDestIPAddr,
                           uint16_t* const aFragOffset,
                           uint16_t* const aFragLen,
                           uint32_t* const aMoreFrags,
                           BL_INET_L4Protocol* const aL4Proto,
                           uint16_t* const aIdent);

uint16_t BL_INET_IPCalcChksum(const uint8_t* const ipHdr,
                              const uint16_t hdrLen,
                              const uint16_t chksumStartByte);

int32_t BL_INET_ICMPReceive(const BL_INET_InterfaceId aIntfId,
                            const uint8_t* const aICMPBuf,
                            const uint16_t aICMPLen,
                            const uint8_t* const aSrcMACAddr,
                            const BL_INET_IPAddress aSrcIPAddr);

#endif /* BL_INET_COMMON_H */
