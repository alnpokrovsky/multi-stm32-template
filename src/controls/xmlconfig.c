#if defined(SAXML_LIB) && defined(FATFS_LIB)

#include "xmlconfig.h"
#include "ff.h"


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

#endif