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

#include "NTP.hpp"

#include "nvs_flash.h"


namespace NTP
{

  const char* NVS_NAMESPACE = "NTP";
  const char* NVS_KEY       = "Server";
  uint32_t nvs_handle = 0;


  void enable()
  {
    configTime(timezone*3600, daysavetime*3600, Servers[currentServer].addr, Servers[3].addr, Servers[4].addr );
    struct tm tmstruct ;
    tmstruct.tm_year = 0;

    int max_attempts = 5;
    while( !enabled && max_attempts-->0 ) {
      if( getLocalTime(&tmstruct, 5000) ) {
        Logger::logsprintf("NTP Sync: %d-%02d-%02d %02d:%02d:%02d",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
        enabled = true;
      }
      delay(500);
    }

    if( !enabled ) { // all attempts consumed
      Logger::logmsg("NTP Sync Failed");
    }
  }

  void setTimezone( float tz )
  {
    timezone = tz;
  }

  void setDst( bool set )
  {
    daysavetime = set ? 1 : 0;
  }

  void setServer( uint8_t id )
  {
    size_t servers_count = sizeof( Servers ) / sizeof( Server );

    if( id < servers_count ) {
      if( id != currentServer ) {
        currentServer = id;
        log_v("Setting NTP server to #%d ( %s / %s )", currentServer, Servers[currentServer].name, Servers[currentServer].addr );
        if (ESP_OK == nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle)) {
          if( ESP_OK == nvs_set_u8(nvs_handle, NVS_KEY, currentServer) ) {
            log_i("[NTP] saved to nvs::%s.%s=%d", NVS_NAMESPACE, NVS_KEY, currentServer);
          } else {
            log_e("[NTP] saving failed for nvs::%s.%s=%d", NVS_NAMESPACE, NVS_KEY, currentServer);
          }
          nvs_close(nvs_handle);
        } else {
          log_e("Can't open nvs::%s for writing", NVS_NAMESPACE );
        }
      }
      return;
    }
    log_e("Invalid NTP requested: #%d", id );
  }


  void loadPrefServer()
  {
    if (ESP_OK == nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle)) {
      uint8_t nvs_ntpserver = 0;
      if(ESP_OK == nvs_get_u8(nvs_handle, NVS_KEY, static_cast<uint8_t*>(&nvs_ntpserver)) ) {
        currentServer = nvs_ntpserver;
        log_i("[NTP] load from NVS: server=%d", nvs_ntpserver);
      } else {
        log_w("Can't access nvs::%s.%s", NVS_NAMESPACE, NVS_KEY );
      }
      nvs_close(nvs_handle);
    } else {
      log_e("Can't open nvs::%s for reading", NVS_NAMESPACE );
    }
  }

}
