//#if defined(SAXML_LIB) && defined(FATFS_LIB)

#include "pouconfig.h"
#include "ff.h"
#include "saxml.h"
#include "controls/pca9555.h"

#define IO_EXPANDERS_MAX 4

static PCA9555_config ioConfigs[IO_EXPANDERS_MAX];
uint8_t pouconfig_ioCnt = 0;

void pouconfig_init(void) {
    ioConfigs[0] = (PCA9555_config) {
        IIC_1, 0x20, 0b0000000011110000
    };
    ++pouconfig_ioCnt;
}

PCA9555_config* pouconfig_get_io(uint8_t expanderNum) {
    return &ioConfigs[expanderNum];
}



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

//#endif