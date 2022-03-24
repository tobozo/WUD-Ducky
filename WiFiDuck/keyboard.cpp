/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */
#pragma once

#include "keyboard.hpp"


namespace keyboard
{

  hid_layout_t layouts[] =
  {
    { "US",     &locale_us },
    { "DE",     &locale_de },
    { "RU",     &locale_ru },
    { "GB",     &locale_gb },
    { "ES",     &locale_es },
    { "FR",     &locale_fr },
    { "DK",     &locale_dk },
    { "BE",     &locale_be },
    { "PT",     &locale_pt },
    { "IT",     &locale_it },
    { "SK",     &locale_sk },
    { "CZ",     &locale_cz },
    { "SI",     &locale_si },
    { "BG",     &locale_bg },
    { "CA-FR",  &locale_cafr },
    { "CH-DE",  &locale_chde },
    { "CH-FR",  &locale_chfr },
    { "HU",     &locale_hu }
  };


  // ====== PRIVATE ====== //
  const char* localecode = layouts[0].code;
  hid_locale_t* locale   = layouts[0].locale;
  KeyReport prev_report = KeyReport { KEY_NONE, KEY_NONE, { KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE } };


  KeyReport makeReport(uint8_t modifiers = 0, uint8_t key1 = 0, uint8_t key2 = 0, uint8_t key3 = 0, uint8_t key4 = 0, uint8_t key5 = 0, uint8_t key6 = 0)
  {
    KeyReport k =
    {
      .modifiers = modifiers,
      .reserved  = 0x00,
      .keys      = { key1, key2, key3, key4, key5, key6 }
    };

    return k;
  }


  void setLocale( hid_layout_t* layout )
  {
    locale     = layout->locale;
    localecode = layout->code;
  }


  void send(KeyReport* k)
  {
    Keyboard.sendReport(k);
  }


  void release()
  {
    prev_report = makeReport();
    Keyboard.sendReport(&prev_report);
  }


  void pressKey(uint8_t key, uint8_t modifiers)
  {
    for (uint8_t i = 0; i<6; ++i) {
      if (prev_report.keys[i] == KEY_NONE) {
        prev_report.modifiers |= modifiers;
        prev_report.keys[i]    = key;
        Keyboard.sendReport(&prev_report);
        return;
      }
    }
  }


  void pressModifier(uint8_t key)
  {
    prev_report.modifiers |= key;

    Keyboard.sendReport(&prev_report);
  }


  uint8_t press(const char* strPtr)
  {
    // Convert string pointer into a byte pointer
    uint8_t* b = (uint8_t*)strPtr;

    // Key combinations (accent keys)
    // We have to check them first, because sometimes ASCII keys are in here
    for (uint8_t i = 0; i<locale->combinations_len; ++i) {
      uint8_t res = 0;

      // Read utf8 code and match it with the given data
      for (uint8_t j = 0; j<4; ++j) {
        uint8_t key_code = pgm_read_byte(locale->combinations + (i * 8) + j);

        if (key_code == 0) {
          break;
        }

        if (key_code == b[j]) {
          ++res;
        } else {
          res = 0;
          break;
        }
      }

      // If a match was found, read out the data and type it
      if (res > 0) {
        uint8_t comboModifiers = pgm_read_byte(locale->combinations + (i * 8) + 4);
        uint8_t comboKey       = pgm_read_byte(locale->combinations + (i * 8) + 5);

        uint8_t modifiers = pgm_read_byte(locale->combinations + (i * 8) + 6);
        uint8_t key       = pgm_read_byte(locale->combinations + (i * 8) + 7);

        pressKey(comboKey, comboModifiers);
        release();
        pressKey(key, modifiers);
        release();

        // Return the number of extra bytes we used from the string pointer
        return res-1;
      }
    }

    // ASCII
    if (b[0] < locale->ascii_len) {
      uint8_t modifiers = pgm_read_byte(locale->ascii + (b[0] * 2) + 0);
      uint8_t key       = pgm_read_byte(locale->ascii + (b[0] * 2) + 1);

      pressKey(key, modifiers);

      return 0;
    }

    // UTF8
    for (size_t i = 0; i<locale->utf8_len; ++i) {
      uint8_t res = 0;

      // Read utf8 code and match it with the given data
      for (uint8_t j = 0; j<4; ++j) {
        uint8_t key_code = pgm_read_byte(locale->utf8 + (i * 6) + j);

        if (key_code == 0) {
          break;
        }

        if (key_code == b[j]) {
          ++res;
        } else {
          res = 0;
          break;
        }
      }

      // If a match was found, read out the data and type it
      if (res > 0) {
        uint8_t modifiers = pgm_read_byte(locale->utf8 + (i * 6) + 4);
        uint8_t key       = pgm_read_byte(locale->utf8 + (i * 6) + 5);

        pressKey(key, modifiers);

        // Return the number of extra bytes we used from the string pointer
        return res-1;
      }
    }

    return 0;
  }


  uint8_t write(const char* c)
  {
    uint8_t res = press(c);

    release();

    return res;
  }


  void write(const char* str, size_t len)
  {
    for (size_t i = 0; i<len; ++i) {
      i += write(&str[i]);
    }
  }
}
