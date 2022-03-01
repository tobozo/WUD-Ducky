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

// WORK IN PROGRESS, NOTHING IS FUNCTIONAL HERE

#if 0


#include "USBVendor.h"
#include "../logger.h"

static const char * strRequestDirections[] = {"OUT", "IN"};
static const char * strRequestTypes[] = {"STANDARD", "CLASS", "VENDOR", "INVALID"};
static const char * strRequestRecipients[] = {"DEVICE", "INTERFACE", "ENDPOINT", "OTHER"};
static const char * strRequestStages[] = {"SETUP", "DATA", "ACK"};

//CDC Control Requests
#define REQUEST_SET_LINE_CODING 0x20
#define REQUEST_GET_LINE_CODING 0x21
#define REQUEST_SET_CONTROL_LINE_STATE 0x22



// VENDOR sub-class for the reset interface
#define RESET_INTERFACE_SUBCLASS 0x00
// VENDOR protocol for the reset interface
#define RESET_INTERFACE_PROTOCOL 0x01

// CONTROL requests:

// reset to BOOTSEL
#define RESET_REQUEST_BOOTSEL 0x01
// regular flash boot
#define RESET_REQUEST_FLASH 0x02
// https://github.com/raspberrypi/pico-sdk/blob/ed45e8ae3b4b8f54fd286d12e3ed8ec196329a0c/src/rp2_common/pico_stdio_usb/reset_interface.c
#include "tusb.h"
#include "device/usbd_pvt.h"

static uint8_t itf_num;

static void resetd_init(void)
{
}

static void resetd_reset(uint8_t __unused rhport)
{
    itf_num = 0;
}

static uint16_t resetd_open(uint8_t __unused rhport, tusb_desc_interface_t const *itf_desc, uint16_t max_len)
{
    TU_VERIFY(TUSB_CLASS_VENDOR_SPECIFIC == itf_desc->bInterfaceClass &&
              RESET_INTERFACE_SUBCLASS == itf_desc->bInterfaceSubClass &&
              RESET_INTERFACE_PROTOCOL == itf_desc->bInterfaceProtocol, 0);

    uint16_t const drv_len = sizeof(tusb_desc_interface_t);
    TU_VERIFY(max_len >= drv_len, 0);

    itf_num = itf_desc->bInterfaceNumber;
    return drv_len;
}

// Support for parameterized reset via vendor interface control request
static bool resetd_control_xfer_cb(uint8_t __unused rhport, uint8_t stage, tusb_control_request_t const * request) {
    // nothing to do with DATA & ACK stage

    Logger::logsprintf("Vendor Request: Stage: %5s, Type: %8s, bRequest: 0x%02x, wValue: 0x%04x, wIndex: %u, wLength: %u",
      strRequestStages[stage],
      strRequestTypes[request->bmRequestType],
      request->bRequest, request->wValue, request->wIndex, request->wLength
    );


    if (stage != CONTROL_STAGE_SETUP) return true;

    if (request->wIndex == itf_num) {

//         Logger::logsprintf("USB Request: Type: %8s, bRequest: 0x%02x, wValue: 0x%04x, wIndex: %u, wLength: %u",
//           strRequestTypes[request->bmRequestType],
//           request->bRequest, request->wValue, request->wIndex, request->wLength
//         );

/*
#if PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_BOOTSEL
        if (request->bRequest == RESET_REQUEST_BOOTSEL) {
#ifdef PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED
            uint gpio_mask = 1u << PICO_STDIO_USB_RESET_BOOTSEL_ACTIVITY_LED;
#else
            uint gpio_mask = 0u;
#endif
#if !PICO_STDIO_USB_RESET_BOOTSEL_FIXED_ACTIVITY_LED
            if (request->wValue & 0x100) {
                gpio_mask = 1u << (request->wValue >> 9u);
            }
#endif
            reset_usb_boot(gpio_mask, (request->wValue & 0x7f) | PICO_STDIO_USB_RESET_BOOTSEL_INTERFACE_DISABLE_MASK);
            // does not return, otherwise we'd return true
        }
#endif

#if PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_RESET_TO_FLASH_BOOT
        if (request->bRequest == RESET_REQUEST_FLASH) {
            watchdog_reboot(0, 0, PICO_STDIO_USB_RESET_RESET_TO_FLASH_DELAY_MS);
            return true;
        }
#endif
*/
    }
    return false;
}
/*
static bool resetd_control_complete_cb(uint8_t rhport, tusb_control_request_t __unused const *request)
{
  Logger::logsprintf("resetd_control_complete_cb rhport: %d", rhport );
  return true;
}*/

