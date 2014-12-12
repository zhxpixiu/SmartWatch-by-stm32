#include "delay.h"  
#include "sys.h"
#include "usart.h"
#include "stm32f10x_gpio.h"  
#include "timer.h"
#include "UI.h"
#include "sim900a.h"
#include "stm32f10x.h"
#include "prohead.h"
#include "driver.h" 
#include "lcdlib.h"
#include "Touch.h"
#include "image.h"
#include "remote.h"
#include "key.h"
#include <stdio.h>

#define BLOCK_ON_COLOR			WHITE
#define BLOCK_OFF_COLOR			LIGHTBLUE
#define BLOCK_FRESH_COLOR			GRAY

#define BORDER_ON_COLOR			LIGHTBLUE
#define BORDER_OFF_COLOR		WHITE

#define FONT_COLOR_ON			LIGHTBLUE
#define FONT_COLOR_OFF			WHITE

#define AIR_CONDITIONER			0
#define LIVING_ROOM_LIGHT 		1
#define BEDROOM_LIGHT			2
#define DOOR 					3
#define WINDOW					5

#define SUCCEED 0
#define FAILED  1
#define ON 1
#define OFF 0

#define SHIFT_TIME 1000

extern u8 TXdatabuf[32];     
extern u8 RXdatabuf[32]; 
bool block_state[9]={OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF};

char temperature[]="  ";
char humidity[]="  ";

void Fill_Block_Color(u8 order,u16 color)
{
	u8 i,j;
	i=order/3;
	j=order%3;
	LCD_DisARectangular(5+i*57,5+j*71,(i+1)*57,(j+1)*71,color);
	
}
void Ctrl_Pannel_Touch_FdBack(u8 order,u16 color)
{
	u8 i,j;
	i=order/3;
	j=order%3;
	LCD_DisAWideBorder(5+i*57,5+j*71,(i+1)*57,(j+1)*71,3,color);
}
void Ctrl_Pannel_Touch_Release_FdBack(u8 order,u16 color)
{
	u8 i,j;
	i=order/3;
	j=order%3;
	LCD_DisAWideBorder(5+i*57,5+j*71,(i+1)*57,(j+1)*71,3,color);
}
void Reset_Txdatabuf(void)
{
	u8 i=0;
	for(i=0;i<=6;i++)
	{
		TXdatabuf[i]=2;//2为无用控制信息，给接收端作判断用
	}
}

