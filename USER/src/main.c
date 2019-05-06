
#include "main.h"

int main(void)
{  
	delay_init();
	LED_Configuration();
	NVIC_Configuration();
	USART_232_Configuration();
	RS232_DMA_Init();
    WIRELESS_SPI_Init()  ;
	RC522_Reset();
	
	while(1) { 
		if(RS232_REC_Flag == 1) {		 
			Parse_Data();					
			RS232_REC_Flag = 0;
		} 
	}
}

