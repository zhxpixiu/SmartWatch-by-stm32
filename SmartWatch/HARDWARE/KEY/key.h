#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"

#define KEY2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//��ȡ����1


#define KEY_WOBBLE_PERIOD      10//ms ��������ʱ��
#define KEY_2HIT_INTERVAL	 200//ms  
#define KEY_LONG_PERIOD    	500//ms	  
#define KEY_STATE_RELEASE      0   //�����ͷ�״̬
#define KEY_STATE_WOBBLE       1   //��������״̬ 
#define KEY_STATE_1DOWN        2   //��������״̬
#define KEY_STATE_1UP    	   3   //
#define KEY_STATE_WOBBLE2	   4
#define KEY_STATE_2DOWN		   5  //
#define KEY_STATE_2UP	   	   6   //
#define KEY_STATE_LONG		   7	//��������״̬
#define KEY_STATE_LONG_HOLD	   8	//��������״̬

#define KEY_LONG               0x80//������־
#define KEY_LONG_HOLD          0x40//������ȴ���־
#define KEY_1HIT			   0X20//������־  
#define KEY_2HIT			   0x10//˫����־
#define KEY_NULL 0xff		   
#define PRESS_DOWN 1 			//�������±�־
#define PRESS_UP   0 			//�����ͷű�־
#define LONG_PERIOD 		 KEY_LONG | 
#define ONE_HIT				 KEY_1HIT |
#define TWO_HIT				 KEY_2HIT |
void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(void);  	//����ɨ�躯��					    
u8 getKeyValue(void);	//����ɨ��,IO��
#endif
