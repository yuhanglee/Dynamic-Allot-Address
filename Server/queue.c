//
// Created by 李玉航 on 2018/9/6.
//

#include "queue.h"



void queueInsert(queue_t * queue, uint16_t n, const char * data, uint8_t len) {
    uint16_t i = 0;

    for (i = 0;i < len;i++) {
        queue[n].data[i] = data[i];
    }
    queue[n].dataLen = len;
}

uint8_t queueIsEmpty(uint8_t head, uint16_t tail) {
    return (uint8_t )(head == tail);
}







