PIPELINE
   isp_bits_per_pixel: 16
END

ENABLE_MODELESS
    agc:            TRUE
    awb:            TRUE
END

ENABLE_MODE
    denoise:        TRUE
    cc:             TRUE
    defpix:         TRUE
END

FRAME
width: 1280
height: 720
bayer_order: GRBG
bayer_enable:
  image_input
  black_level
  lens_shading
  denoise
  statistics
  white_balance_gain
  defective_pixel_auto
  #crosstalk
  fringing
  colour_denoise
  demosaic
 *
ycbcr_enable:
 #sharpen
 colour_correction
 gamma
 ycbcr_conversion
 false_colour
 chrominance_stretch
 #conversion
 #lresize
 hi_res_output
 #lo_res_output
*
END

AWB
    #awb_method:     METHOD_GREY_WORLD
    awb_method:    METHOD_B

    ct_func:
        47       11000
        48       10000
        49        9000
        51        8000
        54        7000
        58        6000
        64        5000
        74        4000
        87        3200
        97        2800
        109        2400
        120        2100
        129        1900
        140        1700
        *
    use_local_ccm: FALSE

    canon_sens_r:   1
    canon_sens_g:   1
    canon_sens_b:   1
    sensor_sens_r:  1
    sensor_sens_g:  1
    sensor_sens_b:  1
    bayes_centre_weight:    256
    BAYES_T 0
        qpath_a:       1.3380
        qpath_b:       -1.7370
        qpath_c:        0.6511
        rmin:           0.1851
        rmax:           0.5368
        ridge_prior: 0
            0.000 0
            1.000 0
             *
       ridge_prior: 1
            0.000 500
            0.185 500
            0.230 500
            0.246 500
            0.270 500
            0.328 500
            0.406 500
            0.518 500
            0.528 500
            0.537 500
            0.550 500
            1.000 500
             *
        ridge_prior_interp:
            0 0
            60 120
            310 256
            65535 256
             *
        std_pos:
            0.00000 0.00600
            0.13672 0.00600
            0.20703 0.00600
            0.21875 0.00800
            0.23438 0.01199
            0.26172 0.01900
            0.30078 0.02000
            0.34375 0.04000
            0.42578 0.04000

            0.44922 0.00999
            0.49219 0.00101
            1.00000 0.00101
             *
        std_neg:
            0.00000 0.00600
            0.13672 0.00600
            0.23047 0.00600
            0.24219 0.01599
            0.25391 0.01900
            0.31250 0.02499
            0.35156 0.02499
            0.39844 0.02499
            0.42188 0.02000
            0.49219 0.00800
            1.00000 0.00800
             *
        res_perp:       0.0056
        res_para:       0.0085
    END
    bayes_t_curve_n:    1
    bayes_inter_curve_strength: 1
    bayes_inter_t_pow:  2
    BAYES_S_T_PDF 0
        w_int:
            -1.000 64
            -0.500 255
            0.750 255
            1.000 64
             *
        w_r_nrm:
            0.000 255
            0.289 255
            0.316 255
            1.000 255
             *
        w_exp:
            0 255
            65535 255
             *
        #pdf_neutral_N.rle.txt
        tex_embedded_rle:   0 676 16 1 64 1 36 1 0 5 64 1 200 1 120 1 6 1 0 4 36 1 120 1 70 1 3 1 0 5 6 1 2 1 0 3393 *
    END
    BAYES_S_T_PDF 1
        w_int:
            -256 0
            -10  0
            0    255
            256  255
            *
        w_r_nrm:
            0     255
            0.288 255
            0.312 0
            0.427 0
            1.0   0
            *
        w_exp:
            0     32
            26    32
            26    0
            65535 0
         *
        tex_embedded_rle: 0 846 2 1 24 1 0 5 6 1 75 1 168 1 0 4 5 1 82 1 212 1 251 1 0 3 4 1 54 1 203 1 252 1 245 1
                           0 2 1 1 66 1 190 1 248 1 247 1 159 1 0 2 4 1 126 1 248 1 251 1 193 1 44 1 0 2 2 1 87 1 212 1
                           187 1 63 1 3 2 7 1 2 1 0 5 100 1 138 1 65 1 1 1 0 4 237 1 229 1 77 1 1 1 0 4 248 1 161 1 21 1
                           0 5 178 1 46 1 1 1 0 5 37 1 1 1 0 153 10 1 48 1 33 1 3 1 0 3001 *
    END
    BAYES_S_T_PDF 2
        w_int:
            -256 64
            -128 255
            128  255
            256  32
            *
        w_r_nrm:
            0.0   150
            0.239 150
            0.280 150
            0.312 64
            1.0   64
            *
        w_exp:
            0     24
            50    24
            65    8
            157   4
            65535 4
            *
        tex_embedded_rle: 0 175 2 1 0 6 4 1 67 1 0 5 3 1 59 1 201 1 0 26 2 1 0 5 1 1 37 1 101 1 45 1
                           1 1 0 3 37 1 170 1 240 1 176 1 36 1 0 3 176 1 245 1 254 1 245 1 139 1 10 1 0 2 251 1
                           254 1 255 1 253 1 190 1 23 1 0 134 6 1 77 1 195 1 250 1 0 4 47 1 206 1 252 1 254 1 0 3
                           2 1 103 1 244 1 254 1 255 1 0 3 3 1 125 1 250 1 255 2 0 3 3 1 127 1 250 1 255 2 0 3
                           3 1 126 1 250 1 255 2 0 3 3 1 119 1 248 1 255 2 0 3 1 1 98 1 243 1 254 3 255 2 251 1
                           160 1 13 1 0 2 255 2 254 1 239 1 90 1 2 1 0 2 255 2 254 1 220 1 49 1 0 3 255 2 253 1
                           190 1 23 1 0 3 255 2 252 1 161 1 11 1 0 3 255 2 248 1 121 1 4 1 0 3 255 1 254 1 240 1
                           87 1 1 1 0 3 254 1 251 1 201 1 45 1 0 136 54 1 217 1 253 1 248 1 0 4 10 1 109 1 170 1
                           136 1 0 5 6 1 13 1 7 1 0 40 227 1 167 1 68 1 4 1 0 4 66 1 18 1 2 1 0 3381 *
   END
   bayes_tex_pdf_n:    3
   fast_startup_frames:    9
   use_sensor_awb: FALSE
