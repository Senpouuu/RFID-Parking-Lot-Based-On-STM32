#include "stm32f10x.h"                  // Device header
#include "RC522.h"
#include "SPI.h"
#include "Serial.h"
#include "stdio.h"
#include "Delay.h"
#include "String.h"

#define RS522_RST(N) GPIO_WriteBit(GPIOA, GPIO_Pin_3, N==1?Bit_SET:Bit_RESET) 
#define RS522_NSS(N) GPIO_WriteBit(GPIOA, GPIO_Pin_4, N==1?Bit_SET:Bit_RESET)

u8 CardType = 0;
const uint8_t KeyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
u8 CardID[4];
u8 RData[16];
u8 WData[16] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	//HELLOWORLD
u8 Wallet_Init[16]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0xFE,0x01,0xFE};							//Value Valueȡ�� Value ��ַ
u8 Recharge_Value[4] = {0x0A, 0x00, 0x00, 0x00};																																					//��ֵ10
u8 Deduction_Value[4] = {0x05, 0x00, 0x00, 0x00};																																					//��ֵ10

//SPI1_SCK 				PA5
//SPI1_MISO				PA6
//SPI1_MOSI 			PA7
//RCC522_RST(CE)  VCC
//RCC522_NSS(SDA��PA4
//RCC522_IRQ 			����

void MFRC_Init(void)
{
    RS522_NSS(1);
    RS522_RST(1);
}


/**************************************************************************************
* �������ƣ�MFRC_WriteReg
* ����������дһ���Ĵ���
* ��ڲ�����-addr:��д�ļĴ�����ַ
*           -data:��д�ļĴ�������
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
***************************************************************************************/
void MFRC_WriteReg(uint8_t addr, uint8_t data)
{
    uint8_t AddrByte;
    AddrByte = (addr << 1 ) & 0x7E; //�����ַ�ֽ�
    RS522_NSS(0);                   //NSS����
    HardwareSPI_SwapByte(AddrByte);         //д��ַ�ֽ�
    HardwareSPI_SwapByte(data);             //д����
    RS522_NSS(1);                   //NSS����
}


/**************************************************************************************
* �������ƣ�MFRC_ReadReg
* ������������һ���Ĵ���
* ��ڲ�����-addr:�����ļĴ�����ַ
* ���ڲ�������
* �� �� ֵ��-data:�����Ĵ���������
* ˵    ������
***************************************************************************************/
uint8_t MFRC_ReadReg(uint8_t addr)
{
    uint8_t AddrByte, data;
    AddrByte = ((addr << 1 ) & 0x7E ) | 0x80;   //�����ַ�ֽ�
    RS522_NSS(0);                               //NSS����
    HardwareSPI_SwapByte(AddrByte);             //д��ַ�ֽ�
    data = HardwareSPI_SwapByte(0x00);          //������
    RS522_NSS(1);                               //NSS����
    return data;
}

/**************************************************************************************
* �������ƣ�MFRC_SetBitMask
* �������������üĴ�����λ
* ��ڲ�����-addr:�����õļĴ�����ַ
*           -mask:�����üĴ�����λ(��ͬʱ���ö��bit)
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
***************************************************************************************/
void MFRC_SetBitMask(uint8_t addr, uint8_t mask)
{
    uint8_t temp = 0x0;
    temp = MFRC_ReadReg(addr);                  //�ȶ��ؼĴ�����ֵ
    MFRC_WriteReg(addr, temp | mask);           //�������������д��Ĵ���
}

/**************************************************************************************
* �������ƣ�MFRC_ClrBitMask
* ��������������Ĵ�����λ
* ��ڲ�����-addr:������ļĴ�����ַ
*           -mask:������Ĵ�����λ(��ͬʱ������bit)
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
***************************************************************************************/
void MFRC_ClrBitMask(uint8_t addr, uint8_t mask)
{
    uint8_t temp;
    temp = MFRC_ReadReg(addr);                  //�ȶ��ؼĴ�����ֵ
    MFRC_WriteReg(addr, temp & ~mask);          //�������������д��Ĵ���
}


