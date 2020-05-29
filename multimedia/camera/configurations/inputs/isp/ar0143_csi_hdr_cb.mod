ENABLE_MODELESS
    awb:            FALSE
    agc:            FALSE
END

ENABLE_MODE
    denoise:        FALSE
    cc:             FALSE
    defpix:         FALSE
    black_level:    FALSE
    tm:             FALSE
END

FRAME
width: 1280
height: 720
bayer_order: GRBG
bayer_enable:
    image_input
    black_level
    #lens_shading
    #denoise
    statistics
    #white_balance_gain
    #defective_pixel_auto
    #crosstalk
    #colour_denoise
    demosaic
    *
ycbcr_enable:
    #sharpen
    colour_correction
    #gamma
    ycbcr_conversion
    #false_colour
    #local_tone_mapping
    chrominance_stretch
    #conversion
    #lresize
    hi_res_output
    #lo_res_output
*
END

AGC
    hdr_metering_ldr_bin: 15
    hdr_metering_ldr_tgt_val: 6
    hdr_metering_ldr_pix_pct: 0.76
    hdr_metering_ldr_hist_scale: 1300
    hdr_metering_hdr_num_sat_pix: 50
    hdr_metering_hdr_filt_gain: 0.04
    use_digital_gain: FALSE
    y_target:
        0  0.270
        30 0.270
        130 0.246
        450 0.246
        840 0.241
        4200 0.192
        65535 0.156
         *

    y_target_scale: 1.0

# do not use dynamic range func for this customer
    bottom_pixel_proportion:    655
    bottom_histogram_proportion:    6554

    hi_dynamic_range:
    0 250
    900 250
        2615 250
        65535 250
    *
    max_saturated_highlight:    0.9990

    y_coeffs:   0.299 0.587 0.114 *

    max_gain:       100.000
    min_gain:       0.125

    num_exp_modes:  1
    EXP_MODE 0
        num_stages:     1
        shutter_min:    100
        shutter_max:    33000 0 0 0 0 0 0 0 0 0 *
        ag_min:         1.000
        ag_max:         64.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *
        aperture_min:   1.000
        aperture_max:   1.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *

        max_gain_product: 64.0
    END
    #higher values means agc filter will respond faster
    #agc_speed_video_up: 0.072
    agc_speed_video_up: 0.288
    agc_speed_video_down: 0.564

    fast_startup_frames:    9
    dropped_startup_frames: 5

    # the frame_quantisation allows the AEC to update the camera every N
    # frames. use if the camera cannot be written to after each write.
    frame_quantisation: 4 #2
    # if you enable frame_quantisation, also consider enabling aec_loop_gain
    # so that the product of aec_speed * frame_quantisation * aec_loop_gain    < 1.0
    # if unset this defalts to 1.0.
    #aec_loop_gain:      0.9
    # true for capture...
    min_gain_unity: TRUE

    # allow gain < 1x for startup frames
    min_gain_unity_startup_disable: TRUE

    flicker_avoidance_period: 10000

    metering_mode: HDR
END

profile ISP_VERSION >= 422
TM_TUNER
    use_sensor_stats: TRUE

    tol_percent: 0.05
    # 20-bit to 8-bit
    dyn_range_scale: 2.5

    ldr_bin: 46
    ldr_point: 4432

    bin_map: 1 2  3  4  5  7  8  10  12  15  17  21  24  27  32  38  47  50
55  61  71  84  103  129  166 *
END
end_profile

