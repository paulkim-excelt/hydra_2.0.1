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

GROUP(libc.a libg.a libnosys.a libm.a)

/* Specify the memory areas */
MEMORY
{
    SRAM (xw)      : ORIGIN = 0x20000000, LENGTH = 2M
    ISYS_SRAM (xw) : ORIGIN = 0x20200000, LENGTH = 384K
}

INCLUDE app_gnu.ld
/* NOTE: Any changes made here must be reflected in the MPU table as well */
main_stack_size = DEFINED(main_stack_size) ? main_stack_size : 0x800;
init_thread_stack_size = 0x100;
fast_dma_region_start = 0x20000000;
fast_dma_region_size = 0x160000;      /* 1408 KB */
code_ro_region_start = fast_dma_region_start + fast_dma_region_size;
code_ro_region_size = 0x40000;  /* 256 KB */
privileged_region_start = code_ro_region_start + code_ro_region_size;
privileged_region_size = 0x10000;     /* 64 KB */
data_region_start = privileged_region_start + privileged_region_size;
data_region_size = 0x50000;     /* 320 KB */
isys_ram_size = 0x60000; /* 384 KB */

cam_data_size       = 0x1000;  /*  4 KB */
comms_data_size     = 0x1000;  /*  4 KB */
ldc_data_size       = 0x1000;  /*  4 KB */
nvm_data_size       = 0x1000;  /*  4 KB */
display_data_size   = 0x1000;  /*  4 KB */
avce_data_size      = 0x1000;  /*  4 KB */
graphics_data_size  = 0x1000;  /*  4 KB */
sys_data_size       = 0x1000;  /*  4 KB */

cam_bss_size       = 0x10000; /* 64 KB */
comms_bss_size     = 0x10000; /* 64 KB */
ldc_bss_size       = 0x8000;  /* 32 KB */
nvm_bss_size       = 0x2000;  /*  8 KB */
display_bss_size   = 0x2000;  /*  8 KB */
avce_bss_size      = 0x1000;  /*  4 KB */
graphics_bss_size  = 0x1000;  /*  4 KB */
sys_bss_size       = 0x1000;  /*  4 KB */

/* Define output sections */
SECTIONS
{
    /* During bootup this region is used for BootROM bss and partion table
     * loading. Once booted, erika shall use this region for DMA */
    .fast_dma (NOLOAD):
    {
        __fast_dma_start__ = .;
        *(.cam.yuv.uncached*)
        *(.pp.uncached*)
        *(.pp.slowmem*)
        *(.dma.fw*)
        *(.iis.play*)
        *(.iis.rec*)
        *(.vgcc.uncached*)
        *(.vgc.*)
        *(.dma.bcm.test*)
        __ethernet_tx_region_start__ = .;
        *(.ethernet.tx0desctbl)
        *(.ethernet.tx0pktbuff)
        *(.ethernet.tx1desctbl)
        *(.ethernet.tx1pktbuff)
        *(.ethernet.tx2desctbl)
        *(.ethernet.tx2pktbuff)
        *(.ethernet.tx3desctbl)
        *(.ethernet.tx3pktbuff)
        __ethernet_tx_region_end__ = .;
        __fast_dma_end__ = .;
        size_check = (__fast_dma_end__ <= fast_dma_region_size);
        ASSERT(size_check, "***LINKER ERROR: DMA Section overflows the limit.");
    } > SRAM

    .text BLOCK(code_ro_region_start):
    {
        __text_start__ = .;
        *(.ethernet.rx0desctbl)
        . = ALIGN(1024);
        KEEP(*(.vector_tbl))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
          __version_data_start__ = .;
          KEEP(*(.version*))
          __version_data_end__ = .;
        __text_end__ = .;
        size_check = (__text_end__  <= code_ro_region_size);
        ASSERT(size_check, "***LINKER ERROR: Code(RO) Section overflows the limit.");
    } > SRAM
}

