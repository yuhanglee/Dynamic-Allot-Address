//
// Created by 李玉航 on 2018/9/6.
//

#ifndef DHCAC_TIMER_H
#define DHCAC_TIMER_H

#include <windows.h>

void timerCycle(void);
void timerAddFunction(void (*func)(void));
#endif //DHCAC_TIMER_H
