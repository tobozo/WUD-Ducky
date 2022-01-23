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
 *   - https://github.com/spacehuhntech/WiFiDuck (AKA https://github.com/spacehuhn/wifi_ducky)
 *
 * Could not exist without:
 *   - https://github.com/chegewara/EspTinyUSB
 *
\*/


// Config SSID and password for access point
const char* SSID        = "WUD-01";
const char* PASSWORD    = "12345678";

#define DEBUG
#include <vector>
#include "config_check.h"
#include <SD.h>
#include "WiFiDuck/duckparser.cpp" // SpaceHuhn's WiFiDuck implementation, with some mouse tweaks
#include "webserver.h"
#include "xbm/alien.xbm.h"

// Cursing after that Serial0/Serial/USBCDC unintuitive example from the official USB library:
#if ARDUINO_USB_CDC_ON_BOOT
  #define HWSerial Serial0
  #define USBSerial Serial
#else
  #define HWSerial Serial
  USBCDC USBSerial;
#endif
// 1) Both Serial USB and UART can be considered hardware
// 2) Which of both Serial interfaces is relevant for debugging? your guess
// 3) Why use two Serial interfaces? because reasons
// 4) Also using defines for that is such a poor choice, C++ can do better than this


// some easter egg to test the mouse in absolute positioning mode
xbmImage_t Alien_128x64 =
{
  alien_width,
  alien_height,
  alien_bytes,
  alien_bytes_per_row,
  Alien_128x64_bits
};

// floating filesystem
fs::FS* duckyFS = nullptr;
// log messages and utilities
std::vector<String> dmesg;
typedef void(*logprintercb)( String msg );

bool serial_begun = false;


void logmsg( String msg )
{
  // format date and prefix message, and why strftime is useless
  char strftime_buf[64];
  uint32_t now = millis();
  uint32_t s = now/1000;
  uint32_t m = now/60000;
  uint32_t h = now/(60000*60);
  uint32_t S  = s%60;
  uint32_t M  = m%60;
  uint32_t H  = h%60;
  uint32_t SS = now%1000;
  sprintf(strftime_buf, "%02d:%02d:%02d.%03d", H, M, S, SS );
  msg = "[@" + String( strftime_buf) + "] " + msg;
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
      case ARDUINO_USB_STARTED_EVENT: logmsg("USB PLUGGED"); break;
      case ARDUINO_USB_STOPPED_EVENT: logmsg("USB UNPLUGGED"); break;
      case ARDUINO_USB_SUSPEND_EVENT: logmsg( String( "USB SUSPENDED: remote_wakeup_en: " + String( data->suspend.remote_wakeup_en ) ) ); break;
      case ARDUINO_USB_RESUME_EVENT: logmsg("USB RESUMED"); break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_CDC_EVENTS) {
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT: logmsg("CDC CONNECTED"); break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT: logmsg("CDC DISCONNECTED"); break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT: logsprintf("CDC LINE STATE: dtr: %u, rts: %u", data->line_state.dtr, data->line_state.rts); break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT: logsprintf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity); break;
      case ARDUINO_USB_CDC_RX_EVENT: /* USBSerial.read(buf, data->rx.len); */ break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_HID_EVENTS){
    arduino_usb_hid_event_data_t * data = (arduino_usb_hid_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_HID_SET_PROTOCOL_EVENT: logsprintf("HID SET PROTOCOL: %s", data->set_protocol.protocol?"REPORT":"BOOT"); break;
      case ARDUINO_USB_HID_SET_IDLE_EVENT: logsprintf("HID SET IDLE: %u", data->set_idle.idle_rate); break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_HID_KEYBOARD_EVENTS){
    arduino_usb_hid_keyboard_event_data_t * data = (arduino_usb_hid_keyboard_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_HID_KEYBOARD_LED_EVENT: logsprintf("HID KEYBOARD LED: NumLock:%u, CapsLock:%u, ScrollLock:%u", data->numlock, data->capslock, data->scrolllock); break;
      default: break;
    }
  }
}