static bool resetd_xfer_cb(uint8_t __unused rhport, uint8_t __unused ep_addr, xfer_result_t __unused result, uint32_t __unused xferred_bytes)
{
    return true;
}

static usbd_class_driver_t const _resetd_driver =
{
#if CFG_TUSB_DEBUG >= 2
    .name = "RESET",
#endif
    .init             = resetd_init,
    .reset            = resetd_reset,
    .open             = resetd_open,
    .control_xfer_cb  = resetd_control_xfer_cb,
    .xfer_cb          = resetd_xfer_cb,
    .sof              = NULL
};
/*
// Implement callback to add our custom driver
usbd_class_driver_t const *usbd_app_driver_get_cb(uint8_t *driver_count)
{
    *driver_count = 1;
    Logger::logsprintf("RST Driver count: %d", *driver_count );
    return &_resetd_driver;
}
*/


/*
#if PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE
// Support for default BOOTSEL reset by changing baud rate
void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    if (p_line_coding->bit_rate == PICO_STDIO_USB_RESET_MAGIC_BAUD_RATE) {
        reset_usb_boot(0, 0);
    }
}
#endif
*/






#define TUD_RPI_RESET_DESC_LEN  9
#define USBD_ITF_MAX       (5)
#define USBD_ITF_RPI_RESET (2)
#define USBD_STR_RPI_RESET (0x05)
#define USBD_STR_0 (0x00)
#define USBD_MAX_POWER_MA (15)

#define USBD_DESC_LEN TUD_RPI_RESET_DESC_LEN // (TUD_CONFIG_DESC_LEN + TUD_RPI_RESET_DESC_LEN)

#define TUD_RPI_RESET_DESCRIPTOR(_itfnum, _stridx) \
  /* Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 0, TUSB_CLASS_VENDOR_SPECIFIC, RESET_INTERFACE_SUBCLASS, RESET_INTERFACE_PROTOCOL, _stridx,

static const uint8_t usbrst_desc_cfg[USBD_DESC_LEN] = {
  /*
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_MAX, USBD_STR_0, USBD_DESC_LEN,
        0, USBD_MAX_POWER_MA),
*/
/*
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
*/
//#if PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE
    TUD_RPI_RESET_DESCRIPTOR(USBD_ITF_RPI_RESET, USBD_STR_RPI_RESET)
//#endif
};
/*
static char usbd_serial_str[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];

static const char *const usbd_desc_str[] = {
    [USBD_STR_MANUF] = "Raspberry Pi",
    [USBD_STR_PRODUCT] = "Pico",
    [USBD_STR_SERIAL] = usbd_serial_str,
    [USBD_STR_CDC] = "Board CDC",
#if PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE
    [USBD_STR_RPI_RESET] = "Reset",
#endif
};
*/
/*
const uint8_t *tud_descriptor_device_cb(void) {
    return (const uint8_t *)&usbd_desc_device;
}
*/
/*
const uint8_t *tud_descriptor_configuration_cb(__unused uint8_t index) {
    return usbrst_desc_cfg;
}
*/
/*
const uint16_t *tud_descriptor_string_cb(uint8_t index, __unused uint16_t langid) {
    #define DESC_STR_MAX (20)
    static uint16_t desc_str[DESC_STR_MAX];

    // Assign the SN using the unique flash id
    if (!usbd_serial_str[0]) {
        pico_get_unique_board_id_string(usbd_serial_str, sizeof(usbd_serial_str));
    }

    uint8_t len;
    if (index == 0) {
        desc_str[1] = 0x0409; // supported language is English
        len = 1;
    } else {
        if (index >= sizeof(usbd_desc_str) / sizeof(usbd_desc_str[0])) {
            return NULL;
        }
        const char *str = usbd_desc_str[index];
        for (len = 0; len < DESC_STR_MAX - 1 && str[len]; ++len) {
            desc_str[1 + len] = str[len];
        }
    }

    // first byte is length (including header), second byte is string type
    desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * len + 2));

    return desc_str;
}
*/


