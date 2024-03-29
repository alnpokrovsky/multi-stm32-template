#ifndef __FREE_MODBUS_CONFIG_H__
#define __FREE_MODBUS_CONFIG_H__


#define MODBUS_TIM               TIM_2
#define MODBUS_TIM_HANDLER()     tim2_handler()

#define MODBUS_UART              UART_1
#define MODBUS_UART_RX_HANDLER() uart1_rx_handler()
#define MODBUS_UART_TX_HANDLER() uart1_tx_handler()
#define MODBUS_UART_TC_HANDLER() uart1_tc_handler()

#define RS485_TXE_PIN            PA_8

#define MODBUS_TABLE_HREG_SIZE   10
#define MODBUS_TABLE_IREG_SIZE   10
#define MODBUS_TABLE_COILS_SIZE  80
#define MODBUS_TABLE_ISTS_SIZE   80


#endif