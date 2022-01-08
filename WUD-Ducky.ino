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
 * Inspired by:
 *   - https://github.com/whid-injector/WHID
 *   - https://github.com/spacehuhn/wifi_ducky
 *
 * Could not exist without:
 *   - https://github.com/chegewara/EspTinyUSB
 *
\*/

// Config SD Card
#define SD_MISO  37
#define SD_MOSI  35
#define SD_SCK   36
#define SD_CS    34

// Config SSID and password for access point
const char* SSID        = "WUD-01";
const char* PASSWORD    = "12345678";

#define DEBUG
#include <vector>
#include "config_check.h"

#include "USBDevices/USBPendrive.h"
#include "USBDevices/USBAbsMouse.h"
#include "USBDevices/USBKeyboard.h"
#include "webserver.h"
#include "payload_runner.h"

USBHID HID;
HIDAbsMouse AbsMouse( &HID );
USBHIDKeyboard Keyboard;
USBMSC MSC;
payloadRunner PayloadRunner;

fs::FS* duckyFS = nullptr;
static GfxMouse *MouseGFX = nullptr;
std::vector<String> dmesg;
typedef void(*logprintercb)( String msg );


void logmsg( String msg )
{
  // TODO: format date and prefix message
  dmesg.push_back( msg );
}


void printdmesg( logprintercb logprint, bool clear_after=true )
{
  if( dmesg.size() > 0 ) {
    for( int i=0; i<dmesg.size(); i++ ) {
      logprint( dmesg[i] );
    }
    if( clear_after ) dmesg.clear();
  }
}


void logsprintf( const char *fmt, ... )
{
  char buf[256];
  va_list va;
  va_start (va, (char*)fmt);
  vsnprintf (buf, 256, fmt, va);
  va_end (va);
  logmsg( String( buf ) );
}


void serialprintln(String msg)
{
  Serial.println( msg );
}


static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT: dmesg.push_back("USB PLUGGED"); break;
      case ARDUINO_USB_STOPPED_EVENT: dmesg.push_back("USB UNPLUGGED"); break;
      case ARDUINO_USB_SUSPEND_EVENT: dmesg.push_back( String( "USB SUSPENDED: remote_wakeup_en: " + String( data->suspend.remote_wakeup_en ) ) ); break;
      case ARDUINO_USB_RESUME_EVENT: dmesg.push_back("USB RESUMED"); break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_CDC_EVENTS) {
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT: dmesg.push_back("CDC CONNECTED"); break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT: dmesg.push_back("CDC DISCONNECTED"); break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT: logsprintf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts); break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT: logsprintf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity); break;
      case ARDUINO_USB_CDC_RX_EVENT: /* USBSerial.read(buf, data->rx.len); */ break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_HID_EVENTS){
    arduino_usb_hid_event_data_t * data = (arduino_usb_hid_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_HID_SET_PROTOCOL_EVENT: logsprintf("HID SET PROTOCOL: %s\n", data->set_protocol.protocol?"REPORT":"BOOT"); break;
      case ARDUINO_USB_HID_SET_IDLE_EVENT: logsprintf("HID SET IDLE: %u\n", data->set_idle.idle_rate); break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_HID_KEYBOARD_EVENTS){
    arduino_usb_hid_keyboard_event_data_t * data = (arduino_usb_hid_keyboard_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_HID_KEYBOARD_LED_EVENT: logsprintf("HID KEYBOARD LED: NumLock:%u, CapsLock:%u, ScrollLock:%u\n", data->numlock, data->capslock, data->scrolllock); break;
      default: break;
    }
  }
}


// some USB-dependant callbacks to attach

PayloadCharCb typewriter = [](char c, uint32_t wait )
{
  Keyboard.press(c);
  vTaskDelay(wait/2);
  Keyboard.releaseAll();
  vTaskDelay(wait/2);
};


void HIDPrintln( String str )
{
  for( int i=0; i<str.length(); i++ ) {
    typewriter( str[i], PayloadRunner.keyDelay );
  }
  typewriter( '\n', PayloadRunner.keyDelay );
}


void MouseDrawer( char* chunk )
{
  uint8_t mouse_btn_mask = (uint8_t)atoi( &chunk[1] );
  int8_t  move8  = (int8_t)atoi( &chunk[1] );
  int32_t move32 = (int32_t)atoi( &chunk[1] );
  switch( chunk[0] ) {
    case 'x': // absolute move X axis
      MouseGFX->moveXabs( move32 );
    break;
    case 'y': // absolute move Y
      MouseGFX->moveYabs( move32 );
    break;
    case 'X': // relative move X axis
      MouseGFX->moveXrel( move8 );
    break;
    case 'Y': // relative move Y axis
      MouseGFX->moveYrel( move8 );
    break;
    case 'C': // click/unclick
      MouseGFX->moveXYrel( 0, 0, mouse_btn_mask );
    break;
    default: // ignore
    break;
  }
}


