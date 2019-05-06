/*
 * rc522.c
 *
 *  Created on: Sep 12, 2013
 *      Author: fei
 */
#include "main.h"
char a;
unsigned char bank;
unsigned char Read_Flag;
unsigned char Test_Buffer1[16];
unsigned char card_data[6];
unsigned char Write_Flag = 0;
unsigned char Count_ns;
unsigned char IC_ID[4];
unsigned char abc;
unsigned char ucAddr;
char status;
extern volatile unsigned char RS232_buff[RS232_REC_BUFF_SIZE];
extern unsigned char gSelect_RC522;
static int md = 0;

extern unsigned char PassWord[6];

static void ClearBitMask(unsigned char reg, unsigned char mask);
static void WriteRawRC(unsigned char Address, unsigned char value);
static void SetBitMask(unsigned char reg, unsigned char mask);
static void CalulateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData);
static unsigned char ReadRawRC(unsigned char Address);
static char PcdComMF522(unsigned char Command, unsigned char *pInData, unsigned char InLenByte,
unsigned char *pOutData, unsigned int *pOutLenBit);



void delay_ns(unsigned int ns)
{
	unsigned int i;
	for (i = 0; i < ns; i++) 
	{

	}
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code, unsigned char *pTagType)
{
	char status;
	unsigned int unLen;
	unsigned char ucComMF522Buf[MAXRLEN];
        
	ClearBitMask(Status2Reg, 0x08);//0X08
	WriteRawRC(BitFramingReg, 0x07);
	SetBitMask(TxControlReg, 0x03);

	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);

	if ((status == MI_OK) && (unLen == 0x10)) {
		*pTagType = ucComMF522Buf[0];
		*(pTagType + 1) = ucComMF522Buf[1];
	} else {
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
	char status;
	unsigned char i, snr_check = 0;
	unsigned int unLen;
	unsigned char ucComMF522Buf[MAXRLEN];

	ClearBitMask(Status2Reg, 0x08);
	WriteRawRC(BitFramingReg, 0x00);
	ClearBitMask(CollReg, 0x80);

	ucComMF522Buf[0] = PICC_ANTICOLL1;
	ucComMF522Buf[1] = 0x20;

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);

	if (status == MI_OK) {
		for (i = 0; i < 4; i++) {
			*(pSnr + i) = ucComMF522Buf[i];
			snr_check ^= ucComMF522Buf[i];
		}
		if (snr_check != ucComMF522Buf[i]) {
			status = MI_ERR;
		}
	}

	SetBitMask(CollReg, 0x80);
	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
	char status;
	unsigned char i;
	unsigned int unLen;
	unsigned char ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_ANTICOLL1;
	ucComMF522Buf[1] = 0x70;
	ucComMF522Buf[6] = 0;
	for (i = 0; i < 4; i++) {
		ucComMF522Buf[i + 2] = *(pSnr + i);
		ucComMF522Buf[6] ^= *(pSnr + i);
	}
	CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);

	ClearBitMask(Status2Reg, 0x08);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);

	if ((status == MI_OK) && (unLen == 0x18)) {
		status = MI_OK;
	} else {
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(unsigned char auth_mode, unsigned char addr, unsigned char *pKey,
		unsigned char *pSnr)
{
	char status;
	unsigned int unLen;
	unsigned char i, ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = auth_mode;
	ucComMF522Buf[1] = addr;
	for (i = 0; i < 6; i++) {
		ucComMF522Buf[i + 2] = *(pKey + i);
	}
	for (i = 0; i < 6; i++) {
		ucComMF522Buf[i + 8] = *(pSnr + i);
	}
	//   memcpy(&ucComMF522Buf[2], pKey, 6);
	//   memcpy(&ucComMF522Buf[8], pSnr, 4);

	status = PcdComMF522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);

     abc=	ReadRawRC(Status2Reg);
	if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08))) {
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr, unsigned char *pData)
{
	char status;
	unsigned int unLen;
	unsigned char i, ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_READ;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
	if ((status == MI_OK) && (unLen == 0x90)) {
		//   {   memcpy(pData, ucComMF522Buf, 16);   }
		for (i = 0; i <16; i++) {
			*(pData + i) = ucComMF522Buf[i];
		}
	} else {
		status = MI_ERR;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr, unsigned char *pData)
{
	char status;
	unsigned int unLen;
	unsigned char i, ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_WRITE;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
		status = MI_ERR;
	}

	if (status == MI_OK) {
		//memcpy(ucComMF522Buf, pData, 16);
		for (i = 0; i < 16; i++) {
			ucComMF522Buf[i] = *(pData + i);
		}
		CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);
		status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
		if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
			status = MI_ERR;
		}
	}

	return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
	char status;
	unsigned int unLen;
	unsigned char ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
	unsigned char i, n;
	ClearBitMask(DivIrqReg, 0x04);
	WriteRawRC(CommandReg, PCD_IDLE);
	SetBitMask(FIFOLevelReg, 0x80);
	for (i = 0; i < len; i++) {
		WriteRawRC(FIFODataReg, *(pIndata + i));
	}
	WriteRawRC(CommandReg, PCD_CALCCRC);
	i = 0xFF;
	do {
		n = ReadRawRC(DivIrqReg);
		i--;
	}
	while ((i != 0) && !(n & 0x04));
	pOutData[0] = ReadRawRC(CRCResultRegL);
	pOutData[1] = ReadRawRC(CRCResultRegM);
}

