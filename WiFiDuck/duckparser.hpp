/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

#include <stddef.h> // size_t
#include "config.h"
#include "parser.h" // word_list and word_node structs


typedef void (*duckCommandCb)();

struct duckCommand
{
  const char* name;
  duckCommandCb cb;
  bool needs_args;
};

struct duckCommandSet
{
  duckCommand *commands;
  size_t count;
};


namespace duckparser
{
  void init( duckCommandSet* user_commands = nullptr );

  void parse(const char* str, size_t len); // legacy parser
  void parse( String str ); // wrapper for legacy parser except it adds a '\n' at the end of the string

  int getRepeats();
  unsigned int getDelayTime();

};
