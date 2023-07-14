#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "Delay.h"
#include "OLED.h"
#include "wave.h"
#include "Serial.h"
#include "RC522.h"
#include "SPI.h"
#include "BEEP.h"
#include "RFID.h"
#include "KEY.h"
#include "Timer.h"
#include "user.h"
#include "ESP8266.h"
#include "Upload.h"

char* Vip1 = "c1368b1d";
char* Vip2 = "6b666f11";
char stat;

int main(void)
{	
	OLED_Init();
	OLED_ShowChinese(2,2,87);
	OLED_ShowChinese(2,3,88);
	OLED_ShowChinese(2,4,89);
	OLED_ShowChinese(2,5,90);
	OLED_ShowChinese(2,6,91);
	OLED_ShowChinese(2,7,92);
	ESP8266_Init();
	OLED_Clear();
	HardwareSPI_Init();
	MFRC_Init();
	PCD_Init();
	Beep_Init();
	KeyIT_Init();
	TIM3_Init();
	Wave_Init();
	TIM4IT_Init();
	BEEP_OFF();
	
	//Beep_TIM(2000,ENABLE);			//30
	//Beep_TIM(1000,ENABLE);			//20
	//BEEP_ON();									//10
	
	while(1)
	{
		KEY1Recharge();
		KEY2Search();
		Wave_Beep();
		//RFID_Read(&stat);
		//switch (stat){
		//	case PCD_OK:					printf("—∞ø®OK\r\n");break;
		//	case PCD_ERR:					printf("—∞ø®ERROR\r\n");break;
		//	case PCD_NOTAGERR:		printf("Œﬁø®\r\n");break;
		//}

		
		if(RFID_Garge() == PCD_OK)
			printf("CardID1:0x%x 0x%x 0x%x 0x%x\r\n",CardID[0],CardID[1],CardID[2],CardID[3]);
		OLED_ShowGarge();
		/*æ‡¿Î*/
		OLED_ShowChinese(4,1,24);
		OLED_ShowChinese(4,2,25);
		OLED_ShowString(4,5," :");
		OLED_ShowNum(4,9,distance/10,3);
		OLED_ShowChar(4,12,'.');
		OLED_ShowNum(4,13,distance%1000,1);
		OLED_ShowString(4,14,"cm");
		//if(RFID_Read() == PCD_OK)
		//{
		//	for(int i = 0;i < 16; i++)
		//		printf("%x ",RData[i]);
		//	printf("%d",Money);
		//	printf("\r\n");
		//}
		DataUpload();
	
		
			
		
		
	}

}


