#
# Copyright: Copyright 2019 Broadcom Limited.
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

import io
import os
import sys
import copy
import ctypes
import itertools
import functools
import traceback
import collections

from os.path import exists, join, split

from lxml import etree
from lxml import objectify

################################################################################
# Utils
################################################################################
#
# The definitions of the below must match the corresponding constructs
# defined in the C header file.
#
BLOCK_ID_SHIFT  = 3
STRUCT_ID_SHIFT = 0

BLOCK_ID_EXTRACT_MASK  = 0x1F
STRUCT_ID_EXTRACT_MASK = 0x07

def MakeSectionId(block_id, struct_id):
    return ((block_id  & BLOCK_ID_EXTRACT_MASK)  << BLOCK_ID_SHIFT)  | \
           ((struct_id & STRUCT_ID_EXTRACT_MASK) << STRUCT_ID_SHIFT)

#
# Helpers to parse PWL's
#
def TryParseInt(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

def ExtractIntegerListFromXML(pwl_obj):
    return [int(a) for a in str(pwl_obj).split(' ') if TryParseInt(a)]

def ParseAbscissaeAndOrdinates(xml_pwl_abscissae_obj, xml_pwl_ordinates_obj):
    abscissae = ExtractIntegerListFromXML(xml_pwl_abscissae_obj)
    ordinates = ExtractIntegerListFromXML(xml_pwl_ordinates_obj)

    if len(abscissae) != len(ordinates):
        raise Exception("Abscissae and Ordinates list lengths mismatch!")

    return [(abss, ordi) for abss, ordi in zip(abscissae, ordinates)]

def GetDuplicates(get_key, iterable):
    return [i for i, rep in collections.Counter(map(get_key, iterable)).iteritems() if rep > 1]

#
# Endian correct c_types
#
TARGET_ENDIANNESS = "le"

int8   = ctypes.c_int8.__ctype_le__
int16  = ctypes.c_int16.__ctype_le__
int32  = ctypes.c_int32.__ctype_le__
int64  = ctypes.c_int64.__ctype_le__
uint8  = ctypes.c_uint8.__ctype_le__
uint16 = ctypes.c_uint16.__ctype_le__
uint32 = ctypes.c_uint32.__ctype_le__
uint64 = ctypes.c_uint64.__ctype_le__

if TARGET_ENDIANNESS == "le":
    # Do nothing
    pass
elif TARGET_ENDIANNESS == "be":
    int8   = ctypes.c_int8.__ctype_be__
    int16  = ctypes.c_int16.__ctype_be__
    int32  = ctypes.c_int32.__ctype_be__
    int64  = ctypes.c_int64.__ctype_be__
    uint8  = ctypes.c_uint8.__ctype_be__
    uint16 = ctypes.c_uint16.__ctype_be__
    uint32 = ctypes.c_uint32.__ctype_be__
    uint64 = ctypes.c_uint64.__ctype_be__
else:
    raise Exception("Invalid endianness set")

###############################################################################
# Class to represent the block configs
###############################################################################
class BlockConfig():

    def __init__(self, block_name, block_id, bayer_en_mask, ycbcr_en_mask, serialized_byte_array):
        self.block_name = block_name
        self.id = block_id
        self.bayer_en_mask = bayer_en_mask
        self.ycbcr_en_mask = ycbcr_en_mask
        self.serialized_byte_array = serialized_byte_array

    def get_serialized_size(self):
        return len(self.serialized_byte_array)

    def serialize(self, fhdl):
        if len(self.serialized_byte_array) > 0:
            fhdl.write(self.serialized_byte_array)
        return

###############################################################################
# Top level config Classes
###############################################################################
class ISPConfig():

    @staticmethod
    def parse(block_name, obj):

        isp_block = None

        # Select the appropriate ISPBlock class that can
        # handle the incoming object.
        if block_name == "image_input":
            isp_block = ISPBlock_ImageInput
        elif block_name == "black_level":
            isp_block = ISPBlock_BlackLevel
        elif block_name == "lens_shading":
            isp_block = ISPBlock_LensShading
        elif block_name == "hdr_fusion":
            isp_block = ISPBlock_HDRFusion
        elif block_name == "bayer_denoise":
            isp_block = ISPBlock_BayerDenoise
        elif block_name == "statistics":
            isp_block = ISPBlock_Statistics
        elif block_name == "white_balance_and_gain":
            isp_block = ISPBlock_WBAndGain
        elif block_name == "defective_pixel_correction":
            isp_block = ISPBlock_DefectivePixel
        elif block_name == "fringing":
            isp_block = ISPBlock_Fringing
        elif block_name == "crosstalk":
            isp_block = ISPBlock_Crosstalk
        elif block_name == "color_denoise":
            isp_block = ISPBlock_ColorDenoise
        elif block_name == "demosaic":
            isp_block = ISPBlock_Demosaic
        elif block_name == "sharpen":
            isp_block = ISPBlock_Sharpen
        elif block_name == "color_correction":
            isp_block = ISPBlock_ColorCorrection
        elif block_name == "gamma":
            isp_block = ISPBlock_Gamma
        elif block_name == "ycbcr_conversion":
            isp_block = ISPBlock_YCBCRConversion
        elif block_name == "false_color":
            isp_block = ISPBlock_FalseColor
        elif block_name == "local_tone_mapping":
            isp_block = ISPBlock_LocalToneMapping
        elif block_name == "tone_mapping":
            isp_block = ISPBlock_ToneMapping
        elif block_name == "chrominance_stretch":
            isp_block = ISPBlock_ChrominanceStretch
        elif block_name == "color_conversion":
            isp_block = ISPBlock_ColorConversion
        elif block_name == "lr_resize":
            isp_block = ISPBlock_LRResize
        elif block_name == "lores_output":
            isp_block = ISPBlock_LresOutput
        elif block_name == "hires_output":
            isp_block = ISPBlock_HresOutput
        else:
            # Incoming object does not specify an ISPBlock
            # configutation. Return.
            return None

        # ISPBlock class determined. See if the incoming object
        # can be mapped to one of the multiple C structs supported.
        ret = None
        for block_config_struct in isp_block._block_config_structs_:
            ret = block_config_struct.parse(block_name, obj)
            if ret != None:
                break

        if ret == None:
            raise Exception("TODO: Add description")

        return ret

    @staticmethod
    def validate_isp_one_pass(pass_name, isp_pass_cfg):

        # The pass must contain one image_input block.
        image_input_block = [block for block in isp_pass_cfg if block.block_name == "image_input"]
        if len(image_input_block) == 0:
            raise Exception("The pass {} has no ISP image_input configuration!", pass_name)

        return

    @staticmethod
    def list_patchable_blocks_one_pass(isp_pass_cfg):
        return filter(lambda block: "patch_block" in dir(block), isp_pass_cfg)

    @staticmethod
    def patch_isp_blocks_one_pass(isp_pass_cfg):
        for patchable_block in ISPConfig.list_patchable_blocks_one_pass(isp_pass_cfg):
            patchable_block.patch_block(isp_pass_cfg)
        return

    @staticmethod
    def validate(isp_configs):

        assert 'common_config' in isp_configs

        if len(isp_configs) == 1:
            # All the ISP configuration is contained in the common_config
            # pass.
            ISPConfig.validate_isp_one_pass('common_config', isp_configs['common_config'])
            ISPConfig.patch_isp_blocks_one_pass(isp_configs['common_config'])

            # Sort blocks
            isp_configs['common_config'].sort(key = lambda s: s.id)

        else:
            # We have multiple passes. If there are any patchable blocks in the
            # common config, we want to remove these blocks from the common list
            # and replicate them on the other pass lists.
            patchable_blocks = ISPConfig.list_patchable_blocks_one_pass(isp_configs['common_config'])

            for (pass_name, pass_isp_cfg) in isp_configs.items():
                if pass_name != 'common_config':
                    for block in patchable_blocks:
                        pass_isp_cfg.append(copy.deepcopy(block))

            for block in patchable_blocks:
                isp_configs['common_config'].remove(block)

            # Next, construct a full pass description for each of the passes,
            # and validate and patch them individually
            for (pass_name, pass_isp_cfg) in isp_configs.items():
                if pass_name != 'common_config':
                    pass_config = isp_configs['common_config'] + pass_isp_cfg

                    # Make sure that no blocks are duplicated between the common pass and
                    # the other passes.
                    duplicate_blocks = GetDuplicates(lambda s: s.block_name, pass_config)
                    if len(duplicate_blocks) > 0:
                        raise Exception("The ISP config blocks {} are repeated across common_config and {}" \
                        .format(duplicate_blocks, pass_name))

                    ISPConfig.validate_isp_one_pass(pass_name, pass_config)
                    ISPConfig.patch_isp_blocks_one_pass(pass_config)

            # TODO: There is an optimization opportunity here. Some of the blocks
            # that we've duplicated can be moved back to the common config at this
            # point, saving space.

            # Sort blocks
            for (pass_name, pass_isp_cfg) in isp_configs.items():
                pass_isp_cfg.sort(key = lambda s: s.id)

            return

class TunerConfig():

    @staticmethod
    def parse(block_name, obj):
        pass

    @staticmethod
    def validate(tuner_configs):
        pass

SUPPORTED_CONFIGS = [
    ISPConfig,
    TunerConfig,
]

SUPPORTED_PASS_NAME_TO_ID_MAP = {
    'common_config':3,
    'hv_config':1,
    'cv_config':2
}

###############################################################################
# Global validation
###############################################################################
def validate_parsed_config(parsed_configs):

    assert len(parsed_configs) > 0

    isp_configs = {pass_name:(pass_config[SUPPORTED_CONFIGS.index(ISPConfig)])      \
        for (pass_name, pass_config) in parsed_configs.items()}

    tuner_configs = {pass_name:(pass_config[SUPPORTED_CONFIGS.index(TunerConfig)])  \
        for (pass_name, pass_config) in parsed_configs.items()}

    ISPConfig.validate(isp_configs)
    TunerConfig.validate(tuner_configs)

    return

###############################################################################
# Image input
###############################################################################
ISPBLOCK_IMAGEINPUT_ID = 0x0
ISPBLOCK_IMAGEINPUT_BAYER_EN_MASK = 0x1
ISPBLOCK_IMAGEINPUT_YCBCR_EN_MASK = 0x0

class ISPBlock_ImageInput_Config0():

    @staticmethod
    def parse(block_name, image_input_xml_config_block):

        # Parse format.
        # The assigned values below are directly picked up from
        # the RDB.
        input_format = 0

        if image_input_xml_config_block.format == "RAW8":
            input_format = 2
        elif image_input_xml_config_block.format == "RAW10":
            input_format = 3
        elif image_input_xml_config_block.format == "RAW12":
            input_format = 4
        elif image_input_xml_config_block.format == "RAW14":
            input_format = 5
        elif image_input_xml_config_block.format == "RAW16":
            input_format = 6
        elif image_input_xml_config_block.format == "RAW10_8":
            input_format = 7
        elif image_input_xml_config_block.format == "RAW12_8":
            input_format = 8
        elif image_input_xml_config_block.format == "RAW14_8":
            input_format = 9
        elif image_input_xml_config_block.format == "RAW10L":
            input_format = 11
        elif image_input_xml_config_block.format == "RAW12L":
            input_format = 12
        elif image_input_xml_config_block.format == "RAW14L":
            input_format = 13
        else:
            raise Exception("Unreachable!")

        # Parse bayer order.
        # The assigned values below are directly picked up from
        # a HW header.
        bayer_order = 0

        if image_input_xml_config_block.bayer_order == "RGGB":
            bayer_order = 0
        elif image_input_xml_config_block.bayer_order == "GBRG":
            bayer_order = 1
        elif image_input_xml_config_block.bayer_order == "BGGR":
            bayer_order = 2
        elif image_input_xml_config_block.bayer_order == "GRBG":
            bayer_order = 3
        else:
            raise Exception("Unreachable!")

        # Check if transpose is set.
        transposed = False

        try:
            transposed = image_input_xml_config_block.transposed
        except AttributeError:
            pass

        # Parse DPCM configuration.
        # The assigned values below are directly picked up from
        # the RDB.
        dpcm_mode = 0
        dpcm_blocklen = 0

        try:
            if image_input_xml_config_block.dpcm.mode == "RAW10_8":
                dpcm_mode = 1
            elif image_input_xml_config_block.dpcm.mode == "RAW12_8":
                dpcm_mode = 4
            elif image_input_xml_config_block.dpcm.mode == "RAW14_8":
                dpcm_mode = 8
            else:
                raise Exception("Unreachable!")

            dpcm_blocklen = image_input_xml_config_block.dpcm.blocklen

        except AttributeError:
            pass

        # Instead of returning an instance of BlockConfig class,
        # we return an instance of ISPBlock_ImageInput_Config0 that
        # implements a super-set of an interface implemented by the
        # BlockConfig class. This is needed because we will need to
        # patch in the 'bayer_en' and 'ycbcr_en' fields after we finish
        # reading the full configuration.
        block_id = MakeSectionId(ISPBLOCK_IMAGEINPUT_ID, 0)

        return ISPBlock_ImageInput_Config0(block_name, block_id,    \
            image_input_xml_config_block.frame_width,               \
            image_input_xml_config_block.frame_height,              \
            image_input_xml_config_block.stride,                    \
            input_format, bayer_order, transposed, dpcm_mode, dpcm_blocklen)

    def __init__(self, block_name, block_id, frame_width, frame_height, frame_stride, \
        input_format, bayer_order, transposed, dpcm_mode, dpcm_blocklen):

        self.bayer_en_mask = ISPBLOCK_IMAGEINPUT_BAYER_EN_MASK
        self.ycbcr_en_mask = ISPBLOCK_IMAGEINPUT_YCBCR_EN_MASK

        self.id = block_id
        self.block_name = block_name
        self.frame_width = frame_width
        self.frame_height = frame_height
        self.frame_stride = frame_stride
        self.input_format = input_format
        self.bayer_order = bayer_order
        self.transposed = transposed
        self.dpcm_mode = dpcm_mode
        self.dpcm_blocklen = dpcm_blocklen
        self.bayer_en = 0
        self.ycbcr_en = 0

        return

    def patch_block(self, pass_config_list):
        for block in pass_config_list:
            self.bayer_en |= block.bayer_en_mask
            self.ycbcr_en |= block.ycbcr_en_mask

        return

    def __construct_serialization_byte_array__(self):
            #
            # Corresponding C struct is defined as -
            #
            #   typedef struct COMP_PACKED _ISPIMGV2_II_BlkCfg0Type {
            #       uint16_t magic;
            #       uint16_t width;
            #       uint16_t height;
            #       uint16_t stride;
            #       uint32_t bayerEn;
            #       uint32_t yCbCrEn;
            #       uint16_t dpcmCfg;
            #       uint8_t formatBayerOrderAndTranspose;
            #   } ISPIMGV2_II_BlkCfg0Type;
            #

            dpcmCfg = (self.dpcm_blocklen & 0x1FF) | ((self.dpcm_mode & 0x7) << 9)

            formatBayerOrderAndTranspose =                                          \
                ((self.transposed & 0x1) << 6) | ((self.bayer_order & 0x3) << 4) |  \
                ((self.input_format & 0xF))

            serialization = bytearray()
            serialization.extend(uint16(self.id))
            serialization.extend(uint16(self.frame_width))
            serialization.extend(uint16(self.frame_height))
            serialization.extend(uint16(self.frame_stride))
            serialization.extend(uint32(self.bayer_en))
            serialization.extend(uint32(self.ycbcr_en))
            serialization.extend(uint16(dpcmCfg))
            serialization.extend(uint8(formatBayerOrderAndTranspose))

            return serialization

    def get_serialized_size(self):
        return len(self.__construct_serialization_byte_array__())

    def serialize(self, fhdl):
        fhdl.write(self.__construct_serialization_byte_array__())
        return

class ISPBlock_ImageInput():
    _block_config_structs_ = [
        ISPBlock_ImageInput_Config0,
    ]

###############################################################################
# Black level
###############################################################################
ISPBLOCK_BLACKLEVEL_ID = 0x1
ISPBLOCK_BLACKLEVEL_BAYER_EN_MASK = (1 << 2)
ISPBLOCK_BLACKLEVEL_YCBCR_EN_MASK = 0x0

class ISPBlock_BlackLevel_Config0():

    @staticmethod
    def parse(block_name, black_level_xml_config_block):
        attr = black_level_xml_config_block.__dict__

        df_border_left =   getattr(black_level_xml_config_block, "df_border_left", 0)
        df_border_right =  getattr(black_level_xml_config_block, "df_border_right", 0)
        df_border_top =    getattr(black_level_xml_config_block, "df_border_top", 0)
        df_border_bottom = getattr(black_level_xml_config_block, "df_border_bottom", 0)
        df_mult =          getattr(black_level_xml_config_block, "df_mult", 0)
        threshold =        getattr(black_level_xml_config_block, "threshold", 0)

        # Long exposure params
        lexp_pwl_r = ParseAbscissaeAndOrdinates(                        \
            black_level_xml_config_block.long_exp.abscissae_r,   \
            black_level_xml_config_block.long_exp.ordinates_r)

        lexp_pwl_b = ParseAbscissaeAndOrdinates(                        \
            black_level_xml_config_block.long_exp.abscissae_b,   \
            black_level_xml_config_block.long_exp.ordinates_b)

        lexp_pwl_gr = ParseAbscissaeAndOrdinates(                       \
            black_level_xml_config_block.long_exp.abscissae_gr,  \
            black_level_xml_config_block.long_exp.ordinates_gr)

        lexp_pwl_gb = ParseAbscissaeAndOrdinates(                       \
            black_level_xml_config_block.long_exp.abscissae_gb,  \
            black_level_xml_config_block.long_exp.ordinates_gb)

        lexp_lvl_r  = getattr(black_level_xml_config_block.long_exp, "lvl_r", 0)
        lexp_lvl_b  = getattr(black_level_xml_config_block.long_exp, "lvl_b", 0)
        lexp_lvl_gr = getattr(black_level_xml_config_block.long_exp, "lvl_gr", 0)
        lexp_lvl_gb = getattr(black_level_xml_config_block.long_exp, "lvl_gb", 0)

        # Short exposure params
        sexp_pwl_r = lexp_pwl_r
        sexp_pwl_b = lexp_pwl_b
        sexp_pwl_gr = lexp_pwl_gr
        sexp_pwl_gb = lexp_pwl_gb

        sexp_lvl_r  = 0
        sexp_lvl_b  = 0
        sexp_lvl_gr = 0
        sexp_lvl_gb = 0

        if "short_exp" in attr:
            sexp_pwl_r = ParseAbscissaeAndOrdinates(                    \
                black_level_xml_config_block.short_exp.abscissae_r,     \
                black_level_xml_config_block.short_exp.ordinates_r)

            sexp_pwl_b = ParseAbscissaeAndOrdinates(                    \
                black_level_xml_config_block.short_exp.abscissae_b,     \
                black_level_xml_config_block.short_exp.ordinates_b)

            sexp_pwl_gr = ParseAbscissaeAndOrdinates(                   \
                black_level_xml_config_block.short_exp.abscissae_gr,    \
                black_level_xml_config_block.short_exp.ordinates_gr)

            sexp_pwl_gb = ParseAbscissaeAndOrdinates(                   \
                black_level_xml_config_block.short_exp.abscissae_gb,    \
                black_level_xml_config_block.short_exp.ordinates_gb)

            sexp_lvl_r  = getattr(black_level_xml_config_block.short_exp, "lvl_r", 0)
            sexp_lvl_b  = getattr(black_level_xml_config_block.short_exp, "lvl_b", 0)
            sexp_lvl_gr = getattr(black_level_xml_config_block.short_exp, "lvl_gr", 0)
            sexp_lvl_gb = getattr(black_level_xml_config_block.short_exp, "lvl_gb", 0)

        block_id = MakeSectionId(ISPBLOCK_BLACKLEVEL_ID, 0)

        #
        # Corresponding C structs is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_BL_PwlElement {
        #     uint16_t abs;
        #     uint16_t ord;
        # } ISPIMGV2_BL_PwlElement;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_BL_BlkCfg0ChannelConfigType {
        #     uint16_t lvlR;
        #     uint16_t lvlB;
        #     uint16_t lvlGR;
        #     uint16_t lvlGB;
        #     uint8_t  pwlLenR;
        #     uint8_t  pwlLenB;
        #     uint8_t  pwlLenGR;
        #     uint8_t  pwlLenGB;
        # } ISPIMGV2_BL_BlkCfg0ChannelConfigType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_BL_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t dfBorderLeft;
        #     uint16_t dfBorderRight;
        #     uint16_t dfBorderTop;
        #     uint16_t dfBorderBottom;
        #     uint16_t df_mult;
        #     uint16_t threshold;
        #     ISPIMGV2_BL_BlkCfg0ChannelConfigType channelConfig[ISP_IN_IDX_MAX];
        #     ISPIMGV2_BL_PwlElementType pwlElements[4U * ISP_IN_IDX_MAX * ISPIMGV2_BL_BLKCFG0_PWL_LEN];
        # } ISPIMGV2_BL_BlkCfg0Type;
        #
        # The IN_IDX is defined as follows -
        #
        #     #define ISP_IN_IDX_LONG_EXP  (0UL)
        #     #define ISP_IN_IDX_SHORT_EXP (1UL)
        #     #define ISP_IN_IDX_MAX       (2UL)
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        df_border_left_offset = len(serialization)
        serialization.extend(uint16(df_border_left))

        df_border_right_offset = len(serialization)
        serialization.extend(uint16(df_border_right))

        df_border_top_offset = len(serialization)
        serialization.extend(uint16(df_border_top))

        df_border_bottom_offset = len(serialization)
        serialization.extend(uint16(df_border_bottom))

        serialization.extend(uint16(df_mult))
        serialization.extend(uint16(threshold))

        serialization.extend(uint16(lexp_lvl_r))
        serialization.extend(uint16(lexp_lvl_b))
        serialization.extend(uint16(lexp_lvl_gr))
        serialization.extend(uint16(lexp_lvl_gb))
        serialization.extend(uint8(len(lexp_pwl_r)))
        serialization.extend(uint8(len(lexp_pwl_b)))
        serialization.extend(uint8(len(lexp_pwl_gr)))
        serialization.extend(uint8(len(lexp_pwl_gb)))

        serialization.extend(uint16(sexp_lvl_r))
        serialization.extend(uint16(sexp_lvl_b))
        serialization.extend(uint16(sexp_lvl_gr))
        serialization.extend(uint16(sexp_lvl_gb))
        serialization.extend(uint8(len(sexp_pwl_r)))
        serialization.extend(uint8(len(sexp_pwl_b)))
        serialization.extend(uint8(len(sexp_pwl_gr)))
        serialization.extend(uint8(len(sexp_pwl_gb)))

        long_exp = (lexp_pwl_r, lexp_pwl_b, lexp_pwl_gr, lexp_pwl_gb)
        short_exp = (sexp_pwl_r, sexp_pwl_b, sexp_pwl_gr, sexp_pwl_gb)

        for pwl in itertools.chain(long_exp, short_exp):
            for absissa, ordinate in pwl:
                serialization.extend(uint16(absissa))
                serialization.extend(uint16(ordinate))

        return ISPBlock_BlackLevel_Config0(block_name, block_id,                    \
            ISPBLOCK_BLACKLEVEL_BAYER_EN_MASK, ISPBLOCK_BLACKLEVEL_YCBCR_EN_MASK,   \
            df_border_left_offset, df_border_right_offset,                          \
            df_border_top_offset, df_border_bottom_offset, serialization)

    def __init__(self, block_name, block_id, bayer_en, ycbcr_en,                    \
        df_border_left_offset, df_border_right_offset,                              \
        df_border_top_offset, df_border_bottom_offset, serialization):

        self.id = block_id
        self.block_name = block_name
        self.bayer_en_mask = bayer_en
        self.ycbcr_en_mask = ycbcr_en
        self.df_border_left_offset = df_border_left_offset
        self.df_border_right_offset = df_border_right_offset
        self.df_border_top_offset = df_border_top_offset
        self.df_border_bottom_offset = df_border_bottom_offset
        self.serialization = serialization

        return

    def patch_block(self, pass_config_list):

        # The right and bottom fields set in the XML are the number of
        # pixels that the HW needs to ignore on the right margin and the
        # bottom margin of the frame respectively. The HW on the other hand
        # expects us to input the last valid row and coloumn numbers in the
        # image. We perform the necessary conversion here.

        # Get the width and height from the image input block
        tmp = filter(lambda block: block.block_name == "image_input", pass_config_list)
        assert len(tmp) == 1
        image_input_block = tmp[0]

        frame_width = image_input_block.frame_width
        frame_height = image_input_block.frame_height

        df_border_left = self.serialization[self.df_border_left_offset]
        df_border_right = self.serialization[self.df_border_right_offset]
        df_border_top = self.serialization[self.df_border_top_offset]
        df_border_bottom = self.serialization[self.df_border_bottom_offset]

        bottom_patched = frame_height - df_border_bottom
        right_patched = frame_width - df_border_left

        assert bottom_patched > df_border_top
        assert right_patched > df_border_left

        self.serialization[self.df_border_right_offset:self.df_border_right_offset+2] = uint16(right_patched)
        self.serialization[self.df_border_bottom_offset:self.df_border_bottom_offset+2] = uint16(bottom_patched)

        return

    def get_serialized_size(self):
        return len(self.serialization)

    def serialize(self, fhdl):
        fhdl.write(self.serialization)
        return


class ISPBlock_BlackLevel():
    _block_config_structs_ = [
        ISPBlock_BlackLevel_Config0,
    ]

###############################################################################
# Lens shading
###############################################################################
ISPBLOCK_LENSSHADING_ID = 0x2
ISPBLOCK_LENSSHADING_BAYER_EN_MASK = (1 << 3)
ISPBLOCK_LENSSHADING_YCBCR_EN_MASK = 0x0

class ISPBlock_LensShading_Config0():

    @staticmethod
    def parse(block_name, lens_shading_xml_config_block):
        attr = lens_shading_xml_config_block.__dict__

        if ("lens_shading_use_default_params" in attr):
            block_id = MakeSectionId(ISPBLOCK_LENSSHADING_ID, 0)

            #
            # Corresponding C struct is defined as -
            #
            #   typedef struct COMP_PACKED _ISPIMGV2_LS_BlkCfg0Type {
            #   } ISPIMGV2_LS_BlkCfg0Type;
            #
            #
            serialization = bytearray()

            return BlockConfig(block_name, block_id,                                    \
                ISPBLOCK_LENSSHADING_BAYER_EN_MASK, ISPBLOCK_LENSSHADING_YCBCR_EN_MASK, \
                serialization)
        else:
            return None

class ISPBlock_LensShading_Config1():

    @staticmethod
    def ParseLensShadingTableFromXML(xml_cv_r, xml_cv_b, xml_cv_gr, xml_cv_gb):
        cv_r  = ExtractIntegerListFromXML(xml_cv_r)
        cv_b  = ExtractIntegerListFromXML(xml_cv_b)
        cv_gr = ExtractIntegerListFromXML(xml_cv_gr)
        cv_gb = ExtractIntegerListFromXML(xml_cv_gb)

        if (len(cv_r)  != len(cv_b)) or \
           (len(cv_b)  != len(cv_gr)) or \
           (len(cv_gr) != len(cv_gb)):
            raise Exception("Invalid lens shading table specified. \
                Not all components have the same length")

        return [elt for elt in zip(cv_r, cv_b, cv_gr, cv_gb)]

    @staticmethod
    def parse(block_name, lens_shading_xml_config_block):
        attr = lens_shading_xml_config_block.__dict__

        if ("pitch" in attr) and            \
           ("vpitch" in attr) and           \
           ("cell_sz_power_x" in attr) and  \
           ("cell_sz_power_y" in attr) and  \
           ("cv_r" in attr) and             \
           ("cv_b" in attr) and             \
           ("cv_gr" in attr) and            \
           ("cv_gb" in attr):

            lens_shading_table = ISPBlock_LensShading_Config1.ParseLensShadingTableFromXML(  \
                lens_shading_xml_config_block.cv_r,             \
                lens_shading_xml_config_block.cv_b,             \
                lens_shading_xml_config_block.cv_gr,            \
                lens_shading_xml_config_block.cv_gb             \
            )

            block_id = MakeSectionId(ISPBLOCK_LENSSHADING_ID, 1)
            cellSizes = ((lens_shading_xml_config_block.cell_sz_power_x & 0xF)) |           \
                        ((lens_shading_xml_config_block.cell_sz_power_y & 0xF) << 4)

            #
            # Corresponding C struct is defined as -
            #
            #   typedef struct COMP_PACKED _ISPIMGV2_LS_TableElementType {
            #       uint16_t cvR;
            #       uint16_t cvB;
            #       uint16_t cvGR;
            #       uint16_t cvGB;
            #   } ISPIMGV2_LS_TableElementType;
            #
            #   typedef struct COMP_PACKED _ISPIMGV2_LS_BlkCfg1Type {
            #       uint16_t magic;
            #       uint8_t pitch;
            #       uint8_t vpitch;
            #       uint8_t cellSizes;
            #       uint16_t tableEltCount;
            #       ISPIMGV2_LS_TableElementType lensShadingTable[ISPIMGV2_LS_BLKCFG1_MAX_TABLE_ELTS];
            #    } ISPIMGV2_LS_BlkCfg1Type;
            #
            serialization = bytearray()
            serialization.extend(uint16(block_id))
            serialization.extend(uint8(lens_shading_xml_config_block.pitch))
            serialization.extend(uint8(lens_shading_xml_config_block.vpitch))
            serialization.extend(uint8(cellSizes))
            serialization.extend(uint16(len(lens_shading_table)))

            for cv_r, cv_b, cv_gr, cv_gb in lens_shading_table:
                serialization.extend(uint16(cv_r))
                serialization.extend(uint16(cv_b))
                serialization.extend(uint16(cv_gr))
                serialization.extend(uint16(cv_gb))

            return BlockConfig(block_name, block_id,                                    \
                ISPBLOCK_LENSSHADING_BAYER_EN_MASK, ISPBLOCK_LENSSHADING_YCBCR_EN_MASK, \
                serialization)

        else:
            return None

class ISPBlock_LensShading():
    _block_config_structs_ = [
        ISPBlock_LensShading_Config0,
        ISPBlock_LensShading_Config1,
    ]

###############################################################################
# HDR Fusion
###############################################################################
ISPBLOCK_HDRFUSION_ID = 0x4
ISPBLOCK_HDRFUSION_BAYER_EN_MASK = (1 << 16)
ISPBLOCK_HDRFUSION_YCBCR_EN_MASK = 0x0

class ISPBlock_HDRFusion_Config0():

    @staticmethod
    def parse(block_name, hdr_fusion_xml_config_block):

        hdr_pwl = ParseAbscissaeAndOrdinates(           \
            hdr_fusion_xml_config_block.abscissae,      \
            hdr_fusion_xml_config_block.ordinates)

        block_id = MakeSectionId(ISPBLOCK_HDRFUSION_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_HDR_PwlElementType {
        #     uint8_t  absHi;
        #     uint16_t absLo;
        #     uint16_t ord;
        # } ISPIMGV2_HDR_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_HDR_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t gainR;
        #     uint16_t gainB;
        #     uint16_t expRatio;
        #     uint32_t wbThres;
        #     uint16_t expCombLoThres;
        #     uint16_t expCombHiThres;
        #     uint32_t expCombWtPwlLenAndExpSwap;
        #     ISPIMGV2_HDR_PwlElementType pwlElements[ISPIMGV2_HDR_BLKCFG0_PWL_LEN];
        # } ISPIMGV2_HDR_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint16(hdr_fusion_xml_config_block.gain_r))
        serialization.extend(uint16(hdr_fusion_xml_config_block.gain_b))
        serialization.extend(uint16(hdr_fusion_xml_config_block.exp_ratio))
        serialization.extend(uint32(hdr_fusion_xml_config_block.threshold))
        serialization.extend(uint16(hdr_fusion_xml_config_block.thr_lo))
        serialization.extend(uint16(hdr_fusion_xml_config_block.thr_hi))
        serialization.extend(uint32(                \
            (hdr_fusion_xml_config_block.wt_sc)   | \
            (len(hdr_pwl) << 20)                  | \
            (hdr_fusion_xml_config_block.swap_exp << 25)
        ))

        for absissa, ordinate in hdr_pwl:
            serialization.extend(uint8(absissa & 0xFF))
            serialization.extend(uint16(absissa >> 8))
            serialization.extend(uint16(ordinate))

        return BlockConfig(block_name, block_id,                                \
            ISPBLOCK_HDRFUSION_BAYER_EN_MASK, ISPBLOCK_HDRFUSION_YCBCR_EN_MASK, \
            serialization)

class ISPBlock_HDRFusion():
    _block_config_structs_ = [
        ISPBlock_HDRFusion_Config0,
    ]

###############################################################################
# Bayer Denoise
###############################################################################
ISPBLOCK_BAYERDENOISE_ID = 0x5
ISPBLOCK_BAYERDENOISE_BAYER_EN_MASK = (1 << 10)
ISPBLOCK_BAYERDENOISE_YCBCR_EN_MASK = 0x0

class ISPBlock_BayerDenoise_Config0():

    @staticmethod
    def parse(block_name, bdn_xml_config_block):

        bdn_pwl = ParseAbscissaeAndOrdinates(       \
            bdn_xml_config_block.abscissae,         \
            bdn_xml_config_block.ordinates)

        mask = 0
        for pos, bit in enumerate(ExtractIntegerListFromXML(bdn_xml_config_block.mask)):
            mask |= (bit << pos)

        maskAndPwlLen = (len(bdn_pwl) << 56) | mask

        block_id = MakeSectionId(ISPBLOCK_BAYERDENOISE_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_BDN_PwlElementType {
        #     uint16_t abs;
        #     uint16_t ord;
        # } ISPIMGV2_BDN_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_BDN_BlkCfg0Type {
        #     uint16_t magic;
        #     uint8_t gainR;
        #     uint8_t gainB;
        #     uint8_t gainGR;
        #     uint8_t gainGB;
        #     uint8_t strengthR;
        #     uint8_t strengthB;
        #     uint8_t strengthGR;
        #     uint8_t strengthGB;
        #     uint64_t maskAndPwlLen;
        #     ISPIMGV2_BDN_PwlElementType pwlElements[ISPIMGV2_BDN_BLKCFG0_PWL_LEN];
        # } ISPIMGV2_BDN_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint8(bdn_xml_config_block.r_gain))
        serialization.extend(uint8(bdn_xml_config_block.b_gain))
        serialization.extend(uint8(bdn_xml_config_block.gr_gain))
        serialization.extend(uint8(bdn_xml_config_block.gb_gain))
        serialization.extend(uint8(bdn_xml_config_block.r_str))
        serialization.extend(uint8(bdn_xml_config_block.b_str))
        serialization.extend(uint8(bdn_xml_config_block.gr_str))
        serialization.extend(uint8(bdn_xml_config_block.gb_str))
        serialization.extend(uint64(maskAndPwlLen))

        for absissa, ordinate in bdn_pwl:
            serialization.extend(uint16(absissa))
            serialization.extend(uint16(ordinate))

        return BlockConfig(block_name, block_id,                                        \
            ISPBLOCK_BAYERDENOISE_BAYER_EN_MASK, ISPBLOCK_BAYERDENOISE_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_BayerDenoise():
    _block_config_structs_ = [
        ISPBlock_BayerDenoise_Config0,
    ]

###############################################################################
# Statistics
###############################################################################
ISPBLOCK_STATISTICS_ID = 0x6
ISPBLOCK_STATISTICS_BAYER_EN_MASK = (1 << 4)
ISPBLOCK_STATISTICS_YCBCR_EN_MASK = 0x0

class ISPBlock_Statistics_Config0():

    @staticmethod
    def parse(block_name, st_xml_config_block):

        row_factor = getattr(st_xml_config_block, "row_factor", 0)
        columns = getattr(st_xml_config_block, "columns", False)

        block_id = MakeSectionId(ISPBLOCK_STATISTICS_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Statistics_GroupType {
        #     uint16_t rLo;
        #     uint16_t rHi;
        #     uint16_t gLo;
        #     uint16_t gHi;
        #     uint16_t bLo;
        #     uint16_t bHi;
        #     int16_t rgLo;
        #     int16_t rgHi;
        #     int16_t bgLo;
        #     int16_t bgHi;
        # } ISPIMGV2_Statistics_GroupThresholdType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Statistics_Region_Group_0_Type {
        #     uint16_t offsetX;
        #     uint16_t offsetY;
        #     uint16_t width;
        #     uint16_t height;
        #     uint16_t hNum;
        #     uint16_t vNum;
        # } ISPIMGV2_Statistics_Region_Group_0_Type;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Statistics_Region_Group_1_2_Type {
        #     uint16_t offsetX;
        #     uint16_t offsetY;
        #     uint16_t width;
        #     uint16_t height;
        # } ISPIMGV2_Statistics_Region_Group_1_2_Type;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Statistics_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t frameWidth;
        #     uint16_t frameHeight;
        #     uint16_t hMask0;
        #     uint16_t hMask1;
        #     uint16_t rowFactor;
        #     uint8_t  pixelShift;
        #     uint8_t  shift;
        #     uint8_t  columns;
        #     uint8_t  numGroup1And2Regions;
        #     ISPIMGV2_Statistics_GroupThresholdType groupThresholds[3UL];
        #     ISPIMGV2_Statistics_Region_Group_0_Type   group0Regions;
        #     ISPIMGV2_Statistics_Region_Group_1_2_Type group1And2Regions[ISPIMGV2_STATISTICS_BLKCFG0_MAX_NUM_REGIONS];
        # } ISPIMGV2_Statistics_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        width_offset = len(serialization)
        serialization.extend(uint16(0))

        height_offset = len(serialization)
        serialization.extend(uint16(0))

        serialization.extend(uint16(st_xml_config_block.hmask_0))
        serialization.extend(uint16(st_xml_config_block.hmask_1))
        serialization.extend(uint16(row_factor))
        serialization.extend(uint8(st_xml_config_block.pix_shift))
        serialization.extend(uint8(st_xml_config_block.shift))
        serialization.extend(uint8(columns))
        serialization.extend(uint8(len(st_xml_config_block.regions_group_1_2.region)))

        # ISPIMGV2_Statistics_GroupThresholdType
        for tag in ("group_0", "group_1", "group_2"):
            if hasattr(st_xml_config_block.group_thresholds, tag):
                thresholds = getattr(st_xml_config_block.group_thresholds, tag)
                serialization.extend(uint16(thresholds.r_lo))
                serialization.extend(uint16(thresholds.r_hi))
                serialization.extend(uint16(thresholds.g_lo))
                serialization.extend(uint16(thresholds.g_hi))
                serialization.extend(uint16(thresholds.b_lo))
                serialization.extend(uint16(thresholds.b_hi))
                serialization.extend(int16(thresholds.rg_lo))
                serialization.extend(int16(thresholds.rg_hi))
                serialization.extend(int16(thresholds.bg_lo))
                serialization.extend(int16(thresholds.bg_hi))
            else:
                serialization.extend(uint16(0))
                serialization.extend(uint16(0))
                serialization.extend(uint16(0))
                serialization.extend(uint16(0))
                serialization.extend(uint16(0))
                serialization.extend(uint16(0))
                serialization.extend(int16(0))
                serialization.extend(int16(0))
                serialization.extend(int16(0))
                serialization.extend(int16(0))

        # ISPIMGV2_Statistics_Region_Group_0_Type
        serialization.extend(uint16(st_xml_config_block.regions_group_0.x_offset))
        serialization.extend(uint16(st_xml_config_block.regions_group_0.y_offset))
        serialization.extend(uint16(st_xml_config_block.regions_group_0.width))
        serialization.extend(uint16(st_xml_config_block.regions_group_0.height))
        serialization.extend(uint16(st_xml_config_block.regions_group_0.h_num))
        serialization.extend(uint16(st_xml_config_block.regions_group_0.v_num))

        # ISPIMGV2_Statistics_Region_Group_1_2_Type
        for rg in st_xml_config_block.regions_group_1_2.region:
            serialization.extend(uint16(rg.x_offset))
            serialization.extend(uint16(rg.y_offset))
            serialization.extend(uint16(rg.width))
            serialization.extend(uint16(rg.height))

        return ISPBlock_Statistics_Config0(block_name, block_id, serialization,     \
            width_offset, height_offset)

    def __init__(self, block_name, block_id, serialization, width_offset, height_offset):
        self.block_name = block_name
        self.id = block_id
        self.bayer_en_mask = ISPBLOCK_STATISTICS_BAYER_EN_MASK
        self.ycbcr_en_mask = ISPBLOCK_STATISTICS_YCBCR_EN_MASK
        self.serialization = serialization
        self.width_offset  = width_offset
        self.height_offset = height_offset
        return

    def patch_block(self, pass_config_list):
        tmp = filter(lambda block: block.block_name == "image_input", pass_config_list)
        assert(len(tmp) == 1)
        image_input_block = tmp[0]

        self.serialization[self.width_offset:self.width_offset+2] = uint16(image_input_block.frame_width)
        self.serialization[self.height_offset:self.height_offset+2] = uint16(image_input_block.frame_height)
        return

    def get_serialized_size(self):
        return len(self.serialization)

    def serialize(self, fhdl):
        fhdl.write(self.serialization)
        return

class ISPBlock_Statistics():
    _block_config_structs_ = [
        ISPBlock_Statistics_Config0,
    ]

###############################################################################
# White balance and gain
###############################################################################
ISPBLOCK_WBGAIN_ID = 0x7
ISPBLOCK_WBGAIN_BAYER_EN_MASK = (1 << 5)
ISPBLOCK_WBGAIN_YCBCR_EN_MASK = 0x0

class ISPBlock_WBAndGain_Config0():

    @staticmethod
    def parse(block_name, wb_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_WBGAIN_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_WBGain_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t gainR;
        #     uint16_t gainB;
        #     uint16_t gainAll;
        #     uint64_t offsets;
        #     uint16_t threshold;
        # } ISPIMGV2_WBGain_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint16(wb_xml_config_block.gain_r))
        serialization.extend(uint16(wb_xml_config_block.gain_b))
        serialization.extend(uint16(wb_xml_config_block.gain_all))
        serialization.extend(uint64(                \
            (wb_xml_config_block.offset_r) |        \
            (wb_xml_config_block.offset_g << 20) |  \
            (wb_xml_config_block.offset_b << 40)))
        serialization.extend(uint16(wb_xml_config_block.threshold))

        return BlockConfig(block_name, block_id,                            \
            ISPBLOCK_WBGAIN_BAYER_EN_MASK, ISPBLOCK_WBGAIN_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_WBAndGain():
    _block_config_structs_ = [
        ISPBlock_WBAndGain_Config0,
    ]

###############################################################################
# Defective pixel
###############################################################################
ISPBLOCK_DPC_ID = 0x8
ISPBLOCK_DPC_BAYER_EN_MASK = (1 << 7)
ISPBLOCK_DPC_YCBCR_EN_MASK = 0x0

class ISPBlock_DefectivePixel_Config0():

    @staticmethod
    def parse(block_name, dpc_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_DPC_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_DefectivePixel_BlkCfg0Type {
        #     uint16_t magic;
        #     int8_t  scaleLo[3U];
        #     int16_t offsetLo[3U];
        #     int8_t  scaleHi[3U];
        #     int16_t offsetHi[3U];
        #     uint8_t replaceIdxs;
        #     uint8_t replaceScaleLo;
        #     uint8_t replaceScaleHi;
        # } ISPIMGV2_DefectivePixel_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        for elt in ExtractIntegerListFromXML(dpc_xml_config_block.scale_lo):
            serialization.extend(int8(elt))

        for elt in ExtractIntegerListFromXML(dpc_xml_config_block.offset_lo):
            serialization.extend(int16(elt))

        for elt in ExtractIntegerListFromXML(dpc_xml_config_block.scale_hi):
            serialization.extend(int8(elt))

        for elt in ExtractIntegerListFromXML(dpc_xml_config_block.offset_hi):
            serialization.extend(int16(elt))

        serialization.extend(uint8(                         \
            (dpc_xml_config_block.replace_index_lo) |       \
            ((dpc_xml_config_block.replace_index_hi) << 4)))

        serialization.extend(uint8(dpc_xml_config_block.replace_scale_lo))
        serialization.extend(uint8(dpc_xml_config_block.replace_scale_hi))

        return BlockConfig(block_name, block_id,                            \
            ISPBLOCK_DPC_BAYER_EN_MASK, ISPBLOCK_DPC_YCBCR_EN_MASK,         \
            serialization)

class ISPBlock_DefectivePixel():
    _block_config_structs_ = [
        ISPBlock_DefectivePixel_Config0,
    ]

###############################################################################
# Fringing
###############################################################################
ISPBLOCK_FR_ID = 0x9
ISPBLOCK_FR_BAYER_EN_MASK = (1 << 13)
ISPBLOCK_FR_YCBCR_EN_MASK = 0x0

class ISPBlock_Fringing_Config0():

    @staticmethod
    def parse(block_name, fr_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_FR_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Fringing_BlkCfg0Type {
        #     uint16_t magic;
        #     int16_t rPos;
        #     int16_t rNeg;
        #     int16_t bPos;
        #     int16_t bNeg;
        #     int16_t grPos;
        #     int16_t grNeg;
        #     int16_t gbPos;
        #     int16_t gbNeg;
        #     uint16_t rPosLim;
        #     uint16_t rNegLim;
        #     uint16_t bPosLim;
        #     uint16_t bNegLim;
        #     uint16_t grPosLim;
        #     uint16_t grNegLim;
        #     uint16_t gbPosLim;
        #     uint16_t gbNegLim;
        # } ISPIMGV2_Fringing_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        serialization.extend(int16(fr_xml_config_block.r_pos))
        serialization.extend(int16(fr_xml_config_block.r_neg))
        serialization.extend(int16(fr_xml_config_block.b_pos))
        serialization.extend(int16(fr_xml_config_block.b_neg))
        serialization.extend(int16(fr_xml_config_block.gr_pos))
        serialization.extend(int16(fr_xml_config_block.gr_neg))
        serialization.extend(int16(fr_xml_config_block.gb_pos))
        serialization.extend(int16(fr_xml_config_block.gb_neg))

        serialization.extend(uint16(fr_xml_config_block.r_pos_lim))
        serialization.extend(uint16(fr_xml_config_block.r_neg_lim))
        serialization.extend(uint16(fr_xml_config_block.b_pos_lim))
        serialization.extend(uint16(fr_xml_config_block.b_neg_lim))
        serialization.extend(uint16(fr_xml_config_block.gr_pos_lim))
        serialization.extend(uint16(fr_xml_config_block.gr_neg_lim))
        serialization.extend(uint16(fr_xml_config_block.gb_pos_lim))
        serialization.extend(uint16(fr_xml_config_block.gb_neg_lim))

        return BlockConfig(block_name, block_id,                            \
            ISPBLOCK_FR_BAYER_EN_MASK, ISPBLOCK_FR_YCBCR_EN_MASK,           \
            serialization)

class ISPBlock_Fringing():
    _block_config_structs_ = [
        ISPBlock_Fringing_Config0,
    ]

###############################################################################
# Crosstalk
###############################################################################
ISPBLOCK_XC_ID = 0xA
ISPBLOCK_XC_BAYER_EN_MASK = (1 << 9)
ISPBLOCK_XC_YCBCR_EN_MASK = 0x0

class ISPBlock_Crosstalk_Config0():

    @staticmethod
    def parse(block_name, xc_xml_config_block):

        xc_pwl = ParseAbscissaeAndOrdinates(            \
            xc_xml_config_block.abscissae_xc_lim,       \
            xc_xml_config_block.ordinates_xc_lim)

        block_id = MakeSectionId(ISPBLOCK_XC_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Crosstalk_BlkCfg0Type {
        #     uint16_t magic;
        #     uint8_t pwlLength;
        #     ISPIMGV2_Crosstalk_PwlElementType pwlElements[ISPIMGV2_XC_BLKCFG0_PWL_LEN];
        # } ISPIMGV2_Crosstalk_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint8(len(xc_pwl)))

        for absissa, ordinate in xc_pwl:
            serialization.extend(uint16(absissa))
            serialization.extend(uint16(ordinate))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_XC_BAYER_EN_MASK, ISPBLOCK_XC_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_Crosstalk():
    _block_config_structs_ = [
        ISPBlock_Crosstalk_Config0,
    ]

###############################################################################
# Crosstalk
###############################################################################
ISPBLOCK_DM_ID = 0xC
ISPBLOCK_DM_BAYER_EN_MASK = (1 << 11)
ISPBLOCK_DM_YCBCR_EN_MASK = 0x0

class ISPBlock_Demosaic_Config0():

    @staticmethod
    def parse(block_name, dm_xml_config_block):

        #
        # Corresponds to the C type and macros defined as follows -
        #
        #   typedef uint8_t ISP_BayerType;
        #   #define ISP_BAYER_TYPE_TRUE       (0U)
        #   #define ISP_BAYER_TYPE_H_COSITED  (1U)
        #   #define ISP_BAYER_TYPE_V_COSITED  (2U)
        #   #define ISP_BAYER_TYPE_HV_COSITED (3U)
        #
        bayer_type_map = {"TRUE":0, "H_COSITED":1, "V_COSITED":2, "HV_COSITED":3}
        bayer_type = bayer_type_map[dm_xml_config_block.bayer_type]

        block_id = MakeSectionId(ISPBLOCK_DM_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Demosaic_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t threshold;
        #     uint16_t bayerTypeHVBiasAndOffShG;
        # } ISPIMGV2_Demosaic_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint16(dm_xml_config_block.threshold))
        serialization.extend(uint16(                \
            bayer_type |                            \
            (dm_xml_config_block.hv_bias << 2) |    \
            (dm_xml_config_block.g_offset_shift << 11)))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_DM_BAYER_EN_MASK, ISPBLOCK_DM_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_Demosaic():
    _block_config_structs_ = [
        ISPBlock_Demosaic_Config0,
    ]

###############################################################################
# Color denoise
###############################################################################
ISPBLOCK_CDN_ID = 0xB
ISPBLOCK_CDN_BAYER_EN_MASK = (1 << 15)
ISPBLOCK_CDN_YCBCR_EN_MASK = 0x0

class ISPBlock_ColorDenoise_Config0():

    @staticmethod
    def parse(block_name, cdn_xml_config_block):

        fir_bg_pwl = ParseAbscissaeAndOrdinates(        \
            cdn_xml_config_block.abscissae_fir_bg,      \
            cdn_xml_config_block.ordinates_fir_bg)

        fir_rg_pwl = ParseAbscissaeAndOrdinates(        \
            cdn_xml_config_block.abscissae_fir_rg,      \
            cdn_xml_config_block.ordinates_fir_rg)

        iir_bg_pwl = ParseAbscissaeAndOrdinates(        \
            cdn_xml_config_block.abscissae_iir_bg,      \
            cdn_xml_config_block.ordinates_iir_bg)

        iir_rg_pwl = ParseAbscissaeAndOrdinates(        \
            cdn_xml_config_block.abscissae_iir_rg,      \
            cdn_xml_config_block.ordinates_iir_rg)

        dist_pwl = ParseAbscissaeAndOrdinates(          \
            cdn_xml_config_block.abscissae_dist,        \
            cdn_xml_config_block.ordinates_dist)

        g_pwl = ParseAbscissaeAndOrdinates(             \
            cdn_xml_config_block.abscissae_g,           \
            cdn_xml_config_block.ordinates_g)

        sat_pwl = ParseAbscissaeAndOrdinates(           \
            cdn_xml_config_block.abscissae_sat,         \
            cdn_xml_config_block.ordinates_sat)

        x_center = 0
        y_center = 0

        if "center" in cdn_xml_config_block.__dict__:
            x_center = cdn_xml_config_block.center.x
            y_center = cdn_xml_config_block.center.y

        block_id = MakeSectionId(ISPBLOCK_CDN_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_ColorDenoise_PwlElementType {
        #     uint16_t abs;
        #     uint16_t ord;
        # } ISPIMGV2_ColorDenoise_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_ColorDenoise_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t misc;
        #     uint16_t xCenter;
        #     uint16_t yCenter;
        #     uint16_t edgeThreshold;
        #     uint16_t edgeStrength;
        #     uint16_t edgeMin;
        #     uint16_t weights;
        #     uint32_t pwlLengths;
        #     ISPIMGV2_ColorDenoise_PwlElementType pwlElements[7UL * ISPIMGV2_CDN_BLKCFG0_PWL_LEN];
        # } ISPIMGV2_ColorDenoise_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint16(                                \
            (cdn_xml_config_block.fir_shift) |                      \
            (cdn_xml_config_block.iir_attenuation_lines << 2) |     \
            (cdn_xml_config_block.iir_attenuation_shift << 6)       \
        ))
        serialization.extend(uint16(x_center))
        serialization.extend(uint16(y_center))
        serialization.extend(uint16(cdn_xml_config_block.edge_threshold))
        serialization.extend(uint16(cdn_xml_config_block.edge_strength))
        serialization.extend(uint16(cdn_xml_config_block.edge_min))
        serialization.extend(uint16(
            (cdn_xml_config_block.g_wt)         |
            (cdn_xml_config_block.bg_wt << 4)   |
            (cdn_xml_config_block.rg_wt << 8)
        ))
        serialization.extend(uint32(
            (len(fir_bg_pwl))           |
            (len(fir_rg_pwl) << 3)      |
            (len(iir_bg_pwl) << 6)      |
            (len(iir_rg_pwl) << 9)      |
            (len(dist_pwl)   << 12)     |
            (len(g_pwl) << 15)          |
            (len(sat_pwl) << 18)
        ))

        pwls = itertools.chain(fir_bg_pwl, fir_rg_pwl, iir_bg_pwl, iir_rg_pwl, dist_pwl, g_pwl, sat_pwl)
        for absissa, ordinate in pwls:
            serialization.extend(uint16(absissa))
            serialization.extend(uint16(ordinate))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_CDN_BAYER_EN_MASK, ISPBLOCK_CDN_YCBCR_EN_MASK, \
            serialization)

class ISPBlock_ColorDenoise():
    _block_config_structs_ = [
        ISPBlock_ColorDenoise_Config0,
    ]

###############################################################################
# Sharpen
###############################################################################
ISPBLOCK_SH_ID = 0xD
ISPBLOCK_SH_BAYER_EN_MASK = 0x0
ISPBLOCK_SH_YCBCR_EN_MASK = (1 << 6)

class ISPBlock_Sharpen_Config0():

    @staticmethod
    def parse(block_name, sh_xml_config_block):

        pos_gain_pwl = ParseAbscissaeAndOrdinates(      \
            sh_xml_config_block.abscissae_pos_gain,     \
            sh_xml_config_block.ordinates_pos_gain)

        v_gain_pwl = ParseAbscissaeAndOrdinates(        \
            sh_xml_config_block.abscissae_v_gain,       \
            sh_xml_config_block.ordinates_v_gain)

        resp_pwl = ParseAbscissaeAndOrdinates(          \
            sh_xml_config_block.abscissae_resp,         \
            sh_xml_config_block.ordinates_resp)

        resp1_pwl = ParseAbscissaeAndOrdinates(         \
            sh_xml_config_block.abscissae_resp1,        \
            sh_xml_config_block.ordinates_resp1)

        resp_gain_pwl = ParseAbscissaeAndOrdinates(     \
            sh_xml_config_block.abscissae_resp_gain,    \
            sh_xml_config_block.ordinates_resp_gain)

        block_id = MakeSectionId(ISPBLOCK_SH_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Sharpen_PwlElementType {
        #     uint16_t abs;
        #     uint16_t ord;
        # } ISPIMGV2_Sharpen_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Sharpen_BlkCfg0Type {
        #     uint16_t magic;
        #     uint8_t  avgShiftG;
        #     uint16_t isotropicStrength;
        #     uint16_t xCenter;
        #     uint16_t yCenter;
        #     uint32_t pwlLengths;
        #     ISPIMGV2_Sharpen_PwlElementType pwlElements[56U];
        # } ISPIMGV2_Sharpen_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint8(sh_xml_config_block.avg_g_shift))
        serialization.extend(uint16(sh_xml_config_block.isotropic_strength))
        serialization.extend(uint16(sh_xml_config_block.x_center))
        serialization.extend(uint16(sh_xml_config_block.y_center))
        serialization.extend(uint32(                                \
            (len(pos_gain_pwl)) |                                   \
            (len(v_gain_pwl) << 4) |                                \
            (len(resp_gain_pwl) << 8) |                             \
            (len(resp_pwl) << 13) |                                 \
            (len(resp1_pwl) << 17)))

        pwls = itertools.chain(pos_gain_pwl, v_gain_pwl, resp_gain_pwl)
        for absissa, ordinate in pwls:
            serialization.extend(uint16(absissa))
            serialization.extend(uint16(ordinate))

        pwls = itertools.chain(resp_pwl, resp1_pwl)
        for absissa, ordinate in pwls:
            serialization.extend(int16(absissa))
            serialization.extend(int16(ordinate))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_SH_BAYER_EN_MASK, ISPBLOCK_SH_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_Sharpen():
    _block_config_structs_ = [
        ISPBlock_Sharpen_Config0,
    ]

###############################################################################
# Color correction
###############################################################################
ISPBLOCK_YC_ID = 0xE
ISPBLOCK_YC_BAYER_EN_MASK = 0x0
ISPBLOCK_YC_YCBCR_EN_MASK = (1 << 1)

class ISPBlock_ColorCorrection_Config0():

    @staticmethod
    def parse(block_name, yc_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_YC_ID, 0)
        offsets = ExtractIntegerListFromXML(yc_xml_config_block.offsets)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_YCYG_BlkCfg0Type {
        #     uint16_t magic;
        #     int16_t matrix[9U];
        #     uint8_t  offsetSignBits;
        #     uint16_t offset0Bits16;
        #     uint16_t offset1Bits16;
        #     uint16_t offset2Bits16;
        # } ISPIMGV2_YCYG_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        for mat_elt in ExtractIntegerListFromXML(yc_xml_config_block.matrix):
            serialization.extend(int16(mat_elt))

        serialization.extend(uint8(             \
            ((offsets[0] & 0x10000) >> 16)   |  \
            ((offsets[1] & 0x10000) >> 15)   |  \
            ((offsets[1] & 0x10000) >> 14)))

        serialization.extend(uint16(offsets[0] & 0xFFFF))
        serialization.extend(uint16(offsets[1] & 0xFFFF))
        serialization.extend(uint16(offsets[2] & 0xFFFF))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_YC_BAYER_EN_MASK, ISPBLOCK_YC_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_ColorCorrection():
    _block_config_structs_ = [
        ISPBlock_ColorCorrection_Config0,
    ]

###############################################################################
# Gamma correction
###############################################################################
ISPBLOCK_GM_ID = 0xF
ISPBLOCK_GM_BAYER_EN_MASK = 0x0
ISPBLOCK_GM_YCBCR_EN_MASK = (1 << 2)

class ISPBlock_Gamma_Config0():

    @staticmethod
    def parse(block_name, gm_xml_config_block):

        r_pwl = ParseAbscissaeAndOrdinates(     \
            gm_xml_config_block.abscissae_r,    \
            gm_xml_config_block.ordinates_r)

        g_pwl = ParseAbscissaeAndOrdinates(     \
            gm_xml_config_block.abscissae_g,    \
            gm_xml_config_block.ordinates_g)

        b_pwl = ParseAbscissaeAndOrdinates(     \
            gm_xml_config_block.abscissae_b,    \
            gm_xml_config_block.ordinates_b)

        block_id = MakeSectionId(ISPBLOCK_GM_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Gamma_PwlElementType {
        #     uint16_t abs;
        #     uint16_t ord;
        # } ISPIMGV2_Gamma_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Gamma_BlkCfg0Type {
        #     uint16_t magic;
        #     uint32_t pwlLengths;
        #     ISPIMGV2_Gamma_PwlElementType pwlElements[3UL * 32UL];
        # } ISPIMGV2_Gamma_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint32(        \
            (len(r_pwl))        |           \
            (len(g_pwl) << 6)   |           \
            (len(b_pwl) << 12)))

        for absissa, ordinate in itertools.chain(r_pwl, g_pwl, b_pwl):
            serialization.extend(uint16(absissa))
            serialization.extend(uint16(ordinate))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_GM_BAYER_EN_MASK, ISPBLOCK_GM_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_Gamma():
    _block_config_structs_ = [
        ISPBlock_Gamma_Config0,
    ]

###############################################################################
# YCBCR conversion conversion
###############################################################################
ISPBLOCK_YG_ID = 0x10
ISPBLOCK_YG_BAYER_EN_MASK = 0x0
ISPBLOCK_YG_YCBCR_EN_MASK = (1 << 3)

class ISPBlock_YCBCRConversion_Config0():

    @staticmethod
    def parse(block_name, yg_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_YG_ID, 0)
        offsets = ExtractIntegerListFromXML(yg_xml_config_block.offsets)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_YCYG_BlkCfg0Type {
        #     uint16_t magic;
        #     int16_t matrix[9U];
        #     uint8_t  offsetSignBits;
        #     uint16_t offset0Bits16;
        #     uint16_t offset1Bits16;
        #     uint16_t offset2Bits16;
        # } ISPIMGV2_YCYG_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        for mat_elt in ExtractIntegerListFromXML(yg_xml_config_block.matrix):
            serialization.extend(int16(mat_elt))

        serialization.extend(uint8(             \
            ((offsets[0] & 0x10000) >> 16)   |  \
            ((offsets[1] & 0x10000) >> 15)   |  \
            ((offsets[1] & 0x10000) >> 14)))

        serialization.extend(uint16(offsets[0] & 0xFFFF))
        serialization.extend(uint16(offsets[1] & 0xFFFF))
        serialization.extend(uint16(offsets[2] & 0xFFFF))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_YG_BAYER_EN_MASK, ISPBLOCK_YG_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_YCBCRConversion():
    _block_config_structs_ = [
        ISPBlock_YCBCRConversion_Config0,
    ]

###############################################################################
# False color
###############################################################################
ISPBLOCK_FC_ID = 0x11
ISPBLOCK_FC_BAYER_EN_MASK = 0x0
ISPBLOCK_FC_YCBCR_EN_MASK = (1 << 5)

class ISPBlock_FalseColor_Config0():

    @staticmethod
    def parse(block_name, fc_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_FC_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_FC_BlkCfg0Type {
        #     uint16_t magic;
        #     uint8_t strength1;
        #     uint8_t strength2;
        #     uint8_t filterMixWt;
        #     uint8_t filterDesatWt;
        #     uint16_t filterMixLo;
        #     uint16_t filterMixHi;
        #     uint16_t filterDesatLo;
        #     uint16_t filterDesatHi;
        # } ISPIMGV2_FC_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint8(fc_xml_config_block.strength1))
        serialization.extend(uint8(fc_xml_config_block.strength2))
        serialization.extend(uint8(fc_xml_config_block.filter_mix_cw))
        serialization.extend(uint8(fc_xml_config_block.filter_desat_cw))
        serialization.extend(uint16(fc_xml_config_block.filter_mix_lo))
        serialization.extend(uint16(fc_xml_config_block.filter_mix_hi))
        serialization.extend(uint16(fc_xml_config_block.filter_desat_lo))
        serialization.extend(uint16(fc_xml_config_block.filter_desat_hi))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_FC_BAYER_EN_MASK, ISPBLOCK_FC_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_FalseColor():
    _block_config_structs_ = [
        ISPBlock_FalseColor_Config0,
    ]

###############################################################################
# Local tone mapping
###############################################################################
ISPBLOCK_LTM_ID = 0x12
ISPBLOCK_LTM_BAYER_EN_MASK = 0x0
ISPBLOCK_LTM_YCBCR_EN_MASK = (1 << 25)

class ISPBlock_LocalToneMapping_Config0():

    @staticmethod
    def parse(block_name, ltm_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_LTM_ID, 0)

        base_pwl = ParseAbscissaeAndOrdinates(      \
            ltm_xml_config_block.abscissae_b,       \
            ltm_xml_config_block.ordinates_b)

        detail_pwl = ParseAbscissaeAndOrdinates(    \
            ltm_xml_config_block.abscissae_d,       \
            ltm_xml_config_block.ordinates_d)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_LTM_PwlElementType {
        #     uint16_t abs;
        #     uint16_t ord;
        # } ISPIMGV2_LTM_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_LTM_BlkCfg0Type {
        #     uint16_t magic;
        #     uint8_t  khp;
        #     uint8_t  kcolor;
        #     uint16_t epsilonVal;
        #     uint32_t divSat;
        #     uint16_t pwlLengthsAndGfEn;
        #     uint32_t gaussianKernel[81UL];
        #     ISPIMGV2_LTM_PwlElementType pwlElements[64UL];
        # } ISPIMGV2_LTM_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint8(ltm_xml_config_block.k_hp))
        serialization.extend(uint8(ltm_xml_config_block.k_color))
        serialization.extend(uint16(ltm_xml_config_block.epsilon))
        serialization.extend(uint32(ltm_xml_config_block.div_sat))
        serialization.extend(uint16(        \
            (len(base_pwl))         |       \
            (len(detail_pwl) << 6)  |       \
            (ltm_xml_config_block.gf_en << 13)))

        for lpf_elt in ExtractIntegerListFromXML(ltm_xml_config_block.gaussian_lpf):
            serialization.extend(uint32(lpf_elt))

        for absissa, ordinate in itertools.chain(base_pwl, detail_pwl):
            serialization.extend(uint16(absissa))
            serialization.extend(uint16(ordinate))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_LTM_BAYER_EN_MASK, ISPBLOCK_LTM_YCBCR_EN_MASK, \
            serialization)

class ISPBlock_LocalToneMapping():
    _block_config_structs_ = [
        ISPBlock_LocalToneMapping_Config0,
    ]

###############################################################################
# Tone Mapping
###############################################################################
ISPBLOCK_TM_ID = 0x13
ISPBLOCK_TM_BAYER_EN_MASK = 0x0
ISPBLOCK_TM_YCBCR_EN_MASK = (1 << 14)

class ISPBlock_ISPBlock_ToneMapping_Config0():

    @staticmethod
    def parse(block_name, tm_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_TM_ID, 0)

        y_pwl = ParseAbscissaeAndOrdinates(     \
            tm_xml_config_block.abscissae_y,    \
            tm_xml_config_block.ordinates_y)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_TM_PwlElementType {
        #     uint16_t abs;
        #     uint16_t ord;
        # } ISPIMGV2_TM_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_TM_BlkCfg0Type {
        #     uint16_t magic;
        #     uint8_t  pwlLength;
        #     ISPIMGV2_TM_PwlElementType pwlElements[32UL];
        # } ISPIMGV2_TM_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))
        serialization.extend(uint8(len(y_pwl)))

        for absissa, ordinate in itertools.chain(y_pwl):
            serialization.extend(uint16(absissa))
            serialization.extend(uint16(ordinate))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_TM_BAYER_EN_MASK, ISPBLOCK_TM_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_ToneMapping():
    _block_config_structs_ = [
        ISPBlock_ISPBlock_ToneMapping_Config0,
    ]

###############################################################################
# Chrominance stretch
###############################################################################
ISPBLOCK_CP_ID = 0x14
ISPBLOCK_CP_BAYER_EN_MASK = 0x0
ISPBLOCK_CP_YCBCR_EN_MASK = (1 << 8)

class ISPBlock_ChrominanceStretch_Config0():

    @staticmethod
    def parse(block_name, cp_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_CP_ID, 0)

        u_pwl = ParseAbscissaeAndOrdinates(     \
            cp_xml_config_block.abscissae_u,    \
            cp_xml_config_block.ordinates_u)

        v_pwl = ParseAbscissaeAndOrdinates(     \
            cp_xml_config_block.abscissae_v,
            cp_xml_config_block.ordinates_v)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_CP_PwlElementType {
        #     int16_t abs;
        #     int16_t ord;
        # } ISPIMGV2_CP_PwlElementType;
        #
        # typedef struct COMP_PACKED _ISPIMGV2_CP_BlkCfg0Type {
        #     uint16_t magic;
        #     uint8_t  pwlLengths;
        #     ISPIMGV2_CP_PwlElementType pwlElements[16UL];
        # } ISPIMGV2_CP_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        serialization.extend(uint8(     \
            (len(u_pwl))        |       \
            (len(v_pwl) << 4)))

        for absissa, ordinate in itertools.chain(u_pwl, v_pwl):
            serialization.extend(int16(absissa))
            serialization.extend(int16(ordinate))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_CP_BAYER_EN_MASK, ISPBLOCK_CP_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_ChrominanceStretch():
    _block_config_structs_ = [
        ISPBlock_ChrominanceStretch_Config0,
    ]

###############################################################################
# Color conversion
###############################################################################
ISPBLOCK_CC_ID = 0x15
ISPBLOCK_CC_BAYER_EN_MASK = 0x0
ISPBLOCK_CC_YCBCR_EN_MASK = (1 << 10)

class ISPBlock_ColorConversion_Config0():

    @staticmethod
    def parse(block_name, cc_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_CC_ID, 0)

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_CC_BlkCfg0Type {
        #     uint16_t magic;
        #     int16_t matrix[9U];
        #     int32_t offsets[3U]
        # } ISPIMGV2_CC_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(block_id))

        for mat_elt in ExtractIntegerListFromXML(cc_xml_config_block.matrix):
            serialization.extend(int16(mat_elt))

        for off_elt in ExtractIntegerListFromXML(cc_xml_config_block.offsets):
            serialization.extend(int32(off_elt))

        return BlockConfig(block_name, block_id,                    \
            ISPBLOCK_CC_BAYER_EN_MASK, ISPBLOCK_CC_YCBCR_EN_MASK,   \
            serialization)

class ISPBlock_ColorConversion():
    _block_config_structs_ = [
        ISPBlock_ColorConversion_Config0,
    ]

###############################################################################
# LR resize
###############################################################################
ISPBLOCK_LR_ID = 0x16
ISPBLOCK_LR_BAYER_EN_MASK = 0x0
ISPBLOCK_LR_YCBCR_EN_MASK = (1 << 11)

class ISPBlock_LRResize_Config0():

    @staticmethod
    def parse(block_name, lr_xml_config_block):

        block_id = MakeSectionId(ISPBLOCK_LR_ID, 0)

        try:
            rb_swap = lr_xml_config_block.rb_swap
        except Exception as e:
            rb_swap = False

        return ISPBlock_LRResize_Config0(block_name, block_id,                      \
            lr_xml_config_block.output_width, lr_xml_config_block.output_height,    \
            lr_xml_config_block.shift, rb_swap)

    def __init__(self, block_name, block_id, output_width, output_height, shift, rb_swap):
        self.block_name = block_name
        self.id = block_id
        self.bayer_en_mask = ISPBLOCK_LR_BAYER_EN_MASK
        self.ycbcr_en_mask = ISPBLOCK_LR_YCBCR_EN_MASK
        self.output_width = output_width
        self.output_height = output_height
        self.shift = shift
        self.rb_swap = rb_swap
        return

    def patch_block(self, pass_config_list):
        tmp = filter(lambda block: block.block_name == "image_input", pass_config_list)
        assert(len(tmp) == 1)
        image_input_block = tmp[0]

        self.frame_width = image_input_block.frame_width
        self.frame_height = image_input_block.frame_height
        return

    def __construct_serialization_byte_array__(self):
        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_LR_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t frameWidth;
        #     uint16_t frameHeight;
        #     uint32_t outputWidth;
        #     uint32_t outputHeight;
        #     uint8_t  shiftAndRbSwap;
        # } ISPIMGV2_LR_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(self.id))
        serialization.extend(uint16(self.frame_width))
        serialization.extend(uint16(self.frame_height))
        serialization.extend(uint32(self.output_width))
        serialization.extend(uint32(self.output_height))
        serialization.extend(uint8(self.shift | (self.rb_swap << 4)))

        return serialization

    def get_serialized_size(self):
        return len(self.__construct_serialization_byte_array__())

    def serialize(self, fhdl):
        fhdl.write(self.__construct_serialization_byte_array__())
        return

class ISPBlock_LRResize():
    _block_config_structs_ = [
        ISPBlock_LRResize_Config0,
    ]

###############################################################################
# Output blocks
###############################################################################
ISPBLOCK_HO_ID = 0x17
ISPBLOCK_HO_BAYER_EN_MASK = 0x0
ISPBLOCK_HO_YCBCR_EN_MASK = (1 << 12)

ISPBLOCK_LO_ID = 0x18
ISPBLOCK_LO_BAYER_EN_MASK = 0x0
ISPBLOCK_LO_YCBCR_EN_MASK = (1 << 13)

def compute_output_strides(output_width, output_height, data_format, bytes_per_sample):

    stride1 = 0
    stride2 = 0

    data_format = str(data_format)

    if data_format == "INTERLEAVED":
        stride1 = 3 * output_width * bytes_per_sample
    elif data_format == "565":
        stride1 = 2 * output_width
    elif (data_format == "RGBA32_0") or (data_format == "RGBA32_255"):
        stride1 = 4 * output_width * bytes_per_sample
    elif data_format == "444":
        stride1 = output_width * bytes_per_sample
        stride2 = ((output_width * bytes_per_sample) + 31) & ~31
    elif (data_format == "422") or (data_format == "420"):
        stride1 = output_width * bytes_per_sample
        stride2 = ((output_width + 1) // 2) * bytes_per_sample
    elif (data_format == "YUYV") or (data_format == "YVYU") or (data_format == "UYVY") or (data_format == "VYUY"):
        stride1 = 2 * output_width * bytes_per_sample
    else:
        raise Exception("Unreachable!")

    stride1 = (stride1 + 31) & ~31
    stride2 = (stride2 + 15) & ~15

    return (stride1, stride2)

class ISPBlock_Output_Config0():

    @staticmethod
    def parse(block_name, output_xml_config_block):
        return ISPBlock_Output_Config0(block_name, output_xml_config_block)

    def __init__(self, block_name, xml_config):

        self.block_name = block_name
        self.xml_cfg = xml_config

        if block_name == "hires_output":
            self.id = MakeSectionId(ISPBLOCK_HO_ID, 0)
            self.bayer_en_mask = ISPBLOCK_HO_BAYER_EN_MASK
            self.ycbcr_en_mask = ISPBLOCK_HO_YCBCR_EN_MASK
        else:
            self.id = MakeSectionId(ISPBLOCK_LO_ID, 0)
            self.bayer_en_mask = ISPBLOCK_LO_BAYER_EN_MASK
            self.ycbcr_en_mask = ISPBLOCK_LO_YCBCR_EN_MASK
        return

    def patch_block(self, pass_config_list):

        if self.xml_cfg.output_width == 1:
            bytes_per_sample = 2
        else:
            bytes_per_sample = 1

        tmp = filter(lambda block: block.block_name == "image_input", pass_config_list)
        assert(len(tmp) == 1)
        image_input_block = tmp[0]

        output_width = image_input_block.frame_width
        output_height = image_input_block.frame_height

        if self.block_name == "lores_output":
            lresize_blocks = filter(lambda block: block.block_name == "lr_resize", pass_config_list)
            if len(lresize_blocks) > 0:
                output_width = lresize_blocks[0].output_width
                output_height = lresize_blocks[0].output_height

        stride1, stride2 = compute_output_strides(output_width, output_height,  \
            self.xml_cfg.data_format, bytes_per_sample)

        self.stride1 = stride1
        self.stride2 = stride2

        return

    def __construct_serialization_byte_array__(self):

        data_format_map = {"INTERLEAVED":0, "565":1, "RGBA32_0":2, "RGBA32_255":3,  \
            "444":4, "422":5, "420":6, "YUYV":8, "YVYU":9, "UYVY":10, "VYUY":11}

        #
        # Corresponding C struct is defined as -
        #
        # typedef struct COMP_PACKED _ISPIMGV2_Output_BlkCfg0Type {
        #     uint16_t magic;
        #     uint16_t misc;
        #     int32_t  stride1;
        #     int32_t  stride2;
        # } ISPIMGV2_Output_BlkCfg0Type;
        #
        serialization = bytearray()

        serialization.extend(uint16(self.id))
        serialization.extend(uint16(                \
            (self.xml_cfg.vflip)                |   \
            (self.xml_cfg.big_endian << 1)      |   \
            (self.xml_cfg.output_width << 2)    |   \
            (self.xml_cfg.sat_depth << 3)       |   \
            (self.xml_cfg.shift << 6)           |   \
            (data_format_map[str(self.xml_cfg.data_format)] << 10)))
        serialization.extend(int32(self.stride1))
        serialization.extend(int32(self.stride2))

        return serialization

    def get_serialized_size(self):
        return len(self.__construct_serialization_byte_array__())

    def serialize(self, fhdl):
        fhdl.write(self.__construct_serialization_byte_array__())
        return

class ISPBlock_HresOutput():
     _block_config_structs_ = [
        ISPBlock_Output_Config0,
    ]

class ISPBlock_LresOutput():
     _block_config_structs_ = [
        ISPBlock_Output_Config0,
    ]

###############################################################################
# Helpers to generate images
###############################################################################
class PassSerializer:

    def __init__(self, pass_id, section_list):
        self.hdr_size =             \
            ctypes.sizeof(uint16) + \
            (len(section_list) * (ctypes.sizeof(uint8) + ctypes.sizeof(uint16)))

        self.pass_id = pass_id
        self.section_list = section_list

        # Compute section offsets
        section_offsets = [self.hdr_size]

        self.section_offsets = functools.reduce(                                        \
            lambda acc_list, s: acc_list + [acc_list[-1] + s.get_serialized_size()],    \
            section_list, section_offsets)

    def get_serialized_size(self):
        return (self.hdr_size) + functools.reduce(lambda acc, s: acc + s.get_serialized_size(), self.section_list, 0)

    def set_base_offset(self, base_offset):
        self.section_offsets_global = map(lambda offset: offset + base_offset, self.section_offsets)

    def serialize(self, fhdl):

        # Write header
        fhdl.write(uint16(len(self.section_list)))
        for section, offset in zip(self.section_list, self.section_offsets_global):
            fhdl.write(uint8(section.id))
            fhdl.write(uint16(offset))

        # Serialize sections
        for section in self.section_list:
            section.serialize(fhdl)

class ImageSerializer:

    def __init__(self, passes):
        self.hdr_size =                         \
            (2 * ctypes.sizeof(uint32)) +       \
            ctypes.sizeof(uint8) +              \
            (len(passes) * (ctypes.sizeof(uint8) + ctypes.sizeof(uint16)))

        self.passes = passes

        # Compute pass offsets in the image.
        pass_offsets = [self.hdr_size]
        self.pass_offsets = functools.reduce(                                           \
            lambda acc_list, s: acc_list + [acc_list[-1] + s.get_serialized_size()],    \
            passes, pass_offsets)

        # For now, just set the base offset to zero
        self.set_base_offset(0)

    def set_base_offset(self, base_offset):
        self.pass_offsets_global = map(lambda offset: offset + base_offset, self.pass_offsets)

        # Set these base offsets on passes
        for p, base_offset in zip(self.passes, self.pass_offsets_global):
            p.set_base_offset(base_offset)

    def serialize(self, fhdl):

        # Write header
        fhdl.write(uint32(2))           # ISP Config in the C struct ISYS_CommonImgHdrType
        fhdl.write(uint32(0xABCE1234))  # Magic
        fhdl.write(uint8(len(self.passes)))
        for p, offset in zip(self.passes, self.pass_offsets_global):
            fhdl.write(uint8(p.pass_id))
            fhdl.write(uint16(offset))

        # Serialize passes
        for p in self.passes:
            p.serialize(fhdl)

def generate_config_images(xml_config_filename, schema_filename, *output_filenames):

    assert len(output_filenames) == len(SUPPORTED_CONFIGS), \
        "We expect to write out one binary file for every supported config block"

    #
    # Parse the config XML file
    #
    with open(xml_config_filename, 'rb') as xml_file_hdl:
        xml_content = xml_file_hdl.read()
        xml_cfg = objectify.fromstring(xml_content, \
            objectify.makeparser(schema = etree.XMLSchema(file = schema_filename)))

    #
    # parsed_configs below is a dictionary.
    # Key   - name of the pass.
    # Value - list of length SUPPORTED_CONFIGS. Each element of this list is
    #         another list holding the block configs set for that pass.
    #
    parsed_configs = {pass_name:[[] for _ in SUPPORTED_CONFIGS] \
                        for pass_name in xml_cfg.__dict__ if pass_name != "comment"}

    assert "common_config" in parsed_configs

    #
    # Populate the parsed_configs data structure from the XML configuration.
    #
    for pass_name, pass_parsed_config_list in parsed_configs.iteritems():

        assert pass_name in SUPPORTED_PASS_NAME_TO_ID_MAP

        # Grab the XML object corresponding to this pass
        pass_xml_obj = getattr(xml_cfg, pass_name)

        # Go over each of its elements and try parsing them.
        for attr in pass_xml_obj.__dict__:

            if attr == "comment":
                continue

            parse_succeded = False
            for parsed_config_list, config_type in zip(pass_parsed_config_list, SUPPORTED_CONFIGS):
                ret = config_type.parse(attr, getattr(pass_xml_obj, attr))
                if ret != None:
                    parse_succeded = True
                    parsed_config_list.append(ret)
                    break

            if parse_succeded == False:
                raise Exception("Unknown config block \"{}\" encountered.".format(attr))

    #
    # Issue global validation
    #
    validate_parsed_config(parsed_configs)

    #
    # All good. Construct the serialization helpers, and issue serialization.
    #
    for output_file_name, config_type in zip(output_filenames, SUPPORTED_CONFIGS):

        passes = []

        for pass_name, pass_parsed_config_list in parsed_configs.iteritems():
            p = PassSerializer(                                                 \
                SUPPORTED_PASS_NAME_TO_ID_MAP[pass_name],                       \
                pass_parsed_config_list[SUPPORTED_CONFIGS.index(config_type)])

            passes.append(p)

        img = ImageSerializer(passes)

        with open(output_file_name, 'wb') as out_file_hdl:
            b = io.BytesIO()
            bhdl = io.BufferedWriter(b)
            img.serialize(bhdl)
            bhdl.flush()
            out_file_hdl.write(b.getvalue())

###############################################################################
# Function to write out a C file that turns an image into a C array
###############################################################################
c_copyright_header = \
"""/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

*****************************************************************************/
"""
def generate_header_file_from_image(image_file_path, header_file_path):

    assert header_file_path.endswith(".h")

    with open(image_file_path, 'rb') as img_hdl:
        img = img_hdl.read()

    with open(header_file_path, "w") as header_hdl:
        header_name = os.path.basename(header_file_path)
        header_name_no_extension = header_name[:len(header_name) - 2]

        guard_name = header_name_no_extension.upper() + "_H"

        header_hdl.write(c_copyright_header)
        header_hdl.write("#ifndef " + guard_name + "\n")
        header_hdl.write("#define " + guard_name + "\n\n")
        header_hdl.write("static const uint8_t " + header_name_no_extension + "_array[] = {\n")

        for idx, byte in enumerate(img):
            header_hdl.write(" 0x%02x"%int(ord(byte)))
            header_hdl.write(",")
            if (idx + 1) % 16 == 0:
                header_hdl.write("\n")
        header_hdl.write("\n};\n")

        header_hdl.write("#endif")

###############################################################################
# The main routine
###############################################################################
def main():

    configs_input_folder = "inputs"
    configs_image_folder = "outputs"
    schema_file_name = "schema_isp_config.xsd"

    #
    # Grab all the isp config files from the input folder
    #
    config_files = []
    for r, d, f in os.walk(configs_input_folder):
        if "isp.xml" in f:
            h, config_name = split(r)
            assert(h == configs_input_folder)
            config_files.append((config_name, join(r, "isp.xml")))

    print("Number of ISP and tuner configuration XML files found : {}".format(len(config_files)))

    #
    # Parse files, and write the output files.
    #
    for isp_cfg_name, isp_cfg_file_path in config_files:
        print("Parsing {} ISP config".format(isp_cfg_name))

        output_folder = join(configs_image_folder, isp_cfg_name)
        if not os.path.exists(output_folder):
            os.mkdir(output_folder)

        isp_image_file_path = join(output_folder, "isp.img")
        tuner_image_file_path = join(output_folder, "tuner.img")

        isp_image_header_file_path = join(output_folder, isp_cfg_name + "_isp_image.h")
        tuner_image_header_file_path = join(output_folder, isp_cfg_name + "_tuner_image.h")

        try:
            generate_config_images(isp_cfg_file_path, schema_file_name, isp_image_file_path, tuner_image_file_path)

            generate_header_file_from_image(isp_image_file_path, isp_image_header_file_path)
            generate_header_file_from_image(tuner_image_file_path, tuner_image_header_file_path)

            print("ISP config {} read, Images generated.".format(isp_cfg_name))

        except Exception as e:
            print("Failed to parse {} ISP configuration.".format(isp_cfg_name))
            print(e.__doc__)
            print(e.message)
            print(traceback.format_exc())

main()

