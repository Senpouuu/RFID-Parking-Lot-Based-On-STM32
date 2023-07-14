#include "stm32f10x.h"                  // Device header
#include "Delay.h"
void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

void BEEP_ON(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}


void BEEP_OFF(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}

void BEEP_Di(void)
{
	BEEP_ON();
	Delay_ms(100);
	BEEP_OFF();
}

void BEEP_DiDi(void)
{
	BEEP_ON();
	Delay_ms(100);
	BEEP_OFF();
	Delay_ms(100);
	BEEP_ON();
	Delay_ms(100);
	BEEP_OFF();
}

void BEEP_DiDiDi(void)
{
	BEEP_ON();
	Delay_ms(100);
	BEEP_OFF();
	Delay_ms(100);
	BEEP_ON();
	Delay_ms(100);
	BEEP_OFF();
	Delay_ms(100);
	BEEP_ON();
	Delay_ms(100);
	BEEP_OFF();
	
}



void Beep300(void)
{
	BEEP_ON();
	Delay_ms(200);
	BEEP_OFF();

}

void Beep200(void)
{
	BEEP_ON();
	Delay_ms(100);
	BEEP_OFF();

}

void Beep100(void)
{
	BEEP_ON();
}
 void BEEP_Turn(void)
{
	/*如果是低电平，就给他一个相反的信号，高电平同理*/
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
	else
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
}



