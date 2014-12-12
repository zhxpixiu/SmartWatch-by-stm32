#ifndef __Driver__
#define __Driver__
/*--------------------------------------------------------------------------
// �ļ�����Driver.h
// ������  ϵͳ����ͷ�ļ�
// ����ߣ�EU����
// �������ڣ�2013��11��10��
// �������ſƼ���Ʒ-��Ȩ����-����ؾ�
// EU-�Ȱ�Ƕ��ʽ����
// http://euse.taobao.com
//-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------
*  ����ӿ�	ΪӦ�ò��ṩ�Ľӿ�										 
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



