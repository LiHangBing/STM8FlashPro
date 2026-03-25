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
#include <stm8s.h>
#include <SPI.h>
#include "defines.h"
#include "spi_cmd.h"
#include "commands.h"
#include "UART.h"

extern byte buff[buffSize];

//7 SPI初始化 ----------------------------------------------
void spi_cmd_init() {
  byte prescaler;
  byte bytesread;
  
  bytesread = UART_Receive_One_Byte(buff, TIME_OUT);      //从串口读取1字节，指示SPI时钟速度
  if (bytesread == 0) {
	  UART_Send_One_Byte(ERROR_TIMOUT, TIME_OUT);	 //超时
    return;
  }

  switch(buff[0])   //SPI时钟速度（STM8S时钟16MHz，分频可选2, 4, 8, 16, 32, 64，128）
  {
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
      prescaler = buff[0];
      break;

    default:
		UART_Send_One_Byte(ERROR_RECV, TIME_OUT);	 //接收错误
      return;
  }

  SPI_Init_Prescaler(prescaler);
  //SPI.beginTransaction(SPISettings(spi_speed, MSBFIRST, SPI_MODE0));  //设置SPI速度，发送时序
  
  UART_Send_One_Byte(FUNC_SPI_INIT, TIME_OUT); //回传cmd给串口（7）
}

//8 关闭SPI -----------------------------------------
void spi_cmd_deinit() {
  SPI_Operation_End();
  
  UART_Send_One_Byte(FUNC_SPI_DEINIT, TIME_OUT);	//回传cmd给串口（8）
}

//9  spi拉低CE引脚 -----------------------------------------
void spi_cmd_ce()
{
	SPI_Operation_Start();     //拉低CS引脚
	UART_Send_One_Byte(FUNC_SPI_CE, TIME_OUT); //回传命令码
}

//10  spi释放CE引脚 -----------------------------------------
void spi_cmd_dece()
{
	SPI_Operation_End();     //拉高CS引脚
	UART_Send_One_Byte(FUNC_SPI_DECE, TIME_OUT); //回传命令码
}

//11  spi读命令 -----------------------------------------
void spi_cmd_read() {
  byte bytesread;

  bytesread = UART_Receive_One_Byte(buff, TIME_OUT);      //从串口读取1字节，指示读的数量
  if (bytesread == 0) {
	  UART_Send_One_Byte(ERROR_TIMOUT, TIME_OUT);	 //超时
    return;
  }
  
  if (buff[0] > buffSize) {
	  UART_Send_One_Byte(ERROR_RECV, TIME_OUT);	 //接收错误
    return;
  }
  bytesread = buff[0];            //要读的数据长度

  UART_Send_One_Byte(FUNC_SPI_READ, TIME_OUT); //回传命令码
  
  SPI_Transfer(buff, bytesread);     //SPI交换数据（buff写入并保存读取的字节）
  UART_Send_Bytes(buff, bytesread, TIME_OUT);   //上传读取的数据

	UART_Send_One_Byte(FUNC_SPI_READ, TIME_OUT); //回传命令码
}

//12  spi写命令 -----------------------------------------
void spi_cmd_write() {
  byte byteswrite;
  byte bytesread;
  
  bytesread = UART_Receive_One_Byte(buff, TIME_OUT);      //从串口读取1字节，指示读的数量
  if (bytesread == 0) {
	  UART_Send_One_Byte(ERROR_TIMOUT, TIME_OUT);	 //超时
    return;
  }
  
  if (buff[0] > buffSize) {
	  UART_Send_One_Byte(ERROR_RECV, TIME_OUT);	 //接收错误
    return;
  }
  byteswrite = buff[0];            //要写的数据长度

  UART_Send_One_Byte(FUNC_SPI_WRITE, TIME_OUT); //回传命令码

  bytesread = UART_Receive_Bytes(buff, byteswrite, TIME_OUT); //从串口读取要写入的数据
  if (byteswrite != bytesread) {
    UART_Send_One_Byte(ERROR_RECV, TIME_OUT);	 //接收错误
    return;
  }

  SPI_Transfer(buff, byteswrite);                 //SPI写入
  UART_Send_One_Byte(FUNC_SPI_WRITE, TIME_OUT); //回传命令码
}

//13  测试命令，用于连通性测试，也可用于波特率识别
void spi_cmd_tst()
{
  byte bytesread;
  
  UART_Send_One_Byte(0xa5, TIME_OUT);		//发送两个识别码
  UART_Send_One_Byte(0x5a, TIME_OUT);      

  bytesread = UART_Receive_Bytes(buff, 2, TIME_OUT);  //接收两个识别码
  if (bytesread != 2 || buff[0] != 0xa5 || buff[1] != 0x5a) {
	  UART_Send_One_Byte(ERROR_RECV, TIME_OUT);	 //接收错误
    return;
  }
  
  UART_Send_One_Byte(FUNC_SPI_TST, TIME_OUT); //回传命令码
}
