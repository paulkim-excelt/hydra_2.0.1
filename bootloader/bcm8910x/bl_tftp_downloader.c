/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_bl_tftp_impl Implementation
    @ingroup grp_bl_tftp

    @addtogroup grp_bl_tftp_impl
    @{
    @file bl_tftp_downloader.c
    @brief TFTP downloader Implementation
    This source file contains the implementation of TFTP downloader

    @version 0.30 Imported from docx
*/

/* Includes */
#include <bl_log.h>

#if defined(BL_ENABLE_INET)
#include <bl_tftp.h>
#include <bl_downloader.h>
#include <bl_bcm_err.h>
#include <bl_edc.h>
#include <bl_ptm.h>
#include <bl_utils.h>
#include <bl_ethernet.h>
#include <bl_board.h>
#include <bl_inet_port.h>
#include <bcm_time.h>
#include <bl_udp.h>
#endif

#include <bl_command.h>
#include <bl_cntrl.h>
#include <bl_tar.h>

/**
    @name TFTP downloader API IDs
    @{
    @brief API IDs for TFTP downloader
*/
#define BRCM_DSGN_BL_TFTP_DWNLD_STATE_TYPE                  (0x80U) /**< @brief #BL_TFTP_DWNLD_StateType */
#define BRCM_DSGN_BL_TFTP_DWNLD_COMP_TYPE                   (0x81U) /**< @brief #BL_TFTP_DWNLD_CompType */
#define BRCM_DSGN_BL_TFTP_DWNLD_VALIDATE_IMG_HDR_PROC       (0x82U) /**< @brief #TFTP_DWNLD_ValidateImgHdr */
#define BRCM_DSGN_BL_TFTP_DWNLD_VALIDATE_IMG_CRC_PROC       (0x83U) /**< @brief #TFTP_DWNLD_ValidateImgCRC */
#define BRCM_DSGN_BL_TFTP_DWNLD_WRITE_PROC                  (0x84U) /**< @brief #TFTP_DWNLD_Write */
#define BRCM_DSGN_BL_TFTP_DWNLD_START_STATE_HANDLER_PROC    (0x85U) /**< @brief #TFTP_DWNLD_StartStateHandler */
#define BRCM_DSGN_BL_TFTP_DWNLD_DWNLDNG_STATE_HANDLER_PROC  (0x86U) /**< @brief #TFTP_DWNLD_DwnldngStateHandler */
#define BRCM_DSGN_BL_TFTP_DWNLD_END_STATE_HANDLER_PROC      (0x87U) /**< @brief #TFTP_DWNLD_EndStateHandler */
#define BRCM_DSGN_BL_TFTP_DWNLD_STATE_HANDLER_PROC          (0x88U) /**< @brief #TFTP_DWNLD_StateHandler */
#define BRCM_DSGN_BL_TFTP_DWNLD_REPORT_ERROR_PROC           (0x89U) /**< @brief #BL_TFTP_DWNLD_ReportError */
/** @} */

#define BL_GetModuleLogLevel()     (BL_LOG_LVL_INFO)

static BL_COMP_NEVER_INLINE void BL_TFTP_DWNLD_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_CTL_ID, aInstanceID, BRCM_DSGN_BL_TFTP_DWNLD_REPORT_ERROR_PROC, aErr, 4UL, values);
}

#if defined(BL_ENABLE_INET)
/**
    @name BL_TFTP_DWNLD_StateType
    @{
    @brief TFTP downloader state type

    @trace #BRCM_DSGN_BL_TFTP_DWNLD_STATE_TYPE #BRCM_REQ_BL_TFTP_DWNLD_STATE
 */
typedef uint32_t BL_TFTP_DWNLD_StateType;     /**< @brief typedef for TFTP download state */
#define BL_TFTP_DWNLD_STATE_START       (0UL) /**< @brief Start state */
#define BL_TFTP_DWNLD_STATE_DWNLDNG     (1UL) /**< @brief Downloading state */
#define BL_TFTP_DWNLD_STATE_END         (2UL) /**< @brief End state */
/** @} */

