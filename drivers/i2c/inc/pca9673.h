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

/**
 * @file pca9673.h
 *
 * @brief PCA9673 GPIO expander IIC Client driver
 */

#ifndef PCA9673_H
#define PCA9673_H

#include "i2c.h"

/* PCA9673 Channels */
typedef uint32_t PCA9673_ChannelType;  /**< Channel type */
#define PCA9673_P0_CHANNEL_0      (0x00UL)   /**< PORT0 GPIO 0  */
#define PCA9673_P0_CHANNEL_1      (0x01UL)   /**< PORT0 GPIO 1  */
#define PCA9673_P0_CHANNEL_2      (0x02UL)   /**< PORT0 GPIO 2  */
#define PCA9673_P0_CHANNEL_3      (0x03UL)   /**< PORT0 GPIO 3  */
#define PCA9673_P0_CHANNEL_4      (0x04UL)   /**< PORT0 GPIO 4  */
#define PCA9673_P0_CHANNEL_5      (0x05UL)   /**< PORT0 GPIO 5  */
#define PCA9673_P0_CHANNEL_6      (0x06UL)   /**< PORT0 GPIO 6  */
#define PCA9673_P0_CHANNEL_7      (0x07UL)   /**< PORT0 GPIO 7  */
#define PCA9673_P1_CHANNEL_0      (0x10UL)   /**< PORT1 GPIO 0  */
#define PCA9673_P1_CHANNEL_1      (0x11UL)   /**< PORT1 GPIO 1  */
#define PCA9673_P1_CHANNEL_2      (0x12UL)   /**< PORT1 GPIO 2  */
#define PCA9673_P1_CHANNEL_3      (0x13UL)   /**< PORT1 GPIO 3  */
#define PCA9673_P1_CHANNEL_4      (0x14UL)   /**< PORT1 GPIO 4  */
#define PCA9673_P1_CHANNEL_5      (0x15UL)   /**< PORT1 GPIO 5  */
#define PCA9673_P1_CHANNEL_6      (0x16UL)   /**< PORT1 GPIO 6  */
#define PCA9673_P1_CHANNEL_7      (0x17UL)   /**< PORT1 GPIO 7  */

typedef uint8_t PCA9673_LevelType;           /**< Type for the possible levels that a GPIO channel can have */
#define PCA9673_LOW               (0x0U)     /**< Physical state 0V         */
#define PCA9673_HIGH              (0x1U)     /**< Physical state 5V or 3.3V */

/** @brief PCA9673 Initialisation API
 *
 * This API is used to initialise PCA9673 by issuing a reset and setting all
 * channels HIGH. Caller has to get the status through PCA9673_GetStatus API
 *
 * param[in]    aMsgQInfo        Pointer to Message Queue info
 * param[in]    aI2cHwId         IIC Bus ID to which, PCA9673 is connected
 * param[in]    aSlaveAddr       PCA9673 IIC slave address
 *
 * @return      BCM_ERR_OK           Initialisation configuration sent to IIC driver
 *              BCM_ERR_INVAL_PARAMS Initialisation failed due to invalid pointer
 */
int32_t PCA9673_Init(uint32_t aClientMask, uint32_t *aJobID,
                     uint8_t aI2cHwId, uint8_t aSlaveAddr);

/** @brief  Configure a output channel
 *
 * This API is used to configure an output channel as either HIGH or LOW.
 * Caller has to get the status through PCA9673_GetStatus API
 *
 * param[in]    aChan       Channel ID
 * param[in]    aLevel      Level of the output(HIGH or LOW)
 * param[in]    aMsgQInfo   Pointer to Message Queue info
 *
 * @return      BCM_ERR_OK      Configuration queued to IIC driver
 *              BCM_ERR_UNINIT  Uninitialised state
 */
int32_t PCA9673_OutputChannel(PCA9673_ChannelType aChan,
                              PCA9673_LevelType aLevel,
                              uint32_t aClientMask,
                              uint32_t *aJobID);

/** @brief  Get the status of IIC transfer
 *
 * This API is used to get the status of IIC request placed through
 * PCA9673_Init or PCA9673_OutputChannel APIs
 *
 * param[in]    aMsgQInfo           Pointer to Message Queue info
 *
 * @return      BCM_ERR_OK              Queued IIC transaction was successful
 * @return      BCM_ERR_INVAL_PARAMS    Invalid bus ID
 * @return      BCM_ERR_INVAL_PARAMS    aPkts is the pointer to packet array
 * @return      BCM_ERR_INVAL_PARAMS    aNumPkts is Zero
 */
int32_t PCA9673_GetStatus(uint32_t aJobID);

#endif /* PCA9673_H */
