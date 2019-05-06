



#include "LED.H"

#include "stm32f10x.h"
extern unsigned char Count_ns;
 unsigned int Count=0;
 unsigned char Time1S_Flag=1;

 void TIM2_Configuration(void)
 {
   
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //����Timer��ʱ��

   TIM_TimeBaseStructure.TIM_Period = 1000; //�����Զ�װ�ؼĴ���      

   TIM_TimeBaseStructure.TIM_Prescaler = 35999; //��Ƶ���� ���磺ʱ��Ƶ��=72/(ʱ��Ԥ��Ƶ+1)	=2Khz   ((1+TIM_Prescaler )/72M)*(1+TIM_Period )=((1+35999)/72M)*(1+2000)=1��*/

   TIM_TimeBaseStructure.TIM_ClockDivision = 0;//ʱ��ָ�ֵ
   
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //ѡ�����ϼ���

   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  

   TIM_Cmd(TIM2, ENABLE); //���ܶ�ʱ��

   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//ʹ�ܶ�ʱ�����ж�
  
   TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

 }

void TIM2_IRQHandler(void)
{

//������������¼����ж��ж�Ҫ����������䣺

if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
	  Time1S_Flag=0;
	  Count++;
	  
	  if(Count%2)
	  {
	   LED1_ON ;
	  }
	  else
	  {
	   LED1_OFF	;
	  }
		
	  if(Count_ns!=0)
	     Count_ns--;
	  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
