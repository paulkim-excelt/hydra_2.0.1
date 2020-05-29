/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

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
#ifndef LIN_DRV_H
#define LIN_DRV_H

#include <stddef.h>
#include <inttypes.h>
#include <lin.h>
#include <osil/lin_osil.h>

#define LIN_DRV_INIT_API_ID                    (0x80U)
#define LIN_DRV_DEINIT_API_ID                  (0x81U)
#define LIN_DRV_TRANSMIT_FRAME_API_ID          (0x82U)
#define LIN_DRV_SLEEP_API_ID                   (0x83U)
#define LIN_DRV_WAKEUP_API_ID                  (0x84U)
#define LIN_DRV_READ_STATUS_API_ID             (0x85U)
#define LIN_DRV_GET_DATA_API_ID                (0x86U)

#define LIN_DRV_SYNC_DATA                      (0x55U)
#define LIN_DRV_DIAG_MASTER_REQ                (0x3CU)
#define LIN_DRV_WAKEUP_DATA                    (0U)
#define LIN_DRV_SYNC_FIELD                     (0UL)
#define LIN_DRV_MID_FIELD                      (1UL)
#define LIN_DRV_DATA_FIELD                     (2UL)
#define LIN_DRV_TX_BUF_SIZE                    (16UL)

extern void LINDrv_Init(LIN_HwIDType aHwID, const LIN_ConfigType * const aConfig);

extern void LINDrv_Deinit(LIN_HwIDType aHwID);

extern int32_t LINDrv_TransmitFrame(LIN_HwIDType aHwID, const LIN_FrmObjType * const aFrmObj);

extern int32_t LINDrv_Sleep(LIN_HwIDType aHwID);

extern int32_t LINDrv_WakeUp(LIN_HwIDType aHwID);

extern LIN_StatusType LINDrv_ReadStatus(LIN_HwIDType aHwID);

extern int32_t LINDrv_GetData(LIN_HwIDType aHwID, uint8_t *const aDataPtr);

#endif /* LIN_DRV_H */
/* Nothing past this line */
