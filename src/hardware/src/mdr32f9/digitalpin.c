#if defined(USE_MDR1986VE9x)

#include "digitalpin.h"
#include "MDR32Fx.h"

typedef struct {
    MDR_PORT_TypeDef * port;
    uint32_t rcc;
    uint8_t pin_pos;
} digitalpin_descript;

static const digitalpin_descript DIGITALPINS[] = {
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 0},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 1},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 2},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 3},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 4},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 5},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 6},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 7},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 8},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 9},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 10},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 11},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 12},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 13},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 14},
    {MDR_PORTA, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTA_Pos, 15},
    
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 0},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 1},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 2},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 3},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 4},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 5},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 6},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 7},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 8},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 9},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 10},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 11},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 12},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 13},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 14},
    {MDR_PORTB, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTB_Pos, 15},

    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 0},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 1},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 2},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 3},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 4},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 5},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 6},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 7},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 8},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 9},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 10},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 11},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 12},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 13},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 14},
    {MDR_PORTC, 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos, 15},
};

void digitalpin_mode(DIGITALPIN_NAME pn, DIGITALPIN_MODE pmode) {
    /* RCC */
    MDR_RST_CLK->PER_CLOCK |= DIGITALPINS[pn].rcc;
    
    /* digital mode */
    DIGITALPINS[pn].port->ANALOG |= (1 << DIGITALPINS[pn].pin_pos);
    DIGITALPINS[pn].port->PULL   |= (1 << DIGITALPINS[pn].pin_pos) << PORT_PULL_DOWN_Pos; // pull down
    DIGITALPINS[pn].port->PULL   |= (1 << DIGITALPINS[pn].pin_pos) << PORT_PULL_UP_Pos;   //pull up
    DIGITALPINS[pn].port->PWR    |= 0x01 << 2*DIGITALPINS[pn].pin_pos; // speed slow

    switch (pmode)
    {
    case DIGITALPIN_INPUT:
        
        break;
    case DIGITALPIN_OUTPUT:
        DIGITALPINS[pn].port->OE |= (1 << DIGITALPINS[pn].pin_pos); // output
        break;
    }
}

void digitalpin_set(DIGITALPIN_NAME pn, uint16_t state) {
    if (state)
        DIGITALPINS[pn].port->RXTX |= (1 << DIGITALPINS[pn].pin_pos);
    else
        DIGITALPINS[pn].port->RXTX &= ~(1 << DIGITALPINS[pn].pin_pos);
}

void digitalpin_toggle(DIGITALPIN_NAME pn) {
    DIGITALPINS[pn].port->RXTX ^= (1 << DIGITALPINS[pn].pin_pos);
}

uint16_t digitalpin_get(DIGITALPIN_NAME pn) {
    return DIGITALPINS[pn].port->RXTX;
}


#endif