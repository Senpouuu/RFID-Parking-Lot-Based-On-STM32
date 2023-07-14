#ifndef __RFID_H
#define __RFID_H


void RFID_Init(void);
char RFID_FindCard(void);
u8 RFID_SelectCard(void);
char RFID_WalletInit(void);
char RFID_Recharge(char* stat);
char RFID_Read(char* stat);
char RFID_DecRead(u32 Duct,u8* stat);
u8 RFID_manage_garge(u8 ID);
u8 RFID_Compare(u8* str1,u8* str2,u8 len);
char RFID_Garge(void);
u8 RFID_MoneyChangeToArr(u16 PMoney,u8* moneyArr);


extern u8 Card1[];
extern u8 Card2[];
extern u8 CardInit[];
extern u32 Money;
extern u8 Garge[];
#endif

