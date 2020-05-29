/************************************************************************************************/
/*                                                                                              */
/*  Copyright 2016  Broadcom Corporation                                                        */
/*                                                                                              */
/*     Unless you and Broadcom execute a separate written software license agreement governing  */
/*     use of this software, this software is licensed to you under the terms of the GNU        */
/*     General Public License version 2 (the GPL), available at                                 */
/*                                                                                              */
/*          http://www.broadcom.com/licenses/GPLv2.php                                          */
/*                                                                                              */
/*     with the following added to such license:                                                */
/*                                                                                              */
/*     As a special exception, the copyright holders of this software give you permission to    */
/*     link this software with independent modules, and to copy and distribute the resulting    */
/*     executable under terms of your choice, provided that you also meet, for each linked      */
/*     independent module, the terms and conditions of the license of that module.              */
/*     An independent module is a module which is not derived from this software.  The special  */
/*     exception does not apply to any modifications of the software.                           */
/*                                                                                              */
/*     Notwithstanding the above, under no circumstances may you combine this software in any   */
/*     way with any other Broadcom software provided under a license other than the GPL,        */
/*     without Broadcom's express prior written consent.                                        */
/*                                                                                              */
/*     Date     : Generated on 11/8/2016 0:48:15                                             */
/*     RDB file : /projects/bcm89100/team                                                                   */
/************************************************************************************************/

#ifndef __BRCM_RDB_CAM_H__
#define __BRCM_RDB_CAM_H__

#define CAM_CAMCTL_OFFSET                                                 0x00000000
#define CAM_CAMCTL_TYPE                                                   UInt32
#define CAM_CAMCTL_RESERVED_MASK                                          0x018000A0
#define    CAM_CAMCTL_TRIG_FB_UPDATE_SHIFT                                31
#define    CAM_CAMCTL_TRIG_FB_UPDATE_MASK                                 0x80000000
#define    CAM_CAMCTL_PAUSE_CAMRX_SHIFT                                   30
#define    CAM_CAMCTL_PAUSE_CAMRX_MASK                                    0x40000000
#define    CAM_CAMCTL_DISABLE_256CROSS_BURST_W_SHIFT                      29
#define    CAM_CAMCTL_DISABLE_256CROSS_BURST_W_MASK                       0x20000000
#define    CAM_CAMCTL_DISABLE_256CROSS_BURST_R_SHIFT                      28
#define    CAM_CAMCTL_DISABLE_256CROSS_BURST_R_MASK                       0x10000000
#define    CAM_CAMCTL_INT_ON_256_CROSS_SHIFT                              27
#define    CAM_CAMCTL_INT_ON_256_CROSS_MASK                               0x08000000
#define    CAM_CAMCTL_CPI_YUV422_PACK_SHIFT                               26
#define    CAM_CAMCTL_CPI_YUV422_PACK_MASK                                0x04000000
#define    CAM_CAMCTL_INT_ON_TO_HW_SYNC_SHIFT                             25
#define    CAM_CAMCTL_INT_ON_TO_HW_SYNC_MASK                              0x02000000
#define    CAM_CAMCTL_DIS_DB_IE_SHIFT                                     22
#define    CAM_CAMCTL_DIS_DB_IE_MASK                                      0x00400000
#define    CAM_CAMCTL_CPM_SHIFT                                           20
#define    CAM_CAMCTL_CPM_MASK                                            0x00300000
#define    CAM_CAMCTL_OET_SHIFT                                           12
#define    CAM_CAMCTL_OET_MASK                                            0x000FF000
#define    CAM_CAMCTL_PFT_SHIFT                                           8
#define    CAM_CAMCTL_PFT_MASK                                            0x00000F00
#define    CAM_CAMCTL_SLS_SHIFT                                           6
#define    CAM_CAMCTL_SLS_MASK                                            0x00000040
#define    CAM_CAMCTL_SOE_SHIFT                                           4
#define    CAM_CAMCTL_SOE_MASK                                            0x00000010
#define    CAM_CAMCTL_CPR_DPHY_SHIFT                                      3
#define    CAM_CAMCTL_CPR_DPHY_MASK                                       0x00000008
#define    CAM_CAMCTL_CPR_SHIFT                                           2
#define    CAM_CAMCTL_CPR_MASK                                            0x00000004
#define    CAM_CAMCTL_MEN_SHIFT                                           1
#define    CAM_CAMCTL_MEN_MASK                                            0x00000002
#define    CAM_CAMCTL_CPE_SHIFT                                           0
#define    CAM_CAMCTL_CPE_MASK                                            0x00000001

#define CAM_CAMSTA_OFFSET                                                 0x00000004
#define CAM_CAMSTA_TYPE                                                   UInt32
#define CAM_CAMSTA_RESERVED_MASK                                          0xC10E0040
#define    CAM_CAMSTA_TO_HW_SYNC_SHIFT                                    29
#define    CAM_CAMSTA_TO_HW_SYNC_MASK                                     0x20000000
#define    CAM_CAMSTA_READ_256CROSS_SHIFT                                 28
#define    CAM_CAMSTA_READ_256CROSS_MASK                                  0x10000000
#define    CAM_CAMSTA_WRITE_256CROSS_SHIFT                                27
#define    CAM_CAMSTA_WRITE_256CROSS_MASK                                 0x08000000
#define    CAM_CAMSTA_AXIERR_SHIFT                                        26
#define    CAM_CAMSTA_AXIERR_MASK                                         0x04000000
#define    CAM_CAMSTA_FIFO_W_ERR_SHIFT                                    25
#define    CAM_CAMSTA_FIFO_W_ERR_MASK                                     0x02000000
#define    CAM_CAMSTA_BUF1_NO_SHIFT                                       23
#define    CAM_CAMSTA_BUF1_NO_MASK                                        0x00800000
#define    CAM_CAMSTA_BUF1_RDY_SHIFT                                      22
#define    CAM_CAMSTA_BUF1_RDY_MASK                                       0x00400000
#define    CAM_CAMSTA_BUF0_NO_SHIFT                                       21
#define    CAM_CAMSTA_BUF0_NO_MASK                                        0x00200000
#define    CAM_CAMSTA_BUF0_RDY_SHIFT                                      20
#define    CAM_CAMSTA_BUF0_RDY_MASK                                       0x00100000
#define    CAM_CAMSTA_PI1_SHIFT                                           16
#define    CAM_CAMSTA_PI1_MASK                                            0x00010000
#define    CAM_CAMSTA_PI0_SHIFT                                           15
#define    CAM_CAMSTA_PI0_MASK                                            0x00008000
#define    CAM_CAMSTA_IS_SHIFT                                            14
#define    CAM_CAMSTA_IS_MASK                                             0x00004000
#define    CAM_CAMSTA_PS_SHIFT                                            13
#define    CAM_CAMSTA_PS_MASK                                             0x00002000
#define    CAM_CAMSTA_DL_SHIFT                                            12
#define    CAM_CAMSTA_DL_MASK                                             0x00001000
#define    CAM_CAMSTA_BFO_SHIFT                                           11
#define    CAM_CAMSTA_BFO_MASK                                            0x00000800
#define    CAM_CAMSTA_OFO_SHIFT                                           10
#define    CAM_CAMSTA_OFO_MASK                                            0x00000400
#define    CAM_CAMSTA_IFO_SHIFT                                           9
#define    CAM_CAMSTA_IFO_MASK                                            0x00000200
#define    CAM_CAMSTA_OES_SHIFT                                           8
#define    CAM_CAMSTA_OES_MASK                                            0x00000100
#define    CAM_CAMSTA_CRCE_SHIFT                                          7
#define    CAM_CAMSTA_CRCE_MASK                                           0x00000080
#define    CAM_CAMSTA_PLE_SHIFT                                           5
#define    CAM_CAMSTA_PLE_MASK                                            0x00000020
#define    CAM_CAMSTA_HOE_SHIFT                                           4
#define    CAM_CAMSTA_HOE_MASK                                            0x00000010
#define    CAM_CAMSTA_PBE_SHIFT                                           3
#define    CAM_CAMSTA_PBE_MASK                                            0x00000008
#define    CAM_CAMSTA_SBE_SHIFT                                           2
#define    CAM_CAMSTA_SBE_MASK                                            0x00000004
#define    CAM_CAMSTA_CS_SHIFT                                            1
#define    CAM_CAMSTA_CS_MASK                                             0x00000002
#define    CAM_CAMSTA_SYN_SHIFT                                           0
#define    CAM_CAMSTA_SYN_MASK                                            0x00000001

#define CAM_CAMANA_OFFSET                                                 0x00000008
#define CAM_CAMANA_TYPE                                                   UInt32
#define CAM_CAMANA_RESERVED_MASK                                          0xFFFCF000
#define    CAM_CAMANA_CORERDY_SHIFT                                       17
#define    CAM_CAMANA_CORERDY_MASK                                        0x00020000
#define    CAM_CAMANA_LDO_PU_SHIFT                                        16
#define    CAM_CAMANA_LDO_PU_MASK                                         0x00010000
#define    CAM_CAMANA_PTATADJ_SHIFT                                       8
#define    CAM_CAMANA_PTATADJ_MASK                                        0x00000F00
#define    CAM_CAMANA_CTATADJ_SHIFT                                       4
#define    CAM_CAMANA_CTATADJ_MASK                                        0x000000F0
#define    CAM_CAMANA_DDL_SHIFT                                           3
#define    CAM_CAMANA_DDL_MASK                                            0x00000008
#define    CAM_CAMANA_AR_SHIFT                                            2
#define    CAM_CAMANA_AR_MASK                                             0x00000004
#define    CAM_CAMANA_BPD_SHIFT                                           1
#define    CAM_CAMANA_BPD_MASK                                            0x00000002
#define    CAM_CAMANA_APD_SHIFT                                           0
#define    CAM_CAMANA_APD_MASK                                            0x00000001

#define CAM_CAMPRI_OFFSET                                                 0x0000000C
#define CAM_CAMPRI_TYPE                                                   UInt32
#define CAM_CAMPRI_RESERVED_MASK                                          0x00FC0008
#define    CAM_CAMPRI_EXT_OET_MSB_SHIFT                                   24
#define    CAM_CAMPRI_EXT_OET_MSB_MASK                                    0xFF000000
#define    CAM_CAMPRI_BL_SHIFT                                            16
#define    CAM_CAMPRI_BL_MASK                                             0x00030000
#define    CAM_CAMPRI_BS_SHIFT                                            12
#define    CAM_CAMPRI_BS_MASK                                             0x0000F000
#define    CAM_CAMPRI_PP_SHIFT                                            8
#define    CAM_CAMPRI_PP_MASK                                             0x00000F00
#define    CAM_CAMPRI_NP_SHIFT                                            4
#define    CAM_CAMPRI_NP_MASK                                             0x000000F0
#define    CAM_CAMPRI_PT_SHIFT                                            1
#define    CAM_CAMPRI_PT_MASK                                             0x00000006
#define    CAM_CAMPRI_PE_SHIFT                                            0
#define    CAM_CAMPRI_PE_MASK                                             0x00000001

#define CAM_CAMCLK_OFFSET                                                 0x00000010
#define CAM_CAMCLK_TYPE                                                   UInt32
#define CAM_CAMCLK_RESERVED_MASK                                          0x10FFFFE0
#define    CAM_CAMCLK_CLDN_SHIFT                                          31
#define    CAM_CAMCLK_CLDN_MASK                                           0x80000000
#define    CAM_CAMCLK_CLDP_SHIFT                                          30
#define    CAM_CAMCLK_CLDP_MASK                                           0x40000000
#define    CAM_CAMCLK_CLSTE_SHIFT                                         29
#define    CAM_CAMCLK_CLSTE_MASK                                          0x20000000
#define    CAM_CAMCLK_CLS_SHIFT                                           24
#define    CAM_CAMCLK_CLS_MASK                                            0x0F000000
#define    CAM_CAMCLK_CLTRE_SHIFT                                         4
#define    CAM_CAMCLK_CLTRE_MASK                                          0x00000010
#define    CAM_CAMCLK_CLHSE_SHIFT                                         3
#define    CAM_CAMCLK_CLHSE_MASK                                          0x00000008
#define    CAM_CAMCLK_CLLPE_SHIFT                                         2
#define    CAM_CAMCLK_CLLPE_MASK                                          0x00000004
#define    CAM_CAMCLK_CLPD_SHIFT                                          1
#define    CAM_CAMCLK_CLPD_MASK                                           0x00000002
#define    CAM_CAMCLK_CLE_SHIFT                                           0
#define    CAM_CAMCLK_CLE_MASK                                            0x00000001

#define CAM_CAMCLT_OFFSET                                                 0x00000014
#define CAM_CAMCLT_TYPE                                                   UInt32
#define CAM_CAMCLT_RESERVED_MASK                                          0xFFFF0000
#define    CAM_CAMCLT_CLT2_SHIFT                                          8
#define    CAM_CAMCLT_CLT2_MASK                                           0x0000FF00
#define    CAM_CAMCLT_CLT1_SHIFT                                          0
#define    CAM_CAMCLT_CLT1_MASK                                           0x000000FF

#define CAM_CAMDAT0_OFFSET                                                0x00000018
#define CAM_CAMDAT0_TYPE                                                  UInt32
#define CAM_CAMDAT0_RESERVED_MASK                                         0x007FFFC0
#define    CAM_CAMDAT0_DLDNN_SHIFT                                        31
#define    CAM_CAMDAT0_DLDNN_MASK                                         0x80000000
#define    CAM_CAMDAT0_DLDPN_SHIFT                                        30
#define    CAM_CAMDAT0_DLDPN_MASK                                         0x40000000
#define    CAM_CAMDAT0_DLSTEN_SHIFT                                       29
#define    CAM_CAMDAT0_DLSTEN_MASK                                        0x20000000
#define    CAM_CAMDAT0_DLFON_SHIFT                                        28
#define    CAM_CAMDAT0_DLFON_MASK                                         0x10000000
#define    CAM_CAMDAT0_DLSN_SHIFT                                         24
#define    CAM_CAMDAT0_DLSN_MASK                                          0x0F000000
#define    CAM_CAMDAT0_DLSEN_SHIFT                                        23
#define    CAM_CAMDAT0_DLSEN_MASK                                         0x00800000
#define    CAM_CAMDAT0_DLSMN_SHIFT                                        5
#define    CAM_CAMDAT0_DLSMN_MASK                                         0x00000020
#define    CAM_CAMDAT0_DLTREN_SHIFT                                       4
#define    CAM_CAMDAT0_DLTREN_MASK                                        0x00000010
#define    CAM_CAMDAT0_DLHSEN_SHIFT                                       3
#define    CAM_CAMDAT0_DLHSEN_MASK                                        0x00000008
#define    CAM_CAMDAT0_DLLPEN_SHIFT                                       2
#define    CAM_CAMDAT0_DLLPEN_MASK                                        0x00000004
#define    CAM_CAMDAT0_DLPDN_SHIFT                                        1
#define    CAM_CAMDAT0_DLPDN_MASK                                         0x00000002
#define    CAM_CAMDAT0_DLEN_SHIFT                                         0
#define    CAM_CAMDAT0_DLEN_MASK                                          0x00000001

#define CAM_CAMDAT1_OFFSET                                                0x0000001C
#define CAM_CAMDAT1_TYPE                                                  UInt32
#define CAM_CAMDAT1_RESERVED_MASK                                         0x007FFFC0
#define    CAM_CAMDAT1_DLDNN_SHIFT                                        31
#define    CAM_CAMDAT1_DLDNN_MASK                                         0x80000000
#define    CAM_CAMDAT1_DLDPN_SHIFT                                        30
#define    CAM_CAMDAT1_DLDPN_MASK                                         0x40000000
#define    CAM_CAMDAT1_DLSTEN_SHIFT                                       29
#define    CAM_CAMDAT1_DLSTEN_MASK                                        0x20000000
#define    CAM_CAMDAT1_DLFON_SHIFT                                        28
#define    CAM_CAMDAT1_DLFON_MASK                                         0x10000000
#define    CAM_CAMDAT1_DLSN_SHIFT                                         24
#define    CAM_CAMDAT1_DLSN_MASK                                          0x0F000000
#define    CAM_CAMDAT1_DLSEN_SHIFT                                        23
#define    CAM_CAMDAT1_DLSEN_MASK                                         0x00800000
#define    CAM_CAMDAT1_DLSMN_SHIFT                                        5
#define    CAM_CAMDAT1_DLSMN_MASK                                         0x00000020
#define    CAM_CAMDAT1_DLTREN_SHIFT                                       4
#define    CAM_CAMDAT1_DLTREN_MASK                                        0x00000010
#define    CAM_CAMDAT1_DLHSEN_SHIFT                                       3
#define    CAM_CAMDAT1_DLHSEN_MASK                                        0x00000008
#define    CAM_CAMDAT1_DLLPEN_SHIFT                                       2
#define    CAM_CAMDAT1_DLLPEN_MASK                                        0x00000004
#define    CAM_CAMDAT1_DLPDN_SHIFT                                        1
#define    CAM_CAMDAT1_DLPDN_MASK                                         0x00000002
#define    CAM_CAMDAT1_DLEN_SHIFT                                         0
#define    CAM_CAMDAT1_DLEN_MASK                                          0x00000001

