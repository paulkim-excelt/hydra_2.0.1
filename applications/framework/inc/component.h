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

/**
    @defgroup grp_msys_comp_ifc Media Component Interface
    @ingroup grp_framework

    @addtogroup grp_msys_comp_ifc
    @{

    @file component.h
    @brief Component interface for framework and component to interact.

    @version 0.92 Import from document
*/

#ifndef COMPONENT_H
#define COMPONENT_H

#include <msg_queue.h>
#include <media_port.h>

/**
    @name Media Component Architecture IDs
    @{
    @brief Architecture IDs for Media Components
*/
#define BRCM_SWARCH_MSYS_COMP_ID_TYPE       (0x00U) /**< @brief #MSYS_CompIDType        */
#define BRCM_SWARCH_MSYS_COMP_CMD_TYPE      (0x01U) /**< @brief #MSYS_CompCmdType       */
#define BRCM_SWARCH_MSYS_CONFIG_MACRO       (0x02U) /**< @brief #MSYS_ConfigIDType      */
#define BRCM_SWARCH_MSYS_CMD_TYPE           (0x03U) /**< @brief #MSYS_CmdType           */
#define BRCM_SWARCH_MSYS_SENDCMD_TYPE       (0x04U) /**< @brief #MSYS_SendCmdType       */
#define BRCM_SWARCH_MSYS_RECVRESP_TYPE      (0x05U) /**< @brief #MSYS_RecvRespType      */
#define BRCM_SWARCH_MSYS_GETOUTPORT_TYPE    (0x06U) /**< @brief #MSYS_GetOutPortType    */
#define BRCM_SWARCH_MSYS_GETINPORT_TYPE     (0x07U) /**< @brief #MSYS_GetInPortType     */
#define BRCM_SWARCH_MSYS_COMP_FNTBL_TYPE    (0x08U) /**< @brief #MSYS_CompFnTblType     */
#define BRCM_SWARCH_MSYS_COMP_FNTBL_PROC    (0x09U) /**< @brief #MSYS_GetCompFnTbl      */
/** @} */

/**
    @name Media System 64-bit component magic ID
    @{
    @brief 64-bit component magic ID

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint64_t MSYS_CompIDType;
#define MSYS_COMP_ID_CAM0    (0x304D41434D435242ULL) /**< Camera0 component ID: BRCMCAM0 */
#define MSYS_COMP_ID_VENC    (0x434E45564D435242ULL) /**< Video encoder component ID: BRCMVENC */
#define MSYS_COMP_ID_VDEC    (0x434544564D435242ULL) /**< Video decoder component ID: BRCMVDEC */
#define MSYS_COMP_ID_OVX0    (0x3058564F4D435242ULL) /**< Optical Flow component ID: BRCMOVX0 */
#define MSYS_COMP_ID_VDUP    (0x505544564D435242ULL) /**< Video Duplicate component ID: BRCMVDUP */
#define MSYS_COMP_ID_STCH    (0x484354534D435242ULL) /**< STCH component ID: BRCMSTCH */
#define MSYS_COMP_ID_LDC     (0x3043444C4D435242ULL) /**< LDC component ID: BRCMLDC0 */
#define MSYS_COMP_ID_AVBT    (0x544256414D435242ULL) /**< AVB Talker component ID: BRCMAVBT */
#define MSYS_COMP_ID_AVBL    (0x4C4256414D435242ULL) /**< AVB Listener component ID: BRCMAVBL */
#define MSYS_COMP_ID_AVCE    (0x454356414D435242ULL) /**< AVC Encoder component ID: BRCMAVCE */
#define MSYS_COMP_ID_AVCD    (0x444356414D435242ULL) /**< AVC Decoder component ID: BRCMAVCD */
#define MSYS_COMP_ID_AREC    (0x434552414D435242ULL) /**< Audio Record component ID: BRCMAREC */
#define MSYS_COMP_ID_APLY    (0x594C50414D435242ULL) /**< Audio Play component ID: BRCMAPLAY */
#define MSYS_COMP_ID_VGCC    (0x434347564D435242ULL) /**< Video/Graphics Composer component ID: BRCMVGCC */
#define MSYS_COMP_ID_GFXSRC  (0x535846474D435242ULL) /**< Graphics Source component ID: BRCMGFXS */
#define MSYS_COMP_ID_EX_SRC  (0x304352534D435242ULL) /**< Example source component ID: BRCMSRC0 */
#define MSYS_COMP_ID_EX_INT  (0x30544E494D435242ULL) /**< Example intermediate component ID: BRCMINT0 */
#define MSYS_COMP_ID_EX_SNK  (0x304B4E534D435242ULL) /**< Test sink component ID: BRCMSNK0 */
#define MSYS_COMP_ID_RTPS    (0x535054524D435242ULL) /**< RTP Sender component ID: BRCMRTPS */
#define MSYS_COMP_ID_RTPR    (0x525054524D435242ULL) /**< RTP Receiver component ID: BRCMRTPR */
#define MSYS_COMP_ID_PGX     (0x435847504D435242ULL) /**< Pallet graphics Source component ID: BRCMPGXC */
#define MSYS_COMP_ID_PPR     (0x435250504D435242ULL) /**< PPR component ID: BRCMPPRC */
#define MSYS_COMP_ID_SGFXIC  (0x434947534D435242ULL) /** Graphics Source component ID: BRCMSGIC */
/** @} */

