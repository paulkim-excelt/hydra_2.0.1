/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_camera_qt Qualification Tests
    @ingroup grp_camera

    @addtogroup grp_camera_qt
    @{

    @file camera_qt.c
    @brief Camera Qualification Test
    This file implements test application for camera media component.
    @version 0.1 Initial version
*/

/**
    @name Camera Qualification Test Design IDs
    @{
    @brief Camera Qualification Test Design IDs
*/
#define BRCM_SWQTDSGN_CAMERA_QT1_RESULT_GLOBAL       (0xF1UL)   /**< @brief #camera_qt1_result    */
#define BRCM_SWQTDSGN_CAMERA_QT2_RESULT_GLOBAL       (0xF2UL)   /**< @brief #camera_qt2_result    */
#define BRCM_SWQTDSGN_CAMERA_QT_TEST_CASE_NUM_GLOBAL (0xF3UL)   /**< @brief #camera_qt_test_num   */
#define BRCM_SWQTDSGN_CAMERA_QT2_ENABLED_GLOBAL      (0xF4UL)   /**< @brief #camera_qt_2_enabled  */
#define BRCM_SWQTDSGN_CAMERA_MEDIA_SYS_GLOBAL        (0xF5UL)   /**< @brief #mediaSys             */
#define BRCM_SWQTDSGN_CAMERA_MEDIA_SYS_BYPASS_GLOBAL (0xF6UL)   /**< @brief #mediaSysBypass       */
#define BRCM_SWQTDSGN_CAMERA_GET_MS_PROC             (0xF7UL)   /**< @brief #MSYS_Get       */
#define BRCM_SWQTDSGN_CAMERA_START_PROC              (0xF8UL)   /**< @brief #CameraCompTest_Start */
#define BRCM_SWQTDSGN_CAMERA_STOP_PROC               (0xF9UL)   /**< @brief #CameraCompTest_Stop  */
#define BRCM_SWQTDSGN_CAMERA_TASK_PROC               (0xFAUL)   /**< @brief #CameraTestTask       */
#define BRCM_SWQTDSGN_CAMERA_EXECUTE_AT_PROC         (0xFBUL)   /**< @brief #BCM_ExecuteAT        */
#define BRCM_SWQTDSGN_CAMERA_GET_RESULT_PROC         (0xFCUL)   /**< @brief #BCM_GetResultAT      */
#define BRCM_SWQTDSGN_CAMERA_APP_INIT_PROC           (0xFDUL)   /**< @brief #ApplicationInit      */
/** @} */

/**
    @brief Camera YUV streaming and data integrity

    configuration:
    Camera sensor: ox3a
    Resolution: 1280x720
    Output format: YUV420P
    Input: 12 bit DCG(Dual Conversion Gain) + 12 bit VS (Very Short)

    Stream YUV data from camera media component with above mentioned configuration
    and verify the data with a predefined pattern.

    CRC(First 16 lines) to be compared:
    Y CRC = 0xd8a2975b
    U CRC = 0xbdce9334
    V CRC = 0x56b74dae

    @trace #BRCM_SWREQ_CAMERA_OX3A_12DCG_12VS_HDR_720P_30FPS_CB
*/
#define BRCM_SWQTST_CAMERA_OX3A_12DCG_12VS_HDR_720P_30FPS_CB_CASE0   (0UL)

/**
    @brief Camera RAW streaming and data integrity

    configuration,
    Camera sensor: Ov10640
    Resolution: 1280x720
    Output format: RAW16
    Input: 12 bit RAW

    Stream RAW(using bypass) data from camera media component with above
    mentioned configuration and verify the data with a predefined pattern.

    CRC(First 16 lines) to be compared: CRC = 0xac82a822

    @trace #BRCM_SWREQ_CAMERA_OV10640_CSI_CB_BYPASS_720P_30FPS
*/
#define BRCM_SWQTST_CAMERA_OV10640_CSI_CB_BYPASS_720P_30FPS_CASE1   (1UL)

/** @brief Camera component's full sequence

    Exercise functionality of camera component with init, configure, start,
    stop and deinit.

    @testproc #BRCM_SWQTST_CAMERA_OX3A_12DCG_12VS_HDR_720P_30FPS_CB_CASE0
    @testproc #BRCM_SWQTST_CAMERA_OV10640_CSI_CB_BYPASS_720P_30FPS_CASE1

    @code{.unparsed}
    Initialize MeidiaSystem
    Check return and report failure if its a failure
    Configure MeidiaSystem
    Check return and report failure if its a failure
    Start MeidiaSystem
    Check return and report failure if its a failure
    Wait for result of test
    Stop MeidiaSystem
    Check return and report failure if its a failure
    Deinit MSYS_Type
    Check return and report failure if its a failure
    @endcode
*/
#define BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE   (1UL)


#define GetModuleLogLevel() ULOG_LVL_INFO

#include <stddef.h>
#include <compiler.h>
#include <string.h>
#include "media_system.h"
#include <camera.h>
#include "ee.h"
#include "ulog.h"
#include <bcm_err.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>
#include <i2c.h>
#include <bcm_time.h>

#define IIC_RETRIES         (3UL)

/**
    @brief Global context for test case 0 result

    This variable stores result for test case 0

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE
*/
static volatile int32_t camera_qt1_result = BCM_AT_NOT_STARTED;

/**
    @brief Global context for test case 1 result

    This variable stores result for test case 1

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE
*/
static volatile int32_t camera_qt2_result = BCM_AT_NOT_STARTED;

/**
    @brief Global context for active test case number

    This variable stores active test case number

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE
*/
uint32_t camera_qt_test_num = 1UL;

/**
    @brief Global context for test case 1 availability

    This variable stores the availability of test case 1

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE
*/
static uint32_t camera_qt_2_enabled = 0UL;

/**
    @brief Media system instance of test case 0

    This has the media system pipeline for test case 0

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE
*/
const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 2UL,

    .component[0].compID = MSYS_COMP_ID_CAM0,
    .component[0].numConfigs = 6UL,
    .component[0].config[0] = MSYS_CONFIG_CAM_MOD,
    .component[0].config[1] = MSYS_CONFIG_CAM_ISP,
    .component[0].config[2] = MSYS_CONFIG_CAM_UCM,
    .component[0].config[3] = MSYS_CONFIG_CAM_CSN,
    .component[0].config[4] = MSYS_CONFIG_CAM_CSN_MODE(1),
    .component[0].config[5] = MSYS_CONFIG_CAM_CTN,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_EX_SNK,
    .component[1].numConfigs = 1UL,
    .component[1].config[0] = 0UL,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 2U,
};

