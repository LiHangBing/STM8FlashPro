/**
  ******************************************************************************
  * @file    TIM2_PWM_Out.h
  * @author  李航兵
  * @version V1.0
  * @date    18-July-2018
  * @brief   这个文档包含了STM8S103F3P6开发板的定时器2的PWM输出功能
  ******************************************************************************
  * @attention
  *     定时器2PWM输出可选引脚
  *			PD4(HS)/BEEP/TIM2_CH1 /UART1_CK
  *			ADC_ETR/TIM2_CH2/AIN4/(HS)PD3
  *			PA3(HS)/TIM2_CH3/[SPI_NSS]
  *		暂时不用引脚重映射
  *		使用该代码后禁止修改主频，否则将导致错误
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM2_PWM_OUT_H
#define __TIM2_PWM_OUT_H


/* Includes ------------------------------------------------------------------*/
#include "stm8s_gpio.h"
#include "stm8s_clk.h"
#include "stm8s_tim2.h"
#include "string.h"

/* Exported variables ------------------------------------------------------- */
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/



/* Exported functions ------------------------------------------------------- */




void TIM2_PWM_Out_Init(u32 frequency,u8 dutyfactor,u8 chanel);     //TIM2_PWM_Out初始化
void TIM2_PWM_Out_Set_Dutyfactor(u8 dutyfactor,u8 chanel);		//设置占空比
void TIM2_PWM_Out_On(u8 chanel);              	//TIM2_PWM_Out输出
void TIM2_PWM_Out_Off(u8 chanel);              	//TIM2_PWM_Out关闭
void TIM2_PWM_Out_Seq(u8 chanel, u8* p_dutyfactor, u8 size);	//输出PWM序列，模拟DMA PWM
u8 Get_TIM2_PWM_Out_Seq_Left(u8 chanel);						//获取序列还剩多少个未执行




#endif /* __TIM2_PWM_OUT_H */

/************************ (C) COPYRIGHT 李航兵 *****END OF FILE****/