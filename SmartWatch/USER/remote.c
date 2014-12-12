#include "delay.h"  
#include "sys.h"
#include "usart.h"
#include "stm32f10x_gpio.h"  
#include "timer.h"
#include "UI.h"
#include "sim900a.h"
#include "stm32f10x.h"
#include "prohead.h"
#include "driver.h" 
#include "lcdlib.h"
#include "Touch.h"
#include "image.h"
#include "remote.h"
#include "key.h"
#include <stdio.h>

#define BLOCK_ON_COLOR			WHITE
#define BLOCK_OFF_COLOR			LIGHTBLUE
#define BLOCK_FRESH_COLOR			GRAY

#define BORDER_ON_COLOR			LIGHTBLUE
#define BORDER_OFF_COLOR		WHITE

#define FONT_COLOR_ON			LIGHTBLUE
#define FONT_COLOR_OFF			WHITE

#define AIR_CONDITIONER			0
#define LIVING_ROOM_LIGHT 		1
#define BEDROOM_LIGHT			2
#define DOOR 					3
#define WINDOW					5

#define SUCCEED 0
#define FAILED  1
#define ON 1
#define OFF 0

#define SHIFT_TIME 1000

extern u8 TXdatabuf[32];     
extern u8 RXdatabuf[32]; 
bool block_state[9]={OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF};

char temperature[]="  ";
char humidity[]="  ";

void Fill_Block_Color(u8 order,u16 color)
{
	u8 i,j;
	i=order/3;
	j=order%3;
	LCD_DisARectangular(5+i*57,5+j*71,(i+1)*57,(j+1)*71,color);
	
}
void Ctrl_Pannel_Touch_FdBack(u8 order,u16 color)
{
	u8 i,j;
	i=order/3;
	j=order%3;
	LCD_DisAWideBorder(5+i*57,5+j*71,(i+1)*57,(j+1)*71,3,color);
}
void Ctrl_Pannel_Touch_Release_FdBack(u8 order,u16 color)
{
	u8 i,j;
	i=order/3;
	j=order%3;
	LCD_DisAWideBorder(5+i*57,5+j*71,(i+1)*57,(j+1)*71,3,color);
}
void Reset_Txdatabuf(void)
{
	u8 i=0;
	for(i=0;i<=6;i++)
	{
		TXdatabuf[i]=2;//2Ϊ���ÿ�����Ϣ�������ն����ж���
	}
}

