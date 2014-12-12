#ifndef __Driver__
#define __Driver__
/*--------------------------------------------------------------------------
// 文件名：Driver.h
// 描述：  系统驱动头文件
// 设计者：EU电子
// 创建日期：2013年11月10日
// 深圳移优科技出品-版权所有-翻版必究
// EU-热爱嵌入式开发
// http://euse.taobao.com
//-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------
*  向外接口	为应用层提供的接口										 
-------------------------------------------------------------------------------------------------------*/
//mcu
extern void Driver_MCU_Init(void);
//lcd
extern void Driver_LcdBacklight(bool sta);
extern void Driver_LcdReset(bool sta);  
extern void Driver_LcdCS(bool sta);
extern void Driver_LcdRS(bool sta);
extern void Driver_LcdRD(bool sta);
extern void Driver_LcdWR(bool sta);
extern void Driver_LcdSendData(u16 Temp);
void Driver_SPICS(bool sta);
void Driver_SPISCK(bool sta);
void Driver_SPIMOSI(bool sta);
bool Driver_SPIMISO(void);  

#endif



