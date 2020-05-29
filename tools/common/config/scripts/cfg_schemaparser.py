#!/usr/bin/python2

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
# CORRESPONDENCE TO nameRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
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
####################################################################################

##  @defgroup grp_config_parser Schema parser
#   @ingroup  grp_config
#
#   @addtogroup grp_config_parser
#   @{
#   @section sec_configgui_parser_overview Overview
#   Python schema file parser
#
#   @limitations The schema parser will not support the below mentioned features of schema
#   -# form[qualified/unqualified], complexContent[extension], simpleContent, ID and other attributes
#   -# XSD attributes, use[optional/prohibited/required]
#   -# Complex Types with Mixed Content
#   -# Group indicator
#   -# If Order indicators(sequence, all and choice) have properties like minOccurs and maxOccurs
#   -# any and anyAttribute
#   -# Element Substitution
#
#   @file cfg_schemaparser.py
#   @brief Python file for Schema Parser
#   @version 0.1 Initial Version

import re, os
import xml.etree.ElementTree as et
from lxml import etree

##  @name Component Architecture IDs
#   @{
#   @brief Architecture IDs for schema parser

    ## @brief #cfg_schemaparser.CFG_DataType
BRCM_SWARCH_CFG_DATA_TYPE_CLASS             = 0
    ## @brief #cfg_schemaparser.CFG_SchemaSyntaxError
BRCM_SWARCH_CFG_SCHEMA_SYNTAX_ERROR_CLASS   = 1
    ## @brief #cfg_schemaparser.CFG_SchemaParseError
BRCM_SWARCH_CFG_SCHEMA_PARSE_ERROR_CLASS    = 2
    ## @brief #cfg_schemaparser.CFG_SchemaParser
BRCM_SWARCH_CFG_SCHEMA_PARSER_CLASS         = 3
    ## @brief #cfg_schemaparser.CFG_Test()
BRCM_SWARCH_CFG_TEST_PROC                   = 4
    ## @brief #cfg_schemaparser.CFG_PrintChildren()
BRCM_SWARCH_CFG_PRINT_CHILD_PROC            = 5
##  @}

