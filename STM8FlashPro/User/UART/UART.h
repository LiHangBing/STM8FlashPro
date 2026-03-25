/**
  ******************************************************************************
  * @file    UART.h
  * @author  李航兵
  * @version V1.0
  * @date    10-July-2018
  * @brief   这个文档包含了STM8S103F3P6的串口功能.
  ******************************************************************************
  * @attention
  *		将使用enableInterrupts()开启中断总开关，该函数在stm8s.h中，使用此函数开启中断后才有用
  *		使用串口进行通信
  *		将printf绑定到串口中，方便使用
  *		接收采用中断的形式
  *		使用两个指针指示当前即将写入/读取的区域
  *		数据缓冲区为FIFO结构
  *		占用引脚：
  *			PD5(HS) /AIN5/UART1_TX
  *			PD6(HS) /AIN6/UART1_RX
  *
  *		本想实现scanf和printf的，结果printf实现完成，但scanf的实现占用空间过大，无法实现
  *			因此该板子上无法实现scanf，而printf的实现也已经占用了较大空间，意义不大
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H


/* Includes ------------------------------------------------------------------*/
#include "stm8s_clk.h"
#include "stm8s_uart1.h"
#include "stm8s_it.h"
#include "stdio.h"


/* Exported variables ------------------------------------------------------- */
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/






void UART_Init(u32 baud_rate);          	//UART初始化
void UART_Send_One_Byte(u8 dat, s8 time_out);			//UART发送一个字节
void UART_Send_Bytes(u8* dat,u8 dat_width, s8 time_out);			//UART发送
bool UART_Receive_One_Byte(u8* dat, s8 time_out);			//UART接收一个字节
u8 UART_Receive_Bytes(u8* dat,u8 dat_width, s8 time_out);	//UART接收
u8 UART_Available();						//返回可读数据量









#endif /* __UART_H */

/************************ (C) COPYRIGHT 李航兵 *****END OF FILE****/