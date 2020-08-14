#ifndef __USB_CONFIG_H__
#define __USB_CONFIG_H__


#define USB_VID                 0x42db
#define USB_PID                 0x4045


//  Index of each USB interface.  Must be consecutive and must sync with interfaces[].
// #define USB_INTERFACE_DFU         0 /* TODO */
// #define USB_INTERFACE_MSC         1
#define USB_INTERFACE_CDC_COMM    2
#define USB_INTERFACE_CDC_DATA    3
// #define USB_INTERFACE_KEYBOARD    4
// #define USB_INTERFACE_HID         5
// #define USB21_INTERFACE              /* Enable USB 2.1 with WebUSB and BOS support.*/

#ifdef USB_INTERFACE_DFU
// DFU loader config
#define USB_DFU_UPLOAD_AVAILABLE   1
#define USB_DFU_DOWNLOAD_AVAILABLE 1
#endif

#ifdef USB_INTERFACE_MSC
#include "controls/ghostfat.h"
#define USB_MSC_VENDOR_ID       "Pokrov"
#define USB_MSC_PRODUCT_ID      "driver"
#define USB_MSC_SECTOR_SIZE     GHOSTFAT_SECTOR_SIZE
#define USB_MSC_TOTAL_SECTORS   GHOSTFAT_TOTAL_SECTORS
#define USB_MSC_INIT()          ghostfat_init()
#define USB_MSC_WRITE_BLOCK     ghostfat_write_block
#define USB_MSC_READ_BLOCK      ghostfat_read_block
#endif

#ifdef USB_INTERFACE_CDC_DATA
#endif

#ifdef USB_INTERFACE_HID
#define USB_HID_DATA_SIZE     16
#endif

#ifdef USB21_INTERFACE
#define USB21_URL "https://visualbluepill.github.io"
#endif

#define USB_STRINGS { \
    "POKROV",        /*  USB Manufacturer */ \
    "STM32HID",       /*  USB Product Description */ \
    (const char*)USBD_StringSerial,   /*  Serial number */ \
    "DFU",           /*  DFU */ \
    "MSC",           /*  MSC */ \
    "Serial Port",   /*  Serial Port */ \
    "COMM",          /*  COMM */ \
    "DATA",          /*  DATA */ \
    "KEYBOARD",                  \
    "HID",                       \
};

/**
 * Index of USB strings.  Must sync with above, starts from 1.
 */
enum usb_strings_index {
    USB_STRINGS_IDX_LANGID_STR = 0,
    USB_STRINGS_IDX_MANUFACTURER = 1,
    USB_STRINGS_IDX_PRODUCT,
    USB_STRINGS_IDX_SERIAL_NUMBER,
    USB_STRINGS_IDX_DFU,
    USB_STRINGS_IDX_MSC,
    USB_STRINGS_IDX_SERIAL_PORT,
    USB_STRINGS_IDX_COMM,
    USB_STRINGS_IDX_DATA,
    USB_STRINGS_IDX_KEYBOARD,
    USB_STRINGS_IDX_HID,
};



#define USBD_MAX_NUM_INTERFACES         1
#define USBD_MAX_NUM_CONFIGURATION      1
#define USBD_DEBUG_LEVEL                0
#define USBD_LPM_ENABLED                1
#define USBD_SELF_POWERED               1
// #define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE     2
// #define USBD_CUSTOM_HID_REPORT_DESC_SIZE     2

/****************************************/
/* #define for FS and HS identification */
#define DEVICE_FS 		0
#define DEVICE_HS 		1


/* Memory management macros */

/** Alias for memory allocation. */
#define USBD_malloc         malloc
#define USBD_free           free
#define USBD_memset         memset
#define USBD_memcpy         memcpy
#define USBD_Delay          HAL_Delay


/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)    printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)
#define USBD_ErrLog(...)    printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
#define USBD_DbgLog(...)    printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif


#endif