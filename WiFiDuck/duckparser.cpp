/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

#include "duckparser.hpp"

#include "keyboard.cpp"
#include "mouse.cpp"


namespace duckparser
{
    // ====== PRIVATE ===== //
    bool inString  = false;
    bool inComment = false;

    int defaultDelay = 5;
    int repeatNum    = 0;

    unsigned long interpretTime  = 0;
    unsigned long sleepStartTime = 0;
    unsigned long sleepTime      = 0;

    void type(const char* str, size_t len)
    {
        keyboard::write(str, len);
    }

    void press(const char* str, size_t len)
    {
        // character
        if (len == 1) {
          keyboard::press(str);
          return;
        }
        int keyCommand = -1;
        for( int i=0;i<keys_count;i++) {
          if (compare(str, len, keys[i].name, CASE_SENSITIVE)) {
            keyCommand = i;
            break;
          }
        }
        if( keyCommand > -1 ) keys[keyCommand].cb();
        else keyboard::press(str); // Utf8 character
    }

    void release()
    {
        keyboard::release();
    }


    unsigned int toInt(const char* str, size_t len)
    {
        if (!str || (len == 0)) return 0;

        unsigned int val = 0;

        // HEX
        if ((len > 2) && (str[0] == '0') && (str[1] == 'x')) {
            for (size_t i = 2; i < len; ++i) {
                uint8_t b = str[i];

                if ((b >= '0') && (b <= '9')) b = b - '0';
                else if ((b >= 'a') && (b <= 'f')) b = b - 'a' + 10;
                else if ((b >= 'A') && (b <= 'F')) b = b - 'A' + 10;

                val = (val << 4) | (b & 0xF);
            }
        }
        // DECIMAL
        else {
            for (size_t i = 0; i < len; ++i) {
                if ((str[i] >= '0') && (str[i] <= '9')) {
                    val = val * 10 + (str[i] - '0');
                }
            }
        }

        return val;
    }

    void sleep(unsigned long time)
    {
        unsigned long offset = millis() - interpretTime;

        if (time > offset) {
            sleepStartTime = millis();
            sleepTime      = time - offset;

            delay(sleepTime);
        }
    }

    // ====== PUBLIC ===== //

    void setKeys(duckCommand* _keys, size_t count)
    {
      keys = _keys;
      keys_count = count;
    }


    void setCommands(duckCommand* _commands, size_t count)
    {
      commands = _commands;
      commands_count = count;
    }

    void parse( String str )
    {
      if( !str.endsWith("\n" ) ) str += "\n"; // append line end
      parse( str.c_str(), str.length() );
    }

