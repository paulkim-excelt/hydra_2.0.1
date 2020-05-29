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

##  @defgroup grp_config_gui Configurator GUI
#   @ingroup  grp_config
#
#   @addtogroup grp_config_gui
#   @{
#   @section sec_config_gui_overview Overview
#   Python configurator gui
#
#   @limitations None
#
#   @file cfg_gui.py
#   @brief Python file for Configurator GUI
#   @version 0.1 Initial Version

import os, sys, re
import datetime
import copy
import tempfile
import subprocess
import shlex
import xml.etree.ElementTree as ET
from xml.dom import minidom
from PyQt4 import QtCore, QtGui
from lxml import etree
from cfg_schemaparser import CFG_SchemaParser

##  @name Component Architecture IDs
#   @{
#   @brief Architecture IDs for Configurator GUI

    ## @brief #cfg_gui.CFG_BRCMAllowedConfigSchema()
BRCM_SWARCH_CFG_BRCM_ALLOWED_CONFIG_SCHEMA_GLOBAL   = 0
    ## @brief #cfg_gui.CFG_BRCMCopyRight()
BRCM_SWARCH_CFG_BRCM_COPYRIGHT_GLOBAL               = 1
    ## @brief #cfg_gui.CFG_tempFileList()
BRCM_SWARCH_CFG_TEMPFILELIST_GLOBAL                 = 2
    ## @brief #cfg_gui.CFG_GUI
BRCM_SWARCH_CFG_GUI_CLASS                           = 3
    ## @brief #cfg_gui.CFG_Mainwindow
BRCM_SWARCH_CFG_MAIN_WINDOW_CLASS                   = 4
##  @}

##  @brief Board configuration schema
#
#   Schema for validating board configuration
#
#   @trace #BRCM_SWREQ_CFG_GUI
CFG_BRCMAllowedConfigSchema = '''<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">

<xs:simpleType name="schemaPath">
    <xs:restriction base="xs:string">
        <xs:pattern value="([0-9A-Za-z/_:.])*.xsd"/>
    </xs:restriction>
</xs:simpleType>

<xs:simpleType name="scriptPath">
    <xs:restriction base="xs:string">
        <xs:pattern value="([0-9A-Za-z/_:.])*.py"/>
    </xs:restriction>
</xs:simpleType>

<xs:complexType name="config">
    <xs:all>
        <xs:element name="schema" type="schemaPath" minOccurs="1" maxOccurs="1"/>
        <xs:element name="script" type="scriptPath" minOccurs="1" maxOccurs="1"/>
    </xs:all>
</xs:complexType>

<xs:element name="BRCM_Allowed_Cfg">
    <xs:complexType>
        <xs:sequence>
            <xs:element name = "component" type="config" maxOccurs="unbounded"/>
        </xs:sequence>
    </xs:complexType>
</xs:element>

</xs:schema>'''

##  @brief BoardCom copyright
#
#   BroadCom Copy right content
#
#   @trace #BRCM_SWREQ_CFG_GUI
CFG_BRCMCopyRight = '''
 Copyright ''' + str(datetime.datetime.now().year) + ''' Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
'''

##  @brief temporary file list
#
#   To remove all the temporary files when closing the gui window
#
#   @trace #BRCM_SWREQ_CFG_GUI
CFG_tempFileList = []

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

