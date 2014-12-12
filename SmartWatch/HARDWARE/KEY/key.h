#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"

#define KEY2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//读取按键1


#define KEY_WOBBLE_PERIOD      10//ms 按键消抖时间
#define KEY_2HIT_INTERVAL	 200//ms  
#define KEY_LONG_PERIOD    	500//ms	  
#define KEY_STATE_RELEASE      0   //按键释放状态
#define KEY_STATE_WOBBLE       1   //按键消抖状态 
#define KEY_STATE_1DOWN        2   //按键单击状态
#define KEY_STATE_1UP    	   3   //
#define KEY_STATE_WOBBLE2	   4
#define KEY_STATE_2DOWN		   5  //
#define KEY_STATE_2UP	   	   6   //
#define KEY_STATE_LONG		   7	//按键长按状态
#define KEY_STATE_LONG_HOLD	   8	//按键长按状态

#define KEY_LONG               0x80//长按标志
#define KEY_LONG_HOLD          0x40//长按后等待标志
#define KEY_1HIT			   0X20//单击标志  
#define KEY_2HIT			   0x10//双击标志
#define KEY_NULL 0xff		   
#define PRESS_DOWN 1 			//按键按下标志
#define PRESS_UP   0 			//按键释放标志
#define LONG_PERIOD 		 KEY_LONG | 
#define ONE_HIT				 KEY_1HIT |
#define TWO_HIT				 KEY_2HIT |
void KEY_Init(void);	//IO初始化
u8 KEY_Scan(void);  	//按键扫描函数					    
u8 getKeyValue(void);	//按键扫描,IO层
#endif
