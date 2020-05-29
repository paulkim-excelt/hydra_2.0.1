PIPELINE
   isp_bits_per_pixel: 16
END

FRAME
width: 1280
height: 720
bayer_order: BGGR
bayer_enable:
  image_input
  black_level
  demosaic
 *
ycbcr_enable:
 gamma
 ycbcr_conversion
 false_colour
 chrominance_stretch
 #conversion
 hi_res_output
 #lo_res_output
*
END

IMAGE_INPUT
format: RAW12
stride: 1920
transposed: 0
END

BLACK_LEVEL
      threshold:      0
      abscissae_r:     0 4095 65535 *
      ordinates_r:     0 16383 16383 *
      abscissae_b:     0 4095 65535 *
      ordinates_b:     0 16383 16383 *
      abscissae_gr:    0 4095 65535 *
      ordinates_gr:    0 16383 16383 *
      abscissae_gb:    0 4095 65535 *
      ordinates_gb:    0 16383 16383 *
END

GAIN                    # global gain applied to all colours and offsets
   gain: 1.0
   offset_r: 0
   offset_g: 0
   offset_b: 0
   threshold: 16383
END


WHITE_BALANCE
   gain_r:         1.0
   gain_b:         1.0
END

DEMOSAIC
   threshold_n:    20
   g_offset_shift: 0    # uint8
   hv_bias: 0           # 8.8
END


GAMMA
abscissae_r: 0  104  128  256  512 1024 2048 4096  8192 12287 16383 65535 *
abscissae_g: 0  104  128  256  512 1024 2048 4096  8192 12287 16383 65535 *
abscissae_b: 0  104  128  256  512 1024 2048 4096  8192 12287 16383 65535 *

ordinates_r: 0 1643 1805 2474 3390 4646 6367 8725 11956 14375 16383 16383 *   # gamma: 2.2
ordinates_g: 0 1643 1805 2474 3390 4646 6367 8725 11956 14375 16383 16383 *
ordinates_b: 0 1643 1805 2474 3390 4646 6367 8725 11956 14375 16383 16383 *

#ordinates_r: 0 1198  1387 2154  3177  4543  6366   8799 12048  14430 16383 16383 *
#ordinates_g: 0 1198  1387 2154  3177  4543  6366   8799 12048  14430 16383 16383 *
#ordinates_b: 0 1198  1387 2154  3177  4543  6366   8799 12048  14430 16383 16383 *

END

Y_G_COEFFS
matrix:
0.2566 0.5064 0.0978
-0.1482 -0.2914 0.4396
0.4389 -0.3677 -0.0713
offsets: 0 0 0
END

FALSE_COLOUR
   strength2:      1.0
   filter_mix_cw:  1.0
   filter_mix_lo:  2000
   filter_mix_hi:  3000
   filter_desat_cw: 8.0
   filter_desat_lo: 65535
   filter_desat_hi: 65535
END


TONE_MAPPING
y_abscissae:  0  4096  8192 16383 65535 *
y_ordinates:  0  20480 36864 65535 65535 *
END


CHROMINANCE_STRETCH
u_abscissae:   -8192 -4096 4096 8191 16383 *
u_ordinates:      0  16384 49152 65532 65532 *
v_abscissae:   -8192 -4096 4096 8191 16383 *
v_ordinates:      0  16384 49152 65532 65532 *
END


CONVERSION
matrix:
1.1641 0 1.5977
1.1641 -0.3906 -0.8125
1.1641 2.0156 0.0000
offsets: -57120 34656 -70816
END

LRESIZE
output_width:  1280
output_height: 720
shift: 0
END

HI_RES_OUTPUT
shift: 8
data_format: 420
output_width: 0
hflip: 0
vflip: 0
END

LO_RES_OUTPUT
shift: 8
data_format: 420
output_width: 0
hflip: 0
vflip: 0
END
