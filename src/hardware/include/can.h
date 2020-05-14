#ifndef __HARDWARE_CAN_H__
#define __HARDWARE_CAN_H__

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    CAN_1,
    CAN_2,
} CAN_PORT;

typedef struct
{
    uint32_t ID;        ///< standard ID
    uint8_t DLC;        ///< data lenght
    uint8_t data[8];    ///< data content
} CAN_Message;

/**
 * init can for 500kbit/sec
*/
void can_init(CAN_PORT port);

void can_rx_tx_interrupt_enable(CAN_PORT port, bool rx_en, bool tx_en);

CAN_Message can_recv(CAN_PORT port);
void can_send(CAN_PORT port, const CAN_Message * msg);


/**
 * tx complete handler
 * @returns true if last byte was just set to send
 */
bool can1_tx_handler(void);
void can1_tc_handler(void);
void can1_rx_handler(void);

bool can2_tx_handler(void);
void can2_tc_handler(void);
void can2_rx_handler(void);

#endif