#define CAM_CAMDAT2_OFFSET                                                0x00000020
#define CAM_CAMDAT2_TYPE                                                  UInt32
#define CAM_CAMDAT2_RESERVED_MASK                                         0x007FFFC0
#define    CAM_CAMDAT2_DLDNN_SHIFT                                        31
#define    CAM_CAMDAT2_DLDNN_MASK                                         0x80000000
#define    CAM_CAMDAT2_DLDPN_SHIFT                                        30
#define    CAM_CAMDAT2_DLDPN_MASK                                         0x40000000
#define    CAM_CAMDAT2_DLSTEN_SHIFT                                       29
#define    CAM_CAMDAT2_DLSTEN_MASK                                        0x20000000
#define    CAM_CAMDAT2_DLFON_SHIFT                                        28
#define    CAM_CAMDAT2_DLFON_MASK                                         0x10000000
#define    CAM_CAMDAT2_DLSN_SHIFT                                         24
#define    CAM_CAMDAT2_DLSN_MASK                                          0x0F000000
#define    CAM_CAMDAT2_DLSEN_SHIFT                                        23
#define    CAM_CAMDAT2_DLSEN_MASK                                         0x00800000
#define    CAM_CAMDAT2_DLSMN_SHIFT                                        5
#define    CAM_CAMDAT2_DLSMN_MASK                                         0x00000020
#define    CAM_CAMDAT2_DLTREN_SHIFT                                       4
#define    CAM_CAMDAT2_DLTREN_MASK                                        0x00000010
#define    CAM_CAMDAT2_DLHSEN_SHIFT                                       3
#define    CAM_CAMDAT2_DLHSEN_MASK                                        0x00000008
#define    CAM_CAMDAT2_DLLPEN_SHIFT                                       2
#define    CAM_CAMDAT2_DLLPEN_MASK                                        0x00000004
#define    CAM_CAMDAT2_DLPDN_SHIFT                                        1
#define    CAM_CAMDAT2_DLPDN_MASK                                         0x00000002
#define    CAM_CAMDAT2_DLEN_SHIFT                                         0
#define    CAM_CAMDAT2_DLEN_MASK                                          0x00000001

#define CAM_CAMDAT3_OFFSET                                                0x00000024
#define CAM_CAMDAT3_TYPE                                                  UInt32
#define CAM_CAMDAT3_RESERVED_MASK                                         0x007FFFC0
#define    CAM_CAMDAT3_DLDNN_SHIFT                                        31
#define    CAM_CAMDAT3_DLDNN_MASK                                         0x80000000
#define    CAM_CAMDAT3_DLDPN_SHIFT                                        30
#define    CAM_CAMDAT3_DLDPN_MASK                                         0x40000000
#define    CAM_CAMDAT3_DLSTEN_SHIFT                                       29
#define    CAM_CAMDAT3_DLSTEN_MASK                                        0x20000000
#define    CAM_CAMDAT3_DLFON_SHIFT                                        28
#define    CAM_CAMDAT3_DLFON_MASK                                         0x10000000
#define    CAM_CAMDAT3_DLSN_SHIFT                                         24
#define    CAM_CAMDAT3_DLSN_MASK                                          0x0F000000
#define    CAM_CAMDAT3_DLSEN_SHIFT                                        23
#define    CAM_CAMDAT3_DLSEN_MASK                                         0x00800000
#define    CAM_CAMDAT3_DLSMN_SHIFT                                        5
#define    CAM_CAMDAT3_DLSMN_MASK                                         0x00000020
#define    CAM_CAMDAT3_DLTREN_SHIFT                                       4
#define    CAM_CAMDAT3_DLTREN_MASK                                        0x00000010
#define    CAM_CAMDAT3_DLHSEN_SHIFT                                       3
#define    CAM_CAMDAT3_DLHSEN_MASK                                        0x00000008
#define    CAM_CAMDAT3_DLLPEN_SHIFT                                       2
#define    CAM_CAMDAT3_DLLPEN_MASK                                        0x00000004
#define    CAM_CAMDAT3_DLPDN_SHIFT                                        1
#define    CAM_CAMDAT3_DLPDN_MASK                                         0x00000002
#define    CAM_CAMDAT3_DLEN_SHIFT                                         0
#define    CAM_CAMDAT3_DLEN_MASK                                          0x00000001

#define CAM_CAMDLT_OFFSET                                                 0x00000028
#define CAM_CAMDLT_TYPE                                                   UInt32
#define CAM_CAMDLT_RESERVED_MASK                                          0xFF000000
#define    CAM_CAMDLT_DLT3_SHIFT                                          16
#define    CAM_CAMDLT_DLT3_MASK                                           0x00FF0000
#define    CAM_CAMDLT_DLT2_SHIFT                                          8
#define    CAM_CAMDLT_DLT2_MASK                                           0x0000FF00
#define    CAM_CAMDLT_DLT1_SHIFT                                          0
#define    CAM_CAMDLT_DLT1_MASK                                           0x000000FF

#define CAM_CAMCMP0_OFFSET                                                0x0000002C
#define CAM_CAMCMP0_TYPE                                                  UInt32
#define CAM_CAMCMP0_RESERVED_MASK                                         0x7FFFFC00
#define    CAM_CAMCMP0_PCEN_SHIFT                                         31
#define    CAM_CAMCMP0_PCEN_MASK                                          0x80000000
#define    CAM_CAMCMP0_GIN_SHIFT                                          9
#define    CAM_CAMCMP0_GIN_MASK                                           0x00000200
#define    CAM_CAMCMP0_CPHN_SHIFT                                         8
#define    CAM_CAMCMP0_CPHN_MASK                                          0x00000100
#define    CAM_CAMCMP0_PCVCN_SHIFT                                        6
#define    CAM_CAMCMP0_PCVCN_MASK                                         0x000000C0
#define    CAM_CAMCMP0_PCDTN_SHIFT                                        0
#define    CAM_CAMCMP0_PCDTN_MASK                                         0x0000003F

#define CAM_CAMCMP1_OFFSET                                                0x00000030
#define CAM_CAMCMP1_TYPE                                                  UInt32
#define CAM_CAMCMP1_RESERVED_MASK                                         0x7FFFFC00
#define    CAM_CAMCMP1_PCEN_SHIFT                                         31
#define    CAM_CAMCMP1_PCEN_MASK                                          0x80000000
#define    CAM_CAMCMP1_GIN_SHIFT                                          9
#define    CAM_CAMCMP1_GIN_MASK                                           0x00000200
#define    CAM_CAMCMP1_CPHN_SHIFT                                         8
#define    CAM_CAMCMP1_CPHN_MASK                                          0x00000100
#define    CAM_CAMCMP1_PCVCN_SHIFT                                        6
#define    CAM_CAMCMP1_PCVCN_MASK                                         0x000000C0
#define    CAM_CAMCMP1_PCDTN_SHIFT                                        0
#define    CAM_CAMCMP1_PCDTN_MASK                                         0x0000003F

#define CAM_CAMCAP0_OFFSET                                                0x00000034
#define CAM_CAMCAP0_TYPE                                                  UInt32
#define CAM_CAMCAP0_RESERVED_MASK                                         0x40000000
#define    CAM_CAMCAP0_CPHV_SHIFT                                         31
#define    CAM_CAMCAP0_CPHV_MASK                                          0x80000000
#define    CAM_CAMCAP0_CECCN_SHIFT                                        24
#define    CAM_CAMCAP0_CECCN_MASK                                         0x3F000000
#define    CAM_CAMCAP0_CWCN_SHIFT                                         8
#define    CAM_CAMCAP0_CWCN_MASK                                          0x00FFFF00
#define    CAM_CAMCAP0_CVCN_SHIFT                                         6
#define    CAM_CAMCAP0_CVCN_MASK                                          0x000000C0
#define    CAM_CAMCAP0_CDTN_SHIFT                                         0
#define    CAM_CAMCAP0_CDTN_MASK                                          0x0000003F

#define CAM_CAMCAP1_OFFSET                                                0x00000038
#define CAM_CAMCAP1_TYPE                                                  UInt32
#define CAM_CAMCAP1_RESERVED_MASK                                         0x40000000
#define    CAM_CAMCAP1_CPHV_SHIFT                                         31
#define    CAM_CAMCAP1_CPHV_MASK                                          0x80000000
#define    CAM_CAMCAP1_CECCN_SHIFT                                        24
#define    CAM_CAMCAP1_CECCN_MASK                                         0x3F000000
#define    CAM_CAMCAP1_CWCN_SHIFT                                         8
#define    CAM_CAMCAP1_CWCN_MASK                                          0x00FFFF00
#define    CAM_CAMCAP1_CVCN_SHIFT                                         6
#define    CAM_CAMCAP1_CVCN_MASK                                          0x000000C0
#define    CAM_CAMCAP1_CDTN_SHIFT                                         0
#define    CAM_CAMCAP1_CDTN_MASK                                          0x0000003F

#define CAM_CAMCLKACTL_OFFSET                                             0x0000003C
#define CAM_CAMCLKACTL_TYPE                                               UInt32
#define CAM_CAMCLKACTL_RESERVED_MASK                                      0x00000000
#define    CAM_CAMCLKACTL_CLAC_SHIFT                                      0
#define    CAM_CAMCLKACTL_CLAC_MASK                                       0xFFFFFFFF

#define CAM_CAMDATACTL0_OFFSET                                            0x00000040
#define CAM_CAMDATACTL0_TYPE                                              UInt32
#define CAM_CAMDATACTL0_RESERVED_MASK                                     0x00000000
#define    CAM_CAMDATACTL0_CLAC_SHIFT                                     0
#define    CAM_CAMDATACTL0_CLAC_MASK                                      0xFFFFFFFF

#define CAM_CAMDATACTL1_OFFSET                                            0x00000044
#define CAM_CAMDATACTL1_TYPE                                              UInt32
#define CAM_CAMDATACTL1_RESERVED_MASK                                     0x00000000
#define    CAM_CAMDATACTL1_CLAC_SHIFT                                     0
#define    CAM_CAMDATACTL1_CLAC_MASK                                      0xFFFFFFFF

#define CAM_CAMDATACTL2_OFFSET                                            0x00000048
#define CAM_CAMDATACTL2_TYPE                                              UInt32
#define CAM_CAMDATACTL2_RESERVED_MASK                                     0x00000000
#define    CAM_CAMDATACTL2_CLAC_SHIFT                                     0
#define    CAM_CAMDATACTL2_CLAC_MASK                                      0xFFFFFFFF

#define CAM_CAMDATACTL3_OFFSET                                            0x0000004C
#define CAM_CAMDATACTL3_TYPE                                              UInt32
#define CAM_CAMDATACTL3_RESERVED_MASK                                     0x00000000
#define    CAM_CAMDATACTL3_CLAC_SHIFT                                     0
#define    CAM_CAMDATACTL3_CLAC_MASK                                      0xFFFFFFFF

#define CAM_CAMLDOCTL_OFFSET                                              0x00000050
#define CAM_CAMLDOCTL_TYPE                                                UInt32
#define CAM_CAMLDOCTL_RESERVED_MASK                                       0xFFFC0000
#define    CAM_CAMLDOCTL_LDOCTL_SHIFT                                     0
#define    CAM_CAMLDOCTL_LDOCTL_MASK                                      0x0003FFFF

#define CAM_CAMPIOCTL_OFFSET                                              0x00000054
#define CAM_CAMPIOCTL_TYPE                                                UInt32
#define CAM_CAMPIOCTL_RESERVED_MASK                                       0xFFFFFE00
#define    CAM_CAMPIOCTL_PIO_CTL_RES_SHIFT                                7
#define    CAM_CAMPIOCTL_PIO_CTL_RES_MASK                                 0x00000180
#define    CAM_CAMPIOCTL_PIO_LPRX_EN_CLK_SHIFT                            6
#define    CAM_CAMPIOCTL_PIO_LPRX_EN_CLK_MASK                             0x00000040
#define    CAM_CAMPIOCTL_PIO_RX0_EN_SHIFT                                 5
#define    CAM_CAMPIOCTL_PIO_RX0_EN_MASK                                  0x00000020
#define    CAM_CAMPIOCTL_PIO_RX1_EN_SHIFT                                 4
#define    CAM_CAMPIOCTL_PIO_RX1_EN_MASK                                  0x00000010
#define    CAM_CAMPIOCTL_PIO_RX2_EN_SHIFT                                 3
#define    CAM_CAMPIOCTL_PIO_RX2_EN_MASK                                  0x00000008
#define    CAM_CAMPIOCTL_PIO_RX3_EN_SHIFT                                 2
#define    CAM_CAMPIOCTL_PIO_RX3_EN_MASK                                  0x00000004
#define    CAM_CAMPIOCTL_PIO_LANE_RX0_EN_SHIFT                            1
#define    CAM_CAMPIOCTL_PIO_LANE_RX0_EN_MASK                             0x00000002
#define    CAM_CAMPIOCTL_PIO_LANE_RX1_EN_SHIFT                            0
#define    CAM_CAMPIOCTL_PIO_LANE_RX1_EN_MASK                             0x00000001

#define CAM_CAMDBG0_OFFSET                                                0x000000F0
#define CAM_CAMDBG0_TYPE                                                  UInt32
#define CAM_CAMDBG0_RESERVED_MASK                                         0xFFF00000
#define    CAM_CAMDBG0_L3PK_SHIFT                                         15
#define    CAM_CAMDBG0_L3PK_MASK                                          0x000F8000
#define    CAM_CAMDBG0_L2PK_SHIFT                                         10
#define    CAM_CAMDBG0_L2PK_MASK                                          0x00007C00
#define    CAM_CAMDBG0_L1PK_SHIFT                                         5
#define    CAM_CAMDBG0_L1PK_MASK                                          0x000003E0
#define    CAM_CAMDBG0_L0PK_SHIFT                                         0
#define    CAM_CAMDBG0_L0PK_MASK                                          0x0000001F

#define CAM_CAMDBG1_OFFSET                                                0x000000F4
#define CAM_CAMDBG1_TYPE                                                  UInt32
#define CAM_CAMDBG1_RESERVED_MASK                                         0x00000000
#define    CAM_CAMDBG1_BFPK_SHIFT                                         16
#define    CAM_CAMDBG1_BFPK_MASK                                          0xFFFF0000
#define    CAM_CAMDBG1_DFPK_SHIFT                                         0
#define    CAM_CAMDBG1_DFPK_MASK                                          0x0000FFFF

#define CAM_CAMDBG2_OFFSET                                                0x000000F8
#define CAM_CAMDBG2_TYPE                                                  UInt32
#define CAM_CAMDBG2_RESERVED_MASK                                         0x00000000
#define    CAM_CAMDBG2_BFC_SHIFT                                          16
#define    CAM_CAMDBG2_BFC_MASK                                           0xFFFF0000
#define    CAM_CAMDBG2_DFC_SHIFT                                          0
#define    CAM_CAMDBG2_DFC_MASK                                           0x0000FFFF

#define CAM_CAMDBG3_OFFSET                                                0x000000FC
#define CAM_CAMDBG3_TYPE                                                  UInt32
#define CAM_CAMDBG3_RESERVED_MASK                                         0x0000FF80
#define    CAM_CAMDBG3_IFPK_SHIFT                                         16
#define    CAM_CAMDBG3_IFPK_MASK                                          0xFFFF0000
#define    CAM_CAMDBG3_PFS_SHIFT                                          4
#define    CAM_CAMDBG3_PFS_MASK                                           0x00000070
#define    CAM_CAMDBG3_HSSYNC_SHIFT                                       0
#define    CAM_CAMDBG3_HSSYNC_MASK                                        0x0000000F

#define CAM_CAMICTL_OFFSET                                                0x00000100
#define CAM_CAMICTL_TYPE                                                  UInt32
#define CAM_CAMICTL_RESERVED_MASK                                         0x00000000
#define    CAM_CAMICTL_DLSTE_EN_LN3_SHIFT                                 31
#define    CAM_CAMICTL_DLSTE_EN_LN3_MASK                                  0x80000000
#define    CAM_CAMICTL_DLSTE_EN_LN2_SHIFT                                 30
#define    CAM_CAMICTL_DLSTE_EN_LN2_MASK                                  0x40000000
#define    CAM_CAMICTL_CAMSTAT_IB_DIS_SHIFT                               29
#define    CAM_CAMICTL_CAMSTAT_IB_DIS_MASK                                0x20000000
#define    CAM_CAMICTL_LCIE_SHIFT                                         16
#define    CAM_CAMICTL_LCIE_MASK                                          0x1FFF0000
#define    CAM_CAMICTL_DLSTE_EN_LN1_SHIFT                                 15
#define    CAM_CAMICTL_DLSTE_EN_LN1_MASK                                  0x00008000
#define    CAM_CAMICTL_DLSTE_EN_LN0_SHIFT                                 14
#define    CAM_CAMICTL_DLSTE_EN_LN0_MASK                                  0x00004000
#define    CAM_CAMICTL_CLSTE_EN_SHIFT                                     13
#define    CAM_CAMICTL_CLSTE_EN_MASK                                      0x00002000
#define    CAM_CAMICTL_CRCE_EN_SHIFT                                      12
#define    CAM_CAMICTL_CRCE_EN_MASK                                       0x00001000
#define    CAM_CAMICTL_PLE_EN_SHIFT                                       11
#define    CAM_CAMICTL_PLE_EN_MASK                                        0x00000800
#define    CAM_CAMICTL_HOE_EN_SHIFT                                       10
#define    CAM_CAMICTL_HOE_EN_MASK                                        0x00000400
#define    CAM_CAMICTL_PBE_EN_SHIFT                                       9
#define    CAM_CAMICTL_PBE_EN_MASK                                        0x00000200
#define    CAM_CAMICTL_SBE_EN_SHIFT                                       8
#define    CAM_CAMICTL_SBE_EN_MASK                                        0x00000100
#define    CAM_CAMICTL_AXIERR_INT_EN_SHIFT                                7
#define    CAM_CAMICTL_AXIERR_INT_EN_MASK                                 0x00000080
#define    CAM_CAMICTL_LIP_SHIFT                                          5
#define    CAM_CAMICTL_LIP_MASK                                           0x00000060
#define    CAM_CAMICTL_TFC_SHIFT                                          4
#define    CAM_CAMICTL_TFC_MASK                                           0x00000010
#define    CAM_CAMICTL_FCM_SHIFT                                          3
#define    CAM_CAMICTL_FCM_MASK                                           0x00000008
#define    CAM_CAMICTL_IBOB_SHIFT                                         2
#define    CAM_CAMICTL_IBOB_MASK                                          0x00000004
#define    CAM_CAMICTL_FEIE_SHIFT                                         1
#define    CAM_CAMICTL_FEIE_MASK                                          0x00000002
#define    CAM_CAMICTL_FSIE_SHIFT                                         0
#define    CAM_CAMICTL_FSIE_MASK                                          0x00000001

