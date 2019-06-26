#ifndef __HARDWARE_UART_H__
#define __HARDWARE_UART_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PAR_NONE,    /*!< No parity. */
    PAR_ODD,     /*!< Odd parity. */
    PAR_EVEN     /*!< Even parity. */
} UART_PARITY;

typedef enum {
    UART_1,
    UART_2,
} UART_PORT;

void uart_init(
    UART_PORT port,
    uint32_t ulBaudRate,
    uint8_t ucDataBits,
    UART_PARITY parity
);

void uart_rx_tx_interrupt_enable(UART_PORT port, bool rx_en, bool tx_en);

char uart_recv(UART_PORT port);
void uart_send(UART_PORT port, char c);


void uart1_rx_handler(void);
bool uart1_tx_handler(void);
void uart1_tc_handler(void);

void uart2_rx_handler(void);
bool uart2_tx_handler(void);
void uart2_tc_handler(void);

#endif