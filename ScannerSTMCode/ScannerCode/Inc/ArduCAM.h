#ifndef __ARDUCAM_H
#define __ARDUCAM_H

#include "stdbool.h"
#include "spi.h"
#include "sscb_bus.h"
#include "ov2640_regs.h"

#define MAX_FIFO_SIZE		0x7FFFF			//512KByte
#define ARDUCHIP_TEST1       	0x00  //TEST register
#define ARDUCHIP_FRAMES			  0x01
#define ARDUCHIP_MODE      		0x02  //Mode register
#define VSYNC_LEVEL_MASK   		0x02  //0 = High active , 		1 = Low active
#define ARDUCHIP_TIM       		0x03  //Timming control
#define ARDUCHIP_FIFO      		0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK    		0x01
#define FIFO_START_MASK    		0x02
#define BURST_FIFO_READ			0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ		0x3D  //Single FIFO read operation
#define ARDUCHIP_TRIG      		0x41  //Trigger source
#define VSYNC_MASK         		0x01
#define CAP_DONE_MASK      		0x08
#define FIFO_SIZE1				0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3				0x44  //Camera write FIFO size[18:16]
//ArduCAM CS define
#define		CS_PORT		GPIOB
#define		CS_PIN		GPIO_PIN_1
#define ARDUCAM_SPI_PORT		0x60
#define ARDUCAM_SPI_TIMEOUT		1000

#define ARDUCAM_DELAY_MS(delay) HAL_Delay(delay)

#define OV2640_160x120 		0	//160x120
#define OV2640_176x144 		1	//176x144
#define OV2640_320x240 		2	//320x240
#define OV2640_352x288 		3	//352x288
#define OV2640_640x480		4	//640x480
#define OV2640_800x600 		5	//800x600
#define OV2640_1024x768		6	//1024x768
#define OV2640_1280x1024	7	//1280x1024
#define OV2640_1600x1200	8	//1600x1200

#define I2C_ADDR_8BIT 0
#define I2C_ADDR_16BIT 1
#define I2C_REG_8BIT 0
#define I2C_REG_16BIT 1
#define I2C_DAT_8BIT 0
#define I2C_DAT_16BIT 1

/* Register initialization tables for SENSORs */
/* Terminating list entry for reg */
#define SENSOR_REG_TERM_8BIT                0xFF
#define SENSOR_REG_TERM_16BIT               0xFFFF
/* Terminating list entry for val */
#define SENSOR_VAL_TERM_8BIT                0xFF
#define SENSOR_VAL_TERM_16BIT               0xFFFF

#define RWBIT									0x80  //READ AND WRITE BIT IS BIT[7]

#define ARDUCHIP_TEST1       	0x00  //TEST register

#if !(defined OV2640_MINI_2MP)
	#define ARDUCHIP_FRAMES			  0x01  //FRAME control register, Bit[2:0] = Number of frames to be captured																		//On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full
#endif

#define ARDUCHIP_MODE      		0x02  //Mode register
#define MCU2LCD_MODE       		0x00
#define CAM2LCD_MODE       		0x01
#define LCD2MCU_MODE       		0x02

#define ARDUCHIP_TIM       		0x03  //Timming control
#if !(defined OV2640_MINI_2MP)
	#define HREF_LEVEL_MASK    		0x01  //0 = High active , 		1 = Low active
	#define VSYNC_LEVEL_MASK   		0x02  //0 = High active , 		1 = Low active
	#define LCD_BKEN_MASK      		0x04  //0 = Enable, 					1 = Disable
	#if (defined ARDUCAM_SHIELD_V2)
		#define PCLK_REVERSE_MASK  	0x08  //0 = Normal PCLK, 		1 = REVERSED PCLK
	#else
		#define PCLK_DELAY_MASK  		0x08  //0 = data no delay,		1 = data delayed one PCLK
	#endif
	//#define MODE_MASK          		0x10  //0 = LCD mode, 				1 = FIFO mode