/**
    @brief Media system instance of test case 1

    This has the media system pipeline for test case 1

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE
*/
const MSYS_Type mediaSysBypass = {
    .magicID = MSYS_MAGIC,
    .count = 2UL,

    .component[0].compID = MSYS_COMP_ID_CAM0,
    .component[0].numConfigs = 6UL,
    .component[0].config[0] = MSYS_CONFIG_CAM_MOD+1UL,
    .component[0].config[1] = MSYS_CONFIG_CAM_ISP+1UL,
    .component[0].config[2] = MSYS_CONFIG_CAM_UCM+1UL,
    .component[0].config[3] = MSYS_CONFIG_CAM_CSN+2UL,
    .component[0].config[4] = MSYS_CONFIG_CAM_CSN_MODE(3),
    .component[0].config[5] = MSYS_CONFIG_CAM_CTN+1UL,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_EX_SNK,
    .component[1].numConfigs = 1UL,
    .component[1].config[0] = 0UL,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
};

/** @brief Get handle to active Media System instance

    This API is invoked by the framework to retrieve the media system connection
    information.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIdx       Media System Index

    @retval     MSYS_Type*   Pointer to #MSYS_Type
    @retval     NULL           Invalid index

    @post None

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE

    @limitations None

    This API is invoked by the framework to retrieve the media system connection
    information.

    @code{.unparsed}
    Check index
    if (valid)
        return pointer to media system
    @endcode

*/
const MSYS_Type* MSYS_Get(uint32_t aIdx)
{
    const MSYS_Type* mSys = NULL;

    if (0UL == aIdx) {
        mSys = &mediaSys;
    } else if (1UL == aIdx) {
        mSys = &mediaSysBypass;
    }
    return mSys;
}

/* MSYS_WdsErrorCb */
void MSYS_WdsErrorCb(const char* aName,
                     int32_t aError,
                     const WDS_MonitorType *aMonitor,
                     uint32_t aMaxAllowedErrors)
{
    ULOG_ERR("ERROR > Name %s aError %x \n", aName, aError);
    BCM_SetEvent(CameraTestTask, CAMTEST_WDS_EVENT);
}

