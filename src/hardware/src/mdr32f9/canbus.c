#if defined(USE_MDR1986VE9x)

#include "canbus.h"
#include <MDR32Fx.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_can.h>
#include <MDR32F9Qx_it.h>

static void null_handler1(void) {}
static bool null_handler2(void) { return false; }

#pragma weak can1_rx_handler = null_handler1
#pragma weak can1_tx_handler = null_handler2
#pragma weak can1_tc_handler = null_handler1

#pragma weak can2_rx_handler = null_handler1
#pragma weak can2_tx_handler = null_handler2
#pragma weak can2_tc_handler = null_handler1


typedef struct
{
    MDR_CAN_TypeDef * can;
    IRQn_Type can_irq;
    uint32_t rst_clk_can;
    MDR_PORT_TypeDef * port;
    uint8_t pin_tx;
    uint8_t pin_rx;
} can_descript;


static const can_descript CANS[] = {
    {
        MDR_CAN1, CAN1_IRQn, RST_CLK_CAN_CLOCK_CAN1_CLK_EN,
        MDR_PORTA, 6, 7,
    },
    {
        MDR_CAN2, CAN2_IRQn, RST_CLK_CAN_CLOCK_CAN2_CLK_EN,
        MDR_PORTE, 7, 6,
    },
};

void canbus_init(CANBUS_PORT n) {
    /* тактирование порта */
    RST_CLK_PCLKcmd(PCLK_BIT(CANS[n].port), ENABLE);
    /* тактирование can */
    RST_CLK_PCLKcmd(PCLK_BIT(CANS[n].can), ENABLE);
    MDR_RST_CLK->CAN_CLOCK |= CANS[n].rst_clk_can;

    /* настройка пинов */
    CANS[n].port->FUNC |= ((2 << CANS[n].pin_tx*2) 
                         | (2 << CANS[n].pin_rx*2)); //режим работы порта
    CANS[n].port->ANALOG |= ((1 << CANS[n].pin_tx ) 
                           | (1 << CANS[n].pin_rx )); //цифровой
    CANS[n].port->PWR |= ((3 << CANS[n].pin_tx*2) 
                        | (3 << CANS[n].pin_rx*2)); //максимально быcтрый
    CANS[n].port->PD &=~ ((1 << CANS[n].pin_tx ) 
                        | (1 << CANS[n].pin_rx )); //управляемый драйвер

    /* настройка can */
    CANS[n].can->BITTMNG = 
        9           /* Предделитель системной частоты */
        |(1 << 16)  /* фаза PSEG: 1xTQ */
        |(6 << 19)  /* SEG1: 6xTQ */
        |(5 << 22)  /* SEG2: 5xTQ */
        |(3 << 25)  /* SJW: 3xTQ */
        |(0 << 27); /* Семплирование: однократное */
    CANS[n].can->CONTROL =
         (1<<CAN_CONTROL_CAN_EN_Pos)    /* разрешение работы */
        |(0<<CAN_CONTROL_ROM_Pos)       /* Режим «Только прием» */
        |(0<<CAN_CONTROL_STM_Pos)       /* Режим самотестирования */
        |(0<<CAN_CONTROL_SAP_Pos)       /* Подтверждение собственных пакетов */
        |(0<<CAN_CONTROL_ROP_Pos);      /* Прием собственных пакетов */

    /* включаем поддержку прерываний */
    NVIC_EnableIRQ(CANS[n].can_irq);
    CAN_ITConfig(CANS[n].can, CAN_IT_GLBINTEN, ENABLE);
}

void canbus_rx_tx_interrupt_enable(CANBUS_PORT n, bool rx_en, bool tx_en) {
    /* Enable RX_INT interrupt */
    CAN_ITConfig(CANS[n].can,  CAN_IT_RXINTEN, rx_en);
    /* Enable TX_INT interrupt */
    CAN_ITConfig(CANS[n].can,  CAN_IT_TXINTEN, tx_en);
}

void canbus_addFilter(CANBUS_PORT n, const CANBUS_Filter * filter) {
    (void) n;
    (void) filter;
}

bool canbus_send(CANBUS_PORT n, CANBUS_Priority pr, const CANBUS_Message * msg) {
    (void) pr;

    CAN_TxMsgTypeDef TxMsg = {
        .IDE     = CAN_ID_STD,      /* тип сообщения (стандартный/расширенный) */
        .DLC     = msg->DLC,        /* количество байт данных */
        .PRIOR_0 = DISABLE,
        .ID      = CAN_STDID_TO_EXTID(msg->ID), /* стандартный ID */
        .Data = {
            msg->data32[0].u,
            msg->data32[1].u,
        }
    };
    CAN_Transmit(CANS[n].can, 0, &TxMsg);
    return true;
}

bool canbus_recv(CANBUS_PORT n, CANBUS_Message * rcv) {
    (void) n;
    (void) rcv;
    return false;
}

void CAN1_IRQHandler(void)
{
//   CAN_RxMsgTypeDef RxMessage;

//   CAN_GetRawReceivedData(MDR_CAN1, rx_buf, &RxMessage);

//   if((RxMessage.Rx_Header.ID==0x15555555) && (RxMessage.Rx_Header.IDE==CAN_ID_EXT)
//      && (RxMessage.Rx_Header.DLC==4) && (RxMessage.Data[0]==0x12345678))
//   {
//     ret = 1;
//   }
//   else
//   {
//     ret = 0;
//   }
//   CAN_ITClearRxTxPendingBit(MDR_CAN1, rx_buf, CAN_STATUS_RX_READY);
}

void CAN2_IRQHandler(void)
{

}

#endif
