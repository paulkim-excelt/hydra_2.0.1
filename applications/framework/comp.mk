#
# Copyright 2018-2019 Broadcom Limited.  All rights reserved.
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



##  @defchapter grp_media_system Media System
#
#   @addtogroup grp_media_system
#   @{
#   @image html framework_overview.jpg "Media System Overview"
#   Architecture defines a Media system consisting of Media Framework and Media Components.
#   Media Framework layer consists of 
#       -# Media System interface for application to interact with media system
#       -# Media Ports for media components to interact with each other
#       -# Media Component interface to which media component implementations to be compliant to
#
#   Configuration of each component and the connection between components is established by
#   the application. Media Framework provides communication means for modules and application.
#
#   @}
#
#   @defgroup grp_framework Media Framework
#   @ingroup grp_media_system
#
#   @addtogroup grp_framework
#   @{
#   Media Framework mandates all media components to provide a unified interface. This helps to
#   reduce bulkiness of framework itself. Media Framework expects each component/module to
#   implement a command queue for processing the requests asynchronously.
#
#   Framework expects an Output Data Queue to be implemented by all components which expect
#   output to be fed to other components in the middleware.
#
#   Media Framework (and media system) works in data pull mode. For example, consider data
#   flows from Component1 to Component2, and Component2 to Component3.
#   In this case, Component3 provides buffers to Component2 to fill. And, Component2 provides
#   buffers to Component1 to fill.
#   These are components which implement part of the multimedia pipeline. The media data along
#   with some metadata is encapsulated in a media buffer. Exchange of media buffers between
#   components is done via a logical entity known as a ‘port’. An input port is a consumer
#   of ‘filled’ buffers while an output port is responsible for ‘filling’ the media buffers.
#   A sink component has one or more input ports while a source component has one or more
#   output ports. There are also components which implement both sink and source functionality
#   and hence have both kinds of ports.
#   To keep the component design simple, the ports are unaware of the port/component at the
#   other end. The framework acts as the glue between them. An input port will request the
#   framework for filling up a buffer which relays this request to the output port.
#   When the output port has processed this request, the input port can invoke the framework
#   to receive the buffers back.
#
#   For example, consider data flows from Component1 to Component 2, and Component 2 to Component 3.
#   Component 1 is a source and Component 3 is a sink. Component 2 is a source for Component 3 and
#   a sink for Component 1. In this case, Component3 provides buffers to Component2 to fill.
#   And, Component2 provides buffers to Component1 to fill.
#   @image html framework_connection.jpg "Media Pipe"
#
#   @file applications/framework/comp.mk
#   @brief Makefile for Media Framework
#   @version 0.1 Initial version
#   @}

BRCM_CONFIGURATION_ITEM := comp.mk
BRCM_CONFIGURATION_ITEM += doc/media_system_req.c
BRCM_CONFIGURATION_ITEM += inc/media_system.h
BRCM_CONFIGURATION_ITEM += inc/media_port.h
BRCM_CONFIGURATION_ITEM += inc/component.h
BRCM_CONFIGURATION_ITEM += os/common/media_port.c
#BRCM_CONFIGURATION_ITEM += media_system.c

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

BRCM_COMP_NAME := msys
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := doc

