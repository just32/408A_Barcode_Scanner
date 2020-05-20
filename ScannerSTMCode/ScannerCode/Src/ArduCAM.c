#include "ArduCAM.h"

SPI_HandleTypeDef spi;
//Control the CS pin
void CS_HIGH(void)
{
	HAL_GPIO_WritePin(CS_PORT,CS_PIN, GPIO_PIN_SET);
}

void CS_LOW(void)
{
	HAL_GPIO_WritePin(CS_PORT,CS_PIN, GPIO_PIN_RESET);
}

void ArduCAM_Init()
{
	/* reset Arduchip*/
	Arducam_Reset_Arduchip();

	SSCB_wrSensorReg16_8(OV2640_CHIP_SSCB_BUS_ADDR_W, 0x3008, 0x80);

	HAL_Delay(100);

  	SSCB_wrSensorRegs16_8(OV2640_320x240_JPEG);
	HAL_Delay(100);
	SSCB_wrSensorReg16_8(OV2640_CHIP_SSCB_BUS_ADDR_W, 0x3818, 0xa8);
	SSCB_wrSensorReg16_8(OV2640_CHIP_SSCB_BUS_ADDR_W, 0x3621, 0x10);
	SSCB_wrSensorReg16_8(OV2640_CHIP_SSCB_BUS_ADDR_W, 0x3801, 0xb0);
	SSCB_wrSensorReg16_8(OV2640_CHIP_SSCB_BUS_ADDR_W, 0x4407, 0x04);
	SSCB_wrSensorReg16_8(OV2640_CHIP_SSCB_BUS_ADDR_W, 0x5888, 0x00);

    Arducam_Set_Bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);

	Arducam_Clear_FIFO_Flag();
	Arducam_Write_Register(ARDUCHIP_FRAMES, 0x00);
	HAL_Delay(100);
}
/**
 * This function will send dummy data to Arduchip for flush
 * spi bus communication
 */
void Arducam_Send_Dummy(){
	/* start spi communication*/
	CS_LOW();
	/* declare variable with dummy values*/
	uint8_t tx_buf[1] = {0x0};
	/* transfer one byte over SPI bus*/
	HAL_SPI_Transmit(&spi, (uint8_t*)tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);

	/* stop spi bus communication*/
	CS_HIGH();
}
/**
 * This function will test SPI communication between Arduchip
 * @return is operation successfully?
 */
bool Arducam_SPI_Bus_Test(){
	/* declare variables for test results*/
	uint8_t reg_value = 0, received_data = 0, test_pass = 0, test_fail = 0;

	/* flush old spi communication*/
	Arducam_Send_Dummy();

	/* try to communicate Arduchip on SPI bus*/
	for(uint8_t iterator = 0 ; iterator < 5 ; iterator++){
		/* write data to SPI bus*/
		Arducam_Write_Register(ARDUCHIP_TEST1, reg_value);
		/* read data from SPI bus*/
		received_data = Arducam_Read_Register(ARDUCHIP_TEST1);
		ARDUCAM_DELAY_MS(1);
		/* if sended and received data is equal */
		if (received_data == reg_value)
			test_pass++;
		else
			test_fail++;
		/* increase register value*/
		reg_value++;
	}
	/* if test passed return true*/
	if(test_pass == 5){
		return true;
	}else
		return false;
}
/**
 * This function will init Arducam 5MP module with default settings
 * @return is operation successfully?
 */
bool Arducam_Mini_5MP_Init(){
	/* declare variable for return value*/
	bool ret_val = false;
	/* test Arducam SPI bus communication*/
	ret_val = Arducam_SPI_Bus_Test();
	ret_val = Camera_Test_SSCB_Bus(OV2640_CHIPID_HIGH, OV2640_CHIPID_LOW);
	/* if spi or SSCB bus communication error occured return false*/
	if(!ret_val)
		return false;
	/* init camera with default settings*/
	ArduCAM_Init();
	/* wait for camera stabilized*/
	ARDUCAM_DELAY_MS(1000);
	/* return true*/
	return true;
}
