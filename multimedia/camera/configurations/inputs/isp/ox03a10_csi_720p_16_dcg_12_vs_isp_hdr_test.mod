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
  denoise
  statistics
  white_balance_gain
  defective_pixel_auto
  crosstalk
  fringing
  colour_denoise
  demosaic
 *
ycbcr_enable:
 sharpen
 colour_correction
 gamma
 ycbcr_conversion
 false_colour
 local_tone_mapping
 chrominance_stretch
 tone_mapping
 conversion
 #hi_res_output
 lresize
 lo_res_output
*
END

HIGH_DYNAMIC_RANGE
exp_ratio: 16.0
thr_hi: 65000
thr_lo: 62500
wt_sc: 26844
swap_exp: 0
gain_r: 1.0
gain_b: 1.0
threshold: 1048575
abscissae: 0 16384 65535 524288 1048575 4194303 *
ordinates: 0 16384 40960 55296 63487 65535 *
END

DENOISE
    abscissae:  0 1500 3015 4000  4800 7360 12000 14950 26750 29450 35150 65535  *
    ordinates:  0 45   2000  3000 3500 4000 5000  5000  2000  2000  2000  2000   *

    gr_gain: 2.625
    gb_gain: 2.625
    r_gain:  2.625
    b_gain:  2.625

    r_str: 0.539
    gr_str: 0.539
    gb_str: 0.539
    b_str: 0.539

    mask: 1 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 1 *
END

AWB
    awb_method:     METHOD_GREY_WORLD

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
   fast_startup_frames: 9
   tuner_run_rate: 3
   use_sensor_awb: TRUE
END

AGC
    y_target:
        0  0.08
        30 0.08
        130 0.08
        450 0.08
        840 0.07
        4200 0.065
        65535 0.057
        *

    y_target_scale: 1.0

# do not use dynamic range func for this customer
    bottom_pixel_proportion:    655
    bottom_histogram_proportion:    6554

    hi_dynamic_range:
        0 255
        900 255
        2615 255
        65535 255
         *
    max_saturated_highlight:    0.9725

    y_coeffs:   0.299 0.587 0.114 *

    num_exp_modes:  1
    EXP_MODE 0
        num_stages:     1
        shutter_min:    100
        shutter_max:    30000 0 0 0 0 0 0 0 0 0 *
        ag_min:         1.5
        ag_max:         64.00 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *
        aperture_min:   1.000
        aperture_max:   1.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *

        max_gain_product: 128.0
    END

    agc_speed_video_up: 0.125
    agc_speed_video_down:   0.355

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
    # allow gain < 1x for startup frames
    min_gain_unity_startup_disable: TRUE

    flicker_avoidance_period: 10000

    metering_mode: HIGHLIGHT
END

IMAGE_INPUT
format: RAW16
stride: 2560
transposed: 0
END

# these are measured using macbeth chart patch 21 under d50 LE7, 300lx illumination
# very useful to remove the effect of color filter array.
WHITE_BALANCE
gain_r: 1.0
gain_b: 1.0
END

GAIN
gain: 1.0
offset_r: 0
offset_g: 0
offset_b: 0
threshold: 65535
END

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
   abscissae_r: 0 64 65535 65535 *
   ordinates_r: 0 0  65535 65535 *
   abscissae_b: 0 64 65535 65535 *
   ordinates_b: 0 0  65535 65535 *
   abscissae_gr: 0 64 65535 65535 *
   ordinates_gr: 0 0  65535 65535 *
   abscissae_gb: 0 64 65535 65535 *
   ordinates_gb: 0 0  65535 65535 *

    abscissae_r1: 0 4 4095 65535 *
    ordinates_r1: 0  0 65535 65535  *
    abscissae_gr1: 0 4 4095 65535 *
    ordinates_gr1: 0 0 65535 65535  *
    abscissae_b1: 0 4 4095 65535 *
    ordinates_b1: 0 0  65535 65535  *
    abscissae_gb1: 0 4 4095 65535 *
    ordinates_gb1: 0 0 65535 65535  *
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

SHARPEN
    x_centre: 640
    y_centre: 360
    pos_gain_absc: 0 750 1500 2000 2500 3000 65535 *
    pos_gain_ord: 1.000 1.000 0.916 0.818 0.659 0.464 0.464 *

    v_gain_absc: 0 1600 3000 5000 7000 12000 65535 *
    v_gain_ord: 0.240 0.240 0.240 0.240 0.200 0.160 0.160 *

    resp_absc: -32768 -300 -200 -100 -64 -10 10 64 100 200 300 32767 *
    resp_ord: -600 -600 -536 -382 -213 0 0 162 275 466 550 550 *

    resp1_absc: -32768 -300 -100 -64 -16 12 16 64 100 200 300 32767 *
    resp1_ord: -120 -120 -40 -20 0 0 0 20 40 80 120 120 *

    resp_gain_absc: 0 1126 1438 2124 2498 4032 4734 6768 7470 9910 10612 11450 12896 13850 14956 65535 *
    resp_gain_ord: 50 96 147 146 192 245 326 370 431 477 549 548 664 661 420 420 *

    isotropic_strength: 0.500
    avg_g_shift: 0
