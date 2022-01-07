#pragma once

#include "USB.h"
#include "USBMSC.h"
#include "SD.h"


void (*USBPendriveErrorLogger)( String err );



static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
  //HWSerial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
  return true;
}


static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
  //HWSerial.printf("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  SD.writeRAW((uint8_t*)buffer, lba);
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  //HWSerial.printf("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  SD.readRAW((uint8_t*)buffer, lba);
  return bufsize;
}


bool initSD(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
  static SPIClass* spi = NULL;
  spi = new SPIClass(FSPI);
  spi->begin(sck, miso, mosi, ss);
  if(!SD.begin(ss, *spi, 40000000)){
    if( USBPendriveErrorLogger ) {
      USBPendriveErrorLogger("Card Mount Failed");
    }
    //Serial.println("Card Mount Failed");
    return false;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    if( USBPendriveErrorLogger ) {
      USBPendriveErrorLogger("No SD card attached");
    }
    Serial.println("No SD card attached");
    return false;
  }

  return true;
}

