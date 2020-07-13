#if defined(USE_MDR1986VE9x) && defined(NO)

#include "digitalpin.h"
#include <MDR32Fx.h>
#include <MDR32F9Qx_rst_clk.h>


typedef struct {
    MDR_PORT_TypeDef * port;
    uint8_t pin_pos;
} digitalpin_descript;

static const digitalpin_descript DIGITALPINS[] = {
    {MDR_PORTA, 0},
    {MDR_PORTA, 1},
    {MDR_PORTA, 2},
    {MDR_PORTA, 3},
    {MDR_PORTA, 4},
    {MDR_PORTA, 5},
    {MDR_PORTA, 6},
    {MDR_PORTA, 7},
    {MDR_PORTA, 8},
    {MDR_PORTA, 9},
    {MDR_PORTA, 10},
    {MDR_PORTA, 11},
    {MDR_PORTA, 12},
    {MDR_PORTA, 13},
    {MDR_PORTA, 14},
    {MDR_PORTA, 15},
    
    {MDR_PORTB, 0},
    {MDR_PORTB, 1},
    {MDR_PORTB, 2},
    {MDR_PORTB, 3},
    {MDR_PORTB, 4},
    {MDR_PORTB, 5},
    {MDR_PORTB, 6},
    {MDR_PORTB, 7},
    {MDR_PORTB, 8},
    {MDR_PORTB, 9},
    {MDR_PORTB, 10},
    {MDR_PORTB, 11},
    {MDR_PORTB, 12},
    {MDR_PORTB, 13},
    {MDR_PORTB, 14},
    {MDR_PORTB, 15},

    {MDR_PORTC, 0},
    {MDR_PORTC, 1},
    {MDR_PORTC, 2},
    {MDR_PORTC, 3},
    {MDR_PORTC, 4},
    {MDR_PORTC, 5},
    {MDR_PORTC, 6},
    {MDR_PORTC, 7},
    {MDR_PORTC, 8},
    {MDR_PORTC, 9},
    {MDR_PORTC, 10},
    {MDR_PORTC, 11},
    {MDR_PORTC, 12},
    {MDR_PORTC, 13},
    {MDR_PORTC, 14},
    {MDR_PORTC, 15},
};

void digitalpin_mode(DIGITALPIN_NAME pn, DIGITALPIN_MODE pmode) {
    /* тактирование порта */
    RST_CLK_PCLKcmd(PCLK_BIT(DIGITALPINS[pn].port), ENABLE);
    
    /* digital mode */
    DIGITALPINS[pn].port->ANALOG |= (1UL << DIGITALPINS[pn].pin_pos);
    DIGITALPINS[pn].port->PULL   |= (1UL << DIGITALPINS[pn].pin_pos) << PORT_PULL_DOWN_Pos; // pull down
    DIGITALPINS[pn].port->PULL   |= (1UL << DIGITALPINS[pn].pin_pos) << PORT_PULL_UP_Pos;   //pull up
    DIGITALPINS[pn].port->PWR    |= 0x01 << 2*DIGITALPINS[pn].pin_pos; // speed slow

    switch (pmode)
    {
    case DIGITALPIN_INPUT:
        
        break;
    case DIGITALPIN_OUTPUT:
        DIGITALPINS[pn].port->OE |= (1UL << DIGITALPINS[pn].pin_pos); // output
        break;
    }
}

void digitalpin_set(DIGITALPIN_NAME pn, bool state) {
    if (state)
        DIGITALPINS[pn].port->RXTX |= (1UL << DIGITALPINS[pn].pin_pos);
    else
        DIGITALPINS[pn].port->RXTX &= ~(1UL << DIGITALPINS[pn].pin_pos);
}

void digitalpin_toggle(DIGITALPIN_NAME pn) {
    DIGITALPINS[pn].port->RXTX ^= (1UL << DIGITALPINS[pn].pin_pos);
}

bool digitalpin_get(DIGITALPIN_NAME pn) {
    return DIGITALPINS[pn].port->RXTX;
}


#endif