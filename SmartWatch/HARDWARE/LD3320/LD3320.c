#include "LD3320.h"
#include "Lcdlib.h"
#include "key.h"
#include <stdio.h>
#include <string.h>
/************************************************************************************
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:			��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:			��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
*********************************************************************************/
//////////////////////////////////////////////////
#define ON 1
#define OFF 0
#define SUCCEED 0
#define FAILED  1
extern u8 TXdatabuf[32];     //ÿһλ���ڼ�¼��Ӧ��״̬��1Ϊ�򿪣�0Ϊ�ر�
extern u8 RXdatabuf[32];
#define VAD 0x1E  //��ֵԽСԽ���������������У���ֵԽ��Խ�������������еĿ��ܼ���,Ĭ��Ϊ12h,��������΢����С��,

#define CLEAR_WORDS LCD_ClrARectangular(60,50,96,200)

///////////////////////////////////////
u8 nAsrStatus = 0;	
u8 nLD_Mode = LD_MODE_IDLE;//������¼��ǰ���ڽ���ASRʶ�����ڲ���MP3
u8 ucRegVal;

static u8	dirty_words=0;
///�û��޸�
void Voice_Ctrl(void)
{
//	static bool VoiceSwitch = ON;
	u8 nAsrRes=0;
	u8 keyVal=KEY_NULL;
	u8 i;

	for(i=0;i<32;i++)
	{
		TXdatabuf[i]=0;
	}

 	LCD_DisGB2312String(60,62,"������������",WHITE,1);
		
	nAsrStatus = LD_ASR_NONE;//��ʼ״̬��û������ASR

	while(1)
	{
	  	keyVal=KEY_Scan();
		if(keyVal==(ONE_HIT 2))  
		{
			break;
		}
		for(i=2;i<=6;i++)
		{
			TXdatabuf[i]=2;//2Ϊ������Ϣ����ʾ������Ӧλ���в���
		}

		// while(VoiceSwitch==ON) 
		{
			switch(nAsrStatus)
			{
				case LD_ASR_RUNING:
				case LD_ASR_ERROR:
						break;
				case LD_ASR_NONE:
						nAsrStatus=LD_ASR_RUNING;
						if (RunASR()==0)//����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
						{		
							nAsrStatus = LD_ASR_ERROR;
							Show_Ch_Message("ʶ�����");
						}
						break;
				case LD_ASR_FOUNDOK:

						 nAsrRes = LD_GetResult( );//һ��ASRʶ�����̽�����ȥȡASRʶ����							

						 switch(nAsrRes)
						{
							case CODE_RECOGNIZE_FAILURE:
								Show_Ch_Message("ʶ��ʧ��������");
								break;
							case CODE_AIR_CONDITIONER_ON:					
								Show_Ch_Message("���ڴ򿪿յ�");
								TXdatabuf[2]=1;
								break;
							case CODE_AIR_CONDITIONER_OFF:	 					
								Show_Ch_Message("���ڹرտյ�");
								TXdatabuf[2]=0;
								break;
							case CODE_LIVING_ROOM_LIGHT_ON:					
								Show_Ch_Message("���ڴ򿪿�����");
								TXdatabuf[3]=1;
								break;
							case CODE_LIVING_ROOM_LIGHT_OFF:					
								Show_Ch_Message("���ڹرտ�����");
								TXdatabuf[3]=0;
								break;
							case CODE_BEDROOM_LIGHT_ON:						
								Show_Ch_Message("���ڴ����ҵ�");
								TXdatabuf[4]=1;
								break;
							case CODE_BEDROOM_LIGHT_OFF:					
								Show_Ch_Message("���ڹر����ҵ�");
								TXdatabuf[4]=0; 
								break;
							case  CODE_DOOR_OPEN:	 					
								Show_Ch_Message("���ڿ���");
								TXdatabuf[5]=1; 
								break;
							case CODE_DOOR_CLOSE:					
								Show_Ch_Message("���ڹ���");
								TXdatabuf[5]=0;  
								break;
							case CODE_WINDOW_OPEN:						
								Show_Ch_Message("���ڿ���");
								TXdatabuf[6]=1; 
								break;
							case CODE_WINDOW_CLOSE:						
								Show_Ch_Message("���ڹش�");
								TXdatabuf[6]=0;
								break;
							case CODE_NI_MEI:
								CLEAR_WORDS;
								switch(dirty_words)
								{
									case 0:
										Show_Ch_Message("������");
										dirty_words++;
										break;
									case 1:
										Show_En_Message("Fuck You");
										dirty_words=0;
										break;
									default:break;
								}
								break;
							default:break;
						}
							
						if(nAsrRes!=CODE_RECOGNIZE_FAILURE && nAsrRes<=CODE_WINDOW_CLOSE)	                   //����sin900a����	
						{	
							if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)
							{
								if(RXdatabuf[0]==SUCCEED)//RXdatabuf��1λΪ���ճɹ���־λ
						  		{
								  	i=(nAsrRes+1)/2+1;
								  	if(RXdatabuf[i]==TXdatabuf[i])
							  		{
							  			if(RXdatabuf[i]==ON)
							  			{
							  				switch(i)
							  				{
							  					case 2: 
							  						Show_Ch_Message("�յ��Ѵ�");
													break;
												case 3: 
							  						Show_Ch_Message("�������Ѵ�");
													break;	
												case 4:
													Show_Ch_Message("���ҵ��Ѵ�");
													break;
												case 5: 
													Show_Ch_Message("���Ѵ�");	
													break;
												case 6: 
													Show_Ch_Message("���Ѵ�");
													break;		
												default:break;
							  				}
							  			}
							  			else
							  			{
							  				switch(i)
							  				{
							  					case 2: 
								  					Show_Ch_Message("�յ��ѹر�");
													break;
												case 3: 
								  					Show_Ch_Message("�������ѹر�");
													break;	
												case 4:
								  					Show_Ch_Message("���ҵ��ѹر�");
													break;
												case 5: 
								  					Show_Ch_Message("���ѹر�");
													break;
												case 6: 
								  					Show_Ch_Message("���ѹر�");
													break;		
												default:break;
							  				}
							  			}

							  		}
							  		else
							  		{
							  			Show_Ch_Message("����ʧ��");
							  		}
								}	
							}
							else
							{
					  			Show_Ch_Message("����ʧ��������");
							}	
						}	
						nAsrStatus = LD_ASR_NONE;
						break;
				case LD_ASR_FOUNDZERO:
				default:
						nAsrStatus = LD_ASR_NONE;
						break;
			}
		}
	}
}

