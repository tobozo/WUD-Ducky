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

#define MOUSE_LEFT_BTN    0x01
#define MOUSE_RIGHT_BTN   0x02
#define MOUSE_MIDDLE_BTN  0x03
#define MOUSE_BACK_BTN    0x04
#define MOUSE_FORWARD_BTN 0x05


#include "USB.h"
#include "USBHID.h"


// some easter egg to test the mouse in absolute positioning mode

#include "../xbm/alien.xbm.h"

typedef struct
{
  uint32_t width;
  uint32_t height;
  uint32_t len;
  uint32_t rowlen;
  const uint8_t* bytes;
} xbmImage_t;

xbmImage_t Alien_128x64 =
{
  alien_width,
  alien_height,
  alien_bytes,
  alien_bytes_per_row,
  Alien_128x64_bits
};


void (*MouseLogger)( String err );


static uint8_t _report_id = 0x05;

// we use a custom HID report descriptor with absolute mouse positioning
static const uint8_t report_descriptor[] PROGMEM = {
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


bool absmouse_begun = false;

class HIDAbsMouse: public USBHIDDevice
{

public:

  HIDAbsMouse(USBHID *_HID)
  {
    static bool initialized = false;
    HID = _HID;
    if(!initialized){
      initialized = true;
      HID->addDevice(this, sizeof(report_descriptor));
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
    memcpy(buffer, report_descriptor, sizeof(report_descriptor));
    return sizeof(report_descriptor);
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

// GFX class to draw with the mouse

class GfxMouse
{

public:

  GfxMouse( HIDAbsMouse *_AbsMouse, uint32_t w=1920, uint32_t h=1080 )
  {
    AbsMouse = _AbsMouse;
    setDisplay( w, h );
  };

  int32_t screen_width;
  int32_t screen_height;
  uint32_t delayafter = 20; // ms wait after each report

  // the display width/height is used to translate absolute coordinates to values understood by the mouse driver (0-32767)
  void setDisplay( uint32_t w, uint32_t h )
  {
    screen_width = w;
    screen_height = h;
    if( MouseLogger ) MouseLogger( String( "Mouse moves will translate to ["+String(w)+"*"+String(h)+"]" ) );
    sendReport( screen_width/2, screen_height/2, 0 );
  }

  // where x and y are absolute coordinates (in pixels) on the display
  void setRealCoords( int32_t x, int32_t y )
  {
    // constrain coords to screen width/height
    x = max(0, min(screen_width-1, x));
    y = max(0, min(screen_height-1, y));
    // store position
    _lastx = x;
    _lasty = y;
    // map coords to the display
    int32_t px = map( x, 0, screen_width-1, 0, 32767 );
    int32_t py = map( y, 0, screen_height-1, 0, 32767 );
    MouseReport.x = px;
    MouseReport.y = py;
  }

  void sendDoubleClick( uint8_t buttons_mask, uint32_t pushed, uint32_t released )
  {
    sendButtons( buttons_mask );
    delay( pushed );
    sendButtons( 0 );
    delay( released );
    sendButtons( buttons_mask );
    delay( pushed );
    sendButtons( 0 );
    delay( released );
  }

  // relative move on the X axis
  void moveXrel( int8_t x )
  {
    sendReport( _lastx+x, _lasty, _lastbtnmask );
  }

  // relative move on the Y axis
  void moveYrel( int8_t y )
  {
    sendReport( _lastx, _lasty+y, _lastbtnmask );
  }

  // absolute move on the X axis
  void moveXabs( int32_t x )
  {
    sendReport( x, _lasty, _lastbtnmask );
  }

  // absolute move on the Y axis
  void moveYabs( int32_t y )
  {
    sendReport( _lastx, y, _lastbtnmask );
  }

  // relative move on the X+Y axis with button state
  void moveXYrel( int8_t x, int8_t y, uint8_t buttons_mask )
  {
    sendReport( _lastx+x, _lasty+y, buttons_mask );
  }

  // relative move on the X+Y axis with button state
  void moveXYabs( int32_t x, int32_t y, uint8_t buttons_mask )
  {
    sendReport( x, y, buttons_mask );
  }

  // absolute move on the X+Y axis with button state
  /*
  void moveTo( int x, int y, uint8_t buttons_mask )
  {
    sendReport( x, y, buttons_mask );
  }*/

  abs_mouse_report_t *getMouseReport()
  {
    return &MouseReport;
  }

  // change buttons state without sending report
  void setButtons( uint8_t buttons_mask )
  {
    MouseReport.buttons = buttons_mask;
    _lastbtnmask = buttons_mask;
  }

  uint8_t getButtons()
  {
    return _lastbtnmask;
  }

  void sendButtons( uint8_t buttons_mask )
  {
    setButtons( buttons_mask );
    sendReport();
  }

  void sendReport( int x, int y, uint8_t buttons_mask )
  {
    setButtons( buttons_mask );
    setRealCoords( x, y );
    sendReport();
    if( _lastbtnmask != buttons_mask ) {
      _lastbtnmask = buttons_mask;
      vTaskDelay(delayafter);
    }
  }

  void sendReport()
  {
    if( absmouse_begun ) AbsMouse->send(&MouseReport);
  }



  void drawLine(int x0, int y0, int x1, int y1, uint8_t buttons_mask )
  {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
      std::swap(x0, y0);
      std::swap(x1, y1);
      steep = true;
    }
    if (x0>x1) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }

    sendReport( x0, y0, 0 );
    setButtons( buttons_mask );

    int dx = x1-x0;
    int dy = y1-y0;
    float derror = std::abs(dy/float(dx));
    float error = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
      if (steep) {
        sendReport( y, x, buttons_mask );
      } else {
        sendReport( x, y, buttons_mask );
      }
      error += derror;
      if (error>.5) {
        y += (y1>y0?1:-1);
        error -= 1.;
      }
    }
    sendReport( _lastx, _lasty, 0 );
  }

  void drawXbm( xbmImage_t* xbmImage, int32_t startx, int32_t starty )
  {
    sendReport( startx, starty, 0 );
    for( int i=0; i<xbmImage->len; i++ ) {
      uint8_t block = xbmImage->bytes[i];
      int32_t x = (i%xbmImage->rowlen)*8;
      int32_t y = i/xbmImage->rowlen;
      for( uint8_t a=0; a<8; a++ ) {
        if( x+a > xbmImage->width ) {
          x = 0;
          y++;
        }
        sendReport( startx+x+a, starty+y, (block & (1<<a)) ? MOUSE_LEFT_BTN : 0 );
      }
      delay(delayafter);
    }
    sendReport( _lastx, _lasty, 0 );
  }

  //   void drawCircle(int x, int y, int radius ) { }
  //   void fillCircle(int x, int y, int radius ) { }
  //   void drawRect(int x, int y, int width, int height ) { }
  //   void fillRect(int x, int y, int width, int height ) { }


private:
  HIDAbsMouse *AbsMouse;
  abs_mouse_report_t MouseReport;
  int32_t _lastx;
  int32_t _lasty;
  uint8_t _lastbtnmask;

};

