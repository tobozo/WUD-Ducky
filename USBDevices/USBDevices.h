#pragma once

#include "../USBDevices/USBKeyboard.h"
#include "../USBDevices/USBAbsMouse.h"
#include "../USBDevices/USBPendrive.h" // this defines HAS_PENDRIVE

USBHID HID;
HIDAbsMouse AbsMouse( &HID );
DuckyKeyboard Keyboard;
