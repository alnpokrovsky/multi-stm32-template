#if defined(STM32F4)

#include "gpio.h"
#include <stm32f4xx.h>
#include <stm32f4xx_it.h>
#include <stdarg.h>


#define GPIO_MODER(n, mode)		((uint32_t)(mode) << (2 * (n)))
#define GPIO_MODER_MASK(n)		((uint32_t)(0x03) << (2 * (n)))

#define GPIO_PUPDR(n, pupd)		((uint32_t)(pupd) << (2 * (n)))
#define GPIO_PUPDR_MASK(n)		((uint32_t)(0x03) << (2 * (n)))

#define GPIO_OSPEEDR(n, speed)	((uint32_t)(speed) << (2 * (n)))
#define GPIO_OSPEEDR_MASK(n)	((uint32_t)(0x03)  << (2 * (n)))
#define GPIO_OSPEED_100MHZ	    0x3

#define GPIO_AFR(n, af)			((uint64_t)(af)  << ((n) * 4))
#define GPIO_AFR_MASK(n)		((uint64_t)(0xf) << ((n) * 4))


typedef struct {
    GPIO_TypeDef* base;
    uint32_t rcc;
} GPIO_Descr;

static const GPIO_Descr GPIOS[] = {
    { GPIOA, RCC_AHB1ENR_GPIOAEN },
    { GPIOB, RCC_AHB1ENR_GPIOBEN },
    { GPIOC, RCC_AHB1ENR_GPIOCEN },
    { GPIOD, RCC_AHB1ENR_GPIODEN },
    { GPIOE, RCC_AHB1ENR_GPIOEEN },
    { GPIOF, RCC_AHB1ENR_GPIOFEN },
    { GPIOG, RCC_AHB1ENR_GPIOGEN },
    { GPIOH, RCC_AHB1ENR_GPIOHEN },
    { GPIOI, RCC_AHB1ENR_GPIOIEN },
};

void gpio_init(const GPIO_Pins * gpio) {
    /* rcc on */
    RCC->AHB1ENR |= GPIOS[gpio->port].rcc;

	/*
	 * We want to set the config only for the pins mentioned in gpios,
	 * but keeping the others, so read out the actual config first.
	 */
	uint32_t moder = GPIOS[gpio->port].base->MODER;
	uint32_t pupdr = GPIOS[gpio->port].base->PUPDR;
    uint32_t ospeedr = GPIOS[gpio->port].base->OSPEEDR;
    uint64_t afr = *(uint64_t *)GPIOS[gpio->port].base->AFR;

	for (uint8_t pinn = 0; pinn < 16; ++pinn) {
		if ((gpio->pins & (1 << pinn)) != 0) {
            moder &= ~GPIO_MODER_MASK(pinn);
            moder |= GPIO_MODER(pinn, gpio->mode);
            pupdr &= ~GPIO_PUPDR_MASK(pinn);
            pupdr |= GPIO_PUPDR(pinn, gpio->pupd);
            ospeedr &= ~GPIO_OSPEEDR_MASK(pinn);
            ospeedr |= GPIO_OSPEEDR(pinn, GPIO_OSPEED_100MHZ); /* always max speed */
            afr &= ~GPIO_AFR_MASK(pinn);
            afr |= GPIO_AFR(pinn, gpio->af);
        }
	}

	/* Set mode and pull up/down control registers. */
	GPIOS[gpio->port].base->MODER = moder;
	GPIOS[gpio->port].base->PUPDR = pupdr;
    GPIOS[gpio->port].base->OSPEEDR = ospeedr;
    *(uint64_t *)GPIOS[gpio->port].base->AFR = afr;
    GPIOS[gpio->port].base->OTYPER &= ~(gpio->pins); /* always push-pull */
}

void gpio_set(const GPIO_Pins * gpio, uint16_t val) {
    GPIOS[gpio->port].base->ODR |= val & gpio->pins;
}

uint16_t gpio_get(const GPIO_Pins * gpio) {
    return GPIOS[gpio->port].base->IDR & gpio->pins;
}


#endif