void RC522SelfTest()
{
	u8 tmp = 0, sum = 0, i, n, buf[64];

	n = ReadRawRC(VersionReg);
	if (n > 0) {
		md++;
	}
	WriteRawRC(CommandReg, PCD_RESETPHASE);
	delay_ns(10);
	WriteRawRC(CommandReg, PCD_IDLE);
	while ((tmp = ReadRawRC(CommandReg)) != 0) {
		delay_ns(10);
		sum += tmp;
	}
	WriteRawRC(ComIEnReg, 0xf7);
	ClearBitMask(ComIrqReg, 0x80);
	WriteRawRC(CommandReg, PCD_IDLE);
	SetBitMask(FIFOLevelReg, 0x80);

	for (i = 0; i < 25; i++) {
		WriteRawRC(FIFODataReg, 0x00);
	}
	n = ReadRawRC(FIFOLevelReg);
	if (n > 0) {
		md++;
	}
	delay_ns(100);
	n = ReadRawRC(ErrorReg);
	if (n > 0) {
		md++;
	}
	WriteRawRC(CommandReg, PCD_MEM);
	delay_ns(100);
	n = ReadRawRC(FIFOLevelReg);
	if (n > 0) {
		md++;
	}
	n = ReadRawRC(ErrorReg);
	if (n > 0) {
		md++;
	}
	WriteRawRC(AutoTestReg, 0x09);
	WriteRawRC(FIFODataReg, 0x00);
	WriteRawRC(CommandReg, PCD_CALCCRC);
	delay_ns(100);
	n = ReadRawRC(ErrorReg);
	if (n > 0) {
		md++;
	}
	WriteRawRC(CommandReg, PCD_MEM);
	delay_ns(100);
	n = ReadRawRC(ErrorReg);
	if (n > 0) {
		md++;
	}
	n = ReadRawRC(FIFOLevelReg);
	if (n > 0) {
		md++;
		for (i = 0; i < n; i++) {
			buf[i] = ReadRawRC(FIFODataReg);
		}
	}
	md = buf[n - 1];
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
	u8 tmp = 0;    
	WriteRawRC(CommandReg, PCD_RESETPHASE);
	WriteRawRC(CommandReg, PCD_IDLE);
	delay_ns(1000);
#if 0
	while ((tmp = ReadRawRC(CommandReg)) != 0) {
		delay_ns(10);
		md += tmp;
	}
#endif
	WriteRawRC(ModeReg, 0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
	WriteRawRC(TReloadRegL, 30);
	WriteRawRC(TReloadRegH, 0);
	WriteRawRC(TModeReg, 0xaD);
	WriteRawRC(TPrescalerReg, 0x3E);
//	WriteRawRC(RxModeReg, 0x20);
//	WriteRawRC(TxModeReg, 0x20);
	WriteRawRC(TxAutoReg, 0x40);            //����Ҫ
	//RC522SelfTest();
	return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//����RC632�Ĺ�����ʽ 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(unsigned char type)
{
	if (type == 'A') {                     //ISO14443_A
		ClearBitMask(Status2Reg, 0x08);
		WriteRawRC(ModeReg, 0x3D);                     //3F
		WriteRawRC(RxSelReg, 0x86);                     //84
		WriteRawRC(RFCfgReg, 0x1F);   //4F
		WriteRawRC(TReloadRegL, 30);   //tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
		WriteRawRC(TReloadRegH, 0);
		WriteRawRC(TModeReg, 0x8D);
		WriteRawRC(TPrescalerReg, 0x3E);
		delay_ns(1000);
		delay_ns(1000);
		delay_ns(1000);
		PcdAntennaOn();
		delay_ns(1000);
		delay_ns(1000);
		delay_ns(1000);
	} else {
		return -1;
	}

	return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
	unsigned char ucAddr;
	unsigned char ucResult = 0;
    Set_RC522_CS;
	ucAddr = ((Address << 1) & 0x7E) | 0x80;
    SPI_ReadWriteByte(ucAddr);
	ucResult = SPI_ReadWriteByte(0xff);
    Clr_RC522_CS;
	return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{
	unsigned char ucAddr;
    Set_RC522_CS;
	ucAddr = ((Address << 1) & 0x7E);
	SPI_ReadWriteByte(ucAddr);
	SPI_ReadWriteByte(value);
    Clr_RC522_CS;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg, unsigned char mask)
{
	char tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg, unsigned char mask)
{
	char tmp = 0x0;
	tmp = ReadRawRC(reg);
	WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command,
		unsigned char *pInData,
		unsigned char InLenByte,
		unsigned char *pOutData,
		unsigned int *pOutLenBit)
{
	char status = MI_ERR;
	unsigned char irqEn = 0x00;
	unsigned char waitFor = 0x00;
	unsigned char lastBits;
	unsigned char n;
	unsigned int i;
	unsigned int err = 0;
	switch (Command)
	{
	case PCD_AUTHENT:
		irqEn = 0x12;
		waitFor = 0x10;
		break;
	case PCD_TRANSCEIVE:
		irqEn = 0x77;
		waitFor = 0x30;
		break;
	default:
		break;
	}

	WriteRawRC(ComIEnReg, irqEn | 0x80);
	ClearBitMask(ComIrqReg, 0x80);
	WriteRawRC(CommandReg, PCD_IDLE);
	SetBitMask(FIFOLevelReg, 0x80);

	n = ReadRawRC(ComIrqReg);
	if (n > 0) {
		md++;
	}
	n = ReadRawRC(FIFOLevelReg);
	if (n > 0) {
		md++;
	}

	for (i = 0; i < InLenByte; i++) {
		WriteRawRC(FIFODataReg, pInData[i]);	 //д����
	}
	n = ReadRawRC(ComIrqReg);
	if (n > 0) {
		md++;
	}

	WriteRawRC(CommandReg, Command);

	if (Command == PCD_TRANSCEIVE) {
		SetBitMask(BitFramingReg, 0x80);
	}

	//i = ~0;  //����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
	i = 2000;
	do {
		n = ReadRawRC(ComIrqReg);
		i--;
                md++;
	}
	while ((i != 0) && !(n & 0x01) && !(n & waitFor));
	ClearBitMask(BitFramingReg, 0x80);
	if (n > 0) {
		md++;
	}
	err = ReadRawRC(ErrorReg);
	if (i != 0) {
          md = waitFor;
		if (!(err & 0x1B)) {
			status = MI_OK;
			if (n & irqEn & 0x01) {
				status = MI_NOTAGERR;
			}
			if (Command == PCD_TRANSCEIVE) {
				n = ReadRawRC(FIFOLevelReg);
				lastBits = ReadRawRC(ControlReg) & 0x07;
				if (lastBits) {
					*pOutLenBit = (n - 1) * 8 + lastBits;
				} else {
					*pOutLenBit = n * 8;
				}
				if (n == 0) {
					n = 1;
				}
				if (n > MAXRLEN) {
					n = MAXRLEN;
				}
				for (i = 0; i < n; i++) {
					pOutData[i] = ReadRawRC(FIFODataReg);
				}
			}
		} else {
			status = MI_ERR;
		}

	}

	SetBitMask(ControlReg, 0x80);           // stop timer now
	WriteRawRC(CommandReg, PCD_IDLE);
	return status;
}

/////////////////////////////////////////////////////////////////////
//��������  //ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
	unsigned char i;
	i = ReadRawRC(TxControlReg);
	if (!(i & 0x03)) {
		SetBitMask(TxControlReg, 0x03);
	}
}

/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}


void RC522_Reset(void)
{
  
    PcdReset();		 //��λRC522
	PcdAntennaOff();//������
	delay_ns(5000);
	delay_ns(5000);
	delay_ns(5000);
	delay_ns(5000);
	delay_ns(5000);
	delay_ns(5000);
	PcdAntennaOn();//������
	M500PcdConfigISOType('A');//���ù�����ʽ

}

unsigned char Read_ID(unsigned char *card_id)
{
	unsigned char RevBuffer[30];        
    status = PcdRequest(PICC_REQALL, &card_id[0]); //Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�
	status = PcdAnticoll(&card_id[2]); //����ײ�����ؿ������к� 4�ֽ�   
	PcdHalt();  
	return status;
}

unsigned char Read_Bank(unsigned char Addr,unsigned char *data)
{
	unsigned char LastKeyA[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; //NO.2��
	unsigned char NewKeyA[6] = { 0x19, 0x84, 0x07, 0x15, 0x76, 0x14 }; //NO.2��
	unsigned char NewKey[16] = { 0x19, 0x84, 0x07, 0x15, 0x76, 0x14,0xff, 0x07, 0x80, 0x69, 0x19, 0x84, 0x07, 0x15, 0x76, 0x14 };
 
    unsigned char RevBuffer[30];
    unsigned char MLastSelectedSnr[4];
    char status;
	unsigned char Bank_Password = ((Addr/4)*4+3);

    status = PcdRequest(PICC_REQALL, &RevBuffer[0]); //Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�
    status = PcdAnticoll(&RevBuffer[2]); //����ײ�����ؿ������к� 4�ֽ�
    memcpy(MLastSelectedSnr, &RevBuffer[2], 4);	
    status = PcdSelect(MLastSelectedSnr); //ѡ��
	status = PcdAuthState(PICC_AUTHENT1A, Bank_Password, PassWord, MLastSelectedSnr);//��֤������
    status = PcdRead(Addr,&data[0]);
	PcdHalt();  
 
	return status; 
}

//дBANK���ɹ����أ�ʧ�ܷ���ʧ������
unsigned char Write_Bank(unsigned char Addr ,unsigned char *p)
{
   	unsigned char LastKeyA[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; //NO.2��
	unsigned char NewKeyA[6] = { 0x19, 0x84, 0x07, 0x15, 0x76, 0x14 }; //NO.2��
	unsigned char NewKey[16] = { 0x19, 0x84, 0x07, 0x15, 0x76, 0x14,0xff, 0x07, 0x80, 0x69, 0x19, 0x84, 0x07, 0x15, 0x76, 0x14 };
 
    unsigned char RevBuffer[30];
    unsigned char MLastSelectedSnr[4];
    char status;
	unsigned char Bank_Password_addr = ((Addr/4)*4+3); //���������ַ  ��4����

    status = PcdRequest(PICC_REQALL, &RevBuffer[0]); 	//Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�
    status = PcdAnticoll(&RevBuffer[2]); 				//����ײ�����ؿ������к� 4�ֽ�
    memcpy(MLastSelectedSnr, &RevBuffer[2], 4);	
    status = PcdSelect(MLastSelectedSnr); 				//ѡ��
	//status = PcdAuthState(PICC_AUTHENT1A,Bank_Password_addr , LastKeyA, MLastSelectedSnr);//��֤������
	status = PcdAuthState(PICC_AUTHENT1A,Bank_Password_addr , PassWord, MLastSelectedSnr);//��֤������
	PcdWrite(Addr,p);
	PcdHalt(); 
	return status  ;


}

// AC 01 CA DE
// FC E7 C8 DE
unsigned char send_data[19];
unsigned char Write_Bank_Data[16];
void Parse_Data(void)
{
	unsigned int i = 0;

	if(RS232_buff[0] = FRAME_HEAD && RS232_buff[RS232_send_data_length - 1] == FRAME_TAIL)
	{
		switch(RS232_buff[1])
		{
			case READ_ID:
			{
				One_LED_OFF(1);
				One_LED_OFF(2);
				while(Read_ID(&send_data[2]) != MI_OK)
				{
					i ++;
					if(i >= 3)	 //���3�β��ܳɹ����򷵻�
					{
						return;
					}		
				}
				send_data[0] = FRAME_HEAD;
				send_data[1] = READ_ID;
				send_data[8] = FRAME_TAIL;
				RS232_Send_Data(send_data,9);

				if (send_data[4] == 0xAC) {
					One_LED_ON(1);

				}else {
					One_LED_ON(2);
				}
				
				break;
			}
			case READ_BANK:
			{
				while(Read_Bank(RS232_buff[2],&send_data[3]) != MI_OK)
				{
					i ++;
					if(i >= 3) //���3�β��ܳɹ����򷵻�
					{
						return;
					}						
				}
				send_data[0] = FRAME_HEAD;
				send_data[1] = READ_BANK;
				send_data[2] = RS232_buff[2];
				send_data[19] = FRAME_TAIL;
				RS232_Send_Data(send_data,20);
				break;					
			}
			case WRITE_BANK:
			{
				for(i = 0; i < 16;i ++)
				{
					Write_Bank_Data[i] = RS232_buff[i + 3];
				}
				i = 0;
				while((Write_Bank(RS232_buff[2],Write_Bank_Data)) != MI_OK)
				{
					i ++;
					if(i >= 3) //���3�β��ܳɹ����򷵻�
					{
						return;
					}						
				}
				RS232_Send_Data(RS232_buff,RS232_send_data_length);
				break;								
			}
			case PASSWORD_CHECKED:
			{
				for(i = 0;i < 6; i ++)
				{
					PassWord[i] = RS232_buff[i + 2] ;
				}
				RS232_Send_Data(RS232_buff,RS232_send_data_length);
				break;				
			}
			default:
			{
				break;
			}
		}	
	}	
}