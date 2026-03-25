/*
	该部分仅用于调试，显示最近一次错误码
    Copyright (C) 2023  LiHangBing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/



#include "Dbg.h"
#include "TIM2_PWM_Out.h"


/*
输出超慢的PWM波以指示信息码,周期约为500ms
T=0.5S，其中70%时间亮表示bit1，30%时间亮表示bit0
*/
#define DBG_FREQ_HZ		2				//慢速PWM的频率，Hz
#define DBG_NULL_RATE	100				//占空比百分数，如果接的LED阴极，需要对100取补
#define DBG_BIT0_RATE	70
#define DBG_BIT1_RATE	30
#define DBG_LED_CHN		2					//LED通道，2=PA3(HS)/TIM2_CH3/[SPI_NSS]

#define CODE_WIDTH	8			//指示代码的位数
uint8_t code_rate[CODE_WIDTH+1]; 		//标记每一个脉冲的计数值以指示0或1，最后一个脉冲用于关闭




/*
配置定时器工作在1KHz左右的计数频率
*/
void Dbg_init()
{
	TIM2_PWM_Out_Init(DBG_FREQ_HZ, DBG_NULL_RATE, DBG_LED_CHN);
}

//小端字节序，多余高位舍弃
void Dbg_set(u8* code, u8 width)
{
	assert_param(width <= CODE_WIDTH);
	while(Get_TIM2_PWM_Out_Seq_Left(DBG_LED_CHN));
	
	for(u8 i=0; i<width/8; i++)			//8位整字节部分
		for(u8 j=0; j<8; j++)
			code_rate[i*8+j] = code[i] & (1<<j) ? DBG_BIT1_RATE : DBG_BIT0_RATE;
	
	for(u8 j=0; j< width%8; j++)
		code_rate[width-width%8 +j] = code[width/8] & (1<<j) ? DBG_BIT1_RATE : DBG_BIT0_RATE;
	
	code_rate[width] = DBG_NULL_RATE;			//最后补一个关灯
	TIM2_PWM_Out_Seq(DBG_LED_CHN, code_rate, width+1);
}

void Dbg_set8(u8 code)
{
	Dbg_set(&code, 8);
}



