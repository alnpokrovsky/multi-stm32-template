#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usbd_def.h"



/* USBD Low Level Driver */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev);

USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                  uint8_t ep_type, uint16_t ep_mps);

USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr);

USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                    const uint8_t *pbuf, uint32_t size);

USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr,
                                          uint8_t *pbuf, uint32_t size);

uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t  ep_addr);

void  USBD_LL_Delay(uint32_t Delay);


#endif /* __USBD_CONF__H__ */