static u8 LD_AsrAddFixed(void)
{
	u8 k, flag;
	u8 nAsrAddLength;
	#define DATE_A 16//29    //�����ά��ֵ
	#define DATE_B 18 	//����һά��ֵ
	u8  sRecog[DATE_A][DATE_B] =
	{
				"kong tiao kai",\
				"kong tiao guan",\
				"ke ting deng kai",\
				"ke ting deng guan",\
				"wo shi deng kai",\
				"wo shi deng guan",\
				"kai men",\
				"da kai men",\
				"ba men da kai",\
				"guan men",\
				"guan shang men",\
				"kai chuang",\
				"da kai chuang",\
				"guan chuang",\
				"ni mei a",\
				"ni mei",\
	};

	u8  pCode[DATE_A] = {
				CODE_AIR_CONDITIONER_ON,\
				CODE_AIR_CONDITIONER_OFF,\
				CODE_LIVING_ROOM_LIGHT_ON,\
				CODE_LIVING_ROOM_LIGHT_OFF,\
				CODE_BEDROOM_LIGHT_ON,\
				CODE_BEDROOM_LIGHT_OFF,\
				CODE_DOOR_OPEN,\
				CODE_DOOR_OPEN,\
				CODE_DOOR_OPEN,\
				CODE_DOOR_CLOSE,\
				CODE_DOOR_CLOSE,\
				CODE_WINDOW_OPEN,\
				CODE_WINDOW_OPEN,\
				CODE_WINDOW_CLOSE,\
				CODE_NI_MEI,\
				CODE_NI_MEI,
	};	//���ʶ���룬�û��޸�
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}

		LD_WriteReg(0xc1, pCode[k] );//ʶ���� Index��00H��FFH��
		LD_WriteReg(0xc3, 0);//ʶ�������ʱд�� 00
		LD_WriteReg(0x08, 0x04);//д�� 1����� FIFO_EXT
		LD3320_delay(1);
		LD_WriteReg(0x08, 0x00);//��� FIFO ���ݣ����ָ�� FIFO ����д��һ�� 00H��
		LD3320_delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);//��ǰ���ʶ�����ַ�������
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);//֪ͨ DSP Ҫ���һ��ʶ��䡣
	}	 
	return flag;
}



///�û��޸� end

///�м��
//void EXTI15_10_IRQHandler(void)
void EXTI2_IRQHandler(void) 
{
	if(EXTI_GetITStatus(LD3320IRQEXITLINE)!= RESET ) 
	{
		ProcessInt(); 
 		printf("�����ж�13\r\n");	
		EXTI_ClearFlag(LD3320IRQEXITLINE);
		EXTI_ClearITPendingBit(LD3320IRQEXITLINE);//���LINE�ϵ��жϱ�־λ  
	} 
}

