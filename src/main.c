// #include "rcc.h"
// #include "digitalpin.h"
// #include "controls/modbus.h"


// int main(void)
// {
// 	rcc_init();

// 	// digitalpin_mode(PC_0, DIGITALPIN_OUTPUT);
// 	// digitalpin_mode(PC_1, DIGITALPIN_OUTPUT);
// 	// digitalpin_mode(PC_2, DIGITALPIN_OUTPUT);
// 	// digitalpin_mode(PC_3, DIGITALPIN_OUTPUT);

// 	// hrpwm_init();

// 	// while(1) {
// 	// 	delay_some();
// 	// 	digitalpin_toggle(PC_0);
// 	// }
	

// 	// gpio_pin_mode(PC_13, GPIO_OUTPUT);
// 	// gpio_pin_set(PC_13, 1);

// 	// encoder_init();
// 	// i2c_init();
// 	// struct PCA9555 * expanderOut = pca9555_init(0x20);
// 	// pca9555_config(expanderOut, 0);
// 	// struct PCA9555 * expanderIn = pca9555_init(0x24);
// 	// pca9555_config(expanderIn, 0xFFFF);

// 	// modbus_init();

// 	// gpio_pin_set(PC_13, 0);

// 	// while (1) {
// 	// 	modbus_set_Ireg(0, encoder_get());
// 	// 	pca9555_write(expanderOut, modbus_Coil_word(0));
// 	// 	modbus_set_Ists_word(0, pca9555_read(expanderIn));
// 	// 	modbus_poll();
// 	// }

// 	return 0;
// }


#include "MDR32Fx.h"  //Подключаем заголовочный файл с регистрами
#include "system_MDR32F9Qx.h" //Подключаем файл с инициализацией периферии
#include "delay.h"

int main(void)
{
    SystemInit();//Системная функция, которая инициализирует тактовый генератор
    
    MDR_RST_CLK->PER_CLOCK |= 1 << RST_CLK_PER_CLOCK_PCLK_EN_PORTC_Pos;

    MDR_PORTC->OE     |= 1 << 2; // output
    MDR_PORTC->ANALOG |= 1 << 2; // digital
    MDR_PORTC->PULL   |= (1 << 2) << PORT_PULL_DOWN_Pos; // pull down
    MDR_PORTC->PULL   |= (1 << 2) << PORT_PULL_UP_Pos;   //pull up
    MDR_PORTC->PWR    |= 0x01 << PORT_PWR2_Pos; // speed slow

    while(1)
    {
        delay_some();
        MDR_PORTC->RXTX |= 1 << 2;
        delay_some();
        MDR_PORTC->RXTX &= ~(1 << 2);
    }
}