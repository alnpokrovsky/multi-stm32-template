#include "rcc.h"
#include "digitalpin.h"
#include "uart.h"
// #include "controls/milua.h"
#include "controls/isdriver.h"
#include "controls/rtos.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>


// static void vLed1Task(void * arg) {
//     (void)arg;
//     digitalpin_mode(PG_13, DIGITALPIN_OUTPUT);
//     RTOS_DELAY_LOOP {
//         digitalpin_toggle(PG_13);
//         RTOS_DELAY_NEXT_MS( 500 );
//     }
// }

// static void vLed2Task(void * arg) {
//     (void)arg;
//     digitalpin_mode(PG_14, DIGITALPIN_OUTPUT);
//     RTOS_DELAY_LOOP {
//         digitalpin_toggle(PG_14);
//         RTOS_DELAY_NEXT_MS( 1000 );
//     }
// }

// // static void vUartTask(void * arg) {
// //     (void) arg;
// //     uart_init(UART_2, 115200, 8, PAR_NONE);
// //     RTOS_DELAY_LOOP {
// //         uart_send(UART_2, 'a');
// //         RTOS_DELAY_NEXT_MS( 1000 );
// //     }
// // }

// #include <stdlib.h> 
// static void vSyncTask(void * arg) {
//     (void) arg;
//     // digitalpin_mode(PD_14, DIGITALPIN_OUTPUT);
//     isdriver_sync_write(CANBUS_1, SYNC_RESET_TIME, 0);

//     RTOS_DELAY_LOOP {
//         // uint32_t time = xLastWakeTime + (rand()>>5);
//         uint32_t time = xLastWakeTime;
//         isdriver_sync_write(CANBUS_1, SYNC_CORRECT_TIME, time);
        
//         // digitalpin_toggle(PD_14);        
//         char buf[36];
//         sprintf(buf, "%lu", time);
//         // gui_putString(0, 45, buf);
        
//         RTOS_DELAY_NEXT_MS(3000);
//     }
// }

// static void vDriverTask(void * arg) {
//     (void) arg;

//     canbus_init(CANBUS_1);
    
//     ISDRIVER_Driver driver1 = {.port = CANBUS_1, .id = 0x01};
//     ISDRIVER_Driver driver2 = {.port = CANBUS_1, .id = 0x02};

// // RESTART_DRIVERS_LBL:
//     // gui_clear();
//     // gui_putString(36,0, "dr1");
//     // gui_putString(80,0, "dr2");
//     // gui_putString(0, 15, "init");
//     // gui_putString(36, 15, "w");
//     // gui_putString(80, 15, "w");
//     // gui_poll();

//     while (!isdriver_init(&driver1)) RTOS_DELAY_MS(1000);
//     // gui_putString(36, 15, "ok");
//     while (!isdriver_init(&driver2)) RTOS_DELAY_MS(1000);
//     // gui_putString(80, 15, "ok");

//     // gui_putString(0, 30, "send");
//     RTOS_TASK_CREATE(RTOS_MEDIUM_PRIORITY+1, vSyncTask);
//     // RTOS_Handle syncHandle = RTOS_TASK_CREATE(RTOS_MEDIUM_PRIORITY+1, vSyncTask);

//     while(1) {
//         isdriver_sync_write(CANBUS_1, SYNC_MOTION_OFF, 0); // stop all

//         // set task for next cycle
//         static const int repeats = 5;
//         static const int step = 6;
//         // UG_FillFrame(0, 30, SSD1306_WIDTH, 40, C_BLACK); // clear cicles area
//         for (int i = step; i<=repeats*step; i+=step) {
//             isdriver_setPos(&driver1, 50000);
//             isdriver_setPos(&driver2, 50000);
//             // UG_DrawCircle(36+i, 35, 3, C_WHITE); if (isdriver_setPos(&driver1, 50000)) UG_FillCircle(36+i, 35, 3, C_WHITE);
//             // UG_DrawCircle(80+i, 35, 3, C_WHITE); if (isdriver_setPos(&driver2, 50000)) UG_FillCircle(80+i, 35, 3, C_WHITE);
//         }

//         // check if 
//         // if ( (driver1.errCntSoft >= repeats) || (driver2.errCntSoft >= repeats) ) {
//         //     RTOS_TASK_DELETE(syncHandle);
//         //     goto RESTART_DRIVERS_LBL;
//         // }

