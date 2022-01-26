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


#include "config_check.h"
#include "WiFiDuck/duckparser.cpp" // SpaceHuhn's WiFiDuck implementation, with some mouse tweaks
#include "webserver.h"

#if ARDUINO_USB_CDC_ON_BOOT
  static bool serial_begun = true;
#else
  static bool serial_begun = false;
#endif


// floating filesystem
fs::FS* duckyFS = nullptr;


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


/*
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
*/



void serialprintln(String msg)
{
  USBSerial.println( msg );
}


void SerialPrintHelp()
{
  using namespace duckparser;
  USBSerial.println("Legacy WiFiDuck named keys:");
  for( int i=0;i<keys->count;i++ ) {
    if( i%8==0 ) USBSerial.println();
    USBSerial.printf(" %-12s", keys->commands[i].name );
  }
  USBSerial.println("\n");
  USBSerial.println("Legacy WiFiDuck commands:");
  USBSerial.println("  - REM");
  USBSerial.println("  - STRING");
  for( int i=0;i<legacy_commands->count;i++ ) {
    USBSerial.printf("  - %s\n", legacy_commands->commands[i].name );
  }
  USBSerial.println();
  USBSerial.println("WUD Ducky commands:");
  for( int i=0;i<custom_commands->count;i++ ) {
    USBSerial.printf("  - %s\n", custom_commands->commands[i].name );
  }
  USBSerial.println();
  return;
}


void SerialBegin()
{
  if( serial_begun ) {
    logmsg("Serial already started");
    return;
  }
  USBSerial.begin();
  HWSerial.begin(115200);
  HWSerial.setDebugOutput(true);
  serial_begun = true;
  //Serial.setDebugOutput(true);
}

void SerialDebug()
{
  static bool debug_enabled = false;
  debug_enabled = !debug_enabled;
  USBSerial.setDebugOutput(debug_enabled); // doing this may cause instability, use only for debug and expect more bugs :-)
  logsprintf("setDebugOutput(%s)", debug_enabled?"true":"false");
}

void InitWiFi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);
}

void InitSPIFFS()
{
  if( !SPIFFS.begin() ) logmsg("SPIFFS failed!");
}

void InitSD()
{
  if( !initSD() ) duckyFS = &SPIFFS;
  else duckyFS = &SD;
}

void InitMouse()
{
  AbsMouse.begin();
  MouseGFX = new GfxMouse( &AbsMouse );
  MouseGFX->setDisplay( 1920*2, 1080 );
}

void StartUSB()
{
  #if (ARDUINO_USB_CDC_ON_BOOT|ARDUINO_USB_MSC_ON_BOOT|ARDUINO_USB_DFU_ON_BOOT)
    // USB was already started by arduino core
    logmsg("USB already started");
    while( !HID.ready() ) vTaskDelay(1);
    return;
  #else
    USB.VID( USB_VID );
    USB.PID( USB_PID );
    USB.productName( USB_PRODUCT );
    USB.manufacturerName( USB_MANUFACTURER );
    USB.serialNumber( USB_SERIAL );
    if( !USB.begin() ) logmsg("USB Failed to start");
    else { while( !HID.ready() ) vTaskDelay(1);  };
  #endif
}





// WUD-Ducky system functionalities as Ducky Commands in a mix of real and lambda functions array.
// A few are still missing, such as ESP-Restart, AP controls, debug, etc
duckCommand WUDDuckCommands[] =
{
  { "help",              SerialPrintHelp,                      false },
  { "SerialBegin",       SerialBegin,                          false },
  { "SerialDebug",       SerialDebug,                          false },
  { "StopSD",            deinitSD,                             false },
  { "StopPenDrive",      deinitPenDrive,                       false },
  { "StartUSB",          StartUSB,                             false },
  { "InitWiFi",          InitWiFi,                             false },
  { "InitSPIFFS",        InitSPIFFS,                           false },
  { "InitSD",            InitSD,                               false },
  { "InitMouse",         InitMouse,                            false },
  { "InitPenDrive",      [](){ initPenDrive(); },              false },
  { "InitKeyboard",      [](){ Keyboard.begin(); },            false },
  { "StartWebServer",    [](){ startWebServer(); },            false },
  { "StopSerial",        [](){ USBSerial.end(); },                false },
  { "StopWiFi",          [](){ WiFi.mode(WIFI_OFF); },         false },
  { "StopKeyboard",      [](){ Keyboard.end(); },              false },
  { "StopMouse",         [](){ AbsMouse.end(); },              false },
  { "logs",              [](){ printdmesg( serialprintln ); }, false },
  //{"StopUSB", [](){ } },
  //{"StopWebServer", [](){  } },
  //{"SetWiFiMode", [](){  } },
  //{"SetWiFiSSID", [](){  } },
  //{"SetWiFiPass", [](){  } },
};


duckCommandSet WUDDuckCommandsSet = {WUDDuckCommands, sizeof( WUDDuckCommands ) / sizeof( duckCommand )};


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
  //Keyboard.onEvent(usbEventCallback);
  //HID.onEvent(usbEventCallback);
  WiFi.onEvent(WiFiEventCallback);
  // attach loggers to USB items, messages are deferred for later viewing with ducky "logs" command
  USBPenDriveLogger = logsprintf;

  WebServerLogger = logmsg;
  MouseLogger = logmsg;
  WebServerLogsPrinter = printdmesg; // web server can deliver logs

  // attach custom ducky commands to the payload runner
  duckparser::init(&WUDDuckCommandsSet);

  // use custom ducky commands to run this skech :-)
  duckparser::parse( "SerialBegin" );
  duckparser::parse( "InitKeyboard" );
  duckparser::parse( "InitMouse" );
  duckparser::parse( "InitPenDrive" );
  duckparser::parse( "StartUSB" );
  duckparser::parse( "InitSPIFFS" );
  vTaskDelay(5000); // let the USB tasks finish their shift before doing critical detection stuff
  duckparser::parse( "InitWiFi" ); // why the fuck does it fail on first connect ???
  duckparser::parse( "StartWebServer" );
}


void loop()
{
  if( webserver_begun ) server.handleClient(); // process webserver commands
  if( serial_begun && USBSerial.available() )     // process serial commands
  {
    String line = "";
    while( USBSerial.available() && line.length()<MAX_SERIAL_INPUT ) line += (char)USBSerial.read();
    line.trim();
    int repeats = duckparser::getRepeats();
    do {
      duckparser::parse( line );
    } while( --repeats > 0 );
  }
  vTaskDelay(10);
}

