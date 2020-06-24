#include "keyboard.h"
#include "usb_keyboard.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <libopencm3/usb/hid.h>
#include "UsbConfig.h"
#include "basic/aggregate.h"
#include "../private/setup.h"


#ifdef USB_INTERFACE_KEYBOARD

#define ENDP_KEYBOARD  USB_ENDPOINT_ADDR_IN(USB_INTERFACE_KEYBOARD)

static const uint8_t hid_report_descriptor[] =
{
    0x05, 0x01, /* Usage Page (Generic Desktop)             */
    0x09, 0x02, /* Usage (Mouse)                            */
    0xA1, 0x01, /* Collection (Application)                 */
    0x09, 0x01, /*  Usage (Pointer)                         */
    0xA1, 0x00, /*  Collection (Physical)                   */
    0x85, 0x01,  /*   Report ID  */
    0x05, 0x09, /*      Usage Page (Buttons)                */
    0x19, 0x01, /*      Usage Minimum (01)                  */
    0x29, 0x03, /*      Usage Maximum (03)                  */
    0x15, 0x00, /*      Logical Minimum (0)                 */
    0x25, 0x01, /*      Logical Maximum (0)                 */
    0x95, 0x03, /*      Report Count (3)                    */
    0x75, 0x01, /*      Report Size (1)                     */
    0x81, 0x02, /*      Input (Data, Variable, Absolute)    */
    0x95, 0x01, /*      Report Count (1)                    */
    0x75, 0x05, /*      Report Size (5)                     */
    0x81, 0x01, /*      Input (Constant)    ;5 bit padding  */
    0x05, 0x01, /*      Usage Page (Generic Desktop)        */
    0x09, 0x30, /*      Usage (X)                           */
    0x09, 0x31, /*      Usage (Y)                           */
    0x15, 0x81, /*      Logical Minimum (-127)              */
    0x25, 0x7F, /*      Logical Maximum (127)               */
    0x75, 0x08, /*      Report Size (8)                     */
    0x95, 0x02, /*      Report Count (2)                    */
    0x81, 0x06, /*      Input (Data, Variable, Relative)    */
    0xC0, 0xC0,/* End Collection,End Collection            */
//
    0x09, 0x06, /*		Usage (Keyboard)                    */
    0xA1, 0x01, /*		Collection (Application)            */
    0x85, 0x02,  /*   Report ID  */
    0x05, 0x07, /*  	Usage (Key codes)                   */
    0x19, 0xE0, /*      Usage Minimum (224)                 */
    0x29, 0xE7, /*      Usage Maximum (231)                 */
    0x15, 0x00, /*      Logical Minimum (0)                 */
    0x25, 0x01, /*      Logical Maximum (1)                 */
    0x75, 0x01, /*      Report Size (1)                     */
    0x95, 0x08, /*      Report Count (8)                    */
    0x81, 0x02, /*      Input (Data, Variable, Absolute)    */
    0x95, 0x01, /*      Report Count (1)                    */
    0x75, 0x08, /*      Report Size (8)                     */
    0x81, 0x01, /*      Input (Constant)    ;5 bit padding  */
    0x95, 0x05, /*      Report Count (5)                    */
    0x75, 0x01, /*      Report Size (1)                     */
    0x05, 0x08, /*      Usage Page (Page# for LEDs)         */
    0x19, 0x01, /*      Usage Minimum (01)                  */
    0x29, 0x05, /*      Usage Maximum (05)                  */
    0x91, 0x02, /*      Output (Data, Variable, Absolute)   */
    0x95, 0x01, /*      Report Count (1)                    */
    0x75, 0x03, /*      Report Size (3)                     */
    0x91, 0x01, /*      Output (Constant)                   */
    0x95, 0x06, /*      Report Count (1)                    */
    0x75, 0x08, /*      Report Size (3)                     */
    0x15, 0x00, /*      Logical Minimum (0)                 */
    0x25, 0x65, /*      Logical Maximum (101)               */
    0x05, 0x07, /*  	Usage (Key codes)                   */
    0x19, 0x00, /*      Usage Minimum (00)                  */
    0x29, 0x65, /*      Usage Maximum (101)                 */
    0x81, 0x00, /*      Input (Data, Array)                 */
    0xC0        /* 		End Collection,End Collection       */
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
	},
};

