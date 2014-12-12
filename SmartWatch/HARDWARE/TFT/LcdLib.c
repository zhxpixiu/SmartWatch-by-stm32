/*--------------------------------------------------------------------------
// 文件名：LcdLib.c
// 描述：  LCD库函数
// 设计者：EU电子 
// 深圳移优科技出品-版权所有-翻版必究
// EU-热爱嵌入式开发
// http://euse.taobao.com
//-------------------------------------------------------------------------*/
/* 头文件包含 INCLUDES */
#include "prohead.h"
#include "driver.h"
#include "lcdlib.h"
#include "ASC8x16.h"
#include "ASC12x24.h" 
#include "ASC16x32.h"
#include "ASC24x48_Part.h" 
#include "GB2312.h" 
#include "image.h"
/*-------------------------------------------------------------------------------------------------------
*  内部声明								 
-------------------------------------------------------------------------------------------------------*/
//lcd drivers
#define	 LCD_RESET_H  		Driver_LcdReset(1)
#define	 LCD_RESET_L  		Driver_LcdReset(0)
#define	 LCD_CS_H  				Driver_LcdCS(1)
#define	 LCD_CS_L  				Driver_LcdCS(0)
#define	 LCD_RS_H  				Driver_LcdRS(1)
#define	 LCD_RS_L  				Driver_LcdRS(0)
#define	 LCD_RD_H  				Driver_LcdRD(1)
#define	 LCD_RD_L  				Driver_LcdRD(0)
#define	 LCD_WR_H  				Driver_LcdWR(1)
#define	 LCD_WR_L  				Driver_LcdWR(0)
#define  LCD_SEND(x)    	Driver_LcdSendData((x))
#define  WIDTH 176
//lcd resolution 
#if LCD_DIRECT==1
	#define  LCD_ROW_NUM    176                //行数
	#define  LCD_COL_NUM    220                //列数
#else
	#define  LCD_ROW_NUM    220                //行数
	#define  LCD_COL_NUM    176                //列数
#endif

/*-------------------------------------------------------------------------------------------------------
*  资源定义											 
-------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------
*  函数声明												 
-------------------------------------------------------------------------------------------------------*/
//lcd bottom funtions
void LCD_WriteReg(u16 Index);
void LCD_WriteData(u16 Data);
void LCD_Delayms(u8 Num);
void LCD_TimerCountHandle(void);
void LCD_PortInit(void);
void LCD_Reset(void);
void LCD_Init(void);
//back light
//...
//lcd display
void LCD_OpenWin(u16 x0, u16 y0, u16 x1, u16 y1);
void LCD_ClrScr(u16 BackColor);
void LCD_DisAPoint(u16 x0, u16 y0, u16 Color);
void LCD_DisALine(u16 x0, u16 y0, u8 dir, u16 lenth, u16 color);
void LCD_DisABorder(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color);
void LCD_DisALoop(u16 x0, u16 y0, u8 r, u16 Color);
void LCD_DisASquare(u16 x0, u16 y0, u16 wide, u16 Color);   
void LCD_DisARectangular(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color);
// void LCD_DisASCString16x8(u16 x0, u16 y0, char *s, u16 fColor, u16 bColor);
void LCD_DisGB2312String16x16(u16 x0, u16 y0, u8 *s, u16 fColor, u16 bColor);
void LCD_DisGB2312String32x32(u16 x0, u16 y0, u8 *s, u16 fColor, u16 bColor);
void LCD_DisAPhoto(u16 x0, u16 y0, u16 high, u16 wide,const u8 *pData);