END

# For 5000K
COLOUR_CORRECTION
matrix:
1.4873 -0.4097 -0.0776
-0.1875 1.5227 -0.3352
-0.0393 -0.4031 1.4421
offsets: 0 0 0
END

GAMMA
abscissae_r:    0 128 256 512 1024 1536 2048 4096 8192 16384 32768 65535 *
ordinates_r:    0 13561 18586 25467 34900 41964 45013 52934 57510 61687 64031 65535 *
abscissae_g:    0 128 256 512 1024 1536 2048 4096 8192 16384 32768 65535 *
ordinates_g:    0 13561 18586 25467 34900 41964 45013 52934 57510 61687 64031 65535 *
abscissae_b:    0 128 256 512 1024 1536 2048 4096 8192 16384 32768 65535 *
ordinates_b:    0 13561 18586 25467 34900 41964 45013 52934 57510 61687 64031 65535 *
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
output_width:  640
output_height: 480
shift: 0
END

TONE_MAPPING
y_abscissae: 0  65535 65535 *
y_ordinates: 0 65535 65535 *
END

CHROMINANCE_STRETCH
u_abscissae:   -32768 -16384 16384 32767 *
u_ordinates:      0  16384 49152 65535  *
v_abscissae:   -32768 -16384 16384 32767 *
v_ordinates:      0  16384 49152 65535  *
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
   pitch:          15
   vpitch:         9
   cell_sz_power_x: 7
   cell_sz_power_y: 7

   cv_r: 4095   4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095

      *

   cv_gr: 4095  4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095

      *

   cv_gb: 4095  4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095

      *

   cv_b: 4095   4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095
4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095    4095

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

FRINGING
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
   gb_pos_lim: 3000
   gb_neg_lim: 2000
END

CROSSTALK
   abscissae_lim: 0   1024 8192 65535 *
   ordinates_lim: 200 600  900  900   *
END

COLOUR_DENOISE
    fir_bg_abscissae: 0 60 108 65535 *
    fir_bg_ordinates: 32 32 0 0 *
    fir_rg_abscissae: 0 60 108 65535 *
    fir_rg_ordinates: 32 32 0 0 *
    iir_bg_abscissae: 0 40 72 65535 *
    iir_bg_ordinates: 128 128 0 0 *
    iir_rg_abscissae: 0 40 72 65535 *
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

LOCAL_TONE_MAPPING
gauss_lpf:
    # when using gaussian LPF use these parameters
    0 0 2 8 14 8 2 0 0
    0 5 63 281 463 281 63 5 0
    2 63 764 3425 5646 3425 764 63 2
    8 281 3425 15349 25306 15349 3425 281 8
    14 463 5646 25306 41722 25306 5646 463 14
    8 281 3425 15349 25306 15349 3425 281 8
    2 63 764 3425 5646 3425 764 63 2
    0 5 63 281 463 281 63 5 0
    0 0 2 8 14 8 2 0 0

k_hp: 6.0 #unsigned 4p4 format
k_color: 1.0

gf_en: 0          # g for guided filter
epsilon: 10485    # 0p16
div_sat:  1835008 # fractional resolution of 16 bits, 1835008
lpf_abs:     0   128     256     512     1024    1536    2048    4096    8192    16384  32768  65535 *
lpf_ord:     0   1656    3312    6616    13232   19848   26464   52928   57504   61680  64024  65535  *

gain_abs: 0     128     256     512     1024    1536    2048    4096    8192    16384   32768   65535 *
gain_ord: 1656  1656    3304    6616    6616    6616    6616    4576    4176    2344    1512    1512 *
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
r_lo: 64
r_hi: 32768
g_lo: 64
g_hi: 32768
b_lo: 64
b_hi: 32768
rg_lo: -32768
rg_hi: 32767
bg_lo: -32768
bg_hi: 32767
END

GROUP 2
r_lo: 0
r_hi: 65535
g_lo: 0
g_hi: 65535
b_lo: 0
b_hi: 65535
rg_lo: -32768
rg_hi: 32767
bg_lo: -32768
bg_hi: 32767
END
