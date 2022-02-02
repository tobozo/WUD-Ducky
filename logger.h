#pragma once

#include <vector>


namespace Logger
{
  std::vector<String> dmesg; // ring buffer
  int max_log_len = 4096; // max bytes in ring buffer
  int log_len = 0; // bytes in ring buffer

  typedef void(*logprintercb)( String msg );
  bool enabled = true;


  void clear()
  {
    dmesg.clear();
  }

  void enable()
  {
    enabled = true;
  }

  void disable()
  {
    enabled = false;
    clear();
  }

  // log to ring buffer
  void push( String msg )
  {
    if( !enabled ) return;
    size_t msg_len = msg.length() + 1;
    if( msg_len > max_log_len ) return;
    while( msg_len + log_len > max_log_len && dmesg.size() > 0 ) {
      log_len -= dmesg[0].length() + 1;
      dmesg.erase (dmesg.begin());
    }
    dmesg.push_back( msg );
    log_len += msg_len;
  }

  // log String style
  void logmsg( String msg )
  {
    if( !enabled ) return;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t now = (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
    // format date and prefix message
    char strftime_buf[64];
    uint32_t s = now/1000;
    uint32_t m = now/60000;
    uint32_t h = now/(60000*60);
    uint32_t S  = s%60;
    uint32_t M  = m%60;
    uint32_t H  = h%60;
    uint32_t SS = now%1000;
    sprintf(strftime_buf, "%02d:%02d:%02d.%03d", H, M, S, SS );
    msg = "[@" + String( strftime_buf) + "] " + msg;
    push( msg );
  }

  // log printf style
  void logsprintf( const char *fmt, ... )
  {
    if( !enabled ) return;
    char buf[256];
    va_list va;
    va_start (va, (char*)fmt);
    vsnprintf (buf, 256, fmt, va);
    va_end (va);
    logmsg( String( buf ) );
  }

  // dump logs using callback
  void printdmesg( logprintercb logprint, bool clear_after=false )
  {
    if( !enabled ) return;
    if( dmesg.size() > 0 ) {
      for( int i=0; i<dmesg.size(); i++ ) {
        logprint( dmesg[i] );
      }
      if( clear_after ) clear();
    }
  }



};
