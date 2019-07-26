#if defined(STM32F4)

#include "digitalpin.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

typedef struct {
    uint32_t port;
    uint32_t rcc;
    uint16_t pin;
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
            gpio_mode_setup(DIGITALPINS[pn].port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, DIGITALPINS[pn].pin);
            break;
        case DIGITALPIN_OUTPUT:
            gpio_mode_setup(DIGITALPINS[pn].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DIGITALPINS[pn].pin);
            break;
    }
}

void digitalpin_set(DIGITALPIN_NAME pn, bool state) {
    if (state) {
        gpio_set(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
    } else {
        gpio_clear(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
    }
}

void digitalpin_toggle(DIGITALPIN_NAME pn) {
    gpio_toggle(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
}

bool digitalpin_get(DIGITALPIN_NAME pn) {
    return gpio_get(DIGITALPINS[pn].port, DIGITALPINS[pn].pin);
}


#endif
