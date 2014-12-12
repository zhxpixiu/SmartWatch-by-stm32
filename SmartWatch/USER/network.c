#include "delay.h"
#include "sys.h"
#include "prohead.h"
#include "driver.h" 
#include "lcdlib.h"
#include "Touch.h"
#include "UI.h"
#include <stdio.h>
#include "sim900a.h"
#include "network.h"
void NetworkChooseBtTouchFdback(bool bt)
{
	if(bt==ETHERNET)
	{
		LCD_DisAWideBorder(60+5,20+5,60+57-5,20+71-5,3,WHITE);
	}
	if(bt==GPRS)
	{
		LCD_DisAWideBorder(60+5,130+5,60+57-5,130+71-5,3,WHITE);
	}
	
}
void NetworkChooseBtTouchFdbackRelease(bool bt)
{
	if(bt==ETHERNET)
	{
		LCD_DisARectangular(60+5,20+5,60+57-5,20+71-5,WHITE);
		LCD_DisGB2312String16x16(80,32,"以太网",LIGHTBLUE,WHITE);
	}
	if(bt==GPRS)
	{
		LCD_DisARectangular(60+5,130+5,60+57-5,130+71-5,WHITE);
		LCD_DisASCString(80,149,"GPRS",LIGHTBLUE,1,KEEP_BG);
	}
	
}
void LoadNetworkChooseBt(void)
{
	LCD_DisARectangular(60+5,20+5,60+57-5,20+71-5,LIGHTBLUE);
	LCD_DisGB2312String16x16(80,32,"以太网",WHITE,LIGHTBLUE);
	LCD_DisARectangular(60+5,130+5,60+57-5,130+71-5,LIGHTBLUE);
	LCD_DisASCString(80,149,"GPRS",WHITE,1,KEEP_BG);
}
u8 TouchNetworkChooseBt(void)
{
	u16 x,y;
	Touch_GetSite(&x,&y);
	if(x>(60+5) && y>(20+5) && x<(60+57-5) && y<(20+71-5))
	{
		return ETHERNET;
	}
	if(x>(60+5) && y>(130+5) && x<(60+57-5) && y<(130+71-5))
	{
		return GPRS;
	}
	return TOUCH_NULL;
}

void ChooseNetwork(void)
{
	u8 TouchValTemp;
	LoadNetworkChooseBt();
	while(1)
	{
		TouchValTemp=TOUCH_Scan(TouchNetworkChooseBt);
		if(TouchValTemp!=TOUCH_NULL)
		{
			if(TouchValTemp<2)//小于2说明处在长按状态
			{
				NetworkChooseBtTouchFdback(TouchValTemp);
			}
			else
			{	
				networkOption=TouchValTemp & 0x0f;
				NetworkChooseBtTouchFdbackRelease(networkOption);
				return;
			}
		}
	}
}

