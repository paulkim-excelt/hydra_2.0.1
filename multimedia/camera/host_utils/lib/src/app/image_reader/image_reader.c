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
File Name:  image_reader.c
Description: Main application to read images.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "cam_mode_image.h"
#include "isp_image.h"
#include "tuner_module.h"
#include "isp_image_util.h"
#include "mode_image_util.h"
#include "unicam_image_util.h"
#include "sensor_image_util.h"

/* Macros */
#define DEBUG_ERR printf
#define MAX_ARGS  3

/******************************************************************************
* FUNCTION NAME: main
* ARGUMENTS: arg1 -> Component name
*            arg2 -> Image file
* RETURNS:
* BCM_ERR_OK
* BCM_ERR_INVALID_PARAMS
*******************************************************************************/
int32_t main(int32_t argc, char *argv[])
{
    int32_t ret = BCM_ERR_OK;
    ISP_ImageType          ispImage;
    ISYS_ModeImageType      modeImage;
    UNICAM_ImageType       unicamImage;
    CSN_ImageType sensorImage;
    Tuner_Image        tunerImage;
    FILE *fp = NULL;

    memset(&ispImage, 0, sizeof(ISP_ImageType));
    memset(&modeImage, 0, sizeof(ISYS_ModeImageType));
    memset(&unicamImage, 0, sizeof(UNICAM_ImageType));
    memset(&sensorImage, 0, sizeof(CSN_ImageType));
    memset(&tunerImage, 0, sizeof(Tuner_Image));

    /* Args check */
    if (argc < MAX_ARGS) {
        DEBUG_ERR("%s: Insufficient arguments\n", __FUNCTION__);
        DEBUG_ERR("Arguments to run ./image_reader [comp_name] [image_file]\n");
        DEBUG_ERR("Supported [comp_name] are:\n1)isp\n2)mode\n3)unicam\n4)sensor\n5)tuner\n");
        DEBUG_ERR("Supported [image_file] are:\
                \n1)isp.img\n2)mode.img\n3)unicam.img\n4)camera_sensor_*.img\n5)tuner.img\n");
        ret = BCM_ERR_INVAL_PARAMS;
        return ret;
    }

    if (!(strcmp(argv[1], "isp"))) {
        fp = fopen(argv[2], "r");
        if (NULL == fp) {
            DEBUG_ERR("%s:ERROR: Failed to open file\n", __FUNCTION__);
            ret = BCM_ERR_INVAL_PARAMS;
            return ret;
         }
        fread(&ispImage, sizeof(ISP_ImageType), 1, fp);
        fclose(fp);

        /* Read ISP Image */
        ret = IspImageReader(&ispImage);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to read ISP image\n", __FUNCTION__);
        }
    } else if (!(strcmp(argv[1], "tuner"))) {
        fp = fopen(argv[2], "r");
        if (NULL == fp) {
            DEBUG_ERR("%s:ERROR: Failed to open file\n", __FUNCTION__);
            ret = BCM_ERR_INVAL_PARAMS;
            return ret;
         }
        fread(&tunerImage, sizeof(Tuner_Image), 1, fp);
        fclose(fp);

        /* Read ISP tuner Image */
        ret = IspImageReader(&tunerImage);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to read tuner image\n", __FUNCTION__);
        }
    } else if (!(strcmp(argv[1], "mode"))) {
        fp = fopen(argv[2], "rb+");
        if (NULL == fp) {
            DEBUG_ERR("%s:ERROR: Failed to open file\n", __FUNCTION__);
            ret = BCM_ERR_INVAL_PARAMS;
            return ret;
        }
        fread(&modeImage, sizeof(ISYS_ModeImageType), 1, fp);
        fclose(fp);

        /* Read Mode Image */
        ret = ModeImageReader(&modeImage);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to read Mode image\n", __FUNCTION__);
        }
    } else if (!(strcmp(argv[1], "unicam"))) {
        fp = fopen(argv[2], "rb+");
        if (!fp) {
            DEBUG_ERR("%s:ERROR: Failed to open file\n", __FUNCTION__);
            ret = BCM_ERR_INVAL_PARAMS;
            return ret;
        }
        fread(&unicamImage, sizeof(UNICAM_ImageType), 1, fp);
        fclose(fp);

        /* Read Mode Image */
        ret = Unicam_ImageReader(&unicamImage);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to read Unicam image\n", __FUNCTION__);
        }
    } else if (!(strcmp(argv[1], "sensor"))) {
        fp = fopen(argv[2], "rb+");
        if (!fp) {
            DEBUG_ERR("%s:ERROR: Failed to open file\n", __FUNCTION__);
            ret = BCM_ERR_INVAL_PARAMS;
            return ret;
        }
        fread(&sensorImage, sizeof(CSN_ImageType), 1, fp);
        fclose(fp);

        /* Read Mode Image */
        ret = SensorImageReader(&sensorImage);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: Failed to read Sensor image\n", __FUNCTION__);
        }
    }
    else {
        DEBUG_ERR("%s: Comp_name not supported\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}