/*-------------------------------------------------------------------------------------------------------
*  执行代码													 
-------------------------------------------------------------------------------------------------------*/
/********************************************************************************************************
*  Function: LCD_WriteReg				                                                           
*  Object: lcd write reg
*  Input: index
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_WriteReg(u16 Index)
{
		LCD_CS_L;
		LCD_RS_L;
		#if (LCD_BUSTYPE == 1)	
			//16位数据总线
			LCD_SEND(Index);
			LCD_WR_L;
			LCD_WR_H;
		#else 
			//8位数据总线
			LCD_SEND(Index/0x100);
			LCD_WR_L;
			LCD_WR_H;
			LCD_SEND(Index%0x100);
			LCD_WR_L;
			LCD_WR_H;
		#endif
		LCD_CS_H;
}

/********************************************************************************************************
*  Function: LCD_WR_DATA				                                                           
*  Object: lcd write data
*  Input: index and data
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_WriteData(u16 Data)
{
		LCD_CS_L;
		LCD_RS_H;
		#if (LCD_BUSTYPE == 1)
			//16位数据总线
			LCD_SEND(Data);
			LCD_WR_L;
			LCD_WR_H;
		#else 
			//8位数据总线
			LCD_SEND(Data/0x100);
			LCD_WR_L;
			LCD_WR_H;
			LCD_SEND(Data%0x100);
			LCD_WR_L;
			LCD_WR_H;
		#endif
		LCD_CS_H;
}

/********************************************************************************************************
*  Function: LCD_Delayms				                                                           
*  Object: lcd init wait..
*  Input: Num
*  Output: none                                  
*  brief: time = Num * 1ms
********************************************************************************************************/
void LCD_Delayms(u8 Num)
{
		u16 Timer;
		while(Num--)
		{
		 	Timer = 10;
			while(Timer--); 
		}
}

