#include "stm32f10x.h"                  // Device header
#include "RFID.h"
#include "String.h"
u32 second[4] = {0,0,0,0};

void TIM3_Init(void)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	/*����TIMʱ����Ԫ����*/
	TIM_DeInit(TIM3);
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//ѡ���Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//ѡ�����ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 10000;					//1000ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;					
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = DISABLE;			//�ر��ظ�������
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);							//ʹ��TIM3�ж�,��Ϊ�����ж�
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);		
	/*��ʼ��TIM*/
	TIM_Cmd(TIM3,DISABLE);
	
	/*����NVIC*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
}

void TIM3Disable(u8 ID)
{
	second[ID] = 0;
	TIM_SetCounter(TIM3,0);//��ռ�����
	//TIM_Cmd(TIM3,DISABLE);
}

void TIM3Enable(void)
{
	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		if(Garge[0] != 0)
			second[0]++;
		
		if(Garge[1] != 0)
			second[1]++;
		
		if(Garge[2] != 0)
			second[2]++;
		
		if(Garge[3] != 0)
			second[3]++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
}

