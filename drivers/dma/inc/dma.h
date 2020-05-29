/*****************************************************************************
 Copyright 2016-2017 Broadcom Limited.  All rights reserved.

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
 * @file dma.h
 *
 * @brief System DMA Interface APIs.
 *
 * This file provides interface APIs for the DMA driver
 */

#ifndef DMA_H
#define DMA_H

#define DMA_INSTANCE_ID             (0x0U)
#define DMA_INIT_API_ID             (0x0U)
#define DMA_XFER_SETUP_API_ID       (0x1U)
#define DMA_XFER_TRIGGER_API_ID     (0x2U)
#define DMA_GET_XFER_STATUS_API_ID  (0x3U)

/** DMA address type */
typedef uint32_t DMA_AddrType;

/** DMA channel IDs */
typedef uint32_t DMA_ChannelIDType;
#define DMA_CHANNEL_ID_0        (0UL)               /**< channel 0 */
#define DMA_CHANNEL_ID_1        (1UL)               /**< channel 1 */
#define DMA_CHANNEL_ID_2        (2UL)               /**< channel 2 */
#define DMA_CHANNEL_ID_3        (3UL)               /**< channel 3 */
#define DMA_CHANNEL_ID_4        (4UL)               /**< channel 4 */
#define DMA_CHANNEL_ID_5        (5UL)               /**< channel 5 */
#define DMA_CHANNEL_ID_6        (6UL)               /**< channel 6 */
#define DMA_CHANNEL_ID_7        (7UL)               /**< channel 7 */
#define DMA_CHANNEL_ID_MAX      (DMA_CHANNEL_ID_7)  /**< channel max */

/** DMA peripheral IDs (refer to TRM for supported peripherals) */
typedef uint8_t DMA_PeriphIDType;

/**
 * DMA burst size (number of bytes within each DMA
 * beat)
 * Burst size depends upon the AXI width of DMA,
 * refer to the chip TRM for supported width
 */
typedef uint32_t DMA_BurstSize;
#define DMA_BURST_SZ_1        (0UL)   /**< 1 byte per beat */
#define DMA_BURST_SZ_2        (1UL)   /**< 2 bytes per beat */
#define DMA_BURST_SZ_4        (2UL)   /**< 4 bytes per beat */
#define DMA_BURST_SZ_8        (3UL)   /**< 8 bytes per beat */
#define DMA_BURST_SZ_16       (4UL)   /**< 16 bytes per beat */
#define DMA_BURST_SZ_32       (5UL)   /**< 32 bytes per beat */
#define DMA_BURST_SZ_64       (6UL)   /**< 64 bytes per beat */
#define DMA_BURST_SZ_MAX      (DMA_BURST_SZ_64) /**< max burst size */

/** DMA burst lenght type (number of beats within each burst) */
typedef uint32_t DMA_BurstLen;

/**
 * DMA transfer type (not all the transfer type may be supported in the chip
 * refer to TRM for details
 */
typedef uint32_t DMA_XferType;
#define DMA_XFER_TYPE_MEM_TO_MEM        (0UL)   /**< Mem to mem transfer */
#define DMA_XFER_TYPE_MEM_TO_PERIPH     (1UL)   /**< mem to peripheral transfer */
#define DMA_XFER_TYPE_PERIPH_TO_MEM     (2UL)   /**< peripheral to mem transfer */
#define DMA_XFER_TYPE_PERIPH_TO_PERIPH  (3UL)   /**< peripheral to peripheral transfer */
#define DMA_XFER_TYPE_MAX               (DMA_XFER_TYPE_PERIPH_TO_PERIPH)

/** DMA transfer status */
typedef uint32_t DMA_XferStatusType;
#define DMA_XFER_STATUS_SUCCESS         (0UL)   /**< DMA Transfer was successful */
#define DMA_XFER_STATUS_ERROR           (1UL)   /**< Error occured during DMA transfer */
#define DMA_XFER_STATUS_BUSY            (2UL)   /**< DMA transfer on-going */

/**
 * DMA Trasfer Request packet
 */
typedef struct {
    DMA_AddrType            srcAddr;        /**< source address */
    DMA_AddrType            destAddr;       /**< destination address */
    uint32_t                rowSize;        /**< DMA transfer size in bytes */
    uint32_t                nRows;          /**< Number of rows to be transferred */
    int16_t                 srcSkip;        /**< Source address skip in bytes
                                                (after copying each row) */
    int16_t                 dstSkip;        /**< Destination address skip in bytes
                                                (after copying each row) */
} DMA_XferReqType;

/** @brief Initialize DMA driver
 * This function initializes system DMA driver
 *
 * @param[in]  aConfig      configuration structure
 *
 * @return  void
 *
 */
extern void DMA_Init(void);

/** @brief Initiates DMA transfer from Memory to Memory
 *
 * This function initiates Memory to Memory transfer using DMA.
 * Only a fixed type of transfer is allowed on a channel
 * (which is passed to DMA driver during initialization).
 * This API will chain the DMA requests (@aXferReq) and trigger
 * the DMA automatically. Upon completion of all transfers
 * it will raise an event to client.
 *
 * @param[in]  aID         DMA channel ID
 * @param[in]  aXferReq    Pointer to DMA transfer requests array
 * @param[in]  aReqSz      Size of the DMA transfer request array
 *
 * @retval      BCM_ERR_OK              Setup was successful
 * @retval      BCM_ERR_INVAL_PARAMS    aID is invalid
 * @retval      BCM_ERR_INVAL_PARAMS    aXferReq is NULL
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_NOMEM           Transfer request too big
 */
