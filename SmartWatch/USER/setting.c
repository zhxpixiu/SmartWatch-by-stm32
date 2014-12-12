#include "stm32f10x.h"
#include "UI.h"
#include "prohead.h"
#include "driver.h" 
#include "lcdlib.h"
#include "Touch.h"
#include "rtc.h" 
#include "image.h"
#include "delay.h"
#include "key.h"

#define DEL "\1"
#define OK  "\2"


#define BT_COLOR      		WHITE
#define BORDER_COLOR  		LIGHTBLUE
#define BT_FCOLOR			LIGHTBLUE
#define BT_TOCHED_FCOLOR	WHITE
#define BT_TOCHED_COLOR		LIGHTBLUE

#define TIME_SETTING 0
#define DATE_SETTING 1

#define ON 1
#define OFF 0

#define YES 1
#define	NO	0

#define BT_T_OR_D_POS_X 22
#define BT_T_OR_D_POS_Y 0

#define SHIFT_BT 12
#define HOUR_AREA 13
#define MIN_AREA 14
#define SEC_AREA 15

//切换按钮坐标 22，0，55，40
//时间显示坐标 时：（22，75，22+31,75+31）  分：（22，75+47，22+31，75+32+47） 秒（22,75+47+48,22+31,75+48+48+31）

char *keyVal[]={"1","2","3","4","5","6","7","8","9",DEL,"0",OK}; //两个特殊字符的已宏定义为"\1""\2"

void Dis_Bt_Time_Or_Date(bool option)
{	
	char temp[]="        ";
	if(option==TIME_SETTING)
	{
		LCD_DisARectangular(22,0,37,40,LIGHTBLUE);
		LCD_DisARectangular(38,0,55,40,WHITE);
		LCD_DisGB2312String16x16(22,4,"时间",WHITE,LIGHTBLUE);
		LCD_DisGB2312String16x16(38,4,"日期",LIGHTBLUE,WHITE);
		sprintf(temp,"%02u:%02u:%02u",calendar.hour,calendar.min,calendar.sec);
		LCD_DisASCString(22,75 ,temp, WHITE, 4,RE_WRITE_BG);

		LCD_DisARectangular(26,75,18+31,75+31,WHITE);
		sprintf(temp,"%02u",calendar.hour);
		temp[2]=0;
		LCD_DisASCString(22,75 ,temp, LIGHTBLUE, 4,KEEP_BG);
	}
	else
	{
		LCD_DisARectangular(22,0,37,40,WHITE);
		LCD_DisARectangular(38,0,55,40,LIGHTBLUE);
		LCD_DisGB2312String16x16(22,4,"时间",LIGHTBLUE,WHITE);
		LCD_DisGB2312String16x16(38,4,"日期",WHITE,LIGHTBLUE);
		sprintf(temp,"%02u-%02u-%02u",calendar.w_year%100,calendar.w_month,calendar.w_date);
		LCD_DisASCString(22,75 ,temp, WHITE, 4,RE_WRITE_BG);

		LCD_DisARectangular(26,75,18+31,75+31,WHITE);
		sprintf(temp,"%02u",calendar.w_year%100);
		temp[2]=0;
		LCD_DisASCString(22,75 ,temp, LIGHTBLUE, 4,KEEP_BG);
	}
}
void Load_Keyborad(void)
{	
		u8 i;
		LCD_WriteReg(0X03);
		#if (LCD_DIRECT == 1)	
			LCD_WriteData(0X1008);		
		#else
			LCD_WriteData(0X1030);      //vertical
		#endif

		LCD_DisARectangular(75,0,175,219,BT_COLOR);

		LCD_WriteReg(0X03);
		#if (LCD_DIRECT == 1)	
			LCD_WriteData(0X1038);		//horizontal
		#else
			LCD_WriteData(0X1030);      //vertical
		#endif

		LCD_DisABorder(75,0,175,219,BORDER_COLOR);
		LCD_DisALine(100,0,1,220,BORDER_COLOR);
		LCD_DisALine(125,0,1,220,BORDER_COLOR);
		LCD_DisALine(150,0,1,220,BORDER_COLOR);
		LCD_DisALine(75,73,2,100,BORDER_COLOR);
		LCD_DisALine(75,146,2,100,BORDER_COLOR);
		
		for(i=0;i<12;i++)
		{
			LCD_DisASCString(76+(i/3)*25,31+(i%3)*73,keyVal[i],BT_FCOLOR,2,KEEP_BG);
		}
		Dis_Bt_Time_Or_Date(TIME_SETTING);
	
}
/*
input:area:0-11;
*/
void Bt_Feedback(u8 area)
{	
	if(area<12)
	{
		LCD_DisARectangular(76+(area/3)*25,1+(area%3)*73,76+(area/3)*25+23,1+(area%3)*73+71,BT_TOCHED_COLOR);
		LCD_DisABorder(76+(area/3)*25,1+(area%3)*73,76+(area/3)*25+23,1+(area%3)*73+71,WHITE);
		LCD_DisASCString(76+(area/3)*25,31+(area%3)*73,keyVal[area],BT_TOCHED_FCOLOR,2,KEEP_BG);
		// return keyVal[area];
	}
}
void Bt_Feedback_Cancel(u8 area)
{
	if(area<12)
	{
		LCD_DisARectangular(76+(area/3)*25,1+(area%3)*73,76+(area/3)*25+23,1+(area%3)*73+71,BT_COLOR);
		LCD_DisASCString(76+(area/3)*25,31+(area%3)*73,keyVal[area],BT_FCOLOR,2,KEEP_BG);
	}
}
bool Is_Date_Exsit(u16 year,u8 month,u8 date)
{
	switch(month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return (date<=31&&date>0);
		case 4:
		case 6:
		case 9:
		case 11:
			return (date<=30&&date>0);
		case 2:
			if(Is_Leap_Year(year)==YES)//如果是闰年
			{
				return (date<=29&&date>0);
			}
			else
			{
				return (date<=28&&date>0);
			}
		default: break;	
	}
	return NO;
}
//切换按钮坐标 22，0，55，40
//时间显示坐标 时：（22，75，22+31,75+31）  分：（22，75+47，22+31，75+32+47） 秒（22,75+47+48,22+31,75+48+48+31）
u8 Touch_Setting_Pannel(void)
{
	u16 x,y;
	u8  i,j;
	Touch_GetSite(&x,&y);
	for(i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			if(x>(76+i*25) && y>(1+j*73) && x<(76+(i)*25+23) && y<(1+(j)*73+71))
			{
				return i*3+j;
			}
		}
	}
	if(x>22 && y>0 && x<55 && y<(40))
	{
		return SHIFT_BT;
	}
	if(x>22 && y>75 && x<22+31 && y<75+31)
	{
		return HOUR_AREA;
	}
	if(x>22 && y>75+47 && x<22+31 && y<75+32+47)
	{
		return MIN_AREA;
	}
	if(x>22 && y>75+47+48 && x<22+31 && y<75+48+48+31)
	{
		return SEC_AREA;
	}
	return TOUCH_NULL;
}

