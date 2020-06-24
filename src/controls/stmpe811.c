#include "stmpe811.h"
#include "iic.h"
#include "delay.h"
#include "minmax.h"

#define	STMPE811_ADDRESS	0x41

typedef enum {
/* Registers */
	STMPE811_CHIP_ID =				0x00,	//STMPE811 Device identification
	STMPE811_ID_VER =				0x02,	//STMPE811 Revision number; 0x01 for engineering sample; 0x03 for final silicon
	STMPE811_SYS_CTRL1 =			0x03,	//Reset control
	STMPE811_SYS_CTRL2 =			0x04,	//Clock control
	STMPE811_SPI_CFG =				0x08,	//SPI interface configuration
	STMPE811_INT_CTRL =				0x09,	//Interrupt control register
	STMPE811_INT_EN =				0x0A,	//Interrupt enable register
	STMPE811_INT_STA =				0x0B,	//Interrupt status register
	STMPE811_GPIO_EN =				0x0C,	//GPIO interrupt enable register
	STMPE811_GPIO_INT_STA =			0x0D,	//GPIO interrupt status register
	STMPE811_ADC_INT_EN =			0x0E,	//ADC interrupt enable register
	STMPE811_ADC_INT_STA =			0x0F,	//ADC interface status register
	STMPE811_GPIO_SET_PIN =			0x10,	//GPIO set pin register
	STMPE811_GPIO_CLR_PIN =			0x11,	//GPIO clear pin register
	STMPE811_MP_STA =				0x12,	//GPIO monitor pin state register
	STMPE811_GPIO_DIR =				0x13,	//GPIO direction register
	STMPE811_GPIO_ED =				0x14,	//GPIO edge detect register
	STMPE811_GPIO_RE =				0x15,	//GPIO rising edge register
	STMPE811_GPIO_FE =				0x16,	//GPIO falling edge register
	STMPE811_GPIO_AF =				0x17,	//alternate function register
	STMPE811_ADC_CTRL1 =			0x20,	//ADC control
	STMPE811_ADC_CTRL2 =			0x21,	//ADC control
	STMPE811_ADC_CAPT =				0x22,	//To initiate ADC data acquisition
	STMPE811_ADC_DATA_CHO =			0x30,	//ADC channel 0
	STMPE811_ADC_DATA_CH1 =			0x32,	//ADC channel 1
	STMPE811_ADC_DATA_CH2 =			0x34,	//ADC channel 2
	STMPE811_ADC_DATA_CH3 =			0x36,	//ADC channel 3
	STMPE811_ADC_DATA_CH4 =			0x38,	//ADC channel 4
	STMPE811_ADC_DATA_CH5 =			0x3A,	//ADC channel 5
	STMPE811_ADC_DATA_CH6 =			0x3C,	//ADC channel 6
	STMPE811_ADC_DATA_CH7 =			0x3E,	//ADC channel 7
	STMPE811_TSC_CTRL =				0x40,	//4-wire touchscreen controller setup
	STMPE811_TSC_CFG =				0x41,	//Touchscreen controller configuration
	STMPE811_WDW_TR_X =				0x42,	//Window setup for top right X
	STMPE811_WDW_TR_Y =				0x44,	//Window setup for top right Y
	STMPE811_WDW_BL_X =				0x46,	//Window setup for bottom left X
	STMPE811_WDW_BL_Y =				0x48,	//Window setup for bottom left Y
	STMPE811_FIFO_TH =				0x4A,	//FIFO level to generate interrupt
	STMPE811_FIFO_STA =				0x4B,	//Current status of FIFO
	STMPE811_FIFO_SIZE =			0x4C,	//Current filled level of FIFO
	STMPE811_TSC_DATA_X =			0x4D,	//Data port for touchscreen controller data access
	STMPE811_TSC_DATA_Y =			0x4F,	//Data port for touchscreen controller data access
	STMPE811_TSC_DATA_Z =			0x51,	//Data port for touchscreen controller data access
	STMPE811_TSC_DATA_XYZ =			0x52,	//Data port for touchscreen controller data access
	STMPE811_TSC_FRACTION_Z =		0x56,	//Touchscreen controller FRACTION_Z
	STMPE811_TSC_DATA =				0x57,	//Data port for touchscreen controller data access
	STMPE811_TSC_I_DRIVE =			0x58,	//Touchscreen controller drivel
	STMPE811_TSC_SHIELD =			0x59,	//Touchscreen controller shield
	STMPE811_TEMP_CTRL =			0x60,	//Temperature sensor setup
	STMPE811_TEMP_DATA =			0x61,	//Temperature data access port
	STMPE811_TEMP_TH =				0x62,	//Threshold for temperature controlled interrupt
} STMPE811_COMMAND;

