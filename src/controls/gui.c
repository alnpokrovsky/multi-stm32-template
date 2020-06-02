#ifdef LVGL_LIB

#include "gui.h"
#include <lvgl.h>
#include "controls/ssd1306.h"

static SSD1306 oled = {.spi = SPI_1, .cs = PA_0, .dc = PA_1};

static void my_disp_flush(struct _disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    for(int16_t y = area->y1; y <= area->y2; y++) {
        for(int16_t x = area->x1; x <= area->x2; x++) {
            uint8_t br = lv_color_brightness(*color_p) > 128 ? 1 : 0;
            ssd1306_draw_point(&oled, x, y, br);  /* Put a pixel to the display.*/
            color_p++;
        }
    }
    ssd1306_flush(&oled);
    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

void gui_init(void) {
    /* init hardware */
    ssd1306_init(&oled);

    /* lv initiation */
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf[LV_HOR_RES_MAX*8];
    lv_init();
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX*8); /*Initialize the display buffer*/
    lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
    disp_drv.buffer = &disp_buf;          /*Assign the buffer to the display*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/
}

void gui_poll(uint8_t ms) {
    lv_tick_inc(ms);
    lv_task_handler();
}

#endif