/**
    @brief TFTP downloader component type

    @trace #BRCM_DSGN_BL_TFTP_DWNLD_COMP_TYPE #BRCM_REQ_BL_TFTP_DWNLD_COMP
*/
typedef struct {
    uint8_t *startAddr;         /**< @brief Address where downloader starts writing the image */
    uint8_t *endAddr;           /**< @brief Downloader can write only till this end address */
    uint8_t *currAddr;          /**< @brief Current address where downloader is writing the image */
    BL_TFTP_DWNLD_StateType state; /**< @brief State of the TFTP downloader */
    uint32_t imgSize;           /**< @brief Size of the image */
    BL_DWNLD_ImgHdrType imgHdr;    /**< @brief Image header */
    BL_INET_IPAddress rmtIP;       /**< @brief Remote IP */
} BL_TFTP_DWNLD_CompType;

/* Global variables for INET timer processing */
static BCM_TimeType Time1 = {0UL, 0UL};
static BCM_TimeType Time2 = {0UL, 0UL};

typedef void (*img_entry)(void);
extern uint8_t BL_DWNLD_StartAddr[], BL_DWNLD_EndAddr[];

static BL_TFTP_DWNLD_CompType BL_TFTP_DWNLD_Comp =
{
    .startAddr = BL_DWNLD_StartAddr,
    .endAddr = BL_DWNLD_EndAddr,
    .currAddr = BL_DWNLD_StartAddr,
    .state = BL_TFTP_DWNLD_STATE_START,
    .imgSize = 0UL,
};

static void BL_TFTP_DWNLD_SendStatus(uint32_t aPercent)
{
    uint8_t percent = (uint8_t)aPercent;

    BL_SendStatus(BL_TFTP_DWNLD_Comp.rmtIP, BL_STATUS_BUSY, &percent, 1UL);
}

/** @brief Function to validate Image header

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Valid Image header
    @retval     #BL_BCM_ERR_INVAL_MAGIC    Invalid Magic number
    @retval     #BL_BCM_ERR_NOMEM          Image size is greater than the available memory
    @retval     #BL_BCM_ERR_INVAL_PARAMS   Invalid flash ID

    @post None

    @trace  #BRCM_DSGN_BL_TFTP_DWNLD_VALIDATE_IMG_HDR_PROC  #BRCM_ARCH_BL_TFTP_DWNLD_VALIDATE_IMG_HDR

    @limitations None

    @code{.c}
    imgHdr = BL_TFTP_DWNLD_Comp.imgHdr
    flashID = ((imgHdr->flags & BL_DWNLD_FLAG_FLASH_ID_MASK) >> BL_DWNLD_FLAG_FLASH_ID_SHIFT)
    if (BL_DWNLD_ITBL_MAGIC != imgHdr->magicNum) and (BL_DWNLD_TAR_MAGIC_NUM != imgHdr->magicNum)
        return BL_BCM_ERR_INVAL_MAGIC
    if imgHdr->imgSize > (BL_TFTP_DWNLD_Comp.endAddr - BL_TFTP_DWNLD_Comp.startAddr)
        return BL_BCM_ERR_NOMEM
    if flashID > BL_FLASH_MAX_HW_ID
        return BL_BCM_ERR_INVAL_PARAMS
    return BL_BCM_ERR_OK
    @endcode
*/
static int32_t BL_TFTP_DWNLD_ValidateImgHdr(void)
{
    int32_t err = BL_BCM_ERR_OK;
    BL_DWNLD_ImgHdrType *imgHdr = &BL_TFTP_DWNLD_Comp.imgHdr;
    BL_FLASH_HwIDType flashID;

    flashID = ((imgHdr->flags & BL_DWNLD_FLAG_FLASH_ID_MASK) >>
            BL_DWNLD_FLAG_FLASH_ID_SHIFT);
    if (BL_DWNLD_TAR_MAGIC_NUM != imgHdr->magicNum) {
        err = BL_BCM_ERR_INVAL_MAGIC;
        goto err;
    }
    if (imgHdr->imgSize > (BL_TFTP_DWNLD_Comp.endAddr - BL_TFTP_DWNLD_Comp.startAddr)) {
        err = BL_BCM_ERR_NOMEM;
        goto err;
    }
    if (flashID > BL_FLASH_MAX_HW_ID) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        goto err;
    }
err:
    return err;
}