static bool tinyusb_rst_is_initialized = false;

/*
static uint16_t _onGetDescriptor(uint8_t* buffer)
{
    memcpy(buffer, usbrst_desc_cfg, sizeof(usbrst_desc_cfg));
    return sizeof(usbrst_desc_cfg);
}
*/

extern "C" uint16_t tusb_rst_load_descriptor(uint8_t * dst, uint8_t * itf)
{
    if(tinyusb_rst_is_initialized){
        return 0;
    }
    tinyusb_rst_is_initialized = true;

    uint8_t str_index = tinyusb_add_string_descriptor("TinyUSB RST");
    uint8_t ep_in = tinyusb_get_free_in_endpoint();
    TU_VERIFY (ep_in != 0);
    uint8_t ep_out = tinyusb_get_free_out_endpoint();
    TU_VERIFY (ep_out != 0);

    *itf+=1;
    memcpy(dst, usbrst_desc_cfg, USBD_DESC_LEN);
    return USBD_DESC_LEN;
}



// Now the absolute mouse mouse implementation, keep it minimalistic for
// the meantime since an official AbsMouse implementation is on the way.
class USBReset
{
private:
    //USBHID hid;
public:
    USBReset(void);
    void begin(void);
    //uint16_t _onGetDescriptor(uint8_t* buffer);
    //bool sendReport(abs_mouse_report_t * value);
    //void end();
};


USBReset::USBReset() {
    static bool initialized = false;
    if(!initialized){
        initialized = true;
        //hid.addDevice(this, sizeof(usbrst_desc_cfg));
        tinyusb_enable_interface(USB_INTERFACE_VENDOR, USBD_DESC_LEN, tusb_rst_load_descriptor);
    }
}

void USBReset::begin(void)
{
    //hid.begin();
}
/*
uint16_t USBReset::_onGetDescriptor(uint8_t* buffer)
{
    memcpy(buffer, usbrst_desc_cfg, sizeof(usbrst_desc_cfg));
    return sizeof(usbrst_desc_cfg);
}*/
/*
bool USBReset::sendReport(abs_mouse_report_t * report)
{
    return hid.SendReport( HID_REPORT_ID_MOUSE, report, sizeof(abs_mouse_report_t) );
}*/
/*
void USBReset::end()
{
    //hid.end();
}
*/



//USBReset Rst;



USBVendor Vendor;



namespace GuessedHost {
  typedef enum {
      UNSURE,
      LINUX,
      WINDOWS,
      MACOS,
  } OSVariant;
};



//extern "C" {
  //#include "device/usbd_pvt.h"

  /*
    typedef struct
    {
      #if CFG_TUSB_DEBUG >= 2
      char const* name;
      #endif

      void     (* init             ) (void);
      void     (* reset            ) (uint8_t rhport);
      uint16_t (* open             ) (uint8_t rhport, tusb_desc_interface_t const * desc_intf, uint16_t max_len);
      bool     (* control_xfer_cb  ) (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
      bool     (* xfer_cb          ) (uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes);
      void     (* sof              ) (uint8_t rhport); // optional
    } usbd_class_driver_t;
   */
/*
  static usbd_class_driver_t const _resetd_driver =
  {
  #if CFG_TUSB_DEBUG >= 2
      .name = "RESET",
  #endif
      .init             = resetd_init,
      .reset            = resetd_reset,
      .open             = resetd_open,
      .control_request  = resetd_control_request_cb,
      .control_complete = resetd_control_complete_cb,
      .xfer_cb          = resetd_xfer_cb,
      .sof              = NULL
  };

  // Implement callback to add our custom driver
  usbd_class_driver_t const *usbd_app_driver_get_cb(uint8_t *driver_count) {
      *driver_count = 1;
      return &_resetd_driver;
  }
*/
  /*

  usbd_class_driver_t const* usbd_app_driver_get_cb(uint8_t* driver_count)
  {
    Logger::logsprintf("%d", *driver_count );
    return NULL;
  }
  */
