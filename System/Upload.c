#include "stm32f10x.h"                  // Device header
#include "ESP8266.h"
#include "Delay.h"
#include "RFID.h"
u8 send = 0; 

/*��ʱ���жϳ�ʼ��*/
void TIM4IT_Init(void)
{	
	/*����TIM����*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);			//����TIM4ʱ���ź�
	TIM_InternalClockConfig(TIM4);								//ѡ���ڲ�TIM4ʱ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//ѡ���Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//ѡ�����ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 30000 - 1;					//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;					//PSC ��ʱ���չ�ʽ �����һ��
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;				//�ر��ظ�������
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);							//ʹ��TIM4�ж�,��Ϊ�����ж�
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);								//��ֹ�����ͽ��ж�

	/*����NVIC���ȼ�*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);			//����������
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;			//ѡ��TIM4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;//������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4,ENABLE);									//ʹ��TIM4����
	
}

void TIM4IT_DeInit(void)
{
	TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);							//ʹ��TIM4�ж�,��Ϊ�����ж�
}

void TIM4_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)						//�жϱ�־λ�жϣ�ȷ�Ͻ����ж�
	{	
		send = 1;
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);

}


