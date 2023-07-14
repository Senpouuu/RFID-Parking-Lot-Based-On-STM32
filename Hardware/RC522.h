#ifndef _RC522_H
#define _RC522_H
#include "stm32f10x.h"                  // Device header



/***********************************************************************************
*								MFRC522驱动程序			        				   *
************************************************************************************/
/*MFRC522寄存器定义*/
//PAGE0
#define MFRC_RFU00              		0x00    
#define MFRC_CommandReg         		0x01    
#define MFRC_ComIEnReg             	0x02    
#define MFRC_DivlEnReg             	0x03    
#define MFRC_ComIrqReg             	0x04    
#define MFRC_DivIrqReg             	0x05
#define MFRC_ErrorReg              	0x06    
#define MFRC_Status1Reg            	0x07    
#define MFRC_Status2Reg            	0x08    
#define MFRC_FIFODataReg           	0x09
#define MFRC_FIFOLevelReg          	0x0A
#define MFRC_WaterLevelReg         	0x0B
#define MFRC_ControlReg            	0x0C
#define MFRC_BitFramingReg         	0x0D
#define MFRC_CollReg               	0x0E
#define MFRC_RFU0F                 	0x0F
//PAGE1     
#define MFRC_RFU10                 	0x10
#define MFRC_ModeReg               	0x11
#define MFRC_TxModeReg             	0x12
#define MFRC_RxModeReg             	0x13
#define MFRC_TxControlReg          	0x14
#define MFRC_TxAutoReg             	0x15 //中文手册有误
#define MFRC_TxSelReg              	0x16
#define MFRC_RxSelReg              	0x17
#define MFRC_RxThresholdReg        	0x18
#define MFRC_DemodReg              	0x19
#define MFRC_RFU1A                 	0x1A
#define MFRC_RFU1B                 	0x1B
#define MFRC_MifareReg             	0x1C
#define MFRC_RFU1D                 	0x1D
#define MFRC_RFU1E                 	0x1E
#define MFRC_SerialSpeedReg        	0x1F
//PAGE2    
#define MFRC_RFU20                 	0x20  
#define MFRC_CRCResultRegM         	0x21
#define MFRC_CRCResultRegL         	0x22
#define MFRC_RFU23                 	0x23
#define MFRC_ModWidthReg           	0x24
#define MFRC_RFU25                 	0x25
#define MFRC_RFCfgReg              	0x26
#define MFRC_GsNReg                	0x27
#define MFRC_CWGsCfgReg            	0x28
#define MFRC_ModGsCfgReg           	0x29
#define MFRC_TModeReg              	0x2A
#define MFRC_TPrescalerReg         	0x2B
#define MFRC_TReloadRegH           	0x2C
#define MFRC_TReloadRegL           	0x2D
#define MFRC_TCounterValueRegH     	0x2E
#define MFRC_TCounterValueRegL     	0x2F
//PAGE3      
#define MFRC_RFU30                 	0x30
#define MFRC_TestSel1Reg           	0x31
#define MFRC_TestSel2Reg           	0x32
#define MFRC_TestPinEnReg          	0x33
#define MFRC_TestPinValueReg       	0x34
#define MFRC_TestBusReg            	0x35
#define MFRC_AutoTestReg           	0x36
#define MFRC_VersionReg            	0x37
#define MFRC_AnalogTestReg         	0x38
#define MFRC_TestDAC1Reg           	0x39  
#define MFRC_TestDAC2Reg           	0x3A   
#define MFRC_TestADCReg            	0x3B   
#define MFRC_RFU3C                 	0x3C   
#define MFRC_RFU3D                 	0x3D   
#define MFRC_RFU3E                 	0x3E   
#define MFRC_RFU3F                 	0x3F

/*MFRC522的FIFO长度定义*/
#define MFRC_FIFO_LENGTH       		64 

/*MFRC522传输的帧长定义*/
#define MFRC_MAXRLEN                18                

