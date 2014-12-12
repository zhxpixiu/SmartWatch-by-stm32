#include "LD3320.h"
#include "Lcdlib.h"
#include "key.h"
#include <stdio.h>
#include <string.h>
/************************************************************************************
//	nAsrStatus 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:			表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:			表示一次识别流程中LD3320芯片内部出现不正确的状态
*********************************************************************************/
//////////////////////////////////////////////////
#define ON 1
#define OFF 0
#define SUCCEED 0
#define FAILED  1
extern u8 TXdatabuf[32];     //每一位用于记录相应的状态，1为打开，0为关闭
extern u8 RXdatabuf[32];
#define VAD 0x1E  //数值越小越灵敏，但容易误判；数值越大越不灵敏，但误判的可能减少,默认为12h,可以入稍微再往小调,

#define CLEAR_WORDS LCD_ClrARectangular(60,50,96,200)

///////////////////////////////////////
u8 nAsrStatus = 0;	
u8 nLD_Mode = LD_MODE_IDLE;//用来记录当前是在进行ASR识别还是在播放MP3
u8 ucRegVal;

static u8	dirty_words=0;
///用户修改
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

 	LCD_DisGB2312String(60,62,"语音控制启动",WHITE,1);
		
	nAsrStatus = LD_ASR_NONE;//初始状态：没有在作ASR

	while(1)
	{
	  	keyVal=KEY_Scan();
		if(keyVal==(ONE_HIT 2))  
		{
			break;
		}
		for(i=2;i<=6;i++)
		{
			TXdatabuf[i]=2;//2为无用信息，表示不对相应位进行操作
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
						if (RunASR()==0)//启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算
						{		
							nAsrStatus = LD_ASR_ERROR;
							Show_Ch_Message("识别错误");
						}
						break;
				case LD_ASR_FOUNDOK:

						 nAsrRes = LD_GetResult( );//一次ASR识别流程结束，去取ASR识别结果							

						 switch(nAsrRes)
						{
							case CODE_RECOGNIZE_FAILURE:
								Show_Ch_Message("识别失败请重试");
								break;
							case CODE_AIR_CONDITIONER_ON:					
								Show_Ch_Message("正在打开空调");
								TXdatabuf[2]=1;
								break;
							case CODE_AIR_CONDITIONER_OFF:	 					
								Show_Ch_Message("正在关闭空调");
								TXdatabuf[2]=0;
								break;
							case CODE_LIVING_ROOM_LIGHT_ON:					
								Show_Ch_Message("正在打开客厅灯");
								TXdatabuf[3]=1;
								break;
							case CODE_LIVING_ROOM_LIGHT_OFF:					
								Show_Ch_Message("正在关闭客厅灯");
								TXdatabuf[3]=0;
								break;
							case CODE_BEDROOM_LIGHT_ON:						
								Show_Ch_Message("正在打开卧室灯");
								TXdatabuf[4]=1;
								break;
							case CODE_BEDROOM_LIGHT_OFF:					
								Show_Ch_Message("正在关闭卧室灯");
								TXdatabuf[4]=0; 
								break;
							case  CODE_DOOR_OPEN:	 					
								Show_Ch_Message("正在开门");
								TXdatabuf[5]=1; 
								break;
							case CODE_DOOR_CLOSE:					
								Show_Ch_Message("正在关门");
								TXdatabuf[5]=0;  
								break;
							case CODE_WINDOW_OPEN:						
								Show_Ch_Message("正在开窗");
								TXdatabuf[6]=1; 
								break;
							case CODE_WINDOW_CLOSE:						
								Show_Ch_Message("正在关窗");
								TXdatabuf[6]=0;
								break;
							case CODE_NI_MEI:
								CLEAR_WORDS;
								switch(dirty_words)
								{
									case 0:
										Show_Ch_Message("草泥玛");
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
							
						if(nAsrRes!=CODE_RECOGNIZE_FAILURE && nAsrRes<=CODE_WINDOW_CLOSE)	                   //调用sin900a发送	
						{	
							if(sim900a_send_ctrl(TXdatabuf,RXdatabuf)==SUCCEED)
							{
								if(RXdatabuf[0]==SUCCEED)//RXdatabuf第1位为接收成功标志位
						  		{
								  	i=(nAsrRes+1)/2+1;
								  	if(RXdatabuf[i]==TXdatabuf[i])
							  		{
							  			if(RXdatabuf[i]==ON)
							  			{
							  				switch(i)
							  				{
							  					case 2: 
							  						Show_Ch_Message("空调已打开");
													break;
												case 3: 
							  						Show_Ch_Message("客厅灯已打开");
													break;	
												case 4:
													Show_Ch_Message("卧室灯已打开");
													break;
												case 5: 
													Show_Ch_Message("门已打开");	
													break;
												case 6: 
													Show_Ch_Message("窗已打开");
													break;		
												default:break;
							  				}
							  			}
							  			else
							  			{
							  				switch(i)
							  				{
							  					case 2: 
								  					Show_Ch_Message("空调已关闭");
													break;
												case 3: 
								  					Show_Ch_Message("客厅灯已关闭");
													break;	
												case 4:
								  					Show_Ch_Message("卧室灯已关闭");
													break;
												case 5: 
								  					Show_Ch_Message("门已关闭");
													break;
												case 6: 
								  					Show_Ch_Message("窗已关闭");
													break;		
												default:break;
							  				}
							  			}

							  		}
							  		else
							  		{
							  			Show_Ch_Message("操作失败");
							  		}
								}	
							}
							else
							{
					  			Show_Ch_Message("连接失败请重试");
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
	#define DATE_A 16//29    //数组二维数值
	#define DATE_B 18 	//数组一维数值
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
	};	//添加识别码，用户修改
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}

		LD_WriteReg(0xc1, pCode[k] );//识别字 Index（00H—FFH）
		LD_WriteReg(0xc3, 0);//识别字添加时写入 00
		LD_WriteReg(0x08, 0x04);//写入 1→清除 FIFO_EXT
		LD3320_delay(1);
		LD_WriteReg(0x08, 0x00);//清除 FIFO 内容（清除指定 FIFO 后再写入一次 00H）
		LD3320_delay(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);//当前添加识别句的字符串长度
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);//通知 DSP 要添加一项识别句。
	}	 
	return flag;
}



///用户修改 end

///中间层
//void EXTI15_10_IRQHandler(void)
void EXTI2_IRQHandler(void) 
{
	if(EXTI_GetITStatus(LD3320IRQEXITLINE)!= RESET ) 
	{
		ProcessInt(); 
 		printf("进入中断13\r\n");	
		EXTI_ClearFlag(LD3320IRQEXITLINE);
		EXTI_ClearITPendingBit(LD3320IRQEXITLINE);//清除LINE上的中断标志位  
	} 
}

static void ProcessInt(void)
{
	u8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);

// 语音识别产生的中断
//（有声音输入，不论识别成功或失败都有中断）
	LD_WriteReg(0x29,0) ;//关中断
	LD_WriteReg(0x02,0) ;//关中断

	if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
	{	 
			nAsrResCount = LD_ReadReg(0xba);

			if(nAsrResCount>0 && nAsrResCount<=4) //有待修改
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
		nAsrStatus=LD_ASR_FOUNDZERO;//执行没有识别
	}

	LD_WriteReg(0x2b,0);
	LD_WriteReg(0x1C,0);//写0:ADC不可用
	LD_WriteReg(0x29,0);
	LD_WriteReg(0x02,0);
	LD_WriteReg(0x2B,0);
	LD_WriteReg(0xBA,0);	
	LD_WriteReg(0xBC,0);	
	LD_WriteReg(0x08,1);//清除FIFO_DATA
	LD_WriteReg(0x08,0);//清除FIFO_DATA后 再次写0
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
1:成功
0:失败
经测试,成功启动一次ASR流程所需时间约为53ms
*/
static u8 RunASR(void)
{
	u8 i=0;
	u8 asrflag=0;
	for (i=0; i<5; i++)		//防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD_AsrStart();			//初始化ASR
		LD3320_delay(100);
		if (LD_AsrAddFixed()==0)	//添加关键词语到LD3320芯片中
		{
			LD_reset();				//LD3320芯片内部出现不正常，立即重启LD3320芯片
			LD3320_delay(50);	//并从初始化开始重新ASR识别流程
			continue;
		}
		LD3320_delay(10);
		if (LD_AsrRun() == 0)
		{
			LD_reset();			 //LD3320芯片内部出现不正常，立即重启LD3320芯片
			LD3320_delay(50);//并从初始化开始重新ASR识别流程
			continue;
		}
		asrflag=1;
		break;						//ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
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
1:闲
0:忙
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
///中间层end


///寄存器操作
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
	LD_WriteReg(0x35, MIC_VOL);//麦克风（MIC）音量
	LD_WriteReg(0x1C, 0x09);//Reserve 保留命令字
	LD_WriteReg(0xBD, 0x20);//Reserve 保留命令字
	LD_WriteReg(0x08, 0x01);//写入 1→清除 FIFO_DATA
	LD3320_delay( 5 );
	LD_WriteReg(0x08, 0x00);//清除 FIFO 内容（清除指定 FIFO 后再写入一次 00H）
	LD3320_delay( 5);

	if(LD_Check_ASRBusyFlag_b2() == 0)//若忙
	{
		return 0;
	}

	LD_WriteReg(0xB3,VAD);//数值越小越灵敏，但容易误判；数值越大越不灵敏，但误判的可能减少默认为12h,

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);//通知 DSP 开始识别语音
	LD_WriteReg(0x37, 0x06);
	LD3320_delay(5);
	LD_WriteReg(0x1C, 0x0b);//麦克风输入 ADC 通道可用
	LD_WriteReg(0x29, 0x10);//同步中断允许,FIFO中断不允许
	LD_WriteReg(0xBD, 0x00);//然后启动；为 ASR 模块   
	return 1;
}


