/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

// If you get an error here, you probably have selected the wrong board
// under Tools > Board

#include "../USBDevices/USBDevices.h"
#include "locales.h"

namespace keyboard
{
    void setLocale( hid_layout_t* layout );

    void send(KeyReport* k);
    void release();

    void pressKey(uint8_t key, uint8_t modifiers = KEY_NONE);
    void pressModifier(uint8_t key);

    uint8_t press(const char* strPtr);

    uint8_t write(const char* c);
    void write(const char* str, size_t len);
}
