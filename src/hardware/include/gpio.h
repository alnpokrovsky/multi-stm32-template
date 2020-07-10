#ifndef __HARDWARE_DIGITALPIN_H__
#define __HARDWARE_DIGITALPIN_H__

#include <stdint.h>

typedef enum {
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
    GPIO_PORT_H,
    GPIO_PORT_I,
} GPIO_PORT;

#define GPIO_PIN_0   (0x0001U)
#define GPIO_PIN_1   (0x0002U)
#define GPIO_PIN_2   (0x0004U)
#define GPIO_PIN_3   (0x0008U)
#define GPIO_PIN_4   (0x0010U)
#define GPIO_PIN_5   (0x0020U)
#define GPIO_PIN_6   (0x0040U)
#define GPIO_PIN_7   (0x0080U)
#define GPIO_PIN_8   (0x0100U)
#define GPIO_PIN_9   (0x0200U)
#define GPIO_PIN_10  (0x0400U)
#define GPIO_PIN_11  (0x0800U)
#define GPIO_PIN_12  (0x1000U)
#define GPIO_PIN_13  (0x2000U)
#define GPIO_PIN_14  (0x4000U)
#define GPIO_PIN_15  (0x8000U)
#define GPIO_PINS_ON  (0xffffU)
#define GPIO_PINS_OFF (0x0000U)

typedef enum {
    GPIO_PUPD_NONE =		0x0,
    GPIO_PUPD_PULLUP =		0x1,
    GPIO_PUPD_PULLDOWN =	0x2,
} GPIO_PUPD;

typedef enum {
    GPIO_MODE_INPUT =		0x0,
    GPIO_MODE_OUTPUT =		0x1,
    GPIO_MODE_AF =			0x2,
    GPIO_MODE_ANALOG =		0x3,
} GPIO_MODE;

typedef struct {
    GPIO_PORT   port;
    uint16_t    pins;
    GPIO_PUPD   pupd;
    GPIO_MODE   mode;
    uint8_t     af;
} GPIO_Pins;


void gpio_init(const GPIO_Pins * gpio);

void gpio_set(const GPIO_Pins * gpio, uint16_t val);

uint16_t gpio_get(const GPIO_Pins * gpio);


#endif
