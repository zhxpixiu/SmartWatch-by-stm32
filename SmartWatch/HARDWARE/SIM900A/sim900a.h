#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"

#define ETHERNET 0
#define	GPRS 1
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-SIM900A GSM/GPRS模块驱动	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2013/5/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved	
//********************************************************************************
//无
//////////////////////////////////////////////////////////////////////////////////	

#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//高低字节交换宏定义


void xToChar1(uint8_t *data,char *s);
u8 sim900a_tcpudp_test(u8* ipdata,char *p);
//u8 sim_at_response(u8 mode);
u8 sim_at_response(u8 mode,u8 *rx_buf);
u8* sim900a_check_cmd(u8 *str);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 sim900a_chr2hex(u8 chr);
u8 sim900a_hex2chr(u8 hex);
u8 sim900a_gprs_init(void);
void xToChar(uint8_t *data,char *s);
u8 sim900a_get_data(u8 *data);
u8 sim900a_send_data(u8 *data);
u8 sim900a_send_ctrl(u8 *data,u8 *data1);
void SetNull(char *s);
u8 sim900a_set_time(void);
void sim900a_test(void);
extern u8 TXdatabuf[32];
extern u8 RXdatabuf[32];
extern u8 networkOption;
#endif





