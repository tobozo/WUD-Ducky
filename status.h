#pragma once

#if (ARDUINO_USB_CDC_ON_BOOT|ARDUINO_USB_MSC_ON_BOOT|ARDUINO_USB_DFU_ON_BOOT)
  static bool usb_begun       = true;
#else
  static bool usb_begun       = false;
#endif

#if ARDUINO_USB_CDC_ON_BOOT
  static bool usbserial_begun = true; // WUD will spawn a serial port
#else
  static bool usbserial_begun = false; // no serial port at boot
#endif

static bool hwserial_begun  = false; // for debug
static bool keyboard_begun  = false;
static bool absmouse_begun  = false;
static bool hid_ready       = false;
static bool sd_begun        = false;
static bool pendrive_begun  = false;
static bool spiffs_begun    = false;
static bool webserver_begun = false;
static bool softap_begun    = false;
static bool wifista_begun   = false;


#define SSID_MAX_LEN            32
// WPA2 calls for 8-63 ASCII printable characters
#define PASSWORD_MIN_LEN        8
#define PASSWORD_MAX_LEN        63

static char AP_SSID[SSID_MAX_LEN]          = {0};
static char AP_PASSWORD[PASSWORD_MAX_LEN]  = {0};

static char STA_SSID[SSID_MAX_LEN]         = {0};
static char STA_PASSWORD[PASSWORD_MAX_LEN] = {0};