END

AGC

    y_target:
        0  0.04
        30 0.04
        130 0.03
        450 0.03
        840 0.02
        4200 0.02
        65535 0.01
        *

    y_target_scale: 1.0

# do not use dynamic range func for this customer
    bottom_pixel_proportion:    655
    bottom_histogram_proportion:    6554

    hi_dynamic_range:
        0 255
        900 255
        2615 250
        65535 200
         *
    max_saturated_highlight:    0.998

    y_coeffs:   0.299 0.587 0.114 *

    max_gain:       100.000
    min_gain:       0.125

    num_exp_modes:  1
    EXP_MODE 0
        num_stages:     1
        shutter_min:    100
        shutter_max:    33333 0 0 0 0 0 0 0 0 0 *
        ag_min:         1.350
        ag_max:         128.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *
        aperture_min:   1.000
        aperture_max:   1.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *

        max_gain_product: 128.0
    END

    agc_speed_video_up: 0.072
    agc_speed_video_down:   0.564

    fast_startup_frames:    9
    dropped_startup_frames: 5

    # the frame_quantisation allows the AEC to update the camera every N
    # frames. use if the camera cannot be written to after each write.
    frame_quantisation: 2 #2
    # if you enable frame_quantisation, also consider enabling aec_loop_gain
    # so that the product of aec_speed * frame_quantisation * aec_loop_gain    < 1.0
    # if unset this defalts to 1.0.
    #aec_loop_gain:      0.9
    # true for capture...
    min_gain_unity: TRUE

    # allow gain < 1x for startup frames
    min_gain_unity_startup_disable: TRUE

    flicker_avoidance_period: 10000

    metering_mode: HIGHLIGHT
