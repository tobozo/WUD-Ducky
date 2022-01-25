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

#define HAS_KEYBOARD

// this is just a copy of the official USBHIDKeyboard class with 'sendReport' moved from private to public

#include "USBConfig.h"
#include "Print.h"
#include "USBHID.h"
#if CONFIG_TINYUSB_HID_ENABLED

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(ARDUINO_USB_HID_KEYBOARD_EVENTS);

typedef enum {
    ARDUINO_USB_HID_KEYBOARD_ANY_EVENT = ESP_EVENT_ANY_ID,
    ARDUINO_USB_HID_KEYBOARD_LED_EVENT = 0,
    ARDUINO_USB_HID_KEYBOARD_MAX_EVENT,
} arduino_usb_hid_keyboard_event_t;

typedef union {
    struct {
            uint8_t numlock:1;
            uint8_t capslock:1;
            uint8_t scrolllock:1;
            uint8_t compose:1;
            uint8_t kana:1;
            uint8_t reserved:3;
    };
    uint8_t leds;
} arduino_usb_hid_keyboard_event_data_t;

#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT  0x81
#define KEY_LEFT_ALT    0x82
#define KEY_LEFT_GUI    0x83
#define KEY_RIGHT_CTRL  0x84
#define KEY_RIGHT_SHIFT 0x85
#define KEY_RIGHT_ALT   0x86
#define KEY_RIGHT_GUI   0x87

#define KEY_UP_ARROW    0xDA
#define KEY_DOWN_ARROW  0xD9
#define KEY_LEFT_ARROW  0xD8
#define KEY_RIGHT_ARROW 0xD7
#define KEY_BACKSPACE   0xB2
#define KEY_TAB         0xB3
#define KEY_RETURN      0xB0
#define KEY_ESC         0xB1
#define KEY_INSERT      0xD1
#define KEY_DELETE      0xD4
#define KEY_PAGE_UP     0xD3
#define KEY_PAGE_DOWN   0xD6
#define KEY_HOME        0xD2
#define KEY_END         0xD5
#define KEY_CAPS_LOCK   0xC1
#define KEY_F1          0xC2
#define KEY_F2          0xC3
#define KEY_F3          0xC4
#define KEY_F4          0xC5
#define KEY_F5          0xC6
#define KEY_F6          0xC7
#define KEY_F7          0xC8
#define KEY_F8          0xC9
#define KEY_F9          0xCA
#define KEY_F10         0xCB
#define KEY_F11         0xCC
#define KEY_F12         0xCD
#define KEY_F13         0xF0
#define KEY_F14         0xF1
#define KEY_F15         0xF2
#define KEY_F16         0xF3
#define KEY_F17         0xF4
#define KEY_F18         0xF5
#define KEY_F19         0xF6
#define KEY_F20         0xF7
#define KEY_F21         0xF8
#define KEY_F22         0xF9
#define KEY_F23         0xFA
#define KEY_F24         0xFB

#define LED_NUMLOCK     0x01
#define LED_CAPSLOCK    0x02
#define LED_SCROLLLOCK  0x04
#define LED_COMPOSE     0x08
#define LED_KANA        0x10

//  Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

class DuckyKeyboard: public USBHIDDevice, public Print
{
private:
    USBHID hid;
    KeyReport _keyReport;
public:
    DuckyKeyboard(void);
    void sendReport(KeyReport* keys);
    void begin(void);
    void end(void);
    size_t write(uint8_t k);
    size_t write(const uint8_t *buffer, size_t size);
    size_t press(uint8_t k);
    size_t release(uint8_t k);
    void releaseAll(void);

    //raw functions work with TinyUSB's HID_KEY_* macros
    size_t pressRaw(uint8_t k);
    size_t releaseRaw(uint8_t k);

    void onEvent(esp_event_handler_t callback);
    void onEvent(arduino_usb_hid_keyboard_event_t event, esp_event_handler_t callback);

    // internal use
    uint16_t _onGetDescriptor(uint8_t* buffer);
    void _onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len);
};

//ESP_EVENT_DEFINE_BASE(ARDUINO_USB_HID_KEYBOARD_EVENTS);
esp_err_t arduino_usb_event_post(esp_event_base_t event_base, int32_t event_id, void *event_data, size_t event_data_size, TickType_t ticks_to_wait);
esp_err_t arduino_usb_event_handler_register_with(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler, void *event_handler_arg);

static const uint8_t report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_REPORT_ID_KEYBOARD))
};

DuckyKeyboard::DuckyKeyboard(): hid(){
    static bool initialized = false;
    if(!initialized){
        initialized = true;
        hid.addDevice(this, sizeof(report_descriptor));
    }
}

