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

/******************************************************************************
File Name:  image_writer.c
Descritpion: Main application to dump images.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "parser.h"
#include "isp_image_util.h"
#include "mode_image_util.h"
#include "unicam_image_util.h"
#include "sensor_image_util.h"

/* Externs */
/* ISP DPF Parser API to get version */
void IDP_PrintVersion(void);

/* Macros */
#define MAX_LEN 1024
#define DEBUG_ERR printf

/*Globals*/
typedef struct {
    char ispHV_DPF_Filename[MAX_LEN];
    char ispCV_DPF_Filename[MAX_LEN];
    char sensorFilename[MAX_LEN];
} FileNames;

FileNames gFileName;

/* List of config files syntax */
SyntaxEntry gConfigFileNameSyntax[] = {
    {"isp_hv_dpf_file", eChar, 0, 1, (void *)gFileName.ispHV_DPF_Filename, NULL, 0, 0},
    {"isp_cv_dpf_file", eChar, 0, 0, (void *)gFileName.ispCV_DPF_Filename, NULL, 0, 0},
    {"sensor_file", eChar, 0, 1, (void *)gFileName.sensorFilename, NULL, 0, 0},
    {}
};

/******************************************************************************
FUNCTION NAME: GetConfigFileSyntax

ARGUMENTS: None

DESCRIPTION: This function gets the config files syntax.

RETURNS: SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetConfigFileSyntax(void)
{
    return gConfigFileNameSyntax;
}

/******************************************************************************
FUNCTION NAME: main

ARGUMENTS:
Pass Type   -> "pass_hv"/"pass_cv"
Config file -> Full path of config file.

DESCRIPTION: Main function to call parser and image dump Apis.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t main(int32_t argc, char *argv[])
{
    int32_t ret = BCM_ERR_OK;
    uint32_t passIdx = 0UL;

    /* Print binary version */
    printf("Image_Writer version: %s\n", VERSION);
    IDP_PrintVersion();

    if (argc < 3) {
        DEBUG_ERR("%s: Need input config file\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (0 == strncmp("pass_hv", argv[1], 7)) {
        passIdx = ISYS_PASS_IDX_HV;
    } else if (0 == strncmp("pass_cv", argv[1], 7)) {
        passIdx = ISYS_PASS_IDX_CV;
    } else {
        DEBUG_ERR("%s: Invalid pass type\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* memset global structure */
    memset(&gFileName, 0, sizeof(gFileName));

    /* Parse config files */
    const SyntaxEntry *configFileSyntax[] = {
        GetConfigFileSyntax(),
    };

    /* Parse Config file names */
    ret = ParseConfig(argv[2], configFileSyntax,
            (sizeof(configFileSyntax) / sizeof(SyntaxEntry *)));
    if (eSuccess != ret) {
        printf("%s:Error: parser failed\n", __func__);
        goto exit;
    }

    /* Sensor Image creater */
    ret = SensorImageWriter(gFileName.sensorFilename);
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: Failed to create Sensor image\n", __FUNCTION__);
        goto exit;
    }

    /* Unicam Image creater */
    ret = Unicam_ImageWriter();
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: Failed to create Unicam image\n", __FUNCTION__);
        goto exit;
    }

    /* ISP Image Crteater */
    if (ISYS_PASS_IDX_HV == passIdx) {
        ret = IspImageWriter(gFileName.ispHV_DPF_Filename);
    } else if (0L != gConfigFileNameSyntax[1].parsed) {
        ret = IspImageWriter(gFileName.ispCV_DPF_Filename);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: Failed to create ISP image\n", __FUNCTION__);
        goto exit;
    }

    /* Mode image create */
    ret = ModeImageWriter(passIdx);
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: Falied to create Mode image\n", __FUNCTION__);
    }

exit:
    return ret;
}
