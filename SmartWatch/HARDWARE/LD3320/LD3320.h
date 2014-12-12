#ifndef __LD3320_H
#define	__LD3320_H

#include "stm32f10x.h"
#include <stdio.h>
#include "usart.h"
#include "LD3320_config.h"
#include "key.h"
#include "sim900a.h"
#include "usart2.h"

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

///以下三个状态定义用来记录程序是在运行ASR识别还是在运行MP3播放
#define LD_MODE_IDLE			0x00
#define LD_MODE_ASR_RUN	    	0x08
#define LD_MODE_MP3		 		0x40

///以下五个状态定义用来记录程序是在运行ASR识别过程中的哪个状态
#define LD_ASR_NONE					0x00	//表示没有在作ASR识别
#define LD_ASR_RUNING				0x01	//表示LD3320正在作ASR识别中
#define LD_ASR_FOUNDOK				0x10	//表示一次识别流程结束后，有一个识别结果
#define LD_ASR_FOUNDZERO 			0x11	//表示一次识别流程结束后，没有识别结果
#define LD_ASR_ERROR	 			0x31	//	表示一次识别流程中LD3320芯片内部出现不正确的状态

#define CLK_IN   				8/* user need modify this value according to clock in */
#define LD_PLL_11				(uint8)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19			0x0f
#define LD_PLL_MP3_1B			0x18
#define LD_PLL_MP3_1D   		(uint8)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 			(uint8)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 			0x48
#define LD_PLL_ASR_1D 			0x1f

#define MIC_VOL 0x40

///用户修改函数
void  Voice_Ctrl(void);
static uint8 LD_AsrAddFixed(void);
static void Delayms(uint16 i);

///相关初始化
void LD3320_init(void);
static void LD3320_GPIO_Cfg(void);
static void LD3320_EXTI_Cfg(void);
static void LD3320_SPI_cfg(void);

///中间层
static void LD3320_delay(unsigned long uldata);
static uint8 RunASR(void);
static void LD_reset(void);
static void LD_AsrStart(void);
static uint8 LD_Check_ASRBusyFlag_b2(void);

///寄存器操作
static uint8 spi_send_byte(uint8 byte);
static void LD_WriteReg(uint8 data1,uint8 data2);
static uint8 LD_ReadReg(uint8 reg_add);
static uint8 LD_GetResult(void);
static uint8 LD_AsrRun(void);
static void ProcessInt(void);
static void LD_Init_Common(void);
static void LD_Init_ASR(void);

/********************************************自编函数分割线**********************/
static void Show_Ch_Message( u8 *message);
static void Show_En_Message( char *message);
#endif /*__LD3320_H */
