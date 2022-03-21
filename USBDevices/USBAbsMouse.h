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

// inherit relative mouse constants
#include "USBHIDMouse.h"

#if !defined TUD_HID_REPORT_DESC_ABSMOUSE
  // This version of arduino-esp32 does not handle absolute mouse natively.
  // Let's throw a minimalistic implementation of absmouse driver.
  // See: https://github.com/hathach/tinyusb/pull/1363
  // Also see: https://github.com/espressif/arduino-esp32/pull/6331

  // Absolute Mouse data struct is a copy of the relative mouse struct
  // with int16_t instead of int8_t for X and Y coordinates.
  typedef struct TU_ATTR_PACKED
  {
      uint8_t buttons = 0;
      int16_t x = 0;
      int16_t y = 0;
      int8_t wheel = 0;
      int8_t pan = 0;
  } hid_abs_mouse_report_t;


  // Absolute Mouse Report Descriptor Template applies those datatype changes too
  #define TUD_HID_REPORT_DESC_ABSMOUSE(...) \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP      )                   ,\
    HID_USAGE      ( HID_USAGE_DESKTOP_MOUSE     )                   ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION  )                   ,\
      /* Report ID if any */\
      __VA_ARGS__ \
      HID_USAGE      ( HID_USAGE_DESKTOP_POINTER )                   ,\
      HID_COLLECTION ( HID_COLLECTION_PHYSICAL   )                   ,\
        HID_USAGE_PAGE  ( HID_USAGE_PAGE_BUTTON  )                   ,\
          HID_USAGE_MIN   ( 1                                      ) ,\
          HID_USAGE_MAX   ( 5                                      ) ,\
          HID_LOGICAL_MIN ( 0                                      ) ,\
          HID_LOGICAL_MAX ( 1                                      ) ,\
          /* Left, Right, Middle, Backward, Forward buttons */ \
          HID_REPORT_COUNT( 5                                      ) ,\
          HID_REPORT_SIZE ( 1                                      ) ,\
          HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
          /* 3 bit padding */ \
          HID_REPORT_COUNT( 1                                      ) ,\
          HID_REPORT_SIZE ( 3                                      ) ,\
          HID_INPUT       ( HID_CONSTANT                           ) ,\
        HID_USAGE_PAGE  ( HID_USAGE_PAGE_DESKTOP )                   ,\
          /* X, Y absolute position [0, 32767] */ \
          HID_USAGE       ( HID_USAGE_DESKTOP_X                    ) ,\
          HID_USAGE       ( HID_USAGE_DESKTOP_Y                    ) ,\
          HID_LOGICAL_MIN  ( 0x00                                ) ,\
          HID_LOGICAL_MAX_N( 0x7FFF, 2                           ) ,\
          HID_REPORT_SIZE  ( 16                                  ) ,\
          HID_REPORT_COUNT ( 2                                   ) ,\
          HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
          /* Vertical wheel scroll [-127, 127] */ \
          HID_USAGE       ( HID_USAGE_DESKTOP_WHEEL                )  ,\
          HID_LOGICAL_MIN ( 0x81                                   )  ,\
          HID_LOGICAL_MAX ( 0x7f                                   )  ,\
          HID_REPORT_COUNT( 1                                      )  ,\
          HID_REPORT_SIZE ( 8                                      )  ,\
          HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE )  ,\
        HID_USAGE_PAGE  ( HID_USAGE_PAGE_CONSUMER ), \
        /* Horizontal wheel scroll [-127, 127] */ \
          HID_USAGE_N     ( HID_USAGE_CONSUMER_AC_PAN, 2           ), \
          HID_LOGICAL_MIN ( 0x81                                   ), \
          HID_LOGICAL_MAX ( 0x7f                                   ), \
          HID_REPORT_COUNT( 1                                      ), \
          HID_REPORT_SIZE ( 8                                      ), \
          HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_RELATIVE ), \
      HID_COLLECTION_END                                            , \
    HID_COLLECTION_END \

    static const uint8_t abs_mouse_report_descriptor[] = {
        TUD_HID_REPORT_DESC_ABSMOUSE(HID_REPORT_ID(HID_REPORT_ID_MOUSE))
    };


    // Now the absolute mouse mouse implementation, keep it minimalistic for
    // the meantime since an official AbsMouse implementation is on the way.
    class USBHIDAbsMouse: public USBHIDDevice
    {
    private:
        USBHID hid;
    public:
        USBHIDAbsMouse(void);
        void begin(void);
        uint16_t _onGetDescriptor(uint8_t* buffer);
        bool sendReport(hid_abs_mouse_report_t report);
        void end();
    };


    USBHIDAbsMouse::USBHIDAbsMouse(): hid() {
        static bool initialized = false;
        if(!initialized){
            initialized = true;
            hid.addDevice(this, sizeof(abs_mouse_report_descriptor));
        }
    }

    void USBHIDAbsMouse::begin(void)
    {
        hid.begin();
    }

    uint16_t USBHIDAbsMouse::_onGetDescriptor(uint8_t* buffer)
    {
        memcpy(buffer, abs_mouse_report_descriptor, sizeof(abs_mouse_report_descriptor));
        return sizeof(abs_mouse_report_descriptor);
    }

    bool USBHIDAbsMouse::sendReport(hid_abs_mouse_report_t report)
    {
        return hid.SendReport( HID_REPORT_ID_MOUSE, &report, sizeof(hid_abs_mouse_report_t) );
    }

    void USBHIDAbsMouse::end()
    {
        hid.end();
    }


    typedef USBHIDAbsMouse USBHIDAbsoluteMouse;


#endif // !defined TUD_HID_REPORT_DESC_ABSMOUSE