#define CAM_CAMISTA_OFFSET                                                0x00000104
#define CAM_CAMISTA_TYPE                                                  UInt32
#define CAM_CAMISTA_RESERVED_MASK                                         0xFFFFFFF8
#define    CAM_CAMISTA_LCI_SHIFT                                          2
#define    CAM_CAMISTA_LCI_MASK                                           0x00000004
#define    CAM_CAMISTA_FEI_SHIFT                                          1
#define    CAM_CAMISTA_FEI_MASK                                           0x00000002
#define    CAM_CAMISTA_FSI_SHIFT                                          0
#define    CAM_CAMISTA_FSI_MASK                                           0x00000001

#define CAM_CAMIDI0_OFFSET                                                0x00000108
#define CAM_CAMIDI0_TYPE                                                  UInt32
#define CAM_CAMIDI0_RESERVED_MASK                                         0x00000000
#define    CAM_CAMIDI0_IDI3_SHIFT                                         24
#define    CAM_CAMIDI0_IDI3_MASK                                          0xFF000000
#define    CAM_CAMIDI0_IDI2_SHIFT                                         16
#define    CAM_CAMIDI0_IDI2_MASK                                          0x00FF0000
#define    CAM_CAMIDI0_IDI1_SHIFT                                         8
#define    CAM_CAMIDI0_IDI1_MASK                                          0x0000FF00
#define    CAM_CAMIDI0_IDI0_SHIFT                                         0
#define    CAM_CAMIDI0_IDI0_MASK                                          0x000000FF

#define CAM_CAMIPIPE_OFFSET                                               0x0000010C
#define CAM_CAMIPIPE_TYPE                                                 UInt32
#define CAM_CAMIPIPE_RESERVED_MASK                                        0xFFFFFF00
#define    CAM_CAMIPIPE_DDM_SHIFT                                         4
#define    CAM_CAMIPIPE_DDM_MASK                                          0x000000F0
#define    CAM_CAMIPIPE_PUM_SHIFT                                         0
#define    CAM_CAMIPIPE_PUM_MASK                                          0x0000000F

#define CAM_CAMIBSA0_OFFSET                                               0x00000110
#define CAM_CAMIBSA0_TYPE                                                 UInt32
#define CAM_CAMIBSA0_RESERVED_MASK                                        0x00000000
#define    CAM_CAMIBSA0_IBSA0_SHIFT                                       0
#define    CAM_CAMIBSA0_IBSA0_MASK                                        0xFFFFFFFF

#define CAM_CAMIBEA0_OFFSET                                               0x00000114
#define CAM_CAMIBEA0_TYPE                                                 UInt32
#define CAM_CAMIBEA0_RESERVED_MASK                                        0x00000000
#define    CAM_CAMIBEA0_IBEA0_SHIFT                                       0
#define    CAM_CAMIBEA0_IBEA0_MASK                                        0xFFFFFFFF

#define CAM_CAMIBLS_OFFSET                                                0x00000118
#define CAM_CAMIBLS_TYPE                                                  UInt32
#define CAM_CAMIBLS_RESERVED_MASK                                         0xFFFF0000
#define    CAM_CAMIBLS_IBLS_SHIFT                                         0
#define    CAM_CAMIBLS_IBLS_MASK                                          0x0000FFFF

#define CAM_CAMIBWP_OFFSET                                                0x0000011C
#define CAM_CAMIBWP_TYPE                                                  UInt32
#define CAM_CAMIBWP_RESERVED_MASK                                         0x00000000
#define    CAM_CAMIBWP_IBWP_SHIFT                                         0
#define    CAM_CAMIBWP_IBWP_MASK                                          0xFFFFFFFF

#define CAM_CAMICTL_SE_OFFSET                                             0x00000170
#define CAM_CAMICTL_SE_TYPE                                               UInt32
#define CAM_CAMICTL_SE_RESERVED_MASK                                      0xFFF98000
#define    CAM_CAMICTL_SE_DI_END_SE_EN_SHIFT                              18
#define    CAM_CAMICTL_SE_DI_END_SE_EN_MASK                               0x00040000
#define    CAM_CAMICTL_SE_DI_SE_EN_SHIFT                                  17
#define    CAM_CAMICTL_SE_DI_SE_EN_MASK                                   0x00020000
#define    CAM_CAMICTL_SE_LCI_SE_EN_SHIFT                                 2
#define    CAM_CAMICTL_SE_LCI_SE_EN_MASK                                  0x00007FFC
#define    CAM_CAMICTL_SE_FEI_SE_EN_SHIFT                                 1
#define    CAM_CAMICTL_SE_FEI_SE_EN_MASK                                  0x00000002
#define    CAM_CAMICTL_SE_FSI_SE_EN_SHIFT                                 0
#define    CAM_CAMICTL_SE_FSI_SE_EN_MASK                                  0x00000001

#define CAM_CAMISTA_SE_OFFSET                                             0x00000174
#define CAM_CAMISTA_SE_TYPE                                               UInt32
#define CAM_CAMISTA_SE_RESERVED_MASK                                      0xFFFFFFE0
#define    CAM_CAMISTA_SE_DI_END_SE_SHIFT                                 4
#define    CAM_CAMISTA_SE_DI_END_SE_MASK                                  0x00000010
#define    CAM_CAMISTA_SE_DI_SE_SHIFT                                     3
#define    CAM_CAMISTA_SE_DI_SE_MASK                                      0x00000008
#define    CAM_CAMISTA_SE_LCI_SE_SHIFT                                    2
#define    CAM_CAMISTA_SE_LCI_SE_MASK                                     0x00000004
#define    CAM_CAMISTA_SE_FEI_SE_SHIFT                                    1
#define    CAM_CAMISTA_SE_FEI_SE_MASK                                     0x00000002
#define    CAM_CAMISTA_SE_FSI_SE_SHIFT                                    0
#define    CAM_CAMISTA_SE_FSI_SE_MASK                                     0x00000001

#define CAM_CAMIDATA_SE_OFFSET                                            0x00000178
#define CAM_CAMIDATA_SE_TYPE                                              UInt32
#define CAM_CAMIDATA_SE_RESERVED_MASK                                     0xFFF00F00
#define    CAM_CAMIDATA_SE_EDL_END_SE_SHIFT                               12
#define    CAM_CAMIDATA_SE_EDL_END_SE_MASK                                0x000FF000
#define    CAM_CAMIDATA_SE_EDL_SE_SHIFT                                   0
#define    CAM_CAMIDATA_SE_EDL_SE_MASK                                    0x000000FF

#define CAM_CAMDCS_OFFSET                                                 0x00000200
#define CAM_CAMDCS_TYPE                                                   UInt32
#define CAM_CAMDCS_RESERVED_MASK                                          0xFC0000D2
#define    CAM_CAMDCS_EDL_END_SHIFT                                       18
#define    CAM_CAMDCS_EDL_END_MASK                                        0x03FC0000
#define    CAM_CAMDCS_DI_END_SHIFT                                        17
#define    CAM_CAMDCS_DI_END_MASK                                         0x00020000
#define    CAM_CAMDCS_DIE_END_SHIFT                                       16
#define    CAM_CAMDCS_DIE_END_MASK                                        0x00010000
#define    CAM_CAMDCS_EDL_SHIFT                                           8
#define    CAM_CAMDCS_EDL_MASK                                            0x0000FF00
#define    CAM_CAMDCS_LDP_SHIFT                                           5
#define    CAM_CAMDCS_LDP_MASK                                            0x00000020
#define    CAM_CAMDCS_DBOB_SHIFT                                          3
#define    CAM_CAMDCS_DBOB_MASK                                           0x00000008
#define    CAM_CAMDCS_DI_SHIFT                                            2
#define    CAM_CAMDCS_DI_MASK                                             0x00000004
#define    CAM_CAMDCS_DIE_SHIFT                                           0
#define    CAM_CAMDCS_DIE_MASK                                            0x00000001

#define CAM_CAMDBSA0_OFFSET                                               0x00000204
#define CAM_CAMDBSA0_TYPE                                                 UInt32
#define CAM_CAMDBSA0_RESERVED_MASK                                        0x00000000
#define    CAM_CAMDBSA0_DBSA0_SHIFT                                       0
#define    CAM_CAMDBSA0_DBSA0_MASK                                        0xFFFFFFFF

#define CAM_CAMDBEA0_OFFSET                                               0x00000208
#define CAM_CAMDBEA0_TYPE                                                 UInt32
#define CAM_CAMDBEA0_RESERVED_MASK                                        0x00000000
#define    CAM_CAMDBEA0_DBEA0_SHIFT                                       0
#define    CAM_CAMDBEA0_DBEA0_MASK                                        0xFFFFFFFF

#define CAM_CAMDBWP_OFFSET                                                0x0000020C
#define CAM_CAMDBWP_TYPE                                                  UInt32
#define CAM_CAMDBWP_RESERVED_MASK                                         0x00000000
#define    CAM_CAMDBWP_DBWP_SHIFT                                         0
#define    CAM_CAMDBWP_DBWP_MASK                                          0xFFFFFFFF

#define CAM_CAMDBCTL_OFFSET                                               0x00000300
#define CAM_CAMDBCTL_TYPE                                                 UInt32
#define CAM_CAMDBCTL_RESERVED_MASK                                        0xFFFFFFF0
#define    CAM_CAMDBCTL_DB_EN_DATA_SHIFT                                  3
#define    CAM_CAMDBCTL_DB_EN_DATA_MASK                                   0x00000008
#define    CAM_CAMDBCTL_BUF1_IE_SHIFT                                     2
#define    CAM_CAMDBCTL_BUF1_IE_MASK                                      0x00000004
#define    CAM_CAMDBCTL_BUF0_IE_SHIFT                                     1
#define    CAM_CAMDBCTL_BUF0_IE_MASK                                      0x00000002
#define    CAM_CAMDBCTL_DB_EN_SHIFT                                       0
#define    CAM_CAMDBCTL_DB_EN_MASK                                        0x00000001

#define CAM_CAMIBSA1_OFFSET                                               0x00000304
#define CAM_CAMIBSA1_TYPE                                                 UInt32
#define CAM_CAMIBSA1_RESERVED_MASK                                        0x00000000
#define    CAM_CAMIBSA1_IBSA1_SHIFT                                       0
#define    CAM_CAMIBSA1_IBSA1_MASK                                        0xFFFFFFFF

#define CAM_CAMIBEA1_OFFSET                                               0x00000308
#define CAM_CAMIBEA1_TYPE                                                 UInt32
#define CAM_CAMIBEA1_RESERVED_MASK                                        0x00000000
#define    CAM_CAMIBEA1_IBEA1_SHIFT                                       0
#define    CAM_CAMIBEA1_IBEA1_MASK                                        0xFFFFFFFF

#define CAM_CAMIDI1_OFFSET                                                0x0000030C
#define CAM_CAMIDI1_TYPE                                                  UInt32
#define CAM_CAMIDI1_RESERVED_MASK                                         0x00000000
#define    CAM_CAMIDI1_IDI7_SHIFT                                         24
#define    CAM_CAMIDI1_IDI7_MASK                                          0xFF000000
#define    CAM_CAMIDI1_IDI6_SHIFT                                         16
#define    CAM_CAMIDI1_IDI6_MASK                                          0x00FF0000
#define    CAM_CAMIDI1_IDI5_SHIFT                                         8
#define    CAM_CAMIDI1_IDI5_MASK                                          0x0000FF00
#define    CAM_CAMIDI1_IDI4_SHIFT                                         0
#define    CAM_CAMIDI1_IDI4_MASK                                          0x000000FF

#define CAM_CAMDBSA1_OFFSET                                               0x00000310
#define CAM_CAMDBSA1_TYPE                                                 UInt32
#define CAM_CAMDBSA1_RESERVED_MASK                                        0x00000000
#define    CAM_CAMDBSA1_DBSA1_SHIFT                                       0
#define    CAM_CAMDBSA1_DBSA1_MASK                                        0xFFFFFFFF

#define CAM_CAMDBEA1_OFFSET                                               0x00000314
#define CAM_CAMDBEA1_TYPE                                                 UInt32
#define CAM_CAMDBEA1_RESERVED_MASK                                        0x00000000
#define    CAM_CAMDBEA1_DBEA1_SHIFT                                       0
#define    CAM_CAMDBEA1_DBEA1_MASK                                        0xFFFFFFFF

#define CAM_CAMFRSZ_OFFSET                                                0x00000320
#define CAM_CAMFRSZ_TYPE                                                  UInt32
#define CAM_CAMFRSZ_RESERVED_MASK                                         0x00000000
#define    CAM_CAMFRSZ_CTL_FRAME_HEIGHT_SHIFT                             16
#define    CAM_CAMFRSZ_CTL_FRAME_HEIGHT_MASK                              0xFFFF0000
#define    CAM_CAMFRSZ_CTL_LINE_LENGTH_SHIFT                              0
#define    CAM_CAMFRSZ_CTL_LINE_LENGTH_MASK                               0x0000FFFF

#define CAM_CAMLINCTL_OFFSET                                              0x00000324
#define CAM_CAMLINCTL_TYPE                                                UInt32
#define CAM_CAMLINCTL_RESERVED_MASK                                       0xFFFFFF80
#define    CAM_CAMLINCTL_CTL_CHECK_LE_SYNC_SHIFT                          6
#define    CAM_CAMLINCTL_CTL_CHECK_LE_SYNC_MASK                           0x00000040
#define    CAM_CAMLINCTL_CTL_GEN_LE_SYNC_SHIFT                            5
#define    CAM_CAMLINCTL_CTL_GEN_LE_SYNC_MASK                             0x00000020
#define    CAM_CAMLINCTL_CTL_GEN_LS_SYNC_SHIFT                            4
#define    CAM_CAMLINCTL_CTL_GEN_LS_SYNC_MASK                             0x00000010
#define    CAM_CAMLINCTL_UNPACK_FLUSH_LE_SHIFT                            3
#define    CAM_CAMLINCTL_UNPACK_FLUSH_LE_MASK                             0x00000008
#define    CAM_CAMLINCTL_USE_LE_SHIFT                                     2
#define    CAM_CAMLINCTL_USE_LE_MASK                                      0x00000004
#define    CAM_CAMLINCTL_PASS_LE_SHIFT                                    1
#define    CAM_CAMLINCTL_PASS_LE_MASK                                     0x00000002
#define    CAM_CAMLINCTL_PASS_LS_SHIFT                                    0
#define    CAM_CAMLINCTL_PASS_LS_MASK                                     0x00000001

#define CAM_CAMEXP1CTL_OFFSET                                             0x00000328
#define CAM_CAMEXP1CTL_TYPE                                               UInt32
#define CAM_CAMEXP1CTL_RESERVED_MASK                                      0xFFFFFF00
#define    CAM_CAMEXP1CTL_PIXF_EXP1_SHIFT                                 4
#define    CAM_CAMEXP1CTL_PIXF_EXP1_MASK                                  0x000000F0
#define    CAM_CAMEXP1CTL_PIXF_EXP0_SHIFT                                 0
#define    CAM_CAMEXP1CTL_PIXF_EXP0_MASK                                  0x0000000F

