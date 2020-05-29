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

#include <stdlib.h>
#include <string.h>

#include "ee.h"
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>
#include <bcm_utils.h>
#include "crf.h"

/**
  @addtogroup grp_avb_qt
  @{

  @file crflib_qt.c
  @brief CLock Reference Format qualification Test
  This source file contains the qualification tests for Clock
  Reference Foamt (CRF) packetization and parsing interface present
  within the AVB library.
*/

/**
    @name Qualification Test Sequence IDs
    @{
    @brief Qualification Test Sequence IDs
*/
#define BRCM_SWQTDSGN_AVB_CRF_SEQ0_PROC (0xF0UL) /**< @brief #CRF_LIB_IT1 */
/** @} */

/**
    @brief Configuration test

    This is to validate the configuration capability of the
    AVB library for CRF functionality.

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
#define BRCM_SWQTST_AVB_CRF_CONFIG_CASE0   (0UL)

/**
    @brief Packetization/Parsing test

    This is to validate the CRF packetization and CRF packet parsing
    capability of the AVB library.

    @trace #BRCM_SWREQ_AVB_CRF
*/
#define BRCM_SWQTST_AVB_CRF_CONFIG_CASE1   (1UL)

/** @brief CRF packetization and parsing Test Functionality

    This test sequence will verify the CRF packetization and CRF packet
    parsing feature of the AVB library. The ability of the library to
    generate packets based on the configuration and parse packets
    shall be verified.

    @testproc #BRCM_SWQTST_AVB_CRF_CONFIG_CASE0
    @testproc #BRCM_SWQTST_AVB_CRF_CONFIG_CASE1

    @code{.unparsed}
    Run this (for BRCM_SWQTST_AVB_CRF_CONFIG_CASE0)
    Run this (for BRCM_SWQTST_AVB_CRF_CONFIG_CASE1)
    @endcode

*/
#define BRCM_SWQTSEQ_AVB_CRF_SEQ0   (1UL)

/** @brief CRF Library Interface Test

    This test validates the CRF interface of AVB library. Both the
    packetization and parsing interface shall be verified.

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWQTSEQ_AVB_CRF_SEQ0

    @limitations None

    @code{.c}
    retVal = BCM_ERR_OK

    Update the stream params to #AVB_CRF_TlkrParamsType
    retVal = #AVB_CRF_ConstructPacket to create CRF packet
    #CRF_CHECK_ERR to check retVal
    retVal = #CRF_ValidateCRFTxPacket to validate CRF packet
    #CRF_CHECK_ERR to check retVal

    Update the stream params to #AVB_CRF_LstrParamsType
    retVal = #AVB_ExtractCRFTimestamp to parse CRF packet
    #CRF_CHECK_ERR to check retVal
    retVal = #AVBL_ValidateCRFParams to validate library fetched params
    #CRF_CHECK_ERR to check retVal

    Update test execution status
    return

    @endcode
*/
void CRF_LIB_IT1(void);
/** @} */

#define GetModuleLogLevel() ULOG_LVL_VERBOSE

static volatile int32_t crf_lib_qt1_result = BCM_AT_NOT_STARTED;

#define CRF_CHECK_ERR(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); crf_lib_qt1_result = ret;}

#define CRF_BUF_SIZE     (200UL)

uint8_t dataBuf[CRF_BUF_SIZE];

uint8_t outBufCRFRefPkt[100] = {0x04, 0x80, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x01, 0x02, 0x65, 0xF5, 0xE1, 0x00,
                                0x00, 0x08, 0x00, 0x01};

uint8_t inpCRFRefPkt[100] = {0x04, 0x80, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x01, 0x02, 0x65, 0xF5, 0xE1, 0x00,
                             0x00, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x20, 0x00, 0x00};

static int32_t CRF_ValidateCRFTxPacket(
                                    uint8_t *buf,
                                    uint32_t bufSize,
                                    AVB_CRF_TlkrParamsType *params);

