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
GROUP(libc.a libg.a libnosys.a libm.a)

/* Specify the memory areas */
MEMORY
{
    SRAM (xw)      : ORIGIN = 0x20000000, LENGTH = 2M
}

sram_start = ORIGIN(SRAM);
bootrom_img_dwnld_addr = 0x1cf40; /* bootrom download area in tftp download mode */
dwnld_hdr_size = 0x14;
hdr_rsvd_size = 0xEC;
rsa_signature_size = 0x100;
bl_hdr_size = dwnld_hdr_size + rsa_signature_size + hdr_rsvd_size; /* 512 bytes of the header */
main_stack_size = 0x2000;
text_region_start = sram_start + bootrom_img_dwnld_addr;

/* Define output sections */
SECTIONS
{
    .bss :
    {
        . = ALIGN(32);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(32);
        __bss_end__ = .;
    } > SRAM

    .stack :
    {
        __stack_start__ = .;
        . = ALIGN(8);
        . = . + main_stack_size;
        __main_stack__ = .;
        __stack_end__ = .;
    } > SRAM

    /* There are tree dummy reset handler
     * functions which simply jumps to
     * actual RESET_HANDLER in the BL
     * Each dummy reset handler is followed
     * by the 256 bytes of the padding
     *
     * During the tftp secure signature generation
     * first reset function with padding is removed
     * and replaced by the rsa key and second reset
     * handler along with dummy padding is replaced
     * by the actual signature of the BL
     */
    .text BLOCK(text_region_start):
	{
        __text_start__ = .;
        KEEP(*(.reset_func1*))
        . = . + dwnld_hdr_size + hdr_rsvd_size - 4;
        KEEP(*(.reset_func2*))
        . = . + rsa_signature_size - 4;
        KEEP(*(.reset_func3*))
        /** cortex MX exception table must be aligned to
         * power of 2 with alignement greater or equal to
         * (MAX_EXCEPTIONS * 4)
         * Assuming maximum exception as 100, vector table
         * shall be aligned to min 500 byte boundary
         */
        . = ALIGN(512);
        KEEP(*(.vector_tbl))
        *(.text*)
        *(.rodata*)
        __text_end__ = .;
    } > SRAM

    .data :
    {
        *(.data.dwnldinfo)
        . = ALIGN(32);
        __data_start__ = .;
        *(vtable)
        *(.data*)
        . = ALIGN(32);
        /* All data end */
        __data_end__ = .;
    } > SRAM

    .noload.data (NOLOAD):
    {
        . = ALIGN(32);
        __noload_data_start__ = .;
        *(.noload.data*)
        . = ALIGN(32);
        /* All data end */
        __noload_data_end__ = .;
    } > SRAM

    . = ALIGN(4);
}
