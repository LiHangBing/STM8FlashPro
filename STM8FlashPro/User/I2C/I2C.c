#include <I2C.h>

//I2C引脚
//SCL PB4
//SDA PB5

void I2C_Init_Speed(u32 speed)
{
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
	I2C_DeInit();			//关闭I2C并初始化为输入模式
	I2C_Init(speed, 0x00, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, CLK_GetClockFreq()/1000000);	//应答需要设置，在读的时候手动关闭最后的应答
	I2C_Cmd(ENABLE);
	//某些I2C异常中断会导致设备将总线拉死（时钟为高数据为低），当检测到总线被拉死时，手动恢复.TBD---
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_FAST);
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);
}

void I2C_Init_Standard_Mode(void)
{
	I2C_Init_Speed(I2C_SPEED_STANDARD);
}

void I2C_Init_Fast_Mode(void)
{
	I2C_Init_Speed(I2C_SPEED_FAST);
}

void I2C_Init_Low_Speed(void)
{
	I2C_Init_Speed(I2C_SPEED_LOW);
}

void I2C_Deinit(void)
{
	I2C_Cmd(DISABLE);
	I2C_DeInit();
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);
}

u8 I2C_Start(void)
{
	u16 timeout = 0xFFFF;
	
	/* While the bus is busy */			//不检测等待项，因为这会阻碍发起restart
  //timeout = 0xFFFF;
  //while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY))
  //  if((timeout--) == 0) return 1;

  /* Send START condition */
  I2C_GenerateSTART( ENABLE);

  /* Test on EV5 and clear it */
  timeout = 0xFFFF;
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
    if((timeout--) == 0) return 2;
  //BUSY, MSL and SB flag，起始地址位（SB）只有当读SR1寄存器随后向数据寄存器写入才清零，BUSY和MSL只有传输结束后才清零
 
	return 0;
}

void I2C_Stop(void)
{
	//以下情况会阻塞STOP的执行：1、未正确NAK:此时需要读一次状态寄存器和数据寄存器
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	if(I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED))
		I2C_ReceiveData();
	/* Send STOP condition */
    I2C_GenerateSTOP(ENABLE);
}


u8 I2C_WriteByte(u8 byte)
{
	u16 timeout = 0xFFFF;
	if(I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))		//在发送起始信号后，读SR1寄存器并向数据寄存器写入才清零
	{
		I2C_Send7bitAddress(byte & 0xfe, byte & 0x01);
		
		if(byte & 0x01 == I2C_DIRECTION_TX)			//发送，这个判断必须要有，已检验设备是否在
		{
			while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && timeout)	//BUSY, MSL, ADDR, TXE and TRA flags
				timeout--;
		}
		else										//接收，这个判断必须要有，已检验设备是否在
		{
			while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && timeout)	//BUSY, MSL and ADDR flags
				timeout--;
		}
		I2C_AcknowledgeConfig(I2C_ACK_CURR);	//如果发送的是地址，需提前开启应答，在读的时候手动关闭最后的应答
	}
	else
	{
		I2C_SendData(byte);
		while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING) && timeout)
			timeout--;
	}
	if(timeout == 0)
		return 1;
	else
		return 0;
}


//I2C读，返回0表示无错误
//由于SI I2C IP的硬件限制，建议多次调用I2C_ReadBytes读数据时，最后一次调用读取的字节数>2,否则NAK可能会失效
u8 I2C_ReadBytes(u8* buf, u8 length, bool nak_last)
{
	if(length == 0)			//非常不建议这样用
	{
		I2C_AcknowledgeConfig(nak_last ? I2C_ACK_NONE : I2C_ACK_CURR);
		return 0;
	}
	
	while(length > 3)
	{
		u16 timeout = 0xFFFF;
		while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) && timeout)		//此处不能判断ACK是否已发出
			timeout--;
		if(timeout == 0)
			return 0xFF;
		*buf++ = I2C_ReceiveData();
		length--;
	}
	//BTF的置位无法精确抓取，因此此处采取完全等待的方式
	u16 timeout = 0xFFFF;
	while(!I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) && timeout)
		if(timeout-- == 0)
			return 0xFE;
	//还剩3字节，此时寄存器状态：DR+SR+ACK 即DR和SR均已有数据，且已经ACK，那么下一次必须读一字节数据才会释放总线
	I2C_AcknowledgeConfig(nak_last ? I2C_ACK_NONE : I2C_ACK_CURR);		//如果此时启用了NAK，那么最后一次释放
	
	while(length--)
	{
		u16 timeout = 0xFFFF;
		while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) && timeout)
			timeout--;
		if(timeout == 0)
			return 0xFF;
		*buf++ = I2C_ReceiveData();
	}
	//经过上述操作，I2C能准确NACK，但是由于脚本执行速度较慢，STOP指令不能准时下达，因此I2C仍然会空读两次。。。
	//由于已经精确地NACK，因此理论上不会造成明确影响
	return 0;
}

u8 I2C_WriteBytes(u8* buf, u8 length)
{
	u8 i;
	u8 result = 0;
	
	for(i = 0; i < length; i++)
	{
		if(I2C_WriteByte(buf[i]) != 0)
		{
			result = 1;
			break;
		}
	}
	return result;
}
