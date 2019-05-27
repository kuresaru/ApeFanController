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

//0位M 1位- 2位+ 3位等待
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
	{//如果有按键按下
		if (waitKeyUp)//如果正在等待抬起
			tmp |= 0x8;//返回结果标记等待
		waitKeyUp = 1;//标记等待抬起状态
	} 
	else 
	{//没有按键按下
		if (waitKeyUp)//正在等待抬起
		{
			waitKeyUp = 0;//标记按键抬起
			deshake = 1;//开始去抖
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
		{//有按键被按下
			if ((tmp & 0xC) == 0x4)
			{//+键
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
			{//-键
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
			{//M键被按下
				if (mkTime < 1600)
				{
					mkTime++;
				}
				else if (mkTime != 0xFFFF)
				{//M键按了500ms以上
					mkTime = 0xFFFF;
					start = !start;
					setFan(start ? level : 0);
				}
			}
			led = 0;//按下时不显示
		}
		else
		{//没有按键被按下
			if (mkTime > 0)
			{//没有键按下 但是M键曾经被按过
				if (mkTime != 0xFFFF)
				{
					mkTime = 0;
					levelMode = !levelMode;
				}
				mkTime = 0;
			}
			if (levelMode)
			{//显示强度
				led = 1 << (level - 1);
			}
			else
			{//显示时间
				led = flip200ms ? time : 0;
			}
		}
		if (!LEDB && (++lowProtCount > 10000))
		{//低电保护
			start = 0;
			setFan(0);
			P3 &= 0xF1;
			PCON |= 0x02;
		}
	}
}
