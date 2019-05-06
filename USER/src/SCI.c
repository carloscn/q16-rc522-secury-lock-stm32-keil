
#include "stm32f10x.h"
#include "SCI.H"
#include "DMA.H"
#include "stm32f10x_dma.h"
/***********************************************************************
文件名称：SCI.C
功    能：完成对usart1和usart2的操作
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/

volatile unsigned char RS232_REC_Flag = 0;
volatile unsigned char RS232_buff[RS232_REC_BUFF_SIZE];//用于接收数据
volatile unsigned int RS232_rec_counter = 0;//用于RS232接收计数
unsigned char PassWord[6] = {0xFF , 0XFF ,0xFF , 0XFF ,0xFF , 0XFF };
unsigned short RS232_send_data_length = 0;

/***********************************************************************
函数名称：void USART_232_Configuration(void) 
功    能：
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：RS232用的是USART2
***********************************************************************/
void USART_232_Configuration(void)
{ 
  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);		   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//9位数据
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
	USART_Init(USART2, &USART_InitStructure); 

	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
// 	USART_ITConfig(USART2, USART_IT_TC, ENABLE);// 
	
	//采用DMA方式发送  
// 	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  
	//采用DMA方式接收  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 
	
	
	USART_Cmd(USART2, ENABLE); 
	USART_ClearITPendingBit(USART2, USART_IT_TC);//清除中断TC位	
}

/***********************************************************************
函数名称：void USART2_IRQHandler(void) 
功    能：完成SCI的数据的接收，并做标识
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：RS232用的是USART2
***********************************************************************/
void USART2_IRQHandler(void)  
{

	unsigned char temp = 0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)				//空闲中断
	{	
		temp = USART2->SR;  
		temp = USART2->DR; 												//清USART_IT_IDLE标志 
		RS232_REC_Flag = 1;	   											//DMA接收标志
		DMA_Cmd(DMA1_Channel6,DISABLE);  							 	//读取数据长度先关闭DMA
		RS232_send_data_length = RS232_REC_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);
		//获取DMA接收的数据长度，
		DMA_SetCurrDataCounter(DMA1_Channel6,RS232_REC_BUFF_SIZE);		//设置传输数据长度    
		DMA_Cmd(DMA1_Channel6,ENABLE);									//打开DMA
	}
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)					//串口发送中断
	{
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		RS232_dma_send_flag = 0;										//允许再次发送
	}	
}
/***********************************************************************
函数名称：RS232_Send_Data(unsigned char *send_buff,unsigned int length)
功    能：RS232发送字符串
输入参数：send_buff:待发送的数据指针；length：发送的数据长度（字符个数）
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/
void RS232_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	for(i = 0;i < length;i ++)
	{			
		USART2->DR = send_buff[i];
		while((USART2->SR&0X40)==0);	
	}	
}
/***********************************************************************
函数名称：RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
功    能：RS232  DMA方式发送字符串
输入参数：send_buff:待发送的数据指针；length：发送的数据长度（字符个数）
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/
void RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
{
	
	unsigned int counter_232 = 0;
	while(RS232_dma_send_flag != 0)					//等待上次的DMA传输完成
	{
		counter_232 ++;
		if(counter_232 >= 0xfffffffe)
		{
			break;
		}
	}						 
	RS232_dma_send_flag = 1;
	DMA1_Channel7 -> CMAR=(u32)send_buff;   		//设置发送指针 
	DMA_SetCurrDataCounter(DMA1_Channel7,length);  	//设置传输长度
	DMA_Cmd(DMA1_Channel7,ENABLE);					//启动DMA传输
}
