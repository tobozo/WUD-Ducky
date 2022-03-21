/*\
 *
 * ESP32-S2 WUD-Ducky
 *
 * Copyright (c) 2021 tobozo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
\*/

// Configuration checks Arduino core, esp-idf, USB menu options


// check Arduino core version
#if !defined ESP_ARDUINO_VERSION_VAL
  #error "Invalid esp32-core version (expected: >=2.0.2)"
#else
  //#if __has_include("core_version.h") // for platformio
  #include "core_version.h"
  //#endif
  #if defined ESP_ARDUINO_VERSION && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
    #if ARDUINO_ESP32_GIT_VER == 0x44c11981
      // Missing SD.open(create-subdirs), SD.readRAW(), SD.writeRAW(), SD.numSectors() and SD.cardSize()
      #error "ESP32 Arduino 2.0.0 (0x44c11981) is unsupported (>=2.0.2 required)"
    #elif ARDUINO_ESP32_GIT_VER == 0x15bbd0a1
      // This version introduces SD.open(create-subdirs)
      // Missing SD.readRAW() / SD.writeRAW() / SD.numSectors() and SD.cardSize()
      #error "ESP32 Arduino 2.0.1 RC1 (0x15bbd0a1) is unsupported (>=2.0.2 required)"
    #elif ARDUINO_ESP32_GIT_VER == 0xd218e58f
      // OTA support broken, missing SD.readRAW() / SD.writeRAW() / SD.numSectors() and SD.cardSize()
      #error "ESP32 Arduino 2.0.1 (0xd218e58f) is unsupported (>=2.0.2 required)"
    #elif ARDUINO_ESP32_GIT_VER == 0xcaef4006
      // Fine, that's the exact esp32 core version this app is expecting.
      // This version introduces the much needed SD.readRAW(), SD.writeRAW(), SD.numSectors() and SD.cardSize()
      // However SD support needs patching and is highly unstable in some situations.
      #warning "ESP32 Arduino 2.0.2 (0xcaef4006) support it still experimental, and SD library needs a patch"
    #else
      #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 2)
        // 2.0.3 and further versions
        #warning "Untested arduino-esp32 version, feel free to experiment"
      #else
        // ESP32 Arduino 1.0.6?
        #error "Unknown arduino-esp32 version, comment this out if your package version >= 2.0.2"
      #endif
    #endif
  #else
    #error "Invalid esp32-core version (expected: >=2.0.2)"
  #endif
#endif


// check esp-idf version
#if !defined ESP_IDF_VERSION_VAL
  #error "Invalid esp-idf version (expected: >=4.4.0)"
#else
  #if defined ESP_IDF_VERSION && ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
    // fine, that's our target for esp-idf core
  #else
    #error "Invalid esp-idf version (expected: >=4.4.0)"
  #endif
#endif

// check target config for device
#if !defined CONFIG_IDF_TARGET_ESP32S2
  #error "This project is for ESP32-S2 only!"
#endif

// check config for USB
#if !defined CONFIG_TINYUSB_ENABLED
  #error "This project needs CONFIG_TINYUSB_ENABLED in sdkconfig"
#endif


// check that required Arduino menu options are present
#if !defined ARDUINO_USB_CDC_ON_BOOT
  #error "ARDUINO_USB_CDC_ON_BOOT build option is missing"
#endif

#if !defined ARDUINO_USB_MSC_ON_BOOT
  #error "ARDUINO_USB_MSC_ON_BOOT build option is missing"
#endif
#if !defined ARDUINO_USB_DFU_ON_BOOT
  #error "ARDUINO_USB_DFU_ON_BOOT build option is missing"
#endif

// check that required Arduino menu options are well set
#if ARDUINO_USB_CDC_ON_BOOT!=0
  #error "The menu option 'Tools / USB CDC On Boot' should be disabled"
#endif
#if ARDUINO_USB_MSC_ON_BOOT!=0
  #warning "The menu option 'Tools / USB Firmware MSC On Boot' should be disabled or the SD Card will be ignored!"
#endif
#if ARDUINO_USB_DFU_ON_BOOT!=0
  #error "The menu option 'Tools / USB DFU On Boot' should be disabled!"
#endif


#if (ARDUINO_USB_CDC_ON_BOOT|ARDUINO_USB_MSC_ON_BOOT|ARDUINO_USB_DFU_ON_BOOT)
  #if !defined ARDUINO_ESP32S2_WUD
    #warning "Using default USB VendorID and ProductID"
  #endif
#endif

#include <vector>
#include <SD.h>
#include "logger.h"
#include "WiFiDuck/config.h"
// floating filesystem
static fs::FS* duckyFS = nullptr;
