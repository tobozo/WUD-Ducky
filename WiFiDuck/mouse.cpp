#pragma once

#include "mouse.hpp"


GfxMouse::GfxMouse( HIDAbsMouse *_AbsMouse, uint32_t w, uint32_t h )
{
  AbsMouse = _AbsMouse;
  screen_width = w;
  screen_height = h;
};

// the display width/height is used to translate absolute coordinates to values understood by the mouse driver (0-32767)
void GfxMouse::setDisplay( uint32_t w, uint32_t h )
{
  screen_width = w;
  screen_height = h;
  if( MouseLogger ) MouseLogger( String( "Mouse moves will translate to ["+String(w)+"*"+String(h)+"]" ) );
  sendReport( screen_width/2, screen_height/2, 0 );
}

// where x and y are absolute coordinates (in pixels) on the display
void GfxMouse::setRealCoords( int32_t x, int32_t y )
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

void GfxMouse::sendDoubleClick( uint8_t buttons_mask, uint32_t pushed, uint32_t released )
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
void GfxMouse::moveXrel( int8_t x )
{
  sendReport( _lastx+x, _lasty, _buttons_mask );
}

// relative move on the Y axis
void GfxMouse::moveYrel( int8_t y )
{
  sendReport( _lastx, _lasty+y, _buttons_mask );
}

// absolute move on the X axis
void GfxMouse::moveXabs( int32_t x )
{
  sendReport( x, _lasty, _buttons_mask );
}

// absolute move on the Y axis
void GfxMouse::moveYabs( int32_t y )
{
  sendReport( _lastx, y, _buttons_mask );
}

// relative move on the X+Y axis with button state
void GfxMouse::moveXYrel( int8_t x, int8_t y, uint8_t buttons_mask )
{
  sendReport( _lastx+x, _lasty+y, buttons_mask );
}

// absolute move on the X+Y axis with button state
void GfxMouse::moveXYabs( int32_t x, int32_t y, uint8_t buttons_mask )
{
  sendReport( x, y, buttons_mask );
}

// specific button state getter
bool GfxMouse::isPressed(uint8_t b)
{
  if ((b & _buttons_mask) > 0) {
    return true;
  }
  return false;
}

// release specific button state, call before sendReport()
void GfxMouse::releaseButton(uint8_t b)
{
  setButtons(_buttons_mask & ~b);
}

// change specific button state, call before sendReport()
void GfxMouse::setButton(uint8_t b)
{
  setButtons(_buttons_mask | b);
}

// change buttons state, call before sendReport()
void GfxMouse::setButtons( uint8_t buttons_mask )
{
  MouseReport.buttons = buttons_mask;
  _buttons_mask = buttons_mask;
}

void GfxMouse::sendButtons( uint8_t buttons_mask )
{
  setButtons( buttons_mask );
  sendReport();
}

void GfxMouse::sendReport( int x, int y, uint8_t buttons_mask )
{
  setButtons( buttons_mask );
  setRealCoords( x, y );
  sendReport();
  if( _buttons_mask != buttons_mask ) {
    _buttons_mask = buttons_mask;
    vTaskDelay(delayafter);
  }
}

void GfxMouse::sendReport()
{
  if( absmouse_begun ) AbsMouse->send(&MouseReport);
}

abs_mouse_report_t *GfxMouse::getMouseReport()
{
  return &MouseReport;
}

uint8_t GfxMouse::getButtons()
{
  return _buttons_mask;
}

void GfxMouse::drawLine(int x0, int y0, int x1, int y1, uint8_t buttons_mask )
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

void GfxMouse::drawXbm( xbmImage_t* xbmImage, int32_t startx, int32_t starty )
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

//   void GfxMouse::drawCircle(int x, int y, int radius ) { }
//   void GfxMouse::fillCircle(int x, int y, int radius ) { }
//   void GfxMouse::drawRect(int x, int y, int width, int height ) { }
//   void GfxMouse::fillRect(int x, int y, int width, int height ) { }

