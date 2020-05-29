include ox03a10_csi_720p_16_dcg_12_vs_isp_hdr.mod

PIPELINE
   isp_bits_per_pixel: 16
END

ENABLE_MODELESS
    agc:            FALSE
    awb:            FALSE
END

ENABLE_MODE
    denoise:        FALSE
    cc:             FALSE
    defpix:         FALSE
END

FRAME
width: 1280
height: 720
bayer_order: BGGR
bayer_enable:
  image_input
  dark_frame_input
  black_level
  lens_shading
  high_dynamic_range
  #denoise
  #statistics
  #white_balance_gain
  #defective_pixel_auto
  #crosstalk
  #fringing
  #colour_denoise
  demosaic
 *
ycbcr_enable:
 #sharpen
 colour_correction
 gamma
 ycbcr_conversion
 #false_colour
 chrominance_stretch
 #conversion
 #lresize
 #hi_res_output
 lo_res_output
*
END