uint16_t DuckyKeyboard::_onGetDescriptor(uint8_t* dst){
    memcpy(dst, report_descriptor, sizeof(report_descriptor));
    return sizeof(report_descriptor);
}

void DuckyKeyboard::begin(){
    hid.begin();
}

void DuckyKeyboard::end(){
}

void DuckyKeyboard::onEvent(esp_event_handler_t callback){
    onEvent(ARDUINO_USB_HID_KEYBOARD_ANY_EVENT, callback);
}
void DuckyKeyboard::onEvent(arduino_usb_hid_keyboard_event_t event, esp_event_handler_t callback){
    arduino_usb_event_handler_register_with(ARDUINO_USB_HID_KEYBOARD_EVENTS, event, callback, this);
}

void DuckyKeyboard::_onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len){
    if(report_id == HID_REPORT_ID_KEYBOARD){
        arduino_usb_hid_keyboard_event_data_t p;
        p.leds = buffer[0];
        arduino_usb_event_post(ARDUINO_USB_HID_KEYBOARD_EVENTS, ARDUINO_USB_HID_KEYBOARD_LED_EVENT, &p, sizeof(arduino_usb_hid_keyboard_event_data_t), portMAX_DELAY);
    }
}

void DuckyKeyboard::sendReport(KeyReport* keys)
{
    hid_keyboard_report_t report;
    report.reserved = 0;
    report.modifier = keys->modifiers;
    if (keys->keys) {
        memcpy(report.keycode, keys->keys, 6);
    } else {
        memset(report.keycode, 0, 6);
    }
    hid.SendReport(HID_REPORT_ID_KEYBOARD, &report, sizeof(report));
}

#define SHIFT 0x80
const uint8_t _asciimap[128] =
{
    0x00,          // NUL
    0x00,          // SOH
    0x00,          // STX
    0x00,          // ETX
    0x00,          // EOT
    0x00,          // ENQ
    0x00,          // ACK
    0x00,          // BEL
    0x2a,          // BS   Backspace
    0x2b,          // TAB  Tab
    0x28,          // LF   Enter
    0x00,          // VT
    0x00,          // FF
    0x00,          // CR
    0x00,          // SO
    0x00,          // SI
    0x00,          // DEL
    0x00,          // DC1
    0x00,          // DC2
    0x00,          // DC3
    0x00,          // DC4
    0x00,          // NAK
    0x00,          // SYN
    0x00,          // ETB
    0x00,          // CAN
    0x00,          // EM
    0x00,          // SUB
    0x00,          // ESC
    0x00,          // FS
    0x00,          // GS
    0x00,          // RS
    0x00,          // US

    0x2c,          //  ' '
    0x1e|SHIFT,    // !
    0x34|SHIFT,    // "
    0x20|SHIFT,    // #
    0x21|SHIFT,    // $
    0x22|SHIFT,    // %
    0x24|SHIFT,    // &
    0x34,          // '
    0x26|SHIFT,    // (
    0x27|SHIFT,    // )
    0x25|SHIFT,    // *
    0x2e|SHIFT,    // +
    0x36,          // ,
    0x2d,          // -
    0x37,          // .
    0x38,          // /
    0x27,          // 0
    0x1e,          // 1
    0x1f,          // 2
    0x20,          // 3
    0x21,          // 4
    0x22,          // 5
    0x23,          // 6
    0x24,          // 7
    0x25,          // 8
    0x26,          // 9
    0x33|SHIFT,    // :
    0x33,          // ;
    0x36|SHIFT,    // <
    0x2e,          // =
    0x37|SHIFT,    // >
    0x38|SHIFT,    // ?
    0x1f|SHIFT,    // @
    0x04|SHIFT,    // A
    0x05|SHIFT,    // B
    0x06|SHIFT,    // C
    0x07|SHIFT,    // D
    0x08|SHIFT,    // E
    0x09|SHIFT,    // F
    0x0a|SHIFT,    // G
    0x0b|SHIFT,    // H
    0x0c|SHIFT,    // I
    0x0d|SHIFT,    // J
    0x0e|SHIFT,    // K
    0x0f|SHIFT,    // L
    0x10|SHIFT,    // M
    0x11|SHIFT,    // N
    0x12|SHIFT,    // O
    0x13|SHIFT,    // P
    0x14|SHIFT,    // Q
    0x15|SHIFT,    // R
    0x16|SHIFT,    // S
    0x17|SHIFT,    // T
    0x18|SHIFT,    // U
    0x19|SHIFT,    // V
    0x1a|SHIFT,    // W
    0x1b|SHIFT,    // X
    0x1c|SHIFT,    // Y
    0x1d|SHIFT,    // Z
    0x2f,          // [
    0x31,          // bslash
    0x30,          // ]
    0x23|SHIFT,    // ^
    0x2d|SHIFT,    // _
    0x35,          // `
    0x04,          // a
    0x05,          // b
    0x06,          // c
    0x07,          // d
    0x08,          // e
    0x09,          // f
    0x0a,          // g
    0x0b,          // h
    0x0c,          // i
    0x0d,          // j
    0x0e,          // k
    0x0f,          // l
    0x10,          // m
    0x11,          // n
    0x12,          // o
    0x13,          // p
    0x14,          // q
    0x15,          // r
    0x16,          // s
    0x17,          // t
    0x18,          // u
    0x19,          // v
    0x1a,          // w
    0x1b,          // x
    0x1c,          // y
    0x1d,          // z
    0x2f|SHIFT,    // {
    0x31|SHIFT,    // |
    0x30|SHIFT,    // }
    0x35|SHIFT,    // ~
    0              // DEL
};

