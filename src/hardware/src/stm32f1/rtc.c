#if defined(STM32F1)

#include "rtc.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/cm3/nvic.h>

#define RCC_RTC RCC_LSE
#define rtc_alarm_isr rtc_isr
#define NVIC_RTC_ALARM_IRQ NVIC_RTC_IRQ

void rtc_init() {
    rcc_periph_clock_enable(RCC_RTC);
    /*
	 * If the RTC is pre-configured just allow access, don't reconfigure.
	 * Otherwise enable it with the LSE as clock source and 0x7fff as
	 * prescale value.
	 */
	// rtc_auto_awake(RCC_LSE, 0x7fff);

	/* The above mode will not reset the RTC when you press the RST button.
	 * It will also continue to count while the MCU is held in reset. If
	 * you want it to reset, comment out the above and use the following:
	 */
	rtc_awake_from_off(RCC_RTC);
	rtc_set_prescale_val(0x7fff);

    nvic_enable_irq(NVIC_RTC_ALARM_IRQ);
	nvic_set_priority(NVIC_RTC_ALARM_IRQ, 1);
}



void rtc_alarm_isr(void) {
    rtc_handler(rtc_get_counter_val());
    rtc_clear_flag(RTC_SEC);
}

__attribute__((weak))
void rtc_handler(uint32_t cnt) {
    (void)cnt;
}

#endif
