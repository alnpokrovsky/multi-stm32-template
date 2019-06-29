#if defined(STM32F1)

#include "uart.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <assert.h>


static void null_handler1(void) {}
static bool null_handler2(void) { return false; }

#pragma weak uart1_rx_handler = null_handler1
#pragma weak uart1_tx_handler = null_handler2
#pragma weak uart1_tc_handler = null_handler1

#pragma weak uart2_rx_handler = null_handler1
#pragma weak uart2_tx_handler = null_handler2
#pragma weak uart2_tc_handler = null_handler1


typedef struct {
    uint32_t usart_base;
    uint32_t usart_rcc;
    uint8_t usart_irq;
    uint32_t gpio_port;
    uint16_t gpio_port_rcc;
    uint32_t gpio_tx;
    uint32_t gpio_rx;
} Uart_descript;

static const Uart_descript UARTS[] = {
    {
        USART1, RCC_USART1, NVIC_USART1_IRQ,
        GPIOA, RCC_GPIOA, GPIO9, GPIO10,
    },
    {
        USART2, RCC_USART2, NVIC_USART2_IRQ,
        GPIOA, RCC_GPIOA, GPIO2, GPIO3,
    },
};

static const uint16_t PARITIES[] = {
    USART_PARITY_NONE,
    USART_PARITY_ODD,
    USART_PARITY_EVEN,
};

void uart_init(
    UART_PORT port,
    uint32_t ulBaudRate,
    uint8_t ucDataBits,
    UART_PARITY parity
) {
    rcc_periph_clock_enable(UARTS[port].gpio_port_rcc);
    rcc_periph_clock_enable(UARTS[port].usart_rcc);

    /* gpio init */
	gpio_set_mode(UARTS[port].gpio_port, 
        GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
        UARTS[port].gpio_tx
    );
	gpio_set_mode(UARTS[port].gpio_port, 
        GPIO_MODE_INPUT,
		GPIO_CNF_INPUT_FLOAT,
        UARTS[port].gpio_rx
    );

    /* Setup UART parameters. */
	usart_set_baudrate(UARTS[port].usart_base, ulBaudRate);
	usart_set_stopbits(UARTS[port].usart_base, USART_STOPBITS_1);
	usart_set_flow_control(UARTS[port].usart_base, USART_FLOWCONTROL_NONE);
	usart_set_mode(UARTS[port].usart_base, USART_MODE_TX_RX);
    usart_set_parity(UARTS[port].usart_base, PARITIES[parity]);
    /* word length includes parity. */
    switch ( ucDataBits )
    {
    case 8:
		if (parity == PAR_NONE)
			usart_set_databits(UARTS[port].usart_base, 8);
		else
			usart_set_databits(UARTS[port].usart_base, 9);
        break;
    case 7:
        /* 7 bits no parity not possible */
		assert(parity != PAR_NONE);
        usart_set_databits(UARTS[port].usart_base, 8);
        break;
    default:
        /* impossible to config other */
        assert(0);
    }

    /* Enable the USART1 interrupt. */
	nvic_enable_irq(UARTS[port].usart_irq);
    /* disable interupts for now, 
       call tx/rx_enable when you need it */
    usart_disable_rx_interrupt(UARTS[port].usart_base);
    usart_disable_tx_interrupt(UARTS[port].usart_base);

    /* Finally enable the USART. */
    usart_enable(UARTS[port].usart_base);
}

char uart_recv(UART_PORT port) {
    return usart_recv(UARTS[port].usart_base);
}

void uart_send(UART_PORT port, char c) {
    usart_send(UARTS[port].usart_base, c);
}

void uart_rx_tx_interrupt_enable(UART_PORT port, bool rx_en, bool tx_en) {
    if (rx_en) {
		usart_enable_rx_interrupt(UARTS[port].usart_base);
    } else {
        usart_disable_rx_interrupt(UARTS[port].usart_base);
    }

    if (tx_en) {
        usart_enable_tx_interrupt(UARTS[port].usart_base);
    } else {
		usart_disable_tx_interrupt(UARTS[port].usart_base);
    }
}

void usart1_isr() {
    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) && usart_get_flag(USART1, USART_FLAG_RXNE))
    {
        uart1_rx_handler();
    }
    /* Check if we were called because of TXE. */
    if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) && usart_get_flag(USART1, USART_FLAG_TXE))
    {
        /* Check if we need to interrupt on this transfer complete */
        if(!uart1_tx_handler())
        {
            USART_SR (USART1) &= ~USART_FLAG_TC; /* Clear TC flag*/
            USART_CR1(USART1) |= USART_CR1_TCIE; /* Enable transfer complite interrupt*/
        }
    }
    /* Check if we were called because of transfer comlite*/
    if (((USART_CR1(USART1) & USART_CR1_TCIE) != 0) && usart_get_flag(USART1, USART_FLAG_TC))
    {
        uart1_tc_handler();

        USART_CR1(USART1) &= ~USART_CR1_TCIE;/* Disble transfer complite interrupt*/
        USART_SR (USART1) &= ~USART_FLAG_TC;   /* Clear TC flag*/
    }
}

void usart2_isr() {
    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) && usart_get_flag(USART2, USART_FLAG_RXNE))
    {
        uart2_rx_handler();
    }
    /* Check if we were called because of TXE. */
    if (((USART_CR1(USART2) & USART_CR1_TXEIE) != 0) && usart_get_flag(USART2, USART_FLAG_TXE))
    {
        /* Check if we need to interrupt on this transfer complete */
        if(uart2_tx_handler())
        {
            USART_SR (USART2) &= ~USART_FLAG_TC; /* Clear TC flag*/
            USART_CR1(USART2) |= USART_CR1_TCIE; /* Enable transfer complite interrupt*/
        }
    }
    /* Check if we were called because of transfer comlite*/
    if (((USART_CR1(USART2) & USART_CR1_TCIE) != 0) && usart_get_flag(USART2, USART_FLAG_TC))
    {
        uart2_tc_handler();

        USART_CR1(USART2) &= ~USART_CR1_TCIE;/* Disble transfer complite interrupt*/
        USART_SR (USART2) &= ~USART_FLAG_TC;   /* Clear TC flag*/
    }
}

#endif