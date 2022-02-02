#pragma once


#define SSID_MAX_LEN            32
// WPA2 calls for 8-63 ASCII printable characters
#define PASSWORD_MIN_LEN        8
#define PASSWORD_MAX_LEN        63
char AP_SSID[SSID_MAX_LEN]          = {0};
char AP_PASSWORD[PASSWORD_MAX_LEN]  = {0};

char STA_SSID[SSID_MAX_LEN]         = {0};
char STA_PASSWORD[PASSWORD_MAX_LEN] = {0};

namespace WUDStatus
{

  #if (ARDUINO_USB_CDC_ON_BOOT|ARDUINO_USB_MSC_ON_BOOT|ARDUINO_USB_DFU_ON_BOOT)
    bool usb_begun       = true;
  #else
    bool usb_begun       = false;
  #endif

  #if ARDUINO_USB_CDC_ON_BOOT
    bool usbserial_begun = true; // WUD will spawn a serial port
  #else
    bool usbserial_begun = false; // no serial port at boot
  #endif

  bool hwserial_begun  = false; // for debug
  bool keyboard_begun  = false;
  bool absmouse_begun  = false;
  bool hid_ready       = false;
  bool sd_begun        = false;
  bool pendrive_begun  = false;
  bool spiffs_begun    = false;
  bool webserver_begun = false;
  bool softap_begun    = false;
  bool wifista_begun   = false;
  bool disclaimer_done = false;
  bool numlock_on      = false;
  bool capslock_on     = false;
  bool scrolllock_on   = false;

};