extern int32_t DMA_Mem2MemXfer(DMA_ChannelIDType aID,
        DMA_XferReqType aXferReq[], uint32_t aReqSZ);

/** @brief Get transfer status
 *
 * This API returns the status of the DMA last DMA transfer which
 * was triggered using DMA_Mem2MemXfer.
 *
 * @param[in]   aID         DMA channel ID
 *
 * @retval      DMA_XferStatusType
 */
extern int32_t DMA_GetXferStatus(DMA_ChannelIDType aID,
                            DMA_XferStatusType *aStatus);

/** @brief Initiates DMA transfer from Memory to Memory
 *
 * This function initiates Memory to Memory transfer using DMA.
 * Only a fixed type of transfer is allowed on a channel
 * (which is passed to DMA driver during initialization).
 * This API will chain the DMA requests (@aXferReq) and trigger
 * the DMA automatically. Upon completion of all transfers
 * it will raise a callback to client.
 *
 * @param[in]  aID         DMA channel ID
 * @param[in]  aXferReq    Pointer to DMA transfer requests array
 * @param[in]  aReqSz      Size of the DMA transfer request array
 *
 * @retval      BCM_ERR_OK              Setup was successful
 * @retval      BCM_ERR_INVAL_PARAMS    aID is invalid
 * @retval      BCM_ERR_INVAL_PARAMS    aXferReq is NULL
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_NOMEM           Transfer request too big
 */
extern int32_t DMA_Mem2MemXferSetup(DMA_ChannelIDType aID,
        DMA_XferReqType aXferReq[], uint32_t aReqSZ);

/** @brief Trigger DMA transfer for Memory to Memory
 *
 * This function triggers the DMA transfer which was
 * setup using DMA_Mem2MemXferSetup API. Upon completion of the
 * transfer, DMA driver will raise a callback in interrupt
 * context.
 *
 * @param[in]   aID         DMA channel ID
 *
 * @retval      BCM_ERR_OK              DMA was triggered successfully
 * @retval      BCM_ERR_INVAL_PARAMS    aReqIdx is invalid
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_INVAL_STATE     DMA is not setup
 */
extern int32_t DMA_Mem2MemXferTrigger(DMA_ChannelIDType aID);

/** @brief Setup DMA transfer from Memory to Peripheral or vice-versa
 *
 * This function sets up Memory to Peripheral (or vice-versa) transfer
 * using DMA. Only a fixed type of transfer is allowed on a channel
 * (which is passed to DMA driver during initialization).
 * this API	will setup the DMA descriptors (not chained)
 * and client need to trigger the DMA to start the transfer.
 * Upon completion client(driver) will receive the callback
 * and then it shall trigger the DMA again for next descriptor
 *
 * @param[in]  aID         DMA channel ID
 * @param[in]  aXferReq    Pointer to DMA transfer requests array
 * @param[in]  aReqSz      Size of the DMA transfer request array
 *
 * @retval      BCM_ERR_OK              Setup was successful
 * @retval      BCM_ERR_INVAL_PARAMS    aID is invalid
 * @retval      BCM_ERR_INVAL_PARAMS    aXferReq is NULL
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_NOMEM           Transfer request too big
 */
extern int32_t DMA_PeriphXferSetup(DMA_ChannelIDType aID,
        DMA_XferReqType aXferReq[], uint32_t aReqSZ);

/** @brief Trigger DMA transfer for Memory to Peripheral (or vice-versa)
 *
 * This function triggers the DMA transfer which was
 * setup using DMA_XferSetup API. Upon completion of the
 * transfer, DMA driver will raise a callback in interrupt
 * context (if transfer is between memory and peripheral)
 * or raise a event (if transfer is between memory)
 *
 * @param[in]   aID         DMA channel ID
 * @param[in]   aReqIdx     DMA request Index (index of the descriptor)
 *
 * @retval      BCM_ERR_OK              DMA was triggered successfully
 * @retval      BCM_ERR_INVAL_PARAMS    aReqIdx is invalid
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_INVAL_STATE     DMA is not setup
 */
extern int32_t DMA_PeriphXferTrigger(DMA_ChannelIDType aID, uint32_t aReqIdx);

/** @brief Flush the Peripheral DMA Queue
 *
 * This function flushes the DMA request queue of
 * the DMA controller
 *
 * This function shall be called by the client before
 * triggering the DMA and before peripheral is started to ensure
 * that any stale DMA request from the peripheral is flushed
 *
 * Note: This function shall not be called while the peripheral
 * is active because it will flush the DMA request queue and will
 * loose the sync with peripheral
 *
 * @param[in]   aID         DMA channel ID
 *
 * @retval      BCM_ERR_OK              DMA flush was successfull
 * @retval      BCM_ERR_INVAL_PARAMS    aID is invalid
 * @retval      BCM_ERR_UNINIT          DMA driver not initialized
 * @retval      BCM_ERR_INVAL_STATE     DMA driver is busy
 */
extern int32_t DMA_PeriphFlush(DMA_ChannelIDType aID);
#endif /* DMA_H */
