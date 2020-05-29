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

/**
    @defgroup grp_i2c_qt Qualification Tests
    @ingroup grp_i2cdrv

    @addtogroup grp_i2c_qt
    @{

    @file comp_qt.c
    @brief I2C driver Qualification test
    This source file QT for the I2C driver
    @version 0.1 Changelist0
*/

#include <stdlib.h>
#include <string.h>
#include <ulog.h>
#include <bcm_test.h>
#include <bcm_err.h>
#include <i2c_osil.h>
#include <shell.h>
#include <osil/bcm_osil.h>
#include "ee.h"


/**
    @brief I2C Driver Initialization and De-Initialization tests

    @pre None

    @test Test the init and de-init sequence of I2C Driver

    @post Driver should get initialized and de-initialized successfully.

    @trace #BRCM_SWREQ_IIC_INITIALIZATION
*/
#define BRCM_SWQTST_IIC_CONFIG_CASE0   (0UL)

/**
    @brief I2C Driver Communication test

    @pre I2C driver should be initialized before.

    @test Test the communication sequence of I2C Driver

    @post Driver should be able to communicate with the slave device successfully.

    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
#define BRCM_SWQTST_IIC_CONFIG_CASE1   (1UL)

/**
    @brief I2C Driver Communication test

    @pre I2C driver should be initialized before.

    @test Test the interrupt sequence of I2C Driver

    @post Driver should be able to communicate with the slave device successfully.

    @trace #BRCM_SWREQ_IIC_INTERRUPT_PROCESS
*/
#define BRCM_SWQTST_IIC_CONFIG_CASE2   (2UL)

/**
 * IIC slave specific
 */
#define PCA9673_IIC_HW_ID   (0UL)
#define PCA9673_DEVID_ADDR  (0xF8U >> 1U)
#define PCA9673_RESET_ADDR  (0x00U >> 1U)
#define PCA9673_SLAVE_ADDR  (0x4AU >> 1U)
#define PCA9673_RESET_DATA  (0x06U)
#define PCA9673_PORT_SIZE   (0x02U)
#define PCA9673_13TO15_MASK (0xE0U)

#define IIC_HW_ID           (PCA9673_IIC_HW_ID)
#define IIC_SLV_10BIT       (0U)
#define IIC_SPEED           (IIC_SPEED_100K)
#define IIC_RETRIES         (3UL)
#define IIC_SLAVE_ADDR      (PCA9673_SLAVE_ADDR)

#define DEV_ID_LEN          (3UL)
#define PORT_READ_LEN       (4UL)
#define PORT_WRITE_LEN      (PORT_READ_LEN)
#define MAX_BURST_LEN       (6UL)

#define IIC_IT1             (1UL)

#define GetModuleLogLevel() ULOG_LVL_ERROR

static int32_t IICIT1_Result = BCM_AT_NOT_STARTED;
static uint8_t DeviceIdVal[DEV_ID_LEN] = {0x00, 0x02, 0x20};

static uint32_t JobID = 0xFFFFFFFFUL;

static int32_t IICTransferRequest(IIC_PktType *aPkts, uint32_t aNumPkts)
{
    BCM_EventMaskType mask;
    int32_t ret;

    ULOG_ERR("IICTransferRequest++\n");

    ret = IIC_Transfer(IIC_HW_ID, aPkts, aNumPkts, IICTestEvent, &JobID);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("IIC_TransferRequest failed\n");
        goto err_exit;
    }

    /* Wait for server to process the request */
    (void)BCM_WaitEvent(IICTestEvent);
    (void)BCM_GetEvent(IICIT_Task, &mask);
    if (0UL != (mask & IICTestEvent)) {
        (void)BCM_ClearEvent(IICTestEvent);
    }

    /* Fetch the response from MsgQ */
    ret = IIC_GetStatus(IIC_HW_ID, JobID);
err_exit:
    return ret;
}

static int32_t IICReadPCA9673(uint32_t aSlaveAddr, uint8_t *aBuf, uint32_t len)
{
    int32_t ret;
    IIC_PktType pkt[1];

    /* Read register address */
    pkt[0].flags = (IIC_PKT_FLG_OP_RD);
    pkt[0].slaveAddr = aSlaveAddr;
    pkt[0].length = len;
    pkt[0].data = aBuf;

    ret = IICTransferRequest(pkt, 1);
    return ret;
}

