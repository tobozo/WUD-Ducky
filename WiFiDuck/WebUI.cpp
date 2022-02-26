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


#include "WebUI.hpp"
#include "NTP.hpp"

#include "../WebUI/index_html.h"
#include "../WebUI/styles_css.h"
#include "../WebUI/script_js.h"
#include "../WebUI/disclaimer_html.h"
#include "../WebUI/credits_html.h"
#include "../WebUI/templates/info_txt.h"
#include "../WebUI/templates/template_html.h"

#include "../mustache.h"
#include "../status.h"
#include "../logger.h"

#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

extern fs::FS* duckyFS;

namespace WebUI
{

  //Moustache moustache;

  DynamicWebContent DynamicDocuments[] =
  {
    {"info",            contentTypeHtml, getSystemInfoTXT },
    {"info.json",       contentTypeJson, getSystemInfoJSON },
    {"help",            contentTypeHtml, getHelpItemsTXT },
    {"help.json",       contentTypeJson, getHelpItemsJSON },
    {"list",            contentTypeHtml, ls },
    {"logs",            contentTypeText, getLogs },
    {"index.html",      contentTypeHtml, getIndexPage },
  };

  StaticWebContent StaticDocuments[] =
  {
    //{"index.html",      contentTypeHtml, index_html },
    {"styles.css",      contentTypeCss,  styles_css},
    {"script.js",       contentTypeJs,   script_js },
    {"disclaimer.html", contentTypeHtml, disclaimer_html },
    {"credits.html",    contentTypeHtml, credits_html },
  };


  struct FileDescriptor
  {
    bool is_dir;
    const char* path;
    size_t size;
    bool readonly;
    void toJson( String &output)
    {
      char lastchar = output[output.length()-1];
      if (lastchar == '}' || lastchar == ']' ) {
        // last char was a closing bracket, insert comma
        output += ',';
      }



      StaticJsonDocument<128> fileInfoDoc;
      fileInfoDoc["name"]     = String(path);
      fileInfoDoc["type"]     = (is_dir) ? "dir" : "file";
      fileInfoDoc["size"]     = size;

      fileInfoDoc["readonly"] = (readonly) ? true : false;
      serializeJson(fileInfoDoc, output);

/*
      output += "{\"type\":\"";
      output += (is_dir) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(path);
      output += "\",\"size\":";
      output += String(size);
      output += ",\"readonly\":";
      output += (readonly) ? "true" : "false";
      output += "}";
*/
    }
  };



