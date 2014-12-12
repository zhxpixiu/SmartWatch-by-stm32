#include "key.h"
#include "sys.h" 
#include "timer.h"
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化KEY0-->GPIOA.13,KEY1-->GPIOA.15  上拉输入
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;//PA0~1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA0,1
}

u8 getKeyValue(void)
{	 
	if(KEY2==0)
	{
	//	if(KEY1==0)return 1;
	    if(KEY2==0)return 2;
	}  
 	return KEY_NULL;// 无按键按下
}
/********************************************************************************************************
*  Function: KEY_Scan				                                                           
*  Object: get the key
*  Input: index and data
*  Output: the value of the key :KEY_NULL ,keyVal(quick hit), LONG_PERIOD keyVal(holding)                             
*  brief: none
********************************************************************************************************/
u8 KEY_Scan(void)
{
	static u8 keyState=KEY_STATE_RELEASE;//定义状态机
	static u8 lastKeyValue;//最终键值
	u8 keyTemp;//键值暂态量
	keyTemp=getKeyValue();//获取全键盘扫描后的值
	switch(keyState)
	{			
		case KEY_STATE_RELEASE: //按键释放状态
		{
			if(keyTemp!=KEY_NULL)//有按键被按下
			{
				keyState=KEY_STATE_WOBBLE;//进入消抖状态
				KeyCountMs=0;
			}
			else lastKeyValue=KEY_NULL;//无按键按下则返回空值
		}
		break;
		case KEY_STATE_WOBBLE://按键消抖状态
		{
			if(keyTemp!=KEY_NULL)//有按键被按下
			{
				if((KeyCountMs)>=KEY_WOBBLE_PERIOD)//若从第一次按键被按下经过的时间大于按键抖动区间
				{
					keyState=KEY_STATE_1DOWN;//按键确实被按下
					lastKeyValue=keyTemp;//记录键值
				}	
			}
			else keyState=KEY_STATE_RELEASE;//否则回到按键释放状态
		}
		break;
		case KEY_STATE_1DOWN://按键按下状态
		{
			if(keyTemp!=KEY_NULL)//有按键被按下
			{
				if((KeyCountMs)>=KEY_LONG_PERIOD)//若从第一次按键被按下经过的时间大于定义的长按时间
				{                           
					keyState=KEY_STATE_LONG;//按键确实被按下,进入长按状态
					lastKeyValue=keyTemp|KEY_LONG;//记录键值
				}	
			}
			else 
			{
				keyState=KEY_STATE_1UP;//按键松开
				KeyCountMs=0;//定时器清零,进行新的计数
			}
		}
		break;
		case KEY_STATE_1UP://第一按下后松开
		{
			if(KeyCountMs<KEY_2HIT_INTERVAL)
			{
				if(KeyCountMs>50)//防止第一次按键松开时的抖动被误判为双击,待 XXms后再判断
				{
					if(keyTemp!=KEY_NULL)//有按键被按下
					{
						keyState=KEY_STATE_WOBBLE2;
						KeyCountMs=0;//定时器清零,进行新的计数
					}
				}
			}	
			else
			{
				lastKeyValue|=KEY_1HIT;//记录键值
				keyState=KEY_STATE_RELEASE;//否则回到按键释放状态
			}
		}
		break;
		case KEY_STATE_WOBBLE2:
		{	
			if(keyTemp!=KEY_NULL)//有按键被按下
			{
				if(KeyCountMs>KEY_WOBBLE_PERIOD)
				{
					keyState=KEY_STATE_2DOWN;
				}
			}
			else
			{
				lastKeyValue|=KEY_1HIT;//记录键值,单击
				keyState=KEY_STATE_RELEASE;//否则回到按键释放状态
			}
		}
		break;
		case KEY_STATE_2DOWN://双击按下状态
		{
			if(keyTemp==KEY_NULL)//无按键被按下,按键
			{
				keyState=KEY_STATE_RELEASE;//返回按键松开状态
				lastKeyValue|=KEY_2HIT;//记录键值,双击
			}
		}
		break;
		case KEY_STATE_LONG://长按状态
		{
			if(keyTemp!=KEY_NULL)//有按键被按下
			{
				{                           
					lastKeyValue=keyTemp|KEY_LONG_HOLD;//记录键值,长按
				}	
			}
			else keyState=KEY_STATE_RELEASE;//否则回到按键释放状态
		}
		break;
		default:break;
	}
	return lastKeyValue;//返回最终键值
}





