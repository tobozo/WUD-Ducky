#pragma once

#include "prefs.hpp"

#include <Preferences.h>
#include "../logger.h"

Preferences _prefs;

namespace prefs
{

  void reset()
  {
     _prefs.begin("WUD", false );
     _prefs.clear();
     _prefs.end();
  }

  void set( const char *name, const char *value, size_t len )
  {
    _prefs.begin("WUD", false );
    char buf[len+2] = {0};
    snprintf( buf, len+1, "%s", value );
    if( _prefs.putString(name, buf) ) {
      logsprintf("Pref saved: %s=%s", name, buf );
    } else {
      logsprintf("Pref saving failed! %s=%s", name, value );
    }
    _prefs.end();
  }
  void get( const char *name, char *dest, size_t max_len, const char *default_value )
  {
    _prefs.begin("WUD", true );
    size_t len = _prefs.getString(name, dest, max_len );
    if( len > 0 ) {
      logsprintf("Pref thawed: %s='%s' (default='%s')", name, dest, default_value );
    } else {
      len = strlen(default_value)+1;
      snprintf( dest, max_len, "%s", default_value );
      logsprintf("Pref defaulted: %s='%s' (default='%s')", name, dest, default_value );
    }
    _prefs.end();
  }

};
