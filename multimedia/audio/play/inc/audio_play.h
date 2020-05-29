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

#ifndef AUDIO_PLAY_H
#define AUDIO_PLAY_H

#include <msg_queue.h>
#include <component.h>
#include <media_port.h>

/* Audio play component API IDs for report error */
/**< AudioPlay_SendCmd API ID */
#define AUDIO_PLAY_SEND_CMD_API_ID                   (0x00U)
/**< AudioPlay_RecvResp API ID */
#define AUDIO_PLAY_RECV_RESP_API_ID                  (0x01U)
/**< AudioPlay_GetNumMediaOutPort API ID */
#define AUDIO_PLAY_GET_NUM_MEDIA_OUTPORT_API_ID      (0x02U)
/**< AudioPlay_GetMediaOutPort API ID */
#define AUDIO_PLAY_GET_MEDIA_OUTPORT_API_ID          (0x03U)
/**< AudioPlay_GetMediaOutPortConfig API ID */
#define AUDIO_PLAY_GET_MEDIA_OUTPORT_CONFIG_API_ID   (0x04U)
/**< AudioPlay_PrintStatistics API ID */
#define AUDIO_PLAY_PRINT_STATISTICS_API_ID           (0x05U)

/** Audio play component port0 buffer count */
#define AUDIO_PLAY_PORT0_BUF_CNT        (8UL)
/** Audio play component port0 buffer size */
#define AUDIO_PLAY_PORT0_BUF_SIZE       (4320UL)
/** Audio play component port1 buffer count */
#define AUDIO_PLAY_PORT1_BUF_CNT        (8UL)
/** Audio play component port1 buffer size */
#define AUDIO_PLAY_PORT1_BUF_SIZE       (4320UL)
/** Audio play component port2 buffer count  */
#define AUDIO_PLAY_PORT2_BUF_CNT        (8UL)
/** Audio play component port2 buffer size */
#define AUDIO_PLAY_PORT2_BUF_SIZE       (4320UL)

/** @brief Post a command to audio play component
 *
 *  @details     Post a command to the command queue of the component
 *
 *  @param[in]   aCmd    Pointer to Cmd structure
 *  @param[in]   aMask   Mask to signal client task
 *  @param[out]  aHdr    Handle to get response, valid only when return value
 *                       is BCM_ERR_OK
 *
 *  @retval      BCM_ERR_INVAL_PARAMS   aCmd is NULL
 *  @retval      BCM_ERR_INVAL_PARAMS   Invalid magic ID of aCmd
 *  @retval      BCM_ERR_INVAL_PARAMS   aMask is zero
 *  @retval      BCM_ERR_INVAL_PARAMS   aHdr is NULL
 *  @retval      others as retured by MSGQ_SendMsg()
 */
int32_t AudioPlay_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr);

/** @brief Receive response for the command posted previously
 *
 *  @details     Receive response to the command posted previously to the
 *               command queue of the component
 *  @param[in]   aHdr    Handle returned by SendCmd API
 *  @param[out]  aCmd    Response from the component for posted command
 *                       (command corresponding to handle in hdr)
 *
 *  @retval      BCM_ERR_INVAL_PARAMS   aCmd is NULL
 *  @retval      BCM_ERR_INVAL_PARAMS   aHdr is NULL
 *  @retval      others as retured by MSGQ_RecvMsg()
 */
int32_t AudioPlay_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd);

/**
 *  @brief Get the num of outports of avcd component
 *
 *  @details    Get the number of media outports of avcd component
 *
 *  @return     Number of media outports of avcd component
 */
uint32_t AudioPlay_GetNumMediaOutPort(void);

/** @brief Get the MSYS_OutPortType where the message needs to be posted to
 *
 *  @details     Get the MSYS_OutPortType where the message needs to be posted to
 *  (triggered when the sink component requests avcd component to fill the
 *  media buffer)
 *
 *  @param[in]   aIndex          Port Index
 *
 *  @retval      NULL aIndex is not valid
 *  @retval      pointer to the media outport
 */
const MSYS_OutPortType* AudioPlay_GetMediaOutPort(const uint32_t aIndex);

#endif /* AUDIO_PLAY_H */
