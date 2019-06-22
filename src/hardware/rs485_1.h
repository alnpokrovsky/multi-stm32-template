#ifndef __HARDWARE_RS485_1_H__
#define __HARDWARE_RS485_1_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    PAR_NONE,                /*!< No parity. */
    PAR_ODD,                 /*!< Odd parity. */
    PAR_EVEN                 /*!< Even parity. */
} Parity;

void rs485_1_init(void);

bool rs485_1_setup(uint32_t ulBaudRate, uint8_t ucDataBits, Parity parity);

void rs485_1_tx(bool enable);

void rs485_1_rx(bool enable);

char rs485_1_recv(void);

void rs485_1_send(char c);

void rs485_1_tx_handler(void);
void rs485_1_rx_handler(void);


#endif
