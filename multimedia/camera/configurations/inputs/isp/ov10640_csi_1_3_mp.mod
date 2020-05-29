PIPELINE
   isp_bits_per_pixel: 14
END

ENABLE_MODELESS
    agc:            FALSE
    awb:            TRUE
END

ENABLE_MODE
    denoise:        FALSE
    cc:             FALSE
END

FRAME
width: 1280
height: 1024
bayer_order: GRBG
bayer_enable:
  image_input
  black_level
# lens_shading
  denoise
  statistics
  white_balance_gain
# defective_pixel_auto
# crosstalk
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
 #hi_res_output
 lo_res_output
*
END


AWB
    awb_method:     METHOD_GREY_WORLD
    #awb_method:    METHOD_B

    canon_sens_r:   1
    canon_sens_g:   1
    canon_sens_b:   1
    sensor_sens_r:  1
    sensor_sens_g:  1
    sensor_sens_b:  1
    bayes_centre_weight:    1.5
    BAYES_T 0
        qpath_b:        -1.183268
        qpath_c:        0.6373097
        rmin:           0.216
        rmax:           0.352
        ridge_prior: 0
            0.000 0
            0.258 0
            0.262 500
            0.324 500
            0.340 0
            1.000 0
             *
        ridge_prior: 1
            0.000 0
            0.160 0
            0.164 50
            0.195 500
            0.234 500
            0.289 50
            0.301 0
            1.000 0
             *
        ridge_prior_interp:
            0 0
            1 0
            7 25
            12 92
            16 256
            65535 256
             *
        std_pos:
            0.00000 0.00600
            0.15234 0.00600
            0.22656 0.00600
            0.23828 0.00800
            0.24609 0.00998
            0.31250 0.00999
            0.34375 0.00349
            1.00000 0.00349
             *
        std_neg:
            0.00000 0.00600
            0.20703 0.00600
            0.22656 0.00650
            0.23438 0.00700
            0.23828 0.00999
            0.25391 0.02000
            0.30469 0.02000
            0.35938 0.01665
            0.41797 0.00970
            1.00000 0.00600
             *
        res_perp:       0.00325
        res_para:       0.00425
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

    bayes_tex_pdf_n:    1
    face_weight:    0.0
    face_pdf_tex_embedded_rle:  0 4096 *
    flash_r_nrm:    0.2678
    flash_b_nrm:    0.2882
    flash_tol:      0.06
    #flash_weight:      255  #TO-DO: Need to FIX
    use_sensor_awb: FALSE
END

AGC
    y_target:
        0 0.49875
        42 0.49875
        183 0.49875
        635 0.49875
        1184 0.49875
        5922 0.49875
        18000 0.49875
        65535 0.49875
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
    max_saturated_highlight:    0.93

    y_coeffs:   0.299 0.587 0.114 *

    max_gain:       100.000
    min_gain:       0.125

    num_exp_modes:  1
    EXP_MODE 0
        num_stages:     1
        shutter_min:    100
        shutter_max:    33333 0 0 0 0 0 0 0 0 0 *
        ag_min:         1.000
        ag_max:         8.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *
        aperture_min:   1.000
        aperture_max:   1.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 *

        max_gain_product: 12.0
    END

    agc_speed_video_up: 0.072
    agc_speed_video_down:   0.564

    fast_startup_frames:    9
    dropped_startup_frames: 5

    # the frame_quantisation allows the AEC to update the camera every N
    # frames. use if the camera cannot be written to after each write.
    frame_quantisation: 3 #2
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


DENOISE
   abscissae:   0 331 875 1780 3323 5187 7587 8192 65535 *
   ordinates:   25 25 39 52 73 98 141 141 141 *

   gr_gain: 2.625
   gb_gain: 2.625
   r_gain: 2.625
   b_gain: 2.625

   r_str: 0.539
   gr_str: 0.539
   gb_str: 0.539
   b_str: 0.539
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
bottom: 56
left: 0
right: 0
threshold: 0
dark_frame_multiplier: 0

