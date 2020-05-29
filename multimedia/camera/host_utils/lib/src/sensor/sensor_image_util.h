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
#ifndef SENSOR_IMAGE_UTIL_H
#define SENSOR_IMAGE_UTIL_H

/* Includes */
#include <stdint.h>
#include <camera_sensor_image.h>
#include "parser.h"
#include "unicam_user.h"

/******************************************************************************
File Name: sensor_image_util.h
Descritpion: This file exposes Sensor image writer and sensor syntax API.
******************************************************************************/

/* Endianess */
typedef enum {
    eLitteEndian = 1,
    eBigEndian = 2
} EEndianess;

/******************************************************************************
FUNCTION NAME: SensorImageWriter

ARGUMENTS: aFileName

DESCRIPTION: This function dumps sensor image.

RETURNS: BCM_ERR_OK or BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t SensorImageWriter(char *aFileName);

/******************************************************************************
FUNCTION NAME: SensorImageReader

ARGUMENTS: None

DESCRIPTION: This function reads sensor image and dumps to a file.

RETURNS: BCM_ERR_OK or BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t SensorImageReader(const CSN_ImageType *aCfg);

/******************************************************************************
FUNCTION NAME: GetSensorDetailsSyntax

ARGUMENTS: None

DESCRIPTION: This function gets Sensor detail syntax table.

RETURNS: const SyntaxEntry *
******************************************************************************/
const SyntaxEntry *GetSensorDetailsSyntax(void);

/******************************************************************************
FUNCTION NAME: GetSensorModeSyntax

ARGUMENTS: None

DESCRIPTION: This function gets Sensor mode syntax table.

RETURNS: const SyntaxEntry *
******************************************************************************/
const SyntaxEntry *GetSensorModeSyntax(void);

/******************************************************************************
FUNCTION NAME: GetUnicamDetailsSyntax

ARGUMENTS: None

DESCRIPTION: This function gets Unicam details synatx table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetSensorModeDetailsSyntax(void);

/******************************************************************************
FUNCTION NAME: GetUnicamCpiSyntax

ARGUMENTS: None

DESCRIPTION: This function gets CPI syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetUnicamCpiSyntax(void);

/******************************************************************************
FUNCTION NAME: GetUnicamCpiSyntax

ARGUMENTS: None

DESCRIPTION: This function gets TG syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetUnicamTgSyntax(void);

/******************************************************************************
FUNCTION NAME: GetUnicamCsi2Syntax

ARGUMENTS: None

DESCRIPTION: This function gets CSI syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetUnicamCsi2Syntax(void);


#endif /* SENSOR_IMAGE_UTIL_H */
