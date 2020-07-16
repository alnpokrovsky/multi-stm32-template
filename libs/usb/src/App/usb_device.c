#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_custom_hid_if.h"

USBD_HandleTypeDef hUsbDeviceFS;

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  
  /* Init Device Library, add supported class and start the library. */
  USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
  USBD_RegisterClass(&hUsbDeviceFS, &USBD_CUSTOM_HID);
  USBD_CUSTOM_HID_RegisterInterface(&hUsbDeviceFS, &USBD_CustomHID_fops_FS);
  USBD_Start(&hUsbDeviceFS);
}
