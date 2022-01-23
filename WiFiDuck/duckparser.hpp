/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

#include <stddef.h> // size_t
#include "config.h"
#include "parser.h" // word_list and word_node structs


typedef void (*duckPayloadCharCb)(char c, uint32_t wait);
typedef void (*duckPayloadChunkCb)(char *c);
typedef void (*duckCommandCb)();


struct duckCommand
{
  const char* name;
  duckCommandCb cb;
};



namespace duckparser
{
    void parse(const char* str, size_t len);
    void parse( String str );
    int getRepeats();
    unsigned int getDelayTime();

    void setCommands(duckCommand* _commands, size_t count);
    duckCommand* commands;
    size_t commands_count;

    duckCommand* keys;
    size_t keys_count;


};
