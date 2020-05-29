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
File Name:  parser.c
Descritpion: This file implements parser logic.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

/* Macros */
#define MAX_KEYWORD (50UL)
#define MAX_SYNTAX  (6UL)

/* Globals */
char modeString[MAX_NUM_MODES][MAX_LINE] = {{0}};

/* Debug Logs */
#ifdef DEBUG
#define PARSER_DEBUG printf
#else
#define PARSER_DEBUG
#endif

#define DEBUG_ERR printf

/******************************************************************************
FUNCTION NAME: ValidateDataValue

ARGUMENTS: aVal, aMin, aMax

DESCRIPTION: This function validate the aVal.

RETURNS: EParseStatus

 ******************************************************************************/
int32_t ValidateDataValue(uint32_t aVal, uint32_t aMin, uint32_t aMax)
{
    int32_t valid = 0;

    if (aVal >= aMin && aVal <= aMax) {
                valid = 1;
        }
    if (!valid)
        DEBUG_ERR("Error: Out of range data %d min %d max %d\n", aVal, aMin, aMax);
    return valid;
}

/******************************************************************************
FUNCTION NAME: IsDigit

ARGUMENTS: aPtr

DESCRIPTION: This function checks for digit.

RETURNS: EParseStatus

 ******************************************************************************/
EParseStatus IsDigit(const char *aPtr)
{
    EParseStatus status = eSuccess;
    int32_t i;
    int32_t size = strlen(aPtr);

    for(i = 0; i < size; i++) {
        if (!isdigit(*(aPtr++)))
            status = eError;
    }
    return status;
}

/******************************************************************************
FUNCTION NAME: RemoveExtraSpace

ARGUMENTS: aLineBuffer

DESCRIPTION: This function removes extra space.

RETURNS: char *

 ******************************************************************************/
char* RemoveExtraSpace(char *aLineBuffer) {
    if (aLineBuffer) {
        while (*aLineBuffer == '\t' || *aLineBuffer == ' ')
                aLineBuffer++;
    }
    return aLineBuffer;
}

/******************************************************************************
FUNCTION NAME: ValidatehexData

ARGUMENTS: aLineBuffer

DESCRIPTION: This function validates the hex data.

RETURNS: EParseStatus

 ******************************************************************************/
EParseStatus ValidatehexData(char *aLineBuffer)
{
    char *addrPtr;
    EParseStatus status = eSuccess;

    if (!strstr(aLineBuffer, ",")) {
        DEBUG_ERR("Syntax Error at %s\n", aLineBuffer);
        status = eError;
    } else {
        addrPtr = strtok(aLineBuffer, ",");
        while (addrPtr != NULL) {
            if (!strstr(addrPtr, "x")) {
                status = eError;
                DEBUG_ERR("Error not in hex format %s\n", addrPtr);
            }
        addrPtr = strtok(NULL, ",");
        }
    }
    return status;
}

/******************************************************************************
FUNCTION NAME: ParsehexData

ARGUMENTS: aAddr aFp

DESCRIPTION: This function parse Hex data.

RETURNS: EParseStatus

 ******************************************************************************/
EParseStatus ParsehexData(void *aAddr, FILE *aFp)
{
    EParseStatus status = eSuccess;
    char lineBuf[MAX_LINE] = {0};
    char localBuf[30] = {0};
    char *localBufPtr = &localBuf[0];
    char *lineBufPtr = NULL;
    char *startPtr = NULL;
    char *endPtr = NULL;
    uint32_t dataIndex = 0UL;

    if (NULL == aAddr || NULL == aFp) {
        DEBUG_ERR("%s: Null pointer\n", __FUNCTION__);
        status = eError;
        goto exit;
    }

    memset((*(RegDataConfig *)aAddr).cfg, 0, MAX_BUF_SIZE);

    while(1){
        if (dataIndex > MAX_BUF_SIZE) {
            DEBUG_ERR("%s: Reached MAX_BUF_SIZE\n", __FUNCTION__);
            status = eError;
            goto exit;
        }
        fgets(lineBuf, MAX_LINE, aFp);
        strtok(lineBuf, "\n");
        lineBufPtr = RemoveExtraSpace(&lineBuf[0]);
        if (!strcmp(lineBufPtr, "*")) {
            /* Reached end */
            break;
        } else if (!strcmp(lineBufPtr, "\n")) {
            /* Do-Noting */
        } else {
            strcpy(localBufPtr, lineBufPtr);
            status = ValidatehexData(localBufPtr);
            if (eSuccess != status) {
                DEBUG_ERR("%s: Not valid hex data\n", __FUNCTION__);
                goto exit;
            }
            (*(RegDataConfig *)aAddr).cfg[dataIndex].addr = strtoul(lineBufPtr, &endPtr, 16);
            startPtr = strtok(endPtr, ",");
            (*(RegDataConfig *)aAddr).cfg[dataIndex].data = strtoul(startPtr, &endPtr, 16);
            dataIndex++;
        }
    };

    /* Update final index */
    (*(RegDataConfig *)aAddr).count = dataIndex;
exit:
    return status;
}