#12b to 16b decompanding for HDR - take lower 16b
   abscissae_r:  0 512  1408  2176  2560  4095  65535  *
   ordinates_r:  0 1920 16256 65408 65408 65535 65535  *
   abscissae_b:  0 512  1408  2176  2560  4095  65535  *
   ordinates_b:  0 1920 16256 65408 65408 65535 65535  *
   abscissae_gr: 0 512  1408  2176  2560  4095  65535  *
   ordinates_gr: 0 1920 16256 65408 65408 65535 65535  *
   abscissae_gb: 0 512  1408  2176  2560  4095  65535  *
   ordinates_gb: 0 1920 16256 65408 65408 65535 65535  *
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
abscissae_r: 	0  	128 	256 	512 	1024	1536    2048    4096    8192    16384   32768       65535 *
ordinates_r:  	0   1695    2323    3183    4362    5245    5626    6616    7188    7710    8003        8192  *
abscissae_g: 	0  	128 	256 	512 	1024	1536    2048    4096    8192    16384   32768       65535 *
ordinates_g:  	0   1695    2323    3183    4362    5245    5626    6616    7188    7710    8003        8192  *
abscissae_b: 	0  	128 	256 	512 	1024	1536    2048    4096    8192    16384   32768       65535 *
ordinates_b:  	0   1695    2323    3183    4362    5245    5626    6616    7188    7710    8003        8192  *
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
   cv_r:           19838 14035 11045 9461 8315 7472 6901 6495 6277 6174 6238 6429 6744 7220 7942 8894 10290 12198 15778 23757 32767
 16087 12160 10021 8577 7530 6771 6245 5892 5668 5606 5626 5820 6111 6550 7186 8102 9298 10969 13633 19089 31828
 13669 10905 9045 7800 6868 6193 5712 5380 5160 5087 5123 5284 5563 5984 6580 7366 8463 9967 12084 16121 24206
 12384 10151 8466 7296 6422 5831 5335 4992 4783 4674 4720 4884 5195 5583 6137 6887 7917 9255 11202 14387 20104
 11713 9757 8141 7003 6146 5533 5071 4743 4475 4361 4381 4586 4881 5304 5858 6584 7542 8841 10653 13485 18365
 11493 9480 7944 6880 6045 5434 4976 4595 4323 4130 4169 4415 4750 5177 5736 6421 7320 8628 10381 13008 17414
 11803 9504 7941 6839 6023 5431 4942 4599 4292 4096 4144 4401 4741 5178 5720 6438 7371 8642 10370 13006 17441
 11728 9707 8163 7026 6176 5518 5063 4705 4471 4330 4350 4552 4873 5285 5829 6566 7503 8851 10590 13267 17756
 12404 10193 8624 7339 6460 5791 5306 4971 4748 4637 4656 4832 5139 5577 6103 6840 7850 9194 11064 14098 19425
 13571 10932 9140 7869 6952 6232 5744 5348 5143 5026 5052 5219 5492 5944 6513 7305 8369 9820 11852 15670 23119
 15919 12144 10068 8645 7601 6873 6290 5903 5675 5541 5573 5727 6025 6485 7107 7990 9131 10751 13140 18257 29900
 19689 14239 11368 9693 8512 7653 7031 6685 6422 6284 6306 6464 6773 7259 7922 8891 10205 12080 15472 23115 32767
 25800 17207 13053 11031 9672 8631 7970 7706 7394 7258 7261 7418 7734 8243 8948 10022 11565 13786 18811 31496 32767
 *

   cv_gr:          19912 14388 11426 9760 8477 7663 7045 6631 6414 6287 6362 6534 6882 7383 8120 9149 10547 12564 16097 24347 32767
 16348 12548 10372 8818 7716 6956 6384 6009 5773 5704 5720 5918 6227 6688 7333 8296 9554 11216 13967 19400 31752
 14026 11218 9342 8007 7016 6336 5821 5458 5239 5145 5192 5349 5661 6096 6711 7538 8670 10238 12431 16483 24454
 12660 10505 8730 7488 6581 5944 5416 5051 4841 4722 4762 4944 5251 5678 6249 7042 8090 9537 11506 14734 20481
 12019 10031 8344 7166 6283 5631 5140 4782 4523 4390 4420 4621 4948 5383 5959 6727 7689 9101 10948 13799 18658
 11758 9802 8142 7040 6169 5511 5026 4634 4328 4126 4181 4443 4796 5237 5831 6561 7535 8853 10710 13355 17736
 12088 9783 8115 6993 6118 5508 4994 4623 4308 4096 4161 4436 4795 5246 5817 6542 7518 8869 10681 13381 17908
 12023 9969 8362 7159 6286 5588 5105 4740 4482 4346 4380 4589 4925 5358 5941 6714 7693 9088 10919 13683 18320
 12676 10473 8836 7517 6601 5883 5356 5008 4776 4664 4699 4884 5190 5658 6215 7016 8050 9467 11444 14498 19776
 13939 11236 9397 8058 7078 6345 5830 5409 5181 5071 5115 5278 5589 6052 6640 7453 8581 10120 12227 16031 23268
 16077 12504 10368 8848 7753 7004 6399 6003 5759 5603 5650 5816 6138 6629 7260 8172 9405 11052 13550 18647 29891
 19697 14559 11679 9943 8740 7811 7166 6808 6520 6409 6400 6564 6883 7409 8127 9136 10484 12364 15943 23577 32767
 25423 17424 13368 11348 10016 8827 8143 7862 7513 7486 7381 7534 7834 8398 9228 10358 11842 14030 19361 32048 32767
 *

   cv_b:           19494 14059 11362 9713 8521 7647 7091 6636 6423 6297 6363 6564 6896 7432 8143 9174 10634 12494 15865 23632 32767
 15830 12374 10311 8767 7728 6938 6392 6017 5791 5718 5726 5925 6252 6705 7401 8307 9567 11264 13898 19064 30342
 13697 11103 9249 7945 7051 6317 5811 5479 5253 5159 5196 5362 5667 6130 6764 7588 8685 10298 12425 16273 23574
 12450 10435 8657 7479 6587 5932 5416 5057 4838 4726 4771 4952 5271 5701 6290 7057 8126 9560 11540 14599 19865
 11812 9956 8269 7140 6253 5617 5146 4786 4513 4400 4420 4641 4968 5403 5978 6733 7742 9101 11023 13730 18199
 11613 9688 8093 7024 6144 5498 5024 4615 4326 4131 4190 4453 4819 5259 5861 6589 7570 8877 10717 13318 17585
 11957 9681 8079 6970 6097 5501 4984 4607 4297 4096 4168 4443 4804 5265 5835 6588 7570 8862 10686 13308 17633
 11890 9840 8291 7117 6257 5558 5097 4719 4479 4341 4387 4613 4940 5352 5954 6758 7726 9142 10957 13524 17661
 12428 10362 8754 7455 6556 5854 5333 5009 4766 4662 4710 4895 5198 5674 6227 7044 8050 9491 11486 14313 18984
 13472 11136 9311 8001 7055 6302 5820 5385 5181 5087 5121 5286 5600 6062 6658 7490 8599 10163 12225 15818 22403
 15473 12274 10266 8773 7712 6950 6382 5986 5731 5604 5651 5825 6136 6628 7299 8197 9368 11058 13448 18320 28726
 18799 14151 11502 9815 8594 7746 7160 6795 6483 6373 6410 6568 6903 7424 8102 9125 10471 12379 15561 22780 32767
 23947 16706 13078 11140 9702 8749 8154 7857 7463 7385 7406 7530 7888 8438 9104 10289 11867 14058 18461 30109 32767
 *

   cv_gb:          20178 14493 11461 9825 8586 7677 7100 6682 6463 6319 6387 6572 6912 7401 8155 9194 10553 12612 16109 24348 32767
 16526 12574 10449 8861 7768 6989 6429 6039 5800 5723 5753 5936 6252 6720 7365 8290 9576 11262 14026 19443 31680
 14133 11267 9407 8056 7065 6371 5853 5491 5261 5159 5212 5378 5681 6112 6737 7570 8697 10264 12442 16538 24653
 12834 10565 8794 7548 6607 5970 5446 5072 4859 4745 4780 4952 5268 5687 6288 7050 8111 9570 11539 14777 20541
 12077 10125 8362 7188 6299 5651 5159 4813 4536 4412 4424 4632 4963 5397 5980 6729 7713 9075 11008 13817 18550
 11810 9807 8167 7077 6169 5521 5045 4650 4350 4139 4182 4448 4807 5242 5835 6568 7507 8870 10710 13402 17904
 12119 9810 8138 7026 6139 5539 5009 4632 4324 4096 4165 4445 4800 5253 5822 6555 7544 8856 10670 13385 17951
 12062 9996 8387 7188 6292 5600 5112 4755 4501 4352 4384 4589 4936 5373 5934 6719 7701 9089 10928 13628 18099
 12689 10515 8837 7525 6611 5897 5370 5029 4784 4668 4711 4889 5193 5661 6221 6991 8042 9454 11437 14487 19756
 13912 11300 9415 8076 7094 6341 5843 5413 5190 5081 5121 5280 5596 6059 6655 7487 8604 10083 12253 16013 23103
 16058 12504 10348 8875 7782 7003 6404 6010 5753 5613 5648 5830 6137 6622 7281 8170 9393 11049 13489 18609 29992
 19559 14531 11639 9948 8692 7798 7171 6807 6519 6394 6409 6569 6895 7412 8098 9108 10456 12368 15795 23460 32767
 25012 17343 13299 11315 9843 8797 8146 7848 7520 7426 7406 7522 7867 8416 9122 10289 11789 14045 19052 31730 32767
 *

   cell_sz_power_x: 6
   cell_sz_power_y: 6
   pitch: 21
   vpitch: 13
END

STATISTICS
hmask0: 255
hmask1: 65280
shift: 5 # bpp - 7
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

