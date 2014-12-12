#include "Touch.h"
#include "delay.h"  
#include "stm32f10x_gpio.h"  
#include "lcdlib.h" 
#include "Driver.h"
#include "timer.h"

TOUCH_VAL_TYPE TouchVal;  


/*-------------------------------------------------------------------------------------------------------
*  触摸IC检测代码					 
-------------------------------------------------------------------------------------------------------*/
//往SPI接口发送数据
void TOUCH_init(void) //IO初始化
{ 
 	//touch_cs
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
}
void Touch_SPIWrite(u8 Val)
{
    u8 i;
    Driver_SPISCK(0);
    for(i=0; i<8; i++) 
		{
				if(Val&CHSBIT7)
					Driver_SPIMOSI(1);
				else
					Driver_SPIMOSI(0);
				Val <<= 1;
        Driver_SPISCK(0);
        __nop();
				__nop();
				__nop();
        Driver_SPISCK(1);
        __nop();
				__nop();
				__nop();
    }
}

//从SPI接口接收数据
u16 Touch_SPIRead(void)
{
    u8 i;
    u16 Val = 0;
    for(i=0; i<12; i++)
		{
        Val <<= 1;
        Driver_SPISCK(1);
        __nop();
				__nop();
				__nop();
        Driver_SPISCK(0);
        __nop();
				__nop();
				__nop();
        if(Driver_SPIMISO())
					Val++;
    }
    return Val;
}

//读一次触摸检测值
void Touch_GetVal(TOUCH_VAL_TYPE *Point)
{
    Driver_SPICS(0);
    Touch_SPIWrite(0x90);   //读取Y轴 检测值
    Driver_SPISCK(1);
    __nop();
		__nop();
		__nop();
    Driver_SPISCK(0);
    __nop();
		__nop();
		__nop();
    (*Point).y = Touch_SPIRead();
    Touch_SPIWrite(0xd0);  //读取X轴 检测值
    Driver_SPISCK(1);
    __nop();
		__nop();
		__nop();
    Driver_SPISCK(0);
    __nop();
		__nop();
		__nop();
    (*Point).x = Touch_SPIRead();
    Driver_SPICS(1);
}

//触摸数据采样处理 并转换为坐标
int Touch_GetSite(u16 *pRow, u16 *pColumn)
{
		u8 i,j;
		u16 Temp;
		TOUCH_VAL_TYPE Point[10];
		//读10次触摸采样值
		for(i = 0; i < 10; i++)
			Touch_GetVal(&Point[i]);
		//消抖 去掉前2个大值
		for(j = 0; j < 2; j++)
		{
				for(i = 0; i < 10-1; i++)
				{
						if(Point[i].x > Point[i+1].x)
						{
								Temp = Point[i+1].x;
								Point[i+1].x = Point[i].x;
								Point[i].x = Temp;
						}
						if(Point[i].y > Point[i+1].y)
						{
								Temp = Point[i+1].y;
								Point[i+1].y = Point[i].y;
								Point[i].y = Temp;
						}
				}
		}
		//消抖 去掉前2个小值
		for(j = 0; j < 2; j++)
		{
				for(i = 0; i < 10-1; i++)
				{
						if(Point[i].x < Point[i+1].x)
						{
								Temp = Point[i+1].x;
								Point[i+1].x = Point[i].x;
								Point[i].x = Temp;
						}
						if(Point[i].y < Point[i+1].y)
						{
								Temp = Point[i+1].y;
								Point[i+1].y = Point[i].y;
								Point[i].y = Temp;
						}
				}
		}
		//剩余求平均值
		for(i = 1; i < 6; i++)
			Point[0].x += Point[i].x;
		Point[0].x /= 6;
		for(i = 1; i < 6; i++)
			Point[0].y += Point[i].y;
		Point[0].y /= 6;
		//将触摸ADC值 转换为屏对应像素坐标
		if((Point[0].x > 255 && Point[0].x < 3755) && (Point[0].y > 470 && Point[0].y < 3800))
		{
				//转换为像素坐标
				Point[0].x = (Point[0].x-255)/((3855-255)/176);
				Point[0].y = (Point[0].y-470)/((3800-470)/220);
				//转换为行列值
				/**pRow = 220 - Point[0].y;
				*pColumn = Point[0].x;*/
				*pRow = Point[0].x;
				*pColumn = Point[0].y;
				return 1;
		}
    return 0;
}

//选择触摸颜色并更新显示

/*-------------------------------------------------------------------------------------------------------
*  触屏操作处理			  								 
-------------------------------------------------------------------------------------------------------*/



u8 Touch_Icons()
{
	u16 x,y;
	Touch_GetSite(&x,&y);
	if(x>110 && x<152 && y>23 && y<65)
	{
		return 1;
	}
	if(x>110 && x<152 && y>90 && y<132)
	{
		return 2;
	}
	if(x>110 && x<152 && y>155 && y<197)
	{
		return 3;
	}
	return TOUCH_NULL;
}

u8 Touch_Ctrl_Pannel(void)
{
	u16 x,y;
	u8  i,j;
	Touch_GetSite(&x,&y);
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			if(x>(5+i*57) && y>(5+j*71) && x<((i+1)*57) && y<((j+1)*71))
			{
				return i*3+j;
			}
		}
	}
	return TOUCH_NULL;
}

u8 TOUCH_Scan(u8 (*fun)(void))
{
	static u8 touchState=TOUCH_STATE_RELEASE;//定义状态机
	static u8 lastTouchValue;//最终键值
	
	u8 touchTemp;//键值暂态量
	touchTemp=fun();//获取扫描后的值
	switch(touchState)
	{			
		case TOUCH_STATE_RELEASE: //按键释放状态
		{
			if(touchTemp!=TOUCH_NULL)//有按键被按下
			{
				touchState=TOUCH_STATE_WOBBLE;//进入消抖状态
				TouchCountMs=0;
			}
			else lastTouchValue=TOUCH_NULL;//无按键按下则返回空值
		}
		break;
		case TOUCH_STATE_WOBBLE://按键消抖状态
		{
			if(touchTemp!=TOUCH_NULL)//有按键被按下
			{
				if(TouchCountMs>=TOUCH_WOBBLE_PERIOD)//若从第一次按键被按下经过的时间大于按键抖动区间
				{
					touchState=TOUCH_STATE_PRESS;//按键确实被按下
					lastTouchValue=touchTemp ;//记录键值

					TouchCountMs=0;
				}	
			}
			else 
			{
				touchState=TOUCH_STATE_RELEASE;//否则回到按键释放状态
				lastTouchValue=TOUCH_NULL;//无按键按下则返回空值
			}
		}
		break;
		case TOUCH_STATE_PRESS://按键按下状态
		{
			
			if(touchTemp!=lastTouchValue)
			{
				touchState=TOUCH_STATE_RELEASE;
				 // if(touchTemp==TOUCH_NULL)
				{                           
					touchTemp=lastTouchValue;
					lastTouchValue=TOUCH_NULL;
					return ( touchTemp |TOUCH_CLICK);
				}	
			}	
		}
		break;
		default:break;
	}
	return lastTouchValue;//返回最终键值
}