/**
    @name Media component command
    @{
    @brief Media component command

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint32_t MSYS_CompCmdType;
#define MSYS_COMP_CMD_INIT    (0UL)   /**< @brief Command to Initialize component */
#define MSYS_COMP_CMD_CONFIG  (1UL)   /**< @brief Command to configure component */
#define MSYS_COMP_CMD_START   (2UL)   /**< @brief Command to start processing */
#define MSYS_COMP_CMD_STOP    (3UL)   /**< @brief Command to stop processing */
#define MSYS_COMP_CMD_DEINIT  (4UL)   /**< @brief Command to Deinitialize component */
#define MSYS_COMP_CMD_CUSTOM0 (128UL) /**< @brief Custom command 0 for component */
#define MSYS_COMP_CMD_CUSTOM1 (129UL) /**< @brief Custom command 1 for component */
/** @} */

/**
    @name Media component config ID
    @{
    @brief 16-bit Config ID (represented in 32-bit).
    Valid only if cmd is COMP_CONFIG_CMD
    Reserved: 0xFF00 - 0xFFFF

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint32_t MSYS_ConfigIDType;
#define MSYS_CONFIG_CAM_MOD       (0xCC00UL)  /**< BRCM Camera Mode Config           */
#define MSYS_CONFIG_CAM_ISP       (0xCC10UL)  /**< BRCM Camera ISP Config            */
#define MSYS_CONFIG_CAM_UCM       (0xCC20UL)  /**< BRCM Camera UCM Config            */
#define MSYS_CONFIG_CAM_CSN       (0xCC30UL)  /**< BRCM Camera CSN Config            */
#define MSYS_CONFIG_CAM_CSN_MODE(mode)  \
            (MSYS_CONFIG_CAM_CSN + (mode))  /**< BRCM Camera CSN Mode Configs      */
