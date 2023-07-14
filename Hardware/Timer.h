#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"                  // Device header

void TIM3_Init(void);
void TIM3Enable(void);
void TIM3Disable(u8 ID);

extern u32 second[];

#endif