// WUD-Ducky functionalities as Ducky Commands in a lambda functions array
// A few are still missing, such as ESP-Restart, AP controls, debug, etc
DuckyCommand DuckyCommands[] =
{
  //
  {"Rem", [](){ PayloadRunner.DelayLength = 0; } },
  {"SerialBegin", [](){ Serial.begin(115200); Serial.setDebugOutput(true); } },
  {"InitWiFi", [](){ WiFi.mode(WIFI_AP); WiFi.softAP(SSID, PASSWORD);  } },
  {"InitSPIFFS", [](){ if( !SPIFFS.begin() ) dmesg.push_back("SPIFFS failed!"); } },
  {"InitSD", [](){ if(initSD(SD_SCK, SD_MISO, SD_MOSI, SD_CS)) dmesg.push_back("SD started successfully"); else { dmesg.push_back("Error, PenDrive disabled"); duckyFS = &SPIFFS; }} },
  {"InitPenDrive", [](){ if(initPenDrive(&MSC)) { dmesg.push_back("USB PenDrive started successfully"); duckyFS = &SD; } else  dmesg.push_back("USB PenDrive failed to start"); } },
  {"InitKeyboard", [](){ Keyboard.begin(); } },
  {"InitMouse", [](){ AbsMouse.begin(); MouseGFX = new GfxMouse( &AbsMouse ); MouseGFX->setDisplay( 1920*2, 1080 ); } },
  {"StartUSB", [](){ if( !USB.begin() ) dmesg.push_back("USB Failed to start"); else { while( !HID.ready() ) vTaskDelay(1);  }; } },
  {"StartWebServer", [](){ startWebServer(); } },
  {"StopSerial", [](){ Serial.end(); } },
  {"StopWiFi", [](){ WiFi.mode(WIFI_OFF); } },
  {"StopSD", [](){ deinitSD(); } },
  {"StopPenDrive", [](){  deinitPenDrive(&MSC); } },
  {"StopKeyboard", [](){ Keyboard.end(); } },
  {"StopMouse", [](){ AbsMouse.end(); } },
  {"DefaultDelay", [](){ PayloadRunner.DelayLength = 1500; PayloadRunner.defaultdelay = PayloadRunner.cmdinput.toInt(); } },
  {"CustomDelay", [](){ PayloadRunner.custom_delay = PayloadRunner.cmdinput.toInt(); PayloadRunner.DelayLength = PayloadRunner.custom_delay; } },
  {"KeyDelay", [](){ PayloadRunner.DelayLength = 0; PayloadRunner.keyDelay = atoi(PayloadRunner.cmdinput.c_str()); } },
  {"Key", [](){ int key = atoi(PayloadRunner.cmdinput.c_str()); vTaskDelay(25); Keyboard.pressRaw(key); vTaskDelay(25); Keyboard.releaseAll(); } },
  {"DrawSpaceInvaders", [](){ MouseGFX->drawXbm( &Alien_128x64, 900, 500 ); } },
  {"logs", [](){ printdmesg( serialprintln ); } },
  {"MouseMoveUp", [](){   int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveYrel( -mousemoveamt ); } },
  {"MouseMoveDown", [](){ int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveYrel( mousemoveamt  ); } },
  {"MouseMoveLeft", [](){ int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveXrel( -mousemoveamt ); } },
  {"MouseMoveRight", [](){ int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveXrel( mousemoveamt );} },
  {"MouseClickRelease", [](){ MouseGFX->moveXYrel( 0, 0, 0 ); vTaskDelay(25);} },
  {"MouseClickRIGHT", [](){ MouseGFX->moveXYrel( 0, 0, MOUSE_RIGHT_BTN );vTaskDelay(25); } },
  {"MouseClickLEFT", [](){ MouseGFX->moveXYrel( 0, 0, MOUSE_LEFT_BTN );vTaskDelay(25); } },
  {"MouseClickMIDDLE", [](){ MouseGFX->moveXYrel( 0, 0, MOUSE_MIDDLE_BTN );vTaskDelay(25); } },
  {"MouseDoubleClickLEFT", [](){ MouseGFX->sendDoubleClick( MOUSE_LEFT_BTN, 100, 100 ); }},
  {"MouseMoveToCoords", []() { int x, y; int *coords[2] = {&x,&y}; if( PayloadRunner.getNumericArgs(coords, 2) ) { MouseGFX->moveXYabs( x, y, MouseGFX->getButtons() ); } } },
  {"Print", [](){ PayloadRunner.readChars( typewriter, PayloadRunner.keyDelay ); } },
  {"PrintLine", [](){ PayloadRunner.readChars( typewriter, PayloadRunner.keyDelay ); vTaskDelay(250); Keyboard.pressRaw(HID_KEY_ENTER); vTaskDelay(25); Keyboard.releaseAll(); } },
  {"SetDisplay", [](){ int w, h; int *resolution[2] = {&w,&h}; if( PayloadRunner.getNumericArgs(resolution, 2) ) { MouseGFX->setDisplay( w, h ); } } },
  {"MouseDrawStr", [](){ PayloadRunner.readDelimited(MouseDrawer); } },
  {"Press", [](){
      PayloadRunner.keypressdone=1;
      String fullkeys = PayloadRunner.cmdinput;
      int str_len = fullkeys.length()+1;
      char keyarray[str_len];
      fullkeys.toCharArray(keyarray, str_len);
      PayloadRunner.keypart = strtok(keyarray, PayloadRunner.plusdelimiter);
      while(PayloadRunner.keypart != NULL) {
        int key = atoi(PayloadRunner.keypart);
        vTaskDelay(25);
        Keyboard.pressRaw(key);
        PayloadRunner.keypart = strtok(NULL, PayloadRunner.plusdelimiter);
      }
      if(PayloadRunner.keypressdone==1) {
        vTaskDelay(25);
        Keyboard.releaseAll();
        PayloadRunner.keypressdone=0;
      }
    }
  },
  //{"StopUSB", [](){ } },
  //{"StopWebServer", [](){  } },
  //{"SetWiFiMode", [](){  } },
  //{"SetWiFiSSID", [](){  } },
  //{"SetWiFiPass", [](){  } },
};



