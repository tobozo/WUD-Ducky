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

#define HAS_PENDRIVE

#include "USBConfig.h"
#include "USBMSC.h"
#include <SD.h>
#include <LittleFS.h>
#include "../WiFiDuck/led_controls.hpp"

// Config SD Card
#define SD_MISO  37
#define SD_MOSI  35
#define SD_SCK   36
#define SD_CS    34

//void (*USBPendriveLogger)( String err );
void (*USBPenDriveLogger)( const char *fmt, ... );
//bool sd_begun = false;
//bool pendrive_begun = false;

USBMSC MSC;
extern fs::FS* duckyFS;

bool initPenDrive();
void deinitPenDrive();
bool initSD();
bool initSD(int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
void deinitSD();

void USBPendriveLog( String msg )
{
  if( USBPenDriveLogger ) USBPenDriveLogger( "%s", msg.c_str() );
  // else {
  //   if( usbserial_begun ) USBSerial.println( msg.c_str() );
  // }
}



// default pendrive partition, if SD fails

#define FAT_U8(v) ((v) & 0xFF)
#define FAT_U16(v) FAT_U8(v), FAT_U8((v) >> 8)
#define FAT_U32(v) FAT_U8(v), FAT_U8((v) >> 8), FAT_U8((v) >> 16), FAT_U8((v) >> 24)
#define FAT_MS2B(s,ms)    FAT_U8(((((s) & 0x1) * 1000) + (ms)) / 10)
#define FAT_HMS2B(h,m,s)  FAT_U8(((s) >> 1)|(((m) & 0x7) << 5)),      FAT_U8((((m) >> 3) & 0x7)|((h) << 3))
#define FAT_YMD2B(y,m,d)  FAT_U8(((d) & 0x1F)|(((m) & 0x7) << 5)),    FAT_U8((((m) >> 3) & 0x1)|((((y) - 1980) & 0x7F) << 1))
#define FAT_TBL2B(l,h)    FAT_U8(l), FAT_U8(((l >> 8) & 0xF) | ((h << 4) & 0xF0)), FAT_U8(h >> 4)

#define README_CONTENTS "This is tinyusb's MassStorage Class demo.\r\n\r\nIf you find any bugs or get any questions, feel free to file an\r\nissue at github.com/hathach/tinyusb"

static const uint32_t DISK_SECTOR_COUNT = 2 * 8; // 8KB is the smallest size that windows allow to mount
static const uint16_t DISK_SECTOR_SIZE = 512;    // Should be 512
static const uint16_t DISC_SECTORS_PER_TABLE = 1; //each table sector can fit 170KB (340 sectors)

static uint8_t msc_disk[DISK_SECTOR_COUNT][DISK_SECTOR_SIZE] =
{
  //------------- Block0: Boot Sector -------------//
  {
    // Header (62 bytes)
    0xEB, 0x3C, 0x90, //jump_instruction
    'M' , 'S' , 'D' , 'O' , 'S' , '5' , '.' , '0' , //oem_name
    FAT_U16(DISK_SECTOR_SIZE), //bytes_per_sector
    FAT_U8(1),    //sectors_per_cluster
    FAT_U16(1),   //reserved_sectors_count
    FAT_U8(1),    //file_alloc_tables_num
    FAT_U16(16),  //max_root_dir_entries
    FAT_U16(DISK_SECTOR_COUNT), //fat12_sector_num
    0xF8,         //media_descriptor
    FAT_U16(DISC_SECTORS_PER_TABLE),   //sectors_per_alloc_table;//FAT12 and FAT16
    FAT_U16(1),   //sectors_per_track;//A value of 0 may indicate LBA-only access
    FAT_U16(1),   //num_heads
    FAT_U32(0),   //hidden_sectors_count
    FAT_U32(0),   //total_sectors_32
    0x00,         //physical_drive_number;0x00 for (first) removable media, 0x80 for (first) fixed disk
    0x00,         //reserved
    0x29,         //extended_boot_signature;//should be 0x29
    FAT_U32(0x1234), //serial_number: 0x1234 => 1234
    'T' , 'i' , 'n' , 'y' , 'U' , 'S' , 'B' , ' ' , 'M' , 'S' , 'C' , //volume_label padded with spaces (0x20)
    'F' , 'A' , 'T' , '1' , '2' , ' ' , ' ' , ' ' ,  //file_system_type padded with spaces (0x20)

    // Zero up to 2 last bytes of FAT magic code (448 bytes)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

     //boot signature (2 bytes)
    0x55, 0xAA
  },

  //------------- Block1: FAT12 Table -------------//
  {
    FAT_TBL2B(0xFF8, 0xFFF), FAT_TBL2B(0xFFF, 0x000) // first 2 entries must be 0xFF8 0xFFF, third entry is cluster end of readme file
  },

  //------------- Block2: Root Directory -------------//
  {
    // first entry is volume label
    'E' , 'S' , 'P' , '3' , '2' , 'S' , '2' , '-' ,
    'W' , 'U' , 'D' ,
    0x08, //FILE_ATTR_VOLUME_LABEL
    0x00,
    FAT_MS2B(0,0),
    FAT_HMS2B(0,0,0),
    FAT_YMD2B(0,0,0),
    FAT_YMD2B(0,0,0),
    FAT_U16(0),
    FAT_HMS2B(13,42,30),  //last_modified_hms
    FAT_YMD2B(2018,11,5), //last_modified_ymd
    FAT_U16(0),
    FAT_U32(0),

    // second entry is readme file
    'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' ,//file_name[8]; padded with spaces (0x20)
    'T' , 'X' , 'T' ,     //file_extension[3]; padded with spaces (0x20)
    0x20,                 //file attributes: FILE_ATTR_ARCHIVE
    0x00,                 //ignore
    FAT_MS2B(1,980),      //creation_time_10_ms (max 199x10 = 1s 990ms)
    FAT_HMS2B(13,42,36),  //create_time_hms [5:6:5] => h:m:(s/2)
    FAT_YMD2B(2018,11,5), //create_time_ymd [7:4:5] => (y+1980):m:d
    FAT_YMD2B(2020,11,5), //last_access_ymd
    FAT_U16(0),           //extended_attributes
    FAT_HMS2B(13,44,16),  //last_modified_hms
    FAT_YMD2B(2019,11,5), //last_modified_ymd
    FAT_U16(2),           //start of file in cluster
    FAT_U32(sizeof(README_CONTENTS) - 1) //file size
  },

  //------------- Block3: Readme Content -------------//
  README_CONTENTS
};

static int32_t onWriteDefault(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
  //if( USBPenDriveLogger ) USBPenDriveLogger("MSC WRITE: lba: %u, offset: %u, bufsize: %u", lba, offset, bufsize);
  memcpy(msc_disk[lba] + offset, buffer, bufsize);
  return bufsize;
}

static int32_t onReadDefault(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
  //if( USBPenDriveLogger) USBPenDriveLogger("MSC READ: lba: %u, offset: %u, bufsize: %u", lba, offset, bufsize);
  memcpy(buffer, msc_disk[lba] + offset, bufsize);
  return bufsize;
}


static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
  USBPendriveLog(String("MSC START/STOP: power: "+String(power_condition)+", start: "+String(start)+", eject: "+String(load_eject)));
  return true;
}



