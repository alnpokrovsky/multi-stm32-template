#ifndef __USB_CONFIG_H__
#define __USB_CONFIG_H__

#include "memflash.h"

#define USB_VID                 0x1209
#define USB_PID                 0xdb42

#define USB_MSC_VENDOR_ID       "BluePill"
#define USB_MSC_PRODUCT_ID      "MSC Config"


#define USB_SERIAL_NUM_LENGTH   24
#define USB_CONTROL_BUF_SIZE    256  //  Previously 1024
#define USB_MAX_PACKET_SIZE     64   //  Previously 32
#define USB_CDC_PACKET_SIZE     16


//  Index of each USB interface.  Must be consecutive and must sync with interfaces[].
//#define INTF_DFU                0 /* TODO */
#define USB_INTERFACE_MSC         1
#define USB_INTERFACE_CDC_COMM    2
#define USB_INTERFACE_CDC_DATA    3
#define USB21_INTERFACE              /* Enable USB 2.1 with WebUSB and BOS support.*/

#ifdef INTF_DFU
// DFU loader config
#define USB_DFU_UPLOAD_AVAILABLE 1
#define USB_DFU_DOWNLOAD_AVAILABLE 1
#endif


#define USB_STRINGS { \
    "POKROV",        /*  USB Manufacturer */ \
    "Product",       /*  USB Product */ \
    serial_number,   /*  Serial number */ \
    "DFU",           /*  DFU */ \
    "MSC",           /*  MSC */ \
    "Serial Port",   /*  Serial Port */ \
    "COMM",          /*  COMM */ \
    "DATA",          /*  DATA */ \
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