static const struct usb_endpoint_descriptor keyboard_endpoint = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = ENDP_KEYBOARD,
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize = 9,
	.bInterval = 100,
};

const struct usb_interface_descriptor keyboard_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = USB_INTERFACE_KEYBOARD,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_HID,
	.bInterfaceSubClass = 1, // boot
	.bInterfaceProtocol = 1, // keyboard
	.iInterface = USB_STRINGS_KEYBOARD,

	.endpoint = &keyboard_endpoint,

	.extra = &hid_function,
	.extralen = sizeof(hid_function),
};

static enum usbd_request_return_codes keyboard_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete)
{
	(void)complete;
	(void)usbd_dev;

	// This request is asking for information sent to the host using request
    // GET_DESCRIPTOR.
    if ((req->bmRequestType & USB_REQ_TYPE_DIRECTION) == USB_REQ_TYPE_IN &&
        (req->bmRequestType & USB_REQ_TYPE_TYPE) == USB_REQ_TYPE_STANDARD &&
        (req->bRequest == USB_REQ_GET_DESCRIPTOR) &&
        (req->wIndex == USB_INTERFACE_KEYBOARD)) {

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

/*********************************************************************/

#define REPORT_ID_MOUSE 1
#define REPORT_ID_KEYBOARD 2

typedef struct {
    unsigned char keycode;
    unsigned char modifier;
} KEYMAP;


static const KEYMAP keymap[] = {
    {0, 0},             /* NUL */
    {0, 0},             /* SOH */
    {0x4f, KEY_SHIFT},  /* RIGHT_ARROW */ /* Combo selection arrows */
    {0x50, KEY_SHIFT},  /* LEFT_ARROW */ /* Combo selection arrows */
    {0x51, KEY_SHIFT},  /* DOWN_ARROW */ /* Combo selection arrows */
    {0x52, KEY_SHIFT},  /* UP_ARROW */  /* Combo selection arrows */
    {0x06, KEY_CTRL},   /* C   */  /* Combo ctrl+c */
    {0x19, KEY_CTRL},   /* V   */  /* Combo ctrl+v */
    {0x4c, 0},          /* DEL */  /* Keyboard Del */
    {0x2a, 0},          /* BS  */  /* Keyboard Backspace */
    {0x2b, 0},          /* TAB */  /* Keyboard Tab */
    {0x28, 0},          /* LF  */  /* Keyboard Return (Enter) */
    {0x29, 0},          /* ESC */  /* Keyboard ESC */
    {0, 0},             /* VT  */
    {0, 0},             /* FF  */
    {0, 0},             /* CR  */
    {0, 0},             /* SO  */
    {0, 0},             /* SI  */
    {0, 0},             /* DC1 */
    {0, 0},             /* DC2 */
    {0, 0},             /* DC3 */
    {0, 0},             /* DC4 */
    {0, 0},             /* NAK */
    {0, 0},             /* SYN */
    {0, 0},             /* ETB */
    {0, 0},             /* CAN */
    {0, 0},             /* EM  */
    {0, 0},             /* SUB */
    {0, 0},             /* FS  */
    {0, 0},             /* GS  */
    {0, 0},             /* RS  */
    {0, 0},             /* US  */
    {0x2c, 0},          /*   */
    {0x1e, KEY_SHIFT},      /* ! */
    {0x1f, KEY_SHIFT},      /* " */
    {0x32, 0},          /* # */
    {0x21, KEY_SHIFT},      /* $ */
    {0x22, KEY_SHIFT},      /* % */
    {0x24, KEY_SHIFT},      /* & */
    {0x34, 0},          /* ' */
    {0x26, KEY_SHIFT},      /* ( */
    {0x27, KEY_SHIFT},      /* ) */
    {0x25, KEY_SHIFT},      /* * */
    {0x2e, KEY_SHIFT},      /* + */
    {0x36, 0},          /* , */
    {0x2d, 0},          /* - */
    {0x37, 0},          /* . */
    {0x38, 0},          /* / */
    {0x27, 0},          /* 0 */
    {0x1e, 0},          /* 1 */
    {0x1f, 0},          /* 2 */
    {0x20, 0},          /* 3 */
    {0x21, 0},          /* 4 */
    {0x22, 0},          /* 5 */
    {0x23, 0},          /* 6 */
    {0x24, 0},          /* 7 */
    {0x25, 0},          /* 8 */
    {0x26, 0},          /* 9 */
    {0x33, KEY_SHIFT},      /* : */
    {0x33, 0},          /* ; */
    {0x36, KEY_SHIFT},      /* < */
    {0x2e, 0},          /* = */
    {0x37, KEY_SHIFT},      /* > */
    {0x38, KEY_SHIFT},      /* ? */
    {0x34, KEY_SHIFT},      /* @ */
    {0x04, KEY_SHIFT},      /* A */
    {0x05, KEY_SHIFT},      /* B */
    {0x06, KEY_SHIFT},      /* C */
    {0x07, KEY_SHIFT},      /* D */
    {0x08, KEY_SHIFT},      /* E */
    {0x09, KEY_SHIFT},      /* F */
    {0x0a, KEY_SHIFT},      /* G */
    {0x0b, KEY_SHIFT},      /* H */
    {0x0c, KEY_SHIFT},      /* I */
    {0x0d, KEY_SHIFT},      /* J */
    {0x0e, KEY_SHIFT},      /* K */
    {0x0f, KEY_SHIFT},      /* L */
    {0x10, KEY_SHIFT},      /* M */
    {0x11, KEY_SHIFT},      /* N */
    {0x12, KEY_SHIFT},      /* O */
    {0x13, KEY_SHIFT},      /* P */
    {0x14, KEY_SHIFT},      /* Q */
    {0x15, KEY_SHIFT},      /* R */
    {0x16, KEY_SHIFT},      /* S */
    {0x17, KEY_SHIFT},      /* T */
    {0x18, KEY_SHIFT},      /* U */
    {0x19, KEY_SHIFT},      /* V */
    {0x1a, KEY_SHIFT},      /* W */
    {0x1b, KEY_SHIFT},      /* X */
    {0x1c, KEY_SHIFT},      /* Y */
    {0x1d, KEY_SHIFT},      /* Z */
    {0x2f, 0},          /* [ */
    {0x64, 0},          /* \ */
    {0x30, 0},          /* ] */
    {0x23, KEY_SHIFT},      /* ^ */
    {0x2d, KEY_SHIFT},      /* _ */
    {0x35, 0},          /* ` */
    {0x04, 0},          /* a */
    {0x05, 0},          /* b */
    {0x06, 0},          /* c */
    {0x07, 0},          /* d */
    {0x08, 0},          /* e */
    {0x09, 0},          /* f */
    {0x0a, 0},          /* g */
    {0x0b, 0},          /* h */
    {0x0c, 0},          /* i */
    {0x0d, 0},          /* j */
    {0x0e, 0},          /* k */
    {0x0f, 0},          /* l */
    {0x10, 0},          /* m */
    {0x11, 0},          /* n */
    {0x12, 0},          /* o */
    {0x13, 0},          /* p */
    {0x14, 0},          /* q */
    {0x15, 0},          /* r */
    {0x16, 0},          /* s */
    {0x17, 0},          /* t */
    {0x18, 0},          /* u */
    {0x19, 0},          /* v */
    {0x1a, 0},          /* w */
    {0x1b, 0},          /* x */
    {0x1c, 0},          /* y */
    {0x1d, 0},          /* z */
    {0x2f, KEY_SHIFT},      /* { */
    {0x64, KEY_SHIFT},      /* | */
    {0x30, KEY_SHIFT},      /* } */
    {0x32, KEY_SHIFT},      /* ~ */
    {0,0},             /* DEL */

    {0x3a, 0},          /* F1 */
    {0x3b, 0},          /* F2 */
    {0x3c, 0},          /* F3 */
    {0x3d, 0},          /* F4 */
    {0x3e, 0},          /* F5 */
    {0x3f, 0},          /* F6 */
    {0x40, 0},          /* F7 */
    {0x41, 0},          /* F8 */
    {0x42, 0},          /* F9 */
    {0x43, 0},          /* F10 */
    {0x44, 0},          /* F11 */
    {0x45, 0},          /* F12 */

    {0x46, 0},          /* PRINT_SCREEN */
    {0x47, 0},          /* SCROLL_LOCK */
    {0x48, 0},          /* PAUSE */
    {0x39, 0},          /* CAPS_LOCK */
    {0x53, 0},          /* NUM_LOCK */
    {0x49, 0},          /* INSERT */
    {0x4a, 0},          /* HOME */
    {0x4b, 0},          /* PAGE_UP */
    {0x4d, 0},          /* END */
    {0x4e, 0},          /* PAGE_DOWN */

    {0x4f, 0},          /* RIGHT_ARROW */
    {0x50, 0},          /* LEFT_ARROW */
    {0x51, 0},          /* DOWN_ARROW */
    {0x52, 0},          /* UP_ARROW */
};


/**********************************************************************/

static usbd_device *keyboardd_dev;
static bool usbd_configured = false;


static void keyboard_set_config(usbd_device *usbd_p, uint16_t wValue)
{
	(void)wValue; // shut up compiller's 'unused parameter'

	usbd_ep_setup(usbd_p, ENDP_KEYBOARD, USB_ENDPOINT_ATTR_INTERRUPT, 4, NULL);
	aggregate_register_callback(
				usbd_p,
				USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				keyboard_control_request);
    
    usbd_configured = true;
}

static void usb_keyboard_send_report(uint8_t *buf, uint32_t len) {
    if (!usbd_configured) return; // don't allow to send anything before configured
	while(usbd_ep_write_packet(keyboardd_dev,
        ENDP_KEYBOARD,
        buf, len) == 0);
}


void keyboard_setup(usbd_device* usbd_dev) {
    keyboardd_dev = usbd_dev;
	aggregate_register_config_callback(usbd_dev, keyboard_set_config);
}

void usb_keyboard_move_mouse(int8_t x, int8_t y){
    /*
     * buf[0]: 1 - report ID
     * buf[1]: bit2 - middle button, bit1 - right, bit0 - left
     * buf[2]: move X
     * buf[3]: move Y
     * buf[4]: wheel
     */
	uint8_t buf[5] = {REPORT_ID_MOUSE,0,x,y,0};
	usb_keyboard_send_report(buf, 5);
}


void usb_keyboard_press(FUNCTION_KEY key, MODIFIER_KEY modifier)
{
    /*
     * Keyboard buffer:
     * buf[1]: MOD
     * buf[2]: reserved
     * buf[3]..buf[8] - keycodes 1..6
     */
    static uint8_t report_buf[9] = {
        REPORT_ID_KEYBOARD,0,0,0,0,0,0,0,0
    };
    report_buf[1] = keymap[key].modifier != 0 ? keymap[key].modifier : modifier;
    report_buf[3] = keymap[key].keycode;
	usb_keyboard_send_report(report_buf, 9);
}

void usb_keyboard_release() {
    static uint8_t report_buf[9] = {
        REPORT_ID_KEYBOARD,0,0,0,0,0,0,0,0
    };
	usb_keyboard_send_report(report_buf, 9);
}

#endif