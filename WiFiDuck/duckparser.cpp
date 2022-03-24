/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#pragma once

#include "duckparser.hpp"

#include "keyboard.cpp"
#include "mouse.cpp"
#include "led_controls.cpp"
#include "prefs.cpp"

#include "../xbm/alien.xbm.h"

// some easter egg to test the mouse in absolute positioning mode
xbmImage_t Alien_128x64 =
{
  alien_width,
  alien_height,
  alien_bytes,
  alien_bytes_per_row,
  Alien_128x64_bits
};




namespace duckparser
{
  // PRIVATE (not really since 'using namespace duckparser;' will expose those)

  bool inited = false;

  // some local values to walk through lines/words nodes
  bool ignore_delay;
  word_list* wordlist;
  word_node* cmd;
  word_node* wordnode;
  line_list* linelist;
  line_node* linenode;

  const char* line_str;
  size_t line_str_len;

  char last_char;
  bool line_end;

  // Keys and Command sets
  duckCommandSet *keys = nullptr;            // Legacy WiFiDuck named keys
  duckCommandSet *legacy_commands = nullptr; // Legacy WiFiDuck commands
  duckCommandSet *custom_commands = nullptr; // WUD commands

  // state flags
  bool inString  = false;
  bool inComment = false;

  int defaultDelay = 5;
  int repeatNum    = 0;

  unsigned long interpretTime  = 0;
  unsigned long sleepStartTime = 0;
  unsigned long sleepTime      = 0;


  // shortcut / key presser
  void press(const char* str, size_t len)
  {
    if (len == 1) { // single character
      keyboard::press(str);
      return;
    }
    duckCommand* match_key = nullptr;
    for( int i=0;i<keys->count;i++) {
      if (compare(str, len, keys->commands[i].name, CASE_SENSITIVE)) {
        match_key = &keys->commands[i];
        break;
      }
    }
    if( match_key ) match_key->cb();
    else keyboard::press(str); // Utf8 character
  }


  // input parser
  unsigned int toInt(const char* str, size_t len)
  {
    if (!str || (len == 0)) return 0;
    unsigned int val = 0;
    if ((len > 2) && (str[0] == '0') && (str[1] == 'x')) { // HEX
      for (size_t i = 2; i < len; ++i) {
        uint8_t b = tolower(str[i]);
        if ((b >= '0') && (b <= '9')) b = b - '0';
        else if ((b >= 'a') && (b <= 'f')) b = b - 'a' + 10;
        //else if ((b >= 'A') && (b <= 'F')) b = b - 'A' + 10;
        val = (val << 4) | (b & 0xF);
      }
    } else { // DECIMAL
      for (size_t i = 0; i < len; ++i) {
        if ((str[i] >= '0') && (str[i] <= '9')) {
          val = val * 10 + (str[i] - '0');
        }
      }
    }
    return val;
  }

  // sandman
  void sleep(unsigned long time)
  {
    unsigned long offset = millis() - interpretTime;
    if (time > offset) {
      sleepStartTime = millis();
      sleepTime      = time - offset;
      delay(sleepTime);
    }
  }



  // Legacy Commands (internals)

  void setDelay()
  {
    sleep(toInt(line_str, line_str_len));
    ignore_delay = true;
  }

  void setDefaultDelay()
  {
    defaultDelay = toInt(line_str, line_str_len);
    ignore_delay = true;
  }

  void setRepeat()
  {
    repeatNum    = toInt(line_str, line_str_len) + 1;
    ignore_delay = true;
  }

  void setLed()
  {
    int c[3];
    for (uint8_t i = 0; i<3; ++i) {
      if (wordnode) {
        c[i] = toInt(wordnode->str, wordnode->len);
        wordnode    = wordnode->next;
      } else c[i] = 0;
    }
    if( c[1] + c[2] == 0 ) ledControl::set( c[0]==0 ? false:true ); // no delays provided, just a state change
    else                   ledControl::blink( c[1], c[2] );
  }

  void setKeycode()
  {
    KeyReport k;
    k.modifiers = (uint8_t)toInt(wordnode->str, wordnode->len);
    k.reserved  = 0;
    wordnode    = wordnode->next;
    for (uint8_t i = 0; i<6; ++i) {
      if (wordnode) {
        k.keys[i] = (uint8_t)toInt(wordnode->str, wordnode->len);
        wordnode  = wordnode->next;
      } else k.keys[i] = 0;
    }
    keyboard::send(&k);
    keyboard::release();
  }

