# ESP32-S2 WUD-Ducky

WUD-Ducky is to [WUD](https://wiki.aprbrother.com/en/wud.html#wud) what [WHID-Injector](https://github.com/whid-injector/WHID) is to [Cactus-WHID](https://blog.aprbrother.com/product/cactus-whid): a rubber ducky script parser, with mouse and pendrive support.

**This is project requires basic troubleshooting skills and a good understanding of the Arduino IDE.**

WUD-Ducky is an attempt to add USB HID to the default example provided with @AprilBrother's [WUD](https://wiki.aprbrother.com/en/wud.html) (*), however it also supports any ESP32-S2 equipped with a SD Card.


The default [platformio POC](https://github.com/volca/wireless_usb_disk) coming with the WUD has been extended to support HID Composite (Keyboard+Mouse) and implements a payload parser.
The payload syntax is heavily inspired by [whid-injector](https://github.com/whid-injector/WHID) and [wifi_ducky](https://github.com/spacehuhn/wifi_ducky).

# Features

 - Dongle is detected as Mouse, Keyboard and USB Pendrive
 - Mouse driver uses absolute positioning, can be controlled from WebUI
 - Mounted MicroSD card allows editing files from the OS, ~~and~~ or reading from the ESP
 - Starts WiFi in AP or APSTA Mode
 - Runs a Webserver with the following endpoints:
   - `/quack` send script, key or string sequence to the keyboard
   - `/list` list available files on the SD Card
   - `/logs` call the last logs (and purges the log queue)
   - `/changefs` select the filesystem, SD or SPIFFS
   - `/runpayload` run ducky script from the selected filesystem
   - `/delete` delete a file
   - `/upload` upload a file
   - `/*` serves the file from the selected filesystem, makes them editable when SPIFFS is selected
 - Implements **[SpaceHuhn's WiFiDuck commands](https://github.com/SpacehuhnTech/WiFiDuck#scripting)**, but also comes with a few extras:
   - `help` : list all non standard ducky commands
   - `logs` : show last logs
   - `info` : print system information
   - `SetDisplay` [uint16_t+uint16_t] set display dimensions for mouse e.g. `SetDisplay:1920+1080`
   - `DrawSpaceInvaders` : draw space invaders with the mouse, needs paint software full screen with brush preselected
   - `MouseClickRelease` : release all mouse buttons
   - `MouseClickRIGHT` : pushes right button, call MouseClickRelease to release
   - `MouseClickLEFT` : pushes left button, call MouseClickRelease to release
   - `MouseClickMIDDLE` : pushes middle button, call MouseClickRelease to release
   - `MouseDoubleClickLEFT` : does not need MouseClickRelease
   - `MouseMoveUp` [int8_t] move n pixels to the top, relative to last position, from -127 to 127
   - `MouseMoveDown` [int8_t] move n pixels to the bottom, relative to last position, from -127 to 127
   - `MouseMoveLeft` [int8_t] move n pixels to the left, relative to last position, from -127 to 127
   - `MouseMoveRight` [int8_t] move n pixels to the roght, relative to last position, from -127 to 127
 - The following custom commands are also internally used at setup/boot, and kept exposed for debug or development:
   - `SerialBegin` : start the serial interface
   - `SerialDebug` : enable debug on serial
   - `InitWiFi` : start wifi (implicit)
   - `InitSPIFFS` : start SPIFFS (implicit)
   - `InitSD` : start SD card (implicit)
   - `InitPenDrive` : start USB PenDrive (implicit)
   - `StopSD` : stop SD card (experimental)
   - `StopPenDrive` : stop USB Pendrive (experimental)
   - `InitKeyboard` : start USB Keyboard (implicit)
   - `InitMouse` : start USB mouse (implicit)
   - `StartUSB` : start USB (implicit)
   - `StartWebServer` : start web server (implicit)
   - `StopSerial` : stop serial
   - `StopWiFi` : stop WiFi
   - `StopKeyboard` : stop USB Keyboard (experimental)
   - `StopMouse` : stop USB Mouse (experimental)
   - `EnableOTA`         Enable OTA flashing, default = on
   - `DisableOTA`        Disnable OTA flashing
   - `ResetPrefs`        Reset all prefs (wifi, ntp)
   - `SetSSID_AP`        Set the SSID for the WiFi Access Point
   - `SetSSID_STA`       Set the SSID for the WiFi Station
   - `SetPassword_AP`    Set the Password for the WiFi Access Point
   - `SetPassword_STA`   Set the Password for the WiFi Station
   - `SetNTPZone`        Set the NTP preferred zone
   - `SetDST`            Set the Daylight Savings time
   - `SetTZ`             Set the Time Zone


 - Comes with many bugs but also with an easter egg


# Debugging:

With the lastest changed, `USB CDC On Boot` should be **disabled** as it glitches the USB device detection with Windows.
It's not an issue since OTA updates are now enabled.
WiFi STA credentials should be set using `SetSSID_STA` and `SetPassword_STA` **ducky commands** though.
This only needs to be done once as those will be saved in NVS memory and used on next restart.

~~With the latests changes, it's not necessary to use the magnet when flashing as long as the following settings are enabled:~~
~~ - `USB CDC On Boot` : `enabled`~~
~~ - `Upload Mode` : `Internal USB`~~

# Customizing:

The `boards` folder of this project has two files:
  - `plaform.local.txt` snippet for the board profile, same as esp32-dev but bound to a different pins_arduino.h and has USB_CDC_ON_BOOT enabled by default
  - `variants/esp32s2_wud/pins_arduino.h` uses the default pinout for esp32-s2 but is editable to customize USB vid/pid, etc

Both need each other, so just copy the contents of the `boards` folder where your `board.txt` is.

```log
usb 2-6.3: new full-speed USB device number 33 using ehci-pci
usb 2-6.3: New USB device found, idVendor=cafe, idProduct=deca, bcdDevice= 1.00
usb 2-6.3: New USB device strings: Mfr=1, Product=2, SerialNumber=3
usb 2-6.3: Product: WUD-Nutquacker
usb 2-6.3: Manufacturer: IGNSOC
usb 2-6.3: SerialNumber: 0xDEADB33F
usb-storage 2-6.3:1.0: USB Mass Storage device detected
scsi host20: usb-storage 2-6.3:1.0
input: IGNSOC WUD-Nutquacker Mouse as /devices/pci0000:00/0000:00:1d.7/usb2/2-6/2-6.3/2-6.3:1.1/0003:CAFE:DECA.0052/input/input171
input: IGNSOC WUD-Nutquacker Keyboard as /devices/pci0000:00/0000:00:1d.7/usb2/2-6/2-6.3/2-6.3:1.1/0003:CAFE:DECA.0052/input/input172
hid-generic 0003:CAFE:DECA.0052: input,hidraw2: USB HID v1.11 Mouse [IGNSOC WUD-Nutquacker] on usb-0000:00:1d.7-6.3/input1
cdc_acm 2-6.3:1.2: ttyACM1: USB ACM device
scsi 20:0:0:0: Direct-Access     Pendrive TheQuacken       0x2a PQ: 0 ANSI: 2
sd 20:0:0:0: Attached scsi generic sg4 type 0
sd 20:0:0:0: [sdc] 15564800 512-byte logical blocks: (7.97 GB/7.42 GiB)
sd 20:0:0:0: [sdc] Write Protect is off
sd 20:0:0:0: [sdc] Mode Sense: 03 00 00 00
sd 20:0:0:0: [sdc] No Caching mode page found
sd 20:0:0:0: [sdc] Assuming drive cache: write through
 sdc: sdc1
sd 20:0:0:0: [sdc] Attached SCSI removable disk
```






# WUD w00t?

WUD is the recursive acronym for **WiFi USB Disk**: it is an ESP32-S2 dongle made by @AprilBrother with a Micro SDCard slot hidden in the USB port.

See the [wiki page](https://wiki.aprbrother.com/en/wud.html) for more info.


[![WUD Product image](https://user-images.githubusercontent.com/1893754/134933549-c7af84b7-2575-4f5b-87c0-4d1dec573ff4.png)](https://wiki.aprbrother.com/en/wud.html)


## Hardware/software Requirements:

 - ESP32-S2 with SD Card
 - Arduino IDE
 - [arduino-esp32 Core 2.0.2](https://github.com/espressif/arduino-esp32/releases/tag/2.0.2)


## Why Arduino?

 - Because [platformio POC](https://github.com/volca/wireless_usb_disk) only works with older packages e.g. EspTinyUSB 1.2.0 / idf 4.2 or lags behind
 - Because I'm lazy

## Roadmap

 - Improved payload parser
 - Improved web UI
 - Add RNDIS/CDC-ECM (network interface)
 - More lambda ducky commands (e.g. read exfiltrated data from USBSerial)
 - OS Fingerprinting
 - LED communication (for optical data exfiltration)


## Credits:

 - @chegewara for helping me starting with this project and maintaining the awesome ESPTinyUSB library.
 - @SpaceHuhn for letting my copy parts of the WiFiDuck parser.
 - @whid-injector for providing inspiration.
 - @bblanchon for his awesome ArduinoJson library.
 - https://github.com/spacehuhn/wifi_ducky
 - https://github.com/chegewara/EspTinyUSB
 - https://github.com/whid-injector/WHID
 - https://github.com/bblanchon/ArduinoJson
 - https://github.com/me-no-dev/ESPAsyncWebServer


