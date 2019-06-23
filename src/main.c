#include "rcc.h"
#include "digitalpin.h"
#include "controls/modbus.h"


int main(void)
{
	rcc_init();

	// digitalpin_mode(PC_0, DIGITALPIN_OUTPUT);
	// digitalpin_mode(PC_1, DIGITALPIN_OUTPUT);
	// digitalpin_mode(PC_2, DIGITALPIN_OUTPUT);
	// digitalpin_mode(PC_3, DIGITALPIN_OUTPUT);

	// hrpwm_init();

	// while(1) {
	// 	delay_some();
	// 	digitalpin_toggle(PC_0);
	// }
	

	// gpio_pin_mode(PC_13, GPIO_OUTPUT);
	// gpio_pin_set(PC_13, 1);

	// encoder_init();
	// i2c_init();
	// struct PCA9555 * expanderOut = pca9555_init(0x20);
	// pca9555_config(expanderOut, 0);
	// struct PCA9555 * expanderIn = pca9555_init(0x24);
	// pca9555_config(expanderIn, 0xFFFF);

	// modbus_init();

	// gpio_pin_set(PC_13, 0);

	// while (1) {
	// 	modbus_set_Ireg(0, encoder_get());
	// 	pca9555_write(expanderOut, modbus_Coil_word(0));
	// 	modbus_set_Ists_word(0, pca9555_read(expanderIn));
	// 	modbus_poll();
	// }

	return 0;
}


