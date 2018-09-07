//
// Created by 李玉航 on 2018/9/6.
//

#include "timer.h"
#include "types.h"

static void (*ptrFunc)(void);
static uint16_t time = 0;


void timerCycle(void) {
    Sleep(1000);
    ptrFunc();
}


void timerAddFunction(void (*func)(void)) {
    ptrFunc = func;
}