size_t DuckyKeyboard::pressRaw(uint8_t k)
{
    uint8_t i;
    if (k >= 0xE0 && k < 0xE8) {
        // it's a modifier key
        _keyReport.modifiers |= (1<<(k-0x80));
    } else if (k && k < 0xA5) {
        // Add k to the key report only if it's not already present
        // and if there is an empty slot.
        if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
            _keyReport.keys[2] != k && _keyReport.keys[3] != k &&
            _keyReport.keys[4] != k && _keyReport.keys[5] != k) {

            for (i=0; i<6; i++) {
                if (_keyReport.keys[i] == 0x00) {
                    _keyReport.keys[i] = k;
                    break;
                }
            }
            if (i == 6) {
                return 0;
            }
        }
    } else {
        //not a modifier and not a key
        return 0;
    }
    sendReport(&_keyReport);
    return 1;
}

size_t DuckyKeyboard::releaseRaw(uint8_t k)
{
    uint8_t i;
    if (k >= 0xE0 && k < 0xE8) {
        // it's a modifier key
        _keyReport.modifiers &= ~(1<<(k-0x80));
    } else if (k && k < 0xA5) {
        // Test the key report to see if k is present.  Clear it if it exists.
        // Check all positions in case the key is present more than once (which it shouldn't be)
        for (i=0; i<6; i++) {
            if (0 != k && _keyReport.keys[i] == k) {
                _keyReport.keys[i] = 0x00;
            }
        }
    } else {
        //not a modifier and not a key
        return 0;
    }

    sendReport(&_keyReport);
    return 1;
}

// press() adds the specified key (printing, non-printing, or modifier)
// to the persistent key report and sends the report.  Because of the way
// USB HID works, the host acts like the key remains pressed until we
// call release(), releaseAll(), or otherwise clear the report and resend.
size_t DuckyKeyboard::press(uint8_t k)
{
    uint8_t i;
    if (k >= 0x88) {         // it's a non-printing key (not a modifier)
        k = k - 0x88;
    } else if (k >= 0x80) {  // it's a modifier key
        _keyReport.modifiers |= (1<<(k-0x80));
        k = 0;
    } else {                // it's a printing key
        k = _asciimap[k];
        if (!k) {
            return 0;
        }
        if (k & 0x80) {                     // it's a capital letter or other character reached with shift
            _keyReport.modifiers |= 0x02;   // the left shift modifier
            k &= 0x7F;
        }
    }
    return pressRaw(k);
}

// release() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t DuckyKeyboard::release(uint8_t k)
{
    uint8_t i;
    if (k >= 0x88) {         // it's a non-printing key (not a modifier)
        k = k - 0x88;
    } else if (k >= 0x80) {  // it's a modifier key
        _keyReport.modifiers &= ~(1<<(k-0x80));
        k = 0;
    } else {                // it's a printing key
        k = _asciimap[k];
        if (!k) {
            return 0;
        }
        if (k & 0x80) {                         // it's a capital letter or other character reached with shift
            _keyReport.modifiers &= ~(0x02);    // the left shift modifier
            k &= 0x7F;
        }
    }
    return releaseRaw(k);
}

void DuckyKeyboard::releaseAll(void)
{
    _keyReport.keys[0] = 0;
    _keyReport.keys[1] = 0;
    _keyReport.keys[2] = 0;
    _keyReport.keys[3] = 0;
    _keyReport.keys[4] = 0;
    _keyReport.keys[5] = 0;
    _keyReport.modifiers = 0;
    sendReport(&_keyReport);
}

size_t DuckyKeyboard::write(uint8_t c)
{
    uint8_t p = press(c);  // Keydown
    release(c);            // Keyup
    return p;              // just return the result of press() since release() almost always returns 1
}

size_t DuckyKeyboard::write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
        if (*buffer != '\r') {
            if (write(*buffer)) {
              n++;
            } else {
              break;
            }
        }
        buffer++;
    }
    return n;
}

#endif // CONFIG_TINYUSB_HID_ENABLED
