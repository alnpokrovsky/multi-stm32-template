#include "hid.h"
#include "usb_hid.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>
#include <libopencm3/cm3/nvic.h>
#include "basic/aggregate.h"
#include "../private/setup.h"
#include "UsbConfig.h"
#include <stdlib.h>

#ifdef USB_INTERFACE_HID

#define ENDP_ADDRESS_IN     USB_ENDPOINT_ADDR_IN (USB_INTERFACE_HID)
#define ENDP_ADDRESS_OUT    USB_ENDPOINT_ADDR_OUT(USB_INTERFACE_HID)


static void null_handler(uint8_t * buf, uint16_t len)
{
	/* echo by default */
	usb_hid_send(buf, len);
}

#pragma weak usb_hid_rx_handler = null_handler


static const uint8_t hid_report_descriptor[] =
{
       // Usage Page = 0xFF00 (Vendor Defined Page 1)
    0x06, 0xFF, 0xFF,
    // Usage (Vendor Usage 1)
    0x09, 0x01,
    // Collection (Application)
    0xA1, 0x01,
    //   Usage Minimum
    0x19, 0x01,
    //   Usage Maximum. 64 input usages total (0x01 to USB_HID_DATA_SIZE (prev 0x40)).
    0x29, USB_HID_DATA_SIZE,
    //   Logical Minimum (data bytes in the report may have minimum value =
    //   0x00).
    0x15, 0x00,
    //   Logical Maximum (data bytes in the report may have
    //     maximum value = 0x00FF = unsigned 255).
    // TODO: Can this be one byte?
    0x26, 0xFF, 0x00,
    //   Report Size: 8-bit field size
    0x75, 0x08,
    //   Report Count: Make sixty-four 8-bit fields (the next time the parser
    //     hits an "Input", "Output", or "Feature" item).
    0x95, USB_HID_DATA_SIZE,
    //   Input (Data, Array, Abs): Instantiates input packet fields based on the
    //     above report size, count, logical min/max, and usage.
    0x81, 0x00,
    //   Usage Minimum
    0x19, 0x01,
    //   Usage Maximum. 64 output usages total (0x01 to 0x40)
    0x29, USB_HID_DATA_SIZE,
    //   Output (Data, Array, Abs): Instantiates output packet fields. Uses same
    //     report size and count as "Input" fields, since nothing new/different
    //     was specified to the parser since the "Input" item.
    0x91, 0x00,
    // End Collection
    0xC0, 
 };


static const struct {
    struct usb_hid_descriptor hid_descriptor;
    struct {
        uint8_t bReportDescriptorType;
        uint16_t wDescriptorLength;
    } __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {
    .hid_descriptor = {
        .bLength = sizeof(hid_function),
        .bDescriptorType = USB_DT_HID,
        .bcdHID = 0x0100,
        .bCountryCode = 0,
        .bNumDescriptors = 1,
    },
    .hid_report = {
        .bReportDescriptorType = USB_DT_REPORT,
        .wDescriptorLength = sizeof(hid_report_descriptor),
    }
};

static const struct usb_endpoint_descriptor hid_endpoints[] = {{
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = ENDP_ADDRESS_IN,
    .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize = USB_HID_DATA_SIZE,
    .bInterval = 10,
},
{
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = ENDP_ADDRESS_OUT,
    .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize = USB_HID_DATA_SIZE,
    .bInterval = 10,
}};

const struct usb_interface_descriptor hid_iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = USB_INTERFACE_HID,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = 0, /* no boot */
    .bInterfaceProtocol = 0, /* user (no mouse, keyboard, etc...)*/
    .iInterface = USB_STRINGS_HID,

    .endpoint = hid_endpoints,

    .extra = &hid_function,
    .extralen = sizeof(hid_function),
};


static enum usbd_request_return_codes hid_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
		uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
    (void)complete;
    (void)usbd_dev;

    // This request is asking for information sent to the host using request
    // GET_DESCRIPTOR.
    if ((req->bmRequestType & USB_REQ_TYPE_DIRECTION) == USB_REQ_TYPE_IN &&
        (req->bmRequestType & USB_REQ_TYPE_TYPE) == USB_REQ_TYPE_STANDARD &&
        (req->bRequest == USB_REQ_GET_DESCRIPTOR) &&
        (req->wIndex == USB_INTERFACE_HID)) {

        // - High byte: Descriptor type is HID report (0x22)
        // - Low byte: Index 0
        switch (req->wValue) {
        case 0x2200:
            // Send the HID report descriptor.
            *buf = (uint8_t *)hid_report_descriptor;
            *len = sizeof(hid_report_descriptor);
            return USBD_REQ_HANDLED;
        case 0x2100:
            *buf = (uint8_t *)&hid_function;
            *len = sizeof(hid_function);
            return USBD_REQ_HANDLED;
        default:
            return USBD_REQ_NOTSUPP;
        }
    }

    return USBD_REQ_NEXT_CALLBACK;
}

//This callback that is executed when the endpoint "OUT" request arrives.
static void data_rx(usbd_device *dev, uint8_t ep)
{
    (void)ep;

    uint8_t buf[USB_HID_DATA_SIZE];
    int len = usbd_ep_read_packet(dev, ENDP_ADDRESS_OUT, buf, USB_HID_DATA_SIZE);
    
    usb_hid_rx_handler(buf, len);
}

//In this function, configure the endpoints and callbacks.
static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
    (void)wValue;

    usbd_ep_setup(dev, ENDP_ADDRESS_IN, USB_ENDPOINT_ATTR_INTERRUPT, USB_HID_DATA_SIZE, NULL);
    usbd_ep_setup(dev, ENDP_ADDRESS_OUT, USB_ENDPOINT_ATTR_INTERRUPT, USB_HID_DATA_SIZE, data_rx);

    aggregate_register_callback(
                dev,
                USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
                USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
                hid_control_request);
}

static usbd_device *hidd_dev;

void hid_setup(usbd_device *usbd_dev)
{
    hidd_dev = usbd_dev;
	aggregate_register_config_callback(usbd_dev, hid_set_config);
}


void usb_hid_send(const uint8_t * buf, uint16_t len)
{
    usbd_ep_write_packet(hidd_dev, ENDP_ADDRESS_IN, buf, len);    
}

#endif