/******************************************************************************
FUNCTION NAME: ParseUint32Data

ARGUMENTS: aData32 aAddr aMin aMax

DESCRIPTION: This function parse uint32 data.

RETURNS: EParseStatus

 ******************************************************************************/
EParseStatus ParseUint32Data(uint32_t aData32, void *aAddr, uint32_t aMin, uint32_t aMax)
{
    EParseStatus status = eSuccess;

    if (ValidateDataValue(aData32, aMin, aMax)) {
        if (aAddr) {
            *(uint32_t *)aAddr = aData32;
        } else {
            DEBUG_ERR("No store Addr for data %d\n", aData32);
        }
    } else {
        DEBUG_ERR("Error : Data is out of range %d\n", aData32);
        status = eError;
    }
    return status;
}

/******************************************************************************
FUNCTION NAME: ParseUint16Data

ARGUMENTS: aData16 aAddr aMin aMax

DESCRIPTION: This function parse uint16 data.

RETURNS: EParseStatus

 ******************************************************************************/
EParseStatus ParseUint16Data(uint16_t aData16, void *aAddr, uint16_t aMin, uint16_t aMax)
{
    EParseStatus status = eSuccess;

    if (ValidateDataValue(aData16, aMin, aMax)) {
        if (aAddr) {
            *(uint16_t *)aAddr = aData16;
        } else {
            DEBUG_ERR("No store Addr for data %d\n", aData16);
        }
    } else {
        DEBUG_ERR("Error : Data is out of range %d\n", aData16);
        status = eError;
    }
    return status;
}

/******************************************************************************
FUNCTION NAME: ParseEnumData

ARGUMENTS: aPtr aReadInfo aAddr

DESCRIPTION: This function parse enum data.

RETURNS: EParseStatus

******************************************************************************/
EParseStatus ParseEnumData(char *aPtr, void *aReadInfo, void *aAddr)
{
    EParseStatus status = eSuccess;
    EnumReadInfo *enumInfo = (EnumReadInfo *)aReadInfo;
    const char **name = enumInfo->enumNames;
    int32_t *value = enumInfo->values;

    while (*name != NULL && value)
    {
        if (!strcmp(aPtr, *name)) {
            if (aAddr){
                *(int32_t *)aAddr = *value;
                break;
            }
            else {
                DEBUG_ERR("No store addr Enum matched %s & value %d\n", aPtr, *value);
                break;
            }
        }
        name++;
        value++;
    }
    return status;
}

/******************************************************************************
FUNCTION NAME: ParseConfig

ARGUMENTS: aFileName, aSyntax, aNumSyntax

DESCRIPTION: This function parse the tokens.

RETURNS: EParseStatus

 ******************************************************************************/