SECTIONS
{
    .data.privileged BLOCK(privileged_region_start):
    {
        priv_data_start = .;
        __stack_start__ = .;
        . = ALIGN(8);
        . = . + main_stack_size;
        __main_stack__ = .;
        . = ALIGN(2048);
        *(.stack.2k.*)
        *(.stack.1k.*)
        *(.stack.512.*)
        *(.stack.256.*)
        . = . + init_thread_stack_size;
        __init_thread_stack__ = .;
        . = ALIGN(16384);
        *(.stack.16k.*)
        *(.stack.8k.*)
        *(.stack.4k.*)
        __stack_end__ = .;
        __data_os_start__ = .;
        *(.data.os*)
        __data_os_end__ = .;
        __data_drivers_start__ = .;
        *(.data.drivers*)
        *(.pp.mbox*)
        __data_drivers_end__ = .;

        . = ALIGN(sys_data_size);
        __sys_data_start__ = .;
        *(.data.sys.*);
        __sys_data_end__ = .;
        . = ALIGN(sys_data_size >> 3);
        sys_data_size_check = (__sys_data_end__ - __sys_data_start__ <= sys_data_size) ;
        ASSERT(sys_data_size_check , "***LINKER ERROR: SYS Data Section overflows the limit.");

        . = ALIGN(sys_bss_size);
        __sys_bss_start__ = .;
        *(.bss.sys.*);
        __sys_bss_end__ = .;
        . = ALIGN(sys_bss_size >> 3);
        sys_bss_size_check = (__sys_bss_end__ - __sys_bss_start__ <= sys_bss_size) ;
        ASSERT(sys_bss_size_check , "***LINKER ERROR: SYS BSS Section overflows the limit.");

        priv_data_end = .;
        size_check = (priv_data_end  - priv_data_start <= privileged_region_size);
        ASSERT(size_check, "***LINKER ERROR: .data.privileged section overflows the limit.");
    } > SRAM
}

