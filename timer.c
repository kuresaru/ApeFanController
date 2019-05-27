#include <STC15F2K60S2.H>
#include "timer.h"

sbit FAN = P3^0;

extern bit start;
extern u8 time;
void setFan(u8 level);

u8 data pwmTable[] = {
	// 关           开
		0xFF, 0x10,	0xFE, 0x98, 	//60%
		0xFF, 0x4C,	0xFE, 0x5C, 	//70%
		0xFF, 0x88,	0xFE, 0x20, 	//80%
};

u8 pwmMode = 0;
bit flip200ms = 0;

u8 tick5ms = 0;
u8 tick1s = 0;
u8 tick1m = 0;
u8 tick30m = 0;
bit pwmSta = 0;

void Timer0Init(void)
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0xFF;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	ET0 = 1;
}


void Timer2Init(void)		//5毫秒@6.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xD0;		//设置定时初值
	T2H = 0x8A;		//设置定时初值
	IE2 |= 0x04;//开定2中断
	AUXR |= 0x10;		//定时器2开始计时
}

void Timer0_IRQHandler() interrupt 1
{
	if (pwmSta)
	{
		pwmSta = 0;
		FAN = 1;
		TH0 = pwmTable[pwmMode * 4 + 2];
		TL0 = pwmTable[pwmMode * 4 + 3];
	}
	else
	{
		pwmSta = 1;
		FAN = 0;
		TH0 = pwmTable[pwmMode * 4];
		TL0 = pwmTable[pwmMode * 4 + 1];
	}
}

void Timer2_IRQHandler() interrupt 12
{
	tick5ms++;
	if (tick5ms % 40 == 0)
		flip200ms = !flip200ms;
	if (tick5ms == 200)
	{
		tick5ms = 0;
		if (start)
		{
			tick1s++;
			if (tick1s == 60)
			{
				tick1s = 0;
				tick1m++;
				if (tick1m == 30)
				{
					tick1m = 0;
					tick30m++;
					if (tick30m >= time)
					{
						tick30m = 0;
						start = 0;
						setFan(0);
					}
				}
			}
		}
		else
		{
			tick1s = tick1m = tick30m = 0;
		}
	}
}
