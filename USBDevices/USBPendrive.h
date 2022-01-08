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

#include "USB.h"
#include "USBMSC.h"
#include "SD.h"


void (*USBPendriveLogger)( String err );
bool sd_begun = false;
bool pendrive_begun = false;
USBMSC *_MSC;

bool initPenDrive( USBMSC *MSC );
void deinitPenDrive( USBMSC *MSC );
bool initSD(int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
void deinitSD();

void USBPendriveLog( String msg )
{
  if( USBPendriveLogger ) USBPendriveLogger(msg);
  else Serial.println( msg.c_str() );
}


static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
  USBPendriveLog(String("MSC START/STOP: power: "+String(power_condition)+", start: "+String(start)+", eject: "+String(load_eject)));
  return true;
}

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{

  if( SD.writeRAW((uint8_t*)buffer, lba) ) log_v("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  else log_e("MSC WRITE FAIL: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  log_v("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  SD.readRAW((uint8_t*)buffer, lba);
  return bufsize;
}




bool initPenDrive( USBMSC *MSC )
{
  if( !sd_begun ) {
    USBPendriveLog("SD Card not started");
    return false;
  }
  MSC->vendorID("ESP32-S2");//max 8 chars
  MSC->productID("SD card");//max 16 chars
  MSC->productRevision("1.0");//max 4 chars
  MSC->onStartStop(onStartStop);
  MSC->onRead(onRead);
  MSC->onWrite(onWrite);
  MSC->mediaPresent(true);
  pendrive_begun = MSC->begin( SD.cardSize()/512, 512 );
  _MSC = MSC;
  return pendrive_begun;
}

void deinitPenDrive( USBMSC *MSC )
{
  MSC->end();
  pendrive_begun = false;
  USBPendriveLog("PenDrive stopped");
}


bool initSD(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
  static SPIClass* spi = NULL;
  spi = new SPIClass(FSPI);
  spi->begin(sck, miso, mosi, ss);
  if(!SD.begin(ss, *spi, 40000000)){
    USBPendriveLog("Card Mount Failed");
    Serial.println("Card Mount Failed");
    return false;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    USBPendriveLog("No SD card attached");
    Serial.println("No SD card attached");
    return false;
  }
  sd_begun = true;
  return true;
}

void deinitSD()
{
  if( pendrive_begun ) {
    deinitPenDrive(_MSC);
  }
  SD.end();
  sd_begun = false;
}