  // set keyboard layout according to the ducky input
  void setLocale()
  {
    size_t layouts_count = sizeof( keyboard::layouts ) / sizeof(hid_layout_t);
    for( int i=0; i<layouts_count; i++ ) {
      if (compare(wordnode->str, wordnode->len, keyboard::layouts[i].code, CASE_SENSITIVE)) {
        keyboard::setLocale( &keyboard::layouts[i] );
      }
    }
    ignore_delay = true;
  }

  // Mouse commands (internals)

  void MouseMoveUp() // relative
  {
    int mousemoveamt = toInt(line_str, line_str_len);
    MouseGFX->moveYrel( -mousemoveamt );
  }

  void MouseMoveDown() // relative
  {
    int mousemoveamt = toInt(line_str, line_str_len);
    MouseGFX->moveYrel( mousemoveamt  );
  }

  void MouseMoveLeft() // relative
  {
    int mousemoveamt = toInt(line_str, line_str_len);
    MouseGFX->moveXrel( -mousemoveamt );
  }

  void MouseMoveRight() // relative
  {
    int mousemoveamt = toInt(line_str, line_str_len);
    MouseGFX->moveXrel( mousemoveamt );
  }

  void MouseClickRelease()
  {
    MouseGFX->moveXYrel( 0, 0, 0 );
  }

  void MouseClickRIGHT()
  {
    MouseGFX->moveXYrel( 0, 0, MOUSE_RIGHT );
  }

  void MouseClickLEFT()
  {
    MouseGFX->moveXYrel( 0, 0, MOUSE_LEFT );
  }

  void MouseClickMIDDLE()
  {
    MouseGFX->moveXYrel( 0, 0, MOUSE_MIDDLE );
  }

  void MouseDoubleClickLEFT()
  {
    MouseGFX->sendDoubleClick( MOUSE_LEFT, 100, 100 );
  }