void refresh(void)
{
	u16 temp;

	if(sim900a_get_data(RXdatabuf)==SUCCEED)
	{
		if(RXdatabuf[2]!=2)block_state[AIR_CONDITIONER]=RXdatabuf[2];
		if(RXdatabuf[3]!=2)block_state[LIVING_ROOM_LIGHT]=RXdatabuf[3];
		if(RXdatabuf[4]!=2)block_state[BEDROOM_LIGHT]=RXdatabuf[4];
		if(RXdatabuf[5]!=2)block_state[DOOR]=RXdatabuf[5];
		if(RXdatabuf[6]!=2)block_state[WINDOW]=RXdatabuf[6];

		//重新显示更新的面板
		if(block_state[AIR_CONDITIONER]==OFF)
		{
			Fill_Block_Color(0,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22,22,"空调",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(0,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22,22,"空调",LIGHTBLUE,BLOCK_ON_COLOR);
		}

		if(block_state[LIVING_ROOM_LIGHT]==OFF)
		{
			Fill_Block_Color(1,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(1,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",LIGHTBLUE,BLOCK_ON_COLOR);
		}
		
		if(block_state[BEDROOM_LIGHT]==OFF)
		{
			Fill_Block_Color(2,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8+71,"卧室灯",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(2,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8+71,"卧室灯",LIGHTBLUE,BLOCK_ON_COLOR);
		}
		
		if(block_state[DOOR]==OFF)
		{
			Fill_Block_Color(3,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22+57,30,"门",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(3,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22+57,30,"门",LIGHTBLUE,BLOCK_ON_COLOR);
		}
		if(block_state[WINDOW]==OFF)
		{
			Fill_Block_Color(5,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22+57,30+142,"窗",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(5,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22+57,30+142,"窗",LIGHTBLUE,BLOCK_ON_COLOR);
		}

		//此处显示温湿度更新
		if(RXdatabuf[6]!=2)
		{
			temp=(u16)RXdatabuf[7];
			temp <<=8;
			temp = (u16)(temp + RXdatabuf[8])/10;
			sprintf(temperature,"%02u",temp);

			temp=(u16)RXdatabuf[9];
			temp <<=8;
			temp = (u16)(temp + RXdatabuf[10])/10;
			sprintf(humidity,"%02u",temp);

			LCD_DisARectangular(145,45,145+16,70,LIGHTBLUE);
			LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
		
			LCD_DisARectangular(145,45+71,145+16,70+71,LIGHTBLUE);
			LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
		}

		//
		LCD_DisGB2312String16x16(22+57,22+71-16,"更新完成",WHITE,GRAY);
		delay_ms(800);
		Fill_Block_Color(4,LIGHTBLUE);
		LCD_DisGB2312String16x16(22+57,22+71,"更新",WHITE,LIGHTBLUE);

	}
	else
	{
		LCD_DisGB2312String16x16(22+57,22+71-16,"更新失败",WHITE,GRAY);
		delay_ms(1500);
		Fill_Block_Color(4,LIGHTBLUE);
		LCD_DisGB2312String16x16(22+57,22+71,"更新",WHITE,LIGHTBLUE);
	}
}

void Load_Ctrl_Pannel(void)
{
	LCD_WriteReg(0X03);
	#if (LCD_DIRECT == 1)	
		LCD_WriteData(0X1008);		
	#else
		LCD_WriteData(0X1030);      //vertical
	#endif
	LCD_ClrScr(LIGHTBLUE);
	LCD_WriteReg(0X03);
	#if (LCD_DIRECT == 1)	
		LCD_WriteData(0X1038);		//horizontal
	#else
		LCD_WriteData(0X1030);      //vertical
	#endif

	TimeShowingFlag=RESET;
	flagShowIcons=RESET;
	DateShowingFlag=RESET;

	LCD_DisAWideBorder(0,0,175,219,5,BLACK);
	LCD_DisLines(57,0,1,220,4,BLACK);
	LCD_DisLines(114,0,1,220,4,BLACK);
	LCD_DisLines(0,71,2,176,4,BLACK);
	LCD_DisLines(0,142,2,176,4,BLACK);
	LCD_DisLines(0,213,2,176,2,BLACK);


	//载入面板，各方块的状态维持最近一次更新的状态
	if(block_state[AIR_CONDITIONER]==OFF)
	{
		LCD_DisGB2312String16x16(22,22,"空调",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(0,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22,22,"空调",LIGHTBLUE,BLOCK_ON_COLOR);
	}

	if(block_state[LIVING_ROOM_LIGHT]==OFF)
	{
		LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(1,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	
	if(block_state[BEDROOM_LIGHT]==OFF)
	{
		LCD_DisGB2312String16x16(22,22+71-8+71,"卧室灯",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(2,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22,22+71-8+71,"卧室灯",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	
	if(block_state[DOOR]==OFF)
	{
		LCD_DisGB2312String16x16(22+57,30,"门",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(3,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22+57,30,"门",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	if(block_state[WINDOW]==OFF)
	{
		LCD_DisGB2312String16x16(22+57,30+142,"窗",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(5,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22+57,30+142,"窗",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	Fill_Block_Color(4,GRAY);	
	LCD_DisGB2312String16x16(22+57,22+71-16,"正在更新",WHITE,GRAY);

	LCD_DisGB2312String16x16(10+57+57,10,"温度",WHITE,LIGHTBLUE);
	
	LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
	
	LCD_DisGB2312String16x16(10+57+57,10+71,"湿度",WHITE,LIGHTBLUE);
	
	LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);

	LCD_DisGB2312String16x16(22+57+57,24+142,"返回",WHITE,LIGHTBLUE);

 	//更新设备状态
	refresh();

}

void Ctrl_Pannel_Touch_Handler(void)
{
	u8 TouchValTemp;
	u8 keyValue;
	u8 block;
	u16 temp;
	u8 i;
	while(1)
	{
		keyValue=KEY_Scan();
		if(keyValue==(ONE_HIT 2))
		{
			return;
		}

		TouchValTemp=TOUCH_Scan(Touch_Ctrl_Pannel);//输出值为0－8或上相应的标志
		if(TouchValTemp!=TOUCH_NULL)
		{
			if(TouchValTemp<9)
			{
				
				if(block_state[TouchValTemp]==OFF)//根据区块当前状态
				{
					Ctrl_Pannel_Touch_FdBack(TouchValTemp,WHITE);
				}
				else
				{
					Ctrl_Pannel_Touch_FdBack(TouchValTemp,LIGHTBLUE);
				}
			}
			else
			{
				block = TouchValTemp & 0x0f;//block 取值为0~8
				if(block<8)
				{
					Fill_Block_Color(block,BLOCK_FRESH_COLOR);
				}
				switch(block)
				{

					case 0:
						Reset_Txdatabuf();

						if(block_state[AIR_CONDITIONER]==ON)
						{
							TXdatabuf[2]=OFF;
							LCD_DisGB2312String16x16(22,22-16,"正在关闭",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[2]=ON;
							LCD_DisGB2312String16x16(22,22-16,"正在打开",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//发送成功
						{
							if(RXdatabuf[0]==SUCCEED)//返回信息成功
							{
								if(TXdatabuf[2]==RXdatabuf[2])//发送控制信息与对应返回状态信息一致
								{
									if(block_state[AIR_CONDITIONER]==ON)
									{
										block_state[AIR_CONDITIONER]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[AIR_CONDITIONER]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//操作失败
								{
									LCD_DisGB2312String16x16(22,22-16,"操作失败",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[AIR_CONDITIONER]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//连接失败
							{
								LCD_DisGB2312String16x16(22,22-16,"连接失败",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[AIR_CONDITIONER]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//发送失败
						{
							LCD_DisGB2312String16x16(22,22-16,"发送失败",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[AIR_CONDITIONER]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22,22,"空调",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;

					case 1:
						Reset_Txdatabuf();

						if(block_state[LIVING_ROOM_LIGHT]==ON) 
						{
							TXdatabuf[3]=OFF;
							LCD_DisGB2312String16x16(22,22-16+71,"正在关闭",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[3]=ON;
							LCD_DisGB2312String16x16(22,22-16+71,"正在打开",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//发送成功
						{
							if(RXdatabuf[0]==SUCCEED)//返回信息成功
							{
								if(TXdatabuf[3]==RXdatabuf[3])//发送控制信息与对应返回状态信息一致
								{
									if(block_state[LIVING_ROOM_LIGHT]==ON)
									{
										block_state[LIVING_ROOM_LIGHT]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[LIVING_ROOM_LIGHT]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//操作失败
								{
									LCD_DisGB2312String16x16(22,22-16+71,"操作失败",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[LIVING_ROOM_LIGHT]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//连接失败
							{
								LCD_DisGB2312String16x16(22,22-16+71,"连接失败",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[LIVING_ROOM_LIGHT]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//发送失败
						{
							LCD_DisGB2312String16x16(22,22-16+71,"发送失败",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[LIVING_ROOM_LIGHT]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;

					case 2:
						Reset_Txdatabuf();

						if(block_state[BEDROOM_LIGHT]==ON) 
						{
							TXdatabuf[4]=OFF;
							LCD_DisGB2312String16x16(22,22-16+142,"正在关闭",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[4]=ON;
							LCD_DisGB2312String16x16(22,22-16+142,"正在打开",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//发送成功
						{
							if(RXdatabuf[0]==SUCCEED)//返回信息成功
							{
								if(TXdatabuf[4]==RXdatabuf[4])//发送控制信息与对应返回状态信息一致
								{
									if(block_state[BEDROOM_LIGHT]==ON)
									{
										block_state[BEDROOM_LIGHT]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[BEDROOM_LIGHT]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//操作失败
								{
									LCD_DisGB2312String16x16(22,22-16+142,"操作失败",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[BEDROOM_LIGHT]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//连接失败
							{
								LCD_DisGB2312String16x16(22,22-16+142,"连接失败",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[BEDROOM_LIGHT]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//发送失败
						{
							LCD_DisGB2312String16x16(22,22-16+142,"发送失败",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[BEDROOM_LIGHT]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22,22+142-8,"卧室灯",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;
					case 3:
												Reset_Txdatabuf();

						if(block_state[DOOR]==ON)
						{
							TXdatabuf[5]=OFF;
							LCD_DisGB2312String16x16(22+57,22-16,"正在关闭",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[5]=ON;
							LCD_DisGB2312String16x16(22+57,22-16,"正在打开",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//发送成功
						{
							if(RXdatabuf[0]==SUCCEED)//返回信息成功
							{
								if(TXdatabuf[5]==RXdatabuf[5])//发送控制信息与对应返回状态信息一致
								{
									if(block_state[DOOR]==ON)
									{
										block_state[DOOR]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[DOOR]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//操作失败
								{
									LCD_DisGB2312String16x16(22+57,22-16,"操作失败",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[DOOR]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//连接失败
							{
								LCD_DisGB2312String16x16(22+57,22-16,"连接失败",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[DOOR]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//发送失败
						{
							LCD_DisGB2312String16x16(22+57,22-16,"发送失败",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[DOOR]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22+57,30,"门",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;
					case 4:
						LCD_DisGB2312String16x16(22+57,22+71-16,"正在更新",WHITE,BLOCK_FRESH_COLOR);	
						refresh();
						 break;	 
					case 5:
						Reset_Txdatabuf();

						if(block_state[WINDOW]==ON)
						{
							TXdatabuf[6]=OFF;
							LCD_DisGB2312String16x16(22+57,22-16+142,"正在关闭",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[6]=ON;
							LCD_DisGB2312String16x16(22+57,22-16+142,"正在打开",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//发送成功
						{
							if(RXdatabuf[0]==SUCCEED)//返回信息成功
							{
								if(TXdatabuf[6]==RXdatabuf[6])//发送控制信息与对应返回状态信息一致
								{
									if(block_state[WINDOW]==ON)
									{
										block_state[WINDOW]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[WINDOW]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//操作失败
								{
									LCD_DisGB2312String16x16(22+57,22-16+142,"操作失败",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[WINDOW]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//连接失败
							{
								LCD_DisGB2312String16x16(22+57,22-16+142,"连接失败",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[WINDOW]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//发送失败
						{
							LCD_DisGB2312String16x16(22+57,22-16+142,"发送失败",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[WINDOW]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22+57,30+142,"窗",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;
					case 6:
						LCD_DisGB2312String16x16(10+57+57,10,"温度",WHITE,BLOCK_FRESH_COLOR);
						LCD_DisGB2312String16x16(150,14+10,"更新中",WHITE,BLOCK_FRESH_COLOR);
						if(sim900a_get_data(RXdatabuf)==SUCCEED)
						{
							if(RXdatabuf[0]==SUCCEED)
							{
								if(RXdatabuf[6]!=2)
								{
									LCD_DisGB2312String16x16(150,10,"更新成功",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									temp=(u16)RXdatabuf[7];
									temp <<=8;
									temp = (u16)(temp + RXdatabuf[8])/10;
									sprintf(temperature,"%02u",temp);
									Fill_Block_Color(6,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10,"温度",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
								}
								else//更新失败
								{
									LCD_DisARectangular(150,10,150+16,70,BLOCK_FRESH_COLOR);
									LCD_DisGB2312String16x16(150,10,"更新失败",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									Fill_Block_Color(6,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10,"温度",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
								}
							}
							else//连接失败
							{
								LCD_DisARectangular(150,10,150+16,70,BLOCK_FRESH_COLOR);
								LCD_DisGB2312String16x16(150,10,"连接失败",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								Fill_Block_Color(6,LIGHTBLUE);
								LCD_DisGB2312String16x16(10+57+57,10,"温度",WHITE,LIGHTBLUE);
								LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
							}
						}
						else//发送失败
						{
							LCD_DisARectangular(150,10,150+16,70,BLOCK_FRESH_COLOR);
							LCD_DisGB2312String16x16(150,10,"发送失败",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							Fill_Block_Color(6,LIGHTBLUE);
							LCD_DisGB2312String16x16(10+57+57,10,"温度",WHITE,LIGHTBLUE);
							LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
						}

						 break;
					case 7:
						LCD_DisGB2312String16x16(10+57+57,10+71,"湿度",WHITE,BLOCK_FRESH_COLOR);
						LCD_DisGB2312String16x16(150,14+10+71,"更新中",WHITE,BLOCK_FRESH_COLOR);
						if(sim900a_get_data(RXdatabuf)==SUCCEED)
						{
							if(RXdatabuf[0]==SUCCEED)
							{
								if(RXdatabuf[6]!=2)
								{
									LCD_DisGB2312String16x16(150,10+71,"更新成功",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									temp=(u16)RXdatabuf[9];
									temp <<=8;
									temp = (u16)(temp + RXdatabuf[10])/10;
									sprintf(humidity,"%02u",temp);
									Fill_Block_Color(7,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10+71,"湿度",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
								}
								else//更新失败
								{
									LCD_DisARectangular(150,10+71,150+16,70+71,BLOCK_FRESH_COLOR);
									LCD_DisGB2312String16x16(150,10+71,"更新失败",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									Fill_Block_Color(7,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10+71,"湿度",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
								}
							}
							else//连接失败
							{
								LCD_DisARectangular(150,10+71,150+16,70+71,BLOCK_FRESH_COLOR);
								LCD_DisGB2312String16x16(150,10+71,"连接失败",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								Fill_Block_Color(7,LIGHTBLUE);
								LCD_DisGB2312String16x16(10+57+57,10+71,"湿度",WHITE,LIGHTBLUE);
								LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
							}
						}
						else//发送失败
						{
							LCD_DisARectangular(150,10+71,150+16,70+71,BLOCK_FRESH_COLOR);
							LCD_DisGB2312String16x16(150,10+71,"发送失败",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							Fill_Block_Color(7,LIGHTBLUE);
							LCD_DisGB2312String16x16(10+57+57,10+71,"湿度",WHITE,LIGHTBLUE);
							LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
						}
						 break;	 
					case 8:
						Fill_Block_Color(8,BLOCK_FRESH_COLOR);
						LCD_DisGB2312String16x16(22+57+57,24+142,"返回",WHITE,BLOCK_FRESH_COLOR);	
						return;
				/*		LCD_WriteReg(0X03);
						#if (LCD_DIRECT == 1)	
							LCD_WriteData(0X1030);		
						#else
							LCD_WriteData(0X1030);      //vertical
						#endif
						// LCD_ClrScr(LIGHTBLUE);
						for(i=0;i<9;i++)
						{
							Fill_Block_Color(i,LIGHTBLUE);
						}	
						LCD_WriteReg(0X03);
						#if (LCD_DIRECT == 1)	
							LCD_WriteData(0X1038);		//horizontal
						#else
							LCD_WriteData(0X1030);      //vertical
						#endif

								TimeShowingFlag=RESET;
								flagShowIcons=RESET;
								DateShowingFlag=RESET;
						

								LCD_DisGB2312String16x16(22+57+57,24+142,"返回",WHITE,LIGHTBLUE);
								while(1)
								{
									TouchValTemp=TOUCH_Scan(Touch_Ctrl_Pannel);//输出值为0－8或上相应的标志
									if(TouchValTemp!=TOUCH_NULL)
									{
										if(TouchValTemp<9)
										{
											Ctrl_Pannel_Touch_FdBack(TouchValTemp,WHITE);
										}
										else
										{
											block = TouchValTemp & 0x0f;//block 取值为0~8
											Ctrl_Pannel_Touch_Release_FdBack(block,LIGHTBLUE);
											if(block==8)
											{
												break;
											}
										}
									}
								}
								LCD_WriteReg(0X03);
								#if (LCD_DIRECT == 1)	
									LCD_WriteData(0X1008);		
								#else
									LCD_WriteData(0X1030);      //vertical
								#endif
								// LCD_ClrScr(LIGHTBLUE);
								for(i=0;i<9;i++)
								{
									Fill_Block_Color(i,LIGHTBLUE);
								}	
								LCD_WriteReg(0X03);
								#if (LCD_DIRECT == 1)	
									LCD_WriteData(0X1038);		//horizontal
								#else
									LCD_WriteData(0X1030);      //vertical
								#endif

								TimeShowingFlag=RESET;
								flagShowIcons=RESET;
								DateShowingFlag=RESET;


								//载入面板，各方块的状态维持最近一次更新的状态
								if(block_state[AIR_CONDITIONER]==OFF)
								{
									LCD_DisGB2312String16x16(22,22,"空调",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(0,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22,"空调",LIGHTBLUE,BLOCK_ON_COLOR);
								}

								if(block_state[LIVING_ROOM_LIGHT]==OFF)
								{
									LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(1,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8,"客厅灯",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								
								if(block_state[BEDROOM_LIGHT]==OFF)
								{
									LCD_DisGB2312String16x16(22,22+71-8+71,"卧室灯",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(2,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8+71,"卧室灯",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								
								if(block_state[DOOR]==OFF)
								{
									LCD_DisGB2312String16x16(22+57,30,"门",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(3,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30,"门",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								if(block_state[WINDOW]==OFF)
								{
									LCD_DisGB2312String16x16(22+57,30+142,"窗",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(5,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30+142,"窗",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								LCD_DisGB2312String16x16(22+57,22+71,"更新",WHITE,LIGHTBLUE);

								LCD_DisGB2312String16x16(10+57+57,10,"温度",WHITE,LIGHTBLUE);
								
								LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
								
								LCD_DisGB2312String16x16(10+57+57,10+71,"湿度",WHITE,LIGHTBLUE);
								
								LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);

								LCD_DisGB2312String16x16(22+57+57,24+142,"返回",WHITE,LIGHTBLUE);
								break;*/		
					default:break;
				}
			}
		}
	}
}





