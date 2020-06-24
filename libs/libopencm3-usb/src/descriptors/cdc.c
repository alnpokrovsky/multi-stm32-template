#include "cdc.h"
#include "UsbConfig.h"

#if defined(USB_INTERFACE_CDC_COMM) && defined(USB_INTERFACE_CDC_DATA)

#include <libopencm3/usb/cdc.h>
#include <stdint.h>
#include <string.h>
#include "basic/aggregate.h"
#include "usb_cdc.h"
#include "minmax.h"

#define ENDP_DATA_OUT                USB_ENDPOINT_ADDR_OUT(USB_INTERFACE_CDC_DATA)
#define ENDP_DATA_IN                 USB_ENDPOINT_ADDR_IN (USB_INTERFACE_CDC_DATA)
#define ENDP_COMM_IN                 USB_ENDPOINT_ADDR_IN (USB_INTERFACE_CDC_COMM)

#define CONTROL_CALLBACK_TYPE (USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)
#define CONTROL_CALLBACK_MASK (USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT)
#define USB_CDC_REQ_GET_LINE_CODING		0x21


static void null_handler(char * buf, uint16_t len)
{
	/* echo by default */
	usb_cdc_tx(buf, len);
}

#pragma weak usb_cdc_rx_handler = null_handler


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
		.bEndpointAddress = ENDP_COMM_IN,
		.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
		.wMaxPacketSize = 16,  //  Smaller than others
		.bInterval = 255,
	}
};

static const struct usb_endpoint_descriptor data_endp[] = {
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = ENDP_DATA_OUT,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = USB_MAX_PACKET_SIZE,
		.bInterval = 100,
	}, {
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = ENDP_DATA_IN,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = USB_MAX_PACKET_SIZE,
		.bInterval = 100,
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
		.bDataInterface = USB_INTERFACE_CDC_DATA,  //  DATA Interface
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
		.bControlInterface = USB_INTERFACE_CDC_COMM,       //  COMM Interface
		.bSubordinateInterface0 = USB_INTERFACE_CDC_DATA,  //  DATA Interface
	 }
};

//  CDC Interfaces
const struct usb_iface_assoc_descriptor cdc_iface_assoc = {  //  Copied from microbit.  Mandatory for composite device.
	.bLength = USB_DT_INTERFACE_ASSOCIATION_SIZE,
	.bDescriptorType = USB_DT_INTERFACE_ASSOCIATION,
	.bFirstInterface = USB_INTERFACE_CDC_COMM,  //  First associated interface (USB_INTERFACE_CDC_COMM and USB_INTERFACE_CDC_DATA)
	.bInterfaceCount = 2,          //  Total number of associated interfaces (USB_INTERFACE_CDC_COMM and USB_INTERFACE_CDC_DATA), ID must be consecutive.
	.bFunctionClass = USB_CLASS_CDC,
	.bFunctionSubClass = USB_CDC_SUBCLASS_ACM,
	.bFunctionProtocol = USB_CDC_PROTOCOL_AT,
	.iFunction = USB_STRINGS_SERIAL_PORT  //  Name of Serial Port
};

const struct usb_interface_descriptor comm_iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = USB_INTERFACE_CDC_COMM,
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
    .bInterfaceNumber = USB_INTERFACE_CDC_DATA,
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
	if (req->wIndex == USB_INTERFACE_CDC_COMM) {
		switch (req->bRequest) {
			case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
				/*This Linux cdc_acm driver requires this to be implemented
				* even though it's optional in the CDC spec, and we don't
				* advertise it in the ACM functional descriptor. */
				char local_buf[10];
				struct usb_cdc_notification *notif = (void *)local_buf;
				/* We echo signals back to host as notification. */
				notif->bmRequestType = 0xA1;
				notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
				notif->wValue = 0;
				notif->wIndex = 0;
				notif->wLength = 2;
				local_buf[8] = req->wValue & 3;
				local_buf[9] = 0;
				usbd_ep_write_packet(usbd_dev, ENDP_COMM_IN, local_buf, 10);
				return USBD_REQ_HANDLED;
			}
			case USB_CDC_REQ_GET_LINE_CODING: {
				if ( *len < sizeof(struct usb_cdc_line_coding) ) return USBD_REQ_NOTSUPP;
				*buf = (uint8_t *) &line_coding;
				*len = sizeof(struct usb_cdc_line_coding);
				return USBD_REQ_HANDLED;
			}
			case USB_CDC_REQ_SET_LINE_CODING: {
				if ( *len < sizeof(struct usb_cdc_line_coding) ) return USBD_REQ_NOTSUPP;
				return USBD_REQ_HANDLED;
			}
		}
	}
	return USBD_REQ_NEXT_CALLBACK;
}

/**
 * rx buffer
 */
static char cdcbuf[USB_MAX_PACKET_SIZE + 1];


/*
 * USB Receive Callback:
 */
static void
cdcacm_data_rx_cb(
  usbd_device *usbd_dev,
  uint8_t ep
) {
	(void) ep;

	uint16_t len = usbd_ep_read_packet(usbd_dev, ENDP_DATA_OUT, cdcbuf, USB_MAX_PACKET_SIZE);
    if (len == 0) { return; }
    len = (len < USB_MAX_PACKET_SIZE) ? len : USB_MAX_PACKET_SIZE;

	usb_cdc_rx_handler(cdcbuf, len);
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
	usbd_ep_setup(usbd_dev, ENDP_DATA_OUT, USB_ENDPOINT_ATTR_BULK, USB_MAX_PACKET_SIZE, cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, ENDP_DATA_IN, USB_ENDPOINT_ATTR_BULK, USB_MAX_PACKET_SIZE, NULL);
	usbd_ep_setup(usbd_dev, ENDP_COMM_IN, USB_ENDPOINT_ATTR_INTERRUPT, 16, cdcacm_comm_cb);
	aggregate_register_callback(
				usbd_dev,
				CONTROL_CALLBACK_TYPE,
				CONTROL_CALLBACK_MASK,
				cdcacm_control_request);
}

static usbd_device* cdcd_dev;

uint16_t usb_cdc_tx(const char* data, uint16_t len) {
		len = MIN(USB_MAX_PACKET_SIZE, len);
		usbd_ep_write_packet(cdcd_dev, ENDP_DATA_IN, data, len);
		return len;
}

void cdc_setup(usbd_device* usbd_dev) {
	cdcd_dev = usbd_dev;
	aggregate_register_config_callback(usbd_dev, cdcacm_set_config);
}


#endif