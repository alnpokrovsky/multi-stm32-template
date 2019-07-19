#include "rcc.h"
#include "digitalpin.h"
#include "usb.h"
#include "delay.h"
#include "ff.h"

//#define LED PC_2
#define LED PC_13

int main(void) {
    rcc_init();

    digitalpin_mode(LED, DIGITALPIN_OUTPUT);
    // digitalpin_toggle(LED);
    
    // // mount the default drive
    // FATFS fs;
    // FRESULT res;
    // f_mount(&fs, "", 0);

    // FIL fin;
    // res = f_open(&fin, "board.cnf", FA_READ);
    // if (res != FR_OK) {
        
    // }
    // // write file
    // res = f_open(&logFile, "log.txt", FA_OPEN_APPEND | FA_WRITE);
    // if ( (res==13) ) {
    //     // digitalpin_toggle(LED);
    // }
    
    // unsigned int bytesWritten;
    // char writeBuff[] = "Hello, world!";
    // f_write(&logFile, writeBuff, sizeof(writeBuff), &bytesWritten);
    // f_close(&logFile);
    // // Unmount
    // f_mount(0, "", 0);

    usb_init("POU");

    while (1) {
        digitalpin_toggle(LED);
        delay_some();
        delay_some();
        delay_some();
        delay_some();
        delay_some();
    }
}
