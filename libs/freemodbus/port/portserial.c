/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- STM32F hardware ----------------------------------*/
#include "uart.h"
#include "digitalpin.h"

/* --------------------------- Config ---------------------------------------*/
#include "FreeModbusConfig.h"

/* ----------------------- Start implementation -----------------------------*/


static volatile bool txen = false;


BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    (void)ucPORT;

    /* init uart */
	uart_init(MODBUS_UART, ulBaudRate, ucDataBits, eParity);
    /* init txe pin */
    digitalpin_mode(RS485_TXE_PIN, DIGITALPIN_OUTPUT);
    digitalpin_set(RS485_TXE_PIN, 0);

    return TRUE;
}


void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /*
     * If xRXEnable enable serial receive interrupts.
     * If xTxENable enable transmitter empty interrupts. 
     */
    uart_rx_tx_interrupt_enable(MODBUS_UART, xRxEnable, xTxEnable);
    
    /* rs485 enable pin control */
    if (xRxEnable) {
        txen = false;
    }
    if (xTxEnable) {
        txen = true;
        digitalpin_set(RS485_TXE_PIN, 1);
    } else {
        txen = false;
    }
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = uart_recv(MODBUS_UART);
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    uart_send(MODBUS_UART, ucByte);
    return TRUE;
}

/* ----------------------- USART ISR ----------------------------------*/
/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
/* Find out what interrupted and get or send data as appropriate */

void MODBUS_UART_RX_HANDLER()
{
    digitalpin_set(PC_13, 0);

    pxMBFrameCBByteReceived();    
}

bool MODBUS_UART_TX_HANDLER()
{
    pxMBFrameCBTransmitterEmpty();
    /* returns true if we need to disable 
       transmitter on tc event */
    return !txen;
}

void MODBUS_UART_TC_HANDLER()
{
    /* Disable transmitter when complete */
    digitalpin_set(RS485_TXE_PIN, 0);
}