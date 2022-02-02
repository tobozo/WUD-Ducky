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


#include "config_check.h" // fail early if the right board settings aren't selected
#include "WiFiDuck/duckparser.cpp" // SpaceHuhn's WiFiDuck implementation, with some mouse tweaks
#include "WiFiDuck/WebServer.cpp"  // WUD Ducky Web UI



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
  Logger::logsprintf("[WiFi-event]: #%d (%s)", event, arduino_event_names[event]);
}



void serialprintln(String msg)
{
  USBSerial.println( msg );
}


void SystemInfo()
{
  String sysInfo;
  WebUI::getSystemInfo( &sysInfo, WebUI::SYSINFO_TXT );
  USBSerial.println( sysInfo );
}


void SerialPrintHelp()
{
  using namespace duckparser;
  if( !WUDStatus::usbserial_begun ) return;
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
  if( ! WUDStatus::hwserial_begun ) {
    HWSerial.begin(115200);
    HWSerial.setDebugOutput(false);
    WUDStatus::hwserial_begun = true;
  }

  if( !WUDStatus::usbserial_begun ) {
    USBSerial.begin();
    USBSerial.setDebugOutput(false);
    WUDStatus::usbserial_begun = true;
  }
}


void StopSerial()
{
  if( WUDStatus::usbserial_begun ) {
    USBSerial.end();
    WUDStatus::usbserial_begun = false;
  }
}


void SerialDebug()
{
  static bool debug_enabled = false;
  if( !WUDStatus::hwserial_begun ) return;
  debug_enabled = !debug_enabled;
  HWSerial.setDebugOutput(debug_enabled); // doing this may cause instability, use only with HWSerial connected and expect more bugs :-)
  Logger::logsprintf("setDebugOutput(%s)", debug_enabled?"true":"false");
}

void ResetPrefs()
{
  prefs::reset();
}

void LoadWiFiCreds()
{
  // retrieve SSID and Password for access point and station mode
  prefs::get( "AP_SSID",      AP_SSID,      SSID_MAX_LEN,     DEFAULT_AP_SSID );
  prefs::get( "AP_PASSWORD",  AP_PASSWORD,  PASSWORD_MAX_LEN, DEFAULT_AP_PASSWORD );
  prefs::get( "STA_SSID",     STA_SSID,     SSID_MAX_LEN,     DEFAULT_STA_SSID );
  prefs::get( "STA_PASSWORD", STA_PASSWORD, PASSWORD_MAX_LEN, DEFAULT_STA_PASSWORD );
  //logmsg( String( "Prefs AP_SSID='" + String(AP_SSID) + "' AP_PASSWORD='" + String(AP_PASSWORD) + "' STA_SSID='" + String(STA_SSID) + "' STA_PASSWORD='" + String(STA_PASSWORD) + "'" ) );
  //wificreds_loaded = true;
}

void SetSSID_AP()
{
  if( duckparser::wordnode->len > 0 ) {
    prefs::set( "AP_SSID", duckparser::wordnode->str, duckparser::wordnode->len );
  }
}
void SetSSID_STA()
{
  if( duckparser::wordnode->len > 0 ) {
    prefs::set( "STA_SSID", duckparser::wordnode->str, duckparser::wordnode->len );
  }
}
void SetPassword_AP()
{
  if( duckparser::wordnode->len > 0 ) {
    prefs::set( "AP_PASSWORD", duckparser::wordnode->str, duckparser::wordnode->len );
  }
}
void SetPassword_STA()
{
  if( duckparser::wordnode->len > 0 ) {
    prefs::set( "STA_PASSWORD", duckparser::wordnode->str, duckparser::wordnode->len );
  }
}


void InitWiFiAP()
{
  WiFi.mode( WIFI_MODE_APSTA );
  WiFi.softAP( AP_SSID, AP_PASSWORD );
  //Logger::logsprintf("Soft AP Started with SSID(%s) and PASSWORD(%s)", AP_SSID, AP_PASSWORD );
  WUDStatus::softap_begun = true;
}


void InitWiFiSTA()
{
  if( STA_SSID[0] != '\0' ) {
    WiFi.begin( STA_SSID, STA_PASSWORD );
  } else {
    WiFi.begin();
  }

  // advertise the product name via mDNS
  // so it becomes accessible via http://wud-nutquacker.local/

  if(!MDNS.begin( USB_PRODUCT )) {
    Logger::logmsg("Error starting mDNS");
  }

  // TODO: use WiFi events and a separate task
  unsigned long timeout = 10000, now = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if( millis()-now > timeout ) {
      Logger::logmsg("WiFi Timed out");
      return;
    }
    vTaskDelay(10);
  }
  WUDStatus::wifista_begun = true;
  Logger::logmsg( "WiFi STA IP: " + WiFi.localIP().toString() );
  NTP::enable();
}


void StopWiFiSTA()
{
  WiFi.mode( WIFI_OFF );
  if( WUDStatus::softap_begun ) {
    InitWiFiAP();
  }
}







void InitSPIFFS()
{
  if( !SPIFFS.begin() ) {
    Logger::logmsg("SPIFFS failed!");
    return;
  }
  duckyFS = &SPIFFS;
  WUDStatus::spiffs_begun = true;
}

void InitSD()
{
  if( !initSD() ) duckyFS = &SPIFFS;
  //else duckyFS = &SD;
}

