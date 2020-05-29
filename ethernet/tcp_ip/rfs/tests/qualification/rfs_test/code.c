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

/* Includes */
#include "ee.h"
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>

#include <rfs.h>

extern uint32_t GetAPPLogLevel();
#define GetModuleLogLevel   GetAPPLogLevel

uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}

int32_t rfs_qt_result = BCM_AT_NOT_STARTED;

#define RFS_TEST_CHUNK_SIZE (512UL + 256UL) /* To test both multiple of 512 and non multiple of 512 */
#define RFS_TEST_NUM_CHUNKS (4UL)

#define RFS_TEST_DATA_SIZE  (RFS_TEST_CHUNK_SIZE * RFS_TEST_NUM_CHUNKS)
#define RFS_TEST_EVENT      (SystemEvent3)

uint8_t COMP_SECTION(".dma.bcm.test.buffer") RFS_TestData[RFS_TEST_DATA_SIZE];
uint8_t COMP_SECTION(".dma.bcm.test.buffer") RFS_TestDataRx[RFS_TEST_DATA_SIZE];

RFS_FileHandleType RFS_FileHandle;

void RFS_WaitForEvent(uint32_t aMask) {
    int32_t ret;
    BCM_EventMaskType mask = 0UL;
    ret = BCM_WaitEvent(aMask);
    if (BCM_ERR_OK != ret) {
        ULOG_INFO("BCM_WaitEvent failed\n");
    }
    ret = BCM_GetEvent(RFS_Test,&mask);
    if (BCM_ERR_OK != ret) {
        ULOG_INFO("BCM_GetEvent failed\n");
    }
    ret = BCM_ClearEvent(mask);
    if (BCM_ERR_OK != ret) {
        ULOG_INFO("BCM_ClearEvent failed\n");
    }
}
int32_t RFSTest()
{
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;
    uint8_t fileName[RFS_MAX_FILENAME_LEN] = "rfs_test_data.bin";
    uint8_t fileName1[RFS_MAX_FILENAME_LEN] = "test_random.bin";


    /* prepare input data */
    for (i = 0UL; i < RFS_TEST_DATA_SIZE; i++) {
        RFS_TestData[i] = (uint8_t) (i);
    }

    /* open a file for writing */
    retVal = RFS_Open(&RFS_FileHandle,
                      fileName,
                      RFS_MODE_WRITE);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Open failed (%d)\n",retVal);
        goto exit;
    }

    for (i = 0UL; i < RFS_TEST_NUM_CHUNKS; i++) {
        /* write the data */
        retVal = RFS_Write(&RFS_FileHandle,
                           &RFS_TestData[i * RFS_TEST_CHUNK_SIZE],
                           RFS_TEST_CHUNK_SIZE,
                           RFS_TEST_EVENT);
        if (BCM_ERR_OK != retVal) {
            ULOG_INFO("RFS_Write failed (%d)\n",retVal);
            goto exit;
        }

        do {
            uint32_t len;
            retVal = RFS_GetStatus(&RFS_FileHandle,&len);
            if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
                ULOG_INFO("RFS_GetStatus failed (%d)\n",retVal);
                goto exit;
            }
            if ((BCM_ERR_OK == retVal) && (RFS_TEST_CHUNK_SIZE != len)) {
                retVal = BCM_ERR_DATA_INTEG;
                ULOG_INFO("RFS_Write failed. Asked %d written %d \n",RFS_TEST_CHUNK_SIZE,len);
                goto exit;
            }
            if (BCM_ERR_BUSY == retVal) {
                RFS_WaitForEvent(RFS_TEST_EVENT);
            }
        } while (BCM_ERR_BUSY == retVal);
    }
    /* close file */
    do {
        retVal = RFS_Close(&RFS_FileHandle);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_Close failed (%d)\n",retVal);
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);

    /* Read the same file */
    /* open a file for reading */
    retVal = RFS_Open(&RFS_FileHandle,
                      fileName,
                      RFS_MODE_READ);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Open failed (%d)\n",retVal);
        goto exit;
    }

    for (i = 0UL; i < RFS_TEST_NUM_CHUNKS; i++) {
        /* read the data */
        retVal = RFS_Read(&RFS_FileHandle,
                          &RFS_TestDataRx[i * RFS_TEST_CHUNK_SIZE],
                          RFS_TEST_CHUNK_SIZE,
                          RFS_TEST_EVENT);
        if (BCM_ERR_OK != retVal) {
            ULOG_INFO("RFS_Read failed (%d)\n",retVal);
            goto exit;
        }

        do {
            uint32_t len;
            retVal = RFS_GetStatus(&RFS_FileHandle,&len);
            if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
                ULOG_INFO("RFS_GetStatus failed (%d)\n",retVal);
                goto exit;
            }
            if ((BCM_ERR_OK == retVal) && (RFS_TEST_CHUNK_SIZE != len)) {
                retVal = BCM_ERR_DATA_INTEG;
                ULOG_INFO("RFS_Read failed. Asked %d written %d \n",RFS_TEST_CHUNK_SIZE,len);
                goto exit;
            }
            if (BCM_ERR_BUSY == retVal) {
                RFS_WaitForEvent(RFS_TEST_EVENT);
            }
        } while (BCM_ERR_BUSY == retVal);
    }

    /* close file */
    do {
        retVal = RFS_Close(&RFS_FileHandle);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_Close failed (%d)\n",retVal);
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);


    /* verify data */
    for (i = 0UL; i < RFS_TEST_DATA_SIZE; i++) {
        if (RFS_TestData[i] != RFS_TestDataRx[i]) {
            retVal = BCM_ERR_DATA_INTEG;
            goto exit;
        }
    }

    /* prepare data for next test */
    for (i = 0UL; i < RFS_TEST_DATA_SIZE; i++) {
        RFS_TestData[i] = 0xFFU;
    }

    /* open in append mode and add data */
    retVal = RFS_Open(&RFS_FileHandle,
                      fileName,
                      RFS_MODE_APPEND);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Open failed (%d)\n",retVal);
        goto exit;
    }

    /* write the data */
    retVal = RFS_Write(&RFS_FileHandle,
                      RFS_TestData,
                      RFS_TEST_DATA_SIZE,
                      RFS_TEST_EVENT);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Write failed (%d)\n",retVal);
        goto exit;
    }

    do {
        uint32_t len;
        retVal = RFS_GetStatus(&RFS_FileHandle,&len);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_GetStatus failed (%d)\n",retVal);
            goto exit;
        }
        if ((BCM_ERR_OK == retVal) && (RFS_TEST_DATA_SIZE != len)) {
            retVal = BCM_ERR_DATA_INTEG;
            ULOG_INFO("RFS_Write failed. Asked %d written %d \n",RFS_TEST_DATA_SIZE,len);
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);

    /* close file */
    do {
        retVal = RFS_Close(&RFS_FileHandle);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_Close failed (%d)\n",retVal);
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);

    /* Read the same file and seek to middle*/
    /* open a file for reading */
    retVal = RFS_Open(&RFS_FileHandle,
                      fileName,
                      RFS_MODE_READ);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Open failed (%d)\n",retVal);
        goto exit;
    }

    /* seek set */
    retVal = RFS_Seek(&RFS_FileHandle,
                      RFS_SEEK_SET,
                      RFS_TEST_DATA_SIZE);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Seek failed (%d)\n",retVal);
        goto exit;
    }

    /* read the data */
    retVal = RFS_Read(&RFS_FileHandle,
                      RFS_TestDataRx,
                      RFS_TEST_DATA_SIZE,
                      RFS_TEST_EVENT);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Read failed (%d)\n",retVal);
        goto exit;
    }

    do {
        uint32_t len;
        retVal = RFS_GetStatus(&RFS_FileHandle,&len);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_GetStatus failed (%d)\n",retVal);
            goto exit;
        }
        if ((BCM_ERR_OK == retVal) && (RFS_TEST_DATA_SIZE != len)) {
            retVal = BCM_ERR_DATA_INTEG;
            ULOG_INFO("RFS_Read failed. Asked %d written %d \n",RFS_TEST_DATA_SIZE,len);
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);

    /* close file */
    do {
        retVal = RFS_Close(&RFS_FileHandle);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_Close failed (%d)\n",retVal);
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);


    /* verify data */
    for (i = 0UL; i < RFS_TEST_DATA_SIZE; i++) {
        if (RFS_TestData[i] != RFS_TestDataRx[i]) {
            retVal = BCM_ERR_DATA_INTEG;
            goto exit;
        }
    }


    /* Read a file not present */
    retVal = RFS_Open(&RFS_FileHandle,
                      fileName1,
                      RFS_MODE_READ);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Open failed (%d)\n",retVal);
        goto exit;
    }

    /* read the data */
    retVal = RFS_Read(&RFS_FileHandle,
                      RFS_TestDataRx,
                      RFS_TEST_DATA_SIZE,
                      RFS_TEST_EVENT);
    if (BCM_ERR_OK != retVal) {
        ULOG_INFO("RFS_Read failed (%d)\n",retVal);
        goto exit;
    }

    do {
        uint32_t len;
        retVal = RFS_GetStatus(&RFS_FileHandle,&len);
        if ((BCM_ERR_NOT_FOUND != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_GetStatus failed %d\n",retVal);
            goto exit;
        }
        if (BCM_ERR_OK == retVal) {
            retVal = BCM_ERR_UNKNOWN;
            ULOG_INFO("RFS_Read failed. Negative test %d \n");
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);

    /* Overwrite the error for negative test case */
    retVal = BCM_ERR_OK;

    /* close file */
    do {
        retVal = RFS_Close(&RFS_FileHandle);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
            ULOG_INFO("RFS_Close failed (%d)\n",retVal);
            goto exit;
        }
        if (BCM_ERR_BUSY == retVal) {
            RFS_WaitForEvent(RFS_TEST_EVENT);
        }
    } while (BCM_ERR_BUSY == retVal);

exit:
    return retVal;
}

TASK(RFS_Test)
{
    rfs_qt_result = RFSTest();
    ULOG_INFO("\nTest Completed (%d)\n",rfs_qt_result);
    BCM_TerminateTask();

}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        rfs_qt_result = BCM_AT_EXECUTING;
        ULOG_INFO("\nTest started \n");
        BCM_ActivateTask(RFS_Test);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = rfs_qt_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}
