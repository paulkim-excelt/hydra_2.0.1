/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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

#include <string.h>
#include <mcu.h>
#include <clk.h>
#include <log.h>
#include <bcm_err.h>
#include <flash.h>

#define PAGE_SIZE       (256UL)
#define SECTOR_SIZE     (64UL * 1024UL)
#define SUBSECTOR_SIZE  (4UL * 1024UL)
#define FLASH0_SIZE     (2 * 1024 * 1024)
#define FLASH1_SIZE     (2 * 1024 * 1024)

#define GetModuleLogLevel()     (LOG_LVL_INFO)

uint8_t rxBuf[20UL * PAGE_SIZE];
uint8_t txBuf[2UL * PAGE_SIZE];

#ifdef __BCM8910X__
#define BL_FLASH0_SPEED    (FLASH_SPI_SPEED_80M)
#define BL_FLASH1_SPEED    (FLASH_SPI_SPEED_25M)

#if (BL_FLASH0_SPEED == FLASH_SPI_SPEED_80M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC400_80MHZ)
#elif (BL_FLASH0_SPEED == FLASH_SPI_SPEED_66M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC400_66MHZ)
#elif (BL_FLASH0_SPEED == FLASH_SPI_SPEED_50M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC400_50MHZ)
#elif (BL_FLASH0_SPEED == FLASH_SPI_SPEED_25M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC200_25MHZ)
#else
#error "Invalid flash1 speed"
#endif

#if (BL_FLASH1_SPEED == FLASH_SPI_SPEED_80M)
#define BL_FLASH1_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI1_SRC400_80MHZ)
#elif (BL_FLASH1_SPEED == FLASH_SPI_SPEED_66M)
#define BL_FLASH1_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI1_SRC400_66MHZ)
#elif (BL_FLASH1_SPEED == FLASH_SPI_SPEED_50M)
#define BL_FLASH1_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI1_SRC400_50MHZ)
#elif (BL_FLASH1_SPEED == FLASH_SPI_SPEED_25M)
#define BL_FLASH1_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI1_SRC200_25MHZ)
#else
#error "Invalid flash1 speed"
#endif

const FLASH_CfgType FlashConfig[] = {
    {
        .hwID = FLASH_HW_ID_0,
        .size = (2UL * 1024UL * 1024UL), /* 2MB */
        .pageSize = 256UL,
        .sectorSize = SECTOR_SIZE, /* 64KB */
        .subSectorSize = SUBSECTOR_SIZE, /* 4KB */
        .SPIMode = FLASH_SPI_MODE_3,
        .speed = BL_FLASH0_SPEED,
        .readLane = FLASH_READ_LANE_SINGLE,
        .clkCfgID = BL_FLASH0_CLK_CFG_ID,
    },
    {
        .hwID = FLASH_HW_ID_1,
        .size = (64UL * 1024UL * 1024UL),
        .pageSize = 256UL,
        .sectorSize = SECTOR_SIZE, /* 64KB */
        .subSectorSize = SUBSECTOR_SIZE, /* 4KB */
        .SPIMode = FLASH_SPI_MODE_3,
        .speed = BL_FLASH1_SPEED,
        .readLane = FLASH_READ_LANE_SINGLE,
        .clkCfgID = BL_FLASH1_CLK_CFG_ID,
    },
};
#endif /* __BCM8910X__ */

#ifdef __BCM8953X__
#define BL_FLASH0_SPEED    (FLASH_SPI_SPEED_50M)

#if (BL_FLASH0_SPEED == FLASH_SPI_SPEED_80M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_83MHZ)
#elif (BL_FLASH0_SPEED == FLASH_SPI_SPEED_66M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_62MHZ)
#elif (BL_FLASH0_SPEED == FLASH_SPI_SPEED_50M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_50MHZ)
#elif (BL_FLASH0_SPEED == FLASH_SPI_SPEED_25M)
#define BL_FLASH0_CLK_CFG_ID       (BL_MCU_CLK_CFG_ID_QSPI0_SRC250_25MHZ)
#else
#error "Invalid flash1 speed"
#endif

const FLASH_CfgType FlashConfig[] = {
    {
        .hwID = FLASH_HW_ID_0,
        .size = (32UL * 1024UL * 1024UL), /* 2MB */
        .pageSize = 256UL,
        .sectorSize = SECTOR_SIZE, /* 64KB */
        .subSectorSize = SUBSECTOR_SIZE, /* 4KB */
        .SPIMode = FLASH_SPI_MODE_3,
        .speed = BL_FLASH0_SPEED,
        .readLane = FLASH_READ_LANE_SINGLE,
        .clkCfgID = BL_FLASH0_CLK_CFG_ID,
    },
};
#endif /* __BCM8953X__ */