void InitMouse()
{
  AbsMouse.begin();
  MouseGFX = new GfxMouse( &AbsMouse );
  MouseGFX->setDisplay( 1920*2, 1080 );
}

void StartUSB()
{
  if( WUDStatus::usb_begun ) {
    // USB was started by arduino core
    Logger::logmsg("USB already started");
  } else {
    USB.VID( USB_VID );
    USB.PID( USB_PID );
    USB.productName( USB_PRODUCT );
    USB.manufacturerName( USB_MANUFACTURER );
    USB.serialNumber( USB_SERIAL );
    if( !USB.begin() ) {
      Logger::logmsg("USB Failed to start");
      return;
    }
    WUDStatus::usb_begun = true;
  }

  unsigned long timeout = 10000, now = millis();
  while( !HID.ready() ) {
    if( millis()-now > timeout ) {
      Logger::logmsg("HID Timed out");
      return;
    }
    vTaskDelay(10);
  }
  WUDStatus::hid_ready = true;
}

void InitKeyboard()
{
  if( WUDStatus::keyboard_begun ) return;
  Keyboard.begin();
  WUDStatus::keyboard_begun = true;
}

void StopKeyboard()
{
  if( !WUDStatus::keyboard_begun ) return;
  Keyboard.end();
  WUDStatus::keyboard_begun = false;
}


// WUD-Ducky system functionalities as Ducky Commands in a mix of real and lambda functions array.
// A few are still missing, such as ESP-Restart, AP controls, debug, etc
duckCommand WUDDuckCommands[] =
{
  { "help",              SerialPrintHelp,                      false },
  { "SysInfo",           SystemInfo,                           false },
  { "SerialBegin",       SerialBegin,                          false },
  { "StopSerial",        StopSerial,                           false },
  { "SerialDebug",       SerialDebug,                          false },
  { "StopSD",            deinitSD,                             false },
  { "StopPenDrive",      deinitPenDrive,                       false },
  { "StartUSB",          StartUSB,                             false },
//  { "InitWiFi",          InitWiFi,                             false },

  { "LoadWiFiCreds",     LoadWiFiCreds,                        false }, // load AP/STA settings from preferences
  { "InitWiFiAP",        InitWiFiAP,                           false },
  { "InitWiFiSTA",       InitWiFiSTA,                          false },
  { "StopWiFiSTA",       StopWiFiSTA,                          false },


  { "ResetPrefs",        ResetPrefs,                           false },
  { "SetSSID_AP",        SetSSID_AP,                           true  },
  { "SetSSID_STA",       SetSSID_STA,                          true  },
  { "SetPassword_AP",    SetPassword_AP,                       true  },
  { "SetPassword_STA",   SetPassword_STA,                      true  },


  { "InitSPIFFS",        InitSPIFFS,                           false },
  { "InitSD",            InitSD,                               false },
  { "InitMouse",         InitMouse,                            false },
  { "InitKeyboard",      InitKeyboard,                         false },
  { "StopKeyboard",      StopKeyboard,                         false },
  { "InitPenDrive",      [](){ initPenDrive(); },              false },
  { "StartWebServer",    [](){ WS::startWebServer(); },            false },
  { "StopWiFi",          [](){ WiFi.mode(WIFI_OFF); },         false },
  { "StopMouse",         [](){ AbsMouse.end(); },              false },
  { "logs",              [](){ Logger::printdmesg( serialprintln ); }, false },
  { "logs-disable",      [](){ Logger::disable(); },           false },
  { "logs-enable",       [](){ Logger::enable(); },            false },
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
  if( !fs->exists( payload ) ) {
    Logger::logsprintf("runpayload error: file %s does not exist", payload );
    return;
  }
  fs::File f = fs->open(payload, "r");
  if( !f ) {
    Logger::logsprintf("runpayload error: file %s cannot be opened", payload );
    return;
  }
  String fileContent = "";
  while(f.available()) {
    String lineContent = f.readStringUntil('\n');
    duckparser::parse( lineContent );
    //fileContent += (char)f.read();
  }
  f.close();
  //duckparser::parse(fileContent);
  Logger::logsprintf("payload from file %s run successfully", payload );
}



void setup()
{
  //USB.onEvent(usbEventCallback);
  Keyboard.onEvent(kbdEventCallback);
  //HID.onEvent(usbEventCallback);
  WiFi.onEvent(WiFiEventCallback);
  // attach loggers to USB items, messages are deferred for later viewing with ducky "logs" command
  USBPenDriveLogger        = Logger::logsprintf;

  WS::WebServerLogger      = Logger::logmsg;          // basic logger
  WS::WebServerLogsPrinter = Logger::printdmesg; // logs history printer
  WS::HIDKeySender         = duckparser::parse;  // string quacker
  WS::runpayload           = runpayload;           // file quacker

  MouseLogger              = Logger::logmsg;

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
  duckparser::parse( "LoadWiFiCreds" );
  duckparser::parse( "InitWiFiAP" );
  duckparser::parse( "InitWiFiSTA" );
  duckparser::parse( "StartWebServer" );
}


void loop()
{
  if( WUDStatus::webserver_begun ) WS::server.handleClient(); // process webserver commands
  if( WUDStatus::usbserial_begun && USBSerial.available() )     // process serial commands
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

