#ifndef __HARDWARE_DMA_H__
#define __HARDWARE_DMA_H__

#include <stdint.h>

typedef enum {
    DMA_CH1,
    DMA_CH2,
    DMA_CH3,
    DMA_CH4,
    DMA_CH5,
    DMA_CH6,
    DMA_CH7,
} DMA_CHANNEL;

typedef enum {
    LOW,
    MEDIUM,
    HIGH,
    VERY_HIGH,
} DMA_PRIORITY;

void dma_priority(DMA_CHANNEL ch, DMA_PRIORITY prior);

void dma_mem2mem_init(DMA_CHANNEL ch, uint32_t from, uint32_t to);

void dma_mem2periph_init(DMA_CHANNEL ch, uint32_t from, uint32_t to);

void dma_periph2mem_init(DMA_CHANNEL ch, uint32_t from, uint32_t to);

void dma_start_once(DMA_CHANNEL ch, uint16_t len);

void dma_start_cyclic(DMA_CHANNEL ch, uint16_t len);

void dma_ch1_complete_handler(void);
void dma_ch2_complete_handler(void);
void dma_ch3_complete_handler(void);
void dma_ch4_complete_handler(void);
void dma_ch5_complete_handler(void);
void dma_ch6_complete_handler(void);
void dma_ch7_complete_handler(void);

#endif