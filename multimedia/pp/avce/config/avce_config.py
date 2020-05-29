#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2018 Broadcom Limited.
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
import binascii
import lxml

from lxml import etree
from lxml import objectify


class avce_config_t:
    def __init__(self, width, ht, fps, bps, prof, lvl, numcores, minqp, maxqp, sliceRow):
        #print sliceRow
        self.magic_number = 0x45435641
        self.version = sys.getsizeof(avce_config_t) - 4
        self.width = (width + 15) & (~15)
        self.height = (ht + 15) & (~15)
        self.cropLeft = 0
        self.cropRight = self.width - width
        self.cropTop = 0
        self.cropBottom = self.height - ht
        self.fps = fps
        self.bitrate = bps
        self.profile = prof
        self.level = lvl
        self.numcores = numcores
        self.minqp = minqp
        self.maxqp = maxqp
        self.sliceRow = sliceRow

    def write(self, outfile):
        fil = open(outfile,'wb')
        fil.write(struct.pack('I', self.magic_number))
        fil.write(struct.pack('I',self.version))
        fil.write(struct.pack('I',self.width))
        fil.write(struct.pack('I',self.height))
        fil.write(struct.pack('I',self.cropLeft))
        fil.write(struct.pack('I',self.cropRight))
        fil.write(struct.pack('I',self.cropTop))
        fil.write(struct.pack('I',self.cropBottom))
        fil.write(struct.pack('I',self.fps))
        fil.write(struct.pack('I',self.bitrate))
        fil.write(struct.pack('I',self.profile))
        fil.write(struct.pack('I',self.level))
        fil.write(struct.pack('I',self.numcores))
        fil.write(struct.pack('I',self.minqp))
        fil.write(struct.pack('I',self.maxqp))
        for i in range(len(self.sliceRow)):
            fil.write(struct.pack('I',self.sliceRow[i]))
        fil.close()

class XMLParser:
    def __init__(self, schemafile):
        self.repo = None
        self.dirs = []
        self.outdir = None
        self.schema = etree.XMLSchema(file = schemafile)
        self.parser = objectify.makeparser(schema = self.schema)
    def loadXML(self, file):
        fp = open(str(file),'r')
        xml = fp.read()
        fp.close
        try:
            brcm_cfg = objectify.fromstring(xml, self.parser)
        except:
            return None
        return brcm_cfg
    def processCompXML(self, compfile, outfile):
        cfg = self.loadXML(compfile)
        if cfg is None:
            print compfile + ' is not a valid xml file'
            return
        if (cfg.width & 0x1 != 0):
            print 'Width should be a mutliple of 2. flooring width to nearest multiple of 2.'
            cfg.width = (cfg.width>>1)<<1
        if (cfg.height & 0x1 != 0):
            print 'height should be a mutliple of 2. flooring height to nearest multiple of 2.'
            cfg.height = (cfg.height>>1)<<1
        sliceRow = []

        slices = str(cfg.slice_rows).split(',')

        if int(slices[0]) != 0:
            print 'first slice always starts with 0th Row'
            sliceRow.append(0)
        for i in range (len(slices)-1):
            if (int(slices[i]) > int(slices[i+1])):
                print 'slice order is jumbled. Not supported'
                print outfile+' cannot be generated'
                sys.exit()

        for slice in slices:
            sliceRow.append(int(slice))

        for i in range(128 - len(sliceRow)):
            sliceRow.append(0)
        avce = avce_config_t(cfg.width, cfg.height, cfg.framerate, cfg.bitrate, cfg.profile, cfg.level, cfg.numcores, cfg.minqp, cfg.maxqp, sliceRow)
        avce.write(outfile)
        print outfile+' is generated'

# ******************************************************************************
def usage():
    print("python avce_config.py -i <input> -o <output> -s <schema>")
    sys.exit(1)

compfiles = []
compOutfiles = []
schemafile = None
try:
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:s:",["help=", "input=", "output=", "schema="])
except:
    print("Error in parsing arguments")
    usage()
for opt, arg in opts:
    if opt in ("-h", "--help"):
        usage()
    elif opt in ("-i", "--input"):
        compfiles.append(arg)
    elif opt in ("-o", "--output"):
        compOutfiles.append(arg)
    elif opt in ("-s", "--schema"):
        schemafile = arg
    else:
        print("Unknown argument %s" % opt)
        usage()
if schemafile is None:
    print 'schema file is needed.. Exiting'
    sys.exit()
xmlParser = XMLParser(schemafile)
if len(compfiles) is not 0 and len(compOutfiles) is 0:
    pre, ext = os.path.splitext(compfiles[0])
    outfile = pre +'.bin'
    compOutfiles.append(outfile)
if len(compfiles) is 0:
    compOutfiles = []
    #parse the current folder for xml files
    for filename in os.listdir('.'):
        if not filename.endswith('.xml'): continue
        compfiles.append(filename)
        pre, ext = os.path.splitext(filename)
        outfile = pre +'.bin'
        compOutfiles.append(outfile)
for i,compfile in enumerate(compfiles):
    xmlParser.processCompXML(compfile,compOutfiles[i])
