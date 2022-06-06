/*
		LCD1602��ʱ��
*/

#include "reg51.h"
#include "intrins.h"
#include "lcd.h"
#include "string.h"


//��������
sbit K1 = P3^1; //ʱ��+1
sbit K2 = P3^0; //ʱ��-1
sbit K3 = P3^2; //����ʱ������˵�
sbit K4 = P3^3; //�������

bit msFlag;  //��ʱ��T0
bit secFlag; 
bit minFlag;
bit houFlag;
bit dayFlag;
bit monFlag;

bit keyFlag;  //�ⲿ�ж�
bit recFlag;

unsigned int CLOCK;

unsigned char mss1 = 0x30; //ʱ�����
unsigned char sec1 = 0x30;
unsigned char sec2 = 0x30;
unsigned char min1 = 0x30;
unsigned char min2 = 0x30;

unsigned char rec = 0;

unsigned char R1_mss1 = 0x30;
unsigned char R1_sec1 = 0x30;
unsigned char R1_sec2 = 0x30;
unsigned char R1_min1 = 0x30;
unsigned char R1_min2 = 0x30;

unsigned char R2_mss1 = 0x30;
unsigned char R2_sec1 = 0x30; 
unsigned char R2_sec2 = 0x30;
unsigned char R2_min1 = 0x30;
unsigned char R2_min2 = 0x30;

//��ʱ����
void DelayUs(unsigned char tu)
{
	while(--tu);
}

void DelayMs(unsigned char tm)
{
	while(tm--)
	{
		DelayUs(248);
		DelayUs(248);
	}
}

