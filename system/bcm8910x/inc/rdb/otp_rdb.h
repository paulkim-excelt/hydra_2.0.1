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
    @file otp_rdb.h
    @brief RDB File for OTP

    @version 2018May25_rdb
*/

#ifndef OTP_RDB_H
#define OTP_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t OTP_CONTROL_TYPE;
#define OTP_CONTROL_CPU_MODE_MASK (0x80000000UL)
#define OTP_CONTROL_CPU_MODE_SHIFT (31UL)
#define OTP_CONTROL_BRCM_OTP_DIS_MASK (0x40000000UL)
#define OTP_CONTROL_BRCM_OTP_DIS_SHIFT (30UL)
#define OTP_CONTROL_CUST_OTP_DIS_MASK (0x20000000UL)
#define OTP_CONTROL_CUST_OTP_DIS_SHIFT (29UL)
#define OTP_CONTROL_CPU_ADDRESS_MASK (0xff80UL)
#define OTP_CONTROL_CPU_ADDRESS_SHIFT (7UL)
#define OTP_CONTROL_CPU_COMMAND_MASK (0x7cUL)
#define OTP_CONTROL_CPU_COMMAND_SHIFT (2UL)
#define OTP_CONTROL_CPU_WRITE_ENABLE_MASK (0x2UL)
#define OTP_CONTROL_CPU_WRITE_ENABLE_SHIFT (1UL)
#define OTP_CONTROL_CPU_PROG_EN_MASK (0x1UL)
#define OTP_CONTROL_CPU_PROG_EN_SHIFT (0UL)




typedef uint32_t OTP_STATUS_TYPE;
#define OTP_STATUS_ECC_DED_FLAG_MASK (0x800000UL)
#define OTP_STATUS_ECC_DED_FLAG_SHIFT (23UL)
#define OTP_STATUS_ECC_SEC_FLAG_MASK (0x400000UL)
#define OTP_STATUS_ECC_SEC_FLAG_SHIFT (22UL)
#define OTP_STATUS_PRESCREEN_FAIL_MASK (0x100000UL)
#define OTP_STATUS_PRESCREEN_FAIL_SHIFT (20UL)
#define OTP_STATUS_PROG_EN_MASK (0x2000UL)
#define OTP_STATUS_PROG_EN_SHIFT (13UL)
#define OTP_STATUS_PROG_BLOCK_CMD_MASK (0x1000UL)
#define OTP_STATUS_PROG_BLOCK_CMD_SHIFT (12UL)
#define OTP_STATUS_PROG_SCREEN_FAIL_MASK (0x800UL)
#define OTP_STATUS_PROG_SCREEN_FAIL_SHIFT (11UL)
#define OTP_STATUS_PROG_WORD_FAIL_MASK (0x400UL)
#define OTP_STATUS_PROG_WORD_FAIL_SHIFT (10UL)
#define OTP_STATUS_INVALID_ADDR_MASK (0x200UL)
#define OTP_STATUS_INVALID_ADDR_SHIFT (9UL)
#define OTP_STATUS_DEBUG_ENABLE_MASK (0x100UL)
#define OTP_STATUS_DEBUG_ENABLE_SHIFT (8UL)
#define OTP_STATUS_MST_FSM_ERROR_MASK (0x80UL)
#define OTP_STATUS_MST_FSM_ERROR_SHIFT (7UL)
#define OTP_STATUS_DEBUG_MODE_SET_MASK (0x40UL)
#define OTP_STATUS_DEBUG_MODE_SET_SHIFT (6UL)
#define OTP_STATUS_REFOK_MASK (0x20UL)
#define OTP_STATUS_REFOK_SHIFT (5UL)
#define OTP_STATUS_CMD_FAIL_MASK (0x10UL)
#define OTP_STATUS_CMD_FAIL_SHIFT (4UL)
#define OTP_STATUS_FDONE_MASK (0x8UL)
#define OTP_STATUS_FDONE_SHIFT (3UL)
#define OTP_STATUS_PROGOK_MASK (0x4UL)
#define OTP_STATUS_PROGOK_SHIFT (2UL)
#define OTP_STATUS_CMD_DONE_MASK (0x2UL)
#define OTP_STATUS_CMD_DONE_SHIFT (1UL)
#define OTP_STATUS_DATA_VALID_MASK (0x1UL)
#define OTP_STATUS_DATA_VALID_SHIFT (0UL)




typedef uint32_t OTP_WRITE_DATA_TYPE;
#define OTP_WRITE_DATA_CPU_DATA_MASK (0xffffffffUL)
#define OTP_WRITE_DATA_CPU_DATA_SHIFT (0UL)




typedef uint32_t OTP_READ_DATA_TYPE;
#define OTP_READ_DATA_CPU_DATA_MASK (0xffffffffUL)
#define OTP_READ_DATA_CPU_DATA_SHIFT (0UL)




typedef uint32_t OTP_CFG_STATUS_TYPE;
#define OTP_CFG_STATUS_INITVTOR_DEC_MASK (0x80000000UL)
#define OTP_CFG_STATUS_INITVTOR_DEC_SHIFT (31UL)
#define OTP_CFG_STATUS_INITVTOR_SEC_MASK (0x40000000UL)
#define OTP_CFG_STATUS_INITVTOR_SEC_SHIFT (30UL)
#define OTP_CFG_STATUS_SPI_SLV_DISABLE_MASK (0x10UL)
#define OTP_CFG_STATUS_SPI_SLV_DISABLE_SHIFT (4UL)
#define OTP_CFG_STATUS_CPU_PROG_DISABLE_MASK (0x8UL)
#define OTP_CFG_STATUS_CPU_PROG_DISABLE_SHIFT (3UL)
#define OTP_CFG_STATUS_JTAG_DISABLE_MASK (0x4UL)
#define OTP_CFG_STATUS_JTAG_DISABLE_SHIFT (2UL)
#define OTP_CFG_STATUS_DBG_ENABLE_MASK (0x2UL)
#define OTP_CFG_STATUS_DBG_ENABLE_SHIFT (1UL)
#define OTP_CFG_STATUS_INITVTOR_SEL_MASK (0x1UL)
#define OTP_CFG_STATUS_INITVTOR_SEL_SHIFT (0UL)




typedef volatile struct COMP_PACKED _OTP_RDBType {
    OTP_CONTROL_TYPE control; /* OFFSET: 0x0 */
    OTP_STATUS_TYPE status; /* OFFSET: 0x4 */
    OTP_WRITE_DATA_TYPE write_data; /* OFFSET: 0x8 */
    OTP_READ_DATA_TYPE read_data; /* OFFSET: 0xc */
    OTP_CFG_STATUS_TYPE cfg_status; /* OFFSET: 0x10 */
} OTP_RDBType;


#define OTP_BASE                        (0xE0101000UL)



#define OTP_MAX_HW_ID                   (1UL)


#define OTP_ADDR_START                  (0UL)


#define OTP_ADDR_END                    (255UL)


#define OTP_DATA_MASK_WITH_ECC          (0x3FFFFFFUL)


#define OTP_ECC_BITS                    (6UL)


#define OTP_ADDR_BRCM_REG_START         (12UL)


#define OTP_ADDR_BRCM_REG_END           (63UL)

#endif /* OTP_RDB_H */
