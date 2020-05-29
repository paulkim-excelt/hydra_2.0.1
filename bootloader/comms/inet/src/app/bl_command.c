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
    @defgroup grp_bl_cmd_impl Implementation
    @ingroup grp_bl_cmd

    @addtogroup grp_bl_cmd_impl
    @{
    @file bl_command.c
    @brief Command parser Implementation
    This source file contains the implementation of Command parser

    @version 0.30 Imported from docx
*/

#include <stdint.h>
#include <bl_bcm_err.h>
#include <bl_command.h>
#include <bl_board.h>
#include <bl_utils.h>
#include <bl_udp.h>
#include <bl_log.h>

/**
    @name Command parser API IDs
    @{
    @brief API IDs for Command parser
*/
#define BRCM_DSGN_BL_CMD_SEND_RESP_PROC     (0x80U) /**< @brief #BL_SendResp */
#define BRCM_DSGN_BL_CMD_RECV_CB_PROC       (0x81U) /**< @brief #BL_CmdRecvCb */
#define BRCM_DSGN_BL_CMD_PKT_PARSER_PROC    (0x82U) /**< @brief #BL_CmdPktParser */
/** @} */

#define BL_GetModuleLogLevel()     (BL_LOG_LVL_INFO)

#define BL_CMD_PAYLOAD_ID   (0x345UL)

#define BL_CMD_BUF_SIZE        (1536UL)

typedef struct {
    uint32_t state;
#define BL_CMD_STATE_UNINIT         (0UL)
#define BL_CMD_STATE_INIT           (1UL)
#define BL_CMD_STATE_PROCESS_CMD    (2UL)
    BL_CmdIDType currCmdID;
    uint32_t len;
    BL_INET_IPAddress rmtIP;
    uint16_t rmtPort;
} BL_CmdCompType;

static uint8_t BL_TxCmdBuf[BL_CMD_BUF_SIZE];

static uint32_t BL_Busy = 0UL;

static BL_CmdCompType BL_CmdComp = {
    .state = BL_CMD_STATE_UNINIT,
    .currCmdID = 0UL,
    .len = 0UL,
    .rmtIP = 0UL,
    .rmtPort = 0U,
};

/** @brief API to send the response to the client

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf        Pointer to data buffer
    @param[in]      aIP         IP of the client
    @param[in]      aPort       Client port
    @param[in]      aLen        length of the buffer

    Return values are documented in reverse-chronological order
    @retval     others      Errors from #UDP_Send

    @post None

    @trace  #BRCM_DSGN_BL_CMD_SEND_RESP_PROC  #BRCM_ARCH_BL_CMD_SEND_RESP

    @limitations None

    @image html bl_cmd_flowchart_send_resp.jpg
*/
static int32_t BL_SendResp(uint8_t *const aBuf,
                           BL_INET_IPAddress aIP,
                           uint16_t aPort,
                           uint16_t aLen)
{
    return BL_UDP_Send(BL_CMD_PAYLOAD_ID, aBuf, aLen);
}

/** @brief API to parse the command packet and take necessary action

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf        Pointer to data buffer
    @param[in]      aIP         Source IP
    @param[in]      aPort       Source Port

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_CMD_PKT_PARSER_PROC  #BRCM_ARCH_BL_CMD_PKT_PARSER

    @limitations None

    @image html bl_cmd_flowchart_pkt_parser.jpg
*/
static void BL_CmdPktParser(const uint8_t *const aBuf,
                            BL_INET_IPAddress aIP,
                            BL_INET_Port aPort)
{
    uint32_t i;
    BL_FLASH_HwIDType flashID;
    int32_t retVal;
    int32_t ret;
    BL_CmdPktType *resp;
    uint32_t percent;
    uint32_t addr;
    BL_CmdPktType *cmd = ((BL_CmdPktType *)aBuf);
    uint32_t sectors = 0UL;
    uint32_t sectorSize = 0UL;
    BL_CmdStatusType status = BL_STATUS_SUCCESS;

    if (cmd->magic != BL_CMD_MAGIC_NUM) {
        goto exit;
    }

    BL_CmdComp.currCmdID = cmd->cmdID;
    switch (cmd->cmdID) {
        case BL_CMD_ID_ERASE_FLASH:
            BL_SendStatus(aIP, BL_STATUS_BUSY, NULL, 0UL);
            flashID = cmd->data[0];
            /* Erase full flash */
            for (i = 0UL; i < BL_FLASH_CfgSize; i++) {
                if (flashID == BL_FLASH_Config[i].hwID) {
                    sectorSize = BL_FLASH_Config[i].sectorSize;
                    sectors = BL_FLASH_Config[i].size / sectorSize;
                    break;
                }
            }

            if (0UL == sectors) {
                status = BL_STATUS_UNKNOWN_ERR;
                break;
            }
            for (i = 0UL; i < sectors; i++) {
                addr = i * sectorSize;
                ret = BL_FLASH_EraseSector(flashID, addr, sectorSize);
                if (BL_BCM_ERR_OK != ret) {
                    status = BL_STATUS_UNKNOWN_ERR;
                    break;
                }
                percent = ((i + 1UL) * 100UL) / sectors;
                BL_SendStatus(aIP, BL_STATUS_BUSY, (uint8_t *)(&percent), 1UL);
            }
            break;
        default:
            status = BL_STATUS_INVALID_CMD;
            break;
    }

    /* send the response */
    resp = (BL_CmdPktType *)BL_TxCmdBuf;
    resp->magic = BL_CMD_MAGIC_NUM;
    resp->cmdID = cmd->cmdID;
    resp->status = status;

    BL_LOG_ERR("sending response for cmd: %d status: %d port: %u\n", resp->cmdID,
            resp->status, aPort);

    retVal = BL_SendResp(BL_TxCmdBuf, aIP, aPort, sizeof(BL_CmdPktType));
    if (retVal != BL_BCM_ERR_OK) {
        BL_CmdComp.state = BL_CMD_STATE_PROCESS_CMD;
        BL_CmdComp.rmtIP = aIP;
        BL_CmdComp.rmtPort = aPort;
        BL_CmdComp.len = sizeof(BL_CmdPktType);
    } else {
        BL_CmdComp.state = BL_CMD_STATE_INIT;
    }

    BL_Busy = 0;

exit:
    return;
}

/** @brief API to send the response to the client

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf        Pointer to data buffer
    @param[in]      aLen        length of the buffer
    @param[in]      aSrcIPAddr  Source IP address
    @param[in]      aSrcPort    Source Port

    @return     void

    @post None

    @trace  #BRCM_DSGN_BL_CMD_RECV_CB_PROC  #BRCM_ARCH_BL_CMD_RECV_CB

    @limitations None

    @image html bl_cmd_flowchart_recv_cb.jpg
*/
void BL_CmdRecvCb(const uint8_t* const aBuf,
                  uint16_t aLen,
                  BL_INET_IPAddress aSrcIPAddr,
                  BL_INET_Port aSrcPort)
{
    BL_LOG_ERR("Received BL cmd pkt\n");

    if (0UL == BL_Busy) {
        BL_Busy = 1UL;
        BL_CmdPktParser(aBuf, aSrcIPAddr, aSrcPort);
    } else {
        BL_LOG_ERR("BL_Busy processing prev cmd\n");
    }
}

/**
    @image html bl_cmd_flowchart_send_status.jpg
*/
void BL_SendStatus(BL_INET_IPAddress aIP,
                   BL_CmdStatusType aStatus,
                   uint8_t *const aData,
                   uint32_t aLen)
{
    int32_t retVal;
    BL_CmdPktType *pkt = ((BL_CmdPktType *)BL_TxCmdBuf);
    uint16_t len = (uint16_t)(sizeof(BL_CmdPktType) + aLen);

    if ((NULL == aData) && (aLen > 0UL)) {
        BL_LOG_ERR("BL_SendStatus: Inval params\n");
        goto exit;
    }
    pkt->magic = BL_CMD_MAGIC_NUM;
    pkt->cmdID = BL_CMD_ID_BL_STATUS;
    pkt->status = aStatus;
    if (NULL != aData) {
        BL_BCM_MemCpy(pkt->data, aData, aLen);
    }
    retVal = BL_SendResp(BL_TxCmdBuf, aIP, BL_CMD_LISTEN_PORT, len);
    if (BL_BCM_ERR_OK != retVal) {
        BL_CmdComp.state = BL_CMD_STATE_PROCESS_CMD;
        BL_CmdComp.rmtIP = aIP;
        BL_CmdComp.rmtPort = BL_CMD_LISTEN_PORT;
        BL_CmdComp.len = len;
    } else {
        BL_CmdComp.state = BL_CMD_STATE_INIT;
    }

exit:
    return;
}

void BL_CmdInit(void)
{
    if (BL_CMD_STATE_UNINIT == BL_CmdComp.state) {
        BL_CmdComp.state = BL_CMD_STATE_INIT;
    } else {
        BL_LOG_ERR("%s called in wrong state(%u)\n", __func__, BL_CmdComp.state);
    }
}

void BL_CmdStateHandler(void)
{
    int32_t retVal;

    if (BL_CMD_STATE_UNINIT == BL_CmdComp.state) {
        BL_LOG_ERR("%s called in wrong state(%u)\n", __func__, BL_CmdComp.state);
        goto err;
    } else if (BL_CMD_STATE_PROCESS_CMD == BL_CmdComp.state) {
        retVal = BL_SendResp(BL_TxCmdBuf,
                            BL_CmdComp.rmtIP,
                            BL_CmdComp.rmtPort,
                            BL_CmdComp.len);
        if (BL_BCM_ERR_OK == retVal) {
            BL_CmdComp.state = BL_CMD_STATE_INIT;
        }
    }

err:
    return;
}

/** @} */
