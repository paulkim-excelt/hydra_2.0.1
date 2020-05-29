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


class venc_config_t:
    def __init__(self, width, ht, fps, bps, quality, prof, entropycoding, intraperiod, hdrperiod, numcores):
        self.magic_number = 0x434E4556
        self.version = sys.getsizeof(venc_config_t) - 4
        self.width = width
        self.height = ht
        self.fps = fps
        self.bitrate = bps
        self.profile = prof
        self.quality = quality
        self.entropycoding = entropycoding
        self.intraperiod = intraperiod
        self.hdrperiod = hdrperiod
        self.numcores = numcores

    def write(self, outfile):
        fil = open(outfile,'wb')
        fil.write(struct.pack('I', self.magic_number))
        fil.write(struct.pack('I',self.version))
        fil.write(struct.pack('I',self.width))
        fil.write(struct.pack('I',self.height))
        fil.write(struct.pack('I',self.profile))
        fil.write(struct.pack('I',self.entropycoding))
        fil.write(struct.pack('I',self.fps))
        fil.write(struct.pack('I',self.bitrate))
        fil.write(struct.pack('I',self.intraperiod))
        fil.write(struct.pack('I',self.quality))
        fil.write(struct.pack('I',self.hdrperiod))
        fil.write(struct.pack('I',self.numcores))
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

        venc = venc_config_t(cfg.width, cfg.height, cfg.framerate, cfg.bitrate, cfg.quality, cfg.profile, cfg.entropycoding, cfg.intraperiod, cfg.hdrperiod, cfg.numcores)
        venc.write(outfile)
        print outfile+' is generated'

# ******************************************************************************
def usage():
    print("python venc_config.py -i <input> -o <output> -s <schema>")
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