#
# Copyright 2016-2019 Broadcom Limited.  All rights reserved.
#
# This program is the proprietary software of Broadcom Limited and/or its
# licensors, and may only be used, duplicated, modified or distributed pursuant
# to the terms and conditions of a separate, written license agreement executed
# between you and Broadcom (an "Authorized License").
#
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
# LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
# IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
# 1. This program, including its structure, sequence and organization,
#    constitutes the valuable trade secrets of Broadcom, and you shall use all
#    reasonable efforts to protect the confidentiality thereof, and to use this
#    information only in connection with your use of Broadcom integrated
#    circuit products.
#
# 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
#    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
#    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
#    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
#    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
#    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
#    SOFTWARE.
#
# 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
#    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
#    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
#    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################
#!/bin/bash

#Save CONFIGS Path
CONFIGS_PATH=$PWD

#Check the command exit status
check_output () {
if [ $1 -ne 0 ]
then
    echo $BASH_SOURCE "Failed"
    rm *.c *.img *.log
    exit 3
fi
}

#Configuration list
config_list=(
     'tg'
     'tg_prodesign'
     'ov10640_csi'
     'ov10640_csi_bayesian'
     'ov10640_csi_prodesign'
     'ov10640_csi_cb'
     'ov10640_csi_cb_prodesign'
     'ov10640_csi_hdr'
     'ov10640_csi_60fps'
     'ov10640_csi_1_3_mp'
     'ov10640_csi_dp'
     'ov10640_csi_cb_bypass'
     'ar0143_csi_hdr'
     'ar0143_csi_hdr_cb'
     'ar0143_csi_968p_hdr'
     'ar0143_csi_hdr_awb_gw'
     'ox03a10_csi_1280p_16_dcg_12_vs_isp_hdr'
     'ox03a10_csi_1080p_16_dcg_12_vs_isp_hdr'
     'ox03a10_csi_1920x1280_16_dcg_12_vs_isp_hdr'
     'ox03a10_csi_720p_ds_16_dcg_12_vs_isp_hdr'
     'ox03a10_csi_720p_16_dcg_12_vs_isp_hdr'
     'ox03a10_csi_720p_16_dcg_12_vs_isp_hdr_cb'
     'ox03a10_csi_720p_16_dcg_12_vs_isp_hdr_dp'
     'ox03a10_csi_720p_16_dcg_12_vs_isp_hdr_60fps'
     'ox03a10_csi_720p_16_dcg_12_vs_isp_hdr_test'
     'ox03a10_csi_720p_16_dcg_12_vs_isp_hdr_bypass'
     'ov9716_csi_1392x976_16_dcg_12_vs_isp_hdr'
     'ov9716_csi_720p_16_dcg_12_vs_isp_hdr'
   )

#Generate images specific to revision
generate () {
echo config=$1
   #Strings for HV and CV
   PASS_HV="_HV"
   PASS_CV="_CV"
   #Remove existing outputs
   rm -rf $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/
   rm -rf $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_HV/
   rm -rf $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV/
   #Create directory for HV
   mkdir -p $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_HV
   #Generate images for HV
   ./binaries/image_writer pass_hv $CONFIGS_PATH/inputs/config_$1.txt
   #Check output of image_writer
   check_output $?
   #Move image arrays to output directory
   mv *.c $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_HV/
   mv *.img $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_HV/
   #Remove log files
   rm *.log
   #Create directory for CV
   mkdir -p $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV
   #Generate images for CV
   ./binaries/image_writer pass_cv $CONFIGS_PATH/inputs/config_$1.txt
   #Check output of image_writer for CV
   if [ $? -ne 0 ]
    then
        #No CV file exists, so clean HV
        #Remove CV dir
        rm -rf $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV/
        rm *.log
        rm *.img
        rm *.c
        #Make HV as the default configuration
        mv $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_HV/ $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/
    else
        #CV exists
        #Move image arrays to output directory
        mv *.c $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV/
        mv *.img $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV/
        #Remove log files
        rm *.log
        #Move HV to revision configuration
        mv $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_HV/ $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/
        #Prepeare for combine
        mv $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp.img \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp_hv.img
        cp $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV/isp.img \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp_cv.img
        mv $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode.img \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode_hv.img
        cp $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV/mode.img \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode_cv.img
        #Remove CV dir
        rm -rf $CONFIGS_PATH/outputs/$1_$CHIP_REVISION$PASS_CV/
        #Run combine for isp
        ./binaries/image_combine pass isp $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp_hv.img  \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp_cv.img \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp.img
        #Run combine for mode
        ./binaries/image_combine pass mode $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode_hv.img  \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode_cv.img \
            $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode.img
        #Check output of image_combine
        check_output $?
        #Copy output ".c"s
        mv *.c $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/
        #Clean inputs for combine
        rm $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp_hv.img
        rm $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/isp_cv.img
        rm $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode_hv.img
        rm $CONFIGS_PATH/outputs/$1_$CHIP_REVISION/mode_cv.img
   fi
}

#Remove old outputs
rm -rf $CONFIGS_PATH/outputs/

#Loop and generate images for B0
count=0
CHIP_REVISION="B0"
while [ "x${config_list[count]}" != "x" ]
do
   cfg_name=${config_list[count]}
   #Remove existing outputs
   rm -rf $CONFIGS_PATH/outputs/$cfg_name
   generate ${cfg_name}
   #Move B0 ouptuts to outputs
   mv $CONFIGS_PATH/outputs/${cfg_name}_B0 $CONFIGS_PATH/outputs/$cfg_name
   count=$(( $count + 1 ))
done
