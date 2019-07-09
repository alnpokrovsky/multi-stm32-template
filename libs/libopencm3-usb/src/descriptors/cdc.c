//  CDC code from https://github.com/Apress/Beg-STM32-Devel-FreeRTOS-libopencm3-GCC/blob/master/rtos/usbcdcdemo/usbcdc.c
#include "cdc.h"
#include <libopencm3/usb/cdc.h>
#include "core/aggregate.h"
#include "UsbConfig.h"
#include <stdlib.h>

#define DATA_OUT                0x03
#define DATA_IN                 0x84
#define COMM_IN                 0x85

#define CONTROL_CALLBACK_TYPE (USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)
#define CONTROL_CALLBACK_MASK (USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT)
#define USB_CDC_REQ_GET_LINE_CODING		0x21

//  Line config to be returned.
static const struct usb_cdc_line_coding line_coding = {
	.dwDTERate = 115200,
	.bCharFormat = USB_CDC_1_STOP_BITS,
	.bParityType = USB_CDC_NO_PARITY,
	.bDataBits = 0x08
};

/////////////////////////////////////////////////////////////////////
//  CDC Endpoints

/*
 * This notification endpoint isn't implemented. According to CDC spec it's
 * optional, but its absence causes a NULL pointer dereference in the
 * Linux cdc_acm driver. (Gareth McMullin <gareth@blacksphere.co.nz>)
 */
static const struct usb_endpoint_descriptor comm_endp[] = {
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = COMM_IN,
		.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
		.wMaxPacketSize = USB_CDC_PACKET_SIZE,  //  Smaller than others
		.bInterval = 255,
	}
};

static const struct usb_endpoint_descriptor data_endp[] = {
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = DATA_OUT,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = USB_MAX_PACKET_SIZE,
		.bInterval = 1,
	}, {
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = DATA_IN,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = USB_MAX_PACKET_SIZE,
		.bInterval = 1,
	}
};

static const struct {
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor call_mgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) cdcacm_functional_descriptors = {
	.header = {
		.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_HEADER,
		.bcdCDC = 0x0110,
	},
	.call_mgmt = {
		.bFunctionLength =
			sizeof(struct usb_cdc_call_management_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
		.bmCapabilities = 0,
		.bDataInterface = INTF_DATA,  //  DATA Interface
	},
	.acm = {
		.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_ACM,
		.bmCapabilities = 0,
	},
	.cdc_union = {
		.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_UNION,
		.bControlInterface = INTF_COMM,       //  COMM Interface
		.bSubordinateInterface0 = INTF_DATA,  //  DATA Interface
	 }
};

//  CDC Interfaces
const struct usb_iface_assoc_descriptor cdc_iface_assoc = {  //  Copied from microbit.  Mandatory for composite device.
	.bLength = USB_DT_INTERFACE_ASSOCIATION_SIZE,
	.bDescriptorType = USB_DT_INTERFACE_ASSOCIATION,
	.bFirstInterface = INTF_COMM,  //  First associated interface (INTF_COMM and INTF_DATA)
	.bInterfaceCount = 2,          //  Total number of associated interfaces (INTF_COMM and INTF_DATA), ID must be consecutive.
	.bFunctionClass = USB_CLASS_CDC,
	.bFunctionSubClass = USB_CDC_SUBCLASS_ACM,
	.bFunctionProtocol = USB_CDC_PROTOCOL_AT,
	.iFunction = USB_STRINGS_SERIAL_PORT  //  Name of Serial Port
};

const struct usb_interface_descriptor comm_iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = INTF_COMM,
    .bAlternateSetting = 0,
    .bNumEndpoints = 1,
    .bInterfaceClass = USB_CLASS_CDC,
    .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
    .bInterfaceProtocol = USB_CDC_PROTOCOL_AT,
    .iInterface = USB_STRINGS_COMM,  //  Name of COMM
    .endpoint = comm_endp,           //  COMM Endpoint
    .extra = &cdcacm_functional_descriptors,
    .extralen = sizeof(cdcacm_functional_descriptors)
};

const struct usb_interface_descriptor data_iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = INTF_DATA,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = USB_STRINGS_DATA,  //  Name of DATA
    .endpoint = data_endp,           //  DATA Endpoints
};


///////////////////////////////////////////////////////////////////////////////


