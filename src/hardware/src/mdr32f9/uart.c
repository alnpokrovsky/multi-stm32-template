#if defined(USE_MDR1986VE9x)

#include "uart.h"
#include <MDR32Fx.h>
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
    MDR_UART_TypeDef * uart_base;
    uint8_t uart_rcc_pos;
    uint8_t uart_clock_pos;
    uint8_t uart_clock_brg_pos;
    uint8_t uart_irq;
    MDR_PORT_TypeDef * gpio_port;
    uint8_t gpio_port_rcc_pos;
    uint8_t gpio_rx_pos;
    uint8_t gpio_tx_pos;
} Uart_descript;

static const Uart_descript UARTS[] = {
    {
        MDR_UART1, RST_CLK_PER_CLOCK_PCLK_EN_UART1_Pos, RST_CLK_UART_CLOCK_UART1_CLK_EN_Pos, RST_CLK_UART_CLOCK_UART1_BRG_Pos, UART1_IRQn,
        MDR_PORTA, RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 6, 7
    },
    {
        MDR_UART2, RST_CLK_PER_CLOCK_PCLK_EN_UART2_Pos, RST_CLK_UART_CLOCK_UART2_CLK_EN_Pos, RST_CLK_UART_CLOCK_UART2_BRG_Pos, UART2_IRQn,
        MDR_PORTF, RST_CLK_PER_CLOCK_PCLK_EN_PORTF_Pos, 0, 1
    },
};

void uart_init(
    UART_PORT port,
    uint32_t ulBaudRate,
    uint8_t ucDataBits,
    UART_PARITY parity
) {
    /* тактирование порта */
    MDR_RST_CLK->PER_CLOCK |= (1UL << UARTS[port].gpio_port_rcc_pos);
    /* переопределенная функция */
    UARTS[port].gpio_port->FUNC |= PORT_FUNC_MODE_OVER << 2*UARTS[port].gpio_rx_pos;
    UARTS[port].gpio_port->FUNC |= PORT_FUNC_MODE_OVER << 2*UARTS[port].gpio_tx_pos;
    /* цифровые */
    UARTS[port].gpio_port->ANALOG |= 1UL << UARTS[port].gpio_rx_pos;
    UARTS[port].gpio_port->ANALOG |= 1UL << UARTS[port].gpio_tx_pos;
    /* максимальная скорость */
    UARTS[port].gpio_port->PWR |= PORT_PWR_MAX_FAST << 2*UARTS[port].gpio_rx_pos;
    UARTS[port].gpio_port->PWR |= PORT_PWR_MAX_FAST << 2*UARTS[port].gpio_tx_pos;

    /* Тактирование UART */
    MDR_RST_CLK->PER_CLOCK |= 1UL << UARTS[port].uart_rcc_pos;
    /* предделитель UART */
    MDR_RST_CLK->UART_CLOCK |= RST_CLK_UART_CLOCK_UART_BRG_HCLK << UARTS[port].uart_clock_brg_pos;
    MDR_RST_CLK->UART_CLOCK |= 1UL << UARTS[port].uart_clock_pos;
    /* Разрешаем FIFO */
    //UARTS[port].uart_base->LCR_H |= UART_LCR_H_FEN;

    float coef = 80000000.0 / (16*ulBaudRate);
    uint16_t ce = (uint16_t) coef;
    uint16_t fl = (uint16_t) ((coef - ce) * 64 + 0.5);
    UARTS[port].uart_base->IBRD = ce;
    UARTS[port].uart_base->FBRD = fl;

    switch ( ucDataBits )
    {
    case 8:
		if (parity == PAR_NONE)
            UARTS[port].uart_base->LCR_H |= UART_LCR_H_WLEN_8_BITS << UART_LCR_H_WLEN_Pos;
        else 
            assert(parity == PAR_NONE); // not support 9bits
        break;
    case 7:
        /* 7 bits no parity not possible */
		assert(parity != PAR_NONE);
        UARTS[port].uart_base->LCR_H |= UART_LCR_H_WLEN_8_BITS << UART_LCR_H_WLEN_Pos;
        break;
    default:
        /* impossible to config other */
        assert(0);
    }

    switch (parity)
    {
    case PAR_NONE:
        break;    
    case PAR_EVEN:
        UARTS[port].uart_base->LCR_H |= UART_LCR_H_BRK;
        break;
    case PAR_ODD:
        UARTS[port].uart_base->LCR_H |= UART_LCR_H_BRK;
        UARTS[port].uart_base->LCR_H |= UART_LCR_H_PEN;
        break;
    }

    /* разрешаем прерываение */
    NVIC_EnableIRQ(UARTS[port].uart_irq);
    
    /* передачик и приемник разрешен, разрешение приемопередатчика UART1 */
    UARTS[port].uart_base->CR |= UART_CR_TXE | UART_CR_RXE;
    UARTS[port].uart_base->CR |= UART_CR_UARTEN;
}

void uart_rx_tx_interrupt_enable(
    UART_PORT port,
    bool rx_en, bool tx_en
) {
    if (rx_en) {
        UARTS[port].uart_base->IMSC |= UART_IMSC_RXIM;
    } else {
        UARTS[port].uart_base->IMSC &= ~UART_IMSC_RXIM;
    }

    if (tx_en) {
        UARTS[port].uart_base->IMSC |= UART_IMSC_TXIM;
        switch (port)
        {
        case UART_1:
            uart1_tx_handler();
            break;
        case UART_2:
            uart2_tx_handler();
            break;
        }
    } else {
        UARTS[port].uart_base->IMSC &= ~UART_IMSC_TXIM;
    }
}

char uart_recv(UART_PORT port) {
    return UARTS[port].uart_base->DR;
}

void uart_send(UART_PORT port, char c) {
    UARTS[port].uart_base->DR = c;
}

void UART1_IRQHandler(void);
void UART1_IRQHandler(void) {
    /* Check if we were called because of RXNE. */
    if ((MDR_UART1->MIS & UART_MIS_RXMIS) != 0)
    {
        uart1_rx_handler();
    }
    /* Check if we were called because of TXE. */
    if ((MDR_UART1->MIS & UART_MIS_TXMIS) != 0)
    {
        static bool TCE = true;
        /* Check if we were called because of transfer comlite*/
        if (!TCE) {
            /* Check if we need to interrupt on this transfer complete */
            if(!uart1_tx_handler()) {
                /* enable transfer complete */
                TCE = true;
            }
        } else {
            /* disable transfer complete */
            TCE = false;
            uart1_tc_handler();
        }
    }
}

void UART2_IRQHandler(void);
void UART2_IRQHandler(void) {
    uart2_rx_handler();
    /* Check if we were called because of RXNE. */
    if ((MDR_UART2->MIS & UART_MIS_RXMIS) != 0)
    {
        uart2_rx_handler();
    }
    /* Check if we were called because of TXE. */
    if ((MDR_UART2->MIS & UART_MIS_TXMIS) != 0)
    {
        static bool TCE = false;
        /* Check if we were called because of transfer comlite*/
        if (!TCE) {
            /* Check if we need to interrupt on this transfer complete */
            if(!uart2_tx_handler()) {
                /* enable transfer complete */
                TCE = true;
            }
        } else {
            /* disable transfer complete */
            TCE = false;
            uart2_tc_handler();
        }
    }
}

#endif