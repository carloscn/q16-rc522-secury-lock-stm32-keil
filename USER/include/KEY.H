/***********************************************************************
文件名称：KEY.h
功    能：按键配置
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/

#ifndef _KEY_H_
#define _KEY_H_

/***********以下为按键按下判断宏定义*****************************/
#define KEY1   			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)
#define KEY2			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)
#define KEY3			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)
#define KEY4 			!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)

/***********以下为按键键值宏定义*****************************/
#define KEY1_VALUE       1
#define KEY2_VALUE       2
#define KEY3_VALUE       3
#define KEY4_VALUE       4

void Key_Configuration(void);
unsigned char Key_Value(void);

#endif
