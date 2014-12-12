#include "key.h"
#include "sys.h" 
#include "timer.h"
								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	//��ʼ��KEY0-->GPIOA.13,KEY1-->GPIOA.15  ��������
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;//PA0~1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA0,1
}

u8 getKeyValue(void)
{	 
	if(KEY2==0)
	{
	//	if(KEY1==0)return 1;
	    if(KEY2==0)return 2;
	}  
 	return KEY_NULL;// �ް�������
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
	static u8 keyState=KEY_STATE_RELEASE;//����״̬��
	static u8 lastKeyValue;//���ռ�ֵ
	u8 keyTemp;//��ֵ��̬��
	keyTemp=getKeyValue();//��ȡȫ����ɨ����ֵ
	switch(keyState)
	{			
		case KEY_STATE_RELEASE: //�����ͷ�״̬
		{
			if(keyTemp!=KEY_NULL)//�а���������
			{
				keyState=KEY_STATE_WOBBLE;//��������״̬
				KeyCountMs=0;
			}
			else lastKeyValue=KEY_NULL;//�ް��������򷵻ؿ�ֵ
		}
		break;
		case KEY_STATE_WOBBLE://��������״̬
		{
			if(keyTemp!=KEY_NULL)//�а���������
			{
				if((KeyCountMs)>=KEY_WOBBLE_PERIOD)//���ӵ�һ�ΰ��������¾�����ʱ����ڰ�����������
				{
					keyState=KEY_STATE_1DOWN;//����ȷʵ������
					lastKeyValue=keyTemp;//��¼��ֵ
				}	
			}
			else keyState=KEY_STATE_RELEASE;//����ص������ͷ�״̬
		}
		break;
		case KEY_STATE_1DOWN://��������״̬
		{
			if(keyTemp!=KEY_NULL)//�а���������
			{
				if((KeyCountMs)>=KEY_LONG_PERIOD)//���ӵ�һ�ΰ��������¾�����ʱ����ڶ���ĳ���ʱ��
				{                           
					keyState=KEY_STATE_LONG;//����ȷʵ������,���볤��״̬
					lastKeyValue=keyTemp|KEY_LONG;//��¼��ֵ
				}	
			}
			else 
			{
				keyState=KEY_STATE_1UP;//�����ɿ�
				KeyCountMs=0;//��ʱ������,�����µļ���
			}
		}
		break;
		case KEY_STATE_1UP://��һ���º��ɿ�
		{
			if(KeyCountMs<KEY_2HIT_INTERVAL)
			{
				if(KeyCountMs>50)//��ֹ��һ�ΰ����ɿ�ʱ�Ķ���������Ϊ˫��,�� XXms�����ж�
				{
					if(keyTemp!=KEY_NULL)//�а���������
					{
						keyState=KEY_STATE_WOBBLE2;
						KeyCountMs=0;//��ʱ������,�����µļ���
					}
				}
			}	
			else
			{
				lastKeyValue|=KEY_1HIT;//��¼��ֵ
				keyState=KEY_STATE_RELEASE;//����ص������ͷ�״̬
			}
		}
		break;
		case KEY_STATE_WOBBLE2:
		{	
			if(keyTemp!=KEY_NULL)//�а���������
			{
				if(KeyCountMs>KEY_WOBBLE_PERIOD)
				{
					keyState=KEY_STATE_2DOWN;
				}
			}
			else
			{
				lastKeyValue|=KEY_1HIT;//��¼��ֵ,����
				keyState=KEY_STATE_RELEASE;//����ص������ͷ�״̬
			}
		}
		break;
		case KEY_STATE_2DOWN://˫������״̬
		{
			if(keyTemp==KEY_NULL)//�ް���������,����
			{
				keyState=KEY_STATE_RELEASE;//���ذ����ɿ�״̬
				lastKeyValue|=KEY_2HIT;//��¼��ֵ,˫��
			}
		}
		break;
		case KEY_STATE_LONG://����״̬
		{
			if(keyTemp!=KEY_NULL)//�а���������
			{
				{                           
					lastKeyValue=keyTemp|KEY_LONG_HOLD;//��¼��ֵ,����
				}	
			}
			else keyState=KEY_STATE_RELEASE;//����ص������ͷ�״̬
		}
		break;
		default:break;
	}
	return lastKeyValue;//�������ռ�ֵ
}





