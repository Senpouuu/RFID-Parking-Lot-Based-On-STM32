#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"                  // Device header

void SoftwareSPI_Init(void);
void SPI_Start(void);
void SPI_Stop(void);
uint8_t SoftwareSPI_SwapByteMode0(uint8_t ByteSend);
uint8_t SoftwareSPI_SwapByteMode1(uint8_t ByteSend);
uint8_t SoftwareSPI_SwapByteMode2(uint8_t ByteSend);
uint8_t SoftwareSPI_SwapByteMode3(uint8_t ByteSend);

void HardwareSPI_Init(void);
uint8_t HardwareSPI_SwapByte(uint8_t ByteSend);

#endif                                             

