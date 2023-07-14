#include "stm32f10x.h"                  // Device header

//SPI1_SCK 				PA5
//SPI1_MISO				PA6
//SPI1_MOSI 			PA7
//RCC522_RST(CE)  PA3
//RCC522_NSS(SDA��PA4
//RCC522_IRQ 			����

//дƬѡ
void SoftwareSPI_W_CS(uint8_t BitVaule)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitVaule);
}


//дʱ��
void SoftwareSPI_W_SCK(uint8_t BitVaule)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)BitVaule);
}


//д���
void SoftwareSPI_W_MOSI(uint8_t BitVaule)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)BitVaule);
}

//������
uint8_t SoftwareSPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
}


void SoftwareSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;			//���/�ӻ����룬ʱ�ӣ�Ƭѡ
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);

	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_6;					//����/�ӻ����
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	SoftwareSPI_W_CS(1);	//Ĭ�ϲ�Ƭѡ
	SoftwareSPI_W_SCK(0);	//ʱ��Ĭ��ģʽ0������ʱΪ�͵�ƽ��

}

void HardwareSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;										//CS��RST
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
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//ʱ��32��Ƶ
	//ͨ������CPHA��CPOL��ֵ��������SPIģʽ����ʱΪMODE0
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;													//�ڵ�һ��ʱ�ӱ��ؽ������ݲ���
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;														//SPIģʽ0��ʱ��Ĭ�ϵ͵�ƽ
	SPI_InitStructure.SPI_CRCPolynomial = 7;															//8Ϊ���ݲ���8λУ��λ					
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;											//һ�δ�8λ
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//ȫ˫��ģʽ
	SPI_InitStructure.SPI_FirstBit = 	SPI_FirstBit_MSB;										//��λ����
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;													//SPI����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;															//������÷�����ģʽ
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	
	SoftwareSPI_W_CS(1);	//Ĭ�ϲ�Ƭѡ
}



void SPI_Start(void)
{
	SoftwareSPI_W_CS(0);
}

void SPI_Stop(void)
{
	SoftwareSPI_W_CS(1);
}

/*SPI��������
�����ǰ�˳��ִ�еģ�
����CS�͵�ƽ�������ӻ�ͬʱ�Ƴ����ݣ�
Ȼ��SCK�ߵ�ƽ�������ӻ�ͬʱ�Ƴ�����
Ȼ��SCK�͵�ƽ�������ӻ��ٴ�ͬʱ�Ƴ����ݣ�
����ѭ��������ȥ��*/
uint8_t SoftwareSPI_SwapByteMode0(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	for(i = 0;i<8;i++)
	{
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//�������������λ�Ƶ�MOSI�ϣ���ʱ�ӻ����Զ��Ƴ�
		SoftwareSPI_W_SCK(1);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//������λΪ1�����룬���򱣳�
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
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//�������������λ�Ƶ�MOSI�ϣ���ʱ�ӻ����Զ��Ƴ�
		SoftwareSPI_W_SCK(0);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//������λΪ1�����룬���򱣳�	
	}
	return ByteReceive;
}

uint8_t SoftwareSPI_SwapByteMode2(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	for(i = 0;i<8;i++)
	{
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//�������������λ�Ƶ�MOSI�ϣ���ʱ�ӻ����Զ��Ƴ�
		SoftwareSPI_W_SCK(0);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//������λΪ1�����룬���򱣳�
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
		SoftwareSPI_W_MOSI(ByteSend & (0x80>>i));		//�������������λ�Ƶ�MOSI�ϣ���ʱ�ӻ����Զ��Ƴ�
		SoftwareSPI_W_SCK(1);
		if(SoftwareSPI_R_MISO() == 1) 	ByteReceive |= (0x80>>i);		//������λΪ1�����룬���򱣳�
	} 
	return ByteReceive;
}


uint8_t HardwareSPI_SwapByte(uint8_t ByteSend)
{	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);		// �ȴ����ͻ�����Ϊ�գ���ʾ���Է�������
	SPI_I2S_SendData(SPI1, ByteSend);																// �������ݵ� SPI ����														
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);	// �ȴ����ջ������ǿգ���ʾ���Զ�ȡ���յ�������
	return SPI_I2S_ReceiveData(SPI1);																//���ؽ��յ�������
}
