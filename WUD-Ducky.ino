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

static GfxMouse *MouseGFX = nullptr;
fs::FS* duckyFS = nullptr;


std::vector<String> dmesg;
typedef void(*logprintercb)( String msg );

void serialprinter(String msg)
{
  Serial.println( msg );
}

void logmsg( String msg )
{
  dmesg.push_back( msg );
}

void printdmesg( logprintercb logprint )
{
  if( dmesg.size() > 0 ) {
    for( int i=0; i<dmesg.size(); i++ ) {
      logprint( dmesg[i] );
    }
    dmesg.clear();
  }
}


/*
void MouseDrawStr( String line )
{
  String strline = line;
  int str_len = strline.length()+1;
  char keyarray[str_len];
  strline.toCharArray(keyarray, str_len);
  char delimiter[] = "+";
  char *strblock;
  strblock = strtok(keyarray, delimiter);
  int8_t move;

  uint8_t mousestate = MouseGFX->getMouseReport()->buttons; // memoize mouse buttons state

  MouseGFX->moveRelative( 0, 0, 0 ); // reset mouse buttons state

  while(strblock != NULL) {
    switch( strblock[0] ) {
      case 'X': // move X axis
        move = atoi( &strblock[1] );
        MouseGFX->moveX( move );
      break;
      case 'Y': // move Y axis
        move = atoi( &strblock[1] );
        MouseGFX->moveY( move );
      break;
      case 'C': // click/unclick
      {
        vTaskDelay(15);
        uint8_t mouse_btn_mask = (uint8_t)atoi(&strblock[1]);
        MouseGFX->moveRelative( 0, 0, mouse_btn_mask );
      }
      break;
      default: // ignore
      break;
    }
    strblock = strtok(NULL, delimiter);
  }

  MouseGFX->moveRelative( 0, 0, mousestate ); // restore mouse buttons state
}
*/

void HIDKeySend( String str )
{
  for( int i=0; i<str.length(); i++ ) {
    Keyboard.press(str[i]);
    vTaskDelay(PayloadRunner.keyDelay/2);
    Keyboard.releaseAll();
    vTaskDelay(PayloadRunner.keyDelay/2);
  }
  Keyboard.press('\n');
  vTaskDelay(PayloadRunner.keyDelay/2);
  Keyboard.releaseAll();
  vTaskDelay(PayloadRunner.keyDelay/2);
}






DuckyCommand DuckyCommands[] =
{
  {"Rem", [](){ PayloadRunner.DelayLength = 0; } },
  {"DefaultDelay", [](){ PayloadRunner.DelayLength = 1500; PayloadRunner.defaultdelay = PayloadRunner.cmdinput.toInt(); } },
  {"CustomDelay", [](){ PayloadRunner.custom_delay = PayloadRunner.cmdinput.toInt(); PayloadRunner.DelayLength = PayloadRunner.custom_delay; } },
  {"KeyDelay", [](){ PayloadRunner.DelayLength = 0; PayloadRunner.keyDelay = atoi(PayloadRunner.cmdinput.c_str()); } },
  {"Key", [](){ int key = atoi(PayloadRunner.cmdinput.c_str()); vTaskDelay(25); Keyboard.press(key); vTaskDelay(25); Keyboard.releaseAll(); } },
  {"DrawSpaceInvaders", [](){ MouseGFX->drawXbm( &Alien_128x64, 900, 500 ); } },
  {"logs", [](){ printdmesg( serialprinter ); } },
  {"MouseMoveUp", [](){   int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveRelative( 0, -mousemoveamt, MouseGFX->button_pushed ); vTaskDelay(25);} },
  {"MouseMoveDown", [](){ int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveRelative( 0, mousemoveamt, MouseGFX->button_pushed ); vTaskDelay(25);} },
  {"MouseMoveLeft", [](){ int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveRelative( -mousemoveamt, 0, MouseGFX->button_pushed ); vTaskDelay(25);} },
  {"MouseMoveRight", [](){ int mousemoveamt = PayloadRunner.cmdinput.toInt(); MouseGFX->moveRelative( mousemoveamt, 0, MouseGFX->button_pushed ); vTaskDelay(25);} },
  {"MouseClickRelease", [](){ MouseGFX->moveRelative( 0, 0, 0 ); vTaskDelay(25);} },
  {"MouseClickRIGHT", [](){ MouseGFX->moveRelative( 0, 0, MOUSE_RIGHT_BTN );vTaskDelay(25); } },
  {"MouseClickLEFT", [](){ MouseGFX->moveRelative( 0, 0, MOUSE_LEFT_BTN );vTaskDelay(25); } },
  {"MouseClickMIDDLE", [](){ MouseGFX->moveRelative( 0, 0, MOUSE_MIDDLE_BTN );vTaskDelay(25); } },
  {"MouseDoubleClickLEFT", [](){ MouseGFX->moveRelative( 0, 0, MOUSE_LEFT_BTN ); vTaskDelay(25); MouseGFX->moveRelative( 0, 0, MOUSE_LEFT_BTN ); vTaskDelay(25); MouseGFX->moveRelative( 0, 0, 0 );}},
  //{"MouseDrawStr", [](){ vTaskDelay(25);MouseDrawStr( PayloadRunner.cmdinput ); } },
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
        Keyboard.press(key);
        PayloadRunner.keypart = strtok(NULL, PayloadRunner.plusdelimiter);
      }
      if(PayloadRunner.keypressdone==1) {
        vTaskDelay(25);
        Keyboard.releaseAll();
        PayloadRunner.keypressdone=0;
      }
    }
  },
  {"Print", [](){
      vTaskDelay(25);
      size_t len = PayloadRunner.cmdinput.length();
      uint8_t keycode;
      for(size_t i = 0; i < len; i++) {
        Keyboard.press(PayloadRunner.cmdinput[i]);
        vTaskDelay(PayloadRunner.keyDelay/2);
        Keyboard.releaseAll();
        vTaskDelay(PayloadRunner.keyDelay/2);
      }
    }
  },

  {"PrintLine", [](){
      vTaskDelay(25);
      size_t len = PayloadRunner.cmdinput.length();
      uint8_t keycode;
      for(size_t i = 0; i < len; i++) {
        Keyboard.press(PayloadRunner.cmdinput[i]);
        vTaskDelay(PayloadRunner.keyDelay/2);
        Keyboard.releaseAll();
        vTaskDelay(PayloadRunner.keyDelay/2);
      }
      vTaskDelay(250);
      Keyboard.press(HID_KEY_ENTER);
      vTaskDelay(25);
      Keyboard.releaseAll();
    }
  },
  {"SetDisplay", [](){
      String fullkeys = PayloadRunner.cmdinput;
      int str_len = fullkeys.length()+1;
      char keyarray[str_len];
      fullkeys.toCharArray(keyarray, str_len);
      int w, h;
      PayloadRunner.keypart = strtok(keyarray, PayloadRunner.plusdelimiter);
      if(PayloadRunner.keypart != NULL) {
        w = atoi(PayloadRunner.keypart);
        Serial.printf("SetDisplay new width: %d\n", w );
        PayloadRunner.keypart = strtok(NULL, PayloadRunner.plusdelimiter);
        if(PayloadRunner.keypart != NULL) {
          h = atoi(PayloadRunner.keypart);
          Serial.printf("SetDisplay new height: %d\n", w );
          MouseGFX->setDisplay( w, h ); // dual HD screen
        } else {
          Serial.println("SetDisplay error: failed to get height");
        }
      } else {
        Serial.println("SetDisplay error: failed to get width");
      }
    }
  },


};





