/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
 *  @file openvx_lib.h
 *  @brief This file contains the AVC Decoder functions
 */
/**
    @defgroup grp_openvx_comp OpenVX Component
    @ingroup grp_openvx
*/

#ifndef OPENVX_LIB
#define OPENVX_LIB

#include <stdint.h>

/** OPENVX State */
typedef uint32_t OpenVXLib_State;

/** OPENVX Library process flags. These flags needs to be left shifted by the corrsponding stream ID ::OpenVXLib_StreamID*/
typedef uint32_t OpenVXLib_Flags;
#define OPENVX_LIB_BUSY                0x0001UL /**< Busy */
#define OPENVX_LIB_DONE                0x0002UL /**< Job done */
#define OPENVX_LIB_ERROR               0x0004UL /**< Error occured */


/** Size of the  OPENVX library private data in words*/
#define OPENVX_LIB_DATA0_SIZE    (0x20UL)
#define OPENVX_LIB_DATA1_SIZE    (0x80UL)
#define OPENVX_LIB_DATA_SIZE     (OPENVX_LIB_DATA0_SIZE + OPENVX_LIB_DATA1_SIZE)

/** (1 << OPENVX_LIB_TEMP_BUF_SHIFT) is the size of the temp buffer needed for each job */
#define OPENVX_LIB_TEMP_BUF_SHIFT  (6UL)

/** OPENVX Libray Ccontext structure */
typedef struct _OpenVXLib_Context  {
    uint32_t magic;                 /**< Magic to verify the authenticity */

    OpenVXLib_State state;            /**< Current state of the library */
#define OPENVX_LIB_STATE_RESET    0UL /**< Reset state */
#define OPENVX_LIB_STATE_INIT     1UL /**< Library Initialized */
#define OPENVX_LIB_STATE_IDLE     2UL /**< Ready for decoding */
#define OPENVX_LIB_STATE_RUNNING  3UL /**< Decoding under process */
#define OPENVX_LIB_STATE_ERROR    4UL /**< Error occured */
    uint32_t numPPCores;              /**< Maximum number of pp cores alloted */
    uint32_t data[OPENVX_LIB_DATA_SIZE];            /**< Private data */
} OpenVXLib_Context;

/** OpenVX image information structure */
typedef struct _OpenVXLib_StorageInfo {
    uint8_t *storageAddr;     /**< Address of the storage */
    uint32_t storageSize;     /**< Size of the storage in bytes */
} OpenVXLib_StorageInfo;

/** OPENVX Libray Rectangle */
typedef struct _OpenVXLib_Rectangle  {
    uint32_t startX;                /**< Top left X coordinate  */
    uint32_t startY;                /**< Top left Y coordinate  */
    uint32_t endX;                  /**< Bottom right X coordinate  */
    uint32_t endY;                  /**< Bottom right Y coordinate  */
} OpenVXLib_Rectangle;


/** OPENVX Libray Keypoint */
typedef struct _OpenVXLib_KeyPoint  {
    int32_t x;                /**< X coordinate  */
    int32_t y;                /**< Y coordinate  */
    int32_t strength;         /**< The strength of the keypoint  */
    int32_t scale;            /**< Unfilled  */
    int32_t orientation;      /**< Unfilled  */
    int32_t trackingStatus;   /**< Tracking status */
#define OPENVX_LIB_KEY_POINT_LOST        (0)
#define OPENVX_LIB_KEY_POINT_DETECTED    (1)
#define OPENVX_LIB_KEY_POINT_TRACKED     (2)
    int32_t error;            /**< Unfilled */
} OpenVXLib_KeyPoint;

/**
 *  @brief Initialize the OPENVX library
 *
 *  @details Initializes the context variables of the library
 *
 *  @param [in] aContext Pointer to the library sttucture
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_STATE  aContext->state is not ::OPENVX_LIB_STATE_RESET
 *  @retval ::BCM_ERR_OK           Otherwise
 */
int32_t OpenVXLib_Init(OpenVXLib_Context *aContext);

/**
 *  @brief Boots the PP with provided binary image
 *
 *  @details Boots the PP with provided binary image
 *
 *  @param [in] aContext Pointer to library context structure
 *  @param [in] aConfig  16-bit Config IDs
 *  @param [in] aBinPtr  Pointer to binary data
 *  @param [in] aBinSize Size of binary data
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval ::BCM_ERR_INVAL_PARAMS aBinPtr is NULL
 *  @retval ::BCM_ERR_INVAL_PARAMS aBinSize is 0
 *  @retval ::BCM_ERR_INVAL_PARAMS aBinPtr is provided with invalid data
 *  @retval ::BCM_ERR_INVAL_PARAMS aConfig is not valid
 *  @retval ::BCM_ERR_UNKNOWN      PP_BootCore() failed
 *  @retval ::BCM_ERR_OK           Otherwise
 */
int32_t OpenVXLib_Boot(OpenVXLib_Context *aContext,
                     uint32_t aNumPPCores,
                     const uint8_t *const aBinPtr,
                     uint32_t aBinSize);
/**
 *  @brief Remove emulation prevention bytes
 *
 *  @details Removes emulation prevention bytes from input buffer and copies to
 *  data to output buffer
 *
 *  @param [out]   aOut  Pointer to store the output
 *  @param [in]    aIn   pointer to input buffer
 *  @param [inout] aSize Size of the input buffer.
 *                       It will get updated with size used after the EPB copy
 *
 *  @returns None
 */
int32_t OpenVXLib_Start(OpenVXLib_Context *aContext,
                        uint8_t *aGraphInfo,
                        const OpenVXLib_StorageInfo *aStorageInfo,
                        uint8_t *aTableAddr,
                        uint32_t aUniqueID);

/**
 *  @brief Process Events from v-MP
 *
 *  @details Process Events from v-MP.  (Shall be called by client
 *           when MAILBOX event is triggered to client)
 *
 *  @param [in] aContext Pointer to library context structure
 *  @param [in] aTempMem   Pointer to the temporary memory
 *
 *  @retval uint32_t Bitmask as described in ::OpenVXLib_Flags
 */
OpenVXLib_Flags OpenVXLib_Process(OpenVXLib_Context *aContext,
                                  uint8_t *aGraphInfo,
                                  const OpenVXLib_StorageInfo *aStorageInfo,
                                  uint8_t *aTableAddr,
                                  uint32_t aUniqueID);
/**
 *  @brief Deinitialize the OPENVX library
 *
 *  @details Deinitialize the OPENVX library
 *
 *  @param [inout] aContext Pointer to library context structure
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval Others                 as returned by PP_FreeCore() if it fails.
 *  @retval ::BCM_ERR_OK           success
 */
int32_t OpenVXLib_DeInit(OpenVXLib_Context *aContext);

#endif /* OPENVX_LIB */
