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

#define ENDP_ADDRESS_IN     ( 0x81 + USB_INTERFACE_HID )
#define ENDP_ADDRESS_OUT    ( USB_INTERFACE_HID )
#define MAXPACKETSIZEIN     16
#define MAXPACKETSIZEOUT    8

static void null_handler(uint8_t * buf, uint16_t len)
{
	/* echo by default */
	usb_hid_send(buf, len);
}

#pragma weak usb_hid_recv_handler = null_handler


static const uint8_t hid_report_descriptor[] =
{
      0x06, 0xFF, 0xFF,         // 04|2   , Usage Page (vendordefined?)
      0x09, 0x01,               // 08|1   , Usage      (vendordefined
      0xA1, 0x01,               // A0|1   , Collection (Application)
      // Feature report
      0x09, 0x06,               // 08|1   , Usage      (vendordefined)
      0x09, 0x07,               // 08|1   , Usage      (vendordefined)
      0x15, 0x00,               // 14|1   , LogicalMinimum(0 for signed byte)
      0x75, 0x0F,               // 74|1   , Report Size(16) =field size in bits = 1 byte
      0x95, 0x08,               //_0x04,               // 94|1:ReportCount
      0xB1, 0x02,               // B0|1:   Feature report
      0xC0                      // C0|0    , End Collection
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
    .wMaxPacketSize = MAXPACKETSIZEIN,
    .bInterval = 1,
},
{
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = ENDP_ADDRESS_OUT,
    .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize = MAXPACKETSIZEOUT,
    .bInterval = 1,
}};

const struct usb_interface_descriptor hid_iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = 0,
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


//This function looks identical in all examples.
//And it is as I understand monitors all inbound hid-requests.
static enum usbd_request_return_codes hid_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
		uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
    (void)complete;
    (void)usbd_dev;

    if((req->bmRequestType != ENDP_ADDRESS_IN) ||
       (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
       (req->wValue != 0x2200))
        return USBD_REQ_NOTSUPP;

    /* Handle the HID report descriptor. */
    *buf = (uint8_t *)hid_report_descriptor;
    *len = sizeof(hid_report_descriptor);

    return USBD_REQ_HANDLED;
}

//This callback that is executed when the endpoint "OUT" request arrives.
static void data_rx(usbd_device *dev, uint8_t ep)
{
    (void)ep;

    //gpio_toggle(LED_PORT, LED_PIN);

    uint8_t buf[MAXPACKETSIZEIN];
    int len = usbd_ep_read_packet(dev, ENDP_ADDRESS_OUT, buf, MAXPACKETSIZEOUT);
    
    usb_hid_recv_handler(buf, len);
}

//In this function, configure the endpoints and callbacks.
static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
    (void)wValue;

    usbd_ep_setup(dev, ENDP_ADDRESS_IN, USB_ENDPOINT_ATTR_INTERRUPT, MAXPACKETSIZEIN, NULL);
    usbd_ep_setup(dev, ENDP_ADDRESS_OUT, USB_ENDPOINT_ATTR_INTERRUPT, MAXPACKETSIZEOUT, data_rx);

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