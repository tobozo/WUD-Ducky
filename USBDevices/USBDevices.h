#pragma once

// Cursing after that Serial0/Serial/USBCDC unintuitive example from the official USB library:
#if ARDUINO_USB_CDC_ON_BOOT
  #define HWSerial Serial0
  #define USBSerial Serial
#else
  #include "USBCDC.h"
  #define HWSerial Serial
  USBCDC USBSerial;
#endif
// While this covers about all in terms of exhibiting the different flavours of Serial
// with or without USB, the mix of defines, objects and prefixes is more than confusing,
// and the resulting behaviour is opaque.
// 1) Both Serial USB and UART can be considered hardware.
// 2) Which of both Serial interfaces is relevant for debugging? your guess :-)
// 3) Why most USB examples from the official librayr use two Serial interfaces?
// 4) Using defines for that is such a poor choice, C++ can do so much better than this.
// 5) Enabling ARDUINO_USB_CDC_ON_BOOT starts USB before the setup() is reached

#include "../USBDevices/USBKeyboard.h"
#include "../USBDevices/USBAbsMouse.h"
#include "../USBDevices/USBPendrive.h"


USBHID HID;
HIDAbsMouse AbsMouse( &HID );
DuckyKeyboard Keyboard;


static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT: logmsg("USB PLUGGED"); break;
      case ARDUINO_USB_STOPPED_EVENT: logmsg("USB UNPLUGGED"); break;
      case ARDUINO_USB_SUSPEND_EVENT: logmsg( String( "USB SUSPENDED: remote_wakeup_en: " + String( data->suspend.remote_wakeup_en ) ) ); break;
      case ARDUINO_USB_RESUME_EVENT: logmsg("USB RESUMED"); break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_CDC_EVENTS) {
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT: logmsg("CDC CONNECTED"); break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT: logmsg("CDC DISCONNECTED"); break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT: logsprintf("CDC LINE STATE: dtr: %u, rts: %u", data->line_state.dtr, data->line_state.rts); break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT: logsprintf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity); break;
      case ARDUINO_USB_CDC_RX_EVENT: /* USBSerial.read(buf, data->rx.len); */ break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_HID_EVENTS) {
    arduino_usb_hid_event_data_t * data = (arduino_usb_hid_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_HID_SET_PROTOCOL_EVENT: logsprintf("HID SET PROTOCOL: %s", data->set_protocol.protocol?"REPORT":"BOOT"); break;
      case ARDUINO_USB_HID_SET_IDLE_EVENT: logsprintf("HID SET IDLE: %u", data->set_idle.idle_rate); break;
      default: break;
    }
  } else if(event_base == ARDUINO_USB_HID_KEYBOARD_EVENTS) {
    arduino_usb_hid_keyboard_event_data_t * data = (arduino_usb_hid_keyboard_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_HID_KEYBOARD_LED_EVENT: logsprintf("HID KEYBOARD LED: NumLock:%u, CapsLock:%u, ScrollLock:%u", data->numlock, data->capslock, data->scrolllock); break;
      default: break;
    }
  }
}
