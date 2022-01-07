#pragma once

typedef void (*DuckyCommandCb)();
void (*PayloadLogger)( String err );

struct DuckyCommand
{
  const char* name;
  DuckyCommandCb cb;
};


struct _PayloadRunner
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

  void parseLine( String line )
  {
    line.replace("&lt;", "<");
    fullkeys = line;
    str_len = fullkeys.length()+1;
    keyarray = new char[str_len];
    fullkeys.toCharArray(keyarray, str_len);

    String cmd;
    String cmdinput;
    String output;
    if( line.indexOf(":") > 0 ) { // command has arg separator
      cmd = String(strtok_r(keyarray,":",&c));
      if( cmd.length()+1 < line.length() ) { // command has args
        cmdinput = String(strtok_r(NULL,":",&c));
        output = "Running command with args: " + cmd + " => " + cmdinput;
        if( PayloadLogger ) PayloadLogger( output );
        Serial.println(output);
      } else {
        output = "Running command with empty args: " + cmd;
        if( PayloadLogger ) PayloadLogger( output );
        Serial.println(output);
      }
    } else {
      output = "Running raw command without args: " + line;
      if( PayloadLogger ) PayloadLogger( output );
      Serial.println(output);
      cmd = line; // command with no args
    }

    bool found = false;

    for( int i=0;i<commands_count;i++ ) {
      if( String(commands[i].name).equals( cmd ) ) {
        commands[i].cb();
        found = true;
      }
    }
    delete keyarray;
    vTaskDelay(DelayLength); //delay between repeated commands in payload, sometimes running it slower works best
    DelayLength = defaultdelay;
    if(!found) {
      output = "Unknown command: " + cmd;
      if( PayloadLogger ) PayloadLogger( output );
      Serial.println(output);
    }
  }

} PayloadRunner;