bool stmpe811_init(void) {
    iic_init(IIC_3);

	/* Check for STMPE811 Connected */
	uint8_t id = iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_CHIP_ID);
	if (id != 0x08) return false;
	
	/* Reset */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_SYS_CTRL1, 0x02);
	delay_ms(5);
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_SYS_CTRL1, 0x00);
	delay_ms(2);

	uint8_t mode = iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_SYS_CTRL2); 
	mode &= ~(0x01);
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_SYS_CTRL2, mode);
	mode = iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_SYS_CTRL2); 
	mode &= ~(0x02);
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_SYS_CTRL2, mode);
	

	/* Select Sample Time, bit number and ADC Reference */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_SYS_CTRL1, 0x49);
	delay_ms(2);

	/* Select the ADC clock speed: 3.25 MHz */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_ADC_CTRL2, 0x01);

	/* Select TSC pins in non default mode */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_GPIO_AF, 0x1E);

	/* Select 2 nF filter capacitor */
	/* Configuration: 
	- Touch average control    : 4 samples
	- Touch delay time         : 500 uS
	- Panel driver setting time: 500 uS 
	*/
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_CFG, 0x9A);

	/* Configure the Touch FIFO threshold: single point reading */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_FIFO_TH, 0x01);

	/* Clear the FIFO memory content. */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_FIFO_STA, 0x01);

	/* Put the FIFO back into operation mode  */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_FIFO_STA, 0x00);

	/* Set the range and accuracy pf the pressure measurement (Z) : 
	- Fractional part :7 
	- Whole part      :1 
	*/
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_FRACTION_Z, 0x01);

	/* Set the driving capability (limit) of the device for TSC pins: 50mA */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_I_DRIVE, 0x01);

	/* Touch screen control configuration (enable TSC):
	- No window tracking index
	- XYZ acquisition mode
	*/
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_CTRL, 0x03);

	/* Clear all the status pending bits if any */
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_INT_STA, 0xFF);
	
	/* Enable global interrupts */
	/*
	mode = TM_I2C_Read(IIC_3, STMPE811_ADDRESS, STMPE811_INT_CTRL);
	mode |= 0x01;
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_INT_CTRL, mode);
	*/
	/* Enable touch interrupt */
	/*
	mode = TM_I2C_Read(IIC_3, STMPE811_ADDRESS, STMPE811_INT_EN);
	mode |= 0x01;
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_INT_EN, mode);
	*/
	/* Wait for 2 ms delay */
	delay_ms(2);
	return true;
}

// //For touch screen (origin lower left)
// static const uint16_t MIN_X = 350;
// static const uint16_t MIN_Y = 280;
// static const uint16_t MAX_X = 3650;
// static const uint16_t MAX_Y = 3800;

// For screen (origin upper left)
static const uint16_t X_WIDTH = 239;
static const uint16_t Y_HEIGHT = 319;

static uint16_t readX(void) {
	int16_t val = iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_DATA_X+1);
	val |= iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_DATA_X) << 8;
	
	if (val <= 3000) val = 3900 - val;
	else 			 val = 3800 - val;
	val /= 15;
	
	if (val < 0) val = 0;
	else if (val > X_WIDTH) val = X_WIDTH;

	return val;
}

static uint16_t readY(void) {
	int16_t val = iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_DATA_Y+1);
	val |= iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_DATA_Y) << 8;
		
	val -= 360;
	val = val / 11;

	if (val < 0) val = 0;
	else if (val > Y_HEIGHT) val = Y_HEIGHT;

	return Y_HEIGHT - val;
}

STMPE811_Point stmpe811_read(void) {
	STMPE811_Point p;

	uint8_t stat = iic_recv(IIC_3, STMPE811_ADDRESS, STMPE811_TSC_CTRL);
	if ((stat & 0x80) == 0) {
		//Not pressed
		p.z = 0;
	} else {
		//Pressed
		p.z = 1;
		p.x = readX();
		p.y = readY();
	}
	
	//Reset Fifo
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_FIFO_STA, 0x01);
	iic_send(IIC_3, STMPE811_ADDRESS, STMPE811_FIFO_STA, 0x00);

	return p;
}
