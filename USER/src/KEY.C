/***********************************************************************
文件名称：KEY.C
功    能：按键配置
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "main.h"

static void Key_Delay(uint32_t nCount);
/***********************************************************************
函数名称：Key_Configuration(void)
功    能：完成key的配置
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：key1->PC6,
		  key2->PC7,
		  key3->PC8,
		  key4->PC9,
***********************************************************************/
void Key_Configuration(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable the GPIO_KEY Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

/***********************************************************************
函数名称：Key_Value(void)
功    能：返回按键的值
输入参数：
输出参数：按键的值,,无键按下返回0
编写时间：2013.4.25
编 写 人：
注    意：key1 ：1,
		  key2 ：2,
		  key3 ：3,
		  key4 ：4,
		  无键按下时返回 0
***********************************************************************/
unsigned char Key_Value(void)
{
	if(KEY1)//KEY1按下
	{
		Key_Delay(0XFFFF);//延时消抖
		if(KEY1)
		{
			return KEY1_VALUE;
		}	
	}
	else
	if(KEY2)//KEY2按下
	{
		Key_Delay(0XFFFF);//延时消抖
		if(KEY2)
		{
			return KEY2_VALUE;
		}	
	}
	else
	if(KEY3)//KEY3按下
	{
		Key_Delay(0XFFFF);//延时消抖
		if(KEY3)
		{
			return KEY3_VALUE;
		}	
	}
	else
	if(KEY4)//KEY4按下
	{
		Key_Delay(0XFFFF);//延时消抖
		if(KEY4)
		{
			return KEY4_VALUE;
		}	
	}
	else	//无键按下
	{
		return 0;
	}
	return 0;
}



/***********************************************************************
函数名称：Key_Delay(uint32_t nCount)
功    能：按键消抖延时
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
static void Key_Delay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }

}
