#include "delay.h"

#ifndef __TIMER_H
#define __TIMER_H

extern u8 pwmMode;//pwm时间 范围0-2
extern bit flip200ms;//每200ms变一次

void Timer0Init();
void Timer2Init();

#endif