/*
  void dddddinit()
  {
    uint8_t drvcount = 0x00;
    uint8_t* drvnum = &drvcount;
    //const usbd_class_driver_t *drv = usbd_app_driver_get_cb(drvnum);
    //Logger::logsprintf("%d %d %d %d %d %d", *drv->init, *drv->reset, *drv->open, *drv->control_xfer_cb, *drv->xfer_cb, *drv->sof );
  }
*/
//}


namespace FingerPrintUSBHost
{




  using namespace GuessedHost;


  struct
  {
      uint8_t maybe_linux:1;
      uint8_t maybe_win:1;
      uint8_t maybe_mac:1;
      uint8_t not_linux:1;
      uint8_t not_win:1;
      uint8_t not_mac:1;
  } guess;


  OSVariant guessHostOS(void)
  {
      if (guess.not_mac && guess.not_linux && guess.maybe_win) {
          return GuessedHost::WINDOWS;
      } else if (guess.maybe_linux && !guess.not_linux) {
          return GuessedHost::LINUX;
      } else if (! guess.not_mac)  {
          return GuessedHost::MACOS;

      } else {
          return GuessedHost::UNSURE;
      }
  }

  void guessHostOS(String &os) {
      switch (guessHostOS()) {
      case GuessedHost::WINDOWS:
          os="Windows";
          break;
      case GuessedHost::LINUX:
          os="Linux";
          break;
      case GuessedHost::MACOS:
          os="MacOS";
          break;
      default:
          os="unsure";
          break;
      }
  }





  //CDC Line Coding Control Request Structure
  typedef struct __attribute__ ((packed)) {
    uint32_t bit_rate;
    uint8_t  stop_bits; //0: 1 stop bit, 1: 1.5 stop bits, 2: 2 stop bits
    uint8_t  parity;    //0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
    uint8_t  data_bits; //5, 6, 7, 8 or 16
  } request_line_coding_t;

  static request_line_coding_t vendor_line_coding = {115200, 0, 0, 8};

  // Bit 0:  DTR (Data Terminal Ready), Bit 1: RTS (Request to Send)
  static uint8_t vendor_line_state = 0;