## @brief Class data type
#
# This class will take each node of the xml schema and depending on its properties and type
# it links globally declared element types with local types. This class will also give
# the current node children, properties and position with given path from its top root
# element to till the current node as a list.
#
# @trace #BRCM_SWREQ_CFG_SCHEMA_PARSER
class CFG_DataType:
    def __init__(self, aNode, aNameSpaceLen, aAlias, aSchema, aParent):
        self.node = aNode
        self.tagName = None
        self.properties = {}
        self.nameSpaceLen = aNameSpaceLen
        self.alias = aAlias
        self.parent = aParent
        self.name = None
        self.typeObj = []
        self.children = []
        if self.node is not None:
            self.__contruct__(aSchema)

    def __contruct__(self, aSchema):
        self.tagName = self.node.tag[self.nameSpaceLen:]
        self.properties = self.node.attrib
        # if name attribute is present store that
        if 'name' in self.properties.keys():
            self.name = self.properties['name']
            self.properties.pop('name', None)

        # If its (simpleType or complexType) and has name attribute then its a type
        if (self.tagName == 'simpleType' or self.tagName == 'complexType') and self.name is not None:
            aSchema.addDataType(self.name, self)
            self.parent = None

        # if type attribute is present store that
        if 'type' in self.properties.keys():
            self.__addTypeName__(self.properties['type'])
            self.properties.pop('type', None)

        if self.tagName == 'list':
            self.parent.properties['elementType'] = 'list'
            if 'itemType' in self.properties: # Because itemType and simpleType child are mutually exclusive
                self.__addTypeName__(self.properties['itemType'])
                self.properties.pop('itemType', None)
            else:
                for child in self.node.getchildren():
                    obj = CFG_DataType(child, self.nameSpaceLen, self.alias, aSchema, self)
                    obj.__setName__('local' + '_' + obj.tagName)
                    self.__addTypeObj__(obj)
        elif self.tagName == 'union':
            if 'memberTypes' in self.properties.keys():
                for typeName in self.properties['memberTypes'].split():
                    self.__addTypeName__(typeName)
                self.properties.pop('memberTypes', None)
            for child in self.node.getchildren():
                obj = CFG_DataType(child, self.nameSpaceLen, self.alias, aSchema, self)
                obj.__setName__('local' + '_' + obj.tagName)
                self.__addTypeObj__(obj)
        elif self.tagName == 'annotation':
            for child in self.node.getchildren():
                self.parent.__addProperty__('documentation', child.text.strip())
        elif self.tagName == 'restriction':
            # parse completely. Take care of 'base' and children 'values'
            # Add the 'base' as 'type' for the parent
            if 'base' in self.properties.keys():
                self.parent.__addTypeName__(self.properties['base'])

            # All the children would be properties of the parent
            for child in self.node.getchildren():
                self.parent.__addProperty__(child.tag[self.nameSpaceLen:], child.attrib['value'])
        else:
            for child in self.node.getchildren():
                obj = CFG_DataType(child, self.nameSpaceLen, self.alias, aSchema, self)
                if obj.parent is not None and obj.tagName != 'restriction' and obj.tagName != 'unique':
                    if obj.tagName == 'simpleType' or obj.tagName == 'complexType':
                        obj.__setName__('local' + '_' + obj.tagName)
                        self.__addTypeObj__(obj)
                    else:
                        self.children.append(obj)

    def __addTypeName__(self, aType):
        self.typeObj.append(aType)

    def __addTypeObj__(self, aTypeObj):
        self.typeObj.append(aTypeObj)

    def __setName__(self,aName):
        self.name = aName

    def __addProperty__(self, aKeyName, aValue):
        if aKeyName in self.properties.keys():
            if aKeyName == 'documentation':
                self.properties['documentation'] = self.properties['documentation'] + aValue
            else:
                self.properties[aKeyName].append(aValue)
        else:
            if aKeyName == 'enumeration':
                self.properties[aKeyName] = []
                self.properties[aKeyName].append(aValue)
            else:
                self.properties[aKeyName] = aValue

    def link(self,aSchema):
        # for all the typenames get the objects
        for i,child in enumerate(self.typeObj):
            if not isinstance(child,CFG_DataType):
                self.typeObj[i] = aSchema.getDataType(child)

        for child in self.typeObj:
            child.link(aSchema)

        for child in self.children:
            child.link(aSchema)

    def getName(self):
        return self.name

    def getTypes(self):
        typeList = []
        for child in self.typeObj:
            typeList.append(child)
            tempList = child.getTypes()
            if len(tempList):
                typeList.pop()
                typeList.extend(tempList)
        for child in self.children:
            if child.getName() is None:
                typeList.extend(child.getTypes())
        return typeList

    def getChildren(self):
        childList = []
        for child in self.typeObj:
            childList.extend(child.getChildren())
        for child in self.children:
            if child.getName() is not None:
                childList.append(child)
            else:
                childList.extend(child.getChildren())

        return childList

    def getCompleteProperties(self, aDict):
        for child in self.typeObj:
            prop = child.getProperties()
            childProp = child.getCompleteProperties({})
            if len(childProp.keys()):
                for key in childProp.keys():
                    idx = 1
                    name = key
                    while name in aDict.keys():
                        name = key + ':'+ str(idx)
                        idx = idx + 1
                    aDict[name] = childProp[key]
                    aDict[name].update(prop)
            else:
                idx = 1
                name = child.getName()
                while name in aDict.keys():
                    name = child.getName() + ':'+ str(idx)
                    idx = idx + 1
                aDict[name] = dict(prop)

        for child in self.children:
            if child.getName() is None:
                childProp = child.getCompleteProperties({})
                if len(childProp.keys()):
                    for key in childProp.keys():
                        idx = 1
                        name = key
                        while name in aDict.keys():
                            name = key + ':'+ str(idx)
                            idx = idx + 1
                        aDict[name] = childProp[key]
        return aDict

    def getProperties(self):
        return dict(self.properties)

    def getActiveChildren(self, aChildList):
        childList = []

        for child in self.typeObj:
            if child.tagName == 'complexType':
                childList.extend(child.getActiveChildren(aChildList))

        for child in self.children:
            if child.getName() is not None:
                if child in aChildList:
                    maxOccurs = 1
                    if 'maxOccurs' in child.properties:
                        if child.properties['maxOccurs'] != 'unbounded':
                            maxOccurs = int(child.properties['maxOccurs'])
                        else:
                            maxOccurs = float('inf')

                    if maxOccurs > aChildList.count(child):
                        childList.append(child)
                else:
                    childList.append(child)

            else:
                # If choice select one, else all
                if child.tagName == 'choice':
                    children = []
                    found = False
                    for gChild in child.children:
                        if gChild.getName() is not None:
                            if gChild in aChildList:
                                found = True
                                break
                        else:
                            tempChildList = gChild.getChildren()
                            if (set(tempChildList) & set(aChildList)):
                                found = True
                                children = gChild.getActiveChildren(aChildList)
                                break

                    if not found:
                        children = child.getActiveChildren(aChildList)

                    childList.extend(children)
                else:
                    childList.extend(child.getActiveChildren(aChildList))

        return childList

    def getActivePosition(self, aChildUsed, aSelection, aChildAllowed):

        for child in self.typeObj:
            if child.tagName == 'complexType':
                return child.getActivePosition(aChildUsed, aSelection, aChildAllowed)

        parsedChild = []
        for child in self.children:
            if child.getName() is not None:
                if child in aChildAllowed:
                    if child is aSelection:
                        usedParsedChild = [temp for temp in aChildUsed if temp in parsedChild]
                        return len(usedParsedChild)
                    else:
                        parsedChild.append(child)
            else:
                childList = child.getChildren()
                if aSelection in childList:
                    usedParsedChild = [temp for temp in aChildUsed if temp in parsedChild]
                    usedChild = [temp for temp in aChildUsed if temp not in parsedChild]
                    position = child.getActivePosition(usedChild, aSelection, childList)
                    return position + len(usedParsedChild)
                else:
                    parsedChild.extend(childList)

    def getCurNode(self, aPath):
        if len(aPath) == 0:
            return self
        else:
            for child in self.getChildren():
                if child.getName() == aPath[0]:
                    return child.getCurNode(aPath[1:])
            return None

