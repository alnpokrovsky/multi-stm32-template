#if defined(STM32F4)

#include "dma2d.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma2d.h>
#include <libopencm3/cm3/nvic.h>
#include <stdlib.h>

#define DMA2D_WAIT { while(DMA2D_CR & DMA2D_CR_START); }


typedef struct
{
    uint32_t pixFormat;
    uint32_t pixSize;
} colormodel_descript;


static const colormodel_descript COLOR_MODELS[] = {
    { DMA2D_OPFCCR_CM_ARGB8888, sizeof(uint32_t) },
    { DMA2D_OPFCCR_CM_ARGB4444, sizeof(uint16_t) },
};

static void dma2d_transfer(uint32_t mode,
    void * dest, uint16_t dest_width,
    const void *src1, const void *src2, 
    const DMA2D_Rect * r
) {
    DMA2D_WAIT;

    uint32_t height = r->y2 - r->y1 + 1;
    uint32_t width  = r->x2 - r->x1 + 1;

    /* reset DMA2D */
	RCC_AHB1RSTR |= RCC_AHB1RSTR_DMA2DRST;
	RCC_AHB1RSTR &= ~RCC_AHB1RSTR_DMA2DRST;

	/* Configures the color mode of the output image */
    DMA2D_CR &= 0xFFFCE0FC;
	DMA2D_CR |= mode << DMA2D_CR_MODE_SHIFT;
	/* Configures the color mode of the output image */
	DMA2D_OPFCCR &= ~(DMA2D_OPFCCR_CM_MASK << DMA2D_OPFCCR_CM_SHIFT);
	DMA2D_OPFCCR |= COLOR_MODELS[r->cm].pixFormat << DMA2D_OPFCCR_CM_SHIFT;

	/* Configures the output memory address */
    uint32_t offset = (r->y1 * dest_width + r->x1) * COLOR_MODELS[r->cm].pixSize;
	DMA2D_OMAR = offset + (uint32_t)dest;
	/* Configure the lines Offset */
	DMA2D_OOR &= ~DMA2D_OOR_LO_MASK;
    DMA2D_OOR |= dest_width - width;
    /* Configure the number of lines and pixel per line */
    DMA2D_NLR &= ~(DMA2D_NLR_NL_MASK << DMA2D_NLR_NL_SHIFT);
    DMA2D_NLR |= height << DMA2D_NLR_NL_SHIFT;
    DMA2D_NLR &= ~(DMA2D_NLR_PL_MASK << DMA2D_NLR_PL_SHIFT);
    DMA2D_NLR |= width << DMA2D_NLR_PL_SHIFT;

    /* Configures the output color */
	DMA2D_OCOLR = r->color;
    /* Configure DMA2D source address */
    DMA2D_FGMAR = (uint32_t)src1;
    DMA2D_BGMAR = (uint32_t)src2;

    /* Start transfer */
	DMA2D_CR |= DMA2D_CR_START;
    DMA2D_WAIT;
}


void dma2d_init(void) {
	rcc_periph_clock_enable(RCC_DMA2D);
    /* enabling interrupt */
    // nvic_enable_irq(NVIC_DMA2D_IRQ);
    // DMA2D_CR |= DMA2D_CR_TCIE; // transfer complite
    // DMA2D_CR |= DMA2D_CR_TEIE; // transfer error interrupt
    // DMA2D_CR |= DMA2D_CR_CEIE; // config error interrupt
}

void dma2d_blend(void* dest, uint16_t dest_width,
    const void* src1, const void* src2, const DMA2D_Rect * r
) {
    dma2d_transfer(DMA2D_CR_MODE_M2MWB, dest, dest_width, src1, src2, r);
}

void dma2d_copy(void* dest, uint16_t dest_width,
    const void* src, const DMA2D_Rect * r
) {
    dma2d_transfer(DMA2D_CR_MODE_M2M, dest, dest_width, src, NULL, r);
}

void dma2d_fill(void * dest, uint16_t dest_width,
    const DMA2D_Rect * r
) {
    dma2d_transfer(DMA2D_CR_MODE_R2M, dest, dest_width, NULL, NULL, r);
}

void dma2d_abort(void) {
    DMA2D_CR |= DMA2D_CR_ABORT;
}

void dma2d_isr(void) {
    if (DMA2D_ISR & DMA2D_ISR_TCIF) {
        // on transfer complete
        dma2d_handler();
    } else if ( DMA2D_ISR & (DMA2D_ISR_CEIF | DMA2D_ISR_TEIF) ) {
        // on error
    }
    /* clear flags */
    DMA2D_IFCR |= DMA2D_IFCR_CTCIF;
    DMA2D_IFCR |= DMA2D_IFCR_CTEIF;
    DMA2D_IFCR |= DMA2D_IFCR_CCEIF;
}

static void null_handler(void) {;}
#pragma weak dma2d_handler = null_handler

#endif