  //Handle USB requests to the vendor interface
  bool vendorRequestCallback(uint8_t rhport, uint8_t requestStage, arduino_usb_control_request_t const * request)
  {
    String reason = "";

/*

    if (request->bmRequestDirection == REQUEST_DIRECTION_OUT &&
        request->bmRequestType == REQUEST_TYPE_STANDARD &&
        request->bmRequestRecipient == REQUEST_RECIPIENT_INTERFACE &&
        request->bRequest == 0x0b
      ) {
      if (requestStage == REQUEST_STAGE_SETUP) {
        // response with status OK
        //result = Vendor.sendResponse(rhport, request);
        //if (request->bmRequestType != REQUEST_DEVICETOHOST || request->bRequest != GET_DESCRIPTOR || request->wValue != USB_STRING_DESCRIPTOR_TYPE )
        if (request->bmRequestType != 0x80  || request->bRequest != 0x06 || request->wValue !=  0x03 )  {
            // return false;
            // ignore
            reason = "ignored (1)";
        } else {

          if (request->wLength == 0xff) {
              guess.maybe_linux = 1;
              guess.maybe_win = 1;
              guess.not_mac = 1; // In testing, MacOS NEVER sets a descript request lenght of 255
          } else {
              guess.not_linux = 1; // In testing, Linux ALWAYS sets a descriptor request length of 255;
          }
          reason = "guessed";
        }
      } else {
        reason = "ignored (non-setup)";
      }
    } else {
      reason = "ignored (dir)";
    }

    Logger::logsprintf("%s Vendor Request: Stage: %5s, Direction: %3s, Type: %8s, Recipient: %9s, bRequest: 0x%02x, wValue: 0x%04x, wIndex: %u, wLength: %u",
      reason.c_str(),
      strRequestStages[requestStage],
      strRequestDirections[request->bmRequestDirection],
      strRequestTypes[request->bmRequestType],
      strRequestRecipients[request->bmRequestRecipient],
      request->bRequest, request->wValue, request->wIndex, request->wLength
    );

    return false;
    */


    Logger::logsprintf("Vendor Request: Stage: %5s, Direction: %3s, Type: %8s, Recipient: %9s, bRequest: 0x%02x, wValue: 0x%04x, wIndex: %u, wLength: %u\n",
      strRequestStages[requestStage],
      strRequestDirections[request->bmRequestDirection],
      strRequestTypes[request->bmRequestType],
      strRequestRecipients[request->bmRequestRecipient],
      request->bRequest, request->wValue, request->wIndex, request->wLength);

    bool result = false;

    if (request->bmRequestDirection == REQUEST_DIRECTION_OUT &&
        request->bmRequestType == REQUEST_TYPE_STANDARD &&
        request->bmRequestRecipient == REQUEST_RECIPIENT_INTERFACE &&
        request->bRequest == 0x0b
      ) {
      if (requestStage == REQUEST_STAGE_SETUP) {
        // response with status OK
        result = Vendor.sendResponse(rhport, request);
      } else {
        result = true;
      }
    } else {
      //Implement CDC Control Requests
      if (request->bmRequestType == REQUEST_TYPE_CLASS && request->bmRequestRecipient == REQUEST_RECIPIENT_DEVICE) {
        switch (request->bRequest) {

          case REQUEST_SET_LINE_CODING: //0x20
            // Accept only direction OUT with data size 7
            if (request->wLength != sizeof(request_line_coding_t) || request->bmRequestDirection != REQUEST_DIRECTION_OUT) {
              break;
            }
            if (requestStage == REQUEST_STAGE_SETUP) {
              //Send the response in setup stage (it will write the data to vendor_line_coding in the DATA stage)
              result = Vendor.sendResponse(rhport, request, (void*) &vendor_line_coding, sizeof(request_line_coding_t));
            } else if (requestStage == REQUEST_STAGE_ACK) {
              //In the ACK stage the response is complete
              Logger::logsprintf("Vendor Line Coding: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", vendor_line_coding.bit_rate, vendor_line_coding.data_bits, vendor_line_coding.stop_bits, vendor_line_coding.parity);
            }
            result = true;
            break;

          case REQUEST_GET_LINE_CODING: //0x21
            // Accept only direction IN with data size 7
            if (request->wLength != sizeof(request_line_coding_t) || request->bmRequestDirection != REQUEST_DIRECTION_IN) {
              break;
            }
            if (requestStage == REQUEST_STAGE_SETUP) {
              //Send the response in setup stage (it will write the data to vendor_line_coding in the DATA stage)
              result = Vendor.sendResponse(rhport, request, (void*) &vendor_line_coding, sizeof(request_line_coding_t));
            }
            result = true;
            break;

          case REQUEST_SET_CONTROL_LINE_STATE: //0x22
            // Accept only direction OUT with data size 0
            if (request->wLength != 0 || request->bmRequestDirection != REQUEST_DIRECTION_OUT) {
              break;
            }
            if (requestStage == REQUEST_STAGE_SETUP) {
              //Send the response in setup stage
              vendor_line_state = request->wValue;
              result = Vendor.sendResponse(rhport, request);
            } else if (requestStage == REQUEST_STAGE_ACK) {
              //In the ACK stage the response is complete
              bool dtr = (vendor_line_state & 1) != 0;
              bool rts = (vendor_line_state & 2) != 0;
              Logger::logsprintf("Vendor Line State: dtr: %u, rts: %u\n", dtr, rts);
            }
            result = true;
            break;

          default:
            // stall unknown request
            break;
        }
      }
    }
    return result;

  }


  // typedef bool (*tuh_control_complete_cb_t)(uint8_t dev_addr, tusb_control_request_t const * request, xfer_result_t result);
  // typedef bool (*usbd_control_xfer_cb_t)(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);

  bool my_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
  {
    Logger::logsprintf("USB Request: Stage: %5s, Type: %8s, bRequest: 0x%02x, wValue: 0x%04x, wIndex: %u, wLength: %u",
      strRequestStages[stage],
      strRequestTypes[request->bmRequestType],
      request->bRequest, request->wValue, request->wIndex, request->wLength
    );
    return true;
  }


};

/*
extern "C"
{

  usbd_control_set_complete_callback( (usbd_control_xfer_cb_t)FingerPrintUSBHost::my_control_xfer_cb );
}
*/

#endif
