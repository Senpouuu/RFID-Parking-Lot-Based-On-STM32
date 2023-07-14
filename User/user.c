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
#include "Upload.h"
#include "ESP8266.h"
#include "Timer.h"
u32 distance;

/*按键1刷卡充值*/
void KEY1Recharge(void)
{
	if(Key1 == 1)
	{
		char status = PCD_ERR,stat = PCD_NOTAGERR;
		u8 i = 0;
		OLED_Clear();
		/*请放入您的停车卡*/
		OLED_ShowChinese(1,1,2);
		OLED_ShowChinese(1,2,3);
		OLED_ShowChinese(1,3,4);
		OLED_ShowChinese(1,4,5);
		OLED_ShowChinese(1,5,6);
		OLED_ShowChinese(1,6,7);
		OLED_ShowChinese(1,7,8);
		OLED_ShowChinese(1,8,9);
		KeyIT_SwiInit(DISABLE);
		while(stat == PCD_NOTAGERR)
		{
			status = RFID_Recharge(&stat);
			
			//switch (status){
			//	case PCD_OK:					printf("寻卡OK\r\n");break;
			//	case PCD_ERR:					printf("寻卡ERROR\r\n");break;
			//	case PCD_NOTAGERR:		printf("无卡\r\n");break;
			//}
			
			Delay_ms(100);
			i++;
			if(i == 50)
				break;
		}
		if(i == 50)
			BEEP_DiDiDi();
		if(status == PCD_OK)
		{
			OLED_Clear();
			/*充值成功*/
			OLED_ShowChinese(1,1,10);
			OLED_ShowChinese(1,2,11);
			OLED_ShowChinese(1,3,12);
			OLED_ShowChinese(1,4,13);
			BEEP_Di();
			Delay_ms(1000);
			OLED_Clear();
		}
		else if(status == PCD_ERR)
		{
			OLED_Clear();
			/*刷卡失败*/
			OLED_ShowChinese(1,1,32);
			OLED_ShowChinese(1,2,33);
			OLED_ShowChinese(1,3,34);
			OLED_ShowChinese(1,4,35);
			/*请检查您的卡片*/
			OLED_ShowChinese(2,1,36);
			OLED_ShowChinese(2,2,37);
			OLED_ShowChinese(2,3,38);
			OLED_ShowChinese(2,4,39);
			OLED_ShowChinese(2,5,40);
			OLED_ShowChinese(2,6,41);
			OLED_ShowChinese(2,7,42);
			OLED_ShowChinese(2,8,43);
			/*然后重新放入*/
			OLED_ShowChinese(3,1,44);
			OLED_ShowChinese(3,2,45);
			OLED_ShowChinese(3,3,46);
			OLED_ShowChinese(3,4,47);
			OLED_ShowChinese(3,5,48);
			OLED_ShowChinese(3,6,49);
			BEEP_DiDiDi();
			Delay_ms(1000);
			OLED_Clear();
		}
	}
	Key1 = 0;
	KeyIT_SwiInit(ENABLE);
}


