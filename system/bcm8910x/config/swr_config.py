#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2017-2018 Broadcom Limited.
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

##  @defgroup grp_swr_cfg_config SWR Configuration Generator
##  @ingroup grp_swr_cfg
##
##  @addtogroup grp_swr_cfg_config
##  @{
##  @section sec_swr_cfg_config_overview Overview
##  TBD
##
##  @limitations None
##
##  @file swr_config.py
##  @brief SWR Configurator
##  This script file contains the configurator implementation for SWR
##  @version 0.30 Initial version
##

import sys
import getopt
import os
import struct
import binascii
import lxml

from lxml import etree
from lxml import objectify

##  @brief Generate byte array
##
##  This function generates the bytearray bytearray representation
##  for the given integer.
##
##  @behavior Sync, Re-entrant
##
##  @pre None
##
##  @param[in]      entry       Integer to be converted into bytearray
##  @param[in]      size        Size of the bytearray (in bytes)
##
##  @return     Bytearray representation of the given integer
##
##  @post None
##
##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
##
##  @limitations any limitations for this api
def getByteArray(entry, size):
    if size == 64:
        data = struct.pack('Q', entry)
    elif size == 16:
        data = struct.pack('H', entry)
    else:
        data = struct.pack('I', entry)
    return bytearray(data)

##  @brief Schema for #SYSCFG_MMIWriteType
##
##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
SYSCFG_MMIWriteTypeSchemaGlobal = """
<xs:complexType name="mmiConfigType">
    <xs:attribute name="paddr" type="xs:string" use="required"/>
    <xs:attribute name="raddr" type="xs:string" use="required"/>
    <xs:attribute name="value" type="xs:string" use="required"/>
</xs:complexType>"""

##  @brief Schema for #SYSCFG_SWRConfigType
##
##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
SYSCFG_SWRConfigTypeSchemaGlobal = """
<xs:element name="swr_config">
    <xs:complexType>
        <xs:sequence>
            <xs:element name="mmi" type="mmiConfigType"  maxOccurs="32"/>
        </xs:sequence>
    </xs:complexType>
</xs:element>"""

##  @brief MMI Configurator
##
##  MMI configurator class for generating MMI configuration binary
##
##  @trace #BRCM_SWREQ_SYSCFG_MMI_WRITE_TYPE
class MMIConfig:
    ##  @brief Instantiation operation
    ##
    ##  Creates a new instanace of the class
    ##
    ##  @behavior Sync, Non-reentrant
    ##
    ##  @pre TBD
    ##
    ##  @param[in]      self        #MMIConfig object
    ##  @param[in]      cfg         Objectify tree for #SYSCFG_MMIWriteType
    ##
    ##  Return values are documented in reverse-chronological order
    ##  @return     An instance of this class
    ##
    ##  @post TBD
    ##
    ##  @trace #BRCM_SWREQ_SYSCFG_MMI_WRITE_TYPE
    ##
    ##  @limitations None
    def __init__(self, cfg):
        ## @brief PHY address
        self.paddr = int(str(cfg.attrib['paddr']), 16)
        ## @brief Register Address
        self.raddr = int(str(cfg.attrib['raddr']), 16)
        ## @brief value to be programmed
        self.value = int(str(cfg.attrib['value']), 16)
    ##  @brief Generate binary data
    ##
    ##  This function generates the binary data corressponding
    ##  #SYSCFG_MMIWriteType.
    ##
    ##  @behavior Sync/Async, Re-entrant/Non-reentrant
    ##
    ##  @pre TBD
    ##
    ##  @param[in]      self        #MMIConfig object
    ##
    ##  @return     MMI write configuration in binary format
    ##
    ##  @post TBD
    ##
    ##  @trace #BRCM_SWREQ_SYSCFG_MMI_WRITE_TYPE
    ##
    ##  @limitations None
    ##
    def getData(self):
        data = bytearray()
        data.append(self.paddr)
        data.append(self.raddr)
        data += getByteArray(self.value, 16)
        return data