/********************************************************************************************************
*  Function: LCD_PortInit				                                                           
*  Object: lcd port init
*  Input: none
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_PortInit(void)
{
		LCD_CS_H;
		LCD_RS_H;
		LCD_WR_H;
		LCD_RD_H;
}

/********************************************************************************************************
*  Function: LCD_Reset				                                                           
*  Object: lcd reset control
*  Input: none
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_Reset(void)
{
		LCD_RESET_L;
		LCD_Delayms(15);
		LCD_RESET_H;
		LCD_Delayms(5);
}

/********************************************************************************************************
*  Function: Driver_LcdReset				                                                           
*  Object: lcd reset control
*  Input: sta
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_Init(void)
{ 
		//-- LCD PORT INIT --
		LCD_PortInit();
		//-- LCD RESET--
		LCD_Reset();
		//-------------- LGDP4522B Initial Sequence ---------------
		//220*176
		//power on..
		LCD_WriteReg(0X14);
		LCD_WriteData(0X0030);
		LCD_WriteReg(0X0F);
		LCD_WriteData(0X0008);
		LCD_WriteReg(0X11);
		LCD_WriteData(0X0231);
		LCD_WriteReg(0X12);
		LCD_WriteData(0X0009);
		LCD_WriteReg(0X13);
		LCD_WriteData(0X2958);
		LCD_WriteReg(0X10);
		LCD_WriteData(0X5268);
		LCD_Delayms(40);
		LCD_WriteReg(0X12);
		LCD_WriteData(0X0019);
		LCD_Delayms(40);
		LCD_WriteReg(0X10);
		LCD_WriteData(0X3260);
		LCD_WriteReg(0X13);
		LCD_WriteData(0X1958); 
		LCD_Delayms(40);
		//gamma setting..
		LCD_WriteReg(0X30);
		LCD_WriteData(0X0000);
		LCD_WriteReg(0X31);
		LCD_WriteData(0X0501);
		LCD_WriteReg(0X32);
		LCD_WriteData(0X0207);
		LCD_WriteReg(0X33);
		LCD_WriteData(0X0502);
		LCD_WriteReg(0X34);
		LCD_WriteData(0X0007);
		LCD_WriteReg(0X35);
		LCD_WriteData(0X0601);
		LCD_WriteReg(0X36);
		LCD_WriteData(0X0707);
		LCD_WriteReg(0X37);
		LCD_WriteData(0X0305);
		LCD_WriteReg(0X38);
		LCD_WriteData(0X040e);
		LCD_WriteReg(0X39);
		LCD_WriteData(0X040e);
		LCD_WriteReg(0X3a);
		LCD_WriteData(0X0101);
		LCD_WriteReg(0X3b);
		LCD_WriteData(0X0101);
		LCD_WriteReg(0X3c);
		LCD_WriteData(0X0101);
		LCD_WriteReg(0X3d);
		LCD_WriteData(0X0101);
		LCD_WriteReg(0X3e);
		LCD_WriteData(0X0001);
		LCD_WriteReg(0X3f);
		LCD_WriteData(0X0001);
		LCD_WriteReg(0X21);
		LCD_WriteData(0x0000);
		//display mode..			
		LCD_WriteReg(0X01);  
		#if (LCD_DIRECT == 1)		
			LCD_WriteData(0X031B);        
		#else
			LCD_WriteData(0X011B);			
		#endif
		LCD_WriteReg(0X02);
		LCD_WriteData(0X0700);

		LCD_WriteReg(0X03);
		#if (LCD_DIRECT == 1)	
			LCD_WriteData(0X1038);		//horizontal
		#else
			LCD_WriteData(0X1030);      //vertical
		#endif
		LCD_WriteReg(0X08);
		LCD_WriteData(0X1010);
		LCD_WriteReg(0X0A);
		LCD_WriteData(0X4420);
		LCD_WriteReg(0X0B);
		LCD_WriteData(0X5030);
		LCD_WriteReg(0X0F);
		LCD_WriteData(0X000D);
		LCD_Delayms(10);
		//display on..
		LCD_WriteReg(0X07);
		LCD_WriteData(0X0005);
		LCD_WriteReg(0X07);
		LCD_WriteData(0X0025);
		LCD_WriteReg(0X07);
		LCD_WriteData(0X0027);
		LCD_WriteReg(0X07);
		LCD_WriteData(0X0037);	
}

/********************************************************************************************************
*  Function: LCD_OpenWin				                                                           
*  Object: lcd open window for display
*  Input: x0,y0, x1, y1
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_OpenWin(u16 x0, u16 y0, u16 x1, u16 y1)
{
		#if (LCD_DIRECT == 1)
			LCD_WriteReg(0x0044);
			LCD_WriteData((x1<<8)+x0);
			LCD_WriteReg(0x0045);
			LCD_WriteData((y1<<8)+y0);	
			LCD_WriteReg(0x0021);
			LCD_WriteData((y0<<8)+x0); 
		#else
			LCD_WriteReg(0x0045);
			LCD_WriteData((x1<<8)+x0);
			LCD_WriteReg(0x0044);
			LCD_WriteData((y1<<8)+y0);
			LCD_WriteReg(0x0021);
			LCD_WriteData((x0<<8)+y0); 
		#endif
			LCD_WriteReg(0x0022);
		//S_DogFeed();
}

/********************************************************************************************************
*  Function: LCD_ClrScr				                                                           
*  Object: lcd clear screen
*  Input: backcolor
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_ClrScr(u16 BackColor)
{
		u16 i,j;
		LCD_OpenWin(0, 0, LCD_ROW_NUM-1, LCD_COL_NUM-1);
		for(i = 0; i < LCD_ROW_NUM; i++)
			 for(j =0; j < LCD_COL_NUM; j++)
					 LCD_WriteData(BackColor);
}

/********************************************************************************************************
*  Function: LCD_DisAPoint				                                                           
*  Object: Display a point at screen
*  Input: site and color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisAPoint(u16 x0, u16 y0, u16 Color)
{
		LCD_DisASquare(x0, y0, 1, Color);
}

/********************************************************************************************************
*  Function: LCD_DisALine				                                                           
*  Object: Display a line
*  Input: site dir lenth wide color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisALine(u16 x0, u16 y0, u8 dir, u16 lenth, u16 color)
{
		u16 x1,y1;
		x1 = x0;
		y1 = y0;
		if(dir == 1)
			y1 = y1 + lenth;
		else
			x1 = x1 + lenth;
		LCD_DisARectangular(x0, y0, x1, y1, color);
}

/********************************************************************************************************
*  Function: LCD_DisABorder				                                                           
*  Object: Display a border
*  Input: x0, y0, x1, y1, color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisABorder(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color)
{
		LCD_DisALine(x0, y0, 1, y1-y0, Color);
		LCD_DisALine(x1, y0, 1, y1-y0, Color);
		LCD_DisALine(x0, y0, 2, x1-x0, Color);
		LCD_DisALine(x0, y1, 2, x1-x0, Color);
}

/********************************************************************************************************
*  Function: LCD_DisALoop				                                                           
*  Object: Display a loop
*  Input: site,radius and color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisALoop(u16 x0, u16 y0, u8 r, u16 Color)
{
		s16 a,b,next;
		a	=	0;
		b = r;	  
		next = 3 - (r<<1);            
		while(a <= b)
		{
				LCD_DisAPoint(x0+a, y0-b, Color);             
				LCD_DisAPoint(x0+b, y0-a, Color);                      
				LCD_DisAPoint(x0+b, y0+a, Color);                          
				LCD_DisAPoint(x0+a, y0+b, Color);             
				LCD_DisAPoint(x0-a, y0+b, Color);                  
				LCD_DisAPoint(x0-b, y0+a, Color);             
				LCD_DisAPoint(x0-a, y0-b, Color);                          
				LCD_DisAPoint(x0-b, y0-a, Color);              	         
				a++;
				//use the bresenham    
				if(next<0)
					next += 4*a+6;	  
				else
				{
						next += 10+4*(a-b);   
						b--;
				} 						    
		}
} 

/********************************************************************************************************
*  Function: LCD_DisASquare				                                                           
*  Object: Display a square
*  Input: start point, wide, color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisASquare(u16 x0, u16 y0, u16 wide, u16 Color)
{
		u16 i,j;
		LCD_OpenWin(x0, y0, x0+wide-1, y0+wide-1);
		for(i = 0; i < wide; i++)
			for(j = 0; j < wide; j++)
					 LCD_WriteData(Color);
}

/********************************************************************************************************
*  Function: LCD_DisARectangular				                                                           
*  Object: Display a rectangular
*  Input: start point, end point, color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisARectangular(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color)
{
		u16 i,j;
		LCD_OpenWin(x0, y0, x1, y1);
		for(i = 0; i <= x1-x0; i++)
			 for(j = 0; j <= y1-y0; j++)
					 LCD_WriteData(Color);
}

/********************************************************************************************************
*  Function: LCD_DisGB2312String16x16				                                                           
*  Object: display a chinese string
*  Input: site, char, fColor, bColor
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisGB2312String16x16(u16 x0, u16 y0, u8 *s, u16 fColor, u16 bColor)
{
		u16 Num;
		u8 i,j,m,l = 1;
		while(*s)
		{
				LCD_OpenWin(x0, y0+(16-1)*(l-1), x0+16-1, y0+(16-1)*l);
				for(Num = 0; Num < sizeof(GB2312Code16x16)/35; Num++)
				{
						if((GB2312Code16x16[Num].Head[0] == *s) && (GB2312Code16x16[Num].Head[1] == *(s+1)))
						{ 
								for(i = 0; i < 32; i++) 
								{
										m = GB2312Code16x16[Num].Infor[i];
										for(j = 0; j<8; j++) 
										{
												if(m&CHSBIT7)
													LCD_WriteData(fColor);
												else 
													LCD_WriteData(bColor);
												m<<=1;
										} 
								}
						}  
				}
				s+=2;
				l++;
		}
}

/********************************************************************************************************
*  Function: LCD_DisGB2312String32x32				                                                           
*  Object: display a chinese string
*  Input: site, char, fColor, bColor
*  Output: none                                  
*  brief: none
********************************************************************************************************/

