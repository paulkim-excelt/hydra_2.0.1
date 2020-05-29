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
#include <bcm_time.h>
#include <console.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

#include "avb_listener.h"
#include "avb_core.h"

/**
  @addtogroup grp_avb_qt
  @{
  @file avblistenerlib_qt.c
  @brief 1722 Listener qualification Test
  This source file contains the qualification tests for 1722 Listener
  interface of the AVB library
*/

/**
    @name Qualification Test Sequence IDs
    @{
    @brief Qualification Test Sequence IDs
*/
#define BRCM_SWQTDSGN_AVB_LSTR_SEQ0_PROC (0xF0UL) /**< @brief #AVB_LISTENER_LIB_IT1 */
/** @} */

/**
    @brief Parsing test

    This is to validate the 1722 packet parsing capability of the
    AVB library.

    @trace #BRCM_SWREQ_AVB_1722
*/
#define BRCM_SWQTST_AVB_LSTR_CONFIG_CASE0   (1UL)

/** @brief 1722 Parsing Test Functionality

    This test sequence will verify the 1722 packet parsing feature of the
    AVB library. The ability of the library to handle different
    payload segments within the 1722 packet is verified.

    @testproc #BRCM_SWQTST_AVB_LSTR_CONFIG_CASE0

    @code{.unparsed}
    Run this (for BRCM_SWQTST_AVB_LSTR_CONFIG_CASE0)
    @endcode

*/
#define BRCM_SWQTSEQ_AVB_LSTR_SEQ0   (1UL)

/** @brief 1722 packet parsing Test

    This test validates the 1722 packet parsing interface of
    AVB library.

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return void

    @post None

    @trace #BRCM_SWQTSEQ_AVB_LSTR_SEQ0

    @limitations None

    @code{.c}
    retVal = BCM_ERR_OK

    For a loop of 10 times
        uint8_t *buf = NULL
        Update the stream params to #AVB_1722LstrBufParamsType
        #buf[AVBL_SEQNUM_OFFSET] = update the packet sequence number
        #buf[AVBL_TIMESTAMP_OFFSET] = Update the timestamp into the
                                        1722 header in BE format
        retVal = #AVB_Extract1722Header to parse #buf
        #AVB_CHECK_ERR to check retVal
        retVal = #AVBL_ValidateOutputPacket to check the parsed params
        #AVB_CHECK_ERR to check retVal

    Update test execution status
    return

    @endcode
*/
void AVB_LISTENER_LIB_IT1(void);
/** @} */

#define AVBL_TEST_COUNT            (10UL)
#define AVBL_SEQNUM_OFFSET         (0x14U)
#define AVBL_TIMESTAMP_OFFSET      (0x1EU)
#define AVB_AVTP_PAYLOAD_SIZE      (1440UL)

#define GetModuleLogLevel() ULOG_LVL_VERBOSE

static volatile int32_t avbl_lib_qt1_result = BCM_AT_NOT_STARTED;

#define AVB_CHECK_ERR(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); avbl_lib_qt1_result = ret;}

/* Sample 1722 packet with header */
uint8_t inpDataBuf[1600] = {0x03, 0x80, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04,
                            0x05, 0x06, 0x07, 0x08, 0x11, 0x22, 0x33, 0x44,
                            0x02, 0x01, 0x00, 0x00, 0x05, 0xC4, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x7C, 0x85};
uint8_t outDataBuf[1600] = {0x0};

static int32_t AVBL_ValidateOutputPacket(
                                AVB_1722LstrBufParamsType *pktParams);

int32_t AVBL_ValidateOutputPacket(AVB_1722LstrBufParamsType *pktParams)
{
    int32_t retVal = BCM_ERR_OK;

    /* Verify last sequence num flag */
    if ((0UL == pktParams->payDetails.pktSeqNum) ||
        (0UL == pktParams->payDetails.payLoadLen) ||
        (0UL == pktParams->payDetails.payLoadOffset)){
        retVal = BCM_ERR_UNKNOWN;
        AVB_CHECK_ERR(retVal,__FILE__, __LINE__);
    }
    return retVal;
}

void AVB_LISTENER_LIB_IT1(void)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t count = 0UL;
    uint32_t packetTS = 0UL;
    AVB_1722LstrBufParamsType pktParams;

    /* Check packet for AVBL_TEST_COUNT */
    for (;count < AVBL_TEST_COUNT; count++) {

        uint8_t *buf = NULL;

        buf = &inpDataBuf[0];
        packetTS = (count+1UL) * 1000;

        /* Update the sequence number for each packet */
        buf[AVBL_SEQNUM_OFFSET] = (uint8_t)(count + 1UL);

        /* Update the timestamp into the 1722 header in BE format */
        buf[AVBL_TIMESTAMP_OFFSET]    = (uint8_t)
                                            ((packetTS >> 24U)& 0xFFU);
        buf[AVBL_TIMESTAMP_OFFSET+1U] = (uint8_t)
                                            ((packetTS >> 16U)& 0xFFU);
        buf[AVBL_TIMESTAMP_OFFSET+2U] = (uint8_t)
                                            ((packetTS >> 8U)& 0xFFU);
        buf[AVBL_TIMESTAMP_OFFSET+3U] = (uint8_t)
                                            ((packetTS) & 0xFFU);

        /* Reset flags for next call */
        pktParams.payDetails.isTSUncertain = 0UL;
        pktParams.payDetails.isSliceEnd = 0UL;

        /* Invoke API under test */
        retVal = AVB_Extract1722Header(buf,
                                        1518UL,
                                        &pktParams);
        AVB_CHECK_ERR(retVal,__FILE__, __LINE__);

        /* Validate the output buffer */
        retVal = AVBL_ValidateOutputPacket(&pktParams);
        AVB_CHECK_ERR(retVal,__FILE__, __LINE__);

    }

    if (BCM_AT_EXECUTING == avbl_lib_qt1_result) {
        avbl_lib_qt1_result = BCM_ERR_OK;
    }

    ULOG_ERR("AVB_LISTENER_LIB_IT1 Completed\n");
    return;
}

TASK(AVBLLibTest)
{
    AVB_LISTENER_LIB_IT1();
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        avbl_lib_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(AVBLLibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = avbl_lib_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}
