/***********************************************************************
文件名称：DMA.H
功    能：
编写时间：
编 写 人：
注    意：本例程是通过判断两个特定的字符来确定一帧数据是否结束的。
***********************************************************************/
#ifndef _DMA_H_
#define _DMA_H_



extern volatile unsigned char RS232_dma_send_flag;

void RS232_DMA_Init(void);
void DMA_RS232_Send_Configuration(unsigned char *send_buff,unsigned int send_size);
#endif
