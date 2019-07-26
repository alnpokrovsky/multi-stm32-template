#if defined(STM32F1)


#include "iic.h"
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

typedef struct 
{
	uint32_t i2c_base;
	uint32_t i2c_rcc;
	uint32_t gpio_pins;
} iic_descr;

static const iic_descr IICS[] = {
	{
		I2C1, RCC_I2C1, 
		GPIO_I2C1_SCL/*B6*/|GPIO_I2C1_SDA/*B7*/,
	},
	{
		I2C2, RCC_I2C2, 
		GPIO_I2C2_SCL/*B10*/|GPIO_I2C2_SDA/*B11*/,
	},
};

void iic_init(IIC_PORT port) {
	/* both iic on portB */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(IICS[port].i2c_rcc);

	/* Set alternate functions for the SCL and SDA pins of I2C1. */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
		      IICS[port].gpio_pins);

	/* Disable the I2C before changing any configuration. */
	i2c_reset(IICS[port].i2c_base);
	i2c_peripheral_disable(IICS[port].i2c_base);

	i2c_set_speed(IICS[port].i2c_base,
		i2c_speed_fm_400k, I2C_CR2_FREQ_36MHZ);

	/* This is our slave address - needed only if we want to receive from other masters. */
	//i2c_set_own_7bit_slave_address(I2C1, 0x32);

    i2c_peripheral_enable(IICS[port].i2c_base);
}

void iic_send(IIC_PORT port, byte address, byte reg, byte value) {
	uint8_t data[2] = {reg, value};
    i2c_transfer7(IICS[port].i2c_base,
		address, data, 2, NULL, 0);
}

word iic_recv(IIC_PORT port, byte address, byte reg) {
    uint8_t data[2] = {reg, 0};
    i2c_transfer7(IICS[port].i2c_base,
		address, data, 1, data, 2);
    return data[0];
}

#endif