//         isdriver_sync_write(CANBUS_1, SYNC_MOTION_ON, 0); // start all
//         RTOS_DELAY_MS( 20000 );
//     }
// }

// static void vGraphicsTask(void * arg) {
//     (void) arg;

//     RTOS_DELAY_LOOP {
//         // gui_poll();
//         RTOS_DELAY_NEXT_MS(1000);
//     }
// }

/*
 * Checkerboard pattern.  Odd squares are transparent; even squares are
 * all different colors.
 */

static void draw_layer_1(const LCD_Layer * l)
{
	const int cel_count = (l->width >> 5) + (l->height >> 5);
	for (int row = 0; row < l->height; row++) {
		for (int col = 0; col < l->width; col++) {
			// size_t i = row * l->width + col;
			uint32_t cel = (row >> 5) + (col >> 5);
			uint8_t a = cel & 1 ? 0 : 0xFF;
			uint8_t r = row * 0xFF / l->height;
			uint8_t g = col * 0xFF / l->width;
			uint8_t b = 0xFF * (cel_count - cel - 1) / cel_count;
			if (!(cel & 3)) {
				b = 0;
			}

			/* Put black and white borders around the squares. */
			if (row % 32 == 0 || col % 32 == 0) {
				r = g = b = a ? 0xFF : 0;
				a = 0xFF;
			}
			uint32_t pix = a << 24 | r << 16 | g << 8 | b << 0;

			/*
			 * Outline the screen in white.  Put a black
			 * dot at the origin.
			 *
			 * (The origin is in the lower left!)
			 */
			if (row == 0 || col == 0 || row == 319 || col == 239) {
				pix = 0xFFFFFFFF;
			} else if (row < 20 && col < 20) {
				pix = 0xFF000000;
			}
			// FRAMEBUFFER_1[i] = pix;
			lcd_setPixel(l, col, row, pix);
		}
	}
}

/*
 * Layer 2 holds the sprite.  The sprite is a semitransparent
 * magenta/cyan diamond outlined in black.
 */

static void draw_layer_2(const LCD_Layer * l)
{
	const uint8_t hw = l->width / 2;
	const uint8_t hh = l->height / 2;
	const uint8_t sz = (hw + hh) / 2;

	for (int row = 0; row < l->height; row++) {
		for (int col = 0; col < l->width; col++) {
			// size_t i = row * l->width + col;
			uint8_t dx = abs(col  - hw);
			uint8_t dy = abs(row  - hh);
			uint8_t dxy = dx + dy;
			uint8_t a = dxy <= sz ? 0xF * dxy / (sz / 2) : 0x0;
			if (a > 0xF) {
				if (a < 0x14) {
					a = 0xF;
				} else {
					a &= 0xF;
				}
			}
			uint8_t r = dx >= dy ? 0xF : 0x0;
			uint8_t g = dy >= dx ? 0xF : 0x0;
			uint8_t b = 0xF;
			if (dx + dy >= sz - 2 || dx == dy) {
				r = g = b = 0;
			}
			uint16_t pix = a << 12 | r << 8 | g << 4 | b << 0;
			// FRAMEBUFFER_2[i] = pix;
			lcd_setPixel(l, col, row, pix);
		}
	}
}

#include "controls/stmpe811.h"
#include "delay.h"

int main(void) {
    rcc_init();

    static LCD_Layer l1 = {1, ARGB8888, 0, 0, LCD_WIDTH, LCD_HEIGHT, 0xff};
    static LCD_Layer l2 = {2, ARGB4444, 0, 0, 128, 128, 0xff};
    lcd_init(&l1, &l2);
    draw_layer_1(&l1);
    draw_layer_2(&l2);
    stmpe811_init();
    // milua_init();
    // gui_init();

    // RTOS_TASK_CREATE(RTOS_LOW_PRIORITY, vLed1Task);
    // RTOS_TASK_CREATE(RTOS_LOW_PRIORITY+1, vGraphicsTask);

    // RTOS_TASK_CREATE(RTOS_MEDIUM_PRIORITY, vDriverTask);

    // RTOS_TASK_CREATE(RTOS_HIGH_PRIORITY+5, vLed2Task);

    // RTOS_START();

    while (1) {
        STMPE811_Point p = stmpe811_read();
        if (p.z > 0) {
            l2.x = p.x - l2.width/2;
            l2.y = p.y - l2.height/2;
            lcd_setLayer(&l2);   
        }
        delay_ms(1);
    }
}