  void getSystemInfo( String *output, output_format format )
  {
    using namespace WUDStatus;
    float flashFreqFloat = (float)ESP.getFlashChipSpeed() / 1000.0 / 1000.0;
    FlashMode_t _ideMode = ESP.getFlashChipMode();

    String SketchMD5       = ESP.getSketchMD5();
    String flashFreq       = String( flashFreqFloat );
    String ideMode         = String( _ideMode == FM_QIO ? "QIO" : _ideMode == FM_QOUT ? "QOUT" : _ideMode == FM_DIO ? "DIO" : _ideMode == FM_DOUT ? "DOUT" : "UNKNOWN" );
    String SdkVersion      = String( ESP.getSdkVersion() );
    String ChipModel       = String( ESP.getChipModel() );
    String CpuFreqMHz      = String( ESP.getCpuFreqMHz() );
    String FlashChipMode   = String( ESP.getFlashChipMode() );
    String FlashChipSize   = Logger::formatBytes( ESP.getFlashChipSize() );
    String SketchSize      = Logger::formatBytes( ESP.getSketchSize() );
    String FreeSketchSpace = Logger::formatBytes( ESP.getFreeSketchSpace() );
    String HeapSize        = Logger::formatBytes( ESP.getHeapSize() );
    String FreeHeap        = Logger::formatBytes( ESP.getFreeHeap() );
    String MinFreeHeap     = Logger::formatBytes( ESP.getMinFreeHeap() );
    String MaxAllocHeap    = Logger::formatBytes( ESP.getMaxAllocHeap() );
    String ProductId       = "0x" + String( USB.PID(), HEX );
    String VendorId        = "0x" + String( USB.VID(), HEX );
    String MacAddr         = WiFi.macAddress();
    String IpAddr          = WiFi.localIP().toString();

    const char* productName      = USB.productName();
    const char* manufacturerName = USB.manufacturerName();
    const char* serialNumber     = USB.serialNumber();

    const char* SerialDebug = ARDUINO_USB_CDC_ON_BOOT==0 ? "UART0" : "USBSerial";

    const char* online     = "online";
    const char* offline    = "offline";
    const char* on         = "on";
    const char* off        = "off";
    const char* enabled    = "enabled";
    const char* disabled   = "disabled";
    const char* started    = "started";
    const char* stopped    = "stopped";

    const char* fakepass = "*********";

    Poil tplvalues[] =
    {
      { "getSdkVersion"      , SdkVersion.c_str()      },
      { "getChipModel"       , ChipModel.c_str()       },
      { "getCpuFreqMHz"      , CpuFreqMHz.c_str()      },
      { "getFlashChipMode"   , FlashChipMode.c_str()   },
      { "getFlashChipSize"   , FlashChipSize.c_str()   },
      { "flashFreq"          , flashFreq.c_str()       },
      { "ideMode"            , ideMode.c_str()         },
      { "getSketchMD5"       , SketchMD5.c_str()       },
      { "getSketchSize"      , SketchSize.c_str()      },
      { "getFreeSketchSpace" , FreeSketchSpace.c_str() },
      { "getHeapSize"        , HeapSize.c_str()        },
      { "getFreeHeap"        , FreeHeap.c_str()        },
      { "getMinFreeHeap"     , MinFreeHeap.c_str()     },
      { "getMaxAllocHeap"    , MaxAllocHeap.c_str()    },
      { "USB_VID"            , ProductId.c_str()       },
      { "USB_PID"            , VendorId.c_str()        },
      { "USB_MANUFACTURER"   , manufacturerName        },
      { "USB_PRODUCT"        , productName             },
      { "USB_SERIAL"         , serialNumber            },
      { "SerialDebug"        , SerialDebug             },
      { "usb_begun"          , usb_begun       ?started:stopped },
      { "hwserial_begun"     , hwserial_begun  ?started:stopped },
      { "usbserial_begun"    , usbserial_begun ?started:stopped },
      { "keyboard_begun"     , keyboard_begun  ?started:stopped },
      { "absmouse_begun"     , absmouse_begun  ?started:stopped },
      { "hid_ready"          , hid_ready       ?on:off },
      { "sd_begun"           , sd_begun        ?enabled:disabled },
      { "pendrive_begun"     , pendrive_begun  ?enabled:disabled },
      { "spiffs_begun"       , spiffs_begun    ?enabled:disabled },
      { "webserver_begun"    , webserver_begun ?online:offline },
      { "logging_enabled"    , Logger::enabled ?enabled:disabled },
      { "ota_enabled"        , WUDStatus::ota_enabled ?enabled:disabled },
      { "numlock_on"         , numlock_on      ?on:off },
      { "capslock_on"        , capslock_on     ?on:off },
      { "scrolllock_on"      , scrolllock_on   ?on:off },
      { "MAC_ADDR"           , MacAddr.c_str()         },
      { "STA_ADDR"           , IpAddr.c_str()          },
      { "softap_begun"       , softap_begun ?online:offline    },
      { "HOST_NAME"          , USB.productName()       },
      { "AP_SSID"            , AP_SSID                 },
      { "AP_PASSWORD"        , fakepass/*AP_PASSWORD*/ },
      { "wifista_begun"      , wifista_begun ?started:stopped   },
      { "STA_SSID"           , STA_SSID                },
      { "STA_PASSWORD"       , fakepass/*STA_PASSWORD*/},
      { "ntp_enabled"        , NTP::enabled ?enabled:disabled },
      { "NTP_ZONE"           , NTP::Servers[NTP::currentServer].name },
      { "NTP_SERVERADDR"     , NTP::Servers[NTP::currentServer].addr },

    };

    size_t markers_count =  sizeof(tplvalues)/sizeof(Poil);

    if( format == SYSINFO_TXT ) {
      Poils barbe = { tplvalues, markers_count };
      Moustache moustache;
      moustache.set( info_txt, output, &barbe );
      moustache.parse();
    } else {
      // SYSINFO_JSON
      StaticJsonDocument<1024> sysInfoDoc;
      for( int i=0; i<markers_count; i++ ) {
        sysInfoDoc[tplvalues[i].name] = tplvalues[i].value;
      }
      //serializeJsonPretty(sysInfoDoc, *output);
      serializeJson(sysInfoDoc, *output);
    }
  }


  void getSystemInfoTXT( String *output )  { getSystemInfo( output, SYSINFO_TXT ); }
  void getSystemInfoJSON( String *output ) { getSystemInfo( output, SYSINFO_JSON ); }


  void getIndexPage( String *output )
  {
    if( WUDStatus::disclaimer_done ) {
      *output += String( index_html );
    } else {
      *output += String( disclaimer_html );
      WUDStatus::disclaimer_done = true;
    }
  }