/**************************************************************************************
* �������ƣ�MFRC_Calulate 
* ������������MFRC����CRC���
* ��ڲ�����-pInData��������CRC���������
*           -len��������CRC��������ݳ���
*           -pOutData��CRC������
* ���ڲ�����-pOutData��CRC������
* �� �� ֵ����
* ˵    ������
***************************************************************************************/
void MFRC_CalulateCRC(uint8_t *pInData, uint8_t len, uint8_t *pOutData)
{
    //0xc1 1        2           pInData[2]
    uint8_t temp;
    u32 i;
    MFRC_ClrBitMask(MFRC_DivIrqReg, 0x04);                  //ʹ��CRC�ж�
    MFRC_WriteReg(MFRC_CommandReg, MFRC_IDLE);              //ȡ����ǰ�����ִ��
    MFRC_SetBitMask(MFRC_FIFOLevelReg, 0x80);               //���FIFO�����־λ
    for(i = 0; i < len; i++)                                //����CRC���������д��FIFO
    {
        MFRC_WriteReg(MFRC_FIFODataReg, *(pInData + i));
    }
    MFRC_WriteReg(MFRC_CommandReg, MFRC_CALCCRC);           //ִ��CRC����
    i = 100000;
    do
    {
        temp = MFRC_ReadReg(MFRC_DivIrqReg);                //��ȡDivIrqReg�Ĵ�����ֵ
        i--;
    }
    while((i != 0) && !(temp & 0x04));                      //�ȴ�CRC�������
    pOutData[0] = MFRC_ReadReg(MFRC_CRCResultRegL);         //��ȡCRC������
    pOutData[1] = MFRC_ReadReg(MFRC_CRCResultRegM);
}



/**************************************************************************************
* �������ƣ�MFRC_CmdFrame  IMPORTANT!!!
* ����������MFRC522��ISO14443A��ͨѶ������֡����
* ��ڲ�����-cmd��MFRC522������
*           -pIndata��MFRC522���͸�MF1�������ݵĻ������׵�ַ   1443-A 
*           -InLenByte���������ݵ��ֽڳ���
*           -pOutdata�����ڽ���MF1��Ƭ�������ݵĻ������׵�ַ
*           -pOutLenBit��MF1���������ݵ�λ����
* ���ڲ�����-pOutdata�����ڽ���MF1��Ƭ�������ݵĻ������׵�ַ
*           -pOutLenBit������MF1����������λ���ȵ��׵�ַ
* �� �� ֵ��-status���������(MFRC_OK��MFRC_NOTAGERR��MFRC_ERR)
* ˵    ������
***************************************************************************************/
char MFRC_CmdFrame(uint8_t cmd, uint8_t *pInData, uint8_t InLenByte, uint8_t *pOutData, uint16_t *pOutLenBit)
{
    uint8_t lastBits;
    uint8_t n;
    u32 i;
    char status = MFRC_ERR;
    uint8_t irqEn   = 0x00;
    uint8_t waitFor = 0x00;

    /*�����������ñ�־λ*/
    switch(cmd)
    {
        case MFRC_AUTHENT:                  //Mifare��֤
            irqEn = 0x12;
            waitFor = 0x10;                 //idleIRq�жϱ�־
            break;
        case MFRC_TRANSCEIVE:               //���Ͳ���������
            irqEn = 0x77;
            waitFor = 0x30;                 //RxIRq��idleIRq�жϱ�־
            break;
				default:
						break;
    }

    /*��������֡ǰ׼��*/
    MFRC_WriteReg(MFRC_ComIEnReg, irqEn | 0x80);    //���ж�
    MFRC_ClrBitMask(MFRC_ComIrqReg, 0x80);          //����жϱ�־λSET1
    MFRC_WriteReg(MFRC_CommandReg, MFRC_IDLE);      //ȡ����ǰ�����ִ��
    MFRC_SetBitMask(MFRC_FIFOLevelReg, 0x80);       //���FIFO�����������־λ

    /*��������֡*/
    for(i = 0; i < InLenByte; i++)                  //д���������
    {
        MFRC_WriteReg(MFRC_FIFODataReg, pInData[i]);
    }
		
		
    MFRC_WriteReg(MFRC_CommandReg, cmd);            //ִ������
		
		
    if(cmd == MFRC_TRANSCEIVE)
    {
        MFRC_SetBitMask(MFRC_BitFramingReg, 0x80);  //��������
    }
		
    i = 500;                                     //����ʱ��Ƶ�ʵ���,����M1�����ȴ�ʱ��25ms
    
		do
    {
        n = MFRC_ReadReg(MFRC_ComIrqReg);
        i--;
    }
    while((i != 0) && !(n & 0x01) && !(n & waitFor));     //�ȴ��������
		
    MFRC_ClrBitMask(MFRC_BitFramingReg, 0x80);          //ֹͣ����

    /*������յ�����*/
    if(i != 0)
    {
        if(!(MFRC_ReadReg(MFRC_ErrorReg) & 0x1B))
        {
            status = MFRC_OK;
            if(n & irqEn & 0x01)
            {
                status = MFRC_NOTAGERR;
            }
            if(cmd == MFRC_TRANSCEIVE)
            {
                n = MFRC_ReadReg(MFRC_FIFOLevelReg);
                lastBits = MFRC_ReadReg(MFRC_ControlReg) & 0x07;
                if (lastBits)
                {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *pOutLenBit = n * 8;
                }
                if(n == 0)
                {
                    n = 1;
                }
                if(n > MFRC_MAXRLEN)
                {
                    n = MFRC_MAXRLEN;
                }
                for(i = 0; i < n; i++)
                {
                    pOutData[i] = MFRC_ReadReg(MFRC_FIFODataReg);
                }
            }
        }
        else
        {
            status = MFRC_ERR;
        }
    }

    MFRC_SetBitMask(MFRC_ControlReg, 0x80);               //ֹͣ��ʱ������
    MFRC_WriteReg(MFRC_CommandReg, MFRC_IDLE);            //ȡ����ǰ�����ִ��

    return status;
}


 


