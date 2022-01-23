/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */
#pragma once

#include "keyboard.hpp"


namespace keyboard
{
    // ====== PRIVATE ====== //
    hid_locale_t* locale      { &locale_us };

    KeyReport prev_report = KeyReport { KEY_NONE, KEY_NONE, { KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE } };

    KeyReport makeReport(uint8_t modifiers = 0, uint8_t key1 = 0, uint8_t key2 = 0, uint8_t key3 = 0, uint8_t key4 = 0, uint8_t key5 = 0, uint8_t key6 = 0);

    KeyReport makeReport(uint8_t modifiers, uint8_t key1, uint8_t key2, uint8_t key3, uint8_t key4, uint8_t key5, uint8_t key6)
    {
        KeyReport k;

        k.modifiers = modifiers;

        k.reserved = 0x00;

        k.keys[0] = key1;
        k.keys[1] = key2;
        k.keys[2] = key3;
        k.keys[3] = key4;
        k.keys[4] = key5;
        k.keys[5] = key6;

        return k;
    }

    void setLocale(hid_locale_t* locale)
    {
        keyboard::locale = locale;
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

        // Extended ASCII
        for (uint8_t i = 0; i<locale->extended_ascii_len; ++i) {
            uint8_t key_code = pgm_read_byte(locale->extended_ascii + (i * 3));

            if (b[0] == key_code) {
                uint8_t modifiers = pgm_read_byte(locale->extended_ascii + (i * 3) + 1);
                uint8_t key       = pgm_read_byte(locale->extended_ascii + (i * 3) + 2);

                pressKey(key, modifiers);

                return 0;
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
