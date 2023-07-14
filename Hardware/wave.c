#include "stm32f10x.h"                  // Device header
#include "wave.h"
#include "Delay.h"
#include "OLED.h"

uint32_t ultra_value;
u32 msCount=0;

void Wave_Start(void)
{
		GPIO_SetBits(GPIOA, GPIO_Pin_1); //TRIG�ø�
		Delay_us(20); //��ʱ20us
		GPIO_ResetBits(GPIOA, GPIO_Pin_1); //TRIG�õ�
}
	

void Wave_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	/*TRIG*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	/*ECHO*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	/*����TIMʱ����Ԫ����*/
	TIM_DeInit(TIM2);
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//ѡ���Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//ѡ�����ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 1000;					//1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;					
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = DISABLE;			//�ر��ظ�������
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);							//ʹ��TIM2�ж�,��Ϊ�����ж�
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);		
	/*��ʼ��TIM*/
	TIM_Cmd(TIM2,DISABLE);
	
	/*����NVIC*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}


float Wave_Avg(float x)
{
	float sum;
	for(int i = 1;i < 10;i++)
		sum = sum + x;
	return sum*0.1;
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{
		msCount++;
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
}


float Wave_Distance(void)
{
	u32 Count;
	Wave_Start();
	float Distance = 0;
	
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == 0);	//���ο�ʼ
	
	TIM_SetCounter(TIM2,0);//��ռ�����
	msCount = 0;//����жϼ�����ֵ
	TIM_Cmd(TIM2,ENABLE);

	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == 1);	//���ν���
	TIM_Cmd(TIM2,DISABLE);
	
	Count = msCount * 1000;//us = ms * 1000
	Count = Count + TIM_GetCounter(TIM2);//�ߵ�ƽʱ��
	
	if(Count>120  && Count < 27000)
			Distance = Count*0.172;

	
	Delay_ms(100);
	
	return Distance;
}
	


