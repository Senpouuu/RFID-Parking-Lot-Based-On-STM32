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

/*连接OneNet*/
void ESP8266_Init(void)
{
	Serial_Init();
	while(1)
	{
		if (strstr((const char*)Serial_RxPacket,"WIFI GOT IP"))				//等待wifi连接
		{
				memset(Serial_RxPacket,0,500);														//接收区清零
				break;
		}
	}
	Delay_ms(2000);
	
	memset(Serial_RxPacket,0,500);
	printf("AT+IOTSTATUS\r\n");							//配置地址
	Delay_ms(2000);
	
	memset(Serial_RxPacket,0,500);
	printf("AT+IOTCFG=%s,%s,%s\r\n",DeviceID,ProductID,Device_AuthID);							//配置地址
	Delay_ms(2000);

	
}


/*ESP发送数据
指令AT+IOTSEND=a,b,c,(d)
a：0：数据是数值类型
1：数据是字符串类型
2：数据是gps
b：数据流名
c：数据值
d：数据值 (只有上传GPS时才会用到)*/
void ESP8266_Send(uint8_t a,char* b,float c)
{
		printf("AT+IOTSEND=%d,%s,%.1f\r\n",a,b,c);												
}



