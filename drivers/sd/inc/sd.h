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
#ifndef SD_H
#define SD_H

#include <stdint.h>
#include <msg_queue.h>

#define SD_INIT_API_ID                      (0x00U)
#define SD_ENUMERATE_API_ID                 (0x01U)
#define SD_SET_BUS_WIDTH_API_ID             (0x02U)
#define SD_ENABLE_CARD_INTR_API_ID          (0x03U)
#define SD_DISABLE_CARD_INTR_API_ID         (0x04U)
#define SD_GET_OP_RESULT_API_ID             (0x05U)
#define SD_GET_CARD_STATE_API_ID            (0x06U)
#define SD_SEND_CMD_API_ID                  (0x07U)
#define SD_IO_RW_SINGLE_API_ID              (0x08U)
#define SD_IO_RW_MULTIPLE_API_ID            (0x09U)
#define SD_MMC_READ_BLOCKS_API_ID           (0x0AU)
#define SD_MMC_WRITE_BLOCKS_API_ID          (0x0BU)
#define SD_MMC_ERASE_BLOCKS_API_ID          (0x0CU)

typedef uint32_t SD_SlotIDType;

typedef uint32_t SD_CardType;
#define SD_CARD_TYPE_MEM        (0UL)
#define SD_CARD_TYPE_IO         (1UL)
#define SD_CARD_TYPE_COMBO      (2UL)

typedef uint32_t SD_CardStateType;
#define SD_CARD_STATE_NOT_CONNECTED      (0UL)
#define SD_CARD_STATE_INACTIVE           (1UL)
#define SD_CARD_STATE_ACTIVE             (2UL)

typedef uint32_t SD_XferWidthType;
#define MMC_XFER_WIDTH_1_BIT                (1UL)
#define MMC_XFER_WIDTH_4_BIT                (2UL)

typedef uint32_t SD_BusVoltageType;
#define SD_BUS_VOLT_1V8                 (0x1UL)
#define SD_BUS_VOLT_3V0                 (0x2UL)
#define SD_BUS_VOLT_3V3                 (0x3UL)

typedef uint32_t SD_ClkType;
#define SD_MAX_CLK_FREQ                 (208000000ULL)

typedef uint32_t SD_OpResultType;
#define SD_OP_RESULT_SUCCESS             (1UL)
#define SD_OP_RESULT_FAILED              (2UL)

typedef uint32_t SD_CmdIDType;

typedef uint32_t SD_RespDataType[4UL];

typedef uint32_t  SD_CmdRespType;
#define MMC_RESP_LEN_136       (0x1U)
#define MMC_RESP_LEN_48        (0x2U)
#define MMC_RESP_LEN_48_BUSY   (0x3U)
#define MMC_RESP_HAS_CRC            (0x1U << 3U)
#define MMC_RESP_HAS_CMD_IDX            (0x1U << 4U)

#define MMC_RESP_NONE   (0x0UL)
#define MMC_RESP_R1     (MMC_RESP_LEN_48 | MMC_RESP_HAS_CRC | MMC_RESP_HAS_CMD_IDX)
#define MMC_RESP_R1B    (MMC_RESP_LEN_48_BUSY | MMC_RESP_HAS_CRC | MMC_RESP_HAS_CMD_IDX)
#define MMC_RESP_R2     (MMC_RESP_LEN_136 | MMC_RESP_HAS_CRC)
#define MMC_RESP_R3     (MMC_RESP_LEN_48)
#define MMC_RESP_R4     (MMC_RESP_LEN_48)
#define MMC_RESP_R5     (MMC_RESP_LEN_48 | MMC_RESP_HAS_CRC | MMC_RESP_HAS_CMD_IDX)
#define MMC_RESP_R5b    (MMC_RESP_LEN_48_BUSY | MMC_RESP_HAS_CRC | MMC_RESP_HAS_CMD_IDX)
#define MMC_RESP_R6     (MMC_RESP_LEN_48 | MMC_RESP_HAS_CRC | MMC_RESP_HAS_CMD_IDX)
#define MMC_RESP_R7     (MMC_RESP_LEN_48 | MMC_RESP_HAS_CRC | MMC_RESP_HAS_CMD_IDX)

typedef uint32_t SD_ClientIDType;
#define SD_CLIENT_ID_MEM_MASK  (0xFUL)
#define SD_CLIENT_ID_IO_MASK   (0xF0UL)
#define SD_CLIENT_ID_ALL_MASK  (SD_CLIENT_ID_MEM_MASK | SD_CLIENT_ID_IO_MASK)

