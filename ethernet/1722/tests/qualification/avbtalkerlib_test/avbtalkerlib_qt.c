/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
#include <component.h>
#include <msg_queue.h>
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

#include "avb_talker.h"
#include "avb_core.h"

/**
  @defgroup grp_avb_qt 1722 Qualification Tests
  @ingroup grp_avb

  @addtogroup grp_avb_qt
  @{

  @file avbtalkerlib_qt.c
  @brief 1722 Talker qualification Test
  This source file contains the qualification tests for 1722 Talker
  interface of the AVB library
*/

/**
    @name Qualification Test Sequence IDs
    @{
    @brief Qualification Test Sequence IDs
*/
#define BRCM_SWQTDSGN_AVB_TLKR_SEQ0_PROC (0xF0UL) /**< @brief #AVB_TALKER_LIB_IT1 */
/** @} */

/**
    @brief Configuration test

    This is to validate the configuration capability of the
    AVB library for packetization functionality.

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
#define BRCM_SWQTST_AVB_TLKR_CONFIG_CASE0   (0UL)

/**
    @brief Packetization test

    This is to validate the 1722 packetization capability of the
    AVB library.

    @trace #BRCM_SWREQ_AVB_1722
*/
#define BRCM_SWQTST_AVB_TLKR_CONFIG_CASE1   (1UL)

/** @brief 1722 Packetization Test Functionality

    This test sequence will verify the 1722 packetization feature of the
    AVB library. The ability of the library to handle different
    stream parameters is verified.

    @testproc #BRCM_SWQTST_AVB_TLKR_CONFIG_CASE0
    @testproc #BRCM_SWQTST_AVB_TLKR_CONFIG_CASE1

    @code{.unparsed}
    Run this (for BRCM_SWQTST_AVB_TLKR_CONFIG_CASE0)
    Run this (for BRCM_SWQTST_AVB_TLKR_CONFIG_CASE1)
    @endcode

*/
#define BRCM_SWQTSEQ_AVB_TLKR_SEQ0   (1UL)

/** @brief 1722 Packetization Test

    This test validates the 1722 talker header addition interface of
    AVB library.

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWQTSEQ_AVB_TLKR_SEQ0

    @limitations None

    @code{.c}
    retVal = BCM_ERR_OK

    Fill the stream config and params into #AVB_1722TlkrBufParamsType
    for SPS/PPS NAL
    retVal = #AVB_Add1722Header for packetization
    invoke #AVB_CHECK_ERR to check retVal

    Fill the stream config and params into #AVB_1722TlkrBufParamsType
    for IDR NAL slice start payload
    #AVB_1722TlkrBufParamsType.payInfo.sliceStart = 1
    #AVB_1722TlkrBufParamsType.params.payInfo.sliceEnd = 0
    #AVB_1722TlkrBufParamsType.params.payInfo.streamInfo = 0x65
    retVal = #AVB_Add1722Header for packetization
    invoke #AVB_CHECK_ERR to check retVal

    Fill the stream config and params into #AVB_1722TlkrBufParamsType
    for IDR NAL mid-slice payload
    #AVB_1722TlkrBufParamsType.payInfo.sliceStart = 0
    #AVB_1722TlkrBufParamsType.params.payInfo.sliceEnd = 0
    #AVB_1722TlkrBufParamsType.params.payInfo.streamInfo = 0x65
    retVal = #AVB_Add1722Header for packetization
    invoke #AVB_CHECK_ERR to check retVal

    Fill the stream config and params into #AVB_1722TlkrBufParamsType
    for IDR NAL slice end payload
    #AVB_1722TlkrBufParamsType.payInfo.sliceStart = 0
    #AVB_1722TlkrBufParamsType.params.payInfo.sliceEnd = 1
    #AVB_1722TlkrBufParamsType.params.payInfo.streamInfo = 0x65
    retVal = #AVB_Add1722Header for packetization
    invoke #AVB_CHECK_ERR to check retVal

    Update test execution status
    return
    @endcode
*/
void AVB_TALKER_LIB_IT1(void);
/** @} */

#define GetModuleLogLevel() ULOG_LVL_VERBOSE
#define AVBL_TEST_COUNT            (10UL)
#define AVB_AVTP_PAYLOAD_SIZE      (1440UL)

static volatile int32_t avbt_lib_qt1_result = BCM_AT_NOT_STARTED;

#define AVB_CHECK_ERR(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); avbt_lib_qt1_result = ret;}

uint8_t dataBuf[1600] = {0xAAU};

uint8_t outBufRefPkt[100] = {0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00,
                             0x02, 0x01, 0x00, 0x00, 0x05, 0xA4, 0x30, 0x00,
                             0x00, 0x00, 0x00, 0x00};

static int32_t AVBT_ValidateComposedPacket(
                                    uint8_t *buf,
                                    uint32_t bufSize,
                                    AVB_1722TlkrBufParamsType *params);

