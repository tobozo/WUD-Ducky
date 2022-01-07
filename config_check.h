// Configuration checks Arduino core, esp-idf, USB menu options


// check Arduino core version
#if !defined ESP_ARDUINO_VERSION_VAL
  #error "Invalid esp32-core version (expected: >=2.0.2)"
#else
  #if defined ESP_ARDUINO_VERSION && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0)
    // fine, that's our target esp32 core
  #else
    #error "Invalid esp32-core version (expected: >=2.0.2)"
  #endif
#endif


// check esp-idf version
#if !defined ESP_IDF_VERSION_VAL
  #error "Invalid esp-idf version (expected: >=4.4.0)"
#else
  #if defined ESP_IDF_VERSION && ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
    // fine, that's our target for esp-idf core
  #else
    #error "Invalid esp-idf version (expected: >=4.4.0)"
  #endif
#endif

// check target config for device
#if !defined CONFIG_IDF_TARGET_ESP32S2
  #error "This project is for ESP32-S2 only!"
#endif

// check config for USB
#if !defined CONFIG_TINYUSB_ENABLED
  #error "This project needs CONFIG_TINYUSB_ENABLED in sdkconfig"
#endif

// check that required Arduino menu options are present
#if !defined ARDUINO_USB_CDC_ON_BOOT
  #error "ARDUINO_USB_CDC_ON_BOOT build option is missing"
#endif

#if !defined ARDUINO_USB_MSC_ON_BOOT
  #error "ARDUINO_USB_MSC_ON_BOOT build option is missing"
#endif
#if !defined ARDUINO_USB_DFU_ON_BOOT
  #error "ARDUINO_USB_DFU_ON_BOOT build option is missing"
#endif

// check that required Arduino menu options are well set
#if ARDUINO_USB_CDC_ON_BOOT!=0
  #warning "The menu option 'Tools / USB CDC On Boot' should be disabled for more furtivity!"
#endif
#if ARDUINO_USB_MSC_ON_BOOT==0
  #warning "The menu option 'Tools / USB Firmware MSC On Boot' should be disabled or the SD Card will be ignored!"
#endif
#if ARDUINO_USB_DFU_ON_BOOT!=0
  #error "The menu option 'Tools / USB DFU On Boot' should be disabled!"
#endif
