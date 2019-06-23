#if defined(STM32F1)


#include "i2c.h"
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>


void i2c_init() {
    /* Enable clocks for I2C1 and AFIO. */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_I2C1);

	/* Set alternate functions for the SCL and SDA pins of I2C1. */

	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
		      GPIO_I2C1_SCL | GPIO_I2C1_SDA);

	/* Disable the I2C before changing any configuration. */
	i2c_reset(I2C1);
	i2c_peripheral_disable(I2C1);

	i2c_set_speed(I2C1, i2c_speed_fm_400k, I2C_CR2_FREQ_36MHZ);

	/*
	 * This is our slave address - needed only if we want to receive from
	 * other masters.
	 */
	//i2c_set_own_7bit_slave_address(I2C1, 0x32);

	/* If everything is configured -> enable the peripheral. */
    i2c_peripheral_enable(I2C1);
}

void i2c_send(byte address, byte reg, byte value) {
	uint8_t data[2] = {reg, value};
    i2c_transfer7(I2C1, address, data, 2, NULL, 0);
}

word i2c_recv(byte address, byte reg) {
    uint8_t data[2] = {reg, 0};
    i2c_transfer7(I2C1, address, data, 1, data, 2);
    return data[0];
}

#endif