END

IMAGE_INPUT
format: RAW12
stride: 1920
transposed: 0
END


# these are measured using macbeth chart patch 21 under d50 LE7, 300lx illuminnation
# very useful to remove the effect of color filter array.
WHITE_BALANCE
gain_r: 1.8196
gain_b: 1.5857
END

GAIN
gain: 1.0
offset_r: 0
offset_g: 0
offset_b: 0
threshold: 65535
END


#DENOISE
#   abscissae:   0 331 875 1780 3323 5187 7587 8192 65535 *
#   ordinates:   25 25 39 52 73 98 141 141 141 *
#
#   gr_gain: 2.625
#   gb_gain: 2.625
#   r_gain: 2.625
#   b_gain: 2.625
#
#   r_str: 0.539
#   gr_str: 0.539
#   gb_str: 0.539
#   b_str: 0.539
#END

DENOISE_TUNER
    BAYER_HW
        #pixel value -> strength
        #AG1
        CURVE 0
            abscissae:  0   200 263 351 582 1168    2653    5744    12672   34465   55205   65535 *
            ordinates:  0   4   8   16  32  76  182 378 724 1238    1356    1356  *
        END


        #AG2
        CURVE 1
            abscissae:  0   207 266 357 588 1188    2652    5824    13005   35185   56079   65535 *
            ordinates:  0   6   10  18  34  78  178 380 750 1330    1520    1520  *
        END


        #AG4
        CURVE 2
            abscissae:  0   205 260 357 597 1209    2747    6043    13596   36195   57059   65535 *
            ordinates:  0   18  24  32  54  106 236 490 376 1770    1898    1898  *
        END


        #AG8
        CURVE 3
            abscissae:  0   198 253 341 566 1152    2657    6043    13185   35699   56025   65535 *
            ordinates:  0   104 128 168 272  544    1208    2592    5032    9496    11224   11224 *
        END

        #analogue gain -> curve to use
        interp:
            0 0.0000
            256 0.00000
            512 1.00000
            1024 2.00000
            2048 3.00000
            65535 3.00000
             *
        #Max cannot go beyond 3.9375, unsigned 2.4 bit format
        gr_gain:        3.9375

        #3.0
        gb_gain:        3.9375

        #3.0
        r_gain:         3.9375

        #3.0
        b_gain:         3.9375

        gr_str:
            0 0.539
            256 0.539
            512 0.480
            1024 0.398
            2048 0.340
            2500 0.201
            65535 0.201
             *
        gb_str:
            0 0.539
            256 0.539
            512 0.480
            1024 0.398
            2048 0.340
            2500 0.201
            65535 0.201
             *
        r_str:
            0 0.539
            256 0.539
            512 0.480
            1024 0.398
            2048 0.340
            2500 0.201
            65535 0.201
             *
        b_str:
            0 0.539
            256 0.539
            512 0.480
            1024 0.398
            2048 0.340
            2500 0.201
            65535 0.201
             *
        mask:           1 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 1 *
    END
    COLOUR_HW
            red_strength:
                1.0  0.3
                16.0 0.8
                *
            blue_strength:
                1.0  0.15
                16.0 0.8
                *
        END


END

CORRECTION_TUNER

    TABLE 0
        matrix:          1.071 0.094 -0.166 -0.294 1.567 -0.273 -0.058 -1.358 2.415 *
    END

    TABLE 1
        matrix:          1.4561 -0.3523 -0.1035 -0.2104 1.6265 -0.4160 -0.0317 -0.7229 1.7546 *
    END

    TABLE 2
        matrix:          1.4873 -0.4097 -0.0776 -0.1875 1.5227 -0.3352 -0.0393 -0.4031 1.4421 *
    END

    interp_func:
        0 0
        2900 0
        5000 256
        6000 512
        65535 512
    *

    # Step 3:
    #   Finally, The saturation of the colour matrix is adjusted
    #   The PWL below maps EEWDG to saturation
    #   N.B. EEDWG = Effective Exposure With Digital Gain, a measure of scene brightness
    saturation_func:
        0 1.0
        10 1.0
        20 1.0
        120 1.0
        330 1.0
        2000 1.0
        4000 0.906250
        8000 0.839844
        65535 0.839844
         *
