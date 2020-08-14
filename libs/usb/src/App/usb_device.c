#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_custom_hid_if.h"
#include "usbd_cdc_if.h"

#include "UsbConfig.h"

static const USBD_ClassTypeDef USBD_CLASS =
{
  .Init = USBD_CLASS_Init,
  .DeInit = USBD_CLASS_DeInit,
  .Setup = USBD_CLASS_Setup,
  .EP0_TxSent = NULL,                 /* EP0_TxSent, */
  .EP0_RxReady = USBD_CLASS_EP0_RxReady,
  .DataIn = USBD_CLASS_DataIn,
  .DataOut = USBD_CLASS_DataOut,
  .SOF = NULL,
  .IsoINIncomplete = NULL,
  .IsoOUTIncomplete = NULL,
  .GetHSConfigDescriptor = USBD_CLASS_GetHSCfgDesc,
  .GetFSConfigDescriptor = USBD_CLASS_GetFSCfgDesc,
  .GetOtherSpeedConfigDescriptor = USBD_CLASS_GetOtherSpeedCfgDesc,
  .GetDeviceQualifierDescriptor = USBD_CLASS_GetDeviceQualifierDescriptor,
};

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{  
  static USBD_HandleTypeDef hUsbDeviceFS;
  /* Init Device Library, add supported class and start the library. */
  USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);

#if defined(NO)
  USBD_RegisterClass(&hUsbDeviceFS, &USBD_CUSTOM_HID);
  RegisterInterface(&hUsbDeviceFS, &USBD_CustomHID_fops_FS);
#elif defined(USB_INTERFACE_CDC_COMM)
  USBD_RegisterClass(&hUsbDeviceFS, &USBD_CLASS);
#endif

  USBD_Start(&hUsbDeviceFS);
}
