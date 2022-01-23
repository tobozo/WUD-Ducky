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

  GfxMouse( HIDAbsMouse *_AbsMouse, uint32_t w=1920, uint32_t h=1080 );
  int32_t screen_width;
  int32_t screen_height;
  uint32_t delayafter = 20; // ms wait after each report

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
  abs_mouse_report_t *getMouseReport();
  uint8_t getButtons();
  void drawLine(int x0, int y0, int x1, int y1, uint8_t buttons_mask );
  void drawXbm( xbmImage_t* xbmImage, int32_t startx, int32_t starty );
  //   void drawCircle(int x, int y, int radius ) { }
  //   void fillCircle(int x, int y, int radius ) { }
  //   void drawRect(int x, int y, int width, int height ) { }
  //   void fillRect(int x, int y, int width, int height ) { }

private:
  HIDAbsMouse *AbsMouse;
  abs_mouse_report_t MouseReport;
  int32_t _lastx;
  int32_t _lasty;
  uint8_t _buttons_mask;

};

static GfxMouse *MouseGFX = nullptr;