## @brief Class Schema Syntax Error
#
#   This class is created by subclassing Exception. This is used to raise user defined exception when
#   there is error in the syntax of schema.
#
# @trace #BRCM_SWREQ_CFG_SCHEMA_PARSER
class CFG_SchemaSyntaxError(Exception):
    def __init__(self, aSchemaFileName, aSyntaxError):
        self.schemaFile = aSchemaFileName
        self.syntaxError = aSyntaxError
    def __str__(self):
        return('Syntax Error in File: ' + repr(self.schemaFile) + '\n' + repr(self.syntaxError))

## @brief Class Schema Parse Error
#
#   This class is created by subclassing Exception. This is used to raise user defined exception when
#   there is error while parsing the schema.
#
# @trace #BRCM_SWREQ_CFG_SCHEMA_PARSER
class CFG_SchemaParseError(Exception):
    def __init__(self, aSchemaFileName, aParseError):
        self.schemaFile = aSchemaFileName
        self.parseError = aParseError
    def __str__(self):
        return('Parse Error in File: ' + repr(self.schemaFile) + '\n' + repr(self.parseError))

## @brief Class Schema Parser
#
#   This class will parse the schema file using xml ElementTree class.
#   And it constructs object of CFG_DataType class with root of schema
#   It provides interface for cfg_gui.py to get the children, type and properties of the current node.
#   It also gives the position of the choosen element to be inserted in treeview
#
# @trace #BRCM_SWREQ_CFG_SCHEMA_PARSER
class CFG_SchemaParser:
    def __init__(self, aSchemaFileName):
        self.schemaFile = aSchemaFileName
        try:
            doc = etree.parse(aSchemaFileName)
            schema = etree.XMLSchema(doc)
        except etree.XMLSyntaxError as syntaxError:
            raise CFG_SchemaSyntaxError(aSchemaFileName, syntaxError)
        except etree.XMLSchemaParseError as parseError:
            raise CFG_SchemaParseError(aSchemaFileName, parseError)

        eNameSpace = re.compile('(?<=\<)[A-Za-z0-9_]+\:schema.*xmlns:(?P<alias>[A-Za-z0-9_]+) *= *[\"\']{1}(?P<nameSpaceURI>.*?)[\"\'].*(?=\<\/[A-Za-z0-9_]+:schema\>)', flags = re.DOTALL)
        with open(self.schemaFile, 'r') as file:
            content = file.read()
        nameSpace = eNameSpace.search(content).group('nameSpaceURI')
        self.alias = eNameSpace.search(content).group('alias')
        self.nameSpaceLen = len('{' + nameSpace + '}')

        self.types = {}
        for child in self.xsdDataTypes():
            typeName = self.alias + ':' + child
            obj = CFG_DataType(None, self.nameSpaceLen, self.alias, None, None)
            obj.__setName__(child)
            self.addDataType(typeName, obj)

        tree = et.parse(self.schemaFile)
        root = tree.getroot()

        self.root = CFG_DataType(root, self.nameSpaceLen, self.alias, self, None)
        self.root.link(self)

    def getDataType(self, aType):
        if aType in self.types.keys():
            return self.types[aType]
        else:
            return None

    def addDataType(self, aType, aObj):
        if aType not in self.types.keys():
            self.types[aType] = aObj
            return True
        else:
            return False

    def xsdDataTypes(self):
        dataTypes = []
        dataTypes.append('byte')
        dataTypes.append('decimal')
        dataTypes.append('int')
        dataTypes.append('integer')
        dataTypes.append('long')
        dataTypes.append('negativeInteger')
        dataTypes.append('nonNegativeInteger')
        dataTypes.append('nonPositiveInteger')
        dataTypes.append('positiveInteger')
        dataTypes.append('short')
        dataTypes.append('unsignedLong')
        dataTypes.append('unsignedInt')
        dataTypes.append('unsignedShort')
        dataTypes.append('unsignedByte')
        dataTypes.append('boolean')
        dataTypes.append('string')
        dataTypes.append('normalizedString')
        dataTypes.append('token')
        dataTypes.append('date')
        dataTypes.append('dateTime')
        dataTypes.append('duration')
        dataTypes.append('gDay')
        dataTypes.append('gMonth')
        dataTypes.append('gMonthDay')
        dataTypes.append('gYear')
        dataTypes.append('gYearMonth')
        dataTypes.append('time')
        return dataTypes

    def xsdRestrictionConstraints(self):
        constraints = []
        constraints.append('enumeration')
        constraints.append('fractionDigits')
        constraints.append('length')
        constraints.append('maxExclusive')
        constraints.append('maxInclusive')
        constraints.append('maxLength')
        constraints.append('minExclusive')
        constraints.append('minInclusive')
        constraints.append('minLength')
        constraints.append('pattern')
        constraints.append('totalDigits')
        constraints.append('whiteSpace')
        return constraints

    def getChildren(self, aPath, aCurChild):
        curNode = self.root.getCurNode(aPath)
        if curNode is None:
            return None

        #curNode is the node where children are requested
        childList = []
        for curChild in aCurChild:
            for child in curNode.getChildren():
                if child.getName() == curChild:
                    childList.append(child)
                    break

        childList = curNode.getActiveChildren(childList)
        finalList = []
        for child in childList:
            finalList.append(child.getName())

        return finalList

    def getPosition(self, aPath, aCurChild, aSelection):
        curNode = self.root.getCurNode(aPath)
        if curNode is None:
            return None

        #curNode is the node where children are requested
        childList = []
        for curChild in aCurChild:
            for child in curNode.getChildren():
                if child.getName() == curChild:
                    childList.append(child)
                    break

        selection = None
        # check if the selection present in the allowed child
        children = curNode.getActiveChildren(childList)
        for child in children:
            if child.getName() == aSelection:
                selection = child
                break

        if selection is not None:
            return curNode.getActivePosition(childList, selection, children)
        else:
            return None

    def getTypes(self, aPath):
        curNode = self.root.getCurNode(aPath)
        if curNode is None:
            return None

        tempList = [node.getName() for node in curNode.getTypes()]

        tempDict = {} # idx to be used next

        newList = []
        for (i,temp) in enumerate(tempList):
            if tempList.count(temp) > 1:
                if temp in tempDict.keys():
                    idx = tempDict[temp]
                    temp = temp + ':' + str(idx)
                else:
                    idx = 0

                tempDict[tempList[i]] = idx + 1
            newList.append(temp)
        return newList

    def getNodeProperties(self, aPath):
        curNode = self.root.getCurNode(aPath)
        if curNode is None:
            return None

        return curNode.getProperties()

    def getTypeProperties(self, aPath):
        curNode = self.root.getCurNode(aPath)
        if curNode is None:
            return None

        return curNode.getCompleteProperties({})

