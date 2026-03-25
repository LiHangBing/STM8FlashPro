#include <SPI.h>



/******************************************
函数名：SPI_Init_Prescaler
参数：prescaler，分频系数，SPI时钟速度（分频可选2, 4, 8, 16, 32, 64，128）
返回值：无
功能：初始化SPI接口
*********************************************/
void SPI_Init_Prescaler(u8 prescaler)			//SPI初始化
{
	SPI_BaudRatePrescaler_TypeDef spi_baudratescaler;
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);		//CE推挽输出高电平
	GPIO_Init(GPIOC,GPIO_PIN_6|GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);	//MOSI SCk推挽输出，高
	GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT);									//MISO上拉输入，高
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI,ENABLE);
	SPI_Cmd(DISABLE);
	SPI_DeInit();
	switch(prescaler)
	{
		case 2:		spi_baudratescaler = SPI_BAUDRATEPRESCALER_2;	break;
		case 4:		spi_baudratescaler = SPI_BAUDRATEPRESCALER_4;	break;
		case 8:		spi_baudratescaler = SPI_BAUDRATEPRESCALER_8;	break;
		case 16:	spi_baudratescaler = SPI_BAUDRATEPRESCALER_16;	break;
		case 32:	spi_baudratescaler = SPI_BAUDRATEPRESCALER_32;	break;
		case 64:	spi_baudratescaler = SPI_BAUDRATEPRESCALER_64;	break;
		case 128:	spi_baudratescaler = SPI_BAUDRATEPRESCALER_128;	break;
		default:	spi_baudratescaler = SPI_BAUDRATEPRESCALER_128;	break;
	}
	SPI_Init(SPI_FIRSTBIT_MSB, spi_baudratescaler, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, 		
					SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);		//此处CRC暂时无用，但不能置0，否则进入assert_failed
	SPI_Cmd(ENABLE);
}


/******************************************
函数名：SPI_Init_Low_Speed
参数：无
返回值：无
功能：初始化SPI接口(低速)
*********************************************/
void SPI_Init_Low_Speed(void)			//SPI初始化(低速)
{
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);		//CE推挽输出高电平
	GPIO_Init(GPIOC,GPIO_PIN_6|GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);	//MOSI SCk推挽输出，高
	GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT);									//MISO上拉输入，高
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI,ENABLE);
	SPI_Cmd(DISABLE);
	SPI_DeInit();
	SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_256, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, 		
					SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);		//此处CRC暂时无用，但不能置0，否则进入assert_failed
	SPI_Cmd(ENABLE);
}



/******************************************
函数名：SPI_Init
参数：无
返回值：无
功能：初始化SPI接口
*********************************************/
void SPI_Init_High_Speed(void)						//SPI初始化
{
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);		//CE推挽输出高电平
	GPIO_Init(GPIOC,GPIO_PIN_6|GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);	//MOSI SCk推挽输出，高
	GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT);									//MISO上拉输入，高
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI,ENABLE);
	SPI_Cmd(DISABLE);
	SPI_DeInit();
	SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_2, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, 		
					SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
	SPI_Cmd(ENABLE);
}



/******************************************
函数名：SPI_Operation_Start
参数：无
返回值：无
功能：开始SPI操作
*********************************************/
void SPI_Operation_Start(void)		//开始SPI操作
{
	GPIO_WriteLow(GPIOA, GPIO_PIN_3);
}



/******************************************
函数名：SPI_Operation_End
参数：无
返回值：无
功能：结束SPI操作
*********************************************/
void SPI_Operation_End(void)		//结束SPI操作
{
	GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
}



/******************************************
函数名：SPI_Read_Write_One_Byte
参数：写入的字节
返回值：读出的字节
功能：SPI读写一字节
*********************************************/
u8 SPI_Read_Write_One_Byte(u8 byte)			//SPI读写一字节
{
	uint16_t wait_time = 0;
	while( RESET == SPI_GetFlagStatus( SPI_FLAG_TXE )&&SPI_GetFlagStatus(SPI_FLAG_BSY)==SET )		//等待发送缓冲区空以及SPI空闲
	{
		wait_time++;
		if(wait_time==0xffff)
		{
			break;			//等待超时
		}
	}
	SPI_SendData( byte );	//发送数据
	wait_time = 0xffff / 2;
	
	
	
	while( RESET==SPI_GetFlagStatus( SPI_FLAG_RXNE )&&SPI_GetFlagStatus(SPI_FLAG_BSY)==SET )	//等待接收缓冲区非空以及SPI空闲
	{
		wait_time++;
		if( wait_time==0xffff )
		{
			break;			//等待超时
		}
	}
	byte = SPI_ReceiveData( );
	return byte;
}


void SPI_Transfer(u8* buf, u8 length)			//SPI交换一定数量字节
{
	while(length--)
	{
		*buf = SPI_Read_Write_One_Byte(*buf);
		buf++;
	}
}


