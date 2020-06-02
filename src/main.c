#include "rcc.h"
#include "digitalpin.h"
#include "uart.h"
#include "gui.h"
#include "controls/isdriver.h"
#include "controls/rtos.h"
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>


static void vLed1Task(void * arg) {
    (void)arg;
    digitalpin_mode(PD_13, DIGITALPIN_OUTPUT);
    RTOS_DELAY_LOOP {
        digitalpin_toggle(PD_13);
        RTOS_DELAY_NEXT_MS( 500 );
    }
}

static void vLed2Task(void * arg) {
    (void)arg;
    digitalpin_mode(PD_12, DIGITALPIN_OUTPUT);
    RTOS_DELAY_LOOP {
        digitalpin_toggle(PD_12);
        RTOS_DELAY_NEXT_MS( 1000 );
    }
}

// static void vUartTask(void * arg) {
//     (void) arg;
//     uart_init(UART_2, 115200, 8, PAR_NONE);
//     RTOS_DELAY_LOOP {
//         uart_send(UART_2, 'a');
//         RTOS_DELAY_NEXT_MS( 1000 );
//     }
// }

static void vSyncTask(void * arg) {
    (void) arg;
    digitalpin_mode(PD_14, DIGITALPIN_OUTPUT);
    isdriver_sync_init(CANBUS_1);

    RTOS_DELAY_LOOP {
        // isdriver_sync_write(CANBUS_1, SYNC_SET_TIME, 0);
        isdriver_sync_write(CANBUS_1, SYNC_CORRECT_TIME, xLastWakeTime);
        
        digitalpin_toggle(PD_14);        
        char buf[36];
        sprintf(buf, "%lu", xLastWakeTime);
        gui_putString(0, 45, buf);
        
        RTOS_DELAY_NEXT_MS(2000);
    }
}

static void vDriverTask(void * arg) {
    (void) arg;

    canbus_init(CANBUS_1);
    
    ISDRIVER_Driver driver1 = {.port = CANBUS_1, .id = 0x01};
    ISDRIVER_Driver driver2 = {.port = CANBUS_1, .id = 0x02};
    
    gui_putString(36,0, "dr1");
    gui_putString(80,0, "dr2");
    gui_putString(0, 15, "init");
    gui_putString(36, 15, "w");
    gui_putString(80, 15, "w");
    gui_poll();

    while (!isdriver_init(&driver1)) RTOS_DELAY_MS(10000);
    gui_putString(36, 15, "ok");
    while (!isdriver_init(&driver2)) RTOS_DELAY_MS(10000);
    gui_putString(80, 15, "ok");

    gui_putString(0, 30, "send");
    RTOS_TASK(RTOS_MEDIUM_PRIORITY+1, vSyncTask);

    while(1) {
        UG_FillFrame(0, 30, SSD1306_WIDTH, 40, C_BLACK); // clear cicles area 
        isdriver_sync_write(CANBUS_1, SYNC_MOTION_OFF, 0);
        static const int step = 6;
        for (int i = step; i<=5*step; i+=step) {
            UG_DrawCircle(36+i, 35, 3, C_WHITE); if (isdriver_setPos(&driver1, 50000)) UG_FillCircle(36+i, 35, 3, C_WHITE);
            UG_DrawCircle(80+i, 35, 3, C_WHITE); if (isdriver_setPos(&driver2, 50000)) UG_FillCircle(80+i, 35, 3, C_WHITE);
        }
        isdriver_sync_write(CANBUS_1, SYNC_MOTION_ON, 0);
        RTOS_DELAY_MS( 20000 );
    }
}

static void vGraphicsTask(void * arg) {
    (void) arg;

    RTOS_DELAY_LOOP {
        gui_poll();
        RTOS_DELAY_NEXT_MS(1000);
    }
}

int main(void) {
    rcc_init();

    gui_init();

    RTOS_TASK(RTOS_LOW_PRIORITY, vLed1Task);
    RTOS_TASK(RTOS_LOW_PRIORITY+1, vGraphicsTask);

    RTOS_TASK(RTOS_MEDIUM_PRIORITY, vDriverTask);

    RTOS_TASK(RTOS_HIGH_PRIORITY+5, vLed2Task);

    RTOS_START();

    while (1);
}


void hard_fault_handler(void) {
    gui_putString(0, 50, "hardfall");
    gui_poll();
}