static uint32_t JobID = 0xFFFFFFFFUL;

#define IIC_HW_ID (3U)

/* IICTransferRequest */
static int32_t IICTransferRequest(IIC_PktType *aPkts, uint32_t aNumPkts)
{
    int32_t ret = BCM_ERR_OK;

    if (0UL != aNumPkts) {
        BCM_EventMaskType mask;

        ret = IIC_Transfer(IIC_HW_ID, aPkts, aNumPkts, CAMTEST_CSN_EVENT, &JobID);
        if (ret != BCM_ERR_OK) {
            ULOG_ERR("IIC_TransferRequest failed\n");
            goto func_exit;
        }

        /* Wait for server to process the request */
        (void)BCM_WaitEvent(CAMTEST_CSN_EVENT);
        (void)BCM_GetEvent(CameraTestTask, &mask);
        if (0UL != (mask & CAMTEST_CSN_EVENT)) {
            (void)BCM_ClearEvent(CAMTEST_CSN_EVENT);
        }

        /* Fetch the response from MsgQ */
        ret = IIC_GetStatus(IIC_HW_ID, JobID);
    }

func_exit:
    return ret;
}

#define CAMERA_QT_IIC_MAX_NUM_PKTS (4U)
static IIC_PktType CAMERA_QTIICpkt[CAMERA_QT_IIC_MAX_NUM_PKTS];
static uint32_t CAMERA_QTIICPktCount;

/* CAMERA_QT_IICConstructPkt */
static int32_t CAMERA_QT_IICConstructPkt(uint32_t aSlaveAddr, uint8_t *aBuf, uint32_t aLen)
{
    int32_t ret = BCM_ERR_OK;

    if (0UL != aLen) {
        if (CAMERA_QT_IIC_MAX_NUM_PKTS > CAMERA_QTIICPktCount) {
            /* Write register address */
            CAMERA_QTIICpkt[CAMERA_QTIICPktCount].flags = (0UL);
            CAMERA_QTIICpkt[CAMERA_QTIICPktCount].slaveAddr = aSlaveAddr;
            CAMERA_QTIICpkt[CAMERA_QTIICPktCount].length = aLen;
            CAMERA_QTIICpkt[CAMERA_QTIICPktCount].data = aBuf;
            CAMERA_QTIICPktCount++;
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    } else {
        ret = BCM_ERR_OK;
    }

    return ret;
}

/** @brief Full sequence test for Camera component

    This method implements the qualification test sequence by starting the
    test, waiting for the result from sink component and stopping the
    test.

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval     void

    @post None

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE

    @limitations None

    This method performs qualification test sequence.

    @code{.unparsed}
    Call CameraCompTest_Start
    Check return and report failure if its a failure
    Wait for result of test
    Call CameraCompTest_Stop
    Check return and report failure if its a failure

    Check for all returns and test result
    Set the result to failed if any of the returns or verification fails
    @endcode

*/
#define MAX_BURST_LEN       (16UL)
TASK(CameraTestTask)
{
    BCM_EventMaskType mask = 0UL;
    int32_t ret = BCM_ERR_OK;
    const MSYS_Type *mediaSysP = NULL;
    uint32_t loopCount = 10UL;
    uint32_t i, j;
    uint8_t data[MAX_BURST_LEN];
    uint32_t slaveAddr = 0UL;
    uint32_t length = 0UL;
    uint8_t *dataBase = &data[0UL];

    if (2UL == camera_qt_test_num) {
        mediaSysP = &mediaSysBypass;
    } else {
        mediaSysP = &mediaSys;
    }

    ret = MSYS_Init(mediaSysP);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        goto func_exit;
    }

    for (i = 0UL; i < loopCount; i++) {
        ret = MSYS_Configure(mediaSysP);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("MSYS_Configure Failed %d\n",ret);
            goto func_exit;
        }

        BCM_ClearEvent(CAMERA_TEST_EVENT_PASS | CAMERA_TEST_EVENT_FAIL);
        ret = MSYS_Start(mediaSysP);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("MSYS_Start Failed %d\n",ret);
            goto func_exit;
        }

        /* Inject fault */
