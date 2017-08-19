/***************************残疾人鼠标程序****************************
**  工程名称：YS-V0.7语音识别模块驱动程序
**	CPU: STC11L08XE
**	晶振：22.1184MHZ
**	波特率：9600 bit/S
**  说明：残疾人鼠标程序
/***************************残疾人鼠标程序******************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus: 用来在main主程序中表示程序运行的状态，不是LD3320芯片内部的状态寄存器
//	LD_ASR_NONE:		表示没有在作ASR识别
//	LD_ASR_RUNING：		表示LD3320正在作ASR识别中
//	LD_ASR_FOUNDOK:		表示一次识别流程结束后，有一个识别结果
//	LD_ASR_FOUNDZERO:	表示一次识别流程结束后，没有识别结果
//	LD_ASR_ERROR:		表示一次识别流程中LD3320芯片内部出现不正确的状态
/***********************************************************************************/
uint8 idata nAsrStatus=0;	
void MCU_init(); 
void ProcessInt0(); //识别处理函数
void delay(unsigned long uldata);
void User_handle(uint8 dat);//用户执行操作函数
void Led_test(void);//单片机工作指示
void Delay200ms();
uint8_t G0_flag=DISABLE;//运行标志，ENABLE:运行。DISABLE:禁止运行 
sbit LED=P4^2;//信号指示灯

//应用IO口定义 （模块标注 P1）
sbit PA1=P1^0; //对应板上标号 P1.0
sbit PA2=P1^1;  //对应板上标号 P1.1
sbit PA3=P1^2;  //.....
sbit PA4=P1^3;  //.....
sbit PA5=P1^4;  //.....
sbit PA6=P1^5;  //.....
sbit PA7=P1^6;  //对应板上标号 P1.6
sbit PA8=P1^7;  //对应板上标号 P1.7


/***********************************************************
* 名    称： void  main(void)
* 功    能： 主函数	程序入口
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void  main(void)
{
	uint8 idata nAsrRes;
	uint8 i=0;
	UartIni(); /*串口初始化*/
	PrintCom("LED Testing....\r\n");
	Led_test();
	PrintCom("MCU Init.... .\r\n");
	MCU_init(); // 单片机初始化
	PrintCom("LD Reset.... .\r\n");
	LD_Reset();

	nAsrStatus = LD_ASR_NONE;		//	初始状态：没有在作ASR
	
	#ifdef TEST	
    PrintCom("先说鼠标\r\n，再说左键,右键，开始"); /*text.....*/
	#endif

	while(1)
	{
		switch(nAsrStatus)
		{
			case LD_ASR_RUNING:
			case LD_ASR_ERROR:		
				break;
			case LD_ASR_NONE:
			{
				nAsrStatus=LD_ASR_RUNING;
				if (RunASR()==0)	/*	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算*/
				{
					nAsrStatus = LD_ASR_ERROR;
				}
				break;
			}
			case LD_ASR_FOUNDOK: /*	一次ASR识别流程结束，去取ASR识别结果*/
			{				
				nAsrRes = LD_GetResult();		/*获取结果*/
				User_handle(nAsrRes); //用户执行函数 
				nAsrStatus = LD_ASR_NONE;
				break;
			}
			case LD_ASR_FOUNDZERO:
			default:
			{
				nAsrStatus = LD_ASR_NONE;
				break;
			}
		}// switch	 			
	}// while

}
/***********************************************************
* 名    称： LED灯测试
* 功    能： 单片机是否工作指示
* 入口参数： 无 
* 出口参数：无
* 说    明： 					 
**********************************************************/
void Led_test(void)
{

	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
	Delay200ms();
	LED=~ LED;
}
/***********************************************************
* 名    称： void MCU_init()
* 功    能： 单片机初始化
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void MCU_init()
{
	P0 = 0xff;
	P1 = 0x00;	  // 将P1所有值为关闭
	P2 = 0xff;    // 关闭
	P3 = 0xff;    
	P4 = 0xff;

	P1M0=0XFF;	//P1端口设置为推挽输出功能，即提高IO口驱动能力，从驱动继电器模块工作
	P1M1=0X00;

	LD_MODE = 0; //	设置MD管脚为低，并行模式读写
	IE0=1;
	EX0=1;
	EA=1;
}
/***********************************************************
* 名    称：	延时函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void Delay200us()		//@22.1184MHz
{
	unsigned char i, j;
	_nop_();
	_nop_();
	i = 5;
	j = 73;
	do
	{
		while (--j);
	} while (--i);
}

void  delay(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	while(uldata--)
	Delay200us();
}

void Delay200ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

/***********************************************************
* 名    称： 中断处理函数
* 功    能：
* 入口参数：  
* 出口参数：
* 说    明： 					 
* 调用方法： 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
	ProcessInt0();				/*	LD3320 送出中断信号，包括ASR和播放MP3的中断，需要在中断处理函数中分别处理*/
}
/***********************************************************
* 名    称：用户执行函数 
* 功    能：识别成功后，执行动作可在此进行修改 
* 入口参数： 无 
* 出口参数：无
* 说    明： 通过控制PAx端口的高低电平，从而控制外部继电器的通断					 
**********************************************************/
void 	User_handle(uint8 dat)
{
     //UARTSendByte(dat);//串口识别码（十六进制）
		 if(0==dat){
		  	G0_flag=ENABLE;
			PrintCom("“鼠标”命令识别成功\r\n"); //串口输出提示信息（可删除）
			LED=0;
		 }else if(ENABLE==G0_flag){	
		 	G0_flag=DISABLE;
			LED=1;
			 switch(dat)		   /*对结果执行相关操作,客户修改*/
			  {
			  /*
			 "shu biao",鼠标,CODE_1
			"su biao",CODE_2 
		    "zuo jian",左键,CODE_3
			"you jian",右键,CODE_4
			"shang ji",CODE_5
			"xia yi",CODE_6
			"ting zhi",CODE_7	
			"shuang ji",双击,CODE_8
			*/

				  case CODE_1:	
						PrintCom("“????鼠标”命令识别成功\r\n"); //串口输出提示信息（可删除）
						// PA1=1;//让PA1端口为高电平
						break;
					case CODE_3:
						PrintCom("“左键”命令识别成功\r\n");//串口输出提示信息（可删除）
						PA1=1;
						PA5=1;
						Delay200ms();
						PA1=0;
						PA5=0;
						break;
					case CODE_4:		
						PrintCom("“右键”命令识别成功\r\n"); //串口输出提示信息（可删除）
						PA2=1;
						PA7=1;
						Delay200ms();
						PA2=0;
						PA7=0;
						break;
					case CODE_6:
					case CODE_7:
					case CODE_8:			
						PrintCom("“双击”命令识别成功\r\n"); //串口输出提示信息（可删除）
						PA1=1;
						PA5=1;
						Delay200ms();
						PA5=0;
						PA1=0;
						Delay200ms();
						PA5=1;
						PA1=1;
						Delay200ms();
						PA5=0;
						PA1=0;
						break;																									
					default:PrintCom("请重新识别发口令\r\n"); //串口输出提示信息（可删除）
						break;
				}	
			}else{
				PrintCom("请说出一级口令: 鼠标\r\n"); //串口输出提示信息（可删除）	
				PrintCom("然后说: 左键，右键和打开三个口令\r\n"); //串口输出提示信息（可删除）
			}
}	 