static void LD_Init_Common(void)
{
	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); // Soft Reset
	LD3320_delay(5);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  //模拟电路控制初始化
	LD3320_delay(5);
	LD_WriteReg(0xCF, 0x43);  //内部省电模式设置初始化
	LD3320_delay(5);
	LD_WriteReg(0xCB, 0x02);  //?读取ASR结果（候补?4)
	
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
		LD_WriteReg(0x19, LD_PLL_ASR_19);  //时钟频率设置1
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);  //时钟频率设置2		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);  //时钟频率设置3
	}
	LD3320_delay(5);
	
	LD_WriteReg(0xCD, 0x04);//允许 DSP 休眠
	LD_WriteReg(0x17, 0x4c); //使 DSP 休眠
	LD3320_delay(1);
	LD_WriteReg(0xB9, 0x00);//当前添加识别句的字符串长度（拼音字符串）
	LD_WriteReg(0xCF, 0x4F); //内部省电模式设置
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
///寄存器操作 end


///相关初始化
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
	//外部中断线配置
  GPIO_EXTILineConfig(LD3320IRQEXIT_PORTSOURCE, LD3320IRQPINSOURCE);
  EXTI_InitStructure.EXTI_Line = LD3320IRQEXITLINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	//中断嵌套配置
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
  //spi端口配置
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

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   	//全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   						//主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   					//8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						   							//时钟极性 空闲状态时，SCK保持低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   						//时钟相位 数据采样从第一个时钟边沿开始
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   							//软件产生NSS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;   //波特率控制 SYSCLK/128
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   					//数据高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7;							   							//CRC多项式寄存器初始值为7
	SPI_Init(LD3320SPI, &SPI_InitStructure);

	SPI_Cmd(LD3320SPI, ENABLE);
}

///相关初始化 end 


/********************************************自编函数分割线**********************/
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