/** @brief Function to validate Image CRC/Checksum

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             CRC/Checksum validation successful
    @retval     others                  Errors from #EDC_ValidateCRC, #EDC_ValidateChcksm

    @post None

    @trace  #BRCM_DSGN_BL_TFTP_DWNLD_VALIDATE_IMG_CRC_PROC  #BRCM_ARCH_BL_TFTP_DWNLD_VALIDATE_IMG_CRC

    @limitations None

    @code{.c}
    imgHdr = BL_TFTP_DWNLD_Comp.imgHdr

    if PTBL_ERR_DET_CRC = (imgHdr->flags & PTBL_ERR_DET_CRC)
        err = EDC_ValidateCRC(BL_TFTP_DWNLD_Comp.startAddr, imgHdr->imgSize,
                imgHdr->errDetCode)
    else if PTBL_ERR_DET_CHECKSUM = (imgHdr->flags & PTBL_ERR_DET_CHECKSUM)
        err = EDC_ValidateChcksm(BL_TFTP_DWNLD_Comp.startAddr, imgHdr->imgSize,imgHdr->errDetCode)
    else
        err = BL_BCM_ERR_OK

    return err
    @endcode
*/
static int32_t BL_TFTP_DWNLD_ValidateImgCRC(void)
{
    int32_t err;
    BL_DWNLD_ImgHdrType *imgHdr = &BL_TFTP_DWNLD_Comp.imgHdr;

    if (PTBL_ERR_DET_CRC == (imgHdr->flags & PTBL_ERR_DET_CRC)) {
        err = BL_EDC_ValidateCRC(BL_TFTP_DWNLD_Comp.startAddr, imgHdr->imgSize,
                imgHdr->errDetCode);
    } else if (PTBL_ERR_DET_CHECKSUM == (imgHdr->flags & PTBL_ERR_DET_CHECKSUM)) {
        err = BL_EDC_ValidateChcksm(BL_TFTP_DWNLD_Comp.startAddr, imgHdr->imgSize,
                imgHdr->errDetCode);
    } else {
        err = BL_BCM_ERR_OK;
    }

    return err;
}

/** @brief Function to write the given data to downloader memory

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf    Input data
    @param[in]      aSize   Size of the buffer

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Data written successfully
    @retval     #BL_BCM_ERR_NOMEM          aSize is greater than the available memory

    @post None

    @trace  #BRCM_DSGN_BL_TFTP_DWNLD_WRITE_PROC  #BRCM_ARCH_BL_TFTP_DWNLD_WRITE

    @limitations None

    @code{.c}
    if aSize > (BL_TFTP_DWNLD_Comp.endAddr - BL_TFTP_DWNLD_Comp.currAddr)
        return BL_BCM_ERR_NOMEM
    else
        BL_BCM_MemCpy(BL_TFTP_DWNLD_Comp.currAddr, aBuf, aSize)
        BL_TFTP_DWNLD_Comp.currAddr += aSize
        return BL_BCM_ERR_OK
    @endcode
*/
static int32_t BL_TFTP_DWNLD_Write(const void *const aBuf,
                                   uint32_t aSize)
{
    int32_t err;

    if (aSize > (BL_TFTP_DWNLD_Comp.endAddr - BL_TFTP_DWNLD_Comp.currAddr)) {
        err = BL_BCM_ERR_NOMEM;
    } else {
        BL_BCM_MemCpy((void *)BL_TFTP_DWNLD_Comp.currAddr, aBuf, aSize);
        BL_TFTP_DWNLD_Comp.currAddr += aSize;
        err = BL_BCM_ERR_OK;
    }

    return err;
}

