/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
File Name:  image_combine.c
Description: Main application to combine config images.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <isp_image.h>
#include <isp_image_util.h>
#include <image_to_array.h>
#include <mode_image_util.h>

/* Macros */
#define DEBUG_ERR printf
#define MAX_ARGS  5

/* Image combine type */
typedef uint32_t Combine_ImageType;
#define COMBINE_IMAGE_NONE (0x0UL)
#define COMBINE_IMAGE_ISP  (0x1UL)
#define COMBINE_IMAGE_MODE (0x2UL)

/* Combine operation type */
typedef uint32_t Combine_OpType;
#define COMBINE_OP_NONE    (0x0UL)
#define COMBINE_OP_VERSION (0x1UL)
#define COMBINE_OP_PASS    (0x2UL)

/******************************************************************************
* FUNCTION NAME: main
* ARGUMENTS: arg1 -> "pass" or "version"
*            arg1 -> "isp" or "mode"
*            arg2 -> Image1 file (A0/HV)
*            arg3 -> Image2 file (B0/CV)
*            arg4 -> Final Image file
* RETURNS:
* BCM_ERR_OK
* BCM_ERR_INVALID_PARAMS
*******************************************************************************/
int32_t main(int32_t argc, char *argv[])
{
    int32_t ret = BCM_ERR_OK;
    ISP_ImageType ispImageI0;
    ISP_ImageType ispImageI1;
    ISP_ImageType ispImageCombined;
    ISYS_ModeImageType modeImageI0;
    ISYS_ModeImageType modeImageI1;
    ISYS_ModeImageType modeImageCombined;
    FILE *fpI0 = NULL;
    FILE *fpI1 = NULL;
    FILE *fpCombined = NULL;
    Combine_OpType opType = COMBINE_OP_NONE;
    Combine_ImageType imgType = COMBINE_IMAGE_NONE;

    memset(&ispImageCombined, 0, sizeof(ISP_ImageType));
    memset(&ispImageI0, 0, sizeof(ISP_ImageType));
    memset(&ispImageI1, 0, sizeof(ISP_ImageType));

    memset(&modeImageCombined, 0, sizeof(ISYS_ModeImageType));
    memset(&modeImageI0, 0, sizeof(ISYS_ModeImageType));
    memset(&modeImageI1, 0, sizeof(ISYS_ModeImageType));

    /* Args check */
    if (argc < MAX_ARGS) {
        DEBUG_ERR("%s: Insufficient arguments\n", __FUNCTION__);
        DEBUG_ERR("Arguments to run ./image_combine [combine_type] [image_type]"
                            "[image1_path] [image2_path] [final_image_path]\n");
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (0 == strncmp("pass", argv[1], 4)) {
        opType = COMBINE_OP_PASS;
    }

    if (0 == strncmp("version", argv[1], 7)) {
        opType = COMBINE_OP_VERSION;
    }

    if (COMBINE_OP_NONE == opType) {
        DEBUG_ERR("%s:ERROR: Invalid combine op\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (0 == strncmp("isp", argv[2], 3)) {
        imgType = COMBINE_IMAGE_ISP;
    }

    if (0 == strncmp("mode", argv[2], 4)) {
        imgType = COMBINE_IMAGE_MODE;
    }

    if (COMBINE_IMAGE_NONE == imgType) {
        DEBUG_ERR("%s:ERROR: Invalid image type\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    fpI0 = fopen(argv[3], "r");
    if (NULL == fpI0) {
        DEBUG_ERR("%s:ERROR: Failed to open file\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    fpI1 = fopen(argv[4], "r");
    if (NULL == fpI1) {
        DEBUG_ERR("%s:ERROR: Failed to open file\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (COMBINE_IMAGE_ISP == imgType) {
        fread(&ispImageI0, sizeof(ISP_ImageType), 1, fpI0);
        fread(&ispImageI1, sizeof(ISP_ImageType), 1, fpI1);
    } else {
        fread(&modeImageI0, sizeof(ISYS_ModeImageType), 1, fpI0);
        fread(&modeImageI1, sizeof(ISYS_ModeImageType), 1, fpI1);
    }

    if (COMBINE_OP_VERSION == opType) {
        /* Combine ISP for different versions */
        ret = IspImageCombineVersion(&ispImageI0, &ispImageI1, &ispImageCombined);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to combine ISP image\n", __FUNCTION__);
            goto exit;
        }
    } else if ((COMBINE_OP_PASS == opType) && (COMBINE_IMAGE_ISP == imgType)) {
        /* Combine passes for isp */
        ret = IspImageCombinePass(&ispImageI0, &ispImageI1, &ispImageCombined);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to combine ISP image\n", __FUNCTION__);
            goto exit;
        }
    } else {
        /* Combine passes for mode */
        ret = ModeImageCombinePass(&modeImageI0, &modeImageI1, &modeImageCombined);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to combine ISP image\n", __FUNCTION__);
            goto exit;
        }
    }

    fpCombined = fopen(argv[5], "wb+");
    if (NULL == fpCombined) {
        DEBUG_ERR("Error: opening file\n");
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (COMBINE_IMAGE_ISP == imgType) {
        /* Write final ISP image */
        if (sizeof(ispImageCombined) != fwrite(&ispImageCombined, 1, sizeof(ispImageCombined), fpCombined)) {
            DEBUG_ERR("Error: in fwritre\n");
            ret = BCM_ERR_UNKNOWN;
            goto exit;
        }

        /* Write .c for final image */
        ret = ImageToArray("isp_image", (unsigned char *)&ispImageCombined, sizeof(ispImageCombined));
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("Error: in ImageToArray\n");
            goto exit;
        }
    } else {
        /* Write final mode image */
        if (sizeof(modeImageCombined) != fwrite(&modeImageCombined, 1, sizeof(modeImageCombined), fpCombined)) {
            DEBUG_ERR("Error: in fwritre\n");
            ret = BCM_ERR_UNKNOWN;
            goto exit;
        }

        /* Write .c for final image */
        ret = ImageToArray("cam_mode_image", (unsigned char *)&modeImageCombined, sizeof(modeImageCombined));
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("Error: in ImageToArray\n");
            goto exit;
        }
    }

exit:
    if (NULL != fpCombined) {
        fclose(fpCombined);
    }

    if (NULL != fpI0) {
        fclose(fpI0);
    }

    if (NULL != fpI1) {
        fclose(fpI1);
    }

    return ret;
}
