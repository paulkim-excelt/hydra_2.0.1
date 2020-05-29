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

#include "ee.h"
#include "brcm_cmd_if.h"
#include <camera.h>
#include <io.h>
#include <i2c.h>
#include <shell.h>
#include <build_info.h>
#include <ulog.h>
#include <inet.h>
#include <udp.h>
#include <bcm_utils.h>
#include <inet_cfg.h>
#include <osil/bcm_osil.h>
#include <dbgmem.h>

#define GetModuleLogLevel()     (ULOG_LVL_ERROR)

#define IIC_HW_ID               (3UL)
#define IICTransferEvent        (SystemEvent0)
#define BRCMCmdEvent            (SystemEvent1)
#define SENSOR_FREQUENCY        (13500000UL)
#define PKT_SIZE                (1500UL)

CmdPktType *CmdPkt;
static INET_IPAddress destIpaddr;
static uint16_t destPort;
static uint32_t PktLen;
static uint32_t busy = 0UL;
char CmdBuffer[1536];
static uint32_t msgQInfo = 0xFFFFFFFFUL;

static uint8_t TxPktBuf[PKT_SIZE];
static uint8_t RxPktBuf[PKT_SIZE];
CmdPktHdrType *CmdPktHdr = (CmdPktHdrType *)RxPktBuf;
INET_EndPointIPPort entPtCfg = {58900U, 58900U, INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))};

typedef struct {
    volatile int32_t busy;
    const MSGQ_MsgHdrType *hdr;
    uint16_t status;
} CaptureStatus;

static CaptureStatus FrameCapture = {
    .busy = 0,
    .hdr = NULL,
    .status = STATUS_SUCCESS,
};

static CaptureStatus SliceCapture = {
    .busy = 0,
    .hdr = NULL,
    .status = STATUS_SUCCESS,
};

static int32_t IICTransferRequest(IIC_PktType *aPkts, uint32_t aNumPkts)
{
    BCM_EventMaskType mask;
    int32_t ret = BCM_ERR_OK;

    ULOG_INFO("IICTransferRequest++\n");

    ret = IIC_Transfer(IIC_HW_ID, aPkts, aNumPkts, IICTransferEvent, &msgQInfo);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("IIC_TransferRequest failed (%d)\n", ret);
        goto exit;
    }

    /* Wait for server to process the request */
    BCM_WaitEvent(IICTransferEvent);
    BCM_GetEvent(TaskCmdParser, &mask);
    if (mask & IICTransferEvent) {
        BCM_ClearEvent(IICTransferEvent);
    }

    /* Fetch the response from MsgQ */
    ret = IIC_GetStatus(IIC_HW_ID, msgQInfo);
    if (ret != BCM_ERR_OK) {
        ULOG_ERR("IIC_RecvResponse failed (%d)!!\n", ret);
    }

exit:
    return ret;
}

static void SendRes(CmdPktHdrType *CmdPktHdr, uint16_t response)
{
    int32_t ret = BCM_ERR_OK;

    BCM_MemSet(TxPktBuf, 0x0L, PktLen);
    BCM_MemCpy(TxPktBuf, CmdPktHdr, PktLen);
    *((uint16_t *)TxPktBuf + 3) = response;

    COMP_ASM("dmb");
    COMP_ASM("isb");
    busy = 0;

    ret = UDP_SendTo(0x1111, INET_UDP_PAYLOAD_ID0, TxPktBuf, PktLen, &entPtCfg);

    while (ret == BCM_ERR_BUSY) {
        ret = UDP_Process(0x1111, INET_UDP_PAYLOAD_ID0);
    }

    if (ret != BCM_ERR_OK) {
        ULOG_ERR("Error in sending Response packet (%d)\n", ret);
    }

    return;
}

static int32_t IICRegRead(PktHdr *aPktHdr, uint8_t *aData, uint32_t aLen)
{
    IIC_PktType pkt[2];
    uint8_t reg[2];
    uint32_t slaveAddr = ((aPktHdr->hdr & SLAVE_ID_MASK) >> SLAVE_ID_SHIFT);
    int32_t ret = BCM_ERR_OK;

    pkt[0].flags = 0UL;
    pkt[0].slaveAddr = slaveAddr;
    /* The reg addr is assumed to be 16 bit always */
    reg[0] = (aPktHdr->addr >> 8) & 0xFF;
    reg[1] = (aPktHdr->addr & 0xFF);
    pkt[0].length = 2UL;
    pkt[0].data = reg;
    pkt[1].flags = (IIC_PKT_FLG_OP_RD | IIC_PKT_FLG_RESTART);
    pkt[1].slaveAddr = slaveAddr;
    pkt[1].length = aLen;
    pkt[1].data = aData;
    ret = IICTransferRequest(pkt, 2);

    return ret;
}

