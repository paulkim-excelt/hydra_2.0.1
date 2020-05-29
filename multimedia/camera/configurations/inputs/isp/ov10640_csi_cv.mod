include ov10640_csi_hv.mod

FRAME
width: 1280
height: 720
bayer_order: GRBG
bayer_enable:
  image_input
  black_level
  lens_shading
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
  #gamma
  ycbcr_conversion
  false_colour
  chrominance_stretch
  #conversion
  #lresize
  hi_res_output
  #lo_res_output
*
END