/** @brief Function to handle TFTP downloader #BL_TFTP_DWNLD_STATE_END state. This API writes the image on the flash.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @retval     void

    @post None

    @trace  #BRCM_DSGN_BL_TFTP_DWNLD_END_STATE_HANDLER_PROC  #BRCM_ARCH_BL_TFTP_DWNLD_END_STATE_HANDLER

    @limitations None

    @code{.c}
    dwnldImgHdr = BL_TFTP_DWNLD_Comp.imgHdr
    flashID = (dwnldImgHdr->flags & BL_DWNLD_FLAG_FLASH_ID_MASK)
                        >> BL_DWNLD_FLAG_FLASH_ID_SHIFT
    pid = ((dwnldImgHdr->flags & BL_DWNLD_FLAG_PID_MASK) >> BL_DWNLD_FLAG_PID_SHIFT) | (((dwnldImgHdr->flags & BL_DWNLD_FLAG_COPY_ID_MASK) >> BL_DWNLD_FLAG_COPY_ID_SHIFT) << 8UL)
    imgID = (dwnldImgHdr->flags & BL_DWNLD_FLAG_IMG_ID_MASK) >>
            BL_DWNLD_FLAG_IMG_ID_SHIFT

    if PT_IMG_PID = pid
        err = PTM_WritePT(flashID, BL_TFTP_DWNLD_Comp.startAddr, BL_TFTP_DWNLD_Comp.imgSize, dwnldImgHdr->errDetCode, BL_TFTP_DWNLD_SendStatus)
    else
        if BL_DWNLD_ITBL_MAGIC = dwnldImgHdr->magicNum
            err = PTM_WriteImg(flashID, pid, imageID, BL_TFTP_DWNLD_Comp.startAddr, BL_TFTP_DWNLD_Comp.imgSize, dwnldImgHdr->errDetCode, BL_TFTP_DWNLD_SendStatus)
        else
            err = TAR_UpdateFlash(flashID, BL_TFTP_DWNLD_Comp.startAddr,
                           BL_TFTP_DWNLD_Comp.imgSize, BL_TFTP_DWNLD_SendStatus)
    if BL_BCM_ERR_OK = err
        BL_SendStatus(&BL_TFTP_DWNLD_Comp.remote, STATUS_SUCCESS, NULL, 0UL)
    else if BL_BCM_ERR_NOT_FOUND = err
        BL_SendStatus(&BL_TFTP_DWNLD_Comp.remote, STATUS_UNKNOWN_ERR, NULL, 0UL)
    else
        BL_SendStatus(&BL_TFTP_DWNLD_Comp.remote, STATUS_UNKNOWN_ERR, NULL, 0UL)
    @endcode
*/
static void BL_TFTP_DWNLD_EndStateHandler(void)
{
    int32_t err;
    BL_DWNLD_ImgHdrType *dwnldImgHdr = &BL_TFTP_DWNLD_Comp.imgHdr;
    BL_FLASH_HwIDType flashID = ((dwnldImgHdr->flags & BL_DWNLD_FLAG_FLASH_ID_MASK)
                        >> BL_DWNLD_FLAG_FLASH_ID_SHIFT);

    /* Check if it's a PT image */
    err = BL_TAR_UpdateFlash(flashID, BL_TFTP_DWNLD_Comp.startAddr,
                    BL_TFTP_DWNLD_Comp.imgSize, BL_TFTP_DWNLD_SendStatus);

    if (BL_BCM_ERR_OK == err) {
        BL_LOG_ERR("Image(pid:0x%x imgId:0x%x) flashed successfully\n", 0UL, 0UL);
        BL_SendStatus(BL_TFTP_DWNLD_Comp.rmtIP, BL_STATUS_SUCCESS, NULL, 0UL);
    } else if (BL_BCM_ERR_NOT_FOUND == err) {
        BL_LOG_ERR("Image(pid:0x%x imgId:0x%x) not found\n", 0UL, 0UL);
        BL_SendStatus(BL_TFTP_DWNLD_Comp.rmtIP, BL_STATUS_UNKNOWN_ERR, NULL, 0UL);
    } else {
        BL_LOG_ERR("Failed to flash image(pid:0x%x imgId:0x%x) err:%d\n", 0UL, 0UL, err);
        BL_SendStatus(BL_TFTP_DWNLD_Comp.rmtIP, BL_STATUS_UNKNOWN_ERR, NULL, 0UL);
    }
}

