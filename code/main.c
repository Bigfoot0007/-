/***************************�м���������****************************
**  �������ƣ�YS-V0.7����ʶ��ģ����������
**	CPU: STC11L08XE
**	����22.1184MHZ
**	�����ʣ�9600 bit/S
**  ˵�����м���������
/***************************�м���������******************************/
#include "config.h"
/************************************************************************************/
//	nAsrStatus: ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
/***********************************************************************************/
uint8 idata nAsrStatus=0;	
void MCU_init(); 
void ProcessInt0(); //ʶ������
void delay(unsigned long uldata);
void User_handle(uint8 dat);//�û�ִ�в�������
void Led_test(void);//��Ƭ������ָʾ
void Delay200ms();
uint8_t G0_flag=DISABLE;//���б�־��ENABLE:���С�DISABLE:��ֹ���� 
sbit LED=P4^2;//�ź�ָʾ��

//Ӧ��IO�ڶ��� ��ģ���ע P1��
sbit PA1=P1^0; //��Ӧ���ϱ�� P1.0
sbit PA2=P1^1;  //��Ӧ���ϱ�� P1.1
sbit PA3=P1^2;  //.....
sbit PA4=P1^3;  //.....
sbit PA5=P1^4;  //.....
sbit PA6=P1^5;  //.....
sbit PA7=P1^6;  //��Ӧ���ϱ�� P1.6
sbit PA8=P1^7;  //��Ӧ���ϱ�� P1.7


/***********************************************************
* ��    �ƣ� void  main(void)
* ��    �ܣ� ������	�������
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void  main(void)
{
	uint8 idata nAsrRes;
	uint8 i=0;
	UartIni(); /*���ڳ�ʼ��*/
	PrintCom("LED Testing....\r\n");
	Led_test();
	PrintCom("MCU Init.... .\r\n");
	MCU_init(); // ��Ƭ����ʼ��
	PrintCom("LD Reset.... .\r\n");
	LD_Reset();

	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
	
	#ifdef TEST	
    PrintCom("��˵���\r\n����˵���,�Ҽ�����ʼ"); /*text.....*/
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
				if (RunASR()==0)	/*	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����*/
				{
					nAsrStatus = LD_ASR_ERROR;
				}
				break;
			}
			case LD_ASR_FOUNDOK: /*	һ��ASRʶ�����̽�����ȥȡASRʶ����*/
			{				
				nAsrRes = LD_GetResult();		/*��ȡ���*/
				User_handle(nAsrRes); //�û�ִ�к��� 
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
* ��    �ƣ� LED�Ʋ���
* ��    �ܣ� ��Ƭ���Ƿ���ָʾ
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 					 
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
* ��    �ƣ� void MCU_init()
* ��    �ܣ� ��Ƭ����ʼ��
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void MCU_init()
{
	P0 = 0xff;
	P1 = 0x00;	  // ��P1����ֵΪ�ر�
	P2 = 0xff;    // �ر�
	P3 = 0xff;    
	P4 = 0xff;

	P1M0=0XFF;	//P1�˿�����Ϊ����������ܣ������IO�������������������̵���ģ�鹤��
	P1M1=0X00;

	LD_MODE = 0; //	����MD�ܽ�Ϊ�ͣ�����ģʽ��д
	IE0=1;
	EX0=1;
	EA=1;
}
/***********************************************************
* ��    �ƣ�	��ʱ����
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
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
* ��    �ƣ� �жϴ�����
* ��    �ܣ�
* ��ڲ�����  
* ���ڲ�����
* ˵    ���� 					 
* ���÷����� 
**********************************************************/ 
void ExtInt0Handler(void) interrupt 0  
{ 	
	ProcessInt0();				/*	LD3320 �ͳ��ж��źţ�����ASR�Ͳ���MP3���жϣ���Ҫ���жϴ������зֱ���*/
}
/***********************************************************
* ��    �ƣ��û�ִ�к��� 
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸� 
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� ͨ������PAx�˿ڵĸߵ͵�ƽ���Ӷ������ⲿ�̵�����ͨ��					 
**********************************************************/
void 	User_handle(uint8 dat)
{
     //UARTSendByte(dat);//����ʶ���루ʮ�����ƣ�
		 if(0==dat){
		  	G0_flag=ENABLE;
			PrintCom("����ꡱ����ʶ��ɹ�\r\n"); //���������ʾ��Ϣ����ɾ����
			LED=0;
		 }else if(ENABLE==G0_flag){	
		 	G0_flag=DISABLE;
			LED=1;
			 switch(dat)		   /*�Խ��ִ����ز���,�ͻ��޸�*/
			  {
			  /*
			 "shu biao",���,CODE_1
			"su biao",CODE_2 
		    "zuo jian",���,CODE_3
			"you jian",�Ҽ�,CODE_4
			"shang ji",CODE_5
			"xia yi",CODE_6
			"ting zhi",CODE_7	
			"shuang ji",˫��,CODE_8
			*/

				  case CODE_1:	
						PrintCom("��????��ꡱ����ʶ��ɹ�\r\n"); //���������ʾ��Ϣ����ɾ����
						// PA1=1;//��PA1�˿�Ϊ�ߵ�ƽ
						break;
					case CODE_3:
						PrintCom("�����������ʶ��ɹ�\r\n");//���������ʾ��Ϣ����ɾ����
						PA1=1;
						PA5=1;
						Delay200ms();
						PA1=0;
						PA5=0;
						break;
					case CODE_4:		
						PrintCom("���Ҽ�������ʶ��ɹ�\r\n"); //���������ʾ��Ϣ����ɾ����
						PA2=1;
						PA7=1;
						Delay200ms();
						PA2=0;
						PA7=0;
						break;
					case CODE_6:
					case CODE_7:
					case CODE_8:			
						PrintCom("��˫��������ʶ��ɹ�\r\n"); //���������ʾ��Ϣ����ɾ����
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
					default:PrintCom("������ʶ�𷢿���\r\n"); //���������ʾ��Ϣ����ɾ����
						break;
				}	
			}else{
				PrintCom("��˵��һ������: ���\r\n"); //���������ʾ��Ϣ����ɾ����	
				PrintCom("Ȼ��˵: ������Ҽ��ʹ���������\r\n"); //���������ʾ��Ϣ����ɾ����
			}
}	 
