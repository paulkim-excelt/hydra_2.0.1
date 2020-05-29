/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

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
/**
 *  @file stitch_lib.h
 *  @brief This file contains the AVC Encoder functions
 */
#ifndef STCH_LIB
#define STCH_LIB

#include <stdint.h>


/**
* STCH process status defines
*/
#define STCH_LIB_DATA_PROCESSED    (0x1UL) /**< MB Row processed */

/**
* STCH state
*/
typedef uint32_t STCHLib_State;

/**
* STCH context structure
*/
typedef struct _STCHLib_Context  {
    uint32_t magic;                         /**< Magic to verify the authenticity   */
#define STCH_LIB_STATE_RESET       (0UL)    /**< Reset state */
#define STCH_LIB_STATE_INIT        (1UL)    /**< Library Initialized */
#define STCH_LIB_STATE_IDLE        (2UL)    /**< HW is ready */
#define STCH_LIB_STATE_RUNNING     (3UL)    /**< HW is busy processing */
#define STCH_LIB_STATE_ERROR       (4UL)    /**< Error occured */
    STCHLib_State state;                    /**< Current state of the Library       */

    uint32_t data[32UL];                    /**< Context Data */
} STCHLib_Context;


/**
* STCH input YUV buffer structure
*/
typedef struct _STCHLib_YUVBuffer {
    uint8_t *lumaAddr;              /**< Pointer to luma data */
    uint8_t *cbAddr;                /**< Pointer to chroma cb data */
    uint8_t *crAddr;                /**< Pointer to chroma cr data */
    uint16_t lumaWidth;             /**< Luma Width */
    uint16_t lumaHeight;            /**< Luma Height */
    uint16_t chromaWidth;           /**< Chroma Width */
    uint16_t chromaHeight;          /**< Chroma Height */
    uint16_t lumaStride;            /**< Luma stride */
    uint16_t chromaStride;          /**< Chroma stride */
} STCHLib_YUVBuffer;

/**
 *  @brief Initialize the STCH library
 *
 *  @details Initializes the context variables of the library
 *
 *  @param [in] aContext Pointer to the library sttucture
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_STATE  aContext->state is not ::STCH_LIB_STATE_RESET
 *  @retval ::BCM_ERR_OK           Otherwise
 */
int32_t STCHLib_Init(STCHLib_Context *aContext);

/**
 *  @brief Boots the PP with provided binary image
 *
 *  @details Boots the PP with provided binary image
 *
 *  @param [in] aContext Pointer to library context structure
 *  @param [in] aConfig  16-bit Config IDs
 *  @param [in] aBinPtr  Pointer to binary data
 *  @param [in] aBinSize Size of binary data
 *  @param [in] aTempMem pointer to the temparory memory
 *              This should be 16KB + (WIDTH*2) aligned to 1KB multiples
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval ::BCM_ERR_INVAL_PARAMS aBinPtr is NULL
 *  @retval ::BCM_ERR_INVAL_PARAMS aBinSize is 0
 *  @retval ::BCM_ERR_INVAL_PARAMS aBinPtr is provided with invalid data
 *  @retval ::BCM_ERR_INVAL_PARAMS aConfig is not valid
 *  @retval ::BCM_ERR_UNKNOWN      PP responce is invalid
 *  @retval ::BCM_ERR_OK           Otherwise
 */
int32_t STCHLib_Boot(STCHLib_Context *aContext,
                  uint16_t aConfig,
                  const uint8_t *const aBinPtr,
                  uint32_t aBinSize);

/**
 *  @brief Complete encoding of current slice
 *
 *  @details Complete current slice with trailing bits after completion of
 *  currently processing MB Rows
 *
 *  @param [in] aContext Pointer to library context structure
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval ::BCM_ERR_INVAL_STATE  aContext->state is not ::STCH_LIB_STATE_RUNNING
 *  @retval Others                 as returned by PP_SendMsg()
 */
int32_t STCHLib_DecimateCopy(STCHLib_Context *aContext,STCHLib_YUVBuffer *inYuvFrame, STCHLib_YUVBuffer *outYuvFrame);

/**
 *  @brief Process Events from PP
 *
 *  @details Process Events from PP (shall be called by client when MAILBOX event is triggered to client.
             Client shall call this API in a loop until this API returns zero)
 *
 *  @param [in] aContext Pointer to library context structure
 *
 *  @retval Bitmask as described in STCH Process Status
 */
uint32_t STCHLib_Process(STCHLib_Context *aContext);
/**
 *  @brief Deinitialize the STCH library
 *
 *  @details Deinitialize the STCH library
 *
 *  @param [inout] aContext Pointer to library context structure
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval Others                 as returned by PP_FreeCore() if it fails.
 *  @retval ::BCM_ERR_OK           success
 */
int32_t STCHLib_DeInit(STCHLib_Context *aContext);

#endif /* STCH_LIB */
