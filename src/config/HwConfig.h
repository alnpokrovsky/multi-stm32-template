#ifndef CONFIG_HWCONFIG_H_
#define CONFIG_HWCONFIG_H_

#include <libopencm3_cpp_extensions/gpio_ext.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/f4/nvic.h>

namespace hw {

namespace config {

	static constexpr GPIO_CPP_Extension::Pinout LED_RED 			= PA0;
	static constexpr GPIO_CPP_Extension::Pinout LED_GREEN 			= PA1;
	static constexpr GPIO_CPP_Extension::Pinout LED_BLUE 			= PA2;

	static constexpr GPIO_CPP_Extension::Pinout USB_HOST_P 			= PB15;
	static constexpr GPIO_CPP_Extension::Pinout USB_HOST_M 			= PB14;
	static constexpr GPIO_CPP_Extension::AF_Number USB_HOST_AF 		= GPIO_CPP_Extension::AF_Number::AF12;
	static constexpr uint8_t  USB_HOST_TIMER_NUMBER     			= 6;
	static constexpr uint16_t USB_HOST_TIMER_PRESCALER  			= (8400 - 1);
	static constexpr uint16_t USB_HOST_TIMER_PERIOD     			= (65535);

	static constexpr GPIO_CPP_Extension::Pinout USB_DEVICE_P 		= PA11;
	static constexpr GPIO_CPP_Extension::Pinout USB_DEVICE_M 		= PA12;
	static constexpr GPIO_CPP_Extension::AF_Number USB_DEVICE_AF 	= GPIO_CPP_Extension::AF_Number::AF10;
	static constexpr uint8_t USB_DEVICE_IRQ    						= NVIC_OTG_FS_IRQ;
	static constexpr uint8_t USB_DEVICE_IRQ_PRIORITY    			= 0xB0;

	static constexpr uint8_t SDIO_PIN_COUNT                			= 6;
	static constexpr GPIO_CPP_Extension::AF_Number SDIO_AF 			= GPIO_CPP_Extension::AF_Number::AF12;
	static constexpr GPIO_CPP_Extension::Pinout SDIO_D0    			= PC8;
	static constexpr GPIO_CPP_Extension::Pinout SDIO_D1    			= PC9;
	static constexpr GPIO_CPP_Extension::Pinout SDIO_D2    			= PC10;
	static constexpr GPIO_CPP_Extension::Pinout SDIO_D3    			= PC11;
	static constexpr GPIO_CPP_Extension::Pinout SDIO_CK    			= PC12;
	static constexpr GPIO_CPP_Extension::Pinout SDIO_CM    			= PD2;
	static constexpr GPIO_CPP_Extension::Pinout SD_DET     			= PC7;
	static constexpr uint32_t SDIO_DMA		               			= DMA2;
	static constexpr uint32_t SDIO_DMA_STREAM              			= DMA_STREAM3;
	static constexpr uint32_t SDIO_DMA_CHANNEL             			= DMA_SxCR_CHSEL_4;
	static constexpr uint32_t SDIO_IRQ             		   			= NVIC_SDIO_IRQ;
	static constexpr uint32_t SDIO_DMA_IRQ                  		= NVIC_DMA2_STREAM3_IRQ;
	static constexpr uint32_t SDIO_IRQ_PRIORITY            			= 0x00;
	static constexpr uint32_t SDIO_DMA_IRQ_PRIORITY         		= 0x80;

} /* namespace config */

} /* namespace hw */

#endif /* CONFIG_HWCONFIG_H_ */