END

DEFECTIVE_PIXEL_TUNER
    scale_hi_0:
        0 0.1
        65535 0.1
         *
    scale_hi_1:
        0 0
        65535 0
         *
    scale_hi_2:
        0 0
        65535 0
         *
    scale_lo_0:
        0 0.1
        65535 0.1
         *
    scale_lo_1:
        0 0
        65535 0
         *
    scale_lo_2:
        0 0
        65535 0
         *
    offset_hi_0:
        0 256
        300 92
        600 0
        1200 -1024
        1700 -16384
        65535 -16384
         *
    offset_hi_1:
        0 16384
        65535 16384
         *
    offset_hi_2:
        0 16384
        65535 16384
         *
    offset_lo_0:
        0 -64
        700 -256
        1774 -16384
        65535 -16384
         *
    offset_lo_1:
        0 1024
        700 0
        1774 -16384
        65535 -16384
         *
    offset_lo_2:
        0 16384
        65535 16384
         *
    replace_index_lo:
        0 3
        65535 3
         *
    replace_index_hi:
        0 5
        65535 5
         *
    replace_scale_lo:
        0 0
        65535 0
         *
    replace_scale_hi:
        0 0
        65535 0
         *
END


DEFECTIVE_PIXEL
# 50
scale_hi: 1.0 1.0 1.0
offset_hi: 5000 5000 5000
scale_lo: 1.0 1.0 1.0
offset_lo: 5000 5000 5000

#weak
replace_index_lo: 1
replace_index_hi: 9
replace_scale_hi:  1.0
replace_scale_lo:   1.0
END

# FOR BT.601
Y_G_COEFFS
matrix:
0.2566 0.5064 0.0978
-0.1482 -0.2914 0.4396
0.4389 -0.3677 -0.0713
offsets: 0 0 0
END

BLACK_LEVEL
top: 0
bottom: 0
left: 0
right: 0
threshold: 0
dark_frame_multiplier: 0

   abscissae_r:  0 512  1408  2176  2560  4095  65535  *
   ordinates_r:  0 1920 16256 65407 65535 65535 65535 *
   abscissae_gr: 0 512  1408  2176  2560  4095  65535  *
   ordinates_gr: 0 1920 16256 65407 65535 65535 65535 *
   abscissae_gb: 0 512  1408  2176  2560  4095  65535  *
   ordinates_gb: 0 1920 16256 65407 65535 65535 65535 *
   abscissae_b:  0 512  1408  2176  2560  4095  65535  *
   ordinates_b:  0 1920 16256 65407 65535 65535 65535 *
END

COLOUR_DENOISE
    fir_bg_abscissae: 0 120 216 65535 *
    fir_bg_ordinates: 32 32 0 0 *
    fir_rg_abscissae: 0 120 216 65535 *
    fir_rg_ordinates: 32 32 0 0 *
    iir_bg_abscissae: 0 80 144 65535 *
    iir_bg_ordinates: 128 128 0 0 *
    iir_rg_abscissae: 0 80 144 65535 *
    iir_rg_ordinates: 128 128 0 0 *
    fir_shift: 1
    weights: 6 5 5
    dist_abscissae: 0 1512 2016 3528 65535 *
    dist_ordinates: 127 127 95 15 15 *
    g_abscissae: 0 300 65535 *
    g_ordinates: 48 16 16 *
    sat_abscissae: 0 200 400 65535 *
    sat_ordinates: 32 32 64 64 *
    iir_attenuation_lines: 6
    iir_attenuation_shift: 2
    edge_threshold: 5
    edge_strength: 4.0
    edge_min: 0.15
END

