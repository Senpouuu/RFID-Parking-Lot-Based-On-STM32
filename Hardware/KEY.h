#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"                  // Device header

void Key_Init(void);
uint8_t Key_GetNum(void);
void KeyIT_Init(void);
void KeyIT_SwiInit(FunctionalState i);

extern u8 Key1;
extern u8 Key2;
#endif

