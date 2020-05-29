include ar0143_csi_hdr.mod

FRAME
width: 1344
height: 968
bayer_order: GRBG
bayer_enable:
    image_input
    black_level
    #lens_shading
    denoise
    statistics
    white_balance_gain
    defective_pixel_auto
    #crosstalk
    #colour_denoise
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
    #conversion
    #lresize
    hi_res_output
    #lo_res_output
*
END


IMAGE_INPUT
format: RAW16
stride: 2688
transposed: 0
END