static enum usbd_request_return_codes cdcacm_control_request(
  usbd_device *usbd_dev __attribute__((unused)),
  struct usb_setup_data *req,
  uint8_t **buf __attribute__((unused)),
  uint16_t *len,
  void (**complete)(
    usbd_device *usbd_dev,
    struct usb_setup_data *req
  ) __attribute__((unused))
) {
	//  Handle USB Control Requests
	switch (req->bRequest) {
		case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
			/* From https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f3/stm32f3-discovery/usb_cdcacm/cdcacm.c
			* This Linux cdc_acm driver requires this to be implemented
			* even though it's optional in the CDC spec, and we don't
			* advertise it in the ACM functional descriptor. */
			return USBD_REQ_HANDLED;
		}
		case USB_CDC_REQ_GET_LINE_CODING: {
			//  Windows requires this request, not Mac or Linux.
			//  From https://github.com/PX4/Bootloader/blob/master/stm32/cdcacm.c
			if ( *len < sizeof(struct usb_cdc_line_coding) ) {
				// debug_print("*** cdcacm_control notsupp line_coding "); debug_print_unsigned(sizeof(struct usb_cdc_line_coding)); 
				// debug_print(", len "); debug_print_unsigned(*len);
				// debug_println(""); debug_flush(); ////
				return USBD_REQ_NOTSUPP;
			}
			*buf = (uint8_t *) &line_coding;
			*len = sizeof(struct usb_cdc_line_coding);
			return USBD_REQ_HANDLED;
		}
		case USB_CDC_REQ_SET_LINE_CODING: {
			if ( *len < sizeof(struct usb_cdc_line_coding) ) {
				// debug_print("*** cdcacm_control notsupp line_coding "); debug_print_unsigned(sizeof(struct usb_cdc_line_coding)); 
				// debug_print(", len "); debug_print_unsigned(*len);
				// debug_println(""); debug_flush(); ////
				return USBD_REQ_NOTSUPP;
			}
			return USBD_REQ_HANDLED;
		}
	}
	return USBD_REQ_NEXT_CALLBACK;  //  Previously USBD_REQ_NOTSUPP
}

//  TODO: TX Up to USB_MAX_PACKET_SIZE
//  usbd_ep_write_packet(usbd_dev, DATA_IN, txbuf, txlen)

static char cdcbuf[USB_MAX_PACKET_SIZE + 1];   // rx buffer

/*
 * USB Receive Callback:
 */
static void
cdcacm_data_rx_cb(
  usbd_device *usbd_dev,
  uint8_t ep __attribute__((unused))
) {
	uint16_t len = usbd_ep_read_packet(usbd_dev, DATA_OUT, cdcbuf, USB_MAX_PACKET_SIZE);
    if (len == 0) { return; }
    uint16_t pos = (len < USB_MAX_PACKET_SIZE) ? len : USB_MAX_PACKET_SIZE;
    cdcbuf[pos] = 0;

	usbd_ep_write_packet(usbd_dev, DATA_IN, cdcbuf, pos); ////  Echo the packet.
}

static void
cdcacm_comm_cb(
  usbd_device *usbd_dev,
  uint8_t ep
) {
	(void)usbd_dev;
	(void)ep;
}

/*
 * USB Configuration:
 */
static void
cdcacm_set_config(
  usbd_device *usbd_dev,
  uint16_t wValue __attribute__((unused))
) {
	//  From https://github.com/libopencm3/libopencm3-examples/blob/master/examples/stm32/f3/stm32f3-discovery/usb_cdcacm/cdcacm.c
    //  debug_println("*** cdcacm_set_config"); ////
	usbd_ep_setup(usbd_dev, DATA_OUT, USB_ENDPOINT_ATTR_BULK, USB_MAX_PACKET_SIZE, cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, DATA_IN, USB_ENDPOINT_ATTR_BULK, USB_MAX_PACKET_SIZE, NULL);
	usbd_ep_setup(usbd_dev, COMM_IN, USB_ENDPOINT_ATTR_INTERRUPT, USB_CDC_PACKET_SIZE, cdcacm_comm_cb);
	int status = aggregate_register_callback(
		usbd_dev,
		CONTROL_CALLBACK_TYPE,
		CONTROL_CALLBACK_MASK,
		cdcacm_control_request);
	if (status < 0) {
		// debug_println("*** cdcacm_set_config failed");
		// debug_flush();
	}
}


void cdc_setup(usbd_device* usbd_dev) {
	int status = aggregate_register_config_callback(usbd_dev, cdcacm_set_config);
	if (status < 0) {
		// debug_println("*** cdc_setup failed");
		// debug_flush();
	}
}