##  @brief Test print of children
#
#   To test the functionality of schema parser
#
#   @behavior Sync, Re-entrant
#
#   @pre None
#
#   @param[in]      aTree       Object of CFG_SchemaParser class
#   @param[in]      aLevel      Level of child element with respect to its root element
#   @param[in]      aChild      List of elements from the selected element to its top parent element
#
#   @return void
#
#   @post We should see the print of root element of the schema and its children with their properties
#            and type
#
#   @trace #BRCM_SWREQ_CFG_SCHEMA_PARSER
#
#   @limitations None
#
def CFG_PrintChildren(aTree, aLevel, aChild):
    existingChildren = []
    level = aLevel
    for child in aTree.getChildren(aChild, existingChildren):
        print '\t'*level, child, aTree.getPosition(aChild, existingChildren, child), aTree.getTypes(aChild+ [child]), aTree.getTypeProperties(aChild+ [child]), aTree.getNodeProperties(aChild+ [child])
        CFG_PrintChildren(aTree, level + 1, aChild + [child])

##  @brief Test function for schema parser
#
#   To test the functionality of schema parser
#
#   @behavior Sync/Async, Re-entrant/Non-reentrant
#
#   @pre            None
#
#   @param[in]      void
#
#   @return         void
#
#   @post parsing of schema should be successful, We should see the print of xml schema
#           elements with thier children and properties
#
#   @trace #BRCM_SWREQ_CFG_SCHEMA_PARSER
#
#   @limitations None
#
def CFG_Test():
    sample = '''<?xml version="1.0" encoding="UTF-8" ?>
                <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">

                <xs:simpleType name="sizeType">
                    <xs:union>
                        <xs:simpleType>
                            <xs:restriction base="xs:integer">
                                <xs:minInclusive value="2"/>
                                <xs:maxInclusive value="18"/>
                            </xs:restriction>
                        </xs:simpleType>
                        <xs:simpleType>
                            <xs:restriction base="xs:string">
                                <xs:enumeration value="small"/>
                                <xs:enumeration value="medium"/>
                                <xs:enumeration value="large"/>
                            </xs:restriction>
                        </xs:simpleType>
                    </xs:union>
                </xs:simpleType>

                <xs:simpleType name="sizeMemTypes">
                    <xs:union memberTypes="dressSizeType">
                        <xs:simpleType>
                            <xs:restriction base="xs:string">
                                <xs:enumeration value="small"/>
                                <xs:enumeration value="medium"/>
                                <xs:enumeration value="large"/>
                            </xs:restriction>
                        </xs:simpleType>
                    </xs:union>
                </xs:simpleType>
                <xs:simpleType name="dressSizeType">
                    <xs:restriction base="xs:integer">
                        <xs:minInclusive value="2"/>
                        <xs:maxInclusive value="18"/>
                    </xs:restriction>
                </xs:simpleType>
                <xs:simpleType name = "payloadFormatType">
                    <xs:restriction base = "xs:string">
                        <xs:enumeration value = "FORMAT_UNDEFINED"/>
                        <xs:enumeration value = "FORMAT_FU_A_SLICE"/>
                        <xs:enumeration value = "FORMAT_MPEGTS"/>
                    </xs:restriction>
                </xs:simpleType>

                <xs:simpleType name="itemBrandType">
                    <xs:restriction base = 'xs:string'>
                        <xs:enumeration value = "Apple"/>
                        <xs:enumeration value = "DELL"/>
                    </xs:restriction>
                </xs:simpleType>

                <xs:complexType name="payloadType">
                    <xs:sequence>
                        <xs:element name="payloadFormat" type="payloadFormatType"/>
                        <xs:element name="streamID" type="xs:unsignedInt"/>
                        <xs:element name="remotePort">
                            <xs:complexType>
                                <xs:sequence>
                                    <xs:element name="remotePortID" type="xs:unsignedInt"/>
                                </xs:sequence>
                            </xs:complexType>
                        </xs:element>
                        <xs:element name="simpleTypeEx">
                            <xs:simpleType>
                                <xs:restriction base="xs:string">
                                    <xs:pattern value="ox[0-9A-Fa-f]{9}"/>
                                </xs:restriction>
                            </xs:simpleType>
                        </xs:element>
                    </xs:sequence>
                </xs:complexType>

                <xs:simpleType name="fixedp_type_u4.12_list_len_1089_t">
                  <xs:restriction base="fixedp_type_u4.12_list_t">
                    <xs:minLength value="1"/>
                    <xs:maxLength value="1089"/>
                  </xs:restriction>
                </xs:simpleType>

                <xs:simpleType name="fixedp_type_u4.12_list_t">
                  <xs:list itemType="fixedp_type_u4.12_t"/>
                </xs:simpleType>

                <xs:simpleType name="fixedp_type_u4.12_t">
                  <xs:restriction base="xs:unsignedShort"/>
                </xs:simpleType>

                <xs:element name="sample_cfg">
                  <xs:complexType>
                    <xs:sequence>
                      <xs:element name="cv_b" type="fixedp_type_u4.12_list_len_1089_t" minOccurs="1" maxOccurs="1"/>
                      <xs:element name="orderperson" type="xs:string" default="unknown"/>
                      <xs:element name="itemConunt" type="xs:integer" default="5"/>
                      <xs:element name="location" type="xs:string" fixed="INDIA"/>
                      <xs:element name="distance" type="xs:integer" fixed="5000"/>
                      <xs:element name="loyal" type="xs:boolean" default="true"/>
                      <xs:element name="Achiever" type="xs:boolean" fixed="true"/>
                      <!-- For a string to be a valid default, the type
                      definition must be a simple type or a complex type with
                      mixed content and a particle emptiable. -->
                      <xs:element name="age" fixed="5">
                          <xs:simpleType>
                              <xs:restriction base="xs:integer">
                                  <xs:minInclusive value="1"/>
                                  <xs:maxInclusive value="10"/>
                              </xs:restriction>
                          </xs:simpleType>
                      </xs:element>
                      <xs:element name="age1" default="5">
                          <xs:simpleType>
                              <xs:restriction base="xs:integer">
                                  <xs:enumeration value="1"/>
                                  <xs:enumeration value="5"/>
                                  <xs:enumeration value="10"/>
                              </xs:restriction>
                          </xs:simpleType>
                      </xs:element>
                      <xs:element name="shipto" minOccurs="0" maxOccurs="2">
                        <xs:complexType>
                          <xs:sequence>
                            <xs:element name="name" type="xs:string"/>
                            <xs:element name="ID" type="xs:integer"/>
                            <xs:element name="address" type="xs:string"/>
                            <xs:element name="city" type="xs:string"/>
                            <xs:element name="country">
                                <xs:simpleType>
                                    <xs:restriction base="xs:string">
                                        <xs:enumeration value = "INDIA"/>
                                        <xs:enumeration value = "USA"/>
                                        <xs:enumeration value = "CHINA"/>
                                    </xs:restriction>
                                </xs:simpleType>
                            </xs:element>
                            <xs:element name="checkunion" type="sizeType"/>
                            <xs:element name="height">
                                <xs:simpleType>
                                    <xs:restriction base="xs:integer">
                                        <xs:minInclusive value="5"/>
                                        <xs:maxInclusive value="10"/>
                                    </xs:restriction>
                                </xs:simpleType>
                            </xs:element>
                          </xs:sequence>
                        </xs:complexType>
                      </xs:element>
                      <xs:element name="unionTest" minOccurs="0">
                        <xs:simpleType>
                          <xs:union>
                            <xs:simpleType>
                                <xs:restriction base='xs:unsignedShort'>
                                    <xs:minInclusive value="0"/>
                                    <xs:maxInclusive value="39999"/>
                                </xs:restriction>
                            </xs:simpleType>
                            <xs:simpleType>
                                <xs:restriction base='xs:unsignedShort'>
                                    <xs:minInclusive value="41000"/>
                                    <xs:maxInclusive value="65535"/>
                                </xs:restriction>
                            </xs:simpleType>
                            <xs:simpleType>
                                <xs:restriction base='xs:unsignedShort'>
                                    <xs:minInclusive value="40000"/>
                                    <xs:maxInclusive value="40090"/>
                                </xs:restriction>
                            </xs:simpleType>
                          </xs:union>
                        </xs:simpleType>
                      </xs:element>
                      <xs:element name="item" minOccurs="0" maxOccurs="unbounded">
                        <xs:complexType>
                          <xs:sequence>
                            <xs:element name="title" type="xs:string"/>
                            <xs:element name="note" type="xs:string" minOccurs="0"/>
                            <xs:element name="quantity" type="xs:positiveInteger"/>
                            <xs:element name="price" type="xs:decimal"/>
                            <xs:element name="brand">
                                <xs:complexType>
                                    <xs:sequence>
                                        <xs:element name="itemBrand" type="itemBrandType"/>
                                        <xs:element name="complexItem">
                                            <xs:complexType>
                                                <xs:sequence>
                                                    <xs:element name="itemName" type="xs:string"/>
                                                </xs:sequence>
                                            </xs:complexType>
                                        </xs:element>
                                        <xs:element name="simpleItem">
                                            <xs:simpleType>
                                                <xs:restriction base="xs:string">
                                                    <xs:pattern value="0x[0-9A-Fa-f]{8}"/>
                                                </xs:restriction>
                                            </xs:simpleType>
                                        </xs:element>
                                    </xs:sequence>
                                </xs:complexType>
                            </xs:element>
                            <xs:element name="color">
                                <xs:complexType>
                                    <xs:sequence>
                                        <xs:element name="complexColor">
                                            <xs:complexType>
                                                <xs:sequence>
                                                    <xs:element name="complexColorName" type="xs:string"/>
                                                </xs:sequence>
                                            </xs:complexType>
                                        </xs:element>
                                    </xs:sequence>
                                </xs:complexType>
                            </xs:element>
                          </xs:sequence>
                        </xs:complexType>
                      </xs:element>
                      <xs:element name="payload" type="payloadType" minOccurs="0"/>
                      <xs:element name="unionForSize" type="sizeType" minOccurs="0"/>
                      <xs:element name="unionMemType" type="sizeMemTypes" minOccurs="0"/>
                      <xs:element name="checkForGetRoot" minOccurs="0">
                        <xs:complexType>
                            <xs:sequence>
                                <xs:element name="sample_cfg">
                                    <xs:complexType>
                                        <xs:sequence>
                                            <xs:element name="name" type="xs:string"/>
                                            <xs:element name="ID" type="xs:string"/>
                                            <xs:element name="place" type="xs:string"/>
                                            <xs:element name="sample_cfg">
                                                <xs:complexType>
                                                    <xs:sequence>
                                                        <xs:element name="address">
                                                            <xs:simpleType>
                                                                <xs:restriction base="xs:string">
                                                                    <xs:pattern value="[0-9A-Fa-f]{20}"/>
                                                                </xs:restriction>
                                                            </xs:simpleType>
                                                        </xs:element>
                                                        <xs:element name="sample_cfg">
                                                            <xs:simpleType>
                                                                <xs:restriction base="xs:string">
                                                                    <xs:enumeration value = "first"/>
                                                                    <xs:enumeration value = "second"/>
                                                                </xs:restriction>
                                                            </xs:simpleType>
                                                        </xs:element>
                                                    </xs:sequence>
                                                </xs:complexType>
                                            </xs:element>
                                        </xs:sequence>
                                    </xs:complexType>
                                </xs:element>
                            </xs:sequence>
                        </xs:complexType>
                      </xs:element>
                      <xs:element name="checkOrderType" minOccurs="0">
                        <xs:complexType>
                            <xs:choice>
                                <xs:sequence>
                                    <xs:element name="option1" type="xs:string" maxOccurs="2"/>
                                    <xs:element name="option2" type="xs:string"/>
                                    <xs:sequence>
                                        <xs:choice>
                                            <xs:element name="a" type="xs:string"/>
                                            <xs:element name="b" type="xs:string"/>
                                        </xs:choice>
                                    </xs:sequence>
                                    <xs:choice>
                                        <xs:element name="c" type="xs:string"/>
                                        <xs:element name="d" type="xs:string"/>
                                    </xs:choice>
                                </xs:sequence>
                                <xs:sequence>
                                    <xs:element name="option3" type="xs:string"/>
                                    <xs:element name="option4" type="xs:string"/>
                                </xs:sequence>
                                <xs:element name="option" type="xs:string"/>
                            </xs:choice>
                        </xs:complexType>
                      </xs:element>
                    </xs:sequence>
                  </xs:complexType>
                  <xs:unique name="UniqueID">
                    <xs:selector xpath="shipto"/>
                    <xs:field xpath="ID"/>
                  </xs:unique>
                  <xs:unique name="UniqueName">
                    <xs:selector xpath="shipto"/>
                    <xs:field xpath="name"/>
                  </xs:unique>
                </xs:element>
                </xs:schema>'''

    import tempfile
    temp = tempfile.NamedTemporaryFile('w', delete=False)
    temp.write(sample)
    temp.close()

    try:
        treeObj = CFG_SchemaParser(temp.name)
    except Exception as error:
        print error
    else:
        CFG_PrintChildren(treeObj, 0, [])
    finally:
        os.remove(temp.name)
        print 'done'

if __name__ == '__main__':
    CFG_Test()
## @}
