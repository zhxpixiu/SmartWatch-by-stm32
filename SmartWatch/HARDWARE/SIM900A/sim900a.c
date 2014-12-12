#include <stdio.h>
#include "sim900a.h"
#include "usart.h"		
#include "delay.h"	
#include "key.h"	 	 	 	 	 	  
#include "string.h"    		
#include "usart2.h" 
#include "rtc.h"
#include "lcdlib.h"
u8 TXdatabuf[32];
u8 RXdatabuf[32];
u8 networkOption;
// #define 27 37
/*u8 TXPhoneNum[]="18825174443";
u8 FeixinPwd[]="mima8565279";
u8 RXPhoneNum[]="18826484299";*/

//ATK-SIM900A �������(���Ų��ԡ����Ų��ԡ�GPRS����)���ô���
//usmart֧�ֲ��� 
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART2_RX_STA;
//     1,����USART2_RX_STA;
u8 sim_at_response(u8 mode,u8 *rx_buf)
{
    u8 len=0,i;
	
//	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
		len=(USART2_RX_STA&0X7FFF);
		if(len>32) len=32;
	    for(i=0;i<len;i++) 
		rx_buf[i]=USART2_RX_BUF[i];
		if(mode)USART2_RX_STA=0;		
	} 	 
	return len;
}

u8 sim_gsm_response(u8 mode,u8 *rx_buf)
{
    u16 len=0;
	u16 i;	
//	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		len=(USART2_RX_STA&0X7FFF);
		if(len>1) len=128;
	    for(i=0;i<len;i++) 
		rx_buf[i]=USART2_RX_BUF[i+68];
		if(mode)USART2_RX_STA=0;		
	} 	 
	return len;
}
//sim900a���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��sim900a��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
		USART2->DR=(u32)cmd;
	} else u2_printf("%s\r\n",cmd);//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim900a_check_cmd(ack))break;//�õ���Ч���� 
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0) res=1; 
	}
	return res;
} 
u8 sim900a_send1_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
		USART2->DR=(u32)cmd;
	} else u2_printf("%s",cmd);//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim900a_check_cmd(ack))break;//�õ���Ч���� 
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0) res=1; 
	}
	return res;
}
u8 	sim900a_gsm_get(u8 *data)
{
	u8  sim_reg=1;
	u16 waittime=1500; 
	// u16 waittime=800; 
	u8  i;
	u8 data1[128];

	sim_reg=sim900a_send_cmd("AT+CMGR=27","OK",500);
//	USART2_RX_STA=0;
	while(--waittime)	//�ȴ�����ʱ
	{	
		if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
		{
			sim_gsm_response(0,data1);

			for(i=0;i<32;i++)
			{
				*(data+i)=sim900a_chr2hex(data1[(4*i+3)]);
			}
			*(data+0)=2;
			*(data+1)=2;
			sim900a_send_cmd("AT+CMGD=27","OK",200);	  //ÿ�ζ�ɾ������	
			return sim_reg;	
		}
 		delay_ms(5);
	}	  
	if(waittime==0)
	{							  	
		*(data+1)=1;			
		*(data+0)=1;
	}
	sim900a_send_cmd("AT+CMGD=27","OK",200);	  //ÿ�ζ�ɾ������
    return sim_reg;
}
//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//����ֵ:�ַ�
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
//16����ת���ɶ�Ӧ���ַ���
void xToChar(uint8_t *data,char *s)
{
  u8 i;
  char temp[64];
  for ( i=0;i<32;i++)
  {
     sprintf(temp,"%x",data[i]);
     if (data[i]<16) strcat(s,"0");
     strcat(s,temp);
  }
  i=0;	
}


