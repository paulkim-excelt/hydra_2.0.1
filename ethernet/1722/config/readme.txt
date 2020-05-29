Python scripts used to generate the configs
===========================================

[x] avbl_config.py
    Usage: ./avbl_config.py -i <input_configuration_file.xml> -o <output_config_file.bin> -s <xml_schema.xsd>

[x] avbt_config.py
    Usage: ./avbt_config.py -i <input_configuration_file.xml> -o <output_config_file.bin> -s <xml_schema.xsd>


AVB Talker configuration files
==============================

[x] avb_talker_config.xml
    Used by vidxmtr, vidxcvr and stitch component application
    Enabled ports: 1
    Description: Default configuration used for video streaming

[x] avb_talker_config1.xml
    Used by AVBT Integration test application
    Enabled ports: 4
    Description: Configuration used for Integration Testing all ports of AVB Talker component

[x] avb_talker_config2.xml
    Used by audvidxcvr application
    Enabled ports: 2
    Description: Configuration used for sending the audio and video streams

[x] avb_talker_config11.xml
    Used by Tx-1 node of multi-decode-stitch use case
    Enabled ports: 1
    Description: Configuration used for sending the first stream used in multi-decode-stitch use case

[x] avb_talker_config12.xml
    Used by Tx-2 node of multi-decode-stitch use case
    Enabled ports: 1
    Description: Configuration used for sending the second stream used in multi-decode-stitch use case

[x] avb_talker_config13.xml
    Used by Tx-3 node of multi-decode-stitch use case
    Enabled ports: 1
    Description: Configuration used for sending the third stream used in multi-decode-stitch use case

[x] avb_talker_config14.xml
    Used by Tx-4 node of multi-decode-stitch use case
    Enabled ports: 1
    Description: Configuration used for sending the last stream used in multi-decode-stitch use case

[x] avb_talker_config15.xml
    Used by talker_mv application
    Enabled ports: 4
    Description: Configuration used for sending four streams to be used by talker MV component for streaming

AVB Listener configuration files
================================

[x] avb_listener_config.xml
    Used by vidrcvr, vidxcvr and stitched stream playback application
    Enabled ports: 1
    Description: Default configuration used for video reception

[x] avb_listener_config1.xml
    Used by AVBL Integration test application
    Enabled ports: 4
    Description: Configuration used for Integration Testing all ports of AVB Listener component

[x] avb_listener_config2.xml
    Used by audvidxcvr application
    Enabled ports: 2
    Description: Configuration used for receiving the audio and video streams

[x] avb_listener_config15.xml
    Used by stitch application
    Enabled ports: 4
    Description: Configuration used for receiving four streams to be used by stitch component

Command to generate all BIN files
=================================

# Talker Config Files
./avbt_config.py -s avbt_config.xsd -i avb_talker_config.xml -o avb_talker_config.bin
./avbt_config.py -s avbt_config.xsd -i avb_talker_config1.xml -o avb_talker_config1.bin
./avbt_config.py -s avbt_config.xsd -i avb_talker_config2.xml -o avb_talker_config2.bin
./avbt_config.py -s avbt_config.xsd -i avb_talker_config11.xml -o avb_talker_config11.bin
./avbt_config.py -s avbt_config.xsd -i avb_talker_config12.xml -o avb_talker_config12.bin
./avbt_config.py -s avbt_config.xsd -i avb_talker_config13.xml -o avb_talker_config13.bin
./avbt_config.py -s avbt_config.xsd -i avb_talker_config14.xml -o avb_talker_config14.bin
./avbt_config.py -s avbt_config.xsd -i avb_talker_config15.xml -o avb_talker_config15.bin

# Listener Config Files
./avbl_config.py -s avbl_config.xsd -i avb_listener_config.xml -o avb_listener_config.bin
./avbl_config.py -s avbl_config.xsd -i avb_listener_config1.xml -o avb_listener_config1.bin
./avbl_config.py -s avbl_config.xsd -i avb_listener_config2.xml -o avb_listener_config2.bin
./avbl_config.py -s avbl_config.xsd -i avb_listener_config15.xml -o avb_listener_config15.bin