static void WiFiEventCallback(WiFiEvent_t event)
{
  const char * arduino_event_names[] = {
      "WIFI_READY",
      "SCAN_DONE",
      "STA_START", "STA_STOP", "STA_CONNECTED", "STA_DISCONNECTED", "STA_AUTHMODE_CHANGE", "STA_GOT_IP", "STA_GOT_IP6", "STA_LOST_IP",
      "AP_START", "AP_STOP", "AP_STACONNECTED", "AP_STADISCONNECTED", "AP_STAIPASSIGNED", "AP_PROBEREQRECVED", "AP_GOT_IP6",
      "FTM_REPORT",
      "ETH_START", "ETH_STOP", "ETH_CONNECTED", "ETH_DISCONNECTED", "ETH_GOT_IP", "ETH_GOT_IP6",
      "WPS_ER_SUCCESS", "WPS_ER_FAILED", "WPS_ER_TIMEOUT", "WPS_ER_PIN", "WPS_ER_PBC_OVERLAP",
      "SC_SCAN_DONE", "SC_FOUND_CHANNEL", "SC_GOT_SSID_PSWD", "SC_SEND_ACK_DONE",
      "PROV_INIT", "PROV_DEINIT", "PROV_START", "PROV_END", "PROV_CRED_RECV", "PROV_CRED_FAIL", "PROV_CRED_SUCCESS"
  };
  logsprintf("[WiFi-event]: #%d (%s)", event, arduino_event_names[event]);
}



// some USB-dependant callbacks to attach

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
duckCommand DuckyCommands[] =
{
  {"SerialBegin", [](){
      Serial.begin(115200);
      serial_begun = true;
      //Serial.setDebugOutput(true);
      delay(5000);
    }
  },
  {"SerialDebug", [](){
      static bool debug_enabled = false;
      debug_enabled = !debug_enabled;
      Serial.setDebugOutput(debug_enabled); // doing this may cause instability, use only for debug and expect more bugs :-)
      logsprintf("setDebugOutput(%s)", debug_enabled?"true":"false");
    }
  },
  {"InitWiFi", [](){ WiFi.mode(WIFI_AP); WiFi.softAP(SSID, PASSWORD);  } },
  {"InitSPIFFS", [](){ if( !SPIFFS.begin() ) logmsg("SPIFFS failed!"); } },
#ifdef HAS_PENDRIVE
  {"InitSD", [](){ if( !initSD() ) duckyFS = &SPIFFS; else duckyFS = &SD; } },
  {"InitPenDrive", [](){ initPenDrive(); } },
  {"StopSD", [](){ deinitSD(); } },
  {"StopPenDrive", [](){  deinitPenDrive(); } },
#else // SD not exposed as pendrive, currently used for QA testing
  {"InitSD", [](){
      // Config SD Card
      #define SD_MISO  37
      #define SD_MOSI  35
      #define SD_SCK   36
      #define SD_CS    34

      static bool sd_begun = false;

      SPIClass * spi = = new SPIClass(FSPI);
      //spi = new SPIClass(HSPI);
      spi->setFrequency(4000000);

      spi->begin(SD_SCK, SD_MISO, SD_MOSI, -1);

      // trying random shit before calling sd.begin
      spi->transfer(0);
      spi->transfer(0);
      delay(100);

      uint8_t attempts = 0;
      uint8_t maxattempts = 10;

      // hammering the SD is necessary since SD.h was fucked up (2.0.2)
      do {
        sd_begun = SD.begin(SD_CS, *spi, 4000000);
        if(sd_begun) break;
        logmsg("Card Mount attempt: " + String(attempts));
        vTaskDelay(100);
      } while( attempts++ <= maxattempts );

      if( !sd_begun) {
        logmsg("Card Mount Failed");
        return;// true;
      }

      uint8_t cardType = SD.cardType();

      if(cardType == CARD_NONE){
        logmsg("No SD card attached");
        //log_e("No SD card attached");
        return;// true;
      }

      Serial.setDebugOutput(true);

      String test;
      for( int i=0;i<16384;i++ ) {
        test += (char)random(255);
      }

      for( int i=0;i<50;i++ ) {
        Serial.printf("Pass #%d\n", i );
        fs::File blah = SD.open("/test.txt", FILE_WRITE );
        blah.write( (uint8_t*)test.c_str(), test.length()+1 );
        blah.close();
      }

      Serial.setDebugOutput(false);

      return;// true;
    }
  },
#endif
  {"InitKeyboard", [](){ Keyboard.begin(); } },
  {"InitMouse", [](){ AbsMouse.begin(); MouseGFX = new GfxMouse( &AbsMouse ); MouseGFX->setDisplay( 1920*2, 1080 ); } },
  {"StartUSB", [](){ if( !USB.begin() ) logmsg("USB Failed to start"); else { while( !HID.ready() ) vTaskDelay(1);  }; } },
  {"StartWebServer", [](){ startWebServer(); } },
  {"StopSerial", [](){ Serial.end(); } },
  {"StopWiFi", [](){ WiFi.mode(WIFI_OFF); } },
  {"StopKeyboard", [](){ Keyboard.end(); } },
  {"StopMouse", [](){ AbsMouse.end(); } },
  {"DrawSpaceInvaders", [](){ MouseGFX->drawXbm( &Alien_128x64, 900, 500 ); } },
  {"logs", [](){ printdmesg( serialprintln ); } },
  //{"StopUSB", [](){ } },
  //{"StopWebServer", [](){  } },
  //{"SetWiFiMode", [](){  } },
  //{"SetWiFiSSID", [](){  } },
  //{"SetWiFiPass", [](){  } },
};