/**************************************************************************************
* �������ƣ�PCD_Reset
* ����������PCD��λ
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
***************************************************************************************/
void PCD_Reset(void)
{
    /*Ӳ��λ*/
    RS522_RST(1);//�õ����ǵĸ�λ����
    Delay_ms(2);
    RS522_RST(0);
    Delay_ms(2);
    RS522_RST(1);
    Delay_ms(2);

    /*��λ*/
    MFRC_WriteReg(MFRC_CommandReg, MFRC_RESETPHASE);
    Delay_ms(2);

    /*��λ��ĳ�ʼ������*/
    MFRC_WriteReg(MFRC_ModeReg, 0x3D);              //CRC��ʼֵ0x6363
    MFRC_WriteReg(MFRC_TReloadRegL, 30);            //��ʱ����װֵ
    MFRC_WriteReg(MFRC_TReloadRegH, 0);
    MFRC_WriteReg(MFRC_TModeReg, 0x8D);             //��ʱ������
    MFRC_WriteReg(MFRC_TPrescalerReg, 0x3E);        //��ʱ��Ԥ��Ƶֵ
    MFRC_WriteReg(MFRC_TxAutoReg, 0x40);            //100%ASK

    PCD_AntennaOff();                               //������
    Delay_ms(2);
    PCD_AntennaOn();                                //������
}


/**************************************************************************************
* �������ƣ�PCD_AntennaOn
* ������������������,ʹ��PCD���������ز��ź�
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ����
* ˵    ����ÿ�ο�����ر�����֮��Ӧ������1ms�ļ��
***************************************************************************************/
void PCD_AntennaOn(void)
{
    uint8_t temp;
    temp = MFRC_ReadReg(MFRC_TxControlReg);
    if (!(temp & 0x03))
    {
        MFRC_SetBitMask(MFRC_TxControlReg, 0x03);
    }
}


/**************************************************************************************
* �������ƣ�PCD_AntennaOff
* �����������ر�����,ʧ��PCD���������ز��ź�
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ����
* ˵    ����ÿ�ο�����ر�����֮��Ӧ������1ms�ļ��
***************************************************************************************/
void PCD_AntennaOff(void)
{
    MFRC_ClrBitMask(MFRC_TxControlReg, 0x03);
}


