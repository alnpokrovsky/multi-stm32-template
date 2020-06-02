#include "isdriver.h"
#include "canbus.h"
#include "delay.h"


typedef enum {
    DRIVER_TYPE = 0x2A,     ///< тип электрического двигателя
    CIRCUIT_CONF = 0x2B,    ///< конфигурацию замкнутой системы
    POWER_OFF = 0x2C,       ///< выключить питание
    POWER_ON = 0x2D,        ///< включить питание
    MOTION_MODE = 0x00,     ///< режим позиционирования
    MOTION_ON = 0x31,       ///< начать движение
    MOTION_OFF = 0x32,      ///< остановить движение
    SYNC_ON = 0x4F,         ///< включить синхронизацию по CAN
    SYNC_OFF = 0x50,        ///< выключить синхронизацию по CAN
    SYNC_TIME = 0x45,       ///< возврат величины интервала тактирования

    SET_ABS_POS = 0x23,     ///< абсолютный угол поворота движка
    GET_ABS_POS = 0x1E,     ///< абсолютный угол оси двигателя
    FIFO_REL_POS = 0x47,    ///< буферное приращение
    FIFO_CLEAN = 0x46,      ///< очистить буфер
} ISDRIVER_BASE_COMMAND;

typedef enum {
    ABS_ANGLE_MODE = 0x02,
    FIFO_REL_ANGLE_MODE = 0x0B,
} ISDRIVER_MOTION_MODE;

typedef enum {
    ERROR_NONE = 0,
    ERROR_NOT_SEND,
    ERROR_NO_ANSWER,
    ERROR_NOT_ACCEPTED,
    ERROR_WRONG_FORMAT,
} ISDRIVER_ERROR;


static const uint32_t TypeMask = 0x0F << 7;
static const uint32_t TypeBase = 0x04 << 7;
static const uint32_t TypeSync = 0x08 << 7;
static const uint8_t DevClass = 0x02;
static const uint8_t WriteBit = 1 << 7;


// static ISDRIVER_Cmd isdriver_msg2Cmd(const CANBUS_Message * msg) {
//     ISDRIVER_Cmd cmd;
//     cmd.cmd = msg->data32[0].b[1] & ~WriteBit;
//     cmd.wr = msg->data32[0].b[1] & WriteBit;
//     cmd.id = msg->ID & ~Type;
//     cmd.data32 = msg->data32[1];
//     return cmd;
// }

static ISDRIVER_ERROR isdriver_write(ISDRIVER_Driver * driver, ISDRIVER_BASE_COMMAND command, int32_t param, CANBUS_Priority pr) {
    CANBUS_Message msg;
    msg.ID = TypeBase | driver->id;
    msg.DLC = 8;
    msg.data32[0].b[0] = 0x00;//DevClass;
    msg.data32[0].b[1] = command | WriteBit;
    msg.data32[0].b[2] = 0x00;
    msg.data32[0].b[3] = 0x00;
    msg.data32[1].i = param;
    // проверка на отправку
    bool isSend;
    isSend = canbus_send(driver->port, pr, &msg);
    if (!isSend) return ERROR_NOT_SEND;
    // проверка на прием
    bool isRecv;
    DELAY_TILL_5(isRecv = canbus_recv(driver->port, &msg));
    if (!isRecv) return ERROR_NO_ANSWER;
    // проверка на валидность
    driver->errCnt = msg.data32[0].b[3]; // обновляем счетчик ошибки
    if (msg.data32[0].b[2] != 0) return ERROR_NOT_ACCEPTED;
    return ERROR_NONE;
}

static ISDRIVER_ERROR isdriver_read(ISDRIVER_Driver * driver, ISDRIVER_BASE_COMMAND command, int32_t * data) {
    CANBUS_Message msg;
    msg.ID = TypeBase | driver->id;
    msg.DLC = 8;
    msg.data32[0].b[0] = 0x00; //DevClass;
    msg.data32[0].b[1] = command;
    msg.data32[0].b[2] = 0x00;
    msg.data32[0].b[3] = 0x00;
    msg.data32[1].i = 0;
    // проверка на отправку
    bool isSend;
    isSend = canbus_send(driver->port, CANBUS_PR_LOW, &msg);
    if (!isSend) return ERROR_NOT_SEND;
    // проверка на прием
    bool isRecv;
    DELAY_TILL_5(isRecv = canbus_recv(driver->port, &msg));
    if (!isRecv) return ERROR_NO_ANSWER;
    // проверка на корректность
    if (msg.data32[0].b[0] != DevClass || msg.data32[0].b[1] != command) return ERROR_WRONG_FORMAT;
    // обновляем
    driver->errCnt = msg.data32[0].b[3]; // обновляем счетчик ошибки
    *data = msg.data32[1].i;
    return ERROR_NONE;
}

static bool write(ISDRIVER_Driver * driver, ISDRIVER_BASE_COMMAND cmd, int32_t param) {
    return isdriver_write(driver, cmd, param, CANBUS_PR_MEDIUM) == ERROR_NONE;
}

bool isdriver_init(ISDRIVER_Driver * driver) {
    CANBUS_Filter filter = {.n = driver->id, .id = TypeBase, .mask = TypeMask};
    driver->errCnt = 0;
    canbus_addFilter(driver->port, &filter);

    if (!write(driver, DRIVER_TYPE,     1)) return false;
    if (!write(driver, CIRCUIT_CONF,    1)) return false;
    if (!write(driver, POWER_ON,        0)) return false;
    if (!write(driver, MOTION_MODE,     FIFO_REL_ANGLE_MODE)) return false;
    if (!write(driver, SYNC_ON,         0)) return false;
    if (!write(driver, MOTION_ON,       0)) return false;
    
    return true;
}

bool isdriver_setPos(ISDRIVER_Driver * driver, int32_t pos) {
    return isdriver_write(driver, FIFO_REL_POS, pos, CANBUS_PR_MEDIUM) == ERROR_NONE;
}

int32_t isdriver_getPos(ISDRIVER_Driver * driver) {
    int32_t tmp;
    isdriver_read(driver, GET_ABS_POS, &tmp);
    return tmp;
}

void isdriver_sync_write(CANBUS_PORT port, ISDRIVER_SYNC_COMMAND cmd, uint32_t param) {
    CANBUS_Message msg;
    msg.ID = TypeSync | 0x00; //broadcast
    msg.DLC = 8;
    msg.data32[0].b[0] = DevClass;
    msg.data32[0].b[1] = cmd;
    msg.data32[0].b[2] = 0x00;
    msg.data32[0].b[3] = 0x00;
    msg.data32[1].i = param;
    DELAY_TILL(canbus_send(port, CANBUS_PR_HIGH, &msg));
}

void isdriver_sync_init(CANBUS_PORT port) {
    isdriver_sync_write(port, SYNC_RESET_TIME, 0);
}
