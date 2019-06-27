#if defined(USE_MDR1986VE9x)

#include "uart.h"
#include "delay.h"
#include <MDR32Fx.h>
#include <MDR32F9Qx_uart.h>
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
    uint8_t uart_irq;
    MDR_PORT_TypeDef * gpio_port;
    uint8_t gpio_port_rcc_pos;
    uint8_t gpio_rx_pos;
    uint8_t gpio_tx_pos;
} Uart_descript;

static const Uart_descript UARTS[] = {
    {
        MDR_UART1, RST_CLK_PER_CLOCK_PCLK_EN_UART1_Pos, UART1_IRQn,
        MDR_PORTA, RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 6, 7
    },
    {
        MDR_UART2, RST_CLK_PER_CLOCK_PCLK_EN_UART2_Pos, UART2_IRQn,
        MDR_PORTF, RST_CLK_PER_CLOCK_PCLK_EN_PORTF_Pos, 0, 1
    },
};

static const uint16_t PARITIES[] = {
    UART_Parity_No,
    UART_Parity_Even,
    UART_Parity_Odd,
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
    /* Set the HCLK division factor = 1 for UART2*/
    UART_BRGInit(MDR_UART2, UART_HCLKdiv1);
    
    static UART_InitTypeDef UART_InitStructure;

    /* Initialize UART_InitStructure */
    UART_InitStructure.UART_BaudRate                = ulBaudRate;
    UART_InitStructure.UART_WordLength              = UART_WordLength8b;
    UART_InitStructure.UART_StopBits                = UART_StopBits1;
    UART_InitStructure.UART_Parity                  = PARITIES[parity];
    UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
    UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

    assert(((ucDataBits == 8) && (parity == PAR_NONE))
        || ((ucDataBits == 7) && (parity != PAR_NONE))
    );

    /* разрешаем прерываение */
    NVIC_EnableIRQ(UARTS[port].uart_irq);
    
    /* Configure UART2 parameters*/
    UART_Init (UARTS[port].uart_base, &UART_InitStructure);

    /* разрешение приемопередатчика UART */
    UART_Cmd(UARTS[port].uart_base, ENABLE);

    delay_some();
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
    return UART_ReceiveData(UARTS[port].uart_base);
}

void uart_send(UART_PORT port, char c) {
    UART_SendData(UARTS[port].uart_base, c);
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