/***************************************************************************************
* �������ƣ�PCD_Init
* ������������д����ʼ��
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ����
* ˵    ������
***************************************************************************************/
void PCD_Init(void)
{
    MFRC_Init();      //MFRC�ܽ�����
    PCD_Reset();      //PCD��λ  ����ʼ������
    PCD_AntennaOff(); //�ر�����
		Delay_ms(2);
    PCD_AntennaOn();   //��������
}


/***************************************************************************************
* �������ƣ�PCD_Request
* ����������Ѱ��
* ��ڲ����� -RequestMode��Ѷ����ʽ
*             PICC_REQIDL��Ѱ��������δ��������״̬
*             PICC_REQALL��Ѱ��������ȫ����
*            -pCardType�����ڱ��濨Ƭ����
* ���ڲ�����-pCardType����Ƭ����
*                       0x4400��Mifare_UltraLight
*                       0x0400��Mifare_One(S50)
*                       0x0200��Mifare_One(S70)
*                       0x0800��Mifare_Pro(X)
*                       0x4403��Mifare_DESFire
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ������
***************************************************************************************/
char PCD_Request(uint8_t RequestMode, uint8_t *pCardType)
{
    int status;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    MFRC_ClrBitMask(MFRC_Status2Reg, 0x08);//���ڲ��¶ȴ�����
    MFRC_WriteReg(MFRC_BitFramingReg, 0x07); //�洢ģʽ������ģʽ���Ƿ��������͵�
    MFRC_SetBitMask(MFRC_TxControlReg, 0x03);//���õ����ź�13.56MHZ

    CmdFrameBuf[0] = RequestMode;

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 1, CmdFrameBuf, &unLen);		//MFRC_TRANSCEIVE ��RC552 CmdFrameBuf����Ƭ Ҳ����14443-AЭ��
		
		//printf("request:  %x   %x   \r\n",CmdFrameBuf[0],CmdFrameBuf[1]);

		
		if((status == PCD_OK) && (unLen == 0x10))
    {
        *pCardType = CmdFrameBuf[0];
        *(pCardType + 1) = CmdFrameBuf[1];
    }
		else
		{   
				status = PCD_ERR;   
		}

    return status;
}


/***************************************************************************************
* �������ƣ�PCD_Anticoll
* ��������������ͻ,��ȡ����
* ��ڲ�����-pSnr�����ڱ��濨Ƭ���к�,4�ֽ�
* ���ڲ�����-pSnr����Ƭ���к�,4�ֽ�
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ������
***************************************************************************************/
char PCD_Anticoll(uint8_t *pSnr)
{
    char status;
    uint8_t i, snr_check = 0;
    uint16_t  unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    MFRC_ClrBitMask(MFRC_Status2Reg, 0x08);						//�����־λ
    MFRC_WriteReg(MFRC_BitFramingReg, 0x00);					//���һ���ֽڵ�����λ�����ͣ���ֹ����ֽ�
    MFRC_ClrBitMask(MFRC_CollReg, 0x80);							//������ͻ������б�־λ 

    CmdFrameBuf[0] = PICC_ANTICOLL1;									//14443 ��Ƭ����ͻ
    CmdFrameBuf[1] = 0x20;

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 2, CmdFrameBuf, &unLen);		//���Ϳ�Ƭ����ͻָ��
	
    if(status == PCD_OK)
    {		
				//printf("CardID:");
        for(i = 0; i < 4; i++)
        {
            *(pSnr + i)  = CmdFrameBuf[i];			//���ID
						//printf("%x",CmdFrameBuf[i]);
            snr_check ^= CmdFrameBuf[i];				//���CRC
        }
				//printf("\r\n");
        if(snr_check != CmdFrameBuf[i])
        {
            status = PCD_ERR;
        }
    }
		//switch (status){
		//	case PCD_OK:					printf("Ѱ��OK\r\n");break;
		//	case PCD_ERR:					printf("Ѱ��ERROR\r\n");break;
		//	case PCD_NOTAGERR:		printf("�޿�\r\n");break;
		//}

    MFRC_SetBitMask(MFRC_CollReg, 0x80);						//�÷���ͻλ
    return status;
}


