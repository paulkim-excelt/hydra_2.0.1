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
#ifndef BRCM_CMD_IF_H
#define BRCM_CMD_IF_H

#include <stdint.h>
#include <stddef.h>

typedef uint32_t CmdType;

#define CMD_ID_READ             (0x0001U)
#define CMD_ID_WRITE            (0x0002U)
#define CMD_ID_START            (0x0003U)
#define CMD_ID_STOP             (0x0004U)
#define CMD_ID_FRAME_CAPTURE    (0x0005U)
#define CMD_ID_FRAME_STATUS     (0x0006U)
#define CMD_ID_GET_VERSION      (0x0007U)
#define CMD_ID_SLICE_CAPTURE    (0x0008U)
#define CMD_ID_SLICE_STATUS     (0x0009U)

#define DEV_TYPE_IO             (0x0001U)
#define DEV_TYPE_IIC            (0x0002U)

#define STATUS_SUCCESS          (0x0001U)
#define STATUS_INVALID_CMD      (0x0002U)
#define STATUS_UNKNOWN_ERR      (0x0003U)
#define STATUS_BUSY             (0x0004U)

#define CMD_MAGIC_NUM           (0x4252434DU) /* 'B' 'R' 'C' 'M' */
#define BRCM_CMD_LISTEN_PORT    (58900)

#define CMD_MASK                (0x3F)
#define CMD_SHIFT               (0)
#define WIDTH_MASK              (0xC0)
#define WIDTH_SHIFT             (6)
#define DEV_TYPE_MASK           (0x300)
#define DEV_TYPE_SHIFT          (8)
#define SLAVE_ID_MASK           (0x1FFC00)
#define SLAVE_ID_SHIFT          (10)
#define LEN_MASK                (0xFFE00000)
#define LEN_SHIFT               (21)

#define WIDTH_8                 (0)
#define WIDTH_16                (1)
#define WIDTH_32                (2)
#define WIDTH_64                (3)

typedef struct _PktHdr {
    uint32_t hdr;           /* bits[5:0] - cmd, bits[7:6] - width, bits[9:8] - dev_type, bits[20:10] - slave_id, bits[31:21] - len */
    uint32_t addr;          /* memory address to be read/written */
} PktHdr;

typedef struct {
    PktHdr hdr;
    uint8_t data[];         /* variable len of data */
} CmdPktType;

/* command/response packet format */
typedef struct {
    uint32_t magic;         /* set to 'BRCM' */
    uint16_t seqNo;         /* Sequence number */
    uint16_t status;        /* status */
    uint32_t totalTuples;   /* Total number of requests queued */
    CmdPktType cmdPkts[];   /* command packets */
} CmdPktHdrType;

extern void VideoXmtrInit(void);

extern void VideoXmtrStart(void);

extern void VideoXmtrStop(void);

extern void VideoXmtrDeinit(void);

#endif /* BRCM_CMD_IF_H */
