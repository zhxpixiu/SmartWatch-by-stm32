/*--------------------------------------------------------------------------
// 文件名：UI.c
// 描述：  屏幕显示函数
// 设计者：zhx
// 日期：2014.8.1
//-------------------------------------------------------------------------*/
/* 头文件包含 INCLUDES */
#include "stm32f10x.h"
#include "UI.h"
#include "prohead.h"
#include "driver.h" 
#include "lcdlib.h"
#include "Touch.h"
#include "rtc.h" 
#include "image.h"
#include "sim900a.h"
#include "delay.h"
/*-------------------------------------------------------------------------------------------------------
*  内部声明								
-------------------------------------------------------------------------------------------------------*/


#define TIME_DIS_POS_X		20
#define TIME_DIS_POS_Y		14

#define DATE_DIS_POS_X		TIME_DIS_POS_X + 48 + 5 + 45
#define DATE_DIS_POS_Y		TIME_DIS_POS_X + 24

#define WEEK_DIS_POS_X		DATE_DIS_POS_X

#define WEEK_DIS_POS_Y		140


#define DATE_FONT_WIDTH 	8



#define TIME_SETTING 0
#define DATE_SETTING 1

#define ON 1
#define OFF 1
/*-------------------------------------------------------------------------------------------------------
*  资源定义											 
-------------------------------------------------------------------------------------------------------*/
u8  *week[]={"星期天","星期一","星期二","星期三","星期四","星期五","星期六"};
bool TimeShowingFlag=RESET;
bool DateShowingFlag=RESET;
bool flagShowIcons = RESET;
bool flagIconFdback = RESET;
/*-------------------------------------------------------------------------------------------------------
*  函数声明												 
-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------
*  执行代码													 
-------------------------------------------------------------------------------------------------------*/


void Load_BGImage()
{
	LCD_DisAPhoto(0,0,176,220,(u8*) gImage_bg);
	TimeShowingFlag=RESET;
	flagShowIcons=RESET;
	DateShowingFlag=RESET;
}

void Show_Iocns(void)
{
	if(DateShowingFlag==SET)
	{
		LCD_ClrARectangular(DATE_DIS_POS_X,DATE_DIS_POS_Y,DATE_DIS_POS_X+16,DATE_DIS_POS_Y+16*8);
		LCD_ClrARectangular(WEEK_DIS_POS_X,WEEK_DIS_POS_Y,WEEK_DIS_POS_X+16,WEEK_DIS_POS_Y+16*3);
		// delay_ms(1000);
	}
	LCD_DisAIcon(110,155,42,42,gImage_setting,BLACK);
	LCD_DisAIcon(110,90,42,42,gImage_remote,RED);
	LCD_DisAIcon(110,23,42,42,gImage_voice,RED);
	DateShowingFlag=RESET;

}

void Display_Time(void)
{	
	static u8 secOld=100;
	static u8 minOld=100;
	static u8 hourOld=100;
	char temp[]="  ";
	char time[]="        ";
	if(TimeShowingFlag==RESET)
	{
		sprintf(time,"%02u:%02u:%02u",calendar.hour,calendar.min,calendar.sec);
		LCD_DisASCString(TIME_DIS_POS_X, TIME_DIS_POS_Y,time, WHITE, 3,RE_WRITE_BG);
		TimeShowingFlag=SET;
	}
	if(calendar.sec!=secOld)
	{	
		secOld=calendar.sec;
		sprintf(temp,"%02u",calendar.sec);
		LCD_DisASCString(TIME_DIS_POS_X, TIME_DIS_POS_Y+24*6,temp, WHITE, 3,RE_WRITE_BG);
		if(calendar.min!=minOld)
		{
			minOld=calendar.min;
			sprintf(temp,"%02u",calendar.min);
			LCD_DisASCString(TIME_DIS_POS_X, TIME_DIS_POS_Y+24*3,temp, WHITE, 3,RE_WRITE_BG);
			if(calendar.hour!=hourOld)
			{
				hourOld=calendar.hour;
				sprintf(temp,"%02u",calendar.hour);
				LCD_DisASCString(TIME_DIS_POS_X, TIME_DIS_POS_Y,temp, WHITE, 3,RE_WRITE_BG);
			}
		}
	}
}

void Display_Date(void)
{	
	static u8 dateOld=100;
	u8 pos_y;//起始坐标
	char temp1[]="  ";
	
	if(DateShowingFlag==RESET || calendar.w_date!=dateOld)
	{
		pos_y=DATE_DIS_POS_Y;
		dateOld=calendar.w_date;
		sprintf(temp1,"%-u",calendar.w_month);
		LCD_DisASCString(DATE_DIS_POS_X, pos_y,temp1, WHITE, 1,RE_WRITE_BG);
		if(calendar.w_month>9)//月份两位数
		{
			pos_y += 2 *DATE_FONT_WIDTH;
		}
		else
		{
			pos_y += DATE_FONT_WIDTH;
		}
		LCD_DisGB2312String(DATE_DIS_POS_X, pos_y,"月", WHITE,1);
		pos_y += 2 *DATE_FONT_WIDTH;
		sprintf(temp1,"%-u",calendar.w_date);
		LCD_DisASCString(DATE_DIS_POS_X, pos_y,temp1, WHITE, 1,RE_WRITE_BG);
		if(calendar.w_date>9)//两位数
		{
			pos_y += 2 *DATE_FONT_WIDTH;
		}
		else
		{
			pos_y +=  DATE_FONT_WIDTH;
		}
		LCD_DisGB2312String(DATE_DIS_POS_X, pos_y,"日", WHITE,1);
		LCD_DisGB2312String(WEEK_DIS_POS_X, WEEK_DIS_POS_Y,week[calendar.week], WHITE,1);
		DateShowingFlag=SET;
	}
}

void Icon_Touch_fdback(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 removeColor,u16 bgColor)
{
		u16 temp;
		unsigned int i,lenth;
		lenth = height * height * 2;  	
		LCD_DisASquare(x0-width,y0-width,height+2*width,bgColor);	
		LCD_OpenWin(x0, y0, x0+height-1, y0+height-1);
		for(i = 0; i < lenth; i+=2)
		{
			temp=(*(pData+i))*0x100 + (*(pData+i+1));
			if(temp!=removeColor)
			{
				LCD_WriteData(temp);
			}
				else
			{
				LCD_WriteData(bgColor);
			}
		}
	
}
void Load_voice_Ctrl_Pannel(void)
{
	Load_BGImage();
	LCD_DisAIcon(120,100,42,20,gImage_voice2,WHITE);
	TimeShowingFlag=RESET;
	flagShowIcons=RESET;
	DateShowingFlag=RESET;
}