void KEY2Search(void)
{
	if(Key2 == 1)
	{
		char status,stat = PCD_NOTAGERR;
		u8 i = 0;
		OLED_Clear();
		/*请放入您的停车卡*/
		OLED_ShowChinese(1,1,2);
		OLED_ShowChinese(1,2,3);
		OLED_ShowChinese(1,3,4);
		OLED_ShowChinese(1,4,5);
		OLED_ShowChinese(1,5,6);
		OLED_ShowChinese(1,6,7);
		OLED_ShowChinese(1,7,8);
		OLED_ShowChinese(1,8,9);
		KeyIT_SwiInit(DISABLE);
		while(stat == PCD_NOTAGERR)
		{
			status = RFID_Read(&stat);
			//
			//switch (status){
			//	case PCD_OK:					printf("寻卡OK\r\n");break;
			//	case PCD_ERR:					printf("寻卡ERROR\r\n");break;
			//	case PCD_NOTAGERR:		printf("无卡\r\n");break;
			//}
			//
			Delay_ms(100);
			i++;
			if(i == 50)
				break;
		}
		if(i == 50)
			BEEP_DiDiDi();
		if(status == PCD_OK)
		{
			OLED_Clear();
			/*卡上余额为*/
			OLED_ShowChinese(1,1,26);
			OLED_ShowChinese(1,2,27);
			OLED_ShowChinese(1,3,28);
			OLED_ShowChinese(1,4,29);
			OLED_ShowChinese(1,5,30);
			OLED_ShowNum(1,11,Money,4);
			OLED_ShowChinese(1,8,31);
			BEEP_Di();
			Delay_ms(1000);
			OLED_Clear();
		}
		else if(status == PCD_ERR)
		{
			OLED_Clear();
			/*刷卡失败*/
			OLED_ShowChinese(1,1,32);
			OLED_ShowChinese(1,2,33);
			OLED_ShowChinese(1,3,34);
			OLED_ShowChinese(1,4,35);
			/*请检查您的卡片*/
			OLED_ShowChinese(2,1,36);
			OLED_ShowChinese(2,2,37);
			OLED_ShowChinese(2,3,38);
			OLED_ShowChinese(2,4,39);
			OLED_ShowChinese(2,5,40);
			OLED_ShowChinese(2,6,41);
			OLED_ShowChinese(2,7,42);
			OLED_ShowChinese(2,8,43);
			/*然后重新放入*/
			OLED_ShowChinese(3,1,44);
			OLED_ShowChinese(3,2,45);
			OLED_ShowChinese(3,3,46);
			OLED_ShowChinese(3,4,47);
			OLED_ShowChinese(3,5,48);
			OLED_ShowChinese(3,6,49);
			BEEP_DiDiDi();
			Delay_ms(1000);
			OLED_Clear();
		}
	}
	Key2 = 0;
	KeyIT_SwiInit(ENABLE);
	

}

//u8 Garge[4] = {0,0,0,0};
void OLED_ShowGarge(void)
{
	/*车位1*/
	OLED_ShowChinese(1,1,0);
	OLED_ShowChinese(1,2,1);
	OLED_ShowString(1,5,"1:");
	if(Garge[0] != 0)
	{
		if(Garge[0] == 1)
			OLED_ShowString(1,9,"VIP1");
		else if(Garge[0] == 2)
			OLED_ShowString(1,9,"VIP2");
	}
	else
	{
		OLED_ShowChinese(1,5,20);
		OLED_ShowChinese(1,6,21);
	}
	
	/*车位2*/
	OLED_ShowChinese(2,1,0);
	OLED_ShowChinese(2,2,1);
	OLED_ShowString(2,5,"2:");
	if(Garge[1] != 0)
	{
		if(Garge[1] == 1)
			OLED_ShowString(2,9,"VIP1");
		else if(Garge[1] == 2)
			OLED_ShowString(2,9,"VIP2");
	}
	else
	{
		OLED_ShowChinese(2,5,20);
		OLED_ShowChinese(2,6,21);
	}

	/*车位3*/
	OLED_ShowChinese(3,1,0);
	OLED_ShowChinese(3,2,1);
	OLED_ShowString(3,5,"3:");

	if(Garge[2] != 0)
	{
		if(Garge[2] == 1)
			OLED_ShowString(3,9,"VIP1");
		else if(Garge[2] == 2)
			OLED_ShowString(3,9,"VIP2");
	}
	else
	{
		OLED_ShowChinese(3,5,20);
		OLED_ShowChinese(3,6,21);
	}
}
	
//Beep_TIM(2000,ENABLE);			//30
//Beep_TIM(1000,ENABLE);			//20
//BEEP_ON();									//10
void Wave_Beep(void)
{
	distance = Wave_Distance();
	
	if(distance > 300)
	{
		BEEP_OFF();
	}
	
	else if(distance > 200)
	{
		Beep300();
	}	
	
	else if(distance > 100)
	{	
		Beep200();
	}
	
	else if(distance < 100)
	{
		Beep100();
	}			


}


void DataUpload(void)
{
	if(send == 1)
	{
		Delay_ms(50);
		ESP8266_Send(0,"Garge1",Garge[0]);
		
		Delay_ms(50);
		ESP8266_Send(0,"Garge2",Garge[1]);
		
		Delay_ms(50);
		ESP8266_Send(0,"Garge3",Garge[2]);
		
		Delay_ms(50);
		ESP8266_Send(0,"Time1",second[0]);
		
		Delay_ms(50);
		ESP8266_Send(0,"Time2",second[1]);
		
		Delay_ms(50);
		ESP8266_Send(0,"Time3",second[2]);
		
		Delay_ms(50);
		ESP8266_Send(0,"Distance",distance/10);
		
		send = 0;
	}
}