void runpayload( fs::FS *fs, const char* payload)
{
  if( !fs->exists( payload ) ) return;
  fs::File f = fs->open(payload, "r");
  if( !f ) return;
  // memoize delay settings
  PayloadRunner.defaultdelay = PayloadRunner.DelayLength;
  PayloadRunner.settingsdefaultdelay = PayloadRunner.DelayLength;
  while(f.available()) {
    String line = f.readStringUntil('\n');
    PayloadRunner.parseLine( line );
  }
  f.close();
  // restore delay settings
  PayloadRunner.DelayLength = PayloadRunner.settingsdefaultdelay;
}



static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        dmesg.push_back("USB PLUGGED");
      break;
      case ARDUINO_USB_STOPPED_EVENT:
        dmesg.push_back("USB UNPLUGGED");
      break;
      case ARDUINO_USB_SUSPEND_EVENT:
        dmesg.push_back( String( "USB SUSPENDED: remote_wakeup_en: " + String( data->suspend.remote_wakeup_en ) ) );
      break;
      case ARDUINO_USB_RESUME_EVENT:
        dmesg.push_back("USB RESUMED");
      break;
      default: break;
    }
  }
}




void setup()
{
  USB.onEvent(usbEventCallback);
  USBPendriveErrorLogger = logmsg; // messages are deferred in a vector string for later viewing with ducky "logs" command
  WebServerLogger = logmsg;
  WebServerLogsPrinter = printdmesg;
  MouseLogger = logmsg;
  Serial.begin(115200); // ineffective if USBCDC on boot is off
  // start WiFi, the earlier the best
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);
  // Init USB PenDrive to SD
  if( initSD(SD_SCK, SD_MISO, SD_MOSI, SD_CS) ) {
    dmesg.push_back("SD started successfully");
    MSC.vendorID("ESP32-S2");//max 8 chars
    MSC.productID("SD card");//max 16 chars
    MSC.productRevision("1.0");//max 4 chars
    MSC.onStartStop(onStartStop);
    MSC.onRead(onRead);
    MSC.onWrite(onWrite);
    MSC.mediaPresent(true);
    if( MSC.begin( SD.cardSize()/512, 512 ) ) {
      dmesg.push_back("USB PenDrive started successfully");
      duckyFS = &SD;
    } else {
      dmesg.push_back("USB PenDrive failed to start");
    }
  } else {
    dmesg.push_back("Failed to start SD, PenDrive disabled");
    duckyFS = &SPIFFS;
  }

  Keyboard.begin();
  AbsMouse.begin();

  if( !USB.begin() ) dmesg.push_back("USB Failed to start");
  else {
    while( !HID.ready() ) vTaskDelay(1); // wait for HID (e.g. driver being installed on the host OS)
  }

  MouseGFX = new GfxMouse( &AbsMouse );
  MouseGFX->setDisplay( 1920*2, 1080 ); // (1920*2, 1080) = dual HD screen, adjust accordingly with ducky "SetDisplay:800+600" command

  PayloadRunner.setDuckyCommands( DuckyCommands, sizeof( DuckyCommands ) / sizeof( DuckyCommand ) );

  vTaskDelay(5000); // let the SD be detected by the host OS before running the webserver

  if( !SPIFFS.begin() ) dmesg.push_back("SPIFFS failed!");

  startFileServer();
}


void loop()
{
  server.handleClient();
  if( Serial.available() )
  {
    String line = Serial.readStringUntil('\n');
    if( line != "help" ) {
      Serial.printf("Will handle payload: %s\n", line.c_str() );
      PayloadRunner.parseLine( line );
    } else {
      Serial.println("Available Ducky commands:");
      for( int i=0;i<PayloadRunner.commands_count;i++ ) {
        Serial.printf("  - %s\n", PayloadRunner.commands[i].name );
      }
    }
  }
  vTaskDelay(2);
}

