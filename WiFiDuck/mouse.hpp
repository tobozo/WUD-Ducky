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

#include "../USBDevices/USBDevices.h"

// 1bit XBM image struct for mouse drawing
typedef struct
{
  uint32_t width;
  uint32_t height;
  uint32_t len;
  uint32_t rowlen;
  const uint8_t* bytes;
} xbmImage_t;


// GFX class to draw with the mouse

class GfxMouse
{

public:

  GfxMouse( USBHIDAbsoluteMouse *_AbsMouse, uint32_t w=1920, uint32_t h=1080 );
  int32_t screen_width;
  int32_t screen_height;
  uint32_t delayafter = 20; // ms wait after each report
  uint32_t timeout = 1000; // ms max wait for HID.ready()

  // the display width/height is used to translate absolute coordinates to values understood by the mouse driver (0-32767)
  void setDisplay( uint32_t w, uint32_t h );
  // where x and y are absolute coordinates (in pixels) on the display
  void setRealCoords( int32_t x, int32_t y );
  void sendDoubleClick( uint8_t buttons_mask, uint32_t pushed_delay, uint32_t released_delay );
  // relative move on the X axis
  void moveXrel( int8_t x );
  // relative move on the Y axis
  void moveYrel( int8_t y );
  // absolute move on the X axis
  void moveXabs( int32_t x );
  // absolute move on the Y axis
  void moveYabs( int32_t y );
  // relative move on the X+Y axis with button state
  void moveXYrel( int8_t x, int8_t y, uint8_t buttons_mask );
  // absolute move on the X+Y axis with button state
  void moveXYabs( int32_t x, int32_t y, uint8_t buttons_mask );
  // specific button state getter
  bool isPressed(uint8_t b);
  // release specific button state, call before sendReport()
  void releaseButton(uint8_t b);
  // change specific button state, call before sendReport()
  void setButton(uint8_t b);
  // change buttons state, call before sendReport()
  void setButtons( uint8_t buttons_mask );
  void sendButtons( uint8_t buttons_mask );
  void sendReport( int x, int y, uint8_t buttons_mask );
  void sendReport();
  hid_abs_mouse_report_t *getMouseReport();
  uint8_t getButtons();
  void drawLine(int x0, int y0, int x1, int y1, uint8_t buttons_mask );
  void drawXbm( xbmImage_t* xbmImage, int32_t startx, int32_t starty );
  //   void drawCircle(int x, int y, int radius ) { }
  //   void fillCircle(int x, int y, int radius ) { }
  //   void drawRect(int x, int y, int width, int height ) { }
  //   void fillRect(int x, int y, int width, int height ) { }

private:
  USBHIDAbsoluteMouse *AbsMouse;
  hid_abs_mouse_report_t MouseReport;
  int32_t _lastx;
  int32_t _lasty;
  uint8_t _buttons_mask;

};

static GfxMouse *MouseGFX = nullptr;

