#ifndef __Touch_H__
#define __Touch_H__
#include "stm32f10x.h"

#define PEN  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)

#define TOUCH_WOBBLE_PERIOD      25//ms
#define TOUCH_LONG_PERIOD    	1000//ms	  
#define TOUCH_STATE_RELEASE      0   //按键释放状态
#define TOUCH_STATE_WOBBLE       1   //按键消抖状态 
#define TOUCH_STATE_PRESS        2   //按键单击状态
#define TOUCH_STATE_PRESS_HOLD   3 	//按键等待释放状态
#define TOUCH_STATE_LONG		   4	//按键长按状态
#define TOUCH_STATE_LONG_HOLD	   5	//按键长按状态

#define TOUCH_CLICK        	 0x20//长按后等待标志
#define TOUCH_PERESS			   0X10//单击标志  
#define TOUCH_NULL 0xff		   
#define CLICK 				 TOUCH_CLICK | 
#define PERESS				 TOUCH_PERESS |


typedef struct{
	u16 x;
	u16 y;	
}TOUCH_VAL_TYPE;//触摸检测值 数据类型
u8 TOUCH_Scan(u8 (*fun)(void));
int Touch_GetSite(u16 *pRow, u16 *pColumn);
void TOUCH_init(void);
u8 Touch_Icons(void);
u8 Touch_Ctrl_Pannel(void);
void Ctrl_Pannel_Touch_Handler(void);
#endif  
  

