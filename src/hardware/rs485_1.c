#include "rs485_1.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include "hardware/gpio.h"


static void null_handler(void)
{
	/* Do nothing. */
}

#pragma weak rs485_1_tx_handler = null_handler
#pragma weak rs485_1_rx_handler = null_handler

#define TXE_PIN PB_12


void rs485_1_init()
{
    /* init txe pin */
    gpio_pin_mode(TXE_PIN, GPIO_OUTPUT);
    gpio_pin_set(TXE_PIN, 0);

#if defined(STM32F1)
    /* Enable clocks for GPIO port A (for GPIO_USART1_TX) and USART1. */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN |
				    RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN);
	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port A for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	/* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port A for receive. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
#elif defined(STM32F3) || defined(STM32F4)
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
    gpio_set_af(GPIOA, GPIO_AF1, GPIO9 | GPIO10);
#endif

	/* Enable the USART1 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);
}

bool rs485_1_setup(uint32_t ulBaudRate, uint8_t ucDataBits, Parity parity)
{
    /* Setup UART parameters. */
	usart_set_baudrate(USART1, ulBaudRate);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);

    switch ( parity )
    {
    case PAR_NONE:
        usart_set_parity(USART1, USART_PARITY_NONE);
        break;
    case PAR_ODD:
        usart_set_parity(USART1, USART_PARITY_ODD);
        break;
    case PAR_EVEN:
        usart_set_parity(USART1, USART_PARITY_EVEN);
        break;
    default:
        return false;
        break;
    }

/* Oddity of STM32F series: word length includes parity. 7 bits no parity
   not possible */
    uint8_t wordLength;
    switch ( ucDataBits )
    {
    case 8:
		if (parity == PAR_NONE)
			wordLength = 8;
		else
			wordLength = 9;
        usart_set_databits(USART1,wordLength);
        break;
    case 7:
		if (parity == PAR_NONE)
			return false;
		else
        	usart_set_databits(USART1,8);
        break;
    default:
        return false;
    }

    /* Finally enable the USART. */
    usart_disable_rx_interrupt(USART1);
    usart_disable_tx_interrupt(USART1);
    usart_enable(USART1);

    return true;
}

static volatile bool txen = false;

void rs485_1_tx(bool enable) {
    if (enable) {
        txen = true;
        usart_enable_tx_interrupt(USART1);
        gpio_pin_set(TXE_PIN, 1);
    } else {
        txen = false;
		usart_disable_tx_interrupt(USART1);
    }
}

void rs485_1_rx(bool enable) {
    if (enable) {
        txen = false;
		usart_enable_rx_interrupt(USART1);
    } else {
        usart_disable_rx_interrupt(USART1);
    }
}

char rs485_1_recv() {
    return usart_recv(USART1);
}

void rs485_1_send(char c) {
    usart_send(USART1, c);
}

void usart1_isr(void)
{
	/* Check if we were called because of RXNE. */
    if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) && ((USART_SR(USART1) & USART_SR_RXNE) != 0))
    {
        rs485_1_rx_handler();
    }
    /* Check if we were called because of TXE. */
    if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) && ((USART_SR(USART1) & USART_SR_TXE) != 0))
    {
        rs485_1_tx_handler();
        /* Check if we need to disable transmitter*/
        if(!txen)
        {
            USART_SR (USART1) &= ~USART_SR_TC;   /* Clear TC flag*/
            USART_CR1(USART1) |= USART_CR1_TCIE; /* Enable transfer complite interrupt*/
        }
    }
    /* Disable transmitter on transfer comlite*/
    if (((USART_CR1(USART1) & USART_CR1_TCIE) != 0) && ((USART_SR(USART1) & USART_SR_TC) != 0))
    {
        USART_CR1(USART1) &= ~USART_CR1_TCIE;/* Disble transfer complite interrupt*/
        USART_SR (USART1) &= ~USART_SR_TC;   /* Clear TC flag*/
        /* Disable transmitter*/
        gpio_pin_set(TXE_PIN, 0);
    }
}