#ifdef ENABLE_CAM_MODULE_OX3A10
#if defined OX3A_SAFETY_INDUCE_ONLINE_PIXEL_TEST || \
    defined OX3A_SAFETY_INDUCE_TEMP_MONITOR_TEST || \
    defined OX3A_SAFETY_VFIFO_CRC_TEST || \
    defined OX3A_SAFETY_INTERNAL_REGISTER_CHECK_TEST
        slaveAddr = 0x36U;
        length = 0UL;
        CAMERA_QTIICPktCount = 0UL;

        /* Disable SCCB Write Lock */
        dataBase = &data[length];
        data[length++] = 0x31U;
        data[length++] = 0x87U;
        data[length++] = 0xffU;
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, 3UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#endif
#ifdef OX3A_SAFETY_INDUCE_ONLINE_PIXEL_TEST
        dataBase = &data[length];
        data[length++] = 0x36U;
        data[length++] = 0x00U;
        data[length++] = 0x04U;
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, 3UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#elif defined OX3A_SAFETY_INDUCE_TEMP_MONITOR_TEST
        dataBase = &data[length];
        data[length++] = 0x4dU;
        data[length++] = 0x0eU;
        data[length++] = 0x00U;
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, 3UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#elif defined OX3A_SAFETY_VFIFO_CRC_TEST
        dataBase = &data[length];
        data[length++] = 0x46U;
        data[length++] = 0x0aU;
        data[length++] = 0xffU;
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, 3UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#elif defined OX3A_SAFETY_INTERNAL_REGISTER_CHECK_TEST
        dataBase = &data[length];
        data[length++] = 0x01U;
        data[length++] = 0x00U;
        data[length++] = 0x81U;
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, 3UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#endif
#if defined OX3A_SAFETY_INDUCE_ONLINE_PIXEL_TEST || \
    defined OX3A_SAFETY_INDUCE_TEMP_MONITOR_TEST || \
    defined OX3A_SAFETY_VFIFO_CRC_TEST || \
    defined OX3A_SAFETY_INTERNAL_REGISTER_CHECK_TEST
        /* Enable SCCB Write Lock */
        dataBase = &data[length];
        data[length++] = 0x31U;
        data[length++] = 0x87U;
        data[length++] = 0x00U;
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, 3UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#else
        /* Dummy Call */
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, length);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#endif
#else
        ret = CAMERA_QT_IICConstructPkt(slaveAddr, dataBase, length);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("CAMERA_QT_IICConstructPkt Failed %d\n",ret);
            goto func_exit;
        }
#endif /* ENABLE_CAM_MODULE_OX3A10 */

        for (j = 0UL; j < IIC_RETRIES; j++) {
            ret = IICTransferRequest(CAMERA_QTIICpkt, CAMERA_QTIICPktCount);
            if (ret == BCM_ERR_OK) {
                break;
            }
        }
        if (ret != BCM_ERR_OK) {
            ULOG_ERR("IICTransferRequest: failed\n");
            goto func_exit;
        }

        /* Wait for result */
        BCM_WaitEvent(CAMERA_TEST_EVENT_PASS | CAMERA_TEST_EVENT_FAIL | CAMTEST_WDS_EVENT);
        BCM_GetEvent(CameraTestTask, &mask);
        BCM_ClearEvent(mask);

        if (0UL != (mask & CAMTEST_WDS_EVENT)) {
            ret = MSYS_Stop(mediaSysP);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("MSYS_Stop Failed %d\n",ret);
                goto func_exit;
            }

            ret = MSYS_DeInit(mediaSysP);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("MSYS_DeInit Failed %d\n",ret);
            }

            ret = MSYS_Init(mediaSysP);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("MSYS_Init Failed %d\n",ret);
                goto func_exit;
            }

            ret = MSYS_Configure(mediaSysP);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("MSYS_Configure Failed %d\n",ret);
                goto func_exit;
            }

            BCM_ClearEvent(CAMERA_TEST_EVENT_PASS | CAMERA_TEST_EVENT_FAIL);
            ret = MSYS_Start(mediaSysP);
            if (BCM_ERR_OK != ret) {
                ULOG_ERR("MSYS_Start Failed %d\n",ret);
                goto func_exit;
            }

            /* Wait for result */
            BCM_WaitEvent(CAMERA_TEST_EVENT_PASS | CAMERA_TEST_EVENT_FAIL);
            BCM_GetEvent(CameraTestTask, &mask);
            BCM_ClearEvent(mask);
        }

        if (0UL != (mask & CAMERA_TEST_EVENT_FAIL)) {
            if (2UL == camera_qt_test_num) {
                camera_qt2_result = BCM_ERR_UNKNOWN;
            } else {
                camera_qt1_result = BCM_ERR_UNKNOWN;
            }
        } else if (0UL != (mask & CAMERA_TEST_EVENT_PASS)) {
            if (2UL == camera_qt_test_num) {
                camera_qt2_result = BCM_ERR_OK;
            } else {
                camera_qt1_result = BCM_ERR_OK;
            }
        }

        ret = MSYS_Stop(mediaSysP);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("MSYS_Stop Failed %d\n",ret);
            goto func_exit;
        }

        if (2UL == camera_qt_test_num) {
            if (BCM_ERR_OK != camera_qt2_result) {
                break;
            }
        } else {
            if (BCM_ERR_OK != camera_qt1_result) {
                break;
            }
        }

        /* Delay for WDS monitor to execute */
        BCM_CpuNDelay(100UL * 1000UL * 1000UL);
    }
    ret = MSYS_DeInit(mediaSysP);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_DeInit Failed %d\n",ret);
    }
    if (2UL == camera_qt_test_num) {
        if (BCM_ERR_OK == camera_qt2_result) {
            ULOG_INFO("Test Passed\n");
        } else {
            ULOG_INFO("Test Failed\n");
        }
    } else {
        if (BCM_ERR_OK == camera_qt1_result) {
            ULOG_INFO("Test Passed\n");
        } else {
            ULOG_INFO("Test Failed\n");
        }
    }