#define CAM_CAMHDRCTL_OFFSET                                              0x00000330
#define CAM_CAMHDRCTL_TYPE                                                UInt32
#define CAM_CAMHDRCTL_RESERVED_MASK                                       0x7FF800C0
#define    CAM_CAMHDRCTL_MULTI_RES_HDR_SHIFT                              31
#define    CAM_CAMHDRCTL_MULTI_RES_HDR_MASK                               0x80000000
#define    CAM_CAMHDRCTL_PASS_OB_LINES_SHIFT                              18
#define    CAM_CAMHDRCTL_PASS_OB_LINES_MASK                               0x00040000
#define    CAM_CAMHDRCTL_SONY_HDR_MODE_SHIFT                              17
#define    CAM_CAMHDRCTL_SONY_HDR_MODE_MASK                               0x00020000
#define    CAM_CAMHDRCTL_HDR_INIT_EXP_SHIFT                               16
#define    CAM_CAMHDRCTL_HDR_INIT_EXP_MASK                                0x00010000
#define    CAM_CAMHDRCTL_HDR_LINE_OFFSET_SHIFT                            8
#define    CAM_CAMHDRCTL_HDR_LINE_OFFSET_MASK                             0x0000FF00
#define    CAM_CAMHDRCTL_HDR_CSI2_SE_VC_SHIFT                             4
#define    CAM_CAMHDRCTL_HDR_CSI2_SE_VC_MASK                              0x00000030
#define    CAM_CAMHDRCTL_HDR_CSI2_LE_VC_SHIFT                             2
#define    CAM_CAMHDRCTL_HDR_CSI2_LE_VC_MASK                              0x0000000C
#define    CAM_CAMHDRCTL_HDR_CSI2_VC_EN_SHIFT                             1
#define    CAM_CAMHDRCTL_HDR_CSI2_VC_EN_MASK                              0x00000002
#define    CAM_CAMHDRCTL_HDR_EN_SHIFT                                     0
#define    CAM_CAMHDRCTL_HDR_EN_MASK                                      0x00000001

#define CAM_CAMISP_HWSYC_CTL_OFFSET                                       0x00000334
#define CAM_CAMISP_HWSYC_CTL_TYPE                                         UInt32
#define CAM_CAMISP_HWSYC_CTL_RESERVED_MASK                                0x20000000
#define    CAM_CAMISP_HWSYC_CTL_CTL_EN_AXI_HWSYNC_SHIFT                   31
#define    CAM_CAMISP_HWSYC_CTL_CTL_EN_AXI_HWSYNC_MASK                    0x80000000
#define    CAM_CAMISP_HWSYC_CTL_CTL_EN_FE_AXI_SYNC_SHIFT                  30
#define    CAM_CAMISP_HWSYC_CTL_CTL_EN_FE_AXI_SYNC_MASK                   0x40000000
#define    CAM_CAMISP_HWSYC_CTL_CTL_TIMEOUT_EN_SHIFT                      28
#define    CAM_CAMISP_HWSYC_CTL_CTL_TIMEOUT_EN_MASK                       0x10000000
#define    CAM_CAMISP_HWSYC_CTL_CTL_TIMEOUT_VAL_SHIFT                     16
#define    CAM_CAMISP_HWSYC_CTL_CTL_TIMEOUT_VAL_MASK                      0x0FFF0000
#define    CAM_CAMISP_HWSYC_CTL_CTL_LINE_LENGTHMEM_SHIFT                  0
#define    CAM_CAMISP_HWSYC_CTL_CTL_LINE_LENGTHMEM_MASK                   0x0000FFFF

#define CAM_CAMISP_HWSYC_STA_OFFSET                                       0x00000338
#define CAM_CAMISP_HWSYC_STA_TYPE                                         UInt32
#define CAM_CAMISP_HWSYC_STA_RESERVED_MASK                                0x00000000
#define    CAM_CAMISP_HWSYC_STA_LINE_DONE1_CNT_SHIFT                      16
#define    CAM_CAMISP_HWSYC_STA_LINE_DONE1_CNT_MASK                       0xFFFF0000
#define    CAM_CAMISP_HWSYC_STA_LINE_DONE_CNT_SHIFT                       0
#define    CAM_CAMISP_HWSYC_STA_LINE_DONE_CNT_MASK                        0x0000FFFF

#define CAM_CAMISP_MISC_OFFSET                                            0x0000033C
#define CAM_CAMISP_MISC_TYPE                                              UInt32
#define CAM_CAMISP_MISC_RESERVED_MASK                                     0x00000000
#define    CAM_CAMISP_MISC_CTL_CAM_ISP_RES_SHIFT                          12
#define    CAM_CAMISP_MISC_CTL_CAM_ISP_RES_MASK                           0xFFFFF000
#define    CAM_CAMISP_MISC_CTL_FRAME_HT_SONY_SHIFT                        0
#define    CAM_CAMISP_MISC_CTL_FRAME_HT_SONY_MASK                         0x00000FFF

#define CAM_CAMDBG_OFFSET                                                 0x00000340
#define CAM_CAMDBG_TYPE                                                   UInt32
#define CAM_CAMDBG_RESERVED_MASK                                          0xFFFFFFF8
#define    CAM_CAMDBG_NO_CHAN_INT_EN_SHIFT                                2
#define    CAM_CAMDBG_NO_CHAN_INT_EN_MASK                                 0x00000004
#define    CAM_CAMDBG_WRAP_MID_BURST_SHIFT                                1
#define    CAM_CAMDBG_WRAP_MID_BURST_MASK                                 0x00000002
#define    CAM_CAMDBG_SEND_WHOLE_FRAMES_SHIFT                             0
#define    CAM_CAMDBG_SEND_WHOLE_FRAMES_MASK                              0x00000001

#define CAM_CAMIDI2_OFFSET                                                0x00000360
#define CAM_CAMIDI2_TYPE                                                  UInt32
#define CAM_CAMIDI2_RESERVED_MASK                                         0x00000000
#define    CAM_CAMIDI2_IDI11_SHIFT                                        24
#define    CAM_CAMIDI2_IDI11_MASK                                         0xFF000000
#define    CAM_CAMIDI2_IDI10_SHIFT                                        16
#define    CAM_CAMIDI2_IDI10_MASK                                         0x00FF0000
#define    CAM_CAMIDI2_IDI9_SHIFT                                         8
#define    CAM_CAMIDI2_IDI9_MASK                                          0x0000FF00
#define    CAM_CAMIDI2_IDI8_SHIFT                                         0
#define    CAM_CAMIDI2_IDI8_MASK                                          0x000000FF

#define CAM_CAMIDI3_OFFSET                                                0x00000364
#define CAM_CAMIDI3_TYPE                                                  UInt32
#define CAM_CAMIDI3_RESERVED_MASK                                         0x00000000
#define    CAM_CAMIDI3_IDI15_SHIFT                                        24
#define    CAM_CAMIDI3_IDI15_MASK                                         0xFF000000
#define    CAM_CAMIDI3_IDI14_SHIFT                                        16
#define    CAM_CAMIDI3_IDI14_MASK                                         0x00FF0000
#define    CAM_CAMIDI3_IDI13_SHIFT                                        8
#define    CAM_CAMIDI3_IDI13_MASK                                         0x0000FF00
#define    CAM_CAMIDI3_IDI12_SHIFT                                        0
#define    CAM_CAMIDI3_IDI12_MASK                                         0x000000FF

#define CAM_CAMVER_OFFSET                                                 0x00000408
#define CAM_CAMVER_TYPE                                                   UInt32
#define CAM_CAMVER_RESERVED_MASK                                          0x00000000
#define    CAM_CAMVER_UP_VER_SHIFT                                        16
#define    CAM_CAMVER_UP_VER_MASK                                         0xFFFF0000
#define    CAM_CAMVER_LOW_VER_SHIFT                                       0
#define    CAM_CAMVER_LOW_VER_MASK                                        0x0000FFFF

#define CAM_CAMFET_OFFSET                                                 0x0000040C
#define CAM_CAMFET_TYPE                                                   UInt32
#define CAM_CAMFET_RESERVED_MASK                                          0x00E00F00
#define    CAM_CAMFET_AXI_ID_SHIFT                                        24
#define    CAM_CAMFET_AXI_ID_MASK                                         0xFF000000
#define    CAM_CAMFET_SCALAR_SHIFT                                        20
#define    CAM_CAMFET_SCALAR_MASK                                         0x00100000
#define    CAM_CAMFET_PIX_CORR_SHIFT                                      19
#define    CAM_CAMFET_PIX_CORR_MASK                                       0x00080000
#define    CAM_CAMFET_CPI_SHIFT                                           18
#define    CAM_CAMFET_CPI_MASK                                            0x00040000
#define    CAM_CAMFET_CCP2_SHIFT                                          17
#define    CAM_CAMFET_CCP2_MASK                                           0x00020000
#define    CAM_CAMFET_CSI2_SHIFT                                          16
#define    CAM_CAMFET_CSI2_MASK                                           0x00010000
#define    CAM_CAMFET_CAM_LANES_SHIFT                                     12
#define    CAM_CAMFET_CAM_LANES_MASK                                      0x0000F000
#define    CAM_CAMFET_FET_VER_SHIFT                                       0
#define    CAM_CAMFET_FET_VER_MASK                                        0x000000FF

#define CAM_CAMWRAP_OFFSET                                                0x00000410
#define CAM_CAMWRAP_TYPE                                                  UInt32
#define CAM_CAMWRAP_RESERVED_MASK                                         0xFFFF0000
#define    CAM_CAMWRAP_WRAP_SHIFT                                         0
#define    CAM_CAMWRAP_WRAP_MASK                                          0x0000FFFF

#define CAM_DBG_PIX_CMP_FIF_OFFSET                                        0x00000460
#define CAM_DBG_PIX_CMP_FIF_TYPE                                          UInt32
#define CAM_DBG_PIX_CMP_FIF_RESERVED_MASK                                 0x00000000
#define    CAM_DBG_PIX_CMP_FIF_PIX_CMP_FIF_SHIFT                          0
#define    CAM_DBG_PIX_CMP_FIF_PIX_CMP_FIF_MASK                           0xFFFFFFFF

#define CAM_DBG_PIX_RET_UPK_OFFSET                                        0x00000464
#define CAM_DBG_PIX_RET_UPK_TYPE                                          UInt32
#define CAM_DBG_PIX_RET_UPK_RESERVED_MASK                                 0x00000000
#define    CAM_DBG_PIX_RET_UPK_PIX_RET_UPK_SHIFT                          0
#define    CAM_DBG_PIX_RET_UPK_PIX_RET_UPK_MASK                           0xFFFFFFFF

#define CAM_DBG_PIX_OPE_DPM_OFFSET                                        0x00000468
#define CAM_DBG_PIX_OPE_DPM_TYPE                                          UInt32
#define CAM_DBG_PIX_OPE_DPM_RESERVED_MASK                                 0x00000000
#define    CAM_DBG_PIX_OPE_DPM_PIX_OPE_DPM_SHIFT                          0
#define    CAM_DBG_PIX_OPE_DPM_PIX_OPE_DPM_MASK                           0xFFFFFFFF

#define CAM_PRBS_PHYCTL0_OFFSET                                           0x00000470
#define CAM_PRBS_PHYCTL0_TYPE                                             UInt32
#define CAM_PRBS_PHYCTL0_RESERVED_MASK                                    0xFFFF0000
#define    CAM_PRBS_PHYCTL0_PRBS_PHYCTL0_RES_SHIFT                        11
#define    CAM_PRBS_PHYCTL0_PRBS_PHYCTL0_RES_MASK                         0x0000F800
#define    CAM_PRBS_PHYCTL0_HSTX_RESET_CLK_SHIFT                          10
#define    CAM_PRBS_PHYCTL0_HSTX_RESET_CLK_MASK                           0x00000400
#define    CAM_PRBS_PHYCTL0_HSTX_EN_CLK_SHIFT                             9
#define    CAM_PRBS_PHYCTL0_HSTX_EN_CLK_MASK                              0x00000200
#define    CAM_PRBS_PHYCTL0_HSTX_EN_SHIFT                                 5
#define    CAM_PRBS_PHYCTL0_HSTX_EN_MASK                                  0x000001E0
#define    CAM_PRBS_PHYCTL0_TXDDRCLKEN_SHIFT                              4
#define    CAM_PRBS_PHYCTL0_TXDDRCLKEN_MASK                               0x00000010
#define    CAM_PRBS_PHYCTL0_TXDDRCLK2EN_SHIFT                             3
#define    CAM_PRBS_PHYCTL0_TXDDRCLK2EN_MASK                              0x00000008
#define    CAM_PRBS_PHYCTL0_LDORSTB_SHIFT                                 2
#define    CAM_PRBS_PHYCTL0_LDORSTB_MASK                                  0x00000004
#define    CAM_PRBS_PHYCTL0_LDOCNTLEN_SHIFT                               1
#define    CAM_PRBS_PHYCTL0_LDOCNTLEN_MASK                                0x00000002
#define    CAM_PRBS_PHYCTL0_FS2X_EN_CLK_SHIFT                             0
#define    CAM_PRBS_PHYCTL0_FS2X_EN_CLK_MASK                              0x00000001

#define CAM_PRBS_PHYCTL1_OFFSET                                           0x00000474
#define CAM_PRBS_PHYCTL1_TYPE                                             UInt32
#define CAM_PRBS_PHYCTL1_RESERVED_MASK                                    0xFF000000
#define    CAM_PRBS_PHYCTL1_PRBS_PHYCTL1_RES_SHIFT                        20
#define    CAM_PRBS_PHYCTL1_PRBS_PHYCTL1_RES_MASK                         0x00F00000
#define    CAM_PRBS_PHYCTL1_LPTX_EN_CLK_SHIFT                             19
#define    CAM_PRBS_PHYCTL1_LPTX_EN_CLK_MASK                              0x00080000
#define    CAM_PRBS_PHYCTL1_LPTX_DP_IN_CLK_SHIFT                          18
#define    CAM_PRBS_PHYCTL1_LPTX_DP_IN_CLK_MASK                           0x00040000
#define    CAM_PRBS_PHYCTL1_LPTX_DM_IN_CLK_SHIFT                          17
#define    CAM_PRBS_PHYCTL1_LPTX_DM_IN_CLK_MASK                           0x00020000
#define    CAM_PRBS_PHYCTL1_LPTX_EN_SHIFT                                 13
#define    CAM_PRBS_PHYCTL1_LPTX_EN_MASK                                  0x0001E000
#define    CAM_PRBS_PHYCTL1_LPTX_DP_IN_SHIFT                              9
#define    CAM_PRBS_PHYCTL1_LPTX_DP_IN_MASK                               0x00001E00
#define    CAM_PRBS_PHYCTL1_LPTX_DM_IN_SHIFT                              5
#define    CAM_PRBS_PHYCTL1_LPTX_DM_IN_MASK                               0x000001E0
#define    CAM_PRBS_PHYCTL1_LPCD_EN_SHIFT                                 1
#define    CAM_PRBS_PHYCTL1_LPCD_EN_MASK                                  0x0000001E
#define    CAM_PRBS_PHYCTL1_LP_EN_SHIFT                                   0
#define    CAM_PRBS_PHYCTL1_LP_EN_MASK                                    0x00000001

#define CAM_PRBS_LN0_CTL_OFFSET                                           0x00000478
#define CAM_PRBS_LN0_CTL_TYPE                                             UInt32
#define CAM_PRBS_LN0_CTL_RESERVED_MASK                                    0xFE00C000
#define    CAM_PRBS_LN0_CTL_CONST_REG_EN_SHIFT                            24
#define    CAM_PRBS_LN0_CTL_CONST_REG_EN_MASK                             0x01000000
#define    CAM_PRBS_LN0_CTL_CONSTANT_REG_SHIFT                            16
#define    CAM_PRBS_LN0_CTL_CONSTANT_REG_MASK                             0x00FF0000
#define    CAM_PRBS_LN0_CTL_PM_INV_SHIFT                                  13
#define    CAM_PRBS_LN0_CTL_PM_INV_MASK                                   0x00002000
#define    CAM_PRBS_LN0_CTL_PRBS_LN0_CTL_RES_SHIFT                        12
#define    CAM_PRBS_LN0_CTL_PRBS_LN0_CTL_RES_MASK                         0x00001000
#define    CAM_PRBS_LN0_CTL_PM_ORDER_SHIFT                                10
#define    CAM_PRBS_LN0_CTL_PM_ORDER_MASK                                 0x00000C00
#define    CAM_PRBS_LN0_CTL_PM_ERR_CLR_SHIFT                              9
#define    CAM_PRBS_LN0_CTL_PM_ERR_CLR_MASK                               0x00000200
#define    CAM_PRBS_LN0_CTL_PM_ENABLE_SHIFT                               8
#define    CAM_PRBS_LN0_CTL_PM_ENABLE_MASK                                0x00000100
#define    CAM_PRBS_LN0_CTL_PM_WIDTH_SEL_SHIFT                            7
#define    CAM_PRBS_LN0_CTL_PM_WIDTH_SEL_MASK                             0x00000080
#define    CAM_PRBS_LN0_CTL_PM_SOFTRST_N_SHIFT                            6
#define    CAM_PRBS_LN0_CTL_PM_SOFTRST_N_MASK                             0x00000040
#define    CAM_PRBS_LN0_CTL_PG_ORDER_SHIFT                                4
#define    CAM_PRBS_LN0_CTL_PG_ORDER_MASK                                 0x00000030
#define    CAM_PRBS_LN0_CTL_PG_INV_SHIFT                                  3
#define    CAM_PRBS_LN0_CTL_PG_INV_MASK                                   0x00000008
#define    CAM_PRBS_LN0_CTL_PG_ENABLE_SHIFT                               2
#define    CAM_PRBS_LN0_CTL_PG_ENABLE_MASK                                0x00000004
#define    CAM_PRBS_LN0_CTL_PG_WIDTH_SEL_SHIFT                            1
#define    CAM_PRBS_LN0_CTL_PG_WIDTH_SEL_MASK                             0x00000002
#define    CAM_PRBS_LN0_CTL_PG_SOFTRST_N_SHIFT                            0
#define    CAM_PRBS_LN0_CTL_PG_SOFTRST_N_MASK                             0x00000001