/** @brief Function to handle TFTP downloader #BL_TFTP_DWNLD_STATE_DWNLDNG state

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf    Input data
    @param[in]      aSize   Size of the buffer

    Return values are documented in reverse-chronological order
    @retval     others      Errors from #BL_TFTP_DWNLD_Write,  #BL_TFTP_DWNLD_ValidateImgCRC

    @post None

    @trace  #BRCM_DSGN_BL_TFTP_DWNLD_DWNLDNG_STATE_HANDLER_PROC  #BRCM_ARCH_BL_TFTP_DWNLD_DWNLDNG_STATE_HANDLER

    @limitations None

    @code{.c}
    err = BL_TFTP_DWNLD_Write(aBuf, aSize)
    if BL_BCM_ERR_OK = err
        BL_TFTP_DWNLD_Comp.imgSize += aSize
        if BL_TFTP_DWNLD_Comp.imgSize = BL_TFTP_DWNLD_Comp.imgHdr.imgSize
            err = BL_TFTP_DWNLD_ValidateImgCRC()

    return err;
    @endcode
*/
static int32_t BL_TFTP_DWNLD_DwnldngStateHandler(const void *const aBuf,
                                                 uint32_t aSize)
{
    int32_t err;

    err = BL_TFTP_DWNLD_Write(aBuf, aSize);
    if (BL_BCM_ERR_OK == err) {
        BL_TFTP_DWNLD_Comp.imgSize += aSize;
        if (BL_TFTP_DWNLD_Comp.imgSize == BL_TFTP_DWNLD_Comp.imgHdr.imgSize) {
            err = BL_TFTP_DWNLD_ValidateImgCRC();
        }
    }

    return err;
}

/** @brief Function to handle TFTP downloader #BL_TFTP_DWNLD_STATE_START state

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf    Input data
    @param[in]      aSize   Size of the buffer

    Return values are documented in reverse-chronological order
    @retval     others      Errors from #BL_TFTP_DWNLD_ValidateImgHdr, #BL_TFTP_DWNLD_Write, #BL_TFTP_DWNLD_ValidateImgCRC

    @post None

    @trace  #BRCM_DSGN_BL_TFTP_DWNLD_START_STATE_HANDLER_PROC  #BRCM_ARCH_BL_TFTP_DWNLD_START_STATE_HANDLER

    @limitations None

    @code{.c}
    buf = aBuf

    BL_BCM_MemCpy(BL_TFTP_DWNLD_Comp.imgHdr, aBuf, BL_DWNLD_ITBL_HDR_SIZE)
    err = BL_TFTP_DWNLD_ValidateImgHdr()
    if BL_BCM_ERR_OK = err
        err = BL_TFTP_DWNLD_Write((buf + BL_DWNLD_ITBL_HDR_SIZE),
                (aSize - BL_DWNLD_ITBL_HDR_SIZE))
        if BL_BCM_ERR_OK = err
            BL_TFTP_DWNLD_Comp.imgSize += (aSize - BL_DWNLD_ITBL_HDR_SIZE)
            if BL_TFTP_DWNLD_Comp.imgSize = BL_TFTP_DWNLD_Comp.imgHdr.imgSize
                err = BL_TFTP_DWNLD_ValidateImgCRC()

    return err
    @endcode
*/
static int32_t BL_TFTP_DWNLD_StartStateHandler(const void *const aBuf,
                                               uint32_t aSize)
{
    int32_t err;
    uint8_t *buf = (uint8_t *)aBuf;

    BL_BCM_MemCpy((void *)&BL_TFTP_DWNLD_Comp.imgHdr, aBuf, BL_DWNLD_ITBL_HDR_SIZE);
    err = BL_TFTP_DWNLD_ValidateImgHdr();
    if (BL_BCM_ERR_OK == err) {
        err = BL_TFTP_DWNLD_Write((void *)(buf + BL_DWNLD_ITBL_HDR_SIZE),
                (aSize - BL_DWNLD_ITBL_HDR_SIZE));
        if (BL_BCM_ERR_OK == err) {
            BL_TFTP_DWNLD_Comp.imgSize += (aSize - BL_DWNLD_ITBL_HDR_SIZE);
            if (BL_TFTP_DWNLD_Comp.imgSize == BL_TFTP_DWNLD_Comp.imgHdr.imgSize) {
                err = BL_TFTP_DWNLD_ValidateImgCRC();
            }
        }
    }

    return err;
}

