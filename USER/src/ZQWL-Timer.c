



#include "LED.H"

#include "stm32f10x.h"
extern unsigned char Count_ns;
 unsigned int Count=0;
 unsigned char Time1S_Flag=1;

 void TIM2_Configuration(void)
 {
   
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //开启Timer的时钟

   TIM_TimeBaseStructure.TIM_Period = 1000; //设置自动装载寄存器      

   TIM_TimeBaseStructure.TIM_Prescaler = 35999; //分频计数 例如：时钟频率=72/(时钟预分频+1)	=2Khz   ((1+TIM_Prescaler )/72M)*(1+TIM_Period )=((1+35999)/72M)*(1+2000)=1秒*/

   TIM_TimeBaseStructure.TIM_ClockDivision = 0;//时间分割值
   
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //选择向上计数

   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  

   TIM_Cmd(TIM2, ENABLE); //是能定时器

   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//使能定时器的中断
  
   TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

 }

void TIM2_IRQHandler(void)
{

//所以这个更新事件的中断判断要依靠以下语句：

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
