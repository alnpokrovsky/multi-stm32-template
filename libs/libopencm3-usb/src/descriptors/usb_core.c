//  CDC code from https://github.com/Apress/Beg-STM32-Devel-FreeRTOS-libopencm3-GCC/blob/master/rtos/usbcdcdemo/usbcdc.c
/*
 * Copyright (c) 2016, Devan Lai
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "usb_core.h"
#include "UsbConfig.h"

#include <stdlib.h>
#include <string.h>

#include <libopencm3/usb/usbd.h>

#include "dfu.h"
#include "cdc.h"
#include "webusb.h"
#include "winusb.h"
#include "usb21_standard.h"
#include "../private/setup.h"
#include "bootloader.h"
#include "msc.h"
#include "core/aggregate.h"


#ifdef USB21_INTERFACE
static const char* origin_url = "https://visualbluepill.github.io";
#endif  //  USB21_INTERFACE

static char serial_number[USB_SERIAL_NUM_LENGTH+1];
static const char *usb_strings[] = USB_STRINGS;

#define USB_CLASS_MISCELLANEOUS 0xef  //  Copy from microbit.



//  USB Device
static const struct usb_device_descriptor dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
#ifdef USB21_INTERFACE
    .bcdUSB = 0x0210,  //  USB Version 2.1.  Need to handle special requests e.g. BOS.
#else
    .bcdUSB = 0x0200,  //  USB Version 2.0.  No need to handle special requests e.g. BOS.
#endif  //  USB21_INTERFACE

#ifdef SERIAL_USB_INTERFACE  //  If we are providing serial interface only...
	.bDeviceClass = USB_CLASS_CDC,  //  Set the class to CDC if it's only serial.  Serial interface will not start on Windows when class = 0.
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
#else  //  If we are providing multiple interfaces...
    .bDeviceClass = USB_CLASS_MISCELLANEOUS,  //  Copied from microbit. For composite device, let host probe the interfaces.
    .bDeviceSubClass = 2,  //  Common Class
    .bDeviceProtocol = 1,  //  Interface Association Descriptor
#endif  //  SERIAL_USB_INTERFACE
    .bMaxPacketSize0 = USB_MAX_PACKET_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0220,  //  Device Release number 2.2
    .iManufacturer = USB_STRINGS_MANUFACTURER,
    .iProduct = USB_STRINGS_PRODUCT,
    .iSerialNumber = USB_STRINGS_SERIAL_NUMBER,
    .bNumConfigurations = 1,
};




//  All USB Interfaces
static const struct usb_interface interfaces[] = {
#ifdef INTF_DFU    
    {
        .num_altsetting = 1,
        .altsetting = &dfu_iface,  //  Index must sync with INTF_DFU.
    }, 
#endif  //  INTF_DFU
#ifdef USB_INTERFACE_MSC    
    {
        .num_altsetting = 1,
        .altsetting = &msc_iface,  //  Index must sync with USB_INTERFACE_MSC.
    }, 	
#endif  //  USB_INTERFACE_MSC
#ifdef USB_INTERFACE_CDC_COMM
    {
        .num_altsetting = 1,
#ifndef SERIAL_USB_INTERFACE
	    .iface_assoc = &cdc_iface_assoc,  //  Mandatory for composite device with multiple interfaces.
#endif  //  SERIAL_USB_INTERFACE
        .altsetting = &comm_iface,  //  Index must sync with USB_INTERFACE_CDC_COMM.
    }, 
    {
        .num_altsetting = 1,
        .altsetting = &data_iface,  //  Index must sync with USB_INTERFACE_CDC_DATA.
    },
#endif  //  USB_INTERFACE_CDC_COMM
};

//  USB Config
static const struct usb_config_descriptor config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = sizeof(interfaces) / sizeof(struct usb_interface),
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,  //  Bus-powered, i.e. it draws power from USB bus.
    .bMaxPower = 0xfa,     //  500 mA. Copied from microbit.
    .interface = interfaces,
};



void usb_core_set_serial_number(const char* serial) {
    serial_number[0] = '\0';
    if (serial) {
        strncpy(serial_number, serial, USB_SERIAL_NUM_LENGTH);
        serial_number[USB_SERIAL_NUM_LENGTH] = '\0';
    }
}

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[USB_CONTROL_BUF_SIZE] __attribute__ ((aligned (2)));
static usbd_device* usbd_dev = NULL;

usbd_device* usb_core_init(void) {
    int num_strings = sizeof(usb_strings) / sizeof(const char*);
    // debug_print("usb_setup num_strings "); debug_print_int(num_strings); debug_println(""); // debug_flush(); ////
    usbd_dev = usb_setup(
        &dev, &config, 
        usb_strings, num_strings,
        usbd_control_buffer, sizeof(usbd_control_buffer));

//  The following USB setup functions will call aggregate_register_callback() to register callbacks.
#ifdef INTF_DFU    
    dfu_setup(usbd_dev, &bootloader_manifest_app, NULL, NULL);
#endif  //  INTF_DFU
#ifdef USB_INTERFACE_MSC    
    msc_setup(usbd_dev);
#endif  //  USB_INTERFACE_MSC
#ifdef USB_INTERFACE_CDC_COMM    
    cdc_setup(usbd_dev);
#endif  //  USB_INTERFACE_CDC_COMM

#ifdef USB21_INTERFACE
    //  Define USB 2.1 BOS interface used by WebUSB.
	usb21_setup(usbd_dev, &bos_descriptor);
#ifdef INTF_DFU
	winusb_setup(usbd_dev, INTF_DFU);
#else
	webusb_setup(usbd_dev, origin_url);
#endif
#endif  //  USB21_INTERFACE

    //  Set the aggregate callback.    
	usbd_register_set_config_callback(usbd_dev, aggregate_set_callback);
    // if (status < 0) { debug_println("*** usb_setup failed"); debug_flush(); }

    //  For WinUSB: Windows probes the compatible ID before setting the configuration, so also register the callback now.
    aggregate_set_callback(usbd_dev, (uint16_t) -1);
    return usbd_dev;
}
