#include "stm32f10x.h"                  // Device header
#include <string.h>
#include <stdio.h>
#include "Serial.h"
#include "ESP8266.h"
#include "Delay.h"
#include "OLED.h"

const char* ProductID="571107";
const char* UserID="316452";
const char* DeviceID="1081152794";
const char* Device_AuthID="garge";

/*����OneNet*/
void ESP8266_Init(void)
{
	Serial_Init();
	while(1)
	{
		if (strstr((const char*)Serial_RxPacket,"WIFI GOT IP"))				//�ȴ�wifi����
		{
				memset(Serial_RxPacket,0,500);														//����������
				break;
		}
	}
	Delay_ms(2000);
	
	memset(Serial_RxPacket,0,500);
	printf("AT+IOTSTATUS\r\n");							//���õ�ַ
	Delay_ms(2000);
	
	memset(Serial_RxPacket,0,500);
	printf("AT+IOTCFG=%s,%s,%s\r\n",DeviceID,ProductID,Device_AuthID);							//���õ�ַ
	Delay_ms(2000);

	
}


/*ESP��������
ָ��AT+IOTSEND=a,b,c,(d)
a��0����������ֵ����
1���������ַ�������
2��������gps
b����������
c������ֵ
d������ֵ (ֻ���ϴ�GPSʱ�Ż��õ�)*/
void ESP8266_Send(uint8_t a,char* b,float c)
{
		printf("AT+IOTSEND=%d,%s,%.1f\r\n",a,b,c);												
}



