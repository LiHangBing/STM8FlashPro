#ifndef I2C_1
#define I2C_1

#include <stm8s_clk.h>
#include <stm8s_gpio.h>
#include <stm8s_i2c.h>

#define I2C_SPEED_STANDARD  100000
#define I2C_SPEED_FAST      400000
#define I2C_SPEED_LOW       10000

void I2C_Init_Speed(u32 speed);
void I2C_Init_Standard_Mode(void);
void I2C_Init_Fast_Mode(void);
void I2C_Init_Low_Speed(void);
void I2C_Deinit(void);
u8 I2C_Start(void);
void I2C_Stop(void);
//u8 I2C_SendAddress(u8 address, u8 direction);
u8 I2C_WriteByte(u8 byte);
//u8 I2C_ReadByte(u8 ack);
u8 I2C_ReadBytes(u8* buf, u8 length, bool nak_last);
u8 I2C_WriteBytes(u8* buf, u8 length);

#endif