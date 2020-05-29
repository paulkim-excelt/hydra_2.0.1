include ox03a10_csi_1080p_16_dcg_12_vs_isp_hdr.mod

FRAME
width: 1920
height: 1080
bayer_order: BGGR
bayer_enable:
  image_input
  dark_frame_input
  black_level
  lens_shading
  high_dynamic_range
  #denoise
  statistics
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
 lresize
 #hi_res_output
 lo_res_output
*
END

LRESIZE
output_width:  1280
output_height: 720
shift: 0
END
