#ifndef __HARDWARE_CANBUS_H__
#define __HARDWARE_CANBUS_H__

#include "data32.h"
#include <stdbool.h>


typedef enum {
    CANBUS_1,
    CANBUS_2,
} CANBUS_PORT;

typedef struct {
    uint32_t ID;       ///< standard ID
    uint8_t DLC;       ///< data lenght
    Data32 data32[2];  ///< data content
} CANBUS_Message;

typedef struct {
    int8_t n;
    uint32_t id;
    uint32_t mask;
} CANBUS_Filter;

typedef enum {
    CANBUS_PR_HIGH = 0,
    CANBUS_PR_MEDIUM = 1,
    CANBUS_PR_LOW = 2,
} CANBUS_Priority;

/**
 * init can for 500kbit/sec
*/
void canbus_init(CANBUS_PORT n);

void canbus_addFilter(CANBUS_PORT n, const CANBUS_Filter * filter);

void canbus_rx_tx_interrupt_enable(CANBUS_PORT n, bool rx_en, bool tx_en);

bool canbus_recv(CANBUS_PORT n, CANBUS_Message * rcv);
bool canbus_send(CANBUS_PORT n, CANBUS_Priority pr, const CANBUS_Message * msg);


/**
 * tx complete handler
 * @returns true if last byte was just set to send
 */
bool canbus1_tx_handler(void);
void canbus1_tc_handler(void);
void canbus1_rx_handler(CANBUS_Message * rcv);

bool canbus2_tx_handler(void);
void canbus2_tc_handler(void);
void canbus2_rx_handler(CANBUS_Message * rcv);

#endif