/*\
 *
 * ESP32-S2 WUD-Ducky
 *
 * Copyright (c) 2021 tobozo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
\*/

#pragma once

#include "led_controls.hpp"


namespace ledControl
{
  int led_pin = BUILTIN_LED_PIN;
  bool enabled = false;
  bool blinking = false;
  unsigned long blinkdelay_on  = 500;
  unsigned long blinkdelay_off = 300;
  TaskHandle_t blinkTaskHandle = NULL;
  void set( bool enable )
  {
    blinking = false;
    if( enabled == enable ) return;
    enabled = enable;
  }
  void toggle()
  {
    enabled = !enabled;
  }
  void blink( bool enable_blink )
  {
    blinking = enable_blink;
  }
  void blink( unsigned long on, unsigned long off )
  {
    blinkdelay_on  = on;
    blinkdelay_off = off;
    blink( true );
  }
  void ledTask( void* param )
  {
    bool laststate = false;
    unsigned long lastblink = 0;
    while(1) {
      if( blinking ) {
        unsigned long since_last_blink = millis() - lastblink;
        unsigned long that_delay = (enabled?blinkdelay_on:blinkdelay_off);
        if( since_last_blink > that_delay ) {
          toggle();
          lastblink = millis();
        }
      }
      if( laststate != enabled ) {
         digitalWrite( led_pin, enabled ? LOW : HIGH );
         laststate = enabled;
      }
      vTaskDelay( 200 ); // 5 ticks per second
    }
  }
  void init()
  {
    // enable builtin led
    pinMode( led_pin, OUTPUT );
    if( blinkTaskHandle == NULL ) {
      xTaskCreate( ledTask, "LED Task", 1000, NULL, 1, &blinkTaskHandle );
    }
  }
  void deinit()
  {
    vTaskDelete( blinkTaskHandle );
    vTaskDelay(100);
    blinkTaskHandle = NULL;
  }
};

