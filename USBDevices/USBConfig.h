#pragma once

// moving this to pins_arduino.h is more efficient, especially if using USB_CDC_ON_BOOT

//#if ARDUINO_USB_CDC_ON_BOOT
  // USB driver has an early init and uses internal defines,
  // the only place to override those is in pins_arduino.h
//#else

  #ifndef USB_VID
    #define USB_VID          0xcafe
  #endif
  #ifndef USB_PID
    #define USB_PID          0xdeca
  #endif
  #ifndef USB_MANUFACTURER
    #define USB_MANUFACTURER "AprilBrothers"
  #endif
  #ifndef USB_PRODUCT
    #define USB_PRODUCT      "ESP32-WUD"
  #endif
  #ifndef USB_SERIAL
    #define USB_SERIAL       "0xDEADB33F"
  #endif

//#endif
#include "USB.h"