/*MFRC522命令集,中文手册P59*/
#define MFRC_IDLE              		0x00		//取消当前命令的执行
#define MFRC_CALCCRC           		0x03    //激活CRC计算
#define MFRC_TRANSMIT          		0x04    //发送FIFO缓冲区内容
#define MFRC_NOCMDCHANGE          0x07		//无命令改变
#define MFRC_RECEIVE           		0x08    //激活接收器接收数据
#define MFRC_TRANSCEIVE        		0x0C    //发送并接收数据
#define MFRC_AUTHENT           		0x0E    //执行Mifare认证(验证密钥)
#define MFRC_RESETPHASE        		0x0F    //复位MFRC522

/*MFRC522通讯时返回的错误代码*/
#define MFRC_OK                 	(char)0
#define MFRC_NOTAGERR            	(char)(-1)
#define MFRC_ERR                	(char)(-2)

/*MFRC522函数声明*/
void MFRC_Init(void);
void MFRC_WriteReg(uint8_t addr, uint8_t data);
uint8_t MFRC_ReadReg(uint8_t addr);
void MFRC_SetBitMask(uint8_t addr, uint8_t mask);
void MFRC_ClrBitMask(uint8_t addr, uint8_t mask);
void MFRC_CalulateCRC(uint8_t *pInData, uint8_t len, uint8_t *pOutData);
char MFRC_CmdFrame(uint8_t cmd, uint8_t *pInData, uint8_t InLenByte, uint8_t *pOutData, uint16_t *pOutLenBit);



/***********************************************************************************
*							MFRC552与MF1卡通讯接口程序	 		     	     	   *
************************************************************************************/
/*Mifare1卡片命令字*/
#define PICC_REQIDL           	0x26               	//寻天线区内未进入休眠状态的卡
#define PICC_REQALL           	0x52               	//寻天线区内全部卡
#define PICC_ANTICOLL1        	0x93               	//防冲撞
#define PICC_ANTICOLL2        	0x95               	//防冲撞
#define PICC_AUTHENT1A        	0x60               	//验证A密钥
#define PICC_AUTHENT1B        	0x61               	//验证B密钥
#define PICC_READ             	0x30               	//读块
#define PICC_WRITE            	0xA0               	//写块
#define PICC_DECREMENT        	0xC0               	//减值(扣除)
#define PICC_INCREMENT        	0xC1               	//增值(充值)
#define PICC_TRANSFER         	0xB0               	//转存(传送)
#define PICC_RESTORE          	0xC2               	//恢复(重储)
#define PICC_HALT             	0x50               	//休眠

/*PCD通讯时返回的错误代码*/
#define PCD_OK                 	(char)0				//成功
#define PCD_NOTAGERR            (char)(-1)			//无卡
#define PCD_ERR                	(char)(-2)			//出错

//******************************************************************/
//                    RC522通讯返回错误代码                         /
//******************************************************************/
#define MI_ERR                      0xFE 
//#define MI_ERR                         //(-2)


// Mifare Error Codes 
// Each function returns a status value, which corresponds to the 
// mifare error codes. 

#define MI_OK                          0 
#define MI_CHK_OK                      0 
#define MI_CRC_ZERO                    0 

#define MI_CRC_NOTZERO                 1 

#define MI_NOTAGERR                 0xFF 
#define MI_CHK_FAILED               0xFF 
#define MI_CRCERR                   0xFE 
#define MI_CHK_COMPERR              0xFE 
#define MI_EMPTY                    0xFD 
#define MI_AUTHERR                  0xFC 
#define MI_PARITYERR                0xFB 
#define MI_CODEERR                  0xFA 