void LCD_DisGB2312String32x32(u16 x0, u16 y0, u8 *s, u16 fColor, u16 bColor)
{
		u16 Num;
		u8 i,j,m,l = 1;
		while(*s)
		{
				LCD_OpenWin(x0, y0+(32-1)*(l-1), x0+32-1, y0+(32-1)*l);
				for(Num = 0; Num < sizeof(GB2312Code32x32)/131; Num++)
				{
						if((GB2312Code32x32[Num].Head[0] == *s) && (GB2312Code32x32[Num].Head[1] == *(s+1)))
						{ 
								for(i = 0; i < 128; i++) 
								{
										m = GB2312Code32x32[Num].Infor[i];
										for(j = 0; j<8; j++) 
										{
												if(m&CHSBIT7)
													LCD_WriteData(fColor);
												else 
													LCD_WriteData(bColor);
												m<<=1;
										} 
								}
						}  
				}
				s+=2;
				l++;
		}
}


/********************************************************************************************************
*  Function: LCD_DisAPhoto				                                                           
*  Object: display a photo
*  Input: Site(x0,y0), high and wide, pData
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisAPhoto(u16 x0, u16 y0, u16 high, u16 wide,const u8 *pData)
{
		unsigned int i,lenth;
		lenth = high * wide * 2;  		//RGB565 每一像素点占用两个字节
		LCD_OpenWin(x0, y0, x0+high-1, y0+wide-1);
		for(i = 0; i < lenth; i+=2)
			LCD_WriteData((*(pData+i))*0x100 + (*(pData+i+1)));
}

/***************************************自编函数分割线**************************************************/

