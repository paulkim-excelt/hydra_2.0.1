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
 * @file dma_osil.h
 *
 * @brief DMA driver OS integration layer APIs.
 */

#ifndef DMA_OSIL_H
#define DMA_OSIL_H

#include <system.h>
#include <dma.h>

/**
 * DMA configuration structure (external) refered
 * by DMA driver
 *
 * A constant table with DMA configuration
 * shall be defined by system
 */
typedef void (*DMA_PeriphXferDoneCb)(DMA_ChannelIDType aID, uint32_t aReqIdx,
        DMA_XferStatusType status);

/**
 * DMA channel configuration structure
 */
typedef struct {
    /**
     * 16-bit System component ID OR
     * 64-bit Media component ID
     */
    uint64_t            compID;
    /**
     * instance ID, starting from zero
     */
    uint32_t            instanceID;
    /**
     * DMA transfer type allowed on this channel
     */
    DMA_XferType        xferType;

    /**
     * DMA peripheral ID if transfer type(@xferType)
     * is either from peripheral to memory
     * or memory to peripheral
     */
    DMA_PeriphIDType    periphID;

    /** DMA burst size for this channel */
    DMA_BurstSize       burstSz;

    /** DMA burst lenght for this channel */
    DMA_BurstLen        burstLen;

    /**
     * DMA transfer done callback function pointer
     * (To be populated only for peripheral transfers)
     */
    DMA_PeriphXferDoneCb      xferDoneCb;

    /**
     * DMA code address (Address where DMA
     * instructions will be populated by the driver
     */
    uint8_t       *codeAddr;
    /**
     * Size of the DMA code region
     */
    uint32_t            codeSize;
} DMA_ChannCfgType;

typedef struct {
    const DMA_ChannCfgType          *channCfg;
    uint32_t                  channCfgSz;
} DMA_ConfigType;

extern const DMA_ConfigType DMA_CfgTbl;

/**
 * DMA command type
 */
typedef uint32_t DMA_CmdType;
#define DMA_CMD_INIT            (1UL)   /**< Init command */
#define DMA_CMD_XFER_REQ        (2UL)   /**< Mem to Mem transfer request command */
#define DMA_CMD_GET_XFER_STATUS (3UL)

/**
 * DMA IO structure (used for system command request)
 */
typedef struct _DMAIO {
    int32_t                   retVal;                 /**< Return value */
    const DMA_ConfigType      *config;
    DMA_ChannelIDType         channelID;
    DMA_XferReqType           *xferReq;
    DMA_XferStatusType        *xferStatus;
    uint32_t                  aReqSZ;
} DMAIO;

/** @brief Handles DMA commands
 *
 * This API is called from DMA_SysCmdHandler for DMA commands
 *
 * @param[in]       aCmd        DMA command ID
 * @param[inout]    aIO         Pointer to DMAIO structure
 *
 * @retval      BCM_ERR_OK              DMA command success
 * @retval      BCM_ERR_INVAL_PARAMS    aIO is NULL or invalid aCmd
 */
extern int32_t DMA_CmdHandler(DMA_CmdType aCmd, DMAIO *const aIO);

/** @brief Handles DMA commands (to be installed in SVC Layer)
 *
 * This API is called from SysCmdHandler for DMA commands
 *
 * @param[in]       aMagicID    DMA magic ID
 * @param[in]       aCmd        DMA commands
 * @param[inout]    aSysIO      uint8_t *, pointer for DMAIO
 *
 * @return      void
 */
extern void DMA_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief DMA system command request
 *
 * Interface for DMA system command request
 *
 * @param[in]   aCmd     DMA command
 * @param[in]   aIO      Pointer to DMA request IO
 *
 * @retval      BCM_ERR_OK              DMA command success
 * @retval      BCM_ERR_INVAL_PARAMS    aIO is NULL or invalid aCmd
 * @retval      BCM_ERR_UNKNOWN         Error in system command request
 */
extern int32_t DMA_SysCmdReq(DMA_CmdType aCmd, DMAIO *const aIO);

/** @brief DMA Interrupt request handler
 *
 * This API will do DMA Channel interrupt handling.
 *
 * @param[in]   aID             DMA channel ID
 *
 * @return      uint32_t      TRUE if client needs to be signalled
 *                            FALSE if client need not be signalled
 */
extern uint32_t DMADrv_ChannIRQHandler(DMA_ChannelIDType aID);

/** @brief DMA Abort Interrupt request handler
 *
 * This API will do DMA Abort interrupt handling.
 *
 * @param[in]   void
 *
 * @return      uint32_t      Bit mask of all clients to be signlled
 *                            (1 - signal, 0 - no signal)
 */
extern uint32_t DMADrv_AbortIRQHandler(void);

#endif /* DMA_OSIL_H */