func_exit:
    if (BCM_ERR_OK != ret) {
        if (2UL == camera_qt_test_num) {
            camera_qt2_result = ret;
        } else {
            camera_qt1_result = ret;
        }
    }

    BCM_TerminateTask();
}

/** @brief Trigger test task

    This method triggers test task for appropriate test case

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aIndex       Test case index

    @retval     #BCM_ERR_OK           Test case trigger is successful
    @retval     #BCM_AT_NOT_AVAILABLE Test case is is not available

    @post None

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE

    @limitations None

    This method checks for the test case index and trigger the test task

    @code{.unparsed}
    Check validity of index
    Return BCM_AT_NOT_AVAILABLE if not valid
    If valid,
    Set global active test case id
    Set test case state to executing
    Activate test task
    @endcode

*/
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
#ifdef ENABLE_CAM_MODULE_OV10640
    camera_qt_2_enabled = 1UL;
#endif
    if (1UL == aIndex) {
        camera_qt1_result = BCM_AT_EXECUTING;
        camera_qt_test_num = 1UL;
        BCM_ActivateTask(CameraTestTask);
        ret = BCM_ERR_OK;
    } else if ((2UL == aIndex) &&
               (0UL != camera_qt_2_enabled)) {
        camera_qt2_result = BCM_AT_EXECUTING;
        camera_qt_test_num = 2UL;
        BCM_ActivateTask(CameraTestTask);
        ret = BCM_ERR_OK;
    }

    return ret;
}

/** @brief Get result for the test case

    This method retrieves result for the test case

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aIndex       Test case index

    @retval     #BCM_ERR_OK           Test case result retrieval is successful
    @retval     #BCM_AT_NOT_AVAILABLE Test case is is not available

    @post None

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE

    @limitations None

    This method retrieves result for the test case

    @code{.unparsed}
    Check validity of index
    Return BCM_AT_NOT_AVAILABLE if not valid
    If valid,
    return global test case result
    @endcode

*/
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;

    if (1UL == aIndex) {
        ret = camera_qt1_result;
    } else if (2UL == aIndex) {
        ret = camera_qt2_result;
    }

    return ret;
}

/** @brief Initialize application

    This API initializes the application.

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @retval      void

    @post None

    @trace #BRCM_SWQTSEQ_CAMERA_FULL_SEQUENCE

    @limitations None

    This API is invoked by the framework to initialize the application

    @code{.unparsed}
    Activate command process task for sink component
    @endcode

*/
void ApplicationInit(void)
{
    BCM_ActivateTask(ExSnk_CmdProcessTask);
}
/** @} */
