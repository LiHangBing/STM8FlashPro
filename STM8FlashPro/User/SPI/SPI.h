#ifndef SPI_1
#define SPI_1
//此处加上_1是因为头文件中有定义了
#include <stm8s_clk.h>
#include <stm8s_gpio.h>
#include <stm8s_spi.h>

//SPI引脚：	SCk：PC5		MOSI：PC6	MISO：PC7
//			NSS：PA3（可不用）


void SPI_Init_Prescaler(u8 prescaler);			//SPI初始化
void SPI_Init_Low_Speed(void);
void SPI_Init_High_Speed(void);						//SPI初始化
void SPI_Operation_Start(void);		//开始SPI操作
void SPI_Operation_End(void);		//结束SPI操作
u8 SPI_Read_Write_One_Byte(u8 byte);			//SPI读写一字节
void SPI_Transfer(u8* buf, u8 length);			//SPI交换一定数量字节






#endif