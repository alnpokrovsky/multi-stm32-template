#if defined(STM32F7)

#include "dma2d.h"
#include "stm32f7xx.h"
#include "stm32f7xx_it.h"
#include <stdlib.h>

#define DMA2D_WAIT { while(DMA2D->CR & DMA2D_CR_START); }

/* dma2d modes */
#define DMA2D_CR_MODE_M2M		    0	/* memory to memory */
#define DMA2D_CR_MODE_M2MWPFC	    1	/* memory to memory with pix convert */
#define DMA2D_CR_MODE_M2MWB		    2	/* memory to memory with blend */
#define DMA2D_CR_MODE_R2M		    3	/* register to memory */
/* color models */
#define DMA2D_OPFCCR_CM_ARGB8888	0
#define DMA2D_OPFCCR_CM_RGB888		1
#define DMA2D_OPFCCR_CM_RGB565		2
#define DMA2D_OPFCCR_CM_ARGB1555	3
#define DMA2D_OPFCCR_CM_ARGB4444	4
/* blending modes */
#define DMA2D_xPFCCR_AM_NONE		0
#define DMA2D_xPFCCR_AM_FORCE		1
#define DMA2D_xPFCCR_AM_PRODUCT		2

typedef struct
{
    uint32_t pixFormat;
    uint32_t pixSize;
} colormodel_descript;


static const colormodel_descript COLOR_MODELS[] = {
    { DMA2D_OPFCCR_CM_ARGB8888, sizeof(uint32_t) },
    { DMA2D_OPFCCR_CM_ARGB4444, sizeof(uint16_t) },
    { DMA2D_OPFCCR_CM_RGB888,   3 },
    { DMA2D_OPFCCR_CM_RGB565,   sizeof(uint16_t) },
};

static void dma2d_transfer(
    uint32_t mode, uint8_t transform_cm,
    void * dest, uint16_t dest_width,
    const void *bg, const void *fg, 
    const DMA2D_Rect * r
) {
#if __DCACHE_PRESENT
    if(SCB->CCR & (uint32_t)SCB_CCR_DC_Msk) {
        SCB_CleanInvalidateDCache();
    }
#endif

    DMA2D_WAIT;

    uint32_t height = r->y2 - r->y1 + 1;
    uint32_t width  = r->x2 - r->x1 + 1;

    /* reset DMA2D */
	RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2DRST;
	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2DRST;

	/* Configures the color mode of the output image */
    DMA2D->CR &= 0xFFFCE0FC;
	DMA2D->CR |= mode << DMA2D_CR_MODE_Pos;
    /* Configures the color mode of the input images (we think they are same) */
	DMA2D->FGPFCCR &= ~(DMA2D_FGPFCCR_CM_Msk << DMA2D_FGPFCCR_CM_Pos);
    DMA2D->FGPFCCR |= COLOR_MODELS[r->cm].pixFormat << DMA2D_FGPFCCR_CM_Pos;
    DMA2D->BGPFCCR &= ~(DMA2D_BGPFCCR_CM_Msk << DMA2D_BGPFCCR_CM_Pos);
    DMA2D->BGPFCCR |= COLOR_MODELS[r->cm].pixFormat << DMA2D_BGPFCCR_CM_Pos;
    /* Configures the color mode of the output image */
	DMA2D->OPFCCR &= ~(DMA2D_OPFCCR_CM_Msk << DMA2D_OPFCCR_CM_Pos);
	DMA2D->OPFCCR |= COLOR_MODELS[transform_cm].pixFormat << DMA2D_OPFCCR_CM_Pos;
    
	/* Configures the output memory address */
    uint32_t offset = (r->y1 * dest_width + r->x1) * COLOR_MODELS[transform_cm].pixSize;
	DMA2D->OMAR = offset + (uint32_t)dest;
	/* Configure the lines Offset */
	DMA2D->OOR &= ~DMA2D_OOR_LO_Msk;
    DMA2D->OOR |= (dest_width - width);
    /* Configure the number of lines and pixel per line */
    DMA2D->NLR &= ~(DMA2D_NLR_NL_Msk << DMA2D_NLR_NL_Pos);
    DMA2D->NLR |= height << DMA2D_NLR_NL_Pos;
    DMA2D->NLR &= ~(DMA2D_NLR_PL_Msk << DMA2D_NLR_PL_Pos);
    DMA2D->NLR |= width << DMA2D_NLR_PL_Pos;

    /* Configures the output color */
	DMA2D->OCOLR = r->color;
    /* blending alpha */
    DMA2D->FGPFCCR &= ~(DMA2D_FGPFCCR_AM_Msk << DMA2D_FGPFCCR_AM_Pos);
    DMA2D->FGPFCCR |= DMA2D_xPFCCR_AM_PRODUCT << DMA2D_FGPFCCR_AM_Pos;
    DMA2D->FGPFCCR &= ~(DMA2D_FGPFCCR_ALPHA_Msk << DMA2D_FGPFCCR_ALPHA_Pos);
    DMA2D->FGPFCCR |= (r->color & DMA2D_FGPFCCR_ALPHA_Msk) << DMA2D_FGPFCCR_ALPHA_Pos;
    /* Configure DMA2D source address */
    DMA2D->BGMAR = (uint32_t)bg;
    DMA2D->FGMAR = (uint32_t)fg;

    /* Start transfer */
	DMA2D->CR |= DMA2D_CR_START;
    DMA2D_WAIT;
}