/***************************************************************************************
* �������ƣ�PCD_Select
* ����������ѡ��
* ��ڲ�����-pSnr����Ƭ���к�,4�ֽ�
* ���ڲ�������
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ������
***************************************************************************************/
char PCD_Select(uint8_t *pSnr)
{
    char status;
    uint8_t i;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];
		//ѡ��ָ��0x93 0x70  93H 70H UID0-UID3 BCC Ϊ7���ֽ�
    CmdFrameBuf[0] = PICC_ANTICOLL1;						//����ͻ
    CmdFrameBuf[1] = 0x70;
    CmdFrameBuf[6] = 0;													//��0
	
		//���濨ƬID
    for(i = 0; i < 4; i++)
    {
        CmdFrameBuf[i + 2] = *(pSnr + i);	
        CmdFrameBuf[6]  ^= *(pSnr + i);
    }
    MFRC_CalulateCRC(CmdFrameBuf, 7, &CmdFrameBuf[7]);	//CRCУ�� 

    MFRC_ClrBitMask(MFRC_Status2Reg, 0x08);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 9, CmdFrameBuf, &unLen);		//CRC �����Ϊ9���ֽ�
		
		//printf("select:%x\n",CmdFrameBuf[0]);																						//��ӡӦ��ɹ�����ֵ

    if((status == PCD_OK) && (unLen == 0x18))
    {
        status = PCD_OK;
    }
    else
    {
        status = PCD_ERR;
    }
    return status;
}




/***************************************************************************************
* �������ƣ�PCD_AuthState
* ������������֤��Ƭ����
* ��ڲ�����-AuthMode����֤ģʽ
*                   PICC_AUTHENT1A����֤A����
*                   PICC_AUTHENT1B����֤B����
*           -BlockAddr�����ַ(0~63)
*           -pKey������       48�ֽ� 
*           -pSnr����Ƭ���к�,4�ֽ�
* ���ڲ�������
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ������֤����ʱ,������Ϊ��λ,BlockAddr����������ͬһ�������������
***************************************************************************************/
char PCD_AuthState(uint8_t AuthMode, uint8_t BlockAddr, const uint8_t *pKey, uint8_t *pSnr)
{
    char status;
    uint16_t unLen;
    uint8_t i, CmdFrameBuf[MFRC_MAXRLEN]; 
		//��֤����60 61 ���ַ ��Կ �����к�
    CmdFrameBuf[0] = AuthMode;		
    CmdFrameBuf[1] = BlockAddr;			
    for(i = 0; i < 6; i++)
    {
        CmdFrameBuf[i + 2] = *(pKey + i);
				//printf("pKey[%d] = %x\r\n",i,pKey[i]);
				//CmdFrameBuf[i + 2] = 0xFF;			//��ʼ����
    }
    for(i = 0; i < 4; i++)
    {
        CmdFrameBuf[i + 8] = *(pSnr + i);
    }

    status = MFRC_CmdFrame(MFRC_AUTHENT, CmdFrameBuf, 12, CmdFrameBuf, &unLen);
		
		
    if((status != PCD_OK) || (!(MFRC_ReadReg(MFRC_Status2Reg) & 0x08)))
    {
        status = PCD_ERR;
    }

    return status;
}


