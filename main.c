#include <STC15F2K60S2.H>
#include "delay.h"
#include "timer.h"

sbit KEY1 = P3^1;
sbit KEY2 = P3^2;
sbit KEY3 = P3^3;
sbit FAN = P3^0;
sbit LEDA = P3^4;
sbit LEDB = P3^5;

u8 led = 0;
u8 level = 4;
u8 time = 5;
u8 deshake = 0;
bit waitKeyUp = 0;
bit levelMode = 1;
bit start = 0;
u16 lowProtCount = 0;

void AFC_Init()
{
	P3 = 0xFE;
	EA = 1;
	Timer0Init();
	Timer2Init();
}

void Led_Scan()
{
	P3 &= 0xF1;
	P3 |= (led & 0x07) << 1;
	LEDA = 0;
	Delay100us();
	LEDA = 1;
	P3 &= 0xF1;
	P3 |= (led & 0x38) >> 2;
	LEDB = 0;
	Delay100us();
	LEDB = 1;
	P3 |= 0x0E;
}

//0λM 1λ- 2λ+ 3λ�ȴ�
u8 Key_Scan()
{
	u8 tmp = 0;
	if (!KEY1)
		tmp = 0x1;
	else if (!KEY2)
		tmp = 0x2;
	else if (!KEY3)
		tmp = 0x4;
	if (tmp)
	{//����а�������
		if (waitKeyUp)//������ڵȴ�̧��
			tmp |= 0x8;//���ؽ����ǵȴ�
		waitKeyUp = 1;//��ǵȴ�̧��״̬
	} 
	else 
	{//û�а�������
		if (waitKeyUp)//���ڵȴ�̧��
		{
			waitKeyUp = 0;//��ǰ���̧��
			deshake = 1;//��ʼȥ��
		}
	}
	if (deshake)
	{
		deshake++;
		if (deshake > 70)
			deshake = 0;
	}
	Delay100us();
	return deshake ? 0 : tmp;
}

void setFan(u8 level) //0-4
{
	if (level == 0)
	{
		TR0 = 0;
		FAN = 0;
	}
	else if (level == 4)
	{
		TR0 = 0;
		FAN = 1;
	}
	else
	{
		pwmMode = level - 1;
		TR0 = 1;
	}
}

void main()
{
	u8 tmp;
	u16 mkTime = 0;
	AFC_Init();
	led = 0x3F;
	while (1)
	{
		Led_Scan();
		tmp = Key_Scan();
		if (tmp)
		{//�а���������
			if ((tmp & 0xC) == 0x4)
			{//+��
				if (levelMode && level < 4)
				{
					level++;
					if (start)
						setFan(level);
				}
				else if (!levelMode && time < 63)
					time++;
			} 
			else if ((tmp & 0xA) == 0x2)
			{//-��
				if (levelMode && level > 1)
				{
					level--;
					if (start)
						setFan(level);
				}
				else if (!levelMode && time > 1)
					time--;
			}
			else if (tmp & 0x1)
			{//M��������
				if (mkTime < 1600)
				{
					mkTime++;
				}
				else if (mkTime != 0xFFFF)
				{//M������500ms����
					mkTime = 0xFFFF;
					start = !start;
					setFan(start ? level : 0);
				}
			}
			led = 0;//����ʱ����ʾ
		}
		else
		{//û�а���������
			if (mkTime > 0)
			{//û�м����� ����M������������
				if (mkTime != 0xFFFF)
				{
					mkTime = 0;
					levelMode = !levelMode;
				}
				mkTime = 0;
			}
			if (levelMode)
			{//��ʾǿ��
				led = 1 << (level - 1);
			}
			else
			{//��ʾʱ��
				led = flip200ms ? time : 0;
			}
		}
		if (!LEDB && (++lowProtCount > 10000))
		{//�͵籣��
			start = 0;
			setFan(0);
			P3 &= 0xF1;
			PCON |= 0x02;
		}
	}
}
