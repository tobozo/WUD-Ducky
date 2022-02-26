#pragma once

// WORK IN PROGRESS, NOTHING IS FUNCTIONAL HERE


#if 0

//#include "USB.h"


// #ifndef CFG_TUD_ENDPOINT0_SIZE
// #define CFG_TUD_ENDPOINT0_SIZE    64
// #endif
//
// //------------- CLASS -------------//
// #define CFG_TUD_CDC               0
// #define CFG_TUD_MSC               0
// #define CFG_TUD_HID               0
// #define CFG_TUD_MIDI              0
// #define CFG_TUD_VENDOR            0
// #define CFG_TUD_NET               1

#include "esp32-hal-tinyusb.h"
//#include "tinyusb.h"
//#include "tusb_net.h"
//#include "tusb_bth.h"

// usb rndis/emc MAC address
const uint8_t tud_network_mac_address[6] = {0x02,0x02,0x84,0x6A,0x96,0x00};

//tinyusb network interface callbacks
bool tud_network_recv_cb(const uint8_t *src, uint16_t size);
uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg);
void tud_network_init_cb(void);

//extern tusb_desc_device_t desc_device;
//extern tusb_desc_strarray_device_t desc_string;
// gv
static esp_netif_t* netif;
static uint8_t tud_rx_buf[2048];
static size_t tud_rx_len = 0;

// usb descriptors

tusb_desc_device_t const dsc_device =
{
  .bLength            = sizeof(tusb_desc_device_t),
  .bDescriptorType    = TUSB_DESC_DEVICE,
  .bcdUSB             = 0x0200,

  // Use Interface Association Descriptor (IAD) device class
  .bDeviceClass       = TUSB_CLASS_MISC,
  .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
  .bDeviceProtocol    = MISC_PROTOCOL_IAD,

  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

  .idVendor           = 0xeeee, //vendor ID
  .idProduct          = 0x0001, //product ID
  .bcdDevice          = 0x0101, //device

  .iManufacturer      = 0x01, //manufacturer index in string descriptors
  .iProduct           = 0x02, //product index in string descriptors
  .iSerialNumber      = 0x03, //serial number in string descriptors

  .bNumConfigurations = 0x02 // multiple configurations (RNDIS + ECM)
};

// array of pointer to string descriptors
static const char *dsc_string[] =
{
  [0] = (char[]) { 0x09, 0x04 }, // supported language is English (0x0409)
  [1] = "Tech Genesis Enterprise",     // Manufacturer
  [2] = "usb-rndis-2-pppos",             // Product
  [3] = "000001",                      // Serial
  [4] = "USB PPPOS Network Interface"  // Interface Description
  // STRID_MAC index is handled separately
};

/*
tinyusb_config_cdcacm_t amc_cfg =
{
    .usb_dev = TINYUSB_USBDEV_0,
    .cdc_port = TINYUSB_CDC_ACM_0,
    .rx_unread_buf_sz = 128,
    .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
    .callback_rx_wanted_char = NULL,
    .callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
    .callback_line_coding_changed = NULL
};
*/

/*
tinyusb_config_t tusb_rndis_cfg =
{
  .descriptor = &dsc_device,
  .string_descriptor = dsc_string,
  .external_phy = false // In the most cases you need to use a `false` value
};
*/

#endif
