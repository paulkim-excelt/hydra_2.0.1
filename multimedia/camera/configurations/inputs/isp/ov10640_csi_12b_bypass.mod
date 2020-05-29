PIPELINE
   isp_bits_per_pixel: 16
END

FRAME
width: 1280
height: 720
bayer_order: BGGR
bayer_enable:
  image_input
 *
ycbcr_enable:
 hi_res_output
*
END

IMAGE_INPUT
format: RAW12
stride: 1920
transposed: 0
END

# Raw will be available in first plane in 16 bit unpacked mode.
HI_RES_OUTPUT
shift: 0
data_format: 444
output_width: 1
hflip: 0
vflip: 0
END
