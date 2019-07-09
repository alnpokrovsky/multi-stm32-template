#ifndef __USB_CONFIG_H__
#define __USB_CONFIG_H__

#include "memflash.h"

#define USB_VID                 0x1209
#define USB_PID                 0xdb42

#define MSC_VENDOR_ID           "BluePill"        //  Max 8 chars
#define MSC_PRODUCT_ID          "UF2 Bootloader"  //  Max 16 chars

#define PRODUCT_NAME            "STM32BLUEPILL"
#define BOARD_ID                "STM32BLUEPILL"
#define INDEX_URL               "https://visualbluepill.github.io"
#define VOLUME_LABEL            "BLUEPILL"
// where the UF2 files are allowed to write data - we allow MBR, since it seems part of the softdevice .hex file
#define USER_FLASH_START        (uint32_t)(APP_BASE_ADDRESS)
#define USER_FLASH_END          (memflash_end())


#define USB_SERIAL_NUM_LENGTH   24
#define USB_CONTROL_BUF_SIZE    256  //  Previously 1024
#define USB_MAX_PACKET_SIZE     64   //  Previously 32
#define USB_CDC_PACKET_SIZE     16

#define USB21_INTERFACE              //  Enable USB 2.1 with WebUSB and BOS support.
#define ALL_USB_INTERFACES           //  Enable all USB interfaces.

//  Index of each USB interface.  Must be consecutive and must sync with interfaces[].
#ifdef ALL_USB_INTERFACES
#define INTF_DFU                0
#define INTF_MSC                1
#define INTF_COMM               2
#define INTF_DATA               3
#endif  //  ALL_USB_INTERFACES


#define USB_STRINGS { \
    "Devanarchy",              /*  USB Manufacturer */ \
    "DAPBoot DFU Bootloader",  /*  USB Product */ \
    serial_number,             /*  Serial number */ \
    "DAPBoot DFU",             /*  DFU */ \
    "Blue Pill MSC",           /*  MSC */ \
    "Blue Pill Serial Port",   /*  Serial Port */ \
    "Blue Pill COMM",          /*  COMM */ \
    "Blue Pill DATA",          /*  DATA */ \
};

/**
 * Index of USB strings.  Must sync with above, starts from 1.
 */
enum usb_strings_index {
    USB_STRINGS_MANUFACTURER = 1,
    USB_STRINGS_PRODUCT,
    USB_STRINGS_SERIAL_NUMBER,
    USB_STRINGS_DFU,
    USB_STRINGS_MSC,
    USB_STRINGS_SERIAL_PORT,
    USB_STRINGS_COMM,
    USB_STRINGS_DATA,
};

#endif