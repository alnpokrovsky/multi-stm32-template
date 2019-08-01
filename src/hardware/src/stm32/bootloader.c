
#include "bootloader.h"
#include <stdint.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include "backup.h"

static const uint32_t CMD_BOOT = 0x544F4F42UL;
static const uint32_t CMD_APP = 0x3f82722aUL;

void bootloader_manifest_app(void) {
    backup_write(BKP0, CMD_APP);
    bootloader_reboot();
}

void bootloader_reboot(void) {
    scb_reset_system();
}

bool bootloader_force_app(void) {
    if (backup_read(BKP0) == CMD_APP) {
        backup_write(BKP0, 0);
        return true;        
    }
    return false;
}

bool bootloader_force_boot(void) {
    bool force = true;
    /* Check the RTC backup register */
    uint32_t cmd = backup_read(BKP0);
    if (cmd == CMD_BOOT) {
        // asked to go into bootloader?
        backup_write(BKP0, 0);
        return true;
    }
    if (cmd == CMD_APP) {        
        // we were told to reset into app
        backup_write(BKP0, 0);
        return false;
    }

    // a reset now should go into app
    backup_write(BKP0, CMD_APP);

    return force;
}