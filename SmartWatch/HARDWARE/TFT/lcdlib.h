#ifndef __Lcdlib__
#define __Lcdlib__
/*--------------------------------------------------------------------------
// �ļ�����Lcdlib.h
// ������  LCD��ͷ�ļ�
// ����ߣ�EU����
// �������ڣ�2013��10��16��
// �������ſƼ���Ʒ-��Ȩ����-����ؾ�
// EU-�Ȱ�Ƕ��ʽ����
// http://euse.taobao.com
//-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------*/
//�Աຯ������غ궨��
#define RE_WRITE_BG	1
#define KEEP_BG			0
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------
*  ��������					 
-------------------------------------------------------------------------------------------------------*/
//��ɫֵ
#include"prohead.h"
#define BLACK          0x0000   //��

#define RED            0xF800   //��  
#define ORANGE 		   	 0xFB00   //��
#define YELLOW         0xFFE0   //��
#define GREEN          0x07E0   //��
#define BLUE           0x001F   //��
#define PURPLE		   	 0x881F   //��
#define GRAY  		     0X8430   //��
#define WHITE          0xFFFF   //��

#define GOLDEN         0XBC40   //��
#define LIGHTBLUE      0x051F   //ǳ��
#define MAGENTA        0xF81F   //����
#define CYAN           0x7FFF   //��
#define DARKBLUE      	 0X01CF	//����ɫ
#define GRAYBLUE       	 0X5458 //����ɫ


//lcd direction
#define  LCD_DIRECT        1           		   //1 ����  2 ����

//lcd bus chose
#define  LCD_BUSTYPE       2                 //1:16λ����  2:8λ����

/*-------------------------------------------------------------------------------------------------------
*  ����ӿ�								 
-------------------------------------------------------------------------------------------------------*/
//------------------------- APP USER LIB -----------------------------
//lcd��ʼ��
extern void LCD_Init(void);
//lcd����Ļ
extern void LCD_ClrScr(u16 BackColor);

//--------------------------------------------------------------------
//---------------- ��Ļ���Ͻ�Ϊԭ��,xָ��, yָ�� ---------------------
//--------------------------------------------------------------------
//------------------------- �� -----------------------------
//��x0,y0λ����ʾһ����ɫΪColor��
extern void LCD_DisAPoint(u16 x0, u16 y0, u16 Color);

//------------------------- �� -----------------------------
//��x0,y0λ����ʾһ��ֱ��(dir:����1/����2),����Ϊlenth,��ɫΪcolor
extern void LCD_DisALine(u16 x0, u16 y0, u8 dir, u16 lenth, u16 color);
//��x0,y0 ~ �Խ�x1,y1,��һ����ɫΪColor�ľ��α߿�
extern void LCD_DisABorder(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color);
//��x0,y0λ�û�һ���뾶Ϊr,��ɫΪColor��Բ��
extern void LCD_DisALoop(u16 x0, u16 y0, u8 r, u16 Color);

//------------------------- �� -----------------------------
//��x0,y0λ�ÿ�ʼ,��ʾһ�����Ϊwide,��ɫΪColor��������
extern void LCD_DisASquare(u16 x0, u16 y0, u16 wide, u16 Color);   
//��x0,y0 ~ x1,y1λ�ô�,��ʾһ����ɫΪColor�ĳ�����
extern void LCD_DisARectangular(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color);

//--------------------- �����ַ� ---------------------------
//��x0,y0λ����ʾһ�����ַ���,��ɫΪfColor,����ɫΪbColor
extern void LCD_DisGB2312String16x16(u16 x0, u16 y0, u8 *s, u16 fColor, u16 bColor);
//��x0,y0λ����ʾһ�����ַ���,��ɫΪfColor,����ɫΪbColor  
extern void LCD_DisGB2312String32x32(u16 x0, u16 y0, u8 *s, u16 fColor, u16 bColor);

//------------------------ ͼƬ -----------------------------
//��x0,y0λ����ʾһ����Ϊhigh,��Ϊwide��ͼƬ, ͼƬ��Ϣ��RGB565��֯(��˸�ʽÿ�����ص�ռ��2���ֽ�)
//ͼƬ��Ϣͷ��ַָ��ΪpData
void LCD_DisAPhoto(u16 x0, u16 y0, u16 high, u16 wide,const u8 *pData);
/***************************************�Աຯ���ָ���**************************************************/
void LCD_DisASCString(u16 x0, u16 y0, char *s, u16 fColor,u8 size,bool method);//4������Ϊ16x32
u16 bgColor(u16 x,u16 y,const u8 *pData);
void LCD_DisGB2312String(u16 x0, u16 y0, u8 *s, u16 fColor, u8 size);
void LCD_ClrARectangular(u16 x0, u16 y0, u16 x1, u16 y1);
void LCD_DisAIcon(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 removeColor);
void LCD_DisLines(u16 x0, u16 y0, u8 dir, u16 lenth,u16 width, u16 color);
void LCD_DisAWideBorder(u16 x0, u16 y0, u16 x1, u16 y1,u16 width, u16 Color);
void LCD_DisAIconWithBGC(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 removeColor,u16 fColor,u16 bColor);
void LCD_DisAIconWithoutBGC(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 chosenColor);
void LCD_WriteReg(u16 Index);
void LCD_WriteData(u16 Data);
void LCD_OpenWin(u16 x0, u16 y0, u16 x1, u16 y1);
void Show_Ch_Message_Centre(u8 *message);
void ShowEn_Message_Centre(char *message);

#define CLEAR_MESSAGE LCD_ClrARectangular(80,50,96,200)
#endif



