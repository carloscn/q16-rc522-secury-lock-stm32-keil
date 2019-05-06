
#include "stm32f10x.h"
#include "SCI.H"
#include "DMA.H"
#include "stm32f10x_dma.h"
/***********************************************************************
�ļ����ƣ�SCI.C
��    �ܣ���ɶ�usart1��usart2�Ĳ���
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
***********************************************************************/

volatile unsigned char RS232_REC_Flag = 0;
volatile unsigned char RS232_buff[RS232_REC_BUFF_SIZE];//���ڽ�������
volatile unsigned int RS232_rec_counter = 0;//����RS232���ռ���
unsigned char PassWord[6] = {0xFF , 0XFF ,0xFF , 0XFF ,0xFF , 0XFF };
unsigned short RS232_send_data_length = 0;

/***********************************************************************
�������ƣ�void USART_232_Configuration(void) 
��    �ܣ�
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺RS232�õ���USART2
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
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//9λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
	USART_Init(USART2, &USART_InitStructure); 

	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
// 	USART_ITConfig(USART2, USART_IT_TC, ENABLE);// 
	
	//����DMA��ʽ����  
// 	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); 
	
	
	USART_Cmd(USART2, ENABLE); 
	USART_ClearITPendingBit(USART2, USART_IT_TC);//����ж�TCλ	
}

/***********************************************************************
�������ƣ�void USART2_IRQHandler(void) 
��    �ܣ����SCI�����ݵĽ��գ�������ʶ
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺RS232�õ���USART2
***********************************************************************/
void USART2_IRQHandler(void)  
{

	unsigned char temp = 0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)				//�����ж�
	{	
		temp = USART2->SR;  
		temp = USART2->DR; 												//��USART_IT_IDLE��־ 
		RS232_REC_Flag = 1;	   											//DMA���ձ�־
		DMA_Cmd(DMA1_Channel6,DISABLE);  							 	//��ȡ���ݳ����ȹر�DMA
		RS232_send_data_length = RS232_REC_BUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);
		//��ȡDMA���յ����ݳ��ȣ�
		DMA_SetCurrDataCounter(DMA1_Channel6,RS232_REC_BUFF_SIZE);		//���ô������ݳ���    
		DMA_Cmd(DMA1_Channel6,ENABLE);									//��DMA
	}
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)					//���ڷ����ж�
	{
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		RS232_dma_send_flag = 0;										//�����ٴη���
	}	
}
/***********************************************************************
�������ƣ�RS232_Send_Data(unsigned char *send_buff,unsigned int length)
��    �ܣ�RS232�����ַ���
���������send_buff:�����͵�����ָ�룻length�����͵����ݳ��ȣ��ַ�������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
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
�������ƣ�RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
��    �ܣ�RS232  DMA��ʽ�����ַ���
���������send_buff:�����͵�����ָ�룻length�����͵����ݳ��ȣ��ַ�������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
***********************************************************************/
void RS232_DMA_Send(unsigned char *send_buff,unsigned int length)
{
	
	unsigned int counter_232 = 0;
	while(RS232_dma_send_flag != 0)					//�ȴ��ϴε�DMA�������
	{
		counter_232 ++;
		if(counter_232 >= 0xfffffffe)
		{
			break;
		}
	}						 
	RS232_dma_send_flag = 1;
	DMA1_Channel7 -> CMAR=(u32)send_buff;   		//���÷���ָ�� 
	DMA_SetCurrDataCounter(DMA1_Channel7,length);  	//���ô��䳤��
	DMA_Cmd(DMA1_Channel7,ENABLE);					//����DMA����
}
