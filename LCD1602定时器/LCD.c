#include "reg51.h"
#include "intrins.h"
#include "lcd.h"
#include "main.h"

#define DATA P0 //LCD1602数据输入/输出

sbit W_R = P2^5; //LCD1602
sbit RS = P2^6;
sbit E = P2^7;

//检测繁忙
unsigned char Busy_Check()
{
	unsigned char ucData;
	RS = 0;
	W_R = 1;
	DelayUs(10);	
	E = 0;
	DelayUs(10);
	E = 1;
	DelayUs(10);		//模式设为读取忙标志
	ucData = DATA;
	//返回“1”意味繁忙，返回“0”意味空闲
	
	return (DATA&0x80); // 1表示忙，0表示空闲
}

//写入指令
void Com_Write(unsigned char Command)
{
	while(Busy_Check()); //空闲时执行写入命令
	
	RS = 0;
	W_R = 0;  
	E = 1;  //模式设为写入指令
	DelayUs(10);
	DATA = Command;
	DelayUs(10);
	E = 0;
	DelayUs(10);
}

//写入数据
void Data_Write(unsigned char Data)
{
	while(Busy_Check()); //空闲时执行写入命令
	
	RS = 1;
	W_R = 0;
	E = 1;
	DelayUs(10);
	DATA = Data;
	DelayUs(10);
	E = 0;
	DelayUs(10);
}

//写入字符     					行					列				字符
void Char_Write(unsigned char Hang, unsigned char Lie, unsigned char n)
{
	//第1行，及列数
	if(Hang == 0)
	{
		Com_Write(0x80 + Lie); //设定显示的位置
	}
	//第2行，及列数
	else
	{
		Com_Write(0xC0 + Lie);
	}
	
	Data_Write(n); //写入字符'n'的ASCII
	
}

//写入字符串
void String_Write(unsigned char Hang1, unsigned char Lie1, unsigned char *s)
{
//	RS = 1;
//	W_R = 0; //模式设为写入数据
	//第1行，及列数
	if(Hang1 == 0)
	{
		Com_Write(0x80 + Lie1); //设定显示的位置
	}
	//第2行，及列数
	else
	{
		Com_Write(0xC0 + Lie1);
	}
	
	while(*s)
	{
		Data_Write(*s);
		s++;
	}
	
}

//清屏
void Clear()
{
	Com_Write(0x01);
	DelayMs(5);
}

//按要求初始化
void Lcd_Init()
{
	Com_Write(0x38); //功能：8位总线、2行显示、5*7点阵
	DelayMs(5);
	Com_Write(0x08); //功能：显示屏关显示、不显示光标、光标闪烁
	Com_Write(0x01); //清屏
	Com_Write(0x06); //功能：输入数据时光标右移、输入字符时显示屏全部显示不移动
	DelayMs(5);
	Com_Write(0x0c); //显示屏开显示、不显示光标、光标闪烁
}
