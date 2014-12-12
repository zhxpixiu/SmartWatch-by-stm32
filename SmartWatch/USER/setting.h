#ifndef __TIME_SET_H_
#define __TIME_SET_H_
#include "stm32f10x.h"


#define UNLOAD_KEY_BOARD LCD_ClrARectangular(75,0,175,219);\
						 LCD_ClrARectangular(22,0,55,40);\
						 flagShowIcons=RESET;

void Load_Keyborad(void);
void  Bt_Feedback(u8 area);
void Bt_Feedback_Cancel(u8 area);	
void Ctrl_Setting_Touch_Handler(void);
extern char *keyVal[];
					
#endif






