#ifdef FREEMODBUS_LIB

#include "modbus.h"
#include "mb.h"
#include "mbutils.h"
#include <assert.h>
#include <string.h>
#include "word.h"

static uint16_t Hreg_table[10];
#define TABLE_HREG_SIZE ( sizeof(Hreg_table) / sizeof(Hreg_table[0]) )

static uint8_t Coils_table[10];
#define TABLE_COILS_SIZE ( sizeof(Coils_table) * sizeof(Coils_table[0]) )

static uint8_t Ists_table[10];
#define TABLE_ISTS_SIZE ( sizeof(Ists_table) * sizeof(Ists_table[0]) )

static uint16_t Ireg_table[10];
#define TABLE_IREG_SIZE ( sizeof(Ireg_table) / sizeof(Ireg_table[0]) )


uint16_t modbus_Hreg(uint16_t addr) {
    return word_swap(Hreg_table[addr]);
}
void modbus_set_Hreg(uint16_t addr, uint16_t val) {
    Hreg_table[addr] = word_swap(val);
}

bool modbus_Coil(uint16_t addr) {
    return xMBUtilGetBits(Coils_table, addr, 1);
}
uint16_t modbus_Coil_word(uint16_t addr16) {
    return word_from_bytes(Coils_table, addr16);
}
void modbus_set_Coil(uint16_t addr, bool val) {
    xMBUtilSetBits(Coils_table, addr, 1, val ? 1 : 0);
}

void modbus_set_Ists(uint16_t addr, bool val) {
    xMBUtilSetBits(Ists_table, addr, 1, val ? 1 : 0);
}
void modbus_set_Ists_word(uint16_t addr16, uint16_t val16) {
    word_to_bytes(val16, Ists_table, addr16);
}

void modbus_set_Ireg(uint16_t addr, uint16_t val) {
    Ireg_table[addr] = word_swap(val);
}

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
	USHORT usNRegs)
{
	usAddress -= 1; /* to c-style address */
    uint16_t * ir = &Ireg_table[usAddress];
    uint16_t * buf = (uint16_t*) pucRegBuffer;

    /* check if we away of table size */
    if (usAddress + usNRegs > TABLE_IREG_SIZE) {
        return MB_ENOREG;
    }

    for (int i = 0; i < usNRegs; i++) {
        buf[i] = ir[i];
    }

	return MB_ENOERR;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
	USHORT usNRegs, eMBRegisterMode eMode)
{
    usAddress -= 1; /* to c-style address */
    uint16_t * hr = &Hreg_table[usAddress];
    uint16_t * buf = (uint16_t*) pucRegBuffer;

    /* check if we away of table size */
    if (usAddress + usNRegs > TABLE_HREG_SIZE) {
        return MB_ENOREG;
    }

    switch (eMode)
    {
        case MB_REG_WRITE:
            for (int i = 0; i < usNRegs; i++) {
                hr[i] = buf[i];
            }
            break;
        case MB_REG_READ:
            for (int i = 0; i < usNRegs; i++) {
                buf[i] = hr[i];
            }
            break;
    }

	return MB_ENOERR;
}

eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress,
	USHORT usNCoils, eMBRegisterMode eMode)
{
	usAddress -= 1; /* to c-style address */

    /* check if we away of table size */
    if (usAddress + usNCoils > TABLE_COILS_SIZE) {
        return MB_ENOREG;
    }

    switch (eMode)
    {
        case MB_REG_WRITE:
            for (int i = 0; i < usNCoils; i++) {
                UCHAR wbit = xMBUtilGetBits(pucRegBuffer, i, 1 );
                xMBUtilSetBits( Coils_table, usAddress+i, 1, wbit );
            }
            break;
        case MB_REG_READ:
            for (int i = 0; i < usNCoils; i++) {
                UCHAR rbit = xMBUtilGetBits( Coils_table, usAddress+i, 1 );
                xMBUtilSetBits( pucRegBuffer, i, 1, rbit );
            }
            break;
    }

	return MB_ENOERR;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress,
	USHORT usNDiscrete)
{
	usAddress -= 1; /* to c-style address */

    /* check if we away of table size */
    if (usAddress + usNDiscrete > TABLE_ISTS_SIZE) {
        return MB_ENOREG;
    }

    for (int i = 0; i < usNDiscrete; i++) {
        UCHAR rbit = xMBUtilGetBits( Ists_table, usAddress+i, 1 );
        xMBUtilSetBits( pucRegBuffer, i, 1, rbit );
    }

	return MB_ENOERR;
}

#include "digitalpin.h"

void modbus_init() {
    eMBErrorCode eStatus = MB_ENOERR;
    (void) eStatus;
	eStatus = eMBInit(MB_RTU, 11, 1, 19200, MB_PAR_EVEN);
    assert(eStatus == MB_ENOERR);

	const char *report_data = "karlwashere";
	eStatus = eMBSetSlaveID(0x34, TRUE, (UCHAR *) report_data, strlen(report_data));
	assert(eStatus == MB_ENOERR);
    

	eStatus = eMBEnable();
	assert(eStatus == MB_ENOERR);
}

inline void modbus_poll() {
    eMBPoll();
}


#endif