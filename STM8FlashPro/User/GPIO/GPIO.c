#include <GPIO.h>

GPIO_TypeDef* s_gpiox[] = GPIO_N;
GPIO_Pin_TypeDef s_gpio_pin_x[] = GPIO_PIN_N;
#define GPIO_SUM (sizeof(s_gpiox) / sizeof(s_gpiox[0]))

void pinMode(uint8_t pin, uint8_t mode)
{
	if(pin >= GPIO_SUM)
		return;
	
	assert_param((sizeof(s_gpiox) / sizeof(s_gpiox[0])) == (sizeof(s_gpio_pin_x) / sizeof(s_gpio_pin_x[0])));
	
	if(mode == INPUT)
	{
		GPIO_Init(s_gpiox[pin], s_gpio_pin_x[pin], GPIO_MODE_IN_PU_NO_IT);
	}
	else if(mode == OUTPUT)
	{
		GPIO_Init(s_gpiox[pin], s_gpio_pin_x[pin], GPIO_MODE_OUT_PP_LOW_FAST);
	}
	else if(mode == INPUT_PULLUP)
	{
		GPIO_Init(s_gpiox[pin], s_gpio_pin_x[pin], GPIO_MODE_IN_PU_NO_IT);
	}
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	if(pin >= GPIO_SUM)
		return;
	
	if(val == HIGH)
	{
		GPIO_WriteHigh(s_gpiox[pin], s_gpio_pin_x[pin]);
	}
	else
	{
		GPIO_WriteLow(s_gpiox[pin], s_gpio_pin_x[pin]);
	}
}

uint8_t digitalRead(uint8_t pin)
{
	if(pin >= GPIO_SUM)
		return 0;
	
	return GPIO_ReadInputPin(s_gpiox[pin], s_gpio_pin_x[pin]);
}
