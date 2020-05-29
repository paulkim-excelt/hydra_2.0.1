include ar0143_csi_hdr.mod

AWB
    awb_method:    METHOD_GREY_WORLD

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

    use_sensor_awb: TRUE
    tuner_run_rate: 4
END