FRINGING
#r_pos:                 0.0
#r_neg:                 0.15
#gr_pos:                0
#gr_neg:                0
#b_pos:                 0
#b_neg:                 0.15
#gb_pos:                0
#gb_neg:                0
#r_pos_lim:             1024
#r_neg_lim:             2048
#gr_pos_lim:            1024
#gr_neg_lim:            1024
#b_pos_lim:             1024
#b_neg_lim:             2048
#gb_pos_lim:            1024
#gb_neg_lim:            1024
#   r_pos: 0.10
#   r_neg: 0.08
#   b_pos: 0.06
#   b_neg: 0.04
#   gr_pos: 0.05
#   gr_neg: 0.05
#   gb_pos: 0.05
#   gb_neg: 0.05
#
#   r_pos_lim: 2000
#   r_neg_lim: 2000
#   gr_pos_lim: 1500
#   gr_neg_lim: 1000
#   b_pos_lim: 2000
#   b_neg_lim: 2000
#   gb_pos_lim: 1000
#   gb_neg_lim: 1000

   r_pos: 0.20
   r_neg: 0.16
   b_pos: 0.12
   b_neg: 0.08
   gr_pos: -0.02
   gr_neg: 0.00
   gb_pos: -0.02
   gb_neg: 0.00

   r_pos_lim: 4000
   r_neg_lim: 4000
   gr_pos_lim: 3000
   gr_neg_lim: 2000
   b_pos_lim: 4000
   b_neg_lim: 4000
   gb_pos_lim: 2000
   gb_neg_lim: 2000
END

DEMOSAIC
bayer_type: TRUE
threshold_n:  20    # uint16
g_offset_shift: 0   # uint8
hv_bias: 0          # 8.8
show_dir: 0         # uint8
profile ISP_VERSION == 425
new_direction_filter: 1 # uint8
reduce_fc: 0        # uint8
threshold_c: 10     # uint16
shift: 0            # uint8
end_profile
profile ISP_VERSION == 427
new_direction_filter: 1
reduce_fc: 0
threshold_c: 20
shift: 0        # uint8
end_profile
END

#SHARPEN
#   x_centre: 640
#   y_centre: 360
#   pos_gain_absc: 0 750 1500 2000 2500 3000 65535 *
#   pos_gain_ord: 1.000 1.000 0.916 0.818 0.659 0.464 0.464 *
#
#   v_gain_absc: 0 1600 3000 5000 7000 12000 65535 *
#   v_gain_ord: 0.240 0.240 0.240 0.240 0.200 0.160 0.160 *
#
#   resp_absc: -32768 -300 -200 -100 -64 -10 10 64 100 200 300 32767 *
#   resp_ord: -600 -600 -536 -382 -213 0 0 162 275 466 550 550 *
#
#   resp1_absc: -32768 -300 -100 -64 -16 12 16 64 100 200 300 32767 *
#   resp1_ord: -120 -120 -40 -20 0 0 0 20 40 80 120 120 *
#
#   resp_gain_absc: 0 1126 1438 2124 2498 4032 4734 6768 7470 9910 10612 11450 12896 13850 14956 65535 *
#   resp_gain_ord: 50 96 147 146 192 245 326 370 431 477 549 548 664 661 420 420 *
#
#   isotropic_strength: 0.500
#   avg_g_shift: 0
#END

# For 5000K
COLOUR_CORRECTION
matrix:
1.4873 -0.4097 -0.0776
-0.1875 1.5227 -0.3352
-0.0393 -0.4031 1.4421
offsets: 0 0 0
END


GAMMA
abscissae_r:    0   128     256     512     1024    1536    2048    4096    8192    16384   32768       65535 *
ordinates_r:    0   1695    2323    3183    4362    5245    5626    6616    7188    7710    8003        8192  *
abscissae_g:    0   128     256     512     1024    1536    2048    4096    8192    16384   32768       65535 *
ordinates_g:    0   1695    2323    3183    4362    5245    5626    6616    7188    7710    8003        8192  *
abscissae_b:    0   128     256     512     1024    1536    2048    4096    8192    16384   32768       65535 *
ordinates_b:    0   1695    2323    3183    4362    5245    5626    6616    7188    7710    8003        8192  *
END

