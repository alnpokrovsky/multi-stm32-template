#if defined(USE_MDR1986VE9x)

#include "delay.h"
#include <MDR32Fx.h>
#include <MDR32F9Qx_usb_handlers.h>
#include <MDR32F9Qx_rst_clk.h>
#include <assert.h>


void usb_cdc_send(const uint8_t * buf, int len);
void usb_cdc_init(void);
void usb_cdc_read_handler(uint8_t* Buffer, uint32_t Length);


// Буфер для USB
#define BUFFER_LENGTH 64
static uint8_t USB_Buffer[BUFFER_LENGTH];

static void null_handler(uint8_t * buf, uint32_t len)
{
	/* echo by default */
	usb_cdc_send(buf, len);
}

#pragma weak usb_cdc_read_handler = null_handler


#ifdef USB_CDC_LINE_CODING_SUPPORTED

static USB_CDC_LineCoding_TypeDef LineCoding;

//Эти два запроса отправляются хостом, чтобы изменить или прочитать конфигурацию последовательной линии связи, которая включает в себя:
//• Baudrate (скорость передачи данных)
//• Number of stop bits (количество стоп-битов)
//• Parity check (наличие бита контроля четности)
//• Number of data bits (количество бит данных)
//Когда программа терминала (ПО хоста наподобие HyperTerminal) меняет установки COM-порта, то
//отправляется запрос SetLineCoding с новыми параметрами. Хост может также запросить текущие
//настройки запросом GetLineCoding, и не менять их, если настройки корректны.
/* USB_CDC_HANDLE_GET_LINE_CODING implementation example */
USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA)
{
    assert_param(DATA);
    if (wINDEX != 0)
    {
    /* Invalid interface */
    return USB_ERR_INV_REQ;
    }
    /* Just store received settings */
    *DATA = LineCoding;
    return USB_SUCCESS;
}
/* USB_CDC_HANDLE_SET_LINE_CODING implementation example */
USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA)
{
    assert_param(DATA);
    if (wINDEX != 0)
    {
    /* Invalid interface */
    return USB_ERR_INV_REQ;
    }
    /* Just send back settings stored earlier */
    LineCoding = *DATA;
    return USB_SUCCESS;
}
#endif /* USB_CDC_LINE_CODING_SUPPORTED */

/* Задание конфигурации последовательной линии связи которую может прочитать хост*/
static void VCom_Configuration(void)
{
#ifdef USB_CDC_LINE_CODING_SUPPORTED
    LineCoding.dwDTERate = 115200;
    LineCoding.bCharFormat = 0;
    LineCoding.bParityType = 0;
    LineCoding.bDataBits = 8;
#endif /* USB_CDC_LINE_CODING_SUPPORTED */
}

static void usb_setup(void) {
    /* Включение тактирования схемы USB*/
    RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, ENABLE);

    /* Настройка под 16МГц */
    assert(HSE_Value == 16000000);
    static USB_Clock_TypeDef USB_Clock_InitStruct;
    // Выбор источника тактирования USB
    USB_Clock_InitStruct.USB_USBC1_Source = USB_C1HSEdiv1;
    // Выбор коэффициента умножения схемы PLL для USB
    USB_Clock_InitStruct.USB_PLLUSBMUL = USB_PLLUSBMUL3;

    static USB_DeviceBUSParam_TypeDef USB_DeviceBUSParam;
    //Выбор режима USB FULL Speed
    USB_DeviceBUSParam.MODE = USB_SC_SCFSP_Full;
    //Выбор скорости передачи данных 12мБайт в сек.
    USB_DeviceBUSParam.SPEED = USB_SC_SCFSR_12Mb;
    // Подтягивание линии DP к питанию.
    USB_DeviceBUSParam.PULL = USB_HSCR_DP_PULLUP_Set;
    
    // Инициализация USB с заданными параметрами
    USB_DeviceInit(&USB_Clock_InitStruct, &USB_DeviceBUSParam);
    /* Разрешение всех видов прерываний от USB*/
    USB_SetSIM(USB_SIS_Msk);
    // Включение питания USB и разрешение передачи и приема данных
    USB_DevicePowerOn();
    /* Включение прерываний USB */
#ifdef USB_INT_HANDLE_REQUIRED
    NVIC_EnableIRQ(USB_IRQn);
#endif /* USB_INT_HANDLE_REQUIRED */
    USB_DEVICE_HANDLE_RESET;
}

void usb_cdc_init(void) {
    VCom_Configuration();
    // инициализация Буфера длинной 1 байт
    USB_CDC_Init(USB_Buffer, 1, SET);
    // настройка периферии
    usb_setup();
    
    /* wait till init */
    delay_ms(1000);
}

void usb_cdc_send(const uint8_t * buf, int len) {
    USB_CDC_SendData((uint8_t*)buf, len);
}

/*USB_CDC_HANDLE_DATA_RECEIVE implementation - data echoing*/
// Данная процедура автоматически вызывается при приеме данных по USB.
USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length)
{
    usb_cdc_read_handler(Buffer, Length);
    return USB_SUCCESS;
}


#endif