/********************************************************************************************************
*  Function: bgColor				                                                           
*  Object: get the certain point's background colour 
*  Input: start point x(0~219) ,y(0~175),the pointer
*  Output: colour's value                                 
*  brief: none
********************************************************************************************************/
u16 bgColor(u16 x,u16 y,const u8 *pData)
{	
	unsigned int i;
	#if(LCD_DIRECT == 1)
		i = (x*220 + y)*2;
	#else 
		i = (x*176 + y)*2;
	#endif
	return (*(pData+i))*0x100 + (*(pData+i+1));
}
/********************************************************************************************************
*  Function: LCD_DisASC1				                                                           
*  Object: Display a ASC
*  Input: site, char, fColor, bColor,size,
		  method(1:re-draw the whole rectangular area of the string ,0:only draw the dots of the string)
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisASCString(u16 x0, u16 y0, char *s, u16 fColor,u8 size,bool method)
{
		u16 i=0,j=0,l=1;
		u8 adjust=0;
		u8 char_width;
		u8 char_height;		 
		const u8 * char_set;
		u16 char_byte;
		switch(size)
		{
			case 1:
				char_width=8;
				char_height=16;
				char_set=InforCode_Font8x16;
				break;
			/*case 2:
				char_width=16;
				char_height=32;
				char_set=InforCode_Font16x32;
				break;*/
			case 2:
				char_width=12;
				char_height=24;
				char_set=InforCode_Font12x24;
				break;
			case 3:
				char_width=24;
				char_height=48;
				char_set=InforCode_Font24x48;
				adjust=48;//24X48的字库只有0~9和：，由字符“0”~":" -48可得到在该字符在字库的对应的位置
				break;	
			case 4:
				char_width=16;
				char_height=32;
				char_set=InforCode_Font16x32;
				adjust=45;//24X48的字库只有- . / 0~9和：，由字符“0”~":" -48可得到在该字符在字库的对应的位置
				break;		
		}
		char_byte = char_width * char_height/8;

		if(size==2)//尺寸（长，宽）不为8的倍数的字符需要特别处理，以下为12x24的字符显示程序，还不能适配其他非8倍尺寸的字符，若有需要须再修改
		{
			char_byte = 16 * char_height/8;
			switch(method)
			{
				case RE_WRITE_BG:
					while(*s)
					{
							LCD_OpenWin(x0, y0+char_width*(l-1), x0+char_height-1, y0+char_width*l-1);
							for(i=0; i<char_byte; i++) 
							{
									u8 m =*( char_set+(*s-adjust)*char_byte+i);
									for(j=0; j<8; j++) 
									{
											if(m&CHSBIT7)
												LCD_WriteData(fColor);
											else   
												LCD_WriteData(bgColor(x0+i*8/char_width,y0+char_width*(l-1)+i*8%char_width+j, gImage_bg));
											m <<= 1;
									}
									i++;
									m =*( char_set+(*s-adjust)*char_byte+i);
									for(j=0; j<4; j++) 
									{
											if(m&CHSBIT7)
												LCD_WriteData(fColor);
											else   
												LCD_WriteData(bgColor(x0+i*8/char_width,y0+char_width*(l-1)+i*8%char_width+j, gImage_bg));
											m <<= 1;
									}

							}
							s++;
							l++;
					}
					break;
				case KEEP_BG:
					l=0;
					while(*s)
					{
						u8 k;
						u16 x,y;
						for(i=0;i<char_height;i++)
						{
							for(j=0;j<16/8;j++)
							{
								u8 m =*( char_set+(*s-adjust)*char_byte+i*16/8+j);
								for(k=0; k<8; k++) 
								{
										if(m&CHSBIT7)
										{	
											x=x0+i;
											y=y0+j*8+l*char_width+k;
											LCD_OpenWin(x,y,x,y);
											LCD_WriteData(fColor);
										}
										m <<= 1;
								}
								j++;
								m =*( char_set+(*s-adjust)*char_byte+i*16/8+j);
								for(k=0; k<4; k++) 
								{
										if(m&CHSBIT7)
										{	
											x=x0+i;
											y=y0+j*8+l*char_width+k;
											LCD_OpenWin(x,y,x,y);
											LCD_WriteData(fColor);
										}
										m <<= 1;
								}
							}
						}
						s++;
						l++;  
					}
					break;
			}
		}
		else
		{
			switch(method)
			{
				case RE_WRITE_BG:
					while(*s)
					{
							LCD_OpenWin(x0, y0+char_width*(l-1), x0+char_height-1, y0+char_width*l-1);
							for(i=0; i<char_byte; i++) 
							{
									u8 m =*( char_set+(*s-adjust)*char_byte+i);
									for(j=0; j<8; j++) 
									{
											if(m&CHSBIT7)
												LCD_WriteData(fColor);
											else   
												LCD_WriteData(bgColor(x0+i*8/char_width,y0+char_width*(l-1)+i*8%char_width+j, gImage_bg));
											m <<= 1;
									}
							}
							s++;
							l++;
					}
					break;
				case KEEP_BG:
					l=0;
					while(*s)
					{
						u8 k;
						u16 x,y;
						for(i=0;i<char_height;i++)
						{
							for(j=0;j<char_width/8;j++)
							{
								u8 m =*( char_set+(*s-adjust)*char_byte+i*char_width/8+j);
								for(k=0; k<8; k++) 
								{
										if(m&CHSBIT7)
										{	
											x=x0+i;
											y=y0+j*8+l*char_width+k;
											LCD_OpenWin(x,y,x,y);
											LCD_WriteData(fColor);
										}
										m <<= 1;
								}
							}
						}
						s++;
						l++;  
					}
					break;
			}
		}
}
/********************************************************************************************************
*  Function: LCD_DisGB2312String32x32				                                                           
*  Object: display a chinese string
*  Input: site, char, fColor, bColor
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisGB2312String(u16 x0, u16 y0, u8 *s, u16 fColor,u8 size)
{
		u16 Num;
		u8 i,j,m,l = 1;
		u8 char_width;
		u8 char_height;	
		u16 char_byte;
		switch (size)
		{
			case 1:
				char_width=16;
				char_height=16;
				char_byte = char_width * char_height/8;
				while(*s)
				{
						LCD_OpenWin(x0, y0+char_width*(l-1), x0+char_height-1, y0+char_width*l-1);
						for(Num = 0; Num < sizeof(GB2312Code16x16)/35; Num++)
						{
								if((GB2312Code16x16[Num].Head[0] == *s) && (GB2312Code16x16[Num].Head[1] == *(s+1)))
								{ 
										for(i = 0; i < char_byte; i++) 
										{
												m = GB2312Code16x16[Num].Infor[i];
												for(j = 0; j<8; j++) 
												{
														if(m&CHSBIT7)
															LCD_WriteData(fColor);
														else 
															LCD_WriteData(bgColor(x0+i*8/char_width,y0+char_width*(l-1)+i*8%char_width+j, gImage_bg));
														m<<=1;
												} 
										}
								}  
						}
						s+=2;
						l++;
				}
				break;
			case 2:
				char_width=32;
				char_height=32;
				char_byte = char_width * char_height/8;
				while(*s)
				{
						LCD_OpenWin(x0, y0+char_width*(l-1), x0+char_height-1, y0+char_width*l-1);
						for(Num = 0; Num < sizeof(GB2312Code32x32)/131; Num++)
						{
								if((GB2312Code32x32[Num].Head[0] == *s) && (GB2312Code32x32[Num].Head[1] == *(s+1)))
								{ 
										for(i = 0; i < char_byte; i++) 
										{
												m = GB2312Code32x32[Num].Infor[i];
												for(j = 0; j<8; j++) 
												{
														if(m&CHSBIT7)
															LCD_WriteData(fColor);
														else 
															LCD_WriteData(bgColor(x0+i*8/char_width,y0+char_width*(l-1)+i*8%char_width+j, gImage_bg));
														m<<=1;
												} 
										}
								}  
						}
						s+=2;
						l++;
				}
				break;
		}

}
/********************************************************************************************************
*  Function: LCD_ClrARectangular				                                                           
*  Object: Re
*  Input: start point, end point, 
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_ClrARectangular(u16 x0, u16 y0, u16 x1, u16 y1)
{
		u16 i,j;
		LCD_OpenWin(x0, y0, x1, y1);
		for(i = 0; i <= x1-x0; i++)
			 for(j = 0; j <= y1-y0; j++)
			 {	
		 		LCD_WriteData(bgColor(x0+i,y0+j,gImage_bg));
			 }	
					 
}
/********************************************************************************************************
*  Function: LCD_DisACircle				                                                           
*  Object: Display a loop
*  Input: site,radius and color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisACircle(u16 x0, u16 y0, u8 r, u16 Color)
{
	char i;
	for(i=r;i>0;i--)
	{
		LCD_DisALoop(x0,y0,i,Color);
	}
} 

/********************************************************************************************************
*  Function: LCD_DisAIcon				                                                           
*  Object: display a Icon
*  Input: Site(x0,y0), high and wide, pData
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisAIcon(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 removeColor)
{
		u16 temp;
		unsigned int i,lenth;
		lenth = height * width * 2;  		//RGB565 每一像素点占用两个字节
		LCD_OpenWin(x0, y0, x0+height-1, y0+width-1);
		for(i = 0; i < lenth; i+=2)
		{
			temp=(*(pData+i))*0x100 + (*(pData+i+1));
			if(temp!=removeColor)
			{
				LCD_WriteData(temp);
			}
			else
			{
				LCD_WriteData(bgColor(x0+i/2/width,y0+i/2%width,gImage_bg));
			}
		}
	
}
/********************************************************************************************************
*  Function: LCD_DisLines				                                                           
*  Object: Display  lines
*  Input: site dir lenth wide color
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisLines(u16 x0, u16 y0, u8 dir, u16 lenth,u16 width, u16 color)
{
		u16 x1,y1;
		x1 = x0;
		y1 = y0;
		if(dir == 1)
		{
			x1 = x1 + width;
			y1 = y1 + lenth;
		}
		else
		{
			x1 = x1 + lenth;
			y1 = y1 + width;
		}
		LCD_DisARectangular(x0, y0, x1, y1, color);
}
/********************************************************************************************************
*  Function: LCD_DisAWideBorder				                                                           
*  Object: Display a border
*  Input: x0, y0, x1, y1, color
*  Output: none            `                      
*  brief: none
********************************************************************************************************/
void LCD_DisAWideBorder(u16 x0, u16 y0, u16 x1, u16 y1,u16 width, u16 Color)
{
		u16 i ;
		for(i=0;i<width;i++)
		{
			LCD_DisABorder(x0+i,y0+i,x1-i,y1-i,Color);
		}
}
/********************************************************************************************************
*  Function: LCD_DisAIconWithBGC				                                                           
*  Object: display a Icon
*  Input: Site(x0,y0), high and wide, pData
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisAIconWithBGC(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 removeColor,u16 fColor,u16 bColor)
{
		u16 temp;
		unsigned int i,lenth;
		lenth = height * width * 2;  		
		LCD_OpenWin(x0, y0, x0+height-1, y0+width-1);
		for(i = 0; i < lenth; i+=2)
		{
			temp=(*(pData+i))*0x100 + (*(pData+i+1));
			if(temp!=removeColor)
			{
				LCD_WriteData(fColor);
			}
			else
			{
				LCD_WriteData(bColor);
			}
		}
}
/********************************************************************************************************
*  Function: LCD_DisAIconWithoutBGC				                                                           
*  Object: display a Icon
*  Input: Site(x0,y0), high and wide, pData
*  Output: none                                  
*  brief: none
********************************************************************************************************/
void LCD_DisAIconWithoutBGC(u16 x0, u16 y0, u16 height, u16 width,const u8 *pData,u16 chosenColor)
{
		u16 temp;
		unsigned int i,lenth;
		lenth = height * width * 2;  		//RGB565 每一像素点占用两个字节
		LCD_OpenWin(x0, y0, x0+height-1, y0+width-1);
		for(i = 0; i < lenth; i+=2)
		{
			temp=(*(pData+i))*0x100 + (*(pData+i+1));
			if(temp==chosenColor)
			{
				LCD_WriteData(temp);
			}
			else
			{
				LCD_WriteData(bgColor(x0+i/2/width,y0+i/2%width,gImage_bg));
			}
		}
	
}


void Show_Ch_Message_Centre(u8 *message)
{
	LCD_ClrARectangular(80,50,96,200);
	LCD_DisGB2312String(80,110-16*strlen((const char*)message)/4,message,WHITE,1);
}
void ShowEn_Message_Centre(char *message)
{ 
	LCD_ClrARectangular(80,50,96,200);
	LCD_DisASCString(80,110-12*strlen((const char*)message)/2,message,WHITE,2,RE_WRITE_BG);
}

