#include "stm32f10x.h"                  // Device header

//SPI1_SCK 				PA5
//SPI1_MISO				PA6
//SPI1_MOSI 			PA7
//RCC522_RST(CE)  PA3
//RCC522_NSS(SDA）PA4
//RCC522_IRQ 			悬空

//写片选
void SoftwareSPI_W_CS(uint8_t BitVaule)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitVaule);
}


//写时钟
void SoftwareSPI_W_SCK(uint8_t BitVaule)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)BitVaule);
}


//写输出
void SoftwareSPI_W_MOSI(uint8_t BitVaule)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)BitVaule);
}

//读输入
uint8_t SoftwareSPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
}


void SoftwareSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;			//输出/从机输入，时钟，片选
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);

	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_6;					//输入/从机输出
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	SoftwareSPI_W_CS(1);	//默认不片选
	SoftwareSPI_W_SCK(0);	//时钟默认模式0，空闲时为低电平。

}

void HardwareSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;										//CS、RST
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;			//SCK MOSI
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;								//MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//时钟32分频
	//通过设置CPHA和CPOL的值可以设置SPI模式，此时为MODE0
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;													//在第一个时钟边沿进行数据捕获
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;														//SPI模式0，时钟默认低电平
	SPI_InitStructure.SPI_CRCPolynomial = 7;															//8为数据采用8位校验位					
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;											//一次传8位
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//全双工模式
	SPI_InitStructure.SPI_FirstBit = 	SPI_FirstBit_MSB;										//高位先行
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;													//SPI主机
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;															//软件设置非连续模式
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	
	SoftwareSPI_W_CS(1);	//默认不片选
}



void SPI_Start(void)
{
	SoftwareSPI_W_CS(0);
}

void SPI_Stop(void)
{
	SoftwareSPI_W_CS(1);
}

/*SPI交换数据
程序是按顺序执行的，
首先CS低电平，主机从机同时移出数据，
然后SCK高电平，主机从机同时移出数据
然后SCK低电平，主机从机再次同时移出数据，
依次循环进行下去。*/
uint8_t SoftwareSPI_SwapByteMode0(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	for(i = 0;i<8;i++)
	{
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//主机将数据最高位移到MOSI上，这时从机会自动移出
		SoftwareSPI_W_SCK(1);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//如果最高位为1，移入，否则保持
		SoftwareSPI_W_SCK(0);

	}
	return ByteReceive;
}


uint8_t SoftwareSPI_SwapByteMode1(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	for(i = 0;i<8;i++)
	{
		SoftwareSPI_W_SCK(1);
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//主机将数据最高位移到MOSI上，这时从机会自动移出
		SoftwareSPI_W_SCK(0);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//如果最高位为1，移入，否则保持	
	}
	return ByteReceive;
}

uint8_t SoftwareSPI_SwapByteMode2(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	for(i = 0;i<8;i++)
	{
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//主机将数据最高位移到MOSI上，这时从机会自动移出
		SoftwareSPI_W_SCK(0);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//如果最高位为1，移入，否则保持
		SoftwareSPI_W_SCK(1);

	}
	return ByteReceive;
}

uint8_t SoftwareSPI_SwapByteMode3(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	for(i = 0;i<8;i++)
	{
		SoftwareSPI_W_SCK(0);
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//主机将数据最高位移到MOSI上，这时从机会自动移出
		SoftwareSPI_W_SCK(1);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//如果最高位为1，移入，否则保持
	} 
	return ByteReceive;
}


uint8_t HardwareSPI_SwapByte(uint8_t ByteSend)
{	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);		// 等待发送缓冲区为空，表示可以发送数据
	SPI_I2S_SendData(SPI1, ByteSend);																// 发送数据到 SPI 外设														
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);	// 等待接收缓冲区非空，表示可以读取接收到的数据
	return SPI_I2S_ReceiveData(SPI1);																//返回接收到的数据
}