FALSE_COLOUR
strength1:         0.0
strength2:         1.0
filter_mix_cw:     1.0
filter_mix_lo:    2000
filter_mix_hi:    3000
filter_desat_cw:   8.0
filter_desat_lo: 65535
filter_desat_hi: 65535
END

LRESIZE
output_width:  1280
output_height: 720
shift: 0
END

TONE_MAPPING
y_abscissae: 0 8191 65535 *
y_ordinates: 0 65535 65535 *
END

CHROMINANCE_STRETCH
u_abscissae:   -4096  -2048 2048  4095  *
u_ordinates:      0   16384 49152 65535  *
v_abscissae:   -4096  -2048 2048  4095  *
v_ordinates:      0   16384 49152 65535  *
END


CONVERSION
matrix:
#-- For 16 bit HDR path kindly use these
1.1619  0.0009 1.5994
1.1616 -0.3899 -0.8108
1.1617  2.0166 0.0018
offsets: -52441 39346 -66138
END

HI_RES_OUTPUT
shift: 8
data_format: 420            # RGB or yuv444
output_width: 0             # 1 means 16 bits, 0 means based on the shift parameter
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


LENS_SHADING
   pitch:          22
   vpitch:         13
   cell_sz_power_x: 6
   cell_sz_power_y: 6


   cv_r: 6808 6808 6662 5748 5194 4867 4656 4508 4450 4466 4474 4502 4616 4778 5000 5319 5820 6636 6758 6758 6758 6493
6748 6744 6070 5333 4912 4649 4462 4356 4296 4306 4341 4378 4440 4572 4775 5048 5454 6110 6793 6803 6803 6803
6758 6560 5545 5003 4670 4439 4299 4196 4167 4176 4221 4279 4359 4427 4584 4812 5153 5678 6554 6791 6791 6791
6751 6141 5273 4794 4506 4311 4208 4131 4113 4125 4162 4201 4290 4370 4491 4665 4969 5412 6148 6795 6795 6795
6744 5873 5111 4678 4412 4242 4147 4102 4116 4169 4184 4206 4237 4336 4456 4605 4864 5243 5896 6707 6704 6702
6700 5742 5031 4613 4352 4204 4121 4103 4096 4103 4142 4182 4233 4329 4435 4565 4804 5171 5768 6688 6734 6781
6662 5717 5022 4608 4349 4216 4157 4154 4189 4151 4179 4207 4241 4312 4416 4565 4789 5156 5718 6653 6728 6804
6719 5829 5095 4667 4384 4239 4204 4193 4174 4192 4207 4263 4292 4345 4452 4578 4836 5200 5745 6694 6754 6814
6750 6095 5270 4794 4486 4334 4278 4261 4253 4251 4264 4284 4308 4378 4478 4629 4910 5265 5903 6733 6727 6720
6749 6481 5562 4998 4649 4451 4375 4370 4365 4365 4300 4320 4379 4442 4560 4726 4998 5426 6229 6748 6748 6748
6764 6753 5972 5296 4888 4610 4509 4506 4448 4417 4432 4446 4489 4555 4682 4884 5203 5758 6677 6774 6774 6774
6768 6768 6552 5699 5196 4866 4662 4593 4597 4592 4557 4544 4591 4704 4870 5132 5563 6307 6714 6714 6714 6714
6772 6784 7256 6170 5544 5152 4825 4684 4755 4782 4689 4646 4698 4863 5074 5407 5978 6971 6751 6654 6654 5223
      *

   cv_gr: 8307 8121 5883 5083 4690 4465 4345 4261 4254 4288 4299 4300 4365 4465 4618 4843 5239 5973 7791 8708 8708 8033
