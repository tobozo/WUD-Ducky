#pragma once

#include <vector>


namespace Logger
{
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

};
