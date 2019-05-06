/***********************************************************************
�ļ����ƣ�KEY.h
��    �ܣ���������
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺
***********************************************************************/

#ifndef _KEY_H_
#define _KEY_H_

/***********����Ϊ���������жϺ궨��*****************************/
#define KEY1   			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)
#define KEY2			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)
#define KEY3			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)
#define KEY4 			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)

/***********����Ϊ������ֵ�궨��*****************************/
#define KEY1_VALUE       1
#define KEY2_VALUE       2
#define KEY3_VALUE       3
#define KEY4_VALUE       4

void Key_Configuration(void);
unsigned char Key_Value(void);

#endif