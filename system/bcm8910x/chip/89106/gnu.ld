data_region_start = 0x60000000;
data_region_size = 0x80000;             /* 512 KB */
bss_region_size = 0x80000;              /* 512 KB */
data_no_load_region_start = data_region_start + data_region_size + bss_region_size;
data_no_load_region_size = 0x700000;    /* 7168 KB */
slow_dma_region_start = data_no_load_region_start + data_no_load_region_size;
slow_dma_region_size = 0x1800000;       /* 24576 KB */

MEMORY
{
    DRAM (xw)      : ORIGIN = 0x60000000, LENGTH = 32M
}

INCLUDE chip/common/sram_dram_32m.ld