#define CAM_PRBS_LN0_ST_OFFSET                                            0x0000047C
#define CAM_PRBS_LN0_ST_TYPE                                              UInt32
#define CAM_PRBS_LN0_ST_RESERVED_MASK                                     0xC000FF0C
#define    CAM_PRBS_LN0_ST_RPBS_ERRORS_SHIFT                              16
#define    CAM_PRBS_LN0_ST_RPBS_ERRORS_MASK                               0x3FFF0000
#define    CAM_PRBS_LN0_ST_PRBS_STATE_SHIFT                               4
#define    CAM_PRBS_LN0_ST_PRBS_STATE_MASK                                0x000000F0
#define    CAM_PRBS_LN0_ST_PRBS_STKY_SHIFT                                1
#define    CAM_PRBS_LN0_ST_PRBS_STKY_MASK                                 0x00000002
#define    CAM_PRBS_LN0_ST_PRBS_LOCK_SHIFT                                0
#define    CAM_PRBS_LN0_ST_PRBS_LOCK_MASK                                 0x00000001

#define CAM_PRBS_LN1_CTL_OFFSET                                           0x00000480
#define CAM_PRBS_LN1_CTL_TYPE                                             UInt32
#define CAM_PRBS_LN1_CTL_RESERVED_MASK                                    0xFE00C000
#define    CAM_PRBS_LN1_CTL_CONST_REG_EN_SHIFT                            24
#define    CAM_PRBS_LN1_CTL_CONST_REG_EN_MASK                             0x01000000
#define    CAM_PRBS_LN1_CTL_CONSTANT_REG_SHIFT                            16
#define    CAM_PRBS_LN1_CTL_CONSTANT_REG_MASK                             0x00FF0000
#define    CAM_PRBS_LN1_CTL_PM_INV_SHIFT                                  13
#define    CAM_PRBS_LN1_CTL_PM_INV_MASK                                   0x00002000
#define    CAM_PRBS_LN1_CTL_PRBS_LN0_CTL_RES_SHIFT                        12
#define    CAM_PRBS_LN1_CTL_PRBS_LN0_CTL_RES_MASK                         0x00001000
#define    CAM_PRBS_LN1_CTL_PM_ORDER_SHIFT                                10
#define    CAM_PRBS_LN1_CTL_PM_ORDER_MASK                                 0x00000C00
#define    CAM_PRBS_LN1_CTL_PM_ERR_CLR_SHIFT                              9
#define    CAM_PRBS_LN1_CTL_PM_ERR_CLR_MASK                               0x00000200
#define    CAM_PRBS_LN1_CTL_PM_ENABLE_SHIFT                               8
#define    CAM_PRBS_LN1_CTL_PM_ENABLE_MASK                                0x00000100
#define    CAM_PRBS_LN1_CTL_PM_WIDTH_SEL_SHIFT                            7
#define    CAM_PRBS_LN1_CTL_PM_WIDTH_SEL_MASK                             0x00000080
#define    CAM_PRBS_LN1_CTL_PM_SOFTRST_N_SHIFT                            6
#define    CAM_PRBS_LN1_CTL_PM_SOFTRST_N_MASK                             0x00000040
#define    CAM_PRBS_LN1_CTL_PG_ORDER_SHIFT                                4
#define    CAM_PRBS_LN1_CTL_PG_ORDER_MASK                                 0x00000030
#define    CAM_PRBS_LN1_CTL_PG_INV_SHIFT                                  3
#define    CAM_PRBS_LN1_CTL_PG_INV_MASK                                   0x00000008
#define    CAM_PRBS_LN1_CTL_PG_ENABLE_SHIFT                               2
#define    CAM_PRBS_LN1_CTL_PG_ENABLE_MASK                                0x00000004
#define    CAM_PRBS_LN1_CTL_PG_WIDTH_SEL_SHIFT                            1
#define    CAM_PRBS_LN1_CTL_PG_WIDTH_SEL_MASK                             0x00000002
#define    CAM_PRBS_LN1_CTL_PG_SOFTRST_N_SHIFT                            0
#define    CAM_PRBS_LN1_CTL_PG_SOFTRST_N_MASK                             0x00000001

#define CAM_PRBS_LN1_ST_OFFSET                                            0x00000484
#define CAM_PRBS_LN1_ST_TYPE                                              UInt32
#define CAM_PRBS_LN1_ST_RESERVED_MASK                                     0xC000FF0C
#define    CAM_PRBS_LN1_ST_RPBS_ERRORS_SHIFT                              16
#define    CAM_PRBS_LN1_ST_RPBS_ERRORS_MASK                               0x3FFF0000
#define    CAM_PRBS_LN1_ST_PRBS_STATE_SHIFT                               4
#define    CAM_PRBS_LN1_ST_PRBS_STATE_MASK                                0x000000F0
#define    CAM_PRBS_LN1_ST_PRBS_STKY_SHIFT                                1
#define    CAM_PRBS_LN1_ST_PRBS_STKY_MASK                                 0x00000002
#define    CAM_PRBS_LN1_ST_PRBS_LOCK_SHIFT                                0
#define    CAM_PRBS_LN1_ST_PRBS_LOCK_MASK                                 0x00000001

#define CAM_PRBS_LN2_CTL_OFFSET                                           0x00000488
#define CAM_PRBS_LN2_CTL_TYPE                                             UInt32
#define CAM_PRBS_LN2_CTL_RESERVED_MASK                                    0xFE00C000
#define    CAM_PRBS_LN2_CTL_CONST_REG_EN_SHIFT                            24
#define    CAM_PRBS_LN2_CTL_CONST_REG_EN_MASK                             0x01000000
#define    CAM_PRBS_LN2_CTL_CONSTANT_REG_SHIFT                            16
#define    CAM_PRBS_LN2_CTL_CONSTANT_REG_MASK                             0x00FF0000
#define    CAM_PRBS_LN2_CTL_PM_INV_SHIFT                                  13
#define    CAM_PRBS_LN2_CTL_PM_INV_MASK                                   0x00002000
#define    CAM_PRBS_LN2_CTL_PRBS_LN0_CTL_RES_SHIFT                        12
#define    CAM_PRBS_LN2_CTL_PRBS_LN0_CTL_RES_MASK                         0x00001000
#define    CAM_PRBS_LN2_CTL_PM_ORDER_SHIFT                                10
#define    CAM_PRBS_LN2_CTL_PM_ORDER_MASK                                 0x00000C00
#define    CAM_PRBS_LN2_CTL_PM_ERR_CLR_SHIFT                              9
#define    CAM_PRBS_LN2_CTL_PM_ERR_CLR_MASK                               0x00000200
#define    CAM_PRBS_LN2_CTL_PM_ENABLE_SHIFT                               8
#define    CAM_PRBS_LN2_CTL_PM_ENABLE_MASK                                0x00000100
#define    CAM_PRBS_LN2_CTL_PM_WIDTH_SEL_SHIFT                            7
#define    CAM_PRBS_LN2_CTL_PM_WIDTH_SEL_MASK                             0x00000080
#define    CAM_PRBS_LN2_CTL_PM_SOFTRST_N_SHIFT                            6
#define    CAM_PRBS_LN2_CTL_PM_SOFTRST_N_MASK                             0x00000040
#define    CAM_PRBS_LN2_CTL_PG_ORDER_SHIFT                                4
#define    CAM_PRBS_LN2_CTL_PG_ORDER_MASK                                 0x00000030
#define    CAM_PRBS_LN2_CTL_PG_INV_SHIFT                                  3
#define    CAM_PRBS_LN2_CTL_PG_INV_MASK                                   0x00000008
#define    CAM_PRBS_LN2_CTL_PG_ENABLE_SHIFT                               2
#define    CAM_PRBS_LN2_CTL_PG_ENABLE_MASK                                0x00000004
#define    CAM_PRBS_LN2_CTL_PG_WIDTH_SEL_SHIFT                            1
#define    CAM_PRBS_LN2_CTL_PG_WIDTH_SEL_MASK                             0x00000002
#define    CAM_PRBS_LN2_CTL_PG_SOFTRST_N_SHIFT                            0
#define    CAM_PRBS_LN2_CTL_PG_SOFTRST_N_MASK                             0x00000001

#define CAM_PRBS_LN2_ST_OFFSET                                            0x0000048C
#define CAM_PRBS_LN2_ST_TYPE                                              UInt32
#define CAM_PRBS_LN2_ST_RESERVED_MASK                                     0xC000FF0C
#define    CAM_PRBS_LN2_ST_RPBS_ERRORS_SHIFT                              16
#define    CAM_PRBS_LN2_ST_RPBS_ERRORS_MASK                               0x3FFF0000
#define    CAM_PRBS_LN2_ST_PRBS_STATE_SHIFT                               4
#define    CAM_PRBS_LN2_ST_PRBS_STATE_MASK                                0x000000F0
#define    CAM_PRBS_LN2_ST_PRBS_STKY_SHIFT                                1
#define    CAM_PRBS_LN2_ST_PRBS_STKY_MASK                                 0x00000002
#define    CAM_PRBS_LN2_ST_PRBS_LOCK_SHIFT                                0
#define    CAM_PRBS_LN2_ST_PRBS_LOCK_MASK                                 0x00000001

#define CAM_PRBS_LN3_CTL_OFFSET                                           0x00000490
#define CAM_PRBS_LN3_CTL_TYPE                                             UInt32
#define CAM_PRBS_LN3_CTL_RESERVED_MASK                                    0xFE00C000
#define    CAM_PRBS_LN3_CTL_CONST_REG_EN_SHIFT                            24
#define    CAM_PRBS_LN3_CTL_CONST_REG_EN_MASK                             0x01000000
#define    CAM_PRBS_LN3_CTL_CONSTANT_REG_SHIFT                            16
#define    CAM_PRBS_LN3_CTL_CONSTANT_REG_MASK                             0x00FF0000
#define    CAM_PRBS_LN3_CTL_PM_INV_SHIFT                                  13
#define    CAM_PRBS_LN3_CTL_PM_INV_MASK                                   0x00002000
#define    CAM_PRBS_LN3_CTL_PRBS_LN0_CTL_RES_SHIFT                        12
#define    CAM_PRBS_LN3_CTL_PRBS_LN0_CTL_RES_MASK                         0x00001000
#define    CAM_PRBS_LN3_CTL_PM_ORDER_SHIFT                                10
#define    CAM_PRBS_LN3_CTL_PM_ORDER_MASK                                 0x00000C00
#define    CAM_PRBS_LN3_CTL_PM_ERR_CLR_SHIFT                              9
#define    CAM_PRBS_LN3_CTL_PM_ERR_CLR_MASK                               0x00000200
#define    CAM_PRBS_LN3_CTL_PM_ENABLE_SHIFT                               8
#define    CAM_PRBS_LN3_CTL_PM_ENABLE_MASK                                0x00000100
#define    CAM_PRBS_LN3_CTL_PM_WIDTH_SEL_SHIFT                            7
#define    CAM_PRBS_LN3_CTL_PM_WIDTH_SEL_MASK                             0x00000080
#define    CAM_PRBS_LN3_CTL_PM_SOFTRST_N_SHIFT                            6
#define    CAM_PRBS_LN3_CTL_PM_SOFTRST_N_MASK                             0x00000040
#define    CAM_PRBS_LN3_CTL_PG_ORDER_SHIFT                                4
#define    CAM_PRBS_LN3_CTL_PG_ORDER_MASK                                 0x00000030
#define    CAM_PRBS_LN3_CTL_PG_INV_SHIFT                                  3
#define    CAM_PRBS_LN3_CTL_PG_INV_MASK                                   0x00000008
#define    CAM_PRBS_LN3_CTL_PG_ENABLE_SHIFT                               2
#define    CAM_PRBS_LN3_CTL_PG_ENABLE_MASK                                0x00000004
#define    CAM_PRBS_LN3_CTL_PG_WIDTH_SEL_SHIFT                            1
#define    CAM_PRBS_LN3_CTL_PG_WIDTH_SEL_MASK                             0x00000002
#define    CAM_PRBS_LN3_CTL_PG_SOFTRST_N_SHIFT                            0
#define    CAM_PRBS_LN3_CTL_PG_SOFTRST_N_MASK                             0x00000001

#define CAM_PRBS_LN3_ST_OFFSET                                            0x00000494
#define CAM_PRBS_LN3_ST_TYPE                                              UInt32
#define CAM_PRBS_LN3_ST_RESERVED_MASK                                     0xC000FF0C
#define    CAM_PRBS_LN3_ST_RPBS_ERRORS_SHIFT                              16
#define    CAM_PRBS_LN3_ST_RPBS_ERRORS_MASK                               0x3FFF0000
#define    CAM_PRBS_LN3_ST_PRBS_STATE_SHIFT                               4
#define    CAM_PRBS_LN3_ST_PRBS_STATE_MASK                                0x000000F0
#define    CAM_PRBS_LN3_ST_PRBS_STKY_SHIFT                                1
#define    CAM_PRBS_LN3_ST_PRBS_STKY_MASK                                 0x00000002
#define    CAM_PRBS_LN3_ST_PRBS_LOCK_SHIFT                                0
#define    CAM_PRBS_LN3_ST_PRBS_LOCK_MASK                                 0x00000001

#define CAM_CAMCPIS_OFFSET                                                0x00000500
#define CAM_CAMCPIS_TYPE                                                  UInt32
#define CAM_CAMCPIS_RESERVED_MASK                                         0x3FFFFFC8
#define    CAM_CAMCPIS_REGF_SHIFT                                         31
#define    CAM_CAMCPIS_REGF_MASK                                          0x80000000
#define    CAM_CAMCPIS_HERROR_SHIFT                                       30
#define    CAM_CAMCPIS_HERROR_MASK                                        0x40000000
#define    CAM_CAMCPIS_SOFTRST_SHIFT                                      5
#define    CAM_CAMCPIS_SOFTRST_MASK                                       0x00000020
#define    CAM_CAMCPIS_DISACT_SHIFT                                       4
#define    CAM_CAMCPIS_DISACT_MASK                                        0x00000010
#define    CAM_CAMCPIS_CAPT_SHIFT                                         2
#define    CAM_CAMCPIS_CAPT_MASK                                          0x00000004
#define    CAM_CAMCPIS_ACT_SHIFT                                          1
#define    CAM_CAMCPIS_ACT_MASK                                           0x00000002
#define    CAM_CAMCPIS_ENB_SHIFT                                          0
#define    CAM_CAMCPIS_ENB_MASK                                           0x00000001

#define CAM_CAMCPIR_OFFSET                                                0x00000504
#define CAM_CAMCPIR_TYPE                                                  UInt32
#define CAM_CAMCPIR_RESERVED_MASK                                         0xFFFFE000
#define    CAM_CAMCPIR_DATASHIFT_SHIFT                                    9
#define    CAM_CAMCPIR_DATASHIFT_MASK                                     0x00001E00
#define    CAM_CAMCPIR_DWID_SHIFT                                         7
#define    CAM_CAMCPIR_DWID_MASK                                          0x00000180
#define    CAM_CAMCPIR_HSYNC_SHIFT                                        6
#define    CAM_CAMCPIR_HSYNC_MASK                                         0x00000040
#define    CAM_CAMCPIR_VSYNC_SHIFT                                        5
#define    CAM_CAMCPIR_VSYNC_MASK                                         0x00000020
#define    CAM_CAMCPIR_DRSYNC_SHIFT                                       4
#define    CAM_CAMCPIR_DRSYNC_MASK                                        0x00000010
#define    CAM_CAMCPIR_HSRM_SHIFT                                         3
#define    CAM_CAMCPIR_HSRM_MASK                                          0x00000008
#define    CAM_CAMCPIR_VSRM_SHIFT                                         2
#define    CAM_CAMCPIR_VSRM_MASK                                          0x00000004
#define    CAM_CAMCPIR_HSAL_SHIFT                                         1
#define    CAM_CAMCPIR_HSAL_MASK                                          0x00000002
#define    CAM_CAMCPIR_VSAL_SHIFT                                         0
#define    CAM_CAMCPIR_VSAL_MASK                                          0x00000001