/** @brief Function to handles TFTP downloader states

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf    Input data
    @param[in]      aSize   Size of the buffer

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Data loading successful
    @retval     #BL_BCM_ERR_INVAL_STATE    Downloader in invalid state
    @retval     others                  Errors from #BL_TFTP_DWNLD_StartStateHandler, #BL_TFTP_DWNLD_DwnldngStateHandler

    @post None

    @trace  #BRCM_DSGN_BL_TFTP_DWNLD_STATE_HANDLER_PROC  #BRCM_ARCH_BL_TFTP_DWNLD_STATE_HANDLER

    @limitations None

    @code{.c}
    err = BL_BCM_ERR_OK
    switch BL_TFTP_DWNLD_Comp.state
    case BL_TFTP_DWNLD_STATE_START
        if NULL != aBuf
            err = BL_TFTP_DWNLD_StartStateHandler(aBuf, aSize)
            if BL_BCM_ERR_OK = err
                BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_DWNLDNG
        break
    case BL_TFTP_DWNLD_STATE_DWNLDNG
        if NULL != aBuf
            err = BL_TFTP_DWNLD_DwnldngStateHandler(aBuf, aSize)
        break
    case BL_TFTP_DWNLD_STATE_END
        BL_TFTP_DWNLD_EndStateHandler()
        BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_START
        BL_TFTP_DWNLD_Comp.currAddr = BL_DWNLD_StartAddr
        BL_TFTP_DWNLD_Comp.imgSize = 0
        break
    default:
        err = BL_BCM_ERR_INVAL_STATE
        break
    return err
    @endcode
*/
static int32_t BL_TFTP_DWNLD_StateHandler(const void *const aBuf,
                                          uint32_t aSize)
{
    int32_t err = BL_BCM_ERR_OK;

    switch (BL_TFTP_DWNLD_Comp.state) {
    case BL_TFTP_DWNLD_STATE_START:
        if (NULL != aBuf) {
            err = BL_TFTP_DWNLD_StartStateHandler(aBuf, aSize);
            if (BL_BCM_ERR_OK == err) {
                BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_DWNLDNG;
            }
        }
        break;
    case BL_TFTP_DWNLD_STATE_DWNLDNG:
        if (NULL != aBuf) {
            err = BL_TFTP_DWNLD_DwnldngStateHandler(aBuf, aSize);
        }
        break;
    case BL_TFTP_DWNLD_STATE_END:
        BL_TFTP_DWNLD_EndStateHandler();
        BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_START;
        BL_TFTP_DWNLD_Comp.currAddr = BL_DWNLD_StartAddr;
        BL_TFTP_DWNLD_Comp.imgSize = 0UL;
        break;
    default:
        err = BL_BCM_ERR_INVAL_STATE;
        break;
    }

    return err;
}

/**
    @image html bl_tftp_flowchart_start_ind.jpg
*/
int32_t BL_TFTP_StartInd(BL_INET_IPAddress aRmtIP)
{
    int32_t err = BL_BCM_ERR_OK;
    uint32_t lineNo = __LINE__;

    if (BL_TFTP_DWNLD_STATE_END == BL_TFTP_DWNLD_Comp.state) {
        err = BL_BCM_ERR_BUSY;
        lineNo = __LINE__;
        BL_TFTP_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
        goto err;
    }
    BL_TFTP_DWNLD_Comp.imgSize = 0UL;
    BL_TFTP_DWNLD_Comp.currAddr = BL_DWNLD_StartAddr;
    BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_START;
    BL_TFTP_DWNLD_Comp.rmtIP = aRmtIP;

err:
    return err;
}

/**
    @code{.c}
    if aBuf = NULL
        return BL_BCM_ERR_INVAL_PARAMS
    else if 0UL = aSize
        return BL_BCM_ERR_OK
    else
        return BL_TFTP_DWNLD_StateHandler(aBuf, aSize)
    @endcode
*/
int32_t BL_TFTP_RxDataInd(const void *const aBuf,
                          uint32_t aSize)
{
    int32_t err;
    uint32_t lineNo = __LINE__;

    if (aBuf == NULL) {
        err = BL_BCM_ERR_INVAL_PARAMS;
        lineNo = __LINE__;
        BL_TFTP_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
    } else if (0UL == aSize) {
        err = BL_BCM_ERR_OK;
    } else {
        err = BL_TFTP_DWNLD_StateHandler(aBuf, aSize);
    }

    return err;
}

