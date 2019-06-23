#if defined(STM32F1)

#include "dma.h"
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

typedef struct {
	uint32_t dma_ch;
	uint8_t dma_irq;
} Channel_descript;

static const Channel_descript CHANNELS[] = {
	{DMA_CHANNEL1, NVIC_DMA1_CHANNEL1_IRQ},
	{DMA_CHANNEL2, NVIC_DMA1_CHANNEL2_IRQ},
	{DMA_CHANNEL3, NVIC_DMA1_CHANNEL3_IRQ},
	{DMA_CHANNEL4, NVIC_DMA1_CHANNEL4_IRQ},
	{DMA_CHANNEL5, NVIC_DMA1_CHANNEL5_IRQ},
	{DMA_CHANNEL6, NVIC_DMA1_CHANNEL6_IRQ},
	{DMA_CHANNEL7, NVIC_DMA1_CHANNEL7_IRQ},
};

static const uint32_t PRIORITY[] = {
	DMA_CCR_PL_LOW,
	DMA_CCR_PL_MEDIUM,
	DMA_CCR_PL_HIGH,
	DMA_CCR_PL_VERY_HIGH,
};

static void null_handler(void)
{
	/* Do nothing. */
}

#pragma weak dma_ch1_complete_handler = null_handler
#pragma weak dma_ch2_complete_handler = null_handler
#pragma weak dma_ch3_complete_handler = null_handler
#pragma weak dma_ch4_complete_handler = null_handler
#pragma weak dma_ch5_complete_handler = null_handler
#pragma weak dma_ch6_complete_handler = null_handler
#pragma weak dma_ch7_complete_handler = null_handler


void dma_priority(DMA_CHANNEL ch, DMA_PRIORITY prior) {
	dma_set_priority(DMA1, CHANNELS[ch].dma_ch, PRIORITY[prior]);
}

void dma_mem2mem_init(DMA_CHANNEL ch, uint32_t from, uint32_t to) {
    rcc_periph_clock_enable(RCC_DMA1);
	dma_channel_reset(DMA1, CHANNELS[ch].dma_ch);

	/* MEM2MEM mode for channel */
	dma_enable_mem2mem_mode(DMA1, CHANNELS[ch].dma_ch);

	/* 8Bit wide transfer for source and destination. */
	dma_set_peripheral_size(DMA1, CHANNELS[ch].dma_ch, DMA_CCR_MSIZE_8BIT);
	dma_set_memory_size(DMA1, CHANNELS[ch].dma_ch, DMA_CCR_MSIZE_8BIT);

	/* After every 8bits we have to increase the address */
	dma_enable_peripheral_increment_mode(DMA1, CHANNELS[ch].dma_ch);
	dma_enable_memory_increment_mode(DMA1, CHANNELS[ch].dma_ch);

	/* We define the source as peripheral. */
	dma_set_read_from_peripheral(DMA1, CHANNELS[ch].dma_ch);

	/* We want to transfer from. */
	dma_set_peripheral_address(DMA1, CHANNELS[ch].dma_ch, from);
	/* Destination should be to. */
	dma_set_memory_address(DMA1, CHANNELS[ch].dma_ch, to);
}

void dma_mem2periph_init(DMA_CHANNEL ch, uint32_t from, uint32_t to) {
	rcc_periph_clock_enable(RCC_DMA1);

	dma_disable_channel(DMA1, CHANNELS[ch].dma_ch);
	dma_channel_reset(DMA1, CHANNELS[ch].dma_ch);

	/* transfer from memory to periph */
	dma_set_read_from_memory(DMA1, CHANNELS[ch].dma_ch);

	/* 16Bit wide transfer for source and destination. */
    dma_set_memory_size(DMA1, CHANNELS[ch].dma_ch, DMA_CCR_MSIZE_16BIT);
	dma_set_peripheral_size(DMA1, CHANNELS[ch].dma_ch, DMA_CCR_MSIZE_32BIT);
	
	/* inc memory pointer */
    dma_enable_memory_increment_mode(DMA1, CHANNELS[ch].dma_ch);
	dma_disable_peripheral_increment_mode(DMA1, CHANNELS[ch].dma_ch);
    
	/* The array pointer memory would be get from */
	dma_set_memory_address(DMA1, CHANNELS[ch].dma_ch, from);
    /* Setting target periph */
	dma_set_peripheral_address(DMA1, CHANNELS[ch].dma_ch, to);
}