#define CAM_CAMCPIF_OFFSET                                                0x00000508
#define CAM_CAMCPIF_TYPE                                                  UInt32
#define CAM_CAMCPIF_RESERVED_MASK                                         0xF0008000
#define    CAM_CAMCPIF_FIELD_BIT_SHIFT                                    24
#define    CAM_CAMCPIF_FIELD_BIT_MASK                                     0x0F000000
#define    CAM_CAMCPIF_HSYNC_BIT_SHIFT                                    20
#define    CAM_CAMCPIF_HSYNC_BIT_MASK                                     0x00F00000
#define    CAM_CAMCPIF_VSYNC_BIT_SHIFT                                    16
#define    CAM_CAMCPIF_VSYNC_BIT_MASK                                     0x000F0000
#define    CAM_CAMCPIF_SHIFT_SYNC_SHIFT                                   13
#define    CAM_CAMCPIF_SHIFT_SYNC_MASK                                    0x00006000
#define    CAM_CAMCPIF_EMB_SYNCSHIFT_EN_SHIFT                             12
#define    CAM_CAMCPIF_EMB_SYNCSHIFT_EN_MASK                              0x00001000
#define    CAM_CAMCPIF_FDETECT_SHIFT                                      11
#define    CAM_CAMCPIF_FDETECT_MASK                                       0x00000800
#define    CAM_CAMCPIF_STATE_SHIFT                                        8
#define    CAM_CAMCPIF_STATE_MASK                                         0x00000700
#define    CAM_CAMCPIF_SCMODE_SHIFT                                       7
#define    CAM_CAMCPIF_SCMODE_MASK                                        0x00000080
#define    CAM_CAMCPIF_FMODE_SHIFT                                        5
#define    CAM_CAMCPIF_FMODE_MASK                                         0x00000060
#define    CAM_CAMCPIF_HMODE_SHIFT                                        3
#define    CAM_CAMCPIF_HMODE_MASK                                         0x00000018
#define    CAM_CAMCPIF_VMODE_SHIFT                                        1
#define    CAM_CAMCPIF_VMODE_MASK                                         0x00000006
#define    CAM_CAMCPIF_SMODE_SHIFT                                        0
#define    CAM_CAMCPIF_SMODE_MASK                                         0x00000001

#define CAM_CAMCPIWM_OFFSET                                               0x00000520
#define CAM_CAMCPIWM_TYPE                                                 UInt32
#define CAM_CAMCPIWM_RESERVED_MASK                                        0x0000FF00
#define    CAM_CAMCPIWM_HLINE_MIN_SHIFT                                   16
#define    CAM_CAMCPIWM_HLINE_MIN_MASK                                    0xFFFF0000
#define    CAM_CAMCPIWM_FRM_CNT_SHIFT                                     0
#define    CAM_CAMCPIWM_FRM_CNT_MASK                                      0x000000FF

#define CAM_CAMCPIB_OFFSET                                                0x00000524
#define CAM_CAMCPIB_TYPE                                                  UInt32
#define CAM_CAMCPIB_RESERVED_MASK                                         0xFFFFFFF0
#define    CAM_CAMCPIB_CHANCODE_SHIFT                                     3
#define    CAM_CAMCPIB_CHANCODE_MASK                                      0x00000008
#define    CAM_CAMCPIB_LINECODE_SHIFT                                     0
#define    CAM_CAMCPIB_LINECODE_MASK                                      0x00000007

#define CAM_CAMCPIVCM_OFFSET                                              0x00000530
#define CAM_CAMCPIVCM_TYPE                                                UInt32
#define CAM_CAMCPIVCM_RESERVED_MASK                                       0xFC00FC00
#define    CAM_CAMCPIVCM_VSYNC_FALL_CNT_MAX_SHIFT                         16
#define    CAM_CAMCPIVCM_VSYNC_FALL_CNT_MAX_MASK                          0x03FF0000
#define    CAM_CAMCPIVCM_VSYNC_RISE_CNT_MAX_SHIFT                         0
#define    CAM_CAMCPIVCM_VSYNC_RISE_CNT_MAX_MASK                          0x000003FF

#define CAM_CAMCPIHCM_OFFSET                                              0x00000534
#define CAM_CAMCPIHCM_TYPE                                                UInt32
#define CAM_CAMCPIHCM_RESERVED_MASK                                       0xFC00FC00
#define    CAM_CAMCPIHCM_HSYNC_FALL_CNT_MAX_SHIFT                         16
#define    CAM_CAMCPIHCM_HSYNC_FALL_CNT_MAX_MASK                          0x03FF0000
#define    CAM_CAMCPIHCM_HSYNC_RISE_CNT_MAX_SHIFT                         0
#define    CAM_CAMCPIHCM_HSYNC_RISE_CNT_MAX_MASK                          0x000003FF

#define CAM_CAMCPILECM_OFFSET                                             0x00000538
#define CAM_CAMCPILECM_TYPE                                               UInt32
#define CAM_CAMCPILECM_RESERVED_MASK                                      0xFFFFFFC0
#define    CAM_CAMCPILECM_LE_DLY_CNT_MAX_SHIFT                            0
#define    CAM_CAMCPILECM_LE_DLY_CNT_MAX_MASK                             0x0000003F

#define CAM_CAMHISPI_CTL_OFFSET                                           0x00000600
#define CAM_CAMHISPI_CTL_TYPE                                             UInt32
#define CAM_CAMHISPI_CTL_RESERVED_MASK                                    0x0000F800
#define    CAM_CAMHISPI_CTL_HISPI_CRC_INIT_VAL_SHIFT                      16
#define    CAM_CAMHISPI_CTL_HISPI_CRC_INIT_VAL_MASK                       0xFFFF0000
#define    CAM_CAMHISPI_CTL_HISPI_CTL_RES_SHIFT                           9
#define    CAM_CAMHISPI_CTL_HISPI_CTL_RES_MASK                            0x00000600
#define    CAM_CAMHISPI_CTL_HISPI_CRC_OP_REFLC_SHIFT                      8
#define    CAM_CAMHISPI_CTL_HISPI_CRC_OP_REFLC_MASK                       0x00000100
#define    CAM_CAMHISPI_CTL_HISPI_CRC_IP_REFLC_SHIFT                      7
#define    CAM_CAMHISPI_CTL_HISPI_CRC_IP_REFLC_MASK                       0x00000080
#define    CAM_CAMHISPI_CTL_HISPI_CRC_INT_EN_SHIFT                        6
#define    CAM_CAMHISPI_CTL_HISPI_CRC_INT_EN_MASK                         0x00000040
#define    CAM_CAMHISPI_CTL_HISPI_LANE_MSB_COMES_FIRST_SHIFT              5
#define    CAM_CAMHISPI_CTL_HISPI_LANE_MSB_COMES_FIRST_MASK               0x00000020
#define    CAM_CAMHISPI_CTL_HISPI_CRC_EN_SHIFT                            4
#define    CAM_CAMHISPI_CTL_HISPI_CRC_EN_MASK                             0x00000010
#define    CAM_CAMHISPI_CTL_HISPI_FILLER_EN_SHIFT                         3
#define    CAM_CAMHISPI_CTL_HISPI_FILLER_EN_MASK                          0x00000008
#define    CAM_CAMHISPI_CTL_HISPI_MODE_SHIFT                              0
#define    CAM_CAMHISPI_CTL_HISPI_MODE_MASK                               0x00000007

#define CAM_CAMHISPI_STA_OFFSET                                           0x00000604
#define CAM_CAMHISPI_STA_TYPE                                             UInt32
#define CAM_CAMHISPI_STA_RESERVED_MASK                                    0xFFFFF000
#define    CAM_CAMHISPI_STA_HISPI_STA_RES_SHIFT                           4
#define    CAM_CAMHISPI_STA_HISPI_STA_RES_MASK                            0x00000FF0
#define    CAM_CAMHISPI_STA_HISPI_CRC_ERR_LANES_SHIFT                     0
#define    CAM_CAMHISPI_STA_HISPI_CRC_ERR_LANES_MASK                      0x0000000F

#define CAM_HISPI_DBG1_OFFSET                                             0x00000620
#define CAM_HISPI_DBG1_TYPE                                               UInt32
#define CAM_HISPI_DBG1_RESERVED_MASK                                      0x00000000
#define    CAM_HISPI_DBG1_HISPI_DBG_REG_SHIFT                             0
#define    CAM_HISPI_DBG1_HISPI_DBG_REG_MASK                              0xFFFFFFFF

#define CAM_HISPI_DBG_FIFO_0_OFFSET                                       0x00000624
#define CAM_HISPI_DBG_FIFO_0_TYPE                                         UInt32
#define CAM_HISPI_DBG_FIFO_0_RESERVED_MASK                                0x00000000
#define    CAM_HISPI_DBG_FIFO_0_HISPI_DBG_FIFO_LANE_0_SHIFT               0
#define    CAM_HISPI_DBG_FIFO_0_HISPI_DBG_FIFO_LANE_0_MASK                0xFFFFFFFF

#define CAM_HISPI_DBG_FIFO_1_OFFSET                                       0x00000628
#define CAM_HISPI_DBG_FIFO_1_TYPE                                         UInt32
#define CAM_HISPI_DBG_FIFO_1_RESERVED_MASK                                0x00000000
#define    CAM_HISPI_DBG_FIFO_1_HISPI_DBG_FIFO_LANE_1_SHIFT               0
#define    CAM_HISPI_DBG_FIFO_1_HISPI_DBG_FIFO_LANE_1_MASK                0xFFFFFFFF

#define CAM_HISPI_DBG_FIFO_2_OFFSET                                       0x0000062C
#define CAM_HISPI_DBG_FIFO_2_TYPE                                         UInt32
#define CAM_HISPI_DBG_FIFO_2_RESERVED_MASK                                0x00000000
#define    CAM_HISPI_DBG_FIFO_2_HISPI_DBG_FIFO_LANE_2_SHIFT               0
#define    CAM_HISPI_DBG_FIFO_2_HISPI_DBG_FIFO_LANE_2_MASK                0xFFFFFFFF

#define CAM_HISPI_DBG_FIFO_3_OFFSET                                       0x00000630
#define CAM_HISPI_DBG_FIFO_3_TYPE                                         UInt32
#define CAM_HISPI_DBG_FIFO_3_RESERVED_MASK                                0x00000000
#define    CAM_HISPI_DBG_FIFO_3_HISPI_DBG_FIFO_LANE_3_SHIFT               0
#define    CAM_HISPI_DBG_FIFO_3_HISPI_DBG_FIFO_LANE_3_MASK                0xFFFFFFFF

#define CAM_DBG_PORT_CTL_OFFSET                                           0x000006C0
#define CAM_DBG_PORT_CTL_TYPE                                             UInt32
#define CAM_DBG_PORT_CTL_RESERVED_MASK                                    0xFFFFF800
#define    CAM_DBG_PORT_CTL_DBG_MUX_SEL_SRC_SHIFT                         10
#define    CAM_DBG_PORT_CTL_DBG_MUX_SEL_SRC_MASK                          0x00000400
#define    CAM_DBG_PORT_CTL_DBG_MUX_SEL_SHIFT                             0
#define    CAM_DBG_PORT_CTL_DBG_MUX_SEL_MASK                              0x000003FF

#define CAM_DBG_PORT_STAT_OFFSET                                          0x000006C4
#define CAM_DBG_PORT_STAT_TYPE                                            UInt32
#define CAM_DBG_PORT_STAT_RESERVED_MASK                                   0x00000000
#define    CAM_DBG_PORT_STAT_DBG_BUS_VAL_SHIFT                            0
#define    CAM_DBG_PORT_STAT_DBG_BUS_VAL_MASK                             0xFFFFFFFF

#define CAM_CAM_VC5_TGCTL_OFFSET                                          0x00000700
#define CAM_CAM_VC5_TGCTL_TYPE                                            UInt32
#define CAM_CAM_VC5_TGCTL_RESERVED_MASK                                   0x0000C000
#define    CAM_CAM_VC5_TGCTL_PRE_B_SHIFT                                  16
#define    CAM_CAM_VC5_TGCTL_PRE_B_MASK                                   0xFFFF0000
#define    CAM_CAM_VC5_TGCTL_MODE_SHIFT                                   13
#define    CAM_CAM_VC5_TGCTL_MODE_MASK                                    0x00002000
#define    CAM_CAM_VC5_TGCTL_CFG_BYPASS_SHIFT                             12
#define    CAM_CAM_VC5_TGCTL_CFG_BYPASS_MASK                              0x00001000
#define    CAM_CAM_VC5_TGCTL_BPP_SHIFT                                    8
#define    CAM_CAM_VC5_TGCTL_BPP_MASK                                     0x00000F00
#define    CAM_CAM_VC5_TGCTL_CHAN_SHIFT                                   7
#define    CAM_CAM_VC5_TGCTL_CHAN_MASK                                    0x00000080
#define    CAM_CAM_VC5_TGCTL_ED_SHIFT                                     6
#define    CAM_CAM_VC5_TGCTL_ED_MASK                                      0x00000040
#define    CAM_CAM_VC5_TGCTL_BAYER_SHIFT                                  4
#define    CAM_CAM_VC5_TGCTL_BAYER_MASK                                   0x00000030
#define    CAM_CAM_VC5_TGCTL_DIS_EN_SHIFT                                 3
#define    CAM_CAM_VC5_TGCTL_DIS_EN_MASK                                  0x00000008
#define    CAM_CAM_VC5_TGCTL_TR_EN_SHIFT                                  2
#define    CAM_CAM_VC5_TGCTL_TR_EN_MASK                                   0x00000004
#define    CAM_CAM_VC5_TGCTL_START_SHIFT                                  1
#define    CAM_CAM_VC5_TGCTL_START_MASK                                   0x00000002
#define    CAM_CAM_VC5_TGCTL_TG_ENABLE_SHIFT                              0
#define    CAM_CAM_VC5_TGCTL_TG_ENABLE_MASK                               0x00000001

#define CAM_CAM_VC5_TGISZ_OFFSET                                          0x00000704
#define CAM_CAM_VC5_TGISZ_TYPE                                            UInt32
#define CAM_CAM_VC5_TGISZ_RESERVED_MASK                                   0x00000000
#define    CAM_CAM_VC5_TGISZ_TG_HEIGHT_SHIFT                              16
#define    CAM_CAM_VC5_TGISZ_TG_HEIGHT_MASK                               0xFFFF0000
#define    CAM_CAM_VC5_TGISZ_TG_WIDTH_SHIFT                               0
#define    CAM_CAM_VC5_TGISZ_TG_WIDTH_MASK                                0x0000FFFF

#define CAM_CAM_VC5_TGBL_OFFSET                                           0x00000708
#define CAM_CAM_VC5_TGBL_TYPE                                             UInt32
#define CAM_CAM_VC5_TGBL_RESERVED_MASK                                    0x00000000
#define    CAM_CAM_VC5_TGBL_VBLANK_SHIFT                                  16
#define    CAM_CAM_VC5_TGBL_VBLANK_MASK                                   0xFFFF0000
#define    CAM_CAM_VC5_TGBL_HBLANK_SHIFT                                  0
#define    CAM_CAM_VC5_TGBL_HBLANK_MASK                                   0x0000FFFF

#define CAM_CAM_VC5_TGBSZ_OFFSET                                          0x0000070C
#define CAM_CAM_VC5_TGBSZ_TYPE                                            UInt32
#define CAM_CAM_VC5_TGBSZ_RESERVED_MASK                                   0xFFFFFF00
#define    CAM_CAM_VC5_TGBSZ_BYSIZE_SHIFT                                 4
#define    CAM_CAM_VC5_TGBSZ_BYSIZE_MASK                                  0x000000F0
#define    CAM_CAM_VC5_TGBSZ_BXSIZE_SHIFT                                 0
#define    CAM_CAM_VC5_TGBSZ_BXSIZE_MASK                                  0x0000000F

#define CAM_CAM_VC5_TGLFS_OFFSET                                          0x00000710
#define CAM_CAM_VC5_TGLFS_TYPE                                            UInt32
#define CAM_CAM_VC5_TGLFS_RESERVED_MASK                                   0xFF000000
#define    CAM_CAM_VC5_TGLFS_VALUE_SHIFT                                  0
#define    CAM_CAM_VC5_TGLFS_VALUE_MASK                                   0x00FFFFFF

#define CAM_CAM_VC5_TGPS_OFFSET                                           0x00000714
#define CAM_CAM_VC5_TGPS_TYPE                                             UInt32
#define CAM_CAM_VC5_TGPS_RESERVED_MASK                                    0x0000FFFF
#define    CAM_CAM_VC5_TGPS_B_PRESCALE_SHIFT                              16
#define    CAM_CAM_VC5_TGPS_B_PRESCALE_MASK                               0xFFFF0000

#define CAM_CAM_VC5_TGISTAT_OFFSET                                        0x00000718
#define CAM_CAM_VC5_TGISTAT_TYPE                                          UInt32
#define CAM_CAM_VC5_TGISTAT_RESERVED_MASK                                 0xFFFFFFE0
#define    CAM_CAM_VC5_TGISTAT_FIFO_FULL_SHIFT                            4
#define    CAM_CAM_VC5_TGISTAT_FIFO_FULL_MASK                             0x00000010
#define    CAM_CAM_VC5_TGISTAT_RESP_DECERR_SHIFT                          3
#define    CAM_CAM_VC5_TGISTAT_RESP_DECERR_MASK                           0x00000008
#define    CAM_CAM_VC5_TGISTAT_RESP_SLVERR_SHIFT                          2
#define    CAM_CAM_VC5_TGISTAT_RESP_SLVERR_MASK                           0x00000004
#define    CAM_CAM_VC5_TGISTAT_RESP_EXOKAY_SHIFT                          1
#define    CAM_CAM_VC5_TGISTAT_RESP_EXOKAY_MASK                           0x00000002
#define    CAM_CAM_VC5_TGISTAT_RESP_OKAY_SHIFT                            0
#define    CAM_CAM_VC5_TGISTAT_RESP_OKAY_MASK                             0x00000001