  void MouseMoveToCoords() // absolute
  {
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

  void SetDisplay()
  {
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


  void DrawSpaceInvaders() // easter egg omg!
  {
    xTaskCreate( [](void*param) {  MouseGFX->drawXbm( &Alien_128x64, 900, 500 ); vTaskDelete(NULL); }, "blah", 2048, NULL, 16, NULL );
  }


  void setRem()
  {
    inComment    = !line_end;
    ignore_delay = true;
  }

  void setString()
  {
    if (inString) {
      keyboard::write(linenode->str, linenode->len);
    } else {
      keyboard::write(line_str, line_str_len);
    }
    inString = !line_end;
  }

};




namespace duckcommands
{
  // import duckparser exposing the internal commands
  using namespace duckparser;

  // attach SpaceHuhn's Legacy WiFiDuck Commands + Mouse implementations
  duckCommand DuckyCommands[] =
  {
  //{ "Command",              callbackFunction,     needs_args },
    { "REM",                  setRem,               false },
    { "STRING",               setString,            false },
    { "LOCALE",               setLocale,            true  },
    { "DELAY",                setDelay,             true  },
    { "DEFAULTDELAY",         setDefaultDelay,      true  },
    { "DEFAULT_DELAY",        setDefaultDelay,      true  },
    { "REPEAT",               setRepeat,            true  },
    { "REPLAY",               setRepeat,            true  },
    { "LED",                  setLed,               true  },
    { "KEYCODE",              setKeycode,           true  },
    { "MouseMoveUp",          MouseMoveUp,          true  },
    { "MouseMoveDown",        MouseMoveDown,        true  },
    { "MouseMoveLeft",        MouseMoveLeft,        true  },
    { "MouseMoveRight",       MouseMoveRight,       true  },
    { "MouseClickRelease",    MouseClickRelease,    false },
    { "MouseClickRIGHT",      MouseClickRIGHT,      false },
    { "MouseClickLEFT",       MouseClickLEFT,       false },
    { "MouseClickMIDDLE",     MouseClickMIDDLE,     false },
    { "MouseDoubleClickLEFT", MouseDoubleClickLEFT, false },
    { "MouseMoveToCoords",    MouseMoveToCoords,    true  },
    { "SetDisplay",           SetDisplay,           true  },
    { "DrawSpaceInvaders",    DrawSpaceInvaders,    false },
  };

  // SpaceHuhn's legacy WiFiDuck named keys
  duckCommand KeyCommands[] =
  {
    // named keys
    {"ENTER",       [](){ keyboard::pressKey(KEY_ENTER); } },
    {"MENU",        [](){ keyboard::pressKey(KEY_PROPS); } },
    {"APP",         [](){ keyboard::pressKey(KEY_PROPS); } },
    {"DELETE",      [](){ keyboard::pressKey(KEY_Delete); } },
    {"BACKSPACE",   [](){ keyboard::pressKey(KEY_BCKSPACE); } },
    {"HOME",        [](){ keyboard::pressKey(KEY_Home); } },
    {"INSERT",      [](){ keyboard::pressKey(KEY_INSRT); } },
    {"PAGEUP",      [](){ keyboard::pressKey(KEY_PAGEUP); } },
    {"PAGEDOWN",    [](){ keyboard::pressKey(KEY_PAGEDOWN); } },
    {"UPARROW",     [](){ keyboard::pressKey(KEY_UP); } },
    {"UP",          [](){ keyboard::pressKey(KEY_UP); } },
    {"DOWNARROW",   [](){ keyboard::pressKey(KEY_DOWN); } },
    {"DOWN",        [](){ keyboard::pressKey(KEY_DOWN); } },
    {"LEFTARROW",   [](){ keyboard::pressKey(KEY_LEFT); } },
    {"LEFT",        [](){ keyboard::pressKey(KEY_LEFT); } },
    {"RIGHTARROW",  [](){ keyboard::pressKey(KEY_RIGHT); } },
    {"RIGHT",       [](){ keyboard::pressKey(KEY_RIGHT); } },
    {"TAB",         [](){ keyboard::pressKey(KEY_TABULATION); } },
    {"END",         [](){ keyboard::pressKey(KEY_End); } },
    {"ESC",         [](){ keyboard::pressKey(KEY_ESCAPE); } },
    {"ESCAPE",      [](){ keyboard::pressKey(KEY_ESCAPE); } },
    {"F1",          [](){ keyboard::pressKey(KEY_F_1); } },
    {"F2",          [](){ keyboard::pressKey(KEY_F_2); } },
    {"F3",          [](){ keyboard::pressKey(KEY_F_3); } },
    {"F4",          [](){ keyboard::pressKey(KEY_F_4); } },
    {"F5",          [](){ keyboard::pressKey(KEY_F_5); } },
    {"F6",          [](){ keyboard::pressKey(KEY_F_6); } },
    {"F7",          [](){ keyboard::pressKey(KEY_F_7); } },
    {"F8",          [](){ keyboard::pressKey(KEY_F_8); } },
    {"F9",          [](){ keyboard::pressKey(KEY_F_9); } },
    {"F10",         [](){ keyboard::pressKey(KEY_F_10); } },
    {"F11",         [](){ keyboard::pressKey(KEY_F_11); } },
    {"F12",         [](){ keyboard::pressKey(KEY_F_12); } },
    {"SPACE",       [](){ keyboard::pressKey(KEY_SPACE); } },
    {"PAUSE",       [](){ keyboard::pressKey(KEY_PAUSE); } },
    {"BREAK",       [](){ keyboard::pressKey(KEY_PAUSE); } },
    {"CAPSLOCK",    [](){ keyboard::pressKey(KEY_CAPSLOCK); } },
    {"NUMLOCK",     [](){ keyboard::pressKey(KEY_NUMLOCK); } },
    {"PRINTSCREEN", [](){ keyboard::pressKey(KEY_SYSRQ); } },
    {"SCROLLLOCK",  [](){ keyboard::pressKey(KEY_SCROLLLOCK); } },

    // numpad keys
    {"NUM_0",       [](){ keyboard::pressKey(KEY_KP0);        } },
    {"NUM_1",       [](){ keyboard::pressKey(KEY_KP1);        } },
    {"NUM_2",       [](){ keyboard::pressKey(KEY_KP2);        } },
    {"NUM_3",       [](){ keyboard::pressKey(KEY_KP3);        } },
    {"NUM_4",       [](){ keyboard::pressKey(KEY_KP4);        } },
    {"NUM_5",       [](){ keyboard::pressKey(KEY_KP5);        } },
    {"NUM_6",       [](){ keyboard::pressKey(KEY_KP6);        } },
    {"NUM_7",       [](){ keyboard::pressKey(KEY_KP7);        } },
    {"NUM_8",       [](){ keyboard::pressKey(KEY_KP8);        } },
    {"NUM_9",       [](){ keyboard::pressKey(KEY_KP9);        } },
    {"NUM_ASTERIX", [](){ keyboard::pressKey(KEY_KPASTERISK); } },
    {"NUM_ENTER",   [](){ keyboard::pressKey(KEY_KPENTER);    } },
    {"NUM_MINUS",   [](){ keyboard::pressKey(KEY_KPMINUS);    } },
    {"NUM_DOT",     [](){ keyboard::pressKey(KEY_KPDOT);      } },
    {"NUM_PLUS",    [](){ keyboard::pressKey(KEY_KPPLUS);     } },

    // modifiers
    {"CTRL",        [](){ keyboard::pressModifier(KEY_MOD_LCTRL); } },
    {"CONTROL",     [](){ keyboard::pressModifier(KEY_MOD_LCTRL); } },
    {"SHIFT",       [](){ keyboard::pressModifier(KEY_MOD_LSHIFT); } },
    {"ALT",         [](){ keyboard::pressModifier(KEY_MOD_LALT); } },
    {"WINDOWS",     [](){ keyboard::pressModifier(KEY_MOD_LMETA); } },
    {"GUI",         [](){ keyboard::pressModifier(KEY_MOD_LMETA); } },
  };

};




namespace duckparser
{
  using namespace duckcommands;

  // PUBLIC

  void init( duckCommandSet* user_commands )
  {
    if( !inited ) {
      legacy_commands = new duckCommandSet({ DuckyCommands, sizeof( DuckyCommands ) / sizeof( duckCommand ) });
      keys            = new duckCommandSet({ KeyCommands,   sizeof( KeyCommands )   / sizeof( duckCommand ) });
      ledControl::init();
      inited = true;
    }
    if( user_commands != nullptr ) {
      custom_commands = user_commands;
    }
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
    linelist = parse_lines(str, len);

    // Go through all lines
    linenode = linelist->first;

    while (linenode) {
      ignore_delay = false; // Flag, no default delay after this command

      wordlist  = linenode->words;
      cmd = wordlist->first;
      wordnode = cmd->next;

      line_str = cmd->str + cmd->len + 1;
      line_str_len  = linenode->len - cmd->len - 1;

      last_char = linenode->str[linenode->len];
      line_end  = last_char == '\r' || last_char == '\n';

      duckCommand* match_cmd = nullptr;
      bool command_had_args = (wordnode && wordnode->len > 0 && wordnode->str[0] !='\0');

      // iterate in legacy commands
      for( int i=0;i<legacy_commands->count;i++ ) {
        if(compare(cmd->str, cmd->len, legacy_commands->commands[i].name, CASE_SENSITIVE) ) {
          if( legacy_commands->commands[i].needs_args == command_had_args )
            match_cmd = &legacy_commands->commands[i];
        }
      }
      // iterate in custom commands of no match found
      if( match_cmd == nullptr && custom_commands ) { // commands with or without args
        for( int i=0;i<custom_commands->count;i++ ) {
          if(compare(cmd->str, cmd->len, custom_commands->commands[i].name, CASE_SENSITIVE) ) {
            if( custom_commands->commands[i].needs_args == command_had_args )
              match_cmd = &custom_commands->commands[i];
          }
        }
      }
      if( !inComment && !inString && match_cmd ) {
        match_cmd->cb();
      } else if (inComment || compare(cmd->str, cmd->len, "REM", CASE_SENSITIVE)) { // REM (= Comment -> do nothing)
        setRem();
      } else if (inString || compare(cmd->str, cmd->len, "STRING", CASE_SENSITIVE)) { // STRING (-> type each character)
        setString();
      } else { // Otherwise go through words and look for keys to press
        wordnode = wordlist->first;
        while (wordnode) {
          //
          press(wordnode->str, wordnode->len);
          wordnode = wordnode->next;
        }
        if (line_end) keyboard::release();
      }
      linenode = linenode->next;
      if (!inString && !inComment && !ignore_delay) sleep(defaultDelay);
      if (line_end && (repeatNum > 0)) --repeatNum;
      interpretTime = millis();
    }

    line_list_destroy(linelist);
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


};
