
#include "spi.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "rc522.h"

//串行外设接口SPI的初始化，SPI配置成主模式							  

void WIRELESS_SPI_Init(void)
{	 
   	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_RC522_CS, ENABLE);   //使能GPIO的时钟
    GPIO_InitStructure.GPIO_Pin = RC522_CS;      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_RC522_CS, &GPIO_InitStructure);





    
    // Enable SPI and GPIO clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

    ////SPI口初始化 Set PC10,11,12 as Output push-pull - SCK, MISO and MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //GPIO_SetBits(GPIO_SPI2,SPI2_MISO| SPI2_MOSI| SPI2_SCK);

	/* SPI2 configuration */                                            //初始化SPI结构体
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为两线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                //设置SPI为主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		            //SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                    //串行时钟在不操作时，时钟为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                    //第一个时钟沿开始采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                    //NSS信号由软件（使用SSI位）管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;  //SPI波特率预分频值为8
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                        //CRC值计算的多项式

	SPI_Init(SPI_WIRELESS, &SPI_InitStructure);   //根据SPI_InitStruct中指定的参数初始化外设SPI2寄存器
	
	/* Enable SPI2  */
	SPI_Cmd(SPI_WIRELESS, ENABLE);                                      //使能SPI外设
	
	SPI_ReadWriteByte(0xff);                                            //启动传输		 
}  

u8 SPI_ReadWriteByte(u8 TxData)                                        //SPI读写数据函数
{		
	u8 retry=0;				 	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI_WIRELESS, SPI_I2S_FLAG_TXE) == RESET)      //发送缓存标志位为空
		{
		retry++;
		if(retry>200)return 0;
		}			  
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI_WIRELESS, TxData);                                    //通过外设SPI1发送一个数据
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI_WIRELESS, SPI_I2S_FLAG_RXNE) == RESET);   //接收缓存标志位不为空
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI_WIRELESS);                                 //通过SPI1返回接收数据				    
}






















