#if defined(USE_MDR1986VE9x)

#include "rcc.h"
#include <MDR32Fx.h>
#include <system_MDR32F9Qx.h>

void rcc_init(void) {
    /* init ticks generator */
    SystemInit();

    //Необходимая пауза для работы Flash-памяти программ
    MDR_EEPROM->CMD |= (3 << 3);

    MDR_RST_CLK->HS_CONTROL = 0x01; // вкл. HSE осцилятора
    while ((MDR_RST_CLK->CLOCK_STATUS & (1 << 2)) == 0x00); // ждем пока HSE выйдет в рабочий режим

    MDR_RST_CLK->PLL_CONTROL = ((1 << 2) | (4 << 8)); //вкл. PLL | коэф. умножения = 5
    while((MDR_RST_CLK->CLOCK_STATUS & 0x02) != 0x02); //ждем когда PLL выйдет в раб. режим

    MDR_RST_CLK->CPU_CLOCK = (2 //источник для CPU_C1
    | (1 << 2) //источник для CPU_C2
    | (0 << 4) //предделитель для CPU_C3
    | (1 << 8));//источник для HCLK
    MDR_BKP->REG_0E |= (7 << 0); //режим встроенного регулятора напряжения DUcc(в зависимости от частоты МК)
    MDR_BKP->REG_0E |= (7 << 3); //выбор доп.стабилизирующей нагрузки
}

#endif