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

import getopt
import sys
import os
import struct
import binascii
import tempfile
input_file = None
final_img = None
dwnld_img = None
edc = 0
img_magic_num = 0x494D4748 #'I''M''G''H'
flash_sector_size = 4 * 1024
edc_crc = 0x1

# *****************************************************************************
def getByteArray(entry, size):
    if size == 64:
        data = struct.pack('Q', entry)
    elif size == 16:
        data = struct.pack('H', entry)
    else:
        data = struct.pack('I', entry)
    return bytearray(data)

# *****************************************************************************

class dwnld_hdr_t:
    def __init__(self):
        self.magic_number = img_magic_num
        self.img_size = 0
        self.flags = 0
        self.err_det_code = 0
    def get_data(self):
        data = getByteArray(self.magic_number, 32)
        data += getByteArray(self.img_size, 32)
        data += getByteArray(self.flags, 32)
        data += getByteArray(self.err_det_code, 64)
        return data
    def get_size(self):
        return 20

# *****************************************************************************

def create_img(infile, outfile, hdr_len):
    tempfileh = tempfile.TemporaryFile()
    infileh = open(infile, "rb")
    outfileh = open(outfile, "wb")

    img_stat = os.stat(infile)

    dwnld_hdr = dwnld_hdr_t()
    dwnld_hdr.img_size = img_stat.st_size + hdr_len - dwnld_hdr.get_size()
    dwnld_hdr.flags = edc


    print("hdr_len:", hdr_len)
    pad_size = hdr_len - dwnld_hdr.get_size()
    print("pad_size:", pad_size)

    if hdr_len > 0:
        for i in range (0, pad_size/4):
            tempfileh.write(getByteArray(0x0, 32))

    message = infileh.read()
    while message != '':
        tempfileh.write(message)
        message = infileh.read()

    tempfileh.seek(0)

    crc = binascii.crc32(tempfileh.read(), 0xFFFFFFFF) & 0xFFFFFFFF;
    print("crc", crc)
    dwnld_hdr.err_det_code = crc

    tempfileh.seek(0)

    outfileh.write(dwnld_hdr.get_data())
    data = tempfileh.read()
    while data != '':
        outfileh.write(data)
        data = tempfileh.read()

    tempfileh.close()
    infileh.close()
    outfileh.close()


# *****************************************************************************

def validate_args():

    # Check whether the image exists
    if not os.path.isfile(input_file):
        print("%s file doesn't exist" % input_file)
        sys.exit(1)

# *****************************************************************************

def usage():
    print("./dwnld_img_creator.py --infile <input file name> \
            --outfile <output file name> --edc <err det code> --hdrlen <total header len>")

# *****************************************************************************

def main():

    global input_file
    global final_img
    global edc
    global hdr_len
    hdr_len = 0
    edc_input = 0

    try:
        opts, args = getopt.getopt(sys.argv[1:], "-h", ["infile=", "outfile=",
            "edc=", "hdrlen="])

    except:
        print("Error in parsing arguments")
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-h":
            usage()
            sys.exit(1)
        elif opt == "--infile":
            input_file = arg
        elif opt == "--outfile":
            final_img = arg
        elif opt == "--edc":
            edc = int(arg, 16)
            edc_input = 1
        elif opt == "--hdrlen":
            hdr_len = int(arg, 10)
        else:
            print("Unknown argument %s" % opt)
            sys.exit(1)

    if not input_file:
        print("--in required")
        usage()
        sys.exit(1)
    if not final_img:
        print("--out required")
        usage()
        sys.exit(1)
    if not edc_input:
        print("--edc required")
        usage()
        sys.exit(1)

    validate_args()

    create_img(input_file, final_img, hdr_len)

# *****************************************************************************

main()
