/**
  ******************************************************************************
  * @file    UART.c
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
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "UART.h"
#include "Delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define dat_buff_length 10


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 dat_buff[dat_buff_length];
u8* dat_receive_p;				//接收缓存指针
u8* dat_read_p;			//读取缓存指针



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  UART初始化.
  * @note   PD5和PD6将被占用
  * @note   None
  * @param  band_rate 波特率
  * @retval None
  */
void UART_Init(u32 baud_rate)          	//UART初始化
{
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1,ENABLE);
	UART1_Init(baud_rate,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);       
        //8字节数据、一位停止位、无校验
	
	
	UART1_ReceiverWakeUpCmd(DISABLE);
	enableInterrupts();			//开启中断，用于接收
	
	UART1_ITConfig(UART1_IT_RXNE_OR,ENABLE);
	/*
	这里库文件有个问题，该函数说明为：
			@brief  Enables or disables the specified USART interrupts.
  * @param  UART1_IT specifies the USART interrupt sources to be enabled or disabled.
  *         This parameter can be one of the following values:
  *         - UART1_IT_TXE:  Tansmit Data Register empty interrupt
  *         - UART1_IT_TC:   Transmission complete interrupt
  *         - UART1_IT_RXNE: Receive Data register not empty interrupt
  *         - UART1_IT_OR: Overrun error interrupt
  *         - UART1_IT_IDLE: Idle line detection interrupt
  *         - USRT1_IT_ERR:  Error interrupt
  * @param  NewState new state of the specified USART interrupts.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
	但实际上，参数值：UART1_IT_RXNE是无效的，因为下面有：
	assert_param(IS_UART1_CONFIG_IT_OK(UART1_IT));
	具体为：
	#define IS_UART1_CONFIG_IT_OK(Interrupt) \
  (((Interrupt) == UART1_IT_PE) || \
   ((Interrupt) == UART1_IT_TXE) || \
   ((Interrupt) == UART1_IT_TC) || \
   ((Interrupt) == UART1_IT_RXNE_OR ) || \
   ((Interrupt) == UART1_IT_IDLE) || \
   ((Interrupt) == UART1_IT_LBDF))
	这就说明在判断时，UART1_IT_RXNE是无效的，结果就是进入assert_failed，也就是死机
	*/
	UART1_ClearFlag(UART1_FLAG_RXNE);
	UART1_ClearITPendingBit(UART1_IT_RXNE);		//接收中断
	UART1_Cmd(ENABLE);
	dat_receive_p=&dat_buff[0];
	dat_read_p=&dat_buff[0];
}






/**
  * @brief  UART发送一个字节.
  * @note   None
  * @param  dat	发送的数据
  * @param  time_out	超时时间，单位ms，<0 表示永久等待，0表示立即返回
  * @retval None
  */
void UART_Send_One_Byte(u8 dat, s8 time_out)			//UART发送一个字节
{
	if(time_out < 0)
		while(!UART1_GetFlagStatus(UART1_FLAG_TXE));
	else
		while(!UART1_GetFlagStatus(UART1_FLAG_TXE) && time_out--)
			Delay_us(1000);
	UART1_SendData8(dat);
}



/**
  * @brief  UART发送多个字节.
  * @note   None
  * @param  dat	发送的数据
  * @param  dat_width	发送数据的长度.
  * @param  time_out	超时时间，单位ms，<0 表示永久等待，0表示立即返回
  * @retval None
  */
void UART_Send_Bytes(u8* dat,u8 dat_width, s8 time_out)			//UART发送
{
  	while(dat_width--)
	  	UART_Send_One_Byte(*dat++, time_out);
}



/**
  * @brief  UART接收一个字节.
  * @note   None
  * @param  dat	接收的数据地址
  * @param  time_out	超时时间，单位ms，<0 表示永久等待，0表示立即返回
  * @retval 接收是否成功
  *				TRUE：成功
  *				FALSE：失败
  */
bool UART_Receive_One_Byte(u8* dat, s8 time_out)			//UART接收一个字节
{
	if(time_out < 0)
		while(dat_receive_p==dat_read_p);
	else
		while(dat_receive_p==dat_read_p && time_out--)
			Delay_us(1000);
	
  	if(dat_receive_p!=dat_read_p)
	{
	  	*dat=*dat_read_p;
		if(dat_read_p==&dat_buff[dat_buff_length-1])
		  	dat_read_p=&dat_buff[0];
		else
		  	dat_read_p++;
		return TRUE;
	}
	return FALSE;
}



/**
  * @brief  UART接收多个字节.
  * @note   None
  * @param  dat	接收的数据地址
  * @param  dat_width	接收数据字节数
  * @param  time_out	超时时间，单位ms，<0 表示永久等待，0表示立即返回
  * @retval 接收到的有效字节数
  */
u8 UART_Receive_Bytes(u8* dat,u8 dat_width, s8 time_out)	//USART接收
{
	u8 data_received = 0;
	while(dat_width--)
	{
		if( UART_Receive_One_Byte(dat, time_out) )
		{
			dat++;
			data_received++;
		}
		else
			return data_received;
	}
	return data_received;
}

#define dat_buff_length 10
u8* dat_receive_p;				//接收缓存指针
u8* dat_read_p;			//读取缓存指针

u8 UART_Available()						//返回可读数据量
{
	disableInterrupts();		//关闭中断
	u8 available = dat_read_p >= dat_receive_p ? dat_read_p - dat_receive_p : dat_buff_length + dat_read_p - dat_receive_p;
	enableInterrupts();			//开启中断
	return available;
}


/**
  * @brief  重定义putchar，可使printf标准使用.
  * @note   None
  * @param  Unknown
  * @retval Unknown
  */
int putchar(int c)
{
	UART_Send_One_Byte((u8)c, -1);
	return c;
}




/**
  * @brief UART1 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
   	if(UART1_GetITStatus(UART1_IT_RXNE))
	{
		if(UART1_GetFlagStatus(UART1_FLAG_RXNE))
		{
			*dat_receive_p=UART1_ReceiveData8();
			if(dat_receive_p==&dat_buff[dat_buff_length-1])
		  		dat_receive_p=&dat_buff[0];
			else
		  		dat_receive_p++;
		}
		UART1_ClearFlag(UART1_FLAG_RXNE);
		UART1_ClearITPendingBit(UART1_IT_RXNE);
	}
 }