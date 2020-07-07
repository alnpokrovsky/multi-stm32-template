#ifdef LVGL_LIB

#include "gui.h"
#include <lvgl.h>
#include "ltdc.h"
#include "dma2d.h"
#include "controls/stmpe811.h"
#include <malloc.h>
#include "sramfunc.h"

static LTDC_Layer layer = {1, ARGB4444, 0, 0, LTDC_WIDTH, LTDC_HEIGHT, 0x7F};


static void flush_cb(struct _disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    DMA2D_Rect r = { 
        .cm = layer.cm, 
        .x1 = area->x1,
        .y1 = area->y1,
        .x2 = area->x2,
        .y2 = area->y2,
    };
    dma2d_copy(ltdc_getPixelAddr(&layer, 0, 0), disp->hor_res, color_p, &r);
    
    /* Indicate you are ready with the flushing*/
    lv_disp_flush_ready(disp);
}

static void gpu_fill_cb(
    struct _disp_drv_t * disp_drv,
    lv_color_t * dest_buf,
    lv_coord_t dest_width,
    const lv_area_t * fill_area,
    lv_color_t color
) {
    (void) disp_drv;
    
    DMA2D_Rect r = { 
        .cm = layer.cm, 
        .x1 = fill_area->x1,
        .y1 = fill_area->y1,
        .x2 =  fill_area->x2,
        .y2 = fill_area->y2,
        .color = color.full
    };
    dma2d_fill(dest_buf, dest_width, &r);
}

static void gpu_blend_cb(
    struct _disp_drv_t *disp_drv,
    lv_color_t *dest,
    const lv_color_t *src,
    uint32_t length,
    lv_opa_t opa
) {
    DMA2D_Rect r = { 
        .cm = layer.cm, 
        .x1 = 0,
        .y1 = 0,
        .x2 = disp_drv->hor_res-1,
        .y2 = length / disp_drv->hor_res,
        .color = opa
    };
    dma2d_blend(dest, disp_drv->hor_res, dest, src, &r);
}


static bool read_cb(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    (void) drv;

    static uint16_t touchpad_x = 0;
    static uint16_t touchpad_y = 0;
    // STMPE811_Point p = stmpe811_read();
    // if (p.z) {
    //     touchpad_x = p.x;
    //     touchpad_y = p.y;
    //     data->state = LV_INDEV_STATE_PR;
    // } else {
    //     data->state = LV_INDEV_STATE_REL;
    // }
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;    
    return false;
}

void gui_init(void) {
    /* init hardware */
    // stmpe811_init();

    /* lv initiation */
    lv_init();
    /* buffer init */
    static lv_disp_buf_t disp_buf;
    lv_color_t * buf1 = malloc(LTDC_SIZE * sizeof(lv_color_t));
    // lv_color_t * buf2 = malloc(LTDC_SIZE * sizeof(lv_color_t));    
    lv_disp_buf_init(&disp_buf, buf1, NULL, LTDC_SIZE); /*Initialize the display buffer*/
    /* display driver init */
    lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = flush_cb;    /*Set your driver function*/
    disp_drv.gpu_fill_cb = gpu_fill_cb;
    disp_drv.gpu_blend_cb = gpu_blend_cb;
    disp_drv.buffer = &disp_buf;          /*Assign the buffer to the display*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/
    /* touch driver init */
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = read_cb;
    lv_indev_drv_register(&indev_drv);

    dma2d_init();
    ltdc_init(&layer, NULL);
    ltdc_setBackground(0xffff0000);
}

void SRAM_FUNC gui_poll(int ms) {
    lv_tick_inc(ms);
    lv_task_handler();
}

void SRAM_FUNC ltdc_handler(void) {
    lv_tick_inc(33); // 30Hz
    lv_task_handler();
}

#endif