## @brief Class Configurator GUI
#
# This class creates an user iterface for editing the existing xml files and
# to create new XML files given the XML schema. And also this GUI generates
# bin files using parser script.
#
# @trace #BRCM_SWREQ_CFG_GUI
class CFG_GUI:
    def __init__(self, aApp):
        self.app = aApp
        self.index = None
        self.level = None
        self.action = None
        self.removeItem = None
        self.expandBranch = None
        self.collapseBranch = None
        self.createNewXML = None
        self.saveXMLAsAction = None
        self.generateBinAction = None
        self.openXMLAction = None
        self.duplicate = None
        self.itemObj = None
        self.mainMenu = None
        self.addMenu = None
        self.rootItemIndex = None
        self.validXmlFiles = 0
        self.isBoardCfg = None
        self.isDefaultSet = None
        self.actionDict = {}
        self.treeObjDict = {}
        self.xmlFileDict = {}
        self.xsdDataTypes = []
        self.xsdRestrictionConstraints = []
        self.rootElmtList = []
        self.saveIndices = []
        self.itemStack = []
        self.path = []

    def __setForegroundColor__(self, aLevel, aItem, aColor='red'):
        aItemIndex = aItem.index()
        for i in range(aLevel-1):
            if aItemIndex.parent().isValid():
                item = self.model.itemFromIndex(aItemIndex)
                item.setForeground(QtGui.QColor(aColor))
                aItemIndex = aItemIndex.parent()

    def __iterItems__(self, aRootItem, aColor=None):
        if aRootItem is not None:
            rootItemIndex = aRootItem.index()
            stack = [aRootItem]
            # creating root xml element
            xmlRoot = etree.Element(str(rootItemIndex.data().toString()))
            xmlStack = [xmlRoot]
            while stack:
                parent = stack.pop(0)
                parentElement = xmlStack.pop(0)
                for row in range(parent.rowCount()):
                    child = parent.child(row, 0)
                    index = child.index()
                    level = 0
                    while index.parent().isValid():
                        index = index.parent()
                        level += 1
                    if aColor is not None:
                        self.__setForegroundColor__(level, child, aColor)
                    # Adding subelements to xml root element
                    childElement = etree.SubElement(parentElement, str(child.text()))
                    if child.hasChildren():
                        stack.append(child)
                        xmlStack.append(childElement)
                    else:
                        childElement.text = str(child.data().toString())
        return xmlRoot

    def __writeXML__(self, aWidget):
        xmlRoot = self.__iterItems__(aWidget.item, 'black')
        dom = minidom.parseString(etree.tostring(xmlRoot))
        with open(aWidget.fileName, 'w') as file:
            domString = dom.toprettyxml('    ').replace(u'<?xml version="1.0" ?>',
                              u'<?xml version="1.0" ?>' + str("\n<!--" + CFG_BRCMCopyRight + "-->"))
            file.write(domString)

    def __validateXML__(self, aWidget):
        xmlRoot = self.__iterItems__(aWidget.item)
        dom = minidom.parseString(etree.tostring(xmlRoot))

        # write temporary xml file with treeview elements
        temp = tempfile.NamedTemporaryFile('w', delete=False)
        domString = dom.toprettyxml('    ')
        temp.write(domString)
        temp.close()

        with open(temp.name, 'r') as file:
            xmlDoc = etree.parse(file)
        with open(aWidget.schema, 'r') as file:
            schemaDoc = etree.parse(file)
        schema = etree.XMLSchema(schemaDoc)

        try:
            # validating xml against schema
            schema.assertValid(xmlDoc)
        except etree.DocumentInvalid as e:
            consoleMsg = 'Error in file: '+ aWidget.fileName + '\n' + str(e)
            self.__setConsoleMsg__(consoleMsg)
            return False
        else:
            consoleMsg = 'XML validation for '+ aWidget.fileName +' against the schema is successful.'
            self.__setConsoleMsg__(consoleMsg)
            return True
        finally:
            os.remove(temp.name)

    def __saveFileForGenBin__(self, aSaveBtnObj):
        if self.__validateXML__(aSaveBtnObj):
            saveMsg = "XML validation for " + aSaveBtnObj.fileName +" against the schema is successful. Are you sure you want to save the XML file?"
            reply = QtGui.QMessageBox.question(None, 'Message',
                         saveMsg, QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)
            if reply == QtGui.QMessageBox.Yes:
                self.__writeXML__(aSaveBtnObj)
                self.__setConsoleMsg__(aSaveBtnObj.fileName + ' saved successfully.')
            return reply
        else:
            return False

    def __saveFileForGenBinAll__(self, aSaveBtnObj):
        if self.__validateXML__(aSaveBtnObj):
            return True
        else:
            return False

    def __validateAndAddConfig__(self):
        allFiles = []
        self.xmlFileDict = {}
        # search for board configuration xml files in current working directory
        # and its sub directories
        for root, dirs, files in os.walk(os.getcwd()):
            for each in files:
                filePath = os.path.join(root, each)
                allFiles.append(filePath)
        for eachFile in allFiles:
            if eachFile.endswith('.xml'):
                with open(eachFile, 'r') as file:
                    try:
                        xmlDoc = etree.parse(file)
                    except Exception as error:
                        consoleMsg = 'Error in file ' + eachFile + '\n' + str(error)
                        self.__setConsoleMsg__(consoleMsg)
                    else:
                        # make temporary schema file with CFG_BRCMAllowedConfigSchema
                        schemaFile = tempfile.NamedTemporaryFile('w', delete=False)
                        schemaFile.write(CFG_BRCMAllowedConfigSchema)
                        schemaFile.close()
                        with open(schemaFile.name, 'r') as file:
                            doc = etree.parse(file)
                        schema = etree.XMLSchema(doc)

                        try:
                            # validate the xml file with CFG_BRCMAllowedConfigSchema
                            schema.assertValid(xmlDoc)
                        except etree.DocumentInvalid as error:
                            consoleMsg = 'Error in file ' + eachFile + '\n' + str(error)
                            self.__setConsoleMsg__(consoleMsg)
                        else:
                            fullXMLFileName = os.path.basename(eachFile)
                            xmlFileName = fullXMLFileName[:-4]
                            if xmlFileName in self.xmlFileDict.keys():
                                self.__setConsoleMsg__(eachFile + ' is already present under the current folder.'
                                    + '\n Duplicate entry of the file is not allowed.')
                            else:
                                # Maintaining config xml path with its file name to show in combo box
                                self.xmlFileDict[xmlFileName] = eachFile
                                self.validXmlFiles = self.validXmlFiles + 1
                                rootConfiguration = None
                                try:
                                    # To get access to model top level item
                                    root = self.treeView.model().invisibleRootItem()
                                    rootConfiguration = root.child(0,0).child(0,0).index().data().toString()
                                except:
                                    pass
                                if rootConfiguration == 'BRCM_Allowed_Configuration':
                                    self.isBoardCfg = True

                                allItems = [str(self.chipSelection.itemText(i)) for i in range(self.chipSelection.count())]
                                if xmlFileName not in allItems: # Add new config file to selection combo box
                                    self.chipSelection.blockSignals(True)
                                    self.chipSelection.insertItem(1, xmlFileName)
                                    self.chipSelection.blockSignals(False)
                                    allItems.append(str(eachFile))
                        finally:
                            os.remove(schemaFile.name)

        if self.validXmlFiles == 0:
            # Open board configuration when there are no valid xml files against BRCM_Allowed_Configuration
            allItems = [str(self.chipSelection.itemText(i)) for i in range(self.chipSelection.count())]
            self.isDefaultSet = True
            self.chipSelection.setCurrentIndex(allItems.index('Create Board Config'))
            self.isDefaultSet = False

    def __saveFile__(self, aWidget):
        if self.__validateXML__(aWidget):
            saveMsg = "XML validation for " + aWidget.fileName + " against the schema is successful. Are you sure you want to save the XML file?"
            reply = QtGui.QMessageBox.question(None, 'Message',
                         saveMsg, QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)
            if reply == QtGui.QMessageBox.Yes:
                self.__writeXML__(aWidget)
                self.__setConsoleMsg__(aWidget.fileName + ' saved successfully.')
            return reply
        else:
            quitMsg = "XML validation for " + aWidget.fileName + " against the schema is failed. Are you sure you want to save the XML file?"
            reply = QtGui.QMessageBox.critical(None, 'Message',
                         quitMsg, QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)
            if reply == QtGui.QMessageBox.Yes:
                self.__writeXML__(aWidget)
                self.__setConsoleMsg__(aWidget.fileName + ' saved successfully.')
            return reply

    def saveFileClicked(self):
        widget = self.app.sender()
        self.__saveFile__(widget)

    def __getQStandardItem__(self, aText=None):
        if aText is None:
            item = QtGui.QStandardItem()
        else:
            item = QtGui.QStandardItem(aText)
        item.setEditable(False)
        return item

    def saveXMLAs(self):
        itemIndex = self.treeView.selectedIndexes()[0]
        fileNameIndex = self.treeView.selectedIndexes()[2]
        saveBtnIndex = self.treeView.selectedIndexes()[3]
        fileItem = self.model.itemFromIndex(fileNameIndex)
        fileName = QtGui.QFileDialog.getSaveFileName(None, "Enter File Name")
        if fileName != '':
            fileItem.setText(fileName)
            saveBtnWidget = self.treeView.indexWidget(saveBtnIndex)
            saveBtnWidget.fileName = fileName
            self.__saveFile__(saveBtnWidget)

    def __xmlFileInvalid__(self, aSaveBtnObj):
        consoleMsg = 'Generated XML for ' + aSaveBtnObj.fileName + ' is invalid.'
        consoleMsg = consoleMsg + ' Bin file cannot be generated.'
        self.__setConsoleMsg__(consoleMsg)

    def __writeBin__(self, aSaveBtnObj):
        xmlFileName = aSaveBtnObj.fileName
        outFileName = aSaveBtnObj.fileName[:-3] + str('bin')
        schemaFile = aSaveBtnObj.schema
        pythonScript = aSaveBtnObj.script
        # Run python parser script to generate binary
        process = subprocess.Popen(shlex.split('python ' + str(pythonScript) + ' -i' + str(xmlFileName) + ' -o'
                     + str(outFileName) + ' -s' + str(schemaFile)), stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        # Show the output of the python parser script in console
        consoleMsg = 'For file ' + xmlFileName + ': \n' + str(process.communicate()[0])
        self.__setConsoleMsg__(consoleMsg)

    def generateBinFile(self):
        saveBtnIndex = self.treeView.selectedIndexes()[3]
        saveBtnObj = self.treeView.indexWidget(saveBtnIndex)
        reply = self.__saveFileForGenBin__(saveBtnObj)
        if reply == False:
            self.__xmlFileInvalid__(saveBtnObj)
        if reply == QtGui.QMessageBox.Yes:
            self.__writeBin__(saveBtnObj)

    def expandOrCollapse(self, aParentIndex, aEn):
        self.treeView.setExpanded(aParentIndex, aEn)
        for rowNum in range(0, self.treeView.model().rowCount(aParentIndex)):
            childIndex = self.treeView.model().index(rowNum, 0, aParentIndex)
            self.treeView.setExpanded(childIndex, aEn)
            self.expandOrCollapse(childIndex, aEn)

    def removeElement(self):
        try:
            # If the row we are removing is having save Button
            # Then we need to remove that widget from that from saveIndices list
            saveBtnWidgetIdx = self.treeView.selectedIndexes()[3]
        except IndexError:
            pass
        else:
            widget = self.treeView.indexWidget(saveBtnWidgetIdx)
            if widget in self.saveIndices:
                self.saveIndices.remove(widget)
        self.__setForegroundColor__(self.level, self.model.itemFromIndex(self.index))
        self.treeView.model().removeRow(self.index.row(), self.index.parent())

    def __setColorForLineEditChange__(self):
        widget = self.app.sender()
        widget.item.setData(widget.text())
        self.__setForegroundColor__(widget.level, widget.item)

    def __addTextData__(self, aTagItem, aDataItem, aLevel, aDefaultData=None):
        textWidget = QtGui.QLineEdit()
        textWidget.setFixedHeight(self.widgetHeight)
        textWidget.setPlaceholderText('Enter ' + str(aTagItem.index().data().toString()))
        if aTagItem.data().toString() != '':
            textWidget.setText(aTagItem.data().toString())
            aTagItem.setData(textWidget.text())
        else:
            if aDefaultData is not None:
                textWidget.setText(aDefaultData)
                aTagItem.setData(textWidget.text())
        self.treeView.setIndexWidget(aDataItem.index(), textWidget)
        # store item and level to set the foreground color
        textWidget.item = aTagItem
        textWidget.level = aLevel
        textWidget.textEdited.connect(self.__setColorForLineEditChange__)

    def __setColorForComboChange__(self):
        widget = self.app.sender()
        widget.item.setData(widget.currentText())
        self.__setForegroundColor__(widget.level, widget.item)

    def __addComboData__(self, aTagItem, aDataItem, aDataType, aLevel, aEnumList=None, aDefaultData=None):
        comboWidget = QtGui.QComboBox()
        comboWidget.setFixedHeight(self.widgetHeight)
        if aDataType == 'boolean':
            comboWidget.addItem('true')
            comboWidget.addItem('false')
            itemList = ['true', 'false']
        else:
            itemList = []
            for each in aEnumList:
                comboWidget.addItem(each)
                itemList.append(each)
        self.treeView.setIndexWidget(aDataItem.index(), comboWidget)
        if aTagItem.data().toString() != '':
            try:
                comboWidget.setCurrentIndex(itemList.index(str(aTagItem.data().toString())))
            except:
                # When there is union present, widget at this index might be a lineEdit
                # in that case itemList.index() will raise an exception
                pass
        else:
            if aDefaultData is not None:
                comboWidget.setCurrentIndex(itemList.index(str(aDefaultData)))
        aTagItem.setData(comboWidget.currentText())
        # store index and level to store data in the tag and to set the color
        comboWidget.item = aTagItem
        comboWidget.level = aLevel
        comboWidget.currentIndexChanged.connect(self.__setColorForComboChange__)

    def __makeListForOpenXML__(self, aRoot, aList):
        # get children of current xml tag
        children = aRoot.getchildren()
        for child in children:
            # make tuple with tag, text, childrenList
            tple = ()
            tple = tple + (child.tag,)
            if child.text is not None:
                if not child.text.startswith('\n'):
                    tple = tple + (child.text,)
                else:
                    tple = tple + (None,)
            else:
                tple = tple + (None,)
            newList = []
            tple = tple + (newList,)
            aList.append(tple)
            self.__makeListForOpenXML__(child, newList)

    def __getXMLTreeList__(self, aRoot):
        childList = []
        parentTuple = ()
        parentTuple = parentTuple + (aRoot.tag,)
        parentTuple = parentTuple + (None,)
        # update chilren for the root xml
        self.__makeListForOpenXML__(aRoot, childList)
        parentTuple = parentTuple + (childList,)
        treeList = []
        treeList.append(parentTuple)
        return treeList

    def changeWidget(self):
        comboWidget = self.app.sender()
        for each in comboWidget.itemProps.keys():
            if each == comboWidget.currentText():
                props = comboWidget.itemProps[each]
                if 'enumeration' in props.keys():
                    enumList = props['enumeration']
                    self.__addComboData__(comboWidget.item, comboWidget.itemData,'enumeration', comboWidget.level, enumList)
                else:
                    self.__addTextData__(comboWidget.item, comboWidget.itemData, comboWidget.level)
        self.__setForegroundColor__(comboWidget.level, comboWidget.item)

    def __addItemsforDuplicate__(self, aParent, aElements):
        for text, children, aType, dataWidget in aElements:
            row = []
            item = self.__getQStandardItem__(text)
            row.append(item)
            itemType = self.__getQStandardItem__()
            row.append(itemType)
            itemData = self.__getQStandardItem__()
            row.append(itemData)
            aParent.appendRow(row)
            if aType is not None:
                if str(aType).startswith('<PyQt4.QtGui.QComboBox'):
                    aType.item = item
                    aType.itemData = itemData
                    self.treeView.setIndexWidget(itemType.index(), aType)
                    aType.currentIndexChanged.connect(self.changeWidget)
                else:
                    labelWidget = QtGui.QLabel()
                    labelWidget.setText(aType.text())
                    labelWidget.setFixedHeight(self.widgetHeight)
                    self.treeView.setIndexWidget(itemType.index(), labelWidget)
            if str(dataWidget).startswith('<PyQt4.QtGui.'):
                self.treeView.setIndexWidget(itemData.index(), dataWidget)
                if str(dataWidget).startswith('<PyQt4.QtGui.QLabel'):
                    item.setData(dataWidget.text())
                elif str(dataWidget).startswith('<PyQt4.QtGui.QLineEdit'):
                    dataWidget.item = item
                    item.setData(dataWidget.text())
                    dataWidget.setPlaceholderText('Enter ' + str(dataWidget.item.index().data().toString()))
                    dataWidget.textEdited.connect(self.__setColorForLineEditChange__)
                elif str(dataWidget).startswith('<PyQt4.QtGui.QComboBox'):
                    dataWidget.item = item
                    item.setData(dataWidget.currentText())
                    dataWidget.currentIndexChanged.connect(self.__setColorForComboChange__)
            elif dataWidget is not None:
                itemData.setText(dataWidget)
                item.setData(dataWidget)
            self.__setForegroundColor__(self.level, item)
            if children:
                self.__addItemsforDuplicate__(item, children)

    def __makeListForDuplicate__(self, aStack, aListStack):
        itemStack = []
        listStack = []
        for parent in aStack:
            parentIndex = aStack.index(parent)
            aList = aListStack[parentIndex]
            for row in range(parent.rowCount()):
                # make tuple with text, children, aType, dataWidget
                tple = ()
                levelIndex = parent.child(row, 0).index()
                level = 0
                while levelIndex.parent().isValid():
                    levelIndex = levelIndex.parent()
                    level += 1
                for column in range(parent.columnCount()):
                    child = parent.child(row, column)
                    childIndex = child.index()
                    if self.treeView.indexWidget(childIndex) is not None:
                        widget = self.treeView.indexWidget(childIndex)
                        if str(widget).startswith('<PyQt4.QtGui.QLabel'):
                            labelWidget = QtGui.QLabel()
                            labelWidget.setText(str(widget.text()))
                            labelWidget.setFixedHeight(self.widgetHeight)
                            tple = tple + (labelWidget,)
                            labelWidget.level = level
                        elif str(widget).startswith('<PyQt4.QtGui.QLineEdit'):
                            lineEditWidget = QtGui.QLineEdit()
                            lineEditWidget.setText(str(widget.text()))
                            lineEditWidget.setFixedHeight(self.widgetHeight)
                            tple = tple + (lineEditWidget,)
                            lineEditWidget.level = level
                        elif str(widget).startswith('<PyQt4.QtGui.QComboBox'):
                            comboWidget = QtGui.QComboBox()
                            allItems = [str(widget.itemText(i)) for i in range(widget.count())]
                            for each in allItems:
                                comboWidget.addItem(each)
                            comboWidget.setCurrentIndex(allItems.index(widget.currentText()))
                            comboWidget.setFixedHeight(self.widgetHeight)
                            tple = tple + (comboWidget,)
                            comboWidget.level = level
                            if hasattr(widget, 'itemProps'):
                                comboWidget.itemProps = widget.itemProps

                        elif str(widget).startswith('<PyQt4.QtGui.QPushButton'):
                            pass
                    else:
                        # For QStandardItem with text
                        if str(childIndex.data().toString()) == '':
                            tple = tple + (None,)
                        else:
                            tple = tple + (str(childIndex.data().toString()),)

                    if child.hasChildren():
                        newList = []
                        tple = tple + (newList,)
                        # maintaining list stack to update with its children
                        listStack.append(newList)
                        itemStack.append(child)
                    # If child has no children then add empty list
                    if not child.hasChildren() and column == 0:
                        tple = tple + ([],)
                aList.append(tple)

        if itemStack == []:
            return
        else:
            return self.__makeListForDuplicate__(itemStack, listStack)

    def __duplicatePath__(self, aParent, aItemName, aRow):
        item = self.__getQStandardItem__(aItemName)
        itemType = self.__getQStandardItem__('string')
        itemPath = self.__getQStandardItem__()
        self.model.insertRow(aRow, aParent.index())
        aParent.setChild(aRow, 0, item)
        aParent.setChild(aRow, 1, itemType)
        aParent.setChild(aRow, 2, itemPath)
        self.__addBrowseForPath__(item, itemPath, self.level, None)
        self.__setForegroundColor__(self.level, item)

    def duplicateNode(self, aRoot):
        if aRoot is not None:
            stack = [aRoot]
            itemList = [[]]
            # update itemList with its text, children, aType, dataWidget
            self.__makeListForDuplicate__(stack, itemList)

        newList = []
        tple = ()
        itemIndex = self.treeView.selectedIndexes()[0]
        itemText = str(itemIndex.data().toString())
        labelWidgetIndex = self.treeView.selectedIndexes()[1]
        if self.treeView.indexWidget(labelWidgetIndex) is not None:
            labelWidget = self.treeView.indexWidget(labelWidgetIndex)
            itemType = labelWidget.text()
        else:
            itemType = str(labelWidgetIndex.data().toString())
        itemData = ''
        tple = tple + (itemText,)
        tple = tple + (itemList[0],)
        tple = tple + (itemType,)
        tple = tple + (itemData,)
        newList.append(tple)

        # insert a new row just below the current node
        self.model.insertRow(self.index.row()+1, self.index.parent())
        item = self.model.itemFromIndex(self.index.parent())
        newItem = self.__getQStandardItem__(itemText)
        newItemType = self.__getQStandardItem__()
        newItemData = self.__getQStandardItem__(itemData)
        item.setChild(self.index.row()+1, 0, newItem)
        item.setChild(self.index.row()+1, 1, newItemType)
        item.setChild(self.index.row()+1, 2, newItemData)
        labelWidget = QtGui.QLabel()
        labelWidget.setFixedHeight(self.widgetHeight)
        labelWidget.setText(itemType)
        self.treeView.setIndexWidget(newItemType.index(), labelWidget)

        self.__setForegroundColor__(self.level, newItem)
        root = self.treeView.model().invisibleRootItem()
        rootConfiguration = root.child(0,0).child(0,0).index().data().toString()
        if rootConfiguration == 'BRCM_Allowed_Configuration':
            self.__duplicatePath__(newItem, 'schema', 0)
            self.__duplicatePath__(newItem, 'script', 1)
        else:
            # Add elements to inserted row
            self.__addItemsforDuplicate__(self.model.itemFromIndex(newItem.index()), itemList[0])
        # Add toolTip
        obj = self.treeObjDict[self.itemStack[-1]]
        xmlRootIndex = self.__getStack__(newItem.index(), [], True)
        self.__addWidgets__(obj, xmlRootIndex, True)

    def __generateAllFiles__(self, aSaveBtnObj):
        reply = self.__saveFileForGenBinAll__(aSaveBtnObj)
        if reply == True:
            self.__writeBin__(aSaveBtnObj)
        else:
            self.__xmlFileInvalid__(aSaveBtnObj)

    def __addItems__(self, aParent, aElements):
        # populating all items in the treeview with its text, data and children
        for text, data, children in aElements:
            row = []
            item = self.__getQStandardItem__(text)
            row.append(item)
            itemType = self.__getQStandardItem__()
            row.append(itemType)
            if data is not None:
                itemData = self.__getQStandardItem__(data)
                item.setData(data)
            else:
                itemData = self.__getQStandardItem__()
            row.append(itemData)
            aParent.appendRow(row)
            self.__setForegroundColor__(self.level, item)
            if children:
                self.__addItems__(item, children)

    def __getCurrentChildList__(self, aItem):
        childList = []
        for row in range(aItem.rowCount()):
            childList.append(str(aItem.child(row, 0).index().data().toString()))
        return childList

    def __replacePath__(self, aTreeObj, aDoc):
        srcPath = re.compile('(?<=\<img).*src *= *"(?P<path>.[^"]*)".*(?=\>)')
        schemaPath = aTreeObj.schemaFile
        srcPaths = srcPath.findall(aDoc)
        for srcPath in srcPaths:
            count = srcPath.count('..')
            rootPath = ('/').join(schemaPath.split('/')[:-(count+1)])
            absPath = rootPath + '/' + ('/').join(srcPath.split('/')[count:])
            # replace only the first occurence of path
            aDoc = aDoc.replace(srcPath, absPath, 1)
        return aDoc

    def __getHTMLCode__(self, aTreeObj, aProps, aItemName, aIsRoot, aIsType):
        itemDoc = None
        if 'documentation' in aProps.keys():
            itemDoc = aProps['documentation']
            itemDoc = self.__replacePath__(aTreeObj, itemDoc)
            aProps.pop('documentation')

        table = ''
        # Adding type as the first table element to show the type in the begining.
        # Then we will be showing the other properties of the type otherwise the order
        # may change as we are having properties in dictionary
        if 'type' in aProps.keys():
            itemType = aProps['type']
            trStart = '<tr>'
            th = '<th>' + 'type' + '</th>'
            td = '<td>' + ': ' + str(aProps['type']) + '</td>'
            trEnd = '</tr>'
            table = table + trStart + th + td + trEnd
            aProps.pop('type')
        for each in aProps.keys():
            trStart = '<tr>'
            th = '<th>' + each + '</th>'
            td = '<td>' + ': ' + str(aProps[each]) + '</td>'
            trEnd = '</tr>'
            table = table + trStart + th + td + trEnd
        if aIsType is False and aIsRoot is False:
            if 'minOccurs' not in aProps.keys():
                trStart = '<tr>'
                th = '<th>' + 'minOccurs' + '</th>'
                td = '<td>' + ': 1' + '</td>'
                trEnd = '</tr>'
                table = table + trStart + th + td + trEnd
            if 'maxOccurs' not in aProps.keys():
                trStart = '<tr>'
                th = '<th>' + 'maxOccurs' + '</th>'
                td = '<td>' + ': 1' + '</td>'
                trEnd = '</tr>'
                table = table + trStart + th + td + trEnd

        if itemDoc is None:
            # Creating default documentation with properties when there is no documentation
            doc = '''
                <!DOCTYPE html>
                <html>
                </html>'''
            root = ET.fromstring(doc)
            if not aIsType:
                heading = ET.SubElement(root, 'h2')
                heading.text = aItemName
            body = ET.SubElement(root, 'body')
        else:
            root = ET.fromstring(itemDoc)
            if not aIsType:
                heading = ET.Element('h2')
                heading.text = aItemName
                root.insert(0, heading)
            body = root.find('body')
        tableObj = ET.SubElement(body, 'table', {"style":"width:100%"})
        tableObj.text = table
        string = ET.tostring(root)
        string = re.sub('&lt;', '<', string)
        string = re.sub('&gt;', '>', string)
        return string

    def __addWidgets__(self, aTreeObj, aItemIndex, aIsForDuplicate=False):
        item = self.model.itemFromIndex(aItemIndex.parent())
        if not aIsForDuplicate:
            rootItemName = str(aItemIndex.data().toString())
            rootItemType = aTreeObj.getTypeProperties([rootItemName]).keys()[0]
            for each in range(item.rowCount()):
                widget = QtGui.QLabel()
                widget.setText(rootItemType)
                self.treeView.setIndexWidget(item.child(each, 1).index(), widget)
                doc = ''
                rootNodeProps = aTreeObj.getNodeProperties([rootItemName])
                rootNodeProps['type'] = rootItemType
                rootTypeProps = aTreeObj.getTypeProperties([rootItemName])[rootItemType]
                doc += self.__getHTMLCode__(aTreeObj, rootNodeProps, rootItemName, True, False)
                doc += self.__getHTMLCode__(aTreeObj, rootTypeProps, rootItemName, True, True)
                item.child(each, 0).setToolTip(doc)
        # Stack of items to iterate
        stack = [item]
        # Stack of paths for each item
        pathStack = [[]]
        while stack:
            childItem = stack.pop(0)
            childPath = pathStack.pop(0)
            for i in range(childItem.rowCount()):
                path = copy.copy(childPath)
                # We just need to iterate in first column for xml tag and its data
                for j in range(1):
                    child = childItem.child(i,j)
                    path.append(str(child.index().data().toString()))
                    if aTreeObj.getChildren(path, []) is not None:
                        x = 0
                        # Get current child list from gui
                        curChildList = self.__getCurrentChildList__(child)
                        if curChildList != []:
                            for newChild in curChildList:
                                # If newchild type is not a simpleType
                                # or a simpleType without union
                                newChildPath = path + [newChild]
                                childItemIndex = child.child(x,0).index()
                                childTagItem = self.model.itemFromIndex(childItemIndex)
                                childTypeIndex = child.child(x,1).index()
                                childTypeItem = self.model.itemFromIndex(childTypeIndex)
                                if not aIsForDuplicate:
                                    childTypeItem.setText('')
                                childDataIndex = child.child(x,2).index()
                                childDataItem = self.model.itemFromIndex(childDataIndex)
                                level = len(path)+2
                                childItemName = str(childItemIndex.data().toString())
                                childNodeProps = aTreeObj.getNodeProperties(newChildPath)
                                propsDict = {}
                                doc = ''
                                childNodeProps['type'] = aTreeObj.getTypes(newChildPath)
                                doc += self.__getHTMLCode__(aTreeObj ,childNodeProps, childItemName, False, False)
                                if len(aTreeObj.getTypes(newChildPath)) == 0:
                                    childTagItem.setToolTip(doc)
                                elif len(aTreeObj.getTypes(newChildPath)) == 1:
                                    typeProps = aTreeObj.getTypeProperties(newChildPath)[aTreeObj.getTypes(newChildPath)[0]]
                                    doc += self.__getHTMLCode__(aTreeObj, typeProps, childItemName, False, True)
                                    childTagItem.setToolTip(doc)
                                    if not aIsForDuplicate:
                                        widget = QtGui.QLabel()
                                        widget.setText(aTreeObj.getTypes(newChildPath)[0])
                                        widget.setFixedHeight(self.widgetHeight)
                                        # set label widget for element type in column 1
                                        self.treeView.setIndexWidget(childTypeIndex, widget)
                                        if aTreeObj.getTypes(newChildPath)[0] in self.xsdDataTypes:
                                            childType = aTreeObj.getTypes(newChildPath)[0]
                                            childTypeProps = aTreeObj.getTypeProperties(newChildPath)[childType]
                                            # If element data is fixed then we will just add a QstandardItem
                                            # we dont need to set any widget there for the user to edit
                                            if 'fixed' in childNodeProps.keys():
                                                data = childNodeProps['fixed']
                                                childTagItem.setData(data)
                                                childDataItem.setText(data)
                                            else:
                                                defaultData = None
                                                data = str(childTagItem.data().toString())
                                                if data != '':
                                                    defaultData = data
                                                else:
                                                    if 'default' in childNodeProps.keys():
                                                        defaultData = childNodeProps['default']
                                                if childType != 'boolean':
                                                    if 'enumeration' in childTypeProps.keys():
                                                        enumList = childTypeProps['enumeration']
                                                        self.__addComboData__(childTagItem, childDataItem, 'enumeration', level, enumList, defaultData)
                                                    else:
                                                        root = self.treeView.model().invisibleRootItem()
                                                        rootConfiguration = root.child(0,0).child(0,0).index().data().toString()
                                                        if rootConfiguration == 'BRCM_Allowed_Configuration':
                                                            self.__addBrowseForPath__(childTagItem, childDataItem, level, defaultData, childTypeItem)
                                                        else:
                                                            self.__addTextData__(childTagItem, childDataItem, level, defaultData)
                                                else:
                                                    self.__addComboData__(childTagItem, childDataItem, 'boolean', level, None, defaultData)
                                else:
                                    for each in aTreeObj.getTypes(newChildPath):
                                        typeProps = aTreeObj.getTypeProperties(newChildPath)[each]
                                        typeProps['type'] = each
                                        doc += self.__getHTMLCode__(aTreeObj, typeProps, childItemName, False, True)
                                    childTagItem.setToolTip(doc)
                                    if not aIsForDuplicate:
                                        widget = QtGui.QComboBox()
                                        for each in aTreeObj.getTypes(newChildPath):
                                            widget.addItem(each)
                                        widget.setFixedHeight(self.widgetHeight)
                                        self.treeView.setIndexWidget(child.child(x,1).index(), widget)
                                        itemProps = aTreeObj.getTypeProperties(newChildPath)
                                        widget.itemProps = itemProps
                                        widget.item = child.child(x,0)
                                        widget.itemData = child.child(x,2)
                                        widget.level = level
                                        data = str(childTagItem.data().toString())
                                        isEnumPresent = False
                                        if data != '':
                                            for each in itemProps.keys():
                                                if 'enumeration' in itemProps[each].keys():
                                                    if data in itemProps[each]['enumeration']:
                                                        widget.setCurrentIndex(aTreeObj.getTypes(newChildPath).index(each))
                                                        isEnumPresent = True
                                                        break
                                            if isEnumPresent == True:
                                                for each in widget.itemProps.keys():
                                                    if each == widget.currentText():
                                                        props = widget.itemProps[each]
                                                        if 'enumeration' in props.keys():
                                                            enumList = props['enumeration']
                                                            self.__addComboData__(widget.item, widget.itemData,'enumeration', level, enumList)
                                                        else:
                                                            self.__addTextData__(widget.item, widget.itemData, level)
                                            else:
                                                self.__addTextData__(widget.item, widget.itemData, level)
                                        else:
                                            for each in widget.itemProps.keys():
                                                if each == widget.currentText():
                                                    props = widget.itemProps[each]
                                                    if 'enumeration' in props.keys():
                                                        enumList = props['enumeration']
                                                        self.__addComboData__(widget.item, widget.itemData,'enumeration', level, enumList)
                                                    else:
                                                        self.__addTextData__(widget.item, widget.itemData, level)
                                        widget.currentIndexChanged.connect(self.changeWidget)
                                x += 1
                    if child.hasChildren():
                        pathStack.append(path)
                        stack.append(self.model.itemFromIndex(child.index()))

    def openXMLFile(self):
        item = self.model.itemFromIndex(self.index)
        itemName = self.index.data().toString()
        xmlFile = str(QtGui.QFileDialog.getOpenFileName(None, "Select XML"))
        # If user selects any file then only process that file
        if xmlFile != '':
            try:
                tree = etree.parse(xmlFile)
            except Exception as error:
                self.__setConsoleMsg__('Error in File: ' + xmlFile + '\n' + str(error))
            else:
                root = tree.getroot()
                treeList = self.__getXMLTreeList__(root)
                # Remove _Configuration from its root configuration add _Cfg
                if itemName[:-14] + '_Cfg' == treeList[0][0]:
                    row = []
                    newItem = self.__getQStandardItem__(treeList[0][0])
                    # set key 'RootElement' to recognize it as xml root element
                    # This data 'RootElement' is set as invisible
                    newItem.setData('RootElement')
                    self.__setForegroundColor__(self.level, newItem)
                    row.append(newItem)
                    itemType = self.__getQStandardItem__()
                    row.append(itemType)
                    fileNameItem = self.__getQStandardItem__(xmlFile)
                    row.append(fileNameItem)
                    saveBtnItem = self.__getQStandardItem__()
                    row.append(saveBtnItem)
                    item.appendRow(row)
                    # Update xml root element with its children names and data
                    self.__addItems__(newItem, treeList[0][2])
                    saveBtnWidget = QtGui.QPushButton("Save")
                    saveBtnWidget.setFixedHeight(self.widgetHeight)
                    saveBtnWidget.item = newItem
                    saveBtnWidget.level = self.level
                    saveBtnWidget.fileName = xmlFile
                    saveBtnWidget.schema = self.treeObjDict[treeList[0][0]].schemaFile
                    saveBtnWidget.script = self.treeObjDict[treeList[0][0]].script
                    self.treeView.setIndexWidget(saveBtnItem.index(), saveBtnWidget)
                    self.saveIndices.append(saveBtnWidget)
                    saveBtnWidget.clicked.connect(self.saveFileClicked)
                    # Add widgets at each index of the populated tree from the xml
                    self.__addWidgets__(self.treeObjDict[treeList[0][0]], newItem.index())
                else:
                    consoleMsg = 'The choosen XML file ' + xmlFile +' is not valid under ' + itemName
                    self.__setConsoleMsg__(consoleMsg)

    def __makeDefaultChildren__(self, aTree, aStack, aList):
        existingChildren = []
        newStack = []
        newList = []

        while aStack:
            aChild = aStack.pop(0)
            aChildList = aList.pop(0)
            for child in aTree.getChildren(aChild, existingChildren):
                # Creating tuple with xml tagName, data, children
                tple = ()
                tple = tple + (child,)
                # By default we take data as empty string
                tple = tple + ('',)
                if aTree.getChildren(aChild + [child], []) != []:
                    childList = []
                    tple = tple + (childList,)
                    newStack.append(aChild + [child])
                    newList.append(childList)
                else:
                    tple = tple + ([],)
                aChildList.append(tple)
        if newStack != []:
            self.__makeDefaultChildren__(aTree, newStack, newList)
        return tple

    def createXML(self):
        item = self.model.itemFromIndex(self.index)
        newItemName = str(self.index.data().toString())
        newItem = self.__getQStandardItem__(newItemName[:-14] + '_Cfg')
        # set key 'RootElement' to recognize it as xml root element
        newItem.setData('RootElement')
        row = []
        row.append(newItem)
        fileName = QtGui.QFileDialog.getSaveFileName(None, "Enter File Name")
        if fileName != '':
            typeItem = self.__getQStandardItem__()
            row.append(typeItem)
            fileItem = self.__getQStandardItem__(fileName)
            row.append(fileItem)
            saveBtnItem = self.__getQStandardItem__('')
            row.append(saveBtnItem)
            item.appendRow(row)
            self.__setForegroundColor__(self.level+1, newItem)
            self.treeView.setExpanded(self.index, 1)
            saveBtnWidget = QtGui.QPushButton("Save")
            saveBtnWidget.setFixedHeight(self.widgetHeight)
            saveBtnWidget.item = newItem
            saveBtnWidget.level = self.level
            saveBtnWidget.fileName = fileName
            # storing schemaFile and script for each button so the button is clicked we
            # can validate and generate the created xml file
            saveBtnWidget.schema = self.treeObjDict[newItemName[:-14] + '_Cfg'].schemaFile
            saveBtnWidget.script = self.treeObjDict[newItemName[:-14] + '_Cfg'].script
            self.treeView.setIndexWidget(saveBtnItem.index(), saveBtnWidget)
            # Storing the saveBtnWidget to use it for saveAll
            self.saveIndices.append(saveBtnWidget)
            saveBtnWidget.clicked.connect(self.saveFileClicked)
            obj = self.treeObjDict[newItemName[:-14] + '_Cfg']
            defaultElements = self.__makeDefaultChildren__(obj, [[]], [[]])
            self.__addItems__(newItem, defaultElements[2])
            self.__addWidgets__(obj, newItem.index())

    def __getStack__(self, aIndex, aStack, aIsForDuplicate=False):
        while str(aIndex.data().toString()) not in self.treeObjDict.keys():
            # If the item at we are present is not in self.treeObjDict.keys() then
            # it will not be root item of xml. So, we will add that item
            # into the aStack
            aStack.append(str(aIndex.data().toString()))
            aIndex = aIndex.parent()

        item = self.model.itemFromIndex(aIndex)
        itemTag = str(aIndex.data().toString())
        itemData = str(item.data().toString())

        # Even if the item is in self.treeObjDict.keys() verify with
        # its key 'RootElement' to make sure that it is the xml root element
        if itemTag in self.treeObjDict.keys() and itemData == 'RootElement':
            aStack.append(itemTag)
            if not aIsForDuplicate:
                return aStack
            else:
                return aIndex
        else:
            aStack.append(itemTag)
            return self.__getStack__(aIndex.parent(), aStack)

    def __getItemTypes__(self, aObj):
        (isSimple, isComplex, isSimpleElement, isEmptyElement) = (False, False, False, False)

        if aObj.getTypes(self.itemStack[::-1]) is not None and aObj.getTypes(self.itemStack[::-1]) != []:
            itemTypes = aObj.getTypes(self.itemStack[::-1])
            if len(itemTypes) == 1:
                itemType = itemTypes[0]
                if itemType.startswith('local_complexType'):
                    isComplex = True
                elif itemType.startswith('local_simpleType'):
                    isSimple = True
                elif itemType in self.xsdDataTypes:
                    itemTypeProps = aObj.getTypeProperties(self.itemStack[::-1])
                    if itemTypeProps[itemType].keys() == []:
                        isSimpleElement = True
                    else:
                        for each in itemTypeProps[itemType].keys():
                            if each in self.xsdRestrictionConstraints:
                                isSimple = True
                                break
                        if isSimple is False:
                            isSimpleElement = True
                else:
                    isComplex = True
            else:
                isSimple = True
        elif aObj.getTypes(self.itemStack[::-1]) == []:
            isEmptyElement = True

        return isSimple, isComplex, isSimpleElement, isEmptyElement

    def __addMenuAtLevel__(self):
        stack = []
        stack = self.__getStack__(self.index, stack)
        # Maintaining path stack from the xml root element to till the element
        # on which user has right clicked
        self.itemStack = copy.copy(stack)
        self.path = copy.copy(stack)

        childList = []
        item = self.model.itemFromIndex(self.index)
        # Get current children of the node on which user right clicked from the gui
        for each in range(item.rowCount()):
            childList.append(str(self.index.child(each, 0).data().toString()))
        item = self.model.itemFromIndex(self.index)
        self.itemObj = self.treeObjDict[self.path[-1]]

        (isSimple, isComplex, isSimpleElement, isEmptyElement) = self.__getItemTypes__(self.itemObj)

        if isSimple:
            # Take first element from the indices which are selected in the current row
            element = self.treeView.selectedIndexes()[0]
            item = self.model.itemFromIndex(element)
            self.removeItem = self.mainMenu.addAction("Remove Item")
        elif isComplex:
            self.actionDict = {}
            self.addMenu.setTitle(_translate("MainWindow", "Add", None))
            for each in self.itemObj.getChildren(self.itemStack[::-1], childList):
                # Add current action with the element name
                self.actionDict[each] = self.addMenu.addAction(each)
            # Adding submenu addMenu to the mainMenu
            self.mainMenu.addAction(self.addMenu.menuAction())
            if self.level == 2:
                # Add "save XML as" and "generate Bin File" to the mainMenu at level 2
                self.saveXMLAsAction = self.mainMenu.addAction("save XML as")
                root = self.treeView.model().invisibleRootItem()
                rootConfiguration = root.child(0,0).child(0,0).index().data().toString()
                if rootConfiguration != 'BRCM_Allowed_Configuration':
                    self.generateBinAction = self.mainMenu.addAction("generate Bin File")
            else:
                itemProps = self.itemObj.getNodeProperties(self.itemStack[::-1])
                itemName = str(self.index.data().toString())
                # If the current element on which user right clicked is complex element
                # then provide duplicate option depending on its minOccurs and maxOccurs
                childList = []
                itemCount = 0
                itemParent = self.model.itemFromIndex(self.index.parent())
                for each in range(itemParent.rowCount()):
                    child = str(self.index.parent().child(each, 0).data().toString())
                    if child == itemName:
                        itemCount += 1
                    childList.append(child)

                maxOccurs = 1
                if 'maxOccurs' in itemProps.keys():
                    if itemProps['maxOccurs'] != 'unbounded':
                        maxOccurs = int(itemProps['maxOccurs'])
                        if itemCount < maxOccurs:
                            self.duplicate = self.mainMenu.addAction("duplicate node")
                    else:
                        self.duplicate = self.mainMenu.addAction("duplicate node")
            self.expandBranch = self.mainMenu.addAction("Expand Branch")
            self.collapseBranch = self.mainMenu.addAction("Collapse Branch")
            self.removeItem = self.mainMenu.addAction("Remove Branch")
        elif isSimpleElement:
            element = self.treeView.selectedIndexes()[0]
            item = self.model.itemFromIndex(element)
            self.removeItem = self.mainMenu.addAction("Remove Item")
        else: # EmptyElement
            self.removeItem = self.mainMenu.addAction("Remove Item")

    def __changeFileName__(self, aWidget, aFileName):
        labelIndex = aWidget.label.index()
        if labelIndex.data().toString() != '':
            labelItem = self.model.itemFromIndex(labelIndex)
            labelItem.setText('')
        labelWidget = self.treeView.indexWidget(labelIndex)
        labelWidget.setText(aFileName)
        aWidget.item.setData(aFileName)
        self.__setForegroundColor__(aWidget.level, aWidget.item)

    def __setConsoleMsg__(self, aConsoleMsg):
        currentText = self.textEdit.toPlainText()
        if currentText != '':
            newText = currentText + '\n' + aConsoleMsg
        else:
            newText = aConsoleMsg
        self.textEdit.setText(newText)
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())

    def browseFile(self):
        # Get the widget which is invoking this method
        widget = self.app.sender()
        filePath = str(QtGui.QFileDialog.getOpenFileName(None, "Select File"))
        if filePath != '':
            parentPath = os.path.normpath((os.path.join(os.getcwd(),'../../../../')))
            parentPath = parentPath.replace('\\', '/')
            filePath = filePath.replace(parentPath, '')
            # Update file path in the treeview
            self.__changeFileName__(widget, filePath[1:])

    def __addBrowseForPath__(self, aTagItem, aDataItem, aLevel, aDefaultData, aItemType=None):
        labelWidget = QtGui.QLabel()
        labelWidget.setFixedHeight(self.widgetHeight)
        if aDefaultData is not None:
            aDataItem.setText('')
            labelWidget.setText(aDefaultData)
        if aItemType is not None:
            if aItemType.index() is not None:
                typeItem = self.model.itemFromIndex(aItemType.index())
                typeItem.setText('')
        self.treeView.setIndexWidget(aDataItem.index(), labelWidget)
        browseItem = self.__getQStandardItem__()
        itemParent = self.model.itemFromIndex(aTagItem.index().parent())
        itemParent.setChild(aTagItem.index().row(), 3, browseItem)
        browseWidget = QtGui.QPushButton("Browse")
        browseWidget.setFixedHeight(self.widgetHeight)
        browseWidget.label = aDataItem
        browseWidget.item = aTagItem
        browseWidget.level = aLevel
        self.treeView.setIndexWidget(browseItem.index(), browseWidget)
        browseWidget.clicked.connect(self.browseFile)

    def __addRowWidget__(self, aNewItem, aItemType, aItemData, aSelection=None):
        if aSelection is not None:
            self.itemStack.insert(0, aSelection)

        obj = self.treeObjDict[self.itemStack[-1]]
        # Get the type of element on which user right clicked
        (isSimple, isComplex, isSimpleElement, isEmptyElement) = self.__getItemTypes__(obj)
        itemTypes = obj.getTypes(self.itemStack[::-1])
        itemTypeProps = obj.getTypeProperties(self.itemStack[::-1])
        itemNodeProps = obj.getNodeProperties(self.itemStack[::-1])

        newItemName = str(aNewItem.index().data().toString())
        doc = ''
        itemNodeProps['type'] = itemTypes
        doc += self.__getHTMLCode__(obj, itemNodeProps, newItemName, False, False)
        if isSimple:
            if len(itemTypes) == 1: # simple type without union
                itemTypeProps['type'] = itemTypes[0]
                doc += self.__getHTMLCode__(obj, itemTypeProps[itemTypes[0]], newItemName, False, True)
                aNewItem.setToolTip(doc)
                labelWidget = QtGui.QLabel()
                labelWidget.setText(itemTypes[0])
                labelWidget.setFixedHeight(self.widgetHeight)
                self.treeView.setIndexWidget(aItemType.index(), labelWidget)
                if 'fixed' in itemNodeProps.keys():
                    aItemData.setText(itemNodeProps['fixed'])
                    aNewItem.setData(itemNodeProps['fixed'])
                else:
                    defaultData = None
                    if 'default' in itemNodeProps.keys():
                        defaultData = itemNodeProps['default']
                    if 'enumeration' in itemTypeProps[itemTypes[0]].keys():
                        enumList = itemTypeProps[itemTypes[0]]['enumeration']
                        self.__addComboData__(aNewItem, aItemData,'enumeration', self.level, enumList, defaultData)
                    else:
                        root = self.treeView.model().invisibleRootItem()
                        rootConfiguration = root.child(0,0).child(0,0).index().data().toString()
                        if rootConfiguration == 'BRCM_Allowed_Configuration':
                            self.__addBrowseForPath__(aNewItem, aItemData, self.level, None)
                        else:
                            self.__addTextData__(aNewItem, aItemData, self.level, defaultData)
                    self.__setForegroundColor__(self.level, aNewItem)
            else: # simple type with union
                for each in itemTypeProps.keys():
                    itemTypeProps[each]['type'] = each
                    doc += self.__getHTMLCode__(obj, itemTypeProps[each], newItemName, False, True)
                aNewItem.setToolTip(doc)
                comboWidget = QtGui.QComboBox()
                comboWidget.setFixedHeight(self.widgetHeight)
                for each in itemTypeProps.keys():
                    comboWidget.addItem(each)
                self.treeView.setIndexWidget(aItemType.index(), comboWidget)
                for each in itemTypeProps.keys():
                    if each == comboWidget.currentText():
                        props = itemTypeProps[each]
                        if 'enumeration' in props.keys():
                            enumList = props['enumeration']
                            self.__addComboData__(aNewItem, aItemData,'enumeration', self.level, enumList)
                        else:
                            self.__addTextData__(aNewItem, aItemData, self.level)
                        self.__setForegroundColor__(self.level, aNewItem)
                comboWidget.itemProps = itemTypeProps
                comboWidget.item = aNewItem
                comboWidget.itemData = aItemData
                comboWidget.level = 0
                comboWidget.currentIndexChanged.connect(self.changeWidget)
        elif isComplex:
            doc += self.__getHTMLCode__(obj, itemTypeProps[itemTypes[0]], newItemName, False, True)
            aNewItem.setToolTip(doc)
            labelWidget = QtGui.QLabel()
            labelWidget.setText(itemTypes[0])
            labelWidget.setFixedHeight(self.widgetHeight)
            self.treeView.setIndexWidget(aItemType.index(), labelWidget)
        elif isSimpleElement:
            doc += self.__getHTMLCode__(obj, itemTypeProps[itemTypes[0]], newItemName, False, True)
            aNewItem.setToolTip(doc)
            itemType = itemTypes[0]
            labelWidget = QtGui.QLabel()
            labelWidget.setText(itemType)
            labelWidget.setFixedHeight(self.widgetHeight)
            self.treeView.setIndexWidget(aItemType.index(), labelWidget)
            if 'fixed' in itemNodeProps.keys():
                aItemData.setText(itemNodeProps['fixed'])
                aNewItem.setData(itemNodeProps['fixed'])
            else:
                defaultData = None
                if 'default' in itemNodeProps.keys():
                    defaultData = itemNodeProps['default']
                if itemType == 'boolean':
                    self.__addComboData__(aNewItem, aItemData, 'boolean', self.level, None, defaultData)
                else:
                    self.__addTextData__(aNewItem, aItemData, self.level, defaultData)
                self.__setForegroundColor__(self.level, aNewItem)
        else: # Empty element
            aNewItem.setToolTip(doc)

    def openMenu(self, aPosition):
        # Get the model index of the item at the viewport coordinates point
        # and get the index of that item
        posIndex = self.treeView.indexAt(aPosition)
        # Check if that index is valid then show the menu
        if (posIndex.isValid()):
            # Get the indexes of the currently selected element row
            indexes = self.treeView.selectedIndexes()
            if len(indexes) > 0:
                self.level = 0
                index = indexes[0]
                # Get the level of element from the top level in treeview
                while index.parent().isValid():
                    index = index.parent()
                    self.level += 1

                self.removeItem = None
                self.expandBranch = None
                self.collapseBranch = None
                self.createNewXML = None
                self.saveXMLAsAction = None
                self.generateBinAction = None
                self.openXMLAction = None
                self.duplicate = None
                self.action = None
                self.mainMenu = QtGui.QMenu()
                self.addMenu = QtGui.QMenu()
                self.index = self.treeView.selectedIndexes()[0]

                for i in range(0, 10):
                    # Provide different context menu at each level of treeview
                    if self.level == i:
                        if i == 0:
                            self.expandBranch = self.mainMenu.addAction("Expand Branch")
                            self.collapseBranch = self.mainMenu.addAction("Collapse Branch")
                        elif i == 1:
                            self.expandBranch = self.mainMenu.addAction("Expand Branch")
                            self.collapseBranch = self.mainMenu.addAction("Collapse Branch")
                            self.createNewXML = self.mainMenu.addAction("Create New XML")
                            self.openXMLAction = self.mainMenu.addAction("open XML File")
                        else:
                            self.__addMenuAtLevel__()
                        break

                self.action = self.mainMenu.exec_(self.treeView.viewport().mapToGlobal(aPosition))

                if self.action == None:
                    # No action is choosen from the context menu
                    pass
                elif self.action == self.expandBranch:
                    self.expandOrCollapse(self.index, aEn = 1)
                    self.expandBranch = None
                elif self.action == self.collapseBranch:
                    self.expandOrCollapse(self.index, aEn = 0)
                    self.collapseBranch = None
                elif self.action == self.removeItem:
                    self.removeElement()
                    self.removeItem = None
                elif self.action == self.openXMLAction:
                    self.openXMLFile()
                    self.openXMLAction = None
                elif self.action == self.createNewXML:
                    self.createXML()
                    self.createNewXML = None
                elif self.action == self.saveXMLAsAction:
                    self.saveXMLAs()
                    self.saveXMLAsAction = None
                elif self.action == self.generateBinAction:
                    self.generateBinFile()
                    self.generateBinAction = None
                elif self.action == self.duplicate:
                    self.duplicateNode(self.model.itemFromIndex(self.index))
                    self.duplicate = None
                else:
                    # self.action will be an element which is provided in the context menu
                    selection = None
                    for each in self.actionDict.keys():
                        if self.actionDict[each] == self.action:
                            selection = each
                            item = self.model.itemFromIndex(self.index)
                            obj = self.treeObjDict[self.path[-1]]
                            childList = []
                            for each in range(item.rowCount()):
                                childList.append(str(self.index.child(each, 0).data().toString()))

                            # Get the position of the element from schemaParser where it
                            # should placed under the selected element
                            position = obj.getPosition(self.path[::-1], childList, selection)

                            if position is not None:
                                item = self.model.itemFromIndex(self.index)
                                newItem = self.__getQStandardItem__(selection)
                                itemType = self.__getQStandardItem__()
                                itemData = self.__getQStandardItem__()
                                self.model.insertRow(position, self.index)
                                item.setChild(position, 0, newItem)
                                item.setChild(position, 1, itemType)
                                item.setChild(position, 2, itemData)
                                self.__setForegroundColor__(self.level, newItem)
                                self.__addRowWidget__(newItem, itemType, itemData, selection)
                                self.treeView.setExpanded(self.index, 1)
        self.treeView.clearSelection()

    def __addConfigurations__(self, aParent, aChild):
        rootItem = self.__getQStandardItem__(aChild)
        self.rootItemIndex = rootItem.index()
        # Add root element as the "root" for treeview
        aParent.appendRow(rootItem)
        # Add all the configurations which are provided in board configuration xml file
        for each in self.treeObjDict.keys():
            newItem = self.__getQStandardItem__(each[:-3] + 'Configuration')
            rootItem.appendRow(newItem)

    def openNewWindow(self):
        process = subprocess.Popen(shlex.split('python cfg_gui.py'))

    def saveAll(self):
        if self.saveIndices != []:
            saveAllMsg = "Do you really want to save all the xml files?"
            reply = QtGui.QMessageBox.question(None, 'Message',
                         saveAllMsg, QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)
            if reply == QtGui.QMessageBox.Yes:
                for each in self.saveIndices:
                    self.__writeXML__(each)
                self.__setConsoleMsg__('All XML files saved successfully')

    def generateAll(self):
        if self.saveIndices != []:
            generateAllMsg = "Do you really want to generate all bin files?"
            reply = QtGui.QMessageBox.question(None, 'Message',
                         generateAllMsg, QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)
            if reply == QtGui.QMessageBox.Yes:
                self.saveAll()
                for each in self.saveIndices:
                    self.__generateAllFiles__(each)

    def close(self):
        sys.exit(1)

    def __addHeaderLabels__(self):
        self.model = QtGui.QStandardItemModel()
        # set model for the treeview
        self.treeView.setModel(self.model)
        self.model.setHorizontalHeaderLabels(['Element Name', 'Element Type', 'Element Data', 'Save'])
        elementNameWidth = (self.mainWindowWidth*35)/100
        elementTypeWidth = (self.mainWindowWidth*20)/100
        elementDataWidth = (self.mainWindowWidth*35)/100
        self.treeView.setColumnWidth(0, elementNameWidth)
        self.treeView.setColumnWidth(1, elementTypeWidth)
        self.treeView.setColumnWidth(2, elementDataWidth)
        self.__addConfigurations__(self.model, 'root')

    def __addBoardConfig__(self):
        temp = tempfile.NamedTemporaryFile('w', delete=False)
        temp.write(CFG_BRCMAllowedConfigSchema)
        temp.close()
        try:
            treeObj = CFG_SchemaParser(temp.name)
        except Exception as error:
            consoleMsg = 'Error in boardCfg schema: ' + str(error)
            self.__setConsoleMsg__(consoleMsg)
        else:
            global CFG_tempFileList
            # Add temp file to CFG_tempFileList to remove these files when
            # closing the gui window
            CFG_tempFileList.append(temp.name)
            treeObj.schemaFile = temp.name
            treeObj.script = None
            rootElmtName = treeObj.getChildren([], [])[0]
            self.treeObjDict[rootElmtName] = treeObj
            self.xsdDataTypes = treeObj.xsdDataTypes()
            self.xsdRestrictionConstraints = treeObj.xsdRestrictionConstraints()
            self.__addHeaderLabels__()

    def __changeConfig__(self):
        try:
            # If the model has children then remove the children elements
            if self.model.hasChildren():
                self.treeView.model().removeRow(0, self.rootItemIndex)
                self.treeObjDict = {}
                self.saveIndices = []
        except:
            pass
        # Get the handle of the widget which invoked this method
        widget = self.app.sender()

        if widget.currentText() == 'Create Board Config':
            self.__addBoardConfig__()
        else:
            boardCfg = []
            filePath = self.xmlFileDict[str(widget.currentText())]
            tree = ET.parse(filePath)
            root = tree.getroot()
            for child in root:
                component = []
                parentPath = os.path.normpath((os.path.join(os.getcwd(),'../../../../')))
                parentPath = ('/').join(parentPath.split('\\'))
                schemaScriptPath = parentPath + '/' + child[0].text
                pythonScriptPath = parentPath + '/' + child[1].text
                component.append(schemaScriptPath)
                component.append(pythonScriptPath)
                boardCfg.append(component)
            for each in boardCfg:
                try:
                    treeObj = CFG_SchemaParser(each[0])
                except Exception as error:
                    consoleMsg = 'Error in File: '+ each[0] + '\n' + str(error)
                    self.__setConsoleMsg__(consoleMsg)
                else:
                    treeObj.schemaFile = each[0]
                    treeObj.script = each[1]
                    rootElmtName = treeObj.getChildren([], [])[0]
                    self.treeObjDict[rootElmtName] = treeObj
                    self.xsdDataTypes = treeObj.xsdDataTypes()
                    self.xsdRestrictionConstraints = treeObj.xsdRestrictionConstraints()
            self.__addHeaderLabels__()

    def showCopyRight(self):
        reply = QtGui.QMessageBox.information(None, 'Message',
                'Copyright '+ u'\N{COPYRIGHT SIGN}'+ ' 2019 Broadcom Limited.\n Version 0.1(Initial version)')

    def chooseChip(self):
        allItems = [str(self.chipSelection.itemText(i)) for i in range(self.chipSelection.count())]
        if self.chipSelection.currentText() == 'Choose Chip':
            # We dont need to do any action if the action is choose chip
            # and set the current index of combo box to previous text
            # Block the signals to make sure that setCurrentIndex does not invoke
            # chooseChip method
            self.chipSelection.blockSignals(True)
            self.chipSelection.setCurrentIndex(allItems.index(self.prevText))
            self.chipSelection.blockSignals(False)
        else:
            if self.isDefaultSet is True:
                self.__changeConfig__()
            else:
                rootConfiguration = None
                try:
                    root = self.treeView.model().invisibleRootItem()
                    rootConfiguration = root.child(0,0).child(0,0).index().data().toString()
                except:
                    pass
                if rootConfiguration is None: # if there is no root configuration we don't need to show popup
                    self.__changeConfig__()
                else:
                    popUpMsg = ("Change in Configuration will replace the present configuration with new configuration." + "\n"
                                        " Are you sure you still want to continue?")
                    reply = QtGui.QMessageBox.warning(None, 'Message',
                                 popUpMsg, QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)
                    if reply == QtGui.QMessageBox.Yes:
                        self.__changeConfig__()
                    else:
                        self.chipSelection.blockSignals(True)
                        self.chipSelection.setCurrentIndex(allItems.index(self.prevText))
                        self.chipSelection.blockSignals(False)

    def __previoustext__(self):
        self.prevText = self.chipSelection.currentText()

    def __retranslateUi__(self, aMainWindow):
        aMainWindow.setWindowTitle(_translate("aMainWindow", "Broadcom Automotive Software Configurator", None))
        self.label.setText(_translate("aMainWindow", "Console", None))
        self.menuFile.setTitle(_translate("aMainWindow", "File", None))
        self.menuHelp.setTitle(_translate("aMainWindow", "Help", None))
        self.actionNew.setText(_translate("aMainWindow", "New", None))
        self.actionSaveAll.setText(_translate("aMainWindow", "Save All", None))
        self.actionGenerateAll.setText(_translate("aMainWindow", "Generate All", None))
        self.actionAboutGUI.setText(_translate("aMainWindow", "About GUI", None))

    def setupUi(self, aMainWindow):
        aMainWindow.setObjectName(_fromUtf8("aMainWindow"))
        self.widgetHeight = 27
        self.mainWindowWidth = 1280
        mainWindowHeight = 720
        aMainWindow.setFixedSize(self.mainWindowWidth, mainWindowHeight)

        self.centralwidget = QtGui.QWidget(aMainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.treeView = QtGui.QTreeView(self.centralwidget)

        leftAlignmentSpace = (self.mainWindowWidth*0.7)/100
        topAlignmentSpace = (mainWindowHeight*0.7)/100
        treeViewWidth = self.mainWindowWidth - (leftAlignmentSpace*2)
        treeViewHeight = (mainWindowHeight*70)/100

        self.treeView.setGeometry(QtCore.QRect(leftAlignmentSpace, topAlignmentSpace, treeViewWidth, treeViewHeight))
        self.treeView.setObjectName(_fromUtf8("treeView"))
        self.treeView.setAlternatingRowColors(True)
        self.textEdit = QtGui.QTextEdit(self.centralwidget)
        self.treeView.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.treeView.customContextMenuRequested.connect(self.openMenu)

        topAlignmentSpace = treeViewHeight
        labelHeight = (mainWindowHeight*10)/100
        labelWidth = (self.mainWindowWidth*5)/100

        self.label = QtGui.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(leftAlignmentSpace, topAlignmentSpace, labelHeight, labelWidth))
        self.label.setObjectName(_fromUtf8("label"))

        textEditWidth = treeViewWidth
        textEditHeight = (mainWindowHeight*18)/100
        topAlignmentSpace = topAlignmentSpace + (labelHeight)*60/100

        self.textEdit.setGeometry(QtCore.QRect(leftAlignmentSpace, topAlignmentSpace, textEditWidth, textEditHeight))
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.textEdit.setReadOnly(True)

        aMainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(aMainWindow)

        menubarWidth = (self.mainWindowWidth*10)/100
        menubarHeight = (mainWindowHeight*5)/100
        self.menubar.setGeometry(QtCore.QRect(0, 0, self.mainWindowWidth, mainWindowHeight))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        self.menuFile = QtGui.QMenu(self.menubar)
        self.menuFile.setObjectName(_fromUtf8("menuFile"))
        self.menuHelp = QtGui.QMenu(self.menubar)
        self.menuHelp.setObjectName(_fromUtf8("menuHelp"))

        self.chipSelection = QtGui.QComboBox(self.menubar)
        self.chipSelection.setInsertPolicy(QtGui.QComboBox.InsertAtTop)
        self.chipSelection.addItem('Choose Chip')
        self.chipSelection.addItem('Create Board Config')
        self.chipSelection.currentIndexChanged.connect(self.chooseChip)
        self.chipSelection.highlighted.connect(self.__previoustext__)
        self.__validateAndAddConfig__()

        self.chipSelection.setObjectName(_fromUtf8("chipSelection"))
        leftAlignmentSpace = (self.mainWindowWidth*12)/100
        chipSelectionWidth = (self.mainWindowWidth*20)/100
        chipSelectionHeight = (mainWindowHeight*4)/100
        self.chipSelection.setGeometry(QtCore.QRect(leftAlignmentSpace, 0, chipSelectionWidth, chipSelectionHeight))

        aMainWindow.setMenuBar(self.menubar)
        self.actionNew = QtGui.QAction(aMainWindow)
        self.actionNew.setObjectName(_fromUtf8("actionNew"))
        self.actionSaveAll = QtGui.QAction(aMainWindow)
        self.actionSaveAll.setObjectName(_fromUtf8("actionSaveAll"))
        self.actionGenerateAll = QtGui.QAction(aMainWindow)
        self.actionGenerateAll.setObjectName(_fromUtf8("actionGenerateAll"))
        self.actionAboutGUI = QtGui.QAction(aMainWindow)
        self.actionAboutGUI.setObjectName(_fromUtf8("actionAboutGUI"))
        self.menuFile.addAction(self.actionNew)
        self.actionNew.triggered.connect(self.openNewWindow)
        self.menuFile.addAction(self.actionSaveAll)
        self.actionSaveAll.triggered.connect(self.saveAll)
        self.menuFile.addAction(self.actionGenerateAll)
        self.actionGenerateAll.triggered.connect(self.generateAll)
        self.menuHelp.addAction(self.actionAboutGUI)
        self.actionAboutGUI.triggered.connect(self.showCopyRight)
        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuHelp.menuAction())
        font = QtGui.QFont()
        font.setPointSize(10)
        font.setBold(False)
        font.setWeight(50)
        self.label.setFont(font)
        self.__retranslateUi__(aMainWindow)
        QtCore.QMetaObject.connectSlotsByName(aMainWindow)
        aMainWindow.setAttribute(QtCore.Qt.WA_DeleteOnClose)

## @brief Class Configurator Mainwindow
#
# This class is to override close event of GUI main window.
#
# @trace #BRCM_SWREQ_CFG_GUI
class CFG_Mainwindow(QtGui.QMainWindow):
    def __init__(self):
        super(CFG_Mainwindow, self).__init__()

    def closeEvent(self, aEvent):
        # Subclassing the QtGui.QMainWindow and then overriding the closeEvent of
        # mainWindow to show the popup when closing the window
        quitMsg = "Are you sure you want to quit?"
        reply = QtGui.QMessageBox.question(self, 'Message',
                         quitMsg, QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)

        if reply == QtGui.QMessageBox.Yes:
            global CFG_tempFileList
            for each in CFG_tempFileList:
                os.remove(each)
            aEvent.accept()
        else:
            aEvent.ignore()

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    mainWindow = CFG_Mainwindow()
    ui = CFG_GUI(app)
    ui.setupUi(mainWindow)
    mainWindow.show()
    sys.exit(app.exec_())
## @}