void Ctrl_Setting_Touch_Handler(void)
{
	u8 TouchValTemp;
	static u8 TouchValTempOld=TOUCH_NULL;
	u8 keyValue;

	u8 area;
	bool option=TIME_SETTING;
	char temp[]=" ";
	char string1[]="  ";
	char string2[]="        ";
	u8 year=calendar.w_year%100;
	u8 month=calendar.w_month;
	u8 date=calendar.w_date;
	u8 hour=calendar.hour;
	u8 min=calendar.min;
	u8 sec=calendar.sec;

	u8 InputData[6];
	u8 InputPos=0;//输入位置

	// u8 
	while(1)
	{
		keyValue=KEY_Scan();
		if(keyValue==(ONE_HIT 2))
		{
			break;
		}
		TouchValTemp=TOUCH_Scan(Touch_Setting_Pannel);//输出值为0-15或上相应的标志
		if(TouchValTemp!=TOUCH_NULL)
		{
			if(TouchValTemp<16)//按住状态
			{
				if(TouchValTempOld!=TouchValTemp)
				{
					Bt_Feedback(TouchValTemp);
					TouchValTempOld=TouchValTemp;
				}
			}
			else//按住释放状态
			{
				area = TouchValTemp & 0x0f;// area取值为0~15
				Bt_Feedback_Cancel(area);//取消触屏反馈
				if(area<12)//键盘区域
				{
					if(area<9 || area== 10)
					{
						if(area<9)
						{
							InputData[InputPos]=area+1;//获取对应键值
						}
						else//0
						{
							InputData[InputPos]=0;
						}
						if(option==TIME_SETTING)//设置时间
						{
							switch(InputPos)
							{
								case 0:
									sprintf(temp,"%u",InputData[InputPos]);
									LCD_DisASCString(22,75 ,temp, WHITE, 4,RE_WRITE_BG);
									InputPos++;//指向下一个输入位置
									break;
								case 1:
									if((InputData[InputPos-1]*10+InputData[InputPos])<24)
									{
										hour=InputData[InputPos-1]*10+InputData[InputPos];
										sprintf(string1,"%02u",hour);
										LCD_DisASCString(22,75 ,string1, WHITE, 4,RE_WRITE_BG);
										InputPos++;//指向下一个输入位置
										sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47,22+31-4,75+32+47,WHITE);
										sprintf(string1,"%02u",min);
										LCD_DisASCString(22,75+47 ,string1, LIGHTBLUE, 4,KEEP_BG);
									}
									else
									{
										sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(26,75,18+31,75+31,WHITE);
										sprintf(string1,"%02u",hour);
										LCD_DisASCString(22,75 ,string1, LIGHTBLUE, 4,KEEP_BG);
										InputPos=0;
									}
									break;
								case 2:
									sprintf(temp,"%u",InputData[InputPos]);
									LCD_DisASCString(22,75+15+32,temp, WHITE, 4,RE_WRITE_BG);
									InputPos++;//指向下一个输入位置
									break;
								case 3:
									if((InputData[InputPos-1]*10+InputData[InputPos])<60)
									{
										min=InputData[InputPos-1]*10+InputData[InputPos];
										sprintf(string1,"%02u",min);
										LCD_DisASCString(22,75 ,string1, WHITE, 4,RE_WRITE_BG);
										InputPos++;//指向下一个输入位置

										sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47+48,22+31-4,75+48+48+31,WHITE);
										sprintf(string1,"%02u",sec);
										LCD_DisASCString(22,75+47+48 ,string1, LIGHTBLUE, 4,KEEP_BG);
									}
									else
									{
										sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47,22+31-4,75+32+47,WHITE);
										sprintf(string1,"%02u",min);
										LCD_DisASCString(22,75+47 ,string1, LIGHTBLUE, 4,KEEP_BG);
										InputPos=2;
									}
									break;
								case 4:
									sprintf(temp,"%u",InputData[InputPos]);
									LCD_DisASCString(22,75+15+32+32+16,temp, WHITE, 4,RE_WRITE_BG);
									InputPos++;//指向下一个输入位置
									break;
								case 5:
									if((InputData[InputPos-1]*10+InputData[InputPos])<60)
									{
										sec=InputData[InputPos-1]*10+InputData[InputPos];
										sprintf(string1,"%02u",min);
										LCD_DisASCString(22,75 ,string1, WHITE, 4,RE_WRITE_BG);
										InputPos=0;//指向下一个输入位置

										sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(26,75,18+31,75+31,WHITE);
										sprintf(string1,"%02u",hour);
										LCD_DisASCString(22,75 ,string1, LIGHTBLUE, 4,KEEP_BG);
									}
									else
									{
										sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47+48,22+31-4,75+48+48+31,WHITE);
										sprintf(string1,"%02u",sec);
										LCD_DisASCString(22,75+47+48 ,string1, LIGHTBLUE, 4,KEEP_BG);
										InputPos=4;
									}
									break;		
							}
						}
						else//设置日期
						{
							switch(InputPos)
							{
								case 0:
									sprintf(temp,"%u",InputData[InputPos]);
									LCD_DisASCString(22,75 ,temp, WHITE, 4,RE_WRITE_BG);
									InputPos++;//指向下一个输入位置
									break;
								case 1:
										year=InputData[InputPos-1]*10+InputData[InputPos];
										sprintf(string1,"%02u",year);
										LCD_DisASCString(22,75 ,string1, WHITE, 4,RE_WRITE_BG);
										InputPos++;//指向下一个输入位置
										sprintf(string2,"%02u-%02u-%02u",year,month,date);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47,22+31-4,75+32+47,WHITE);
										sprintf(string1,"%02u",month);
										LCD_DisASCString(22,75+47 ,string1, LIGHTBLUE, 4,KEEP_BG);
									break;
								case 2:
									sprintf(temp,"%u",InputData[InputPos]);
									LCD_DisASCString(22,75+15+32,temp, WHITE, 4,RE_WRITE_BG);
									InputPos++;//指向下一个输入位置
									break;
								case 3:
									if((InputData[InputPos-1]*10+InputData[InputPos])<13&&(InputData[InputPos-1]*10+InputData[InputPos])!=0)
									{
										month=InputData[InputPos-1]*10+InputData[InputPos];
										sprintf(string1,"%02u",month);
										LCD_DisASCString(22,75 ,string1, WHITE, 4,RE_WRITE_BG);
										InputPos++;//指向下一个输入位置

										sprintf(string2,"%02u-%02u-%02u",year,month,date);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47+48,22+31-4,75+48+48+31,WHITE);
										sprintf(string1,"%02u",date);
										LCD_DisASCString(22,75+47+48 ,string1, LIGHTBLUE, 4,KEEP_BG);
									}
									else
									{
										sprintf(string2,"%02u-%02u-%02u",year,month,date);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47,22+31-4,75+32+47,WHITE);
										sprintf(string1,"%02u",month);
										LCD_DisASCString(22,75+47 ,string1, LIGHTBLUE, 4,KEEP_BG);
										InputPos=2;
									}
									break;
								case 4:
									sprintf(temp,"%u",InputData[InputPos]);
									LCD_DisASCString(22,75+15+32+32+16,temp, WHITE, 4,RE_WRITE_BG);
									InputPos++;//指向下一个输入位置
									break;
								case 5:
									if(Is_Date_Exsit(year+2000,month,InputData[InputPos-1]*10+InputData[InputPos])==YES)
									{
										date=InputData[InputPos-1]*10+InputData[InputPos];
										sprintf(string1,"%02u",min);
										LCD_DisASCString(22,75 ,string1, WHITE, 4,RE_WRITE_BG);
										InputPos=0;//指向下一个输入位置

										sprintf(string2,"%02u-%02u-%02u",year,month,date);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(26,75,18+31,75+31,WHITE);
										sprintf(string1,"%02u",year);
										LCD_DisASCString(22,75 ,string1, LIGHTBLUE, 4,KEEP_BG);
									}
									else
									{
										sprintf(string2,"%02u-%02u-%02u",year,month,date);
										LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

										LCD_DisARectangular(22+4,75+47+48,22+31-4,75+48+48+31,WHITE);
										sprintf(string1,"%02u",date);
										LCD_DisASCString(22,75+47+48 ,string1, LIGHTBLUE, 4,KEEP_BG);
										InputPos=4;
									}
									break;		
							}
						}
					}
					else//非数字区
					{
						if(area==9)//back
						{
							return;
						}
						else//finish
						{
							if(InputPos%2==0)
							{
								if(option==TIME_SETTING)
								{
									RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,hour,min,sec); 
									RTC_Get();
									TimeShowingFlag=RESET;
								}
								else
								{
									if(Is_Date_Exsit(year+2000,month,date)==YES)
									{
										RTC_Set(year+2000,month,date,calendar.hour,calendar.min,calendar.sec); 
										RTC_Get();
										DateShowingFlag=RESET;
									}
								}
								return;
							}
						}
					}
				}
				else//非键盘区域
				{
					switch(area)
					{
						case SHIFT_BT:
							option=~option;
							Dis_Bt_Time_Or_Date(option);
							break;
						case HOUR_AREA:
							InputPos=0;
							if(option==TIME_SETTING)
							{
								sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
								LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

								LCD_DisARectangular(26,75,18+31,75+31,WHITE);
								sprintf(string1,"%02u",hour);
								LCD_DisASCString(22,75 ,string1, LIGHTBLUE, 4,KEEP_BG);
							}
							else
							{
								sprintf(string2,"%02u-%02u-%02u",year,month,date);
								LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

								LCD_DisARectangular(26,75,18+31,75+31,WHITE);
								sprintf(string1,"%02u",year);
								LCD_DisASCString(22,75 ,string1, LIGHTBLUE, 4,KEEP_BG);
							}
							break;

						case MIN_AREA:
							InputPos=2;
							if(option==TIME_SETTING)
							{
								sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
								LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

								LCD_DisARectangular(22+4,75+47,22+31-4,75+32+47,WHITE);
								sprintf(string1,"%02u",min);
								LCD_DisASCString(22,75+47 ,string1, LIGHTBLUE, 4,KEEP_BG);
							}
							else
							{
								sprintf(string2,"%02u-%02u-%02u",year,month,date);
								LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

								LCD_DisARectangular(22+4,75+47,22+31-4,75+32+47,WHITE);
								sprintf(string1,"%02u",month);
								LCD_DisASCString(22,75+47 ,string1, LIGHTBLUE, 4,KEEP_BG);
							}
							break;

						case SEC_AREA:
							InputPos=4;
							if(option==TIME_SETTING)
							{
								sprintf(string2,"%02u:%02u:%02u",hour,min,sec);
								LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

								LCD_DisARectangular(22+4,75+47+48,22+31-4,75+48+48+31,WHITE);
								sprintf(string1,"%02u",sec);
								LCD_DisASCString(22,75+47+48 ,string1, LIGHTBLUE, 4,KEEP_BG);
							}
							else
							{
								sprintf(string2,"%02u-%02u-%02u",year,month,date);
								LCD_DisASCString(22,75 ,string2, WHITE, 4,RE_WRITE_BG);

								LCD_DisARectangular(22+4,75+47+48,22+31-4,75+48+48+31,WHITE);
								sprintf(string1,"%02u",date);
								LCD_DisASCString(22,75+47+48 ,string1, LIGHTBLUE, 4,KEEP_BG);
							}
							break;

							default:break;
					}
				}
			}
		}
		else
		{
			TouchValTempOld=TOUCH_NULL;
		}
	}
}
