#include "usbd_cdc_if.h"


/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
static uint8_t CDC_Transmit_FS(USBD_HandleTypeDef * hUsbDeviceFS, uint8_t* Buf, uint16_t Len)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS->pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(hUsbDeviceFS, Buf, Len);
  return USBD_CDC_TransmitPacket(hUsbDeviceFS);
}


/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(USBD_HandleTypeDef * hUsbDeviceFS, uint8_t* Buf, uint32_t *Len)
{
  CDC_Transmit_FS(hUsbDeviceFS, Buf, *Len);

  USBD_CDC_SetRxBuffer(hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(hUsbDeviceFS);
  return (USBD_OK);
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmited callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  (void)(Buf);
  (void)(Len);
  (void)(epnum);
  return USBD_OK;
}


USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};