int32_t AVBT_ValidateComposedPacket(uint8_t *buf,
                                    uint32_t bufSize,
                                    AVB_1722TlkrBufParamsType *params)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t val = 0UL;

    /* Verify last sequence num flag */
    if (0UL == params->libMem.SeqNumCount) {
        retVal = BCM_ERR_UNKNOWN;
        AVB_CHECK_ERR(retVal,__FILE__, __LINE__);
    }

    /* Check the composed 1722 header with reference output */
    if (0x67UL == params->payInfo.streamInfo){
        outBufRefPkt[2] = 1U;  /* seq num update */
        outBufRefPkt[21] = 0xA4U; /* Size update */
        outBufRefPkt[22] = 0x20U; /* Flag update */
        val = memcmp(buf, outBufRefPkt, 28UL);
        if (val != 0UL) {
            retVal = BCM_ERR_DATA_INTEG;
        }
    } else if (0x65UL == params->payInfo.streamInfo){
        if ((1UL == params->payInfo.sliceStart) && (0UL == params->payInfo.sliceEnd)){
            outBufRefPkt[2] = 2U;  /* seq num update */
            outBufRefPkt[21] = 0xA6U; /* Size update */
            outBufRefPkt[22] = 0x20U; /* Flag update */
            outBufRefPkt[28] = 0x7CU; /* FUA Header */
            outBufRefPkt[29] = 0x85U; /* FUA Header */
            val = memcmp(buf, outBufRefPkt, 30UL);
            if (val != 0UL) {
                retVal = BCM_ERR_DATA_INTEG;
            }
        } else if ((0UL == params->payInfo.sliceStart) && (0UL == params->payInfo.sliceEnd)){
            outBufRefPkt[2] = 3U;
            outBufRefPkt[29] = 0x05U; /* FUA Header */
            val = memcmp(buf, outBufRefPkt, 30UL);
            if (val != 0UL) {
                retVal = BCM_ERR_DATA_INTEG;
            }
        } else {
            outBufRefPkt[2] = 4U;
            outBufRefPkt[22] = 0x30U; /* Flag update */
            outBufRefPkt[29] = 0x45U; /* FUA Header */
            val = memcmp(buf, outBufRefPkt, 30UL);
            if (val != 0UL) {
                retVal = BCM_ERR_DATA_INTEG;
            }
        }
    } else {
        /* Nothing */
    }
    return retVal;
}

void AVB_TALKER_LIB_IT1(void)
{
    int32_t retVal = BCM_ERR_OK;
    AVB_1722TlkrCfgType cfgParam;
    uint8_t *buf = NULL;
    uint32_t bufSize = 0UL;
    AVB_1722TlkrBufParamsType params;

    bufSize = AVB_AVTP_PAYLOAD_SIZE;
    buf = &dataBuf[0];

    (void)memset(&cfgParam,0L,sizeof(AVB_1722TlkrCfgType));
    (void)memset(&params,0L,sizeof(AVB_1722TlkrBufParamsType));

    /* Fill valid values for all configurable params */
    cfgParam.streamID = 0x0102;
    cfgParam.mttNs = 200UL;
    cfgParam.payloadFormat = AVB_PAYLFORMAT_H264;

    /* Update stream configuration */
    params.strCfgForHdr = cfgParam;
    params.payInfo.timestamp = 0UL;

    /* Check SPS/PPS data */
    params.payInfo.sliceStart = 1UL;
    params.payInfo.sliceEnd = 1UL;
    params.payInfo.streamInfo = 0x67;
    retVal = AVB_Add1722Header (buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);
    retVal = AVBT_ValidateComposedPacket(buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);

    /* Check slice start packet header */
    params.payInfo.sliceStart = 1UL;
    params.payInfo.sliceEnd = 0UL;
    params.payInfo.streamInfo = 0x65;
    retVal = AVB_Add1722Header (buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);
    retVal = AVBT_ValidateComposedPacket(buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);

    /* Check slice intermediate packet header */
    params.payInfo.sliceStart = 0UL;
    params.payInfo.sliceEnd = 0UL;
    params.payInfo.streamInfo = 0x65;
    retVal = AVB_Add1722Header (buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);
    retVal = AVBT_ValidateComposedPacket(buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);

    /* Check slice end packet header */
    params.payInfo.sliceStart = 0UL;
    params.payInfo.sliceEnd = 1UL;
    params.payInfo.frameEnd = 1UL;
    params.payInfo.streamInfo = 0x65;
    retVal = AVB_Add1722Header (buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);
    retVal = AVBT_ValidateComposedPacket(buf, bufSize, &params);
    AVB_CHECK_ERR(retVal,__FILE__, __LINE__);

    if (BCM_AT_EXECUTING == avbt_lib_qt1_result) {
        avbt_lib_qt1_result = BCM_ERR_OK;
    }

    ULOG_ERR("AVB_TALKER_LIB_IT1 Completed\n");
    return;
}

TASK(AVBTLibTest)
{
    AVB_TALKER_LIB_IT1();
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        avbt_lib_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(AVBTLibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = avbt_lib_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}
