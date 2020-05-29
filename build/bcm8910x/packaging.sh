#!/bin/bash
################################################################################
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

set -e
shopt -s extglob
BLD_SCRIPT_DIR=$( cd "$( dirname "$0" )" && pwd )
BRCM_SDK_ROOT=$(readlink -f ${BLD_SCRIPT_DIR}/../..)


function usage
{
  echo "Usage: packaging.sh [-r -s -b <board_name1,board_name2> -i <Filename containing Customer vs component names separated by comma> -c <customer_name>]"
  echo "e.g. packaging.sh -r -s -b bcm89103_evk,bcm89107_evk,bcm89109_ddr_evk -i cust_config -c default"
  echo "cust_config would contain similar to below:"
  echo "default:lwip,drivers,utils"
  echo "config1:lwip,drivers"
}

board=
release="0"
safety="0"
tot_arg=$#

while getopts ":hrsb:i:c:" opt; do
  case $opt in
    r )
        release="1"
        ;;
    s )
        safety="1"
        ;;
    b )
		board=${OPTARG}
		;;
	i )
		ignore_comp=${OPTARG}
		;;
	c )
		customer=${OPTARG}
		;;
	\? )
		echo "Invalid Option: -${OPTARG}" 1>&2
		usage
		exit 1
		;;
	: )
		echo "Invalid Option: -${OPTARG} requires an argument" 1>&2
		usage
		exit 1
		;;
	h )
		usage
		exit 0
		;;
  esac
done

shift $((OPTIND -1))