// SpaceHuhn's legacy Ducky Key commands
duckCommand KeyCommands[] =
{
  {"ENTER",       [](){ keyboard::pressKey(KEY_ENTER); } },
  {"MENU",        [](){ keyboard::pressKey(KEY_PROPS); } },
  {"APP",         [](){ keyboard::pressKey(KEY_PROPS); } },

  {"DELETE",      [](){ keyboard::pressKey(KEY_Delete); } },
  {"BACKSPACE",   [](){ keyboard::pressKey(KEY_BCKSPACE); } },
  {"HOME",        [](){ keyboard::pressKey(KEY_Home); } },
  {"INSERT",      [](){ keyboard::pressKey(KEY_INSRT); } },
  {"PAGEUP",      [](){ keyboard::pressKey(KEY_PAGEUP); } },
  {"PAGEDOWN",    [](){ keyboard::pressKey(KEY_PAGEDOWN); } },
  {"UPARROW",     [](){ keyboard::pressKey(KEY_UP); } },
  {"UP",          [](){ keyboard::pressKey(KEY_UP); } },
  {"DOWNARROW",   [](){ keyboard::pressKey(KEY_DOWN); } },
  {"DOWN",        [](){ keyboard::pressKey(KEY_DOWN); } },
  {"LEFTARROW",   [](){ keyboard::pressKey(KEY_LEFT); } },
  {"LEFT",        [](){ keyboard::pressKey(KEY_LEFT); } },
  {"RIGHTARROW",  [](){ keyboard::pressKey(KEY_RIGHT); } },
  {"RIGHT",       [](){ keyboard::pressKey(KEY_RIGHT); } },
  {"TAB",         [](){ keyboard::pressKey(KEY_TABULATION); } },
  {"END",         [](){ keyboard::pressKey(KEY_End); } },
  {"ESC",         [](){ keyboard::pressKey(KEY_ESCAPE); } },
  {"ESCAPE",      [](){ keyboard::pressKey(KEY_ESCAPE); } },
  {"F1",          [](){ keyboard::pressKey(KEY_F_1); } },
  {"F2",          [](){ keyboard::pressKey(KEY_F_2); } },
  {"F3",          [](){ keyboard::pressKey(KEY_F_3); } },
  {"F4",          [](){ keyboard::pressKey(KEY_F_4); } },
  {"F5",          [](){ keyboard::pressKey(KEY_F_5); } },
  {"F6",          [](){ keyboard::pressKey(KEY_F_6); } },
  {"F7",          [](){ keyboard::pressKey(KEY_F_7); } },
  {"F8",          [](){ keyboard::pressKey(KEY_F_8); } },
  {"F9",          [](){ keyboard::pressKey(KEY_F_9); } },
  {"F10",         [](){ keyboard::pressKey(KEY_F_10); } },
  {"F11",         [](){ keyboard::pressKey(KEY_F_11); } },
  {"F12",         [](){ keyboard::pressKey(KEY_F_12); } },
  {"SPACE",       [](){ keyboard::pressKey(KEY_SPACE); } },
  {"PAUSE",       [](){ keyboard::pressKey(KEY_PAUSE); } },
  {"CAPSLOCK",    [](){ keyboard::pressKey(KEY_CAPSLOCK); } },
  {"NUMLOCK",     [](){ keyboard::pressKey(KEY_NUMLOCK); } },
  {"PRINTSCREEN", [](){ keyboard::pressKey(KEY_SYSRQ); } },
  {"SCROLLLOCK",  [](){ keyboard::pressKey(KEY_SCROLLLOCK); } },
  // modifiers
  {"CTRL",        [](){ keyboard::pressKey(KEY_MOD_LCTRL); } },
  {"CONTROL",     [](){ keyboard::pressKey(KEY_MOD_LCTRL); } },
  {"SHIFT",       [](){ keyboard::pressKey(KEY_MOD_LSHIFT); } },
  {"ALT",         [](){ keyboard::pressKey(KEY_MOD_LALT); } },
  {"WINDOWS",     [](){ keyboard::pressKey(KEY_MOD_LMETA); } },
};