#define CAM_CAM_VC5_TG_IEN_OFFSET                                         0x0000071C
#define CAM_CAM_VC5_TG_IEN_TYPE                                           UInt32
#define CAM_CAM_VC5_TG_IEN_RESERVED_MASK                                  0xFFFFFFE0
#define    CAM_CAM_VC5_TG_IEN_FIFO_FULL_SHIFT                             4
#define    CAM_CAM_VC5_TG_IEN_FIFO_FULL_MASK                              0x00000010
#define    CAM_CAM_VC5_TG_IEN_RESP_DECERR_SHIFT                           3
#define    CAM_CAM_VC5_TG_IEN_RESP_DECERR_MASK                            0x00000008
#define    CAM_CAM_VC5_TG_IEN_RESP_SLVERR_SHIFT                           2
#define    CAM_CAM_VC5_TG_IEN_RESP_SLVERR_MASK                            0x00000004
#define    CAM_CAM_VC5_TG_IEN_RESP_EXOKAY_SHIFT                           1
#define    CAM_CAM_VC5_TG_IEN_RESP_EXOKAY_MASK                            0x00000002
#define    CAM_CAM_VC5_TG_IEN_RESP_OKAY_SHIFT                             0
#define    CAM_CAM_VC5_TG_IEN_RESP_OKAY_MASK                              0x00000001

#define CAM_CAM_VC5_ACFG_OFFSET                                           0x00000720
#define CAM_CAM_VC5_ACFG_TYPE                                             UInt32
#define CAM_CAM_VC5_ACFG_RESERVED_MASK                                    0xFFFFFFFE
#define    CAM_CAM_VC5_ACFG_CONT_SHIFT                                    0
#define    CAM_CAM_VC5_ACFG_CONT_MASK                                     0x00000001

#define CAM_CAM_VC5_ASTA_OFFSET                                           0x00000724
#define CAM_CAM_VC5_ASTA_TYPE                                             UInt32
#define CAM_CAM_VC5_ASTA_RESERVED_MASK                                    0xFFFFFF0E
#define    CAM_CAM_VC5_ASTA_STATE_SHIFT                                   4
#define    CAM_CAM_VC5_ASTA_STATE_MASK                                    0x000000F0
#define    CAM_CAM_VC5_ASTA_GW_SHIFT                                      0
#define    CAM_CAM_VC5_ASTA_GW_MASK                                       0x00000001

#define CAM_CAM_VC5_ASA_OFFSET                                            0x00000728
#define CAM_CAM_VC5_ASA_TYPE                                              UInt32
#define CAM_CAM_VC5_ASA_RESERVED_MASK                                     0x0000000F
#define    CAM_CAM_VC5_ASA_SA_SHIFT                                       4
#define    CAM_CAM_VC5_ASA_SA_MASK                                        0xFFFFFFF0

#define CAM_CAM_VC5_AEA_OFFSET                                            0x0000072C
#define CAM_CAM_VC5_AEA_TYPE                                              UInt32
#define CAM_CAM_VC5_AEA_RESERVED_MASK                                     0x0000000F
#define    CAM_CAM_VC5_AEA_EA_SHIFT                                       4
#define    CAM_CAM_VC5_AEA_EA_MASK                                        0xFFFFFFF0

#define CAM_CAM_VC5_ASL_OFFSET                                            0x00000730
#define CAM_CAM_VC5_ASL_TYPE                                              UInt32
#define CAM_CAM_VC5_ASL_RESERVED_MASK                                     0x0000000F
#define    CAM_CAM_VC5_ASL_LENGTH_SHIFT                                   16
#define    CAM_CAM_VC5_ASL_LENGTH_MASK                                    0xFFFF0000
#define    CAM_CAM_VC5_ASL_STRIDE_SHIFT                                   4
#define    CAM_CAM_VC5_ASL_STRIDE_MASK                                    0x0000FFF0

#define CAM_CAM_VC5_ALC_OFFSET                                            0x00000734
#define CAM_CAM_VC5_ALC_TYPE                                              UInt32
#define CAM_CAM_VC5_ALC_RESERVED_MASK                                     0xFFFF0000
#define    CAM_CAM_VC5_ALC_LC_SHIFT                                       0
#define    CAM_CAM_VC5_ALC_LC_MASK                                        0x0000FFFF

#define CAM_CAM_VC5_AT_OFFSET                                             0x00000738
#define CAM_CAM_VC5_AT_TYPE                                               UInt32
#define CAM_CAM_VC5_AT_RESERVED_MASK                                      0xFFFF0000
#define    CAM_CAM_VC5_AT_TIMER_SHIFT                                     0
#define    CAM_CAM_VC5_AT_TIMER_MASK                                      0x0000FFFF

#define CAM_CAM_VC5_AMM_OFFSET                                            0x0000073C
#define CAM_CAM_VC5_AMM_TYPE                                              UInt32
#define CAM_CAM_VC5_AMM_RESERVED_MASK                                     0xFF00FF00
#define    CAM_CAM_VC5_AMM_MAX_SHIFT                                      16
#define    CAM_CAM_VC5_AMM_MAX_MASK                                       0x00FF0000
#define    CAM_CAM_VC5_AMM_MIN_SHIFT                                      0
#define    CAM_CAM_VC5_AMM_MIN_MASK                                       0x000000FF

#define CAM_CAM_VC5_ATRIG_OFFSET                                          0x00000740
#define CAM_CAM_VC5_ATRIG_TYPE                                            UInt32
#define CAM_CAM_VC5_ATRIG_RESERVED_MASK                                   0xFFFFFFFE
#define    CAM_CAM_VC5_ATRIG_TRIG_SHIFT                                   0
#define    CAM_CAM_VC5_ATRIG_TRIG_MASK                                    0x00000001

#define CAM_IB0_DBG_CTL_OFFSET                                            0x00000A00
#define CAM_IB0_DBG_CTL_TYPE                                              UInt32
#define CAM_IB0_DBG_CTL_RESERVED_MASK                                     0xF800F800
#define    CAM_IB0_DBG_CTL_NUM_LINES_SHIFT                                16
#define    CAM_IB0_DBG_CTL_NUM_LINES_MASK                                 0x07FF0000
#define    CAM_IB0_DBG_CTL_NUM_LINES_WRAP_SHIFT                           2
#define    CAM_IB0_DBG_CTL_NUM_LINES_WRAP_MASK                            0x000007FC
#define    CAM_IB0_DBG_CTL_WRAP_MODE_SHIFT                                1
#define    CAM_IB0_DBG_CTL_WRAP_MODE_MASK                                 0x00000002
#define    CAM_IB0_DBG_CTL_ENABLE_SHIFT                                   0
#define    CAM_IB0_DBG_CTL_ENABLE_MASK                                    0x00000001

#define CAM_IB0_DBG_LAST_ADDR_OFFSET                                      0x00000A04
#define CAM_IB0_DBG_LAST_ADDR_TYPE                                        UInt32
#define CAM_IB0_DBG_LAST_ADDR_RESERVED_MASK                               0x00000000
#define    CAM_IB0_DBG_LAST_ADDR_IB0_LAST_ADDRESS_SHIFT                   0
#define    CAM_IB0_DBG_LAST_ADDR_IB0_LAST_ADDRESS_MASK                    0xFFFFFFFF

#define CAM_IB0_DBG_LAST_CMD1_OFFSET                                      0x00000A08
#define CAM_IB0_DBG_LAST_CMD1_TYPE                                        UInt32
#define CAM_IB0_DBG_LAST_CMD1_RESERVED_MASK                               0x00000000
#define    CAM_IB0_DBG_LAST_CMD1_IB0_LAST_COMMAND1_SHIFT                  0
#define    CAM_IB0_DBG_LAST_CMD1_IB0_LAST_COMMAND1_MASK                   0xFFFFFFFF

#define CAM_IB0_DBG_LAST_CMD2_OFFSET                                      0x00000A0C
#define CAM_IB0_DBG_LAST_CMD2_TYPE                                        UInt32
#define CAM_IB0_DBG_LAST_CMD2_RESERVED_MASK                               0x00000000
#define    CAM_IB0_DBG_LAST_CMD2_IB0_LAST_COMMAND2_SHIFT                  0
#define    CAM_IB0_DBG_LAST_CMD2_IB0_LAST_COMMAND2_MASK                   0xFFFFFFFF

#define CAM_IB0_DBG_2LAST_ADDR_OFFSET                                     0x00000A10
#define CAM_IB0_DBG_2LAST_ADDR_TYPE                                       UInt32
#define CAM_IB0_DBG_2LAST_ADDR_RESERVED_MASK                              0x00000000
#define    CAM_IB0_DBG_2LAST_ADDR_IB0_2LAST_ADDRESS_SHIFT                 0
#define    CAM_IB0_DBG_2LAST_ADDR_IB0_2LAST_ADDRESS_MASK                  0xFFFFFFFF

#define CAM_IB0_DBG_2LAST_CMD1_OFFSET                                     0x00000A14
#define CAM_IB0_DBG_2LAST_CMD1_TYPE                                       UInt32
#define CAM_IB0_DBG_2LAST_CMD1_RESERVED_MASK                              0x00000000
#define    CAM_IB0_DBG_2LAST_CMD1_IB0_2LAST_COMMAND1_SHIFT                0
#define    CAM_IB0_DBG_2LAST_CMD1_IB0_2LAST_COMMAND1_MASK                 0xFFFFFFFF

#define CAM_IB0_DBG_2LAST_CMD2_OFFSET                                     0x00000A18
#define CAM_IB0_DBG_2LAST_CMD2_TYPE                                       UInt32
#define CAM_IB0_DBG_2LAST_CMD2_RESERVED_MASK                              0x00000000
#define    CAM_IB0_DBG_2LAST_CMD2_IB0_2LAST_COMMAND2_SHIFT                0
#define    CAM_IB0_DBG_2LAST_CMD2_IB0_2LAST_COMMAND2_MASK                 0xFFFFFFFF

#define CAM_IB0_DBG_W_CHAN_OFFSET                                         0x00000A1C
#define CAM_IB0_DBG_W_CHAN_TYPE                                           UInt32
#define CAM_IB0_DBG_W_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_IB0_DBG_W_CHAN_IB0_LAST_W_CHAN_SHIFT                       0
#define    CAM_IB0_DBG_W_CHAN_IB0_LAST_W_CHAN_MASK                        0xFFFFFFFF

#define CAM_IB0_DBG_W_DATA_OFFSET                                         0x00000A20
#define CAM_IB0_DBG_W_DATA_TYPE                                           UInt32
#define CAM_IB0_DBG_W_DATA_RESERVED_MASK                                  0x00000000
#define    CAM_IB0_DBG_W_DATA_IB0_LAST_W_DATA_SHIFT                       0
#define    CAM_IB0_DBG_W_DATA_IB0_LAST_W_DATA_MASK                        0xFFFFFFFF

#define CAM_IB0_DBG_B_CHAN_OFFSET                                         0x00000A24
#define CAM_IB0_DBG_B_CHAN_TYPE                                           UInt32
#define CAM_IB0_DBG_B_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_IB0_DBG_B_CHAN_IB0_LAST_B_CHAN_SHIFT                       0
#define    CAM_IB0_DBG_B_CHAN_IB0_LAST_B_CHAN_MASK                        0xFFFFFFFF

#define CAM_IB0_DBG_BRST_XN_CNT_OFFSET                                    0x00000A28
#define CAM_IB0_DBG_BRST_XN_CNT_TYPE                                      UInt32
#define CAM_IB0_DBG_BRST_XN_CNT_RESERVED_MASK                             0x000000E0
#define    CAM_IB0_DBG_BRST_XN_CNT_IB0_XN_COUNTER_SHIFT                   16
#define    CAM_IB0_DBG_BRST_XN_CNT_IB0_XN_COUNTER_MASK                    0xFFFF0000
#define    CAM_IB0_DBG_BRST_XN_CNT_IB0_OUT_OF_RANGE_SHIFT                 8
#define    CAM_IB0_DBG_BRST_XN_CNT_IB0_OUT_OF_RANGE_MASK                  0x0000FF00
#define    CAM_IB0_DBG_BRST_XN_CNT_IB0_OUTSTDNG_XN_CNT_SHIFT              0
#define    CAM_IB0_DBG_BRST_XN_CNT_IB0_OUTSTDNG_XN_CNT_MASK               0x0000001F

#define CAM_IB0_DBG_LINE_CNT_OFFSET                                       0x00000A2C
#define CAM_IB0_DBG_LINE_CNT_TYPE                                         UInt32
#define CAM_IB0_DBG_LINE_CNT_RESERVED_MASK                                0x00000000
#define    CAM_IB0_DBG_LINE_CNT_IB0_LINE_CNT_SHIFT                        0
#define    CAM_IB0_DBG_LINE_CNT_IB0_LINE_CNT_MASK                         0xFFFFFFFF

#define CAM_IB1_DBG_CTL_OFFSET                                            0x00000A40
#define CAM_IB1_DBG_CTL_TYPE                                              UInt32
#define CAM_IB1_DBG_CTL_RESERVED_MASK                                     0xF800F800
#define    CAM_IB1_DBG_CTL_NUM_LINES_SHIFT                                16
#define    CAM_IB1_DBG_CTL_NUM_LINES_MASK                                 0x07FF0000
#define    CAM_IB1_DBG_CTL_NUM_LINES_WRAP_SHIFT                           2
#define    CAM_IB1_DBG_CTL_NUM_LINES_WRAP_MASK                            0x000007FC
#define    CAM_IB1_DBG_CTL_WRAP_MODE_SHIFT                                1
#define    CAM_IB1_DBG_CTL_WRAP_MODE_MASK                                 0x00000002
#define    CAM_IB1_DBG_CTL_ENABLE_SHIFT                                   0
#define    CAM_IB1_DBG_CTL_ENABLE_MASK                                    0x00000001

#define CAM_IB1_DBG_LAST_ADDR_OFFSET                                      0x00000A44
#define CAM_IB1_DBG_LAST_ADDR_TYPE                                        UInt32
#define CAM_IB1_DBG_LAST_ADDR_RESERVED_MASK                               0x00000000
#define    CAM_IB1_DBG_LAST_ADDR_IB1_LAST_ADDRESS_SHIFT                   0
#define    CAM_IB1_DBG_LAST_ADDR_IB1_LAST_ADDRESS_MASK                    0xFFFFFFFF

#define CAM_IB1_DBG_LAST_CMD1_OFFSET                                      0x00000A48
#define CAM_IB1_DBG_LAST_CMD1_TYPE                                        UInt32
#define CAM_IB1_DBG_LAST_CMD1_RESERVED_MASK                               0x00000000
#define    CAM_IB1_DBG_LAST_CMD1_IB1_LAST_COMMAND1_SHIFT                  0
#define    CAM_IB1_DBG_LAST_CMD1_IB1_LAST_COMMAND1_MASK                   0xFFFFFFFF

#define CAM_IB1_DBG_LAST_CMD2_OFFSET                                      0x00000A4C
#define CAM_IB1_DBG_LAST_CMD2_TYPE                                        UInt32
#define CAM_IB1_DBG_LAST_CMD2_RESERVED_MASK                               0x00000000
#define    CAM_IB1_DBG_LAST_CMD2_IB1_LAST_COMMAND2_SHIFT                  0
#define    CAM_IB1_DBG_LAST_CMD2_IB1_LAST_COMMAND2_MASK                   0xFFFFFFFF

#define CAM_IB1_DBG_2LAST_ADDR_OFFSET                                     0x00000A50
#define CAM_IB1_DBG_2LAST_ADDR_TYPE                                       UInt32
#define CAM_IB1_DBG_2LAST_ADDR_RESERVED_MASK                              0x00000000
#define    CAM_IB1_DBG_2LAST_ADDR_IB1_2LAST_ADDRESS_SHIFT                 0
#define    CAM_IB1_DBG_2LAST_ADDR_IB1_2LAST_ADDRESS_MASK                  0xFFFFFFFF

#define CAM_IB1_DBG_2LAST_CMD1_OFFSET                                     0x00000A54
#define CAM_IB1_DBG_2LAST_CMD1_TYPE                                       UInt32
#define CAM_IB1_DBG_2LAST_CMD1_RESERVED_MASK                              0x00000000
#define    CAM_IB1_DBG_2LAST_CMD1_IB1_2LAST_COMMAND1_SHIFT                0
#define    CAM_IB1_DBG_2LAST_CMD1_IB1_2LAST_COMMAND1_MASK                 0xFFFFFFFF

#define CAM_IB1_DBG_2LAST_CMD2_OFFSET                                     0x00000A58
#define CAM_IB1_DBG_2LAST_CMD2_TYPE                                       UInt32
#define CAM_IB1_DBG_2LAST_CMD2_RESERVED_MASK                              0x00000000
#define    CAM_IB1_DBG_2LAST_CMD2_IB1_2LAST_COMMAND2_SHIFT                0
#define    CAM_IB1_DBG_2LAST_CMD2_IB1_2LAST_COMMAND2_MASK                 0xFFFFFFFF

