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
File Name:  image_to_array.c
Descritpion: This file implements isp image dump.
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <bcm_err.h>
#include <isys.h>

/* Macros */
#define DEBUG_ERR printf

const char* copyright_hdr = "*****************************************************************************";
const char* copyright = " Copyright 2016-2019 Broadcom Limited.  All rights reserved.\n"
"\n"
" This program is the proprietary software of Broadcom Limited and/or its\n"
" licensors, and may only be used, duplicated, modified or distributed pursuant\n"
" to the terms and conditions of a separate, written license agreement executed\n"
" between you and Broadcom (an \"Authorized License\").\n"
"\n"
" Except as set forth in an Authorized License, Broadcom grants no license\n"
" (express or implied), right to use, or waiver of any kind with respect to the\n"
" Software, and Broadcom expressly reserves all rights in and to the Software\n"
" and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED\n"
" LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD\n"
" IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.\n"
"\n"
"  Except as expressly set forth in the Authorized License,\n"
" 1. This program, including its structure, sequence and organization,\n"
"    constitutes the valuable trade secrets of Broadcom, and you shall use all\n"
"    reasonable efforts to protect the confidentiality thereof, and to use this\n"
"    information only in connection with your use of Broadcom integrated\n"
"    circuit products.\n"
"\n"
" 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED \"AS IS\"\n"
"    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR\n"
"    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT\n"
"    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED\n"
"    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A\n"
"    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,\n"
"    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.\n"
"    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE\n"
"    SOFTWARE.\n"
"\n"
" 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS\n"
"    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,\n"
"    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO\n"
"    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN\n"
"    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS\n"
"    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER\n"
"    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF\n"
"    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.\n";

/******************************************************************************
* FUNCTION NAME: ImageToArray
* ARGUMENTS: afilename -> file name to dump
*            aStructPtr -> structure to dump
*            aSize -> total size
* RETURNS:
* 0 or 1
*******************************************************************************/
int32_t ImageToArray(char *afilename, unsigned char *aStructPtr, int aSize)
{
    int32_t ret = BCM_ERR_OK;
    FILE *fp = NULL;
    int32_t i;
    char fileNameBuf[ISYS_MAX_NAME_CHAR] = {0};

    sprintf(fileNameBuf, "%s%s", afilename, ".c");

    fp = fopen(fileNameBuf, "wb");
    if (NULL == fp) {
        DEBUG_ERR("%s: Failed to open file\n", __FUNCTION__);
        ret = BCM_ERR_UNKNOWN;
        goto exit;
    }

    fprintf(fp, "/%s\n", copyright_hdr);
    fprintf(fp, "%s\n", copyright);
    fprintf(fp, "%s/\n", copyright_hdr);
    fprintf(fp, "#include <inttypes.h>\n");
    fprintf(fp, "%s %s%s = %uUL;\n", "const uint32_t", afilename, "_array_size",aSize);
    fprintf(fp, "%s %s%s", "const uint8_t", afilename, "_array[] = {");

    for (i = 0; i < aSize; i++) {
        if ((i&15) == 0) {
            fprintf(fp, "\n");
        }
        fprintf(fp, "0x%02x,", *aStructPtr);
        aStructPtr++;
    }

    fprintf(fp, "\n};\n");

exit:
    if (NULL != fp) {
       fclose(fp);
    }
    return ret;
}
