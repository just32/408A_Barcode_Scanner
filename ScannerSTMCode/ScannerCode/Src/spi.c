#include "spi.h"

#define BMPIMAGEOFFSET 66
#define pgm_read_byte(x)        (*((char *)x))
const char bmp_header[BMPIMAGEOFFSET] =
{
  0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00
};

uint8_t	*picbuf = 0;
bool receive_OK = false;    
bool USB_sendOK = true;    
bool send_OK = true;
uint32_t sendlen = 0;
uint32_t haveRev = 0;
uint32_t noRev = 0;
uint8_t  EP2_SendFinish = 1;
uint8_t	Buf1[BUFFER_MAX_SIZE]={0}, Buf2[BUFFER_MAX_SIZE]={0};
uint32_t length = 0;
extern uint16_t NumPackage;

void SPI1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  __GPIOB_CLK_ENABLE();
  __SPI2_CLK_ENABLE();
  __DMA1_CLK_ENABLE();
  GPIO_InitStructure.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_SetBits(GPIOB,GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

  SPI_InitStructure.Direction = SPI_DIRECTION_2LINES;
  SPI_InitStructure.Mode = SPI_MODE_MASTER;
  SPI_InitStructure.DataSize = SPI_DATASIZE_8BIT;
  SPI_InitStructure.CLKPolarity = SPI_POLARITY_LOW;
  SPI_InitStructure.CLKPhase = SPI_PHASE_1EDGE;
  SPI_InitStructure.NSS = SPI_NSS_SOFT;
  SPI_InitStructure.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  SPI_InitStructure.FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
  SPI2_ReadWriteByte(0xff);

  DMA_InitStructure.Request = DMA_REQUEST_0;
  DMA_InitStructure.Direction = DMA_PERIPH_TO_MEMORY; //DMA_MEMORY_TO_PERIPH
  DMA_InitStructure.PeriphInc = DMA_PINC_DISABLE;
  DMA_InitStructure.MemInc = DMA_MINC_ENABLE;
  DMA_InitStructure.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  DMA_InitStructure.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  DMA_InitStructure.Mode = DMA_NORMAL;
  DMA_InitStructure.Priority = DMA_PRIORITY_HIGH;
  DMA_Init(&DMA_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = PB_SPI_DMA_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_InitStructure.Direction = DMA_MEMORY_TO_PERIPH;
  DMA_InitStructure.Priority = DMA_PRIORITY_VERY_HIGH;
  DMA_Init(&DMA_InitStructure);

  DMA_InitStructure.Request = DMA_REQUEST_0;
  DMA_InitStructure.Direction = DMA_MEMORY_TO_PERIPH;
  DMA_InitStructure.Priority = DMA_PRIORITY_VERY_HIGH;
  DMA_Init(&DMA_InitStructure);

  DMA_ITConfig(PB_SPI_RX_DMA_Channel, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
}   
 
void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;
	SPI_Cmd(SPI2,ENABLE); 
} 

uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{		
	uint8_t retry=0;
	while(HAL_SPI_GetState(SPI2) == RESET)
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI2, TxData);
	retry=0;

	while(HAL_SPI_GetState(SPI2) == RESET)
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI2);				    
}

void	SendbyUSART1(void)
{	
  uint8_t	*sdbuf;
	haveRev += sendlen;
  if(haveRev < length)
	{	
		if(picbuf == Buf1)
		{		
			sdbuf = Buf1;	  picbuf = Buf2;	
		}
		else
		{
			sdbuf = Buf2;	  picbuf = Buf1;
		}
	  UART1_BulkOut(sendlen,sdbuf);
    noRev	= length - haveRev;		
		sendlen	= (noRev>=BUFFER_MAX_SIZE) ? BUFFER_MAX_SIZE : noRev;	
		DMA1_RX(picbuf, sendlen);	
	}
	else
	{
		UART1_BulkOut(sendlen, picbuf);
		send_OK = 1;
	}			 	 					 	 	
}
void SingleCapTransfer(void)
{
	flush_fifo();
	clear_fifo_flag();
	start_capture(); 
	while(!get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)){;}
	//printf("ACK CMD capture done\r\n");
	length= read_fifo_length();
	//printf("ACK CMD the length is %d\r\n",length);
	sendlen = (length>=BUFFER_MAX_SIZE) ? BUFFER_MAX_SIZE : length;
	picbuf = Buf1;
	haveRev = 0;
	DMA1_RX(picbuf, sendlen);
}

void StartBMPcapture(void)
{
	flush_fifo();
	clear_fifo_flag();
	start_capture(); 
	while(!get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)){;}
	//printf("ACK CMD capture done\r\n");
	length= read_fifo_length();
//	printf("ACK CMD the length is %d\r\n",length);		
	while(USART_GetFlagStatus(USART1, UART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xff);
	while(USART_GetFlagStatus(USART1, UART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xaa);
	for(int temp = 0; temp < BMPIMAGEOFFSET; temp++)
	{
		while(USART_GetFlagStatus(USART1, UART_FLAG_TXE) == RESET);
		USART_SendData(USART1, pgm_read_byte(&bmp_header[temp]));

	}
	CS_LOW();
	set_fifo_burst();
	char VH, VL;
	int i = 0, j = 0;
	for (i = 0; i < 240; i++)
	{
		for (j = 0; j < 320; j++)
		{
			VH = SPI2_ReadWriteByte(0x00);			
			VL = SPI2_ReadWriteByte(0x00);		
			while(USART_GetFlagStatus(USART1, UART_FLAG_TXE) == RESET);
			USART_SendData(USART1, VL);
			delay_us(15);
			while(USART_GetFlagStatus(USART1, UART_FLAG_TXE) == RESET);
			USART_SendData(USART1, VH);
			delay_us(15);
		}
	}
	while(USART_GetFlagStatus(USART1, UART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xbb);
	delay_us(12);
	while(USART_GetFlagStatus(USART1, UART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 0xcc);
	CS_HIGH();
}
