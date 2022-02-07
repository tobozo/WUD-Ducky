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

#include "prefs.hpp"


namespace NTP
{

  //const char* PREF_NAMESPACE = "NTP";
  const char* NVS_TZ_KEY      = "TZ";
  const char* NVS_DST_KEY     = "DST";
  const char* NVS_NTPZONE_KEY = "NTPZONE";
  uint32_t nvs_handle = 0;


  void enable()
  {
    configTime(timezone*3600, daysavetime*3600, Servers[currentServer].addr, Servers[3].addr, Servers[4].addr );
    struct tm tmstruct ;
    tmstruct.tm_year = 0;

    int max_attempts = 5;
    while( !enabled && max_attempts-->0 ) {
      if( getLocalTime(&tmstruct, 5000) ) {
        Logger::logsprintf("[NTP][%s][%s] Sync: %d-%02d-%02d %02d:%02d:%02d", Servers[currentServer].name, Servers[currentServer].addr, (tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
        enabled = true;
      }
      delay(500);
    }

    if( !enabled ) { // all attempts consumed
      Logger::logsprintf("[NTP][%s][%s] Sync Failed with server #%d", Servers[currentServer].name, Servers[currentServer].addr, currentServer );
    }
  }

  void setTimezone( float tz )
  {
    timezone = tz;
    prefs::setFloat( NVS_TZ_KEY, timezone );
  }

  void setDst( bool set )
  {
    daysavetime = set ? 1 : 0;
    prefs::setUChar( NVS_DST_KEY, daysavetime );
  }

  void setServer( uint8_t id )
  {
    size_t servers_count = sizeof( Servers ) / sizeof( Server );

    if( id < servers_count ) {
      if( id != currentServer ) {
        currentServer = id;
        Logger::logsprintf("Setting NTP server to #%d ( %s / %s )", currentServer, Servers[currentServer].name, Servers[currentServer].addr );
        prefs::setUChar( NVS_NTPZONE_KEY, currentServer );
        /*
        if (ESP_OK == nvs_open(PREF_NAMESPACE, NVS_READWRITE, &nvs_handle)) {
          if( ESP_OK == nvs_set_u8(nvs_handle, NVS_KEY, currentServer) ) {
            Logger::logsprintf("[NTP] saved to nvs::%s.%s=%d", PREF_NAMESPACE, NVS_KEY, currentServer);
          } else {
            Logger::logsprintf("[NTP] saving failed for nvs::%s.%s=%d", PREF_NAMESPACE, NVS_KEY, currentServer);
          }
          nvs_close(nvs_handle);
        } else {
          Logger::logsprintf("Can't open nvs::%s for writing", PREF_NAMESPACE );
        }
        */
      }
      return;
    }
    Logger::logsprintf("Invalid NTP requested: #%d", id );
  }


  void loadPrefs()
  {

    prefs::getUChar( NVS_NTPZONE_KEY, &currentServer, currentServer );
    prefs::getUChar( NVS_DST_KEY,     &daysavetime,   daysavetime );
    prefs::getFloat( NVS_TZ_KEY,      &timezone,      timezone );
    /*
    if (ESP_OK == nvs_open(PREF_NAMESPACE, NVS_READONLY, &nvs_handle)) {
      uint8_t nvs_ntpserver = 0;
      if(ESP_OK == nvs_get_u8(nvs_handle, NVS_KEY, static_cast<uint8_t*>(&nvs_ntpserver)) ) {
        currentServer = nvs_ntpserver;
        Logger::logsprintf("[NTP] load from NVS: server=%d", nvs_ntpserver);
      } else {
        Logger::logsprintf("Can't access nvs::%s.%s", PREF_NAMESPACE, NVS_KEY );
      }
      nvs_close(nvs_handle);
    } else {
      Logger::logsprintf("Can't open nvs::%s for reading", PREF_NAMESPACE );
    }*/
  }

}
