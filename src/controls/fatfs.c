// #if defined(FATFS_LIB)

#include "fatfs.h"
#include "ff.h"
#include <string.h>

static FATFS fs;


void fatfs_init(void) {
    // mount the default drive
    f_mount(&fs, "", 1);
}

void fatfs_deinit(void) {
    // Unmount
    f_mount(0, "", 0);
}

bool fatfs_write(const char * filename, const char * buf) {
    FIL fout;
    FRESULT res = f_open(&fout, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        return false;
    }
    unsigned int bytesWritten;
    f_write(&fout, buf, strlen(buf), &bytesWritten);
    f_close(&fout);
    return true;
}

bool fatfs_read(const char * filename, char * buf, size_t bufSize) {
    FIL fin;
    FRESULT res = f_open(&fin, filename, FA_OPEN_EXISTING | FA_READ);
    if (res != FR_OK) { // if file doesnt exists
        return false;
    }

    unsigned int bytesRead;
    f_read(&fin, buf, bufSize, &bytesRead);
    f_close(&fin);
    return true;
}