#define MSYS_CONFIG_CAM_CTN       (0xCC40UL)  /**< BRCM Camera CTN Config            */
#define MSYS_CONFIG_OVX0_FW       (0xAC10UL)  /**< BRCM OpenVX Firmware Config       */
#define MSYS_CONFIG_OVX0_TBL      (0xAC11UL)  /**< BRCM OpenVX Table Config          */
#define MSYS_CONFIG_OVX0_GRAPH    (0xAC12UL)  /**< BRCM OpenVX Graph Config          */
#define MSYS_CONFIG_STCH_FW       (0xCE10UL)  /**< BRCM STCH Firmware Config         */
#define MSYS_CONFIG_STCH_DATA     (0xCE20UL)  /**< BRCM STCH Data Config             */
#define MSYS_CONFIG_SGFX_DATA     (0xCE30UL)  /**< BRCM Segger Graphics Data Config  */
#define MSYS_CONFIG_LDC_MOD       (0xDC00UL)  /**< BRCM LDC Mode Config              */
#define MSYS_CONFIG_LDC_FW        (0xDC10UL)  /**< BRCM LDC Firmware Config          */
#define MSYS_CONFIG_AVCE_MOD      (0xEE00UL)  /**< BRCM AVC Encoder Mode Config      */
#define MSYS_CONFIG_AVCE_FW0      (0xEE10UL)  /**< BRCM AVC Encoder Firmware0 Config */
#define MSYS_CONFIG_VENC_MOD      (0xEE80UL)  /**< BRCM Video Encoder Mode Config    */
#define MSYS_CONFIG_VENC_FW0      (0xEE90UL)  /**< BRCM Video Encoder Firmware Config*/
#define MSYS_CONFIG_VENC_FW1      (0xEEA0UL)  /**< BRCM Video Encoder Firmware Config*/
#define MSYS_CONFIG_AVCD_MOD      (0xDE00UL)  /**< BRCM AVC Decoder Mode Config      */
#define MSYS_CONFIG_AVCD_FW0      (0xDE10UL)  /**< BRCM AVC Decoder Firmware0 Config */
#define MSYS_CONFIG_VDEC_MOD      (0xDE80UL)  /**< BRCM Video Decoder Mode Config    */
#define MSYS_CONFIG_VDEC_MP_8BIT  (0xDE90UL)  /**< BRCM Video Decoder MP 8 bit binary*/
#define MSYS_CONFIG_VDEC_MP_NBIT  (0xDE91UL)  /**< BRCM Video Decoder MP N bit binary*/
#define MSYS_CONFIG_VDEC_MP_COPY  (0xDE92UL)  /**< BRCM Video Decoder MP copy binary */
#define MSYS_CONFIG_VDEC_SP_CAVLC (0xDEA0UL)  /**< BRCM Video Decoder SP cavlc binary*/
#define MSYS_CONFIG_VDEC_SP_CABAC (0xDEA1UL)  /**< BRCM Video Decoder SP cabac binary*/
#define MSYS_CONFIG_AREC          (0xAB00UL)  /**< BRCM Audio Record Config          */
#define MSYS_CONFIG_APLY          (0xAB10UL)  /**< BRCM Audio Play Config            */
#define MSYS_CONFIG_AVB_TLKR      (0xAA00UL)  /**< BRCM AVB Talker Config            */
#define MSYS_CONFIG_AVB_LSTR      (0xAA10UL)  /**< BRCM AVB Listener Config          */
#define MSYS_CONFIG_DISP          (0xDD00UL)  /**< BRCM Display component Config     */
#define MSYS_CONFIG_VGCC          (0xDD10UL)  /**< BRCM Video/Graphics Compose Config*/
#define MSYS_CONFIG_RTP_SENDER    (0xAA20UL)  /**< BRCM RTP Sender Config            */
#define MSYS_CONFIG_RTP_RCVR      (0xAA30UL)  /**< BRCM RTP Receiver Config          */
#define MSYS_CONFIG_MPEGTS_LIB    (0xAA40UL)  /**< BRCM MPEGTS library Config        */
#define MSYS_CONFIG_PPR_MOD       (0xDC00UL)  /**< BRCM PPR Mode Config              */
#define MSYS_CONFIG_PPR_FW        (0xDC10UL)  /**< BRCM PPR Firmware Config          */
#define MSYS_CONFIG_PPR_MATRIX    (0xDC20UL)  /**< BRCM PPR Matrix Config            */
/** @} */

/**
    @brief Command structure (passed along with eConfig command)

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
typedef struct _MSYS_CmdType{
    MSYS_CompIDType compID; /**< @brief 64-bit component magic ID   */
    uint32_t version;       /**< @brief Version number              */
    MSYS_CompCmdType cmd;   /**< @brief Command to the component    */
    int32_t response;       /**< @brief Response from component upon
                                completion of command processing    */
    MSYS_ConfigIDType configID; /**< @brief 16-bit Config ID
                                    (represented in 32-bit)         */
    uint32_t size;          /**< @brief Valid size of CfgData.
                                Valid only if cmd is COMP_CONFIG_CMD*/
    const uint8_t *cfgData; /**< @brief  Valid only if cmd is
                                COMP_CONFIG_CMD                     */
    uint32_t customData[16];/**< @brief Data for custom commands.   */
} MSYS_CmdType;

