# ESP32-S2 WUD-Ducky

This project is an attempt to add USB HID to the default example provided with @AprilBrother's [WUD](https://wiki.aprbrother.com/en/wud.html) (*), however it also supports any ESP32-S2 equipped with a SD Card.


The default [platformio POC](https://github.com/volca/wireless_usb_disk) coming with the WUD has been extended to support HID Composite (Keyboard+Mouse) and implements a payload parser.
The payload syntax is heavily inspired by [whid-injector](https://github.com/whid-injector/WHID) and [wifi_ducky](https://github.com/spacehuhn/wifi_ducky).

# Features

 - Dongle is detected as Mouse, Keyboard and USB Pendrive
 - Mouse driver uses absolute positioning
 - Mounted MicroSD card allows editing files from the OS, and reading from the ESP
 - Starts WiFi in AP Mode
 - Runs a Webserver with the following endpoints:
   - `/key` send string sequence to the keyboard
   - `/list` list available files on the SD Card
   - `/logs` call the last logs (and purges the log queue)
   - `/changefs` select the filesystem, SD or SPIFFS
   - `/runpayload` run ducky script from the selected filesystem
   - `/delete` delete a file
   - `/upload` upload a file
   - `/*` serves the file from the selecte filesystem
 - Accepts the following ducky commands:
   - `Rem`
   - `logs` : show last logs
   - `SetDisplay` [uint16_t+uint16_t] set display dimensions for mouse e.g. `SetDisplay:1920+1080`
   - `DrawSpaceInvaders` : draw space invaders with the mouse, needs paint software full screen with brush preselected
   - `MouseClickRelease` : release all mouse buttons
   - `MouseClickRIGHT` : pushes right button, call MouseClickRelease to release
   - `MouseClickLEFT` : pushes left button, call MouseClickRelease to release
   - `MouseClickMIDDLE` : pushes middle button, call MouseClickRelease to release
   - `MouseDoubleClickLEFT` : does not need MouseClickRelease
   - `DefaultDelay` [uint32_t] set default delay between commands
   - `CustomDelay` [uint32_t] set custom delay between commands
   - `KeyDelay` [uint32_t] set delay between keys
   - `Key` [keycode] send a keycode
   - `MouseMoveUp` [int8_t] move n pixels to the top, relative to last position, from -127 to 127
   - `MouseMoveDown` [int8_t] move n pixels to the bottom, relative to last position, from -127 to 127
   - `MouseMoveLeft` [int8_t] move n pixels to the left, relative to last position, from -127 to 127
   - `MouseMoveRight` [int8_t] move n pixels to the roght, relative to last position, from -127 to 127
   - `Press` [keycode+keycode+keycode....] keyboard shortcuts e.g. KEY_LEFT_CTRL+KEY_LEFT_ALT+KEY_DELETE => `Press:128+130+212`
   - `Print` [String] send sentence to keyboard
   - `PrintLine` [String] (same as Print but HID_KEY_ENTER appended)
   - [soon] ~~`MouseDrawStr` [coord+mousestate+coord+coord....] mouse moves/clics in a string e.g. `MouseDrawStr:X127+Y-127+C1+X127+Y127+Y127+C0+X-127+X-127+C1+X127+X127`~~
 - Comes with many bugs but also with an easter egg


# Debugging:

With the latests changes, it's not necessary to use the magnet when flashing as long as the following settings are enabled:

 - `USB CDC On Boot` : `enabled`
 - `Upload Mode` : `Internal USB`



# WUD w00t?

WUD is the recursive acronym for **WiFi USB Disk**: it is an ESP32-S2 dongle made by @AprilBrother with a Micro SDCard slot hidden in the USB port.

See the [wiki page](https://wiki.aprbrother.com/en/wud.html) for more info.


[![WUD Product image](https://user-images.githubusercontent.com/1893754/134933549-c7af84b7-2575-4f5b-87c0-4d1dec573ff4.png)](https://wiki.aprbrother.com/en/wud.html)


## Hardware/software Requirements:

 - ESP32-S2 with SD Card
 - Arduino IDE
 - [arduino-esp32 Core 2.0.2](https://github.com/espressif/arduino-esp32/releases/tag/2.0.2)


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