//���һ���ַ�����sizeof(s)*
void SetNull(char *s)
{
 
  memset(s,0,strlen(s));
 // i=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//GPRS���Բ��ִ���

//sim900a GPRS����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 sim900a_gprs_init(void)
{
 	
	if(networkOption==GPRS)
	{
		sim900a_send_cmd("AT+CSTT","OK",200);				//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
		sim900a_send_cmd("AT+CIICR","OK",100);//����PDP������,��������Э��,��������Ϣ
		sim900a_send_cmd("AT+CIFSR","OK",200);					//����GPRSҵ��
		//http ��ʼ��
		sim900a_send_cmd("AT+HTTPTERM","OK",100);
		sim900a_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",200);	
		sim900a_send_cmd("AT+SAPBR=1,1","OK",500);	
		sim900a_send_cmd("AT+HTTPINIT","OK",200);//��http
		sim900a_send_cmd("AT+CMGD=27","OK",200);	  //ÿ�ζ�ɾ������
		if(sim900a_send_cmd("AT+HTTPPARA=\"CID\",1","OK",200)==0)
		{
			return 0;
		}
		return 1;
	}
	if(networkOption==ETHERNET)
	{
		char *p="AT+CIPSTART=\"TCP\",\"wsn604604.vicp.cc\",\"1200\"";
	 	sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",200);	//�ر�����
		sim900a_send_cmd("AT+CIPSHUT","SHUT OK",200);		//�ر��ƶ����� 
		sim900a_send_cmd("AT+CSTT","OK",200);				//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
		sim900a_send_cmd("AT+CIICR","OK",500);//����PDP������,��������Э��,��������Ϣ
		sim900a_send_cmd("AT+CIFSR","OK",200);					//����GPRSҵ��
		if(sim900a_send_cmd((u8*)p,"OK",500)==0)   return 0; 
		return 1;
	}
	return 1;
} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//0��ʾ�ɹ�,��ʾ�����Ѿ����յ����ݣ��ڵ����״̬δ֪
//1��ʾû�з��ͳ�
//��sim900a����32λ���� 
u8 sim900a_send_data(u8 *data)
{	

    u8 i;
    u8 reg=1;
	//const char p[]="AT+HTTPPARA=\"URL\",\"http://2.smsfx.sinaapp.com/send.php?tel=18825174443&pwd=mima8565279&aim=18826484299&text=";	 
	const char p[]="AT+HTTPPARA=\"URL\",\"http://2.smsfx.sinaapp.com/send.php?tel=18825174443&pwd=mima8565279&aim=18825174443&text=";	 
	char str[34];
    if(networkOption==GPRS)
   	{
	   		 for(i=0;i<32;i++)
	    {
		    str[i] =data[i]+48;
	    }
	    str[32]='\"';
	    str[33]=0;
	    sim900a_send_cmd("AT+HTTPTERM","OK",100);	
	    sim900a_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",100);	
		// sim900a_send_cmd("AT+SAPBR=1,1","OK",100);					
		sim900a_send_cmd("AT+HTTPINIT","OK",100);
		sim900a_send_cmd("AT+HTTPPARA=\"CID\",1","OK",100);
		sim900a_send1_cmd((u8*)p,"ok",10);
	    sim900a_send_cmd((u8*)str,"OK",100);				 
		if(sim900a_send_cmd("AT+HTTPACTION=0","200",2000)==0)   return 0; 
		return 1;
   	}
   	if(networkOption==ETHERNET)
   	{
   // sim900a_gprs_test();                     //sim9000a�ĳ�ʼ�� 	
		if(sim900a_send_cmd("AT+CIPSEND",">",300)==0)		//��������
		{ 
		   u2_unm_printf(33,data);
		   if(sim900a_send_cmd((u8*)0X1A,"SEND OK",300)==0)  //��ȴ�10s
		   reg=0;
		}
		else 
		{
		  sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������
		  reg=1;
		}
		  USART2_RX_STA=0; 	
		  return reg;
   	}
   	return 1;
}

u8 sim900a_get_data(u8 *data)
{
    u8 buf2[32];
    u8 i;
	//  char data1[128];
    char *strs="+CMTI: \"SM\"";
	u16 waittime=4000;  
  
   	if(networkOption==GPRS)
   	{
	   	  for(i=0;i<30;i++)
	    {
	    	buf2[i]=0;
	    }
		buf2[30]=1;  	buf2[31]=1; 
	    if(sim900a_send_data(buf2)==0)
		{ 
			USART2_RX_STA=0;
			while(--waittime)	//�ȴ�����ʱ
		    {
				delay_ms(10);
				if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
				{
	         USART2_RX_BUF[500]=0;//��ӽ�����
				  USART2_RX_STA=0;//���յ��ڴ���Ӧ����
				  if(strstr((const char*)USART2_RX_BUF,(const char*)strs)) //�жϲ��ǵ�27�����ţ������ �Ļ��Ŷ�ȡ��Ϣ                   
	            {
	             sim900a_gsm_get(data);   //
								return 0;		       
		          }	 
					}
				}
						
			if(waittime==0)
			{							  	
				*(data+1)=1;			
				*(data+0)=1;
			}
			sim900a_send_cmd("AT+CMGD=27","OK",200);	  //ÿ�ζ�ɾ������
		  //  return 1;           
		}	    	
	   return 1;
   	}
   	if(networkOption==ETHERNET)
   	{
		waittime=800;  
		buf2[30]=1;  	buf2[31]=1; 
	    if(sim900a_send_data(buf2)==0)
		{ 
			while(--waittime)	//�ȴ�����ʱ
		    {
				delay_ms(10);
				if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
				{
			    	sim_at_response(0,data);	
					return 0;
				}
			}	  
	   }
	   return 1;
   	}
   	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//0��ʾ�ɹ�,��ʾ�����Ѿ����յ����ݣ��ڵ����״̬δ֪
//1��ʾû�з��ͳ�
//��sim900a����32λ����
// 
u8 sim900a_send_ctrl(u8 *data,u8 *data1)
{
    u8  sim_reg=1;
	u16 waittime=4000; 
	
    if(networkOption==GPRS)
   	{
	   	*(data+30)=0;
		*(data+31)=0;
		sim_reg=sim900a_send_data(data);
		if(sim_reg==1)	 return sim_reg;
		sim900a_send_cmd("AT+CMGD=27","OK",500);
		USART2_RX_STA=0;
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				// sim_at_response(0,data1);//��̫��	
				sim900a_gsm_get(data1);//����
				*(data1+0)=0;
				*(data1+1)=0;
				return sim_reg;
			}
		}	  
		if(waittime==0)
		{							  	
			*(data1+1)=1;			
			*(data1+0)=1;
		}
	    return sim_reg;
   	}
   	if(networkOption==ETHERNET)
   	{
 		*(data+30)=0;
		*(data+31)=0;
		sim_reg=sim900a_send_data(data);
		if(sim_reg==1)	 return sim_reg;
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				sim_at_response(0,data1);	
				*(data+0)=0;
				*(data+1)=0;
				return sim_reg;
			}
		}	  
		if(waittime==0)
		{							  	
			*(data+1)=1;			
			*(data+0)=1;
		}
	    return sim_reg;
   	}
   	return 1;
}
/********************************************************************************************************
*  Function: sim900a_set_time				                                                           
*  Object: set time from the internet by sim900a 
*  Input: 
*  Output:                                  
*  brief: none
********************************************************************************************************/
u8 sim900a_set_time(void)
{
	char time[17];//��ʽ:yy/mm/dd,hh:mm:ss
	u8 i;
	sim900a_send_cmd("AT+CLTS=1","OK",200);//�����������ʱ�书��
	if(sim900a_send_cmd("AT+CCLK?","OK",200)==0)//���ͻ�ȡʱ��ָ��
	{
		memcpy(time,(const char*)(u8 *)strstr((const char*)USART2_RX_BUF,"\"")+1,17);//����sim900a��ȡ��ʱ���ַ���(17byte)������str
	
		for(i=0;i<17;i++)//�����ֵ�ASCII�ַ�תΪ��Ӧ������
		{
			time[i] -= 48;
	 	}

		RTC_Set(2000+time[0]*10+time[1],//��
				time[3]*10+time[4],//��
				time[6]*10+time[7],//��
				time[9]*10+time[10],//ʱ
				time[12]*10+time[13],//��
				time[15]*10+time[16]);//��
		return 0;
	}
	return 1;
}

void sim900a_test(void)
{	
	char str[27];
	sim900a_send_cmd("ATE1","OK",200);
	sim900a_send_cmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"","OK",200);
	sim900a_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMWAP\"","OK",200);
	sim900a_send_cmd("AT+SAPBR=1,1","OK",200);
	sim900a_send_cmd("AT+SAPBR=2,1","OK",200);
	sim900a_send_cmd("AT+CIPGSMLOC=1,1","OK",200);
	memcpy(str,(const char*)USART2_RX_BUF,27);
	LCD_DisASCString(0, 0, str, WHITE,1,RE_WRITE_BG);
	memcpy(str,(const char*)USART2_RX_BUF+27,27);
	LCD_DisASCString(16, 0, str, WHITE,1,RE_WRITE_BG);
	memcpy(str,(const char*)USART2_RX_BUF+54,27);
	LCD_DisASCString(32, 0, str, WHITE,1,RE_WRITE_BG);
	while(1);
}