/**
    @code{.c}
    if TRUE = aIsErr
        BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_START
        BL_TFTP_DWNLD_Comp.currAddr = BL_DWNLD_StartAddr
        BL_TFTP_DWNLD_Comp.imgSize = 0
    else
        if BL_TFTP_DWNLD_STATE_DWNLDNG != BL_TFTP_DWNLD_Comp.state
            return
        if BL_TFTP_DWNLD_Comp.imgSize != BL_TFTP_DWNLD_Comp.imgHdr.imgSize
            return
        BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_END
    @endcode
*/
void BL_TFTP_EndInd(uint32_t aIsErr)
{
    uint32_t lineNo = __LINE__;

    if (BL_TRUE == aIsErr) {
        BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_START;
        BL_TFTP_DWNLD_Comp.currAddr = BL_DWNLD_StartAddr;
        BL_TFTP_DWNLD_Comp.imgSize = 0UL;
    } else {
        if (BL_TFTP_DWNLD_STATE_DWNLDNG != BL_TFTP_DWNLD_Comp.state) {
            lineNo = __LINE__;
            BL_TFTP_DWNLD_ReportError(BL_BCM_ERR_INVAL_STATE, 0UL, 0UL, 0UL, 0UL, lineNo);
            goto err;
        }
        if (BL_TFTP_DWNLD_Comp.imgSize != BL_TFTP_DWNLD_Comp.imgHdr.imgSize) {
            lineNo = __LINE__;
            BL_TFTP_DWNLD_ReportError(BL_BCM_ERR_INVAL_PARAMS, 0UL, 0UL, 0UL, 0UL, lineNo);
            goto err;
        }
        BL_TFTP_DWNLD_Comp.state = BL_TFTP_DWNLD_STATE_END;
    }

err:
    return;
}
#endif

static void BL_TFTP_DWNLD_ProcessINETTimer(uint32_t aTimeOut)
{
    BCM_TimeType tDiff;

    BCM_GetSystemTime(&Time1);
    if ((0UL == Time2.s) && (0UL == Time2.ns)) {
        tDiff.s = 0UL;
        tDiff.ns = 0UL;
        Time2 = Time1;
    } else {
        tDiff = BCM_GetTimeAbsDiff(Time1, Time2);
    }

    if (aTimeOut <= ((tDiff.s * 1000000UL) + (tDiff.ns / 1000UL))) {
        Time2 = Time1;
        /* Process internet stack */
        BL_INET_ProcessTimers();
    }
}

/**
    @code{.c}
    TFTP_Init()
    BL_CmdInit()

    while(1)
        ETHER_ProcessPkts()
        BL_TFTP_DWNLD_StateHandler(NULL, 0UL)
        sys_check_timeouts()// Refer Ethernet stack for this definition
    @endcode
*/
void BL_TFTP_DWNLD_Init(MCU_DwnldModeType aDwnldMode)
{
#if defined(BL_ENABLE_INET)
    int32_t err;
    uint32_t inetTimeOut;
    uint32_t lineNo = __LINE__;

    /* Initialize INET and TFTP stack */
    BL_TFTP_Init();
    BL_CmdInit();

    inetTimeOut = 1000UL; /* 10^3 micro second */
    BL_LOG_ERR("ARP resolution in progress\n");
    do {
        err = BL_INET_Init();
        if (BL_BCM_ERR_OK == err) {
            BL_LOG_ERR("ARP resolved\n");
            break;
        }
        BL_ETHER_ProcessPkts();
        BL_TFTP_DWNLD_ProcessINETTimer(inetTimeOut);
    } while (BL_BCM_ERR_EAGAIN == err);

    if (BL_BCM_ERR_OK != err) {
        lineNo = __LINE__;
        BL_TFTP_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
    } else {
        BL_LOG_ERR("Enter TFTP download mode\n");
    }

    Time1.s = 0UL;
    Time1.ns = 0UL;
    Time2.s = 0UL;
    Time2.ns = 0UL;
    inetTimeOut = 1000000UL; /* 10^6 micro second */
    /* Process TX/RX packets and timeouts
    */
    while (1UL) {
        BL_ETHER_ProcessPkts();
        BL_CmdStateHandler();
        BL_TFTP_StateHandler();
        err = BL_TFTP_DWNLD_StateHandler(NULL, 0UL);
        if (BL_BCM_ERR_OK != err) {
            lineNo = __LINE__;
            BL_TFTP_DWNLD_ReportError(err, 0UL, 0UL, 0UL, 0UL, lineNo);
        }
        BL_TFTP_DWNLD_ProcessINETTimer(inetTimeOut);
    }
#else
    BL_LOG_CRIT("INET is not set. Cannot download image\n");
    while(1);
#endif
}

/** @} */
