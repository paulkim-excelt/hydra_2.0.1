/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
#ifndef FACE_DETECT_INTERNAL_H
#define FACE_DETECT_INTERNAL_H


#include <openvx_lib.h>

#define FACE_DETECT_UNIQUE_ID (4165807973UL)

#define FACE_DETECT_NUM_PP_CORES (4UL)

#define FACE_DETECT_MAX_DATA_ENTRIES (4UL)

#define FACE_DETECT_TEMP_STORAGE_IDX (0UL)
#define FACE_DETECT_TEMP_STORAGE_SIZE (562184UL)

#define FACE_DETECT_INPUT_IMG_IDX (1UL)
#define FACE_DETECT_INPUT_IMG_SIZE (921600UL)



#define FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_IDX (2UL)
typedef OpenVXLib_Rectangle FACE_DETECT_OUTPUT_RECTANGLES_ARRAY_TYPE;
#define FACE_DETECT_OUTPUT_RECTANGLES_MAX_COUNT (2944UL)
#define DEFINE_FACE_DETECT_OUTPUT_RECTANGLES(aName)\
    OpenVXLib_Rectangle aName[2944UL]


#define FACE_DETECT_NUM_RECTANGLES_SCALAR_IDX (3UL)
typedef int32_t FACE_DETECT_NUM_RECTANGLES_SCALAR_TYPE;

#endif /* FACE_DETECT_INTERNAL_H */