void refresh(void)
{
	u16 temp;

	if(sim900a_get_data(RXdatabuf)==SUCCEED)
	{
		if(RXdatabuf[2]!=2)block_state[AIR_CONDITIONER]=RXdatabuf[2];
		if(RXdatabuf[3]!=2)block_state[LIVING_ROOM_LIGHT]=RXdatabuf[3];
		if(RXdatabuf[4]!=2)block_state[BEDROOM_LIGHT]=RXdatabuf[4];
		if(RXdatabuf[5]!=2)block_state[DOOR]=RXdatabuf[5];
		if(RXdatabuf[6]!=2)block_state[WINDOW]=RXdatabuf[6];

		//������ʾ���µ����
		if(block_state[AIR_CONDITIONER]==OFF)
		{
			Fill_Block_Color(0,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22,22,"�յ�",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(0,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22,22,"�յ�",LIGHTBLUE,BLOCK_ON_COLOR);
		}

		if(block_state[LIVING_ROOM_LIGHT]==OFF)
		{
			Fill_Block_Color(1,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8,"������",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(1,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8,"������",LIGHTBLUE,BLOCK_ON_COLOR);
		}
		
		if(block_state[BEDROOM_LIGHT]==OFF)
		{
			Fill_Block_Color(2,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8+71,"���ҵ�",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(2,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22,22+71-8+71,"���ҵ�",LIGHTBLUE,BLOCK_ON_COLOR);
		}
		
		if(block_state[DOOR]==OFF)
		{
			Fill_Block_Color(3,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22+57,30,"��",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(3,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22+57,30,"��",LIGHTBLUE,BLOCK_ON_COLOR);
		}
		if(block_state[WINDOW]==OFF)
		{
			Fill_Block_Color(5,BLOCK_OFF_COLOR);
			LCD_DisGB2312String16x16(22+57,30+142,"��",WHITE,LIGHTBLUE);
		}
		else
		{
			Fill_Block_Color(5,BLOCK_ON_COLOR);
			LCD_DisGB2312String16x16(22+57,30+142,"��",LIGHTBLUE,BLOCK_ON_COLOR);
		}

		//�˴���ʾ��ʪ�ȸ���
		if(RXdatabuf[6]!=2)
		{
			temp=(u16)RXdatabuf[7];
			temp <<=8;
			temp = (u16)(temp + RXdatabuf[8])/10;
			sprintf(temperature,"%02u",temp);

			temp=(u16)RXdatabuf[9];
			temp <<=8;
			temp = (u16)(temp + RXdatabuf[10])/10;
			sprintf(humidity,"%02u",temp);

			LCD_DisARectangular(145,45,145+16,70,LIGHTBLUE);
			LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
		
			LCD_DisARectangular(145,45+71,145+16,70+71,LIGHTBLUE);
			LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
		}

		//
		LCD_DisGB2312String16x16(22+57,22+71-16,"�������",WHITE,GRAY);
		delay_ms(800);
		Fill_Block_Color(4,LIGHTBLUE);
		LCD_DisGB2312String16x16(22+57,22+71,"����",WHITE,LIGHTBLUE);

	}
	else
	{
		LCD_DisGB2312String16x16(22+57,22+71-16,"����ʧ��",WHITE,GRAY);
		delay_ms(1500);
		Fill_Block_Color(4,LIGHTBLUE);
		LCD_DisGB2312String16x16(22+57,22+71,"����",WHITE,LIGHTBLUE);
	}
}

void Load_Ctrl_Pannel(void)
{
	LCD_WriteReg(0X03);
	#if (LCD_DIRECT == 1)	
		LCD_WriteData(0X1008);		
	#else
		LCD_WriteData(0X1030);      //vertical
	#endif
	LCD_ClrScr(LIGHTBLUE);
	LCD_WriteReg(0X03);
	#if (LCD_DIRECT == 1)	
		LCD_WriteData(0X1038);		//horizontal
	#else
		LCD_WriteData(0X1030);      //vertical
	#endif

	TimeShowingFlag=RESET;
	flagShowIcons=RESET;
	DateShowingFlag=RESET;

	LCD_DisAWideBorder(0,0,175,219,5,BLACK);
	LCD_DisLines(57,0,1,220,4,BLACK);
	LCD_DisLines(114,0,1,220,4,BLACK);
	LCD_DisLines(0,71,2,176,4,BLACK);
	LCD_DisLines(0,142,2,176,4,BLACK);
	LCD_DisLines(0,213,2,176,2,BLACK);


	//������壬�������״̬ά�����һ�θ��µ�״̬
	if(block_state[AIR_CONDITIONER]==OFF)
	{
		LCD_DisGB2312String16x16(22,22,"�յ�",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(0,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22,22,"�յ�",LIGHTBLUE,BLOCK_ON_COLOR);
	}

	if(block_state[LIVING_ROOM_LIGHT]==OFF)
	{
		LCD_DisGB2312String16x16(22,22+71-8,"������",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(1,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22,22+71-8,"������",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	
	if(block_state[BEDROOM_LIGHT]==OFF)
	{
		LCD_DisGB2312String16x16(22,22+71-8+71,"���ҵ�",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(2,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22,22+71-8+71,"���ҵ�",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	
	if(block_state[DOOR]==OFF)
	{
		LCD_DisGB2312String16x16(22+57,30,"��",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(3,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22+57,30,"��",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	if(block_state[WINDOW]==OFF)
	{
		LCD_DisGB2312String16x16(22+57,30+142,"��",WHITE,LIGHTBLUE);
	}
	else
	{
		Fill_Block_Color(5,BLOCK_ON_COLOR);
		LCD_DisGB2312String16x16(22+57,30+142,"��",LIGHTBLUE,BLOCK_ON_COLOR);
	}
	Fill_Block_Color(4,GRAY);	
	LCD_DisGB2312String16x16(22+57,22+71-16,"���ڸ���",WHITE,GRAY);

	LCD_DisGB2312String16x16(10+57+57,10,"�¶�",WHITE,LIGHTBLUE);
	
	LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
	
	LCD_DisGB2312String16x16(10+57+57,10+71,"ʪ��",WHITE,LIGHTBLUE);
	
	LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);

	LCD_DisGB2312String16x16(22+57+57,24+142,"����",WHITE,LIGHTBLUE);

 	//�����豸״̬
	refresh();

}

void Ctrl_Pannel_Touch_Handler(void)
{
	u8 TouchValTemp;
	u8 keyValue;
	u8 block;
	u16 temp;
	u8 i;
	while(1)
	{
		keyValue=KEY_Scan();
		if(keyValue==(ONE_HIT 2))
		{
			return;
		}

		TouchValTemp=TOUCH_Scan(Touch_Ctrl_Pannel);//���ֵΪ0��8������Ӧ�ı�־
		if(TouchValTemp!=TOUCH_NULL)
		{
			if(TouchValTemp<9)
			{
				
				if(block_state[TouchValTemp]==OFF)//�������鵱ǰ״̬
				{
					Ctrl_Pannel_Touch_FdBack(TouchValTemp,WHITE);
				}
				else
				{
					Ctrl_Pannel_Touch_FdBack(TouchValTemp,LIGHTBLUE);
				}
			}
			else
			{
				block = TouchValTemp & 0x0f;//block ȡֵΪ0~8
				if(block<8)
				{
					Fill_Block_Color(block,BLOCK_FRESH_COLOR);
				}
				switch(block)
				{

					case 0:
						Reset_Txdatabuf();

						if(block_state[AIR_CONDITIONER]==ON)
						{
							TXdatabuf[2]=OFF;
							LCD_DisGB2312String16x16(22,22-16,"���ڹر�",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[2]=ON;
							LCD_DisGB2312String16x16(22,22-16,"���ڴ�",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//���ͳɹ�
						{
							if(RXdatabuf[0]==SUCCEED)//������Ϣ�ɹ�
							{
								if(TXdatabuf[2]==RXdatabuf[2])//���Ϳ�����Ϣ���Ӧ����״̬��Ϣһ��
								{
									if(block_state[AIR_CONDITIONER]==ON)
									{
										block_state[AIR_CONDITIONER]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[AIR_CONDITIONER]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//����ʧ��
								{
									LCD_DisGB2312String16x16(22,22-16,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[AIR_CONDITIONER]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//����ʧ��
							{
								LCD_DisGB2312String16x16(22,22-16,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[AIR_CONDITIONER]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//����ʧ��
						{
							LCD_DisGB2312String16x16(22,22-16,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[AIR_CONDITIONER]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22,22,"�յ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;

					case 1:
						Reset_Txdatabuf();

						if(block_state[LIVING_ROOM_LIGHT]==ON) 
						{
							TXdatabuf[3]=OFF;
							LCD_DisGB2312String16x16(22,22-16+71,"���ڹر�",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[3]=ON;
							LCD_DisGB2312String16x16(22,22-16+71,"���ڴ�",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//���ͳɹ�
						{
							if(RXdatabuf[0]==SUCCEED)//������Ϣ�ɹ�
							{
								if(TXdatabuf[3]==RXdatabuf[3])//���Ϳ�����Ϣ���Ӧ����״̬��Ϣһ��
								{
									if(block_state[LIVING_ROOM_LIGHT]==ON)
									{
										block_state[LIVING_ROOM_LIGHT]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[LIVING_ROOM_LIGHT]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//����ʧ��
								{
									LCD_DisGB2312String16x16(22,22-16+71,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[LIVING_ROOM_LIGHT]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//����ʧ��
							{
								LCD_DisGB2312String16x16(22,22-16+71,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[LIVING_ROOM_LIGHT]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//����ʧ��
						{
							LCD_DisGB2312String16x16(22,22-16+71,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[LIVING_ROOM_LIGHT]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22,22+71-8,"������",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;

					case 2:
						Reset_Txdatabuf();

						if(block_state[BEDROOM_LIGHT]==ON) 
						{
							TXdatabuf[4]=OFF;
							LCD_DisGB2312String16x16(22,22-16+142,"���ڹر�",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[4]=ON;
							LCD_DisGB2312String16x16(22,22-16+142,"���ڴ�",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//���ͳɹ�
						{
							if(RXdatabuf[0]==SUCCEED)//������Ϣ�ɹ�
							{
								if(TXdatabuf[4]==RXdatabuf[4])//���Ϳ�����Ϣ���Ӧ����״̬��Ϣһ��
								{
									if(block_state[BEDROOM_LIGHT]==ON)
									{
										block_state[BEDROOM_LIGHT]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[BEDROOM_LIGHT]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//����ʧ��
								{
									LCD_DisGB2312String16x16(22,22-16+142,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[BEDROOM_LIGHT]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//����ʧ��
							{
								LCD_DisGB2312String16x16(22,22-16+142,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[BEDROOM_LIGHT]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//����ʧ��
						{
							LCD_DisGB2312String16x16(22,22-16+142,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[BEDROOM_LIGHT]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22,22+142-8,"���ҵ�",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;
					case 3:
												Reset_Txdatabuf();

						if(block_state[DOOR]==ON)
						{
							TXdatabuf[5]=OFF;
							LCD_DisGB2312String16x16(22+57,22-16,"���ڹر�",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[5]=ON;
							LCD_DisGB2312String16x16(22+57,22-16,"���ڴ�",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//���ͳɹ�
						{
							if(RXdatabuf[0]==SUCCEED)//������Ϣ�ɹ�
							{
								if(TXdatabuf[5]==RXdatabuf[5])//���Ϳ�����Ϣ���Ӧ����״̬��Ϣһ��
								{
									if(block_state[DOOR]==ON)
									{
										block_state[DOOR]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[DOOR]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//����ʧ��
								{
									LCD_DisGB2312String16x16(22+57,22-16,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[DOOR]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//����ʧ��
							{
								LCD_DisGB2312String16x16(22+57,22-16,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[DOOR]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//����ʧ��
						{
							LCD_DisGB2312String16x16(22+57,22-16,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[DOOR]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22+57,30,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;
					case 4:
						LCD_DisGB2312String16x16(22+57,22+71-16,"���ڸ���",WHITE,BLOCK_FRESH_COLOR);	
						refresh();
						 break;	 
					case 5:
						Reset_Txdatabuf();

						if(block_state[WINDOW]==ON)
						{
							TXdatabuf[6]=OFF;
							LCD_DisGB2312String16x16(22+57,22-16+142,"���ڹر�",WHITE,BLOCK_FRESH_COLOR);
						}
						else
						{
							TXdatabuf[6]=ON;
							LCD_DisGB2312String16x16(22+57,22-16+142,"���ڴ�",WHITE,BLOCK_FRESH_COLOR);
						}

						if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)//���ͳɹ�
						{
							if(RXdatabuf[0]==SUCCEED)//������Ϣ�ɹ�
							{
								if(TXdatabuf[6]==RXdatabuf[6])//���Ϳ�����Ϣ���Ӧ����״̬��Ϣһ��
								{
									if(block_state[WINDOW]==ON)
									{
										block_state[WINDOW]=OFF;
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
									else
									{
										block_state[WINDOW]=ON;
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
								}
								else//����ʧ��
								{
									LCD_DisGB2312String16x16(22+57,22-16+142,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									if(block_state[WINDOW]==ON)
									{
										Fill_Block_Color(block,BLOCK_ON_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
									}
									else
									{
										Fill_Block_Color(block,BLOCK_OFF_COLOR);
										LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
									}
								}
							}
							else//����ʧ��
							{
								LCD_DisGB2312String16x16(22+57,22-16+142,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								if(block_state[WINDOW]==ON)
								{
									Fill_Block_Color(block,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
								}
								else
								{
									Fill_Block_Color(block,BLOCK_OFF_COLOR);
									LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
								}
							}
						}
						else//����ʧ��
						{
							LCD_DisGB2312String16x16(22+57,22-16+142,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							if(block_state[WINDOW]==ON)
							{
								Fill_Block_Color(block,BLOCK_ON_COLOR);
								LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_ON,BLOCK_ON_COLOR);
							}
							else
							{
								Fill_Block_Color(block,BLOCK_OFF_COLOR);
								LCD_DisGB2312String16x16(22+57,30+142,"��",FONT_COLOR_OFF,BLOCK_OFF_COLOR);
							}
						}
						break;
					case 6:
						LCD_DisGB2312String16x16(10+57+57,10,"�¶�",WHITE,BLOCK_FRESH_COLOR);
						LCD_DisGB2312String16x16(150,14+10,"������",WHITE,BLOCK_FRESH_COLOR);
						if(sim900a_get_data(RXdatabuf)==SUCCEED)
						{
							if(RXdatabuf[0]==SUCCEED)
							{
								if(RXdatabuf[6]!=2)
								{
									LCD_DisGB2312String16x16(150,10,"���³ɹ�",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									temp=(u16)RXdatabuf[7];
									temp <<=8;
									temp = (u16)(temp + RXdatabuf[8])/10;
									sprintf(temperature,"%02u",temp);
									Fill_Block_Color(6,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10,"�¶�",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
								}
								else//����ʧ��
								{
									LCD_DisARectangular(150,10,150+16,70,BLOCK_FRESH_COLOR);
									LCD_DisGB2312String16x16(150,10,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									Fill_Block_Color(6,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10,"�¶�",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
								}
							}
							else//����ʧ��
							{
								LCD_DisARectangular(150,10,150+16,70,BLOCK_FRESH_COLOR);
								LCD_DisGB2312String16x16(150,10,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								Fill_Block_Color(6,LIGHTBLUE);
								LCD_DisGB2312String16x16(10+57+57,10,"�¶�",WHITE,LIGHTBLUE);
								LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
							}
						}
						else//����ʧ��
						{
							LCD_DisARectangular(150,10,150+16,70,BLOCK_FRESH_COLOR);
							LCD_DisGB2312String16x16(150,10,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							Fill_Block_Color(6,LIGHTBLUE);
							LCD_DisGB2312String16x16(10+57+57,10,"�¶�",WHITE,LIGHTBLUE);
							LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
						}

						 break;
					case 7:
						LCD_DisGB2312String16x16(10+57+57,10+71,"ʪ��",WHITE,BLOCK_FRESH_COLOR);
						LCD_DisGB2312String16x16(150,14+10+71,"������",WHITE,BLOCK_FRESH_COLOR);
						if(sim900a_get_data(RXdatabuf)==SUCCEED)
						{
							if(RXdatabuf[0]==SUCCEED)
							{
								if(RXdatabuf[6]!=2)
								{
									LCD_DisGB2312String16x16(150,10+71,"���³ɹ�",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									temp=(u16)RXdatabuf[9];
									temp <<=8;
									temp = (u16)(temp + RXdatabuf[10])/10;
									sprintf(humidity,"%02u",temp);
									Fill_Block_Color(7,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10+71,"ʪ��",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
								}
								else//����ʧ��
								{
									LCD_DisARectangular(150,10+71,150+16,70+71,BLOCK_FRESH_COLOR);
									LCD_DisGB2312String16x16(150,10+71,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
									delay_ms(SHIFT_TIME);
									Fill_Block_Color(7,LIGHTBLUE);
									LCD_DisGB2312String16x16(10+57+57,10+71,"ʪ��",WHITE,LIGHTBLUE);
									LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
								}
							}
							else//����ʧ��
							{
								LCD_DisARectangular(150,10+71,150+16,70+71,BLOCK_FRESH_COLOR);
								LCD_DisGB2312String16x16(150,10+71,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
								delay_ms(SHIFT_TIME);
								Fill_Block_Color(7,LIGHTBLUE);
								LCD_DisGB2312String16x16(10+57+57,10+71,"ʪ��",WHITE,LIGHTBLUE);
								LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
							}
						}
						else//����ʧ��
						{
							LCD_DisARectangular(150,10+71,150+16,70+71,BLOCK_FRESH_COLOR);
							LCD_DisGB2312String16x16(150,10+71,"����ʧ��",WHITE,BLOCK_FRESH_COLOR);
							delay_ms(SHIFT_TIME);
							Fill_Block_Color(7,LIGHTBLUE);
							LCD_DisGB2312String16x16(10+57+57,10+71,"ʪ��",WHITE,LIGHTBLUE);
							LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);
						}
						 break;	 
					case 8:
						Fill_Block_Color(8,BLOCK_FRESH_COLOR);
						LCD_DisGB2312String16x16(22+57+57,24+142,"����",WHITE,BLOCK_FRESH_COLOR);	
						return;
				/*		LCD_WriteReg(0X03);
						#if (LCD_DIRECT == 1)	
							LCD_WriteData(0X1030);		
						#else
							LCD_WriteData(0X1030);      //vertical
						#endif
						// LCD_ClrScr(LIGHTBLUE);
						for(i=0;i<9;i++)
						{
							Fill_Block_Color(i,LIGHTBLUE);
						}	
						LCD_WriteReg(0X03);
						#if (LCD_DIRECT == 1)	
							LCD_WriteData(0X1038);		//horizontal
						#else
							LCD_WriteData(0X1030);      //vertical
						#endif

								TimeShowingFlag=RESET;
								flagShowIcons=RESET;
								DateShowingFlag=RESET;
						

								LCD_DisGB2312String16x16(22+57+57,24+142,"����",WHITE,LIGHTBLUE);
								while(1)
								{
									TouchValTemp=TOUCH_Scan(Touch_Ctrl_Pannel);//���ֵΪ0��8������Ӧ�ı�־
									if(TouchValTemp!=TOUCH_NULL)
									{
										if(TouchValTemp<9)
										{
											Ctrl_Pannel_Touch_FdBack(TouchValTemp,WHITE);
										}
										else
										{
											block = TouchValTemp & 0x0f;//block ȡֵΪ0~8
											Ctrl_Pannel_Touch_Release_FdBack(block,LIGHTBLUE);
											if(block==8)
											{
												break;
											}
										}
									}
								}
								LCD_WriteReg(0X03);
								#if (LCD_DIRECT == 1)	
									LCD_WriteData(0X1008);		
								#else
									LCD_WriteData(0X1030);      //vertical
								#endif
								// LCD_ClrScr(LIGHTBLUE);
								for(i=0;i<9;i++)
								{
									Fill_Block_Color(i,LIGHTBLUE);
								}	
								LCD_WriteReg(0X03);
								#if (LCD_DIRECT == 1)	
									LCD_WriteData(0X1038);		//horizontal
								#else
									LCD_WriteData(0X1030);      //vertical
								#endif

								TimeShowingFlag=RESET;
								flagShowIcons=RESET;
								DateShowingFlag=RESET;


								//������壬�������״̬ά�����һ�θ��µ�״̬
								if(block_state[AIR_CONDITIONER]==OFF)
								{
									LCD_DisGB2312String16x16(22,22,"�յ�",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(0,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22,"�յ�",LIGHTBLUE,BLOCK_ON_COLOR);
								}

								if(block_state[LIVING_ROOM_LIGHT]==OFF)
								{
									LCD_DisGB2312String16x16(22,22+71-8,"������",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(1,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8,"������",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								
								if(block_state[BEDROOM_LIGHT]==OFF)
								{
									LCD_DisGB2312String16x16(22,22+71-8+71,"���ҵ�",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(2,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22,22+71-8+71,"���ҵ�",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								
								if(block_state[DOOR]==OFF)
								{
									LCD_DisGB2312String16x16(22+57,30,"��",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(3,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30,"��",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								if(block_state[WINDOW]==OFF)
								{
									LCD_DisGB2312String16x16(22+57,30+142,"��",WHITE,LIGHTBLUE);
								}
								else
								{
									Fill_Block_Color(5,BLOCK_ON_COLOR);
									LCD_DisGB2312String16x16(22+57,30+142,"��",LIGHTBLUE,BLOCK_ON_COLOR);
								}
								LCD_DisGB2312String16x16(22+57,22+71,"����",WHITE,LIGHTBLUE);

								LCD_DisGB2312String16x16(10+57+57,10,"�¶�",WHITE,LIGHTBLUE);
								
								LCD_DisASCString(145,45,temperature,WHITE,2,KEEP_BG);
								
								LCD_DisGB2312String16x16(10+57+57,10+71,"ʪ��",WHITE,LIGHTBLUE);
								
								LCD_DisASCString(145,45+71,humidity,WHITE,2,KEEP_BG);

								LCD_DisGB2312String16x16(22+57+57,24+142,"����",WHITE,LIGHTBLUE);
								break;*/		
					default:break;
				}
			}
		}
	}
}





