/*--------------------------------------------------------------------------
// 文件名：Driver.c
// 描述：  MCU底层驱动  
// 设计者：EU电子
// 深圳移优科技出品-版权所有-翻版必究
// EU-热爱嵌入式开发
// http://euse.taobao.com
//-------------------------------------------------------------------------*/
/* 头文件包含 INCLUDES */
/* type change */
typedef unsigned char       bool;
typedef unsigned char       u8;
typedef unsigned short      u16;
#define  True  1
#define  False 0

//SET BIT.    Example: a |= SETBIT0
enum
{
	SETBIT0 = 0x0001,  SETBIT1 = 0x0002,	SETBIT2 = 0x0004,	 SETBIT3 = 0x0008,
	SETBIT4 = 0x0010,	 SETBIT5 = 0x0020,	SETBIT6 = 0x0040,	 SETBIT7 = 0x0080,
	SETBIT8 = 0x0100,	 SETBIT9 = 0x0200,	SETBIT10 = 0x0400, SETBIT11 = 0x0800,
	SETBIT12 = 0x1000, SETBIT13 = 0x2000,	SETBIT14 = 0x4000, SETBIT15 = 0x8000		
};
//CLR BIT.    Example: a &= CLRBIT0
enum
{
	CLRBIT0 = 0xFFFE,  CLRBIT1 = 0xFFFD,	CLRBIT2 = 0xFFFB,	 CLRBIT3 = 0xFFF7,	
	CLRBIT4 = 0xFFEF,	 CLRBIT5 = 0xFFDF,	CLRBIT6 = 0xFFBF,	 CLRBIT7 = 0xFF7F,
	CLRBIT8 = 0xFEFF,	 CLRBIT9 = 0xFDFF,	CLRBIT10 = 0xFBFF, CLRBIT11 = 0xF7FF,
	CLRBIT12 = 0xEFFF, CLRBIT13 = 0xDFFF,	CLRBIT14 = 0xBFFF, CLRBIT15 = 0x7FFF
};
//CHOSE BIT.  Example: a = b&CHSBIT0
enum
{
	CHSBIT0 = 0x0001,  CHSBIT1 = 0x0002,	CHSBIT2 = 0x0004,	 CHSBIT3 = 0x0008,
	CHSBIT4 = 0x0010,	 CHSBIT5 = 0x0020,	CHSBIT6 = 0x0040,	 CHSBIT7 = 0x0080,
	CHSBIT8 = 0x0100,	 CHSBIT9 = 0x0200,	CHSBIT10 = 0x0400, CHSBIT11 = 0x0800,
	CHSBIT12 = 0x1000, CHSBIT13 = 0x2000,	CHSBIT14 = 0x4000, CHSBIT15 = 0x8000		
};

/* INCLUDES */
//MCU
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
//Derive
#include "Driver.h"

/*-------------------------------------------------------------------------------------------------------
*  函数声明												 
-------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------
*  函数声明												 
-------------------------------------------------------------------------------------------------------*/
//mcu
void Driver_MCU_Init(void);
//lcd
void Driver_LcdBacklight(bool sta);
void Driver_LcdReset(bool sta);
void Driver_LcdCS(bool sta);
void Driver_LcdRS(bool sta);
void Driver_LcdRD(bool sta);
void Driver_LcdWR(bool sta);
void Driver_LcdSendData(u16 Temp);