//T0��Ϊ16λ��ʱ������ʱ50ms
void T0_S()
{
	TMOD = 0x01;
	TH0 = 70; //��ʱ50ms
	TL0 = 0;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

//��ʱ��T0����
void T0_Z(void) interrupt 1 using 1
{
	//���ض�ʱ��
	TH0 = 70;
	TL0 = 0;
	
	CLOCK++;
	//100ms
	if(CLOCK % 2 == 0)
	{
		msFlag = 1;
	}
	//1s
	if(CLOCK % 20 == 0)
	{
		//��ת����ص���ʾ����
		secFlag = 1;
	}
	//1min
	if(CLOCK % 1200 == 0)
	{
		minFlag = 1;
	}
	//1hour
	if(CLOCK % 72000 == 0)
	{
		houFlag = 1;
	}
	//1day
	if(CLOCK % 1728000 == 0)
	{
		dayFlag = 1;
		CLOCK = 0;
	}
}

//�ⲿ�ж�0����
void INT0_S()
{
	EA = 1;
	EX0 = 1;
	IT0 = 1;
}

//�ⲿ�ж�1����
void INT1_S()
{
	EA = 1;
	EX1 = 1;
	IT1 = 1;
}

//�ⲿ�ж�1���ܣ���ʾ��¼��ʱ��
void INT1_Z() interrupt 2 using 2
{
	recFlag = 1;
}

//����λ��ʾ
void Ms_Display()
{
	Char_Write(1, 15, mss1); //����λ
	if(msFlag == 1)
	{
		//P2 = P2^0x01;
		
		mss1++;
		Char_Write(1, 15, mss1); //����λ
		DelayMs(10);
		
		
		if(mss1 == 0x3a);
		{
			mss1 = 0x30;
		}
		
		
		msFlag = 0;
	}
}

//��λ��ʾ
void Sec_Display()
{
	Char_Write(1, 13, sec1);  //��1λ
	Char_Write(1, 12, sec2); //��10λ
	if(secFlag == 1)
	{
		sec1++;
		if(sec1 == 0x3a)
		{
			sec2++;
			sec1 = 0x30;
			if(sec2 == 0x36)
			{
				sec2 = 0x30;
			}
		}
		secFlag = 0;
	}
}

//��λ��ʾ
void Min_Display()
{
	Char_Write(1, 10, min1); //��1λ
	Char_Write(1, 9, min2); //��10λ
	if(minFlag == 1)
	{
		min1++;
		if(min1 == 0x3a)
		{
			min2++;
			min1 = 0x30;
			if(min2 == 0x36)
			{
				min2 = 0x30;
			}
		}
		minFlag = 0;
	}
}

//��Ļ����
void Screen_Init()
{
	String_Write(1, 0, "Timer");
	String_Write(0, 0, "Run");
	String_Write(0, 8, "NanodAaa"); 
	Char_Write(1, 11, 0x3a);
	Char_Write(1, 14, 0x3a);
}

//ʱ����ʾ����
void Time_Display()
{
	Screen_Init(); //��Ļ����
	
	Ms_Display(); //����λ��ʾ
	Sec_Display(); //��λ��ʾ
	Min_Display(); //��λ��ʾ
}

//��ֹ������˸
void Anti_Blink()
{
	Screen_Init(); //��Ļ����
	Char_Write(1, 10, min1); //��1λ
	Char_Write(1, 9, min2); //��10λ
	Char_Write(1, 13, sec1); //��1λ
	Char_Write(1, 12, sec2); //��10λ
	Char_Write(1, 15, mss1); //����λ
}

//�ⲿ�ж�0���ܣ���ͣ��ʱ��
void INT0_Z() interrupt 0 using 0
{
	keyFlag = 1;
	
	while(keyFlag == 1)
	{
		String_Write(0, 0, "Stop");
	//	DelayMs(200); //�趨��Ļˢ���ʣ�
		if(K2 == 0)
		{
			DelayMs(10);
			if(K2 == 0)
			{
				Clear();
				Anti_Blink();
				//Time_Display();
				keyFlag = 0;
			//	DelayMs(100);
			} 
		}	
	}
}

//��¼��ǰʱ��
void Time_Record()
{
	//����K1��¼
	if(K1 == 0)
	{
		DelayMs(10);
		if(K1 == 0)
		{
			
			if(rec == 0)
			{
				R1_mss1 = mss1;
				R1_sec1 = sec1;
				R1_sec2 = sec2;
				R1_min1 = min1;
				R1_min2 = min2;
				DelayMs(5);
			}
			else if(rec == 1)
			{
				R2_mss1 = mss1;
				R2_sec1 = sec1;
				R2_sec2 = sec2;
				R2_min1 = min1;
				R2_min2 = min2;
				DelayMs(5);
			}
			
			rec++;
			if(rec == 2) rec = 0;
			
		}
	}
}


//��ʾ��¼��ʱ��
void Record_Display()
{
	while(recFlag == 1)
	{
		Clear();
		
		Time_Record();  //��¼��ǰʱ��
		//��һ��
		String_Write(0, 0, "First");
		Char_Write(0, 8, R1_min2);
		Char_Write(0, 9, R1_min1);
		Char_Write(0, 10, 0x3a);
		Char_Write(0, 11, R1_sec2);
		Char_Write(0, 12, R1_sec1);
		Char_Write(0, 13, 0x3a);
		Char_Write(0, 14, R1_mss1);
		
		//�ڶ���
		String_Write(1, 0, "Second");
		Char_Write(1, 8, R2_min2);
		Char_Write(1, 9, R2_min1);
		Char_Write(1, 10, 0x3a);
		Char_Write(1, 11, R2_sec2);
		Char_Write(1, 12, R2_sec1);
		Char_Write(1, 13, 0x3a);
		Char_Write(1, 14, R2_mss1);
		
		DelayMs(200);
		if(K4 == 0)
		{
			DelayMs(10);
			while(K4 == 0)
			{
				Clear();
				DelayMs(5);
				recFlag = 0;
			}
		}
	}
}


void main()
{
	//��ʼ��
	T0_S();	
	INT0_S();
	INT1_S();
	Lcd_Init();
	Clear();
	
	//��ѭ��
	while(1)
	{
		Time_Display(); //ʱ����ʾ
		//Time_Stop();    //��ʱ����ͣ
		Time_Record();  //��¼��ǰʱ��
		Record_Display(); //��ʾ��ǰʱ��
		
		DelayMs(50);   //ÿ0.01sˢ��һ����Ļ
	}
}