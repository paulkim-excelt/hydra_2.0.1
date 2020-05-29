/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

 2. TO THE RFS_MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE RFS_MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_rfs_ifc Interface
    @ingroup grp_rfs

    @addtogroup grp_rfs_ifc
    @{
    @section sec_rfs_overview Overview
    This is the interface for the remote file service. Remote file service can
    be used to access files on a network PC. This uses custom TFTP Protocol to
    read and write partial files.

    @section sec_rfs_seq Sequence Diagrams
    @tagseq rfs_seq_diagram.svg "Sequence diagram"
    @startseq
        group {label = "application"; color = "#EFFFFF";  App;};
        group {label = "RFS Server"; color = "#FFEFFF"; RFSClient; RFSServer; TFTPClient};
        group {label = "PC"; color = "#FFFFEF"; CustomTFTPServer;};
        App => RFSClient [label = "RFS_Open"];
        === File Open Complete ===
        loop {
            App => RFSClient [label = "RFS_Read"] {
                   RFSClient -> RFSServer [label = "MSGQ_SendMsg"];
                   RFSClient <-- RFSServer;
                };
                RFSServer -> CustomTFTPServer [note = "TFTP Partial File Read Start"];
                RFSServer <-- CustomTFTPServer [note = "TFTP Partial File Read Complet"];
            App <-- RFSServer [label = "Partial File Read Complete"];
            App => RFSClient [label = "RFS_GetStatus"] {
                   RFSClient -> RFSServer [label = "MSGQ_RecvMsg"];
                   RFSClient <-- RFSServer [note = "Partial File data available to App for processing"];
                };
        };
        App => RFSClient [label = "RFS_Close"];
    @endseq

    @file rfs.h
    @brief RFS interface
    This header file contains the interface functions for RFS component
    @version 0.1 Initial Version

*/

#ifndef RFS_H
#define RFS_H

#include <msg_queue.h>
/**
    @name Component Architecture IDs
    @{
    @brief Architecture IDs for Component
*/
#define BRCM_SWARCH_RFS_MAX_FILENAME_MACRO  (0x00U)  /**< @brief #RFS_MAX_FILENAME_LEN */
#define BRCM_SWARCH_RFS_STATE_TYPE          (0x01U)  /**< @brief #RFS_StateType        */
#define BRCM_SWARCH_RFS_MODE_TYPE           (0x02U)  /**< @brief #RFS_ModeType         */
#define BRCM_SWARCH_RFS_SEEK_TYPE           (0x03U)  /**< @brief #RFS_SeekType         */
#define BRCM_SWARCH_RFS_FILE_HANDLE_TYPE    (0x04U)  /**< @brief #RFS_FileHandleType   */
#define BRCM_SWARCH_RFS_OPEN_PROC           (0x05U)  /**< @brief #RFS_Open             */
#define BRCM_SWARCH_RFS_CLOSE_PROC          (0x06U)  /**< @brief #RFS_Close            */
#define BRCM_SWARCH_RFS_READ_PROC           (0x07U)  /**< @brief #RFS_Read             */
#define BRCM_SWARCH_RFS_WRITE_PROC          (0x08U)  /**< @brief #RFS_Write            */
#define BRCM_SWARCH_RFS_SEEK_PROC           (0x09U)  /**< @brief #RFS_Seek             */
#define BRCM_SWARCH_RFS_GET_STATUS_PROC     (0x0AU)  /**< @brief #RFS_GetStatus        */
/** @} */

/**
    @brief Maxmium length of the file name

    Maximum length of the file name

    @trace #BRCM_SWREQ_RFS_CLIENT
*/
#define RFS_MAX_FILENAME_LEN (96UL)

/**
    @name RFS_StateType IDs
    @{
    @brief State IDS for RFS_StateType

    Different states allowed for the RFS file handler

    @trace #BRCM_SWREQ_RFS_CLIENT
*/
typedef uint32_t RFS_StateType; /**< @brief RFS_StateType typedef */
#define RFS_STATE_RESET (0UL)   /**< @brief Reset state           */
#define RFS_STATE_READ  (1UL)   /**< @brief Read state            */
#define RFS_STATE_WRITE (2UL)   /**< @brief Write state           */
/** @} */

/**
    @name RFS Mode types
    @{
    @brief File modes allowed in RFS

    Different file modes alowed in RFS. File can be opened to read,
    write a new file or append a exisitng file

    @trace #BRCM_SWREQ_RFS_CLIENT
*/
typedef uint32_t RFS_ModeType; /**< @brief RFS_ModeType typedef        */
#define RFS_MODE_READ    (0UL) /**< @brief File to be opened to read   */
#define RFS_MODE_WRITE   (1UL) /**< @brief File to be opened to write  */
#define RFS_MODE_APPEND  (2UL) /**< @brief File to be opened to append */
/** @} */

/**
    @name RFS Seek type
    @{
    @brief RFS seek type

    Different seek types allowed in RFS.

    @trace #BRCM_SWREQ_RFS_CLIENT
*/
typedef uint32_t RFS_SeekType; /**< @brief RFS_State typedef */
#define RFS_SEEK_SET (0UL)     /**< @brief Set the offset absolutely                   */
#define RFS_SEEK_CUR (1UL)     /**< @brief Set the offset relatively to current offset */
#define RFS_SEEK_END (2UL)     /**< @brief Set the offset to end of file               */
/** @} */


/**
    @brief RFS file handler structure

    Stores all the context of the file

    @trace #BRCM_SWREQ_RFS_CLIENT
 */
typedef struct _RFS_FileHandleType {
    RFS_StateType  state;                   /**< @brief state as #RFS_StateType */
    uint8_t fileName[RFS_MAX_FILENAME_LEN]; /**< @brief File name               */
    uint32_t offset;                        /**< @brief Current offset          */
    const MSGQ_MsgHdrType *hdr;             /**< @brief Handle of the request   */
} RFS_FileHandleType;

/** @brief Open file

    Open the file with the given options

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aHandle     Pointer to file handler to hold the context
    @param[in]      aFileName   Name of the file to be opened. This is assumed to be NULL
                                terminated and has size less than #RFS_MAX_FILENAME_LEN.
    @param[in]      aMode       Mode of the file operation as specified in #RFS_ModeType
    Return values are documented in reverse-chronological order
    @retval     BCM_ERR_OK           Successfully opened the file
    @retval     BCM_ERR_INVAL_PARAMS (aHandle is NULL) or
                                     (aFileName is NULL) or
                                     (aFileName size is #RFS_MAX_FILENAME_LEN or more)

    @post The file handler needs to be passed for further operations
          on the same file

    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations aHandle is assumed to be empty(Not pointing to any file)
*/
int32_t RFS_Open(RFS_FileHandleType *const aHandle,
                 const uint8_t *const aFileName,
                 const RFS_ModeType aMode);

/** @brief Close file

    Close the file

    @behavior Sync, Re-entrant

    @pre aHandle is assumed to be pointing to a file

    @param[inout]   aHandle       Pointer to the file handler

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Successfully closed the file
    @retval     #BCM_ERR_BUSY          Previous request is in process. Try again.
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle->state is #RFS_STATE_RESET)
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle is NULL)

    @post None

    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None
*/
int32_t RFS_Close(RFS_FileHandleType *const aHandle);

/** @brief Read data from file

    Read the data of given length from the current offset

    @behavior Sync, Re-entrant

    @pre aHandle is assumed to be pointing to a file opened in #RFS_Read mode

    @param[inout]   aHandle    Pointer to the file handler
    @param[in]      aBuffer    Buffer to hold the data to be read
    @param[in]      aLen       Length of the buffer provided
    @param[in]      aClientMask Client event mask

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Request to read data is accepted and
                                       shall be processed asynchronously. Upon
                                       completion of the request, client shall
                                       be signaled using @c aClientMask
    @retval     #BCM_ERR_UNKNOWN       Integration fault
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle->state is not #RFS_STATE_READ) or
                                       (aHandle->hdr is not NULL)
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle is NULL) or
                                       (aBuffer is NULL) or
                                       (aLen is 0)

    @post Use #RFS_GetStatus to know the status of the request

    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None
*/
int32_t RFS_Read(RFS_FileHandleType *const aHandle,
                 uint8_t *const aBuffer,
                 const uint32_t aLen,
                 const uint32_t aClientMask);

/** @brief Write data from file

    Write the data of given length from the current offset

    @behavior Sync, Re-entrant

    @pre aHandle is assumed to be pointing to a file opened in #RFS_Write or #RFS_Append mode

    @param[inout]   aHandle    Pointer to the file handler
    @param[inout]   aBuffer    Buffer to data to be written
    @param[in]      aLen       Length of the buffer provided
    @param[in]      aClientMask Client event mask

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Request to write data is accepted and
                                       shall be processed asynchronously. Upon
                                       completion of the request, client shall
                                       be signaled using @c aClientMask
    @retval     #BCM_ERR_UNKNOWN       Integration fault
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle->state is not #RFS_STATE_WRITE) or
                                       (aHandle->hdr is not NULL)
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle is NULL) or
                                       (aBuffer is NULL) or
                                       (aLen is 0)

    @post Use #RFS_GetStatus to know the status of the request

    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None
*/
int32_t RFS_Write(RFS_FileHandleType *const aHandle,
                  const uint8_t *const aBuffer,
                  const uint32_t aLen,
                  const uint32_t aClientMask);

/** @brief Seek to the required offset

    To change the current offset of the file to requested offset.

    @behavior Sync, Re-entrant

    @pre aHandle is assumed to be pointing to a file opened

    @param[inout]   aHandle    Pointer to the file handler
    @param[in]      aSeekType  Position from where offset is added as defined by #RFS_SeekType
    @param[in]      aLen       Number of bytes to offset from position

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Successful.
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle->state is #RFS_STATE_RESET) or
                                       (#RFS_SeekType is not valid)
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle is NULL)

    @post None

    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None
*/
int32_t RFS_Seek(RFS_FileHandleType *const aHandle,
                 const RFS_SeekType aSeekType,
                 const uint32_t aLen);

/** @brief Get the status of the current request

    To know the status of the current request

    @behavior Sync, Re-entrant

    @pre aHandle is assumed to be pointing to a file opened

    @param[inout]   aHandle    Pointer to the file handler
    @param[out]     aLen       Pointer to store the number of bytes accessed.
                               Length of the buffer used in read.
                               Length of the data written in write.
                               This would be zero incase of an error.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Requested completed.
    @retval     #BCM_ERR_NOMEM         Insufficient buffer to store remote server data
    @retval     #BCM_ERR_NOT_FOUND     File not found
    @retval     #BCM_ERR_TIME_OUT      Request time out.
    @retval     #BCM_ERR_UNKNOWN       Unknown error.
    @retval     #BCM_ERR_BUSY          Request is still in progress.
    @retval     #BCM_ERR_INVAL_PARAMS  (aHandle->state is #RFS_STATE_RESET)
    @retval     #BCM_ERR_INVAL_PARAMS  (aLen is NULL)

    @post None

    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None
*/
int32_t RFS_GetStatus(RFS_FileHandleType *const aHandle,
                      uint32_t *const aLen);
#endif /* RFS_H */

/** @} */
