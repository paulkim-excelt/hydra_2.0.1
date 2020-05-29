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
#ifndef PARSER_H
#define PARSER_H

/******************************************************************************
File Name:   parser.h
Descritpion: This defines syntax entry.
******************************************************************************/
#include <stdint.h>

#define MAX_LINE  (1024UL)
#define MAX_BUF_SIZE (8192UL)
#define MAX_NUM_MODES (10UL)

/* Return Values */
typedef enum {
    eSuccess = 0,
    eError = 1
} EParseStatus;

/* Data  type Supported */
typedef enum {
    eChar = 0,
    eUint16 = 1,
    eUint32 = 2,
    eHex = 3,
    eHexData = 4,
    eEnum = 5,
    eMax = 6
} EDataType;

/* Regdata pair */
typedef struct {
    uint32_t addr;
    uint32_t data;
} RegData;

/* Camera Block config layout */
typedef struct {
    RegData *cfg;
    uint32_t count;
} RegDataConfig;

/* EnumreadInfo */
typedef struct {
    const char **enumNames;
    int32_t *values;
} EnumReadInfo;

/* SyntaxEntry */
typedef struct {
    char      keyword[MAX_LINE]; /* Token */
    EDataType type;              /* Data type */
    int32_t   parsed;            /* Flag for parser */
    int32_t   flag;              /* To say Token is mandate or optional
                                    1 = mandate 0 = optional */
    void      *addr;             /* Where to store parsed data */
    void      *readInfo;         /* Extra info for ream Enums like values */
    int32_t   rangeMin;          /* Min value */
    int32_t   rangeMax;          /* Max value */
} SyntaxEntry;

/******************************************************************************
FUNCTION NAME: ParseConfig

ARGUMENTS: aFileName, aSyntax, aNumSyntax

DESCRIPTION: This function parse the tokens.

RETURNS: EParseStatus

 ******************************************************************************/
EParseStatus ParseConfig(char *aFileName, const SyntaxEntry **aSyntax, int aNumSyntax);

#endif /* PARSER_H */