#--- denoise tuning was done on 25th may for ar143 with dsl219 lens
DENOISE_TUNER
    ###########
    BAYER_HW

        #pixel value -> strength
        #AG1
        CURVE 0
        abscissae:  0 5958 11915 17873 23831 29789 35746 41704 47662 53620 59577 65535 *
        ordinates:  89 199 304 404 499 589 674 754 828 898 963 1022 *
        END


        #AG4
        CURVE 1
        abscissae:  0 5958 11915 17873 23831 29789 35746 41704 47662 53620 59577 65535 *
        ordinates:  189 347 495 634 764 884 995 1097 1189 1273 1346 1411 *
        END


        #AG8
        CURVE 2
        abscissae:  0 5958 11915 17873 23831 29789 35746 41704 47662 53620 59577 65535 *
        ordinates:  272 511 731 932 1114 1278 1422 1548 1655 1743 1812 1863 *
        END


        #AG16
        CURVE 3
        abscissae:  0 5958 11915 17873 23831 29789 35746 41704 47662 53620 59577 65535 *
        ordinates:  370 715 1025 1301 1542 1748 1920 2056 2159 2226 2259 2257 *
        END

        #analogue gain -> curve to use
        interp:
            0 0.0000
            256 0.00000
            1024 1.00000
            2048 2.00000
            4096 3.00000
            65535 3.00000
             *
        #Max cannot go beyond 3.9375, unsigned 2.4 bit format
        gr_gain:           1.0

        #3.0
        gb_gain:           1.0

        #3.0
        r_gain:            1.0

        #3.0
        b_gain:            1.0

        gr_str:
            0 0.9
            256 0.8
            512 0.7
            1024 0.6
            2048 0.5
            4096 0.3
            8192 0.1
            65535 0.0
             *
        gb_str:
            0 0.9
            256 0.8
            512 0.7
            1024 0.6
            2048 0.5
            4096 0.3
            8192 0.1
            65535 0.0
             *
        r_str:
            0 0.9
            256 0.8
            512 0.7
            1024 0.6
            2048 0.5
            4096 0.3
            8192 0.1
            65535 0.0
             *
        b_str:
            0 0.9
            256 0.8
            512 0.7
            1024 0.6
            2048 0.5
            4096 0.3
            8192 0.1
            65535 0.0
             *
        mask:              1 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 1 *
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

# ccm new tuned parameters for AR143 using DSL219 lens
##### colour correction
CORRECTION_TUNER
    enable:            TRUE

    # matrix to use at 2900K temperature
    TABLE 0
        matrix:        2.052294 -0.798069 -0.254225 -0.202097 1.240782 -0.038685 0.282154 -0.977998 1.695844  *
    END

    # matrix to use at 5000K temperature
    TABLE 1
        matrix:        1.795536 -0.656527 -0.139010 -0.310661 1.453480 -0.142819 0.081785 -0.472349 1.390563  *
    END

    # matrix to use at 6500K temperature
    TABLE 2
        matrix:        1.697102 -0.545572 -0.151530 -0.298837 1.429636 -0.130799 0.022973 -0.315945 1.292971  *
    END


    interp_func:
        0 0.000
        2900 0.000
        5000 1.000
        6250 2.000
        9000 2.000
        65535 2.000
         *

    saturation_func:
        0 1.0
        10 1.0
        20 1.0
        120 1.0
        330 1.0
        2000 1.0
        4000 1.0
        8000 1.0
        65535 1.0
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

IMAGE_INPUT
format: RAW16
stride: 2560
transposed: 0
END

#Tuned AWB settings for ov10640 (2nd May 2016)
AWB
    awb_method:    METHOD_B
    #awb_method:    METHOD_GREY_WORLD

    #ct_func does not support 16 values From Macbeth chart and Light studio. Lens: Ficossa. Sns: AR0143
    ct_func:
        0.2250 9204
        0.2432 8261
        0.2614 7392
        0.2795 6599
        0.2977 5879
        0.3159 5235
        0.3341 4665
        0.3523 4171
        0.3705 3750
        0.3886 3405
        0.4068 3134
        0.4250 2939
        *

    canon_sens_r:      1
    canon_sens_g:      1
    canon_sens_b:      1
    sensor_sens_r:     1
    sensor_sens_g:     1
    sensor_sens_b:     1
    bayes_centre_weight:    1.0

    BAYES_T 0

        #From Macbeth chart and Light studio. Lens: Ficossa. Sns: AR0143
        qpath_a:       1.1084
        qpath_b:       -1.9297
        qpath_c:       0.7604

        #From Macbeth chart and Light studio. Lens: Ficossa. Sns: AR0143
        rmin:              0.225
        rmax:              0.425

        ridge_prior: 0
            0.000 0
            1.000 0
             *

        # modifying the ridge prior to solve the problem of bb2000 kelvin light source
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

        #ov5693 settings values
        std_pos:
            0.00000 0.00600
            0.13672 0.00600
            0.20703 0.00600
            0.21875 0.00800
            0.23438 0.01199
            0.26172 0.01900
            0.30078 0.02000

            #0.34375 0.03000
            #0.42578 0.02499

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

        res_perp:          0.0056
        res_para:          0.0085
    END

    bayes_t_curve_n:    1
    bayes_inter_curve_strength:    1
    bayes_inter_t_pow:    2   # default value should be 2

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
        tex_embedded_rle:    0 676 16 1 64 1 36 1 0 5 64 1 200 1 120 1 6 1 0 4 36 1 120 1 70 1 3 1 0 5 6 1 2 1 0 3393 *
    END

    bayes_tex_pdf_n:    1
    use_sensor_awb: TRUE
    tuner_run_rate: 4
