data_region_start = 0x60000000;
data_region_size = 0x80000;             /* 512 KB */
bss_region_size = 0x80000;              /* 512 KB */
data_no_load_region_start = data_region_start + data_region_size + bss_region_size;
data_no_load_region_size = 0xF00000;    /* 15360 KB */
slow_dma_region_start = data_no_load_region_start + data_no_load_region_size;
slow_dma_region_size = 0x3000000;       /* 49152 KB */

MEMORY
{
    DRAM (xw)      : ORIGIN = 0x60000000, LENGTH = 64M
}

INCLUDE chip/common/sram_dram_64m.ld