#define CAM_IB1_DBG_W_CHAN_OFFSET                                         0x00000A5C
#define CAM_IB1_DBG_W_CHAN_TYPE                                           UInt32
#define CAM_IB1_DBG_W_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_IB1_DBG_W_CHAN_IB1_LAST_W_CHAN_SHIFT                       0
#define    CAM_IB1_DBG_W_CHAN_IB1_LAST_W_CHAN_MASK                        0xFFFFFFFF

#define CAM_IB1_DBG_W_DATA_OFFSET                                         0x00000A60
#define CAM_IB1_DBG_W_DATA_TYPE                                           UInt32
#define CAM_IB1_DBG_W_DATA_RESERVED_MASK                                  0x00000000
#define    CAM_IB1_DBG_W_DATA_IB1_LAST_W_DATA_SHIFT                       0
#define    CAM_IB1_DBG_W_DATA_IB1_LAST_W_DATA_MASK                        0xFFFFFFFF

#define CAM_IB1_DBG_B_CHAN_OFFSET                                         0x00000A64
#define CAM_IB1_DBG_B_CHAN_TYPE                                           UInt32
#define CAM_IB1_DBG_B_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_IB1_DBG_B_CHAN_IB1_LAST_B_CHAN_SHIFT                       0
#define    CAM_IB1_DBG_B_CHAN_IB1_LAST_B_CHAN_MASK                        0xFFFFFFFF

#define CAM_IB1_DBG_BRST_XN_CNT_OFFSET                                    0x00000A68
#define CAM_IB1_DBG_BRST_XN_CNT_TYPE                                      UInt32
#define CAM_IB1_DBG_BRST_XN_CNT_RESERVED_MASK                             0x000000E0
#define    CAM_IB1_DBG_BRST_XN_CNT_IB1_XN_COUNTER_SHIFT                   16
#define    CAM_IB1_DBG_BRST_XN_CNT_IB1_XN_COUNTER_MASK                    0xFFFF0000
#define    CAM_IB1_DBG_BRST_XN_CNT_IB1_OUT_OF_RANGE_SHIFT                 8
#define    CAM_IB1_DBG_BRST_XN_CNT_IB1_OUT_OF_RANGE_MASK                  0x0000FF00
#define    CAM_IB1_DBG_BRST_XN_CNT_IB1_OUTSTDNG_XN_CNT_SHIFT              0
#define    CAM_IB1_DBG_BRST_XN_CNT_IB1_OUTSTDNG_XN_CNT_MASK               0x0000001F

#define CAM_IB1_DBG_LINE_CNT_OFFSET                                       0x00000A6C
#define CAM_IB1_DBG_LINE_CNT_TYPE                                         UInt32
#define CAM_IB1_DBG_LINE_CNT_RESERVED_MASK                                0x00000000
#define    CAM_IB1_DBG_LINE_CNT_IB1_LINE_CNT_SHIFT                        0
#define    CAM_IB1_DBG_LINE_CNT_IB1_LINE_CNT_MASK                         0xFFFFFFFF

#define CAM_DB0_DBG_CTL_OFFSET                                            0x00000A80
#define CAM_DB0_DBG_CTL_TYPE                                              UInt32
#define CAM_DB0_DBG_CTL_RESERVED_MASK                                     0xF800F800
#define    CAM_DB0_DBG_CTL_NUM_LINES_SHIFT                                16
#define    CAM_DB0_DBG_CTL_NUM_LINES_MASK                                 0x07FF0000
#define    CAM_DB0_DBG_CTL_NUM_LINES_WRAP_SHIFT                           2
#define    CAM_DB0_DBG_CTL_NUM_LINES_WRAP_MASK                            0x000007FC
#define    CAM_DB0_DBG_CTL_WRAP_MODE_SHIFT                                1
#define    CAM_DB0_DBG_CTL_WRAP_MODE_MASK                                 0x00000002
#define    CAM_DB0_DBG_CTL_ENABLE_SHIFT                                   0
#define    CAM_DB0_DBG_CTL_ENABLE_MASK                                    0x00000001

#define CAM_DB0_DBG_LAST_ADDR_OFFSET                                      0x00000A84
#define CAM_DB0_DBG_LAST_ADDR_TYPE                                        UInt32
#define CAM_DB0_DBG_LAST_ADDR_RESERVED_MASK                               0x00000000
#define    CAM_DB0_DBG_LAST_ADDR_DB0_LAST_ADDRESS_SHIFT                   0
#define    CAM_DB0_DBG_LAST_ADDR_DB0_LAST_ADDRESS_MASK                    0xFFFFFFFF

#define CAM_DB0_DBG_LAST_CMD1_OFFSET                                      0x00000A88
#define CAM_DB0_DBG_LAST_CMD1_TYPE                                        UInt32
#define CAM_DB0_DBG_LAST_CMD1_RESERVED_MASK                               0x00000000
#define    CAM_DB0_DBG_LAST_CMD1_DB0_LAST_COMMAND1_SHIFT                  0
#define    CAM_DB0_DBG_LAST_CMD1_DB0_LAST_COMMAND1_MASK                   0xFFFFFFFF

#define CAM_DB0_DBG_LAST_CMD2_OFFSET                                      0x00000A8C
#define CAM_DB0_DBG_LAST_CMD2_TYPE                                        UInt32
#define CAM_DB0_DBG_LAST_CMD2_RESERVED_MASK                               0x00000000
#define    CAM_DB0_DBG_LAST_CMD2_DB0_LAST_COMMAND2_SHIFT                  0
#define    CAM_DB0_DBG_LAST_CMD2_DB0_LAST_COMMAND2_MASK                   0xFFFFFFFF

#define CAM_DB0_DBG_2LAST_ADDR_OFFSET                                     0x00000A90
#define CAM_DB0_DBG_2LAST_ADDR_TYPE                                       UInt32
#define CAM_DB0_DBG_2LAST_ADDR_RESERVED_MASK                              0x00000000
#define    CAM_DB0_DBG_2LAST_ADDR_DB0_2LAST_ADDRESS_SHIFT                 0
#define    CAM_DB0_DBG_2LAST_ADDR_DB0_2LAST_ADDRESS_MASK                  0xFFFFFFFF

#define CAM_DB0_DBG_2LAST_CMD1_OFFSET                                     0x00000A94
#define CAM_DB0_DBG_2LAST_CMD1_TYPE                                       UInt32
#define CAM_DB0_DBG_2LAST_CMD1_RESERVED_MASK                              0x00000000
#define    CAM_DB0_DBG_2LAST_CMD1_DB0_2LAST_COMMAND1_SHIFT                0
#define    CAM_DB0_DBG_2LAST_CMD1_DB0_2LAST_COMMAND1_MASK                 0xFFFFFFFF

#define CAM_DB0_DBG_2LAST_CMD2_OFFSET                                     0x00000A98
#define CAM_DB0_DBG_2LAST_CMD2_TYPE                                       UInt32
#define CAM_DB0_DBG_2LAST_CMD2_RESERVED_MASK                              0x00000000
#define    CAM_DB0_DBG_2LAST_CMD2_DB0_2LAST_COMMAND2_SHIFT                0
#define    CAM_DB0_DBG_2LAST_CMD2_DB0_2LAST_COMMAND2_MASK                 0xFFFFFFFF

#define CAM_DB0_DBG_W_CHAN_OFFSET                                         0x00000A9C
#define CAM_DB0_DBG_W_CHAN_TYPE                                           UInt32
#define CAM_DB0_DBG_W_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_DB0_DBG_W_CHAN_DB0_LAST_W_CHAN_SHIFT                       0
#define    CAM_DB0_DBG_W_CHAN_DB0_LAST_W_CHAN_MASK                        0xFFFFFFFF

#define CAM_DB0_DBG_W_DATA_OFFSET                                         0x00000AA0
#define CAM_DB0_DBG_W_DATA_TYPE                                           UInt32
#define CAM_DB0_DBG_W_DATA_RESERVED_MASK                                  0x00000000
#define    CAM_DB0_DBG_W_DATA_DB0_LAST_W_DATA_SHIFT                       0
#define    CAM_DB0_DBG_W_DATA_DB0_LAST_W_DATA_MASK                        0xFFFFFFFF

#define CAM_DB0_DBG_B_CHAN_OFFSET                                         0x00000AA4
#define CAM_DB0_DBG_B_CHAN_TYPE                                           UInt32
#define CAM_DB0_DBG_B_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_DB0_DBG_B_CHAN_DB0_LAST_B_CHAN_SHIFT                       0
#define    CAM_DB0_DBG_B_CHAN_DB0_LAST_B_CHAN_MASK                        0xFFFFFFFF

#define CAM_DB0_DBG_BRST_XN_CNT_OFFSET                                    0x00000AA8
#define CAM_DB0_DBG_BRST_XN_CNT_TYPE                                      UInt32
#define CAM_DB0_DBG_BRST_XN_CNT_RESERVED_MASK                             0x000000E0
#define    CAM_DB0_DBG_BRST_XN_CNT_DB0_XN_COUNTER_SHIFT                   16
#define    CAM_DB0_DBG_BRST_XN_CNT_DB0_XN_COUNTER_MASK                    0xFFFF0000
#define    CAM_DB0_DBG_BRST_XN_CNT_DB0_OUT_OF_RANGE_SHIFT                 8
#define    CAM_DB0_DBG_BRST_XN_CNT_DB0_OUT_OF_RANGE_MASK                  0x0000FF00
#define    CAM_DB0_DBG_BRST_XN_CNT_DB0_OUTSTDNG_XN_CNT_SHIFT              0
#define    CAM_DB0_DBG_BRST_XN_CNT_DB0_OUTSTDNG_XN_CNT_MASK               0x0000001F

#define CAM_DB0_DBG_LINE_CNT_OFFSET                                       0x00000AAC
#define CAM_DB0_DBG_LINE_CNT_TYPE                                         UInt32
#define CAM_DB0_DBG_LINE_CNT_RESERVED_MASK                                0x00000000
#define    CAM_DB0_DBG_LINE_CNT_DB0_LINE_CNT_SHIFT                        0
#define    CAM_DB0_DBG_LINE_CNT_DB0_LINE_CNT_MASK                         0xFFFFFFFF

#define CAM_DB1_DBG_CTL_OFFSET                                            0x00000AC0
#define CAM_DB1_DBG_CTL_TYPE                                              UInt32
#define CAM_DB1_DBG_CTL_RESERVED_MASK                                     0xF800F800
#define    CAM_DB1_DBG_CTL_NUM_LINES_SHIFT                                16
#define    CAM_DB1_DBG_CTL_NUM_LINES_MASK                                 0x07FF0000
#define    CAM_DB1_DBG_CTL_NUM_LINES_WRAP_SHIFT                           2
#define    CAM_DB1_DBG_CTL_NUM_LINES_WRAP_MASK                            0x000007FC
#define    CAM_DB1_DBG_CTL_WRAP_MODE_SHIFT                                1
#define    CAM_DB1_DBG_CTL_WRAP_MODE_MASK                                 0x00000002
#define    CAM_DB1_DBG_CTL_ENABLE_SHIFT                                   0
#define    CAM_DB1_DBG_CTL_ENABLE_MASK                                    0x00000001

#define CAM_DB1_DBG_LAST_ADDR_OFFSET                                      0x00000AC4
#define CAM_DB1_DBG_LAST_ADDR_TYPE                                        UInt32
#define CAM_DB1_DBG_LAST_ADDR_RESERVED_MASK                               0x00000000
#define    CAM_DB1_DBG_LAST_ADDR_DB1_LAST_ADDRESS_SHIFT                   0
#define    CAM_DB1_DBG_LAST_ADDR_DB1_LAST_ADDRESS_MASK                    0xFFFFFFFF

#define CAM_DB1_DBG_LAST_CMD1_OFFSET                                      0x00000AC8
#define CAM_DB1_DBG_LAST_CMD1_TYPE                                        UInt32
#define CAM_DB1_DBG_LAST_CMD1_RESERVED_MASK                               0x00000000
#define    CAM_DB1_DBG_LAST_CMD1_DB1_LAST_COMMAND1_SHIFT                  0
#define    CAM_DB1_DBG_LAST_CMD1_DB1_LAST_COMMAND1_MASK                   0xFFFFFFFF

#define CAM_DB1_DBG_LAST_CMD2_OFFSET                                      0x00000ACC
#define CAM_DB1_DBG_LAST_CMD2_TYPE                                        UInt32
#define CAM_DB1_DBG_LAST_CMD2_RESERVED_MASK                               0x00000000
#define    CAM_DB1_DBG_LAST_CMD2_DB1_LAST_COMMAND2_SHIFT                  0
#define    CAM_DB1_DBG_LAST_CMD2_DB1_LAST_COMMAND2_MASK                   0xFFFFFFFF

#define CAM_DB1_DBG_2LAST_ADDR_OFFSET                                     0x00000AD0
#define CAM_DB1_DBG_2LAST_ADDR_TYPE                                       UInt32
#define CAM_DB1_DBG_2LAST_ADDR_RESERVED_MASK                              0x00000000
#define    CAM_DB1_DBG_2LAST_ADDR_DB1_2LAST_ADDRESS_SHIFT                 0
#define    CAM_DB1_DBG_2LAST_ADDR_DB1_2LAST_ADDRESS_MASK                  0xFFFFFFFF

#define CAM_DB1_DBG_2LAST_CMD1_OFFSET                                     0x00000AD4
#define CAM_DB1_DBG_2LAST_CMD1_TYPE                                       UInt32
#define CAM_DB1_DBG_2LAST_CMD1_RESERVED_MASK                              0x00000000
#define    CAM_DB1_DBG_2LAST_CMD1_DB1_2LAST_COMMAND1_SHIFT                0
#define    CAM_DB1_DBG_2LAST_CMD1_DB1_2LAST_COMMAND1_MASK                 0xFFFFFFFF

#define CAM_DB1_DBG_2LAST_CMD2_OFFSET                                     0x00000AD8
#define CAM_DB1_DBG_2LAST_CMD2_TYPE                                       UInt32
#define CAM_DB1_DBG_2LAST_CMD2_RESERVED_MASK                              0x00000000
#define    CAM_DB1_DBG_2LAST_CMD2_DB1_2LAST_COMMAND2_SHIFT                0
#define    CAM_DB1_DBG_2LAST_CMD2_DB1_2LAST_COMMAND2_MASK                 0xFFFFFFFF

#define CAM_DB1_DBG_W_CHAN_OFFSET                                         0x00000ADC
#define CAM_DB1_DBG_W_CHAN_TYPE                                           UInt32
#define CAM_DB1_DBG_W_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_DB1_DBG_W_CHAN_DB1_LAST_W_CHAN_SHIFT                       0
#define    CAM_DB1_DBG_W_CHAN_DB1_LAST_W_CHAN_MASK                        0xFFFFFFFF

#define CAM_DB1_DBG_W_DATA_OFFSET                                         0x00000AE0
#define CAM_DB1_DBG_W_DATA_TYPE                                           UInt32
#define CAM_DB1_DBG_W_DATA_RESERVED_MASK                                  0x00000000
#define    CAM_DB1_DBG_W_DATA_DB1_LAST_W_DATA_SHIFT                       0
#define    CAM_DB1_DBG_W_DATA_DB1_LAST_W_DATA_MASK                        0xFFFFFFFF

#define CAM_DB1_DBG_B_CHAN_OFFSET                                         0x00000AE4
#define CAM_DB1_DBG_B_CHAN_TYPE                                           UInt32
#define CAM_DB1_DBG_B_CHAN_RESERVED_MASK                                  0x00000000
#define    CAM_DB1_DBG_B_CHAN_DB1_LAST_B_CHAN_SHIFT                       0
#define    CAM_DB1_DBG_B_CHAN_DB1_LAST_B_CHAN_MASK                        0xFFFFFFFF

#define CAM_DB1_DBG_BRST_XN_CNT_OFFSET                                    0x00000AE8
#define CAM_DB1_DBG_BRST_XN_CNT_TYPE                                      UInt32
#define CAM_DB1_DBG_BRST_XN_CNT_RESERVED_MASK                             0x000000E0
#define    CAM_DB1_DBG_BRST_XN_CNT_DB1_XN_COUNTER_SHIFT                   16
#define    CAM_DB1_DBG_BRST_XN_CNT_DB1_XN_COUNTER_MASK                    0xFFFF0000
#define    CAM_DB1_DBG_BRST_XN_CNT_DB1_OUT_OF_RANGE_SHIFT                 8
#define    CAM_DB1_DBG_BRST_XN_CNT_DB1_OUT_OF_RANGE_MASK                  0x0000FF00
#define    CAM_DB1_DBG_BRST_XN_CNT_DB1_OUTSTDNG_XN_CNT_SHIFT              0
#define    CAM_DB1_DBG_BRST_XN_CNT_DB1_OUTSTDNG_XN_CNT_MASK               0x0000001F

#define CAM_DB1_DBG_LINE_CNT_OFFSET                                       0x00000AEC
#define CAM_DB1_DBG_LINE_CNT_TYPE                                         UInt32
#define CAM_DB1_DBG_LINE_CNT_RESERVED_MASK                                0x00000000
#define    CAM_DB1_DBG_LINE_CNT_DB1_LINE_CNT_SHIFT                        0
#define    CAM_DB1_DBG_LINE_CNT_DB1_LINE_CNT_MASK                         0xFFFFFFFF

#endif /* __BRCM_RDB_CAM_H__ */
