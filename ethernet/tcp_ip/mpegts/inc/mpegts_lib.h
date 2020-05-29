/*****************************************************************************
Copyright 2018 Broadcom Limited.  All rights reserved.

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
  @defgroup grp_mpegts_ifc Interface
  @ingroup grp_mpegts

  @addtogroup grp_mpegts_ifc
  @{
  @section sec_lib_overview Overview

  The mpegts library comprises interface for the packetization
  functionality. The library can be used to generate the section tables
  such as Program Association Table and Program Management Table. It
  currently supports H264 video as payload. PES packetization of ES
  payload is done before encapsulating them in TS packets. The library
  takes a parameter 'n' where it packs input data into 'n' output
  TS packets. In case there is undeflow of input data, then it adds
  NULL TS packets to for 'n' TS in one output buffer.

  The media clock synchronization is achieved by transmitting PCR
  at every frame interval. PCR is embedded in the TS adaptation field.
  The PES packets are all timestamped with appropriate Presentation
  Timestamp values.

  @section sec_comp_fsm State Machine
  NA

  @section sec_comp_seq Sequence Diagrams
  @image html mpegts_lib_sequence_diagram.jpg "Sequence diagram"

  @section sec_comp_perf Performance & Memory

  | Performance       | Description                                   |
  |:-----------------:|:---------------------------------------------:|
  | CPU Load          |   NA                                          |
  | Memory Bandwidth  |   NA                                          |
  | HW Utilization    |   NA                                          |

  @includedoc mpegtslib.dep

  @limitations
  Limitations of MPEGTS library
  -# Support for H264 video ES payload only
  -# Support for a single program with one video stream
  -# Only six TS packets in one output buffer has been verified
  -# Stream output is VBR

  @file mpegts_lib.h
  @brief Library interface for MPEGTS Library

  This file provides the interface/APIs which can be used to generate a
  mpegts packet for the payload. It also supports generation of PAT,
  PMT, PCR, NULL and PES (ES) packets.

*/

#ifndef MPEGTS_LIB_H
#define MPEGTS_LIB_H

/**
    @name MPEGTS Library API IDs
    @{
    @brief API IDs for MPEGTS Library
 */
#define BRCM_SWARCH_MPEGTS_PACKETSIZE_MACRO            (0UL) /**< @brief #MPEGTS_PACKET_SIZE     */
#define BRCM_SWARCH_MPEGTS_PACKETTYPE_TYPE             (1UL) /**< @brief #MPEGTS_PacketType      */
#define BRCM_SWARCH_MPEGTS_LIBCONFIG_TYPE              (2UL) /**< @brief #MPEGTS_LibConfigType   */
#define BRCM_SWARCH_MPEGTS_STREAMINFO_TYPE             (3UL) /**< @brief #MPEGTS_StrInfoType     */
#define BRCM_SWARCH_MPEGTS_PACKETIZERLIB_CONTEXT_TYPE  (4UL) /**< @brief #MPEGTS_PacketizerLibContextType */
#define BRCM_SWARCH_MPEGTS_PAYLOADINFO_TYPE            (5UL) /**< @brief #MPEGTS_PayloadInfoType */
#define BRCM_SWARCH_MPEGTS_BUFFERTYPE_TYPE             (6UL) /**< @brief #MPEGTS_DataBufferType  */
#define BRCM_SWARCH_MPEGTS_CREATEPACKET_PROC           (7UL) /**< @brief #MPEGTS_ConstructPacket */
/** @} */

/** @brief A macro indicating maximum number of bytes in a MPEGTS packet

    @trace #BRCM_SWREQ_MPEGTS_CONFIGURATION
*/
#define MPEGTS_PACKET_SIZE           (188UL)

/**
  @name MPEGTS_PacketType
  @{
  @brief Type of MPEGTS Packet used to indicate to the library.
  The library based on the packet type specified, shall create the
  MPEGTS packet using the API @MPEGTS_ConstructPacket

  @trace #BRCM_SWREQ_MPEGTS_PACKETIZATION
  @trace #BRCM_SWREQ_MPEGTS_TABLES_SUPPORT
*/
typedef uint32_t MPEGTS_PacketType; /**< @brief MPEGTS Packet type */
#define MPEGTS_PACKETTYPE_PAT_PMT      (0x0UL) /**< @brief  Program Association & Program Management packet */
#define MPEGTS_PACKETTYPE_PES          (0x1UL) /**< @brief  PES-ES packet */
/** @} */

