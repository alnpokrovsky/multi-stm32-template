#if defined(STM32F1)

#include "digitalpin.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

typedef struct {
    uint32_t port;
    uint16_t rcc;
    uint32_t pin;
} digitalpin_descript;

static const digitalpin_descript DIGITALPINS[] = {
    {GPIOA, RCC_GPIOA, GPIO0},
    {GPIOA, RCC_GPIOA, GPIO1},
    {GPIOA, RCC_GPIOA, GPIO2},
    {GPIOA, RCC_GPIOA, GPIO3},
    {GPIOA, RCC_GPIOA, GPIO4},
    {GPIOA, RCC_GPIOA, GPIO5},
    {GPIOA, RCC_GPIOA, GPIO6},
    {GPIOA, RCC_GPIOA, GPIO7},
    {GPIOA, RCC_GPIOA, GPIO8},
    {GPIOA, RCC_GPIOA, GPIO9},
    {GPIOA, RCC_GPIOA, GPIO10},
    {GPIOA, RCC_GPIOA, GPIO11},
    {GPIOA, RCC_GPIOA, GPIO12},
    {GPIOA, RCC_GPIOA, GPIO13},
    {GPIOA, RCC_GPIOA, GPIO14},
    {GPIOA, RCC_GPIOA, GPIO15},

    {GPIOB, RCC_GPIOB, GPIO0},
    {GPIOB, RCC_GPIOB, GPIO1},
    {GPIOB, RCC_GPIOB, GPIO2},
    {GPIOB, RCC_GPIOB, GPIO3},
    {GPIOB, RCC_GPIOB, GPIO4},
    {GPIOB, RCC_GPIOB, GPIO5},
    {GPIOB, RCC_GPIOB, GPIO6},
    {GPIOB, RCC_GPIOB, GPIO7},
    {GPIOB, RCC_GPIOB, GPIO8},
    {GPIOB, RCC_GPIOB, GPIO9},
    {GPIOB, RCC_GPIOB, GPIO10},
    {GPIOB, RCC_GPIOB, GPIO11},
    {GPIOB, RCC_GPIOB, GPIO12},
    {GPIOB, RCC_GPIOB, GPIO13},
    {GPIOB, RCC_GPIOB, GPIO14},
    {GPIOB, RCC_GPIOB, GPIO15},

    {GPIOC, RCC_GPIOC, GPIO0},
    {GPIOC, RCC_GPIOC, GPIO1},
    {GPIOC, RCC_GPIOC, GPIO2},
    {GPIOC, RCC_GPIOC, GPIO3},
    {GPIOC, RCC_GPIOC, GPIO4},
    {GPIOC, RCC_GPIOC, GPIO5},
    {GPIOC, RCC_GPIOC, GPIO6},
    {GPIOC, RCC_GPIOC, GPIO7},
    {GPIOC, RCC_GPIOC, GPIO8},
    {GPIOC, RCC_GPIOC, GPIO9},
    {GPIOC, RCC_GPIOC, GPIO10},
    {GPIOC, RCC_GPIOC, GPIO11},
    {GPIOC, RCC_GPIOC, GPIO12},
    {GPIOC, RCC_GPIOC, GPIO13},
    {GPIOC, RCC_GPIOC, GPIO14},
    {GPIOC, RCC_GPIOC, GPIO15},
};

void digitalpin_mode(DIGITALPIN_NAME pn, DIGITALPIN_MODE pmode) {
    rcc_periph_clock_enable(DIGITALPINS[pn].rcc);
    switch (pmode)
    {
        case DIGITALPIN_INPUT:
            gpio_set_mode(DIGITALPINS[pn].port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, DIGITALPINS[pn].pin);
            digitalpin_set(pn, false);
            break;
        case DIGITALPIN_OUTPUT:
            if (pn == PB_3) { /* fix JTDO pin */
                rcc_periph_clock_enable(RCC_AFIO);
                gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, 0);
            }
            gpio_set_mode(DIGITALPINS[pn].port, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, DIGITALPINS[pn].pin);
            break;
    }
}

void digitalpin_set(DIGITALPIN_NAME pn, uint16_t state) {
    if (state != 0) {
        gpio_set(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
    } else {
        gpio_clear(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
    }
}

void digitalpin_toggle(DIGITALPIN_NAME pn) {
    gpio_toggle(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
}

uint16_t digitalpin_get(DIGITALPIN_NAME pn) {
    return gpio_get(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
}


#endif
