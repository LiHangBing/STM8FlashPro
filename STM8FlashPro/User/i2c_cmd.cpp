/*
	该程序基于简易的arduino硬件，主要目的是烧录SPI Flash，如W25QXX等，主要原理是实现了UART转SPI的功能，需配合上位机使用。
  也可烧录eepron，如24cxx
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



//#include <arduino.h>
//#include "src/Wire_new.h"             //i2c支持库
#include <stm8s.h>
#include "i2c.h"
#include "defines.h"
#include "i2c_cmd.h"
#include "commands.h"
#include "UART.h"

extern byte buff[buffSize];

//20 I2C初始化 ----------------------------------------------
void i2c_cmd_init() {
  long i2c_speed;
  byte bytesread;

  bytesread = UART_Receive_One_Byte(buff, TIME_OUT);      //从串口读取1字节，指示I2C时钟速度
  if (bytesread == 0) {
	  UART_Send_One_Byte(ERROR_TIMOUT, TIME_OUT);	 //超时
    return;
  }

  switch(buff[0])   //I2C时钟速度，可设置为0(standard mode 100K)、1(fast mode 400K). 2(low speed mode 10K), 
  {
    case 0:
      i2c_speed = I2C_SPEED_STANDARD;
      break;
    case 1:
      i2c_speed = I2C_SPEED_FAST;
      break;
    case 2:
      i2c_speed = I2C_SPEED_LOW;
      break;

    default:
      UART_Send_One_Byte(ERROR_RECV, TIME_OUT);	 //接收错误
      return;
  }

  I2C_Init_Speed(i2c_speed);
  
  UART_Send_One_Byte(FUNC_I2C_INIT, TIME_OUT);     //回传cmd给串口
}

//21 关闭I2C ----------------------------------------------
void i2c_cmd_deinit() {
  I2C_Deinit();

  UART_Send_One_Byte(FUNC_I2C_DEINIT, TIME_OUT);
}


//i2c起始信号
void i2c_cmd_start(){
  byte bytesret;

  bytesret = I2C_Start();
  
  if(bytesret == 0)
    UART_Send_One_Byte(FUNC_I2C_START, TIME_OUT);
  else
    UART_Send_One_Byte(ERROR_OPERAT, TIME_OUT);
}



//i2c停止信号
void i2c_cmd_stop()
{
  I2C_Stop();

  UART_Send_One_Byte(FUNC_I2C_STOP, TIME_OUT);
}





//21 I2C读 ----------------------------------------------
void i2c_cmd_read() {
  byte bytesread;
  byte nack_last;
  byte bytesret;

  bytesread = UART_Receive_Bytes(buff, 2, TIME_OUT);      //从串口读取2字节，指示读取的长度,和最后是否NACK
  if (bytesread != 2) {
    UART_Send_One_Byte(ERROR_TIMOUT, TIME_OUT);	 //超时
    return;
  }

  bytesread = buff[0];
  nack_last = buff[1];
  if(bytesread > buffSize){
	UART_Send_One_Byte(ERROR_RECV, TIME_OUT);   //命令错误
    return;
  }

  UART_Send_One_Byte(FUNC_I2C_READ, TIME_OUT);

  bytesret = I2C_ReadBytes(buff, bytesread, nack_last);     //读数据
  UART_Send_Bytes(buff, bytesread, TIME_OUT);   //上传读取的数据

  if(bytesret == 0)
    UART_Send_One_Byte(FUNC_I2C_READ, TIME_OUT);
  else
    UART_Send_One_Byte(ERROR_OPERAT, TIME_OUT);
}

//22 I2C写
void i2c_cmd_write() {
  byte byteswrite;
  byte bytesread;
  byte bytesret;

  bytesread = UART_Receive_One_Byte(buff, TIME_OUT);      //从串口读取1字节，指示写入的长度
  if (bytesread == 0) {
    UART_Send_One_Byte(ERROR_TIMOUT, TIME_OUT);	 //超时
    return;
  }

  byteswrite = buff[0];
  if(byteswrite > buffSize){
    UART_Send_One_Byte(ERROR_RECV, TIME_OUT);
    return;
  }

  UART_Send_One_Byte(FUNC_I2C_WRITE, TIME_OUT);  //回传命令码

  bytesread = UART_Receive_Bytes(buff, byteswrite, TIME_OUT);
  if (byteswrite != bytesread) {
    UART_Send_One_Byte(ERROR_RECV, TIME_OUT);
    return;
  }

  bytesret = I2C_WriteBytes(buff, byteswrite);    //写入数据，并获取ack

  if(bytesret == 0)
    UART_Send_One_Byte(FUNC_I2C_WRITE, TIME_OUT); //回传命令码
  else
    UART_Send_One_Byte(ERROR_OPERAT, TIME_OUT);   //收到NACK或超时，i2c写入失败
}


//28 测试命令，用于连通性测试
void i2c_cmd_tst()
{
  byte bytesread;

  UART_Send_One_Byte(0xa5, TIME_OUT);
  UART_Send_One_Byte(0x5a, TIME_OUT);

  bytesread = UART_Receive_Bytes(buff, 2, TIME_OUT);  //接收两个识别码
  if (bytesread != 2 || buff[0] != 0xa5 || buff[1] != 0x5a) {
    UART_Send_One_Byte(ERROR_RECV, TIME_OUT);
    return;
  }
  
  UART_Send_One_Byte(FUNC_I2C_TST, TIME_OUT); //回传命令码
}