static void ProcessInt(void)
{
	u8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);

// ����ʶ��������ж�
//�����������룬����ʶ��ɹ���ʧ�ܶ����жϣ�
	LD_WriteReg(0x29,0) ;//���ж�
	LD_WriteReg(0x02,0) ;//���ж�

	if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
	{	 
			nAsrResCount = LD_ReadReg(0xba);

			if(nAsrResCount>0 && nAsrResCount<=4) //�д��޸�
			{
				nAsrStatus=LD_ASR_FOUNDOK; 				
			}
			else
			{
			//	nAsrStatus=LD_ASR_FOUNDZERO;
				nAsrStatus=LD_ASR_FOUNDOK; 
			}	
	}
	else
	{
		nAsrStatus=LD_ASR_FOUNDZERO;//ִ��û��ʶ��
	}

	LD_WriteReg(0x2b,0);
	LD_WriteReg(0x1C,0);//д0:ADC������
	LD_WriteReg(0x29,0);
	LD_WriteReg(0x02,0);
	LD_WriteReg(0x2B,0);
	LD_WriteReg(0xBA,0);	
	LD_WriteReg(0xBC,0);	
	LD_WriteReg(0x08,1);//���FIFO_DATA
	LD_WriteReg(0x08,0);//���FIFO_DATA�� �ٴ�д0
}

static void LD3320_delay(unsigned long uldata)
{
	unsigned int i  =  0;
	unsigned int j  =  0;
	unsigned int k  =  0;
	for (i=0;i<5;i++)
	{
		for (j=0;j<uldata;j++)
		{
			k = 200;
			while(k--);
		}
	}
}
/*
1:�ɹ�
0:ʧ��
������,�ɹ�����һ��ASR��������ʱ��ԼΪ53ms
*/
static u8 RunASR(void)
{
	u8 i=0;
	u8 asrflag=0;
	for (i=0; i<5; i++)		//��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_AsrStart();			//��ʼ��ASR
		LD3320_delay(100);
		if (LD_AsrAddFixed()==0)	//��ӹؼ����ﵽLD3320оƬ��
		{
			LD_reset();				//LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			LD3320_delay(50);	//���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		LD3320_delay(10);
		if (LD_AsrRun() == 0)
		{
			LD_reset();			 //LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			LD3320_delay(50);//���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		asrflag=1;
		break;						//ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}	
	return asrflag;
}

static void LD_reset(void)
{
	LD_RST_H();
	LD3320_delay(100);
	LD_RST_L();
	LD3320_delay(100);
	LD_RST_H();
	LD3320_delay(100);
	LD_CS_L();
	LD3320_delay(100);
	LD_CS_H();		
	LD3320_delay(100);
}

static void LD_AsrStart(void)
{
	LD_Init_ASR();
}
/*
1:��
0:æ
*/
u8 LD_Check_ASRBusyFlag_b2(void)
{
	u8 j;
	u8 flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		LD3320_delay(10);		
	}
	return flag;
}
///�м��end


///�Ĵ�������
static u8 spi_send_byte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(LD3320SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(LD3320SPI,byte);
	while (SPI_I2S_GetFlagStatus(LD3320SPI,SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(LD3320SPI);
}

static void LD_WriteReg(u8 data1,u8 data2)
{
	LD_CS_L();
	LD_SPIS_L();
	spi_send_byte(0x04);
	spi_send_byte(data1);
	spi_send_byte(data2);
	LD_CS_H();
}

static u8 LD_ReadReg(u8 reg_add)
{
	u8 i;
	LD_CS_L();
	LD_SPIS_L();
	spi_send_byte(0x05);
	spi_send_byte(reg_add);
	i=spi_send_byte(0x00);
	LD_CS_H();
	return(i);
}

static u8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5);
}

static u8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);//��˷磨MIC������
	LD_WriteReg(0x1C, 0x09);//Reserve ����������
	LD_WriteReg(0xBD, 0x20);//Reserve ����������
	LD_WriteReg(0x08, 0x01);//д�� 1����� FIFO_DATA
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);//��� FIFO ���ݣ����ָ�� FIFO ����д��һ�� 00H��
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)//��æ
	{
		return 0;
	}

	LD_WriteReg(0xB3,VAD);//��ֵԽСԽ���������������У���ֵԽ��Խ�������������еĿ��ܼ���Ĭ��Ϊ12h,

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);//֪ͨ DSP ��ʼʶ������
	LD_WriteReg(0x37, 0x06);
	LD3320_delay(5);
	LD_WriteReg(0x1C, 0x0b);//��˷����� ADC ͨ������
	LD_WriteReg(0x29, 0x10);//ͬ���ж�����,FIFO�жϲ�����
	LD_WriteReg(0xBD, 0x00);//Ȼ��������Ϊ ASR ģ��   
	return 1;
}


