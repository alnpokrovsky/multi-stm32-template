#if defined(STM32F1)||defined(STM32F3)||defined(STM32F4)

#include <libopencm3/stm32/i2c.h>
#include <delay.h>

static void i2c_read7_v1_patched(uint32_t i2c, int addr, uint8_t *res, size_t n) {
	i2c_send_start(i2c);
	i2c_enable_ack(i2c);

	/* Wait for master mode selected */
	DELAY_TILL((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)));

	i2c_send_7bit_address(i2c, addr, I2C_READ);

	/* Waiting for address is transferred. */
	DELAY_TILL(I2C_SR1(i2c) & I2C_SR1_ADDR);
	/* Clearing ADDR condition sequence. */
	(void)I2C_SR2(i2c);

	for (size_t i = 0; i < n; ++i) {
		if (i == n - 1) {
			i2c_disable_ack(i2c);
		}
		DELAY_TILL(I2C_SR1(i2c) & I2C_SR1_RxNE);
		res[i] = i2c_get_data(i2c);
	}
	i2c_send_stop(i2c);
}

static void i2c_write7_v1_patched(uint32_t i2c, int addr, uint8_t *data, size_t n) {
	DELAY_TILL(!(I2C_SR2(i2c) & I2C_SR2_BUSY));

	i2c_send_start(i2c);

	/* Wait for master mode selected */
	DELAY_TILL((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)));

	i2c_send_7bit_address(i2c, addr, I2C_WRITE);

	/* Waiting for address is transferred. */
	DELAY_TILL(I2C_SR1(i2c) & I2C_SR1_ADDR);

	/* Clearing ADDR condition sequence. */
	(void)I2C_SR2(i2c);

	for (size_t i = 0; i < n; i++) {
		i2c_send_data(i2c, data[i]);
		DELAY_TILL(I2C_SR1(i2c) & (I2C_SR1_BTF));
	}
}

void i2c_transfer7_patched(uint32_t i2c, uint8_t addr, uint8_t *w, size_t wn, uint8_t *r, size_t rn);
void i2c_transfer7_patched(uint32_t i2c, uint8_t addr, uint8_t *w, size_t wn, uint8_t *r, size_t rn) {
	if (wn) {
		i2c_write7_v1_patched(i2c, addr, w, wn);
	}
	if (rn) {
		i2c_read7_v1_patched(i2c, addr, r, rn);
	} else {
		i2c_send_stop(i2c);
	}
}

#endif