static int32_t IICWritePCA9673(uint32_t aSlaveAddr, uint8_t *aBuf, uint32_t len)
{
    int32_t ret;
    IIC_PktType pkt[1];

    /* Write register address */
    pkt[0].flags = (0UL);
    pkt[0].slaveAddr = aSlaveAddr;
    pkt[0].length = len;
    pkt[0].data = aBuf;

    ret = IICTransferRequest(pkt, 1);
    return ret;
}

/*
static int32_t IICReadSingle(uint32_t aSlaveAddr, uint32_t aRegAddr,
        uint8_t *aBuf)
{
    int32_t ret;
    IIC_PktType pkt[2];
    uint8_t reg;

        Write register address
    pkt[0].flags = (0UL);
    pkt[0].slaveAddr = IIC_SLAVE_ADDR;
    pkt[0].length = 1UL;
    reg = (uint8_t)(aRegAddr & 0xFFUL);
    pkt[0].data = &reg;

        Read data
    pkt[1].flags = (IIC_PKT_FLG_OP_RD | IIC_PKT_FLG_RESTART);
    pkt[1].slaveAddr = IIC_SLAVE_ADDR;
    pkt[1].length = 1UL;
    pkt[1].data = aBuf;

    ret = IICTransferRequest(pkt, 2);
    return ret;
}*/

static int32_t IICReadBurst(uint32_t aSlaveAddr, uint32_t aRegAddr,
        uint8_t *aBuf, uint32_t aLen)
{
    int32_t ret;
    IIC_PktType pkt[2];
    uint8_t reg;

    /* Write register address - Device ID Addr */
    pkt[0].flags = (0UL);
    pkt[0].slaveAddr = aSlaveAddr;
    pkt[0].length = 1UL;
    reg = (uint8_t)aRegAddr;
    pkt[0].data = &reg;

    /* Read data */
    pkt[1].flags = (IIC_PKT_FLG_OP_RD | IIC_PKT_FLG_RESTART);
    pkt[1].slaveAddr = aSlaveAddr;
    pkt[1].length = aLen;
    pkt[1].data = aBuf;

    ret = IICTransferRequest(pkt, 2);
    return ret;
}

static int32_t IICInit(void)
{
    IIC_ConfigType config;
    config.speed = IIC_SPEED;
    config.txFifoMode = 0;

    IIC_Init(IIC_HW_ID, &config);

    return BCM_ERR_OK;
}

static int32_t IICDeInit(void)
{
    int32_t ret;

    ULOG_ERR("DeInit IIC\n");
    ret = IIC_DeInit(IIC_HW_ID);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("IIC_DeInitRequest failed!!\n");
    }
    ULOG_ERR("IIC DeInit done\n");
    return ret;
}



/** @brief Brief description of BRCM_SWQTSEQ_IIC_SEQ0

    Detailed description of BRCM_SWQTSEQ_IIC_SEQ0

    @code{.c}
    Initialize the IIC driver
    Read a single data from the slave device by passing valid address
    Verify that the IIC driver is generating a valid interrupt
    Compare the read data with the reference value
    Read a burst of data from the slave device by passing valid addresses
    Verify that the IIC driver is generating valid interrupts
    Compare the read data with the reference values
    De-Initialize the IIC driver
    @endcode

    @board bcm89103_0x3a_evk
    @board bcm89107_ox3a_evk
    @board bcm89107_ar143_evk
    @board bcm89108_ddr_ov10640_evk

    @testproc #BRCM_SWQTST_IIC_CONFIG_CASE0
    @testproc #BRCM_SWQTST_IIC_CONFIG_CASE1
    @testproc #BRCM_SWQTST_IIC_CONFIG_CASE2

*/
#define BRCM_SWQTSEQ_IIC_SEQ0   (1UL)


