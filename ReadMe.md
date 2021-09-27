# ESP32-S2 WUD-Ducky

This project is an attempt to add USB HID to the default example provided with @AprilBrother's [WUD](https://wiki.aprbrother.com/en/wud.html) (*), however it also supports any ESP32-S2 equipped with a SD Card.


The default [platformio POC](https://github.com/volca/wireless_usb_disk) coming with the WUD has been extended to support HID Composite (Keyboard+Mouse) and implements a payload parser.
The payload syntax is heavily inspired by [whid-injector](https://github.com/whid-injector/WHID) and [wifi_ducky](https://github.com/spacehuhn/wifi_ducky).


# Features

 - Dongle is detected as composite HID (Mouse and Keyboard) + USB Pendrive
 - Mounted MicroSD card allows editing files from the OS (**)
 - Starts WiFi in AP Mode
 - Runs a Webserver with the following endpoints:
   - `/key` send string sequence to the keyboard
   - `/list` list available files on the SD Card
   - `/runpayload` run ducky script from the SD Card
   - `/delete` delete a file on the SD Card (**)
   - `/upload` upload a file to the SD Card (**)
   - `/*` serves the file (e.g. index.html) from SPIFFS if it exists


(**) Changes on the SD Card made from the USB Pendrive aren't immediately accessible by the ESP32-S2's `SD.read()`, and vice-versa, so it is recommended to make a full power cycle after either changes are applied.
This is due to the fact that USB Pendrive accesses the SD Partition using `SD.rawRead` and `SD.rawWrite`, and the lack of callable flushing methods with `SD` once the raw changes affect the file allocation table.
Hopefully this will be fixed in the future, but it still wasn't at the time this readme file was written (2021:09:27).


# WUD w00t?

WUD is the recursive acronym for **WiFi USB Disk**: it is an ESP32-S2 dongle with a micro sdcard slot hidden in the USB port. [wiki](https://wiki.aprbrother.com/en/wud.html).

[![WUD Product image](https://i1.aprbrother.com/wud-1.jpg-640.jpg)](https://wiki.aprbrother.com/en/wud.html)


## Hardware/software Requirements:

 - ESP32-S2 with SD Card
 - Arduino IDE
 - [arduino-esp32 Core 2.0.0](https://github.com/espressif/arduino-esp32/releases/tag/2.0.0)
 - [EspTinyUSB 1.3.4](https://github.com/chegewara/EspTinyUSB/releases/tag/1.3.4)


## Why Arduino?

 - Because [platformio POC](https://github.com/volca/wireless_usb_disk) only works with older packages e.g. EspTinyUSB 1.2.0 / idf 4.2
 - Because I'm lazy

## Roadmap

 - Improved payload parser
 - Improved web UI
 - Add RNDIS/CDC-ECM (network interface)


## Credits:

Special thanks to @chegewara for helping me with this project and maintaining the awesome ESPTinyUSB library.

 - https://github.com/chegewara/EspTinyUSB
 - https://github.com/whid-injector/WHID
 - https://github.com/spacehuhn/wifi_ducky