void dma2d_init(void) {
	/* enable rcc */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;
    /* enabling interrupt */
    // nvic_enable_irq(NVIC_DMA2D_IRQ);
    // DMA2D_CR |= DMA2D_CR_TCIE; // transfer complite
    // DMA2D_CR |= DMA2D_CR_TEIE; // transfer error interrupt
    // DMA2D_CR |= DMA2D_CR_CEIE; // config error interrupt
}

void dma2d_blend(void* dest, uint16_t dest_width,
    const void* bg, const void* fg, const DMA2D_Rect * r
) {
    dma2d_transfer(
        DMA2D_CR_MODE_M2MWB, r->cm,
        dest, dest_width,
        bg, fg, r);
}

void dma2d_copy(void* dest, uint16_t dest_width,
    const void* src, const DMA2D_Rect * r
) {
    dma2d_transfer(
        DMA2D_CR_MODE_M2M, r->cm,
        dest, dest_width,
        NULL, src, r);
}

void dma2d_copyTransform(uint8_t dest_cm, void* dest, uint16_t dest_width,
    const void* src, const DMA2D_Rect * r
) {
    dma2d_transfer(
        DMA2D_CR_MODE_M2M, dest_cm,
        dest, dest_width,
        NULL, src, r);
}

void dma2d_fill(void * dest, uint16_t dest_width,
    const DMA2D_Rect * r
) {
    dma2d_transfer(
        DMA2D_CR_MODE_R2M, r->cm,
        dest, dest_width,
        NULL, NULL, r);
}

void dma2d_abort(void) {
    DMA2D->CR |= DMA2D_CR_ABORT;
}


void DMA2D_IRQHandler(void) {
    if (DMA2D->ISR & DMA2D_ISR_TCIF) {
        // on transfer complete
        dma2d_handler();
    } else if ( DMA2D->ISR & (DMA2D_ISR_CEIF | DMA2D_ISR_TEIF) ) {
        // on error
    }
    /* clear flags */
    DMA2D->IFCR |= DMA2D_IFCR_CTCIF;
    DMA2D->IFCR |= DMA2D_IFCR_CTEIF;
    DMA2D->IFCR |= DMA2D_IFCR_CCEIF;
}

__attribute__((weak))
void dma2d_handler(void) {;}

#endif