int32_t CRF_ValidateCRFTxPacket(uint8_t *buf,
                               uint32_t bufSize,
                               AVB_CRF_TlkrParamsType *params)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t val = 0UL;

    /* Check the composed CRF packet with reference output */
    val = memcmp(buf, outBufCRFRefPkt, 20UL);
    if (val != 0UL) retVal = BCM_ERR_DATA_INTEG;

    return retVal;
}

int32_t AVBL_ValidateCRFParams(AVB_CRF_LstrParamsType *pktParams)
{
    int32_t retVal = BCM_ERR_OK;

    /* Verify last sequence num flag */
    if ((0UL != pktParams->pktSeqNum) ||
        (0UL == pktParams->baseFrequency) ||
        (0x0102UL != pktParams->crfStreamID) ||
        (0UL == pktParams->crfTsInterval) ||
        (0UL != pktParams->isMediaClockRestart) ||
        (0UL != pktParams->isTSUncertain) ||
        (0UL == pktParams->numTimestamp) ||
        (3UL != pktParams->pullValue) ||
        (0x200000ULL != pktParams->timestamp[0])){
        retVal = BCM_ERR_UNKNOWN;
        CRF_CHECK_ERR(retVal,__FILE__, __LINE__);
    }
    return retVal;
}

void CRF_LIB_IT1(void)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t *buf = NULL;
    uint32_t bufSize = 0UL;

    bufSize = CRF_BUF_SIZE;
    buf = &dataBuf[0];

    (void)memset(buf,0L,CRF_BUF_SIZE);

    AVB_CRF_TlkrParamsType crfParams;
    (void)memset(&crfParams,0L,sizeof(AVB_CRF_TlkrParamsType));
    crfParams.crfCfgForHdr.baseFrequency = 100 * 1000000;
    crfParams.crfCfgForHdr.crfNumTsPerPacket = 1UL;
    crfParams.crfCfgForHdr.crfStreamID = 0x102UL;
    crfParams.crfCfgForHdr.crfTsInterval = 1UL;
    crfParams.crfCfgForHdr.crfTsType = AVB_CRF_TIMESTAMP_VIDEO_FRAME;
    crfParams.crfCfgForHdr.mttCRFNs = 1000UL;
    crfParams.crfCfgForHdr.pcp = 2UL;
    crfParams.crfCfgForHdr.pullValue = AVB_CRF_CLKPULLVAL_MUL_24_BY_25;
    crfParams.crfCfgForHdr.vlanID = 2UL;
    crfParams.numTimestamp = 1UL;
    crfParams.timestamp[0] = 0x200000ULL;

    /* Verify the CRF packet creation API */
    retVal = AVB_CRF_ConstructPacket (buf, &bufSize, &crfParams);
    CRF_CHECK_ERR(retVal,__FILE__, __LINE__);
    retVal = CRF_ValidateCRFTxPacket(buf, bufSize, &crfParams);
    CRF_CHECK_ERR(retVal,__FILE__, __LINE__);

    /* Verify the CRF packet parse API*/
    AVB_CRF_LstrParamsType CRFPktData;
    BCM_MemSet(&CRFPktData, 0x0, sizeof(AVB_CRF_LstrParamsType));
    retVal = AVB_CRF_ExtractTimestamp(inpCRFRefPkt, 28UL, &CRFPktData);
    CRF_CHECK_ERR(retVal,__FILE__, __LINE__);
    retVal = AVBL_ValidateCRFParams(&CRFPktData);
    CRF_CHECK_ERR(retVal,__FILE__, __LINE__);

    if (BCM_AT_EXECUTING == crf_lib_qt1_result) {
        crf_lib_qt1_result = BCM_ERR_OK;
    }

    return;
}

TASK(CRFLibTest)
{
    CRF_LIB_IT1();
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        crf_lib_qt1_result = BCM_AT_EXECUTING;
        ULOG_INFO("\n Test started");
        BCM_ActivateTask(CRFLibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = crf_lib_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}

