#pragma once

// the only reliable place to override those is in pins_arduino.h
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


#include "USB.h"
#include "../logger.h"
#include "../status.h"