/**
    @brief Configuration structure which specifies the MPEGTS stream
    and table parameters.

    @trace #BRCM_SWREQ_MPEGTS_CONFIGURATION
*/
typedef struct _MPEGTS_LibConfigType {
    uint32_t cfgMagic;                          /**< @brief Magic */
    uint32_t cfgType;                           /**< @brief Config Type */
    uint8_t  refPATPacket[MPEGTS_PACKET_SIZE];  /**< @brief Reference PAT packet */
    uint32_t refPATPacketSize;                  /**< @brief Reference PAT packet size*/
    uint8_t  refPMTPacket[MPEGTS_PACKET_SIZE];  /**< @brief Reference PMT packet */
    uint32_t refPMTPacketSize;                  /**< @brief Reference PMT packet size */
    uint32_t tsPID;                             /**< @brief TS PID value for ES-PES */
    uint32_t pcrPID;                            /**< @brief TS PID value for PCR stream */
    uint32_t ptsOffsetForPES;                   /**< @brief PTS timestamp offset for playback in milliseconds */
    uint32_t numTsPerPayload;                   /**< @brief Number of TS packets per output payload */
} MPEGTS_LibConfigType;

/**
    @brief MPEGTS Stream Information (to be filled by client)
    for packetization

    @trace #BRCM_SWREQ_MPEGTS_PACKETIZATION
*/
typedef struct _MPEGTS_StrInfoType {
    MPEGTS_LibConfigType strCFg;
    uint32_t isStartOfESFrame;                       /**< @brief Bool flag to indicate start of a stream */
    uint64_t payloadTimestamp;                       /**< @brief Timestamp for the payload */
}MPEGTS_StrInfoType;

/**
    @brief Scratch memory used by MPEGTS libary for creation of header
    during packetization. This memory shall be reset to zero by the
    client/application st startup and before invoking the APIs of the
    MPEGTS library.

    @trace #BRCM_SWREQ_MPEGTS_PACKETIZATION
*/
typedef struct _MPEGTS_PacketizerLibContextType {
    uint8_t ccPATPacket;    /**< @brief Continuity counter for PAT */
    uint8_t ccPMTPacket;    /**< @brief Continuity counter for PMT */
    uint8_t ccPCRPacket;    /**< @brief Continuity counter for PCR */
    uint8_t ccESPacket;     /**< @brief Continuity counter for PES/ES */
}MPEGTS_PacketizerLibContextType;

/**
    @brief MPEGTS Payload parameters used for MPEGTS header addition and
    packetization

    @trace #BRCM_SWREQ_MPEGTS_PACKETIZATION
*/
typedef struct _MPEGTS_PayloadInfoType {
    MPEGTS_StrInfoType strParams;             /**< @brief Stream configuration */
    MPEGTS_PacketizerLibContextType libParam; /**< @brief Scratch memory used by library */
}MPEGTS_PayloadInfoType;

/**
    @brief Data buffer type of the TS library interface

    @trace #BRCM_SWREQ_MPEGTS_PACKETIZATION
*/
typedef struct _MPEGTS_DataBufferType {
    uint8_t *buffer;                   /**< @brief Pointer to the buffer data */
    uint32_t length;                   /**< @brief Length of the data */
    uint32_t offset;                   /**< @brief Offset */
} MPEGTS_DataBufferType;

/**
 * @brief Function to create MPEGTS packet
 *
 * @behavior Sync, Re-entrant
 *
 * @pre None
 *
 * This function creates the MPEGTS packet header for the payload at
 * the pointer passed as output buffer. The library does not copy the
 * input payload data to the output buffer and lets the client manage
 * it, for memory optimizations. Based on the value of
 * #MPEGTS_PacketType specified, the API shall generate appropriate
 * TS packet.
 *
 * @param[in]     aPktType              MPEGTS packet type
 * @param[out]    aBuf                  The pointer to output buffer
 * @param[inout]  aBuf                  The output buffer length
 * @param[inout]  aConfig               The stream configuration and library
 *                                      members.
 *
 * @retval      #BCM_ERR_INVAL_PARAMS  Invalid function params
 * @retval      #BCM_ERR_OK            TS packet/header has been created
 *                                     successfully.
 *
 * @post None
 *
 * @trace #BRCM_SWREQ_MPEGTS_PACKETIZATION
 * @trace #BRCM_SWREQ_MPEGTS_PES
 * @trace #BRCM_SWREQ_MPEGTS_TABLES_SUPPORT
 * @trace #BRCM_SWREQ_MPEGTS_PCR
 * @trace #BRCM_SWREQ_MPEGTS_PADDING
 *
 * @limitations None
 */
int32_t MPEGTS_ConstructPacket (const MPEGTS_PacketType aPktType,
                                 MPEGTS_DataBufferType* const inpBuf,
                                 MPEGTS_DataBufferType* const outBuf,
                                 MPEGTS_PayloadInfoType* const aConfig);

#endif /* MPEGTS_LIB_H */
/** @} */
