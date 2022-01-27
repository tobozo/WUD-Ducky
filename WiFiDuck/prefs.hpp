#pragma once

namespace prefs
{
  void reset();
  void set( const char *name, const char *value );
  void getget( const char *name, char *dest, size_t max_len, const char *default_value );
};
