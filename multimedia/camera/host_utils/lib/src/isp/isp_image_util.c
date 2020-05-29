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

/******************************************************************************
File Name:  isp_image_util.c
Description: This file implements isp image dump.
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cam_mode_image.h"
#include "tuner_module.h"
#include "isp_regdump_interface.h"
#include "isp_image_util.h"
#include "isp_hw_config_image.h"
#include "isp_tuner_config_image.h"

/* Error log */
#define DEBUG_ERR printf

/******************************************************************************
FUNCTION NAME: IspImageWriter

ARGUMENTS: aISPDpfFilename

DESCRIPTION: This function dumps ISP image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t IspImageWriter(const char *aISPDpfFilename)
{
    int32_t ret = BCM_ERR_OK;

    /* DPF Parser */
    ret = IDP_Parse(aISPDpfFilename);
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: DPF parser failed\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Dumps ISP Config Data */
    ret = ISPHwConfigWriter();
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: Failed to create ISP image\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Dumps ISP Tuner Config Data */
    ret = ISPTunerConfigWriter();
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: Failed to create ISP Tuner image\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
    }

exit:
    return ret;
}

/******************************************************************************
FUNCTION NAME: IspImageReader

ARGUMENTS: const ISP_ImageType *

DESCRIPTION: This function dumps ISP image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t IspImageReader(void *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    ISYS_CommonImgHdrType *commonHdr;

    /* Check Parameter */
    if (NULL == aCfg) {
        DEBUG_ERR("%s: Null Config Pointer\n", __FUNCTION__);
        ret =  BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    commonHdr = (ISYS_CommonImgHdrType *)aCfg;

    switch (commonHdr->cfgType) {
        case ISYS_CONFIG_ISP:
            ret = ISPHwConfigReader(aCfg);
            break;
        case CAM_CONFIG_TUNER:
            ret = ISPTunerConfigReader(aCfg);
            break;
        default:
            DEBUG_ERR("%s: Wrong config type\n", __FUNCTION__);
            break;
    }

exit:
    return ret;
}