void PrintData(const uint8_t const *aAddr, uint32_t aLen)
{
    uint32_t i;

    for (i = 0UL; i < aLen; i++) {
        LOG_INFO("%02x ", aAddr[i]);
        if ((i > 0UL) && ((i % 16UL) == 0UL)) {
            LOG_INFO("\n");
        }
    }
    LOG_INFO("\n");
}

int32_t FlashReadTest(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    int32_t retVal;
    uint32_t len;

    LOG_INFO("FlashReadTest: aID: %d aAddr: %x aLen:%d\n", aID, aAddr, aLen);
    memset(rxBuf, 0x0, sizeof(rxBuf));
    if (aLen > sizeof(rxBuf)) {
        len = sizeof(rxBuf);
    } else {
        len = aLen;
    }

    retVal = FLASH_Read(aID, aAddr, rxBuf, len);
    if (BCM_ERR_OK == retVal) {
        PrintData(rxBuf, aLen);
    }
    return retVal;
}

int32_t FlashWriteTest(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    int32_t retVal;

    LOG_INFO("FlashWriteTest: aID: %d aAddr: %x aLen:%d\n", aID, aAddr, aLen);
    retVal = FLASH_Write(aID, aAddr, txBuf, aLen);
    if (BCM_ERR_OK != retVal) {
        LOG_ERR("FLASH_Write failed(0x%x)\n", retVal);
        goto err;
    }

    LOG_INFO("Flash Read after write\n");
    memset(rxBuf, 0x0, sizeof(rxBuf));
    retVal = FLASH_Read(aID, aAddr, rxBuf, aLen);
    if (BCM_ERR_OK != retVal) {
        LOG_ERR("FLASH_Read failed(0x%x)\n", retVal);
        goto err;
    }

    /* compare the data */
    retVal = memcmp(txBuf, rxBuf, aLen);
    if (0UL == retVal) {
        LOG_INFO("Data verification success\n");
    } else {
        LOG_INFO("Data verification failed\n");
        goto err;
    }

err:
    return retVal;
}

int32_t FlashSectorEraseTest(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    int32_t retVal;

    /* Erase first sector of the flash */
    LOG_INFO("FlashSectorEraseTest: aID: %d aAddr: %x aLen:%d\n", aID, aAddr, aLen);
    retVal = FLASH_EraseSector(aID, aAddr, aLen);
    if (BCM_ERR_OK != retVal) {
        LOG_ERR("FLASH_Erase failed(0x%x)\n", retVal);
        goto err;
    }

    LOG_INFO("Flash Read after erase\n");
    retVal = FLASH_Read(aID, aAddr, rxBuf, 2UL * PAGE_SIZE);
    if (BCM_ERR_OK != retVal) {
        LOG_ERR("FLASH_Read failed(0x%x)\n",retVal);
        goto err;
    }

    memset(txBuf, 0xFF, sizeof(txBuf));
    retVal = memcmp(txBuf, rxBuf, sizeof(txBuf));
    if (0UL == retVal) {
        LOG_INFO("Data verification success\n");
    } else {
        LOG_INFO("Data verification failed\n");
        goto err;
    }

err:
    return retVal;
}

int32_t FlashSubSectorEraseTest(FLASH_HwIDType aID, FLASH_AddrType aAddr,
                    FLASH_LenType aLen)
{
    int32_t retVal;

    /* Erase first sector of the flash */
    LOG_INFO("FlashSubSectorEraseTest: aID: %d aAddr: %x aLen:%d\n", aID, aAddr, aLen);
    retVal = FLASH_EraseSubSector(aID, aAddr, aLen);
    if (BCM_ERR_OK != retVal) {
        LOG_ERR("FLASH_Erase failed(0x%x)\n", retVal);
        goto err;
    }

    LOG_INFO("Flash Read after erase\n");
    retVal = FLASH_Read(aID, aAddr, rxBuf, 2UL * PAGE_SIZE);
    if (BCM_ERR_OK != retVal) {
        LOG_ERR("FLASH_Read failed(0x%x)\n",retVal);
        goto err;
    }

    memset(txBuf, 0xFF, sizeof(txBuf));
    retVal = memcmp(txBuf, rxBuf, sizeof(txBuf));
    if (0UL == retVal) {
        LOG_INFO("Data verification success\n");
    } else {
        LOG_INFO("Data verification failed\n");
        goto err;
    }

err:
    return retVal;
}

