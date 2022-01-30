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

#include "../WebUI/index_html.h"
#include "../WebUI/styles_css.h"
#include "../WebUI/script_js.h"
#include "../WebUI/disclaimer_html.h"
#include "../WebUI/credits_html.h"
#include "../WebUI/templates/info_html.h"
#include "../WebUI/templates/info_txt.h"
#include "../WebUI/templates/template_html.h"

#include "../mustache.h"
#include "../status.h"
#include "../logger.h"

#include <WiFi.h>
#include <SPIFFS.h>

extern fs::FS* duckyFS;

namespace WebUI
{

  //Moustache moustache;

  DynamicWebContent DynamicDocuments[] =
  {
    {"info",            contentTypeHtml, getSystemInfoHTML },
    {"list",            contentTypeHtml, ls },
    {"logs",            contentTypeText, getLogs },
  };

  StaticWebContent StaticDocuments[] =
  {
    {"index.html",      contentTypeHtml, index_html },
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
      output += "{\"type\":\"";
      output += (is_dir) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(path);
      output += "\",\"size\":";
      output += String(size);
      output += ",\"readonly\":";
      output += (readonly) ? "true" : "false";
      output += "}";
    }
  };


  String formatBytes(size_t bytes)
  {
    if (bytes < 1024){
      return String(bytes)+" B";
    } else if(bytes < (1024 * 1024)){
      return String(bytes/1024.0)+" KB";
    } else if(bytes < (1024 * 1024 * 1024)){
      return String(bytes/1024.0/1024.0)+" MB";
    } else {
      return String(bytes/1024.0/1024.0/1024.0)+" GB";
    }
  }

  void getSystemInfo( String *output, bool html )
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
    String FlashChipSize   = formatBytes( ESP.getFlashChipSize() );
    String SketchSize      = formatBytes( ESP.getSketchSize() );
    String FreeSketchSpace = formatBytes( ESP.getFreeSketchSpace() );
    String HeapSize        = formatBytes( ESP.getHeapSize() );
    String FreeHeap        = formatBytes( ESP.getFreeHeap() );
    String MinFreeHeap     = formatBytes( ESP.getMinFreeHeap() );
    String MaxAllocHeap    = formatBytes( ESP.getMaxAllocHeap() );
    String ProductId       = "0x" + String( USB_PID, HEX );
    String VendorId        = "0x" + String( USB_VID, HEX );
    String MacAddr         = WiFi.macAddress();
    String IpAddr          = WiFi.localIP().toString();

    const char* on       = "online";
    const char* off      = "offline";
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
      { "USB_MANUFACTURER"   , USB_MANUFACTURER        },
      { "USB_PRODUCT"        , USB_PRODUCT             },
      { "USB_SERIAL"         , USB_SERIAL              },
      { "usb_begun"          , usb_begun       ?on:off },
      { "hwserial_begun"     , hwserial_begun  ?on:off },
      { "usbserial_begun"    , usbserial_begun ?on:off },
      { "keyboard_begun"     , keyboard_begun  ?on:off },
      { "absmouse_begun"     , absmouse_begun  ?on:off },
      { "hid_ready"          , hid_ready       ?on:off },
      { "sd_begun"           , sd_begun        ?on:off },
      { "pendrive_begun"     , pendrive_begun  ?on:off },
      { "spiffs_begun"       , spiffs_begun    ?on:off },
      { "webserver_begun"    , webserver_begun ?on:off },

      { "MAC_ADDR"           , MacAddr.c_str()         },
      { "STA_ADDR"           , IpAddr.c_str()          },
      { "softap_begun"       , softap_begun ?on:off    },
      { "AP_SSID"            , AP_SSID                 },
      { "AP_PASSWORD"        , fakepass/*AP_PASSWORD*/ },
      { "wifista_begun"      , wifista_begun ?on:off   },
      { "STA_SSID"           , STA_SSID                },
      { "STA_PASSWORD"       , fakepass/*STA_PASSWORD*/},

    };

    size_t markers_count =  sizeof(tplvalues)/sizeof(Poil);

    Moustache moustache;
    moustache.set( output );
    if( html == true ) moustache.load( info_html );
    else moustache.load( info_txt );
    moustache.parse( tplvalues, markers_count );
  }


  void getSystemInfoHTML( String *output )
  {
    getSystemInfo( output, true );
  }


  void getSystemInfoTXT( String *output )
  {
    getSystemInfo( output, false );
  }


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
      if( !SPIFFS.exists( path ) ) { // already listed and editable
        FileDescriptor fDesc = { false, path.c_str(), strlen(StaticDocuments[i].content), true };
        fDesc.toJson( output );
      }
    }
  }

  void lsJson( const char* path, String &output )
  {
    fs::File root = duckyFS->open(path);

    String fsname = duckyFS==&SPIFFS ? "spiffs" : "sd";

    output = "{\"filesystem\":\""+fsname+"\", \"files\":[";

    if (root && root.isDirectory()) {
      fs::File file = root.openNextFile();
      while (file) {
        fileToJson( &file, output );
        file = root.openNextFile();
      }
    }
    if( duckyFS==&SPIFFS ) {
      // also list files from flash memory in case those haven't been customized to SPIFFS
      staticFilesToJson( output );
    }
    output += "]}";
  }


  void ls( String *output )
  {
    using namespace WS;
    String path;
    if (!server.hasArg("dir")) {
      path = "/";
    } else {
      path = server.arg("dir");
    }
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

