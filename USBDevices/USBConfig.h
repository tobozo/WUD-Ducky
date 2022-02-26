#pragma once


#include "esp32-hal-tinyusb.h"

// the only reliable place to override those is in pins_arduino.h

#ifndef USB_VID
  // not a good idea to modify this: problems can happen on Windows
  //#define USB_VID USB_ESPRESSIF_VID
#endif
#ifndef USB_PID
  // not a good idea to modify those: problems can happen on Windows
  //#define USB_PID 0x0002
#endif
#ifndef USB_MANUFACTURER
  #define USB_MANUFACTURER "AprilBrothers"
  //#define USB_MANUFACTURER CONFIG_TINYUSB_DESC_MANUFACTURER_STRING
#endif
#ifndef USB_PRODUCT
  #define USB_PRODUCT      "ESP32-WUD"
  //#define USB_PRODUCT CONFIG_TINYUSB_DESC_PRODUCT_STRING
#endif
#ifndef USB_SERIAL
  // not a good idea to modify this: problems can happen on Windows
  //#define USB_SERIAL CONFIG_TINYUSB_DESC_SERIAL_STRING
#endif


#include "USB.h"
#include "../logger.h"
#include "../status.h"