static void LD_Init_Common(void)
{
	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); // Soft Reset
	LD3320_delay(5);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  //ģ���·���Ƴ�ʼ��
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);  //�ڲ�ʡ��ģʽ���ó�ʼ��
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);  //?��ȡASR�������?4)
	
	/*PLL setting*/
	LD_WriteReg(0x11, LD_PLL_11);       
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19);  //ʱ��Ƶ������1
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);  //ʱ��Ƶ������2		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);  //ʱ��Ƶ������3
	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);//���� DSP ����
	LD_WriteReg(0x17, 0x4c); //ʹ DSP ����
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);//��ǰ���ʶ�����ַ������ȣ�ƴ���ַ�����
	LD_WriteReg(0xCF, 0x4F); //�ڲ�ʡ��ģʽ����
	LD_WriteReg(0x6F, 0xFF); 
}

static void LD_Init_ASR(void)
{
	nLD_Mode=LD_MODE_ASR_RUN;
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);//ASR MODE
	LD_WriteReg(0x17, 0x48);//enable DSP
	LD3320_delay(5);
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
	LD3320_delay( 1 );
}
///�Ĵ������� end


///��س�ʼ��
void LD3320_init(void)
{
	LD3320_GPIO_Cfg();	
	LD3320_EXTI_Cfg();
	LD3320_SPI_cfg();
	LD_reset();
}

static void LD3320_GPIO_Cfg(void)
{	
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(LD3320RST_GPIO_CLK | LD3320CS_GPIO_CLK,ENABLE);
		//LD_CS	/RSET
		GPIO_InitStructure.GPIO_Pin =LD3320CS_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(LD3320CS_GPIO_PORT,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin =LD3320RST_PIN;
		GPIO_Init(LD3320RST_GPIO_PORT,&GPIO_InitStructure);
}

static void LD3320_EXTI_Cfg(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	

	RCC_APB2PeriphClockCmd(LD3320IRQ_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =LD3320IRQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LD3320IRQ_GPIO_PORT, &GPIO_InitStructure);
	//�ⲿ�ж�������
  GPIO_EXTILineConfig(LD3320IRQEXIT_PORTSOURCE, LD3320IRQPINSOURCE);
  EXTI_InitStructure.EXTI_Line = LD3320IRQEXITLINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	//�ж�Ƕ������
  NVIC_InitStructure.NVIC_IRQChannel = LD3320IRQN;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void LD3320_SPI_cfg(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  //spi�˿�����
	RCC_APB2PeriphClockCmd(LD3320SPI_CLK,ENABLE);		
  RCC_APB2PeriphClockCmd(LD3320WR_GPIO_CLK | LD3320SPIMISO_GPIO_CLK | LD3320SPIMOSI_GPIO_CLK | LD3320SPISCK_GPIO_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = LD3320SPIMISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(LD3320SPIMISO_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LD3320SPIMOSI_PIN;
	GPIO_Init(LD3320SPIMOSI_GPIO_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LD3320SPISCK_PIN;
	GPIO_Init(LD3320SPISCK_GPIO_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LD3320WR_PIN;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LD3320WR_GPIO_PORT, &GPIO_InitStructure);
	
	LD_CS_H();
	
	SPI_Cmd(LD3320SPI, DISABLE);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   	//ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   						//��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   					//8λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						   							//ʱ�Ӽ��� ����״̬ʱ��SCK���ֵ͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   						//ʱ����λ ���ݲ����ӵ�һ��ʱ�ӱ��ؿ�ʼ
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   							//�������NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;   //�����ʿ��� SYSCLK/128
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   					//���ݸ�λ��ǰ
	SPI_InitStructure.SPI_CRCPolynomial = 7;							   							//CRC����ʽ�Ĵ�����ʼֵΪ7
	SPI_Init(LD3320SPI, &SPI_InitStructure);

	SPI_Cmd(LD3320SPI, ENABLE);
}

///��س�ʼ�� end 


/********************************************�Աຯ���ָ���**********************/
static void Show_Ch_Message(u8 *message)
{
	CLEAR_WORDS;
	LCD_DisGB2312String(60,110-16*strlen((const char*)message)/4,message,WHITE,1);
}
static void Show_En_Message(char *message)
{ 
	CLEAR_WORDS;
	LCD_DisASCString(60,110-12*strlen((const char*)message)/2,message,WHITE,2,RE_WRITE_BG);
}

/*********************************************END OF FILE**********************/


