#include "stm32f10x.h"                  // Device header

/*LED使能*/
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_All);	//默认全灭
	
}

void LED0_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}

void LED3_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
}
void LED4_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

void LED5_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}

void LED6_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}

void LED7_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}


void LED1_Turn(void)
{
	/*如果LED是低电平，就给他一个相反的信号，高电平同理*/
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 0)
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET);
	else
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET);
}

void LED2_Turn(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == 0)
		GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_SET);
	else
		GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_RESET);
}



void LED0_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}

void LED3_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
}
void LED4_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

void LED5_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
}

void LED6_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
}

void LED7_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
}