/***************************************************************************************
* �������ƣ�PCD_WriteBlock
* ������������MF1�����ݿ�
* ��ڲ�����-BlockAddr�����ַ
*           -pData: ���ڱ����д�������,16�ֽ�
* ���ڲ�������
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ������
***************************************************************************************/
char PCD_WriteBlock(uint8_t BlockAddr, uint8_t *pData)
{
    char status;
    uint16_t unLen;  
    uint8_t i, CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_WRITE;
    CmdFrameBuf[1] = BlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

    if((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    if(status == PCD_OK)
    {
        for(i = 0; i < 16; i++)
        {
            CmdFrameBuf[i] = *(pData + i);
        }
        MFRC_CalulateCRC(CmdFrameBuf, 16, &CmdFrameBuf[16]);

        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 18, CmdFrameBuf, &unLen);

        if((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
        {
            status = PCD_ERR;
        }
    }

    return status;
}


/***************************************************************************************
* �������ƣ�PCD_ReadBlock
* ������������MF1�����ݿ�
* ��ڲ�����-BlockAddr�����ַ
*           -pData: ���ڱ������������,16�ֽ�
* ���ڲ�����-pData: ���ڱ������������,16�ֽ�
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ������
***************************************************************************************/
char PCD_ReadBlock(uint8_t BlockAddr, uint8_t *pData)
{
    char status;
    uint16_t unLen;
    uint8_t i, CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_READ;			//��ָ��
    CmdFrameBuf[1] = BlockAddr;			//����ַ
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);	//CRC

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);
	
    if((status == PCD_OK) && (unLen == 0x90))
    {
        for(i = 0; i < 16; i++)				//��������
        {
            *(pData + i) = CmdFrameBuf[i];
        }	
    }
    else
    {
        status = PCD_ERR;
    }

    return status;
}


/***************************************************************************************
* �������ƣ�PCD_Value
* ������������MF1�����ݿ�����ֵ����
* ��ڲ�����
*           -BlockAddr�����ַ
*           -pValue�����ֽ���ֵ��ֵ,��λ��ǰ
*           -mode����ֵ�����ģʽ
*                  PICC_INCREMENT����ֵ
*                	 PICC_DECREMENT����ֵ
* ���ڲ�������
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ����ʹ��ǰ��Ǯ����ʼ�� unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0xFE,0x01,0xFE};
***************************************************************************************/
char PCD_Value(uint8_t mode, uint8_t BlockAddr, uint8_t *pValue)
{
    //0XC1        1           Increment[4]={0x03, 0x01, 0x01, 0x01};
    char status;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = mode;
    CmdFrameBuf[1] = BlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

    if((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    if(status == PCD_OK)
    {
				memcpy(CmdFrameBuf, pValue, 4);
        //for(i = 0; i < 16; i++)
        //{
        //    CmdFrameBuf[i] = *(pValue + i);
        //}
        MFRC_CalulateCRC(CmdFrameBuf, 4, &CmdFrameBuf[4]);
        unLen = 0;
        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 6, CmdFrameBuf, &unLen);
        if(status != PCD_ERR)
        {
            status = PCD_OK;
        }
    }

    if(status == PCD_OK)
    {
        CmdFrameBuf[0] = PICC_TRANSFER;
        CmdFrameBuf[1] = BlockAddr;
        MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

        if((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
        {
            status = PCD_ERR;
        }
    }
    return status;
}


/***************************************************************************************
* �������ƣ�PCD_BakValue
* ��������������Ǯ��(��ת��)
* ��ڲ�����-sourceBlockAddr��Դ���ַ
*                -goalBlockAddr   ��Ŀ����ַ
* ���ڲ�������
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ����ֻ����ͬһ��������ת��
***************************************************************************************/
char PCD_BakValue(uint8_t sourceBlockAddr, uint8_t goalBlockAddr)
{
    char status;
    uint16_t  unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_RESTORE;
    CmdFrameBuf[1] = sourceBlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);
    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);
    if((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    if(status == PCD_OK)
    {
        CmdFrameBuf[0] = 0;
        CmdFrameBuf[1] = 0;
        CmdFrameBuf[2] = 0;
        CmdFrameBuf[3] = 0;
        MFRC_CalulateCRC(CmdFrameBuf, 4, &CmdFrameBuf[4]);
        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 6, CmdFrameBuf, &unLen);
        if(status != PCD_ERR)
        {
            status = PCD_OK;
        }
    }

    if(status != PCD_OK)
    {
        return PCD_ERR;
    }

    CmdFrameBuf[0] = PICC_TRANSFER;
    CmdFrameBuf[1] = goalBlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);
    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);
    if((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    return status;
}


/***************************************************************************************
* �������ƣ�PCD_Halt
* �������������Ƭ��������״̬
* ��ڲ�������
* ���ڲ�������
* �� �� ֵ��-status���������(PCD_OK��PCD_NOTAGERR��PCD_ERR)
* ˵    ������
***************************************************************************************/
char PCD_Halt(void)
{
    char status;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_HALT;
    CmdFrameBuf[1] = 0;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

    return status;
}