##  @brief SWR Configurator
##
##  SWR configurator class for generating SWR configuration binary
##
##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
class SYSCFG_SWRConfigurator:
    ##  @brief Instantiation operation
    ##
    ##  Creates a new instanace of the class
    ##
    ##  @behavior Sync, Non-reentrant
    ##
    ##  @pre TBD
    ##
    ##  @param[in]      self        #MMIConfig object
    ##  @param[in]      xmlFile     XML file containing SWR configuration
    ##
    ##  @return     An instance of this class
    ##
    ##  @post TBD
    ##
    ##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
    ##
    ##  @limitations None
    def __init__(self, xmlFile):
        ## @brief SWR magic number (#SWR_CONFIG_MAGIC)
        self.magic = 0x43525753
        ## @brief SWR configuration version number
        self.version = 0x00000100
        ## @brief SWR configuration XML file
        self.xmlFile = xmlFile
        local = """<?xml version="1.0"?>
                   <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">"""
        local += SYSCFG_MMIWriteTypeSchemaGlobal
        local += SYSCFG_SWRConfigTypeSchemaGlobal
        local += """</xs:schema>"""
        schema = etree.XMLSchema(objectify.fromstring(local))
        ## @brief Validating parser for the schema
        self.parser = objectify.makeparser(schema = schema)
        fp = open(xmlFile, 'r')
        xml = fp.read()
        fp.close()
        ## @brief Objectify tree for the SWR configuration
        self.swrCfg = objectify.fromstring(xml, self.parser)
        ## @brief MMI configuration
        self.mmicfg = []
        for mmi in self.swrCfg.mmi:
            self.mmicfg.append(MMIConfig(mmi))
    ##  @brief Generate binary data
    ##
    ##  This function generates the binary data corressponding to XML file
    ##  SYSCFG_SWRConfigurator::xmlFile
    ##
    ##  @behavior Sync/Async, Re-entrant/Non-reentrant
    ##
    ##  @pre TBD
    ##
    ##  @param[in]      self        #SYSCFG_SWRConfigurator object
    ##
    ##  @return     SWR configuration in binary format
    ##
    ##  @post TBD
    ##
    ##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
    ##
    ##  @limitations None
    ##
    def getData(self):
        data = bytearray()
        if (len(self.mmicfg) > 32):
            return data
        data += getByteArray(self.magic, 32)
        data += getByteArray(self.version, 32)
        data += getByteArray(len(self.mmicfg), 32)
        data += getByteArray(0, 32) #Reserved
        for each in self.mmicfg:
            data += each.getData()
        for i in range(len(self.mmicfg),32):
            data += bytearray(4)
        name = str(os.path.basename(self.xmlFile))
        if len(name) > 128:
            name = name[0:126]
            name += bytearray(1)
        else:
            name += bytearray(128-len(name))
        data += name
        return data

##  @brief Print usage for SWR Configurator
##
##  @behavior Sync, Non-reentrant
##
##  @param          void
##
##  @return void
##
##  @post Application exits
##
##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
##
##  @limitations None
def usage():
    print("./xml_port_config.py -i <input XML file> -o <output binary file>")
    sys.exit(1)

##  @brief main function for SWR Configurator
##
##  @behavior Sync, Re-entrant
##
##  @pre TBD
##
##  @param[in]      void
##
##  @return void
##
##  @post TBD
##
##  @trace #BRCM_SWREQ_SYSCFG_SWR_CONFIG_TYPE
##
##  @limitations None
def main():
    input_file = None
    output_file = None
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hi:o:",
                                    ["help=", "input=", "output="])
    except:
        print("Error in parsing arguments")
        usage()
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
        elif opt in ("-i", "--input"):
            input_file = arg
        elif opt in ("-o", "--output"):
            output_file = arg
        else:
            print("Unknown argument %s" % opt)
            usage()
    if ((input_file == None) or (output_file == None)):
        usage()

    configurator = SYSCFG_SWRConfigurator(input_file)
    fp = open(output_file, 'wb')
    fp.write(configurator.getData())
    fp.close
    del configurator

if __name__ == '__main__':
    main()
## @}
