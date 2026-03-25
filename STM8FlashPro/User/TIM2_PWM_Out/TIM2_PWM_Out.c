/**
  ******************************************************************************
  * @file    TIM2_PWM_Out.c
  * @author  李航兵
  * @version V1.0
  * @date    18-July-2018
  * @brief   这个文档包含了STM8S103F3P6开发板的定时器2的PWM输出功能.
  ******************************************************************************
  * @attention
  *		定时器2PWM输出可选引脚
  *			PD4(HS)/BEEP/TIM2_CH1 /UART1_CK
  *			ADC_ETR/TIM2_CH2/AIN4/(HS)PD3
  *			PA3(HS)/TIM2_CH3/[SPI_NSS]
  *		暂时不用引脚重映射
  *		使用该代码后禁止修改主频，否则将导致错误
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "TIM2_PWM_Out.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_CHANNEL		3			//PWM通道数，固定为3
#define MAX_SEQ_SIZE	16			//输出序列的最大大小
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16 frequency_value;			//计时器溢出值
u16 dutyfactor_value;			//计数器比较值

//下面用于输出序列
u8 pwm_seqence[MAX_CHANNEL][MAX_SEQ_SIZE];		//占空比数组
u8 pwm_sequece_idx[MAX_CHANNEL];		//指示序列下一个占空比
u8 pwm_sequece_size[MAX_CHANNEL];	//指示占空比序列大小，如果为0则表示未启用

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  TIM2_PWM_Out初始化.
  * @note   PA3、PD3或PD4将被占用，PWM波输出为先高后低，初始化后将会立即输出PWM波
  *			这里最后发现了一个错误，就是在调用该函数后再调用Delay_Init()，这样会使得输出频率扩大为八倍
  *				原因是修改了主频
  *				因此，使用该函数后禁止修改主频
  * @param  frequency	频率，单位Hz
  * @param  dutyfactor	占空比，为百分数
  * @param  chanel	输出的通道，取值为：
  *						0	PD4(HS)/BEEP/TIM2_CH1 /UART1_CK
  *						1	ADC_ETR/TIM2_CH2/AIN4/(HS)PD3
  *						2	PA3(HS)/TIM2_CH3/[SPI_NSS]
  * @retval None
  */
void TIM2_PWM_Out_Init(u32 frequency,u8 dutyfactor,u8 chanel)              	//TIM2_PWM_Out初始化
{
	//根据输入频率确定定时器分频系数，分频系数往小了取
	u32 sys_clk_freq;
	const u32 maxCount = 0xffff;
	const u32 maxCountT = maxCount + 1;		//定时器最大溢出值为0xffff，对应周期+1
	TIM2_Prescaler_TypeDef prescaler = TIM2_PRESCALER_1;			//从最小分频开始
	
	sys_clk_freq = CLK_GetClockFreq();
	while( (sys_clk_freq / (1<<prescaler) / maxCountT) > frequency)	//保证最大溢出值条件下得到的频率不高于所需频率
		prescaler++;
	
	assert_param(chanel < MAX_CHANNEL);
	assert_param(prescaler <= TIM2_PRESCALER_32768);
	assert_param(sys_clk_freq/(1<<prescaler)%frequency == 0);			//保证周期能精确得到
	
	frequency_value=sys_clk_freq/frequency/(1<<prescaler) - 1;		//计数周期=溢出值+1
	assert_param(frequency_value != maxCount);						//对于这种情况，由于会导致无法输出100%占空比，因此舍弃
	dutyfactor_value=(u32)(frequency_value+1)*dutyfactor/100;
		//这里u32很关键，因为frequency_value*dutyfactor可能会超u16的表示范围
	
	/* Time base configuration */
	TIM2_TimeBaseInit(prescaler, frequency_value);
	
	if(chanel==0)
	{
	  /* PWM1 Mode configuration: Channel1 */ 
	  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	  TIM2_OC1PreloadConfig(ENABLE);
	}
	else if(chanel==1)
	{
	  /* PWM1 Mode configuration: Channel2 */ 
	  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	  TIM2_OC2PreloadConfig(ENABLE);
	}
	else
	{
	  	TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
		TIM2_OC3PreloadConfig(ENABLE);
	}
	
	TIM2_ARRPreloadConfig(ENABLE);
	
	/* TIM2 enable counter */
	TIM2_Cmd(ENABLE);
}


/**
  * @brief  设置占空比.
  * @note   设置占空比后会直接输出
  * @param  dutyfactor	占空比，为百分数
  * @param  chanel	输出的通道，取值为：
  *						0	PD4(HS)/BEEP/TIM2_CH1 /UART1_CK
  *						1	ADC_ETR/TIM2_CH2/AIN4/(HS)PD3
  *						2	PA3(HS)/TIM2_CH3/[SPI_NSS]
  * @retval None
  */
