#if defined(STM32F7)

#include "pwr.h"
#include <stm32f7xx.h>
#include <stm32f7xx_it.h>

void pwr_init(uint32_t scale)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	PWR->CR1 &= ~PWR_CR1_VOS_Msk;
    PWR->CR1 |= scale << PWR_CR1_VOS_Pos;
}

void pwr_setOverdrive(bool enable)
{
    if (enable) {
        PWR->CR1 |= PWR_CR1_ODEN;
        while (!(PWR->CSR1 & PWR_CSR1_ODRDY));
        PWR->CR1 |= PWR_CR1_ODSWEN;
        while (!(PWR->CSR1 & PWR_CSR1_ODSWRDY));
    } else {
        PWR->CR1 &= ~(PWR_CR1_ODEN | PWR_CR1_ODSWEN);
	    while (!(PWR->CSR1 & PWR_CSR1_ODSWRDY));
    }
}

#endif
