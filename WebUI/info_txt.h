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
#pragma once


const char* info_txt = R"infoTXT(

[ WUD Ducky 🦆 System Information ]
----------------------

  == Software ==
    SDK version: {{getSdkVersion}}
    Chip Id: {{getChipModel}}
    WebServer: {{webserver_begun}}
    Logging: {{logging_enabled}}
    Serial debug: {{hwserial_begun}}
       Debug port: {{SerialDebug}}

  == CPU ==
    Frequency: {{getCpuFreqMHz}} MHz

  == Flash chip information ==
    Chip Id: {{getFlashChipMode}}
    Estimated size: {{getFlashChipSize}}
    Frequency: {{flashFreq}} MHz
    Write mode: {{ideMode}}

  == Sketch information ==
    Hash: {{getSketchMD5}}
    Size: {{getSketchSize}}
    Free space available: {{getFreeSketchSpace}}

  == Heap information ==
    Total size: {{getHeapSize}}
    Free available: {{getFreeHeap}}
    Lowest level since boot: {{getMinFreeHeap}}
    Largest allocatable block: {{getMaxAllocHeap}}

  == USB information ==
    Vendor ID: {{USB_VID}}
    Product ID: {{USB_PID}}
    Manufacturer: {{USB_MANUFACTURER}}
    Product Name: {{USB_PRODUCT}}
    Serial Number: {{USB_SERIAL}}
    HUB: {{usb_begun}}
    HID: {{hid_ready}}
    Serial : {{usbserial_begun}}
    Keyboard: {{keyboard_begun}}
       Caps Lock:   {{capslock_on}}
       Num Lock:    {{numlock_on}}
       Scroll Lock: {{scrolllock_on}}
       Locale: {{KEYBOARD_LOCALE}}
    Mouse: {{absmouse_begun}}
    PenDrive: {{pendrive_begun}}

  == Storage ==
    SD: {{sd_begun}}
    LittleFS: {{spiffs_begun}}

  == Network ==
    MAC Address: {{MAC_ADDR}}
    STA IP Address: {{STA_ADDR}}
    WiFi Access Point: {{softap_begun}} (SSID={{AP_SSID}} PASS={{AP_PASSWORD}})
    WiFi Station: {{wifista_begun}} (SSID={{STA_SSID}} PASS={{STA_PASSWORD}})
    NTC Client: {{ntp_enabled}}
       Server zone: {{NTP_ZONE}}
       Server address: {{NTP_SERVERADDR}}


)infoTXT";

