#ifndef DEFINES_H
#define DEFINES_H

#include "stm8s.h"


#define TIME_OUT	-1		//串口通信超时时间，-1表示永久等待

//移植自Arduino版
typedef uint8_t byte;

#define buffSize 64         //缓冲区长度（不能超过串口缓冲区大小 64）

#define ISP_RST   10        //复位引脚可随意，下面为硬件决定
#define ISP_MOSI  16
#define ISP_MISO  14
#define ISP_SCK   15
//Arduino Pro or Pro Mini   11 12 13
//Arduino Leonardo          16 14 15
//STM8S103F3P6				不在此处指定


#define I2C_SCL   A5
#define I2C_SDA   A4
//Arduino Pro or Pro Mini   A5 A4   pullup is needed
//STM8S103F3P6				不在此处指定


#endif
