#ifndef GPIO_H
#define GPIO_H

#include "stm8s_gpio.h"


//{PC3 PC4 PD2 PD3 PD4 }			//PB4 PB5因不支持PP输出，不使用
#define GPIO_N		{GPIOC, 		GPIOC, 		GPIOD, 		GPIOD, 		GPIOD}
#define GPIO_PIN_N	{GPIO_PIN_3,	GPIO_PIN_4,	GPIO_PIN_2,	GPIO_PIN_3,	GPIO_PIN_4}


//移植自arduino{
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define HIGH 0x1
#define LOW  0x0
//}

void digitalWrite(uint8_t pin, uint8_t val);
uint8_t digitalRead(uint8_t pin);
void pinMode(uint8_t pin, uint8_t mode);


#endif