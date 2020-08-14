#include "usbd_custom_hid_if.h"
#include "alignment.h"

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  0x00,
  0xC0    /*     END_COLLECTION	             */
};


extern USBD_HandleTypeDef hUsbDeviceFS;



static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);


USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

static int8_t CUSTOM_HID_Init_FS(void)
{
  return (USBD_OK);
}

static int8_t CUSTOM_HID_DeInit_FS(void)
{
  return (USBD_OK);
}

static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  (void)(event_idx);
  (void)(state);

  /* Start next USB packet transfer once data processing is completed */
  USBD_CUSTOM_HID_ReceivePacket(&hUsbDeviceFS);

  return (USBD_OK);
}