int32_t TestFlash(FLASH_HwIDType aID)
{
    int32_t retVal;
    uint32_t addr;
    uint32_t size;
    uint32_t i;
    FLASH_FlsIDType flashID;

    LOG_ERR("*****Flash-%d Test***** \n", aID);

    /* Initialize the flash clock speed */
    retVal = BCM_MCU_ClkInit(FlashConfig[aID].clkCfgID);
    if (BCM_ERR_OK != retVal) {
        LOG_ERR("BCM_MCU_ClkInit failed\n");
        goto err;
    }

    FLASH_Init(aID, &FlashConfig[aID]);

    /* read ID of the flash */
    flashID = FLASH_ReadID(aID);
    LOG_INFO("flash(%d) ID: %x\n", aID, flashID);
    if (flashID == 0UL) {
        LOG_ERR("Invalid flash ID\n");
    }

    /* aligned adress and aligned size read */
    addr = 0UL;
    size = (2UL * PAGE_SIZE);
    retVal = FlashReadTest(aID, addr, size);
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashReadTest successful\n");
    } else {
        LOG_ERR("FlashReadTest failed. error: %d\n", retVal);
        goto err;
    }

    /* aligned address and unaligned size read */
    addr = 1UL;
    size = ((2UL * PAGE_SIZE) - 1UL);
    retVal = FlashReadTest(aID, addr, size);
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashReadTest successful\n");
    } else {
        LOG_ERR("FlashReadTest failed. error: %d\n", retVal);
        goto err;
    }

    /* unaligned address and unaligned size read (< 4 bytes)*/
    addr = 1UL;
    size = 73UL;
    retVal = FlashReadTest(aID, addr, size);
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashReadTest successful\n");
    } else {
        LOG_ERR("FlashReadTest failed. error: %d\n", retVal);
        goto err;
    }

    addr = 0UL * SECTOR_SIZE;
    retVal = FlashSectorEraseTest(aID, addr, (SECTOR_SIZE));
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashSectorEraseTest successful\n");
    } else {
        LOG_ERR("FlashSectorEraseTest failed(0x%x)\n", retVal);
    }

    addr = 1UL * SECTOR_SIZE;
    retVal = FlashSectorEraseTest(aID, addr, (SECTOR_SIZE));
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashSectorEraseTest successful\n");
    } else {
        LOG_ERR("FlashSectorEraseTest failed(0x%x)\n", retVal);
    }

    addr = 16UL * SECTOR_SIZE;
    retVal = FlashSectorEraseTest(aID, addr, (8UL * SECTOR_SIZE));
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashSectorEraseTest successful\n");
    } else {
        LOG_ERR("FlashSectorEraseTest failed(0x%x)\n",retVal);
    }

    addr = 31UL * SECTOR_SIZE;
    retVal = FlashSectorEraseTest(aID, addr, (SECTOR_SIZE));
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashSectorEraseTest successful\n");
    } else {
        LOG_ERR("FlashSectorEraseTest failed(0x%x)\n", retVal);
        goto err;
    }

    addr = 127UL * SECTOR_SIZE;
    retVal = FlashSectorEraseTest(aID, addr, (SECTOR_SIZE));
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashSectorEraseTest successful\n");
    } else {
        LOG_ERR("FlashSectorEraseTest failed(0x%x)\n", retVal);
    }

    for (i = 0UL; i < sizeof(txBuf); i++) {
        txBuf[i] = (uint8_t)i;
    }

    addr = 0UL * PAGE_SIZE;
    retVal = FlashWriteTest(aID, addr, PAGE_SIZE);
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashWriteTest successful\n");
    } else {
        LOG_ERR("FlashWriteTest failed(0x%x)\n", retVal);
    }

    addr = 0UL * SUBSECTOR_SIZE;
    retVal = FlashSubSectorEraseTest(aID, addr, (SUBSECTOR_SIZE));
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashSectorEraseTest successful\n");
    } else {
        LOG_ERR("FlashSectorEraseTest failed(0x%x)\n", retVal);
    }

    for (i = 0UL; i < sizeof(txBuf); i++) {
        txBuf[i] = (uint8_t)i;
    }

    addr = 0UL * PAGE_SIZE;
    retVal = FlashWriteTest(aID, addr, PAGE_SIZE);
    if (BCM_ERR_OK == retVal) {
        LOG_ERR("FlashWriteTest successful\n");
    } else {
        LOG_ERR("FlashWriteTest failed(0x%x)\n", retVal);
    }

err:
    return retVal;
}

int32_t FlashTest(void)
{
    TestFlash(FLASH_HW_ID_0);
#if (BL_FLASH_MAX_HW_ID > 1)
    TestFlash(FLASH_HW_ID_1);
#endif
    return BCM_ERR_OK;
}
