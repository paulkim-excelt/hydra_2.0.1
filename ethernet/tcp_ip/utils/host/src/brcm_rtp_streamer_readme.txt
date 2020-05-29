Steps on usage of brcm_rtp_streamer application
===============================================

[x] Initialize network interface on linux PC with static IP address 192.168.10.2 (Ubuntu OS is recommended)

[x] Compile the application using gcc and verify that the app "brcm_rtp_streamer" is created
    # gcc brcm_rtp_streamer.c -o brcm_rtp_streamer

[x] Execute the application on Linux PC using the below command
    # sudo ./brcm_rtp_streamer -f <file name> -i <interface> -r <frame_rate> -s <num_slice_per_frame>

    where:
    <file name> : Raw H264 file to be used for streaming
    <interface> : Network interface e.g. eth0, eno1 etc.
    <frame_rate> : The number of H264 video frames that needs to be transmitted per second
    <num_slice_per_frame> : Number of h264 slices/NALs in a video frame_rate


    Example usage: # sudo ./brcm_rtp_streamer -f parkrun_1280x720p.264.bin -i eth0 -r 30 -s 1