static uint16_t RegReadCb(PktHdr *aPktHdr, uint8_t *aData)
{
    uint16_t status;
    uint32_t i, j;
    uint8_t temp;
    uint32_t addr = aPktHdr->addr;
    uint32_t width = ((aPktHdr->hdr & WIDTH_MASK) >> WIDTH_SHIFT);
    uint32_t devType = ((aPktHdr->hdr & DEV_TYPE_MASK) >> DEV_TYPE_SHIFT);
    uint32_t len = ((aPktHdr->hdr & LEN_MASK) >> LEN_SHIFT);
    int32_t ret = BCM_ERR_OK;

    switch (width) {
    case WIDTH_8:
        if (devType == DEV_TYPE_IO) {
            ret = DBGMEM_Copy((uint32_t)aData, (uint32_t)addr, 8, len);
        } else if(devType == DEV_TYPE_IIC) {
            ret = IICRegRead(aPktHdr, aData, len);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
        break;
    case WIDTH_16:
        if(devType == DEV_TYPE_IO) {
            ret = DBGMEM_Copy((uint32_t)aData, (uint32_t)addr, 16, len);
            j = 0UL;
            for (i = 0UL; i < len; i++) {
                temp = aData[j];
                aData[j] = aData[j + 1];
                aData[j+1] = temp;
                j += 2UL;
            }
        } else if(devType == DEV_TYPE_IIC) {
            ret = IICRegRead(aPktHdr, aData, len*2);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
        break;
    case WIDTH_32:
        if(devType == DEV_TYPE_IO) {
            ret = DBGMEM_Copy((uint32_t)aData, (uint32_t)addr, 32, len);
            j = 0UL;
            for (i = 0UL; i < len; i++) {
                temp = aData[j];
                aData[j] = aData[j + 3];
                aData[j + 3] = temp;
                temp = aData[j + 1];
                aData[j + 1] = aData[j + 2];
                aData[j + 2] = temp;
                j += 4UL;
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
        break;
    default:
        ULOG_ERR("read%d is not supported\n", width);
        ret = BCM_ERR_INVAL_PARAMS;
    }
    if (BCM_ERR_OK == ret) {
        status = STATUS_SUCCESS;
    } else {
        status = STATUS_UNKNOWN_ERR;
    }

    return status;
}

static int32_t IICRegWrite(PktHdr *aPktHdr, uint8_t *aData, uint32_t aLen)
{
    IIC_PktType pkt;
    uint8_t buf[22];
    uint32_t i = 0UL;
    uint32_t slaveAddr = ((aPktHdr->hdr & SLAVE_ID_MASK) >> SLAVE_ID_SHIFT);
    int32_t ret = BCM_ERR_OK;

    for (i = 0UL; i < (aLen / 20UL); i++) {
        pkt.flags = 0UL;
        pkt.slaveAddr = slaveAddr;
        buf[0] = (aPktHdr->addr >> 8) & 0xFF;
        buf[1] = (aPktHdr->addr & 0xFF);
        memcpy(&buf[2], &aData[i * 20], 20UL);
        pkt.length = 22UL;
        pkt.data = buf;
        ret = IICTransferRequest(&pkt, 1);
        if (BCM_ERR_OK != ret) {
            goto func_exit;
        }
    }
    if ((aLen % 20UL) != 0UL) {
        pkt.flags = 0UL;
        pkt.slaveAddr = slaveAddr;
        buf[0] = ((aPktHdr->addr + (i * 20)) >> 8) & 0xFF;
        buf[1] = ((aPktHdr->addr + (i * 20)) & 0xFF);
        memcpy(&buf[2], &aData[i * 20], aLen % 20UL);
        pkt.length = 2UL + (aLen % 20UL);
        pkt.data = buf;
        ret = IICTransferRequest(&pkt, 1);
    }

func_exit:
    return ret;
}

static uint16_t RegWriteCb(PktHdr *aPktHdr, uint8_t *aData)
{
    uint16_t status;
    uint32_t i, j;
    uint8_t temp;
    uint32_t addr = aPktHdr->addr;
    uint32_t width = ((aPktHdr->hdr & WIDTH_MASK) >> WIDTH_SHIFT);
    uint32_t devType = ((aPktHdr->hdr & DEV_TYPE_MASK) >> DEV_TYPE_SHIFT);
    uint32_t len = ((aPktHdr->hdr & LEN_MASK) >> LEN_SHIFT);
    int32_t ret = BCM_ERR_OK;

    switch (width) {
    case WIDTH_8:
        if(devType == DEV_TYPE_IO) {
            ret = DBGMEM_Copy((uint32_t)addr, (uint32_t)aData, 8, len);
        } else if(devType == DEV_TYPE_IIC) {
            ret = IICRegWrite(aPktHdr, aData, len);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
        break;
    case WIDTH_16:
        if (devType == DEV_TYPE_IO) {
            j = 0UL;
            for (i = 0UL; i < len; i++) {
                temp = aData[j];
                aData[j] = aData[j + 1];
                aData[j+1] = temp;
                j += 2UL;
            }
            ret = DBGMEM_Copy((uint32_t)addr, (uint32_t)aData, 16, len);
        } else if(devType == DEV_TYPE_IIC) {
            ret = IICRegWrite(aPktHdr, aData, len*2);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
        break;
    case WIDTH_32:
        if (devType == DEV_TYPE_IO) {
            j = 0UL;
            for (i = 0UL; i < len; i++) {
                temp = aData[j];
                aData[j] = aData[j + 3];
                aData[j + 3] = temp;
                temp = aData[j + 1];
                aData[j + 1] = aData[j + 2];
                aData[j + 2] = temp;
                j += 4UL;
            }
            ret = DBGMEM_Copy((uint32_t)addr, (uint32_t)aData, 32, len);
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
        break;
    default:
        ULOG_ERR("write%d is not supported\n", width);
        ret = BCM_ERR_INVAL_PARAMS;
    }
    if (BCM_ERR_OK == ret) {
        status = STATUS_SUCCESS;
    } else {
        status = STATUS_UNKNOWN_ERR;
    }

    return status;
}

static uint16_t GetVersion(PktHdr *aPktHdr, uint8_t *aData)
{
    uint32_t len = ((aPktHdr->hdr & LEN_MASK) >> LEN_SHIFT);
    uint16_t status;

    if (0UL != len) {
        aData[len] = '\0';
        BuildInfo_GetVersionInfo((char * const)aData, &len);
        status = STATUS_SUCCESS;
    } else {
        status = STATUS_UNKNOWN_ERR;
    }

    return status;
}

static void CmdPacketParser()
{
    uint32_t i;
    uint32_t widthByte, len, width;
    uint32_t cmd;
    uint16_t status = STATUS_UNKNOWN_ERR;
    int32_t ret;
    PktHdr pktHdr;

    if (CmdPktHdr->magic == CMD_MAGIC_NUM) {

        for (i = 0UL; i < CmdPktHdr->totalTuples; i++) {
            memcpy(&pktHdr, CmdPkt, sizeof(pktHdr));
            cmd = (pktHdr.hdr & CMD_MASK) >> CMD_SHIFT;
            len = (pktHdr.hdr & LEN_MASK) >> LEN_SHIFT;
            ULOG_VERB("len[%d]\n", len);
            switch (cmd) {
                case CMD_ID_READ:
                    status = RegReadCb(&pktHdr, &CmdPkt->data[0]);
                    break;
                case CMD_ID_WRITE:
                    status = RegWriteCb(&pktHdr, &CmdPkt->data[0]);
                    break;
                case CMD_ID_FRAME_CAPTURE:
                    {
                        MSYS_CmdType frameCapCmd  = {
                            .compID = MSYS_COMP_ID_CAM0,
                            .version = 0UL,
                            .cmd = CAMERA_CUSTOM_CMD_CAPTURE_FRAME,
                            .response = BCM_ERR_UNKNOWN,
                        };
                        FrameCapture.busy = 1;
                        ret = CAMERA_GetCompFnTable()->sendCmd(&frameCapCmd, FrameCaptureEvent,
                                             &FrameCapture.hdr);
                        if (BCM_ERR_OK == ret) {
                            status  = STATUS_SUCCESS;
                        } else {
                            status = STATUS_UNKNOWN_ERR;
                        }
                        break;
                    }
                case CMD_ID_FRAME_STATUS:
                    if (0 == FrameCapture.busy) {
                        status = FrameCapture.status;
                    } else {
                        MSYS_CmdType cmd;
                        ret = CAMERA_GetCompFnTable()->recvResp(FrameCapture.hdr, &cmd);
                        if ((BCM_ERR_BUSY == ret)
                            || (BCM_ERR_BUSY == cmd.response)) {
                            status = STATUS_BUSY;
                        } else if ((BCM_ERR_OK == ret)
                                   && (BCM_ERR_OK == cmd.response)){
                            status = STATUS_SUCCESS;
                        } else {
                            status = STATUS_UNKNOWN_ERR;
                            ULOG_ERR("ret[%x] resp[%x] \n", ret, cmd.response);
                        }
                    }
                    break;
                case CMD_ID_GET_VERSION:
                    status = GetVersion(&pktHdr, &CmdPkt->data[0]);
                    break;
                case CMD_ID_SLICE_CAPTURE:
                    {
                        MSYS_CmdType sliceCapCmd = {
                            .compID = MSYS_COMP_ID_CAM0,
                            .version = 0UL,
                            .cmd = CAMERA_CUSTOM_CMD_CAPTURE_SLICE,
                            .response = BCM_ERR_UNKNOWN,
                            .customData[0] = (uint32_t)CmdPkt->data[0],
                        };

                        if (0 == SliceCapture.busy) {
                            SliceCapture.busy = 1;
                            ULOG_VERB("slice[%d] \n", sliceCapCmd.customData[0]);
                            ULOG_VERB("data[0][%d] \n", CmdPkt->data[0]);
                            ret = CAMERA_GetCompFnTable()->sendCmd(&sliceCapCmd,
                                                 SliceCaptureEvent,
                                                 &SliceCapture.hdr);
                            if (BCM_ERR_OK == ret) {
                                status  = STATUS_SUCCESS;
                            } else {
                                status = STATUS_UNKNOWN_ERR;
                            }
                        } else {
                            status = STATUS_BUSY;
                        }
                        break;
                    }
                case CMD_ID_SLICE_STATUS:
                    if (0 == SliceCapture.busy) {
                        status = SliceCapture.status;
                    } else {
                        MSYS_CmdType cmd;
                        ret = CAMERA_GetCompFnTable()->recvResp(SliceCapture.hdr, &cmd);
                        if ((BCM_ERR_OK == ret)
                            && (BCM_ERR_OK == cmd.response)) {
                            status = STATUS_SUCCESS;
                        } else if ((BCM_ERR_BUSY == ret)
                                   || (BCM_ERR_BUSY == cmd.response)) {
                            status = STATUS_BUSY;
                        } else {
                            status = STATUS_UNKNOWN_ERR;
                            ULOG_ERR("ret[%x] resp[%x] \n", ret, cmd.response);
                        }
                    }
                    break;
                default:
                    ULOG_ERR("Unsupported command\n");
                    status = STATUS_INVALID_CMD;
                    break;
            }
            len = (pktHdr.hdr & LEN_MASK) >> LEN_SHIFT;
            width = (pktHdr.hdr & WIDTH_MASK) >> WIDTH_SHIFT;
            switch (width) {
                case WIDTH_8:
                    widthByte = 8UL;
                    break;
                case WIDTH_16:
                    widthByte = 16UL;
                    break;
                case WIDTH_32:
                    widthByte = 32UL;
                    break;
                case WIDTH_64:
                    widthByte = 64UL;
                    break;
                default:
                    status = STATUS_UNKNOWN_ERR;
                    ULOG_ERR("Invalid width\n");
                    goto exit;
            }
            CmdPkt = (CmdPktType *)((uint8_t *)CmdPkt + sizeof(CmdPktType) + (len * (widthByte / 8)));
        }
    } else {
        ULOG_ERR("It is not a command packet. Dropping it!!\n");
        status = STATUS_UNKNOWN_ERR;
    }

exit:
    SendRes(CmdPktHdr, status);
}

TASK(CmdRcvCb)
{
    uint8_t isMoreData;
    uint16_t len;
    int32_t ret;
    INET_Port port = INET_PORT_INVALID;
    INET_IPAddress ipAddr = INET_IPADDRESS_INVALID;
    BCM_EventMaskType mask = 0UL;

    while (1) {
        len = PKT_SIZE;
        BCM_WaitEvent(UDPRxEvent);
        BCM_GetEvent(CmdRcvCb, &mask);
        if ((mask & UDPRxEvent) == UDPRxEvent) {
            BCM_ClearEvent(UDPRxEvent);
            if (busy == 0) {
                busy = 1;
                do {
                    ret = UDP_Recv(0x1111ULL, INET_UDP_PAYLOAD_ID0, RxPktBuf, &len,
                            &ipAddr, &port, &isMoreData);
                    if (BCM_ERR_OK != ret) {
                        busy = 0;
                        break;
                    }
                } while (1UL == isMoreData);
                if (BCM_ERR_OK == ret) {
                    CmdPkt = CmdPktHdr->cmdPkts;
                    PktLen = len;
                    destIpaddr = ipAddr;
                    destPort = port;
                    BCM_SetEvent(TaskCmdParser, BRCMCmdEvent);
                }
            } else {
                ULOG_ERR("Busy processing..dropping this packet\n");
            }
        }
    }
}

static void CmdListener(void)
{
    ULOG_ERR("CmdListener++\n");
    BCM_EventMaskType mask = 0UL;
    int32_t ret;
    MSYS_CmdType cmd;

    while (1) {
        BCM_WaitEvent(BRCMCmdEvent|FrameCaptureEvent|SliceCaptureEvent);
        ULOG_VERB("BCM_WaitEvent returned\n");
        BCM_GetEvent(TaskCmdParser, &mask);
        if (mask & BRCMCmdEvent) {
            BCM_ClearEvent(BRCMCmdEvent);
            CmdPacketParser();
        }

        if (mask & FrameCaptureEvent) {
            BCM_ClearEvent(FrameCaptureEvent);
            ret = CAMERA_GetCompFnTable()->recvResp(FrameCapture.hdr, &cmd);
            if (BCM_ERR_OK == ret) {
                if (BCM_ERR_OK == cmd.response) {
                    FrameCapture.status = STATUS_SUCCESS;
                    FrameCapture.busy = 0;
                    FrameCapture.hdr = NULL;
                } else if (BCM_ERR_BUSY == cmd.response) {
                    FrameCapture.status = STATUS_BUSY;
                } else {
                    FrameCapture.status = STATUS_UNKNOWN_ERR;
                    FrameCapture.busy = 0;
                    FrameCapture.hdr = NULL;
                }
            } else {
                FrameCapture.status = STATUS_UNKNOWN_ERR;
                FrameCapture.busy = 0;
                FrameCapture.hdr = NULL;
            }
        }

        if (mask & SliceCaptureEvent) {
            BCM_ClearEvent(SliceCaptureEvent);
            ret = CAMERA_GetCompFnTable()->recvResp(SliceCapture.hdr, &cmd);
            if (BCM_ERR_OK == ret) {
                if (BCM_ERR_OK == cmd.response) {
                    SliceCapture.status = STATUS_SUCCESS;
                    SliceCapture.busy = 0;
                    SliceCapture.hdr = NULL;
                } else if (BCM_ERR_BUSY == cmd.response) {
                    SliceCapture.status = STATUS_BUSY;
                } else {
                    SliceCapture.status = STATUS_UNKNOWN_ERR;
                    SliceCapture.busy = 0;
                    SliceCapture.hdr = NULL;
                }
            } else {
                SliceCapture.status = STATUS_UNKNOWN_ERR;
                SliceCapture.busy = 0;
                SliceCapture.hdr = NULL;
            }
        }
    }
}

TASK(TaskCmdParser)
{
    VideoXmtrInit();
    VideoXmtrStart();
    CmdListener();
    BCM_TerminateTask();
}


void ApplicationInit()
{
    BCM_ActivateTask(TaskCmdParser);
    BCM_ActivateTask(CmdRcvCb);
}

void Shell_App(uint32_t aConsoleID, char* aArgString, uint32_t aArgStringLen)
{
    BCM_SetEvent(CAMERA_Task, SystemEvent6);
}

