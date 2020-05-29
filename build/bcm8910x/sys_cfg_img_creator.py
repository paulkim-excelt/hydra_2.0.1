#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2017 Broadcom Limited.
# This program is the proprietary software of Broadcom Limited
# and/or its licensors, and may only be used, duplicated, modified
# or distributed pursuant to the terms and conditions of a separate,
# written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized
# License, Broadcom grants no license (express or implied), right
# to use, or waiver of any kind with respect to the Software, and
# Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE
# NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
# IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
# ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of
# Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
# PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
# OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
# INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
# ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
# TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#

import sys
import getopt
import os
import struct

cfg = None
mac_addr_size = 6
final_img = None
sys_cfg_img = None
sys_cfg_magic = 0x53434647 # 'S' 'C' 'F' G'

# ******************************************************************************

def write_data(tag):

    with open(cfg) as cfg_file:
        for entry in cfg_file:
            if tag in entry:
                value = entry.split()
                if value[0] == "MAC_ADDR":
                    byte = value[1].split(":")
                    for i in range(mac_addr_size):
                        data = struct.pack('B', int(byte[i], 16))
                        sys_cfg_img.write(data)
                else:
                    data = struct.pack('I', int(value[1], 16))
                    sys_cfg_img.write(data)

# ******************************************************************************

def create_img():

    global sys_cfg_img

    sys_cfg_img = open(final_img, "wb")
    data = struct.pack('I', sys_cfg_magic)
    sys_cfg_img.write(data)
    write_data("PHY_CONN_MODE")
    write_data("MAC_ADDR")
    sys_cfg_img.close()

# ******************************************************************************

def validate_args():

    # Check whether the file exists
    if not os.path.isfile(cfg):
        print("%s file doesn't exist" % cfg)
        sys.exit(1)

# ******************************************************************************

def usage():
    print("./sys_cfg_img_creator.py --cfg <system cfg text> --out \
<output file name>")

# ******************************************************************************

def main():

    global cfg
    global final_img

    try:
        opts, args = getopt.getopt(sys.argv[1:], "-h", ["cfg=", "out="])

    except:
        print("Error in parsing arguments")
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-h":
            usage()
            sys.exit(1)
        elif opt == "--cfg":
            cfg = arg
        elif opt == "--out":
            final_img = arg
        else:
            print("Unknown argument %s" % opt)
            sys.exit(1)

    if not cfg:
        print("--cfg required")
        usage()
        sys.exit(1)
    if not final_img:
        print("--out required")
        usage()
        sys.exit(1)

    # Validate arguments
    validate_args()

    # Create final image
    create_img()

# ******************************************************************************

main()
