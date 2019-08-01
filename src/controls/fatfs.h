#ifndef __CONTROLS_FATFS_H__
#define __CONTROLS_FATFS_H__

#include <stdbool.h>
#include <stddef.h>

void fatfs_init(void);

void fatfs_deinit(void);

bool fatfs_write(const char * filename, const char * buf);

bool fatfs_read(const char * filename, char * buf, size_t bufSize);

#endif