// run all payloads in a file, called from the webserver
// TODO: bufferize that
void runpayload( fs::FS *fs, const char* payload)
{
  if( !fs->exists( payload ) ) return;
  fs::File f = fs->open(payload, "r");
  if( !f ) return;
  String fileContent = "";
  while(f.available()) {
    fileContent += f.read();
  }
  f.close();
  duckparser::parse(fileContent);
}



void setup()
{
  //USB.onEvent(usbEventCallback);

  USB.productName("WUD Quack USB Host");
  USB.manufacturerName("INGSOC");
  USB.serialNumber("0xDEADB33F");
  //Keyboard.onEvent(usbEventCallback);
  //HID.onEvent(usbEventCallback);
  WiFi.onEvent(WiFiEventCallback);
  // attach loggers to USB items, messages are deferred for later viewing with ducky "logs" command
  #ifdef HAS_PENDRIVE
    USBPenDriveLogger = logsprintf;
  #endif

  WebServerLogger = logmsg;
  MouseLogger = logmsg;
  WebServerLogsPrinter = printdmesg; // web server can deliver logs

  // attach custom ducky commands to the payload runner
  duckparser::setCommands( DuckyCommands, sizeof( DuckyCommands ) / sizeof( duckCommand ) );
  duckparser::setKeys( KeyCommands, sizeof( KeyCommands ) / sizeof( duckCommand ) );

  // use custom ducky commands to run this skech :-)
  duckparser::parse( "SerialBegin" );
  duckparser::parse( "InitKeyboard" );
  duckparser::parse( "InitMouse" );
  #ifdef HAS_PENDRIVE
    duckparser::parse( "InitPenDrive" );
  #endif
  duckparser::parse( "StartUSB" );
  duckparser::parse( "InitSPIFFS" );
  #ifdef HAS_PENDRIVE
    vTaskDelay(5000); // let the USB tasks finish their shift before doing critical detection stuff
  #endif
  duckparser::parse( "InitWiFi" ); // why the fuck does it fail on first connect ???
  duckparser::parse( "StartWebServer" );
}


void loop()
{
  // process webserver commands
  if( webserver_begun ) server.handleClient();
  // process serial commands
  if( serial_begun && Serial.available() )
  {
    String line = "";
    while( Serial.available() && line.length()<MAX_SERIAL_INPUT ) line += (char)Serial.read();
    line.trim();
    if( line == "help" ) {
      Serial.println("Additional ESP32-WUD ducky commands:");
      for( int i=0;i<duckparser::commands_count;i++ ) {
        Serial.printf("  - %s\n", duckparser::commands[i].name );
      }
      return;
    }
    //log_d("Will handle payload: %s", line.c_str() );
    duckparser::parse( line );
  }
  vTaskDelay(10);
}