  void getHelpItems( String *output, output_format format )
  {
    using namespace duckparser;
    #define TITLE_NAMEDKEYS "Legacy WiFiDuck named keys"
    #define TITLE_LEGACYCMD "Legacy WiFiDuck commands"
    #define TITLE_ESPWUDCMD "WUD Ducky commands"
    //if( !WUDStatus::usbserial_begun ) return;

    String mc = ""; // macro separator

    switch( format ) {
      case SYSINFO_TXT:
        *output += mc + TITLE_NAMEDKEYS+ ":\n";
        for( int i=0;i<keys->count;i++ ) {
          if( i%8==0 ) *output += "\n"; // USBSerial.println();
          *output += " " + String(keys->commands[i].name);
        }
        *output += "\n\n";
        *output += mc + TITLE_LEGACYCMD + ":\n";
        *output += "  - REM\n";
        *output += "  - STRING\n";
        for( int i=0;i<legacy_commands->count;i++ ) {
          *output += "  - " + String( legacy_commands->commands[i].name ) + "\n";
        }
        *output += "\n";
        *output += mc + TITLE_ESPWUDCMD + ":\n";
        for( int i=0;i<custom_commands->count;i++ ) {
          *output += "  - " + String( custom_commands->commands[i].name ) + "\n";
        }
        *output += "\n";
        break;
      case SYSINFO_JSON:
        StaticJsonDocument<2048> duckyCommandsDoc;
        JsonArray legacynk  = duckyCommandsDoc.createNestedArray("legacy-named-keys");
        JsonArray legacycmd = duckyCommandsDoc.createNestedArray("legacy-commands");
        JsonArray wudcmd    = duckyCommandsDoc.createNestedArray("wud-commands");
        for( int i=0;i<keys->count;i++ ) {
          if( i%8==0 ) *output += "\n"; // USBSerial.println();
          legacynk.add( keys->commands[i].name );
        }
        legacycmd.add( "REM" );
        legacycmd.add( "STRING" );
        for( int i=0;i<legacy_commands->count;i++ ) {
          legacynk.add( legacy_commands->commands[i].name );
        }
        for( int i=0;i<custom_commands->count;i++ ) {
          wudcmd.add( custom_commands->commands[i].name );
        }
        //serializeJsonPretty(duckyCommandsDoc, *output);
        serializeJson(duckyCommandsDoc, *output);
      break;
    }
  }




  void getHelpItemTXT( String *output, const char* _key )
  {
    using namespace duckparser;
    String keyStr = String(_key);
    keyStr.trim();
    const char* key = keyStr.c_str();
    *output += "help: " + keyStr + "\n\n\t";
    //Logger::logsprintf("Searching help text for command '%s'", key );
    for( int i=0;i<keys->count;i++ ) {
      if( strcmp( keys->commands[i].name, key ) == 0 ) {
        if( keys->commands[i].help_text )
          *output += String( keys->commands[i].help_text ) + "\n";
        return;
      }
    }
    for( int i=0;i<legacy_commands->count;i++ ) {
      if( strcmp( legacy_commands->commands[i].name, key ) == 0 ) {
        if( legacy_commands->commands[i].help_text )
          *output += String( legacy_commands->commands[i].help_text ) + "\n";
        return;
      }
    }
    for( int i=0;i<custom_commands->count;i++ ) {
      if( strcmp( custom_commands->commands[i].name, key ) == 0 ) {
        if( custom_commands->commands[i].help_text )
          *output += String( custom_commands->commands[i].help_text ) + "\n";
        return;
      }
    }
    // ducks can troll too
    *output += "I know that, I just don't want to share it :p\n";
  }

  void getHelpItemsTXT( String *output )  { getHelpItems(output, SYSINFO_TXT ); }
  void getHelpItemsJSON( String *output ) { getHelpItems(output, SYSINFO_JSON ); }



  void fileToJson( fs::File *file, String &output )
  {
    FileDescriptor fDesc = { file->isDirectory(), file->path(), file->size(), false };
    fDesc.toJson( output );
  }

  void staticFilesToJson( String &output )
  {
    size_t docs_count = sizeof(StaticDocuments)/sizeof(StaticWebContent);
    for( int i=0; i<docs_count; i++ ) {
      String path = "/" + String( StaticDocuments[i].basepath );
      if( !LittleFS.exists( path ) ) { // already listed and editable
        FileDescriptor fDesc = { false, path.c_str(), strlen(StaticDocuments[i].content), true };
        fDesc.toJson( output );
      }
    }
  }

  void lsJson( const char* path, String &output )
  {
    fs::File root = duckyFS->open(path);

    String fsname = duckyFS==&LittleFS ? "spiffs" : "sd";

    output += "{\"filesystem\":\""+fsname+"\"";
    output +=   ",\"files\":[";
    if (root && root.isDirectory()) {
      fs::File file = root.openNextFile();
      while (file) {
        fileToJson( &file, output );
        file = root.openNextFile();
      }
    }
    if( duckyFS==&LittleFS ) {
      // also list files from flash memory in case those haven't been customized to LittleFS
      staticFilesToJson( output );
    }
    output += "]";

    output += ",\"commands\":";
    getHelpItemsJSON( &output );

    output += ",\"sysinfo\":";
    getSystemInfoJSON( &output );

    output += "}";
  }


  void ls( String *output )
  {
    using namespace WS;
    String path = "/";
    //     if (!server.hasArg("dir")) {
    //       path = "/";
    //     } else {
    //       path = server.arg("dir");
    //     }
    lsJson( path.c_str(), *output );
  }


  void getLogs( String *output )
  {
    using namespace WS;
    if(WebServerLogsPrinter) {
      logoutput = "";
      WebServerLogsPrinter( logprinter, false );
      *output = logoutput;
    }
  }



};

