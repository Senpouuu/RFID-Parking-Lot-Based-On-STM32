#include "stm32f10x.h"                  // Device header
#include "ESP8266.h"
#include "Delay.h"
#include "RFID.h"
u8 send = 0; 

/*定时器中断初始化*/
void TIM4IT_Init(void)
{	
	/*配置TIM参数*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);			//开启TIM4时钟信号
	TIM_InternalClockConfig(TIM4);								//选择内部TIM4时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//选择分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//选择向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 30000 - 1;					//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;					//PSC 此时按照公式 五秒记一次
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;				//关闭重复计数器
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);							//使能TIM4中断,记为更新中断
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);								//防止开机就进中断

	/*配置NVIC优先级*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);			//二级组优先
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;			//选择TIM4中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//开启
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;//配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//配置响应优先级
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4,ENABLE);									//使能TIM4外设
	
}

void TIM4IT_DeInit(void)
{
	TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);							//使能TIM4中断,记为更新中断
}

void TIM4_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)						//中断标志位判断，确认进入中断
	{	
		send = 1;
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);

}


