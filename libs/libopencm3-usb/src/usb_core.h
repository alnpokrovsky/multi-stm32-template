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

#ifndef __LIB_USB_PRIVATE_USB_CORE_H__
#define __LIB_USB_PRIVATE_USB_CORE_H__

#include "UsbConfig.h"
#include <libopencm3/usb/usbd.h>



void usb_set_serial_number(const char* serial);
usbd_device* usb_core_init(void);
void msc_setup(usbd_device* usbd_dev0);
uint16_t send_msc_packet(const void *buf, int len);
void dump_usb_request(const char *msg, struct usb_setup_data *req);
int aggregate_register_config_callback(
    usbd_device *usbd_dev,
	usbd_set_config_callback callback);
int aggregate_register_callback(
    usbd_device *usbd_dev, 
    uint8_t type,
    uint8_t type_mask,
    usbd_control_callback callback);

#endif