END

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

#yuv player.exe compatible
Y_G_COEFFS
matrix:
#H.263 Coeff to match CTS YUV420 to RGB conversion. Mainly for BMP captures
0.257 0.508 0.098
-0.148 -0.291 0.439
0.439 -0.368 -0.071
offsets: 512 0 0
END

BLACK_LEVEL
top: 0
bottom: 0
left: 0
right: 0
threshold: 0
dark_frame_multiplier: 0
    #HDR RAW16
    abscissae_r: 0  168  1024 65535 *
    ordinates_r: 0  0  858 65535 *
    abscissae_b: 0  168  1024 65535 *
    ordinates_b: 0  0  858 65535 *
    abscissae_gr: 0  168  1024 65535 *
    ordinates_gr: 0  0  858 65535 *
    abscissae_gb: 0  168  1024 65535 *
    ordinates_gb: 0  0  858 65535 *
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

TONE_MAPPING
y_abscissae: 0 65535 65535 *
y_ordinates: 0 65535 65535 *
END

CHROMINANCE_STRETCH
u_abscissae:   -32768 -16384 16384 32767 *
u_ordinates:      0  16384 49152 65535  *
v_abscissae:   -32768 -16384 16384 32767 *
v_ordinates:      0  16384 49152 65535  *
END

GAMMA
abscissae_r:    0   128     256     512     1024    1536    2048    4096    8192    16384   32768       65535 *
ordinates_r:    0   848    1162    1592     2181    2623    2989    4096    8192    16384   32768       65535 *
abscissae_g:    0   128     256     512     1024    1536    2048    4096    8192    16384   32768       65535 *
ordinates_g:    0   848    1162    1592     2181    2623    2989    4096    8192    16384   32768       65535 *
abscissae_b:    0   128     256     512     1024    1536    2048    4096    8192    16384   32768       65535 *
ordinates_b:    0   848    1162    1592     2181    2623    2989    4096    8192    16384   32768       65535 *
END

COLOUR_CORRECTION
matrix:
1.795536 -0.656527 -0.139010
-0.310661 1.453480 -0.142819
0.081785 -0.472349 1.390563
offsets: 0 0 0
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

profile ISP_VERSION == 422
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
end_profile

CONVERSION
matrix:
##H.263 Coeff to match CTS YUV420 to RGB conversion. Mainly for BMP captures
1.1587   -0.0000    1.5996
1.1587   -0.3896   -0.8097
1.1587    2.0196    0.0025
offsets: -57161 34554 -71009
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

   cv_r: 4695 4554 4441 4357 4289 4256 4235 4220 4206 4214 4214 4206 4211 4217 4230 4292 4374 4408 4558 4783 5156 5156
