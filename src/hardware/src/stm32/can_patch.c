#if defined(NOWHERE) && ( defined(STM32F1)||defined(STM32F3)||defined(STM32F4) )

#include <libopencm3/stm32/can.h>
#include "delay.h"

/*---------------------------------------------------------------------------*/
/** @brief CAN Filter Init

Initialize incoming message filter and assign to FIFO.

@param[in] canport Unsigned int32. CAN block register base @ref can_reg_base.
@param[in] nr Unsigned int32. ID number of the filter.
@param[in] scale_32bit true for single 32bit, false for dual 16bit
@param[in] id_list_mode true for id lists, false for id/mask
@param[in] fr1 Unsigned int32. First filter register content.
@param[in] fr2 Unsigned int32. Second filter register content.
@param[in] fifo Unsigned int32. FIFO id.
@param[in] enable bool. Enable filter?
 */
void can_filter_init_patched(uint32_t canport, uint32_t nr, bool scale_32bit,
		     bool id_list_mode, uint32_t fr1, uint32_t fr2,
		     uint32_t fifo, bool enable);
void can_filter_init_patched(uint32_t canport, uint32_t nr, bool scale_32bit,
		     bool id_list_mode, uint32_t fr1, uint32_t fr2,
		     uint32_t fifo, bool enable)
{
	uint32_t filter_select_bit = 0x00000001 << nr;

	/* Request initialization "enter". */
	CAN_FMR(canport) |= CAN_FMR_FINIT;

	/* Deactivate the filter. */
	CAN_FA1R(canport) &= ~filter_select_bit;

	if (scale_32bit) {
		/* Set 32-bit scale for the filter. */
		CAN_FS1R(canport) |= filter_select_bit;
	} else {
		/* Set 16-bit scale for the filter. */
		CAN_FS1R(canport) &= ~filter_select_bit;
	}

	if (id_list_mode) {
		/* Set filter mode to ID list mode. */
		CAN_FM1R(canport) |= filter_select_bit;
	} else {
		/* Set filter mode to id/mask mode. */
		CAN_FM1R(canport) &= ~filter_select_bit;
	}

	/* Set the first filter register. */
	CAN_FiR1(canport, nr) = fr1;

	/* Set the second filter register. */
	CAN_FiR2(canport, nr) = fr2;

	/* Select FIFO0 or FIFO1 as filter assignement. */
	if (fifo) {
		CAN_FFA1R(canport) |= filter_select_bit;  /* FIFO1 */
	} else {
		CAN_FFA1R(canport) &= ~filter_select_bit; /* FIFO0 */
	}

	if (enable) {
		CAN_FA1R(canport) |= filter_select_bit; /* Activate filter. */
	}

	/* Request initialization "leave". */
	CAN_FMR(canport) &= ~CAN_FMR_FINIT;
}

/*---------------------------------------------------------------------------*/
/** @brief CAN Transmit Message

@param[in] canport Unsigned int32. CAN block register base @ref can_reg_base.
@param[in] mailbox number
@param[in] id Unsigned int32. Message ID.
@param[in] ext bool. Extended message ID?
@param[in] rtr bool. Request transmit?
@param[in] length Unsigned int8. Message payload length.
@param[in] data Unsigned int8[]. Message payload data.
@returns if mailbox was awailable and message was send
 */
bool can_transmit_patched(uint32_t canport, uint32_t mailbox,
    uint32_t id, bool ext, bool rtr, uint8_t length, uint8_t *data);
bool can_transmit_patched(uint32_t canport, uint32_t mailbox,
    uint32_t id, bool ext, bool rtr, uint8_t length, uint8_t *data)
{
	union {
		uint8_t data8[4];
		uint32_t data32;
	} tdlxr, tdhxr;

	uint32_t TME_N;
	uint32_t TXOK_N;

    /* Check if transmit mailbox is empty. */
    switch (mailbox) {
    case 0:
		mailbox = CAN_MBOX0;
		TME_N = CAN_TSR_TME0;
		TXOK_N = CAN_TSR_TXOK0;
        break;
    case 1:
		mailbox = CAN_MBOX1;
		TME_N = CAN_TSR_TME1;
		TXOK_N = CAN_TSR_TXOK1;
        break;
    case 2:
        mailbox = CAN_MBOX2;
		TME_N = CAN_TSR_TME2;
		TXOK_N = CAN_TSR_TXOK2;
        break;
    }

	// wait some if there are queue
	bool flag = DELAY_TILL(flag = CAN_TSR(canport) & TME_N);
	if (!flag) return false;
        

	if (ext) {
		/* Set extended ID. */
		CAN_TIxR(canport, mailbox) = (id << CAN_TIxR_EXID_SHIFT) |
			CAN_TIxR_IDE;
	} else {
		/* Set standard ID. */
		CAN_TIxR(canport, mailbox) = id << CAN_TIxR_STID_SHIFT;
	}

	/* Set/clear remote transmission request bit. */
	if (rtr) {
		CAN_TIxR(canport, mailbox) |= CAN_TIxR_RTR; /* Set */
	}

	/* Set the DLC. */
	CAN_TDTxR(canport, mailbox) &= ~CAN_TDTxR_DLC_MASK;
	CAN_TDTxR(canport, mailbox) |= (length & CAN_TDTxR_DLC_MASK);

	switch (length) {
	case 8:
		tdhxr.data8[3] = data[7];
		/* fall through */
	case 7:
		tdhxr.data8[2] = data[6];
		/* fall through */
	case 6:
		tdhxr.data8[1] = data[5];
		/* fall through */
	case 5:
		tdhxr.data8[0] = data[4];
		/* fall through */
	case 4:
		tdlxr.data8[3] = data[3];
		/* fall through */
	case 3:
		tdlxr.data8[2] = data[2];
		/* fall through */
	case 2:
		tdlxr.data8[1] = data[1];
		/* fall through */
	case 1:
		tdlxr.data8[0] = data[0];
		/* fall through */
	default:
		break;
	}
	/* Set the data. */

	CAN_TDLxR(canport, mailbox) = tdlxr.data32;
	CAN_TDHxR(canport, mailbox) = tdhxr.data32;

	/* Request transmission. */
	CAN_TIxR(canport, mailbox) |= CAN_TIxR_TXRQ;

	// wait till send
	flag = DELAY_TILL(CAN_TSR(canport) & TME_N);
	if (!flag) return false;

	return (CAN_TSR(canport) & TXOK_N) != 0;
}

#endif