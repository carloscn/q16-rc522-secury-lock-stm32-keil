/*
 * rc522.h
 *
 *  Created on: Sep 12, 2013
 *      Author: fei
 */
#ifndef _RC522_H_
#define	_RC522_H_
/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_MEM               0x01               //ȡ����ǰ����
#define PCD_CALCCRC           0x03               //CRC����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����
/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define MAXRLEN  18

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		  0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define 	MI_OK                 0
#define 	MI_NOTAGERR           (-1)
#define 	MI_ERR                (-2)

#define	SHAQU1	0X01
#define	KUAI4	0X04
#define	KUAI7	0X07
#define	REGCARD	0xa1
#define	CONSUME	0xa2
#define READCARD	0xa3
#define ADDMONEY	0xa4

//rc522 SPI�ӿ�CS�ź�
#define RC522_CS      GPIO_Pin_13
#define GPIO_RC522_CS  GPIOD
#define RCC_RC522_CS  RCC_APB2Periph_GPIOD

#define NRF24L01_IRQ      GPIO_Pin_15
#define GPIO_NRF24L01_IRQ  GPIOD
#define RCC_NRF24L01_IRQ  RCC_APB2Periph_GPIOD
		

//SPIƬѡ�ź�	
#define Clr_RC522_CS       {GPIO_SetBits(GPIO_RC522_CS, RC522_CS);}
#define Set_RC522_CS   	  {GPIO_ResetBits(GPIO_RC522_CS, RC522_CS);}



//Э�����
#define FRAME_HEAD   0X48
#define FRAME_TAIL   0X44
#define READ_ID      0X01
#define READ_BANK    0X02
#define WRITE_BANK   0X03
#define PASSWORD_CHECKED 0X04


#define READ_FAIL    0X08
#define WRITE_FAIL   0X09








#if 0
sbit spi_cs = P0 ^ 5;
sbit spi_ck = P0 ^ 6;
sbit spi_mosi = P0 ^ 7;
sbit spi_miso = P4 ^ 1;
sbit spi_rst = P2 ^ 7;

#define SET_SPI_CS  spi_cs=1
#define CLR_SPI_CS  spi_cs=0

#define SET_SPI_CK  spi_ck=1
#define CLR_SPI_CK  spi_ck=0

#define SET_SPI_MOSI  spi_mosi=1
#define CLR_SPI_MOSI  spi_mosi=0

#define STU_SPI_MISO  spi_miso

#define SET_RC522RST  spi_rst=1
#define CLR_RC522RST  spi_rst=0
#else
#include "spi.h"
#endif

char PcdReset(void);
char PcdRequest(unsigned char req_code, unsigned char *pTagType);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
char M500PcdConfigISOType(unsigned char type);
char PcdAnticoll(unsigned char *pSnr);
char PcdSelect(unsigned char *pSnr);
char PcdAuthState(unsigned char auth_mode, unsigned char addr,
		unsigned char *pKey, unsigned char *pSnr);
char PcdWrite(unsigned char addr, unsigned char *pData);
char PcdRead(unsigned char addr, unsigned char *pData);
char PcdHalt(void);
void Select_RC522(unsigned char i);
void RC522_Test(void);
char M500PcdConfigISOType(unsigned char type);
 void PcdAntennaOn(void);
 void PcdAntennaOff(void);
char PcdReset(void);
void RC522_Reset(void);

unsigned char Read_ID(unsigned char *card_id);
unsigned char Read_Bank(unsigned char Addr,unsigned char *data);
unsigned char Write_Bank(unsigned char Addr ,unsigned char *p);


#endif //_RC522_H_