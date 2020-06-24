#if defined(STM32F4)

#include "iic.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

typedef struct 
{
	uint32_t i2c_base;
	uint16_t i2c_rcc;
    uint32_t gpio_scl_port;
    uint32_t gpio_scl_rcc;
    uint32_t gpio_scl_pin;
    uint32_t gpio_sda_port;
    uint32_t gpio_sda_rcc;
	uint32_t gpio_sda_pin;
} iic_descr;

static const iic_descr IICS[] = {
	{},
	{},
    {
		I2C3, RCC_I2C3, 
		GPIOA, RCC_GPIOA, GPIO8,
		GPIOC, RCC_GPIOC, GPIO9,
	},
};

void iic_init(IIC_PORT port) {
    /* Enable clocks for I2C2 and GPIO. */
	rcc_periph_clock_enable(IICS[port].i2c_rcc);
	rcc_periph_clock_enable(IICS[port].gpio_scl_rcc);
	rcc_periph_clock_enable(IICS[port].gpio_sda_rcc);

	/* Set gpio and alternate functions for the SCL and SDA pins of I2C3. */
	gpio_mode_setup(IICS[port].gpio_scl_port, GPIO_MODE_AF, GPIO_PUPD_NONE, IICS[port].gpio_scl_pin);
	gpio_mode_setup(IICS[port].gpio_sda_port, GPIO_MODE_AF, GPIO_PUPD_NONE, IICS[port].gpio_sda_pin);
	gpio_set_af(IICS[port].gpio_scl_port, GPIO_AF4, IICS[port].gpio_scl_pin);
	gpio_set_af(IICS[port].gpio_sda_port, GPIO_AF4, IICS[port].gpio_sda_pin);

	/* Disable the I2C before changing any configuration. */
	i2c_peripheral_disable(IICS[port].i2c_base);

    i2c_set_speed(IICS[port].i2c_base, i2c_speed_sm_100k, I2C_CR2_FREQ_36MHZ);

	/*
	 * This is our slave address - needed only if we want to receive from
	 * other masters.
	 */
	i2c_set_own_7bit_slave_address(IICS[port].i2c_base, 0x32);

	/* If everything is configured -> enable the peripheral. */
    i2c_peripheral_enable(IICS[port].i2c_base);
}

extern void i2c_transfer7_patched(uint32_t i2c, uint8_t addr, uint8_t *w, size_t wn, uint8_t *r, size_t rn);

void iic_send(IIC_PORT port, uint8_t address, uint8_t reg, uint8_t value) {
	uint8_t data[2] = {reg, value};
    i2c_transfer7_patched(IICS[port].i2c_base,
		address, data, 2, NULL, 0);
}

uint8_t iic_recv(IIC_PORT port, uint8_t address, uint8_t reg) {
    uint8_t data[2] = {reg, 0};
    i2c_transfer7_patched(IICS[port].i2c_base,
		address, data, 1, data, 2);
    return data[0];
}

#endif
