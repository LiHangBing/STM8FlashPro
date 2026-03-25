/*
该程序基于简易的arduino硬件，主要目的是烧录SPI Flash，如W25QXX等，主要原理是实现了UART转SPI的功能，需配合上位机使用。
    也可烧录eepron，如24cxx
	甚至于控制多达8路的GPIO
*/

#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_clk.h"

#include "Delay.h"
//#include "Dbg.h"					//PA3连接LED和NSS，因此不在用于调试闪烁灯
#include "UART.h"

#include "commands.h"

		
//$PROJ_DIR$\lnkstm8s103f3-app.icf
#define UART_BAUD_RATE	115200				//已测试通过的最大波特率（在CH340下读写flash）：200000


void sys_init();



void main( void )
{
	sys_init();
	Delay_Init();
	//Dbg_init();					//PA3连接LED和NSS，因此不在用于调试闪烁灯
	UART_Init(UART_BAUD_RATE);

	while(1)
	{
		u8 cmd;
		if(UART_Receive_One_Byte(&cmd, -1))
		{
    		ParseCommand(cmd);              //解析命令
		}
	}
}


void sys_init()
{
	CLK_DeInit();
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif