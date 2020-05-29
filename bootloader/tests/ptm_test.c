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
#include <ptm.h>
#include <bcm_err.h>
#include <log.h>
#include <utils.h>
#include <board.h>

#define GetModuleLogLevel() (LOG_LVL_INFO)
#define JUMP_TO(addr) asm ("bx %[jump]" : : [jump]"r"(addr | 0x1):)

#ifdef __BCM8910X__
static const PTM_BL_LookupTblEntType BL_LookupTbl[] = {
    {
        .flashID = FLASH_HW_ID_0,
        .flashAddr = 0x0,
    },
    {
        .flashID = FLASH_HW_ID_1,
        .flashAddr = 0x0,
    },
};
#endif

#ifdef __BCM8953X__
static const PTM_BL_LookupTblEntType BL_LookupTbl[] = {
    {
        .flashID = FLASH_HW_ID_0,
        .flashAddr = (((BL_NUM_COPIES * BL_MAX_SIZE) / BL_FLASH_SUBSECTOR_SIZE)) * BL_FLASH_SUBSECTOR_SIZE,
    },
};

#endif

static const BL_PTM_CfgType PTM_Config = {
    .ptLoadAddr = NULL,
    .lookupTbl = BL_LookupTbl,
    .lookupTblSize = (sizeof(BL_LookupTbl)/sizeof(PTM_BL_LookupTblEntType)),
};

extern const FLASH_CfgType BL_FLASH_Config[];

void PTMTest()
{
    int32_t err;
    uint32_t imgEntryPt;
    uint32_t imgNum = 0UL;
    uint32_t isVerMatching = FALSE;
    uint8_t buf[100];
    uint32_t i, imgSize = 100UL;

    FLASH_Init(FLASH_HW_ID_0, &BL_FLASH_Config[0]);
    err = PTM_Init(&PTM_Config);
    if (BCM_ERR_OK == err) {
        err = PTM_LoadImg(0xC000, buf, &imgSize, &imgEntryPt ,&imgNum, &isVerMatching);
        LOG_ERR("ImgId:0xC000\t");
        if (BCM_ERR_OK == err) {
            for (i = 0UL; i < imgSize; i++) {
                LOG_ERR("%x\t", buf[i]);
            }
            LOG_ERR("\n");
        } else {
            LOG_ERR("ERR %d\n", err);
        }
        imgSize = 100;
        err = PTM_LoadImg(0xC001, buf, &imgSize, &imgEntryPt, &imgNum, &isVerMatching);
        LOG_ERR("ImgId:0xC001\t");
        if (BCM_ERR_OK == err) {
            for (i = 0UL; i < imgSize; i++) {
                LOG_ERR("%x\t", buf[i]);
            }
            LOG_ERR("\n");
        } else {
            LOG_ERR("ERR %d\n", err);
        }
        imgSize = 100;
        err = PTM_LoadImg(0xC002, buf, &imgSize, &imgEntryPt, &imgNum, &isVerMatching);
        LOG_ERR("ImgId:0xC002\t");
        if (BCM_ERR_OK == err) {
            for (i = 0UL; i < imgSize; i++) {
                LOG_ERR("%x\t", buf[i]);
            }
            LOG_ERR("\n");
        } else {
            LOG_ERR("ERR %d\n", err);
        }
        err = PTM_LoadImg(PTBL_ID_FW, NULL, NULL, &imgEntryPt, &imgNum, &isVerMatching);
        if (BCM_ERR_OK == err) {
            JUMP_TO(imgEntryPt);
        } else {
            LOG_ERR("PTM_LoadImg failed %d\n", err);
        }
    } else {
        LOG_ERR("PTM_Init failed %d\n", err);
    }
}

