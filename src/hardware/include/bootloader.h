#ifndef __HARDWARE_BOOTLOADER_H__
#define __HARDWARE_BOOTLOADER_H__

#include <stdbool.h>

void bootloader_manifest_app(void);

void bootloader_reboot(void);

bool bootloader_force_app(void);

bool bootloader_force_boot(void);

#endif