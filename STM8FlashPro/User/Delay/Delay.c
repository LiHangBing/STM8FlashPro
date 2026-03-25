/**
  ******************************************************************************
  * @file    Delay.c
  * @author  李航兵
  * @version V1.2
  * @date    7-July-2018
  * @brief   这个文档包含了延时等待功能相关的函数.
  ******************************************************************************
  * @attention
  *		将使用enableInterrupts()开启中断总开关，该函数在stm8s.h中，使用此函数开启中断后才有用
  * @更新说明：
  *		支持us、ms级延时，且精度大大提高
  * @下次更新预知：
  *		OS的移植
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "Delay.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/




/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  延时系统初始化.
  * @note   Tim4将被占用.
  *			将使用enableInterrupts()开启中断总开关，该函数在stm8l10x.h中，对于IAR使用此函数
  * @param  None
  * @retval None
  */
void Delay_Init()              //延时系统初始化
{
    TIM4_Prescaler_TypeDef TIM4_Prescaler;
	
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4,ENABLE);  //使能时钟
    TIM4_Cmd(DISABLE);
    switch(CLK_GetClockFreq()/1000000)
    {
    case 4:
        TIM4_Prescaler=TIM4_PRESCALER_4;
        break;
    case 2:
        TIM4_Prescaler=TIM4_PRESCALER_2;
        break;
    case 16:
        TIM4_Prescaler=TIM4_PRESCALER_16;
        break;
    case 8:
        TIM4_Prescaler=TIM4_PRESCALER_8;
        break;
	default:
		assert_param(0);				//频率不足
		break;
    }
    TIM4_TimeBaseInit(TIM4_Prescaler,249);
                      //重装载值249，这样一个定时器重装周期为250us
                      //这里注意溢出值，应选择周期-1
    TIM4_PrescalerConfig(TIM4_Prescaler,TIM4_PSCRELOADMODE_IMMEDIATE);//立即重装
    TIM4_ARRPreloadConfig(ENABLE);      //预加载寄存器
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	//enableInterrupts();
    //TIM4_ITConfig(TIM4_IT_UPDATE,ENABLE);//暂时不用中断
	TIM4_SetCounter(0);
	TIM4_SetAutoreload(249);
    TIM4_Cmd(ENABLE);           //启动定时器
}





/**
  * @brief  微秒延时.
  * @note   
  * @param  us  延时的时间，单位us，us取值范围1~65536
  * @retval None
  */
void Delay_us(u16 us)	//微秒级延时，单位为us，范围：1~65535
{
	u8 ticks_old=TIM4->CNTR;		//前一个计数值
	u8 ticks_new;					//后一个计数值
	u16 ticks_sum=0;				//已经经过的节拍
	u16 ticks_delta=us;		//需要经过的节拍
	while(1)
	{
		ticks_new=TIM4->CNTR;	
		if(ticks_new!=ticks_old)
		{
			if(ticks_new>ticks_old)ticks_sum+=ticks_new-ticks_old;	//这里注意一下递增的计数器就可以了.
			else ticks_sum+=250+ticks_new-ticks_old;	    
			ticks_old=ticks_new;
			if(ticks_sum>=ticks_delta)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	}
}





/**
  * @brief  毫秒延时.
  * @note   
  * @param  ms  延时的时间，单位ms，范围1~65535
  * @retval None
  */
void Delay_Ms(u16 ms)           //毫秒延时
{
    u8 ticks_old=TIM4->CNTR;		//前一个计数值
	u8 ticks_new;					//后一个计数值
	s32 ticks_delta=(s32)ms*1000;		//需要经过的节拍
	while(1)
	{
		ticks_new=TIM4->CNTR;	
		if(ticks_new!=ticks_old)
		{
			if(ticks_new>ticks_old)ticks_delta-=ticks_new-ticks_old;	//这里注意一下递增的计数器就可以了.
			else ticks_delta-=250+ticks_new-ticks_old;	    
			ticks_old=ticks_new;
			if(ticks_delta<=0)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	}
}




/**
  * @brief  中断.
  * @note   操作系统用.
  * @retval None
  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
   
}



/************************ (C) COPYRIGHT 李航兵 *****END OF FILE****/