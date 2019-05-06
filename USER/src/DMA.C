/***********************************************************************
�ļ����ƣ�DMA.C
��    �ܣ�
��дʱ�䣺
�� д �ˣ�
ע    �⣺��������ͨ���ж������ض����ַ���ȷ��һ֡�����Ƿ�����ġ�
***********************************************************************/
#include "main.h"

unsigned char *RS232_send_data = 0;
volatile unsigned char RS232_dma_send_flag = 0;


/***********************************************************************
�������ƣ�void RS232_DMA_Init(void) 
��    �ܣ�
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺RS232�õ��Ǵ���2 ��Ӧ��DMAͨ����DMA1_Channel7�����ͣ���DMA1_Channel6�����գ� 
***********************************************************************/
void RS232_DMA_Init(void)
{
	
	DMA_InitTypeDef DMA_InitStructure;
//	//����DMAʱ��  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1ͨ��DMA1_Channel7����  
	DMA_DeInit(DMA1_Channel7);  
	//�����ַ  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);  
	//�ڴ��ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RS232_send_data;  
	//dma���䷽����  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	//����DMA�ڴ���ʱ�������ĳ���  
	DMA_InitStructure.DMA_BufferSize = 1000;//��������ν�������л�Ҫ�޸�  
	//����DMA���������ģʽ��һ������  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//����DMA���ڴ����ģʽ  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//���������ֳ�  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//�ڴ������ֳ�  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
	//����DMA�Ĵ���ģʽ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//����DMA�����ȼ���  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//����DMA��2��memory�еı����������  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel7,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE); 
//	//ʹ��ͨ��DMA1_Channel7  
//	//DMA_Cmd(DMA1_Channel7, ENABLE); 

	
	//����ΪDMA����
	//������DMA����    
	//����DMAʱ��  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1ͨ��6����  
	DMA_DeInit(DMA1_Channel6);  
	//�����ַ  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);  
	//�ڴ��ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RS232_buff;  
	//dma���䷽����  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
	//����DMA�ڴ���ʱ�������ĳ���  
	DMA_InitStructure.DMA_BufferSize = RS232_REC_BUFF_SIZE;  
	//����DMA���������ģʽ��һ������  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//����DMA���ڴ����ģʽ  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//���������ֳ�  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//�ڴ������ֳ�  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
	//����DMA�Ĵ���ģʽ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//����DMA�����ȼ���  
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
	//����DMA��2��memory�еı����������  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel6,&DMA_InitStructure);  
	 
	//ʹ��ͨ��DMA1_Channel6  
	DMA_Cmd(DMA1_Channel6,ENABLE);  
	
}

// //����2 DMA��ʽ�����ж�  
// void DMA1_Channel7_IRQHandler(void)  
// {  
// 	 
// 	DMA1->IFCR |= DMA1_FLAG_TC7;//�����־λ   
// 	DMA1_Channel7->CCR &= ~(1<<0);//�ر�DMA     
// 	RS232_dma_send_flag = 0;//�����ٴη��� 
// }  
