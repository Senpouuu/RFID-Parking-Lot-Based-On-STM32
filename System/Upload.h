#ifndef __UPLOAD_H
#define __UPLOAD_H
#include "stm32f10x.h"                  // Device header

void TIM4IT_Init(void);
void TIM4IT_DeInit(void);
extern u8 send; 

#endif