4627 4487 4409 4341 4283 4249 4230 4201 4215 4207 4220 4202 4209 4209 4231 4257 4317 4375 4491 4683 4976 5308
4531 4425 4355 4299 4254 4225 4216 4213 4214 4215 4207 4206 4209 4209 4201 4250 4280 4348 4429 4601 4856 5140
4474 4379 4320 4266 4237 4204 4199 4216 4209 4200 4196 4203 4211 4200 4221 4246 4272 4325 4408 4575 4821 5095
4432 4355 4283 4237 4198 4195 4202 4196 4199 4209 4201 4203 4207 4191 4197 4229 4276 4313 4369 4537 4680 4832
4401 4327 4271 4218 4199 4185 4194 4192 4190 4185 4189 4192 4176 4179 4193 4202 4247 4283 4387 4501 4638 4785
4383 4300 4250 4191 4175 4175 4179 4170 4163 4188 4183 4172 4167 4177 4187 4190 4229 4290 4401 4501 4663 4837
4370 4277 4217 4184 4168 4159 4156 4147 4158 4156 4154 4150 4143 4156 4158 4188 4232 4296 4371 4490 4656 4835
4365 4283 4221 4193 4170 4146 4130 4140 4137 4133 4131 4122 4158 4147 4155 4180 4220 4322 4382 4513 4726 4961
4392 4304 4241 4204 4166 4145 4133 4128 4117 4121 4110 4117 4121 4136 4149 4192 4243 4327 4441 4602 4861 5151
4435 4348 4264 4214 4149 4127 4116 4110 4108 4098 4122 4117 4120 4141 4172 4209 4308 4375 4506 4715 5033 5396
4546 4393 4292 4207 4155 4133 4123 4112 4108 4097 4110 4119 4119 4151 4184 4241 4338 4412 4599 4892 5325 5843
4661 4439 4321 4201 4162 4139 4131 4114 4108 4096 4097 4122 4117 4161 4196 4273 4368 4450 4695 5082 5654 5654
      *

   cv_gr: 5046 4829 4631 4506 4419 4370 4335 4337 4316 4321 4309 4290 4303 4312 4321 4385 4500 4591 4793 5106 5580 5580
4908 4707 4561 4459 4379 4337 4323 4306 4312 4300 4295 4283 4279 4279 4299 4330 4404 4496 4667 4956 5330 5766
4763 4603 4476 4388 4336 4313 4302 4293 4293 4283 4271 4271 4267 4262 4262 4301 4336 4424 4585 4823 5179 5591
4688 4532 4410 4350 4315 4289 4278 4289 4271 4271 4260 4268 4264 4255 4264 4288 4311 4392 4541 4781 5106 5478
4621 4484 4379 4315 4273 4274 4266 4263 4257 4274 4257 4262 4253 4237 4245 4265 4316 4380 4485 4726 4955 5207
4579 4444 4348 4292 4267 4257 4254 4247 4254 4239 4235 4223 4226 4215 4221 4242 4286 4338 4488 4670 4906 5166
4545 4425 4336 4262 4234 4222 4226 4225 4215 4222 4221 4210 4208 4206 4214 4219 4266 4338 4495 4675 4919 5191
4548 4406 4300 4244 4221 4208 4207 4193 4201 4189 4182 4182 4172 4179 4185 4212 4267 4351 4467 4666 4929 5222
4547 4422 4301 4245 4211 4183 4173 4186 4168 4163 4158 4149 4170 4167 4176 4204 4248 4380 4482 4693 4994 5337
4595 4458 4329 4248 4193 4176 4171 4155 4144 4139 4139 4128 4138 4150 4168 4215 4274 4384 4545 4791 5123 5505
4673 4509 4352 4267 4192 4166 4143 4138 4125 4119 4141 4131 4131 4160 4188 4239 4349 4443 4621 4913 5328 5820
4787 4568 4399 4292 4212 4174 4145 4137 4130 4113 4119 4131 4141 4177 4218 4290 4399 4507 4740 5113 5650 6312
4908 4629 4448 4316 4233 4182 4147 4136 4134 4106 4096 4131 4151 4195 4249 4341 4449 4574 4864 5331 6012 6012
      *

   cv_gb: 5040 4806 4608 4489 4408 4365 4316 4317 4302 4302 4292 4277 4293 4294 4308 4370 4482 4564 4772 5084 5550 5550