void TIM2_PWM_Out_Set_Dutyfactor(u8 dutyfactor,u8 chanel)		//设置占空比
{
	assert_param(chanel < MAX_CHANNEL);
	dutyfactor_value=(u32)(frequency_value+1)*dutyfactor/100;
	if(chanel==0)
	  	TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	else if(chanel==1)
	  	TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	else
	  	TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
}


/**
  * @brief  TIM2_PWM_Out输出.
  * @note   使用最近一次的配置
  * @param  chanel	输出的通道，取值为：
  *						0	PD4(HS)/BEEP/TIM2_CH1 /UART1_CK
  *						1	ADC_ETR/TIM2_CH2/AIN4/(HS)PD3
  *						2	PA3(HS)/TIM2_CH3/[SPI_NSS]
  * @retval None
  */
void TIM2_PWM_Out_On(u8 chanel)              	//TIM2_PWM_Out输出
{
	assert_param(chanel < MAX_CHANNEL);
  	if(chanel==0)
	  	TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	else if(chanel==1)
	  	TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	else
	  	TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
}




/**
  * @brief  TIM2_PWM_Out关闭.
  * @note   使用最近一次的配置
  * @param  chanel	输出的通道，取值为：
  *						0	PD4(HS)/BEEP/TIM2_CH1 /UART1_CK
  *						1	ADC_ETR/TIM2_CH2/AIN4/(HS)PD3
  *						2	PA3(HS)/TIM2_CH3/[SPI_NSS]
  * @retval None
  */
void TIM2_PWM_Out_Off(u8 chanel)              	//TIM2_PWM_Out关闭
{
	assert_param(chanel < MAX_CHANNEL);
  	if(chanel==0)
	  	TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_DISABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	else if(chanel==1)
	  	TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_DISABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
	else
	  	TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_DISABLE,dutyfactor_value, TIM2_OCPOLARITY_HIGH);
}

/**
  * @brief  输出PWM序列，模拟DMA PWM，高电平在前
  * @note   建议使用前确保通道为固定电平,
				若初始通过TIM2_PWM_Out_Set_Dutyfactor设置为低电平（0占空比），第一个脉冲精度略低
				若初始通过TIM2_PWM_Out_Set_Dutyfactor设置为高电平（满占空比），则不会有上述问题
			如果最后要输出高/低电平，将最后一个元素设置为100/0即可
			由于采用中断方式模拟DMA，因此占空比不能设置过小，会导致响应不足、占空比不准
			由于开启了中断，因此其他功能应等待序列完成后才可用
  * @param  chanel	输出的通道，取值为：
  *						0	PD4(HS)/BEEP/TIM2_CH1 /UART1_CK
  *						1	ADC_ETR/TIM2_CH2/AIN4/(HS)PD3
  *						2	PA3(HS)/TIM2_CH3/[SPI_NSS]
  * @param  p_dutyfactor	占空比序列，数组中每一个字节表示占空比
  * @param  size	序列的大小，如果设置为0，则表示停止当前正在输出的序列
  * @retval None
  */
void TIM2_PWM_Out_Seq(u8 chanel, u8* p_dutyfactor, u8 size)	//输出PWM序列
{
	assert_param(chanel < MAX_CHANNEL);
	assert_param(size < MAX_SEQ_SIZE);
	
	TIM2_Cmd(DISABLE);
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);		//清除通道中断标志
	disableInterrupts();				//关中断
	memcpy(pwm_seqence[chanel], p_dutyfactor, size);
	pwm_sequece_size[chanel] = size;
	pwm_sequece_idx[chanel] = 0;
	enableInterrupts();					//开中断
	TIM2_PWM_Out_On(chanel);					//使能通道输出
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);			//中断设置，溢出中断用于更新占空比
	TIM2_Cmd(ENABLE);
}
u8 Get_TIM2_PWM_Out_Seq_Left(u8 chanel)						//获取序列还剩多少个未执行
{
	assert_param(chanel < MAX_CHANNEL);
	return pwm_sequece_size[chanel] - pwm_sequece_idx[chanel];
}


/**
  * @brief Timer2 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	//if(TIM2_GetITStatus(TIM2_IT_UPDATE))
	{
		bool b_all_off = TRUE;
		
		for(u8 i=0; i<MAX_CHANNEL; i++)
		{
			if(pwm_sequece_size[i] != 0)	//对应通道的序列已启用
			{
				TIM2_PWM_Out_Set_Dutyfactor(pwm_seqence[i][ pwm_sequece_idx[i] ], i);
				pwm_sequece_idx[i]++;
				if(pwm_sequece_idx[i] >= pwm_sequece_size[i])	//后面已经没有了
				{
					pwm_sequece_size[i] = 0;
					pwm_sequece_idx[i] = 0;
				}
				b_all_off = FALSE;
			}
			
		}

		if(b_all_off)
			TIM2_ITConfig( TIM2_IT_UPDATE, DISABLE);		//失能溢出中断
		TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
	}

 }


/************************ (C) COPYRIGHT 李航兵 *****END OF FILE****/