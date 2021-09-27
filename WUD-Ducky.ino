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
 * Inspired by:
 *   - https://github.com/whid-injector/WHID
 *   - https://github.com/spacehuhn/wifi_ducky
 *
 * Could not exist without:
 *   - https://github.com/chegewara/EspTinyUSB
 *
\*/

#include "config_check.h"

#include <sdusb.h>        // https://github.com/chegewara/EspTinyUSB 1.3.4
#include <hidcomposite.h> // https://github.com/chegewara/EspTinyUSB 1.3.4

#include "webserver.h"

// Config SD Card
#define SD_MISO  37
#define SD_MOSI  35
#define SD_SCK   36
#define SD_CS    34

// Config SSID and password
const char* SSID        = "WUD-01";
const char* PASSWORD    = "12345678";

HIDcomposite KeyboardMouse;
SDCard2USB PenDrive;


void HIDKeySend( String str )
{
  KeyboardMouse.sendString(str+"\n");
}


int keyDelay = 10; // delay (ms) between keys when sending a string
int DelayLength = 500; // delay between commands
int keypressdone=0;

void runpayload( fs::FS &fs, const char* payload)
{
  if( !fs.exists( payload ) ) return;
  fs::File f = fs.open(payload, "r");
  if( !f ) return;
  // memoize settings
  int defaultdelay = DelayLength;
  int settingsdefaultdelay = DelayLength;
  int custom_delay;
  while(f.available()) {

    String line = f.readStringUntil('\n');
    line.replace("&lt;", "<");

    String fullkeys = line;
    int str_len = fullkeys.length()+1;
    char keyarray[str_len];
    fullkeys.toCharArray(keyarray, str_len);

    char *i;
    String cmd;
    String cmdinput;
    cmd = String(strtok_r(keyarray,":",&i));
    cmdinput = String(strtok_r(NULL,":",&i));

    if(cmd == "Rem") {
      DelayLength = 0;
    } else if(cmd == "DefaultDelay") {
      DelayLength = 1500;
      defaultdelay = cmdinput.toInt();
    } else if(cmd == "CustomDelay") {
      custom_delay = cmdinput.toInt();
      DelayLength = custom_delay;
    } else if(cmd == "KeyDelay") {
      DelayLength = 0;
      keyDelay = atoi(cmdinput.c_str());
    } else if(cmd == "Key"){ //If command equals "Key:X"
      int key = atoi(cmdinput.c_str());
      vTaskDelay(25);
      KeyboardMouse.sendPress(key);
      vTaskDelay(25);
      KeyboardMouse.sendRelease();
    } else if(cmd == "Press"){ //If command equals "Press:X" or "Press:X+Y+ETC"
      keypressdone=1;
      String fullkeys = cmdinput;
      int str_len = fullkeys.length()+1;
      char keyarray[str_len];
      fullkeys.toCharArray(keyarray, str_len);
      char delimiter[] = "+";
      char *keypart;
      keypart = strtok(keyarray, delimiter);
      while(keypart != NULL) {
        int key = atoi(keypart);
        vTaskDelay(25);
        KeyboardMouse.sendPress(key);
        keypart = strtok(NULL, delimiter);
      }
      if(keypressdone==1) {
        vTaskDelay(25);
        KeyboardMouse.sendRelease();
        keypressdone=0;
      }
    } else if(cmd == "Print") { //If command equals "Print:X"
      vTaskDelay(25);
      size_t len = cmdinput.length();
      uint8_t keycode;
      for(size_t i = 0; i < len; i++) {
        KeyboardMouse.sendChar(cmdinput[i]);
        vTaskDelay(keyDelay);
      }
    } else if(cmd == "PrintLine") { //If command equals "PrintLine:X"
      vTaskDelay(25);
      size_t len = cmdinput.length();
      uint8_t keycode;
      for(size_t i = 0; i < len; i++) {
        KeyboardMouse.sendChar(cmdinput[i]);
        vTaskDelay(keyDelay);
      }
      vTaskDelay(250);
      KeyboardMouse.sendPress(HID_KEY_ENTER);
      vTaskDelay(25);
      KeyboardMouse.sendRelease();
    } else if(cmd == "MouseMoveUp") { //If command equals "MouseMoveUp:X"
      int mousemoveamt = cmdinput.toInt();
      vTaskDelay(25);
      KeyboardMouse.move(0, 0);
      KeyboardMouse.move(0, mousemoveamt*-1);
    } else if(cmd == "MouseMoveDown") { //If command equals "MouseMoveDown:X"
      int mousemoveamt = cmdinput.toInt();
      vTaskDelay(25);
      KeyboardMouse.move(0, 0);
      KeyboardMouse.move(0, mousemoveamt);
    } else if(cmd == "MouseMoveLeft") { //If command equals "MouseMoveLeft:X"
      int mousemoveamt = cmdinput.toInt();
      vTaskDelay(25);
      KeyboardMouse.move(0, 0);
      KeyboardMouse.move(mousemoveamt*-1, 0);
    } else if(cmd == "MouseMoveRight") { //If command equals "MouseMoveRight:X"
      int mousemoveamt = cmdinput.toInt();
      vTaskDelay(25);
      KeyboardMouse.move(0, 0);
      KeyboardMouse.move(mousemoveamt, 0);
    } else if(cmd == "MouseClickRIGHT") { //mouse click command EX: MouseClickLEFT: MouseClickRIGHT: MouseClickMIDDLE:
      vTaskDelay(25);
      KeyboardMouse.pressRight();
    } else if(cmd == "MouseClickLEFT") {
      vTaskDelay(25);
      KeyboardMouse.pressLeft();
    } else if(cmd == "MouseClickMIDDLE") {
      vTaskDelay(25);
      KeyboardMouse.pressMiddle();
    } else if(cmd == "MouseDoubleClickLEFT") {
      vTaskDelay(25);
      KeyboardMouse.doublePressLeft();
    } else {
      // wut ?
    }
    vTaskDelay(DelayLength); //delay between lines in payload, I found running it slower works best
    DelayLength = defaultdelay;
  }
  f.close();
  // restore settings
  DelayLength = settingsdefaultdelay;
}





void setup()
{
  Serial.begin(115200);

  KeyboardMouse.begin();

  if(PenDrive.initSD(SD_SCK, SD_MISO, SD_MOSI, SD_CS))
  {
    if(PenDrive.begin()) {
      Serial.println("MSC lun 1 begin");
    } else {
      log_e("LUN 1 failed");
      while(1) vTaskDelay(1);
    }
    vTaskDelay(10000);
  } else {
    Serial.println("Failed to init SD");
    while(1) vTaskDelay(1);
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);

  const bool formatOnFail = true;
  SPIFFS.begin(formatOnFail);

  vTaskDelay(1000);
  startFileServer();
}


void loop() {
 server.handleClient();
 vTaskDelay(2);
}

