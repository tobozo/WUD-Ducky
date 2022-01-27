#pragma once

std::vector<String> dmesg;
typedef void(*logprintercb)( String msg );


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
    if( dmesg.size() > 64 ) {
      if( clear_after ) dmesg.clear();
    }
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


String formatBytes(size_t bytes){
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




#include "index.h"
#include "mustache.h"
#include "status.h"
#include <WiFi.h>


void getSystemInfo( String &output, bool html = true )
{

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

  if( html == true ) moustache.load( info_html );
  else moustache.load( info_txt );
  moustache.parse( tplvalues, markers_count );
  output = String( moustache.tpl );
}