// SD Read/Write methods


static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
  ledControl::toggle();
  if( SD.writeRAW((uint8_t*)buffer, lba) ) {
    return bufsize;
  }
  ledControl::blink(500, 500);
  USBPendriveLog("Write Error");
  return -1;
  //if( SD.writeRAW((uint8_t*)buffer, lba) ) log_v("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  //else log_e("MSC WRITE FAIL: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  ledControl::toggle();
  //log_v("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  if( SD.readRAW((uint8_t*)buffer, lba) ) return bufsize;
  USBPendriveLog("Read Error");
  ledControl::blink(500, 500);
  return -1;
}




bool initPenDrive()
{
  if( WUDStatus::pendrive_begun ) {
    USBPendriveLog("PenDrive already started");
    return true;
  }

  if( !WUDStatus::sd_begun ) {
    initSD();
    //if( !initSD() ) duckyFS = &LittleFS;
    //else duckyFS = &SD;
  }

  MSC.vendorID("Pendrive");//max 8 chars
  MSC.productID("TheQuacken");//max 16 chars
  MSC.productRevision("0x2a");//max 4 chars
  MSC.onStartStop(onStartStop);
  MSC.mediaPresent(true);

  if( !WUDStatus::sd_begun ) {
    // no SD card is present or was detected, spawn a fake volume with a readme.txt
    MSC.onRead(onReadDefault);
    MSC.onWrite(onWriteDefault);
    WUDStatus::pendrive_begun = MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
    USBPendriveLog("SD Card not present, spawning fake partition");
  } else {
    MSC.onRead(onRead);
    MSC.onWrite(onWrite);
    WUDStatus::pendrive_begun = MSC.begin( SD.cardSize()/512, 512 );
    if( USBPenDriveLogger ) USBPenDriveLogger("PenDrive+SD %s", WUDStatus::pendrive_begun?"started successfully":"failed to start" );
  }

  ledControl::set( WUDStatus::pendrive_begun );

  return WUDStatus::pendrive_begun;
}

void deinitPenDrive()
{
  if( WUDStatus::pendrive_begun ) {
    MSC.end();
    WUDStatus::pendrive_begun = false;
    USBPendriveLog("PenDrive stopped");
  } else {
    USBPendriveLog("PenDrive not running, nothing to stop");
  }
  ledControl::set( WUDStatus::pendrive_begun );
}


bool initSD()
{
  if(initSD(SD_SCK, SD_MISO, SD_MOSI, SD_CS)) {
    USBPendriveLog("SD started successfully");
    return true;
  } else {
    USBPendriveLog("Error, PenDrive disabled");
    return false;
  }
}



bool initSD(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
  SPI.begin(sck, miso, mosi, ss);

  uint8_t attempts = 0;
  uint8_t maxattempts = 10;

  // hammering the SD is necessary since SD.h was fucked up (2.0.2)
  do {
    WUDStatus::sd_begun = SD.begin(ss, SPI, 40000000);
    if(WUDStatus::sd_begun) break;
    USBPendriveLog("SD Card Mount attempt: " + String(attempts));
    vTaskDelay(100);
  } while( attempts++ <= maxattempts );

  if( !WUDStatus::sd_begun) {
    USBPendriveLog("SD Card Mount Failed");
    return false;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    USBPendriveLog("No SD card attached");
    return false;
  }
  USBPendriveLog("SD Card Mount Success");
  return true;
}


void deinitSD()
{
  if( WUDStatus::pendrive_begun ) {
    deinitPenDrive();
  }
  SD.end();
  duckyFS = &LittleFS;
  WUDStatus::sd_begun = false;
}