typedef void (*SD_OpDoneIndType) (SD_SlotIDType aID, SD_ClientIDType aClientID,
        SD_OpResultType aResult);
typedef void (*SD_CardIntrIndType) (SD_SlotIDType aID, SD_ClientIDType aClientID);

typedef struct {
    SD_ClientIDType     clientID;
    SD_OpDoneIndType    opDoneInd;
    SD_CardIntrIndType  cardIntrInd;
} SD_ClientInfoTblType;

typedef struct {
    SD_SlotIDType           slotID;
    SD_CardType             cardType;
    SD_ClkType              freq;
    SD_BusVoltageType       detectVolt;
    const SD_ClientInfoTblType    *clientCbTbl;
    uint32_t                clientCbTblSz;
} SD_ConfigType;

/**
 * SD Data type (data transfer
 * with SD command)
 */
typedef struct {
    uint8_t     *buf;
    uint32_t    blockSz;
    uint32_t    blockCnt;
#define DATA_FLG_RD   (0x1)
#define DATA_FLG_WR   (0x2)
    uint32_t    flags;
} SD_DataType;

/**
 * MMC Command type structure
 */
typedef struct {
#define MMC_CMD_FLG_APP_CMD         (0x1UL) /**< application specific command */
    uint32_t            flags;          /**< command flags */
    SD_CmdIDType        cmdIdx;         /**< Command id */
    uint32_t            arg;            /**< 32-bit command argument */
    uint32_t            rca;            /**< 16-bit RCA to be send for CMD55 */
    SD_CmdRespType      respType;       /**< expected response type */
} SD_CmdType;

typedef uint32_t SD_IO_RW_OpType;
#define SD_IO_RW_OP_READ            (0x1UL) /**< read from SDIO card */
#define SD_IO_RW_OP_WRITE           (0x2UL) /**< write to SDIO card */
#define SD_IO_RW_OP_WRITE_READ      (0x3UL) /**< Read after write from SDIO card */

/**
 * Common interfaces (MMC and SDIO)
 */
extern int32_t SD_Init(SD_SlotIDType aID, const SD_ConfigType *const aConfig);
extern int32_t SD_Enumerate(SD_SlotIDType aID, SD_ClientIDType aClientID);

extern int32_t SD_GetCardState(SD_SlotIDType aID, SD_CardStateType *const aState);
extern int32_t SD_SetBusWidth(SD_SlotIDType aID, SD_ClientIDType aClientID,
        SD_XferWidthType aWidth);
extern int32_t SD_EnableCardIntr(SD_SlotIDType aID, SD_ClientIDType aClientID);
extern int32_t SD_DisableCardIntr(SD_SlotIDType aID, SD_ClientIDType aClientID);

/**
 * SD direct command interfaces
 */
extern int32_t SD_SendCmd(SD_SlotIDType aID,
        SD_ClientIDType aClientID,
        const SD_CmdType *const aCmd,
        SD_DataType *const aData,
        SD_RespDataType aResp);

/**
 * SDIO interface
 */

extern int32_t SD_IO_RW_Single(SD_SlotIDType aID,
        SD_ClientIDType aClientID,
        SD_IO_RW_OpType aOp,
                       uint32_t aFunc,
                       uint32_t aAddr,
        uint8_t *const aBuf);

extern int32_t SD_IO_RW_Multiple(SD_SlotIDType aID,
        SD_ClientIDType aClientID,
        SD_IO_RW_OpType aOp,
         uint32_t aFunc,
         uint32_t aAddr,
         uint32_t aAutoIncr,
         uint32_t aBlkSz,
         uint32_t aBlkCnt,
         uint8_t * const aBuf);

/**
 * MMC interfaces
 */
extern int32_t SD_MMC_ReadBlocks(SD_SlotIDType aID,
        SD_ClientIDType aClientID,
        uint32_t aStartAddr,
        uint32_t aNrBlocks,
        uint8_t *const aBuf);

extern int32_t SD_MMC_WriteBlocks(SD_SlotIDType aID,
        SD_ClientIDType aClientID,
        uint32_t aStartAddr,
        uint32_t aNrBlocks,
        uint8_t *const aBuf);

extern int32_t SD_MMC_EraseBlocks(SD_SlotIDType aID,
        SD_ClientIDType aClientID,
        uint32_t aStartAddr,
        uint32_t aNrBlocks);

#endif /* SD_H */
