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

/* Includes */
#include <stdlib.h>
#include <string.h>
#include "ee.h"
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>
#include "mpegts_lib.h"

/**
  @defgroup grp_mpegts_qt Qualification Tests
  @ingroup grp_mpegts

  @addtogroup grp_mpegts_qt
  @{

  @file code.c
  @brief MPEGTS Library Qualification Test
  This source file contains the qualification tests for MPEGTS library
*/

/**
    @name Qualification Test Sequence IDs
    @{
    @brief Qualification Test Sequence IDs
*/
#define BRCM_SWQTDSGN_MPEGTS_SEQ0_PROC      (0xF0UL)   /**< @brief #MPEGTS_LIB_IT1  */
/** @} */

/**
    @brief Configuration test

    This is to validate the configuration capability of the
    MPEGTS library.

    @trace #BRCM_SWREQ_MPEGTS_CONFIGURATION
*/
#define BRCM_SWQTST_MPEGTS_CONFIG_CASE0   (0UL)

/**
    @brief Packetization test

    This is to validate the mpegts packetization capability of the
    library.

    @trace #BRCM_SWREQ_MPEGTS_PACKETIZATION
    @trace #BRCM_SWREQ_MPEGTS_TABLES_SUPPORT
    @trace #BRCM_SWREQ_MPEGTS_PCR
    @trace #BRCM_SWREQ_MPEGTS_PES
    @trace #BRCM_SWREQ_MPEGTS_PADDING
*/
#define BRCM_SWQTST_MPEGTS_CONFIG_CASE1   (1UL)

/** @brief Brief description of BRCM_SWQTSEQ_COMP_SEQ0

    This test sequence will validate PAT, PMT with NULL packets in
    one output buffer. Also, for input payload, it will create PES
    packets in the output buffer. Each output buffer shall be verified
    to contain Six MPEGTS packets.

    @testproc #BRCM_SWQTST_MPEGTS_CONFIG_CASE0
    @testproc #BRCM_SWQTST_MPEGTS_CONFIG_CASE1

    @code{.unparsed}
    Run this (for BRCM_SWQTST_MPEGTS_CONFIG_CASE0)
    Run this (for BRCM_SWQTST_MPEGTS_CONFIG_CASE1)
    @endcode

*/
#define BRCM_SWQTSEQ_MPEGTS_SEQ0   (1UL)

/** @brief Packetization Test

    This test validates the packetization capability of the
    MPEGTS library.

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWQTSEQ_MPEGTS_SEQ0

    @limitations None

    @code{.c}

    Populate config to #MPEGTS_PayloadInfo structure
    Create input and output buffers of type #MPEGTS_DataBufferType

    ret = invoke #MPEGTS_ConstructPacket(PAT & PMT)
    if ret is ERR_OK
        Validate the mpegts packets in output buffer
    else
        Mark error and return

    ret = invoke #MPEGTS_ConstructPacket(PES)
    if ret is ERR_OK
        Validate the mpegts packets in output buffer
    else
        Mark error and return

    check the result of above API tests and log print

    return
    @endcode

*/
static void MPEGTS_LIB_IT1(void);
/** @} */

#define GetModuleLogLevel() ULOG_LVL_VERBOSE

#define ES_PID (17U)
#define PMT_PID (16U)
#define PAT_PID (0U)
#define NULL_PID (0x1FFF)

uint32_t numESPacket = 0UL;
uint32_t numPATPacket = 0UL;
uint32_t numPMTPacket = 0UL;
uint32_t numNULLPacket = 0UL;
uint32_t numESVidFrames = 0UL;
uint32_t numESVidBytes = 0UL;

uint8_t outTestBuf[1500];
uint8_t inTestBuf[1500];