void runpayload( fs::FS *fs, const char* payload)
{
  if( !fs->exists( payload ) ) return;
  fs::File f = fs->open(payload, "r");
  if( !f ) return;
  PayloadRunner.defaultdelay = PayloadRunner.DelayLength; // memoize delay settings
  PayloadRunner.settingsdefaultdelay = PayloadRunner.DelayLength;
  while(f.available()) {
    String line = f.readStringUntil('\n');
    PayloadRunner.runPayload( line );
  }
  f.close();
  PayloadRunner.DelayLength = PayloadRunner.settingsdefaultdelay; // restore delay settings
}



void setup()
{
  USB.onEvent(usbEventCallback);
  Keyboard.onEvent(usbEventCallback);
  HID.onEvent(usbEventCallback);
  // attach loggers to USB items, messages are deferred for later viewing with ducky "logs" command
  USBPendriveLogger = logmsg;
  WebServerLogger = logmsg;
  MouseLogger = logmsg;
  WebServerLogsPrinter = printdmesg; // web server can deliver logs
  HIDKeySender = HIDPrintln;
  // attach ducky commands to the payload runner
  PayloadRunner.setDuckyCommands( DuckyCommands, sizeof( DuckyCommands ) / sizeof( DuckyCommand ) );
  // use ducky payloads to run this skech :-)
  PayloadRunner.runPayload( "SerialBegin" );
  PayloadRunner.runPayload( "InitWiFi" );
  PayloadRunner.runPayload( "InitSD" );
  PayloadRunner.runPayload( "InitPenDrive" );
  PayloadRunner.runPayload( "InitKeyboard" );
  PayloadRunner.runPayload( "InitMouse" );
  PayloadRunner.runPayload( "StartUSB" );
  vTaskDelay(5000); // let the SD be detected by the host OS before running the webserver
  PayloadRunner.runPayload( "InitSPIFFS" );
  PayloadRunner.runPayload( "StartWebServer" );
}


void loop()
{
  server.handleClient();
  if( Serial.available() )
  {
    String line = Serial.readStringUntil('\n');
    if( line == "help" ) {
      Serial.println("Available Ducky commands:");
      for( int i=0;i<PayloadRunner.commands_count;i++ ) {
        Serial.printf("  - %s\n", PayloadRunner.commands[i].name );
      }
      return;
    }
    log_d("Will handle payload: %s\n", line.c_str() );
    PayloadRunner.runPayload( line );
  }
  vTaskDelay(2);
}

