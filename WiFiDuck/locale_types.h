/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

struct hid_locale_t
{
  uint8_t* ascii;
  uint8_t  ascii_len;

  uint8_t* extended_ascii;
  size_t   extended_ascii_len;

  uint8_t* utf8;
  size_t   utf8_len;

  uint8_t* combinations;
  size_t   combinations_len;
};



struct hid_layout_t
{
  const char* code;
  hid_locale_t* locale;
};
