#include "stm32f10x.h"                  // Device header
#include "RC522.h"
#include "SPI.h"
#include "stdio.h"
#include "OLED.h"
#include "Delay.h"
#include "RFID.h"
#include "BEEP.h"
#include "KEY.h"
#include "Serial.h"
#include "String.h"
#include "Timer.h"

u8 Card1[4] = {0xc1,0x36,0x8b,0x1d};
u8 Card2[4] = {0x6b,0x66,0x6f,0x11};
u8 CardInit[4] = {0x0,0x0,0x0,0x0};
u32 Money = 0;

char Hex_str[9];

//���⣬�����±�Ϊ����ţ�����ֵΪ����
u8 Garge[4] = {0,0,0,0};


void RFID_Init(void)
{
	HardwareSPI_Init();
	MFRC_Init();
	PCD_Init();
}



char RFID_FindCard(void)
{
	char status = PCD_ERR;
	PCD_Reset();
	PCD_Request(0x26,&CardType);		//Ѱ��
	status = PCD_Anticoll(CardID);						//����ײ��������ID 
	return status;
}


u8 RFID_SelectCard(void)
{
	char status;
	status = RFID_FindCard();
	if(status == PCD_OK)
	{
		if(RFID_Compare(CardID,Card1,4) == 1)
		{
			memset(CardID,0x00,4);
			return 1;
		}
		
		else if(RFID_Compare(CardID,Card2,4) == 1)
		{
			memset(CardID,0x00,4);
			return 2;
		}
			
		else if(status == PCD_OK)
		{
			return 0; //�Ƿ���
		}
	}
	return 3;
}


char RFID_Garge(void)
{
	char status = PCD_ERR;
	status = RFID_FindCard();
	if(status == PCD_OK)
	{
		KeyIT_SwiInit(DISABLE);
		u8 ID;
	//Card1
		if(RFID_SelectCard() == 1)
		{	
			BEEP_Di();
			ID = 1;
			status = PCD_Select(CardID);		//ѡ��
			RFID_manage_garge(ID);
			memset(CardID,0x00,4);
		}		
	
		//Card2
		else if(RFID_SelectCard() == 2)
		{
			BEEP_Di();
			ID = 2;
			status = PCD_Select(CardID);		//ѡ��
			RFID_manage_garge(ID);
			memset(CardID,0x00,4);
		}
		
		else if(status == PCD_OK)
		{
			BEEP_DiDiDi();
			memset(CardID,0x00,4);
		}
	
	}
	KeyIT_SwiInit(ENABLE);
	return status;
}
	

char RFID_WalletInit(void)
{
	char status = PCD_ERR;
	RFID_FindCard();
	status = PCD_WriteBlock(1,Wallet_Init);
	return status;
}


char RFID_Recharge(char* stat)
{
	memset(CardID,0x00,4);
	char status = PCD_ERR;
	*stat = RFID_FindCard();
	PCD_Select(CardID);		//ѡ��
	PCD_AuthState(PICC_AUTHENT1A,2,KeyA,CardID);									//��֤
	status = PCD_Value(PICC_INCREMENT,2,Recharge_Value);
	return status;
}


char RFID_DecRead(u32 Duct,u8* stat)
{
	Money = 0;
	memset(CardID,0x00,4);
	memset(RData,0x00,4);
	char status = PCD_ERR;
	RFID_FindCard();
	PCD_Select(CardID);		//ѡ��
	PCD_AuthState(PICC_AUTHENT1A,2,KeyA,CardID);									//��֤
	status = PCD_ReadBlock(2,RData);
	Money = RData[0]+RData[1]*256u+RData[2]*65536u+RData[3]*4294967295u;
	if(Money < Duct)
		*stat = 0;
	else
		*stat = 1;
	return status;
}


char RFID_Deduction(void)
{
	memset(CardID,0x00,4);
	char status = PCD_ERR;
	RFID_FindCard();
	PCD_Select(CardID);		//ѡ��
	PCD_AuthState(PICC_AUTHENT1A,2,KeyA,CardID);									//��֤
	status = PCD_Value(PICC_DECREMENT,2,Deduction_Value);
	return status;
}


char RFID_Read(char* stat)
{
	Money = 0;
	memset(CardID,0x00,4);
	memset(RData,0x00,4);
	char status = PCD_ERR;
	*stat = RFID_FindCard();
	PCD_Select(CardID);		//ѡ��
	PCD_AuthState(PICC_AUTHENT1A,2,KeyA,CardID);									//��֤
	status = PCD_ReadBlock(2,RData);
	Money = RData[0]+RData[1]*256u+RData[2]*65536u+RData[3]*4294967295u;
	return status;
}




u8 RFID_Compare(u8* str1,u8* str2,u8 len)
{
	int i;
	for(i = 0;i< len;i++)
	{
		if(str1[i] == str2[i])
			continue;
		else
			return 0;
	}
	return 1;

}

/*parameter:	u16 PMoney:��ֵ�����ѽ��
			u8* moneyArr����Ǯ����ַ������һ����ʽ����Ҫ����Ǯ����ʽд�������*/