8044 6741 5292 4766 4490 4339 4251 4194 4176 4198 4236 4252 4278 4356 4474 4651 4950 5468 6583 8795 8828 8861
7640 5774 4905 4534 4344 4225 4159 4117 4115 4135 4179 4221 4269 4282 4368 4497 4729 5122 5875 7956 8787 9813
6480 5338 4706 4399 4257 4160 4128 4096 4102 4129 4159 4187 4251 4280 4327 4410 4603 4921 5496 6916 8789 12053
6015 5125 4589 4327 4207 4131 4108 4102 4130 4178 4193 4209 4223 4275 4329 4387 4536 4799 5296 6390 8779 14019
5786 5027 4547 4297 4171 4120 4102 4114 4121 4136 4172 4203 4230 4279 4323 4373 4503 4742 5195 6158 8575 14114
5771 5011 4546 4301 4176 4140 4134 4166 4207 4177 4207 4231 4233 4276 4322 4379 4501 4733 5144 6076 8720 15436
5903 5090 4599 4331 4191 4154 4177 4190 4197 4215 4223 4272 4269 4291 4341 4379 4525 4762 5156 6176 8655 14456
6238 5281 4703 4410 4255 4212 4221 4236 4252 4261 4272 4277 4274 4298 4329 4397 4564 4795 5272 6539 8902 13940
6901 5604 4905 4540 4342 4268 4270 4302 4333 4346 4287 4285 4311 4326 4369 4438 4600 4893 5529 7292 8917 11474
8079 6197 5204 4729 4483 4337 4332 4372 4344 4340 4364 4362 4364 4375 4421 4523 4726 5135 6042 8464 8898 9379
8142 7304 5671 5011 4680 4482 4384 4378 4407 4425 4414 4383 4392 4428 4516 4680 4992 5559 7027 9017 9005 8993
8207 8893 6229 5327 4895 4637 4437 4383 4472 4514 4464 4404 4421 4483 4615 4848 5288 6060 8395 9647 9114 15137
      *

   cv_gb: 8306 8167 5924 5093 4693 4472 4341 4264 4250 4287 4293 4297 4362 4464 4613 4840 5224 5962 7786 8824 8824 9010
8090 6811 5313 4778 4499 4344 4252 4198 4177 4198 4241 4256 4276 4352 4467 4651 4942 5456 6561 8750 8776 8802
7688 5800 4921 4542 4350 4228 4161 4118 4116 4139 4182 4224 4268 4284 4366 4494 4726 5114 5856 7928 8810 9913
6527 5358 4713 4404 4261 4160 4132 4096 4103 4128 4159 4189 4247 4279 4328 4410 4597 4918 5481 6878 8811 12257
6048 5140 4595 4332 4208 4133 4108 4097 4129 4179 4193 4207 4217 4276 4325 4386 4534 4795 5288 6354 8737 13980
5815 5044 4550 4303 4173 4127 4106 4116 4120 4136 4169 4205 4233 4279 4326 4373 4502 4736 5183 6131 8604 14424
5780 5021 4543 4301 4176 4135 4132 4160 4200 4172 4205 4225 4228 4270 4315 4373 4485 4724 5123 6040 8554 14656
5931 5104 4598 4337 4196 4153 4174 4195 4196 4214 4221 4271 4276 4290 4341 4381 4520 4760 5143 6140 8592 14305
6263 5294 4705 4416 4257 4211 4223 4237 4254 4265 4273 4277 4271 4302 4330 4396 4560 4789 5256 6494 8912 14202
6942 5617 4910 4541 4344 4271 4271 4296 4333 4344 4282 4287 4313 4322 4366 4435 4594 4883 5500 7218 8862 11474
8094 6220 5210 4737 4492 4346 4334 4381 4347 4344 4367 4364 4370 4375 4424 4523 4719 5121 5996 8409 8883 9415
8158 7326 5674 5012 4678 4483 4382 4377 4413 4426 4412 4383 4389 4432 4507 4674 4979 5531 6953 9006 8994 8983
8223 8910 6229 5321 4879 4628 4431 4373 4481 4512 4457 4402 4408 4490 4593 4835 5269 6012 8273 9694 9108 17194
      *

   cv_b: 6519 6520 5699 4981 4633 4446 4340 4274 4272 4308 4311 4304 4358 4441 4575 4768 5142 5850 6611 6611 6611 8656