/* Reference PAT packet */
uint8_t testPATPacket[] = {
             0x47, 0x40, 0x00, 0x10, 0x00, 0x00, 0xB0, 0x0D, 0x00, 0x22, 0xC1, 0x00, 0x00, 0x00, 0x01, 0xE0,
             0x10, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* Reference output PAT TS packet with updated CRC */
uint8_t outPATPacket[] = {
             0x47, 0x40, 0x00, 0x10, 0x00, 0x00, 0xB0, 0x0D, 0x00, 0x22, 0xC1, 0x00, 0x00, 0x00, 0x01, 0xE0,
             0x10, 0xC5, 0x63, 0x73, 0xAE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* Reference PMT packet */
uint8_t testPMTPacket[] = {
             0x47, 0x40, 0x10, 0x10, 0x00, 0x02, 0xB0, 0x12, 0x00, 0x01, 0xC1, 0x00, 0x00, 0xE0, 0x11, 0xF0,
             0x00, 0x1B, 0xE0, 0x11, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* Reference output PMT TS with updated CRC */
uint8_t outPMTPacket[] = {
             0x47, 0x40, 0x10, 0x10, 0x00, 0x02, 0xB0, 0x12, 0x00, 0x01, 0xC1, 0x00, 0x00, 0xE0, 0x11, 0xF0,
             0x00, 0x1B, 0xE0, 0x11, 0xF0, 0x00, 0xD1, 0xFB, 0x62, 0x71, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* Reference output NULL TS packet */
uint8_t outNULLPacket[] = {
             0x47, 0x1F, 0xFF, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static volatile int32_t mpegts_lib_qt1_result = BCM_AT_NOT_STARTED;

#define MPEGTSLIB_CHECK_ERR_IT1(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); mpegts_lib_qt1_result = ret;}

static int32_t MPEGTS_ValidateTables(
                                    uint8_t *buf,
                                    uint32_t bufLen);

static int32_t MPEGTS_ValidatePES(
                                    uint8_t *buf,
                                    uint32_t bufLen);

static int32_t MPEGTS_ValidateTables(uint8_t *buf, uint32_t bufLen)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t totalLenProc = 0UL;
    uint16_t tsPID = 0U;
    int32_t val;

    while(totalLenProc < bufLen){
        /* Verify the TS sync byte */
        if (0x47U != buf[totalLenProc]) {
            ULOG_ERR("ERR > TS Sync byte Incorrect %u %u --- %x \n",
                                totalLenProc,bufLen,buf[totalLenProc]);
            retVal = BCM_ERR_DATA_INTEG;
            goto error;
        }

        /* Fetch the TS header fields */
        BCM_MemCpy(&tsPID, &buf[totalLenProc+1], 2U);
        tsPID = BE2Host16(tsPID) & 0x1FFFU;
        totalLenProc += 4U;

        /* Check if its a PAT packet */
        if (PAT_PID == tsPID){
            numPATPacket++;
            val = memcmp(&buf[totalLenProc-4],outPATPacket,sizeof(outPATPacket));
            if(val != 0L){
                retVal = BCM_ERR_UNKNOWN;
                goto error;
            }
            totalLenProc += 184U;
        /* Check if its a PMT packet */
        } else if (PMT_PID == tsPID){
            numPMTPacket++;
            val = memcmp(&buf[totalLenProc-4],outPMTPacket,sizeof(outPMTPacket));
            if(val != 0L){
                retVal = BCM_ERR_UNKNOWN;
                goto error;
            }
            totalLenProc += 184U;
        } else if (NULL_PID == tsPID){
            numNULLPacket++;
            val = memcmp(&buf[totalLenProc-4],outNULLPacket,sizeof(outNULLPacket));
            if(val != 0L){
                retVal = BCM_ERR_UNKNOWN;
                goto error;
            }
            totalLenProc += 184U;
        /* In case of an unsupported packet */
        } else {
            retVal = BCM_ERR_NOSUPPORT;
            goto error;
        }
    }

    /* Check if expected packets was detected in buffer */
    if((1UL != numPATPacket) ||
       (1UL != numPMTPacket) ||
       (4UL != numNULLPacket)){
        retVal = BCM_ERR_INVAL_PARAMS;
    }
error:
    return retVal;
}

static int32_t MPEGTS_ValidatePES(uint8_t *buf, uint32_t bufLen)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t totalLenProc = 0UL;
    uint8_t payloadStartInd = 0U;
    uint16_t tsPID = 0U;
    uint8_t adapField = 0U;

    while(totalLenProc < bufLen){
        uint32_t tsPktStartLen = totalLenProc;

        /* Verify the TS sync byte */
        if (0x47U != buf[totalLenProc]) {
            ULOG_ERR("ERR > TS Sync byte Incorrect %u %u --- %x \n",
                                totalLenProc,bufLen,buf[totalLenProc]);
            retVal = BCM_ERR_DATA_INTEG;
            goto error;
        }

        /* Fetch the TS header fields */
        payloadStartInd = (buf[totalLenProc+1] >> 6U) & 0x1;
        BCM_MemCpy(&tsPID, &buf[totalLenProc+1], 2U);
        tsPID = BE2Host16(tsPID) & 0x1FFFU;
        adapField = (buf[totalLenProc+3] >> 4U) & 3U;
        totalLenProc += 4U;


        /* Check if its a ES-PES packet */
        if (ES_PID == tsPID){
            numESPacket++;
            /* Parse Adaptation Control Field */
            if(1U == ((adapField >> 1U) & 1U)){
                uint8_t adapLen = buf[totalLenProc];
                totalLenProc += adapLen+1;
            }

            /* Parse Payload Field */
            if (1U == (adapField & 1U)){
                if((1U == payloadStartInd) &&
                   ((0U == buf[totalLenProc]) &&
                    (0U == buf[totalLenProc+1]) &&
                    (1U == buf[totalLenProc+2]))){
                    numESVidFrames++;
                }
                numESVidBytes += 188UL - (totalLenProc - tsPktStartLen);
                totalLenProc += 188UL - (totalLenProc - tsPktStartLen);
            }
        } else {
            retVal = BCM_ERR_NOSUPPORT;
            goto error;
        }

    }

    /* Check if expected packets was detected in buffer */
    if((6UL != numESPacket) ||
       (1UL != numESVidFrames) ||
       (0UL == numESVidBytes)){
        retVal = BCM_ERR_INVAL_PARAMS;
    }
error:
    return retVal;
}

void MPEGTS_LIB_IT1(void)
{
    int32_t retVal = BCM_ERR_OK;
    MPEGTS_PayloadInfoType config;
    MPEGTS_DataBufferType inpBuf, outBuf;

    /* Initialize the mpegts library configuration structure */
    memset(&config, 0U, sizeof(config));
    memset(&inTestBuf, 0xAAU, sizeof(inTestBuf));

    /* Populate the config structure */
    config.strParams.isStartOfESFrame = 1UL;
    config.strParams.payloadTimestamp = 3000UL;
    config.strParams.strCFg.numTsPerPayload = 6UL;
    config.strParams.strCFg.pcrPID = 17UL;
    config.strParams.strCFg.ptsOffsetForPES = 100UL;
    config.strParams.strCFg.tsPID = 17UL;
    BCM_MemCpy(&config.strParams.strCFg.refPATPacket[0],
               testPATPacket,sizeof(testPATPacket));
    config.strParams.strCFg.refPATPacketSize = 21UL;
    BCM_MemCpy(&config.strParams.strCFg.refPMTPacket[0],
               testPMTPacket,sizeof(testPMTPacket));
    config.strParams.strCFg.refPMTPacketSize = 26UL;

    inpBuf.buffer = &inTestBuf[0];
    inpBuf.length = 1500UL;
    inpBuf.offset = 0UL;
    outBuf.buffer = &outTestBuf[0];
    outBuf.length = 1500UL;
    outBuf.offset = 0UL;

    /* Invoke API to generate the PAT and PMT packet */
    retVal = MPEGTS_ConstructPacket(MPEGTS_PACKETTYPE_PAT_PMT,
                                     &inpBuf,
                                     &outBuf,
                                     &config);

    if(BCM_ERR_OK == retVal){
        retVal = MPEGTS_ValidateTables(outBuf.buffer, outBuf.length);
        MPEGTSLIB_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);
    } else {
        MPEGTSLIB_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);
        goto error;
    }

    inpBuf.buffer = &inTestBuf[0];
    inpBuf.length = 1500UL;
    inpBuf.offset = 0UL;
    outBuf.buffer = &outTestBuf[0];
    outBuf.length = 1500UL;
    outBuf.offset = 0UL;

    /* Invoke API to generate the PES packet */
    retVal = MPEGTS_ConstructPacket(MPEGTS_PACKETTYPE_PES,
                                     &inpBuf,
                                     &outBuf,
                                     &config);
    if(BCM_ERR_OK == retVal){
        retVal = MPEGTS_ValidatePES(outBuf.buffer, outBuf.length);
        MPEGTSLIB_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);
    } else {
        MPEGTSLIB_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);
        goto error;

    }

error:
    if (BCM_AT_EXECUTING == mpegts_lib_qt1_result) {
        mpegts_lib_qt1_result = BCM_ERR_OK;
        ULOG_ERR("MPEGTS_LIB_IT1: PASS\n");
    } else {
        ULOG_ERR("MPEGTS_LIB_IT1: FAIL\n");
    }

    return;
}

TASK(MPEGTSLibTest)
{
    MPEGTS_LIB_IT1();

    /* Reset all stats vars */
    numESPacket = 0UL;
    numPATPacket = 0UL;
    numPMTPacket = 0UL;
    numNULLPacket = 0UL;
    numESVidFrames = 0UL;
    numESVidBytes = 0UL;
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        mpegts_lib_qt1_result = BCM_AT_EXECUTING;
        ULOG_INFO("Test started\n");
        BCM_ActivateTask(MPEGTSLibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = mpegts_lib_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}

