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
  if( WUDStatus::usbserial_begun ) {
    USBSerial.println( msg );
  }
}


void SystemInfo()
{
  String sysInfo;
  WebUI::getSystemInfoTXT( &sysInfo );
  if( WUDStatus::usbserial_begun ) {
    USBSerial.println( sysInfo );
  }
  sysInfo = String();
}


void SerialPrintHelp()
{
  String output;
  WebUI::getHelpItemsTXT( &output );
  if( WUDStatus::usbserial_begun ) {
    USBSerial.println(output);
  }
  output = String();
  /*
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
  */
}

void SerialPrintHelpArg()
{
  String output;
  WebUI::getHelpItemTXT( &output, duckparser::wordnode->str );
  if( WUDStatus::usbserial_begun ) {
    USBSerial.println(output);
  }
  output = String();
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
  NTP::loadPrefs();
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


void SetNTPZone()
{
  if( duckparser::wordnode->len > 0 ) {
    size_t servers_count = sizeof( NTP::Servers ) / sizeof( NTP::Server );
    // TODO: parse float
    uint8_t zonenum = atoi( duckparser::wordnode->str );
    if( zonenum < servers_count ) {
      NTP::setServer( zonenum );
    } else {
      Logger::logsprintf("[NTP] %d is not a valid zone index (input=%s)", duckparser::wordnode->str );
    }
  }
}


void SetDST()
{
  if( duckparser::wordnode->len > 0 ) {
    int8_t dst = atoi( duckparser::wordnode->str );
    NTP::setDst( dst > 0 );
  }
}


void SetTZ()
{
  if( duckparser::wordnode->len > 0 ) {
    int8_t zonenum = atoi( duckparser::wordnode->str );
    if( zonenum > -24.0 && zonenum < 24 ) {
      NTP::setTimezone( (float)zonenum );
    } else {
      Logger::logsprintf("[TZ] %d is not a valid zone index (input=%s)", duckparser::wordnode->str );
    }
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

  if(!MDNS.begin( USB.productName() /*USB_PRODUCT*/ )) {
    Logger::logmsg("[mDNS] Error starting mDNS");
  } else {
    Logger::logsprintf("[mDNS] Advertising: %s.local", USB.productName()/*USB_PRODUCT*/ );
  }

  // TODO: use WiFi events and a separate task
  unsigned long timeout = 10000, now = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if( millis()-now > timeout ) {
      Logger::logmsg("[WiFi] Timeout");
      return;
    }
    vTaskDelay(10);
  }
  WUDStatus::wifista_begun = true;
  Logger::logmsg( "[WiFi] STA IP: " + WiFi.localIP().toString() );
  NTP::enable();
}


void StopWiFiSTA()
{
  WiFi.mode( WIFI_OFF );
  if( WUDStatus::softap_begun ) {
    InitWiFiAP();
  }
}







void InitLittleFS()
{
  if( !LittleFS.begin() ) {
    Logger::logmsg("LittleFS failed!");
    return;
  }
  duckyFS = &LittleFS;
  WUDStatus::spiffs_begun = true;
}

void InitSD()
{
  if( !initSD() ) duckyFS = &LittleFS;
  //else duckyFS = &SD;
}

void InitMouse()
{
  AbsMouse.begin();
  MouseGFX = new GfxMouse( &AbsMouse );
  MouseGFX->setDisplay( 1920*2, 1080 );
  WUDStatus::absmouse_begun = true;
  Logger::logsprintf("[HID] AbsMouse report ID: %d", HID_REPORT_ID_MOUSE );
}

void StartUSB()
{
  if( WUDStatus::usb_begun ) {
    // USB was started by arduino core
    Logger::logmsg("USB already started");
  } else {
    //USB.VID( USB_VID );
    //USB.PID( USB_PID );
    USB.productName( USB_PRODUCT );
    USB.manufacturerName( USB_MANUFACTURER );
    //USB.serialNumber( USB_SERIAL );
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
  Logger::logsprintf("[HID] Keyboard report ID: %d", HID_REPORT_ID_KEYBOARD );
  WUDStatus::keyboard_begun = true;
}

void StopKeyboard()
{
  if( !WUDStatus::keyboard_begun ) return;
  Keyboard.end();
  WUDStatus::keyboard_begun = false;
}


void EnableOTA()
{
  WUDStatus::ota_enabled = true;
}
void DisableOTA()
{
  WUDStatus::ota_enabled = false;
}



#define HELP_TEXT_help            "e.g. help SysInfo"
#define HELP_TEXT_SysInfo         "Print system information."
#define HELP_TEXT_SerialBegin     "Start the serial interface (implicit at boot)."
#define HELP_TEXT_LoadWiFiCreds   "Loads AP/STA and NTP settings from preferences."
#define HELP_TEXT_ResetPrefs      "Reset NVS preferences to the factory values."
#define HELP_TEXT_SetSSID_AP      "Set the SSID for the WiFi Access Point.\n\tMax 32 chars.\n\n\te.g. SetSSID_AP WUD-NUTQUACKER"
#define HELP_TEXT_SetSSID_STA     "Set the SSID for the WiFi Station.\n\tMax 32 chars.\n\n\te.g. SetSSID_STA my-router-ssid"
#define HELP_TEXT_SetPassword_AP  "Set the Password for the WiFi Access Point.\n\tMin 8 chars, max 63 chars.\n\n\te.g. SetPassword_AP myl337p4s5"
#define HELP_TEXT_SetPassword_STA "Set the Password for the  WiFi Station.\n\tMin 8 chars, max 63 chars.\n\n\te.g. SetPassword_STA myl337p4s5"
#define HELP_TEXT_SetNTPZone      "Set the NTP zone (stored as pref).\n\n\t0=Global\n\t1=Africa\n\t2=Asia\n\t3=Europe\n\t4=North America\n\t5=Oceania\n\t6=South America\n\n\te.g. SetNTPZone 3"
#define HELP_TEXT_SetDST          "Set the daylight saving time, 0=off, 1=on  (stored as pref).\n\n\te.g. SetDST 1"
#define HELP_TEXT_SetTZ           "Set the timezone offset in hours (stored as pref).\n\n\te.g. SetTZ 2"


// WUD-Ducky system functionalities as Ducky Commands in a mix of real and lambda functions array.
// A few are still missing, such as ESP-Restart, AP controls, debug, etc
duckCommand WUDDuckCommands[] =
{
  { "help",              SerialPrintHelp,                      false },
  { "help",              SerialPrintHelpArg,                   true,  HELP_TEXT_help },
  { "SysInfo",           SystemInfo,                           false, HELP_TEXT_SysInfo },
  { "SerialBegin",       SerialBegin,                          false, HELP_TEXT_SerialBegin },
  { "StopSerial",        StopSerial,                           false },
  { "SerialDebug",       SerialDebug,                          false },
  { "StopSD",            deinitSD,                             false },
  { "StopPenDrive",      deinitPenDrive,                       false },
  { "StartUSB",          StartUSB,                             false },

  { "LoadWiFiCreds",     LoadWiFiCreds,                        false, HELP_TEXT_LoadWiFiCreds },
  { "InitWiFiAP",        InitWiFiAP,                           false },
  { "InitWiFiSTA",       InitWiFiSTA,                          false },
  { "StopWiFiSTA",       StopWiFiSTA,                          false },

  { "EnableOTA",         EnableOTA,                            false },
  { "DisableOTA",        DisableOTA,                           false },

  { "ResetPrefs",        ResetPrefs,                           false, HELP_TEXT_ResetPrefs  },
  { "SetSSID_AP",        SetSSID_AP,                           true,  HELP_TEXT_SetSSID_AP },
  { "SetSSID_STA",       SetSSID_STA,                          true,  HELP_TEXT_SetSSID_STA },
  { "SetPassword_AP",    SetPassword_AP,                       true,  HELP_TEXT_SetPassword_AP },
  { "SetPassword_STA",   SetPassword_STA,                      true,  HELP_TEXT_SetPassword_STA },

  { "SetNTPZone",        SetNTPZone,                           true,  HELP_TEXT_SetNTPZone },
  { "SetDST",            SetDST,                               true,  HELP_TEXT_SetDST },
  { "SetTZ",             SetTZ,                                true,  HELP_TEXT_SetTZ },

  { "InitLittleFS",      InitLittleFS,                         false },
  { "InitSD",            InitSD,                               false },
  { "InitMouse",         InitMouse,                            false },
  { "InitKeyboard",      InitKeyboard,                         false },
  { "StopKeyboard",      StopKeyboard,                         false },
  { "InitPenDrive",      [](){ initPenDrive(); },              false },
  { "StartWebServer",    [](){ WS::startWebServer(); },        false },
  { "StopWiFi",          [](){ WiFi.mode(WIFI_OFF); },         false },
  { "StopMouse",         [](){ AbsMouse.end(); WUDStatus::absmouse_begun = false; }, false },
  { "logs",              [](){ Logger::printdmesg( serialprintln ); }, false },
  { "logs-disable",      [](){ Logger::disable(); },           false },
  { "logs-enable",       [](){ Logger::enable(); },            false },
  { "logs-clear",        [](){ Logger::clear(); },             false },

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


struct asyncfspayload_t
{
  fs::FS *fs;
  const char* payloadfile;
};


static bool payload_running = false;

static void asyncPayload( void* param )
{
  if( param == NULL ) vTaskDelete(NULL);
  while( payload_running ) vTaskDelay( 100 );
  payload_running = true;
  asyncfspayload_t* aspl = (asyncfspayload_t*) param;
  runpayload( aspl->fs, aspl->payloadfile);
  payload_running = false;
  vTaskDelete(NULL);
}


void runPayloadAsync( fs::FS *fs, const char* payloadfile )
{
  static asyncfspayload_t aspl = { fs, payloadfile };
  xTaskCreate( asyncPayload, "asyncPayload", 8192, &aspl, 1, NULL );
}


void setup()
{
  //USB.onEvent(usbEventCallback);
  Keyboard.onEvent(kbdEventCallback);
  //HID.onEvent(usbEventCallback);
  WiFi.onEvent(WiFiEventCallback);
  // attach loggers to USB items, messages are deferred for later viewing with ducky "logs" command
  USBPenDriveLogger        = Logger::logsprintf;

  WS::WebServerLogger      = Logger::logmsg;     // basic logger
  WS::WebServerLogsPrinter = Logger::printdmesg; // logs history printer
  WS::HIDKeySender         = duckparser::parse;  // string quacker
  WS::runpayload           = runpayload;         // file quacker
  WS::runPayloadAsync      = runPayloadAsync;
  //Vendor._logsprintf       = Logger::logsprintf;

  MouseLogger              = Logger::logmsg;

  // attach custom ducky commands to the payload runner
  duckparser::init(&WUDDuckCommandsSet);

  // use custom ducky commands to run this skech :-)
  if( ARDUINO_USB_CDC_ON_BOOT != 0 ) {
    duckparser::parse( "SerialBegin" );
  }
  duckparser::parse( "InitKeyboard" );
  duckparser::parse( "InitMouse" );
  duckparser::parse( "InitPenDrive" );
  duckparser::parse( "StartUSB" );
  duckparser::parse( "InitLittleFS" );
  vTaskDelay(5000); // let the USB tasks finish their shift before doing critical detection stuff
  duckparser::parse( "LoadWiFiCreds" );
  duckparser::parse( "InitWiFiAP" );
  duckparser::parse( "InitWiFiSTA" );
  duckparser::parse( "StartWebServer" );


}


void loop()
{
  //if( WUDStatus::webserver_begun ) WS::server.handleClient(); // process webserver commands
  if( WUDStatus::webserver_begun ) WS::ws.cleanupClients();
  if( WUDStatus::ota_enabled ) ArduinoOTA.handle();

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
}