6515 6269 5172 4700 4462 4336 4261 4209 4189 4209 4246 4266 4283 4344 4450 4601 4866 5366 6330 6553 6553 6553
6515 5607 4824 4501 4343 4238 4177 4125 4116 4140 4181 4226 4267 4281 4363 4476 4679 5037 5765 6480 6478 6476
6258 5207 4647 4386 4271 4175 4139 4096 4097 4118 4145 4178 4247 4281 4329 4402 4566 4852 5398 6446 6542 6640
5807 5006 4544 4332 4224 4152 4114 4100 4124 4166 4180 4195 4203 4262 4327 4383 4506 4740 5205 6197 6456 6737
5611 4919 4511 4300 4198 4140 4108 4113 4119 4124 4159 4181 4216 4266 4321 4372 4483 4694 5109 6041 6475 6975
5583 4899 4499 4300 4192 4142 4132 4154 4195 4167 4190 4208 4210 4259 4311 4371 4469 4684 5054 5965 6461 7047
5709 4979 4556 4330 4208 4165 4181 4184 4186 4207 4213 4258 4266 4285 4341 4384 4502 4718 5076 6049 6453 6915
6006 5136 4643 4400 4274 4226 4229 4232 4245 4252 4262 4264 4257 4293 4336 4398 4547 4744 5185 6278 6501 6740
6455 5419 4812 4517 4350 4283 4282 4295 4331 4330 4269 4271 4308 4318 4373 4434 4573 4830 5429 6515 6548 6582
6541 5956 5067 4679 4473 4353 4357 4383 4348 4346 4371 4371 4378 4379 4425 4507 4684 5059 5923 6494 6494 6494
6523 6461 5493 4913 4634 4467 4396 4394 4419 4440 4431 4405 4403 4435 4497 4638 4914 5434 6438 6512 6512 6512
6505 7061 5996 5171 4808 4588 4435 4404 4492 4538 4493 4439 4429 4492 4572 4776 5168 5869 7050 6529 6529 7009
      *
END

STATISTICS
hmask0: 255
hmask1: 65280
shift: 9 # bpp - 7
pixel_shift: 2
END


GROUP_0
x_offset: 0
y_offset: 0
width: 80
height: 90
h_num: 16
v_num: 8
END

REGION 3904
x_offset: 0
y_offset: 0
width: 320
height: 180
END

REGION 3905
x_offset: 320
y_offset: 0
width: 320
height: 180
END

REGION 3906
x_offset: 640
y_offset: 0
width: 320
height: 180
END

REGION 3907
x_offset: 960
y_offset: 0
width: 320
height: 180
END

REGION 3908
x_offset: 0
y_offset: 180
width: 320
height: 180
END

REGION 3909
x_offset: 320
y_offset: 180
width: 320
height: 180
END

REGION 3910
x_offset: 640
y_offset: 180
width: 320
height: 180
END

REGION 3911
x_offset: 960
y_offset: 180
width: 320
height: 180
END

REGION 3912
x_offset: 0
y_offset: 360
width: 320
height: 180
END

REGION 3913
x_offset: 320
y_offset: 360
width: 320
height: 180
END

REGION 3914
x_offset: 640
y_offset: 360
width: 320
height: 180
END

REGION 3915
x_offset: 960
y_offset: 360
width: 320
height: 180
END

REGION 3916
x_offset: 0
y_offset: 540
width: 320
height: 180
END

REGION 3917
x_offset: 320
y_offset: 540
width: 320
height: 180
END

REGION 3918
x_offset: 640
y_offset: 540
width: 320
height: 180
END

REGION 3919
x_offset: 960
y_offset: 540
width: 320
height: 180
END


GROUP 0
r_lo: 30
r_hi: 15800
g_lo: 30
g_hi: 15800
b_lo: 30
b_hi: 15800
rg_lo: -32768
rg_hi: 32767
bg_lo: -32768
bg_hi: 32767
END

GROUP 2
r_lo: 30
r_hi: 65535
g_lo: 30
g_hi: 65535
b_lo: 30
b_hi: 65535
rg_lo: -32768
rg_hi: 32767
bg_lo: -32768
bg_hi: 32767
END