u8 RFID_MoneyChangeToArr(u16 PMoney,u8* moneyArr)
{
	u8 i=0,temp;
	do
	{
		moneyArr[i]=PMoney%16;
		PMoney=PMoney/16;
		i++;
	}while(PMoney!=0); //ת��ʮ������
	temp=moneyArr[1];
	moneyArr[1]=moneyArr[0];
	moneyArr[0]=temp;  //���õ�ֵ���н���
	temp=moneyArr[3];
	moneyArr[3]=moneyArr[2];
	moneyArr[2]=temp;  //���õ�ֵ���н���
	moneyArr[0]=moneyArr[0]<<4|moneyArr[1];
	moneyArr[1]=moneyArr[2]<<4|moneyArr[3];  //Ǯ�������Сֵ������ߣ����ֵ1,{0x01,0x00,0x00,0x00}
	moneyArr[2]=0;
	moneyArr[3]=0;  //��������ֵ50000�����Ժ�����λ��0
	return MI_OK;
}


//�����������1��ʾȡ��������2��ʾ�泵
u8 RFID_manage_garge(u8 ID)
{
    //����һ����������ʾͣ����ȡ����״̬��0����ʧ�ܣ�1����ɹ�
    u8 status = 0;
		u16 time = 0;
    //����һ����������ʾ��λ�ţ�-1������Ч
    int slot = -1;
		u8 stat = 0;
    //�������飬�����Ƿ�����RFID������ͬ�ĳ�λ
    for(u8 i = 0; i < 4; i++){
        if(Garge[i] == ID){
            slot = i;
            break;
        }
    }
    //����ҵ�����ͬ�ĳ�λ��˵����ȡ������
    if(slot != -1){
        //���ó�λ��Ϊ0����ʾ����
        Garge[slot] = 0;
				time = second[slot];
				TIM3Disable(slot);
				while(RFID_DecRead(time*2,&stat) == PCD_ERR);
				if(stat == 1)
				{
					RFID_MoneyChangeToArr(time*2,Deduction_Value);
					while(RFID_Deduction() == PCD_ERR);
					//��״̬��Ϊ1����ʾ�ɹ�
					status = 1;
					
					OLED_Clear();
					/*����ͣ��XX��*/
					OLED_ShowChinese(1,1,60);
					OLED_ShowChinese(1,2,61);
					OLED_ShowChinese(1,3,62);
					OLED_ShowChinese(1,4,63);
					OLED_ShowNum(1,9,time,3);
					OLED_ShowChinese(1,7,65);
					
					
					/*�շ�XXԪ*/
					OLED_ShowChinese(2,1,66);
					OLED_ShowChinese(2,2,67);
					OLED_ShowNum(2,5,time*2,3);
					OLED_ShowChinese(2,5,64);
		
					
					/*ף��һ·˳��*/
					OLED_ShowChinese(3,1,50);
					OLED_ShowChinese(3,2,51);
					OLED_ShowChinese(3,3,52);
					OLED_ShowChinese(3,4,53);
					OLED_ShowChinese(3,5,54);
					OLED_ShowChinese(3,6,55);
				
					Delay_ms(1000);
		
					OLED_Clear();
				}
				else
				{
					OLED_Clear();

					/*��������*/
					OLED_ShowChinese(1,1,74);
					OLED_ShowChinese(1,2,75);
					OLED_ShowChinese(1,3,76);
					OLED_ShowChinese(1,4,77);
					OLED_ShowChinese(1,5,78);
					OLED_ShowChinese(1,6,79);
					/*��������ֵ*/
					OLED_ShowChinese(2,1,80);
					OLED_ShowChinese(2,2,81);
					OLED_ShowChinese(2,3,82);
					OLED_ShowChinese(2,4,83);
					OLED_ShowChinese(2,5,84);
					OLED_ShowChinese(2,6,85);
					OLED_ShowChinese(2,6,86);
					Delay_ms(1000);
					OLED_Clear();
				}
					
    }
	
    //���û���ҵ���ͬ�ĳ�λ��˵����ͣ������
    else{
        //����һ����������ʾ�Ƿ��ҵ����еĳ�λ
        u8 found = 0;
        //�������飬�����Ƿ��п��еĳ�λ
        for(u8 i = 0; i < 4; i++){
            if(Garge[i] == 0){
                found = 1;
                break;
            }
        }
		
        //����ҵ��˿��еĳ�λ
        if(found == 1){
            //��˳���ҵ���һ�����еĳ�λ��
            for(u8 i = 0; i < 4; i++){
                if(Garge[i] == 0){
                    slot = i;
                    break;
                }
            }
            //���ó�λ��ΪRFID���ţ���ʾռ��
            Garge[slot] = ID;
            //��״̬��Ϊ1����ʾ�ɹ�
            status = 2;
						OLED_Clear();
						if(ID == 1)
							OLED_ShowString(1,1,"VIP1");
						else
							OLED_ShowString(1,1,"VIP2");
						/*��ӭ����*/
						OLED_ShowChinese(1,4,56);
						OLED_ShowChinese(1,5,57);
						OLED_ShowChinese(1,6,58);
						OLED_ShowChinese(1,7,59);
						
						/*���ĳ�λ��XX��*/
						OLED_ShowChinese(2,1,68);
						OLED_ShowChinese(2,2,69);
						OLED_ShowChinese(2,3,70);
						OLED_ShowChinese(2,4,71);
						OLED_ShowChinese(2,5,72);
						OLED_ShowNum(2,11,slot+1,2);
						OLED_ShowChinese(2,7,73);
						
						TIM3Enable();
						Delay_ms(1000);
						OLED_Clear();
						
        }
        //���û���ҵ����еĳ�λ
        else{
						OLED_Clear();
						OLED_ShowString(1,1,"No Park!");
						Delay_ms(1000);
						OLED_Clear();
        }
    }
    return status;
}
