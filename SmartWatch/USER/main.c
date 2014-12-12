#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	
#include "rtc.h" 
#include "prohead.h"
#include "driver.h" 
#include "lcdlib.h"
#include "setting.h"
#include "Touch.h"
#include "UI.h"
#include "timer.h"
#include "image.h"
#include <stdio.h>
#include "LD3320.h"
#include "sim900a.h"
#include "usart2.h"
#include "remote.h"
#include "network.h"

#define BACK_LIGHT PCout(7)
#define ON 1
#define OFF 0

#define HOME 					0
#define HOME_WITH_ICON			1
#define VOICE_CTRL				2
#define SETTING 				3
#define REMOTE_CTRL				4


void MUC_Init(void);
int main(void)
{	 
	u8 keyValue = KEY_NULL;	
	u8 IconTouchVal=TOUCH_NULL;
	MUC_Init();
	while(1)
	{	
		static u8 state = HOME; 
		keyValue = KEY_Scan();		
		if(keyValue==(TWO_HIT 2))
		{
			BACK_LIGHT=!BACK_LIGHT;
		}
		if(keyValue==(LONG_PERIOD 2))
		{
			state = VOICE_CTRL;
			Load_voice_Ctrl_Pannel();
		}	
		switch(state)
		{
			case HOME:
				Display_Time();
				Display_Date();
				if(keyValue==(ONE_HIT 2))
				{
					state = HOME_WITH_ICON;
				}
				break;
			case HOME_WITH_ICON:
				Display_Time();
				if(flagShowIcons == RESET)
				{
					Show_Iocns();
					flagShowIcons=SET;
				}
				if(keyValue==(ONE_HIT 2))
				{
					UNLOAD_ICONS;
					flagShowIcons=RESET;
					state = HOME;
				}

				IconTouchVal=TOUCH_Scan(Touch_Icons);
				if(IconTouchVal!=TOUCH_NULL)
				{
					switch (IconTouchVal)
					{
						case 1 :
							if(flagIconFdback==RESET)
							{
								 Icon_Touch_fdback(110,23,42,3,gImage_voice,RED,LIGHTBLUE);
								flagIconFdback=SET;
							}
							break;
						case CLICK 1 :
							delay_ms(200);
							flagIconFdback=RESET;
							UNLOAD_ICONS;
							Load_voice_Ctrl_Pannel();
							state=VOICE_CTRL;
							break;
						case 2 :
							if(flagIconFdback==RESET)
							{
								Icon_Touch_fdback(110,90,42,3,gImage_remote,RED,LIGHTBLUE);
								flagIconFdback=SET;
							}
							break;
						case CLICK 2 :
							delay_ms(200);
							flagIconFdback=RESET;
							UNLOAD_ICONS;
							Load_Ctrl_Pannel();
							state=REMOTE_CTRL;
							break;	
						case 3 :
							if(flagIconFdback==RESET)
							{
								Icon_Touch_fdback(110,155,42,3,gImage_setting,BLACK,LIGHTBLUE);
								flagIconFdback=SET;
							}
							break;
						case CLICK 3 :
							delay_ms(200);
							flagIconFdback=RESET;
							UNLOAD_ICONS;
							UNLOAD_CLOCK;
							Load_Keyborad();
							state=SETTING;
							break;	
						default:break;
					}
				}
				break;
			case VOICE_CTRL:
				Voice_Ctrl();
				Load_BGImage();
				state=HOME_WITH_ICON;
				break;
			case SETTING:
				Ctrl_Setting_Touch_Handler();
				Load_BGImage();
				state=HOME_WITH_ICON;
				break;
			case REMOTE_CTRL:
				Ctrl_Pannel_Touch_Handler();
				Load_BGImage();
				state=HOME_WITH_ICON;
				break;	
			default:break;	
		}
	}
}

void MUC_Init()
{ 	
	delay_init();	    
	KEY_Init();         
	NVIC_Configuration(); 
	TIM3_Int_Init(9,7199);
	uart_init(9600);	
    USART2_Init(9600);	
	RTC_Init();	  
	Driver_MCU_Init();
	LCD_Init();	
	Driver_LcdBacklight(True);	 
	LCD_ClrScr(BLACK);	
	Load_BGImage();
	// Display_Time();

	TOUCH_init();	
	LD3320_init();

	
	ChooseNetwork();
	delay_ms(1000);
	UNLOAD_NETWORK_CHOOSE_BT;
	Show_Ch_Message_Centre("正在初始化网络");
	
	if(sim900a_gprs_init()!=0)
	{
		CLEAR_MESSAGE;
		Show_Ch_Message_Centre("初始化网络失败，请重新上电");
		while(1);
	}
	sim900a_set_time();
	CLEAR_MESSAGE;
	TimeShowingFlag=RESET;

}