#define MI_SERNRERR                 0xF8 
#define MI_KEYERR                   0xF7 
#define MI_NOTAUTHERR               0xF6 
#define MI_BITCOUNTERR              0xF5 
#define MI_BYTECOUNTERR             0xF4 
#define MI_IDLE                     0xF3 
#define MI_TRANSERR                 0xF2 
#define MI_WRITEERR                 0xF1 
#define MI_INCRERR                  0xF0 
#define MI_DECRERR                  0xEF 
#define MI_READERR                  0xEE 
#define MI_OVFLERR                  0xED 
#define MI_POLLING                  0xEC 
#define MI_FRAMINGERR               0xEB 
#define MI_ACCESSERR                0xEA 
#define MI_UNKNOWN_COMMAND          0xE9 
#define MI_COLLERR                  0xE8 
#define MI_RESETERR                 0xE7 
#define MI_INITERR                  0xE7 
#define MI_INTERFACEERR             0xE7 
#define MI_ACCESSTIMEOUT            0xE5 
#define MI_NOBITWISEANTICOLL        0xE4 
#define MI_QUIT                     0xE2 

#define MI_RECBUF_OVERFLOW          0xCF 
#define MI_SENDBYTENR               0xCE 

#define MI_SENDBUF_OVERFLOW         0xCC 
#define MI_BAUDRATE_NOT_SUPPORTED   0xCB 
#define MI_SAME_BAUDRATE_REQUIRED   0xCA 

#define MI_WRONG_PARAMETER_VALUE    0xC5 

#define MI_BREAK                    0x9E 
#define MI_NY_IMPLEMENTED           0x9D 
#define MI_NO_MFRC                  0x9C 
#define MI_MFRC_NOTAUTH             0x9B 
#define MI_WRONG_DES_MODE           0x9A 
#define MI_HOST_AUTH_FAILED         0x99 

#define MI_WRONG_LOAD_MODE          0x97 
#define MI_WRONG_DESKEY             0x96 
#define MI_MKLOAD_FAILED            0x95 
#define MI_FIFOERR                  0x94 
#define MI_WRONG_ADDR               0x93 
#define MI_DESKEYLOAD_FAILED        0x92 

#define MI_WRONG_SEL_CNT            0x8F 
#define MI_RC531_WRONG_READVALUE    0x8E //LI ADDED 09-4-24 
#define MI_WRONG_TEST_MODE          0x8C 
#define MI_TEST_FAILED              0x8B 
#define MI_TOC_ERROR                0x8A 
#define MI_COMM_ABORT               0x89 
#define MI_INVALID_BASE             0x88 
#define MI_MFRC_RESET               0x87 
#define MI_WRONG_VALUE              0x86 
#define MI_VALERR                   0x85


/*PCD函数声明*/
void PCD_Init(void);
void PCD_Reset(void);
void PCD_AntennaOn(void);
void PCD_AntennaOff(void);
char PCD_Request(uint8_t RequestMode, uint8_t *pCardType);  //寻卡，并返回卡的类型
char PCD_Anticoll(uint8_t *pSnr);                           //防冲突,返回卡号
char PCD_Select(uint8_t *pSnr);                             //选卡
char PCD_AuthState(uint8_t AuthMode, uint8_t BlockAddr, const uint8_t *pKey, uint8_t *pSnr); //验证密码(密码A和密码B)   
char PCD_WriteBlock(uint8_t BlockAddr, uint8_t *pData);   //写数据
char PCD_ReadBlock(uint8_t BlockAddr, uint8_t *pData);    //读数据
char PCD_Value(uint8_t mode, uint8_t BlockAddr, uint8_t *pValue);   
char PCD_BakValue(uint8_t sourceBlockAddr, uint8_t goalBlockAddr);                                 
char PCD_Halt(void);
void ReaderCard(void);
void Select(void);


extern u8 CardType;
extern const uint8_t KeyA[];
extern u8 CardID[];
extern u8 RData[];
extern u8 WData[];
extern u8 Wallet_Init[];
extern u8 Recharge_Value[]; 																																		
extern u8 Deduction_Value[];	

#endif



