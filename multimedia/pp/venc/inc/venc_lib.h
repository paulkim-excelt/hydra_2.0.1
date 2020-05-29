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
/**
 *  @file venc_lib.h
 *  @brief This file contains the AVC Encoder functions
 */
#ifndef __VENC_LIB__
#define __VENC_LIB__

#include <stdint.h>


/** Library private data size */
#define VENC_LIB_CONTEXT_DATA_SIZE (32UL)

/** FW ID to boot */
#define VENC_LIB_FW0_ID       (0U)
#define VENC_LIB_FW1_ID       (1U)

/**
* VENC Library state
*/
typedef uint32_t VENCLib_State;

/**
* VENC process status defines
*/
typedef uint32_t VENCLib_ProcStatus;
#define VENC_LIB_MBROW_PROCESSED    (0x1UL)   /**< MB Row processed */
#define VENC_LIB_BITSTREAM_FILLED   (0x20UL)  /**< Bitstream available */

/**
* VENC context structure
*/
typedef struct _VENCLib_Context  {
    uint32_t magic;                         /**< Magic to verify the authenticity   */
    VENCLib_State state;                    /**< Current state of the Library       */
#define VENC_LIB_STATE_RESET       (0UL)    /**< Reset state */
#define VENC_LIB_STATE_INIT        (1UL)    /**< Library Initialized */
#define VENC_LIB_STATE_IDLE        (2UL)    /**< HW is ready */
#define VENC_LIB_STATE_RUNNING     (3UL)    /**< HW is busy processing */
#define VENC_LIB_STATE_ERROR       (4UL)    /**< Error occured */
    uint32_t width;                         /**< Width of the video */
    uint32_t height;                        /**< Height of the video */
    uint32_t data[VENC_LIB_CONTEXT_DATA_SIZE];/**< Context Data */
} VENCLib_Context;

/**
* VENC configuration structure
*/
typedef struct _VENCLib_Config {
    uint32_t magic;                 /**< Magic 0x45435641*/
    uint32_t version;               /**< Version of config */
    uint32_t width;                 /**< Width of the video */
    uint32_t height;                /**< Height of the video */
    uint32_t profile;               /**< Profile of the encoder */
#define VENC_PROFILE_I_ONLY         (0UL)
#define VENC_PROFILE_IP             (1UL)
    uint32_t entropyCoding;         /** CAVLC/CABAC to be used */
#define VENC_ENTROPY_CABAC          (0UL)
#define VENC_ENTROPY_CAVLC          (1UL)
    uint32_t fps;                   /**< Framerate */
    uint32_t bitrate;               /**< Bitrate in kilo bits per second. Should be 0 for constant quality */
    uint32_t intraPeriod;           /**< The distance in frames between Intra frames */
    uint32_t quality;               /**< Quality factor */
    uint32_t hdrRefreshPeriod;      /**< The interval (in frames) at which to re-insert SPS/PPS header data into the bitstream */
    uint32_t numPPCores;            /**< Number of PP cores to be used in library */
} VENCLib_Config;

/**
* VENC input YUV buffer structure
*/
typedef struct _VENCLib_YUVBuffer {
    uint8_t *lumaAddr;                      /**< Pointer to luma data */
    uint8_t *cbAddr;                        /**< Pointer to chroma cb data */
    uint8_t *crAddr;                        /**< Pointer to chroma cr data */
    uint32_t lumaStride;                    /**< Luma stride */
    uint32_t chromaStride;                  /**< Chroma stride */
    uint32_t frameNo;                       /**< Current Frame number */
    uint32_t mbRow;                         /**< Current Macroblock Row number  */
} VENCLib_YUVBuffer;

/**
* VENC bitstream information structure
*/
typedef struct _VENCLib_BitStream {
    uint8_t *bsBuffer;                      /**< Bitstream pointer passed to Library */
    uint32_t offset;                        /**< Offset of the current bitstream addr */
    uint32_t size;                          /**< Size of the bitstream in bytes */
    uint32_t nalType;                       /**< NAL type */
#define VENC_NALTYPE_SPS (0UL)
#define VENC_NALTYPE_PPS (1UL)
#define VENC_NALTYPE_VCL (2UL)
#define VENC_NALTYPE_ERR (3UL)
    uint32_t frameStart;                    /**< Indicator for frame start */
    uint32_t frameEnd;                      /**< Indicator for frame end */
} VENCLib_BitStream;


/**
 *  @brief Initialize the VENC library
 *
 *  @details Initializes the context variables of the library
 *
 *  @param [in] aContext Pointer to the library sttucture
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_STATE  aContext->state is not ::VENC_LIB_STATE_RESET
 *  @retval ::BCM_ERR_UNKNOWN      Failed to allocate cores
 *  @retval ::BCM_ERR_OK           Otherwise
 */
int32_t VENCLib_Init(VENCLib_Context *aContext);

