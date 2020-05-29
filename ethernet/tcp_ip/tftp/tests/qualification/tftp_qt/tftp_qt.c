/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_tftp_rt
    @{

    @file tftp_qt.c
    @brief TFTP Component Qualification Test
    This source file contains the QT implementation for TFTP library
    @version 0.1 Initial version
*/

#include <stdlib.h>
#include <string.h>

#include "ee.h"
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>
#include "tftp_qt.h"

#include "tftp.h"

/**
    @name Component Qualification Test Sequence IDs
    @{
    @brief Component Qualification Test Sequence IDs
*/
#define BRCM_SWQTDSGN_TFTP_QTSEQUENCE0_PROC      (0xF0U)   /**< @brief #TFTP_QTSequence0  */
/** @} */

/**
    This QT is tested using Linux Test TFTP server xinetd version 2.3.15
    on Ubuntu 14.04 OS. This tool was installed using apt-get command
    on terminal. The below command was used to execute the tool
    # sudo /usr/sbin/xinetd -d -dontfork
*/

#define GetModuleLogLevel() ULOG_LVL_VERBOSE

static volatile int32_t tftp_qt1_result = BCM_AT_NOT_STARTED;

#define TFTP_CHECK_ERR_QT1(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); tftp_qt1_result = ret;}

#define TFTP_CONVERT_IP_ADDRESS(A, B, C, D) (A & 0xFFU) << 24U|(B & 0xFFU) << 16U\
                                            |(C & 0xFFU) << 8U | (D & 0xFFU)

#define TFTP_BUF_SIZE     (50UL * 1024UL)

static uint8_t COMP_SECTION(".dma.bcm.test") dataBuf[TFTP_BUF_SIZE];

TFTP_ContextType readConnection, writeConnection;

uint32_t totalSize = 0UL;

void TFTP_TEST_AlarmCb(void)
{
    uint8_t erikaRet;
    if (BCM_AT_EXECUTING == tftp_qt1_result) {
        erikaRet = SetEvent(TFTPTest, SystemEvent1);
        if (E_OK != erikaRet) {
            ULOG_ERR("Set Event on alarm failed %x \n",erikaRet);
        }
    }
}

/** @brief TFTP client test sequence

    This function tests the TFTP client interfaces.

    @behavior Sync, Non-reentrant

    @param None

    @retval void

    @trace #BRCM_SWQTSEQ_TFTP_SEQ0

    @code{.unparsed}
    TFTP_ConfigType config1 = {0x1234, 0UL,
                               TFTP_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)),
                               3UL, 300UL}
    TFTP_ConfigType config2 = {0x1234, 1UL,
                               TFTP_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)),
                               3UL, 300UL}

    retVal = TFTP_Init(&readConnection, &config1)
    Check error

    retVal = TFTP_Init(&writeConnection, &config2)
    Check error

     dataSize = TFTP_BUF_SIZE
     for (idx = 0UL; idx < TFTP_BUF_SIZE; idx++)
        dataBuf[idx] = (uint8_t)idx;

     BCM_MemCpy(fname, "test_write.bin", 15)

     retVal = TFTP_ClientWrite(&writeConnection, fname)
     Check error

     dataSize = TFTP_BUF_SIZE / 5UL

     do {
        Wait for events

         if((0UL != (mask & SystemEvent0)) ||
            (0UL != (mask & SystemEvent1)))
             uint32_t TotalDataLen
             retVal = TFTP_PutDataHandler(&writeConnection)
             if (BCM_ERR_BUSY == retVal)
                 if (TFTP_BUF_SIZE == (offset+TotalDataLen))
                    totalWrSize += TotalDataLen
                    dataSize = 0UL
                 else if (dataSize == TotalDataLen)
                    offset += dataSize
                    totalWrSize += TotalDataLen
             else if (BCM_ERR_OK == retVal)
                 break
             else
                 tftp_qt1_result = retVal
                 break
     }while(1);

     dataSize = TFTP_BUF_SIZE
     BCM_MemCpy(fname, "test_write.bin", 15)

     retVal = TFTP_ClientRead(&readConnection, fname)
     Check error

     dataSize = TFTP_BUF_SIZE / 5UL

     do {
         Wait for events

         if((0UL != (mask & SystemEvent0)) ||
            (0UL != (mask & SystemEvent1)))
             uint32_t TotalDataLen
             retVal = TFTP_GetDataHandler(&readConnection)
             if (BCM_ERR_BUSY == retVal)
                 continue
             else if (BCM_ERR_NOMEM == retVal)
                 if (TFTP_BUF_SIZE == (offset+TotalDataLen))
                    totalRdSize += TotalDataLen
                 else if (dataSize == TotalDataLen)
                    offset += dataSize
                    totalRdSize += TotalDataLen
             else if (BCM_ERR_OK == retVal)
                 totalRdSize += TotalDataLen
                 break
             else
                 tftp_qt1_result = retVal
                 break
     }while(1);

    Validate the received data

    TFTP_DeInit(&readConnection);
    TFTP_DeInit(&writeConnection);

    if (BCM_AT_EXECUTING == tftp_qt1_result) {
        tftp_qt1_result = BCM_ERR_OK
    }

    return BCM_ERR_OK
    @endcode
*/
void TFTP_QTSequence0()
{
    int32_t retVal;
    uint32_t dataSize;
    uint32_t totalWrSize = 0UL;
    uint32_t totalRdSize = 0UL;
    BCM_EventMaskType mask = 0UL;
    uint8_t fname[128];
    uint32_t idx;
    uint32_t offset = 0UL;
    TFTP_ConfigType config1 = {0x1234, 0UL,
                               TFTP_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)),
                               3UL, 300UL};
    TFTP_ConfigType config2 = {0x1234, 1UL,
                               TFTP_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)),
                               3UL, 300UL};

    ULOG_INFO("\n TFTP Client Test started\n");

    retVal = TFTP_Init(&readConnection, &config1);
    TFTP_CHECK_ERR_QT1(retVal,__FILE__, __LINE__);

    retVal = TFTP_Init(&writeConnection, &config2);
    TFTP_CHECK_ERR_QT1(retVal,__FILE__, __LINE__);

     dataSize = TFTP_BUF_SIZE;
     for (idx = 0UL; idx < TFTP_BUF_SIZE; idx++){
        dataBuf[idx] = (uint8_t)idx;
     }
     BCM_MemCpy(fname, "test_write.bin", 15);

     retVal = TFTP_ClientWrite(&writeConnection, fname);
     TFTP_CHECK_ERR_QT1(retVal,__FILE__, __LINE__);

     dataSize = TFTP_BUF_SIZE / 5UL;

     do {
         (void)BCM_WaitEvent(SystemEvent0 | SystemEvent1);
         (void)BCM_GetEvent(TFTPTest, &mask);
         (void)BCM_ClearEvent(mask);

         /* Event for an available ethernet Tx/Rx buffer or on alarm expiry */
         if((0UL != (mask & SystemEvent0)) ||
            (0UL != (mask & SystemEvent1))){
             uint32_t TotalDataLen;
             retVal = TFTP_PutDataHandler(&writeConnection, dataBuf + offset, dataSize,
                                          &TotalDataLen);
             if (BCM_ERR_BUSY == retVal) {
                 if (TFTP_BUF_SIZE == (offset+TotalDataLen)) {
                    totalWrSize += TotalDataLen;
                    dataSize = 0UL;
                 } else if (dataSize == TotalDataLen) {
                    offset += dataSize;
                    totalWrSize += TotalDataLen;
                 }
             } else if (BCM_ERR_OK == retVal) {
                 ULOG_ERR("total sent bytes %u \n",totalWrSize);
                 break;
             } else {
                 tftp_qt1_result = retVal;
                 ULOG_ERR("Error from TFTP Write State Handler %x \n",retVal);
                 break;
             }
         }
     }while(1);

     dataSize = TFTP_BUF_SIZE;
     BCM_MemCpy(fname, "test_write.bin", 15);
     retVal = TFTP_ClientRead(&readConnection, fname);
     TFTP_CHECK_ERR_QT1(retVal,__FILE__, __LINE__);

     dataSize = TFTP_BUF_SIZE / 5UL;
     offset = 0UL;

     do {
         (void)BCM_WaitEvent(SystemEvent0 | SystemEvent1);
         (void)BCM_GetEvent(TFTPTest, &mask);
         (void)BCM_ClearEvent(mask);

         /* Event for an available ethernet Tx/Rx buffer or on alarm expiry */
         if((0UL != (mask & SystemEvent0)) ||
            (0UL != (mask & SystemEvent1))){
             uint32_t TotalDataLen;
             retVal = TFTP_GetDataHandler(&readConnection, dataBuf + offset, dataSize, &TotalDataLen);
             if (BCM_ERR_BUSY == retVal) {
                 continue;
             } else if (BCM_ERR_NOMEM == retVal){
                 if (TFTP_BUF_SIZE == (offset+TotalDataLen)) {
                    totalRdSize += TotalDataLen;
                 } else if (dataSize == TotalDataLen) {
                    offset += dataSize;
                    totalRdSize += TotalDataLen;
                 }
             } else if (BCM_ERR_OK == retVal) {
                 totalRdSize += TotalDataLen;
                 ULOG_ERR("total received bytes %u \n",totalRdSize);
                 break;
             } else {
                 tftp_qt1_result = retVal;
                 ULOG_ERR("Error from TFTP Read State Handler %x \n",retVal);
                 break;
             }
         }
     }while(1);

     for (idx = 0UL; idx < TFTP_BUF_SIZE; idx++){
        if (dataBuf[idx] != (uint8_t)idx){
            tftp_qt1_result = BCM_ERR_DATA_INTEG;
        }
     }

    retVal = TFTP_DeInit(&readConnection);
    TFTP_CHECK_ERR_QT1(retVal,__FILE__, __LINE__);

    retVal = TFTP_DeInit(&writeConnection);
    TFTP_CHECK_ERR_QT1(retVal,__FILE__, __LINE__);

    if (BCM_AT_EXECUTING == tftp_qt1_result) {
        ULOG_INFO("\n TFTP Test PASSED \n");
        tftp_qt1_result = BCM_ERR_OK;
    }

    return;
}

TASK(TFTPTest)
{
    TFTP_QTSequence0();
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        tftp_qt1_result = BCM_AT_EXECUTING;
        ULOG_INFO("\n Test started\n");
        BCM_ActivateTask(TFTPTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = tftp_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}

/** @} */
