#ifndef __UI_H_
#define __UI_H_
#include "prohead.h"


#define UNLOAD_ICONS	LCD_ClrARectangular(107,20,155,200);\
						flagShowIcons=RESET;
#define UNLOAD_CLOCK	LCD_ClrARectangular(20,14,20+48,14+24*8);\
						TimeShowingFlag=RESET;
						

void Load_BGImage(void);
void Show_Iocns(void);
void Display_Time(void);
void Display_Date(void);
void Icon_Touch_fdback(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 removeColor,u16 bgColor);
void Load_voice_Ctrl_Pannel(void);
void Dis_Bt_Time_Or_Date(bool option);



extern bool TimeShowingFlag;
extern bool DateShowingFlag;
extern bool flagShowIcons;
extern bool flagIconFdback;

#endif
 


