#ifdef LVGL_LIB

#include "gui.h"
#include <lvgl.h>
#include "ltdc.h"
// #include "controls/stmpe811.h"
#include <malloc.h>
#include "sramfunc.h"

// static SSD1306 oled = {.spi = SPI_1, .cs = PA_0, .dc = PA_1};
static LTDC_Layer layer = {1, ARGB8888, 0, 0, LTDC_WIDTH, LTDC_HEIGHT, 0x1f};

static void my_disp_flush(struct _disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    (void) disp;
    (void) area;
    (void) color_p;
    for(int16_t y = area->y1; y <= area->y2; y++) {
        for(int16_t x = area->x1; x <= area->x2; x++) {
            ltdc_setPixel(&layer, x, y, color_p->full);
            color_p++;
        }
    }
    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

static bool my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
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
    ltdc_init(&layer, NULL);
    ltdc_setBackground(0xff0000ff);

    /* lv initiation */
    lv_init();
    /* buffer init */
    static lv_disp_buf_t disp_buf;
    // static lv_color_t buf[LTDC_WIDTH*8];
    lv_color_t * buf = malloc(LTDC_SIZE * sizeof(lv_color_t));
    lv_disp_buf_init(&disp_buf, buf, NULL, LTDC_SIZE); /*Initialize the display buffer*/
    /* display driver init */
    lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
    disp_drv.buffer = &disp_buf;          /*Assign the buffer to the display*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/
    /* touch driver init */
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_input_read;
    lv_indev_drv_register(&indev_drv);
}

void SRAM_FUNC gui_poll(int ms) {
    lv_tick_inc(ms);
}

void ltdc_handler() {
    // lv_tick_inc(100);
    lv_task_handler();
}

#endif
