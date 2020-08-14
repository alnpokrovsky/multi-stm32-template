#include "usbd_core.h"
#include "usbd_desc.h"
#include "word.h"
#include "UsbConfig.h"
#include <string.h>


#define  USB_SERIAL_STRING_SIZE       26
uint8_t USBD_StringSerial[USB_SERIAL_STRING_SIZE] = {
  USB_SERIAL_STRING_SIZE,
  USB_DESC_TYPE_STRING,
  /* place for unique id
      ... */
};
static const char *usb_strings[] = USB_STRINGS;


/** USB standard device descriptor. */
/* USB Standard Device Descriptor - Table 9-8 */
struct usb_device_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} __attribute__((packed));
#define USB_DT_DEVICE_SIZE sizeof(struct usb_device_descriptor)

static const struct usb_device_descriptor USBD_FS_DeviceDesc = {
  .bLength = USB_LEN_DEV_DESC,
  .bDescriptorType = USB_DESC_TYPE_DEVICE,
#ifdef USB21_INTERFACE
  .bcdUSB = 0x0210,  //  USB Version 2.1.  Need to handle special requests e.g. BOS.
#elif (USBD_LPM_ENABLED == 1)
  .bcdUSB = 0x0201, // USB Version 2.0.1 in order to support LPM L1 suspend
#else 
  .bcdUSB = 0x0200, //  USB Version 2.0.  No need to handle special requests e.g. BOS.
#endif
  .bDeviceClass     = 2,  //  Copied from microbit. For composite device, let host probe the interfaces.
  .bDeviceSubClass  = 2,  //  Common Class
  .bDeviceProtocol  = 1,  //  Interface Association Descriptor
  .bMaxPacketSize0  = USB_MAX_EP0_SIZE,
  .idVendor       = USB_VID,
  .idProduct      = USB_PID,
  .bcdDevice      = 0x0220,  //  Device Release number 2.2
  .iManufacturer  = USB_STRINGS_IDX_MANUFACTURER,
  .iProduct       = USB_STRINGS_IDX_PRODUCT,
  .iSerialNumber  = USB_STRINGS_IDX_SERIAL_NUMBER,
  .bNumConfigurations = USBD_MAX_NUM_CONFIGURATION,
};


/**
  * @brief  Return the device descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
static uint8_t * USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  (void)(speed);
  *length = USB_DT_DEVICE_SIZE;
  return (uint8_t*) &USBD_FS_DeviceDesc;
}

/**
  * @brief  Return the LangID string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
static const uint8_t * USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  (void)(speed);
  /** USB lang indentifier descriptor. */
  #define USBD_LANGID_STRING      1033
  static const uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] =
  {
      USB_LEN_LANGID_STR_DESC,
      USB_DESC_TYPE_STRING,
      LOBYTE(USBD_LANGID_STRING),
      HIBYTE(USBD_LANGID_STRING)
  };
  *length = sizeof(USBD_LangIDDesc);
  return USBD_LangIDDesc;
}

static const char * USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint8_t idx, uint16_t *length) 
{
  (void) speed;
  *length = strlen(usb_strings[idx-1]);
  return usb_strings[idx-1];
}

#if (USBD_LPM_ENABLED == 1)
/**
  * @brief  Return the BOS descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
static uint8_t * USBD_FS_USR_BOSDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  (void)(speed);
  static const uint8_t USBD_FS_BOSDesc[] =
  {
    0x5,
    USB_DESC_TYPE_BOS,
    0xC,
    0x0,
    0x1,  /* 1 device capability*/
          /* device capability*/
    0x7,
    USB_DEVICE_CAPABITY_TYPE,
    0x2,
    0x2,  /* LPM capability bit set*/
    0x0,
    0x0,
    0x0
  };
  *length = sizeof(USBD_FS_BOSDesc);
  return (uint8_t*)USBD_FS_BOSDesc;
}
#endif /* (USBD_LPM_ENABLED == 1) */



USBD_DescriptorsTypeDef FS_Desc =
{
  USBD_FS_DeviceDescriptor,
  USBD_FS_LangIDStrDescriptor,
  USBD_FS_InterfaceStrDescriptor,
#if (USBD_LPM_ENABLED == 1)
  USBD_FS_USR_BOSDescriptor,
#endif /* (USBD_LPM_ENABLED == 1) */
};




/**
  * @brief  Create the serial number string descriptor 
  * @param  None 
  * @retval None
  */
#include "str.h"
static void __attribute__ ((constructor)) premain_InitSerialIdString(void)
{
  uint32_t deviceserial0, deviceserial1, deviceserial2;

  deviceserial0 = 1;
  deviceserial1 = 2;
  deviceserial2 = 3;

  // deviceserial0 = *(uint32_t *) DEVICE_ID1;
  // deviceserial1 = *(uint32_t *) DEVICE_ID2;
  // deviceserial2 = *(uint32_t *) DEVICE_ID3;

  deviceserial0 += deviceserial2;

  if (deviceserial0 != 0)
  {
    strfromint(deviceserial0, &USBD_StringSerial[2], 8);
    strfromint(deviceserial1, &USBD_StringSerial[18], 4);
  }
}