/*-------------------------------------------------------------------------------------------------------
*  执行代码													 
-------------------------------------------------------------------------------------------------------*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++  MCU ++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/********************************************************************************************************
*  Function: Driver_MCU_Init						                                                           
*  Object: MCU初始化               
*  输入： 无
*  输出： 无								                         	                                     
*  备注： MCU启动的一些配置                                      
********************************************************************************************************/
void Driver_MCU_Init(void)
{
	/*	//--------------------------- CLK INIT, HSE PLL ----------------------------
		ErrorStatus HSEStartUpStatus;
		//RCC reset
		RCC_DeInit();
		//开启外部时钟 并执行初始化
		RCC_HSEConfig(RCC_HSE_ON); 
		//等待外部时钟准备好
		HSEStartUpStatus = RCC_WaitForHSEStartUp();
		//启动失败 在这里等待
		while(HSEStartUpStatus == ERROR);
		//设置内部总线时钟
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		//外部时钟为8M 这里倍频到72M
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE); 
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while(RCC_GetSYSCLKSource() != 0x08);

		//----------------------------- CLOSE HSI ---------------------------
		//关闭内部时钟HSI
		RCC_HSICmd(DISABLE);	
*/
		//--------------------------- OPEN GPIO CLK -------------------------
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------- LCD DRIVER -----------------------------------------------------
//-------------------------------------------------------------------------------------------------------
/********************************************************************************************************
*  Function: Driver_LcdBacklight				                                                           
*  Object: lcd backlight control
*  Input: sta
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void Driver_LcdBacklight(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PD1
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_SetBits(GPIOC, GPIO_Pin_7);		else
			GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}

/********************************************************************************************************
*  Function: Driver_LcdReset				                                                           
*  Object: lcd reset control
*  Input: sta
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void Driver_LcdReset(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PC0
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_ResetBits(GPIOC, GPIO_Pin_12);
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
}

/********************************************************************************************************
*  Function: Driver_LcdCS		                                                           
*  Object: lcd enable/disable control
*  Input: sta
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void Driver_LcdCS(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PC1
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_ResetBits(GPIOC, GPIO_Pin_11);
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_11);
}

/********************************************************************************************************
*  Function: Driver_LcdRS		                                                           
*  Object: lcd RS(cmd/data chose) control
*  Input: sta
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void Driver_LcdRS(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PC2
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_ResetBits(GPIOC, GPIO_Pin_10);
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_10);
}

/********************************************************************************************************
*  Function: Driver_LcdRD		                                                           
*  Object: lcd read control
*  Input: sta
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void Driver_LcdRD(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PC0
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_ResetBits(GPIOC, GPIO_Pin_0);
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_0);
}

/********************************************************************************************************
*  Function: Driver_LcdWR		                                                           
*  Object: lcd write control
*  Input: sta
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void Driver_LcdWR(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PA12
	    	GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_ResetBits(GPIOA, GPIO_Pin_12);
		else
			GPIO_SetBits(GPIOA, GPIO_Pin_12);
}

/********************************************************************************************************
*  Function: Driver_LcdSendData		                                                           
*  Object: lcd data send
*  Input: Temp
*  Output: none                                  
*  brief: len is 16bits
********************************************************************************************************/
void Driver_LcdSendData(u16 Temp)
{
		//8位数据总线 驱动
		//----- 若为16位数据总线驱动 则将16bitTemp变量赋给对应16位IO口 -------
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PB8-PB15
			GPIO_InitTypeDef GPIO_InitStructure;
			#if (LCD_BUSTYPE == 1)	
			//16位数据总线
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
			#else 
			//8位数据总线
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
			#endif
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			StartFlag = False;
		}	
		#if (LCD_BUSTYPE == 1)	
			//16位数据总线
			GPIO_SetBits(GPIOB, Temp);
			GPIO_ResetBits(GPIOB, ~Temp);
			#else 
			//8位数据总线
			//GPIO_SetBits(GPIOB, (Temp%0x100)<<8 & 0xff00);
			//GPIO_Bits(GPIOB, (~(Temp%0x100)<<8 & 0xff00));
				GPIO_Write(GPIOB,(Temp%0x100)<<8);
		#endif		
}
void Driver_SPICS(bool sta)
{
		return; 
}

/********************************************************************************************************
*  Function: Driver_SPIMISO					                                                           
*  Object: SPI_MOSI
*  Input: none  
*  Output: 1/0 							                         	                                     
*  brief:	none                                   
********************************************************************************************************/
bool Driver_SPIMISO(void)
{
		static bool StartFlag = True;
		//PC8
		if(StartFlag)
		{	
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}
		return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
}

/********************************************************************************************************
*  Function: Driver_SPIMOSI					                                                           
*  Object: SPI_MOSI
*  Input: 1/0
*  Output: none 							                         	                                     
*  brief:	none                                   
********************************************************************************************************/
void Driver_SPIMOSI(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PC9
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_9);
}

/********************************************************************************************************
*  Function: Driver_SPISCK					                                                           
*  Object: SPI_SCK
*  Input: 1/0
*  Output: none 							                         	                                     
*  brief:	none  	                                  
********************************************************************************************************/
void Driver_SPISCK(bool sta)
{
		static bool StartFlag = True;
		if(StartFlag)
		{
			//PC6
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
			GPIO_Init(GPIOC, &GPIO_InitStructure);
			StartFlag = False;
		}	
		if(!sta)
			GPIO_ResetBits(GPIOC, GPIO_Pin_6);  
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_6);
}  

