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




typedef void (*PayloadCharCb)(char c, uint32_t wait);
typedef void (*PayloadChunkCb)(char *c);

typedef void (*DuckyCommandCb)();
void (*PayloadLogger)( String err );


struct DuckyCommand
{
  const char* name;
  DuckyCommandCb cb;
};


struct payloadRunner
{
  String fullkeys;
  int str_len;
  char *keyarray;
  char plusdelimiter[2] = {'+','\0'};
  char *keypart;
  char *c;
  String cmd;
  String cmdinput;

  int keyDelay = 10; // delay (ms) between keys when sending a string
  int DelayLength = 500; // delay between commands
  int keypressdone=0;
  int defaultdelay;
  int settingsdefaultdelay;
  int custom_delay;

  DuckyCommand* commands;
  size_t commands_count;

  void setDuckyCommands(DuckyCommand* _commands, size_t count)
  {
    commands = _commands;
    commands_count = count;
  }

  void runPayload( String line )
  {
    line.replace("&lt;", "<");
    fullkeys = line;
    str_len = fullkeys.length()+1;
    keyarray = new char[str_len];
    fullkeys.toCharArray(keyarray, str_len);

    String output;
    if( line.indexOf(":") > 0 ) { // command has arg separator
      cmd = String(strtok_r(keyarray,":",&c));
      if( cmd.length()+1 < line.length() ) { // command has args
        cmdinput = String(strtok_r(NULL,":",&c));
        output = "Received command with args: " + cmd + " => " + cmdinput;
      } else {
        output = "Received command with empty args: " + cmd;
      }
    } else {
      output = "Received raw command: " + line;
      cmd = line; // command with no args
    }

    bool found = false;

    for( int i=0;i<commands_count;i++ ) {
      if( String(commands[i].name).equals( cmd ) ) {
        //Serial.println("Running command");
        commands[i].cb();
        found = true;
      }
    }
    delete keyarray;
    vTaskDelay(DelayLength); //delay between repeated commands in payload, sometimes running it slower works best
    DelayLength = defaultdelay;
    if(!found) {
      output = "Unknown payload: " + line;
    }
    if( PayloadLogger ) PayloadLogger( output );
    log_d( "%s", output.c_str() );
  }


  void readChars( PayloadCharCb cb = nullptr, uint32_t wait=0 )
  {
    size_t len = cmdinput.length();
    for(size_t i = 0; i < len; i++) {
      if( cb ) cb(cmdinput[i], wait);
    }
  }





  bool getNumericArgs( int**argv, size_t argc, const char* delimiter = "+" )
  {
    fullkeys = cmdinput;
    int str_len = fullkeys.length()+1;
    keyarray = new char[str_len];
    fullkeys.toCharArray(keyarray, str_len);
    keypart = strtok(keyarray, delimiter);
    size_t argi = 0;
    while( keypart != NULL && argi < argc ) {
      *argv[argi] = atoi(keypart);
      argi++;
    }
    delete( keyarray );
    return argi==argc;
  }



  void readDelimited( PayloadChunkCb cb = nullptr, const char* delimiter = "+" )
  {
    fullkeys = cmdinput;
    int str_len = fullkeys.length()+1;
    keyarray = new char[str_len];
    fullkeys.toCharArray(keyarray, str_len);
    keypart = strtok(keyarray, plusdelimiter);
    while(keypart != NULL) {
      if( cb ) cb(keypart);
      keypart = strtok(NULL, plusdelimiter);
    }
    delete( keyarray );
  }


  void MouseDrawStr()
  {
    fullkeys = cmdinput;
    int str_len = fullkeys.length()+1;
    keyarray = new char[str_len];
    fullkeys.toCharArray(keyarray, str_len);
    keypart = strtok(keyarray, plusdelimiter);
    int8_t move;

    //uint8_t mousestate = MouseGFX->getMouseReport()->buttons; // memoize mouse buttons state
    //MouseGFX->moveRelative( 0, 0, 0 ); // reset mouse buttons state

    while(keypart != NULL) {

      switch( keypart[0] ) {
        case 'X': // move X axis
          move = atoi( &keypart[1] );
          //MouseGFX->moveX( move );
        break;
        case 'Y': // move Y axis
          move = atoi( &keypart[1] );
          //MouseGFX->moveY( move );
        break;
        case 'C': // click/unclick
        {
          //vTaskDelay(15);
          uint8_t mouse_btn_mask = (uint8_t)atoi(&keypart[1]);
          //MouseGFX->moveRelative( 0, 0, mouse_btn_mask );
        }
        break;
        default: // ignore
        break;
      }

      keypart = strtok(NULL, plusdelimiter);
    }
    delete( keyarray );
    //MouseGFX->moveRelative( 0, 0, mousestate ); // restore mouse buttons state
  }



};
