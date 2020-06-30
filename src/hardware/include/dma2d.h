#ifndef __HARDWARE_DMA2D_H__
#define __HARDWARE_DMA2D_H__

#include "ltdc.h"

typedef enum {
    DMA2D_FILL_COLOR,
    DMA2D_COPY_COLOR,
} DMA2D_MODE;

typedef struct
{
    LTDC_COLOR_MODEL cm;    
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
    uint32_t color;
} DMA2D_Rect;

void dma2d_init(void);

void dma2d_fill(
    void * dest, uint16_t dest_width, 
    const DMA2D_Rect * r);

void dma2d_copy(void* dest, uint16_t dest_width, 
    const void* src, const DMA2D_Rect * r);

void dma2d_blend(void* dest, uint16_t dest_width, 
    const void* src1, const void* src2, const DMA2D_Rect * r);


void dma2d_abort(void);


void dma2d_handler(void);

#endif
