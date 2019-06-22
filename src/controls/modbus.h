#ifndef __CONTROLS_MODBUS_H__
#define __CONTROLS_MODBUS_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * Holding Registers (Analog Output)
 */
uint16_t modbus_Hreg(uint16_t addr);
void modbus_set_Hreg(uint16_t addr, uint16_t val);

/**
 * Coils (Digital Output)
 */
bool modbus_Coil(uint16_t addr);
uint16_t modbus_Coil_word(uint16_t addr16);
void modbus_set_Coil(uint16_t addr, bool val);

/**
 * Input Status (Digital Input) / read only
 */
void modbus_set_Ists(uint16_t addr, bool val);
void modbus_set_Ists_word(uint16_t addr16, uint16_t val16);

/**
 * Input Register (Analog Input) / read only
 */
void modbus_set_Ireg(uint16_t addr, uint16_t val);

/**
 * initializing hardware rs485 and freemodbus
 */
void modbus_init(void);

/**
 * periodically call it to serve freemodbus
 */
void modbus_poll(void);

#endif