SECTIONS
{
    .data BLOCK(data_region_start):
    {
        __data_start__ = .;

        __cam_data_start__ = .;
        *(.data.cam.*)
        __cam_data_end__ = .;
        cam_size_check = (__cam_data_end__ - __cam_data_start__ <= cam_data_size) ;
        ASSERT(cam_size_check , "***LINKER ERROR: CAM Data Section overflows the limit.");

        __comms_data_start__ = .;
        *(.data.comms.*)
        __comms_data_end__ = .;
        comms_size_check = (__comms_data_end__ - __comms_data_start__ <= comms_data_size) ;
        ASSERT(comms_size_check , "***LINKER ERROR: Comms Data Section overflows the limit.");

        __ldc_data_start__ = .;
        *(.data.ldc.*)
        __ldc_data_end__ = .;
        ldc_size_check = (__ldc_data_end__ - __ldc_data_start__ <= ldc_data_size) ;
        ASSERT(ldc_size_check , "***LINKER ERROR: LDC Data Section overflows the limit.");

        __nvm_data_start__ = .;
        *(.data.nvm.*)
        __nvm_data_end__ = .;
        nvm_size_check = (__nvm_data_end__ - __nvm_data_start__ <= nvm_data_size) ;
        ASSERT(nvm_size_check , "***LINKER ERROR: NVM Data Section overflows the limit.");

        __display_data_start__ = .;
        *(.data.display.*)
        __display_data_end__ = .;
        display_size_check = (__display_data_end__ - __display_data_start__ <= display_data_size) ;
        ASSERT(display_size_check , "***LINKER ERROR: Display Data Section overflows the limit.");

        __avce_data_start__ = .;
        *(.data.avce.*)
        __avce_data_end__ = .;
        avce_size_check = (__avce_data_end__ - __avce_data_start__ <= avce_data_size) ;
        ASSERT(avce_size_check , "***LINKER ERROR: AVCE Data Section overflows the limit.");

        __graphics_data_start__ = .;
        *(.data.graphics.*)
        __graphics_data_end__ = .;
        graphics_size_check = (__graphics_data_end__ - __graphics_data_start__ <= graphics_data_size) ;
        ASSERT(graphics_size_check , "***LINKER ERROR: Graphics Data Section overflows the limit.");

        __public_data_start__ = .;
        *(.data.console.*)
        __public_data_end__ = .;

        *(.data*)
        __data_end__ = .;
        __test_data_start__ = .;
        *(.bcm.test.*)
        __test_data_end__ = .;
        size_check = (__test_data_end__ <= data_region_size) ;
        ASSERT(size_check , "***LINKER ERROR: Data Section overflows the limit.");
    } > SRAM

    .bss (NOLOAD):
    {
        . = ALIGN(4);
        __bss_start__ = .;

        __cam_bss_start__ = .;
        *(.bss.cam.*)
        __cam_bss_end__ = .;
        cam_bss_size_check = (__cam_bss_end__ - __cam_bss_start__ <= cam_bss_size) ;
        ASSERT(cam_bss_size_check , "***LINKER ERROR: CAM BSS Section overflows the limit.");

        __comms_bss_start__ = .;
        *(.bss.comms.*)
        __comms_bss_end__ = .;
        comms_bss_size_check = (__comms_bss_end__ - __comms_bss_start__ <= comms_bss_size) ;
        ASSERT(comms_bss_size_check, "***LINKER ERROR: Comms bss Section overflows the limit.");

        __ldc_bss_start__ = .;
        *(.bss.ldc.*)
        __ldc_bss_end__ = .;
        ldc_bss_size_check = (__ldc_bss_end__ - __ldc_bss_start__ <= ldc_bss_size) ;
        ASSERT(ldc_bss_size_check, "***LINKER ERROR: LDC bss Section overflows the limit.");

        __nvm_bss_start__ = .;
        *(.bss.nvm.*)
        __nvm_bss_end__ = .;
        nvm_bss_size_check = (__nvm_bss_end__ - __nvm_bss_start__ <= nvm_bss_size) ;
        ASSERT(nvm_bss_size_check, "***LINKER ERROR: NVM bss Section overflows the limit.");

        __display_bss_start__ = .;
        *(.bss.display.*)
        __display_bss_end__ = .;
        display_bss_size_check = (__display_bss_end__ - __display_bss_start__ <= display_bss_size) ;
        ASSERT(display_bss_size_check, "***LINKER ERROR: Display bss Section overflows the limit.");

        __avce_bss_start__ = .;
        *(.bss.avce.*)
        __avce_bss_end__ = .;
        avce_bss_size_check = (__avce_bss_end__ - __avce_bss_start__ <= avce_bss_size) ;
        ASSERT(avce_bss_size_check, "***LINKER ERROR: AVCE bss Section overflows the limit.");

        __graphics_bss_start__ = .;
        *(.bss.graphics.*)
        __graphics_bss_end__ = .;
        graphics_bss_size_check = (__graphics_bss_end__ - __graphics_bss_start__ <= graphics_bss_size) ;
        ASSERT(graphics_bss_size_check , "***LINKER ERROR: Graphics bss Section overflows the limit.");

        __public_bss_start__ = .;
        *(.bss.public.*)
        __public_bss_end__ = .;

        *(.bss*)
        *(.pp.cached.*)
        *(.noload.data.*)
        __ethernet_rx_region_start__ = .;
        *(.ethernet.rx0pktbuff)
        __ethernet_rx_region_end__ = .;
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > SRAM
}

/* Define ISYS SRAM sections */
SECTIONS
{
    .isys_memory (NOLOAD):
    {
        __isys_ram_start__ = .;
        *(.isys.tcb*)
        *(.isys.desc*)
        *(.isys.stats*)
        *(.isys.data.input*)
        *(.isys.image.input*)
        *(.isys.tuner.data*)
    } > ISYS_SRAM
}