4886 4691 4541 4439 4361 4322 4310 4289 4292 4280 4283 4274 4269 4265 4280 4311 4385 4482 4647 4932 5297 5720
4748 4582 4456 4365 4317 4290 4293 4281 4272 4267 4256 4252 4260 4239 4247 4282 4321 4412 4557 4801 5132 5512
4655 4510 4393 4320 4291 4270 4263 4265 4255 4246 4238 4248 4254 4231 4241 4272 4293 4371 4514 4748 5072 5444
4591 4468 4351 4286 4258 4251 4249 4245 4247 4256 4243 4243 4239 4220 4221 4247 4288 4360 4456 4692 4923 5178
4554 4431 4328 4268 4243 4243 4239 4230 4231 4225 4218 4214 4209 4206 4210 4223 4271 4316 4462 4648 4871 5115
4532 4404 4314 4241 4217 4202 4215 4205 4205 4210 4198 4193 4186 4184 4197 4202 4242 4313 4467 4637 4884 5159
4539 4391 4290 4228 4207 4186 4192 4184 4190 4176 4170 4165 4161 4172 4167 4199 4249 4328 4439 4632 4884 5163
4542 4406 4292 4231 4187 4173 4159 4162 4151 4144 4143 4136 4149 4153 4168 4187 4230 4352 4456 4661 4944 5264
4585 4445 4309 4232 4187 4165 4154 4137 4134 4130 4126 4119 4130 4142 4161 4198 4263 4371 4519 4759 5094 5481
4669 4504 4342 4255 4177 4154 4128 4121 4116 4106 4124 4119 4123 4143 4179 4226 4335 4429 4601 4879 5296 5792
4785 4568 4397 4283 4204 4159 4139 4132 4118 4114 4110 4121 4128 4159 4206 4266 4379 4488 4721 5077 5615 6280
4908 4635 4453 4311 4232 4164 4150 4144 4121 4121 4096 4122 4133 4175 4233 4307 4424 4548 4847 5292 5974 5974
      *

   cv_b: 5492 5174 4901 4706 4559 4475 4401 4378 4350 4333 4324 4315 4338 4342 4377 4469 4616 4756 5020 5384 5936 5936
5296 5002 4788 4618 4478 4406 4365 4332 4322 4296 4289 4280 4283 4294 4322 4388 4497 4631 4856 5209 5633 6133
5092 4873 4664 4511 4411 4351 4333 4295 4288 4275 4274 4261 4247 4257 4273 4332 4401 4530 4732 5051 5457 5934
5001 4765 4572 4450 4376 4323 4291 4274 4261 4252 4250 4248 4253 4239 4259 4303 4356 4482 4662 4971 5341 5770
4892 4706 4509 4392 4320 4298 4282 4255 4258 4259 4238 4250 4235 4215 4233 4273 4337 4449 4596 4904 5173 5474
4844 4651 4488 4367 4296 4269 4262 4247 4235 4226 4214 4204 4205 4203 4211 4235 4314 4399 4590 4846 5109 5402
4821 4610 4471 4337 4268 4237 4228 4216 4197 4211 4189 4184 4186 4182 4202 4218 4281 4396 4593 4834 5116 5433
4807 4591 4423 4316 4253 4221 4199 4182 4183 4163 4166 4158 4148 4154 4160 4214 4292 4397 4563 4824 5133 5483
4811 4611 4429 4319 4244 4201 4166 4161 4145 4142 4126 4124 4144 4141 4153 4200 4274 4443 4590 4860 5198 5587
4882 4673 4459 4338 4245 4201 4167 4133 4128 4120 4105 4109 4115 4137 4170 4224 4321 4459 4670 4967 5359 5819
4970 4734 4518 4378 4262 4192 4159 4128 4122 4096 4111 4110 4122 4158 4206 4271 4409 4542 4766 5120 5596 6170
5120 4843 4596 4438 4309 4222 4181 4157 4124 4121 4110 4133 4143 4184 4242 4350 4479 4630 4916 5346 5950 6709
5279 4957 4676 4499 4357 4252 4202 4186 4126 4147 4109 4155 4165 4210 4279 4431 4552 4723 5075 5593 6352 6352
      *
END

STATISTICS
hmask0: 65535
hmask1: 0
shift: 9 # bpp - 7 - pixel_shift (pixel_shift = bpp - 14)
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
r_lo: 0
r_hi: 16383
g_lo: 0
g_hi: 16383
b_lo: 0
b_hi: 16383
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
