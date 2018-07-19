# RemoteDownloadMQTT
Write FPGA firmware for Lattice iCE40 FPGA's via webpage and MQTT!

Please check out the /doc for instructions on how to use this example and set up hardware.

The following project:
1. Provides a PHP website to:
    a. Upload an iCE40 FPGA bitstream in *.hex format from Lattice iCECube or Radiant software
    b. Passes the file to a specified MQTT broker via Mosquito MQTT Client Library for PHP
2. Uses a CloudMQTT broker to publish messages to all device on the same MQTT TOPIC
3. Uses ESP32 to subscribe to the broker as an MQTT client.  Looks for mqtt messages once subscribed
4. Puts Lattice iCE40 FPGA in reset and writes published MQTT data to external SPI flash
5. Releases reset of Lattice iCE40 FPGA, where it boots from the newly written firmware in SPI flash

Libraries and source code shout outs:
    - Mosquitto MQTT Client Library for PHP:
        https://github.com/mgdm/Mosquitto-PHP
    - MQTT Client example for ESP32
        https://pubsubclient.knolleary.net
    - Winbond SPI flash C++ Library
        https://www.instructables.com/id/How-to-Design-with-Discrete-SPI-Flash-Memory/
        