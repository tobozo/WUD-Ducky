#pragma once

#define MOUSE_LEFT_BTN    0x01
#define MOUSE_RIGHT_BTN   0x02
#define MOUSE_MIDDLE_BTN  0x03
#define MOUSE_BACK_BTN    0x04
#define MOUSE_FORWARD_BTN 0x05


#include "USB.h"
#include "USBHID.h"

// we use a custom HID report descriptor with absolute mouse positioning
static const uint8_t report_descriptor[] PROGMEM = {
  0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,           // USAGE (Mouse)
  0xa1, 0x01,           // COLLECTION (Application)
  0x09, 0x01,           //   USAGE (Pointer)
  0xA1, 0x00,           //   COLLECTION (Physical)
  0x85, 0x01,           //     REPORT_ID (1)
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

USBHID HID;

class HIDAbsMouse: public USBHIDDevice {
public:
  HIDAbsMouse(void){
    static bool initialized = false;
    if(!initialized){
      initialized = true;
      HID.addDevice(this, sizeof(report_descriptor));
    }
  }

  void begin(void){
    HID.begin();
  }

  uint16_t _onGetDescriptor(uint8_t* buffer){
    memcpy(buffer, report_descriptor, sizeof(report_descriptor));
    return sizeof(report_descriptor);
  }

  bool send(abs_mouse_report_t * value){
    return HID.SendReport( 1, value, sizeof(abs_mouse_report_t) );
  }

};



#include "alien.xbm.h"


typedef struct {
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


class GfxMouse
{

public:

  GfxMouse( HIDAbsMouse *_AbsMouse, uint32_t w=1920, uint32_t h=1080 )
  {
    AbsMouse = _AbsMouse;
    setDisplay( w, h );
  };

  bool button_pushed = false;

  int32_t screen_width;
  int32_t screen_height;
  uint32_t delayafter = 20; // ms wait after each report

  void setDisplay( uint32_t w, uint32_t h )
  {
    screen_width = w;
    screen_height = h;
    if( MouseLogger ) MouseLogger( String( "Mouse moves will translate to ["+String(w)+"*"+String(h)+"]" ) );
    setRealCoords( screen_width/2, screen_height/2 );
    MouseReport.buttons = 0;
    AbsMouse->send(&MouseReport);
  }

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

  void moveX( int x )
  {
    moveRelative( x, 0, button_pushed );
  }

  void moveY( int y )
  {
    moveRelative( 0, y, button_pushed );
  }

  void moveRelative( int x, int y, int button )
  {
    bool is_pushed = isPushed( button );
    setRealCoords( _lastx+x, _lasty+y );
    if( is_pushed ) AbsMouse->send(&MouseReport);
    vTaskDelay(delayafter);
  }

  void moveTo( int x, int y, int button )
  {
    bool is_pushed = isPushed( button );
    setRealCoords( x, y );
    if( is_pushed ) AbsMouse->send(&MouseReport);
    vTaskDelay(delayafter);
  }

  bool isPushed( int button )
  {
    bool is_pushed = (button>0);
    if( button_pushed != is_pushed ) {
      MouseReport.buttons = is_pushed ? MOUSE_LEFT_BTN : 0;
      AbsMouse->send(&MouseReport);
      button_pushed = is_pushed;
      //Serial.printf("New left button state: %s\n", is_pushed ? "pushed":"released");
      vTaskDelay(delayafter);
    }
    return is_pushed;
  }

  abs_mouse_report_t *getMouseReport()
  {
    return &MouseReport;
  }

  void drawLine(int x0, int y0, int x1, int y1, int button)
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

    MouseReport.buttons = MOUSE_LEFT_BTN;

    int dx = x1-x0;
    int dy = y1-y0;
    float derror = std::abs(dy/float(dx));
    float error = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
      if (steep) {
        setRealCoords( y, x );
        isPushed( button );
        AbsMouse->send(&MouseReport);
        vTaskDelay(delayafter);
      } else {
        setRealCoords( x, y );
        isPushed( button );
        AbsMouse->send(&MouseReport);
        vTaskDelay(delayafter);
      }
      error += derror;
      if (error>.5) {
        y += (y1>y0?1:-1);
        error -= 1.;
      }
    }

    MouseReport.buttons = 0;
  }

  void drawXbm( xbmImage_t* xbmImage, int32_t startx, int32_t starty )
  {
    setRealCoords( startx, starty );
    MouseReport.buttons = 0;
    for( int i=0; i<xbmImage->len; i++ ) {
      uint8_t block = xbmImage->bytes[i];
      int32_t x = (i%xbmImage->rowlen)*8;
      int32_t y = i/xbmImage->rowlen;
      for( uint8_t a=0; a<8; a++ ) {
        if( x+a > xbmImage->width ) {
          x = 0;
          y++;
        }

        bool is_pushed = (block & (1<<a));
        if( button_pushed != is_pushed ) {
          MouseReport.buttons = is_pushed ? MOUSE_LEFT_BTN : 0;
          AbsMouse->send(&MouseReport);
          button_pushed = is_pushed;
          vTaskDelay(25);
        }
        setRealCoords( startx+x+a, starty+y );
        if( is_pushed ) {
          AbsMouse->send(&MouseReport);
          vTaskDelay(10);
        }
      }
      vTaskDelay(1);
    }
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

};

