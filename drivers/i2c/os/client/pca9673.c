/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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

#include <pca9673.h>
#include <bcm_utils.h>
#include <bcm_err.h>

#define PCA9673_RESERVED_ADDR   (0x0U)
#define PCA9673_SWRST           (0x06U)

#define PCA9673_NUM_PORTS       (2UL)
#define PCA9673_PORT0           (0UL)
#define PCA9673_PORT1           (1UL)

typedef struct _Pca9673Driver {
    IIC_PktType pkt;
    uint32_t ready;
    uint32_t portStatus[2UL];
    uint8_t data[2UL];
    uint8_t slaveAddr;
    uint8_t i2cHwId;
} Pca9673Driver;

static Pca9673Driver Pca9673Drv = {
    .ready = FALSE,
    .portStatus = {0xFFU, 0xFFU},
    .data = {0U, 0U},
    .slaveAddr = 0U,
    .i2cHwId = 0U,
};

static int32_t PCA9673_I2cTx(uint8_t aAddr, uint8_t aLen,
                             uint32_t aClientMask, uint32_t *aJobID)
{
    int32_t ret;

    Pca9673Drv.pkt.flags = 0UL;
    Pca9673Drv.pkt.slaveAddr = aAddr;
    Pca9673Drv.pkt.length = aLen;
    Pca9673Drv.pkt.data = Pca9673Drv.data;

    ret = IIC_Transfer(Pca9673Drv.i2cHwId, &Pca9673Drv.pkt, 1UL, aClientMask, aJobID);
    if (BCM_ERR_OK != ret) {
    }

    return ret;
}


int32_t PCA9673_GetStatus(uint32_t aJobID)
{
    return IIC_GetStatus(Pca9673Drv.i2cHwId, aJobID);
}

int32_t PCA9673_Init(uint32_t aClientMask, uint32_t * aJobID,
                     uint8_t aI2cHwId, uint8_t aSlaveAddr)
{
    int32_t ret;
    if (FALSE == Pca9673Drv.ready) {
        /* Software Reset */
        Pca9673Drv.data[0UL] = PCA9673_SWRST;
        ret = PCA9673_I2cTx(PCA9673_RESERVED_ADDR, 1U, aClientMask, aJobID);
        if (BCM_ERR_OK == ret) {
            Pca9673Drv.ready = TRUE;
            Pca9673Drv.i2cHwId = aI2cHwId;
            Pca9673Drv.slaveAddr = aSlaveAddr;
        } else {
            Pca9673Drv.ready = FALSE;
        }
    } else {
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t PCA9673_OutputChannel(PCA9673_ChannelType aChan,
                              PCA9673_LevelType aLevel,
                              uint32_t aClientMask, uint32_t* aJobID)
{
    int32_t ret;
    uint32_t port;
    uint32_t chan;
    if (FALSE == Pca9673Drv.ready) {
        ret = BCM_ERR_UNINIT;
    } else {
        if (aChan <= PCA9673_P0_CHANNEL_7) {
            port = 0UL;
            chan = aChan;
        } else if ((aChan >= PCA9673_P1_CHANNEL_0) && (aChan <= PCA9673_P1_CHANNEL_7)){
            port = 1UL;
            chan = aChan - PCA9673_P1_CHANNEL_0;
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
            goto done;
        }
        Pca9673Drv.portStatus[port] &= ~(1U << chan);
        Pca9673Drv.portStatus[port] |= (((uint32_t)aLevel) << chan);

        Pca9673Drv.data[0UL] = (uint8_t)Pca9673Drv.portStatus[0UL];
        Pca9673Drv.data[1UL] = (uint8_t)Pca9673Drv.portStatus[1UL];
        ret = PCA9673_I2cTx(Pca9673Drv.slaveAddr, 2U, aClientMask, aJobID);
    }
done:
    return ret;
}