function contains() {
  local n=$#
  local value=${!n}
  for ((i=1;i < $#; i++)) {
    #echo "i: ${i}"
	if [ "${!i}" == "${value}" ]; then
	  #echo "y"
	  return 0
	fi
  }
  #echo "n"
  return 1
}

allboardpaths=(`find ${BRCM_SDK_ROOT}/build/bcm8910x/board -mindepth 1 -maxdepth 1 -type d`)
boarddir="${BRCM_SDK_ROOT}/build/bcm8910x/board/"
allboards=("${allboardpaths[@]/${boarddir}/}") #replace $boarddir with nothing in $allboardpaths[@] to get only board names
# If board input argument not provided, it would build and package for all boards.
# Search for all board directory in build/bcm8910x/board folder and generate a list
# of all boards to be built and packaged.
if [[ $board ]]; then
  echo "Board: ${board}"
else
  usage
  buildall=true
  echo ${allboards[@]}
fi

# If customer config file and customer name is given as input argument, then parse the customer config file
# to get list of libraries for which source code would be delivered.
if [[ $board ]] && [[ $tot_arg -gt 2 ]]; then
  if [[ $ignore_comp ]] && [[ $customer ]]; then
    echo "Ignore file: $ignore_comp"
    echo "Customer: $customer"
    default=false
	cust_line=`awk -F: -v keyword="$customer" '$1 == keyword {$1=$1; print}' $ignore_comp`
	if [[ $cust_line ]]; then
	  echo "Cust_line: $cust_line"
	else
	  echo "Customer $customer doesn't exist in $ignore_comp"
	  exit 1
	fi
  else
    usage
    exit 1
  fi
else
  default=true
  echo "Packaging with default"
fi

if [[ $default = true ]]; then
  echo "Packaging with default"
else
  echo "Customer for packaging: $customer"
fi

# Create an array of board lists to be built and packaged
if [[ $buildall = true ]]; then
  echo "Building for all boards..."
  boardlist=("${allboards[@]}")
else
  IFS=',' read -a boardlist <<< "${board}"
fi

for eboard in "${allboards[@]}"
do
    if [ `echo ${board} | grep -c $eboard` -eq 0 ];then
        echo Deleting $boarddir/$eboard
        rm -rf $boarddir/$eboard
    fi
done

cd ${BRCM_SDK_ROOT}/build/
#create sw_version.txt file which contains the latest release label
sw_version=`git for-each-ref --sort=-taggerdate --format=%\(refname:short\) refs/tags --merged | grep -m1 BCM8910X_REL | awk -F"_REL_" '{print $NF}'`
echo "SW_IMAGE_VERSION:=${sw_version}" > ${BRCM_SDK_ROOT}/build/bcm8910x/sw_version.txt
echo "Boards: ${boardlist[@]}"

compiler_list=(LIB_GNU_CC)
prod_family="bcm8910x"

for eachcompiler in ${compiler_list[@]}
do
  echo "Compiling lib with ${eachcompiler}"
  if [[ $eachcompiler == "LIB_GNU_CC" ]]; then
    LIBTYPE="gcc"
  elif [[ $eachcompiler == "LIB_RVCT_CC" ]]; then
    LIBTYPE="rvct"
  fi
  make clean
  for eboard in "${boardlist[@]}"
  do
    echo "$eboard"
    make board=${eboard} cust=1 ${eachcompiler}=1 prebuilds
  done

bllibpaths=(`find ${BRCM_SDK_ROOT}/out/bootloader/ -mindepth 1 -maxdepth 1 -type d`)
bldir="${BRCM_SDK_ROOT}/out/bootloader/"
blboard=("${bllibpaths[@]/${bldir}/}") #replace $bldir with nothing in $bllibpaths[@] to get only board names

prebuilt_dir=${BRCM_SDK_ROOT}/prebuilt
#lib_dir=${BRCM_SDK_ROOT}/out/${board}/lib

echo ${BLD_SCRIPT_DIR}
echo ${BRCM_SDK_ROOT}

alllibpaths=(`find ${BRCM_SDK_ROOT}/out/lib/ -mindepth 1 -maxdepth 1 -type d`)
libdir="${BRCM_SDK_ROOT}/out/lib/"
allchips=("${alllibpaths[@]/${libdir}/}") #replace $boarddir with nothing in $allboardpaths[@] to get only board names

# Delete all libraries for which source code needs to provided.
if [[ $default = false ]]; then
	cust_array=($cust_line)
	echo "component list: ${cust_array[1]}"
	IFS=',' read -a ignore_list <<< "${cust_array[1]}"
	for echip in "${allchips[@]}"
	do
		for element in "${ignore_list[@]}"
		do
			echo "$element"
			skiplib=lib${echip}_${element}.a
			rm -f ${BRCM_SDK_ROOT}/out/lib/${echip}/${skiplib}
			echo "Removing Lib as Source needs to be delivered: ${skiplib}"
		done
	done
fi

echo "Copy libraries to prebuilt directory"

for echip in "${allchips[@]}"
do
	mkdir -p ${prebuilt_dir}/${echip}/lib/${LIBTYPE}
	cp -r ${BRCM_SDK_ROOT}/out/lib/${echip}/lib* ${prebuilt_dir}/${echip}/lib/${LIBTYPE}/
	echo "Copy bootloader lib"
	echo "${blboard[@]}"
	echo "${bllibpaths}"
	echo "${blboard}"
	for blbrd in "${blboard[@]}"
	do
		echo "${blbrd}"
		if [[ ${blbrd} =~ ${echip} ]]; then
			mkdir -p ${prebuilt_dir}/${echip}/bootloader_lib/${blbrd}
			cp -rf ${BRCM_SDK_ROOT}/out/bootloader/${blbrd}/lib/lib* ${prebuilt_dir}/${echip}/bootloader_lib/${blbrd}/
		fi
    done
done
done
echo "Delete out directory..."
rm -rf ${BRCM_SDK_ROOT}/out

# generate list of all components present in $BRCM_SDK_ROOT, parse for type "lib". If the library name
# exists in ${ignore_list[@]}, then do nothing as source code needs to be delivered for that.
# if library name doesn't exists, then delete all source files except comp.mk & os under that component
# library directory
complist=( $(find ${BRCM_SDK_ROOT} -name comp.mk -o -name comp[0-9].mk) )
for comp in "${complist[@]}"
do
  comp_name=$(cat ${comp} | grep "BRCM_COMP_NAME\ \+\:=" | sed -n 's/BRCM_COMP_NAME\s\+:=\s\+\(.*\)/\1/p')
  comp_type=$(cat ${comp} | grep "BRCM_COMP_TYPE\ \+\:=" | sed -n 's/BRCM_COMP_TYPE\s\+:=\s\+\(.*\)/\1/p')
  comp_type=$(echo ${comp_type} | sed 's/^ *//g' | sed 's/ *$//g')
  if [[ "${comp_type}" == "lib" ]]; then
    if contains "${ignore_list[@]}" "${comp_name}"; then
	  echo "${comp_name} LIB Component as source"
	else
      echo "${comp_name} sources can be removed"
      comp_dir=$( dirname ${comp} )
      cd ${comp_dir}
      rm -rf lib
      echo "${comp_name} directory ${comp_dir}"
    fi
  fi
done

# Special packaging for Videantis
if [[ $customer = "videantis" ]]; then
	echo "Retain multimedia/pp as source for $customer"
	cd ${BRCM_SDK_ROOT}/multimedia/pp
	git reset --hard HEAD
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/avcd/
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/avce/
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/dewarp/
	rm -rf ${BRCM_SDK_ROOT}/prebuilt/bcm89103/lib/*/libbcm89103_evk_avce.a
	rm -rf ${BRCM_SDK_ROOT}/prebuilt/bcm89107/lib/*/libbcm89107_evk_avcd.a
	rm -rf ${BRCM_SDK_ROOT}/prebuilt/bcm89107/lib/*/libbcm89107_evk_avce.a
else
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/include/sp.h
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/include/h264_hienc.h
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/include/osil/vid_h264_enc.h
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/include/osil/h264comp.h
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/include/osil/dewarpcomp.h
	rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/include/vid_lowlevelif.h
fi

echo "Delete tests/unit and tests/autotest from all gits"
find ${BRCM_SDK_ROOT} -name unit | grep tests | xargs rm -rf
find ${BRCM_SDK_ROOT} -name autotest | grep tests | xargs rm -rf
gitlist=( $(find ${BRCM_SDK_ROOT} -name .git -type d) )

echo "Delete .gitignore files"
$(find ${BRCM_SDK_ROOT} -type f -name '.gitignore' -exec rm -rf {} \;)

echo "Delete .repo directory"
rm -rf ${BRCM_SDK_ROOT}/.repo

echo "Delete .git directory"
$(find ${BRCM_SDK_ROOT} -type d -name '.git' -exec rm -rf {} +)

echo "Clean up each directory to remove un-necessary files for release"

rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/openvx/os/config/brcm_openvx
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/openvx/os/config/setup.py
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/tests/qualification/ppdma/
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/avce/avc_encoder
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/avcd/avc_decoder
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/ldc/dewarp_old
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/ldc/experiments

rm -rf ${BRCM_SDK_ROOT}/bootloader/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/bootloader/build/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/bootloader/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/bootloader/build/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/bootloader/comms/inet/src/stack
rm -rf ${BRCM_SDK_ROOT}/bootloader/security
rm -rf ${BRCM_SDK_ROOT}/system/init/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/system/init/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/system/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/system/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/system/include/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/drivers/mcu/lib/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/drivers/mcu/inc/osil/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/drivers/mcu/inc/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/drivers/mcu/doc/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/drivers/mcu/lib/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/drivers/mcu/inc/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/drivers/mcu/inc/osil/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/drivers/include/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/drivers/include/osil/bcm8953x

rm -rf ${BRCM_SDK_ROOT}/build/all
rm -rf ${BRCM_SDK_ROOT}/build/bcm8950x
rm -rf ${BRCM_SDK_ROOT}/build/bcm8953x
rm -rf ${BRCM_SDK_ROOT}/build/bcm8956x
rm -rf ${BRCM_SDK_ROOT}/build/bcm8910x/deprecated
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/erika/pkg/mcu/bcm8953x
#rm -rf ${BRCM_SDK_ROOT}/build/common/dwnld_img_creator.py

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/unit_tests
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/examples
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/config_scripts
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/batchfiles
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/testcase
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/old

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/altera_nios2
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/atmel_atmega
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/atmel_atxmega
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/atmel_sam3
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5643l
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5644a
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5668
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/freescale_mpc5674f
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/hs12xs
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/infineon_common_tc2Yx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/infineon_tc26x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/infineon_tc27x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/mico32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/microchip_dspic
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/microchip_pic32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/msp430
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/nordic_nrf51x22
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/nxp_lpcxpresso_lpc12xx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/renesas_r5f5210x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/samsung_ks32c50100
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/st_sta2051
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/st_stm32_stm32f4xx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/tc179x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/ti_stellaris_lm4f232xxxx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/unibo_mparm
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/x86

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/8910x_prodesign_v1

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/8953x
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/8950x

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/common/src/bcm_timer.c
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/common/src/cortex_rx_mpu.c
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/common/src/cortex_rx_startup.S

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/common/inc/bcm_timer.h
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/mcu/bcm/common/inc/cortex_rx_mpu.h

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/arduino_uno
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/arm_evaluator7t
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/atmel_stk500
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/atmel_stk600
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/axiom_mpc5674fxmb
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ee_easylab
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ee_flex
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ee_miniflex
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/esi_risc
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/fpg-eye
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/freescale_xpc564xl
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/hs12xs_demo9s12xsfame
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/infineon_tc1775b
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/infineon_tc1796b
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/infineon_TriBoard_TC2X5
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ipermob_board_v2
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ipermob_db_pic32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ipermob_mb_pic32
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/lattice_xp2_ev_board
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/microchip_dspicdem11plus
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/microchip_esk
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/microchip_explorer16
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/msp430
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/nordic_pca
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/renesas_rskrx210
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/ti_stellaris_lm4f232xxxx
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/twrs12g128
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/unibo_mparm
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/utmost
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/xbow_mib5x0

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/bcm/89100A0_prodesign
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/bcm/89103_palladium
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/bcm/8910x_prodesign_v1
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/bcm/989501r
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/pkg/board/bcm/989530c1

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/amazing
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/arduino
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/as
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/atmel
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/atmel802_15_4
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/cal
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/console
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/drivers
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/ieee802154
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/lwip
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/memory
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/microchip
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/misc
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/MiWiP2P
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/MiWiP2Pv2
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/network
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/nordic
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/nxp
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/qpc_v521
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/readme.txt
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/scicos
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/st
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/StellarisWare

rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/bcm/drivers
rm -rf ${BRCM_SDK_ROOT}/erika-sdk/contrib/bcm/utils

rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/cfg
rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/bin/pt_img_creator
rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/include
rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/scripts/deprecated

rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/src/
rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/bin/
rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/images/
rm -rf ${BRCM_SDK_ROOT}/tools/bcm8910x/scripts/write.py
rm -rf ${BRCM_SDK_ROOT}/tools/common/mkfs.ubifs
rm -rf ${BRCM_SDK_ROOT}/tools/common/rdbgen
rm -rf ${BRCM_SDK_ROOT}/tools/common/ubinize
rm -rf ${BRCM_SDK_ROOT}/tools/common/map_parser

rm -rf ${BRCM_SDK_ROOT}/applications/osek/
rm -rf ${BRCM_SDK_ROOT}/applications/vidxmtr720p60
rm -rf ${BRCM_SDK_ROOT}/applications/csitx_bayer_stream

rm -rf ${BRCM_SDK_ROOT}/test
#remove automation git
rm -rf ${BRCM_SDK_ROOT}/automation
#demo related packaging changes
rm -rf ${BRCM_SDK_ROOT}/demo
rm -rf ${BRCM_SDK_ROOT}/syscfg
find ${BRCM_SDK_ROOT} -name bcm8956x | xargs rm -rf
find ${BRCM_SDK_ROOT} -name bcm8953x | xargs rm -rf
find ${BRCM_SDK_ROOT} -name bcm8908x | xargs rm -rf

find ${BRCM_SDK_ROOT}/build/bcm8910x/board/ -name demo.mk | xargs rm -rf
find ${BRCM_SDK_ROOT}/build/bcm8910x/board/ -name ut.mk | xargs rm -rf
#End demo related changes
#remove *_req.c files
find ${BRCM_SDK_ROOT} -name *_req.c -not -path "${BRCM_SDK_ROOT}/erika-sdk/*" | xargs rm -rf
#End remove *_req.c files

rm -rf ${BRCM_SDK_ROOT}/multimedia/camera/utils
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/controller/lib/gmac.c
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/controller/lib/eth_time_v2.c
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/transceiver/lib/sgmii.c
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/switch/lib/eth_switch.c
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/switch/lib/switch_drv.c
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/switch/lib/cfp_drv.c
rm -rf ${BRCM_SDK_ROOT}/ethernet/1722/utils/host/src/brcm_1722_rcvr.c
#Delete lwip
rm -rf ${BRCM_SDK_ROOT}/ethernet/tcp_ip/lwip_1_4_1

#rm -rf ${BRCM_SDK_ROOT}/common/include/bcm8910x/sysmap.h
rm -rf ${BRCM_SDK_ROOT}/common/include/bcm8910x/bcm8910x_rdb.h

# Delete the input RDB file
rm -rf ${BRCM_SDK_ROOT}/system/init/bcm8910x/rdb
rm -rf ${BRCM_SDK_ROOT}/system/bcm8910x/rdb

rm -rf ${BRCM_SDK_ROOT}/cpu/arm/lib/cortex_rx
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/ipc/comp.mk
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/ipc/tests
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/ipc/os
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/switch/config
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/switch/os
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/switch/tests
rm -rf ${BRCM_SDK_ROOT}/ethernet/drivers/switch/comp.mk
rm -rf ${BRCM_SDK_ROOT}/ethernet/common/rpc/os

# Remove avb apps source
rm -rf ${BRCM_SDK_ROOT}/applications/avb_switch
#rm -rf ${BRCM_SDK_ROOT}/ethernet/1722/tests/qualification/avbcomp_test
#rm -rf ${BRCM_SDK_ROOT}/ethernet/1722/tests/qualification/avbtalkerlib_test
#rm -rf ${BRCM_SDK_ROOT}/ethernet/1722/tests/qualification/avblistenerlib_test

# remove empty wireless folder
#Delete RTP related application sources and RTP media component sources
#rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidxmtr_wifi
#rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidrcvr_wifi
#rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidrcvr
#rm -rf ${BRCM_SDK_ROOT}/applications/rtpvidxmtr
#rm -rf ${BRCM_SDK_ROOT}/ethernet/tcp_ip/tests/qualification/rtpslib_test
#rm -rf ${BRCM_SDK_ROOT}/ethernet/tcp_ip/tests/qualification/rtpcomp_test
#rm -rf ${BRCM_SDK_ROOT}/ethernet/tcp_ip/rtp
#rm -rf ${BRCM_SDK_ROOT}/ethernet/tcp_ip/rtp/os/erika/receiver

#demo related packaging changes
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/ppr
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/pgx
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/tests/qualification/pgxlib_test
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/tests/qualification/pprlib_test
rm -rf ${BRCM_SDK_ROOT}/prebuilt/bcm89103/lib/*/libbcm89103_ppr.a
rm -rf ${BRCM_SDK_ROOT}/prebuilt/bcm89107/lib/*/libbcm89107_ppr.a
rm -rf ${BRCM_SDK_ROOT}/prebuilt/bcm89103/lib/*/libbcm89103_pgx.a
rm -rf ${BRCM_SDK_ROOT}/prebuilt/bcm89107/lib/*/libbcm89107_pgx.a

rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/ldc/dewarp
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/stitch/top_view
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/stitch/downsize2x2
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/avcd/h264_intra_decoder
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/avce/h264_intra_encoder
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/tutorials
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/math_lib
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/tests/qualification/math_lib_example
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/tests/integration/math_lib_regression
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/pixel_processor
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/pplib/sp
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/pplib/lowlevellib
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/ppr/preproc
rm -rf ${BRCM_SDK_ROOT}/multimedia/pp/tests/unit/vmp_mbox_simple

cd ${BRCM_SDK_ROOT}/build/

# Verify that build compiles fine with source package.
for eachcompiler in ${compiler_list[@]}
do
  make clean
  make family=${prod_family} cust=1 SW_IMAGE_TYPE=Rel ${eachcompiler}=1
done

make clean
#run doxygen build to generate documentation
make family=${prod_family} doc cust=1 release=${release} safety=${safety}
#remove process git after generating doxygen documentation
rm -rf ${BRCM_SDK_ROOT}/process
rm -rf ${BRCM_SDK_ROOT}/build/doc.mk
rm -rf ${BRCM_SDK_ROOT}/build/bcm8910x/bcm8910x_doc.mk
#remove all docref folders after generating documentation
echo "remove all docref folders after generating documentation"
find ${BRCM_SDK_ROOT} -name docref | xargs rm -rf
find ${BRCM_SDK_ROOT} -name doc | grep -v erika-sdk | grep -v out | xargs rm -rf

cd ${BRCM_SDK_ROOT}