/** @brief Brief description of IIC_QTSequence0

    Test the positive functionality of I2C driver. The test shall:
    - Initialize the driver
    - Communicate with the driver
    - Verify whether interrupts are generated
    - Validate the data read from the driver
    - De-Initialize the driver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      Task ID     Task ID for the I2C task

    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWQTSEQ_IIC_SEQ0

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
TASK(IICIT_Task)
{
    uint32_t i;
    uint8_t data[MAX_BURST_LEN];
    int32_t err = BCM_ERR_OK;
    uint8_t readData[PCA9673_PORT_SIZE];

    ULOG_ERR("IIC TEST: START\n");

    ULOG_ERR("IIC TEST: Initialize IIC\n");
    err = IICInit();

    if (err != BCM_ERR_OK) {
        goto err_exit;
    }

    /* Reset PCA9673 using single write */
    for (i = 0UL; i < IIC_RETRIES; i++) {
        data[0] = PCA9673_RESET_DATA;
        err = IICWritePCA9673(PCA9673_RESET_ADDR, data, 1UL);
        if (err == BCM_ERR_OK) {
            break;
        }
    }
    if (err != BCM_ERR_OK) {
        ULOG_ERR("IIC TEST: Reset PCA9673 failed\n");
        goto err_exit;
    } else {
        ULOG_ERR("PCA9673 Reset Done \n");
    }

    /* Read Device ID using burst read */
    for (i = 0UL; i < IIC_RETRIES; i++) {
        err = IICReadBurst(PCA9673_DEVID_ADDR, (PCA9673_SLAVE_ADDR << 1), data, DEV_ID_LEN);
        if (err == BCM_ERR_OK) {
            break;
        }
    }
    if (err != BCM_ERR_OK) {
        ULOG_ERR("IIC TEST: Burst read failed\n");
        goto err_exit;
    }

    ULOG_ERR("DATA:", data[i]);
    for (i = 0UL; i < DEV_ID_LEN; i++) {
        ULOG_ERR(" %02x", data[i]);
        if (data[i] != DeviceIdVal[i]) {
            ULOG_ERR("IIC TEST: Device ID mismatch!!\n");
            err = BCM_ERR_DATA_INTEG;
            goto err_exit;
        }
    }
    ULOG_ERR("\n");

    /* Read Port Data */
    for (i = 0UL; i < IIC_RETRIES; i++) {
        err = IICReadPCA9673(PCA9673_SLAVE_ADDR, data, PORT_READ_LEN);
        if (err == BCM_ERR_OK) {
            break;
        }
    }
    if (err != BCM_ERR_OK) {
        ULOG_ERR("IIC TEST: PCA9673 Port read failed\n");
        goto err_exit;
    }

    ULOG_ERR("Port Data:");
    for (i = 0UL; i < PORT_READ_LEN; i++) {
        ULOG_ERR(" %02x", data[i]);
    }
    ULOG_ERR("\n");
    if (data[0] != data[2]
        || data[1] != data[3]) {
        ULOG_ERR("IIC TEST: Port Read mismatch!!\n");
        err = BCM_ERR_DATA_INTEG;
        goto err_exit;
    }
    readData[0] = data[0];
    data[1] ^= (PCA9673_13TO15_MASK);
    data[3] ^= (PCA9673_13TO15_MASK);
    readData[1] = data[1];

    /* Write Port Data */
    for (i = 0UL; i < IIC_RETRIES; i++) {
        err = IICWritePCA9673(PCA9673_SLAVE_ADDR, data, PORT_READ_LEN);
        if (err == BCM_ERR_OK) {
            break;
        }
    }
    if (err != BCM_ERR_OK) {
        ULOG_ERR("IIC TEST: PCA9673 Port write failed\n");
        goto err_exit;
    } else {
        ULOG_ERR("PCA9673 Port write done \n");
    }

    /* Read Port Data again */
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0xFF;
    data[3] = 0xFF;
    for (i = 0UL; i < IIC_RETRIES; i++) {
        err = IICReadPCA9673(PCA9673_SLAVE_ADDR, data, PORT_READ_LEN);
        if (err == BCM_ERR_OK) {
            break;
        }
    }
    if (err != BCM_ERR_OK) {
        ULOG_ERR("IIC TEST: PCA9673 Port re-read failed\n");
        goto err_exit;
    }

    ULOG_ERR("Port Data:");
    for (i = 0UL; i < PORT_READ_LEN; i++) {
        ULOG_ERR(" %02x", data[i]);
    }
    ULOG_ERR("\n");
    if (readData[0] != data[0]
        || readData[1] != data[3]) {
        ULOG_ERR("IIC TEST: Port re-read mismatch!!\n");
        err = BCM_ERR_DATA_INTEG;
        goto err_exit;
    }

    err = IICDeInit();

err_exit:
    if (err == BCM_ERR_OK) {
        ULOG_ERR("IIC TEST: PASSED!!\n");
        IICIT1_Result = BCM_ERR_OK;
    } else {
        ULOG_ERR("IIC TEST: FAILED!!\n");
        IICIT1_Result = err;
    }

    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex)
    {
    case IIC_IT1:
        if (BCM_AT_EXECUTING != IICIT1_Result) {
            IICIT1_Result = BCM_AT_EXECUTING;
            (void)BCM_ActivateTask(IICIT_Task);
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;

    switch(aIndex)
    {
    case IIC_IT1:
        retVal = IICIT1_Result;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

void ApplicationInit(void)
{
    /* Dummy Init */
}

/** @} */
