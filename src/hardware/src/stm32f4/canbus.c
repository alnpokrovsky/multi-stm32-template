#if defined(STM32F4)

#include "canbus.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

/* пропатченные функции can */
extern void can_filter_init_patched(uint32_t canport, uint32_t nr, bool scale_32bit,
		     bool id_list_mode, uint32_t fr1, uint32_t fr2,
		     uint32_t fifo, bool enable);
extern bool can_transmit_patched(uint32_t canport, uint32_t mailbox,
    uint32_t id, bool ext, bool rtr, uint8_t length, uint8_t *data);

static inline uint8_t can_recv_fifo_cnt(uint32_t canport, uint32_t fifo) {
    if (fifo == 0) return CAN_RF0R(canport) & CAN_RF0R_FMP0_MASK;
    else return CAN_RF1R(canport) & CAN_RF1R_FMP1_MASK;
}

static void null_handler1(void) {}
static bool null_handler2(void) { return false; }
static void null_handler3(CANBUS_Message * rcv) { (void) rcv; }

#pragma weak canbus1_rx_handler = null_handler3
#pragma weak canbus1_tx_handler = null_handler2
#pragma weak canbus1_tc_handler = null_handler1

#pragma weak canbus2_rx_handler = null_handler3
#pragma weak canbus2_tx_handler = null_handler2
#pragma weak canbus2_tc_handler = null_handler1


typedef struct
{
    uint32_t can_base;
    uint16_t canbus_rcc;
    uint8_t canbus_rx_irq;
    uint8_t canbus_tx_irq;
    uint32_t gpio_port;
    uint16_t gpio_port_rcc;
    uint32_t gpio_pins;
    uint8_t gpio_pins_af;
} canbus_descript;


static const canbus_descript CANS[] = {
    {
        CAN1, RCC_CAN1, NVIC_CAN1_RX0_IRQ, NVIC_CAN1_TX_IRQ,
        GPIOB, RCC_GPIOB, GPIO8|GPIO9, GPIO_AF9,
    },
    {
        CAN2, RCC_CAN2, NVIC_CAN2_RX0_IRQ, NVIC_CAN2_TX_IRQ,
        GPIOB, RCC_GPIOB, GPIO12|GPIO13, GPIO_AF9,
    },
};

void canbus_init(CANBUS_PORT n) {
    uint32_t bitrate = 500000; // 500kbit/sec

    /* enable rcc */
    rcc_periph_clock_enable(CANS[n].canbus_rcc);
    rcc_periph_clock_enable(CANS[n].gpio_port_rcc);

    // gpio settings
    //gpio_mode_setup(CANS[n].gpio_port, GPIO_MODE_AF|GPIO_OSPEED_50MHZ, GPIO_OTYPE_PP, GPIO9);
    gpio_mode_setup(CANS[n].gpio_port, GPIO_MODE_AF|GPIO_OSPEED_50MHZ, GPIO_PUPD_NONE, CANS[n].gpio_pins);
    gpio_set_af(CANS[n].gpio_port, CANS[n].gpio_pins_af, CANS[n].gpio_pins);

    /* Reset CAN. */
    can_reset(CANS[n].can_base);
    can_init(CANS[n].can_base,
        false,		   //TTCM: Time triggered comm mode?
        false,		   //ABOM: Automatic bus-off management?
        false,		   //AWUM: Automatic wakeup mode?
        false,		   //NART: No automatic retransmission?
        false,		   //RFLM: Receive FIFO locked mode?
        true,		   //TXFP: Transmit FIFO priority?
        CAN_BTR_SJW_1TQ,
        CAN_BTR_TS1_8TQ,
        CAN_BTR_TS2_5TQ,
        rcc_apb1_frequency/bitrate/14, //BRP+1: Baud rate prescaler
        false,			//LOOPBACK?
        false);	        //SILENT?
}

void canbus_addFilter(CANBUS_PORT n, const CANBUS_Filter * filter) {
    can_filter_init_patched(
        CANS[n].can_base,
        filter->n,	    // Filter ID
        true, false,    // 32bit filter mode
        filter->id,	    // CAN ID
        filter->mask,	// CAN ID mask
        0,	            // FIFO assignment (here: FIFO0)
        true);          // Enable the filter
}

void canbus_rx_tx_interrupt_enable(CANBUS_PORT n, bool rx_en, bool tx_en) {
    (void) tx_en;

    if (rx_en) {
        nvic_enable_irq(CANS[n].canbus_rx_irq);
        can_enable_irq(CANS[n].can_base, CAN_IER_FMPIE0);
    } else {
        nvic_disable_irq(CANS[n].canbus_rx_irq);
        can_disable_irq(CANS[n].can_base, CAN_IER_FMPIE0);
    }
}

bool canbus_send(CANBUS_PORT n, CANBUS_Priority pr, const CANBUS_Message * msg) { 
    return can_transmit_patched(CANS[n].can_base, pr,
        msg->ID,
        false,      /* ext? */
        false,      /* rtr? */
        msg->DLC,
        (uint8_t*)msg->data32);
}

bool canbus_recv(CANBUS_PORT n, CANBUS_Message * rcv) {
    bool ext, rtr;
	uint8_t fmi;
    if (can_recv_fifo_cnt(CANS[n].can_base, 0) == 0) 
        return false; // если ничего не принято
    can_receive(CANS[n].can_base, 0, true, 
        &rcv->ID, 
        &ext, 
        &rtr, 
        &fmi, 
        &rcv->DLC, 
        (uint8_t*)rcv->data32, 
        0);
    return true;
}


void can1_rx0_isr(void)
{
    int msgcnt = can_recv_fifo_cnt(CAN1, 0);
    for (int i = 0; i < msgcnt; ++i) {
        CANBUS_Message rcv;
        canbus_recv(CANBUS_1, &rcv);
        canbus1_rx_handler(&rcv);
    }
}

void can2_rx0_isr(void)
{
    int msgcnt = can_recv_fifo_cnt(CAN2, 0);
    for (int i = 0; i < msgcnt; ++i) {
        CANBUS_Message rcv;
        canbus_recv(CANBUS_2, &rcv);
        canbus2_rx_handler(&rcv);
    }
}

#endif