    void parse(const char* str, size_t len)
    {
        interpretTime = millis();

        // Split str into a list of lines
        line_list* l = parse_lines(str, len);

        // Go through all lines
        line_node* n = l->first;

        // Flag, no default delay after this command
        bool ignore_delay;

        while (n) {
            ignore_delay = false;

            word_list* wl  = n->words;
            word_node* cmd = wl->first;

            const char* line_str = cmd->str + cmd->len + 1;
            size_t line_str_len  = n->len - cmd->len - 1;

            char last_char = n->str[n->len];
            bool line_end  = last_char == '\r' || last_char == '\n';


            int custom_cmd = -1;

            for( int i=0;i<commands_count;i++ ) {
              if(compare(cmd->str, cmd->len, commands[i].name, CASE_SENSITIVE) ) {
                custom_cmd = i;
              }
            }

            if( !inComment && !inString && custom_cmd>-1 ) {
              commands[custom_cmd].cb();
            }

            // REM (= Comment -> do nothing)
            else if (inComment || compare(cmd->str, cmd->len, "REM", CASE_SENSITIVE)) {
                inComment    = !line_end;
                ignore_delay = true;
            }


            else if (compare(cmd->str, cmd->len, "MouseMoveUp", CASE_SENSITIVE)) {
              int mousemoveamt = toInt(line_str, line_str_len);
              MouseGFX->moveYrel( -mousemoveamt );
            }
            else if (compare(cmd->str, cmd->len, "MouseMoveDown", CASE_SENSITIVE)) {
              int mousemoveamt = toInt(line_str, line_str_len);
              MouseGFX->moveYrel( mousemoveamt  );
            }
            else if (compare(cmd->str, cmd->len, "MouseMoveLeft", CASE_SENSITIVE)) {
              int mousemoveamt = toInt(line_str, line_str_len);
              MouseGFX->moveXrel( -mousemoveamt );
            }
            else if (compare(cmd->str, cmd->len, "MouseMoveRight", CASE_SENSITIVE)) {
              int mousemoveamt = toInt(line_str, line_str_len);
              MouseGFX->moveXrel( mousemoveamt );
            }
            else if (compare(cmd->str, cmd->len, "MouseClickRelease", CASE_SENSITIVE)) {
              MouseGFX->moveXYrel( 0, 0, 0 ); //vTaskDelay(25);}
            }
            else if (compare(cmd->str, cmd->len, "MouseClickRIGHT", CASE_SENSITIVE)) {
              MouseGFX->moveXYrel( 0, 0, MOUSE_RIGHT_BTN );//vTaskDelay(25);
            }
            else if (compare(cmd->str, cmd->len, "MouseClickLEFT", CASE_SENSITIVE)) {
              MouseGFX->moveXYrel( 0, 0, MOUSE_LEFT_BTN );//vTaskDelay(25);
            }
            else if (compare(cmd->str, cmd->len, "MouseClickMIDDLE", CASE_SENSITIVE)) {
              MouseGFX->moveXYrel( 0, 0, MOUSE_MIDDLE_BTN );//vTaskDelay(25);
            }
            else if (compare(cmd->str, cmd->len, "MouseDoubleClickLEFT", CASE_SENSITIVE)) {
              MouseGFX->sendDoubleClick( MOUSE_LEFT_BTN, 100, 100 );
            }
            else if (compare(cmd->str, cmd->len, "MouseMoveToCoords", CASE_SENSITIVE)) {
              word_node* w = cmd->next;
              int coords[2];
              for (uint8_t i = 0; i<2; ++i) {
                  if (w) {
                      coords[i] = toInt(w->str, w->len);
                      w    = w->next;
                  } else {
                      coords[i] = 0;
                  }
              }
              MouseGFX->moveXYabs( coords[0], coords[1], MouseGFX->getButtons() );
            }
            else if (compare(cmd->str, cmd->len, "SetDisplay", CASE_SENSITIVE)) {
              word_node* w = cmd->next;
              int coords[2];
              for (uint8_t i = 0; i<2; ++i) {
                  if (w) {
                      coords[i] = toInt(w->str, w->len);
                      w    = w->next;
                  } else {
                      coords[i] = 0;
                  }
              }
              MouseGFX->setDisplay( coords[0], coords[1] );

            }


            // LOCALE (-> change keyboard layout)
            else if (compare(cmd->str, cmd->len, "LOCALE", CASE_SENSITIVE)) {
                word_node* w = cmd->next;

                if (compare(w->str, w->len, "US", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_us);
                } else if (compare(w->str, w->len, "DE", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_de);
                } else if (compare(w->str, w->len, "RU", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_ru);
                } else if (compare(w->str, w->len, "GB", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_gb);
                } else if (compare(w->str, w->len, "ES", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_es);
                } else if (compare(w->str, w->len, "FR", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_fr);
                } else if (compare(w->str, w->len, "DK", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_dk);
                } else if (compare(w->str, w->len, "BE", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_be);
                } else if (compare(w->str, w->len, "PT", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_pt);
                } else if (compare(w->str, w->len, "IT", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_it);
                } else if (compare(w->str, w->len, "SK", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_sk);
                } else if (compare(w->str, w->len, "CZ", CASE_SENSITIVE)) {
                    keyboard::setLocale(&locale_cz);
                }

                ignore_delay = true;
            }

            // DELAY (-> sleep for x ms)
            else if (compare(cmd->str, cmd->len, "DELAY", CASE_SENSITIVE)) {
                sleep(toInt(line_str, line_str_len));
                ignore_delay = true;
            }

            // DEFAULTDELAY/DEFAULT_DELAY (set default delay per command)
            else if (compare(cmd->str, cmd->len, "DEFAULTDELAY", CASE_SENSITIVE) || compare(cmd->str, cmd->len, "DEFAULT_DELAY", CASE_SENSITIVE)) {
                defaultDelay = toInt(line_str, line_str_len);
                ignore_delay = true;
            }

            // REPEAT (-> repeat last command n times)
            else if (compare(cmd->str, cmd->len, "REPEAT", CASE_SENSITIVE) || compare(cmd->str, cmd->len, "REPLAY", CASE_SENSITIVE)) {
                repeatNum    = toInt(line_str, line_str_len) + 1;
                ignore_delay = true;
            }

            // STRING (-> type each character)
            else if (inString || compare(cmd->str, cmd->len, "STRING", CASE_SENSITIVE)) {
                if (inString) {
                    type(n->str, n->len);
                } else {
                    type(line_str, line_str_len);
                }

                inString = !line_end;
            }
/*
            // LED
            else if (compare(cmd->str, cmd->len, "LED", CASE_SENSITIVE)) {
                word_node* w = cmd->next;

                int c[3];

                for (uint8_t i = 0; i<3; ++i) {
                    if (w) {
                        c[i] = toInt(w->str, w->len);
                        w    = w->next;
                    } else {
                        c[i] = 0;
                    }
                }

                led::setColor(c[0], c[1], c[2]);
            }
*/
            // KEYCODE
            else if (compare(cmd->str, cmd->len, "KEYCODE", CASE_SENSITIVE)) {
                word_node* w = cmd->next;
                if (w) {
                    KeyReport k;

                    k.modifiers = (uint8_t)toInt(w->str, w->len);
                    k.reserved  = 0;
                    w           = w->next;

                    for (uint8_t i = 0; i<6; ++i) {
                        if (w) {
                            k.keys[i] = (uint8_t)toInt(w->str, w->len);
                            w         = w->next;
                        } else {
                            k.keys[i] = 0;
                        }
                    }

                    keyboard::send(&k);
                    keyboard::release();
                }
            }

            // Otherwise go through words and look for keys to press
            else {
                word_node* w = wl->first;

                while (w) {
                    press(w->str, w->len);
                    w = w->next;
                }

                if (line_end) release();
            }

            n = n->next;

            if (!inString && !inComment && !ignore_delay) sleep(defaultDelay);

            if (line_end && (repeatNum > 0)) --repeatNum;

            interpretTime = millis();
        }

        line_list_destroy(l);
    }

    int getRepeats()
    {
        return repeatNum;
    }

    unsigned int getDelayTime()
    {
        unsigned long finishTime  = sleepStartTime + sleepTime;
        unsigned long currentTime = millis();

        if (currentTime > finishTime) {
            return 0;
        } else {
            unsigned long remainingTime = finishTime - currentTime;
            return (unsigned int)remainingTime;
        }
    }
}
