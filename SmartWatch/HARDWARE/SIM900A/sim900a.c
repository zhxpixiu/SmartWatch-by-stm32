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

//ATK-SIM900A 各项测试(拨号测试、短信测试、GPRS测试)共用代码
//usmart支持部分 
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//     1,清零USART2_RX_STA;
u8 sim_at_response(u8 mode,u8 *rx_buf)
{
    u8 len=0,i;
	
//	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
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
//	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		len=(USART2_RX_STA&0X7FFF);
		if(len>1) len=128;
	    for(i=0;i<len;i++) 
		rx_buf[i]=USART2_RX_BUF[i+68];
		if(mode)USART2_RX_STA=0;		
	} 	 
	return len;
}
//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
		USART2->DR=(u32)cmd;
	} else u2_printf("%s\r\n",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim900a_check_cmd(ack))break;//得到有效数据 
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
		while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
		USART2->DR=(u32)cmd;
	} else u2_printf("%s",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim900a_check_cmd(ack))break;//得到有效数据 
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
	while(--waittime)	//等待倒计时
	{	
		if(USART2_RX_STA&0X8000)//接收到期待的应答结果
		{
			sim_gsm_response(0,data1);

			for(i=0;i<32;i++)
			{
				*(data+i)=sim900a_chr2hex(data1[(4*i+3)]);
			}
			*(data+0)=2;
			*(data+1)=2;
			sim900a_send_cmd("AT+CMGD=27","OK",200);	  //每次都删掉短信	
			return sim_reg;	
		}
 		delay_ms(5);
	}	  
	if(waittime==0)
	{							  	
		*(data+1)=1;			
		*(data+0)=1;
	}
	sim900a_send_cmd("AT+CMGD=27","OK",200);	  //每次都删掉短信
    return sim_reg;
}
//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//将1个16进制数字转换为字符
//hex:16进制数字,0~15;
//返回值:字符
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
//16进制转换成对应的字符型
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


//清空一个字符数组sizeof(s)*
void SetNull(char *s)
{
 
  memset(s,0,strlen(s));
 // i=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//GPRS测试部分代码

//sim900a GPRS测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 sim900a_gprs_init(void)
{
 	
	if(networkOption==GPRS)
	{
		sim900a_send_cmd("AT+CSTT","OK",200);				//设置GPRS移动台类别为B,支持包交换和数据交换 
		sim900a_send_cmd("AT+CIICR","OK",100);//设置PDP上下文,互联网接协议,接入点等信息
		sim900a_send_cmd("AT+CIFSR","OK",200);					//附着GPRS业务
		//http 初始化
		sim900a_send_cmd("AT+HTTPTERM","OK",100);
		sim900a_send_cmd("AT+SAPBR=3,1,\"APN\",\"CMNET\"","OK",200);	
		sim900a_send_cmd("AT+SAPBR=1,1","OK",500);	
		sim900a_send_cmd("AT+HTTPINIT","OK",200);//开http
		sim900a_send_cmd("AT+CMGD=27","OK",200);	  //每次都删掉短信
		if(sim900a_send_cmd("AT+HTTPPARA=\"CID\",1","OK",200)==0)
		{
			return 0;
		}
		return 1;
	}
	if(networkOption==ETHERNET)
	{
		char *p="AT+CIPSTART=\"TCP\",\"wsn604604.vicp.cc\",\"1200\"";
	 	sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",200);	//关闭连接
		sim900a_send_cmd("AT+CIPSHUT","SHUT OK",200);		//关闭移动场景 
		sim900a_send_cmd("AT+CSTT","OK",200);				//设置GPRS移动台类别为B,支持包交换和数据交换 
		sim900a_send_cmd("AT+CIICR","OK",500);//设置PDP上下文,互联网接协议,接入点等信息
		sim900a_send_cmd("AT+CIFSR","OK",200);					//附着GPRS业务
		if(sim900a_send_cmd((u8*)p,"OK",500)==0)   return 0; 
		return 1;
	}
	return 1;
} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//0表示成功,表示主机已经接收到数据，节点接收状态未知
//1表示没有发送成
//向sim900a发送32位数据 
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
   // sim900a_gprs_test();                     //sim9000a的初始化 	
		if(sim900a_send_cmd("AT+CIPSEND",">",300)==0)		//发送数据
		{ 
		   u2_unm_printf(33,data);
		   if(sim900a_send_cmd((u8*)0X1A,"SEND OK",300)==0)  //最长等待10s
		   reg=0;
		}
		else 
		{
		  sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送
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
			while(--waittime)	//等待倒计时
		    {
				delay_ms(10);
				if(USART2_RX_STA&0X8000)//接收到期待的应答结果
				{
	         USART2_RX_BUF[500]=0;//添加结束符
				  USART2_RX_STA=0;//接收到期待的应答结果
				  if(strstr((const char*)USART2_RX_BUF,(const char*)strs)) //判断不是第27条短信，如果是 的话才读取短息                   
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
			sim900a_send_cmd("AT+CMGD=27","OK",200);	  //每次都删掉短信
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
			while(--waittime)	//等待倒计时
		    {
				delay_ms(10);
				if(USART2_RX_STA&0X8000)//接收到期待的应答结果
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
//0表示成功,表示主机已经接收到数据，节点接收状态未知
//1表示没有发送成
//向sim900a发送32位数据
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
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
			{
				// sim_at_response(0,data1);//以太网	
				sim900a_gsm_get(data1);//短信
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
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
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
	char time[17];//格式:yy/mm/dd,hh:mm:ss
	u8 i;
	sim900a_send_cmd("AT+CLTS=1","OK",200);//开启网络更新时间功能
	if(sim900a_send_cmd("AT+CCLK?","OK",200)==0)//发送获取时间指令
	{
		memcpy(time,(const char*)(u8 *)strstr((const char*)USART2_RX_BUF,"\"")+1,17);//将从sim900a获取的时间字符串(17byte)复制至str
	
		for(i=0;i<17;i++)//由数字的ASCII字符转为对应的数字
		{
			time[i] -= 48;
	 	}

		RTC_Set(2000+time[0]*10+time[1],//年
				time[3]*10+time[4],//月
				time[6]*10+time[7],//日
				time[9]*10+time[10],//时
				time[12]*10+time[13],//分
				time[15]*10+time[16]);//秒
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
