#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2019 Broadcom Limited.
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


class vdec_config_t:
    def __init__(self, width, height, bytesPerPixel, totalMPCores, streamMPCores, numSPCores, frameRate):
        self.version = sys.getsizeof(vdec_config_t) - 4
        self.width = width
        self.height = height
        self.bytesPerPixel = bytesPerPixel
        self.totalMPCores = totalMPCores
        self.streamMPCores = list(map(int, str(streamMPCores).split()))
        self.numSPCores = numSPCores
        self.frameRate = list(map(int, str(frameRate).split()))

    def write(self, outfile):
        fil = open(outfile,'wb')
        fil.write(struct.pack('I',self.version))
        fil.write(struct.pack('I',self.width))
        fil.write(struct.pack('I',self.height))
        fil.write(struct.pack('I',self.bytesPerPixel))
        fil.write(struct.pack('I',self.totalMPCores))
        fil.write(struct.pack('I',self.streamMPCores[0]))
        fil.write(struct.pack('I',self.streamMPCores[1]))
        fil.write(struct.pack('I',self.streamMPCores[2]))
        fil.write(struct.pack('I',self.streamMPCores[3]))
        fil.write(struct.pack('I',self.numSPCores))
        fil.write(struct.pack('I',self.frameRate[0]))
        fil.write(struct.pack('I',self.frameRate[1]))
        fil.write(struct.pack('I',self.frameRate[2]))
        fil.write(struct.pack('I',self.frameRate[3]))
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

        vdec = vdec_config_t(cfg.width, cfg.height, cfg.bytesPerPixel, cfg.totalMPCores, cfg.streamMPCores, cfg.numSPCores, cfg.frameRate)
        vdec.write(outfile)
        print outfile+' is generated'

# ******************************************************************************
def usage():
    print("python vdec_config.py -i <input> -o <output> -s <schema>")
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