EParseStatus ParseConfig(char *aFileName, const SyntaxEntry **aSyntax, int aNumSyntax)
{
    EParseStatus status = eSuccess;
    int32_t i = 0;
    int32_t j = 0;
    char *lineBufPtr = NULL;
    uint16_t data16 = 0U;
    uint32_t data32 = 0UL;
    char *ptr = NULL;
    char *endPtr = NULL;
    FILE *resultFP = NULL;
    uint32_t hexData = 0UL;
    SyntaxEntry *syntax[MAX_SYNTAX] = {0};
    char lineBuf[MAX_LINE] = {0};
    char subLineBuf[MAX_LINE] = {0};
    int32_t firstSyntax = 0;
    char *token = NULL;
    static uint32_t modeImageCount = 0UL;

    memset(&lineBuf, 0, sizeof(lineBuf));

    if ((aNumSyntax > MAX_SYNTAX) || (0 == aNumSyntax)) {
        DEBUG_ERR("Error: Wrong Numsyntax \n");
        status = eError;
        return status;
    }

    if (NULL == aSyntax) {
        DEBUG_ERR("Error: Null syntax\n");
        status = eError;
        return status;
    }

    /* Copy to local syntax array */
    memcpy((void *)syntax, aSyntax, sizeof(SyntaxEntry *)*aNumSyntax);

    FILE *fp = fopen(aFileName, "r");
    if (NULL == fp) {
        DEBUG_ERR("Error: fail to open file %s\n", aFileName);
        status = eError;
        return status;
    }
start:
    while(fgets(lineBuf, MAX_LINE, fp) != NULL) {
        /* Strip down new lines */
        strtok(lineBuf, "\n");
        /* Remove extra spaces */
        lineBufPtr = RemoveExtraSpace(&lineBuf[0]);
        if (!strcmp(lineBuf, "\n")) {
            /*Do Noting*/
        } else if (!strcmp(lineBufPtr, "end")) {
            if (firstSyntax) {
                j++;
                i = 0;
            }
            if (j >= aNumSyntax)
                break;
        }
        else {
            strtok(lineBufPtr, ":");
            if (!strncmp(lineBufPtr,"mode_", 5)) {
                strcpy(subLineBuf, lineBufPtr);
                strcpy(modeString[modeImageCount], lineBufPtr);
                token = strtok(subLineBuf, "_");
                modeImageCount++;
            }
            while(1) {
                if (NULL == (*(syntax + j)))
                    goto start;
                if (!strcmp((*(syntax + j))[i].keyword, ""))
                    break;
                if ((!strcmp(lineBufPtr, (*(syntax + j))[i].keyword)) ||
                        (token && !(strncmp(lineBufPtr, (*(syntax + j))[i].keyword, 5)))) {
                    if (0 == firstSyntax) {
                        firstSyntax = 1;
                    }
                    ptr = (char *)strtok(NULL, ":");
                    ptr = RemoveExtraSpace(ptr);
                    /* Data type */
                    switch((*(syntax + j))[i].type) {
                        case eChar:
                            if (NULL == ptr) {
                                DEBUG_ERR("No Parsed data\n");
                            } else if ((*(syntax + j))[i].addr) {
                                strcpy((char *)(*(syntax + j))[i].addr, ptr);
                            } else {
                                DEBUG_ERR("%s: No store address for %s\n", __FUNCTION__, ptr);
                            }
                            break;
                        case eUint16:
                            if (NULL == ptr) {
                                DEBUG_ERR("Error: No data present near %s:\n", lineBufPtr);
                            } else if (!IsDigit(ptr)) {
                                data16 = atoi(ptr);
                                status = ParseUint16Data(data16, (*(syntax + j))[i].addr,
                                        (*(syntax + j))[i].rangeMin, (*(syntax + j))[i].rangeMax);
                                if (eSuccess != status) {
                                    DEBUG_ERR("Error:Expect digit here\n");
                                }
                            }
                            break;
                        case eUint32:
                            if (NULL == ptr) {
                                DEBUG_ERR("Error: No data present near %s:\n", lineBufPtr);
                            } else if (!IsDigit(ptr)) {
                                data32 = atoi(ptr);
                                status = ParseUint32Data(data32, (*(syntax + j))[i].addr,
                                        (*(syntax + j))[i].rangeMin, (*(syntax + j))[i].rangeMax);
                                if (eSuccess != status) {
                                    DEBUG_ERR("Error:Expect digit here\n");
                                }
                            }
                            break;
                        case eHexData:
                            status = ParsehexData((*(syntax + j))[i].addr, fp);
                            if (eSuccess != status) {
                                DEBUG_ERR("Error: In ParsehexData\n");
                            }
                            break;
                        case eEnum:
                            status = ParseEnumData(ptr, (*(syntax + j))[i].readInfo, (*(syntax + j))[i].addr);
                            if (eSuccess != status) {
                                DEBUG_ERR("Error: In ParseEnumData\n");
                            }
                            break;
                        case eHex:
                            hexData = strtoul(ptr, &endPtr, 16);
                            *(uint32_t *)(*(syntax + j))[i].addr = hexData;
                            break;
                        default:
                            DEBUG_ERR("%s: Error: Unknown data type\n", __FUNCTION__);
                            break;
                    }
                    if (eSuccess != status) {
                        goto func_exit;
                    }
                    (*(syntax + j))[i].parsed = 1;
                    /* For common strings in syntax table */
                    if (token && !(strncmp(lineBufPtr, (*(syntax + j))[i].keyword, 5))) {
                        i++;
                    } else {
                        i = 0;
                    }
                    break;
                } else if ((eChar == (*(syntax + j))[i].type) &&
                        (NULL == (*(syntax + j))[i].addr) &&
                        (1 != (*(syntax + j))[i].parsed)) {
                    /* If first syntax not present in syntax table */
                    /* No need of jumping syntax entry in syntax table */
                    if (0 == firstSyntax) {
                        goto start;
                    } else {
                        /* Syntax itself is not present in the config file */
                        /* Jump to next syntax entry in syntax table */
                        j++;
                        i = 0;
                    }
                } else {
                    i++;
                }
            }
        }
    }
    /* Parse results */
    resultFP = fopen("parse_result.log", "a+");
    if (!resultFP) {
        DEBUG_ERR("%s:Error: Failed to open file \n", __FUNCTION__);
        status = eError;
        return status;
    }
    fprintf(resultFP, "#######Parse Result########\n");
    j = 0;
    i = 0;
    do {
        while(strcmp((*(syntax + i))[j].keyword, "")) {
            if ((*(syntax + i))[j].parsed) {
                fprintf(resultFP, "Keyword: %s parsed %d\n",
                        (*(syntax + i))[j].keyword, (*(syntax + i))[j].parsed);
            } else {
                fprintf(resultFP, "Keyword: %s not Parsed\n",
                        (*(syntax + i))[j].keyword);
            }
            j++;
        }
        i++;
        j = 0;
    } while(i < aNumSyntax);
    fprintf(resultFP, "#######End#####\n");
    fclose(fp);

func_exit:
    return status;
}