void dma_periph2mem_init(DMA_CHANNEL ch, uint32_t from, uint32_t to) {
	rcc_periph_clock_enable(RCC_DMA1);
	
	dma_disable_channel(DMA1, CHANNELS[ch].dma_ch);
	dma_channel_reset(DMA1, CHANNELS[ch].dma_ch);

	/* transfer from memory to periph */
	dma_set_read_from_peripheral(DMA1, CHANNELS[ch].dma_ch);

	/* 16Bit wide transfer for source and destination. */
    dma_set_memory_size(DMA1, CHANNELS[ch].dma_ch, DMA_CCR_MSIZE_16BIT);
	dma_set_peripheral_size(DMA1, CHANNELS[ch].dma_ch, DMA_CCR_MSIZE_16BIT);
	
	/* inc memory pointer */
    dma_enable_memory_increment_mode(DMA1, CHANNELS[ch].dma_ch);
	dma_disable_peripheral_increment_mode(DMA1, CHANNELS[ch].dma_ch);
    
	/* The array pointer memory */
	dma_set_peripheral_address(DMA1, CHANNELS[ch].dma_ch, to);
    /* Setting target periph */
	dma_set_memory_address(DMA1, CHANNELS[ch].dma_ch, from);
}

void dma_start_once(DMA_CHANNEL ch, uint16_t len) {
	/* set memory len */
	dma_set_number_of_data(DMA1, CHANNELS[ch].dma_ch, len);
	/* stop channel on complete irq */
	dma_enable_transfer_complete_interrupt(DMA1, CHANNELS[ch].dma_ch);
	nvic_enable_irq(CHANNELS[ch].dma_irq);
	/* Start DMA transfer. */
	dma_enable_channel(DMA1, CHANNELS[ch].dma_ch);
}

void dma_start_cyclic(DMA_CHANNEL ch, uint16_t len) {
	/* set memory len */
	dma_set_number_of_data(DMA1, CHANNELS[ch].dma_ch, len);
	/* cyclic reload */
	dma_enable_circular_mode(DMA1, CHANNELS[ch].dma_ch);
	/* Start DMA transfer. */
	dma_enable_channel(DMA1, CHANNELS[ch].dma_ch);
}

void dma1_channel1_isr(void)
{
	if ((DMA1_ISR & DMA_ISR_TCIF(DMA_CHANNEL1)) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF(DMA_CHANNEL1);
		dma_disable_channel(DMA1, DMA_CHANNEL1);
		dma_ch1_complete_handler();
	}
}

void dma1_channel2_isr(void)
{
	if ((DMA1_ISR & DMA_ISR_TCIF(DMA_CHANNEL2)) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF(DMA_CHANNEL2);
		dma_disable_channel(DMA1, DMA_CHANNEL2);
		dma_ch2_complete_handler();
	}
}

void dma1_channel3_isr(void)
{
	if ((DMA1_ISR & DMA_ISR_TCIF(DMA_CHANNEL3)) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF(DMA_CHANNEL3);
		dma_disable_channel(DMA1, DMA_CHANNEL3);
		dma_ch3_complete_handler();
	}
}

void dma1_channel4_isr(void)
{
	if ((DMA1_ISR & DMA_ISR_TCIF(DMA_CHANNEL4)) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF(DMA_CHANNEL4);
		dma_disable_channel(DMA1, DMA_CHANNEL4);
		dma_ch4_complete_handler();
	}
}

void dma1_channel5_isr(void)
{
	if ((DMA1_ISR & DMA_ISR_TCIF(DMA_CHANNEL5)) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF(DMA_CHANNEL5);
		dma_disable_channel(DMA1, DMA_CHANNEL5);
		dma_ch5_complete_handler();
	}
}

void dma1_channel6_isr(void)
{
	if ((DMA1_ISR & DMA_ISR_TCIF(DMA_CHANNEL6)) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF(DMA_CHANNEL6);
		dma_disable_channel(DMA1, DMA_CHANNEL6);
		dma_ch6_complete_handler();
	}
}

void dma1_channel7_isr(void)
{
	if ((DMA1_ISR & DMA_ISR_TCIF(DMA_CHANNEL7)) != 0) {
		DMA1_IFCR |= DMA_IFCR_CTCIF(DMA_CHANNEL7);
		dma_disable_channel(DMA1, DMA_CHANNEL7);
		dma_ch7_complete_handler();
	}
}

#endif