/** @brief Post a command to the command queue of the component

    Post a command to the command queue of the component

    @behavior Async, Non-reentrant

    @pre None

    @param[in]   aCmd    Pointer to Cmd structure
    @param[in]   aMask   Mask to signal client task
    @param[out]  aHdr    Handle to get response, valid only when return value
                        is ERR_OK

    Return values are documented in reverse-chronological order
    @return      ERR_INVAL_PARAMS    aCmd is NULL
    @return      ERR_INVAL_PARAMS    aCmd has invalid magic
    @return      ERR_INVAL_PARAMS    aMask is zero
    @return      ERR_INVAL_PARAMS    aHdr is NULL
    @return      ERR_INVAL_PARAMS    compID is invalid or does not belong to
                                    the component
    @return      ERR_INVAL_PARAMS    compID is invalid or does not belong to
                                    the componenti (if it is configure
                                    command)
    @return      ERR_INVAL_PARAMS    Image pointer is NULL (if it is
                                    Configure command)
    @return      as returned by MSGQ_SendMsg in MessageQueue design document

    @post None

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE

    @limitations None
*/
typedef int32_t (*MSYS_SendCmdType)(const MSYS_CmdType *const aCmd,
                                    const uint32_t aMask,
                                    const MSGQ_MsgHdrType** const aHdr);

/** @brief Receive response for the command posted previously

    Receive response to the command posted previously to the command queue of
    the component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aHdr    Handle returned by SendCmd API
    @param[out]  aCmd    Response from the component for posted command
                        (command corresponding to handle in hdr)

    @return  #BCM_ERR_OK        Previously posted command is completed
                                by the component and a response is
                                copied to aCmd
    @return  #BCM_ERR_INVAL_PARAMS    aHdr is NULL
    @return  #BCM_ERR_INVAL_PARAMS    aHdr has invalid magic ID
    @return  #BCM_ERR_INVAL_PARAMS    aCmd is NULL
    @return  as returned by MSGQ_RecvMsg in MessageQueue design document

    @post None

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE

    @limitations None
*/
typedef int32_t  (*MSYS_RecvRespType)(const MSGQ_MsgHdrType* const aHdr,
                                        MSYS_CmdType* const aCmd);

/** @brief Get the MSYS_OutPortType where the message needs to be posted to

    Get the MSYS_OutPortType where the message needs to be posted to (triggered
    when the sink component requests this component to fill the media buffer)

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aIndex          Port Index

    @return      MSYS_OutPortType*   Pointer to MSYS_OutPortType of the component

    @post None

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE

    @limitations None
*/
typedef const MSYS_OutPortType* (*MSYS_GetOutPortType)(const uint32_t aIndex);

/** @brief Get the MSYS_InPortType

    Get the MSYS_InPortType where the message needs to be posted to (triggered
    when the sink component requests this component to fill the media buffer)

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aIndex          Port Index

    @return      MSYS_InPortType*   Pointer to MSYS_InPortType of the component

    @post None

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE

    @limitations None
*/
typedef const MSYS_InPortType* (*MSYS_GetInPortType)(const uint32_t aIndex);

/**
    @brief Function pointer table for each component

    Each Media component shall expose a function which returns below
    function table for media framework to interact with.

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
typedef struct _MSYS_CompFnTblType {
    MSYS_SendCmdType sendCmd;   /**< @brief Send a command              */
    MSYS_RecvRespType recvResp; /**< @brief Receive a response          */
    MSYS_GetOutPortType getOutPort;
                            /**< @brief Get the MSYS_OutPortType where
                                the message needs to be posted to       */
    MSYS_GetInPortType getInPort;
                            /**< @brief Get the MSYS_InPortType where
                                the message needs to be posted to       */
} MSYS_CompFnTblType;

/** @brief Helper function to get the component function table for
    the given component
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
const MSYS_CompFnTblType* MSYS_GetCompFnTbl(MSYS_CompIDType aCompIDName);

#endif /* COMPONENT_H */

/** @} */