/**
 *  @brief Configures the VENC library
 *
 *  @details Configures Video Encoder library
 *
 *  @param [in] aContext       Pointer to library context structure
 *  @param [in] aConfig        Pointer to configuration structure
 *  @param [in] aIspID         ISP ID to be used for handshake
 *  @param [in] aScratchMem    Pointer to scratch memory
 *              This should be 128KB + (WIDTH*16*3*2) aligned to 1KB multiples for intra encoder
 *              This should be 128KB + (WIDTH*HEIGHT*3*2) aligned to 1KB multiples for ip encoder
 *  @param [in] aBSBuffer      Pointer to memory to hold the bitstream
 *  @param [in] aBSBufSize     Bit stream memory size
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval ::BCM_ERR_INVAL_PARAMS aConfig is NULL
 *  @retval ::BCM_ERR_INVAL_STATE  aContext->state is not ::VENC_LIB_STATE_INIT
 *  @retval ::BCM_ERR_INVAL_PARAMS aConfig is passed with invalid values
 *  @retval ::BCM_ERR_OK           Otherwise
 */
int32_t VENCLib_Configure(VENCLib_Context *aContext,
                          const VENCLib_Config *aConfig,
                          uint32_t aIspID,
                          uint8_t *aScratchMem,
                          uint8_t *aBSBuffer,
                          uint32_t aBSBufSize);

/**
 *  @brief Boots the PP with provided binary image
 *
 *  @details Boots the PP with provided binary image
 *
 *  @param [in] aContext    Pointer to library context structure
 *  @param [in] aFwID       FW ID to be booted
 *  @param [in] aBinPtr     Pointer to binary data
 *  @param [in] aBinSize    Size of binary data
 *  @param [in] aScratchMem pointer to the scratch memory
 *              This should be 128KB + (WIDTH*16*3*2) aligned to 1KB multiples for intra encoder
 *              This should be 128KB + (WIDTH*HEIGHT*3*2) aligned to 1KB multiples for ip encoder
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
int32_t VENCLib_Boot(VENCLib_Context *aContext,
                  uint16_t aFwID,
                  const uint8_t *const aBinPtr,
                  uint32_t aBinSize,
                  uint8_t *aScratchMem);
/**
 *  @brief starts encoding of a MB Row
 *
 *  @details Start encoding a MB row in a single call. Changes the state of
 *  Encoder to VENC_LIB_STATE_RUNNING if successful.
 *
 *  @param [in] aContext    Pointer to library context structure
 *  @param [in] aYUVBuffer  Structure containing pointers to one MB Row of YUV data
 *  @param [in] aScratchMem pointer to the scratch memory
 *              This should be 128KB + (WIDTH*16*3*2) aligned to 1KB multiples for intra encoder
 *              This should be 128KB + (WIDTH*HEIGHT*3*2) aligned to 1KB multiples for ip encoder
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval ::BCM_ERR_INVAL_PARAMS aYUVBuffer is NULL
 *  @retval ::BCM_ERR_INVAL_PARAMS aYUVBuffer content is invalid
 *  @retval ::BCM_ERR_INVAL_STATE  aContext->state is not ::VENC_LIB_STATE_IDLE
 *  @retval Others                 as returned by PP_SendMsg()
 */

int32_t VENCLib_Encode(VENCLib_Context *aContext,
                       VENCLib_YUVBuffer *aYUVBuffer,
                       uint8_t *aScratchMem);

/**
 *  @brief Process Events from PP
 *
 *  @details Process Events from PP (shall be called by client when MAILBOX event is triggered to client.
             Client shall call this API in a loop until this API returns zero)
 *
 *  @param [in]    aContext Pointer to library context structure
 *  @param [in]    aScratchMem pointer to the scratch memory
 *                 This should be 128KB + (WIDTH*16*3*2) aligned to 1KB multiples for intra encoder
 *                 This should be 128KB + (WIDTH*HEIGHT*3*2) aligned to 1KB multiples for ip encoder
 *  @param [inout] aBsInfo Pointer to the bit stream information structure.
 *                  Client shall fill the bsBuffer feild with the bit stream memory address and remaining
 *                  will be updated if new bitstream NAL unit is available. This would be notified by
 *                  updating the return value with ::VENC_LIB_BITSTREAM_FILLED
 *
 *  @retval Bitmask as described in VENC Process Status
 */
uint32_t VENCLib_Process(VENCLib_Context *aContext,
                         uint8_t* aScratchMem,
                         VENCLib_BitStream *aBsInfo);

/**
 *  @brief Deinitialize the VENC library
 *
 *  @details Deinitialize the VENC library
 *
 *  @param [inout] aContext Pointer to library context structure
 *
 *  @retval ::BCM_ERR_INVAL_PARAMS aContext is NULL
 *  @retval ::BCM_ERR_INVAL_MAGIC  aContext->magic is not valid
 *  @retval Others                 as returned by PP_FreeCore() if it fails.
 *  @retval ::BCM_ERR_OK           success
 */

int32_t VENCLib_DeInit(VENCLib_Context *aContext);

#endif /* __VENC_LIB__ */
