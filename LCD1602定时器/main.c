/*
		LCD1602计时器
*/

#include "reg51.h"
#include "intrins.h"
#include "lcd.h"
#include "string.h"


//独立按键
sbit K1 = P3^1; //时间+1
sbit K2 = P3^0; //时间-1
sbit K3 = P3^2; //进入时间调整菜单
sbit K4 = P3^3; //光标左移

bit msFlag;  //定时器T0
bit secFlag; 
bit minFlag;
bit houFlag;
bit dayFlag;
bit monFlag;

bit keyFlag;  //外部中断
bit recFlag;

unsigned int CLOCK;

unsigned char mss1 = 0x30; //时间参数
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

//延时函数
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

//T0设为16位定时器，定时50ms
void T0_S()
{
	TMOD = 0x01;
	TH0 = 70; //定时50ms
	TL0 = 0;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

//定时器T0功能
void T0_Z(void) interrupt 1 using 1
{
	//重载定时器
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
		//跳转到相关的显示函数
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

//外部中断0设置
void INT0_S()
{
	EA = 1;
	EX0 = 1;
	IT0 = 1;
}

//外部中断1设置
void INT1_S()
{
	EA = 1;
	EX1 = 1;
	IT1 = 1;
}

//外部中断1功能，显示记录的时间
void INT1_Z() interrupt 2 using 2
{
	recFlag = 1;
}

//毫秒位显示
void Ms_Display()
{
	Char_Write(1, 15, mss1); //毫秒位
	if(msFlag == 1)
	{
		//P2 = P2^0x01;
		
		mss1++;
		Char_Write(1, 15, mss1); //毫秒位
		DelayMs(10);
		
		
		if(mss1 == 0x3a);
		{
			mss1 = 0x30;
		}
		
		
		msFlag = 0;
	}
}

//秒位显示
void Sec_Display()
{
	Char_Write(1, 13, sec1);  //秒1位
	Char_Write(1, 12, sec2); //秒10位
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

//分位显示
void Min_Display()
{
	Char_Write(1, 10, min1); //分1位
	Char_Write(1, 9, min2); //分10位
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

//屏幕界面
void Screen_Init()
{
	String_Write(1, 0, "Timer");
	String_Write(0, 0, "Run");
	String_Write(0, 8, "NanodAaa"); 
	Char_Write(1, 11, 0x3a);
	Char_Write(1, 14, 0x3a);
}

//时间显示函数
void Time_Display()
{
	Screen_Init(); //屏幕界面
	
	Ms_Display(); //毫秒位显示
	Sec_Display(); //秒位显示
	Min_Display(); //分位显示
}

//防止画面闪烁
void Anti_Blink()
{
	Screen_Init(); //屏幕界面
	Char_Write(1, 10, min1); //分1位
	Char_Write(1, 9, min2); //分10位
	Char_Write(1, 13, sec1); //秒1位
	Char_Write(1, 12, sec2); //秒10位
	Char_Write(1, 15, mss1); //毫秒位
}

//外部中断0功能，暂停计时器
void INT0_Z() interrupt 0 using 0
{
	keyFlag = 1;
	
	while(keyFlag == 1)
	{
		String_Write(0, 0, "Stop");
	//	DelayMs(200); //设定屏幕刷新率！
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

//记录当前时间
void Time_Record()
{
	//按下K1记录
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


//显示记录的时间
void Record_Display()
{
	while(recFlag == 1)
	{
		Clear();
		
		Time_Record();  //记录当前时间
		//第一行
		String_Write(0, 0, "First");
		Char_Write(0, 8, R1_min2);
		Char_Write(0, 9, R1_min1);
		Char_Write(0, 10, 0x3a);
		Char_Write(0, 11, R1_sec2);
		Char_Write(0, 12, R1_sec1);
		Char_Write(0, 13, 0x3a);
		Char_Write(0, 14, R1_mss1);
		
		//第二行
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
	//初始化
	T0_S();	
	INT0_S();
	INT1_S();
	Lcd_Init();
	Clear();
	
	//主循环
	while(1)
	{
		Time_Display(); //时间显示
		//Time_Stop();    //计时器暂停
		Time_Record();  //记录当前时间
		Record_Display(); //显示当前时间
		
		DelayMs(50);   //每0.01s刷新一次屏幕
	}
}