#endif
//#define FIFO_PWRDN_MASK	   		0x20  	//0 = Normal operation, 1 = FIFO power down
//#define LOW_POWER_MODE			  0x40  	//0 = Normal mode, 			1 = Low power mode

#define ARDUCHIP_FIFO      		0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK    		0x01
#define FIFO_START_MASK    		0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20

#define ARDUCHIP_GPIO			  0x06  //GPIO Write Register
#if !(defined (ARDUCAM_SHIELD_V2) || defined (ARDUCAM_SHIELD_REVC))
#define GPIO_RESET_MASK			0x01  //0 = Sensor reset,							1 =  Sensor normal operation
#define GPIO_PWDN_MASK			0x02  //0 = Sensor normal operation, 	1 = Sensor standby
#define GPIO_PWREN_MASK			0x04	//0 = Sensor LDO disable, 			1 = sensor LDO enable
#endif

#define BURST_FIFO_READ			0x3C  //Burst FIFO read operation
#define SINGLE_FIFO_READ		0x3D  //Single FIFO read operation

#define ARDUCHIP_REV       		0x40  //ArduCHIP revision
#define VER_LOW_MASK       		0x3F
#define VER_HIGH_MASK      		0xC0

#define ARDUCHIP_TRIG      		0x41  //Trigger source
#define VSYNC_MASK         		0x01
#define SHUTTER_MASK       		0x02
#define CAP_DONE_MASK      		0x08

#define FIFO_SIZE1				0x42  //Camera write FIFO size[7:0] for burst to read
#define FIFO_SIZE2				0x43  //Camera write FIFO size[15:8]
#define FIFO_SIZE3				0x44  //Camera write FIFO size[18:16]

typedef struct{
	uint32_t arducam_fifo_size;
	uint32_t width;
	uint32_t height;
}ts_arducam_info_t;
extern ts_arducam_info_t arducam_info;

void CS_HIGH(void);
void CS_LOW(void);

/**
 * This function will read how many bytes in Arducam FIFO
 * @return total FIFO size on Arduchip
 */
uint32_t Arducam_Read_FIFO_Length(void);
/**
 * This function will clear FIFO flag on Arducam
 */
void Arducam_Clear_FIFO_Flag(void);
/**
 * This function will return existing FIFO size on Arduchip
 * @return How many bytes are in FIFO
 */
uint32_t Arducam_Get_FIFO_Size(void);

bool Arducam_Read_FIFO(uint8_t buffer[], uint32_t * size);
/**
 * This function will reset Arduchip
 */
void Arducam_Reset_Arduchip(void);
/**
 * This function will trigger capture JPEG data on Arduchip
 */
bool Arducam_Start_Capture(void);
/**
 * This function will send dummy data to Arduchip for flush
 * spi bus communication
 */
void Arducam_Send_Dummy(void);
/**
 * This function will test SPI communication between Arduchip
 * @return is operation successfully?
 */
bool Arducam_SPI_Bus_Test(void);
/**
 * This function will init Arducam 5MP module with default settings
 * @return is operation successfully?
 */
bool Arducam_Mini_5MP_Init(void);
/**
 * This function will set bits of register on Arduchip
 * @param addr target register address
 * @param bit which bit?
 */
void Arducam_Set_Bit(uint8_t addr, uint8_t bit);
/**
 * This function will read specific register on Arduchip
 * @param addr target register address
 * @param bit which bit?
 * @return register value currently readed on Arduchip
 */
uint8_t Arducam_Get_Bit(uint8_t addr, uint8_t bit);
/**
 * This high level function will transfer register value to Arduchio
 * @param addr Target Arduchip register address
 * @param data New register value
 */
void Arducam_Write_Register(uint8_t addr, uint8_t data);
/**
 * This high level function will return register value on Arduchip
 * @param addr target Arduchip register address
 * @return register value transferred from SPI bus
 */
uint8_t Arducam_Read_Register(uint8_t addr);
/**
 * This function will help you to burst read
 * @warning SPI communication will not close
 */
void Arducam_Setup_FIFO(void);

#endif
