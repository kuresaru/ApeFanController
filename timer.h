#include "delay.h"

#ifndef __TIMER_H
#define __TIMER_H

extern u8 pwmMode;//pwmʱ�� ��Χ0-2
extern bit flip200ms;//ÿ200ms��һ��

void Timer0Init();
void Timer2Init();

#endif
