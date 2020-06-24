#ifndef _CONTROLS_ISDRIVER_H_
#define _CONTROLS_ISDRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "data32.h"
#include "canbus.h"

typedef struct {
    CANBUS_PORT port;
    uint8_t id;
    uint8_t errCntHard;
    uint16_t errCntSoft;
} ISDRIVER_Driver;

typedef enum {
    SYNC_RESET_TIME = 0x00,     ///< сброс времени
    SYNC_SET_TIME = 0x01,       ///< зафиксировать текущее системное время привода
    SYNC_CORRECT_TIME = 0x02,   ///< скорректировать время в мс 
    SYNC_MOTION_ON = 0x03,      ///< синхронно начать движение
    SYNC_MOTION_OFF = 0x04,     ///< синхронно остановить движение
} ISDRIVER_SYNC_COMMAND;

bool isdriver_init(ISDRIVER_Driver * driver);
bool isdriver_setPos(ISDRIVER_Driver * driver, int32_t pos);
int32_t isdriver_getPos(ISDRIVER_Driver * driver);

void isdriver_sync_write(CANBUS_PORT port, ISDRIVER_SYNC_COMMAND cmd, uint32_t param);


#endif
