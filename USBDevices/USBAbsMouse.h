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

#define HAS_ABSMOUSE

#define MOUSE_LEFT_BTN    0x01
#define MOUSE_RIGHT_BTN   0x02
#define MOUSE_MIDDLE_BTN  0x03
#define MOUSE_BACK_BTN    0x04
#define MOUSE_FORWARD_BTN 0x05

#include "USBConfig.h"
#include "USBHID.h"

void (*MouseLogger)( String err );


static uint8_t _report_id = 0x05;

// we use a custom HID report descriptor with absolute mouse positioning
static const uint8_t abs_mouse_report_descriptor[] = {
  0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,           // USAGE (Mouse)
  0xa1, 0x01,           // COLLECTION (Application)
  0x09, 0x01,           //   USAGE (Pointer)
  0xA1, 0x00,           //   COLLECTION (Physical)
  0x85, _report_id,     //     REPORT_ID (1)
  0x05, 0x09,           //     USAGE_PAGE (Button)
  0x19, 0x01,           //     USAGE_MINIMUM (1)
  0x29, 0x03,           //     USAGE_MAXIMUM (3)
  0x15, 0x00,           //     LOGICAL_MINIMUM (0)
  0x25, 0x01,           //     LOGICAL_MAXIMUM (1)
  0x95, 0x03,           //     REPORT_COUNT (3)
  0x75, 0x01,           //     REPORT_SIZE (1)
  0x81, 0x02,           //     INPUT (Data,Var,Abs)
  0x95, 0x01,           //     REPORT_COUNT (1)
  0x75, 0x05,           //     REPORT_SIZE (5)
  0x81, 0x03,           //     INPUT (Const,Var,Abs)
  0x05, 0x01,           //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,           //     USAGE (X)
  0x09, 0x31,           //     USAGE (Y)
  0x16, 0x00, 0x00,     //     LOGICAL_MINIMUM(0)
  0x26, 0xff, 0x7f,     //     LOGICAL_MAXIMUM(32767)
  0x75, 0x10,           //     REPORT_SIZE (16)
  0x95, 0x02,           //     REPORT_COUNT (2)
  0x81, 0x02,           //     INPUT (Data,Var,Abs)
  0xC0,                 //   END_COLLECTION
  0xC0,                 // END COLLECTION
};

typedef struct TU_ATTR_PACKED
{
  uint8_t buttons = 0;
  int16_t x = 0;
  int16_t y = 0;
} abs_mouse_report_t;


class HIDAbsMouse: public USBHIDDevice
{

public:

  HIDAbsMouse(USBHID *_HID)
  {
    static bool initialized = false;
    HID = _HID;
    if(!initialized){
      initialized = true;
      HID->addDevice(this, sizeof(abs_mouse_report_descriptor));
    }
  }

  void begin(void)
  {
    if( absmouse_begun ) return;
    HID->begin();
    absmouse_begun = true;
  }

  uint16_t _onGetDescriptor(uint8_t* buffer)
  {
    memcpy(buffer, abs_mouse_report_descriptor, sizeof(abs_mouse_report_descriptor));
    return sizeof(abs_mouse_report_descriptor);
  }

  bool send(abs_mouse_report_t * value)
  {
    while(!HID->ready() ) vTaskDelay(1);
    return HID->SendReport( _report_id, value, sizeof(abs_mouse_report_t) );
  }

  void end()
  {
    HID->end();
    absmouse_begun = false;
  }

private:
  USBHID *HID;

};

