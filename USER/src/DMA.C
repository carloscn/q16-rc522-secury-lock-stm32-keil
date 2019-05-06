/***********************************************************************
文件名称：DMA.C
功    能：
编写时间：
编 写 人：
注    意：本例程是通过判断两个特定的字符来确定一帧数据是否结束的。
***********************************************************************/
#include "main.h"

unsigned char *RS232_send_data = 0;
volatile unsigned char RS232_dma_send_flag = 0;


/***********************************************************************
函数名称：void RS232_DMA_Init(void) 
功    能：
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：RS232用的是串口2 对应的DMA通道是DMA1_Channel7（发送）和DMA1_Channel6（接收） 
***********************************************************************/
void RS232_DMA_Init(void)
{
	
	DMA_InitTypeDef DMA_InitStructure;
//	//启动DMA时钟  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1通道DMA1_Channel7配置  
	DMA_DeInit(DMA1_Channel7);  
	//外设地址  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);  
	//内存地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RS232_send_data;  
	//dma传输方向单向  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	//设置DMA在传输时缓冲区的长度  
	DMA_InitStructure.DMA_BufferSize = 1000;//长度无所谓，程序中还要修改  
	//设置DMA的外设递增模式，一个外设  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//设置DMA的内存递增模式  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//外设数据字长  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//内存数据字长  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
	//设置DMA的传输模式  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//设置DMA的优先级别  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//设置DMA的2个memory中的变量互相访问  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel7,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE); 
//	//使能通道DMA1_Channel7  
//	//DMA_Cmd(DMA1_Channel7, ENABLE); 

	
	//以下为DMA接收
	//串口收DMA配置    
	//启动DMA时钟  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1通道6配置  
	DMA_DeInit(DMA1_Channel6);  
	//外设地址  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);  
	//内存地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RS232_buff;  
	//dma传输方向单向  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
	//设置DMA在传输时缓冲区的长度  
	DMA_InitStructure.DMA_BufferSize = RS232_REC_BUFF_SIZE;  
	//设置DMA的外设递增模式，一个外设  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//设置DMA的内存递增模式  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//外设数据字长  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//内存数据字长  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
	//设置DMA的传输模式  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//设置DMA的优先级别  
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
	//设置DMA的2个memory中的变量互相访问  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel6,&DMA_InitStructure);  
	 
	//使能通道DMA1_Channel6  
	DMA_Cmd(DMA1_Channel6,ENABLE);  
	
}

// //串口2 DMA方式发送中断  
// void DMA1_Channel7_IRQHandler(void)  
// {  
// 	 
// 	DMA1->IFCR |= DMA1_FLAG_TC7;//清除标志位   
// 	DMA1_Channel7->CCR &= ~(1<<0);//关闭DMA     
// 	RS232_dma_send_flag = 0;//允许再次发送 
// }  

