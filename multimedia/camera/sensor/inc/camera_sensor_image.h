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
    @addtogroup grp_csn_ifc
    @{
    @section sec_csn_image_ifc Image Interface
    @todo Add overview

    @file camera_sensor_image.h
    @brief Camera Sensor Image Interface
    This header file contains the interface structures for CSN Image
    @version 0.1 Initial Version
*/
#ifndef CAMERA_SENSOR_IMAGE_H
#define CAMERA_SENSOR_IMAGE_H

#include <isys_osil.h>

/**
    @name CSN Image interface IDs
    @{
    @brief interface IDs for CSN Image
*/
#define BRCM_SWARCH_CSN_CONFIG_MACRO       (0x00U)  /**< @brief #CSN_CONFIG         */
#define BRCM_SWARCH_CSN_MAGIC_MACRO        (0x01U)  /**< @brief #CSN_MAGIC          */
#define BRCM_SWARCH_CSN_CFG_MAX_SIZE_MACRO (0x02U)  /**< @brief #CSN_MAX_CFG_SIZE   */
#define BRCM_SWARCH_CSN_MAX_CFGS_MACRO     (0x03U)  /**< @brief #CSN_MAX_CFGS       */
#define BRCM_SWARCH_CSN_CFG_MACRO          (0x04U)  /**< @brief #CSN_CfgType        */
#define BRCM_SWARCH_CSN_CMD_MACRO          (0x05U)  /**< @brief #CSN_CmdType        */
#define BRCM_SWARCH_CSN_ENDIAN_MACRO       (0x06U)  /**< @brief #CSN_EndianType     */
#define BRCM_SWARCH_CSN_IMAGE_HDR_TYPE     (0x07U)  /**< @brief #CSN_ImageHdrType   */
#define BRCM_SWARCH_CSN_CONFIG_INFO_TYPE   (0x08U)  /**< @brief #CSN_ConfigInfoType */
#define BRCM_SWARCH_CSN_IMAGE_TYPE         (0x09U)  /**< @brief #CSN_ImageType      */
/** @} */

/**
    @brief ID for Sensor configuration

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
#define CSN_CONFIG  (5U)

/**
    @brief Magic used for sensor image ("CAMS")

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
#define CSN_MAGIC           (0x43414d53UL)

/**
    @brief Camera Sensor maximum configuration size

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
#define CSN_MAX_CFG_SIZE    (4096U)

/**
    @brief Camera Sensor maximum number of bursts in configuration

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
#define CSN_MAX_CFGS        (496U)

/**
    @name CSN_CfgType
    @{
    @brief Camera Sensor configuration type

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
typedef uint16_t CSN_CfgType;           /**< @brief Sensor configuration type */
#define CSN_CFG_COMMON        (0x0001U) /**< @brief Common configuration */
#define CSN_CFG_MODE          (0x0002U) /**< @brief Mode specific configuration */
#define CSN_CFG_CALIBRATION   (0x0003U) /**< @brief Calibration configuration */
#define CSN_CFG_DYNAMIC       (0x0004U) /**< @brief Dynamic configuration */
#define CSN_CFG_START         (0x0005U) /**< @brief Start configuration */
#define CSN_CFG_STOP          (0x0006U) /**< @brief Stop configuration */
/** @} */

/**
    @name CSN_CmdType
    @{
    @brief Camera Sensor commands

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
typedef uint16_t CSN_CmdType;           /**< @brief Type for Sensor commands    */
#define CSN_CMD_REG_WRITE     (0x0001U) /**< @brief Register write command      */
#define CSN_CMD_DELAY         (0x0002U) /**< @brief Delay command               */
#define CSN_CMD_REG_OVERWRITE (0x0003U) /**< @brief Same Register write command */
/** @} */

/**
    @name CSN_EndianType
    @{
    @brief Camera sensor endianness

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
typedef uint32_t CSN_EndianType;                 /**< @brief Type for Sensor Endianness. */
#define CSN_ENDIAN_LITTLE            (0x1UL)    /**< @brief Little endian */
#define CSN_ENDIAN_BIG               (0x2UL)    /**< @brief Big endian */
/** @} */

/**
    @brief CSN image header

    CSN image header structure

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
 */
typedef struct _CSN_ImageHdrType {
    ISYS_CommonImgHdrType commonHdr;               /**< @brief Common header */
    uint32_t slaveAddr;                            /**< @brief Slave address of the sensor */
    uint32_t i2cPort;                              /**< @brief IIC Port number */
    uint32_t i2cFrequency;                         /**< @brief IIC frequency */
    uint32_t sensorFrequency;                      /**< @brief Sensor frequency */
    uint32_t is_10bitSlave;                        /**< @brief Is sensor a 10 bit slave? */
    uint32_t is_RestartRequired;                   /**< @brief Does sensor require restart sequence */
    uint32_t addrWidth;                            /**< @brief Sensor address-width*/
    uint32_t dataWidth;                            /**< @brief Sensor data-width */
    CSN_EndianType endianness;                     /**< @brief Sensor Endianness */
    uint32_t fpsLow;                               /**< @brief Lower limit for Frame per seconds as per sensor configuration */
    uint32_t fpsHigh;                              /**< @brief Upper limit for Frame per seconds as per sensor configuration */
    uint32_t lineLength;                           /**< @brief Line length in nanoseconds */
    uint32_t integrationMin;                       /**< @brief Minimum lines for integration  */
    uint32_t coarseIntegrationOffset;              /**< @brief Fine integration offset in percent of full line */
    uint32_t minFrameIntegrationDiff;              /**< @brief Minimum lines difference between frame length and integration length */
    uint32_t bpp;                                  /**< @brief Bits per pixel for sensor output */
    uint32_t isHDR;                                /**< @brief Dual stream from sensor */
    uint32_t numConfigs;                           /**< @brief Number of sub-configuration */
    uint16_t metaOffset[ISYS_SENSOR_EXP_IDX_MAX];  /**< @brief Metadata offset for all exposures */
    uint16_t metaSize[ISYS_SENSOR_EXP_IDX_MAX];    /**< @brief Metadata size for all exposures */
    uint16_t statsOffset[ISYS_SENSOR_EXP_IDX_MAX]; /**< @brief Statistics offset for all exposures */
    uint16_t statsSize[ISYS_SENSOR_EXP_IDX_MAX];   /**< @brief Statistics size for all exposures */
    uint32_t imgWidth;                             /**< @brief Image width */
} CSN_ImageHdrType;

/**
    @brief CSN image header

    CSN image header structure

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
 */
typedef struct _CSN_ConfigInfoType {
    CSN_CfgType cfgType; /**<  Config type */
    CSN_CmdType cmdType; /**<  Command type */
    uint16_t dataOffset;        /**<  Data offset in config data */
    uint16_t length;            /**<  Length of data in config data */
} CSN_ConfigInfoType;

/**
    @brief CSN Image structure

    CSN Image structure. This structure is generated by offline utility.

    @trace #BRCM_SWREQ_CSN_CONFIGURATION
 */
typedef struct _CSN_ImageType {
    CSN_ImageHdrType imgHdr;                            /**< @brief Sensor image header */
    CSN_ConfigInfoType cfgs[CSN_MAX_CFGS];      /**< @brief Sensor configuration info */
    uint8_t cameraSensorCfgData[CSN_MAX_CFG_SIZE]; /**< @brief Sensor configuration data */
} CSN_ImageType;

#endif /* CAMERA_SENSOR_IMAGE_H */

/** @} */
