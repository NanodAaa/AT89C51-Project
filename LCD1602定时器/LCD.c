#include "reg51.h"
#include "intrins.h"
#include "lcd.h"
#include "main.h"

#define DATA P0 //LCD1602��������/���

sbit W_R = P2^5; //LCD1602
sbit RS = P2^6;
sbit E = P2^7;

//��ⷱæ
unsigned char Busy_Check()
{
	unsigned char ucData;
	RS = 0;
	W_R = 1;
	DelayUs(10);	
	E = 0;
	DelayUs(10);
	E = 1;
	DelayUs(10);		//ģʽ��Ϊ��ȡæ��־
	ucData = DATA;
	//���ء�1����ζ��æ�����ء�0����ζ����
	
	return (DATA&0x80); // 1��ʾæ��0��ʾ����
}

//д��ָ��
void Com_Write(unsigned char Command)
{
	while(Busy_Check()); //����ʱִ��д������
	
	RS = 0;
	W_R = 0;  
	E = 1;  //ģʽ��Ϊд��ָ��
	DelayUs(10);
	DATA = Command;
	DelayUs(10);
	E = 0;
	DelayUs(10);
}

//д������
void Data_Write(unsigned char Data)
{
	while(Busy_Check()); //����ʱִ��д������
	
	RS = 1;
	W_R = 0;
	E = 1;
	DelayUs(10);
	DATA = Data;
	DelayUs(10);
	E = 0;
	DelayUs(10);
}

//д���ַ�     					��					��				�ַ�
void Char_Write(unsigned char Hang, unsigned char Lie, unsigned char n)
{
	//��1�У�������
	if(Hang == 0)
	{
		Com_Write(0x80 + Lie); //�趨��ʾ��λ��
	}
	//��2�У�������
	else
	{
		Com_Write(0xC0 + Lie);
	}
	
	Data_Write(n); //д���ַ�'n'��ASCII
	
}

//д���ַ���
void String_Write(unsigned char Hang1, unsigned char Lie1, unsigned char *s)
{
//	RS = 1;
//	W_R = 0; //ģʽ��Ϊд������
	//��1�У�������
	if(Hang1 == 0)
	{
		Com_Write(0x80 + Lie1); //�趨��ʾ��λ��
	}
	//��2�У�������
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

//����
void Clear()
{
	Com_Write(0x01);
	DelayMs(5);
}

//��Ҫ���ʼ��
void Lcd_Init()
{
	Com_Write(0x38); //���ܣ�8λ���ߡ�2����ʾ��5*7����
	DelayMs(5);
	Com_Write(0x08); //���ܣ���ʾ������ʾ������ʾ��ꡢ�����˸
	Com_Write(0x01); //����
	Com_Write(0x06); //���ܣ���������ʱ������ơ������ַ�ʱ��ʾ��ȫ����ʾ���ƶ�
	DelayMs(5);
	Com_Write(0x0c); //��ʾ������ʾ������ʾ��ꡢ�����˸
}
